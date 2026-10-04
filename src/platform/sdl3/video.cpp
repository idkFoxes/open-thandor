/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/video.cpp
 * Project code (not in the original game)
 */

/* SDL3 backend: video. The software renderer draws into a plain memory framebuffer (XRGB8888; the game runs in
   32-bit colour only) that stays published in g_DisplayFramebufferAccess, so the framebuffer access hooks
   are the no-op stubs. A present composes the software cursor into the framebuffer (as GraphicsFramebuffer_Present
   does into the DirectDraw back surface), presents it letterboxed and removes the cursor again. Screen captures
   read the memory framebuffer.

   Renderers: the graphics adapters of the display settings are the renderers - "Vulkan (GPU)", "DirectX 12 (GPU)"
   (only when the driver is available) and "Software (CPU)", each with the same display modes - so the original's
   adapter choice selects the renderer and a display mode switch changes it at run time:
   - Vulkan / DirectX 12: the SDL_GPU device of that API (gpu_renderer.cpp) rasterizes the 3D view and presents
     the frame through its own swapchain, so overlays see the chosen API;
   - Software: the software rasterizer, presented through an SDL_Renderer streaming texture; the SDL_Renderer is
     created with the driver "vulkan", else "direct3d12", "direct3d11", else SDL's choice (logged).
   A GPU renderer that cannot start falls back Vulkan -> DirectX 12 -> Software (logged). The choice is kept in
   PERSISTENT_SETTING_RENDERER (default Vulkan). OPEN_THANDOR_GPU=auto keeps the saved choice (as without the
   variable); OPEN_THANDOR_GPU=0|off|software / -SOFTWARE forces software,
   =vulkan / =d3d12 a GPU API, =1 / -GPU the first available GPU API, =compare (developer tools) the GPU compare
   mode; a forced renderer is the only adapter listed and is not saved.

   Display mode kinds (PERSISTENT_SETTING_DISPLAY_MODE_KIND, chosen on the display settings page): exclusive
   fullscreen in the mode or the closest larger one (default, "Vollbild"), borderless fullscreen over the desktop
   ("Vollbildfenster"), or a normal window in the mode's size ("Fenster"); the frame is letterboxed in all three. The developer tools' window (OPEN_THANDOR_WINDOWED) is
   always a window at OPEN_THANDOR_WINDOW_X/Y. */

#include <thandor/platform/sdl3/sdl_objects.h>

#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <span>
#include <utility>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/system/win32.h>

