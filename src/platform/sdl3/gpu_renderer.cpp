/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/gpu_renderer.cpp
 * Project code (not in the original game)
 */

/* SDL3 backend, stage 2: rasterizes the primitive queues on the GPU through SDL_GPU and presents the finished frames
   through the same device (CMake option THANDOR_RENDERER_SDL_GPU). video.cpp decides which renderer runs (the
   display settings' choice Vulkan / DirectX 12 / Software, or OPEN_THANDOR_GPU) and starts the device here with
   StartGpuDevice: Vulkan with the SPIR-V shaders, Direct3D 12 with the DXBC shaders (primitives.hlsl). The device
   claims the main window; PresentWithGpu uploads the software framebuffer (with the cursor) into a texture and
   blits it letterboxed into the swapchain, so no SDL_Renderer (and no second graphics API) runs beside it.

   Only g_GraphicsDrawPrimitiveQueue is replaced: lighting, fog, projection, clipping, culling and the radix sort
   stay on the CPU, so the simulation and the state hash are untouched. One scene (g_GraphicsSetViewportAndClearDepth
   up to g_GraphicsEndScene, i.e. the four passes of FrontendModelPointerContext_RenderWorldViewQueuesClipped) is
   collected and drawn at its end:

   - g_GraphicsSetViewportAndClearDepth: the software clear (black rectangle, new depth epoch) and a new scene.
   - g_GraphicsDrawPrimitiveQueue: copies the sorted queue at once (the packet pool is reused by the next pass) into
     36-byte vertices and runs of equal pipeline and clip rectangle. How a packet becomes GPU triangles is the
     rasterization ([graphics] gpu_rasterization, OPEN_THANDOR_GPU_RASTER; see ChooseRasterization):
       smooth (default): the packet's own triangle as the original's Direct3D renderer drew it - corners at their
         Q12 sub-pixel screen position, w = the view depth, so textures and colours are interpolated
         perspective-correctly (AppendSmoothTriangle). Textures stay put on the geometry while the camera moves.
       exact (compare mode): the triangle the software rasterizer would draw, rebuilt from its fixed-point setup
         (corners snapped to whole pixels; edges, depth, U/V and colour planes, see SoftwareTriangleSetup), so the
         GPU fills the same pixels with the same affine attributes - and swims and jitters like it.
     Each texture used by a packet is looked up in
     an RGBA atlas (4096 x 4096, B8G8R8A8); its texels and palette are hashed once per scene and converted again when
     they changed (the generated shadow textures change every frame).
   - g_GraphicsEndScene: uploads vertices and changed atlas regions, draws the runs into an offscreen colour + depth
     target (cleared to black / far), downloads the clip rectangle and writes it into the software framebuffer, so
     the selection overlays, the UI and the cursor that the CPU draws afterwards stay as they are.

   Pipelines per software mode (index (renderFlags & 0x3F000) >> 12, see docs/software_raster.md):
     0/8/16/24                       opaque, depth write
     1/9/17/25 and 32..63 (except 2) source-alpha blend, no depth write
     2/10/18/26 and 34/42/50/58      additive, no depth write
     4/6/12/14, 20/22/28/30          source-alpha blend, then a depth-only draw of the same run that writes depth
                                     where the modulated alpha is >= 128 (runs split where packets overlap)
     3/5/7/... (empty table entries) not drawn
   The depth test is <= on depth / 2^32 (interpolated linearly in screen space in both rasterizations). Not
   reproduced exactly (exact rasterization): the blend tables (and their over-reads), the
   16-bit quantization after each blend, the 16-bit lane wrap and per-pixel rounding of the MMX interpolation,
   GPU sub-pixel snapping of the rebuilt edges (1/256 pixel).

   OPEN_THANDOR_GPU=compare (developer tools): both rasterizers run; the software picture is shown and every
   OPEN_THANDOR_GPU_COMPARE_MS milliseconds (default 5000) the scene's clip rectangle is written as
   shots\gpucmp_NNNN_sw.bmp, _gpu.bmp and _diff.bmp with the difference statistics in thandor.log. */

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <unordered_map>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/sdl3/platform.h>
#include <thandor/platform/sdl3/sdl_objects.h>
#include <thandor/platform/system/win32.h>

/* The compiled shaders (CMake: <build>/gpu_shaders): DXBC from fxc, SPIR-V from dxc when it was found. */
namespace thandor::sdl3::gpu_shaders {
using BYTE = unsigned char;
#include "gpu_shader_vertex.h"
#include "gpu_shader_fragment.h"
#include "gpu_shader_fragment_alpha_test.h"
#ifdef THANDOR_GPU_SHADERS_SPIRV
#include "gpu_shader_vertex_spirv.h"
#include "gpu_shader_fragment_spirv.h"
#include "gpu_shader_fragment_alpha_test_spirv.h"
#endif
} // namespace thandor::sdl3::gpu_shaders

namespace {

constexpr Uint32 kAtlasSize = 4096;
constexpr uint16_t kUntexturedMask = 0xFFFF;
constexpr double kDepthScale = 1.0 / 4294967296.0;
/* D3D12 copies between buffers and textures need 512-byte aligned offsets and 256-byte aligned row pitches;
   otherwise SDL_GPU copies every upload/download through a temporary buffer. */
constexpr size_t kUploadPlacementPixels = 512 / 4;
constexpr uint32_t kUploadPitchPixels = 256 / 4;

/* One packet corner as the vertex shader reads it (primitives.hlsl). */
struct GpuVertex {
  float x; /* clip space: normalized device coordinates times w */
  float y;
  float z;
  float w; /* 1 (software-exact: affine) or the view depth (smooth: perspective-correct) */
  float u;
  float v;
  uint32_t colorArgb;
  uint16_t atlasX;
  uint16_t atlasY;
  uint16_t widthMask;
  uint16_t heightMask;
};
static_assert(sizeof(GpuVertex) == 36);

enum GpuPipelineIndex : int {
  GPU_PIPELINE_OPAQUE,
  GPU_PIPELINE_ALPHA,
  GPU_PIPELINE_ADDITIVE,
  GPU_PIPELINE_ALPHA_DEPTH, /* depth-only pass of an alpha-tested run */
  GPU_PIPELINE_COUNT
};

/* How a run is drawn: one of the first three pipelines, or the alpha pipeline followed by the depth pass. */
enum GpuRunKind : int { GPU_RUN_OPAQUE, GPU_RUN_ALPHA, GPU_RUN_ADDITIVE, GPU_RUN_ALPHA_WRITES_DEPTH, GPU_RUN_NONE };

/* Screen bounds (normalized device coordinates) of one packet. */
struct RunBounds {
  float minX;
  float minY;
  float maxX;
  float maxY;
};

struct GpuRun {
  GpuRunKind kind;
  SDL_Rect scissor;
  Uint32 firstVertex;
  Uint32 vertexCount;
};

/* A texture in the atlas, keyed by its texels, palette and size. */
struct AtlasKey {
  const uint8_t *texels;
  const uint8_t *palette;
  uint32_t widthLog2;
  uint32_t heightLog2;
  bool operator==(const AtlasKey &other) const noexcept
  {
    return (texels == other.texels) && (palette == other.palette) && (widthLog2 == other.widthLog2) &&
           (heightLog2 == other.heightLog2);
  }
};
struct AtlasKeyHash {
  size_t operator()(const AtlasKey &key) const noexcept
  {
    size_t hash = reinterpret_cast<uintptr_t>(key.texels) * 0x9E3779B1u;
    hash ^= reinterpret_cast<uintptr_t>(key.palette) + 0x7F4A7C15u + (hash << 6) + (hash >> 2);
    return hash ^ (key.widthLog2 << 8) ^ key.heightLog2;
  }
};
struct AtlasSlot {
  uint16_t x;
  uint16_t y;
  uint32_t contentHash;
  uint32_t validatedScene; /* scene serial of the last hash check */
};

/* A shelf of the atlas: textures of one height side by side. */
struct AtlasShelf {
  uint32_t y;
  uint32_t height;
  uint32_t nextX;
};

/* A converted texture waiting for the next upload. */
struct PendingUpload {
  uint32_t x;
  uint32_t y;
  uint32_t width;
  uint32_t height;
  size_t stagingOffset; /* in pixels, a multiple of kUploadPlacementPixels */
  uint32_t rowPixels;   /* staging row pitch, a multiple of kUploadPitchPixels */
};

struct GpuTimes {
  uint64_t collect = 0;
  uint64_t submit = 0;
  uint64_t software = 0;
  uint32_t scenes = 0;
  uint32_t vertices = 0;
  uint32_t runs = 0;
  uint32_t uploadedTexels = 0;
};

enum GpuMode : int { GPU_MODE_OFF, GPU_MODE_ON, GPU_MODE_COMPARE };

/* How a packet becomes GPU triangles (see AppendPacket). */
enum GpuRasterization : int {
  GPU_RASTERIZATION_SMOOTH, /* the packet's own triangle: sub-pixel corners, perspective-correct (default) */
  GPU_RASTERIZATION_EXACT   /* the software rasterizer's triangle (pixel-snapped, affine); compare mode */
};

struct GpuState {
  GpuMode mode = GPU_MODE_OFF;
  GpuRasterization rasterization = GPU_RASTERIZATION_SMOOTH;
  SDL_GPUDevice *device = nullptr;
  SDL_GPUShaderFormat shaderFormat = SDL_GPU_SHADERFORMAT_INVALID;
  SDL_Window *window = nullptr; /* the claimed window */
  SDL_GPUGraphicsPipeline *pipelines[GPU_PIPELINE_COUNT] = {};
  SDL_GPUSampler *sampler = nullptr;
  SDL_GPUTexture *atlas = nullptr;
  SDL_GPUTexture *colorTarget = nullptr;
  SDL_GPUTexture *depthTarget = nullptr;
  Uint32 targetWidth = 0;
  Uint32 targetHeight = 0;
  SDL_GPUBuffer *vertexBuffer = nullptr;
  Uint32 vertexBufferBytes = 0;
  SDL_GPUTransferBuffer *uploadBuffer = nullptr;
  Uint32 uploadBufferBytes = 0;
  SDL_GPUTransferBuffer *downloadBuffer = nullptr;
  Uint32 downloadBufferBytes = 0;

  std::unordered_map<AtlasKey, AtlasSlot, AtlasKeyHash> atlasSlots;
  std::vector<AtlasShelf> shelves;
  uint32_t shelvesBottom = 0;
  std::vector<uint32_t> staging;
  std::vector<PendingUpload> uploads;
  uint32_t paletteLut[256] = {};

