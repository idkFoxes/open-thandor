/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/runtime/core.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/runtime/core.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/runtime/core. */

/* Attaches a field grid ('fld' asset) to the world and computes its triangle normals; any other asset is
   ignored.
*/
void WorldRuntime_AttachFieldGridAsset(FieldGridAsset *asset,WorldRuntimeContext *world)

{
  if (asset->common.magic == ASSET_MAGIC_FLD) {
    world->fieldGrid = asset;
    FieldGrid_RecomputeInteriorTriangleNormalAngles(asset);
    WorldRuntime_ClearFieldGridDirtyFlag(world);
  }
  return;
}

/* Returns the field grid's top surface height (terrain plus the water above it) at a world point, or
   WORLD_HEIGHT_NO_FIELD_GRID when the world has no field grid.
*/
uint32_t WorldRuntime_InterpolateTopSurfaceHeightOrSentinel
                (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  Q12 topSurfaceHeightQ12;

  topSurfaceHeightQ12 = WORLD_HEIGHT_NO_FIELD_GRID;
  if (worldRuntime->fieldGrid != NULL) {
    FieldGrid_InterpolateTopSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid,&topSurfaceHeightQ12);
  }
  return topSurfaceHeightQ12;
}

/* Attaches a caller-owned workspace of count pointer-sized words to the world runtime and zeroes it.
*/
void WorldRuntime_AttachAndClearDwordArray(WorldWorkspaceElementCount count,uintptr_t *array,WorldRuntimeContext *world)

{
  world->dwordArray = array;
  world->dwordArrayCount = count;
  for (; count != 0; count--) {
    *array = 0;
    array = array + 1;
  }
  return;
}
