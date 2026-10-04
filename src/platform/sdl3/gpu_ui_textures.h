/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/gpu_ui_textures.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_SDL3_GPU_UI_TEXTURES_H
#define THANDOR_PLATFORM_SDL3_GPU_UI_TEXTURES_H

/* Step 9: the GPU texture cache of the UI images (docs/plans/step9_gpu_ui.md, section 4.2; work package 2).

   Every subresource of a 'gfx' texture source that the UI draws gets a rectangle in one of the UI atlas pages
   (B8G8R8A8_UNORM, GPU_UI_PAGE_SIZE squared; an image larger than a page gets a page of its own size). The key is
   (asset pointer, subresource, palette bank). The texels are the palette's +0 ARGB colours (paletted images) or the
   stored ARGB texels (paletteIndex -1), straight alpha, never the +4 framebuffer half (section 6.4). Each rectangle
   has a 1 px border that repeats the edge texels, so linear sampling at the edges does not bleed.

   Lookups convert at call time: the converted texels go into a CPU staging buffer at once (the simulation may
   rewrite asset memory between the draw call and the flush, section 6.4 "frame ordering"), and
   GpuUiTextures_FlushUploads copies everything staged since the last flush in one copy pass. Each cached entry is
   validated once per frame (frame = the time between two flushes): its texels and palette bank are hashed
   (as gpu_renderer.cpp's AtlasSlotOfTexture does once per scene) and converted again when they changed, or moved
   when the image size changed. So mutable sources (minimap plane, movie frames, captured assets) and a freed
   pointer that is reused for another asset never show stale pixels. In addition, the texture-source release
   callbacks (g_GraphicsTextureSourceLifecycleCallbacks3: releasePackage, releaseClone) evict every entry of the
   released asset (g_GraphicsTextureSourceReleaseObserver), so its rectangles can be reused; freed rectangles
   become reusable only after the next flush, so a draw of the same frame keeps its pixels.

   When more than GPU_UI_SOFT_PAGE_LIMIT pages are in use at a flush, the cache starts over (all entries are
   converted again on their next use). Streaming CPU images (GpuUiTextures_UploadRegion: minimap, credits, movie
   frames in the MVP) go to separate streaming pages that are refilled every frame.

   Init with a nullptr device runs the cache without a GPU (packing and conversion only; FlushUploads then folds
   the staged texels into GpuUiTexStats::contentHash): used by OPEN_THANDOR_SELFTEST=uiatlas. */

#include <SDL3/SDL_gpu.h>

#include <cstdint>

struct GraphicsTextureSourceAsset;

/* An image in an atlas page: sample [u0, u1) x [v0, v1) of page; w x h is the stored pixel size of the image
   (entry pixelWidth / pixelHeight; the entry origin is the caller's business). */
struct GpuUiTexRegion {
  SDL_GPUTexture *page; /* nullptr without a device (self-test) */
  float u0, v0, u1, v1;
  int w, h;
};

/* paletteBank value of GpuUiTextures_Lookup: use the bank the subresource entry names (what the software blits
   do). Any other value selects that bank of the asset for a paletted subresource (ignored for direct-colour
   subresources); an invalid bank fails the lookup. */
constexpr uint32_t GPU_UI_TEX_ENTRY_PALETTE = 0xFFFFFFFFu;

constexpr uint32_t GPU_UI_PAGE_SIZE = 2048;    /* side of an atlas page */
constexpr uint32_t GPU_UI_SOFT_PAGE_LIMIT = 4; /* more cache pages at a flush: start over */
constexpr uint32_t GPU_UI_HARD_PAGE_LIMIT = 16; /* never more cache pages (with a device) */

/* Starts the cache on device (nullptr: no GPU, see above) and installs the release observer. */
bool GpuUiTextures_Init(SDL_GPUDevice *device);

/* Releases all pages and buffers and removes the release observer (device is the one given to Init; it may be
   omitted). */
void GpuUiTextures_Shutdown(SDL_GPUDevice *device = nullptr);

/* The atlas rectangle of one subresource of a 'gfx' asset, converted and staged for upload when it is new or
   its content changed since the last frame. False (and *out untouched) for a NULL or non-'gfx' asset, an index
   out of range, an invalid palette bank, an empty image or when no page can hold it. */
bool GpuUiTextures_Lookup(const GraphicsTextureSourceAsset *asset, uint32_t subresource, uint32_t paletteBank,
                          GpuUiTexRegion *out);

/* Records a copy pass into commands with every upload staged since the last flush (call before the render pass
   that samples the pages; WP4 calls it at every present site), then ends the frame: the next lookup of each
   entry validates it again, the streaming pages are refilled from scratch, rectangles freed by evictions become
   reusable. With a device, a NULL commands keeps everything staged (nothing happens). */
void GpuUiTextures_FlushUploads(SDL_GPUCommandBuffer *commands);

/* Streaming upload of a CPU image (w x h pixels 0xAARRGGBB, rows pitch BYTES apart) for this frame only: the
   region stays valid until the next GpuUiTextures_FlushUploads has been recorded and must not be used after it.
   The alpha bytes are uploaded as given. Returns page nullptr and w = h = 0 when it fails. */
GpuUiTexRegion GpuUiTextures_UploadRegion(const uint32_t *pixels, int w, int h, int pitch);

/* A dedicated texture for a mutable CPU image that is sampled with linear filtering (the minimap, step 9 WP5):
   w x h pixels 0xAARRGGBB (rows pitch BYTES apart) inside a 1-texel border of 0, so samples beyond the image fade
   to black like the software sampler's "texels outside count as 0". One texture per key; its pixels are staged
   only when generation, w or h differ from its last upload (the caller bumps generation whenever it rewrites the
   pixels), so an unchanged image costs nothing. The returned region covers the inner w x h and stays usable until
   the key's next call that changes it; a texture unused for a while is released. False when it fails. */
bool GpuUiTextures_DedicatedImage(const void *key, uint32_t generation, const uint32_t *pixels, int w, int h,
                                  int pitch, GpuUiTexRegion *out);

/* Forgets every entry of asset (installed as g_GraphicsTextureSourceReleaseObserver by Init). */
void GpuUiTextures_Evict(const GraphicsTextureSourceAsset *asset);

struct GpuUiTexStats {
  uint32_t pages;         /* cache pages (including oversized ones) */
  uint32_t oversizePages; /* pages larger than GPU_UI_PAGE_SIZE for a single big image */
  uint32_t streamPages;
  uint32_t entries;
  uint64_t texels;        /* sum of w * h over all entries */
  uint64_t usedArea;      /* sum of the allocated rectangles (with padding and shelf waste) */
  uint64_t pageArea;      /* sum of the page areas */
  uint64_t uploadedTexels;/* staged and uploaded since Init (with padding) */
  uint32_t uploads;       /* staged uploads since Init */
  uint32_t reconversions; /* entries converted again because their content changed */
  uint32_t moves;         /* entries moved because their image size changed */
  uint32_t evictions;     /* entries removed by GpuUiTextures_Evict */
  uint32_t resets;        /* soft-limit restarts */
  uint32_t contentHash;   /* without a device: hash over every flushed upload (page, position, texels) */
};

void GpuUiTextures_GetStats(GpuUiTexStats *out);

#endif /* THANDOR_PLATFORM_SDL3_GPU_UI_TEXTURES_H */
