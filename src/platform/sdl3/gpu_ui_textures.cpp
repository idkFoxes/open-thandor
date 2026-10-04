/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/gpu_ui_textures.cpp
 * Project code (not in the original game)
 */

/* Step 9 work package 2: the GPU UI texture cache and its atlas pages (see gpu_ui_textures.h). Packing: each page
   has shelves (rows of rectangles of similar height, filled left to right) and a list of rectangles freed by
   evictions; a request takes the best fitting free rectangle, then the best fitting shelf, then a new shelf at
   the bottom, then a new page. The entry layout is the one Blit_SetupSubresource
   (graphics/backend/software_blit_helpers.h) reads: the subresource record at asset + subresourceTableOffset +
   index * 0x20, paletteIndex -1 = ARGB8888 texels, else 8-bit indices into the 256-entry bank at asset + 0x200 +
   bank * 0x800 (8 bytes per entry, +0 ARGB). */

#include "gpu_ui_textures.h"

#include <SDL3/SDL_stdinc.h>

#include <algorithm>
#include <cstring>
#include <map>
#include <tuple>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

namespace {

/* D3D12 copies between buffers and textures need 512-byte aligned offsets and 256-byte aligned row pitches;
   otherwise SDL_GPU copies every upload through a temporary buffer (as in gpu_renderer.cpp). */
constexpr size_t kPlacementPixels = 512 / 4;
constexpr uint32_t kPitchPixels = 256 / 4;
constexpr uint32_t kPadding = 1;               /* border texels around every image */
constexpr uint32_t kStreamIdleFrames = 300;    /* streaming pages unused this long are released (but the first) */

struct Rect {
  uint32_t x, y, w, h;
};

struct Shelf {
  uint32_t y, height, nextX;
};

struct Page {
  SDL_GPUTexture *texture = nullptr;
  uint32_t width = 0;
  uint32_t height = 0;
  std::vector<Shelf> shelves;
  uint32_t bottom = 0;           /* first row below the last shelf */
  std::vector<Rect> freeRects;   /* freed by evictions and moves, reusable */
  uint64_t usedArea = 0;
  uint32_t lastUsedFrame = 0;    /* streaming pages */
  bool cycledThisFrame = false;  /* streaming pages: the first upload of a frame cycles the texture */
};

struct Key {
  const void *asset;
  uint32_t subresource;
  uint32_t paletteBank;
  bool operator<(const Key &other) const noexcept
  {
    return std::tie(asset, subresource, paletteBank) < std::tie(other.asset, other.subresource, other.paletteBank);
  }
};

struct Entry {
  uint32_t page;
  Rect rect;          /* allocated rectangle (with padding; may be larger than needed when it was a free one) */
  uint32_t width;     /* image size */
  uint32_t height;
  uint32_t hash;      /* texels + palette bank */
  uint32_t validatedFrame;
};

struct PendingUpload {
  SDL_GPUTexture *dedicated; /* a dedicated texture (GpuUiTextures_DedicatedImage), else nullptr */
  bool stream;
  uint32_t page;
  uint32_t x, y, width, height; /* destination, padding included */
  size_t stagingOffset;         /* pixels */
  uint32_t rowPixels;
};

struct PendingFree {
  uint32_t page;
  Rect rect;
};

/* A dedicated texture (GpuUiTextures_DedicatedImage). */
struct Dedicated {
  const void *key;
  uint32_t generation;
  uint32_t width;  /* image size (the texture has the border around it) */
  uint32_t height;
  SDL_GPUTexture *texture;
  uint32_t lastUsedFrame;
};

struct State {
  bool active = false;
  SDL_GPUDevice *device = nullptr;
  std::vector<Page> pages;
  std::vector<Page> streamPages;
  std::vector<Dedicated> dedicated;
  std::map<Key, Entry> entries;
  std::vector<uint32_t> staging;
  std::vector<PendingUpload> uploads;
  std::vector<PendingFree> pendingFrees;
  SDL_GPUTransferBuffer *transfer = nullptr;
  uint32_t transferBytes = 0;
  uint32_t frame = 1;
  bool hardLimitLogged = false;
  GpuUiTexStats stats{};
};

State s_ui;

/* --- hashing ------------------------------------------------------------------------------------------------ */

inline uint32_t RotateLeft(uint32_t value, int count) noexcept
{
  return (value << count) | (value >> (32 - count));
}

/* xxHash32-style hash of byteCount bytes (any count), the scheme of gpu_renderer.cpp's HashBytes. */
uint32_t HashBytes(const uint8_t *bytes, size_t byteCount, uint32_t seed) noexcept
{
  constexpr uint32_t kPrime1 = 0x9E3779B1u;
  constexpr uint32_t kPrime2 = 0x85EBCA77u;
  constexpr uint32_t kPrime3 = 0xC2B2AE3Du;
  constexpr uint32_t kPrime5 = 0x165667B1u;
  uint32_t lane0 = seed + kPrime1 + kPrime2;
  uint32_t lane1 = seed + kPrime2;
  uint32_t lane2 = seed;
  uint32_t lane3 = seed - kPrime1;
  size_t offset = 0;
  for (; offset + 16 <= byteCount; offset += 16) {
    uint32_t words[4];
    std::memcpy(words, bytes + offset, sizeof words);
    lane0 = RotateLeft(lane0 + words[0] * kPrime2, 13) * kPrime1;
    lane1 = RotateLeft(lane1 + words[1] * kPrime2, 13) * kPrime1;
    lane2 = RotateLeft(lane2 + words[2] * kPrime2, 13) * kPrime1;
    lane3 = RotateLeft(lane3 + words[3] * kPrime2, 13) * kPrime1;
  }
  uint32_t hash = RotateLeft(lane0, 1) + RotateLeft(lane1, 7) + RotateLeft(lane2, 12) + RotateLeft(lane3, 18);
  for (; offset + 4 <= byteCount; offset += 4) {
    uint32_t word;
    std::memcpy(&word, bytes + offset, sizeof word);
    hash = RotateLeft(hash + word * kPrime3, 17) * 0x27D4EB2Fu;
  }
  for (; offset < byteCount; offset++) {
    hash = RotateLeft(hash + bytes[offset] * kPrime5, 11) * kPrime1;
  }
  hash ^= static_cast<uint32_t>(byteCount);
  hash ^= hash >> 15;
  hash *= kPrime2;
  hash ^= hash >> 13;
  hash *= kPrime3;
  hash ^= hash >> 16;
  return hash;
}

/* --- pages and packing ------------------------------------------------------------------------------------- */

SDL_GPUTexture *CreatePageTexture(uint32_t width, uint32_t height) noexcept
{
  if (s_ui.device == nullptr) {
    return nullptr;
  }
  SDL_GPUTextureCreateInfo info;
  SDL_zero(info);
  info.type = SDL_GPU_TEXTURETYPE_2D;
  info.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
  info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
  info.width = width;
  info.height = height;
  info.layer_count_or_depth = 1;
  info.num_levels = 1;
  info.sample_count = SDL_GPU_SAMPLECOUNT_1;
  SDL_GPUTexture *texture = SDL_CreateGPUTexture(s_ui.device, &info);
  if (texture == nullptr) {
    Thandor_Log("GPU UI textures: page %ux%u failed: %s", width, height, SDL_GetError());
  }
  return texture;
}

/* A page of at least GPU_UI_PAGE_SIZE squared that holds width x height. False when the texture fails. */
bool AddPage(std::vector<Page> &pages, uint32_t width, uint32_t height) noexcept
{
  Page page;
  page.width = std::max(GPU_UI_PAGE_SIZE, (width + 255) / 256 * 256);
  page.height = std::max(GPU_UI_PAGE_SIZE, (height + 255) / 256 * 256);
  if (s_ui.device != nullptr) {
    page.texture = CreatePageTexture(page.width, page.height);
    if (page.texture == nullptr) {
      return false;
    }
  }
  pages.push_back(std::move(page));
  return true;
}

void ClearPacking(Page &page) noexcept
{
  page.shelves.clear();
  page.bottom = 0;
  page.freeRects.clear();
  page.usedArea = 0;
}

/* Shelves accept rectangles up to this much lower than the shelf (less waste than one shelf per height). */
uint32_t ShelfSlack(uint32_t height) noexcept
{
  return std::max<uint32_t>(4, height / 4);
}

/* A width x height rectangle in page (the free rectangle or shelf that wastes least), false when it is full. */
bool AllocateInPage(Page &page, uint32_t width, uint32_t height, Rect &out) noexcept
{
  /* a rectangle freed by an eviction: used whole, so it can be freed whole again */
  size_t bestFree = page.freeRects.size();
  uint64_t bestFreeWaste = UINT64_MAX;
  for (size_t index = 0; index < page.freeRects.size(); index++) {
    const Rect &candidate = page.freeRects[index];
    if ((candidate.w >= width) && (candidate.h >= height)) {
      const uint64_t waste = static_cast<uint64_t>(candidate.w) * candidate.h - static_cast<uint64_t>(width) * height;
      if (waste < bestFreeWaste) {
        bestFreeWaste = waste;
        bestFree = index;
      }
    }
  }
  /* reuse a free rectangle only when it does not waste more than the image itself */
  if ((bestFree < page.freeRects.size()) && (bestFreeWaste <= static_cast<uint64_t>(width) * height)) {
    out = page.freeRects[bestFree];
    page.freeRects.erase(page.freeRects.begin() + static_cast<ptrdiff_t>(bestFree));
    page.usedArea += static_cast<uint64_t>(out.w) * out.h;
    return true;
  }
  Shelf *bestShelf = nullptr;
  for (Shelf &shelf : page.shelves) {
    if ((shelf.height >= height) && (shelf.height - height <= ShelfSlack(height)) &&
        (shelf.nextX + width <= page.width) && ((bestShelf == nullptr) || (shelf.height < bestShelf->height))) {
      bestShelf = &shelf;
    }
  }
  if (bestShelf != nullptr) {
    out = Rect{bestShelf->nextX, bestShelf->y, width, bestShelf->height};
    bestShelf->nextX += width;
    page.usedArea += static_cast<uint64_t>(out.w) * out.h;
    return true;
  }
  if ((width > page.width) || (page.bottom + height > page.height)) {
    return false;
  }
  page.shelves.push_back(Shelf{page.bottom, height, width});
  out = Rect{0, page.bottom, width, height};
  page.bottom += height;
  page.usedArea += static_cast<uint64_t>(out.w) * out.h;
  return true;
}

/* A rectangle for an image (padding included) in a cache page, adding a page when none has room. */
bool AllocateCacheRect(uint32_t width, uint32_t height, uint32_t &outPage, Rect &out) noexcept
{
  for (uint32_t index = 0; index < s_ui.pages.size(); index++) {
    if (AllocateInPage(s_ui.pages[index], width, height, out)) {
      outPage = index;
      return true;
    }
  }
  if ((s_ui.device != nullptr) && (s_ui.pages.size() >= GPU_UI_HARD_PAGE_LIMIT)) {
    if (!s_ui.hardLimitLogged) {
      Thandor_Log("GPU UI textures: %u pages full, image %ux%u not cached", GPU_UI_HARD_PAGE_LIMIT, width, height);
      s_ui.hardLimitLogged = true;
    }
    return false;
  }
  if (!AddPage(s_ui.pages, width, height)) {
    return false;
  }
  outPage = static_cast<uint32_t>(s_ui.pages.size() - 1);
  return AllocateInPage(s_ui.pages.back(), width, height, out);
}

/* The rectangle goes back to its page after the next flush (a draw of this frame may still sample it). */
void FreeCacheRect(uint32_t page, const Rect &rect) noexcept
{
  s_ui.pendingFrees.push_back(PendingFree{page, rect});
}

/* --- conversion --------------------------------------------------------------------------------------------- */

/* Staging room for a (width + 2) x (height + 2) upload, returns its first row; rowPixels gets the pitch. */
uint32_t *StageUpload(bool stream, uint32_t page, uint32_t x, uint32_t y, uint32_t width, uint32_t height) noexcept
{
  const uint32_t paddedWidth = width + 2 * kPadding;
  const uint32_t paddedHeight = height + 2 * kPadding;
  const uint32_t rowPixels = (paddedWidth + kPitchPixels - 1) / kPitchPixels * kPitchPixels;
  const size_t offset = (s_ui.staging.size() + kPlacementPixels - 1) / kPlacementPixels * kPlacementPixels;
  s_ui.staging.resize(offset + static_cast<size_t>(rowPixels) * paddedHeight);
  s_ui.uploads.push_back(PendingUpload{nullptr, stream, page, x, y, paddedWidth, paddedHeight, offset, rowPixels});
  s_ui.stats.uploads++;
  s_ui.stats.uploadedTexels += static_cast<uint64_t>(paddedWidth) * paddedHeight;
  return s_ui.staging.data() + offset;
}

/* Fills the border of a staged upload whose inner rows 1..height are written: edge texels repeated. */
void FillPadding(uint32_t *staged, uint32_t width, uint32_t height, uint32_t rowPixels) noexcept
{
  for (uint32_t row = 1; row <= height; row++) {
    uint32_t *line = staged + static_cast<size_t>(row) * rowPixels;
    line[0] = line[1];
    line[width + 1] = line[width];
  }
  std::memcpy(staged, staged + rowPixels, (width + 2) * sizeof(uint32_t));
  std::memcpy(staged + static_cast<size_t>(height + 1) * rowPixels, staged + static_cast<size_t>(height) * rowPixels,
              (width + 2) * sizeof(uint32_t));
}

/* Converts one image into the staging buffer (palette +0 ARGB or the ARGB texels, both 0xAARRGGBB = B8G8R8A8
   bytes) and queues its upload to rect of the cache page. palette is NULL for direct colour. */
void QueueConversion(uint32_t page, const Rect &rect, const uint8_t *texels, const uint8_t *palette, uint32_t width,
                     uint32_t height) noexcept
{
  uint32_t *staged = StageUpload(false, page, rect.x, rect.y, width, height);
  const uint32_t rowPixels = s_ui.uploads.back().rowPixels;
  if (palette != nullptr) {
    uint32_t lut[256];
    for (int index = 0; index < 256; index++) {
      std::memcpy(&lut[index], palette + index * 8, sizeof(uint32_t)); /* +0: ARGB, straight alpha */
    }
    for (uint32_t row = 0; row < height; row++) {
      const uint8_t *source = texels + static_cast<size_t>(row) * width;
      uint32_t *destination = staged + static_cast<size_t>(row + 1) * rowPixels + 1;
      for (uint32_t column = 0; column < width; column++) {
        destination[column] = lut[source[column]];
      }
    }
  }
  else {
    for (uint32_t row = 0; row < height; row++) {
      std::memcpy(staged + static_cast<size_t>(row + 1) * rowPixels + 1, texels + static_cast<size_t>(row) * width * 4,
                  static_cast<size_t>(width) * 4);
    }
  }
  FillPadding(staged, width, height, rowPixels);
}

GpuUiTexRegion RegionOf(const Page &page, const Rect &rect, uint32_t width, uint32_t height) noexcept
{
  GpuUiTexRegion region;
  region.page = page.texture;
  region.u0 = static_cast<float>(rect.x + kPadding) / static_cast<float>(page.width);
  region.v0 = static_cast<float>(rect.y + kPadding) / static_cast<float>(page.height);
  region.u1 = static_cast<float>(rect.x + kPadding + width) / static_cast<float>(page.width);
  region.v1 = static_cast<float>(rect.y + kPadding + height) / static_cast<float>(page.height);
  region.w = static_cast<int>(width);
  region.h = static_cast<int>(height);
  return region;
}

/* --- frame end ---------------------------------------------------------------------------------------------- */

/* Drops every entry and the packing of all cache pages (pages beyond the soft limit are released). Only called
   right after a flush, so no staged upload or region of the current frame refers to them any more. */
void ResetCache() noexcept
{
  s_ui.entries.clear();
  s_ui.pendingFrees.clear();
  while (s_ui.pages.size() > GPU_UI_SOFT_PAGE_LIMIT) {
    if (s_ui.device != nullptr) {
      SDL_ReleaseGPUTexture(s_ui.device, s_ui.pages.back().texture);
    }
    s_ui.pages.pop_back();
  }
  for (Page &page : s_ui.pages) {
    ClearPacking(page);
  }
  s_ui.stats.resets++;
  Thandor_Log("GPU UI textures: more than %u pages in use, starting over", GPU_UI_SOFT_PAGE_LIMIT);
}

void EndFrame() noexcept
{
  s_ui.staging.clear();
  s_ui.uploads.clear();
  for (const PendingFree &pending : s_ui.pendingFrees) {
    if (pending.page < s_ui.pages.size()) {
      Page &page = s_ui.pages[pending.page];
      page.freeRects.push_back(pending.rect);
      page.usedArea -= std::min<uint64_t>(page.usedArea, static_cast<uint64_t>(pending.rect.w) * pending.rect.h);
    }
  }
  s_ui.pendingFrees.clear();
  /* streaming pages: refilled every frame; long unused ones (but the first) are released */
  for (size_t index = s_ui.streamPages.size(); index-- > 1;) {
    if (s_ui.frame - s_ui.streamPages[index].lastUsedFrame > kStreamIdleFrames) {
      if (s_ui.device != nullptr) {
        SDL_ReleaseGPUTexture(s_ui.device, s_ui.streamPages[index].texture);
      }
      s_ui.streamPages.erase(s_ui.streamPages.begin() + static_cast<ptrdiff_t>(index));
    }
  }
  for (Page &page : s_ui.streamPages) {
    ClearPacking(page);
    page.cycledThisFrame = false;
  }
  for (size_t index = s_ui.dedicated.size(); index-- > 0;) {
    if (s_ui.frame - s_ui.dedicated[index].lastUsedFrame > kStreamIdleFrames) {
      if (s_ui.device != nullptr) {
        SDL_ReleaseGPUTexture(s_ui.device, s_ui.dedicated[index].texture);
      }
      s_ui.dedicated.erase(s_ui.dedicated.begin() + static_cast<ptrdiff_t>(index));
    }
  }
  if ((s_ui.device != nullptr) && (s_ui.pages.size() > GPU_UI_SOFT_PAGE_LIMIT)) {
    ResetCache();
  }
  s_ui.frame++;
}

/* Without a device: folds every staged upload (page, position, texels without the pitch padding) into the
   statistics hash. */
void HashStagedUploads() noexcept
{
  for (const PendingUpload &upload : s_ui.uploads) {
    const uint32_t kind = (upload.dedicated != nullptr) ? 2u : (upload.stream ? 1u : 0u);
    const uint32_t header[6] = {kind, upload.page, upload.x, upload.y, upload.width, upload.height};
    uint32_t hash = HashBytes(reinterpret_cast<const uint8_t *>(header), sizeof header, s_ui.stats.contentHash);
    for (uint32_t row = 0; row < upload.height; row++) {
      hash = HashBytes(reinterpret_cast<const uint8_t *>(s_ui.staging.data() + upload.stagingOffset +
                                                          static_cast<size_t>(row) * upload.rowPixels),
                       static_cast<size_t>(upload.width) * 4, hash);
    }
    s_ui.stats.contentHash = hash;
  }
}

bool EnsureTransferBuffer(uint32_t byteCount) noexcept
{
  if ((s_ui.transfer != nullptr) && (s_ui.transferBytes >= byteCount)) {
    return true;
  }
  SDL_ReleaseGPUTransferBuffer(s_ui.device, s_ui.transfer);
  uint32_t size = std::max<uint32_t>(s_ui.transferBytes * 2, 4u << 20);
  while (size < byteCount) {
    size *= 2;
  }
  SDL_GPUTransferBufferCreateInfo info;
  SDL_zero(info);
  info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  info.size = size;
  s_ui.transfer = SDL_CreateGPUTransferBuffer(s_ui.device, &info);
  s_ui.transferBytes = (s_ui.transfer != nullptr) ? size : 0;
  return s_ui.transfer != nullptr;
}

void ReleaseObserver(const GraphicsTextureSourceAsset *asset)
{
  GpuUiTextures_Evict(asset);
}

} // namespace

