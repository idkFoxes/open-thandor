/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/faction/army_stock.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_FACTION_ARMY_STOCK_H
#define THANDOR_GAMEPLAY_FACTION_ARMY_STOCK_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/faction/army_stock. */

/* GameFactionRuntimeRecord army-asset lists (64 entries each): secondaryArmyAssetPointersOrIds is the production
   queue, primaryArmyAssetPointersOrIds the finished armies waiting for placement. */
#define FACTION_ARMY_ASSET_LIST_CAPACITY 64

/* Functions are grouped by semantic ownership. */

void GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(void *runtimeGroupMember);

Bool8 FactionRuntime_IsArmyAssetNotPending
          (FactionRuntimeIndex factionIndex,ArmyAssetRecordPrefix *armyAssetRecord);

void GameFactionRuntime_RegisterArmyAssetPointers(uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount repetitionCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

void GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
          (uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount requestedCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

void GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex);

void GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedConsumeArgument0,uint32_t unusedConsumeArgument1
          ,FactionRuntimeIndex factionIndex);

void GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
          (uint32_t unusedPlayerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex);

extern GameFactionRuntimeImage g_GameFactionRuntimeImage;

#endif /* THANDOR_GAMEPLAY_FACTION_ARMY_STOCK_H */
