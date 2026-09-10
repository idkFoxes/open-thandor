#include <thandor/assets/package/runtime.h>

/* Implementation ownership: assets/package/runtime. */

/* Address: 0x005460E0.
   Ownership: assets/package/runtime.
   Purpose: Handles level package validate and mount.
   Local calls: Package_Mount, Package_FindEntry, Package_LoadEntry, Package_Unmount.
   Cross-module calls: Resource_Release [assets/resource/runtime], TextResourcePage_LoadCompatibilityAliases
   [assets/text/resources].
*/
undefined8 LevelPackage_ValidateAndMount(word *levelPathUtf16)

{
  dword aliasAddressBase;
  undefined4 in_EAX;
  dword fileHandle;
  int *allocation;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 in_EDX;
  undefined1 in_CF;
  bool bVar1;
  undefined1 uVar2;
  
  fileHandle = Package_Mount(levelPathUtf16);
  if (!(bool)in_CF) {
    Package_FindEntry(0x200,(PckEntryHeader *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15),
                      (word *)u_level___lev_005460a6,fileHandle);
    if (((!(bool)in_CF) && (bVar1 = extraout_ECX == 0, !bVar1)) &&
       (allocation = Package_LoadEntry((word *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15)), !bVar1))
    {
      if ((*allocation == 0x76656c) &&
         (uVar2 = (uint)allocation[3] < 0x70001, allocation[3] == 0x70001)) {
        aliasAddressBase = allocation[0x5c];
        Resource_Release(allocation);
        Package_FindEntry(0x200,(PckEntryHeader *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15),
                          (word *)u_level___str_005460be,fileHandle);
        if (((!(bool)uVar2) && (bVar1 = extraout_ECX_00 == 0, !bVar1)) &&
           (TextResourcePage_LoadCompatibilityAliases
                      (aliasAddressBase,(word *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15)), !bVar1))
        goto LevelPackage_ValidateAndMount_ReturnMountOrCompatibilityValidationStatus;
      }
      else {
        Resource_Release(allocation);
      }
    }
    Package_Unmount(fileHandle);
  }
LevelPackage_ValidateAndMount_ReturnMountOrCompatibilityValidationStatus:
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0040E840.
   Ownership: assets/package/runtime.
   Purpose: Replaces an existing path when present, then appends a new 0x200-byte entry header and encoded payload.
   compressionMethod indexes the verified three-entry encoder table; method 1 writes a four-byte-aligned stored
   payload directly. The first payload dword becomes typeTag. CF reports failure and the mounted directory is
   refreshed after success.
   Local calls: Package_FindEntryInMount, Package_DeleteEntry, Package_ReadDirectory.
*/
dword Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                         void *sourceData,word *path,EngineFileHandle fileHandle)