bool GpuUiTextures_Init(SDL_GPUDevice *device)
{
  GpuUiTextures_Shutdown();
  s_ui.device = device;
  s_ui.active = true;
  s_ui.frame = 1;
  s_ui.stats = GpuUiTexStats{};
  g_GraphicsTextureSourceReleaseObserver = &ReleaseObserver;
  return true;
}

void GpuUiTextures_Shutdown(SDL_GPUDevice *device)
{
  (void)device;
  if (!s_ui.active) {
    return;
  }
  if (g_GraphicsTextureSourceReleaseObserver == &ReleaseObserver) {
    g_GraphicsTextureSourceReleaseObserver = nullptr;
  }
  if (s_ui.device != nullptr) {
    for (Page &page : s_ui.pages) {
      SDL_ReleaseGPUTexture(s_ui.device, page.texture);
    }
    for (Page &page : s_ui.streamPages) {
      SDL_ReleaseGPUTexture(s_ui.device, page.texture);
    }
    for (Dedicated &dedicated : s_ui.dedicated) {
      SDL_ReleaseGPUTexture(s_ui.device, dedicated.texture);
    }
    SDL_ReleaseGPUTransferBuffer(s_ui.device, s_ui.transfer);
  }
  s_ui.pages.clear();
  s_ui.streamPages.clear();
  s_ui.dedicated.clear();
  s_ui.entries.clear();
  s_ui.staging.clear();
  s_ui.staging.shrink_to_fit();
  s_ui.uploads.clear();
  s_ui.pendingFrees.clear();
  s_ui.transfer = nullptr;
  s_ui.transferBytes = 0;
  s_ui.device = nullptr;
  s_ui.hardLimitLogged = false;
  s_ui.active = false;
}

