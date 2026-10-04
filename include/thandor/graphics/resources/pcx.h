/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/pcx.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_PCX_H
#define THANDOR_GRAPHICS_RESOURCES_PCX_H

/*
PCX reading and writing in C. The original executable ran these as machine code from the game data
(engine\pcx.fnc in ENGINE.PCK: export 2 decodes, export 3 encodes); open-thandor no longer loads that module.
Writing: screenshots (Alt+P in game, the movie screenshot command). Reading: the 64x64 player picture
<name>.pcx sent in network games.
*/

#include <thandor/core/types.h>
#include <thandor/platform/sdl3/types.h>
#include <thandor/core/contracts.h>

/* pcx_write.c and pcx_read.c declare their functions below this line. */

/* pcx.fnc export 3: encodes a GFX texture source (a framebuffer capture) as a PCX file (8-bit RLE; 3 planes for
   direct colour, 1 plane plus 256-colour palette for a palette source). true: *outBytes (release with
   g_MemoryApi.free) and *outByteCount; false: error code in *outError. */
Bool8 Pcx_EncodeCapture(GraphicsCapturedTextureSourceAsset *capture,void **outBytes,uint32_t *outByteCount,
                       uint32_t *outError);

/* An 8-bit paletted PCX picture decoded by Pcx_DecodeIndexed8 (pcx.fnc export 2). */
#define PCX_PALETTE_COLOR_COUNT 256
typedef struct PcxIndexedImage {
    uint32_t width;  /* 1..65536 */
    uint32_t height; /* 1..65536 */
    /* 0xFFRRGGBB per colour, i.e. bytes blue, green, red, 0xFF in memory (the original decoder's format) */
    uint32_t paletteColors[PCX_PALETTE_COLOR_COUNT];
    uint8_t *pixels; /* width * height palette indices, top row first (g_MemoryApi allocation) */
} PcxIndexedImage;

/* pcx.fnc export 2, 8-bit paletted files only: true when fileBytes decoded into *outImage (release it with
   Pcx_FreeIndexed8); false for a file the original decoder rejected or decoded as direct colour. */
Bool8 Pcx_DecodeIndexed8(const uint8_t *fileBytes,uint32_t fileByteCount,PcxIndexedImage *outImage);
void Pcx_FreeIndexed8(PcxIndexedImage *image);

#endif