{
  uint *puVar1;
  byte *destination;
  dword dVar2;
  uint uVar3;
  undefined4 extraout_ECX;
  int iVar4;
  byte *pbVar5;
  undefined1 in_CF;
  undefined1 uVar6;
  bool bVar7;
  
  destination = g_PackageScratchBuffer;
  Package_FindEntryInMount(path,fileHandle);
  uVar6 = in_CF;
  if (!(bool)in_CF) {
    dVar2 = Package_DeleteEntry(path,fileHandle);
    uVar6 = 0;
    if ((bool)in_CF) {
      return dVar2;
    }
  }
  dVar2 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  if ((bool)uVar6) {
    return dVar2;
  }
  dVar2 = (*g_FileSystemReadExactCf)(0x200,destination,(void *)fileHandle);
  if ((bool)uVar6) {
    return dVar2;
  }
  dVar2 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  if ((bool)uVar6) {
    return dVar2;
  }
  *(int *)(destination + 0xb0) = *(int *)(destination + 0xb0) + 1;
  bVar7 = compressionMethod == PCK_COMPRESSION_HUFFMAN_RLE;
  if (compressionMethod == PCK_COMPRESSION_STORED) {
    uVar3 = unpackedSize + 3 & 0xfffffffc;
    *(uint *)(destination + 0x3f8) = uVar3;
    uVar3 = uVar3 + 0x200;
    destination[0x3fc] = 1;
    destination[0x3fd] = 0;
    destination[0x3fe] = 0;
    destination[0x3ff] = 0;
    puVar1 = (uint *)(destination + 4);
    bVar7 = CARRY4(*puVar1,uVar3);
    *puVar1 = *puVar1 + uVar3;
    destination[0x3ec] = 0;
    destination[0x3ed] = 0;
    destination[0x3ee] = 0;
    destination[0x3ef] = 0;
    *(undefined4 *)(destination + 0x3f4) = *(undefined4 *)sourceData;
    *(PckDecodedByteCount *)(destination + 0x3f0) = unpackedSize;
    dVar2 = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination,(void *)fileHandle);
    if (bVar7) {
      return dVar2;
    }
    bVar7 = (byte *)0xfffffdff < destination;
    pbVar5 = destination + 0x200;
    for (iVar4 = 0x7b; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined4 *)pbVar5 = *(undefined4 *)path;
      path = path + 2;
      pbVar5 = pbVar5 + 4;
    }
    dVar2 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_END,0,(void *)fileHandle);
    if (bVar7) {
      return dVar2;
    }
    dVar2 = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination + 0x200,(void *)fileHandle);
    uVar3 = (uint)((ulonglong)dVar2 >> 0x20);
    if (bVar7) {
      return (dword)dVar2;
    }
    bVar7 = uVar3 < 0x200;
    dVar2 = (*g_FileSystemWriteExactOrFlushCf)(uVar3 - 0x200,sourceData,(void *)fileHandle);
    if (bVar7) {
      return dVar2;
    }
  }
  else {
    dVar2 = (*g_PckEncoderTable[compressionMethod])
                      (0x7ffc00,destination + 0x400,unpackedSize,sourceData);
    dVar2 = (dword)dVar2;
    if (bVar7) {
      return dVar2;
    }
    *(dword *)(destination + 0x3f8) = dVar2;
    *(undefined4 *)(destination + 0x3fc) = extraout_ECX;
    puVar1 = (uint *)(destination + 4);
    bVar7 = CARRY4(*puVar1,dVar2 + 0x200);
    *puVar1 = *puVar1 + dVar2 + 0x200;
    destination[0x3ec] = 0;
    destination[0x3ed] = 0;
    destination[0x3ee] = 0;
    destination[0x3ef] = 0;
    *(undefined4 *)(destination + 0x3f4) = *(undefined4 *)((ulonglong)dVar2 >> 0x20);
    *(PckDecodedByteCount *)(destination + 0x3f0) = unpackedSize;
    dVar2 = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination,(void *)fileHandle);
    if (bVar7) {
      return dVar2;
    }
    bVar7 = (byte *)0xfffffdff < destination;
    pbVar5 = destination + 0x200;
    for (iVar4 = 0x7b; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined4 *)pbVar5 = *(undefined4 *)path;
      path = path + 2;
      pbVar5 = pbVar5 + 4;
    }
    dVar2 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_END,0,(void *)fileHandle);
    if (bVar7) {
      return (dword)dVar2;
    }
    dVar2 = (*g_FileSystemWriteExactOrFlushCf)
                      ((FileIoByteCount)((ulonglong)dVar2 >> 0x20),destination + 0x200,
                       (void *)fileHandle);
    if (bVar7) {
      return dVar2;
    }
  }
  bVar7 = false;
  dVar2 = Package_ReadDirectory(fileHandle);
  if (bVar7) {
    return dVar2;
  }
  return dVar2;
}

