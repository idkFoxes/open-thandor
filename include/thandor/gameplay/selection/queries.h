/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/selection/queries.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SELECTION_QUERIES_H
#define THANDOR_GAMEPLAY_SELECTION_QUERIES_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/gameplay/selection/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/selection/queries. */

/* Functions are grouped by semantic ownership. */

Bool8 SelectionInfoEntitySlots_ComputeAverageWorldPosition(FixedVectorQ12 *outPosition);

Bool8 SelectionInfo_HasAnyEntry();

Bool8 SelectionInfo_AllEntriesEmptyOrMatchOwner(FactionRuntimeIndex ownerIndex);

Bool8 SelectionInfo_TestNotOwnAircraftPadsWithAircraft(FactionRuntimeIndex ownerIndex);

Bool8 SelectionInfo_TestAnyActiveOrSingleClass13();

Bool8 SelectionInfo_TestPositionCommandAtWorldPoint(Q12 worldXQ12,Q12 worldYQ12,WorldRuntimeContext *inGameRuntime);

Bool8 SelectionInfo_TestNoEntryHasWeaponDamage();

Bool8 SelectionInfo_TestAnyEntryWeaponDamageNonnegative();

GameEntityRuntime * __cdecl SelectionInfo_GetFirstEntry();

Bool8 SelectionInfo_IsEntryAbsent(GameEntityRuntime *entry);

uint32_t SelectionInfo_CollectAttachmentEffectVariantMask();

uint32_t __cdecl SelectionInfo_CollectCapabilityFlags();

extern SelectionInfoEntitySlots *g_SelectionInfoEntitySlots;

#endif /* THANDOR_GAMEPLAY_SELECTION_QUERIES_H */
