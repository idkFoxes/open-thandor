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
   Ownership: core/text/path.
   Purpose: CF remains clear. Typed parameters: p0 extensionCode→PackedFileExtensionCode32_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
WidePath_SetExtensionCode(PackedFileExtensionCode32 extensionCode,uint16_t *path)

{
  uint32_t *extensionWriteCursor;
  short currentCodeUnit;
  
  do {
    extensionWriteCursor = (uint32_t *)0x0;
    while( true ) {
      currentCodeUnit = (short)*(uint32_t *)path;
      if (currentCodeUnit == 0) {
        if (extensionWriteCursor == (uint32_t *)0x0) {
          *path = 0x2e;
          extensionWriteCursor = (uint32_t *)((int)path + 2);
        }
        *extensionWriteCursor = ((extensionCode & 0xff) << 8 | (extensionCode >> 8) << 0x18) >> 8;
        extensionWriteCursor[1] = extensionCode >> 0x10;
        return false;
      }
      path = (uint16_t *)((int)path + 2);
      if (currentCodeUnit == 0x5c) break;
      if (currentCodeUnit == 0x2e) {
        extensionWriteCursor = (uint32_t *)path;
      }
    }
  } while( true );
}


/* Address: 0x0040F320.
   Ownership: core/text/path.
   Purpose: Splits a bounded UTF-16 path at its final backslash into leaf and parent outputs. When no separator
   exists, parent receives the complete path and leaf is cleared.
*/
bool __thandor_cf_preserve_eax_ecx_edx
WidePath_SplitParentAndLeaf(uint16_t *leafOut,uint16_t *parentOut,uint16_t *path)

{
  /* Rewritten from the assembly (0x0040F320): the decompiler lost the start of the final component
     (EDX), so nothing was ever split and every "directory" still ended in the file name. Leaf gets
     everything after the last backslash (with the terminator); parent gets everything before it.
     Without a backslash the leaf is the whole path and the parent is empty. */
  int count;
  int remaining;
  int i;
  uint16_t *leafStart;

  count = 0;
  while (count < 0x100) {
    count = count + 1;
    if (path[count - 1] == 0) {
      break;
    }
  }
  leafStart = path;
  for (remaining = count, i = 0; remaining != 0; remaining--, i++) {
    if (path[i] == 0x5c) {
      leafStart = path + i + 1;
      if (remaining == 1) {
        /* Backslash in the last scanned unit: the whole 0x100-unit buffer becomes the parent. */
        for (i = 0; i < 0x100; i++) {
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
   Ownership: core/text/path.
   Purpose: Copies a bounded directory, appends a backslash when needed, then appends a bounded leaf name into the
   destination.
*/
void __thandor_void_preserve_eax_ecx_edx
WidePath_CombineDirectoryAndLeaf(uint16_t *destination,uint16_t *leaf,uint16_t *directory)

{
  int codeUnitsRemaining;
  int copyCodeUnitsRemaining;
  int leafCodeUnitsRemaining;
  uint16_t *directoryScanCursor;
  uint16_t *currentPathScanCursor;
  bool terminatorFound;
  bool leafTerminatorFound;
  
  terminatorFound = true;
  codeUnitsRemaining = 0x100;
  directoryScanCursor = directory;
  do {
    if (codeUnitsRemaining == 0) break;
    codeUnitsRemaining = codeUnitsRemaining + -1;
    terminatorFound = *directoryScanCursor == 0;
    directoryScanCursor = directoryScanCursor + 1;
  } while (!terminatorFound);
  if (terminatorFound) {
    copyCodeUnitsRemaining = 0xff - codeUnitsRemaining;
    if (copyCodeUnitsRemaining != 0) {
      for (; copyCodeUnitsRemaining != 0; copyCodeUnitsRemaining = copyCodeUnitsRemaining + -1) {
        *destination = *directory;
        directory = directory + 1;
        destination = destination + 1;
      }
      if (destination[-1] != 0x5c) {
        *destination = 0x5c;
        destination = destination + 1;
      }
    }
    leafCodeUnitsRemaining = 0x100;
    leafTerminatorFound = true;
    currentPathScanCursor = leaf;
    do {
      if (leafCodeUnitsRemaining == 0) break;
      leafCodeUnitsRemaining = leafCodeUnitsRemaining + -1;
      leafTerminatorFound = *currentPathScanCursor == 0;
      currentPathScanCursor = currentPathScanCursor + 1;
    } while (!leafTerminatorFound);
    if (leafTerminatorFound) {
      for (leafCodeUnitsRemaining = 0x100 - leafCodeUnitsRemaining; leafCodeUnitsRemaining != 0; leafCodeUnitsRemaining = leafCodeUnitsRemaining + -1) {
        *destination = *leaf;
        leaf = leaf + 1;
        destination = destination + 1;
      }
    }
  }
  return;
}


/* Address: 0x00531170.
   Ownership: core/text/path.
   Purpose: Scans at most 32 UTF-16 code units for the terminator, starts six code units before it, and parses the
   contiguous decimal digits immediately preceding the extension. The parsed value is returned in ECX while EAX is
   preserved.
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
    scanCountOrPlaceValue = scanCountOrPlaceValue + -1;
    terminatorCursor = path + 1;
    currentCodeUnit = *path;
    path = terminatorCursor;
  } while (currentCodeUnit != 0);
  terminatorCursor = terminatorCursor + -6;
  parsedValue = 0;
  scannedCodeUnitCount = scanCountOrPlaceValue + 6;
  scanCountOrPlaceValue = 1;
  digitScanCursor = terminatorCursor;
  while( true ) {
    digitCodeUnit = *digitScanCursor;
    scannedCodeUnitCount = scannedCodeUnitCount + 1;
    digitScanCursor = digitScanCursor + -1;
    digitValue = digitCodeUnit - 0x30;
    if (digitCodeUnit < 0x30) {
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

