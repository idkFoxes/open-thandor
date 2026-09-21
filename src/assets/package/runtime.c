/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/package/runtime.h>

/* Implementation ownership: assets/package/runtime. */

/* Address: 0x005460E0.
   Ownership: assets/package/runtime.
   Purpose: Handles level package validate and mount.
   Local calls: Package_Mount, Package_FindEntry, Package_LoadEntry, Package_Unmount.
   Cross-module calls: Resource_Release [assets/resource/runtime], TextResourcePage_LoadCompatibilityAliases
   [assets/text/resources].
*/
bool __thandor_cf_preserve_eax_ecx_edx LevelPackage_ValidateAndMount(word *levelPathUtf16)

{
  dword aliasAddressBase;
  EngineFileHandle fileHandle;
  int *allocation;
  bool bVar1;
  StatusValueEaxCf5 SVar2;
  PackageLoadEntryEaxCf5 PVar3;
  PackageFindEntryEaxEcxCf9 PVar4;
  
  SVar2 = Package_Mount(levelPathUtf16);
  bVar1 = SVar2.carry;
  fileHandle = SVar2.valueOrError;
  if (!bVar1) {
    PVar4 = Package_FindEntry(0x200,(PckEntryHeader *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15),
                              (word *)u_level___lev_005460a6,fileHandle);
    if ((!PVar4.carry) && (PVar4.matchCount != 0)) {
      PVar3 = Package_LoadEntry((word *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15));
      allocation = PVar3.bufferOrError;
      if (!PVar3.carry) {
        if ((*allocation == 0x76656c) && (allocation[3] == 0x70001)) {
          aliasAddressBase = allocation[0x5c];
          Resource_Release(allocation);
          PVar4 = Package_FindEntry(0x200,(PckEntryHeader *)
                                          (s_NAME__CLIENT__KARTE___00545e91 + 0x15),
                                    (word *)u_level___str_005460be,fileHandle);
          if (((!PVar4.carry) && (PVar4.matchCount != 0)) &&
             (bVar1 = TextResourcePage_LoadCompatibilityAliases
                                (aliasAddressBase,(word *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15))
             , !bVar1)) {
            return bVar1;
          }
        }
        else {
          Resource_Release(allocation);
        }
      }
    }
    Package_Unmount(fileHandle);
    bVar1 = true;
  }
  return bVar1;
}


