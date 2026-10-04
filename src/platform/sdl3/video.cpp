/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/video.cpp
 * Project code (not in the original game)
 */

/* SDL3 backend: video. The software renderer draws into a plain memory framebuffer (RGB565 in 16-bit modes,
   XRGB8888 in 32-bit modes) that stays published in g_DisplayFramebufferAccess, so the framebuffer access hooks
   are the no-op stubs. A present composes the software cursor into the framebuffer (as GraphicsFramebuffer_Present
   does into the DirectDraw back surface), uploads it into a streaming texture, draws that letterboxed with
   SDL_SetRenderLogicalPresentation and removes the cursor again. Screen captures read the memory framebuffer. */

#include <thandor/platform/sdl3/sdl_objects.h>

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <span>
#include <utility>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

namespace {

constexpr int kMinimumModeWidth = 640;
constexpr int kMinimumModeHeight = 480;

/* The framebuffer, its texture and the cursor rectangle drawn into it at the last present. */
struct VideoState {
  std::vector<std::byte> framebuffer;
  thandor::sdl3::TexturePtr texture;
  int bytesPerPixel = 0;
  int pitchBytes = 0;
  int cursorDrawX = 0;
  int cursorDrawY = 0;
};
VideoState s_video;

/* The desktop's bits per pixel (16 or 32), as the developer tools' DirectDraw window uses it. */
uint32_t DesktopBitsPerPixel() noexcept
{
  const SDL_DisplayMode *desktop = SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay());
  if ((desktop != nullptr) && (SDL_BYTESPERPIXEL(desktop->format) == 2)) {
    return 16;
  }
  return 32;
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
   (640x480 itself always), in 16 and 32 bits per pixel, all on adapter 0. */
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
  for (const auto &[width, height] : sizes) {
    for (const FrontendColorDepthBits bitsPerPixel : {16, 32}) {
      if (g_GraphicsDisplayModeCount >= GRAPHICS_DISPLAY_MODE_CAPACITY) {
        return;
      }
      GraphicsDisplayMode &slot = g_GraphicsDisplayModes[g_GraphicsDisplayModeCount];
      slot.width = width;
      slot.height = height;
      slot.bitsPerPixel = bitsPerPixel;
      slot.adapterIndex = 0;
      g_GraphicsDisplayModeCount++;
    }
  }
}

/* GraphicsCursor_SaveSurfaceBackground / RestoreSurfaceBackground on the memory framebuffer: copies the part of
   the w x h rectangle at (drawX, drawY) that lies on screen between the framebuffer and the cursor buffer (same
   layout, origin at the rectangle's top left), towards the buffer when toBuffer is set. */
void CopyCursorRectangle(SoftwareFramebufferAccess &buffer, int drawY, int drawX, bool toBuffer) noexcept
{
  const int bytesPerPixel = (buffer.bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) ? 2 : 4;
  const int rowPixels = static_cast<int>(buffer.width);
  int copyWidth = static_cast<int>(buffer.width);
  int copyHeight = static_cast<int>(buffer.height);
  std::byte *bufferCursor = reinterpret_cast<std::byte *>(buffer.pixels);
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

} // namespace

