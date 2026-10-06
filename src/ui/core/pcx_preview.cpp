/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/pcx_preview.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/core/pcx_preview.h>
#include <thandor/thandor.h>
#include <thandor/graphics/resources/pcx.h>
#include <string.h>

/* Loads a 64x64 8-bit PCX picture named by sourcePath (a leaf name; path and wildcard characters are
   dropped, the extension becomes .pcx, the file is looked up next to the executable) into outputPreview:
   its 256-colour palette (3 bytes per colour in the order blue, green, red) followed by the 4096 pixel
   indices. Returns true when the file is missing, cannot be decoded (see Pcx_DecodeIndexed8: only 8-bit paletted
   files) or has another size.
*/
Bool8 PcxPreview_Load64x64PaletteAndPixels(PcxPreview64 *outputPreview,uint16_t *sourcePath)

{
  uint16_t pathChar;
  void *sourceBytes;
  int colorIndex;
  uint32_t color;
  uint16_t *sanitizedPathCursor;
  PcxIndexedImage image;
  uint32_t sourceByteCount;

  /* copy the leaf, dropping every character that is not allowed in a file name, and '.' (each character is
     written first and the cursor only advances past kept ones) */
  sanitizedPathCursor = g_LevelEndingMovieSourcePath;
  for (pathChar = *sourcePath++; pathChar != 0; pathChar = *sourcePath++) {
    *sanitizedPathCursor = pathChar;
    if (pathChar != '*' && pathChar != '.' && pathChar != '?' && pathChar != '/' && pathChar != '\\' &&
        pathChar != '<' && pathChar != '>' && pathChar != '"' && pathChar != ':' && pathChar != '|') {
      sanitizedPathCursor++;
    }
  }
  *sanitizedPathCursor = 0;
  WidePath_CombineDirectoryAndLeaf
            (g_LevelResourcePathScratchUtf16,g_LevelEndingMovieSourcePath,
             g_ExecutableDirectoryUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_PCX,g_LevelResourcePathScratchUtf16);
  if (Resource_Load(g_LevelResourcePathScratchUtf16,&sourceBytes,&sourceByteCount,nullptr)) {
    /* the original called pcx.fnc export 2 here and accepted only a paletted record (direct-colour 3-plane
       files were rejected as well) */
    if (Pcx_DecodeIndexed8(static_cast<const uint8_t *>(sourceBytes),sourceByteCount,&image)) {
      if (image.width == 64 && image.height == 64) {
        /* Original quirk: each colour dword 0xFFRRGGBB was stored whole with the output advancing by 3
           bytes, the 0xFF byte being overwritten by the next colour (the last one by the first pixels).
           What remains are the bytes blue, green, red, i.e. the PcxRgb24 fields named red/green/blue
           receive blue/green/red. */
        for (colorIndex = 0; colorIndex < PCX_PALETTE_COLOR_COUNT; colorIndex++) {
          color = image.paletteColors[colorIndex];
          outputPreview->palette[colorIndex].red = (uint8_t)color;
          outputPreview->palette[colorIndex].green = (uint8_t)(color >> 8);
          outputPreview->palette[colorIndex].blue = (uint8_t)(color >> 16);
        }
        memcpy(outputPreview->pixels,image.pixels,sizeof outputPreview->pixels);
        Pcx_FreeIndexed8(&image);
        Resource_Release(sourceBytes);
        return false;
      }
      Pcx_FreeIndexed8(&image);
    }
    Resource_Release(sourceBytes);
  }
  return true;
}