/* Address: 0x0040E840.
   Ownership: assets/package/runtime.
   Purpose: Replaces an existing path when present, then appends a new 0x200-byte entry header and encoded payload.
   compressionMethod indexes the verified three-entry encoder table; method 1 writes a four-byte-aligned stored
   payload directly. The first payload dword becomes typeTag. CF reports failure and the mounted directory is
   refreshed after success.
   Local calls: Package_FindEntryInMount, Package_DeleteEntry, Package_ReadDirectory.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   dword *sourceData,word *path,EngineFileHandle fileHandle)

{
  byte *destination;
  dword dVar1;
  FileIoByteCount byteCount;
  uint byteCount_00;
  int iVar2;
  byte *pbVar3;
  PackageEntryEaxCf5 PVar4;
  StatusValueEaxCf5 SVar5;
  FileSystemSeekEaxCf5 FVar6;
  FileSystemReadEaxCf5 FVar7;
  PckCodecEaxCf5 PVar8;
  FileSystemWriteEaxCf5 FVar9;
  
  destination = g_PackageScratchBuffer;
  PVar4 = Package_FindEntryInMount(path,fileHandle);
  if (!PVar4.carry) {
    SVar5 = Package_DeleteEntry(path,fileHandle);
    dVar1 = SVar5.valueOrError;
    if (SVar5.carry) goto LAB_0040ea0c;
  }
  FVar6 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  dVar1 = FVar6.eax;
  if (!FVar6.carry) {
    FVar7 = (*g_FileSystemReadExactCf)(0x200,destination,(void *)fileHandle);
    dVar1 = FVar7.eax;
    if (!FVar7.carry) {
      FVar6 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      dVar1 = FVar6.eax;
      if (!FVar6.carry) {
        *(int *)(destination + 0xb0) = *(int *)(destination + 0xb0) + 1;
        if (compressionMethod == PCK_COMPRESSION_STORED) {
          byteCount_00 = unpackedSize + 3 & 0xfffffffc;
          *(uint *)(destination + 0x3f8) = byteCount_00;
          destination[0x3fc] = 1;
          destination[0x3fd] = 0;
          destination[0x3fe] = 0;
          destination[0x3ff] = 0;
          *(uint *)(destination + 4) = *(int *)(destination + 4) + byteCount_00 + 0x200;
          destination[0x3ec] = 0;
          destination[0x3ed] = 0;
          destination[0x3ee] = 0;
          destination[0x3ef] = 0;
          *(dword *)(destination + 0x3f4) = *sourceData;
          *(PckDecodedByteCount *)(destination + 0x3f0) = unpackedSize;
          FVar9 = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination,(void *)fileHandle);
          dVar1 = FVar9.eax;
          if (FVar9.carry) goto LAB_0040ea0c;
          pbVar3 = destination + 0x200;
          for (iVar2 = 0x7b; iVar2 != 0; iVar2 = iVar2 + -1) {
            *(undefined4 *)pbVar3 = *(undefined4 *)path;
            path = path + 2;
            pbVar3 = pbVar3 + 4;
          }
          FVar6 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_END,0,(void *)fileHandle);
          dVar1 = FVar6.eax;
          if (FVar6.carry) goto LAB_0040ea0c;
          FVar9 = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination + 0x200,(void *)fileHandle);
          dVar1 = FVar9.eax;
          if (FVar9.carry) goto LAB_0040ea0c;
          FVar9 = (*g_FileSystemWriteExactOrFlushCf)(byteCount_00,sourceData,(void *)fileHandle);
          dVar1 = FVar9.eax;
          if (FVar9.carry) goto LAB_0040ea0c;
        }
        else {
          PVar8 = (*g_PckEncoderTable[compressionMethod])
                            (0x7ffc00,destination + 0x400,unpackedSize,(byte *)sourceData);
          dVar1 = PVar8.eax;
          if (PVar8.carry) goto LAB_0040ea0c;
          *(dword *)(destination + 0x3f8) = dVar1;
          byteCount = dVar1 + 0x200;
          *(PckCompressionMethod *)(destination + 0x3fc) = compressionMethod;
          *(FileIoByteCount *)(destination + 4) = *(int *)(destination + 4) + byteCount;
          destination[0x3ec] = 0;
          destination[0x3ed] = 0;
          destination[0x3ee] = 0;
          destination[0x3ef] = 0;
          *(dword *)(destination + 0x3f4) = *sourceData;
          *(PckDecodedByteCount *)(destination + 0x3f0) = unpackedSize;
          FVar9 = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination,(void *)fileHandle);
          dVar1 = FVar9.eax;
          if (FVar9.carry) goto LAB_0040ea0c;
          pbVar3 = destination + 0x200;
          for (iVar2 = 0x7b; iVar2 != 0; iVar2 = iVar2 + -1) {
            *(undefined4 *)pbVar3 = *(undefined4 *)path;
            path = path + 2;
            pbVar3 = pbVar3 + 4;
          }
          FVar6 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_END,0,(void *)fileHandle);
          dVar1 = FVar6.eax;
          if (FVar6.carry) goto LAB_0040ea0c;
          FVar9 = (*g_FileSystemWriteExactOrFlushCf)
                            (byteCount,destination + 0x200,(void *)fileHandle);
          dVar1 = FVar9.eax;
          if (FVar9.carry) goto LAB_0040ea0c;
        }
        SVar5 = Package_ReadDirectory(fileHandle);
        dVar1 = SVar5.valueOrError;
        if (!SVar5.carry) {
          return (StatusValueEaxCf5)((uint5)SVar5 & 0xffffffff);
        }
      }
    }
  }
LAB_0040ea0c:
  SVar5.carry = true;
  SVar5.valueOrError = dVar1;
  return SVar5;
}


/* Address: 0x0040ED00.
   Ownership: assets/package/runtime.
   Purpose: Loads a package entry into a caller-provided buffer when allowed by flags and capacity, otherwise falls
   back to the loose-file path. The high two bits of capacityAndFlags control package bypass and alternate loose-
   path handling.
   Local calls: Package_FindEntryAcrossMounts, Package_DecodeEntryInto, Package_SetLastErrorPath.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_LoadEntryIntoBuffer
          (PckLoadCapacityFlags bufferCapacityAndLoadFlags,byte *destination,word *path)

{
  void *pvVar1;
  PckEntryHeader *entry;
  void *handle;
  void *byteCount;
  PackageDecodeEaxCf5 PVar2;
  StatusValueEaxCf5 SVar3;
  FileSystemOpenEaxCf5 FVar4;
  FileSystemSizeEaxCf5 FVar5;
  FileSystemReadEaxCf5 FVar6;
  StatusValueEaxCf5 SVar7;
  StatusValueEaxCf5 SVar8;
  PackageFindEntryEaxEbxCf9 PVar9;
  
  pvVar1 = (void *)(bufferCapacityAndLoadFlags & 0x3fffffff);
  if ((bufferCapacityAndLoadFlags & 0x80000000) == 0) {
    PVar9 = Package_FindEntryAcrossMounts(path);
    entry = (PckEntryHeader *)PVar9.eax;
    if (!PVar9.carry) {
      handle = (void *)0x5;
      if ((((void *)entry->unpackedSize <= pvVar1) && (entry->packedSize < 0x800001)) &&
         (destination != g_PackageScratchBuffer)) {
        PVar2 = Package_DecodeEntryInto(destination,entry,PVar9.ebx);
        SVar3.valueOrError = PVar2.eax;
        SVar3.carry = PVar2.carry;
        return SVar3;
      }
      Package_SetLastErrorPath(path);
      goto LAB_0040ee1c;
    }
  }
  if ((bufferCapacityAndLoadFlags & 0x40000000) == 0) {
    FVar4 = (*g_FileSystemOpenCf)(0,path);
    handle = (void *)FVar4.eax;
    if (FVar4.carry) goto LAB_0040ee1c;
  }
  else {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
               (word *)&g_ExecutableDirectoryUtf16);
    FVar4 = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    handle = (void *)FVar4.eax;
    if (FVar4.carry) {
      FVar4 = (*g_FileSystemOpenCf)(0,path);
      handle = (void *)FVar4.eax;
      if (FVar4.carry) goto LAB_0040ee1c;
    }
  }
  FVar5 = (*g_FileSystemGetSizeCf)(handle);
  byteCount = (void *)FVar5.eax;
  if (!FVar5.carry) {
    if ((pvVar1 < byteCount) && (byteCount = pvVar1, (void *)0x7fffff < pvVar1)) {
      byteCount = (void *)0x5;
    }
    else {
      FVar6 = (*g_FileSystemReadExactCf)((FileIoByteCount)byteCount,destination,handle);
      byteCount = (void *)FVar6.eax;
      if (!FVar6.carry) {
        (*g_FileSystemClose)(handle);
        SVar7.carry = false;
        SVar7.valueOrError = (dword)byteCount;
        return SVar7;
      }
    }
  }
  (*g_FileSystemClose)(handle);
  handle = byteCount;
LAB_0040ee1c:
  SVar8.carry = true;
  SVar8.valueOrError = (dword)handle;
  return SVar8;
}


/* Address: 0x0040E450.
   Ownership: assets/package/runtime.
   Purpose: Mounts a package into the last free slot while scanning the 1,024-slot mount table backward. Because
   lookups scan forward, this archive has the lowest precedence. Startup uses this path for engine.pck.
   Local calls: Package_ReadDirectory.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Package_MountLowPriority(word *path)

{
  PckEntryHeader *handle;
  PckEntryHeader *allocatedEntryHeaders;
  int iVar1;
  PckMountSlot *mountSlot;
  StatusValueEaxCf5 SVar2;
  FileSystemOpenEaxCf5 FVar3;
  ArenaAllocEaxCf5 AVar4;
  StatusValueEaxCf5 SVar5;
  
  mountSlot = g_PackageMountSlots + 0x3ff;
  iVar1 = 0x400;
  do {
    if (mountSlot->fileHandle == 0) {
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (word *)&g_ExecutableDirectoryUtf16);
      FVar3 = (*g_FileSystemOpenCf)
                        (FILESYSTEM_OPEN_WRITE_ACCESS,(word *)&g_FileSystemCombinedPathScratchUtf16)
      ;
      handle = (PckEntryHeader *)FVar3.eax;
      if (FVar3.carry) {
        FVar3 = (*g_FileSystemOpenCf)(FILESYSTEM_OPEN_WRITE_ACCESS,path);
        handle = (PckEntryHeader *)FVar3.eax;
        if (FVar3.carry) goto LAB_0040e480;
      }
      AVar4 = (*g_MemoryApi.alloc)(0x80000);
      allocatedEntryHeaders = (PckEntryHeader *)AVar4.eax;
      if (!AVar4.carry) {
        mountSlot->fileHandle = (EngineFileHandle)handle;
        mountSlot->entryHeaders = allocatedEntryHeaders;
        mountSlot->entryCount = 0;
        Package_ReadDirectory((EngineFileHandle)handle);
        SVar5.carry = false;
        SVar5.valueOrError = (dword)handle;
        return SVar5;
      }
      (*g_FileSystemClose)(handle);
      handle = allocatedEntryHeaders;
      goto LAB_0040e480;
    }
    mountSlot = mountSlot + -1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  handle = (PckEntryHeader *)0x14;
LAB_0040e480:
  SVar2.carry = true;
  SVar2.valueOrError = (dword)handle;
  return SVar2;
}


/* Address: 0x0040E6F0.
   Ownership: assets/package/runtime.
   Purpose: Deletes one exact path from a writable mounted PCK. Missing entries are treated as success. The routine
   rewrites the archive header, compacts all following bytes through the shared scratch buffer, truncates the file,
   and reloads the in-memory directory. CF reports file or rewrite failure.
   Local calls: Package_FindEntryInMount, Package_ReadDirectory.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_DeleteEntry(word *path,EngineFileHandle fileHandle)

{
  PckStoredByteCount PVar1;
  int iVar2;
  byte *destination;
  PckEntryHeader *pPVar3;
  FileSystemFilePosition distance;
  PckEntryHeader *pPVar4;
  uint byteCount;
  PackageEntryEaxCf5 PVar5;
  FileSystemSeekEaxCf5 FVar6;
  FileSystemReadEaxCf5 FVar7;
  FileSystemWriteEaxCf5 FVar8;
  StatusValueEaxCf5 SVar9;
  StatusValueEaxCf5 SVar10;
  
  destination = g_PackageScratchBuffer;
  PVar5 = Package_FindEntryInMount(path,fileHandle);
  pPVar3 = PVar5.entry;
  pPVar4 = pPVar3;
  if (PVar5.carry) {
LAB_0040e81c:
    SVar10.carry = false;
    SVar10.valueOrError = (dword)pPVar4;
    return SVar10;
  }
  FVar6 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  pPVar4 = (PckEntryHeader *)FVar6.eax;
  if (!FVar6.carry) {
    PVar1 = pPVar3->packedSize;
    FVar7 = (*g_FileSystemReadExactCf)(0x200,destination,(void *)fileHandle);
    pPVar4 = (PckEntryHeader *)FVar7.eax;
    if (!FVar7.carry) {
      iVar2 = *(int *)(destination + 4);
      *(int *)(destination + 0xb0) = *(int *)(destination + 0xb0) + -1;
      *(PckStoredByteCount *)(destination + 4) = *(int *)(destination + 4) - (PVar1 + 0x200);
      FVar6 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      pPVar4 = (PckEntryHeader *)FVar6.eax;
      if (!FVar6.carry) {
        FVar8 = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination,(void *)fileHandle);
        pPVar4 = (PckEntryHeader *)FVar8.eax;
        if (!FVar8.carry) {
          distance = pPVar3->runtimePayloadOffset + pPVar3->packedSize + 0x200;
          byteCount = iVar2 - distance;
          if (byteCount == 0) {
            FVar6 = (*g_FileSystemSeekCf)
                              (FILESYSTEM_SEEK_BEGIN,pPVar3->runtimePayloadOffset,(void *)fileHandle
                              );
            pPVar4 = (PckEntryHeader *)FVar6.eax;
            if (FVar6.carry) goto LAB_0040e828;
            FVar8 = (*g_FileSystemWriteExactOrFlushCf)(0,(void *)0x0,(void *)fileHandle);
            pPVar4 = (PckEntryHeader *)FVar8.eax;
            if (FVar8.carry) goto LAB_0040e828;
          }
          else {
            FVar6 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,distance,(void *)fileHandle);
            pPVar4 = (PckEntryHeader *)FVar6.eax;
            if ((FVar6.carry) || (pPVar4 = (PckEntryHeader *)0x14, 0x800000 < byteCount))
            goto LAB_0040e828;
            FVar7 = (*g_FileSystemReadExactCf)(byteCount,g_PackageScratchBuffer,(void *)fileHandle);
            pPVar4 = (PckEntryHeader *)FVar7.eax;
            if (FVar7.carry) goto LAB_0040e828;
            FVar6 = (*g_FileSystemSeekCf)
                              (FILESYSTEM_SEEK_BEGIN,pPVar3->runtimePayloadOffset,(void *)fileHandle
                              );
            pPVar4 = (PckEntryHeader *)FVar6.eax;
            if (FVar6.carry) goto LAB_0040e828;
            FVar8 = (*g_FileSystemWriteExactOrFlushCf)
                              (byteCount,g_PackageScratchBuffer,(void *)fileHandle);
            pPVar4 = (PckEntryHeader *)FVar8.eax;
            if (FVar8.carry) goto LAB_0040e828;
            FVar8 = (*g_FileSystemWriteExactOrFlushCf)(0,(void *)0x0,(void *)fileHandle);
            pPVar4 = (PckEntryHeader *)FVar8.eax;
            if (FVar8.carry) goto LAB_0040e828;
          }
          SVar9 = Package_ReadDirectory(fileHandle);
          pPVar4 = (PckEntryHeader *)SVar9.valueOrError;
          if (!SVar9.carry) goto LAB_0040e81c;
        }
      }
    }
  }
LAB_0040e828:
  SVar9.carry = true;
  SVar9.valueOrError = (dword)pPVar4;
  return SVar9;
}


/* Address: 0x0040EE30.
   Ownership: assets/package/runtime.
   Purpose: Loads any package or loose-file asset into a newly allocated buffer. The returned allocation is untyped
   at this layer; callers cast it to gfx, fld, lev, mdl, sound, text, and other asset types. CF reports success or
   failure.
   Local calls: Package_FindEntryAcrossMounts, Package_DecodeEntryInto.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
PackageLoadEntryEaxCf5 __thandor_eax_cf_preserve_ecx_edx Package_LoadEntry(word *path)

{
  PckEntryHeader *entry;
  byte *destination;
  byte *pbVar1;
  ArenaAllocEaxCf5 AVar2;
  PackageDecodeEaxCf5 PVar3;
  FileSystemOpenEaxCf5 FVar4;
  FileSystemSizeEaxCf5 FVar5;
  FileSystemReadEaxCf5 FVar6;
  PackageLoadEntryEaxCf5 PVar7;
  PackageFindEntryEaxEbxCf9 PVar8;
  
  PVar8 = Package_FindEntryAcrossMounts(path);
  entry = (PckEntryHeader *)PVar8.eax;
  if (PVar8.carry) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
               (word *)&g_ExecutableDirectoryUtf16);
    FVar4 = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    destination = (byte *)FVar4.eax;
    if (FVar4.carry) {
      FVar4 = (*g_FileSystemOpenCf)(0,path);
      destination = (byte *)FVar4.eax;
      if (FVar4.carry) goto LAB_0040ef3f;
    }
    FVar5 = (*g_FileSystemGetSizeCf)(destination);
    pbVar1 = (byte *)FVar5.eax;
    if (!FVar5.carry) {
      AVar2 = (*g_MemoryApi.alloc)((dword)pbVar1);
      if (AVar2.carry) {
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)pbVar1,g_FatalErrorDetail1Utf16);
        pbVar1 = (byte *)0x5;
      }
      else {
        FVar6 = (*g_FileSystemReadExactCf)((FileIoByteCount)pbVar1,(void *)AVar2.eax,destination);
        pbVar1 = (byte *)FVar6.eax;
        if (!FVar6.carry) {
          (*g_FileSystemClose)(destination);
          return (PackageLoadEntryEaxCf5)((uint5)AVar2 & 0xffffffff);
        }
        (*g_MemoryApi.free)((void *)AVar2.eax);
      }
    }
    (*g_FileSystemClose)(destination);
    destination = pbVar1;
  }
  else {
    destination = (byte *)0x5;
    if (entry->packedSize < 0x800001) {
      AVar2 = (*g_MemoryApi.alloc)(entry->unpackedSize);
      destination = (byte *)AVar2.eax;
      if (!AVar2.carry) {
        PVar3 = Package_DecodeEntryInto(destination,entry,PVar8.ebx);
        if (!PVar3.carry) {
          return (PackageLoadEntryEaxCf5)((uint5)AVar2 & 0xffffffff);
        }
        pbVar1 = (byte *)PVar3.eax;
        (*g_MemoryApi.free)(destination);
        destination = pbVar1;
      }
    }
  }
LAB_0040ef3f:
  PVar7.carry = true;
  PVar7.bufferOrError = destination;
  return PVar7;
}


/* Address: 0x0040E3A0.
   Ownership: assets/package/runtime.
   Purpose: Mounts a package in the first free slot. Lookup scans the table in the same direction, so earlier
   normal mounts have higher precedence.
   Local calls: Package_ReadDirectory.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Package_Mount(word *path)

{
  PckEntryHeader *handle;
  PckEntryHeader *allocatedEntryHeaders;
  int iVar1;
  PckMountSlot *mountSlot;
  StatusValueEaxCf5 SVar2;
  FileSystemOpenEaxCf5 FVar3;
  ArenaAllocEaxCf5 AVar4;
  StatusValueEaxCf5 SVar5;
  
  mountSlot = g_PackageMountSlots;
  iVar1 = 0x400;
  do {
    if (mountSlot->fileHandle == 0) {
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (word *)&g_ExecutableDirectoryUtf16);
      FVar3 = (*g_FileSystemOpenCf)
                        (FILESYSTEM_OPEN_WRITE_ACCESS,(word *)&g_FileSystemCombinedPathScratchUtf16)
      ;
      handle = (PckEntryHeader *)FVar3.eax;
      if (FVar3.carry) {
        FVar3 = (*g_FileSystemOpenCf)(FILESYSTEM_OPEN_WRITE_ACCESS,path);
        handle = (PckEntryHeader *)FVar3.eax;
        if (FVar3.carry) goto LAB_0040e3d0;
      }
      AVar4 = (*g_MemoryApi.alloc)(0x80000);
      allocatedEntryHeaders = (PckEntryHeader *)AVar4.eax;
      if (!AVar4.carry) {
        mountSlot->fileHandle = (EngineFileHandle)handle;
        mountSlot->entryHeaders = allocatedEntryHeaders;
        mountSlot->entryCount = 0;
        Package_ReadDirectory((EngineFileHandle)handle);
        SVar5.carry = false;
        SVar5.valueOrError = (dword)handle;
        return SVar5;
      }
      (*g_FileSystemClose)(handle);
      handle = allocatedEntryHeaders;
      goto LAB_0040e3d0;
    }
    mountSlot = mountSlot + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  handle = (PckEntryHeader *)0x14;
LAB_0040e3d0:
  SVar2.carry = true;
  SVar2.valueOrError = (dword)handle;
  return SVar2;
}


/* Address: 0x0040EB70.
   Ownership: assets/package/runtime.
   Purpose: Collects entry paths in one mounted package that match the package wildcard grammar, copies them into
   0x200-byte output slots, and sorts the results lexicographically. Returns 0x200 with CF clear or error 0x14 with
   CF set.
   Local calls: Package_WildcardPathMatches.
*/
PackageFindEntryEaxEcxCf9 __thandor_eax_cf_preserve_edx
Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries,
                 word *pattern,EngineFileHandle fileHandle)