uint32_t SdlVideo_Init(void)
{
  const uint32_t allocError = Graphics_AllocateTables();
  if (allocError != 0) {
    return allocError;
  }
  /* one adapter */
  GraphicsAdapterRecord &adapter = g_GraphicsAdapters[0];
  std::memset(&adapter, 0, sizeof adapter);
  char adapterName[] = "SDL";
  Text_CopyNarrowToUtf16(sizeof adapter.driverDescriptionUtf16, adapter.driverDescriptionUtf16,
                         reinterpret_cast<uint8_t *>(adapterName));
  g_GraphicsAdapterCount = 1;
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

void SdlVideo_Shutdown(void)
{
  g_GraphicsBackendAccessState = -1; /* nothing presents any more */
  g_DisplayFramebufferAccess.pixels = nullptr;
  s_video.texture.reset();
  s_video.framebuffer = std::vector<std::byte>();
  thandor::sdl3::DestroyMainWindow();
}

Bool8 SdlVideo_ApplyDisplayMode(uint32_t adapterIndex,uint32_t bitsPerPixel,uint32_t height,uint32_t width,
                                uint32_t *errorCode)
{
  g_CursorCurrentVisibilityToken = -1;
  if (thandor::sdl3::Windowed() && (bitsPerPixel != DesktopBitsPerPixel())) {
    /* as the developer tools' DirectDraw window: the desktop's depth, which the blitters chosen afterwards, the
       renderer's queue and the pixel packing all follow */
    Thandor_Log("test aid: windowed %ux%u uses the desktop depth of %u bits instead of %u", width, height,
                DesktopBitsPerPixel(), bitsPerPixel);
    bitsPerPixel = DesktopBitsPerPixel();
  }
  bitsPerPixel = (bitsPerPixel <= 16) ? 16 : 32;
  const int bytesPerPixel = (bitsPerPixel == 16) ? 2 : 4;
  const SDL_PixelFormat pixelFormat = (bitsPerPixel == 16) ? SDL_PIXELFORMAT_RGB565 : SDL_PIXELFORMAT_XRGB8888;
  SDL_Renderer *renderer = thandor::sdl3::MainRenderer();
  thandor::sdl3::TexturePtr texture(SDL_CreateTexture(renderer, pixelFormat, SDL_TEXTUREACCESS_STREAMING,
                                                      static_cast<int>(width), static_cast<int>(height)));
  if (!texture) {
    Thandor_Log("SDL_CreateTexture %ux%u failed: %s", width, height, SDL_GetError());
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR, 0, 10, 1, 0, g_PackageLastErrorPath);
    *errorCode = FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES;
    return false;
  }
  SDL_SetTextureScaleMode(texture.get(), SDL_SCALEMODE_LINEAR);
  s_video.texture = std::move(texture);
  s_video.bytesPerPixel = bytesPerPixel;
  s_video.pitchBytes = static_cast<int>(width) * bytesPerPixel;
  s_video.framebuffer.assign(static_cast<std::size_t>(s_video.pitchBytes) * height, std::byte{0});
  SDL_SetRenderLogicalPresentation(renderer, static_cast<int>(width), static_cast<int>(height),
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
  if (thandor::sdl3::Windowed()) {
    SDL_SetWindowSize(thandor::sdl3::MainWindow(), static_cast<int>(width), static_cast<int>(height));
  }

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

  /* the framebuffer, the 16/32-bit blitters and the active adapter as for DirectDraw; then this backend's
     present, captures and permanent pixels */
  GraphicsDirectDraw_PublishFramebuffer(adapterIndex, bitsPerPixel, height, width);
  g_DisplayFramebufferAccess.pixels = reinterpret_cast<uint8_t *>(s_video.framebuffer.data());
  g_FramebufferRowStrideBytes = static_cast<uint32_t>(s_video.pitchBytes);
  g_GraphicsFramebufferPresent = SdlVideo_Present;
  g_GraphicsFramebufferCaptureRegion = (bitsPerPixel == 16) ? SdlVideo_CaptureRegion16Bit : SdlVideo_CaptureRegion32Bit;
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
  if ((framebuffer == &g_DisplayFramebufferAccess) && s_video.texture && !s_video.framebuffer.empty()) {
    SDL_Renderer *renderer = thandor::sdl3::MainRenderer();
    ComposeCursor();
    SDL_UpdateTexture(s_video.texture.get(), nullptr, s_video.framebuffer.data(), s_video.pitchBytes);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, s_video.texture.get(), nullptr, nullptr);
    SDL_RenderPresent(renderer);
    RestoreCursor();
  }
  g_GraphicsBackendAccessState--;
}

GraphicsCapturedTextureSourceAsset *SdlVideo_CaptureRegion16Bit(uint32_t captureHeight,uint32_t captureWidth,
                                                                int32_t sourceY,int32_t sourceX)
{
  GraphicsCapturedTextureSourceAsset *capturedAsset = AllocateCapture(captureHeight, captureWidth);
  if ((capturedAsset == nullptr) || (captureHeight == 0) || (captureWidth == 0) || s_video.framebuffer.empty()) {
    return capturedAsset;
  }
  /* the first pixel as GraphicsFramebuffer_CaptureRegion16Bit finds it, rows a pitch apart */
  const std::byte *sourceRow = s_video.framebuffer.data() + (sourceY * static_cast<int32_t>(g_FramebufferWidth) + sourceX) * 2;
  uint32_t *destinationPixel = capturedAsset->argb8888Pixels;
  for (uint32_t row = 0; row < captureHeight; row++) {
    const std::byte *sourcePixel = sourceRow;
    for (uint32_t column = 0; column < captureWidth; column++) {
      uint16_t packed = 0;
      std::memcpy(&packed, sourcePixel, sizeof packed);
      const uint32_t pixel = packed;
      const uint8_t red = GraphicsFramebuffer_ExpandChannelTo8Bit(pixel, g_SoftwarePixelFormatConfig.redMask,
                                                                  g_SoftwarePixelFormatConfig.redShift,
                                                                  g_SoftwarePixelFormatConfig.redBitCount);
      const uint8_t green = GraphicsFramebuffer_ExpandChannelTo8Bit(pixel, g_SoftwarePixelFormatConfig.greenMask,
                                                                    g_SoftwarePixelFormatConfig.greenShift,
                                                                    g_SoftwarePixelFormatConfig.greenBitCount);
      const uint8_t blue = GraphicsFramebuffer_ExpandChannelTo8Bit(pixel, g_SoftwarePixelFormatConfig.blueMask,
                                                                   g_SoftwarePixelFormatConfig.blueShift,
                                                                   g_SoftwarePixelFormatConfig.blueBitCount);
      *destinationPixel = (uint32_t{ARGB8888_CHANNEL_MAX} << 24) | (uint32_t{red} << 16) | (uint32_t{green} << 8) | blue;
      sourcePixel += 2;
      destinationPixel++;
    }
    sourceRow += s_video.pitchBytes;
  }
  return capturedAsset;
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
