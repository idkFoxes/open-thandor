/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/objective_overlay_stub.cpp
 * Project code (not in the original game)
 */

/* TEMPORARY stub (objective overlay package O4): lets branch objective-overlay/ov-o4 link on its own until the
   setting "Ziel anzeigen" ([game] objective_overlay, package O5) provides ObjectiveOverlay_Enabled. Remove this file
   (and its CMakeLists.txt line) when merging O5. The overlay stays hidden without lines, which only the objective
   update (O3/O5) or the developer test aid OPEN_THANDOR_OBJECTIVE_SAMPLE sets. */

#include <thandor/ui/ingame/objective_overlay.h>

bool ObjectiveOverlay_Enabled()
{
    return true;
}