{
  EngineFileHandle EVar1;
  word *pwVar2;
  word *pwVar3;
  undefined4 in_ECX;
  int iVar4;
  int iVar5;
  int iVar6;
  EngineFileHandle EVar7;
  PckEntryCount PVar8;
  int iVar9;
  PckMountSlot *pPVar10;
  PckEntryHeader *pPVar11;
  PckEntryHeader *pPVar12;
  PckEntryHeader *pPVar13;
  PckEntryHeader *pPVar14;
  bool bVar15;
  PackageFindEntryEaxEcxCf9 PVar16;
  PackageFindEntryEaxEcxCf9 PVar17;
  dword swappedDword;
  
  pPVar10 = g_PackageMountSlots;
  EVar7 = 0x400;
  EVar1 = fileHandle;
  while( true ) {
    if (EVar1 == 0) {
      PVar16.matchCount = in_ECX;
      PVar16.recordSizeOrError = 0x14;
      PVar16.carry = true;
      return PVar16;
    }
    if (fileHandle == pPVar10->fileHandle) break;
    pPVar10 = pPVar10 + 1;
    EVar7 = EVar7 - 1;
    EVar1 = EVar7;
  }
  PVar8 = pPVar10->entryCount;
  pPVar13 = pPVar10->entryHeaders;
  iVar4 = 0;
  pPVar12 = outputEntries;
  if (PVar8 != 0) {
    do {
      bVar15 = Package_WildcardPathMatches(pattern,pPVar13->path);
      if (!bVar15) {
        bVar15 = outputCapacityBytes < 0x200;
        outputCapacityBytes = outputCapacityBytes - 0x200;
        if (bVar15) goto LAB_0040ec93;
        pPVar11 = pPVar13;
        pPVar14 = pPVar12;
        for (iVar5 = 0xf6; iVar5 != 0; iVar5 = iVar5 + -1) {
          pPVar14->path[0] = pPVar11->path[0];
          pPVar11 = (PckEntryHeader *)(pPVar11->path + 1);
          pPVar14 = (PckEntryHeader *)(pPVar14->path + 1);
        }
        pPVar12 = pPVar12 + 1;
        iVar4 = iVar4 + 1;
      }
      pPVar13 = pPVar13 + 1;
      PVar8 = PVar8 - 1;
    } while (PVar8 != 0);
    if (iVar4 != 0) {
      iVar5 = iVar4 + -1;
      bVar15 = false;
      if (iVar5 != 0) {
        pPVar13 = outputEntries + 1;
        iVar9 = iVar4;
        do {
          do {
            iVar6 = 0x100;
            pPVar12 = pPVar13;
            pPVar11 = outputEntries;
            do {
              if (iVar6 == 0) break;
              iVar6 = iVar6 + -1;
              pwVar2 = pPVar11->path;
              pwVar3 = pPVar12->path;
              bVar15 = *pwVar3 < *pwVar2;
              pPVar12 = (PckEntryHeader *)(pPVar12->path + 1);
              pPVar11 = (PckEntryHeader *)(pPVar11->path + 1);
            } while (*pwVar3 == *pwVar2);
            if (bVar15) {
              iVar6 = 0x80;
              do {
                pPVar11 = outputEntries;
                pPVar12 = pPVar13;
                LOCK();
                swappedDword = *(dword *)pPVar11->path;
                *(undefined4 *)pPVar11->path = *(undefined4 *)pPVar12->path;
                UNLOCK();
                *(dword *)pPVar12->path = swappedDword;
                iVar6 = iVar6 + -1;
                pPVar13 = (PckEntryHeader *)(pPVar12->path + 2);
                outputEntries = (PckEntryHeader *)(pPVar11->path + 2);
              } while (iVar6 != 0);
              pPVar13 = (PckEntryHeader *)(pPVar12[-1].path + 2);
              outputEntries = (PckEntryHeader *)(pPVar11[-1].path + 2);
            }
            bVar15 = (PckEntryHeader *)0xfffffdff < pPVar13;
            pPVar13 = pPVar13 + 1;
            iVar5 = iVar5 + -1;
          } while (iVar5 != 0);
          iVar5 = iVar9 + -2;
          iVar9 = iVar9 + -1;
          pPVar13 = outputEntries + 2;
          bVar15 = false;
          outputEntries = outputEntries + 1;
        } while (iVar5 != 0);
      }
    }
  }
LAB_0040ec93:
  PVar17.matchCount = iVar4;
  PVar17.recordSizeOrError = 0x200;
  PVar17.carry = false;
  return PVar17;
}


