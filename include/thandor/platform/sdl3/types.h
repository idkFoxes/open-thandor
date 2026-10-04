/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/sdl3/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_SDL3_TYPES_H
#define THANDOR_PLATFORM_SDL3_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/graphics/resources/types.h>

/* Types (split out by tools/dev/split_types.py). */

typedef struct GraphicsCapturedTextureSourceAsset GraphicsCapturedTextureSourceAsset, *PGraphicsCapturedTextureSourceAsset;

using TimerFrequencyHz = uint32_t;

struct GraphicsCapturedTextureSourceAsset {
    struct GeneratedAssetCommonPrefix common; 
    struct GraphicsTextureSourceTableDescriptor tableDescriptor;
    uint32_t unusedHeaderDwordBC; // See GraphicsTextureSourceAsset.unusedHeaderDwordBC.
    uint8_t reservedC0_FF[64];
    char unusedText[256]; // See GraphicsTextureSourceAsset.unusedText; the capture functions clear its first byte.
    struct GraphicsTextureSourceEntry sourceEntry;
    uint32_t argb8888Pixels[1];
};

#endif /* THANDOR_PLATFORM_SDL3_TYPES_H */
