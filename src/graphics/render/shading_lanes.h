/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/shading_lanes.h
 * Reverse engineering by idkFoxes 2026
 */

/* Private helpers shared by the graphics/render sources (static inline). */

#ifndef THANDOR_GRAPHICS_RENDER_SHADING_LANES_H
#define THANDOR_GRAPHICS_RENDER_SHADING_LANES_H

#include <thandor/thandor.h>

/* Not in the original: C stand-in for MOVD mm,packed; PUNPCKLBW mm,mm; PSRLW mm,shift. Every byte b of
   packed becomes the word lane ((b << 8) | b) >> shift (byte k -> word lane k), which turns a packed
   colour into four fixed-point factors for PMULHW. */
static __inline uint64_t Shading_DuplicateBytesToWordLanes(uint32_t packed,int shift)
{
  uint64_t lanes;
  uint32_t laneByte;
  int lane;

  lanes = 0;
  for (lane = 0; lane < 4; lane++) {
    laneByte = (packed >> (lane * 8)) & ARGB8888_CHANNEL_MASK;
    lanes = lanes | ((uint64_t)((((laneByte << 8) | laneByte) >> shift) & 0xffff) << (lane * 16));
  }
  return lanes;
}

#endif /* THANDOR_GRAPHICS_RENDER_SHADING_LANES_H */