/* Address: 0x0040E500.
   Ownership: assets/package/runtime.
   Purpose: Finds the mount slot by file handle, frees its entry-header array, closes the file, and clears all
   three slot fields.
*/
void __thandor_preserve_eax_edx Package_Unmount(EngineFileHandle fileHandle)

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
bool __thandor_cf_preserve_eax_ecx_edx Package_WildcardPathMatches(word *pattern,word *candidate)

{
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
      return false;
    }
  }
  return true;
}


/* Address: 0x0040EAF0.
   Ownership: assets/package/runtime.
   Purpose: Seeks to runtimePayloadOffset + 0x200, reads packedSize bytes into g_PackageScratchBuffer, and
   dispatches compressionMethod through g_PckDecoderTable into destination. CF reports failure.
   Local calls: Package_SetLastErrorPath.
*/
PackageDecodeEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_DecodeEntryInto(byte *destination,PckEntryHeader *entry,EngineFileHandle fileHandle)

{
  PckCompressionMethod PVar1;
  dword decoderStatusCode;
  FileSystemSeekEaxCf5 FVar2;
  FileSystemReadEaxCf5 FVar3;
  PckCodecEaxCf5 PVar4;
  PackageDecodeEaxCf5 PVar5;
  PackageDecodeEaxCf5 PVar6;
  
  FVar2 = (*g_FileSystemSeekCf)
                    (FILESYSTEM_SEEK_BEGIN,entry->runtimePayloadOffset + 0x200,(void *)fileHandle);
  decoderStatusCode = FVar2.eax;
  if (!FVar2.carry) {
    PVar1 = entry->compressionMethod;
    FVar3 = (*g_FileSystemReadExactCf)(entry->packedSize,g_PackageScratchBuffer,(void *)fileHandle);
    decoderStatusCode = FVar3.eax;
    if (!FVar3.carry) {
      PVar4 = (*g_PckDecoderTable[PVar1])
                        (entry->unpackedSize,destination,entry->packedSize,g_PackageScratchBuffer);
      decoderStatusCode = PVar4.eax;
      if (!PVar4.carry) {
        PVar5.eax = PVar4.eax;
        PVar5.carry = PVar4.carry;
        return PVar5;
      }
    }
  }
  Package_SetLastErrorPath(entry->path);
  PVar6.carry = true;
  PVar6.eax = decoderStatusCode;
  return PVar6;
}