/* Address: 0x0040ED00.
   Ownership: assets/package/runtime.
   Purpose: Loads a package entry into a caller-provided buffer when allowed by flags and capacity, otherwise falls
   back to the loose-file path. The high two bits of capacityAndFlags control package bypass and alternate loose-
   path handling.
   Local calls: Package_FindEntryAcrossMounts, Package_DecodeEntryInto, Package_SetLastErrorPath.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
dword Package_LoadEntryIntoBuffer
                (PckLoadCapacityFlags bufferCapacityAndLoadFlags,byte *destination,word *path)

{
  uint uVar1;
  PckEntryHeader *entry;
  dword dVar2;
  void *handle;
  void *extraout_EAX;
  EngineFileHandle unaff_EBX;
  bool bVar3;
  undefined1 uVar4;
  
  uVar1 = bufferCapacityAndLoadFlags & 0x3fffffff;
  bVar3 = false;
  if (((bufferCapacityAndLoadFlags & 0x80000000) != 0) ||
     (entry = Package_FindEntryAcrossMounts(path), bVar3)) {
    uVar4 = false;
    if ((bufferCapacityAndLoadFlags & 0x40000000) == 0) {
      handle = (void *)(*g_FileSystemOpenCf)(0,path);
      if ((bool)uVar4) {
        return (dword)handle;
      }
    }
    else {
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (word *)&g_ExecutableDirectoryUtf16);
      handle = (void *)(*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
      if (((bool)uVar4) && (handle = (void *)(*g_FileSystemOpenCf)(0,path), (bool)uVar4)) {
        return (dword)handle;
      }
    }
    bVar3 = false;
    dVar2 = (*g_FileSystemGetSizeCf)(handle);
    if ((!bVar3) &&
       (((bVar3 = dVar2 < uVar1, dVar2 <= uVar1 || (bVar3 = true, dVar2 = uVar1, uVar1 < 0x800000))
        && ((*g_FileSystemReadExactCf)(dVar2,destination,handle), !bVar3)))) {
      (*g_FileSystemClose)(handle);
      return (dword)handle;
    }
    (*g_FileSystemClose)(handle);
  }
  else {
    if (((entry->unpackedSize <= uVar1) && (entry->packedSize < 0x800001)) &&
       (destination != g_PackageScratchBuffer)) {
      dVar2 = Package_DecodeEntryInto(destination,entry,unaff_EBX);
      return dVar2;
    }
    Package_SetLastErrorPath(path);
    handle = extraout_EAX;
  }
  return (dword)handle;
}

/* Address: 0x0040E450.
   Ownership: assets/package/runtime.
   Purpose: Mounts a package into the last free slot while scanning the 1,024-slot mount table backward. Because
   lookups scan forward, this archive has the lowest precedence. Startup uses this path for engine.pck.
   Local calls: Package_ReadDirectory.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
dword Package_MountLowPriority(word *path)

{
  void *handle;
  PckEntryHeader *allocatedEntryHeaders;
  int extraout_ECX;
  int extraout_ECX_00;
  dword bytes;
  int iVar1;
  PckMountSlot *mountSlot;
  undefined1 uVar2;
  bool bVar3;
  
  mountSlot = g_PackageMountSlots + 0x3ff;
  iVar1 = 0x400;
  while (uVar2 = 0, mountSlot->fileHandle != 0) {
    mountSlot = mountSlot + -1;
    iVar1 = iVar1 + -1;
    if (iVar1 == 0) {
      return 0x14;
    }
  }
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_FileSystemCombinedPathScratchUtf16,path,(word *)&g_ExecutableDirectoryUtf16)
  ;
  handle = (void *)(*g_FileSystemOpenCf)
                             (FILESYSTEM_OPEN_WRITE_ACCESS,
                              (word *)&g_FileSystemCombinedPathScratchUtf16);
  iVar1 = extraout_ECX;
  if (((bool)uVar2) &&
     (handle = (void *)(*g_FileSystemOpenCf)(FILESYSTEM_OPEN_WRITE_ACCESS,path),
     iVar1 = extraout_ECX_00, (bool)uVar2)) {
    return (dword)handle;
  }
  bytes = (dword)((longlong)iVar1 * 0x400);
  bVar3 = (longlong)(int)bytes != (longlong)iVar1 * 0x400;
  allocatedEntryHeaders = (*g_MemoryApi.alloc)(bytes);
  if (bVar3) {
    (*g_FileSystemClose)(handle);
    return (dword)handle;
  }
  mountSlot->fileHandle = (EngineFileHandle)handle;
  mountSlot->entryHeaders = allocatedEntryHeaders;
  mountSlot->entryCount = 0;
  Package_ReadDirectory((EngineFileHandle)handle);
  return (dword)handle;
}

/* Address: 0x0040E6F0.
   Ownership: assets/package/runtime.
   Purpose: Deletes one exact path from a writable mounted PCK. Missing entries are treated as success. The routine
   rewrites the archive header, compacts all following bytes through the shared scratch buffer, truncates the file,
   and reloads the in-memory directory. CF reports file or rewrite failure.
   Local calls: Package_FindEntryInMount, Package_ReadDirectory.
*/
dword Package_DeleteEntry(word *path,EngineFileHandle fileHandle)

