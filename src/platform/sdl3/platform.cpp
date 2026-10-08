/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/platform.cpp
 * Project code (not in the original game)
 */

/* SDL3 backend: the main window and the event pump (g_PlatformPumpEvents); video.cpp presents into the window. The pump does what
   the original's Win32_PumpMessages and MainWindowProc do: the developer tools' pump hook first, then
   keys, characters, mouse and focus changes, and a quit request ends the game. */

#include <thandor/platform/sdl3/sdl_objects.h>
#include <thandor/platform/sdl3/window_icon.h>

#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_version.h>

#include <cstdlib>

#include <thandor/thandor.h>
#include <thandor/version.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

namespace thandor::sdl3 {
namespace {

WindowPtr s_window;
bool s_windowed = false;
bool s_vulkanWindow = false;

/* OPEN_THANDOR_WINDOW_X / _Y (developer tools' windowed mode), 0 when unset. */
int WindowCoordinateFromEnvironment(const char *name) noexcept
{
  const char *value = SDL_getenv(name);
  return (value != nullptr) ? std::atoi(value) : 0;
}

} // namespace

SDL_Window *MainWindow() noexcept
{
  return s_window.get();
}

bool Windowed() noexcept
{
  return s_windowed;
}

bool VulkanWindow() noexcept
{
  return s_vulkanWindow;
}

void DestroyMainWindow() noexcept
{
  s_window.reset();
  g_MainWindow = nullptr;
}

} // namespace thandor::sdl3

using namespace thandor::sdl3;

bool SdlPlatform_CreateMainWindow(const char *title)
{
  /* the original window procedure swallows WM_SYSKEYDOWN, so Alt+F4 does not close the game */
  SDL_SetHint(SDL_HINT_WINDOWS_CLOSE_ON_ALT_F4, "0");
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    Thandor_Log("SDL_Init failed: %s", SDL_GetError());
    return false;
  }
  s_windowed = DebugHook_Windowed() != 0;
  const bool minimized = s_windowed && (DebugHook_WindowMinimized() != 0);
  /* hidden until the first display mode switch has applied the renderer and the display mode kind; a Vulkan
     swapchain needs a window created for Vulkan, which needs the Vulkan loader, so without one the window is
     created without and Vulkan is not offered. The developer tools' minimized window (OPEN_THANDOR_WINDOW_MINIMIZED)
     is shown minimized at once without activation (SDL: SW_SHOWMINNOACTIVE, so the later SDL_ShowWindow does
     nothing) and is never activated when shown; the frame loop, the event pump and the software present do not
     depend on visibility or focus, a GPU present without a swapchain texture is skipped (the GPU renderer claims the
     window again when it is restored, see GpuWindowChanged). */
  if (minimized) {
    SDL_SetHint(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, "0");
  }
  const SDL_WindowFlags flags = minimized ? SDL_WINDOW_MINIMIZED : SDL_WINDOW_HIDDEN;
#ifdef THANDOR_RENDERER_SDL_GPU
  s_window.reset(SDL_CreateWindow(title, GAME_START_DISPLAY_WIDTH, GAME_START_DISPLAY_HEIGHT, flags | SDL_WINDOW_VULKAN));
  s_vulkanWindow = static_cast<bool>(s_window);
  if (!s_window) {
    Thandor_Log("no Vulkan window (%s), Vulkan is not offered", SDL_GetError());
  }
#endif
  if (!s_window) {
    s_window.reset(SDL_CreateWindow(title, GAME_START_DISPLAY_WIDTH, GAME_START_DISPLAY_HEIGHT, flags));
  }
  if (!s_window) {
    Thandor_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    return false;
  }
  if (s_windowed) {
    const int x = WindowCoordinateFromEnvironment("OPEN_THANDOR_WINDOW_X");
    const int y = WindowCoordinateFromEnvironment("OPEN_THANDOR_WINDOW_Y");
    SDL_SetWindowPosition(s_window.get(), x, y);
    Thandor_Log("test aid: windowed mode, window at %d,%d%s", x, y, minimized ? ", minimized" : "");
  }
  Thandor_Log("%s", THANDOR_PRODUCT_VERSION_STRING);
  Thandor_Log("SDL %d.%d.%d%s", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION,
              s_windowed ? ", windowed (test aid)" : "");
  /* the window and taskbar icon: thandor.ico of the game directory, if there is one */
  SetWindowIconFromGameDirectory(s_window.get());
  SDL_StartTextInput(s_window.get());
  /* the window handle for the remaining Win32 users (fatal-error message box, file dialogs) */
  g_MainWindow = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(s_window.get()),
                                                          SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
  return true;
}

void SdlPlatform_InstallTimersAndPump()
{
  g_TimerRegisterPeriodic = SdlTimer_RegisterPeriodic;
  g_TimerUnregisterPeriodic = SdlTimer_UnregisterPeriodic;
  g_PlatformPumpEvents = SdlPlatform_PumpEvents;
}

void SdlPlatform_PumpEvents()
{
  /* developer tools: automatic screenshots and the input script, before the queue is read */
  DebugHook_MessagePump();

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (g_WindowDestroyDepth != 0) {
      Runtime_ShutdownAndExit(nullptr); /* the original's Win32_ShutdownAndExit */
    }
    switch (event.type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
      /* MainWindowProc counts WM_CLOSE/WM_DESTROY; the pump then ends the game */
      g_WindowDestroyDepth++;
      break;
    case SDL_EVENT_KEY_DOWN:
      HandleKeyDown(event.key);
      break;
    case SDL_EVENT_KEY_UP:
      HandleKeyUp(event.key);
      break;
    case SDL_EVENT_TEXT_INPUT:
      HandleTextInput(event.text);
      break;
    case SDL_EVENT_MOUSE_MOTION:
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    case SDL_EVENT_MOUSE_WHEEL:
      HandleMouseEvent(event);
      break;
#ifdef THANDOR_RENDERER_SDL_GPU
    case SDL_EVENT_WINDOW_SHOWN:
    case SDL_EVENT_WINDOW_RESTORED:
    case SDL_EVENT_WINDOW_MAXIMIZED:
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
      /* a GPU renderer still without its swapchain (window minimized at the start) claims the window again */
      GpuWindowChanged();
      break;
#endif
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
      HandleFocusGained();
      break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
      HandleFocusLost();
      break;
    default:
      break;
    }
  }
  if (g_WindowDestroyDepth != 0) {
    Runtime_ShutdownAndExit(nullptr); /* the original's Win32_ShutdownAndExit */
  }
}

void SdlPlatform_Quit()
{
  DestroyMainWindow();
  SDL_Quit();
}