namespace {

constexpr int kMinimumModeWidth = 640;
constexpr int kMinimumModeHeight = 480;
constexpr uint32_t kNoRenderer = 0xFFFFFFFFu;

/* The framebuffer, its texture (software renderer) and the cursor rectangle drawn into it at the last present. */
struct VideoState {
  std::vector<std::byte> framebuffer;
  thandor::sdl3::TexturePtr texture;
  int width = 0;
  int height = 0;
  int pitchBytes = 0;
  int cursorDrawX = 0;
  int cursorDrawY = 0;
};
VideoState s_video;

/* The renderers listed as adapters, the running one and the display mode kind. */
struct RendererState {
  thandor::sdl3::RendererPtr sdlRenderer; /* the software renderer's presenter */
  uint32_t active = kNoRenderer;          /* PERSISTENT_RENDERER_* */
  uint32_t adapters[PERSISTENT_RENDERER_COUNT] = {};
  uint32_t adapterCount = 0;
  uint32_t forced = kNoRenderer; /* OPEN_THANDOR_GPU / -GPU / -SOFTWARE */
  bool compare = false;
  uint32_t kind = PERSISTENT_DISPLAY_MODE_FULLSCREEN;        /* applied */
  uint32_t pendingKind = PERSISTENT_DISPLAY_MODE_FULLSCREEN; /* applied by the next display mode switch */
  bool kindChosen = false;
  uint32_t windowedChosenKind = PERSISTENT_DISPLAY_MODE_COUNT; /* developer window: the last applied choice */
  bool shown = false;
  int windowWidth = 0; /* the size last given to the normal window */
  int windowHeight = 0;
};
RendererState s_renderer;

uint16_t s_gpuDetailUtf16[] = {'G', 'P', 'U', 0};
uint16_t s_cpuDetailUtf16[] = {'C', 'P', 'U', 0};

const char *RendererName(uint32_t renderer) noexcept
{
  switch (renderer) {
  case PERSISTENT_RENDERER_VULKAN:
    return "Vulkan";
  case PERSISTENT_RENDERER_DIRECT3D12:
    return "DirectX 12";
  default:
    return "Software";
  }
}

const char *DisplayModeKindName(uint32_t kind) noexcept
{
  switch (kind) {
  case PERSISTENT_DISPLAY_MODE_WINDOW:
    return "window";
  case PERSISTENT_DISPLAY_MODE_FULLSCREEN:
    return "exclusive fullscreen";
  default:
    return "borderless fullscreen";
  }
}

/* Stores a channel mask with its shift (lowest set bit) and bit count in g_SoftwarePixelFormatConfig's style. */
void ChannelOfMask(Uint32 mask, GraphicsPackedPixelMask &outMask, GraphicsPixelChannelBitShift &outShift,
                   GraphicsPixelChannelBitCount &outBitCount) noexcept
{
  int shift = 0;
  int highestBit = 31;
  if (mask != 0) {
    while (((mask >> shift) & 1) == 0) {
      shift++;
    }
    while ((mask >> highestBit) == 0) {
      highestBit--;
    }
  }
  outMask = mask;
  outShift = shift;
  outBitCount = (highestBit + 1) - shift;
}

/* The display modes: every distinct fullscreen size of the primary display from 640x480 up to the desktop size
   (640x480 itself always), all in 32 bits per pixel, the same for every adapter (renderer). */
void ListDisplayModes()
{
  const SDL_DisplayID display = SDL_GetPrimaryDisplay();
  const SDL_DisplayMode *desktop = SDL_GetDesktopDisplayMode(display);
  const int desktopWidth = (desktop != nullptr) ? std::max(desktop->w, kMinimumModeWidth) : kMinimumModeWidth;
  const int desktopHeight = (desktop != nullptr) ? std::max(desktop->h, kMinimumModeHeight) : kMinimumModeHeight;
  std::vector<std::pair<int, int>> sizes{{kMinimumModeWidth, kMinimumModeHeight}};
  int modeCount = 0;
  SDL_DisplayMode **modes = SDL_GetFullscreenDisplayModes(display, &modeCount);
  if (modes != nullptr) {
    for (const SDL_DisplayMode *mode : std::span<SDL_DisplayMode *>(modes, static_cast<std::size_t>(modeCount))) {
      if ((mode->w >= kMinimumModeWidth) && (mode->h >= kMinimumModeHeight) && (mode->w <= desktopWidth) &&
          (mode->h <= desktopHeight)) {
        sizes.emplace_back(mode->w, mode->h);
      }
    }
    SDL_free(modes);
  }
  sizes.emplace_back(desktopWidth, desktopHeight);
  std::sort(sizes.begin(), sizes.end());
  sizes.erase(std::unique(sizes.begin(), sizes.end()), sizes.end());
  for (uint32_t adapterIndex = 0; adapterIndex < g_GraphicsAdapterCount; adapterIndex++) {
    for (const auto &[width, height] : sizes) {
      if (g_GraphicsDisplayModeCount >= GRAPHICS_DISPLAY_MODE_CAPACITY) {
        return;
      }
      GraphicsDisplayMode &slot = g_GraphicsDisplayModes[g_GraphicsDisplayModeCount];
      slot.width = width;
      slot.height = height;
      slot.bitsPerPixel = PERSISTENT_DEFAULT_BITS_PER_PIXEL;
      slot.adapterIndex = adapterIndex;
      g_GraphicsDisplayModeCount++;
    }
  }
}

/* GraphicsCursor_SaveSurfaceBackground / RestoreSurfaceBackground on the memory framebuffer: copies the part of
   the w x h rectangle at (drawX, drawY) that lies on screen between the framebuffer and the cursor buffer (same
   layout, origin at the rectangle's top left), towards the buffer when toBuffer is set. */
void CopyCursorRectangle(SoftwareFramebufferAccess &buffer, int drawY, int drawX, bool toBuffer) noexcept
{
  constexpr int bytesPerPixel = SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT;
  const int rowPixels = static_cast<int>(buffer.width);
  int copyWidth = static_cast<int>(buffer.width);
  int copyHeight = static_cast<int>(buffer.height);
  std::byte *bufferCursor = reinterpret_cast<std::byte *>(static_cast<uint8_t *>(buffer.pixels));
  if (drawX < 0) {
    bufferCursor += -drawX * bytesPerPixel;
    copyWidth += drawX;
    drawX = 0;
  }
  if (drawY < 0) {
    copyHeight += drawY;
    bufferCursor += -drawY * rowPixels * bytesPerPixel;
    drawY = 0;
  }
  copyWidth = std::min(copyWidth, static_cast<int>(g_FramebufferWidth) - drawX);
  copyHeight = std::min(copyHeight, static_cast<int>(g_FramebufferHeight) - drawY);
  if ((copyWidth <= 0) || (copyHeight <= 0) || s_video.framebuffer.empty()) {
    return;
  }
  std::byte *screenCursor = s_video.framebuffer.data() + drawY * s_video.pitchBytes + drawX * bytesPerPixel;
  const std::size_t rowBytes = static_cast<std::size_t>(copyWidth * bytesPerPixel);
  for (; copyHeight != 0; copyHeight--) {
    if (toBuffer) {
      std::memcpy(bufferCursor, screenCursor, rowBytes);
    }
    else {
      std::memcpy(screenCursor, bufferCursor, rowBytes);
    }
    bufferCursor += rowPixels * bytesPerPixel;
    screenCursor += s_video.pitchBytes;
  }
}

/* GraphicsCursor_ComposeBeforePresent on the memory framebuffer: saves the background under the cursor twice,
   blends the cursor frame (the pressed image while a button is down) onto the first copy and writes it back. The
   visibility token is latched so the restore matches what was drawn. */
void ComposeCursor() noexcept
{
  g_CursorCurrentVisibilityToken = g_CursorVisibilityToken;
  if (g_CursorVisibilityToken < 0) {
    return; /* a negative token hides the cursor */
  }
  UiPixelCoordinate cursorX = g_MouseX;
  UiPixelCoordinate cursorY = g_MouseY;
  if (g_CursorUseOverridePosition != 0) {
    cursorX = g_CursorOverrideX;
    cursorY = g_CursorOverrideY;
  }
  const GraphicsCursorFrameRecord &cursorFrame = g_CursorFrameRecords[GraphicsCursor_GetFrameIndex()];
  const int drawX = cursorX - cursorFrame.hotspotX;
  const int drawY = cursorY - cursorFrame.hotspotY;
  s_video.cursorDrawX = drawX;
  s_video.cursorDrawY = drawY;
  CopyCursorRectangle(*g_CursorCompositeBuffer, drawY, drawX, true);
  CopyCursorRectangle(*g_CursorSavedBackground, drawY, drawX, true);
  uint32_t cursorSubresourceIndex = cursorFrame.activeSubresourceIndex;
  if ((g_CursorButtonState & LEFT_MIDDLE_RIGHT) == 0) { /* none of the three mouse buttons is down */
    cursorSubresourceIndex = cursorFrame.idleSubresourceIndex;
  }
  /* the composite buffer holds the saved rectangle at its origin, so the cursor is drawn at (0,0) */
  g_GraphicsTextureSourceBlitSourceAlpha(g_FramebufferHeight, g_FramebufferWidth, 0, 0, 0, 0, cursorSubresourceIndex,
                                         g_CursorSourceAsset, g_CursorCompositeBuffer);
  CopyCursorRectangle(*g_CursorCompositeBuffer, drawY, drawX, false);
}

/* GraphicsCursor_RestoreAfterPresent: writes the saved background back over the cursor. */
void RestoreCursor() noexcept
{
  if (-1 < g_CursorCurrentVisibilityToken) {
    CopyCursorRectangle(*g_CursorSavedBackground, s_video.cursorDrawY, s_video.cursorDrawX, false);
  }
}

/* A newly allocated one-image capture asset of captureWidth x captureHeight, NULL when the allocation fails. */
GraphicsCapturedTextureSourceAsset *AllocateCapture(uint32_t captureHeight, uint32_t captureWidth) noexcept
{
  const uint32_t allocationSize = captureWidth * captureHeight * 4 + GRAPHICS_CAPTURE_PIXELS_OFFSET;
  void *allocation = nullptr;
  if (g_MemoryApi.alloc(allocationSize, &allocation) != 0) {
    return nullptr;
  }
  auto *capturedAsset = static_cast<GraphicsCapturedTextureSourceAsset *>(allocation);
  GraphicsFramebuffer_InitCaptureAsset(capturedAsset, allocationSize, captureWidth, captureHeight);
  return capturedAsset;
}

/* --- renderer choice ------------------------------------------------------------------------------------- */

/* Whether the command line holds the option (e.g. "-GPU"), as a whole word. */
bool CommandLineHasOption(const char *option) noexcept
{
  const char *commandLine = GetCommandLineA();
  const size_t length = SDL_strlen(option);
  for (const char *cursor = commandLine; (cursor != nullptr) && (*cursor != '\0'); cursor++) {
    if (((cursor == commandLine) || (cursor[-1] == ' ')) && (SDL_strncasecmp(cursor, option, length) == 0) &&
        ((cursor[length] == '\0') || (cursor[length] == ' '))) {
      return true;
    }
  }
  return false;
}

/* The renderer forced by OPEN_THANDOR_GPU or the command line (kNoRenderer: none); compare mode in s_renderer. */
uint32_t ForcedRenderer() noexcept
{
  const char *value = SDL_getenv("OPEN_THANDOR_GPU");
  if ((value != nullptr) && (SDL_strcasecmp(value, "auto") == 0)) {
    value = nullptr; /* the saved choice, as without the variable */
  }
  if (value != nullptr) {
    if ((value[0] == '\0') || (SDL_strcmp(value, "0") == 0) || (SDL_strcasecmp(value, "off") == 0) ||
        (SDL_strcasecmp(value, "software") == 0)) {
      return PERSISTENT_RENDERER_SOFTWARE;
    }
    if (SDL_strcasecmp(value, "vulkan") == 0) {
      return PERSISTENT_RENDERER_VULKAN;
    }
    if ((SDL_strcasecmp(value, "d3d12") == 0) || (SDL_strcasecmp(value, "direct3d12") == 0) ||
        (SDL_strcasecmp(value, "dx12") == 0)) {
      return PERSISTENT_RENDERER_DIRECT3D12;
    }
    if (SDL_strcasecmp(value, "compare") == 0) {
#ifdef THANDOR_DEV_TOOLS
      s_renderer.compare = true;
#else
      Thandor_Log("SDL_GPU renderer: compare mode needs the developer tools, using the GPU alone");
#endif
    }
    return PERSISTENT_RENDERER_VULKAN; /* 1 / on / compare: the first GPU renderer that runs */
  }
  if (CommandLineHasOption("-SOFTWARE")) {
    return PERSISTENT_RENDERER_SOFTWARE;
  }
  if (CommandLineHasOption("-GPU")) {
    return PERSISTENT_RENDERER_VULKAN;
  }
  return kNoRenderer;
}

/* Whether a renderer can run here (the GPU ones probed through SDL_GPU, Vulkan also needs the Vulkan window). */
bool RendererAvailable(uint32_t renderer) noexcept
{
#ifdef THANDOR_RENDERER_SDL_GPU
  if (renderer == PERSISTENT_RENDERER_VULKAN) {
    return thandor::sdl3::VulkanWindow() && thandor::sdl3::GpuRendererSupported(renderer);
  }
  if (renderer == PERSISTENT_RENDERER_DIRECT3D12) {
    return thandor::sdl3::GpuRendererSupported(renderer);
  }
  return true;
#else
  return renderer == PERSISTENT_RENDERER_SOFTWARE;
#endif
}

/* Fills s_renderer.adapters: the forced renderer alone (the first available one from it on, in the order Vulkan,
   DirectX 12, Software), else every available GPU renderer and Software. */
void ListRenderers() noexcept
{
  s_renderer.forced = ForcedRenderer();
  bool available[PERSISTENT_RENDERER_COUNT] = {};
  for (uint32_t renderer = 0; renderer < PERSISTENT_RENDERER_COUNT; renderer++) {
    available[renderer] = ((s_renderer.forced == kNoRenderer) || (renderer >= s_renderer.forced)) &&
                          RendererAvailable(renderer);
  }
  s_renderer.adapterCount = 0;
  if (s_renderer.forced != kNoRenderer) {
    uint32_t renderer = s_renderer.forced;
    while (!available[renderer]) {
      Thandor_Log("renderer %s is not available here, trying the next", RendererName(renderer));
      renderer++;
    }
    s_renderer.forced = renderer;
    s_renderer.adapters[s_renderer.adapterCount++] = renderer;
    Thandor_Log("renderer forced to %s (OPEN_THANDOR_GPU / command line)", RendererName(renderer));
    return;
  }
  for (uint32_t renderer = 0; renderer < PERSISTENT_RENDERER_COUNT; renderer++) {
    if (available[renderer]) {
      s_renderer.adapters[s_renderer.adapterCount++] = renderer;
    }
  }
  Thandor_Log("renderers: Vulkan %s, DirectX 12 %s, Software available", available[0] ? "available" : "not available",
              available[1] ? "available" : "not available");
}

uint32_t AdapterOfRenderer(uint32_t renderer) noexcept
{
  for (uint32_t adapterIndex = 0; adapterIndex < s_renderer.adapterCount; adapterIndex++) {
    if (s_renderer.adapters[adapterIndex] == renderer) {
      return adapterIndex;
    }
  }
  return 0;
}

/* The SDL_Renderer of the software renderer: Vulkan, else Direct3D 12, Direct3D 11, else SDL's choice. */
bool CreateSdlRenderer() noexcept
{
  SDL_Window *window = thandor::sdl3::MainWindow();
  for (const char *driver : {"vulkan", "direct3d12", "direct3d11", static_cast<const char *>(nullptr)}) {
    if ((driver != nullptr) && (SDL_strcmp(driver, "vulkan") == 0) && !thandor::sdl3::VulkanWindow()) {
      continue;
    }
    s_renderer.sdlRenderer.reset(SDL_CreateRenderer(window, driver));
    if (s_renderer.sdlRenderer) {
      break;
    }
    Thandor_Log("SDL_CreateRenderer %s failed: %s", (driver != nullptr) ? driver : "(default)", SDL_GetError());
  }
  if (!s_renderer.sdlRenderer) {
    return false;
  }
  Thandor_Log("software renderer, presenting through the SDL_Renderer %s",
              SDL_GetRendererName(s_renderer.sdlRenderer.get()));
  return true;
}

/* Stops the running renderer and starts this one; false (logged) when it cannot start. */
bool StartRenderer(uint32_t renderer) noexcept
{
  s_video.texture.reset();
  s_renderer.sdlRenderer.reset();
#ifdef THANDOR_RENDERER_SDL_GPU
  thandor::sdl3::StopGpuDevice();
  if (renderer != PERSISTENT_RENDERER_SOFTWARE) {
    if (!thandor::sdl3::StartGpuDevice(renderer, thandor::sdl3::MainWindow(), s_renderer.compare)) {
      return false;
    }
    s_renderer.active = renderer;
    return true;
  }
#endif
  if (!CreateSdlRenderer()) {
    return false;
  }
  s_renderer.active = PERSISTENT_RENDERER_SOFTWARE;
  return true;
}

/* Makes the requested renderer the running one, falling back Vulkan -> DirectX 12 -> Software. Returns the
   running renderer, kNoRenderer when none starts. */
uint32_t SwitchRenderer(uint32_t requested) noexcept
{
  if (requested == s_renderer.active) {
    return requested;
  }
  for (uint32_t renderer = requested; renderer < PERSISTENT_RENDERER_COUNT; renderer++) {
    if (StartRenderer(renderer)) {
      if (renderer != requested) {
        Thandor_Log("renderer %s could not start, %s runs instead", RendererName(requested), RendererName(renderer));
      }
      return renderer;
    }
  }
  s_renderer.active = kNoRenderer;
  return kNoRenderer;
}

/* --- window ----------------------------------------------------------------------------------------------- */

/* Applies the display mode kind for a width x height framebuffer. */
void ApplyDisplayModeKind(uint32_t kind, int width, int height) noexcept
{
  SDL_Window *window = thandor::sdl3::MainWindow();
  const uint32_t previousKind = s_renderer.kind;
  s_renderer.kind = kind;
  if (thandor::sdl3::Windowed()) {
    /* the developer tools' window: the mode's size at its fixed position */
    SDL_SetWindowSize(window, width, height);
    SDL_SyncWindow(window);
    return;
  }
  switch (kind) {
  case PERSISTENT_DISPLAY_MODE_WINDOW:
    SDL_SetWindowFullscreen(window, false);
    SDL_SetWindowBordered(window, true);
    SDL_SetWindowResizable(window, true);
    if ((previousKind != PERSISTENT_DISPLAY_MODE_WINDOW) || !s_renderer.shown || (s_renderer.windowWidth != width) ||
        (s_renderer.windowHeight != height)) {
      SDL_SetWindowSize(window, width, height);
      SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
      s_renderer.windowWidth = width;
      s_renderer.windowHeight = height;
    }
    break;
  case PERSISTENT_DISPLAY_MODE_FULLSCREEN: {
    SDL_DisplayMode mode;
    SDL_zero(mode);
    const SDL_DisplayID display = SDL_GetDisplayForWindow(window);
    if (SDL_GetClosestFullscreenDisplayMode(display, width, height, 0.0f, false, &mode)) {
      SDL_SetWindowFullscreenMode(window, &mode);
      Thandor_Log("exclusive fullscreen %dx%d @ %.0f Hz", mode.w, mode.h, static_cast<double>(mode.refresh_rate));
    }
    else {
      Thandor_Log("no fullscreen display mode for %dx%d (%s), borderless instead", width, height, SDL_GetError());
      SDL_SetWindowFullscreenMode(window, nullptr);
    }
    SDL_SetWindowFullscreen(window, true);
    break;
  }
  default:
    SDL_SetWindowFullscreenMode(window, nullptr);
    SDL_SetWindowFullscreen(window, true);
    break;
  }
  SDL_SyncWindow(window);
}

} // namespace