bool GpuUiTextures_Lookup(const GraphicsTextureSourceAsset *asset, uint32_t subresource, uint32_t paletteBank,
                          GpuUiTexRegion *out)
{
  if (!s_ui.active || (asset == nullptr) || (out == nullptr) || (asset->common.magic != ASSET_MAGIC_GFX) ||
      (subresource >= asset->tableDescriptor.subresourceCount)) {
    return false;
  }
  const uint8_t *bytes = reinterpret_cast<const uint8_t *>(asset);
  const auto *entry = reinterpret_cast<const GraphicsTextureSourceEntry *>(
                          bytes + asset->tableDescriptor.subresourceTableOffset) + subresource;
  const uint32_t width = entry->pixelWidth;
  const uint32_t height = entry->pixelHeight;
  if ((width == 0) || (height == 0) || (width > 16384) || (height > 16384)) {
    return false;
  }
  /* The bank of the key is the bank actually used: the draw list (draw2d.h) records a paletted subresource with
     its entry's bank number and a direct-colour one with DRAW2D_PALETTE_BANK_DIRECT (= GPU_UI_TEX_ENTRY_PALETTE),
     while other callers (the cursor) pass GPU_UI_TEX_ENTRY_PALETTE for both; resolving it here keeps one cache
     entry per image instead of one per spelling. Direct-colour subresources ignore the bank. */
  const uint8_t *palette = nullptr;
  uint32_t keyBank = GPU_UI_TEX_ENTRY_PALETTE;
  if (entry->paletteIndex != -1) {
    const uint32_t bank = (paletteBank == GPU_UI_TEX_ENTRY_PALETTE) ? static_cast<uint32_t>(entry->paletteIndex)
                                                                    : paletteBank;
    if (bank >= asset->tableDescriptor.paletteBankCount) {
      return false;
    }
    palette = bytes + GFX_ASSET_HEADER_SIZE + bank * GFX_PALETTE_BANK_SIZE;
    keyBank = bank;
  }
  const uint8_t *texels = bytes + entry->dataOffset;
  const Key key{asset, subresource, keyBank};

  auto found = s_ui.entries.find(key);
  if ((found != s_ui.entries.end()) && (found->second.validatedFrame == s_ui.frame)) {
    *out = RegionOf(s_ui.pages[found->second.page], found->second.rect, width, height);
    return true;
  }
  const size_t texelBytes = static_cast<size_t>(width) * height * ((palette != nullptr) ? 1 : 4);
  uint32_t hash = HashBytes(texels, texelBytes, width * 0x10001u ^ height);
  if (palette != nullptr) {
    hash = HashBytes(palette, GFX_PALETTE_BANK_SIZE, hash);
  }
  if (found != s_ui.entries.end()) {
    Entry &cached = found->second;
    if ((cached.width == width) && (cached.height == height)) {
      cached.validatedFrame = s_ui.frame;
      if (cached.hash != hash) {
        cached.hash = hash;
        QueueConversion(cached.page, cached.rect, texels, palette, width, height);
        s_ui.stats.reconversions++;
      }
      *out = RegionOf(s_ui.pages[cached.page], cached.rect, width, height);
      return true;
    }
    /* same key, other image size (a reused pointer or a rebuilt asset): move it */
    FreeCacheRect(cached.page, cached.rect);
    s_ui.entries.erase(found);
    s_ui.stats.moves++;
  }
  uint32_t page = 0;
  Rect rect{};
  if (!AllocateCacheRect(width + 2 * kPadding, height + 2 * kPadding, page, rect)) {
    return false;
  }
  s_ui.entries.emplace(key, Entry{page, rect, width, height, hash, s_ui.frame});
  QueueConversion(page, rect, texels, palette, width, height);
  *out = RegionOf(s_ui.pages[page], rect, width, height);
  return true;
}