  bool sceneOpen = false;
  uint32_t sceneSerial = 0;
  SDL_Rect sceneClip = {};
  std::vector<GpuVertex> vertices;
  std::vector<GpuRun> runs;
  std::vector<RunBounds> runBounds; /* triangles of the last run, when it is GPU_RUN_ALPHA_WRITES_DEPTH */
  bool atlasResetLogged = false;
  bool renderFailureLogged = false;

  GpuTimes times;
  uint64_t statsStart = 0;

  /* presentation: the framebuffer as a texture, uploaded through its own transfer buffer */
  SDL_GPUTexture *frameTexture = nullptr;
  SDL_GPUTextureFormat frameFormat = SDL_GPU_TEXTUREFORMAT_INVALID;
  Uint32 frameWidth = 0;
  Uint32 frameHeight = 0;
  SDL_GPUTransferBuffer *frameUpload = nullptr;
  Uint32 frameUploadBytes = 0;
  std::vector<uint32_t> frameConverted; /* RGB565 widened to B8G8R8A8 when the device cannot sample B5G6R5 */
  bool presentFailureLogged = false;

  /* compare mode */
  std::vector<uint32_t> gpuPixels;
  uint32_t compareIntervalMs = 5000;
  uint32_t lastCompareTick = 0;
  uint32_t compareNumber = 0;
};
GpuState s_gpu;

/* --- hashing and texture conversion ------------------------------------------------------------------------ */

inline uint32_t RotateLeft(uint32_t value, int count) noexcept
{
  return (value << count) | (value >> (32 - count));
}

/* xxHash32-style hash of byteCount bytes (byteCount a multiple of 4). */
uint32_t HashBytes(const uint8_t *bytes, size_t byteCount, uint32_t seed) noexcept
{
  constexpr uint32_t kPrime1 = 0x9E3779B1u;
  constexpr uint32_t kPrime2 = 0x85EBCA77u;
  constexpr uint32_t kPrime3 = 0xC2B2AE3Du;
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
  hash ^= static_cast<uint32_t>(byteCount);
  hash ^= hash >> 15;
  hash *= kPrime2;
  hash ^= hash >> 13;
  hash *= kPrime3;
  hash ^= hash >> 16;
  return hash;
}

/* Forgets every atlas entry (the atlas is full); textures are converted again on their next use. */
void ResetAtlas() noexcept
{
  s_gpu.atlasSlots.clear();
  s_gpu.shelves.clear();
  s_gpu.shelvesBottom = 0;
  s_gpu.uploads.clear();
  s_gpu.staging.clear();
  if (!s_gpu.atlasResetLogged) {
    Thandor_Log("SDL_GPU renderer: texture atlas (nearly) full, starting over");
    s_gpu.atlasResetLogged = true;
  }
}

/* A free width x height rectangle (powers of two) of the atlas, false when there is none. */
bool AllocateAtlasRectangle(uint32_t width, uint32_t height, uint32_t &outX, uint32_t &outY) noexcept
{
  for (AtlasShelf &shelf : s_gpu.shelves) {
    if ((shelf.height == height) && (shelf.nextX + width <= kAtlasSize)) {
      outX = shelf.nextX;
      outY = shelf.y;
      shelf.nextX += width;
      return true;
    }
  }
  if ((s_gpu.shelvesBottom + height > kAtlasSize) || (width > kAtlasSize)) {
    return false;
  }
  s_gpu.shelves.push_back(AtlasShelf{s_gpu.shelvesBottom, height, width});
  outX = 0;
  outY = s_gpu.shelvesBottom;
  s_gpu.shelvesBottom += height;
  return true;
}

/* Converts the texture to ARGB into the staging pixels and queues its upload to (x, y). */
void QueueTextureConversion(const AtlasKey &key, uint32_t x, uint32_t y) noexcept
{
  const uint32_t width = 1u << key.widthLog2;
  const uint32_t height = 1u << key.heightLog2;
  const size_t texelCount = static_cast<size_t>(width) * height;
  const uint32_t rowPixels = std::max(width, kUploadPitchPixels);
  const size_t stagingOffset =
      (s_gpu.staging.size() + kUploadPlacementPixels - 1) / kUploadPlacementPixels * kUploadPlacementPixels;
  s_gpu.staging.resize(stagingOffset + static_cast<size_t>(rowPixels) * height);
  uint32_t *destination = s_gpu.staging.data() + stagingOffset;
  if (key.palette != nullptr) {
    /* a palette entry is 8 bytes, the ARGB colour at +0 */
    for (int index = 0; index < 256; index++) {
      std::memcpy(&s_gpu.paletteLut[index], key.palette + index * 8, sizeof(uint32_t));
    }
    const uint8_t *source = key.texels;
    for (uint32_t row = 0; row < height; row++, source += width, destination += rowPixels) {
      for (uint32_t column = 0; column < width; column++) {
        destination[column] = s_gpu.paletteLut[source[column]];
      }
    }
  }
  else {
    const uint8_t *source = key.texels;
    for (uint32_t row = 0; row < height; row++, source += width * 4, destination += rowPixels) {
      std::memcpy(destination, source, width * sizeof(uint32_t));
    }
  }
  s_gpu.uploads.push_back(PendingUpload{x, y, width, height, stagingOffset, rowPixels});
  s_gpu.times.uploadedTexels += static_cast<uint32_t>(texelCount);
}

/* The atlas slot of a packet's texture, converted (again) when its texels or palette changed since the last
   scene. False when the texture does not fit. */
bool AtlasSlotOfTexture(const GraphicsTextureSetEntry *entry, AtlasSlot &outSlot) noexcept
{
  const GraphicsTextureSourceEntry *source = entry->sourceEntry;
  const uint8_t *asset = reinterpret_cast<const uint8_t *>(entry->sourceAsset.get());
  if ((source == nullptr) || (asset == nullptr) || (entry->widthLog2 > 12) || (entry->heightLog2 > 12)) {
    return false;
  }
  const int paletteIndex = static_cast<int>(source->paletteIndex);
  AtlasKey key;
  key.texels = asset + source->dataOffset;
  key.palette = (paletteIndex < 0)
                    ? nullptr
                    : asset + GFX_ASSET_HEADER_SIZE + static_cast<uint32_t>(paletteIndex) * GFX_PALETTE_BANK_SIZE;
  key.widthLog2 = entry->widthLog2;
  key.heightLog2 = entry->heightLog2;

  auto found = s_gpu.atlasSlots.find(key);
  if ((found != s_gpu.atlasSlots.end()) && (found->second.validatedScene == s_gpu.sceneSerial)) {
    outSlot = found->second;
    return true;
  }
  const size_t texelCount = static_cast<size_t>(1u << key.widthLog2) << key.heightLog2;
  uint32_t hash = HashBytes(key.texels, (key.palette != nullptr) ? texelCount : texelCount * 4, 0);
  if (key.palette != nullptr) {
    hash = HashBytes(key.palette, GFX_PALETTE_BANK_SIZE, hash);
  }
  if (found != s_gpu.atlasSlots.end()) {
    found->second.validatedScene = s_gpu.sceneSerial;
    if (found->second.contentHash != hash) {
      found->second.contentHash = hash;
      QueueTextureConversion(key, found->second.x, found->second.y);
    }
    outSlot = found->second;
    return true;
  }
  uint32_t x = 0;
  uint32_t y = 0;
  if (!AllocateAtlasRectangle(1u << key.widthLog2, 1u << key.heightLog2, x, y)) {
    ResetAtlas();
    if (!AllocateAtlasRectangle(1u << key.widthLog2, 1u << key.heightLog2, x, y)) {
      return false;
    }
  }
  AtlasSlot slot{static_cast<uint16_t>(x), static_cast<uint16_t>(y), hash, s_gpu.sceneSerial};
  s_gpu.atlasSlots.emplace(key, slot);
  QueueTextureConversion(key, x, y);
  outSlot = slot;
  return true;
}

/* --- packets ----------------------------------------------------------------------------------------------- */

/* The run kind of a software raster handler index (see the table at the top). */
GpuRunKind RunKindOfHandler(uint32_t handlerIndex) noexcept
{
  const uint32_t operation = handlerIndex & 7;
  if (handlerIndex >= 32) {
    if (operation == 2) {
      return GPU_RUN_ADDITIVE;
    }
    return ((operation == 0) || (operation == 1) || (operation == 4) || (operation == 6)) ? GPU_RUN_ALPHA
                                                                                         : GPU_RUN_NONE;
  }
  switch (operation) {
  case 0:
    return GPU_RUN_OPAQUE;
  case 1:
    return GPU_RUN_ALPHA;
  case 2:
    return GPU_RUN_ADDITIVE;
  case 4:
  case 6:
    return GPU_RUN_ALPHA_WRITES_DEPTH;
  default:
    return GPU_RUN_NONE;
  }
}

/* The triangle as the software rasterizer draws it (Raster_SetupTriangle / Raster_WalkTriangle /
   Raster_DrawScanline), rebuilt as at most three GPU triangles in its own sample space: after
   SoftwareRenderer_PrepareTrianglePacket (sorted by Y, snapped to whole pixels), row r of [y0, y2) covers the
   columns between the long edge x0 + k * longXStep and the short edge (x0 + k * upperStep above v1, x1 + k *
   lowerStep below), and the attributes of pixel px are those of the point (px + 1, r) on the long edge's affine
   function a0 + k * longStep + (X - longX(k)) * stepX. All steps are the software rasterizer's fixed-point values
   (1 / height in Q12, the 32-bit plane products), so tall or thin triangles keep the edges, depths and texture
   coordinates the software rasterizer gives them (on large triangles its edges miss the far vertices by many
   pixels) instead of the exact ones. */
struct SoftwareTriangleSetup {
  int x0;
  int row0;
  int longXStep;
  double depth0;
  int longDepthStep;
  int depthStepX;
  int u0;
  int longUStep;
  int uStepX;
  int v0;
  int longVStep;
  int vStepX;
  int color0[4]; /* Q6 */
  int longColorStep[4];
  int colorStepX[4];
};

/* A point of the rebuilt polygon: X in Q12 pixels of the software sample space, Y in rows. */
struct SoftwarePoint {
  double x;
  int row;
};

GpuVertex SoftwareVertexAt(const SoftwareTriangleSetup &setup, const SoftwarePoint &point, const AtlasSlot &slot,
                           uint16_t widthMask, uint16_t heightMask) noexcept
{
  const double k = static_cast<double>(point.row - setup.row0);
  const double fromLongEdge = (point.x - (static_cast<double>(setup.x0) + k * setup.longXStep)) / 4096.0;
  GpuVertex vertex;
  /* GPU pixel centre (px + 0.5, py + 0.5) = software sample point (px + 1, py); moved left by one GPU sub-pixel
     step, so a sample exactly on an edge falls on the side the software rasterizer gives it (left edge out, right
     edge in) */
  const double pixelX = point.x / 4096.0 - 0.5 + 1.0 / 256.0;
  const double pixelY = static_cast<double>(point.row) + 0.5;
  vertex.x = static_cast<float>(pixelX * 2.0 / s_gpu.targetWidth - 1.0);
  vertex.y = static_cast<float>(1.0 - pixelY * 2.0 / s_gpu.targetHeight);
  vertex.z = static_cast<float>((setup.depth0 + k * setup.longDepthStep + fromLongEdge * setup.depthStepX) *
                                kDepthScale);
  vertex.w = 1.0f;
  vertex.u = static_cast<float>((setup.u0 + k * setup.longUStep + fromLongEdge * setup.uStepX) / 4096.0);
  vertex.v = static_cast<float>((setup.v0 + k * setup.longVStep + fromLongEdge * setup.vStepX) / 4096.0);
  uint32_t color = 0;
  for (int lane = 0; lane < 4; lane++) {
    const double q6 = setup.color0[lane] + k * setup.longColorStep[lane] + fromLongEdge * setup.colorStepX[lane];
    const int channel = std::clamp(static_cast<int>(q6 / 64.0 + 0.5), 0, 255);
    color |= static_cast<uint32_t>(channel) << (8 * lane);
  }
  vertex.colorArgb = color;
  vertex.atlasX = slot.x;
  vertex.atlasY = slot.y;
  vertex.widthMask = widthMask;
  vertex.heightMask = heightMask;
  return vertex;
}

/* Appends the packet's triangle as the software rasterizer would draw it (see SoftwareTriangleSetup); returns
   the number of vertices appended (0 when the software rasterizer draws nothing: no height or no area). */
Uint32 AppendSoftwareTriangle(const GraphicsPrimitivePacket *packet, const AtlasSlot &slot, uint16_t widthMask,
                              uint16_t heightMask) noexcept
{
  GraphicsPrimitivePacket sorted = *packet;
  const uint32_t epoch = static_cast<uint32_t>(g_SoftwareDepthEpoch);
  SoftwareRenderer_PrepareTrianglePacket(&sorted); /* sort, snap, depth + epoch, U/V to the texture size */
  const GraphicsPrimitiveVertexRaw &v0 = sorted.vertices[0];
  const GraphicsPrimitiveVertexRaw &v1 = sorted.vertices[1];
  const GraphicsPrimitiveVertexRaw &v2 = sorted.vertices[2];
  const int height = v2.screenY - v0.screenY;
  if (height <= 0) {
    return 0;
  }
  auto mulShift = [](int a, int b, int shift) { return static_cast<int>((static_cast<long long>(a) * b) >> shift); };
  auto diff = [](int a, int b) { return static_cast<int>(static_cast<uint32_t>(a) - static_cast<uint32_t>(b)); };
  const long long cross = static_cast<long long>(v2.screenX - v0.screenX) * (v1.screenY - v0.screenY) -
                          static_cast<long long>(v1.screenX - v0.screenX) * (v2.screenY - v0.screenY);
  const int doubleArea = static_cast<int>(cross >> 12);
  if (doubleArea == 0) {
    return 0;
  }
  const int invHeight = 0x1000000 / height;
  const int invArea = static_cast<int>(0x1000000000LL / doubleArea);
  const int dy10 = v1.screenY - v0.screenY;
  const int dy20 = v2.screenY - v0.screenY;
  auto gradientX = [&](int a0, int a1, int a2, int shift) {
    const long long plane = static_cast<long long>(diff(a2, a0)) * dy10 - static_cast<long long>(diff(a1, a0)) * dy20;
    return mulShift(static_cast<int>(plane >> 12), invArea, shift);
  };
  SoftwareTriangleSetup setup;
  setup.x0 = v0.screenX;
  setup.row0 = v0.screenY >> 12;
  setup.longXStep = mulShift(v2.screenX - v0.screenX, invHeight, 12);
  setup.depth0 = static_cast<double>(static_cast<uint32_t>(v0.depth) - epoch);
  setup.longDepthStep = mulShift(diff(v2.depth, v0.depth), invHeight, 12);
  setup.depthStepX = gradientX(v0.depth, v1.depth, v2.depth, 24);
  setup.u0 = v0.textureU;
  setup.longUStep = mulShift(diff(v2.textureU, v0.textureU), invHeight, 12);
  setup.uStepX = gradientX(v0.textureU, v1.textureU, v2.textureU, 24);
  setup.v0 = v0.textureV;
  setup.longVStep = mulShift(diff(v2.textureV, v0.textureV), invHeight, 12);
  setup.vStepX = gradientX(v0.textureV, v1.textureV, v2.textureV, 24);
  for (int lane = 0; lane < 4; lane++) {
    const int c0 = static_cast<int>((v0.diffuseColor >> (8 * lane)) & 0xFF);
    const int c1 = static_cast<int>((v1.diffuseColor >> (8 * lane)) & 0xFF);
    const int c2 = static_cast<int>((v2.diffuseColor >> (8 * lane)) & 0xFF);
    setup.color0[lane] = c0 << 6;
    setup.longColorStep[lane] = mulShift(c2 - c0, invHeight, 6);
    setup.colorStepX[lane] = gradientX(c0 << 12, c1 << 12, c2 << 12, 30);
  }

  const int upperRows = (v1.screenY - v0.screenY) >> 12;
  const int lowerRows = (v2.screenY - v1.screenY) >> 12;
  const int row1 = setup.row0 + upperRows;
  const int row2 = row1 + lowerRows;
  const double x0 = static_cast<double>(v0.screenX);
  const double x1 = static_cast<double>(v1.screenX);
  const SoftwarePoint longAtRow1{x0 + static_cast<double>(upperRows) * setup.longXStep, row1};
  const SoftwarePoint longAtRow2{x0 + static_cast<double>(upperRows + lowerRows) * setup.longXStep, row2};
  SoftwarePoint polygon[9];
  int pointCount = 0;
  if (upperRows > 0) {
    const int upperStep = mulShift(0x1000000 / (v1.screenY - v0.screenY), v1.screenX - v0.screenX, 12);
    polygon[pointCount++] = SoftwarePoint{x0, setup.row0};
    polygon[pointCount++] = longAtRow1;
    polygon[pointCount++] = SoftwarePoint{x0 + static_cast<double>(upperRows) * upperStep, row1};
  }
  if (lowerRows > 0) {
    const int lowerStep = mulShift(0x1000000 / (v2.screenY - v1.screenY), v2.screenX - v1.screenX, 12);
    const SoftwarePoint shortAtRow1{x1, row1};
    const SoftwarePoint shortAtRow2{x1 + static_cast<double>(lowerRows) * lowerStep, row2};
    polygon[pointCount++] = longAtRow1;
    polygon[pointCount++] = shortAtRow1;
    polygon[pointCount++] = shortAtRow2;
    polygon[pointCount++] = longAtRow1;
    polygon[pointCount++] = shortAtRow2;
    polygon[pointCount++] = longAtRow2;
  }
  for (int index = 0; index < pointCount; index++) {
    s_gpu.vertices.push_back(SoftwareVertexAt(setup, polygon[index], slot, widthMask, heightMask));
  }
  return static_cast<Uint32>(pointCount);
}

/* Texel coordinate of a packet U/V (Q12 of a 256-texel range) for a texture of 2^sizeLog2 texels: the
   software rasterizer's scaling (SoftwareRenderer_PrepareTrianglePacket shifts right by 8 - sizeLog2) without
   dropping the fraction bits. */
double PacketTexel(int coordinate, uint32_t sizeLog2) noexcept
{
  if (sizeLog2 <= 8) {
    return static_cast<double>(coordinate) / (4096.0 * static_cast<double>(1u << (8 - sizeLog2)));
  }
  /* wider than 256 texels: the software rasterizer's masked shift count, as it samples them */
  return static_cast<double>(coordinate >> ((8 - sizeLog2) & 31)) / 4096.0;
}

/* Smooth rasterization: the packet's triangle as the original Direct3D renderer handed it to the device
   (D3DTLVERTEX sx/sy = screen X/Y / 4096, sz = depth, rhw = 1 / depth, tu/tv = U/V, Gouraud colour): corners
   with their full Q12 sub-pixel position (the software rasterizer snaps them to whole pixels), the depth
   interpolated linearly in screen space like the software and Direct3D depth buffers, and w = the view depth
   (the packet depth is the view-space Z of the corner, see GraphicsPrimitiveQueue_AppendTriangle and the terrain
   packet builders), so the GPU interpolates U/V and colour perspective-correctly instead of affinely. The pixel
   grid is the software rasterizer's (sample point (px + 1, py) = GPU pixel centre (px + 0.5, py + 0.5)), so the
   picture lines up with the overlays the CPU draws afterwards. Always appends three vertices. */
Uint32 AppendSmoothTriangle(const GraphicsPrimitivePacket *packet, const AtlasSlot &slot, uint16_t widthMask,
                            uint16_t heightMask) noexcept
{
  uint32_t widthLog2 = 0;
  uint32_t heightLog2 = 0;
  const bool textured = (widthMask != kUntexturedMask);
  if (textured) {
    widthLog2 = packet->textureEntry->widthLog2;
    heightLog2 = packet->textureEntry->heightLog2;
  }
  for (int corner = 0; corner < 3; corner++) {
    const GraphicsPrimitiveVertexRaw &source = packet->vertices[corner];
    const double pixelX = static_cast<double>(source.screenX) / 4096.0 - 0.5;
    const double pixelY = static_cast<double>(source.screenY) / 4096.0 + 0.5;
    const double depth = static_cast<double>(static_cast<uint32_t>(source.depth));
    /* the near plane keeps the view depth positive; the shadow patches' depth bias could bring it to 0 */
    const double w = std::max(depth, 1.0);
    GpuVertex vertex;
    vertex.x = static_cast<float>((pixelX * 2.0 / s_gpu.targetWidth - 1.0) * w);
    vertex.y = static_cast<float>((1.0 - pixelY * 2.0 / s_gpu.targetHeight) * w);
    vertex.z = static_cast<float>(depth * kDepthScale * w);
    vertex.w = static_cast<float>(w);
    vertex.u = textured ? static_cast<float>(PacketTexel(source.textureU, widthLog2)) : 0.0f;
    vertex.v = textured ? static_cast<float>(PacketTexel(source.textureV, heightLog2)) : 0.0f;
    vertex.colorArgb = source.diffuseColor;
    vertex.atlasX = slot.x;
    vertex.atlasY = slot.y;
    vertex.widthMask = widthMask;
    vertex.heightMask = heightMask;
    s_gpu.vertices.push_back(vertex);
  }
  return 3;
}

/* Appends one packet: its texture's atlas slot and its triangle as the software rasterizer draws it. Depths
   beyond 0..2^32 are clamped by the GPU (depth clip off). */
void AppendPacket(const GraphicsPrimitivePacket *packet, const SDL_Rect &scissor) noexcept
{
  const uint32_t handlerIndex = (packet->renderFlags & GRAPHICS_PRIMITIVE_RASTER_HANDLER_MASK) >> 12;
  const GpuRunKind kind = RunKindOfHandler(handlerIndex);
  if (kind == GPU_RUN_NONE) {
    return;
  }
  AtlasSlot slot{0, 0, 0, 0};
  uint16_t widthMask = kUntexturedMask;
  uint16_t heightMask = kUntexturedMask;
  if ((packet->renderFlags & GRAPHICS_PRIMITIVE_FLAG_TEXTURED) != 0) {
    const GraphicsTextureSetEntry *entry = packet->textureEntry;
    if ((entry == nullptr) || !AtlasSlotOfTexture(entry, slot)) {
      return;
    }
    widthMask = static_cast<uint16_t>((1u << entry->widthLog2) - 1);
    heightMask = static_cast<uint16_t>((1u << entry->heightLog2) - 1);
  }
  const Uint32 firstVertex = static_cast<Uint32>(s_gpu.vertices.size());
  const Uint32 vertexCount = (s_gpu.rasterization == GPU_RASTERIZATION_SMOOTH)
                                 ? AppendSmoothTriangle(packet, slot, widthMask, heightMask)
                                 : AppendSoftwareTriangle(packet, slot, widthMask, heightMask);
  if (vertexCount == 0) {
    return;
  }
  /* The alpha-tested modes write depth per pixel while they draw, so a later packet of such a run must not
     overlap an earlier one: their colour and depth draws would both happen before it. Such a packet starts a
     new run (bounding boxes; overlapping is rare apart from foliage). */
  RunBounds bounds{2.0f, 2.0f, -2.0f, -2.0f};
  if (kind == GPU_RUN_ALPHA_WRITES_DEPTH) {
    for (Uint32 index = firstVertex; index < firstVertex + vertexCount; index++) {
      const GpuVertex &vertex = s_gpu.vertices[index];
      const float x = vertex.x / vertex.w;
      const float y = vertex.y / vertex.w;
      bounds.minX = std::min(bounds.minX, x);
      bounds.minY = std::min(bounds.minY, y);
      bounds.maxX = std::max(bounds.maxX, x);
      bounds.maxY = std::max(bounds.maxY, y);
    }
  }
  if (!s_gpu.runs.empty()) {
    GpuRun &last = s_gpu.runs.back();
    bool joins = (last.kind == kind) && (last.scissor.x == scissor.x) && (last.scissor.y == scissor.y) &&
                 (last.scissor.w == scissor.w) && (last.scissor.h == scissor.h) &&
                 (last.firstVertex + last.vertexCount == firstVertex);
    if (joins && (kind == GPU_RUN_ALPHA_WRITES_DEPTH)) {
      for (const RunBounds &earlier : s_gpu.runBounds) {
        if ((bounds.minX < earlier.maxX) && (earlier.minX < bounds.maxX) && (bounds.minY < earlier.maxY) &&
            (earlier.minY < bounds.maxY)) {
          joins = false;
          break;
        }
      }
    }
    if (joins) {
      last.vertexCount += vertexCount;
      if (kind == GPU_RUN_ALPHA_WRITES_DEPTH) {
        s_gpu.runBounds.push_back(bounds);
      }
      return;
    }
  }
  s_gpu.runs.push_back(GpuRun{kind, scissor, firstVertex, vertexCount});
  s_gpu.runBounds.clear();
  if (kind == GPU_RUN_ALPHA_WRITES_DEPTH) {
    s_gpu.runBounds.push_back(bounds);
  }
}

/* --- device objects ---------------------------------------------------------------------------------------- */

/* The shader blobs of one entry point: DXBC and SPIR-V (empty without dxc). */
struct ShaderBlobs {
  const unsigned char *dxbc;
  size_t dxbcSize;
  const unsigned char *spirv;
  size_t spirvSize;
};

SDL_GPUShader *CreateShader(const ShaderBlobs &blobs, const char *entryPoint, SDL_GPUShaderStage stage,
                            Uint32 samplerCount) noexcept
{
  SDL_GPUShaderCreateInfo info;
  SDL_zero(info);
  if (s_gpu.shaderFormat == SDL_GPU_SHADERFORMAT_SPIRV) {
    info.code = blobs.spirv;
    info.code_size = blobs.spirvSize;
    info.entrypoint = entryPoint;
  }
  else {
    info.code = blobs.dxbc;
    info.code_size = blobs.dxbcSize;
    info.entrypoint = entryPoint;
  }
  if (info.code_size == 0) {
    SDL_SetError("no %s shader for %s", (s_gpu.shaderFormat == SDL_GPU_SHADERFORMAT_SPIRV) ? "SPIR-V" : "DXBC",
                 entryPoint);
    return nullptr;
  }
  info.format = s_gpu.shaderFormat;
  info.stage = stage;
  info.num_samplers = samplerCount;
  return SDL_CreateGPUShader(s_gpu.device, &info);
}

bool CreatePipelines() noexcept
{
  namespace shaders = thandor::sdl3::gpu_shaders;
#ifdef THANDOR_GPU_SHADERS_SPIRV
#define THANDOR_SHADER_BLOBS(entry)   ShaderBlobs{shaders::g_##entry, sizeof shaders::g_##entry, shaders::g_##entry##Spirv, sizeof shaders::g_##entry##Spirv}
#else
#define THANDOR_SHADER_BLOBS(entry) ShaderBlobs{shaders::g_##entry, sizeof shaders::g_##entry, nullptr, 0}
#endif
  SDL_GPUShader *vertexShader =
      CreateShader(THANDOR_SHADER_BLOBS(VertexMain), "VertexMain", SDL_GPU_SHADERSTAGE_VERTEX, 0);
  SDL_GPUShader *fragmentShader =
      CreateShader(THANDOR_SHADER_BLOBS(FragmentMain), "FragmentMain", SDL_GPU_SHADERSTAGE_FRAGMENT, 1);
  SDL_GPUShader *alphaTestShader = CreateShader(THANDOR_SHADER_BLOBS(FragmentAlphaTestMain), "FragmentAlphaTestMain",
                                                SDL_GPU_SHADERSTAGE_FRAGMENT, 1);
#undef THANDOR_SHADER_BLOBS
  if ((vertexShader == nullptr) || (fragmentShader == nullptr) || (alphaTestShader == nullptr)) {
    Thandor_Log("SDL_GPU renderer: shader creation failed: %s", SDL_GetError());
    SDL_ReleaseGPUShader(s_gpu.device, vertexShader);
    SDL_ReleaseGPUShader(s_gpu.device, fragmentShader);
    SDL_ReleaseGPUShader(s_gpu.device, alphaTestShader);
    return false;
  }

  SDL_GPUVertexBufferDescription bufferDescription;
  SDL_zero(bufferDescription);
  bufferDescription.slot = 0;
  bufferDescription.pitch = sizeof(GpuVertex);
  bufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  const SDL_GPUVertexAttribute attributes[] = {
      {0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, offsetof(GpuVertex, x)},
      {1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(GpuVertex, u)},
      {2, 0, SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM, offsetof(GpuVertex, colorArgb)},
      {3, 0, SDL_GPU_VERTEXELEMENTFORMAT_USHORT4, offsetof(GpuVertex, atlasX)},
  };

  bool created = true;
  for (int pipelineIndex = 0; pipelineIndex < GPU_PIPELINE_COUNT; pipelineIndex++) {
    SDL_GPUColorTargetDescription colorTarget;
    SDL_zero(colorTarget);
    colorTarget.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
    SDL_GPUColorTargetBlendState &blend = colorTarget.blend_state;
    blend.color_blend_op = SDL_GPU_BLENDOP_ADD;
    blend.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    switch (pipelineIndex) {
    case GPU_PIPELINE_ALPHA:
      blend.enable_blend = true;
      blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
      blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
      blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
      blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
      break;
    case GPU_PIPELINE_ADDITIVE:
      blend.enable_blend = true;
      blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
      blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
      blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
      blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
      break;
    case GPU_PIPELINE_ALPHA_DEPTH:
      blend.enable_color_write_mask = true;
      blend.color_write_mask = 0;
      break;
    default:
      break;
    }

    SDL_GPUGraphicsPipelineCreateInfo info;
    SDL_zero(info);
    info.vertex_shader = vertexShader;
    info.fragment_shader = (pipelineIndex == GPU_PIPELINE_ALPHA_DEPTH) ? alphaTestShader : fragmentShader;
    info.vertex_input_state.vertex_buffer_descriptions = &bufferDescription;
    info.vertex_input_state.num_vertex_buffers = 1;
    info.vertex_input_state.vertex_attributes = attributes;
    info.vertex_input_state.num_vertex_attributes = SDL_arraysize(attributes);
    info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    info.rasterizer_state.enable_depth_clip = false;
    info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL;
    info.depth_stencil_state.enable_depth_test = true;
    info.depth_stencil_state.enable_depth_write =
        (pipelineIndex == GPU_PIPELINE_OPAQUE) || (pipelineIndex == GPU_PIPELINE_ALPHA_DEPTH);
    info.target_info.color_target_descriptions = &colorTarget;
    info.target_info.num_color_targets = 1;
    info.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    info.target_info.has_depth_stencil_target = true;
    s_gpu.pipelines[pipelineIndex] = SDL_CreateGPUGraphicsPipeline(s_gpu.device, &info);
    if (s_gpu.pipelines[pipelineIndex] == nullptr) {
      Thandor_Log("SDL_GPU renderer: pipeline %d failed: %s", pipelineIndex, SDL_GetError());
      created = false;
    }
  }
  SDL_ReleaseGPUShader(s_gpu.device, vertexShader);
  SDL_ReleaseGPUShader(s_gpu.device, fragmentShader);
  SDL_ReleaseGPUShader(s_gpu.device, alphaTestShader);
  return created;
}

SDL_GPUTexture *CreateTexture(SDL_GPUTextureFormat format, SDL_GPUTextureUsageFlags usage, Uint32 width,
                              Uint32 height) noexcept
{
  SDL_GPUTextureCreateInfo info;
  SDL_zero(info);
  info.type = SDL_GPU_TEXTURETYPE_2D;
  info.format = format;
  info.usage = usage;
  info.width = width;
  info.height = height;
  info.layer_count_or_depth = 1;
  info.num_levels = 1;
  info.sample_count = SDL_GPU_SAMPLECOUNT_1;
  return SDL_CreateGPUTexture(s_gpu.device, &info);
}

/* Colour and depth target in framebuffer size. */
bool EnsureTargets() noexcept
{
  const Uint32 width = g_FramebufferWidth;
  const Uint32 height = g_FramebufferHeight;
  if ((s_gpu.colorTarget != nullptr) && (s_gpu.targetWidth == width) && (s_gpu.targetHeight == height)) {
    return true;
  }
  SDL_ReleaseGPUTexture(s_gpu.device, s_gpu.colorTarget);
  SDL_ReleaseGPUTexture(s_gpu.device, s_gpu.depthTarget);
  s_gpu.colorTarget = nullptr;
  s_gpu.depthTarget = nullptr;
  s_gpu.targetWidth = 0;
  s_gpu.targetHeight = 0;
  if ((width == 0) || (height == 0)) {
    return false;
  }
  s_gpu.colorTarget =
      CreateTexture(SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM, SDL_GPU_TEXTUREUSAGE_COLOR_TARGET, width, height);
  s_gpu.depthTarget =
      CreateTexture(SDL_GPU_TEXTUREFORMAT_D32_FLOAT, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET, width, height);
  if ((s_gpu.colorTarget == nullptr) || (s_gpu.depthTarget == nullptr)) {
    Thandor_Log("SDL_GPU renderer: render targets %ux%u failed: %s", width, height, SDL_GetError());
    return false;
  }
  s_gpu.targetWidth = width;
  s_gpu.targetHeight = height;
  return true;
}

/* Grows a buffer to at least byteCount (doubling), true when it is usable. */
bool EnsureVertexBuffer(Uint32 byteCount) noexcept
{
  if ((s_gpu.vertexBuffer != nullptr) && (s_gpu.vertexBufferBytes >= byteCount)) {
    return true;
  }
  SDL_ReleaseGPUBuffer(s_gpu.device, s_gpu.vertexBuffer);
  Uint32 size = std::max<Uint32>(s_gpu.vertexBufferBytes * 2, 1u << 20);
  while (size < byteCount) {
    size *= 2;
  }
  SDL_GPUBufferCreateInfo info;
  SDL_zero(info);
  info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  info.size = size;
  s_gpu.vertexBuffer = SDL_CreateGPUBuffer(s_gpu.device, &info);
  s_gpu.vertexBufferBytes = (s_gpu.vertexBuffer != nullptr) ? size : 0;
  return s_gpu.vertexBuffer != nullptr;
}

bool EnsureTransferBuffer(SDL_GPUTransferBuffer *&buffer, Uint32 &bufferBytes, SDL_GPUTransferBufferUsage usage,
                          Uint32 byteCount) noexcept
{
  if ((buffer != nullptr) && (bufferBytes >= byteCount)) {
    return true;
  }
  SDL_ReleaseGPUTransferBuffer(s_gpu.device, buffer);
  Uint32 size = std::max<Uint32>(bufferBytes * 2, 1u << 20);
  while (size < byteCount) {
    size *= 2;
  }
  SDL_GPUTransferBufferCreateInfo info;
  SDL_zero(info);
  info.usage = usage;
  info.size = size;
  buffer = SDL_CreateGPUTransferBuffer(s_gpu.device, &info);
  bufferBytes = (buffer != nullptr) ? size : 0;
  return buffer != nullptr;
}

/* --- drawing a scene --------------------------------------------------------------------------------------- */

/* Uploads, draws and downloads the collected scene into s_gpu.gpuPixels (clip rectangle, B8G8R8A8 rows).
   False when nothing could be drawn. */
bool RenderScene() noexcept
{
  const SDL_Rect clip = s_gpu.sceneClip;
  const Uint32 vertexBytes = static_cast<Uint32>(s_gpu.vertices.size() * sizeof(GpuVertex));
  const Uint32 textureBytes = static_cast<Uint32>(s_gpu.staging.size() * sizeof(uint32_t));
  const Uint32 downloadRowPixels = (static_cast<Uint32>(clip.w) + kUploadPitchPixels - 1) / kUploadPitchPixels *
                                   kUploadPitchPixels;
  const Uint32 downloadBytes = downloadRowPixels * static_cast<Uint32>(clip.h) * 4;
  if ((clip.w <= 0) || (clip.h <= 0) ||
      !EnsureTransferBuffer(s_gpu.uploadBuffer, s_gpu.uploadBufferBytes, SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                            vertexBytes + textureBytes + 4) ||
      !EnsureTransferBuffer(s_gpu.downloadBuffer, s_gpu.downloadBufferBytes, SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD,
                            downloadBytes) ||
      ((vertexBytes != 0) && !EnsureVertexBuffer(vertexBytes))) {
    return false;
  }
  auto *mapped = static_cast<uint8_t *>(SDL_MapGPUTransferBuffer(s_gpu.device, s_gpu.uploadBuffer, true));
  if (mapped == nullptr) {
    return false;
  }
  /* textures first, so their offsets keep the staging alignment; the vertices after them */
  if (textureBytes != 0) {
    std::memcpy(mapped, s_gpu.staging.data(), textureBytes);
  }
  if (vertexBytes != 0) {
    std::memcpy(mapped + textureBytes, s_gpu.vertices.data(), vertexBytes);
  }
  SDL_UnmapGPUTransferBuffer(s_gpu.device, s_gpu.uploadBuffer);

  SDL_GPUCommandBuffer *commands = SDL_AcquireGPUCommandBuffer(s_gpu.device);
  if (commands == nullptr) {
    return false;
  }
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commands);
  if (vertexBytes != 0) {
    SDL_GPUTransferBufferLocation location{s_gpu.uploadBuffer, textureBytes};
    SDL_GPUBufferRegion region{s_gpu.vertexBuffer, 0, vertexBytes};
    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);
  }
  for (const PendingUpload &upload : s_gpu.uploads) {
    SDL_GPUTextureTransferInfo source;
    SDL_zero(source);
    source.transfer_buffer = s_gpu.uploadBuffer;
    source.offset = static_cast<Uint32>(upload.stagingOffset * sizeof(uint32_t));
    source.pixels_per_row = upload.rowPixels;
    source.rows_per_layer = upload.height;
    SDL_GPUTextureRegion destination;
    SDL_zero(destination);
    destination.texture = s_gpu.atlas;
    destination.x = upload.x;
    destination.y = upload.y;
    destination.w = upload.width;
    destination.h = upload.height;
    destination.d = 1;
    SDL_UploadToGPUTexture(copyPass, &source, &destination, false);
  }
  SDL_EndGPUCopyPass(copyPass);

  SDL_GPUColorTargetInfo colorTarget;
  SDL_zero(colorTarget);
  colorTarget.texture = s_gpu.colorTarget;
  colorTarget.clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f};
  colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTarget.store_op = SDL_GPU_STOREOP_STORE;
  SDL_GPUDepthStencilTargetInfo depthTarget;
  SDL_zero(depthTarget);
  depthTarget.texture = s_gpu.depthTarget;
  depthTarget.clear_depth = 1.0f;
  depthTarget.load_op = SDL_GPU_LOADOP_CLEAR;
  depthTarget.store_op = SDL_GPU_STOREOP_DONT_CARE;
  depthTarget.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
  depthTarget.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
  SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commands, &colorTarget, 1, &depthTarget);
  if (vertexBytes != 0) {
    SDL_GPUBufferBinding binding{s_gpu.vertexBuffer, 0};
    SDL_BindGPUVertexBuffers(renderPass, 0, &binding, 1);
    SDL_GPUTextureSamplerBinding atlasBinding{s_gpu.atlas, s_gpu.sampler};
    SDL_GPUGraphicsPipeline *boundPipeline = nullptr;
    auto bind = [&](SDL_GPUGraphicsPipeline *pipeline) {
      if (pipeline != boundPipeline) {
        SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
        SDL_BindGPUFragmentSamplers(renderPass, 0, &atlasBinding, 1);
        boundPipeline = pipeline;
      }
    };
    for (const GpuRun &run : s_gpu.runs) {
      SDL_SetGPUScissor(renderPass, &run.scissor);
      switch (run.kind) {
      case GPU_RUN_OPAQUE:
        bind(s_gpu.pipelines[GPU_PIPELINE_OPAQUE]);
        break;
      case GPU_RUN_ADDITIVE:
        bind(s_gpu.pipelines[GPU_PIPELINE_ADDITIVE]);
        break;
      default:
        bind(s_gpu.pipelines[GPU_PIPELINE_ALPHA]);
        break;
      }
      SDL_DrawGPUPrimitives(renderPass, run.vertexCount, 1, run.firstVertex, 0);
      if (run.kind == GPU_RUN_ALPHA_WRITES_DEPTH) {
        bind(s_gpu.pipelines[GPU_PIPELINE_ALPHA_DEPTH]);
        SDL_DrawGPUPrimitives(renderPass, run.vertexCount, 1, run.firstVertex, 0);
      }
    }
  }
  SDL_EndGPURenderPass(renderPass);

  copyPass = SDL_BeginGPUCopyPass(commands);
  SDL_GPUTextureRegion source;
  SDL_zero(source);
  source.texture = s_gpu.colorTarget;
  source.x = static_cast<Uint32>(clip.x);
  source.y = static_cast<Uint32>(clip.y);
  source.w = static_cast<Uint32>(clip.w);
  source.h = static_cast<Uint32>(clip.h);
  source.d = 1;
  SDL_GPUTextureTransferInfo destination;
  SDL_zero(destination);
  destination.transfer_buffer = s_gpu.downloadBuffer;
  destination.pixels_per_row = downloadRowPixels;
  destination.rows_per_layer = static_cast<Uint32>(clip.h);
  SDL_DownloadFromGPUTexture(copyPass, &source, &destination);
  SDL_EndGPUCopyPass(copyPass);

  SDL_GPUFence *fence = SDL_SubmitGPUCommandBufferAndAcquireFence(commands);
  if (fence == nullptr) {
    return false;
  }
  SDL_WaitForGPUFences(s_gpu.device, true, &fence, 1);
  SDL_ReleaseGPUFence(s_gpu.device, fence);

  const auto *pixels = static_cast<const uint32_t *>(SDL_MapGPUTransferBuffer(s_gpu.device, s_gpu.downloadBuffer, false));
  if (pixels == nullptr) {
    return false;
  }
  s_gpu.gpuPixels.resize(static_cast<size_t>(clip.w) * clip.h);
  for (int row = 0; row < clip.h; row++) {
    std::memcpy(s_gpu.gpuPixels.data() + static_cast<size_t>(row) * clip.w, pixels + static_cast<size_t>(row) * downloadRowPixels,
                static_cast<size_t>(clip.w) * sizeof(uint32_t));
  }
  SDL_UnmapGPUTransferBuffer(s_gpu.device, s_gpu.downloadBuffer);
  return true;
}

