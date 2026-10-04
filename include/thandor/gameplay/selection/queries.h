/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/selection/queries.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SELECTION_QUERIES_H
#define THANDOR_GAMEPLAY_SELECTION_QUERIES_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/selection/queries. */

/* Functions are grouped by semantic ownership. */

Bool8 SelectionInfoEntitySlots_ComputeAverageWorldPosition(FixedVectorQ12 *outPosition);

Bool8 SelectionInfo_HasAnyEntry(void);

Bool8 SelectionInfo_AllEntriesEmptyOrMatchOwner(FactionRuntimeIndex ownerIndex);

Bool8 SelectionInfo_TestNotOwnAircraftPadsWithAircraft(FactionRuntimeIndex ownerIndex);

Bool8 SelectionInfo_TestAnyActiveOrSingleClass13(void);

Bool8 SelectionInfo_TestPositionCommandAtWorldPoint(Q12 worldXQ12,Q12 worldYQ12,WorldRuntimeContext *inGameRuntime);

Bool8 SelectionInfo_TestNoEntryHasWeaponDamage(void);

Bool8 SelectionInfo_TestAnyEntryWeaponDamageNonnegative(void);

GameEntityRuntime * __cdecl SelectionInfo_GetFirstEntry(void);

Bool8 SelectionInfo_IsEntryAbsent(GameEntityRuntime *entry);

uint32_t SelectionInfo_CollectAttachmentEffectVariantMask(void);

uint32_t __cdecl SelectionInfo_CollectCapabilityFlags(void);

extern SelectionInfoEntitySlots *g_SelectionInfoEntitySlots;

#endif /* THANDOR_GAMEPLAY_SELECTION_QUERIES_H */