namespace thandor::sdl3 {

SDL_FRect LetterboxRect(float outerWidth, float outerHeight, float innerWidth, float innerHeight) noexcept
{
  if ((outerWidth <= 0.0f) || (outerHeight <= 0.0f) || (innerWidth <= 0.0f) || (innerHeight <= 0.0f)) {
    return SDL_FRect{0.0f, 0.0f, outerWidth, outerHeight};
  }
  const float scale = std::min(outerWidth / innerWidth, outerHeight / innerHeight);
  const float width = std::floor(innerWidth * scale + 0.5f);
  const float height = std::floor(innerHeight * scale + 0.5f);
  return SDL_FRect{std::floor((outerWidth - width) / 2.0f), std::floor((outerHeight - height) / 2.0f), width, height};
}

namespace {
/* The framebuffer's rectangle in window coordinates. */
SDL_FRect FramebufferInWindow() noexcept
{
  int windowWidth = 0;
  int windowHeight = 0;
  SDL_GetWindowSize(MainWindow(), &windowWidth, &windowHeight);
  return LetterboxRect(static_cast<float>(windowWidth), static_cast<float>(windowHeight),
                       static_cast<float>(g_FramebufferWidth), static_cast<float>(g_FramebufferHeight));
}
} // namespace

void WindowToFramebuffer(float windowX, float windowY, float &outX, float &outY) noexcept
{
  const SDL_FRect box = FramebufferInWindow();
  outX = windowX;
  outY = windowY;
  if ((box.w > 0.0f) && (box.h > 0.0f)) {
    outX = (windowX - box.x) * static_cast<float>(g_FramebufferWidth) / box.w;
    outY = (windowY - box.y) * static_cast<float>(g_FramebufferHeight) / box.h;
  }
}

void FramebufferToWindow(float x, float y, float &outWindowX, float &outWindowY) noexcept
{
  const SDL_FRect box = FramebufferInWindow();
  outWindowX = x;
  outWindowY = y;
  if ((g_FramebufferWidth != 0) && (g_FramebufferHeight != 0)) {
    outWindowX = box.x + x * box.w / static_cast<float>(g_FramebufferWidth);
    outWindowY = box.y + y * box.h / static_cast<float>(g_FramebufferHeight);
  }
}

bool AbsoluteMouse() noexcept
{
  return Windowed() || (s_renderer.kind == PERSISTENT_DISPLAY_MODE_WINDOW);
}

} // namespace thandor::sdl3