/* Packs one 8-bit channel into the software framebuffer format (top bits, as Raster_Pack16 quantizes). */
inline uint32_t PackChannel(uint32_t value, GraphicsPixelChannelBitShift shift, GraphicsPixelChannelBitCount bits) noexcept
{
  return (bits >= 8) ? (value << shift) : ((value >> (8 - bits)) << shift);
}

/* Writes the downloaded clip rectangle into the software framebuffer. */
void WriteSceneToFramebuffer() noexcept
{
  const SDL_Rect clip = s_gpu.sceneClip;
  SoftwareFramebufferAccess *framebuffer = g_FramebufferAccess;
  if ((framebuffer == nullptr) || (framebuffer->pixels == nullptr) || s_gpu.gpuPixels.empty()) {
    return;
  }
  const SoftwarePixelFormatConfig &format = g_SoftwarePixelFormatConfig;
  const bool sixteenBit = (framebuffer->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT);
  for (int row = 0; row < clip.h; row++) {
    const uint32_t *source = s_gpu.gpuPixels.data() + static_cast<size_t>(row) * clip.w;
    uint8_t *destinationRow = framebuffer->pixels + static_cast<size_t>(clip.y + row) * g_FramebufferRowStrideBytes;
    if (!sixteenBit) {
      std::memcpy(destinationRow + static_cast<size_t>(clip.x) * 4, source, static_cast<size_t>(clip.w) * 4);
      continue;
    }
    auto *destination = reinterpret_cast<uint16_t *>(destinationRow) + clip.x;
    for (int column = 0; column < clip.w; column++) {
      const uint32_t argb = source[column];
      destination[column] = static_cast<uint16_t>(PackChannel((argb >> 16) & 0xFF, format.redShift, format.redBitCount) |
                                                  PackChannel((argb >> 8) & 0xFF, format.greenShift, format.greenBitCount) |
                                                  PackChannel(argb & 0xFF, format.blueShift, format.blueBitCount));
    }
  }
}