{
  uint *puVar1;
  byte *destination;
  PckEntryHeader *pPVar2;
  dword dVar3;
  uint extraout_ECX;
  uint byteCount;
  FileIoByteCount byteCount_00;
  uint uVar4;
  undefined1 in_CF;
  bool bVar5;
  
  destination = g_PackageScratchBuffer;
  pPVar2 = Package_FindEntryInMount(path,fileHandle);
  if ((bool)in_CF) {
    return (dword)pPVar2;
  }
  dVar3 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  if (!(bool)in_CF) {
    dVar3 = (*g_FileSystemReadExactCf)(0x200,destination,(void *)fileHandle);
    dVar3 = (dword)dVar3;
    if (!(bool)in_CF) {
      uVar4 = (int)((ulonglong)dVar3 >> 0x20) + 0x200;
      *(int *)(destination + 0xb0) = *(int *)(destination + 0xb0) + -1;
      puVar1 = (uint *)(destination + 4);
      bVar5 = *puVar1 < uVar4;
      *puVar1 = *puVar1 - uVar4;
      dVar3 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      if ((!bVar5) &&
         (dVar3 = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination,(void *)fileHandle), !bVar5))
      {
        uVar4 = pPVar2->runtimePayloadOffset + pPVar2->packedSize + 0x200;
        bVar5 = extraout_ECX < uVar4;
        if (extraout_ECX == uVar4) {
          dVar3 = (*g_FileSystemSeekCf)
                            (FILESYSTEM_SEEK_BEGIN,pPVar2->runtimePayloadOffset,(void *)fileHandle);
          if (bVar5) {
            return dVar3;
          }
          dVar3 = (*g_FileSystemWriteExactOrFlushCf)(0,(void *)0x0,(void *)fileHandle);
          if (bVar5) {
            return dVar3;
          }
        }
        else {
          dVar3 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,uVar4,(void *)fileHandle);
          if (bVar5) {
            return dVar3;
          }
          bVar5 = byteCount < 0x800000;
          if (0x800000 < byteCount) {
            return 0x14;
          }
          dVar3 = (*g_FileSystemReadExactCf)(byteCount,g_PackageScratchBuffer,(void *)fileHandle);
          if (bVar5) {
            return dVar3;
          }
          dVar3 = (*g_FileSystemSeekCf)
                            (FILESYSTEM_SEEK_BEGIN,pPVar2->runtimePayloadOffset,(void *)fileHandle);
          if (bVar5) {
            return dVar3;
          }
          dVar3 = (*g_FileSystemWriteExactOrFlushCf)
                            (byteCount_00,g_PackageScratchBuffer,(void *)fileHandle);
          if (bVar5) {
            return dVar3;
          }
          dVar3 = (*g_FileSystemWriteExactOrFlushCf)(0,(void *)0x0,(void *)fileHandle);
          if (bVar5) {
            return dVar3;
          }
        }
        bVar5 = false;
        dVar3 = Package_ReadDirectory(fileHandle);
        if (!bVar5) {
          return dVar3;
        }
      }
    }
  }
  return dVar3;
}

/* Address: 0x0040EE30.
   Ownership: assets/package/runtime.
   Purpose: Loads any package or loose-file asset into a newly allocated buffer. The returned allocation is untyped
   at this layer; callers cast it to gfx, fld, lev, mdl, sound, text, and other asset types. CF reports success or
   failure.
   Local calls: Package_FindEntryAcrossMounts, Package_DecodeEntryInto.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
void * Package_LoadEntry(word *path)

{
  PckEntryHeader *entry;
  byte *destination;
  dword dVar1;
  void *destination_00;
  FileIoByteCount byteCount;
  byte *memory;
  void *extraout_EDX;
  EngineFileHandle unaff_EBX;
  undefined1 in_CF;
  bool bVar2;
  
  entry = Package_FindEntryAcrossMounts(path);
  if ((bool)in_CF) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
               (word *)&g_ExecutableDirectoryUtf16);
    destination = (byte *)(*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    if (((bool)in_CF) && (destination = (byte *)(*g_FileSystemOpenCf)(0,path), (bool)in_CF)) {
      return destination;
    }
    bVar2 = false;
    dVar1 = (*g_FileSystemGetSizeCf)(destination);
    if (!bVar2) {
      destination_00 = (*g_MemoryApi.alloc)(dVar1);
      if (bVar2) {
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,byteCount,g_FatalErrorDetail1Utf16);
      }
      else {
        dVar1 = (*g_FileSystemReadExactCf)(byteCount,destination_00,destination);
        if (!bVar2) {
          (*g_FileSystemClose)(destination);
          return extraout_EDX;
        }
        (*g_MemoryApi.free)((void *)((ulonglong)dVar1 >> 0x20));
      }
    }
    (*g_FileSystemClose)(destination);
  }
  else {
    destination = (byte *)0x5;
    bVar2 = entry->packedSize < 0x800000;
    if ((entry->packedSize < 0x800001) &&
       (destination = (*g_MemoryApi.alloc)(entry->unpackedSize), !bVar2)) {
      Package_DecodeEntryInto(destination,entry,unaff_EBX);
      if (!bVar2) {
        return memory;
      }
      destination = memory;
      (*g_MemoryApi.free)(memory);
    }
  }
  return destination;
}

/* Address: 0x0040E3A0.
   Ownership: assets/package/runtime.
   Purpose: Mounts a package in the first free slot. Lookup scans the table in the same direction, so earlier
   normal mounts have higher precedence.
   Local calls: Package_ReadDirectory.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
dword Package_Mount(word *path)

{
  void *handle;
  PckEntryHeader *allocatedEntryHeaders;
  int extraout_ECX;
  int extraout_ECX_00;
  dword bytes;
  int iVar1;
  PckMountSlot *mountSlot;
  undefined1 uVar2;
  bool bVar3;
  
  mountSlot = g_PackageMountSlots;
  iVar1 = 0x400;
  while (uVar2 = 0, mountSlot->fileHandle != 0) {
    mountSlot = mountSlot + 1;
    iVar1 = iVar1 + -1;
    if (iVar1 == 0) {
      return 0x14;
    }
  }
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_FileSystemCombinedPathScratchUtf16,path,(word *)&g_ExecutableDirectoryUtf16)
  ;
  handle = (void *)(*g_FileSystemOpenCf)
                             (FILESYSTEM_OPEN_WRITE_ACCESS,
                              (word *)&g_FileSystemCombinedPathScratchUtf16);
  iVar1 = extraout_ECX;
  if (((bool)uVar2) &&
     (handle = (void *)(*g_FileSystemOpenCf)(FILESYSTEM_OPEN_WRITE_ACCESS,path),
     iVar1 = extraout_ECX_00, (bool)uVar2)) {
    return (dword)handle;
  }
  bytes = (dword)((longlong)iVar1 * 0x400);
  bVar3 = (longlong)(int)bytes != (longlong)iVar1 * 0x400;
  allocatedEntryHeaders = (*g_MemoryApi.alloc)(bytes);
  if (bVar3) {
    (*g_FileSystemClose)(handle);
    return (dword)handle;
  }
  mountSlot->fileHandle = (EngineFileHandle)handle;
  mountSlot->entryHeaders = allocatedEntryHeaders;
  mountSlot->entryCount = 0;
  Package_ReadDirectory((EngineFileHandle)handle);
  return (dword)handle;
}

/* Address: 0x0040EB70.
   Ownership: assets/package/runtime.
   Purpose: Collects entry paths in one mounted package that match the package wildcard grammar, copies them into
   0x200-byte output slots, and sorts the results lexicographically. Returns 0x200 with CF clear or error 0x14 with
   CF set.
   Local calls: Package_WildcardPathMatches.
*/
dword Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries,
                       word *pattern,EngineFileHandle fileHandle)