using namespace thandor::sdl3;

uint32_t SdlVideo_Init()
{
  const uint32_t allocError = Graphics_AllocateTables();
  if (allocError != 0) {
    return allocError;
  }
  /* the renderers as adapters */
  ListRenderers();
  for (uint32_t adapterIndex = 0; adapterIndex < s_renderer.adapterCount; adapterIndex++) {
    GraphicsAdapterRecord &adapter = g_GraphicsAdapters[adapterIndex];
    std::memset(&adapter, 0, sizeof adapter);
    char adapterName[sizeof adapter.driverDescriptionUtf16 / 2];
    SDL_strlcpy(adapterName, RendererName(s_renderer.adapters[adapterIndex]), sizeof adapterName);
    Text_CopyNarrowToUtf16(sizeof adapter.driverDescriptionUtf16, adapter.driverDescriptionUtf16,
                           reinterpret_cast<uint8_t *>(adapterName));
  }
  g_GraphicsAdapterCount = s_renderer.adapterCount;
  ListDisplayModes();
  /* the display-mode hook installed before (the software renderer's base step) finalizes every mode switch */
  g_GraphicsDisplayModeFinalize = g_GraphicsSetDisplayMode;
  g_GraphicsSetDisplayMode = SdlVideo_ApplyDisplayMode;
  g_GraphicsFramebufferBeginAccess = GraphicsFramebuffer_BeginAccessStub;
  g_GraphicsFramebufferEndAccess = GraphicsFramebuffer_EndAccessStub;
  g_GraphicsCreateTextureSet = GraphicsTextureSet_Create;
  g_GraphicsDestroyTextureSet = GraphicsTextureSet_Destroy;
  return 0;
}

