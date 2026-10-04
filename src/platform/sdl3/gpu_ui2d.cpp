/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/gpu_ui2d.cpp
 * Project code (not in the original game)
 */

/* GPU 2D renderer of step 9: pipelines and draws for the 2D draw list (see gpu_ui2d.h and ui2d.hlsl).

   Pipelines (one per GpuUiBlend, fragment entry point Ui2d<Mode>Main, no depth, triangle list, no culling):
     SRC_ALPHA_SKIP0, HALF_RGB, MODULATED, FILL  colour SRC_ALPHA / ONE_MINUS_SRC_ALPHA, alpha ONE /
                                                 ONE_MINUS_SRC_ALPHA (as the 3D renderer's alpha pipeline)
     OPAQUE                                      no blend
   The shaders discard the skipped texels (alpha 0), so the blend only sees visible ones; an alpha of 0xFF blends
   with factor 1 and writes the source exactly, which is the software blits' opaque write (without the brightness
   LUT, see the plan, 5.3).

   Vertices go through vertex uniform slot 0 (4 KiB per chunk, fetched by SV_VertexID), the target scale through
   slot 1: GpuUi2D_Draw runs inside the caller's render pass, where SDL_GPU allows uniform pushes but no copy pass. */

#include "gpu_ui2d.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_stdinc.h>

#include <algorithm>
#include <cstddef>

#include <thandor/platform/bootstrap/image.h>

/* The compiled shaders (CMake: <build>/gpu_shaders): DXBC from fxc, SPIR-V from dxc when it was found. */
namespace thandor::sdl3::gpu_ui2d_shaders {
using BYTE = unsigned char;
#include "gpu_shader_ui2d_vertex.h"
#include "gpu_shader_ui2d_skip0.h"
#include "gpu_shader_ui2d_half_rgb.h"
#include "gpu_shader_ui2d_modulated.h"
#include "gpu_shader_ui2d_opaque.h"
#include "gpu_shader_ui2d_fill.h"
#ifdef THANDOR_GPU_SHADERS_SPIRV
#include "gpu_shader_ui2d_vertex_spirv.h"
#include "gpu_shader_ui2d_skip0_spirv.h"
#include "gpu_shader_ui2d_half_rgb_spirv.h"
#include "gpu_shader_ui2d_modulated_spirv.h"
#include "gpu_shader_ui2d_opaque_spirv.h"
#include "gpu_shader_ui2d_fill_spirv.h"
#endif
} // namespace thandor::sdl3::gpu_ui2d_shaders

namespace {

/* The shader blobs of one entry point: DXBC and SPIR-V (empty without dxc). */
struct ShaderBlobs {
  const unsigned char *dxbc;
  size_t dxbcSize;
  const unsigned char *spirv;
  size_t spirvSize;
};

/* Vertex uniform slot 1 (ui2d.hlsl Ui2dTarget). */
struct TargetUniform {
  float scaleX; /* 2 / width */
  float scaleY; /* 2 / height */
  float unused[2];
};

constexpr uint32_t kChunkBytes = 4096;
static_assert(GPU_UI_VERTICES_PER_CHUNK * sizeof(GpuUiVertex) <= kChunkBytes);
static_assert(GPU_UI_VERTICES_PER_CHUNK % 6 == 0);

struct Ui2dState {
  SDL_GPUDevice *device = nullptr;
  SDL_GPUSampler *sampler = nullptr;
  SDL_GPUGraphicsPipeline *pipelines[GPU_UI_BLEND_COUNT] = {};
};

Ui2dState s_ui2d;

SDL_GPUShader *CreateShader(SDL_GPUDevice *device, SDL_GPUShaderFormat format, const ShaderBlobs &blobs,
                            const char *entryPoint, SDL_GPUShaderStage stage, Uint32 samplerCount,
                            Uint32 uniformBufferCount) noexcept
{
  SDL_GPUShaderCreateInfo info;
  SDL_zero(info);
  if (format == SDL_GPU_SHADERFORMAT_SPIRV) {
    info.code = blobs.spirv;
    info.code_size = blobs.spirvSize;
  }
  else {
    info.code = blobs.dxbc;
    info.code_size = blobs.dxbcSize;
  }
  if (info.code_size == 0) {
    SDL_SetError("no %s shader for %s", (format == SDL_GPU_SHADERFORMAT_SPIRV) ? "SPIR-V" : "DXBC", entryPoint);
    return nullptr;
  }
  info.entrypoint = entryPoint;
  info.format = format;
  info.stage = stage;
  info.num_samplers = samplerCount;
  info.num_uniform_buffers = uniformBufferCount;
  return SDL_CreateGPUShader(device, &info);
}

/* The shader format the device takes (it is created with exactly one of them, see gpu_renderer.cpp). */
SDL_GPUShaderFormat ShaderFormatOf(SDL_GPUDevice *device) noexcept
{
  const SDL_GPUShaderFormat formats = SDL_GetGPUShaderFormats(device);
#ifdef THANDOR_GPU_SHADERS_SPIRV
  if ((formats & SDL_GPU_SHADERFORMAT_SPIRV) != 0) {
    return SDL_GPU_SHADERFORMAT_SPIRV;
  }
#endif
  if ((formats & SDL_GPU_SHADERFORMAT_DXBC) != 0) {
    return SDL_GPU_SHADERFORMAT_DXBC;
  }
  return SDL_GPU_SHADERFORMAT_INVALID;
}

} // namespace