/* Address: 0x0040E2B0.
   Ownership: assets/package/runtime.
   Purpose: Copies up to 256 UTF-16 words from path into g_PackageLastErrorPath. The buffer is used by package and
   loose-file load failures.
*/
void __thandor_void_preserve_eax_ecx_edx Package_SetLastErrorPath(word *path)

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
PackageEntryEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_FindEntryInMount(word *path,EngineFileHandle fileHandle)

{
  int iVar1;
  int iVar2;
  PckEntryCount PVar3;
  PckMountSlot *pPVar4;
  word *pwVar5;
  PckEntryHeader *pPVar6;
  bool bVar8;
  PackageEntryEaxCf5 PVar9;
  PackageEntryEaxCf5 PVar13;
  PackageEntryEaxCf5 PVar10;
  PackageEntryEaxCf5 PVar11;
  PackageEntryEaxCf5 PVar12;
  PckEntryHeader *pPVar7;
  
  bVar8 = true;
  iVar1 = 0xf6;
  pwVar5 = path;
  do {
    if (iVar1 == 0) break;
    iVar1 = iVar1 + -1;
    bVar8 = *pwVar5 == 0;
    pwVar5 = pwVar5 + 1;
  } while (!bVar8);
  if (!bVar8) {
    PVar12.entry = (PckEntryHeader *)0x0;
    PVar12.carry = true;
    return PVar12;
  }
  pPVar4 = g_PackageMountSlots;
  iVar2 = 0x400;
  while (fileHandle != pPVar4->fileHandle) {
    pPVar4 = pPVar4 + 1;
    iVar2 = iVar2 + -1;
    if (iVar2 == 0) {
      PVar11.entry = (PckEntryHeader *)0x0;
      PVar11.carry = true;
      return PVar11;
    }
  }
  pPVar6 = pPVar4->entryHeaders;
  PVar3 = pPVar4->entryCount;
  bVar8 = false;
  iVar2 = -(iVar1 + -0xf6);
  pwVar5 = path;
  pPVar7 = pPVar6;
  if (PVar3 == 0) {
    PVar10.entry = (PckEntryHeader *)0x0;
    PVar10.carry = true;
    return PVar10;
  }
code_r0x0040e6b4:
  do {
    if (iVar2 != 0) {
      bVar8 = *pwVar5 == pPVar6->path[0];
      iVar2 = iVar2 + -1;
      pwVar5 = pwVar5 + 1;
      pPVar6 = (PckEntryHeader *)(pPVar6->path + 1);
      if (bVar8) goto code_r0x0040e6b4;
    }
    if (bVar8) {
      PVar13.carry = false;
      PVar13.entry = pPVar7;
      return PVar13;
    }
    pPVar6 = pPVar7 + 1;
    PVar3 = PVar3 - 1;
    bVar8 = false;
    iVar2 = -(iVar1 + -0xf6);
    pwVar5 = path;
    pPVar7 = pPVar6;
    if (PVar3 == 0) {
      PVar9.entry = (PckEntryHeader *)0x0;
      PVar9.carry = true;
      return PVar9;
    }
  } while( true );
}


