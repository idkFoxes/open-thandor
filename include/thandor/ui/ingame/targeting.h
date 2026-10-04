/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/targeting.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_TARGETING_H
#define THANDOR_UI_INGAME_TARGETING_H

#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

void InGameTargetingContext_AdvanceOrResolveTarget(InGameTargetingRootTraversalView *targetingContext);

void InGameTargetingContext_CancelAndRestoreState(InGameTargetingRootTraversalView *targetingContext);

#endif /* THANDOR_UI_INGAME_TARGETING_H */
