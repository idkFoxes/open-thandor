#include <thandor/core/text/path.h>

/* Implementation ownership: core/text/path. */

/* Address: 0x0040F240.
   Ownership: core/text/path.
   Purpose: Returns up to four low-byte extension characters packed in EAX after the final path separator. EAX is
   zero when no extension exists; CF is clear on both paths.
*/
dword WidePath_GetExtensionCode(word *path)

{
  uint *extensionCursor;
  short currentCodeUnit;
  
  do {
    extensionCursor = (uint *)0x0;
    while( true ) {
      currentCodeUnit = (short)*(uint *)path;
      if (currentCodeUnit == 0) {
        if (extensionCursor == (uint *)0x0) {
          return 0;
        }
        return ((((extensionCursor[1] & 0xffffff) >> 0x10) << 8 | extensionCursor[1] & 0xff) << 8 |
               (*extensionCursor & 0xffffff) >> 0x10) << 8 | *extensionCursor & 0xff;
      }
      path = (word *)((int)path + 2);
      if (currentCodeUnit == 0x5c) break;
      if (currentCodeUnit == 0x2e) {
        extensionCursor = (uint *)path;
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
undefined4 WidePath_SetExtensionCode(PackedFileExtensionCode32 extensionCode,word *path)

{
  undefined4 in_EAX;
  uint *extensionWriteCursor;
  short currentCodeUnit;
  
  do {
    extensionWriteCursor = (uint *)0x0;
    while( true ) {
      currentCodeUnit = (short)*(uint *)path;
      if (currentCodeUnit == 0) {
        if (extensionWriteCursor == (uint *)0x0) {
          *path = 0x2e;
          extensionWriteCursor = (uint *)((int)path + 2);
        }
        *extensionWriteCursor = ((extensionCode & 0xff) << 8 | (extensionCode >> 8) << 0x18) >> 8;
        extensionWriteCursor[1] = extensionCode >> 0x10;
        return in_EAX;
      }
      path = (word *)((int)path + 2);
      if (currentCodeUnit == 0x5c) break;
      if (currentCodeUnit == 0x2e) {
        extensionWriteCursor = (uint *)path;
      }
    }
  } while( true );
}

/* Address: 0x0040F320.
   Ownership: core/text/path.
   Purpose: Splits a bounded UTF-16 path at its final backslash into leaf and parent outputs. When no separator
   exists, parent receives the complete path and leaf is cleared.
*/
undefined8 WidePath_SplitParentAndLeaf(word *leafOut,word *parentOut,word *path)

{
  undefined4 in_EAX;
  int iVar1;
  uint copyCount;
  uint uVar2;
  undefined4 in_EDX;
  word *pwVar3;
  word *componentStartCursor;
  bool bVar4;
  word currentCodeUnit;
  
  iVar1 = 0x100;
  pwVar3 = path;
  do {
    if (iVar1 == 0) break;
    iVar1 = iVar1 + -1;
    currentCodeUnit = *pwVar3;
    pwVar3 = pwVar3 + 1;
  } while (currentCodeUnit != 0);
  iVar1 = 0x100 - iVar1;
  bVar4 = iVar1 == 0;
  componentStartCursor = path;
  componentStartCursor = path;
code_r0x0040f351:
  do {
    if (iVar1 != 0) {
      iVar1 = iVar1 + -1;
      pwVar3 = componentStartCursor + 1;
      bVar4 = *componentStartCursor == 0x5c;
      componentStartCursor = pwVar3;
      if (!bVar4) goto code_r0x0040f351;
    }
    if (!bVar4) {
      pwVar3 = componentStartCursor;
      for (copyCount = (uint)((int)componentStartCursor - (int)componentStartCursor) >> 1;
          copyCount != 0; copyCount = copyCount - 1) {
        *leafOut = *pwVar3;
        pwVar3 = pwVar3 + 1;
        leafOut = leafOut + 1;
      }
      if ((int)componentStartCursor - (int)path != 0) {
        uVar2 = ((int)componentStartCursor - (int)path) - 2U >> 1;
        for (; uVar2 != 0; uVar2 = uVar2 - 1) {
          *parentOut = *path;
          path = path + 1;
          parentOut = parentOut + 1;
        }
      }
      *parentOut = 0;
      return CONCAT44(in_EDX,in_EAX);
    }
    bVar4 = false;
    componentStartCursor = componentStartCursor;
    if (iVar1 == 0) {
      for (iVar1 = 0x80; iVar1 != 0; iVar1 = iVar1 + -1) {
        *(undefined4 *)parentOut = *(undefined4 *)path;
        path = path + 2;
        parentOut = parentOut + 2;
      }
      leafOut[0] = 0;
      leafOut[1] = 0;
      return CONCAT44(in_EDX,in_EAX);
    }
  } while( true );
}

/* Address: 0x0040F3C0.
   Ownership: core/text/path.
   Purpose: Copies a bounded directory, appends a backslash when needed, then appends a bounded leaf name into the
   destination.
*/
undefined4 WidePath_CombineDirectoryAndLeaf(word *destination,word *leaf,word *directory)

{
  undefined4 in_EAX;
  int codeUnitsRemaining;
  int copyCodeUnitsRemaining;
  int iVar1;
  word *directoryScanCursor;
  word *currentPathScanCursor;
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
    iVar1 = 0x100;
    leafTerminatorFound = true;
    currentPathScanCursor = leaf;
    do {
      if (iVar1 == 0) break;
      iVar1 = iVar1 + -1;
      leafTerminatorFound = *currentPathScanCursor == 0;
      currentPathScanCursor = currentPathScanCursor + 1;
    } while (!leafTerminatorFound);
    if (leafTerminatorFound) {
      for (iVar1 = 0x100 - iVar1; iVar1 != 0; iVar1 = iVar1 + -1) {
        *destination = *leaf;
        leaf = leaf + 1;
        destination = destination + 1;
      }
    }
  }
  return in_EAX;
}

/* Address: 0x00531170.
   Ownership: core/text/path.
   Purpose: Scans at most 32 UTF-16 code units for the terminator, starts six code units before it, and parses the
   contiguous decimal digits immediately preceding the extension. The parsed value is returned in ECX while EAX is
   preserved.
*/
undefined8 WidePath_ParseTrailingNumberBeforeExtensionRegs(word *path)

{
  undefined4 in_EAX;
  int iVar1;
  uint uVar2;
  undefined4 in_EDX;
  word *terminatorCursor;
  ushort *digitScanCursor;
  word currentCodeUnit;
  ushort digitCodeUnit;
  
  iVar1 = 0x20;
  do {
    terminatorCursor = path;
    if (iVar1 == 0) break;
    iVar1 = iVar1 + -1;
    terminatorCursor = path + 1;
    currentCodeUnit = *path;
    path = terminatorCursor;
  } while (currentCodeUnit != 0);
  terminatorCursor = terminatorCursor + -6;
  uVar2 = iVar1 + 6;
  digitScanCursor = terminatorCursor;
  do {
    digitCodeUnit = *digitScanCursor;
    uVar2 = uVar2 + 1;
    digitScanCursor = digitScanCursor + -1;
    if ((digitCodeUnit < 0x30) || (9 < digitCodeUnit - 0x30)) break;
  } while (uVar2 < 0x20);
  return CONCAT44(in_EDX,in_EAX);
}
