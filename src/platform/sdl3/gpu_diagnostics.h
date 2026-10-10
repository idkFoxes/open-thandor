/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/gpu_diagnostics.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_SDL3_GPU_DIAGNOSTICS_H
#define THANDOR_PLATFORM_SDL3_GPU_DIAGNOSTICS_H

/* Diagnostics of the SDL_GPU renderers (thandor.log): the display adapters and driver versions, the process address
   space around the big GPU resources, every failed SDL_GPU creation call (rate-limited) and the start marker that
   makes the next start fall back to the next renderer when a GPU start crashed (thandor-gpu-start.txt next to
   thandor.ini, removed after the first presented frame). */

#include <SDL3/SDL_gpu.h>

#include <cstdint>

namespace thandor::sdl3 {

/* Logs the free address space of the process (total and largest free block below the user-mode limit, and below
   2 GB) and the committed bytes, with "when" naming the moment. Rate-limited. */
void LogAddressSpace(const char *when) noexcept;

/* Logs the SDL version and the DXGI display adapters (name, ids, dedicated video memory, driver version), once. */
void LogGpuAdapters() noexcept;

/* Logs a failed SDL_GPU call with SDL_GetError() (printf format, rate-limited). */
void LogGpuFailure(const char *format, ...) noexcept;

/* SDL_GPU creation calls that log their failure (LogGpuFailure); "what" names the object. Big textures and buffers
   log the address space before and after. */
SDL_GPUTexture *CreateGpuTextureLogged(SDL_GPUDevice *device, const SDL_GPUTextureCreateInfo *info,
                                       const char *what) noexcept;
SDL_GPUBuffer *CreateGpuBufferLogged(SDL_GPUDevice *device, const SDL_GPUBufferCreateInfo *info,
                                     const char *what) noexcept;
SDL_GPUTransferBuffer *CreateGpuTransferBufferLogged(SDL_GPUDevice *device, const SDL_GPUTransferBufferCreateInfo *info,
                                                     const char *what) noexcept;
SDL_GPUSampler *CreateGpuSamplerLogged(SDL_GPUDevice *device, const SDL_GPUSamplerCreateInfo *info,
                                       const char *what) noexcept;
SDL_GPUShader *CreateGpuShaderLogged(SDL_GPUDevice *device, const SDL_GPUShaderCreateInfo *info,
                                     const char *what) noexcept;
SDL_GPUGraphicsPipeline *CreateGpuPipelineLogged(SDL_GPUDevice *device, const SDL_GPUGraphicsPipelineCreateInfo *info,
                                                 const char *what) noexcept;
void *MapGpuTransferBufferLogged(SDL_GPUDevice *device, SDL_GPUTransferBuffer *buffer, bool cycle,
                                 const char *what) noexcept;
SDL_GPUCommandBuffer *AcquireGpuCommandBufferLogged(SDL_GPUDevice *device, const char *what) noexcept;

/* Start marker. GpuStartMarker_Take reads and deletes a marker left by a start that did not finish (iniPath: the
   settings file's path, UTF-16); true with the renderer (PERSISTENT_RENDERER_*) it names. GpuStartMarker_Write
   writes it before a GPU renderer starts; GpuStartMarker_Clear deletes it (a failed start, the renderer stopped);
   GpuNoteFrameSubmitted deletes it after the first frame presented through the swapchain (or after 120 submitted
   frames without one: a minimized window). */
bool GpuStartMarker_Take(const uint16_t *iniPath, uint32_t *renderer) noexcept;
void GpuStartMarker_Write(const uint16_t *iniPath, uint32_t renderer, const char *name) noexcept;
void GpuStartMarker_Clear() noexcept;
void GpuNoteFrameSubmitted(bool withSwapchain) noexcept;

} // namespace thandor::sdl3

#endif /* THANDOR_PLATFORM_SDL3_GPU_DIAGNOSTICS_H */
