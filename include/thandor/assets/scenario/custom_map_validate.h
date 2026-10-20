/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/scenario/custom_map_validate.h
 */

#ifndef THANDOR_ASSETS_SCENARIO_CUSTOM_MAP_VALIDATE_H
#define THANDOR_ASSETS_SCENARIO_CUSTOM_MAP_VALIDATE_H

#include <stddef.h>
#include <stdint.h>

/* STUB (experimental/custom-maps): placeholder until the real check of experimental/custom-maps-validate is merged;
   that branch replaces this header and src/assets/scenario/custom_map_validate.cpp (same paths, same signature).
   Checks an untrusted custom map: the .lev image (levSize bytes) and the expanded .fld image (fldSize bytes).
   Returns true when the game may load it; false with a one-line reason (NUL-terminated, cut to reasonSize). */
bool CustomMap_Validate(const uint8_t *lev,size_t levSize,const uint8_t *fld,size_t fldSize,char *reason,
                        size_t reasonSize);

#endif /* THANDOR_ASSETS_SCENARIO_CUSTOM_MAP_VALIDATE_H */
