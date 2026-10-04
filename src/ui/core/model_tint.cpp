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
