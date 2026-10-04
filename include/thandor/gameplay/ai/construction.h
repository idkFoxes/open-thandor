/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/construction.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_CONSTRUCTION_H
#define THANDOR_GAMEPLAY_AI_CONSTRUCTION_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/construction. */

/* Functions are grouped by semantic ownership. */

void AiConstructionPlanner_PlaceSpecialAssetFromWorkspace
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

Bool8 AiConstructionPlanner_ProcessPendingAssetRequests
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiConstructionPlanner_PlaceTritiumStorageNearResourceSite(PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

void AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

void AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

void AiConstructionPlanner_ConsumeFactionPendingArmyAsset
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

extern ModelRuntimeSlot *g_AiWorkspaceOwnedAsset300Runtime;

#endif /* THANDOR_GAMEPLAY_AI_CONSTRUCTION_H */
