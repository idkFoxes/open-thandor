/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/uiatlas_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=uiatlas: the GPU UI texture cache (platform/sdl3/gpu_ui_textures.cpp) without a GPU device.
   Decodes every entry of the packages in the current directory (mount order patch01, patch00, level, daten,
   modelle, graphik, sound, engine; an entry path found again in a later package is skipped, as the mounts would
   hide it), keeps every valid 'gfx' asset and looks up each subresource with its own palette bank, so all of them
   are packed into atlas pages and converted (palette +0 ARGB or ARGB texels, 1 px border). Logs the page count,
   texel totals, the packing efficiency and a hash over all converted uploads, and writes one line per asset to
   uiatlas.txt (package, path, images, banks, texels, largest image). Then checks the cache rules:
     - a second frame of the same lookups converts nothing again (per-frame validation, content unchanged);
     - a changed texel of one image converts exactly that image again;
     - the release observer (g_GraphicsTextureSourceReleaseObserver through
       g_GraphicsTextureSourceLifecycleCallbacks3.releaseClone) evicts all images of an asset;
     - streaming regions of the frame get disjoint rectangles.
   Without the packages it logs "uiatlas: skipped" (game data is not in the repository). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <unordered_set>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/assets/record_bytes.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>

#ifdef THANDOR_RENDERER_SDL_GPU
#include "../sdl3/gpu_ui_textures.h"
#endif

#ifdef THANDOR_RENDERER_SDL_GPU

namespace {

struct LoadedGfx {
  std::string package;
  std::string path;
  GraphicsTextureSourceAsset *asset;
};

uint32_t UiAtlasTest_Alloc(uint32_t bytes, void **outPayload)
{
  void *payload = malloc(bytes);
  if (payload == nullptr) {
    return FATAL_ERROR_ARENA_EXHAUSTED;
  }
  *outPayload = payload;
  return 0;
}

uint32_t UiAtlasTest_Free(void *memory)
{
  free(memory);
  return 0;
}

/* Decodes all entries of one package file and keeps the valid 'gfx' assets whose path is new. */
void LoadPackageGfx(const char *packageName, uint8_t *packed, std::unordered_set<std::string> &seenPaths,
                    std::vector<LoadedGfx> &out, unsigned &entryCount)
{
  FILE *pck = fopen(packageName, "rb");
  if (pck == nullptr) {
    return;
  }
  long position = PCK_ENTRY_HEADER_BYTES;
  for (;;) {
    PckEntryHeader header;
    if ((fseek(pck, position, SEEK_SET) != 0) || (fread(&header, sizeof header, 1, pck) != 1)) {
      break;
    }
    if ((header.packedSize == 0) || (header.packedSize > PACKAGE_SCRATCH_BUFFER_BYTES) ||
        ((uint32_t)header.compressionMethod >= sizeof g_PckDecoderTable / sizeof g_PckDecoderTable[0])) {
      break;
    }
    position += PCK_ENTRY_HEADER_BYTES + (long)header.packedSize;
    entryCount++;
    std::string path;
    for (int k = 0; k < PCK_ENTRY_PATH_UNITS && header.path[k] != 0; k++) {
      const uint16_t unit = header.path[k];
      path += (char)((unit >= 'A' && unit <= 'Z') ? unit + ('a' - 'A') : (unit < 0x80 ? unit : '?'));
    }
    if (!seenPaths.insert(path).second || (header.unpackedSize < GFX_ASSET_HEADER_SIZE) ||
        (g_PckDecoderTable[header.compressionMethod] == nullptr)) {
      continue;
    }
    if (fread(packed, 1, header.packedSize, pck) != header.packedSize) {
      break;
    }
    auto *unpacked = static_cast<uint8_t *>(malloc(header.unpackedSize + 4));
    if (unpacked == nullptr) {
      break;
    }
    if (!g_PckDecoderTable[header.compressionMethod](header.unpackedSize, unpacked, header.packedSize, packed,
                                                       nullptr, nullptr)) {
      free(unpacked);
      continue;
    }
    auto *asset = reinterpret_cast<GraphicsTextureSourceAsset *>(unpacked);
    if ((asset->common.magic != ASSET_MAGIC_GFX) || (asset->common.allocationSizeBytes > header.unpackedSize) ||
        !GraphicsTextureSource_ValidateAsset(asset)) {
      free(unpacked);
      continue;
    }
    out.push_back(LoadedGfx{packageName, path, asset});
  }
  fclose(pck);
}

} // namespace

