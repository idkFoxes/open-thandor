/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/text/path.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/text/path.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Replaces the extension of the final path component with the packed code (one character per byte, first
   character in the lowest byte, e.g. 0x786667 = "gfx"), appending '.' when there is none. Asset
   loaders use it to derive sibling files (.gfx/.pal/.dat, .lev/.fld, ...). Returns false (success).
   Only three characters come out right: a fourth byte would be merged into the third code unit (all callers
   pass three-character codes).
   The original neither bounds the scan nor checks the writes (up to 5 units past the old terminator); bounded
   here because a malformed asset path could be overwritten past its buffer: the path must be terminated
   within pathCapacity code units and the new extension with its terminator must fit in them, otherwise
   nothing is written and true (failure) is returned (logged once). pathCapacity defaults to
   WIDE_PATH_MAX_CODE_UNITS (path.h).
*/
bool WidePath_SetExtensionCode(PackedFileExtensionCode32 extensionCode,uint16_t *path,size_t pathCapacity)

{
  static bool s_RejectionLogged = false;
  uint16_t *pathStart;
  uint16_t *extension;
  uint16_t currentCodeUnit;

  /* A backslash restarts the search, so only a '.' in the last component counts. */
  pathStart = path;
  extension = nullptr;
  while ((size_t)(path - pathStart) < pathCapacity && *path != 0) {
    currentCodeUnit = *path;
    path++;
    if (currentCodeUnit == '\\') {
      extension = nullptr;
    }
    else if (currentCodeUnit == '.') {
      extension = path;
    }
  }
  /* path is at the terminator; the last write is extension[3] (path[4] when '.' is appended) */
  if (((size_t)(path - pathStart) >= pathCapacity) ||
      ((size_t)(((extension != nullptr) ? extension : path + 1) + 3 - pathStart) >= pathCapacity)) {
    if (!s_RejectionLogged) {
      s_RejectionLogged = true;
      Thandor_Log("path: extension does not fit in a %u-unit path, path left unchanged",(unsigned)pathCapacity);
    }
    return true;
  }
  if (extension == nullptr) {
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
   directory part unterminated. The destination may be the directory itself (appending in place).
   The original does not check the destination size (it writes up to 2 * 256 units); bounded here because
   the original tree-list control (since removed) combined nested directory labels into 256-unit buffers:
   a result that does not fit in destinationCapacity code units is cut off and terminated (logged once).
   Callers go through the WidePath_CombineDirectoryAndLeaf template (path.h), which passes the
   destination array's size.
*/
void WidePath_CombineDirectoryAndLeafBounded
          (uint16_t *destination,size_t destinationCapacity,uint16_t *leaf,uint16_t *directory)

{
  static bool s_TruncationLogged = false;
  int codeUnitsRemaining;
  int copyCodeUnitsRemaining;
  int leafCodeUnitsRemaining;
  uint16_t *directoryScanCursor;
  uint16_t *leafScanCursor;
  bool terminatorFound;
  bool leafTerminatorFound;
  bool endsInBackslash;
  bool truncated;
  size_t written;

  if (destinationCapacity == 0) {
    return;
  }
  truncated = false;
  written = 0;
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
      /* the directory's last unit decides the backslash (the copy may be cut off before it) */
      endsInBackslash = directory[copyCodeUnitsRemaining - 1] == '\\';
      for (; copyCodeUnitsRemaining != 0; copyCodeUnitsRemaining--) {
        /* one unit always stays free for the terminator */
        if (written + 1 < destinationCapacity) {
          destination[written++] = *directory;
        }
        else {
          truncated = true;
        }
        directory++;
      }
      if (!endsInBackslash) {
        if (written + 1 < destinationCapacity) {
          destination[written++] = '\\';
        }
        else {
          truncated = true;
        }
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
      /* leaf length without the terminator, then the terminator (it always fits, see above) */
      for (leafCodeUnitsRemaining = WIDE_PATH_MAX_CODE_UNITS - 1 - leafCodeUnitsRemaining;
           leafCodeUnitsRemaining != 0; leafCodeUnitsRemaining--) {
        if (written + 1 < destinationCapacity) {
          destination[written++] = *leaf;
        }
        else {
          truncated = true;
        }
        leaf++;
      }
      destination[written] = 0;
    }
    else if (truncated) {
      destination[written] = 0;
    }
  }
  if (truncated && !s_TruncationLogged) {
    s_TruncationLogged = true;
    Thandor_Log("path: combined path longer than %u code units cut off",(unsigned)destinationCapacity);
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
  /* The original reads before the path when it is shorter than 5 code units (no room for "N.ext");
     bounded here because the digits would come from memory before the buffer: no number. */
  if (scanCursor - path < 6) {
    return 0;
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