{
  undefined4 uVar1;
  EngineFileHandle EVar2;
  word *pwVar3;
  word *pwVar4;
  dword dVar5;
  int extraout_ECX;
  int iVar6;
  int iVar7;
  EngineFileHandle EVar8;
  int extraout_EDX;
  int iVar9;
  PckMountSlot *pPVar10;
  PckEntryHeader *pPVar11;
  PckEntryHeader *pPVar12;
  PckEntryHeader *pPVar13;
  PckEntryHeader *pPVar14;
  bool bVar15;
  
  pPVar10 = g_PackageMountSlots;
  EVar8 = 0x400;
  EVar2 = fileHandle;
  while( true ) {
    if (EVar2 == 0) {
      return 0x14;
    }
    if (fileHandle == pPVar10->fileHandle) break;
    pPVar10 = pPVar10 + 1;
    EVar8 = EVar8 - 1;
    EVar2 = EVar8;
  }
  pPVar13 = pPVar10->entryHeaders;
  dVar5 = 0x200;
  bVar15 = false;
  pPVar12 = outputEntries;
  if (pPVar10->entryCount != 0) {
    do {
      dVar5 = Package_WildcardPathMatches(pattern,pPVar13->path);
      iVar6 = extraout_ECX;
      if (!bVar15) {
        bVar15 = outputCapacityBytes < 0x200;
        outputCapacityBytes = outputCapacityBytes - 0x200;
        if (bVar15) {
          return dVar5;
        }
        pPVar11 = pPVar13;
        pPVar14 = pPVar12;
        for (iVar6 = 0xf6; iVar6 != 0; iVar6 = iVar6 + -1) {
          pPVar14->path[0] = pPVar11->path[0];
          pPVar11 = (PckEntryHeader *)(pPVar11->path + 1);
          pPVar14 = (PckEntryHeader *)(pPVar14->path + 1);
        }
        pPVar12 = pPVar12 + 1;
        iVar6 = extraout_ECX + 1;
      }
      bVar15 = (PckEntryHeader *)0xfffffdff < pPVar13;
      pPVar13 = pPVar13 + 1;
    } while (extraout_EDX != 1);
    if (iVar6 != 0) {
      iVar9 = iVar6 + -1;
      dVar5 = 0x200;
      bVar15 = false;
      if (iVar9 != 0) {
        pPVar13 = outputEntries + 1;
        do {
          do {
            iVar7 = 0x100;
            pPVar12 = pPVar13;
            pPVar11 = outputEntries;
            do {
              if (iVar7 == 0) break;
              iVar7 = iVar7 + -1;
              pwVar3 = pPVar11->path;
              pwVar4 = pPVar12->path;
              bVar15 = *pwVar4 < *pwVar3;
              pPVar12 = (PckEntryHeader *)(pPVar12->path + 1);
              pPVar11 = (PckEntryHeader *)(pPVar11->path + 1);
            } while (*pwVar4 == *pwVar3);
            if (bVar15) {
              iVar7 = 0x80;
              do {
                pPVar11 = outputEntries;
                pPVar12 = pPVar13;
                LOCK();
                uVar1 = *(undefined4 *)pPVar11->path;
                *(undefined4 *)pPVar11->path = *(undefined4 *)pPVar12->path;
                UNLOCK();
                *(undefined4 *)pPVar12->path = uVar1;
                iVar7 = iVar7 + -1;
                pPVar13 = (PckEntryHeader *)(pPVar12->path + 2);
                outputEntries = (PckEntryHeader *)(pPVar11->path + 2);
              } while (iVar7 != 0);
              pPVar13 = (PckEntryHeader *)(pPVar12[-1].path + 2);
              outputEntries = (PckEntryHeader *)(pPVar11[-1].path + 2);
            }
            bVar15 = (PckEntryHeader *)0xfffffdff < pPVar13;
            pPVar13 = pPVar13 + 1;
            iVar9 = iVar9 + -1;
          } while (iVar9 != 0);
          iVar9 = iVar6 + -2;
          iVar6 = iVar6 + -1;
          pPVar13 = outputEntries + 2;
          bVar15 = false;
          outputEntries = outputEntries + 1;
        } while (iVar9 != 0);
        dVar5 = 0x200;
      }
    }
  }
  return dVar5;
}