void Thandor_SelfTestUiAtlas()
{
  static const char *packages[] = {"PATCH01.PCK", "PATCH00.PCK", "LEVEL.PCK", "DATEN.PCK",
                                   "MODELLE.PCK", "GRAPHIK.PCK", "SOUND.PCK", "ENGINE.PCK"};
  uint32_t (*savedAlloc)(uint32_t, void **) = g_MemoryApi.alloc;
  uint32_t (*savedFree)(void *) = g_MemoryApi.free;
  g_MemoryApi.alloc = UiAtlasTest_Alloc;
  g_MemoryApi.free = UiAtlasTest_Free;
  auto *packed = static_cast<uint8_t *>(malloc(PACKAGE_SCRATCH_BUFFER_BYTES));
  std::unordered_set<std::string> seenPaths;
  std::vector<LoadedGfx> assets;
  unsigned entryCount = 0;
  unsigned packageCount = 0;
  for (const char *package : packages) {
    FILE *probe = fopen(package, "rb");
    if (probe == nullptr) {
      continue;
    }
    fclose(probe);
    packageCount++;
    if (packed != nullptr) {
      LoadPackageGfx(package, packed, seenPaths, assets, entryCount);
    }
  }
  free(packed);
  g_MemoryApi.alloc = savedAlloc;
  g_MemoryApi.free = savedFree;
  if (assets.empty()) {
    Thandor_Log("uiatlas: skipped (%u packages, no gfx assets in the current directory)", packageCount);
    return;
  }

  unsigned failures = 0;
  unsigned imageCount = 0;
  unsigned emptyImages = 0;
  unsigned largestW = 0;
  unsigned largestH = 0;
  GpuUiTextures_Init(nullptr);
  FILE *list = fopen("uiatlas.txt", "w");
  /* frame 1: every image of every asset */
  for (const LoadedGfx &loaded : assets) {
    const GraphicsTextureSourceAsset *asset = loaded.asset;
    const auto *entries =
        Asset_RecordAt<const GraphicsTextureSourceEntry>(asset, asset->tableDescriptor.subresourceTableOffset);
    uint64_t texels = 0;
    unsigned maxW = 0;
    unsigned maxH = 0;
    for (uint32_t index = 0; index < asset->tableDescriptor.subresourceCount; index++) {
      GpuUiTexRegion region;
      const uint32_t w = entries[index].pixelWidth;
      const uint32_t h = entries[index].pixelHeight;
      imageCount++;
      if ((w == 0) || (h == 0)) {
        emptyImages++;
        continue;
      }
      if (!GpuUiTextures_Lookup(asset, index, GPU_UI_TEX_ENTRY_PALETTE, &region) || (region.w != (int)w) ||
          (region.h != (int)h) || !(region.u0 < region.u1) || !(region.v0 < region.v1)) {
        if (failures++ < 8) {
          Thandor_Log("uiatlas: FAILED lookup %s %s image %u (%ux%u)", loaded.package.c_str(), loaded.path.c_str(),
                      index, w, h);
        }
        continue;
      }
      texels += (uint64_t)w * h;
      maxW = w > maxW ? w : maxW;
      maxH = h > maxH ? h : maxH;
      if ((uint64_t)w * h > (uint64_t)largestW * largestH) {
        largestW = w;
        largestH = h;
      }
    }
    if (list != nullptr) {
      fprintf(list, "%s %s images %u banks %u texels %llu largest %ux%u\n", loaded.package.c_str(),
              loaded.path.c_str(), asset->tableDescriptor.subresourceCount, asset->tableDescriptor.paletteBankCount,
              (unsigned long long)texels, maxW, maxH);
    }
  }
  if (list != nullptr) {
    fclose(list);
  }
  GpuUiTextures_FlushUploads(nullptr);
  GpuUiTexStats first;
  GpuUiTextures_GetStats(&first);
  Thandor_Log("uiatlas: %u packages, %u entries, %u gfx assets, %u images (%u empty), largest %ux%u",
              packageCount, entryCount, (unsigned)assets.size(), imageCount, emptyImages, largestW, largestH);
  Thandor_Log("uiatlas: %u pages of %u (%u oversized), %u cached images, %llu texels, %llu allocated, "
              "fill %.1f%% of the page area, %u uploads, hash %08X",
              first.pages, GPU_UI_PAGE_SIZE, first.oversizePages, first.entries, (unsigned long long)first.texels,
              (unsigned long long)first.usedArea,
              first.pageArea != 0 ? 100.0 * (double)first.usedArea / (double)first.pageArea : 0.0, first.uploads,
              first.contentHash);

  /* frame 2: the same lookups validate and convert nothing */
  for (const LoadedGfx &loaded : assets) {
    for (uint32_t index = 0; index < loaded.asset->tableDescriptor.subresourceCount; index++) {
      GpuUiTexRegion region;
      GpuUiTextures_Lookup(loaded.asset, index, GPU_UI_TEX_ENTRY_PALETTE, &region);
    }
  }
  GpuUiTexStats second;
  GpuUiTextures_GetStats(&second);
  if (second.uploads != first.uploads) {
    failures++;
    Thandor_Log("uiatlas: FAILED unchanged frame converted %u images", second.uploads - first.uploads);
  }
  GpuUiTextures_FlushUploads(nullptr);

  /* frame 3: one changed texel converts exactly that image again, at the same place */
  {
    GraphicsTextureSourceAsset *asset = assets.front().asset;
    const auto *entry =
        Asset_RecordAt<const GraphicsTextureSourceEntry>(asset, asset->tableDescriptor.subresourceTableOffset);
    uint32_t index = 0;
    while (index < asset->tableDescriptor.subresourceCount && (entry[index].pixelWidth == 0 || entry[index].pixelHeight == 0)) {
      index++;
    }
    if (index < asset->tableDescriptor.subresourceCount) {
      GpuUiTexRegion before;
      GpuUiTexRegion after;
      GpuUiTextures_Lookup(asset, index, GPU_UI_TEX_ENTRY_PALETTE, &before); /* validates, nothing to convert */
      GpuUiTextures_FlushUploads(nullptr);
      uint8_t *texel = Asset_RecordAt(asset, entry[index].dataOffset);
      *texel ^= 0x01;
      GpuUiTexStats before3;
      GpuUiTextures_GetStats(&before3);
      GpuUiTextures_Lookup(asset, index, GPU_UI_TEX_ENTRY_PALETTE, &after);
      GpuUiTextures_Lookup(asset, index, GPU_UI_TEX_ENTRY_PALETTE, &after);
      GpuUiTexStats after3;
      GpuUiTextures_GetStats(&after3);
      *texel ^= 0x01;
      if ((after3.uploads - before3.uploads != 1) || (after3.reconversions - before3.reconversions != 1) ||
          (after.u0 != before.u0) || (after.v0 != before.v0)) {
        failures++;
        Thandor_Log("uiatlas: FAILED changed texel: %u uploads, %u reconversions", after3.uploads - before3.uploads,
                    after3.reconversions - before3.reconversions);
      }
      GpuUiTextures_FlushUploads(nullptr);
    }
  }

  /* streaming regions: disjoint rectangles in the frame */
  {
    static uint32_t pixels[300 * 200];
    for (uint32_t i = 0; i < 300 * 200; i++) {
      pixels[i] = 0xFF000000u | (i * 2654435761u >> 8);
    }
    GpuUiTexRegion a = GpuUiTextures_UploadRegion(pixels, 300, 200, 300 * 4);
    GpuUiTexRegion b = GpuUiTextures_UploadRegion(pixels, 150, 100, 300 * 4);
    GpuUiTexRegion big = GpuUiTextures_UploadRegion(pixels, 3000, 20, 0); /* pitch too small: fails */
    const bool overlap = (a.u0 < b.u1) && (b.u0 < a.u1) && (a.v0 < b.v1) && (b.v0 < a.v1);
    if ((a.w != 300) || (b.h != 100) || overlap || (big.w != 0)) {
      failures++;
      Thandor_Log("uiatlas: FAILED streaming regions (%d x %d, %d x %d, overlap %d)", a.w, a.h, b.w, b.h,
                  overlap ? 1 : 0);
    }
    GpuUiTextures_FlushUploads(nullptr);
  }

  /* release observer: freeing every asset through the clone release callback evicts all entries */
  GpuUiTexStats beforeRelease;
  GpuUiTextures_GetStats(&beforeRelease);
  for (LoadedGfx &loaded : assets) {
    g_MemoryApi.alloc = UiAtlasTest_Alloc;
    g_MemoryApi.free = UiAtlasTest_Free;
    g_GraphicsTextureSourceLifecycleCallbacks3.releaseClone(loaded.asset);
    g_MemoryApi.alloc = savedAlloc;
    g_MemoryApi.free = savedFree;
    loaded.asset = nullptr;
  }
  GpuUiTexStats released;
  GpuUiTextures_GetStats(&released);
  GpuUiTextures_FlushUploads(nullptr);
  if ((released.entries != 0) || (released.evictions - beforeRelease.evictions != beforeRelease.entries)) {
    failures++;
    Thandor_Log("uiatlas: FAILED release: %u entries left, %u evicted", released.entries,
                released.evictions - beforeRelease.evictions);
  }
  GpuUiTextures_Shutdown();
  if (g_GraphicsTextureSourceReleaseObserver != nullptr) {
    failures++;
    Thandor_Log("uiatlas: FAILED release observer still installed after shutdown");
  }
  Thandor_Log("uiatlas: %s, hash %08X", failures == 0 ? "ok" : "FAILED", first.contentHash);
}

#else

void Thandor_SelfTestUiAtlas()
{
  Thandor_Log("uiatlas: skipped (built without the SDL_GPU renderer)");
}

#endif