void SdlVideo_Shutdown()
{
  g_GraphicsBackendAccessState = -1; /* nothing presents any more */
  g_DisplayFramebufferAccess.pixels = nullptr;
#ifdef THANDOR_RENDERER_SDL_GPU
  StopGpuDevice();
#endif
  s_video.texture.reset();
  s_renderer.sdlRenderer.reset();
  s_renderer.active = kNoRenderer;
  s_video.framebuffer = std::vector<std::byte>();
  thandor::sdl3::DestroyMainWindow();
}

uint32_t SdlVideo_SavedAdapterIndex()
{
  uint32_t renderer = s_renderer.forced;
  if (renderer == kNoRenderer) {
    renderer = PersistentSettings_Read(PERSISTENT_RENDERER_VULKAN, PERSISTENT_SETTING_RENDERER);
  }
  return AdapterOfRenderer(renderer);
}

void SdlVideo_SaveAdapterIndex(uint32_t adapterIndex)
{
  /* the original's adapter index: the one display adapter */
  PersistentSettings_Write(PERSISTENT_DEFAULT_ADAPTER_INDEX, PERSISTENT_SETTING_ADAPTER_INDEX);
  if ((s_renderer.forced == kNoRenderer) && (adapterIndex < s_renderer.adapterCount)) {
    PersistentSettings_Write(s_renderer.adapters[adapterIndex], PERSISTENT_SETTING_RENDERER);
  }
}

