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

#include <memory>

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

/* platform.cpp: the main window and its renderer (nullptr before SdlPlatform_CreateMainWindow and after
   DestroyMainWindow), whether it is the developer tools' window instead of the fullscreen one, and their end. */
SDL_Window *MainWindow() noexcept;
SDL_Renderer *MainRenderer() noexcept;
bool Windowed() noexcept;
void DestroyMainWindow() noexcept;

/* input.cpp: the event handlers of the pump. */
void HandleKeyDown(const SDL_KeyboardEvent &event);
void HandleKeyUp(const SDL_KeyboardEvent &event);
void HandleTextInput(const SDL_TextInputEvent &event);
void HandleMouseEvent(const SDL_Event &event);
void HandleFocusGained();
void HandleFocusLost();

} // namespace thandor::sdl3

#endif /* THANDOR_PLATFORM_SDL3_SDL_OBJECTS_H */
