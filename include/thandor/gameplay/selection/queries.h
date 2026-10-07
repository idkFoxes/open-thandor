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

bool SelectionInfoEntitySlots_ComputeAverageWorldPosition(FixedVectorQ12 *outPosition);

bool SelectionInfo_HasAnyEntry();

bool SelectionInfo_AllEntriesEmptyOrMatchOwner(FactionRuntimeIndex ownerIndex);

bool SelectionInfo_TestNotOwnAircraftPadsWithAircraft(FactionRuntimeIndex ownerIndex);

bool SelectionInfo_TestAnyActiveOrSingleClass13();

bool SelectionInfo_TestPositionCommandAtWorldPoint(Q12 worldXQ12,Q12 worldYQ12,WorldRuntimeContext *inGameRuntime);

bool SelectionInfo_TestNoEntryHasWeaponDamage();

bool SelectionInfo_TestAnyEntryWeaponDamageNonnegative();

GameEntityRuntime * SelectionInfo_GetFirstEntry();

bool SelectionInfo_IsEntryAbsent(GameEntityRuntime *entry);

uint32_t SelectionInfo_CollectAttachmentEffectVariantMask();

uint32_t SelectionInfo_CollectCapabilityFlags();

extern SelectionInfoEntitySlots *g_SelectionInfoEntitySlots;

#endif /* THANDOR_GAMEPLAY_SELECTION_QUERIES_H */