uint16_t *SdlVideo_AdapterDetailUtf16(uint32_t adapterIndex)
{
  if ((adapterIndex < s_renderer.adapterCount) && (s_renderer.adapters[adapterIndex] != PERSISTENT_RENDERER_SOFTWARE)) {
    return s_gpuDetailUtf16;
  }
  return s_cpuDetailUtf16;
}

uint32_t SdlVideo_SavedDisplayModeKind()
{
  if (Windowed() && (s_renderer.windowedChosenKind < PERSISTENT_DISPLAY_MODE_COUNT)) {
    return s_renderer.windowedChosenKind; /* not saved, but the settings page treats it as applied */
  }
  const uint32_t kind = PersistentSettings_Read(PERSISTENT_DISPLAY_MODE_FULLSCREEN, PERSISTENT_SETTING_DISPLAY_MODE_KIND);
  return (kind < PERSISTENT_DISPLAY_MODE_COUNT) ? kind : PERSISTENT_DISPLAY_MODE_FULLSCREEN;
}

void SdlVideo_SaveDisplayModeKind(uint32_t kind)
{
  if (kind >= PERSISTENT_DISPLAY_MODE_COUNT) {
    return;
  }
  if (Windowed()) {
    if (kind != SdlVideo_SavedDisplayModeKind()) {
      Thandor_Log("display mode %s chosen; the developer window (OPEN_THANDOR_WINDOWED) stays a window",
                  DisplayModeKindName(kind));
    }
    s_renderer.windowedChosenKind = kind;
    return;
  }
  PersistentSettings_Write(kind, PERSISTENT_SETTING_DISPLAY_MODE_KIND);
}