bool GpuUi2D_Init(SDL_GPUDevice *device, SDL_GPUTextureFormat target)
{
  GpuUi2D_Shutdown(s_ui2d.device);
  if (device == nullptr) {
    return false;
  }
  const SDL_GPUShaderFormat format = ShaderFormatOf(device);
  if (format == SDL_GPU_SHADERFORMAT_INVALID) {
    Thandor_Log("GPU 2D: the device takes neither SPIR-V nor DXBC shaders");
    return false;
  }
  s_ui2d.device = device;

  namespace shaders = thandor::sdl3::gpu_ui2d_shaders;
#ifdef THANDOR_GPU_SHADERS_SPIRV
#define THANDOR_SHADER_BLOBS(entry)                                                                                   \
  ShaderBlobs{shaders::g_##entry, sizeof shaders::g_##entry, shaders::g_##entry##Spirv, sizeof shaders::g_##entry##Spirv}
#else
#define THANDOR_SHADER_BLOBS(entry) ShaderBlobs{shaders::g_##entry, sizeof shaders::g_##entry, nullptr, 0}
#endif
  struct FragmentShader {
    ShaderBlobs blobs;
    const char *entryPoint;
    Uint32 samplerCount;
  };
  const FragmentShader fragmentShaders[GPU_UI_BLEND_COUNT] = {
      {THANDOR_SHADER_BLOBS(Ui2dSkip0Main), "Ui2dSkip0Main", 1},
      {THANDOR_SHADER_BLOBS(Ui2dHalfRgbMain), "Ui2dHalfRgbMain", 1},
      {THANDOR_SHADER_BLOBS(Ui2dModulatedMain), "Ui2dModulatedMain", 1},
      {THANDOR_SHADER_BLOBS(Ui2dOpaqueMain), "Ui2dOpaqueMain", 1},
      {THANDOR_SHADER_BLOBS(Ui2dFillMain), "Ui2dFillMain", 0},
  };
  SDL_GPUShader *vertexShader = CreateShader(device, format, THANDOR_SHADER_BLOBS(Ui2dVertexMain), "Ui2dVertexMain",
                                             SDL_GPU_SHADERSTAGE_VERTEX, 0, 2);
#undef THANDOR_SHADER_BLOBS

  SDL_GPUSamplerCreateInfo samplerInfo;
  SDL_zero(samplerInfo);
  samplerInfo.min_filter = SDL_GPU_FILTER_NEAREST;
  samplerInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
  samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
  samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  s_ui2d.sampler = SDL_CreateGPUSampler(device, &samplerInfo);

  bool created = (vertexShader != nullptr) && (s_ui2d.sampler != nullptr);
  for (int blend = 0; created && (blend < GPU_UI_BLEND_COUNT); blend++) {
    const FragmentShader &fragment = fragmentShaders[blend];
    SDL_GPUShader *fragmentShader = CreateShader(device, format, fragment.blobs, fragment.entryPoint,
                                                 SDL_GPU_SHADERSTAGE_FRAGMENT, fragment.samplerCount, 0);
    if (fragmentShader == nullptr) {
      created = false;
      break;
    }
    SDL_GPUColorTargetDescription colorTarget;
    SDL_zero(colorTarget);
    colorTarget.format = target;
    SDL_GPUColorTargetBlendState &blendState = colorTarget.blend_state;
    if (blend != GPU_UI_BLEND_OPAQUE) {
      blendState.enable_blend = true;
      blendState.color_blend_op = SDL_GPU_BLENDOP_ADD;
      blendState.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
      blendState.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
      blendState.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
      blendState.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
      blendState.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    }

    SDL_GPUGraphicsPipelineCreateInfo info;
    SDL_zero(info);
    info.vertex_shader = vertexShader;
    info.fragment_shader = fragmentShader;
    info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    info.target_info.color_target_descriptions = &colorTarget;
    info.target_info.num_color_targets = 1;
    info.target_info.has_depth_stencil_target = false;
    s_ui2d.pipelines[blend] = SDL_CreateGPUGraphicsPipeline(device, &info);
    SDL_ReleaseGPUShader(device, fragmentShader);
    if (s_ui2d.pipelines[blend] == nullptr) {
      created = false;
    }
  }
  SDL_ReleaseGPUShader(device, vertexShader);
  if (!created) {
    Thandor_Log("GPU 2D: setup failed (%s)", SDL_GetError());
    GpuUi2D_Shutdown(device);
    return false;
  }
  return true;
}

void GpuUi2D_Shutdown(SDL_GPUDevice *device)
{
  if ((device == nullptr) || (device != s_ui2d.device)) {
    return;
  }
  for (SDL_GPUGraphicsPipeline *&pipeline : s_ui2d.pipelines) {
    SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
    pipeline = nullptr;
  }
  SDL_ReleaseGPUSampler(device, s_ui2d.sampler);
  s_ui2d.sampler = nullptr;
  s_ui2d.device = nullptr;
}

void GpuUi2D_Draw(SDL_GPURenderPass *renderPass, SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *page,
                  const GpuUiVertex *vertices, uint32_t count, SDL_Rect scissor, uint8_t blend, int targetW,
                  int targetH)
{
  if ((s_ui2d.device == nullptr) || (renderPass == nullptr) || (commandBuffer == nullptr) || (vertices == nullptr) ||
      (blend >= GPU_UI_BLEND_COUNT) || ((page == nullptr) && (blend != GPU_UI_BLEND_FILL)) || (targetW <= 0) ||
      (targetH <= 0)) {
    return;
  }
  count -= count % 3;
  /* the scissor clamped to the target; an empty one draws nothing */
  const int left = std::max(scissor.x, 0);
  const int top = std::max(scissor.y, 0);
  const int right = std::min(scissor.x + scissor.w, targetW);
  const int bottom = std::min(scissor.y + scissor.h, targetH);
  if ((count == 0) || (right <= left) || (bottom <= top)) {
    return;
  }

  SDL_GPUViewport viewport;
  SDL_zero(viewport);
  viewport.w = (float)targetW;
  viewport.h = (float)targetH;
  viewport.max_depth = 1.0f;
  SDL_SetGPUViewport(renderPass, &viewport);
  const SDL_Rect clipped{left, top, right - left, bottom - top};
  SDL_SetGPUScissor(renderPass, &clipped);
  SDL_BindGPUGraphicsPipeline(renderPass, s_ui2d.pipelines[blend]);
  if (blend != GPU_UI_BLEND_FILL) {
    const SDL_GPUTextureSamplerBinding binding{page, s_ui2d.sampler};
    SDL_BindGPUFragmentSamplers(renderPass, 0, &binding, 1);
  }
  const TargetUniform targetUniform{2.0f / (float)targetW, 2.0f / (float)targetH, {0.0f, 0.0f}};
  SDL_PushGPUVertexUniformData(commandBuffer, 1, &targetUniform, sizeof targetUniform);
  for (uint32_t first = 0; first < count; first += GPU_UI_VERTICES_PER_CHUNK) {
    const uint32_t chunk = std::min(count - first, GPU_UI_VERTICES_PER_CHUNK);
    SDL_PushGPUVertexUniformData(commandBuffer, 0, vertices + first, chunk * (Uint32)sizeof(GpuUiVertex));
    SDL_DrawGPUPrimitives(renderPass, chunk, 1, 0, 0);
  }
}