void GpuUiTextures_FlushUploads(SDL_GPUCommandBuffer *commands)
{
  if (!s_ui.active) {
    return;
  }
  if (s_ui.device == nullptr) {
    HashStagedUploads();
    EndFrame();
    return;
  }
  if (commands == nullptr) {
    return;
  }
  if (!s_ui.uploads.empty()) {
    const uint32_t byteCount = static_cast<uint32_t>(s_ui.staging.size() * sizeof(uint32_t));
    void *mapped = nullptr;
    if (EnsureTransferBuffer(byteCount)) {
      mapped = SDL_MapGPUTransferBuffer(s_ui.device, s_ui.transfer, true);
    }
    if (mapped == nullptr) {
      /* the pixels of this frame are lost: make every entry convert again on its next use */
      Thandor_Log("GPU UI textures: upload of %u bytes failed: %s", byteCount, SDL_GetError());
      for (auto &cached : s_ui.entries) {
        cached.second.hash ^= 0xFFFFFFFFu;
      }
      for (Dedicated &dedicated : s_ui.dedicated) {
        dedicated.generation ^= 0x80000000u; /* upload again on the next use */
      }
    }
    else {
      std::memcpy(mapped, s_ui.staging.data(), byteCount);
      SDL_UnmapGPUTransferBuffer(s_ui.device, s_ui.transfer);
      SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commands);
      for (const PendingUpload &upload : s_ui.uploads) {
        if (upload.dedicated != nullptr) {
          /* the whole texture is rewritten: cycle it, so the previous frame's draws need not finish first */
          SDL_GPUTextureTransferInfo source;
          SDL_zero(source);
          source.transfer_buffer = s_ui.transfer;
          source.offset = static_cast<Uint32>(upload.stagingOffset * sizeof(uint32_t));
          source.pixels_per_row = upload.rowPixels;
          source.rows_per_layer = upload.height;
          SDL_GPUTextureRegion destination;
          SDL_zero(destination);
          destination.texture = upload.dedicated;
          destination.w = upload.width;
          destination.h = upload.height;
          destination.d = 1;
          SDL_UploadToGPUTexture(copyPass, &source, &destination, true);
          continue;
        }
        Page &page = upload.stream ? s_ui.streamPages[upload.page] : s_ui.pages[upload.page];
        SDL_GPUTextureTransferInfo source;
        SDL_zero(source);
        source.transfer_buffer = s_ui.transfer;
        source.offset = static_cast<Uint32>(upload.stagingOffset * sizeof(uint32_t));
        source.pixels_per_row = upload.rowPixels;
        source.rows_per_layer = upload.height;
        SDL_GPUTextureRegion destination;
        SDL_zero(destination);
        destination.texture = page.texture;
        destination.x = upload.x;
        destination.y = upload.y;
        destination.w = upload.width;
        destination.h = upload.height;
        destination.d = 1;
        /* a streaming page is rewritten from scratch every frame: its first upload may cycle the texture, so the
           previous frame's draws need not finish first. Cache pages keep their contents. */
        const bool cycle = upload.stream && !page.cycledThisFrame;
        if (upload.stream) {
          page.cycledThisFrame = true;
        }
        SDL_UploadToGPUTexture(copyPass, &source, &destination, cycle);
      }
      SDL_EndGPUCopyPass(copyPass);
    }
  }
  EndFrame();
}

