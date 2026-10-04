/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/army/editor_cycling.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_ARMY_EDITOR_CYCLING_H
#define THANDOR_ASSETS_ARMY_EDITOR_CYCLING_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/army/editor_cycling. */

/* The placement lists search ids 0..0xFFF and wrap around at 0x1000. */
#define ARMY_ASSET_EDITOR_ID_LIMIT 0x1000

/* Functions are grouped by semantic ownership. */

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

#endif /* THANDOR_ASSETS_ARMY_EDITOR_CYCLING_H */