#ifdef THANDOR_DEV_TOOLS
/* Compare mode: the clip rectangle of the framebuffer (software picture) as ARGB rows. */
std::vector<uint32_t> ReadFramebufferRegion(const SDL_Rect &clip) noexcept
{
  std::vector<uint32_t> result(static_cast<size_t>(clip.w) * clip.h);
  SoftwareFramebufferAccess *framebuffer = g_FramebufferAccess;
  const SoftwarePixelFormatConfig &format = g_SoftwarePixelFormatConfig;
  const bool sixteenBit = (framebuffer->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT);
  for (int row = 0; row < clip.h; row++) {
    const uint8_t *sourceRow = framebuffer->pixels + static_cast<size_t>(clip.y + row) * g_FramebufferRowStrideBytes;
    for (int column = 0; column < clip.w; column++) {
      uint32_t argb;
      if (sixteenBit) {
        uint16_t packed;
        std::memcpy(&packed, sourceRow + (clip.x + column) * 2, 2);
        argb = (uint32_t{GraphicsFramebuffer_ExpandChannelTo8Bit(packed, format.redMask, format.redShift,
                                                                 format.redBitCount)} << 16) |
               (uint32_t{GraphicsFramebuffer_ExpandChannelTo8Bit(packed, format.greenMask, format.greenShift,
                                                                 format.greenBitCount)} << 8) |
               GraphicsFramebuffer_ExpandChannelTo8Bit(packed, format.blueMask, format.blueShift, format.blueBitCount);
      }
      else {
        std::memcpy(&argb, sourceRow + (clip.x + column) * 4, 4);
      }
      result[static_cast<size_t>(row) * clip.w + column] = argb;
    }
  }
  return result;
}

