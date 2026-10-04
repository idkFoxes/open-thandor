/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/sdl3/sdl_objects.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_SDL3_SDL_OBJECTS_H
#define THANDOR_PLATFORM_SDL3_SDL_OBJECTS_H

/* Shared declarations of the SDL3 backend's source files (src/platform/sdl3): owning handles for SDL objects and
   the functions one file calls in another. Only the backend includes this header (it includes SDL). */

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include <cstddef>
#include <cstdint>
#include <memory>

struct GraphicsTextureSourceAsset;

namespace thandor::sdl3 {

/* Owning handles (RAII) for the SDL objects the backend creates. */
struct WindowDeleter {
  void operator()(SDL_Window *window) const noexcept { SDL_DestroyWindow(window); }
};
struct RendererDeleter {
  void operator()(SDL_Renderer *renderer) const noexcept { SDL_DestroyRenderer(renderer); }
};
struct TextureDeleter {
  void operator()(SDL_Texture *texture) const noexcept { SDL_DestroyTexture(texture); }
};
struct AudioStreamDeleter {
  void operator()(SDL_AudioStream *stream) const noexcept { SDL_DestroyAudioStream(stream); }
};
using WindowPtr = std::unique_ptr<SDL_Window, WindowDeleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, RendererDeleter>;
using TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;
using AudioStreamPtr = std::unique_ptr<SDL_AudioStream, AudioStreamDeleter>;

/* platform.cpp: the main window (nullptr before SdlPlatform_CreateMainWindow and after DestroyMainWindow),
   whether it is the developer tools' window (OPEN_THANDOR_WINDOWED), whether it was created for Vulkan
   (SDL_WINDOW_VULKAN; false when no Vulkan loader exists), and its end. */
SDL_Window *MainWindow() noexcept;
bool Windowed() noexcept;
bool VulkanWindow() noexcept;
void DestroyMainWindow() noexcept;

/* video.cpp: the presentation geometry. LetterboxRect is the largest rectangle of the inner size's aspect
   centred in the outer size; the window <-> framebuffer mappings use it with the window size (in window
   coordinates) and the framebuffer size. AbsoluteMouse: the display mode kind is a normal window, so the
   pointer follows the system mouse position instead of SDL's relative mouse mode. */
SDL_FRect LetterboxRect(float outerWidth, float outerHeight, float innerWidth, float innerHeight) noexcept;
void WindowToFramebuffer(float windowX, float windowY, float &outX, float &outY) noexcept;
void FramebufferToWindow(float x, float y, float &outWindowX, float &outWindowY) noexcept;
bool AbsoluteMouse() noexcept;

/* input.cpp: switches SDL's relative mouse mode on or off after a display mode kind change (AbsoluteMouse). */
void UpdateMouseMode() noexcept;

/* gpu_renderer.cpp (THANDOR_RENDERER_SDL_GPU): the SDL_GPU device of the GPU renderers (renderer is
   PERSISTENT_RENDERER_VULKAN or _DIRECT3D12). GpuRendererSupported probes the driver without creating a device.
   StartGpuDevice creates the device, claims the window for its swapchain and installs the GPU rasterization of the
   primitive queues (compare: the developer tools' compare mode); false (logged, nothing left behind) when any step
   fails. StopGpuDevice puts the software rasterizer back and releases the window and the device.
   Step 9: without compare the whole frame is drawn on the GPU (GpuFrameActive): the 2D draw list records the UI
   (graphics/core/draw2d.h) and PresentGpuFrame draws it with the frame's 3D scenes into the persistent frame target
   (framebuffer size x the UI scale, SetGpuUiScale), then presents that letterboxed with the cursor on top (cursor nullptr: hidden).
   ReadGpuFrame downloads a rectangle of the frame target (the last presented frame without the cursor) as
   0xFFRRGGBB pixels, synchronously (captures); the rectangle is in framebuffer (logical) pixels and a UI scale
   N > 1 is point-sampled back to that size. PresentWithGpu (compare mode) uploads the software framebuffer
   (XRGB8888 rows, pitchBytes apart) and blits it letterboxed into the swapchain. CompareGpuFrame (compare mode,
   called by the present before the cursor is composed; does nothing otherwise) draws the recorded frame into the
   frame target without showing it and every OPEN_THANDOR_GPU_COMPARE_MS compares it with the framebuffer
   (shots\gpucmp_NNNN_*.bmp, statistics in thandor.log). Without a swapchain texture (minimized window, dropped
   mailbox frame) the frame is drawn and submitted without being presented. A window that could not get its
   swapchain yet (Vulkan, minimized at the start) is claimed again once it is not minimized; GpuWindowChanged (the
   pump, on window restore / show / size events) makes that happen at the next present. Fullscreen and size
   switches need nothing from here: SDL recreates a claimed window's swapchain itself. */
struct GpuCursorSprite {
  const GraphicsTextureSourceAsset *asset;
  uint32_t subresource;
  int drawX; /* draw position: the entry origin is added, as the blits do */
  int drawY;
};
bool GpuRendererSupported(uint32_t renderer) noexcept;
bool StartGpuDevice(uint32_t renderer, SDL_Window *window, bool compare) noexcept;
void StopGpuDevice() noexcept;
void GpuWindowChanged() noexcept;
bool GpuDeviceRunning() noexcept;
bool GpuFrameActive() noexcept;
bool PresentGpuFrame(const GpuCursorSprite *cursor) noexcept;
bool ReadGpuFrame(int x, int y, int width, int height, uint32_t *outArgb) noexcept;
bool PresentWithGpu(const std::byte *pixels, int pitchBytes, int width, int height) noexcept;
void CompareGpuFrame() noexcept;
/* Step 9 WP8, UI scaling: the GPU frame target (and the 3D targets) are scale x the framebuffer size, which stays
   the logical UI resolution (layout, hit tests, mouse, captures); the 2D quads and the 3D view are drawn at that
   resolution and the frame is presented letterboxed. Takes effect at the next frame (the targets are made anew);
   clamped to 1..kMaxGpuUiScale. video.cpp sets it at every display mode switch (1 for the software renderer). */
constexpr int kMaxGpuUiScale = 8;
void SetGpuUiScale(int scale) noexcept;
/* VSync of the GPU renderers' swapchain: on = vsync present mode and a waiting swapchain acquire (the frame loop runs
   at the display's refresh rate); off = mailbox, else immediate, and a non-waiting acquire (a frame without a free
   swapchain image is dropped). Applied to a claimed window at once and at every later window claim (also of a
   device started later). video.cpp sets it from the vsync setting (SdlVideo_SetVsync). */
void SetGpuVsync(bool on) noexcept;
/* A frame limit is set (video.cpp, SdlVideo_SetFrameLimit): the swapchain texture is acquired waiting also with
   VSync off, so frames are not dropped at the limited rate. Takes effect at the next acquire. */
void SetGpuFrameLimited(bool limited) noexcept;

/* input.cpp: the event handlers of the pump. */
void HandleKeyDown(const SDL_KeyboardEvent &event);
void HandleKeyUp(const SDL_KeyboardEvent &event);
void HandleTextInput(const SDL_TextInputEvent &event);
void HandleMouseEvent(const SDL_Event &event);
void HandleFocusGained();
void HandleFocusLost();

} // namespace thandor::sdl3

#endif /* THANDOR_PLATFORM_SDL3_SDL_OBJECTS_H */
