/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/mode.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <stdlib.h>
#include <string.h>
#include <thandor/world/pathing/mode.h>

/* open-thandor: the session's pathing mode (docs/plans/pathfinding.md 4). */
PathingMode g_PathingMode = PATHING_MODE_ORIGINAL;
PathingMode g_PathingModeSetting = PATHING_MODE_IMPROVED;
bool g_PathingNetworkImprovedAgreed = false;
PathingUnitState g_PathingUnitState[ARMY_RUNTIME_SLOT_COUNT];

void PathingMode_ApplyForNewSession(bool networked)

{
  if (networked) {
    g_PathingMode = g_PathingNetworkImprovedAgreed ? PATHING_MODE_IMPROVED : PATHING_MODE_ORIGINAL;
  }
  else {
    g_PathingMode = g_PathingModeSetting;
  }
  g_PathingMode = PathingMode_DevOverride(g_PathingMode);
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

PathingMode PathingMode_DevOverride(PathingMode mode)

{
#ifdef THANDOR_DEV_TOOLS
  /* open-thandor test aid: OPEN_THANDOR_PATHING=original|improved (tools/test/run_pathing.py, run_determinism.py) */
  const char *text = getenv("OPEN_THANDOR_PATHING");
  if (text != nullptr) {
    if (strcmp(text,"original") == 0) {
      return PATHING_MODE_ORIGINAL;
    }
    if (strcmp(text,"improved") == 0) {
      return PATHING_MODE_IMPROVED;
    }
  }
#endif
  return mode;
}