/* Address: 0x0040E500.
   Ownership: assets/package/runtime.
   Purpose: Finds the mount slot by file handle, frees its entry-header array, closes the file, and clears all
   three slot fields.
*/
void Package_Unmount(EngineFileHandle fileHandle)

{
  EngineFileHandle EVar1;
  EngineFileHandle mountSlotsRemaining;
  PckMountSlot *mountSlotCursor;
  
  mountSlotCursor = g_PackageMountSlots;
  mountSlotsRemaining = 0x400;
  EVar1 = fileHandle;
  while( true ) {
    if (EVar1 == 0) {
      return;
    }
    if (fileHandle == mountSlotCursor->fileHandle) break;
    mountSlotCursor = mountSlotCursor + 1;
    mountSlotsRemaining = mountSlotsRemaining - 1;
    EVar1 = mountSlotsRemaining;
  }
  (*g_MemoryApi.free)(mountSlotCursor->entryHeaders);
  (*g_FileSystemClose)((void *)fileHandle);
  mountSlotCursor->fileHandle = 0;
  mountSlotCursor->entryHeaders = (PckEntryHeader *)0x0;
  mountSlotCursor->entryCount = 0;
  return;
}

/* Address: 0x0040ECA0.
   Ownership: assets/package/runtime.
   Purpose: Compares a UTF-16 archive path against a pattern. '?' matches one word. '*' advances the candidate to
   the next dot or terminator rather than implementing unrestricted globbing. CF clear means match.
*/
dword Package_WildcardPathMatches(word *pattern,word *candidate)

{
  dword in_EAX;
  word patternCodeUnit;
  
  while( true ) {
    while( true ) {
      patternCodeUnit = *pattern;
      pattern = pattern + 1;
      if (patternCodeUnit != 0x2a) break;
      for (; (*candidate != 0x2e && (*candidate != 0)); candidate = candidate + 1) {
      }
    }
    if ((patternCodeUnit != 0x3f) && (patternCodeUnit != *candidate)) break;
    candidate = candidate + 1;
    if (patternCodeUnit == 0) {
      return in_EAX;
    }
  }
  return in_EAX;
}

/* Address: 0x0040EAF0.
   Ownership: assets/package/runtime.
   Purpose: Seeks to runtimePayloadOffset + 0x200, reads packedSize bytes into g_PackageScratchBuffer, and
   dispatches compressionMethod through g_PckDecoderTable into destination. CF reports failure.
   Local calls: Package_SetLastErrorPath.
*/
dword Package_DecodeEntryInto(byte *destination,PckEntryHeader *entry,EngineFileHandle fileHandle)