/* Address: 0x0040EA20.
   Ownership: assets/package/runtime.
   Purpose: Lowercases the caller's path in place and searches mounted archives from slot 0 upward, establishing
   first-mounted-wins precedence. CF clear returns the matching entry header.
*/
PackageFindEntryEaxEbxCf9 __thandor_eax_ebx_cf_preserve_ecx_edx
Package_FindEntryAcrossMounts(word *path)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  PckEntryCount in_EBX;
  PckEntryCount PVar5;
  PckMountSlot *pPVar6;
  word *pwVar7;
  PckEntryHeader *pPVar8;
  bool bVar10;
  PackageFindEntryEaxEbxCf9 PVar11;
  PackageFindEntryEaxEbxCf9 PVar12;
  PckEntryHeader *pPVar9;
  
  uVar1 = 0;
  iVar2 = 0xf6;
  pwVar7 = path;
  do {
    iVar3 = iVar2;
    uVar1 = CONCAT22((short)(uVar1 >> 0x10),*pwVar7);
    if ((0x40 < uVar1) && (uVar1 < 0x5b)) {
      uVar1 = uVar1 + 0x20;
    }
    *pwVar7 = (word)uVar1;
    iVar2 = iVar3 + -1;
    if (iVar2 == 0) goto LAB_0040ead6;
    pwVar7 = pwVar7 + 1;
  } while (uVar1 != 0);
  pPVar6 = g_PackageMountSlots;
  iVar4 = 0x400;
  iVar2 = -(iVar3 + -0xf7);
  do {
    pPVar8 = pPVar6->entryHeaders;
    in_EBX = pPVar6->entryCount;
    if ((pPVar8 != (PckEntryHeader *)0x0) && (in_EBX != 0)) {
      bVar10 = pPVar8 == (PckEntryHeader *)0x0;
      iVar3 = iVar2;
      pwVar7 = path;
      pPVar9 = pPVar8;
      PVar5 = in_EBX;
code_r0x0040eaa7:
      do {
        if (iVar3 != 0) {
          bVar10 = *pwVar7 == pPVar8->path[0];
          iVar3 = iVar3 + -1;
          pwVar7 = pwVar7 + 1;
          pPVar8 = (PckEntryHeader *)(pPVar8->path + 1);
          if (bVar10) goto code_r0x0040eaa7;
        }
        if (bVar10) {
          PVar11.ebx = pPVar6->fileHandle;
          PVar11.eax = (dword)pPVar9;
          PVar11.carry = false;
          return PVar11;
        }
        pPVar8 = pPVar9 + 1;
        PVar5 = PVar5 - 1;
        bVar10 = PVar5 == 0;
        in_EBX = 0;
        iVar3 = iVar2;
        pwVar7 = path;
        pPVar9 = pPVar8;
      } while (!bVar10);
    }
    pPVar6 = pPVar6 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
LAB_0040ead6:
  PVar12.ebx = in_EBX;
  PVar12.eax = uVar1;
  PVar12.carry = true;
  return PVar12;
}


