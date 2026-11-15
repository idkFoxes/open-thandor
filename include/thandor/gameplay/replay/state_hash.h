/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/replay/state_hash.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_GAMEPLAY_REPLAY_STATE_HASH_H
#define THANDOR_GAMEPLAY_REPLAY_STATE_HASH_H

#include <stdint.h>
#include <stdio.h>

/* Not in the original: the hash of the simulation state that the determinism test (OPEN_THANDOR_STATEHASH,
   platform/debug/statehash.cpp) writes per step and the game recorder (gameplay/replay/runtime.h) checks per event.
   FNV-1a over 32-bit words of: the simulation tick, the secondary random seed, every world object in update order
   (class, world position), every army by slot (asset, faction, flags, orders, generation, AI state, targets as slot
   indices, root model health), the faction resources and, in the improved pathing mode, the pathing side table.
   Pointers never go in (addresses differ between builds), only pool indices. Compiled in every build (the recorder
   is not a developer tool); the values are the ones the determinism references were made with. */

struct GameStateHashCounts {
  unsigned armies;  /* armies with a model node */
  unsigned objects; /* nodes of the world owner list */
};

/* The hash of the current world (g_InGameRuntimeRoot must be set). counts (may be nullptr) receives the counts;
   detail (may be nullptr) receives one line per object and army, the OPEN_THANDOR_STATEHASH_DETAIL dump. */
uint64_t GameStateHash_Compute(GameStateHashCounts *counts, FILE *detail);

/* FNV-1a step of one 32-bit word (little-endian bytes), the hash's building block. */
inline uint64_t GameStateHash_AddWord(uint64_t hash, uint32_t word)
{
  for (int byteIndex = 0; byteIndex < 4; byteIndex++) {
    hash ^= (word >> (byteIndex * 8)) & 0xffu;
    hash *= 0x100000001b3ull;
  }
  return hash;
}

inline constexpr uint64_t GAME_STATE_HASH_SEED = 0xcbf29ce484222325ull;

#endif /* THANDOR_GAMEPLAY_REPLAY_STATE_HASH_H */
