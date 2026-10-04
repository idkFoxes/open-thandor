/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/model_tint.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/core/model_tint.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/core/model_tint. */

/* Re-tints a world model (army, effect or shot) after its runtime state bits changed: the tint chosen by
   ModelRuntimeNode_GetStateTintArgb from runtimeFlags 0x04/0x08/0x10 is applied to the whole hierarchy only when it
   differs from the tint the model already has.
*/
void ModelNodeRuntime_RefreshStateTint(ModelRuntimeNode *modelNode)

{
  PackedArgb32 tintArgb;
  
  tintArgb = ModelRuntimeNode_GetStateTintArgb(modelNode);
  if (tintArgb != modelNode->tintArgb) {
    ModelNodeRuntime_ApplyTintRecursive(tintArgb,modelNode);
  }
  return;
}

/* Tint of a world model (both callers pass a ModelRuntimeNode) from its terrain-derived runtimeFlags: 0x04 ->
   opaque white (unchanged colours), else without 0x08 -> 0 (black), with 0x08 and 0x10 -> 0x00FFFFFF, with
   0x08 only -> opaque grey 0x878787.
*/
PackedArgb32 ModelRuntimeNode_GetStateTintArgb(ModelRuntimeNode *node)

{
  ModelRuntimeFlags stateFlags;

  stateFlags = node->runtimeFlags;
  if ((stateFlags & TERRAIN_OCCUPANCY_FLAG_PRESENT) != 0) {
    return UI_MODEL_TINT_OPAQUE_WHITE;
  }
  if ((stateFlags & TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE) == 0) {
    return 0;
  }
  if ((stateFlags & TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED) != 0) {
    return UI_MODEL_TINT_TRANSPARENT_WHITE;
  }
  return UI_MODEL_TINT_OPAQUE_GREY;
}