void WriteBmp(const char *path, const uint32_t *pixels, int width, int height) noexcept
{
  FILE *file = std::fopen(path, "wb");
  if (file == nullptr) {
    return;
  }
  const int rowBytes = (width * 3 + 3) & ~3;
  const uint32_t imageBytes = static_cast<uint32_t>(rowBytes * height);
  uint8_t header[54] = {'B', 'M'};
  auto put32 = [&](int offset, uint32_t value) { std::memcpy(header + offset, &value, 4); };
  put32(2, 54 + imageBytes);
  put32(10, 54);
  put32(14, 40);
  put32(18, static_cast<uint32_t>(width));
  put32(22, static_cast<uint32_t>(height));
  header[26] = 1;
  header[28] = 24;
  put32(34, imageBytes);
  std::fwrite(header, 1, sizeof header, file);
  std::vector<uint8_t> row(static_cast<size_t>(rowBytes), 0);
  for (int y = height - 1; y >= 0; y--) {
    for (int x = 0; x < width; x++) {
      const uint32_t argb = pixels[static_cast<size_t>(y) * width + x];
      row[x * 3 + 0] = static_cast<uint8_t>(argb);
      row[x * 3 + 1] = static_cast<uint8_t>(argb >> 8);
      row[x * 3 + 2] = static_cast<uint8_t>(argb >> 16);
    }
    std::fwrite(row.data(), 1, row.size(), file);
  }
  std::fclose(file);
}

