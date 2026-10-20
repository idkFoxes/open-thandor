/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/scenario/custom_map_validate.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_ASSETS_SCENARIO_CUSTOM_MAP_VALIDATE_H
#define THANDOR_ASSETS_SCENARIO_CUSTOM_MAP_VALIDATE_H

#include <stddef.h>
#include <stdint.h>

/* Custom maps (maps\<name>\<name>.lev, the expanded <name>.fld, map.ini; single player only) come from users and
   are untrusted. Limits of CustomMap_Validate, chosen so that every stock level and field grid passes (all 55 of
   LEVEL.PCK) and every value the level loaders use as a size, offset, count or index stays inside its table:
     - field grid sides: 4k + 3 cells (fld.py rule; every stock grid is 8k + 3), CUSTOM_MAP_FIELD_MIN_SIDE_CELLS ..
       CUSTOM_MAP_FIELD_MAX_SIDE_CELLS (the stock range 59..139: well inside the terrain renderer's 257 rows, the
       path cost queue and the package scratch buffer);
     - level prefix (header .. path strings) at most CUSTOM_MAP_LEVEL_MAX_PREFIX_BYTES (the field grid path offset
       must stay <= 0xFFFF, the frontend's "offset, not yet a pointer" rule), at most CUSTOM_MAP_LEVEL_MAX_PLACEMENTS
       initial army placements (ARMY_RUNTIME_SLOT_COUNT), at most CUSTOM_MAP_LEVEL_MAX_TABLE_RECORDS records per
       EFF/SHT/MDL/ARM file list. */
inline constexpr uint32_t CUSTOM_MAP_FIELD_MIN_SIDE_CELLS = 59;
inline constexpr uint32_t CUSTOM_MAP_FIELD_MAX_SIDE_CELLS = 139;
inline constexpr uint32_t CUSTOM_MAP_LEVEL_MAX_PREFIX_BYTES = 0x10000;
inline constexpr uint32_t CUSTOM_MAP_LEVEL_MAX_PLACEMENTS = 0x400;
inline constexpr uint32_t CUSTOM_MAP_LEVEL_MAX_TABLE_RECORDS = 64;
/* Longest path string of the level (code units without the terminator); the loader-side rewrites (extension,
   terrain material / faction suffix) need LEVEL_IMAGE_PATH_SUFFIX_UNITS more units behind it. */
inline constexpr uint32_t CUSTOM_MAP_LEVEL_MAX_PATH_UNITS = 120;

/* Checks a custom map before anything of it reaches the game: lev/levSize is the .lev file, fld/fldSize the
   expanded (loose, uncompressed) .fld file, both as read from disk. Pure function of the bytes: no game state,
   no allocation, no logging; any input (also NULL or truncated) is safe.
   Returns true when the level and the field grid are safe to load (docs/level_format.md,
   docs/field_grid_format.md and the checks in custom_map_validate.cpp). Otherwise returns false and writes a
   short English reason for the log into reason (NUL terminated, cut to reasonSize; reason may be NULL). */
bool CustomMap_Validate(const uint8_t *lev, size_t levSize, const uint8_t *fld, size_t fldSize, char *reason,
                        size_t reasonSize);

#endif /* THANDOR_ASSETS_SCENARIO_CUSTOM_MAP_VALIDATE_H */
