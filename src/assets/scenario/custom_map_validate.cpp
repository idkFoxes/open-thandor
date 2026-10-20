/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/scenario/custom_map_validate.cpp
 */

/* STUB (experimental/custom-maps): accepts every map. experimental/custom-maps-validate replaces this file with the
   real check (same path, same signature); until then only the custom map scan's own checks apply
   (src/assets/scenario/custom_maps.cpp). */

#include <thandor/assets/scenario/custom_map_validate.h>

bool CustomMap_Validate(const uint8_t *lev,size_t levSize,const uint8_t *fld,size_t fldSize,char *reason,
                        size_t reasonSize)
{
  (void)lev;
  (void)levSize;
  (void)fld;
  (void)fldSize;
  if ((reason != nullptr) && (reasonSize != 0)) {
    reason[0] = '\0';
  }
  return true;
}