/* Compare mode: every compareIntervalMs writes the software and GPU pictures of this scene and their difference
   (per pixel the largest channel difference, x4, grey) and logs the statistics. */
void CompareScene() noexcept
{
  const unsigned now = Thandor_TickCount();
  if ((now - s_gpu.lastCompareTick < s_gpu.compareIntervalMs) || s_gpu.gpuPixels.empty()) {
    return;
  }
  s_gpu.lastCompareTick = now;
  const SDL_Rect clip = s_gpu.sceneClip;
  std::vector<uint32_t> software = ReadFramebufferRegion(clip);
  std::vector<uint32_t> difference(software.size());
  uint64_t sum = 0;
  uint32_t over8 = 0;
  uint32_t over32 = 0;
  for (size_t i = 0; i < software.size(); i++) {
    int largest = 0;
    for (int shift = 0; shift < 24; shift += 8) {
      const int delta = std::abs(static_cast<int>((software[i] >> shift) & 0xFF) -
                                 static_cast<int>((s_gpu.gpuPixels[i] >> shift) & 0xFF));
      largest = std::max(largest, delta);
      sum += static_cast<uint64_t>(delta);
    }
    over8 += (largest > 8) ? 1 : 0;
    over32 += (largest > 32) ? 1 : 0;
    const uint32_t grey = static_cast<uint32_t>(std::min(255, largest * 4));
    difference[i] = (grey << 16) | (grey << 8) | grey;
  }
  const double pixelCount = static_cast<double>(software.size());
  CreateDirectoryA((LPCSTR)"shots", nullptr);
  char path[64];
  std::snprintf(path, sizeof path, "shots\\gpucmp_%04u_sw.bmp", s_gpu.compareNumber);
  WriteBmp(path, software.data(), clip.w, clip.h);
  std::snprintf(path, sizeof path, "shots\\gpucmp_%04u_gpu.bmp", s_gpu.compareNumber);
  WriteBmp(path, s_gpu.gpuPixels.data(), clip.w, clip.h);
  std::snprintf(path, sizeof path, "shots\\gpucmp_%04u_diff.bmp", s_gpu.compareNumber);
  WriteBmp(path, difference.data(), clip.w, clip.h);
  Thandor_Log("SDL_GPU compare %04u: %dx%d, mean channel difference %.2f, pixels > 8: %.2f%%, > 32: %.2f%%",
              s_gpu.compareNumber, clip.w, clip.h, static_cast<double>(sum) / (pixelCount * 3.0),
              100.0 * over8 / pixelCount, 100.0 * over32 / pixelCount);
  s_gpu.compareNumber++;
}
#endif

/* The rasterization of a starting device: OPEN_THANDOR_GPU_RASTER=smooth|exact, else the ini key [graphics]
   gpu_rasterization (PERSISTENT_SETTING_GPU_RASTERIZATION, smooth by default); compare mode measures the
   difference to the software picture and takes the exact one unless the variable asks for smooth. */
GpuRasterization ChooseRasterization(bool compare) noexcept
{
  if (const char *value = SDL_getenv("OPEN_THANDOR_GPU_RASTER")) {
    if (SDL_strcasecmp(value, "exact") == 0) {
      return GPU_RASTERIZATION_EXACT;
    }
    if (SDL_strcasecmp(value, "smooth") == 0) {
      return GPU_RASTERIZATION_SMOOTH;
    }
    Thandor_Log("SDL_GPU renderer: OPEN_THANDOR_GPU_RASTER=%s ignored (smooth or exact)", value);
  }
  if (compare) {
    return GPU_RASTERIZATION_EXACT;
  }
  return (PersistentSettings_Read(PERSISTENT_GPU_RASTERIZATION_SMOOTH, PERSISTENT_SETTING_GPU_RASTERIZATION) ==
          PERSISTENT_GPU_RASTERIZATION_EXACT)
             ? GPU_RASTERIZATION_EXACT
             : GPU_RASTERIZATION_SMOOTH;
}

