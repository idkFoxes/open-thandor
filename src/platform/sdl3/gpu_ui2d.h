/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/gpu_ui2d.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_SDL3_GPU_UI2D_H
#define THANDOR_PLATFORM_SDL3_GPU_UI2D_H

/* GPU 2D renderer of step 9 (docs/plans/step9_gpu_ui.md 5.3): draws textured or filled triangles in target pixels
   with the software blits' blend rules (src/graphics/backend/software_blit.cpp), one pipeline per blend mode, shaders
   in src/platform/sdl3/shaders/ui2d.hlsl. The caller (the 2D draw list's GPU backend) builds the quads; this module
   only owns the pipelines and the draw. */

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_rect.h>

#include <cstdint>

/* One triangle corner. Position in target pixels (top-left origin, so a quad from x0 to x1 covers the pixels
   x0 .. x1 - 1), u/v normalized in the page texture. */
struct GpuUiVertex {
  float x;
  float y;
  float u;
  float v;
  uint32_t tint;  /* ARGB: the modulation (GPU_UI_BLEND_MODULATED) or the colour (GPU_UI_BLEND_FILL); else unused */
  uint32_t flags; /* GPU_UI_VERTEX_FLAG_* */
};
static_assert(sizeof(GpuUiVertex) == 24);

/* The source texel came from a paletted image (GPU_UI_BLEND_HALF_RGB draws it at half strength). */
constexpr uint32_t GPU_UI_VERTEX_FLAG_PALETTED = 1u;

/* Blend modes (GpuUi2D_Draw's blend); 0..3 are Draw2DItem's blend values, FILL is the FILL op.
     SRC_ALPHA_SKIP0  BlitSourceAlpha: alpha 0 skipped, 0xFF written, anything else blended
     HALF_RGB         BlitHalfSourceRgb: alpha 0 skipped, everything else blended; a paletted source (vertex flag)
                      at half strength
     MODULATED        BlitModulatedSourceAlpha: each channel (alpha too) (c * tint) >> 8, then as SRC_ALPHA_SKIP0
                      (the alpha is at most 0xFE, so it always blends)
     OPAQUE           the texel's colour, no blend
     FILL             FillRectArgb: the tint, no texture (page may be null); alpha 0 skipped, 0xFF written */
enum GpuUiBlend : uint8_t {
  GPU_UI_BLEND_SRC_ALPHA_SKIP0 = 0,
  GPU_UI_BLEND_HALF_RGB = 1,
  GPU_UI_BLEND_MODULATED = 2,
  GPU_UI_BLEND_OPAQUE = 3,
  GPU_UI_BLEND_FILL = 4,
  GPU_UI_BLEND_COUNT = 5
};

/* Creates the shaders (SPIR-V or DXBC, whichever the device takes), the nearest sampler and one pipeline per blend
   mode for colour targets of the given format. False (logged) on failure; everything created is released again. */
bool GpuUi2D_Init(SDL_GPUDevice *device, SDL_GPUTextureFormat target);
/* Releases what GpuUi2D_Init created (safe without it). */
void GpuUi2D_Shutdown(SDL_GPUDevice *device);
/* Draws count vertices (a triangle list, count a multiple of 3) inside renderPass into a target of targetW x
   targetH pixels: sets the viewport to the whole target and the scissor (target pixels), binds the blend mode's
   pipeline and page (nearest sampling, clamped; ignored for GPU_UI_BLEND_FILL) and draws. The vertices are pushed
   through commandBuffer as vertex uniform data (no copy pass is possible inside a render pass), in chunks of
   GPU_UI_VERTICES_PER_CHUNK, one draw call each. Does nothing without GpuUi2D_Init, for an unknown blend mode or a
   missing page. */
void GpuUi2D_Draw(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *page,
                  const GpuUiVertex *vertices, uint32_t count, SDL_Rect scissor, uint8_t blend, int targetW,
                  int targetH);

/* Vertices per draw call: 4 KiB of uniform data (ui2d.hlsl), rounded down to whole triangles and quads. */
constexpr uint32_t GPU_UI_VERTICES_PER_CHUNK = 168;

#endif /* THANDOR_PLATFORM_SDL3_GPU_UI2D_H */