GpuUiTexRegion GpuUiTextures_UploadRegion(const uint32_t *pixels, int w, int h, int pitch)
{
  GpuUiTexRegion failed{nullptr, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0};
  if (!s_ui.active || (pixels == nullptr) || (w <= 0) || (h <= 0) || (w > 16384) || (h > 16384) ||
      (pitch < w * 4)) {
    return failed;
  }
  const uint32_t width = static_cast<uint32_t>(w);
  const uint32_t height = static_cast<uint32_t>(h);
  Rect rect{};
  uint32_t pageIndex = 0;
  bool placed = false;
  for (; pageIndex < s_ui.streamPages.size(); pageIndex++) {
    if (AllocateInPage(s_ui.streamPages[pageIndex], width + 2 * kPadding, height + 2 * kPadding, rect)) {
      placed = true;
      break;
    }
  }
  if (!placed) {
    if (!AddPage(s_ui.streamPages, width + 2 * kPadding, height + 2 * kPadding)) {
      return failed;
    }
    pageIndex = static_cast<uint32_t>(s_ui.streamPages.size() - 1);
    if (!AllocateInPage(s_ui.streamPages.back(), width + 2 * kPadding, height + 2 * kPadding, rect)) {
      return failed;
    }
  }
  Page &page = s_ui.streamPages[pageIndex];
  page.lastUsedFrame = s_ui.frame;
  uint32_t *staged = StageUpload(true, pageIndex, rect.x, rect.y, width, height);
  const uint32_t rowPixels = s_ui.uploads.back().rowPixels;
  const auto *source = reinterpret_cast<const uint8_t *>(pixels);
  for (uint32_t row = 0; row < height; row++) {
    std::memcpy(staged + static_cast<size_t>(row + 1) * rowPixels + 1, source + static_cast<size_t>(row) * pitch,
                static_cast<size_t>(width) * 4);
  }
  FillPadding(staged, width, height, rowPixels);
  return RegionOf(page, rect, width, height);
}

