/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/text/path.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/text/path.h>
#include <thandor/thandor.h>

/* Implementation ownership: core/text/path. */

/* Replaces the extension of the final path component with the packed code (one character per byte, first
   character in the lowest byte, e.g. 0x786667 = "gfx"), appending '.' when there is none. Asset
   loaders use it to derive sibling files (.gfx/.pal/.dat, .lev/.fld, ...). Always returns false (success).
   Only three characters come out right: a fourth byte would be merged into the third code unit (all callers
   pass three-character codes).
*/
bool WidePath_SetExtensionCode(PackedFileExtensionCode32 extensionCode,uint16_t *path)

{
  uint16_t *extension;
  uint16_t currentCodeUnit;

  /* A backslash restarts the search, so only a '.' in the last component counts. */
  extension = NULL;
  while (*path != 0) {
    currentCodeUnit = *path;
    path++;
    if (currentCodeUnit == '\\') {
      extension = NULL;
    }
    else if (currentCodeUnit == '.') {
      extension = path;
    }
  }
  if (extension == NULL) {
    *path = '.';
    extension = path + 1;
  }
  /* first two code units: characters 1 and 2 */
  extension[0] = (uint16_t)(extensionCode & 0xff);
  extension[1] = (uint16_t)((extensionCode >> 8) & 0xff);
  /* code units 3 and 4: the upper 16 bits unspread (char3 | char4 << 8, then 0), which also writes
     the terminator for a three-character extension */
  extension[2] = (uint16_t)(extensionCode >> 16);
  extension[3] = 0;
  return false;
}


/* Splits a UTF-16 path (at most WIDE_PATH_MAX_CODE_UNITS units) at its last backslash: leafOut gets the
   file name, parentOut the directory without the trailing backslash. Without a backslash the leaf is the
   whole path and the parent is empty. Always returns false (success).
*/
bool WidePath_SplitParentAndLeaf(uint16_t *leafOut,uint16_t *parentOut,uint16_t *path)

{
  /* Leaf gets everything after the last backslash (with the terminator); parent gets everything before
     it. */
  int count; /* code units including the terminator (at most WIDE_PATH_MAX_CODE_UNITS) */
  int remaining;
  int i;
  uint16_t *leafStart;

  count = 0;
  while (count < WIDE_PATH_MAX_CODE_UNITS) {
    count++;
    if (path[count - 1] == 0) {
      break;
    }
  }
  leafStart = path;
  for (remaining = count, i = 0; remaining != 0; remaining--, i++) {
    if (path[i] == '\\') {
      leafStart = path + i + 1;
      if (remaining == 1) {
        /* Backslash in the last scanned unit (only possible in an unterminated 0x100-unit path): the
           whole buffer becomes the parent and the leaf is empty. */
        for (i = 0; i < WIDE_PATH_MAX_CODE_UNITS; i++) {
          parentOut[i] = path[i];
        }
        leafOut[0] = 0;
        leafOut[1] = 0;
        return false;
      }
    }
  }
  for (i = 0; i < (int)((path + count) - leafStart); i++) {
    leafOut[i] = leafStart[i];
  }
  if (leafStart != path) {
    for (i = 0; i < (int)(leafStart - path) - 1; i++) {
      parentOut[i] = path[i];
    }
    parentOut[i] = 0;
  }
  else {
    parentOut[0] = 0;
  }
  return false;
}


/* Builds "directory\leaf" in destination: copies the directory without its terminator, adds a backslash
   unless it already ends in one (nothing for an empty directory), then copies the leaf with its terminator.
   The loaders use it to try a file relative to the executable directory before the plain path. A directory
   without a terminator in its first WIDE_PATH_MAX_CODE_UNITS units writes nothing; such a leaf leaves the
   directory part unterminated. The destination size is not checked (up to 2 * 256 units).
*/
void WidePath_CombineDirectoryAndLeaf(uint16_t *destination,uint16_t *leaf,uint16_t *directory)

