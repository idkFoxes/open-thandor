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

/* The original's Win32_ShutdownAndExit: shuts the game down and ends the process (does not return). */
[[noreturn]] void ShutdownAndExit()
{
  Runtime_Shutdown();
  SdlPlatform_Quit();
  ExitProcess(0);
  std::abort(); /* not reached */
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

Bool8 SdlPlatform_CreateMainWindow(const char *title)
{
  /* the original window procedure swallows WM_SYSKEYDOWN, so Alt+F4 does not close the game */
  SDL_SetHint(SDL_HINT_WINDOWS_CLOSE_ON_ALT_F4, "0");
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    Thandor_Log("SDL_Init failed: %s", SDL_GetError());
    return false;
  }
  s_windowed = DebugHook_Windowed() != 0;
  /* hidden until the first display mode switch has applied the renderer and the display mode kind; a Vulkan
     swapchain needs a window created for Vulkan, which needs the Vulkan loader, so without one the window is
     created without and Vulkan is not offered */
  const SDL_WindowFlags flags = SDL_WINDOW_HIDDEN;
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
    Thandor_Log("test aid: windowed mode, window at %d,%d", x, y);
  }
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
      ShutdownAndExit();
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
    ShutdownAndExit();
  }
}

void SdlPlatform_Quit()
{
  DestroyMainWindow();
  SDL_Quit();
}