uint32_t SdlVideo_DisplayModeKind()
{
  return Windowed() ? PERSISTENT_DISPLAY_MODE_WINDOW : s_renderer.kind;
}

void SdlVideo_SetDisplayModeKind(uint32_t kind)
{
  s_renderer.pendingKind = (kind < PERSISTENT_DISPLAY_MODE_COUNT) ? kind : PERSISTENT_DISPLAY_MODE_FULLSCREEN;
  s_renderer.kindChosen = true;
}

Bool8 SdlVideo_ApplyDisplayMode(uint32_t adapterIndex,uint32_t bitsPerPixel,uint32_t height,uint32_t width,
                                uint32_t *errorCode)
{
  g_CursorCurrentVisibilityToken = -1;
  if (!s_renderer.kindChosen) {
    /* the first switch runs after the settings are loaded */
    s_renderer.pendingKind = SdlVideo_SavedDisplayModeKind();
    s_renderer.kindChosen = true;
  }
  if (adapterIndex >= s_renderer.adapterCount) {
    adapterIndex = 0;
  }
  const uint32_t renderer = SwitchRenderer(s_renderer.adapters[adapterIndex]);
  if (renderer == kNoRenderer) {
    Thandor_Log("no renderer could start");
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR, 0, 10, 1, 0, g_PackageLastErrorPath);
    *errorCode = FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES;
    return false;
  }
  for (uint32_t index = 0; index < s_renderer.adapterCount; index++) {
    if (s_renderer.adapters[index] == renderer) {
      adapterIndex = index;
    }
  }
  /* 32-bit colour only: a requested depth (an old saved 16, or the 24 colour bits a caller derives from the
     pixel format) is not looked at */
  bitsPerPixel = PERSISTENT_DEFAULT_BITS_PER_PIXEL;
  constexpr int bytesPerPixel = SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT;
  constexpr SDL_PixelFormat pixelFormat = SDL_PIXELFORMAT_XRGB8888;
  ApplyDisplayModeKind(Windowed() ? PERSISTENT_DISPLAY_MODE_WINDOW : s_renderer.pendingKind, static_cast<int>(width),
                       static_cast<int>(height));
  if (renderer == PERSISTENT_RENDERER_SOFTWARE) {
    SDL_Renderer *sdlRenderer = s_renderer.sdlRenderer.get();
    thandor::sdl3::TexturePtr texture(SDL_CreateTexture(sdlRenderer, pixelFormat, SDL_TEXTUREACCESS_STREAMING,
                                                        static_cast<int>(width), static_cast<int>(height)));
    if (!texture) {
      Thandor_Log("SDL_CreateTexture %ux%u failed: %s", width, height, SDL_GetError());
      g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR, 0, 10, 1, 0, g_PackageLastErrorPath);
      *errorCode = FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES;
      return false;
    }
    SDL_SetTextureScaleMode(texture.get(), SDL_SCALEMODE_LINEAR);
    s_video.texture = std::move(texture);
    SDL_SetRenderLogicalPresentation(sdlRenderer, static_cast<int>(width), static_cast<int>(height),
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
  }
  else {
    s_video.texture.reset();
  }
  s_video.width = static_cast<int>(width);
  s_video.height = static_cast<int>(height);
  s_video.pitchBytes = static_cast<int>(width) * bytesPerPixel;
  s_video.framebuffer.assign(static_cast<std::size_t>(s_video.pitchBytes) * height, std::byte{0});
  Thandor_Log("display mode %ux%ux%u, %s, renderer %s", width, height, bitsPerPixel,
              DisplayModeKindName(SdlVideo_DisplayModeKind()), RendererName(renderer));
  if (!s_renderer.shown) {
    SDL_ShowWindow(MainWindow());
    s_renderer.shown = true;
  }
  UpdateMouseMode();

  int formatBits = 0;
  Uint32 redMask = 0;
  Uint32 greenMask = 0;
  Uint32 blueMask = 0;
  Uint32 alphaMask = 0;
  SDL_GetMasksForPixelFormat(pixelFormat, &formatBits, &redMask, &greenMask, &blueMask, &alphaMask);
  ChannelOfMask(redMask, g_SoftwarePixelFormatConfig.redMask, g_SoftwarePixelFormatConfig.redShift,
                g_SoftwarePixelFormatConfig.redBitCount);
  ChannelOfMask(greenMask, g_SoftwarePixelFormatConfig.greenMask, g_SoftwarePixelFormatConfig.greenShift,
                g_SoftwarePixelFormatConfig.greenBitCount);
  ChannelOfMask(blueMask, g_SoftwarePixelFormatConfig.blueMask, g_SoftwarePixelFormatConfig.blueShift,
                g_SoftwarePixelFormatConfig.blueBitCount);

  /* the framebuffer, the blitters and the active adapter as in the original; then this backend's
     present, captures and permanent pixels */
  GraphicsDirectDraw_PublishFramebuffer(adapterIndex, bitsPerPixel, height, width);
  g_DisplayFramebufferAccess.pixels = reinterpret_cast<uint8_t *>(s_video.framebuffer.data());
  g_FramebufferRowStrideBytes = static_cast<uint32_t>(s_video.pitchBytes);
  g_GraphicsFramebufferPresent = SdlVideo_Present;
  g_GraphicsFramebufferCaptureRegion = SdlVideo_CaptureRegion32Bit;
  return g_GraphicsDisplayModeFinalize(adapterIndex, bitsPerPixel, height, width, errorCode);
}

