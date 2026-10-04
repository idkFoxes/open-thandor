/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/color_lanes.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_COLOR_LANES_H
#define THANDOR_CORE_COLOR_LANES_H

/* Not functions of the original: C stand-ins for the MMX colour sequences the original inlines wherever it
   shades a packed ARGB colour (shot and effect tints, world and model lighting, terrain projection and
   composite texture, shadow texture). They used to be repeated as file-local helpers in each of those
   sources. */

#include <stdint.h>
#include <thandor/core/x86_emulation.h>

/* MOVD mm,value; PUNPCKLBW mm,mm; PSRLW mm,shift: byte k of value becomes word lane k = ((b << 8) | b) >> shift
   (b * 0x101 widens a byte to the full 16-bit range), e.g. shift 4 turns a colour channel into a Q12 factor
   (0xFF -> 0x0FFF) for PMULHW. */
static inline uint64_t ColorLanes_UnpackBytesShiftRight(uint32_t value,int shift)

{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane++) {
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * 0x101) >> shift);
  }
  return lanes.q;
}

/* PACKUSWB mm,mm; MOVD dword,mm: the four signed word lanes saturated to unsigned bytes (below 0 -> 0, above
   0xff -> 0xff), word lane k -> byte k, i.e. shaded colour lanes back into one packed ARGB colour. */
static inline uint32_t ColorLanes_PackWordsUnsignedSaturate(uint64_t words)

{
  ThandorMmx lanes;
  uint32_t packed;
  int lane;

  lanes.q = words;
  packed = 0;
  for (lane = 0; lane < 4; lane++) {
    packed = packed |
             (uint32_t)(lanes.sw[lane] < 0 ? 0 : (0xff < lanes.sw[lane] ? 0xff : lanes.sw[lane])) << (lane * 8);
  }
  return packed;
}

#endif /* THANDOR_CORE_COLOR_LANES_H */