bool GpuUiTextures_DedicatedImage(const void *key, uint32_t generation, const uint32_t *pixels, int w, int h,
                                  int pitch, GpuUiTexRegion *out)
{
  if (!s_ui.active || (key == nullptr) || (pixels == nullptr) || (out == nullptr) || (w <= 0) || (h <= 0) ||
      (w > 16384) || (h > 16384) || (pitch < w * 4)) {
    return false;
  }
  const uint32_t width = static_cast<uint32_t>(w);
  const uint32_t height = static_cast<uint32_t>(h);
  size_t index = 0;
  while ((index < s_ui.dedicated.size()) && (s_ui.dedicated[index].key != key)) {
    index++;
  }
  if (index == s_ui.dedicated.size()) {
    s_ui.dedicated.push_back(Dedicated{key, generation, 0, 0, nullptr, s_ui.frame});
  }
  Dedicated &dedicated = s_ui.dedicated[index];
  bool stale = (dedicated.generation != generation);
  if ((dedicated.width != width) || (dedicated.height != height)) {
    if (s_ui.device != nullptr) {
      SDL_ReleaseGPUTexture(s_ui.device, dedicated.texture); /* lives on until the frames using it are done */
    }
    dedicated.texture = nullptr;
    dedicated.width = width;
    dedicated.height = height;
    if (s_ui.device != nullptr) {
      dedicated.texture = CreatePageTexture(width + 2 * kPadding, height + 2 * kPadding);
      if (dedicated.texture == nullptr) {
        s_ui.dedicated.erase(s_ui.dedicated.begin() + static_cast<ptrdiff_t>(index));
        return false;
      }
    }
    stale = true;
  }
  dedicated.lastUsedFrame = s_ui.frame;
  if (stale) {
    dedicated.generation = generation;
    uint32_t *staged = StageUpload(false, 0, 0, 0, width, height);
    PendingUpload &upload = s_ui.uploads.back();
    upload.dedicated = dedicated.texture;
    const uint32_t rowPixels = upload.rowPixels;
    const auto *source = reinterpret_cast<const uint8_t *>(pixels);
    std::memset(staged, 0, static_cast<size_t>(rowPixels) * (height + 2 * kPadding) * sizeof(uint32_t));
    for (uint32_t row = 0; row < height; row++) {
      std::memcpy(staged + static_cast<size_t>(row + kPadding) * rowPixels + kPadding,
                  source + static_cast<size_t>(row) * static_cast<size_t>(pitch), static_cast<size_t>(width) * 4);
    }
  }
  const float textureWidth = static_cast<float>(width + 2 * kPadding);
  const float textureHeight = static_cast<float>(height + 2 * kPadding);
  out->page = dedicated.texture;
  out->u0 = static_cast<float>(kPadding) / textureWidth;
  out->v0 = static_cast<float>(kPadding) / textureHeight;
  out->u1 = static_cast<float>(kPadding + width) / textureWidth;
  out->v1 = static_cast<float>(kPadding + height) / textureHeight;
  out->w = w;
  out->h = h;
  return true;
}