{
  dword decoderStatusCode;
  dword extraout_EAX;
  int extraout_EDX;
  bool bVar1;
  
  bVar1 = 0xfffffdff < entry->runtimePayloadOffset;
  (*g_FileSystemSeekCf)
            (FILESYSTEM_SEEK_BEGIN,entry->runtimePayloadOffset + 0x200,(void *)fileHandle);
  if (!bVar1) {
    (*g_FileSystemReadExactCf)(entry->packedSize,g_PackageScratchBuffer,(void *)fileHandle);
    if (!bVar1) {
      decoderStatusCode =
           (*g_PckDecoderTable[extraout_EDX])
                     (entry->unpackedSize,destination,entry->packedSize,g_PackageScratchBuffer);
      if (!bVar1) {
        return decoderStatusCode;
      }
    }
  }
  Package_SetLastErrorPath(entry->path);
  return extraout_EAX;
}

/* Address: 0x0040E2B0.
   Ownership: assets/package/runtime.
   Purpose: Copies up to 256 UTF-16 words from path into g_PackageLastErrorPath. The buffer is used by package and
   loose-file load failures.
*/
void Package_SetLastErrorPath(word *path)

{
  word wVar1;
  int iVar2;
  word *pwVar3;
  word *pwVar4;
  
  iVar2 = 0x100;
  pwVar4 = path;
  do {
    pwVar3 = pwVar4;
    if (iVar2 == 0) break;
    iVar2 = iVar2 + -1;
    pwVar3 = pwVar4 + 1;
    wVar1 = *pwVar4;
    pwVar4 = pwVar3;
  } while (wVar1 != 0);
  iVar2 = (int)pwVar3 - (int)path;
  pwVar4 = g_PackageLastErrorPath;
  for (; iVar2 != 0; iVar2 = iVar2 + -1) {
    *pwVar4 = *path;
    path = path + 1;
    pwVar4 = pwVar4 + 1;
  }
  return;
}

/* Address: 0x0040E640.
   Ownership: assets/package/runtime.
   Purpose: Finds one exact lowercase UTF-16 path in the selected mounted archive. CF clear returns a
   PckEntryHeader pointer; CF set indicates failure.
*/
PckEntryHeader * Package_FindEntryInMount(word *path,EngineFileHandle fileHandle)

{
  PckEntryHeader *pPVar1;
  int iVar2;
  int iVar3;
  PckEntryCount PVar4;
  PckMountSlot *pPVar5;
  word *pwVar6;
  PckEntryHeader *pPVar7;
  bool bVar9;
  PckEntryHeader *pPVar8;
  
  pPVar1 = (PckEntryHeader *)0x0;
  bVar9 = true;
  iVar2 = 0xf6;
  pwVar6 = path;
  do {
    if (iVar2 == 0) break;
    iVar2 = iVar2 + -1;
    bVar9 = *pwVar6 == 0;
    pwVar6 = pwVar6 + 1;
  } while (!bVar9);
  if (!bVar9) {
    return pPVar1;
  }
  pPVar5 = g_PackageMountSlots;
  iVar3 = 0x400;
  while (fileHandle != pPVar5->fileHandle) {
    pPVar5 = pPVar5 + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) {
      return pPVar1;
    }
  }
  pPVar7 = pPVar5->entryHeaders;
  PVar4 = pPVar5->entryCount;
  bVar9 = false;
  iVar3 = -(iVar2 + -0xf6);
  pwVar6 = path;
  pPVar8 = pPVar7;
  if (PVar4 == 0) {
    return pPVar1;
  }
code_r0x0040e6b4:
  do {
    if (iVar3 != 0) {
      bVar9 = *pwVar6 == pPVar7->path[0];
      iVar3 = iVar3 + -1;
      pwVar6 = pwVar6 + 1;
      pPVar7 = (PckEntryHeader *)(pPVar7->path + 1);
      if (bVar9) goto code_r0x0040e6b4;
    }
    if (bVar9) {
      return pPVar8;
    }
    pPVar7 = pPVar8 + 1;
    PVar4 = PVar4 - 1;
    bVar9 = false;
    iVar3 = -(iVar2 + -0xf6);
    pwVar6 = path;
    pPVar8 = pPVar7;
    if (PVar4 == 0) {
      return pPVar1;
    }
  } while( true );
}

/* Address: 0x0040EA20.
   Ownership: assets/package/runtime.
   Purpose: Lowercases the caller's path in place and searches mounted archives from slot 0 upward, establishing
   first-mounted-wins precedence. CF clear returns the matching entry header.
*/
PckEntryHeader * Package_FindEntryAcrossMounts(word *path)

