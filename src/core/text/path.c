/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/text/path.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/text/path.h>
#include <thandor/thandor.h>

/* Implementation ownership: core/text/path. */

/* Address: 0x0040F240.
   Ownership: core/text/path.
   Purpose: Returns up to four low-byte extension characters packed in EAX after the final path separator. EAX is
   zero when no extension exists; CF is clear on both paths.
*/
uint32_t WidePath_GetExtensionCode(uint16_t *path)

{
  uint32_t *extensionCursor;
  short currentCodeUnit;
  
  do {
    extensionCursor = (uint32_t *)0x0;
    while( true ) {
      currentCodeUnit = (short)*(uint32_t *)path;
      if (currentCodeUnit == 0) {
        if (extensionCursor == (uint32_t *)0x0) {
          return 0;
        }
        return ((((extensionCursor[1] & 0xffffff) >> 0x10) << 8 | extensionCursor[1] & 0xff) << 8 |
               (*extensionCursor & 0xffffff) >> 0x10) << 8 | *extensionCursor & 0xff;
      }
      path = (uint16_t *)((int)path + 2);
      if (currentCodeUnit == 0x5c) break;
      if (currentCodeUnit == 0x2e) {
        extensionCursor = (uint32_t *)path;
      }
    }
  } while( true );
}

/* Address: 0x0040F2B0.
   Replaces the extension of the final path component with the packed code (one character per byte, as
   WidePath_GetExtensionCode returns it, e.g. 0x786667 = "gfx"), appending '.' when there is none. Asset
   loaders use it to derive sibling files (.gfx/.pal/.dat, .lev/.fld, ...). Always returns with CF clear.
   Only three characters come out right: a fourth byte would be merged into the third code unit (all callers
   pass three-character codes).
*/
bool __thandor_cf_preserve_eax_ecx_edx
WidePath_SetExtensionCode(PackedFileExtensionCode32 extensionCode,uint16_t *path)

{
  uint32_t *extensionWriteCursor;
  short currentCodeUnit;

  /* A backslash restarts the search, so only a '.' in the last component counts. */
  do {
    extensionWriteCursor = NULL;
    while( true ) {
      currentCodeUnit = (short)*(uint32_t *)path;
      if (currentCodeUnit == 0) {
        if (extensionWriteCursor == NULL) {
          *path = '.';
          extensionWriteCursor = (uint32_t *)((int)path + 2);
        }
        /* first two code units: characters 1 and 2 */
        *extensionWriteCursor = ((extensionCode & 0xff) << 8 | (extensionCode >> 8) << 0x18) >> 8;
        /* code units 3 and 4: the upper 16 bits unspread (char3 | char4 << 8, then 0), which also writes
           the terminator for a three-character extension */
        extensionWriteCursor[1] = extensionCode >> 0x10;
        return false;
      }
      path = (uint16_t *)((int)path + 2);
      if (currentCodeUnit == '\\') break;
      if (currentCodeUnit == '.') {
        extensionWriteCursor = (uint32_t *)path;
      }
    }
  } while( true );
}


/* Address: 0x0040F320.
   Splits a UTF-16 path (at most WIDE_PATH_MAX_CODE_UNITS units) at its last backslash: leafOut gets the
   file name, parentOut the directory without the trailing backslash. Without a backslash the leaf is the
   whole path and the parent is empty. Always returns false (CF clear).
*/
bool __thandor_cf_preserve_eax_ecx_edx
WidePath_SplitParentAndLeaf(uint16_t *leafOut,uint16_t *parentOut,uint16_t *path)

{
  /* Rewritten from the assembly (0x0040F320): the decompiler lost the start of the final component
     (EDX), so nothing was ever split and every "directory" still ended in the file name. Leaf gets
     everything after the last backslash (with the terminator); parent gets everything before it. */
  int count; /* code units including the terminator (REPNE SCASW, at most WIDE_PATH_MAX_CODE_UNITS) */
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


/* Address: 0x0040F3C0.
   Builds "directory\leaf" in destination: copies the directory without its terminator, adds a backslash
   unless it already ends in one (nothing for an empty directory), then copies the leaf with its terminator.
   The loaders use it to try a file relative to the executable directory before the plain path. A directory
   without a terminator in its first WIDE_PATH_MAX_CODE_UNITS units writes nothing; such a leaf leaves the
   directory part unterminated. The destination size is not checked (up to 2 * 256 units).
*/
void __thandor_void_preserve_eax_ecx_edx
WidePath_CombineDirectoryAndLeaf(uint16_t *destination,uint16_t *leaf,uint16_t *directory)

{
  int codeUnitsRemaining;
  int copyCodeUnitsRemaining;
  int leafCodeUnitsRemaining;
  uint16_t *directoryScanCursor;
  uint16_t *leafScanCursor;
  bool terminatorFound;
  bool leafTerminatorFound;

  /* REPNE SCASW over the directory */
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
      for (leafCodeUnitsRemaining = WIDE_PATH_MAX_CODE_UNITS - leafCodeUnitsRemaining; leafCodeUnitsRemaining != 0; leafCodeUnitsRemaining--) {
        *destination = *leaf;
        leaf++;
        destination++;
      }
    }
  }
}


/* Address: 0x00531170.
   Parses the decimal number that ends right before a 4-character extension (".sav" in "save12.sav" gives 12):
   reads digits backwards from the fifth code unit before the terminator until a non-digit. The terminator
   search and the digit count share a limit of 32 code units. Returns the number in ECX, 0 if there is none.
*/
uint32_t __thandor_preserve_eax_edx WidePath_ParseTrailingNumberBeforeExtensionRegs(uint16_t *path)

{
  uint32_t parsedValue;
  int scanCountOrPlaceValue;
  uint32_t scannedCodeUnitCount;
  uint32_t digitValue;
  uint16_t *terminatorCursor;
  uint16_t *digitScanCursor;
  uint16_t currentCodeUnit;
  uint16_t digitCodeUnit;
  
  scanCountOrPlaceValue = 0x20;
  do {
    terminatorCursor = path;
    if (scanCountOrPlaceValue == 0) break;
    scanCountOrPlaceValue--;
    terminatorCursor = path + 1;
    currentCodeUnit = *path;
    path = terminatorCursor;
  } while (currentCodeUnit != 0);
  /* one past the terminator minus 6: the last code unit before the ".ext" */
  terminatorCursor = terminatorCursor - 6;
  parsedValue = 0;
  scannedCodeUnitCount = scanCountOrPlaceValue + 6;
  scanCountOrPlaceValue = 1;
  digitScanCursor = terminatorCursor;
  while( true ) {
    digitCodeUnit = *digitScanCursor;
    scannedCodeUnitCount++;
    digitScanCursor--;
    digitValue = digitCodeUnit - '0';
    if (digitCodeUnit < '0') {
      return parsedValue;
    }
    if (9 < digitValue) break;
    parsedValue = parsedValue + digitValue * scanCountOrPlaceValue;
    scanCountOrPlaceValue = scanCountOrPlaceValue * 10;
    if (0x1f < scannedCodeUnitCount) {
      return parsedValue;
    }
  }
  return parsedValue;
}