/* Address: 0x0040E570.
   Ownership: assets/package/runtime.
   Purpose: Reads the 0x200-byte archive header, caches entryCount, reads every 0x200-byte entry header, and fills
   runtimePayloadOffset while skipping each packed payload.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_ReadDirectory(EngineFileHandle fileHandle)

{
  PckStoredByteCount *pPVar1;
  byte *pbVar2;
  uint uVar3;
  PckEntryCount PVar4;
  int iVar5;
  FileSystemFilePosition distance;
  PckMountSlot *pPVar6;
  PckEntryHeader *destination;
  StatusValueEaxCf5 SVar7;
  FileSystemSeekEaxCf5 FVar8;
  FileSystemReadEaxCf5 FVar9;
  StatusValueEaxCf5 SVar10;
  
  pPVar6 = g_PackageMountSlots;
  iVar5 = 0x400;
  do {
    if (fileHandle == pPVar6->fileHandle) {
      destination = pPVar6->entryHeaders;
      FVar8 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      pbVar2 = g_PackageScratchBuffer;
      uVar3 = FVar8.eax;
      if (FVar8.carry) goto LAB_0040e59f;
      FVar9 = (*g_FileSystemReadExactCf)(0x200,g_PackageScratchBuffer,(void *)fileHandle);
      uVar3 = FVar9.eax;
      if (FVar9.carry) goto LAB_0040e59f;
      PVar4 = *(PckEntryCount *)(pbVar2 + 0xb0);
      pPVar6->entryCount = PVar4;
      distance = 0x200;
      if (PVar4 != 0) goto LAB_0040e5f0;
      goto LAB_0040e62b;
    }
    pPVar6 = pPVar6 + 1;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  uVar3 = 0x14;
LAB_0040e59f:
  SVar7.carry = true;
  SVar7.valueOrError = uVar3;
  return SVar7;
LAB_0040e5f0:
  FVar9 = (*g_FileSystemReadExactCf)(0x200,destination,(void *)fileHandle);
  uVar3 = FVar9.eax;
  if (FVar9.carry) goto LAB_0040e59f;
  pPVar1 = &destination->packedSize;
  destination->runtimePayloadOffset = distance;
  destination = destination + 1;
  distance = distance + *pPVar1 + 0x200;
  FVar8 = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,distance,(void *)fileHandle);
  uVar3 = FVar8.eax;
  if (FVar8.carry) goto LAB_0040e59f;
  PVar4 = PVar4 - 1;
  if (PVar4 == 0) {
LAB_0040e62b:
    SVar10.carry = false;
    SVar10.valueOrError = uVar3;
    return SVar10;
  }
  goto LAB_0040e5f0;
}