void GpuUiTextures_Evict(const GraphicsTextureSourceAsset *asset)
{
  if (!s_ui.active || (asset == nullptr)) {
    return;
  }
  auto cursor = s_ui.entries.lower_bound(Key{asset, 0, 0});
  while ((cursor != s_ui.entries.end()) && (cursor->first.asset == asset)) {
    FreeCacheRect(cursor->second.page, cursor->second.rect);
    cursor = s_ui.entries.erase(cursor);
    s_ui.stats.evictions++;
  }
}

void GpuUiTextures_GetStats(GpuUiTexStats *out)
{
  if (out == nullptr) {
    return;
  }
  GpuUiTexStats stats = s_ui.stats;
  stats.pages = static_cast<uint32_t>(s_ui.pages.size());
  stats.oversizePages = 0;
  stats.pageArea = 0;
  stats.usedArea = 0;
  for (const Page &page : s_ui.pages) {
    if ((page.width > GPU_UI_PAGE_SIZE) || (page.height > GPU_UI_PAGE_SIZE)) {
      stats.oversizePages++;
    }
    stats.pageArea += static_cast<uint64_t>(page.width) * page.height;
    stats.usedArea += page.usedArea;
  }
  stats.streamPages = static_cast<uint32_t>(s_ui.streamPages.size());
  stats.entries = static_cast<uint32_t>(s_ui.entries.size());
  stats.texels = 0;
  for (const auto &cached : s_ui.entries) {
    stats.texels += static_cast<uint64_t>(cached.second.width) * cached.second.height;
  }
  *out = stats;
}