{
  int codeUnitsRemaining;
  int copyCodeUnitsRemaining;
  int leafCodeUnitsRemaining;
  uint16_t *directoryScanCursor;
  uint16_t *leafScanCursor;
  bool terminatorFound;
  bool leafTerminatorFound;

  /* find the directory's terminator (at most WIDE_PATH_MAX_CODE_UNITS code units) */
  terminatorFound = true;
  codeUnitsRemaining = WIDE_PATH_MAX_CODE_UNITS;
  directoryScanCursor = directory;
  do {
    if (codeUnitsRemaining == 0) break;
    codeUnitsRemaining--;
    terminatorFound = *directoryScanCursor == 0;
    directoryScanCursor++;
  } while (!terminatorFound);
  if (terminatorFound) {
    /* directory length without the terminator */
    copyCodeUnitsRemaining = WIDE_PATH_MAX_CODE_UNITS - 1 - codeUnitsRemaining;
    if (copyCodeUnitsRemaining != 0) {
      for (; copyCodeUnitsRemaining != 0; copyCodeUnitsRemaining--) {
        *destination = *directory;
        directory++;
        destination++;
      }
      if (destination[-1] != '\\') {
        *destination = '\\';
        destination++;
      }
    }
    leafCodeUnitsRemaining = WIDE_PATH_MAX_CODE_UNITS;
    leafTerminatorFound = true;
    leafScanCursor = leaf;
    do {
      if (leafCodeUnitsRemaining == 0) break;
      leafCodeUnitsRemaining--;
      leafTerminatorFound = *leafScanCursor == 0;
      leafScanCursor++;
    } while (!leafTerminatorFound);
    if (leafTerminatorFound) {
      /* leaf length including the terminator */
      for (leafCodeUnitsRemaining = WIDE_PATH_MAX_CODE_UNITS - leafCodeUnitsRemaining; leafCodeUnitsRemaining != 0;
           leafCodeUnitsRemaining--) {
        *destination = *leaf;
        leaf++;
        destination++;
      }
    }
  }
}


/* Parses the decimal number that ends right before a 4-character extension (".sav" in "save12.sav" gives 12):
   reads digits backwards from the fifth code unit before the terminator until a non-digit. The terminator
   search and the digit count share a limit of 32 code units. Returns the number, 0 if there is none.
*/
uint32_t WidePath_ParseTrailingNumberBeforeExtension(uint16_t *path)

{
  uint32_t parsedValue;
  int scanUnitsLeft;
  uint32_t placeValue;
  uint32_t scannedCodeUnitCount;
  uint16_t *scanCursor;
  uint16_t *digitScanCursor;
  uint16_t currentCodeUnit;
  uint16_t digitCodeUnit;

  /* find the terminator (at most WIDE_PATH_NUMBER_SCAN_MAX_UNITS units); scanCursor ends one past it */
  scanUnitsLeft = WIDE_PATH_NUMBER_SCAN_MAX_UNITS;
  scanCursor = path;
  while (scanUnitsLeft != 0) {
    scanUnitsLeft--;
    currentCodeUnit = *scanCursor;
    scanCursor++;
    if (currentCodeUnit == 0) break;
  }
  /* one past the terminator minus 6: the last code unit before the ".ext". The digit count continues
     the terminator search's count, so the digits stop at the start of the string (the first digit is
     read unconditionally). */
  digitScanCursor = scanCursor - 6;
  parsedValue = 0;
  scannedCodeUnitCount = scanUnitsLeft + 6;
  placeValue = 1;
  do {
    digitCodeUnit = *digitScanCursor;
    scannedCodeUnitCount++;
    digitScanCursor--;
    if (digitCodeUnit < '0' || digitCodeUnit > '9') break;
    parsedValue = parsedValue + (uint32_t)(digitCodeUnit - '0') * placeValue;
    placeValue = placeValue * 10;
  } while (scannedCodeUnitCount <= WIDE_PATH_NUMBER_SCAN_MAX_UNITS - 1);
  return parsedValue;
}