void SdlVideo_Present(SoftwareFramebufferAccess *framebuffer)
{
  g_ThandorFrameHeartbeat++;
  /* atomic exchange: take the backend lock and learn whether it was already held */
  const auto previousAccessState = static_cast<int32_t>(THANDOR_ATOMIC_EXCHANGE(&g_GraphicsBackendAccessState, 1));
  if (previousAccessState != 0) {
    return;
  }
  if ((framebuffer == &g_DisplayFramebufferAccess) && !s_video.framebuffer.empty()) {
    ComposeCursor();
#ifdef THANDOR_RENDERER_SDL_GPU
    if (GpuDeviceRunning()) {
      PresentWithGpu(s_video.framebuffer.data(), s_video.pitchBytes, s_video.width, s_video.height);
    }
    else
#endif
    if (s_video.texture && s_renderer.sdlRenderer) {
      SDL_Renderer *sdlRenderer = s_renderer.sdlRenderer.get();
      SDL_UpdateTexture(s_video.texture.get(), nullptr, s_video.framebuffer.data(), s_video.pitchBytes);
      SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderClear(sdlRenderer);
      SDL_RenderTexture(sdlRenderer, s_video.texture.get(), nullptr, nullptr);
      SDL_RenderPresent(sdlRenderer);
    }
    RestoreCursor();
  }
  g_GraphicsBackendAccessState--;
}

GraphicsCapturedTextureSourceAsset *SdlVideo_CaptureRegion32Bit(uint32_t captureHeight,uint32_t captureWidth,
                                                                int32_t sourceY,int32_t sourceX)
{
  GraphicsCapturedTextureSourceAsset *capturedAsset = AllocateCapture(captureHeight, captureWidth);
  if ((capturedAsset == nullptr) || (captureHeight == 0) || (captureWidth == 0) || s_video.framebuffer.empty()) {
    return capturedAsset;
  }
  const std::byte *sourceRow = s_video.framebuffer.data() + (sourceY * static_cast<int32_t>(g_FramebufferWidth) + sourceX) * 4;
  uint32_t *destinationPixel = capturedAsset->argb8888Pixels;
  for (uint32_t row = 0; row < captureHeight; row++) {
    const std::byte *sourcePixel = sourceRow;
    for (uint32_t column = 0; column < captureWidth; column++) {
      uint32_t pixel = 0;
      std::memcpy(&pixel, sourcePixel, sizeof pixel);
      *destinationPixel = pixel | ARGB8888_ALPHA_MASK; /* RGB kept, alpha forced to 0xFF */
      sourcePixel += 4;
      destinationPixel++;
    }
    sourceRow += s_video.pitchBytes;
  }
  return capturedAsset;
}
