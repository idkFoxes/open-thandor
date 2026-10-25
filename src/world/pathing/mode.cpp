/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/mode.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <string.h>
#include <thandor/world/pathing/mode.h>
#include <thandor/core/settings/original_mode.h>

/* open-thandor: the session's pathing mode (docs/plans/pathfinding.md 4). */
PathingMode g_PathingMode = PATHING_MODE_ORIGINAL;
PathingMode g_PathingModeSetting = PATHING_MODE_IMPROVED;
bool g_PathingNetworkImprovedAgreed = false;
PathingUnitState g_PathingUnitState[ARMY_RUNTIME_SLOT_COUNT];

PathingMode PathingMode_EffectiveSetting()
{
  /* "Original Thandor" overrides the setting without changing it (the own setting applies again when it is off) */
  return OtFeature_Active(OT_FEATURE_IMPROVED_PATHING) ? g_PathingModeSetting : PATHING_MODE_ORIGINAL;
}

void PathingMode_ApplyForNewSession(bool networked)

{
  if (networked) {
    g_PathingMode = g_PathingNetworkImprovedAgreed ? PATHING_MODE_IMPROVED : PATHING_MODE_ORIGINAL;
  }
  else {
    g_PathingMode = PathingMode_EffectiveSetting();
  }
  memset(g_PathingUnitState,0,sizeof(g_PathingUnitState));
}

void PathingMode_SetLoaded(PathingMode mode)

{
  g_PathingMode = (mode == PATHING_MODE_IMPROVED) ? PATHING_MODE_IMPROVED : PATHING_MODE_ORIGINAL;
}

void PathingUnitState_Clear(int slotIndex)

{
  if ((slotIndex >= 0) && (slotIndex < ARMY_RUNTIME_SLOT_COUNT)) {
    memset(&g_PathingUnitState[slotIndex],0,sizeof(g_PathingUnitState[slotIndex]));
  }
}
