/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/mode.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <string.h>
#include <stdlib.h>
#include <thandor/world/pathing/mode.h>

/* open-thandor: the session's pathing mode (docs/plans/pathfinding.md 4). */
PathingMode g_PathingMode = PATHING_MODE_ORIGINAL;
PathingMode g_PathingModeSetting = PATHING_MODE_IMPROVED;
bool g_PathingNetworkImprovedAgreed = false;
PathingUnitState g_PathingUnitState[ARMY_RUNTIME_SLOT_COUNT];

/* Developer tools only (tests): OPEN_THANDOR_PATHING=original|improved (or 0|1) overrides the mode of a local
   session. Returns false (mode unchanged) without the variable or in a build without the developer tools. */
static bool PathingMode_DevOverride(PathingMode *mode)

{
#ifdef THANDOR_DEV_TOOLS
  const char *value = getenv("OPEN_THANDOR_PATHING");

  if (value == nullptr) {
    return false;
  }
  if ((strcmp(value,"original") == 0) || (strcmp(value,"0") == 0)) {
    *mode = PATHING_MODE_ORIGINAL;
    return true;
  }
  if ((strcmp(value,"improved") == 0) || (strcmp(value,"1") == 0)) {
    *mode = PATHING_MODE_IMPROVED;
    return true;
  }
#else
  (void)mode;
#endif
  return false;
}

void PathingMode_ApplyForNewSession(bool networked)

{
  if (networked) {
    g_PathingMode = g_PathingNetworkImprovedAgreed ? PATHING_MODE_IMPROVED : PATHING_MODE_ORIGINAL;
  }
  else {
    g_PathingMode = g_PathingModeSetting;
    PathingMode_DevOverride(&g_PathingMode);
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