const char *RasterizationName(GpuRasterization rasterization) noexcept
{
  return (rasterization == GPU_RASTERIZATION_EXACT) ? "software-exact" : "smooth";
}

double TicksToMs(uint64_t ticks) noexcept
{
  return static_cast<double>(ticks) * 1000.0 / static_cast<double>(SDL_GetPerformanceFrequency());
}

/* Every 10 s: average per scene of the collection (queue copy, texture hashing/conversion), the GPU submit +
   download wait + framebuffer write and, in compare mode, the software rasterizer. */
void LogStatistics() noexcept
{
  const uint64_t now = SDL_GetPerformanceCounter();
  if (s_gpu.statsStart == 0) {
    s_gpu.statsStart = now;
    return;
  }
  if ((TicksToMs(now - s_gpu.statsStart) < 10000.0) || (s_gpu.times.scenes == 0)) {
    return;
  }
  const double scenes = s_gpu.times.scenes;
  char software[48] = "";
  if (s_gpu.mode == GPU_MODE_COMPARE) {
    std::snprintf(software, sizeof software, ", software %.2f ms", TicksToMs(s_gpu.times.software) / scenes);
  }
  Thandor_Log("SDL_GPU renderer (%s, %s): %u scenes in %.1f s, per scene: collect %.2f ms, GPU %.2f ms%s, %.0f GPU "
              "triangles, %.0f runs, %.0f texels converted",
              SDL_GetGPUDeviceDriver(s_gpu.device), RasterizationName(s_gpu.rasterization), s_gpu.times.scenes, TicksToMs(now - s_gpu.statsStart) / 1000.0, TicksToMs(s_gpu.times.collect) / scenes,
              TicksToMs(s_gpu.times.submit) / scenes, software, s_gpu.times.vertices / scenes / 3.0,
              s_gpu.times.runs / scenes, s_gpu.times.uploadedTexels / scenes);
  s_gpu.times = GpuTimes{};
  s_gpu.statsStart = now;
}

/* --- hooks ------------------------------------------------------------------------------------------------- */

void GpuRenderer_SetViewportAndClearDepth(GraphicsScreenCoordinate clipMaxY, GraphicsScreenCoordinate clipMaxX,
                                          GraphicsScreenCoordinate clipMinY, GraphicsScreenCoordinate clipMinX)
{
  SoftwareRenderer_ClearViewport(clipMaxY, clipMaxX, clipMinY, clipMinX);
  s_gpu.sceneOpen = EnsureTargets();
  s_gpu.sceneSerial++;
  /* entries of released texture sets stay in the atlas: start it over between scenes once it is three quarters
     full, so it rarely runs full inside a scene (then the packets drawn before see overwritten texels) */
  if (s_gpu.shelvesBottom > kAtlasSize / 4 * 3) {
    ResetAtlas();
  }
  s_gpu.vertices.clear();
  s_gpu.runs.clear();
  s_gpu.runBounds.clear();
  const int minX = std::clamp(static_cast<int>(clipMinX), 0, static_cast<int>(s_gpu.targetWidth));
  const int minY = std::clamp(static_cast<int>(clipMinY), 0, static_cast<int>(s_gpu.targetHeight));
  const int maxX = std::clamp(static_cast<int>(clipMaxX), minX, static_cast<int>(s_gpu.targetWidth));
  const int maxY = std::clamp(static_cast<int>(clipMaxY), minY, static_cast<int>(s_gpu.targetHeight));
  s_gpu.sceneClip = SDL_Rect{minX, minY, maxX - minX, maxY - minY};
}

void GpuRenderer_DrawPrimitiveQueue(GraphicsScreenCoordinate clipMaxY, GraphicsScreenCoordinate clipMaxX,
                                    GraphicsScreenCoordinate clipMinY, GraphicsScreenCoordinate clipMinX,
                                    GraphicsPrimitiveQueue *queue)
{
  if (!s_gpu.sceneOpen) {
    SoftwareRenderer_DrawPrimitiveQueueBridge(clipMaxY, clipMaxX, clipMinY, clipMinX, queue);
    return;
  }
  const uint64_t start = SDL_GetPerformanceCounter();
  const int minX = std::clamp(static_cast<int>(clipMinX), 0, static_cast<int>(s_gpu.targetWidth));
  const int minY = std::clamp(static_cast<int>(clipMinY), 0, static_cast<int>(s_gpu.targetHeight));
  const int maxX = std::clamp(static_cast<int>(clipMaxX), minX, static_cast<int>(s_gpu.targetWidth));
  const int maxY = std::clamp(static_cast<int>(clipMaxY), minY, static_cast<int>(s_gpu.targetHeight));
  const SDL_Rect scissor{minX, minY, maxX - minX, maxY - minY};
  if ((scissor.w > 0) && (scissor.h > 0)) {
    /* walk the sorted nodes without moving the queue's own cursor, so the software renderer can still walk it */
    GraphicsPrimitiveQueueNode *const cursor = queue->traversalCursor;
    for (GraphicsPrimitivePacket *packet = GraphicsPrimitiveQueue_Begin(queue); packet != nullptr;
         packet = GraphicsPrimitiveQueue_Next(queue)) {
      AppendPacket(packet, scissor);
    }
    queue->traversalCursor = cursor;
  }
  const uint64_t collected = SDL_GetPerformanceCounter();
  s_gpu.times.collect += collected - start;
  if (s_gpu.mode == GPU_MODE_COMPARE) {
    SoftwareRenderer_DrawPrimitiveQueueBridge(clipMaxY, clipMaxX, clipMinY, clipMinX, queue);
    s_gpu.times.software += SDL_GetPerformanceCounter() - collected;
    return;
  }
  g_PrimitiveDrawCallCount += GraphicsPrimitiveQueue_GetCount(queue);
}

void GpuRenderer_EndScene(void)
{
  if (!s_gpu.sceneOpen) {
    return;
  }
  s_gpu.sceneOpen = false;
  const uint64_t start = SDL_GetPerformanceCounter();
  s_gpu.times.scenes++;
  s_gpu.times.vertices += static_cast<uint32_t>(s_gpu.vertices.size());
  s_gpu.times.runs += static_cast<uint32_t>(s_gpu.runs.size());
  if (!s_gpu.vertices.empty() || !s_gpu.uploads.empty()) {
    const bool rendered = RenderScene();
    s_gpu.uploads.clear();
    s_gpu.staging.clear();
    if (!rendered && !s_gpu.renderFailureLogged) {
      Thandor_Log("SDL_GPU renderer: drawing a scene failed (%s)", SDL_GetError());
      s_gpu.renderFailureLogged = true;
    }
    if (rendered) {
#ifdef THANDOR_DEV_TOOLS
      if (s_gpu.mode == GPU_MODE_COMPARE) {
        CompareScene();
      }
#endif
      if (s_gpu.mode == GPU_MODE_ON) {
        if (!g_GraphicsFramebufferBeginAccess()) {
          WriteSceneToFramebuffer();
          g_GraphicsFramebufferEndAccess();
        }
      }
    }
  }
  s_gpu.times.submit += SDL_GetPerformanceCounter() - start;
  LogStatistics();
}

/* --- device --------------------------------------------------------------------------------------------- */

/* SDL's driver name and the shader format of a GPU renderer. */
const char *DriverName(uint32_t renderer) noexcept
{
  return (renderer == PERSISTENT_RENDERER_VULKAN) ? "vulkan" : "direct3d12";
}

SDL_GPUShaderFormat ShaderFormatOf(uint32_t renderer) noexcept
{
  return (renderer == PERSISTENT_RENDERER_VULKAN) ? SDL_GPU_SHADERFORMAT_SPIRV : SDL_GPU_SHADERFORMAT_DXBC;
}

/* Device creation properties for a GPU renderer (the caller destroys them). */
SDL_PropertiesID DeviceProperties(uint32_t renderer) noexcept
{
  const SDL_PropertiesID properties = SDL_CreateProperties();
  SDL_SetStringProperty(properties, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, DriverName(renderer));
  SDL_SetBooleanProperty(properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN,
                         ShaderFormatOf(renderer) == SDL_GPU_SHADERFORMAT_SPIRV);
  SDL_SetBooleanProperty(properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXBC_BOOLEAN,
                         ShaderFormatOf(renderer) == SDL_GPU_SHADERFORMAT_DXBC);
  SDL_SetBooleanProperty(properties, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, false);
  return properties;
}

void ReleaseDevice() noexcept
{
  if (s_gpu.device == nullptr) {
    return;
  }
  /* nothing may still use the textures and buffers released below */
  SDL_WaitForGPUIdle(s_gpu.device);
  for (SDL_GPUGraphicsPipeline *&pipeline : s_gpu.pipelines) {
    SDL_ReleaseGPUGraphicsPipeline(s_gpu.device, pipeline);
    pipeline = nullptr;
  }
  SDL_ReleaseGPUSampler(s_gpu.device, s_gpu.sampler);
  SDL_ReleaseGPUTexture(s_gpu.device, s_gpu.atlas);
  SDL_ReleaseGPUTexture(s_gpu.device, s_gpu.colorTarget);
  SDL_ReleaseGPUTexture(s_gpu.device, s_gpu.depthTarget);
  SDL_ReleaseGPUTexture(s_gpu.device, s_gpu.frameTexture);
  SDL_ReleaseGPUBuffer(s_gpu.device, s_gpu.vertexBuffer);
  SDL_ReleaseGPUTransferBuffer(s_gpu.device, s_gpu.uploadBuffer);
  SDL_ReleaseGPUTransferBuffer(s_gpu.device, s_gpu.downloadBuffer);
  SDL_ReleaseGPUTransferBuffer(s_gpu.device, s_gpu.frameUpload);
  if (s_gpu.window != nullptr) {
    SDL_ReleaseWindowFromGPUDevice(s_gpu.device, s_gpu.window);
  }
  SDL_DestroyGPUDevice(s_gpu.device);
  s_gpu = GpuState{};
}

/* The swapchain's present mode: mailbox (no tearing, never waits) where the driver has it, else vsync; the
   swapchain texture is acquired without waiting, so a frame is dropped rather than the game held up. */
void ChoosePresentMode() noexcept
{
  SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;
  if (SDL_WindowSupportsGPUPresentMode(s_gpu.device, s_gpu.window, SDL_GPU_PRESENTMODE_MAILBOX)) {
    presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
  }
  SDL_SetGPUSwapchainParameters(s_gpu.device, s_gpu.window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode);
}