{
  PckEntryHeader *pPVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  PckEntryCount PVar5;
  PckMountSlot *pPVar6;
  word *pwVar7;
  PckEntryHeader *pPVar8;
  bool bVar9;
  
  pPVar1 = (PckEntryHeader *)0x0;
  iVar2 = 0xf6;
  pwVar7 = path;
  do {
    iVar3 = iVar2;
    pPVar1 = (PckEntryHeader *)CONCAT22((short)((uint)pPVar1 >> 0x10),*pwVar7);
    if (((PckEntryHeader *)0x40 < pPVar1) && (pPVar1 < (PckEntryHeader *)0x5b)) {
      pPVar1 = (PckEntryHeader *)(pPVar1->path + 0x10);
    }
    *pwVar7 = (word)pPVar1;
    iVar2 = iVar3 + -1;
    if (iVar2 == 0) {
      return pPVar1;
    }
    pwVar7 = pwVar7 + 1;
  } while (pPVar1 != (PckEntryHeader *)0x0);
  pPVar6 = g_PackageMountSlots;
  iVar4 = 0x400;
  iVar2 = -(iVar3 + -0xf7);
  do {
    pPVar1 = pPVar6->entryHeaders;
    PVar5 = pPVar6->entryCount;
    if ((pPVar1 != (PckEntryHeader *)0x0) && (PVar5 != 0)) {
      bVar9 = pPVar1 == (PckEntryHeader *)0x0;
      iVar3 = iVar2;
      pwVar7 = path;
      pPVar8 = pPVar1;
code_r0x0040eaa7:
      do {
        if (iVar3 != 0) {
          bVar9 = *pwVar7 == pPVar1->path[0];
          iVar3 = iVar3 + -1;
          pwVar7 = pwVar7 + 1;
          pPVar1 = (PckEntryHeader *)(pPVar1->path + 1);
          if (bVar9) goto code_r0x0040eaa7;
        }
        if (bVar9) {
          return pPVar8;
        }
        pPVar1 = pPVar8 + 1;
        PVar5 = PVar5 - 1;
        bVar9 = PVar5 == 0;
        iVar3 = iVar2;
        pwVar7 = path;
        pPVar8 = pPVar1;
      } while (!bVar9);
    }
    pPVar6 = pPVar6 + 1;
    iVar4 = iVar4 + -1;
    if (iVar4 == 0) {
      return (PckEntryHeader *)0x0;
    }
  } while( true );
}

/* Address: 0x0040E570.
   Ownership: assets/package/runtime.
   Purpose: Reads the 0x200-byte archive header, caches entryCount, reads every 0x200-byte entry header, and fills
   runtimePayloadOffset while skipping each packed payload.
*/
dword Package_ReadDirectory(EngineFileHandle fileHandle)

{
  PckEntryCount PVar1;
  dword dVar2;
  uint uVar3;
  int extraout_ECX;
  int iVar4;
  uint uVar5;
  PckMountSlot *pPVar6;
  PckEntryHeader *destination;
  bool bVar7;
  
  pPVar6 = g_PackageMountSlots;
  iVar4 = 0x400;
  while (bVar7 = fileHandle < pPVar6->fileHandle, fileHandle != pPVar6->fileHandle) {
    pPVar6 = pPVar6 + 1;
    iVar4 = iVar4 + -1;
    if (iVar4 == 0) {
      return 0x14;
    }
  }
  destination = pPVar6->entryHeaders;
  dVar2 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  if (bVar7) {
    return dVar2;
  }
  dVar2 = (*g_FileSystemReadExactCf)(0x200,g_PackageScratchBuffer,(void *)fileHandle);
  if (!bVar7) {
    PVar1 = *(PckEntryCount *)((int)((ulonglong)dVar2 >> 0x20) + 0xb0);
    pPVar6->entryCount = PVar1;
    if (PVar1 == 0) {
      return (dword)dVar2;
    }
    while( true ) {
      bVar7 = false;
      dVar2 = (*g_FileSystemReadExactCf)(0x200,destination,(void *)fileHandle);
      uVar5 = (uint)((ulonglong)dVar2 >> 0x20);
      if (bVar7) {
        return (dword)dVar2;
      }
      destination->runtimePayloadOffset = uVar5;
      uVar3 = destination->packedSize + 0x200;
      destination = destination + 1;
      bVar7 = CARRY4(uVar5,uVar3);
      dVar2 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,uVar5 + uVar3,(void *)fileHandle);
      if (bVar7) break;
      if (extraout_ECX == 1) {
        return dVar2;
      }
    }
    return dVar2;
  }
  return (dword)dVar2;
}
