/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/input/targeting.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_INPUT_TARGETING_H
#define THANDOR_GAMEPLAY_INPUT_TARGETING_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/input/targeting. */

/* Functions are grouped by semantic ownership. */

void InGameTargetingContext_AdvanceOrResolveTarget(InGameTargetingRootTraversalView *targetingContext);

void InGameTargetingContext_CancelAndRestoreState(InGameTargetingRootTraversalView *targetingContext);

#endif /* THANDOR_GAMEPLAY_INPUT_TARGETING_H */