/* The framebuffer texture in the framebuffer's size and format: B5G6R5 for RGB565 (same bit layout) when the
   device can sample it, else B8G8R8A8 (RGB565 is widened on the CPU); B8G8R8A8 for XRGB8888. */
bool EnsureFrameTexture(Uint32 width, Uint32 height, bool sixteenBit) noexcept
{
  SDL_GPUTextureFormat format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
  if (sixteenBit && SDL_GPUTextureSupportsFormat(s_gpu.device, SDL_GPU_TEXTUREFORMAT_B5G6R5_UNORM,
                                                 SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_SAMPLER)) {
    format = SDL_GPU_TEXTUREFORMAT_B5G6R5_UNORM;
  }
  if ((s_gpu.frameTexture != nullptr) && (s_gpu.frameWidth == width) && (s_gpu.frameHeight == height) &&
      (s_gpu.frameFormat == format)) {
    return true;
  }
  SDL_ReleaseGPUTexture(s_gpu.device, s_gpu.frameTexture);
  s_gpu.frameTexture = CreateTexture(format, SDL_GPU_TEXTUREUSAGE_SAMPLER, width, height);
  s_gpu.frameFormat = format;
  s_gpu.frameWidth = width;
  s_gpu.frameHeight = height;
  return s_gpu.frameTexture != nullptr;
}

} // namespace

namespace thandor::sdl3 {

bool GpuRendererSupported(uint32_t renderer) noexcept
{
#ifndef THANDOR_GPU_SHADERS_SPIRV
  if (renderer == PERSISTENT_RENDERER_VULKAN) {
    return false; /* built without dxc: no SPIR-V shaders */
  }
#endif
  const SDL_PropertiesID properties = DeviceProperties(renderer);
  const bool supported = SDL_GPUSupportsProperties(properties);
  SDL_DestroyProperties(properties);
  return supported;
}

bool StartGpuDevice(uint32_t renderer, SDL_Window *window, bool compare) noexcept
{
  ReleaseDevice();
  const SDL_PropertiesID properties = DeviceProperties(renderer);
  s_gpu.device = SDL_CreateGPUDeviceWithProperties(properties);
  SDL_DestroyProperties(properties);
  if (s_gpu.device == nullptr) {
    Thandor_Log("SDL_GPU: no %s device (%s)", DriverName(renderer), SDL_GetError());
    return false;
  }
  s_gpu.shaderFormat = ShaderFormatOf(renderer);
  if (!SDL_ClaimWindowForGPUDevice(s_gpu.device, window)) {
    Thandor_Log("SDL_GPU: %s cannot present to the window (%s)", DriverName(renderer), SDL_GetError());
    ReleaseDevice();
    return false;
  }
  s_gpu.window = window;
  ChoosePresentMode();
  SDL_GPUSamplerCreateInfo samplerInfo;
  SDL_zero(samplerInfo);
  samplerInfo.min_filter = SDL_GPU_FILTER_NEAREST;
  samplerInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
  samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
  samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  s_gpu.sampler = SDL_CreateGPUSampler(s_gpu.device, &samplerInfo);
  s_gpu.atlas = CreateTexture(SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER, kAtlasSize,
                              kAtlasSize);
  if ((s_gpu.sampler == nullptr) || (s_gpu.atlas == nullptr) || !CreatePipelines()) {
    Thandor_Log("SDL_GPU: %s setup failed (%s)", DriverName(renderer), SDL_GetError());
    ReleaseDevice();
    return false;
  }
  s_gpu.mode = compare ? GPU_MODE_COMPARE : GPU_MODE_ON;
  s_gpu.rasterization = ChooseRasterization(compare);
#ifdef THANDOR_DEV_TOOLS
  if (const char *interval = SDL_getenv("OPEN_THANDOR_GPU_COMPARE_MS")) {
    s_gpu.compareIntervalMs = static_cast<uint32_t>(std::max(1, SDL_atoi(interval)));
  }
  s_gpu.lastCompareTick = Thandor_TickCount();
#endif
  g_GraphicsSetViewportAndClearDepth = GpuRenderer_SetViewportAndClearDepth;
  g_GraphicsDrawPrimitiveQueue = GpuRenderer_DrawPrimitiveQueue;
  g_GraphicsEndScene = GpuRenderer_EndScene;
  Thandor_Log("SDL_GPU renderer: %s (%s) on %s (%s shaders), presenting through SDL_GPU%s",
              (s_gpu.mode == GPU_MODE_COMPARE) ? "compare mode" : "primitive rasterization",
              RasterizationName(s_gpu.rasterization), SDL_GetGPUDeviceDriver(s_gpu.device),
              (s_gpu.shaderFormat == SDL_GPU_SHADERFORMAT_SPIRV) ? "SPIR-V" : "DXBC",
              (s_gpu.mode == GPU_MODE_COMPARE) ? " (software picture shown, shots\\gpucmp_*)" : "");
  return true;
}

void StopGpuDevice() noexcept
{
  if (s_gpu.device == nullptr) {
    return;
  }
  g_GraphicsSetViewportAndClearDepth = SoftwareRenderer_ClearViewport;
  g_GraphicsDrawPrimitiveQueue = SoftwareRenderer_DrawPrimitiveQueueBridge;
  g_GraphicsEndScene = SoftwareGraphicsDispatch_NoOp;
  ReleaseDevice();
}

bool GpuDeviceRunning() noexcept
{
  return s_gpu.device != nullptr;
}

bool PresentWithGpu(const std::byte *pixels, int pitchBytes, int width, int height, bool sixteenBit) noexcept
{
  if ((s_gpu.device == nullptr) || (width <= 0) || (height <= 0)) {
    return false;
  }
  const auto frameWidth = static_cast<Uint32>(width);
  const auto frameHeight = static_cast<Uint32>(height);
  if (!EnsureFrameTexture(frameWidth, frameHeight, sixteenBit)) {
    if (!s_gpu.presentFailureLogged) {
      Thandor_Log("SDL_GPU: frame texture %dx%d failed: %s", width, height, SDL_GetError());
      s_gpu.presentFailureLogged = true;
    }
    return false;
  }
  const Uint32 texelBytes = (s_gpu.frameFormat == SDL_GPU_TEXTUREFORMAT_B5G6R5_UNORM) ? 2 : 4;
  const Uint32 rowBytes = frameWidth * texelBytes;
  if (!EnsureTransferBuffer(s_gpu.frameUpload, s_gpu.frameUploadBytes, SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                            rowBytes * frameHeight)) {
    return false;
  }
  /* cycle: the previous frame's upload may still be in flight */
  auto *mapped = static_cast<std::byte *>(SDL_MapGPUTransferBuffer(s_gpu.device, s_gpu.frameUpload, true));
  if (mapped == nullptr) {
    return false;
  }
  for (Uint32 row = 0; row < frameHeight; row++) {
    const std::byte *source = pixels + static_cast<size_t>(row) * static_cast<size_t>(pitchBytes);
    std::byte *destination = mapped + static_cast<size_t>(row) * rowBytes;
    if (sixteenBit && (texelBytes == 4)) {
      /* RGB565 widened to B8G8R8A8 (top bits repeated into the low ones) */
      for (Uint32 column = 0; column < frameWidth; column++) {
        uint16_t packed = 0;
        std::memcpy(&packed, source + column * 2, sizeof packed);
        const uint32_t red = (packed >> 11) & 0x1F;
        const uint32_t green = (packed >> 5) & 0x3F;
        const uint32_t blue = packed & 0x1F;
        const uint32_t pixel = 0xFF000000u | (((red << 3) | (red >> 2)) << 16) | (((green << 2) | (green >> 4)) << 8) |
                               ((blue << 3) | (blue >> 2));
        std::memcpy(destination + column * 4, &pixel, sizeof pixel);
      }
    }
    else {
      std::memcpy(destination, source, rowBytes);
    }
  }
  SDL_UnmapGPUTransferBuffer(s_gpu.device, s_gpu.frameUpload);

  SDL_GPUCommandBuffer *commands = SDL_AcquireGPUCommandBuffer(s_gpu.device);
  if (commands == nullptr) {
    return false;
  }
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commands);
  SDL_GPUTextureTransferInfo source;
  SDL_zero(source);
  source.transfer_buffer = s_gpu.frameUpload;
  source.pixels_per_row = frameWidth;
  source.rows_per_layer = frameHeight;
  SDL_GPUTextureRegion destination;
  SDL_zero(destination);
  destination.texture = s_gpu.frameTexture;
  destination.w = frameWidth;
  destination.h = frameHeight;
  destination.d = 1;
  SDL_UploadToGPUTexture(copyPass, &source, &destination, true);
  SDL_EndGPUCopyPass(copyPass);

  SDL_GPUTexture *swapchain = nullptr;
  Uint32 swapchainWidth = 0;
  Uint32 swapchainHeight = 0;
  if (!SDL_AcquireGPUSwapchainTexture(commands, s_gpu.window, &swapchain, &swapchainWidth, &swapchainHeight)) {
    if (!s_gpu.presentFailureLogged) {
      Thandor_Log("SDL_GPU: no swapchain texture (%s)", SDL_GetError());
      s_gpu.presentFailureLogged = true;
    }
  }
  if (swapchain != nullptr) {
    /* letterboxed: the largest rectangle of the framebuffer's aspect, centred, black around it */
    const SDL_FRect box = LetterboxRect(static_cast<float>(swapchainWidth), static_cast<float>(swapchainHeight),
                                        static_cast<float>(frameWidth), static_cast<float>(frameHeight));
    SDL_GPUBlitInfo blit;
    SDL_zero(blit);
    blit.source.texture = s_gpu.frameTexture;
    blit.source.w = frameWidth;
    blit.source.h = frameHeight;
    blit.destination.texture = swapchain;
    blit.destination.x = static_cast<Uint32>(box.x);
    blit.destination.y = static_cast<Uint32>(box.y);
    blit.destination.w = std::max<Uint32>(1, static_cast<Uint32>(box.w));
    blit.destination.h = std::max<Uint32>(1, static_cast<Uint32>(box.h));
    blit.load_op = SDL_GPU_LOADOP_CLEAR;
    blit.clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f};
    blit.filter = SDL_GPU_FILTER_LINEAR;
    SDL_BlitGPUTexture(commands, &blit);
  }
  return SDL_SubmitGPUCommandBuffer(commands);
}

} // namespace thandor::sdl3
