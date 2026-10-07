/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/editor_army_cycling.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_EDITOR_ARMY_CYCLING_H
#define THANDOR_UI_INGAME_EDITOR_ARMY_CYCLING_H

#include <thandor/assets/army/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* The placement lists search ids 0..0xFFF and wrap around at 0x1000. */
inline constexpr int32_t ARMY_ASSET_EDITOR_ID_LIMIT = 0x1000;

ArmyAssetId ArmyAssetRegistry_NormalizeIdToPlaceableUnit(PckArmyAssetIdCatalog recordId);

ArmyAssetId ArmyAssetRegistry_StepForwardPlaceableUnit(ArmyAssetId recordId);

ArmyAssetId ArmyAssetRegistry_StepBackwardPlaceableUnit(ArmyAssetId recordId);

ArmyAssetId ArmyAssetRegistry_FindPreviousPlaceableUnitWrapped(ArmyAssetId recordId);

ArmyAssetId ArmyAssetRegistry_NormalizeIdToPlaceableObject(PckArmyAssetIdCatalog recordId);

ArmyAssetId ArmyAssetRegistry_StepForwardPlaceableObject(ArmyAssetId recordId);

ArmyAssetId ArmyAssetRegistry_StepBackwardPlaceableObject(ArmyAssetId recordId);

ArmyAssetId ArmyAssetRegistry_FindPreviousPlaceableObjectWrapped(ArmyAssetId recordId);

ArmyAssetId ArmyAssetRegistry_FindNextPlaceableUnitWrapped(ArmyAssetId recordId);

ArmyAssetId ArmyAssetRegistry_FindNextPlaceableObjectWrapped(ArmyAssetId recordId);

#endif /* THANDOR_UI_INGAME_EDITOR_ARMY_CYCLING_H */
