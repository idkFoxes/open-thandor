/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/sdl3/window_icon.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_SDL3_WINDOW_ICON_H
#define THANDOR_PLATFORM_SDL3_WINDOW_ICON_H

/* The window and taskbar icon: thandor.ico of the game directory (src/platform/sdl3/window_icon.cpp). SDL3 cannot
   read .ico files, so the module parses them itself: the icon directory and its BMP (DIB) images with 1, 4, 8, 24
   or 32 bits per pixel (palette, XOR bitmap and AND mask); PNG-compressed images are skipped. */

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

struct SDL_Window;

namespace thandor::sdl3 {

/* One image of an icon file, rows top-down, one 0xAARRGGBB value (SDL_PIXELFORMAT_ARGB8888) per pixel. */
struct IconImage {
  int width = 0;
  int height = 0;
  int bitsPerPixel = 0;
  std::vector<uint32_t> pixels;
};

/* The images of an icon file in directory order, and how many entries were left out. */
struct IconFile {
  std::vector<IconImage> images;
  int skippedPngImages = 0;
  int skippedInvalidImages = 0;
};

/* Parses an .ico file. Returns false (out empty) when data is no icon file (header, type, entry count or the
   directory does not fit); single images that are PNG-compressed or broken are counted and skipped. */
bool DecodeIcoFile(std::span<const std::byte> data, IconFile &out);

/* The image to use for 100% display scale: the 32x32 one, else the smallest larger one, else the largest; of
   images of the same size the one with the most bits per pixel. Returns images.size() when there is none. */
std::size_t IconPrimaryImageIndex(const IconFile &icon);

/* The other sizes, one image each (the most bits per pixel), smallest first: the alternate images for other
   display scales. */
std::vector<std::size_t> IconAlternateImageIndices(const IconFile &icon, std::size_t primaryIndex);

/* At startup: loads thandor.ico from the executable's directory, else from the current directory, and makes it
   the window's icon (the other sizes as alternate images). A missing or unusable file is logged in one line and
   the default icon stays. */
void SetWindowIconFromGameDirectory(SDL_Window *window) noexcept;

} // namespace thandor::sdl3

#endif /* THANDOR_PLATFORM_SDL3_WINDOW_ICON_H */
