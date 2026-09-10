#ifndef THANDOR_WORLD_PATHING_INFLUENCE_H
#define THANDOR_WORLD_PATHING_INFLUENCE_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/pathing/influence. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00527330 */
void GridInfluence_AddLowDistanceBands(GameEntityRuntime *entityRuntime);

/* 0x00527380 */
void GridInfluence_RemoveLowDistanceBands(GameEntityRuntime *entityRuntime);

/* 0x00528070 */
void GridInfluence_AddHighDistanceBands(GameEntityRuntime *entityRuntime);

/* 0x005280D0 */
void GridInfluence_RemoveHighDistanceBands(GameEntityRuntime *entityRuntime);

/* 0x00527B50 */
void GridInfluence_AddNoOp(GameEntityRuntime *entityRuntime);

/* 0x00527B60 */
void GridInfluence_RemoveNoOp(GameEntityRuntime *entityRuntime);

/* 0x00535A30 */
void GridInfluence_ClearDistanceBandsAndRefreshEntities(WorldRuntimeNode *entityListHead);

/* 0x00535330 */
void GridInfluence_SetLowDistanceBandsAroundWorldPoint (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00535780 */
void GridInfluence_SetHighDistanceBandsAroundWorldPoint (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00535CC0 */
void GridInfluence_ClearLowDistanceBandsAroundWorldPoint (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00536110 */
void GridInfluence_ClearHighDistanceBandsAroundWorldPoint (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00535190 */
GridInfluenceDiagonalScanRegisterResult GridInfluence_SetLowDistanceBandsDiagonalNegative (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1, FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1, uint *scratchCell);

/* 0x00535260 */
GridInfluenceDiagonalScanRegisterResult GridInfluence_SetLowDistanceBandsDiagonalPositive (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1, FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1, uint *scratchCell);

/* 0x005355E0 */
GridInfluenceDiagonalScanRegisterResult GridInfluence_SetHighDistanceBandsDiagonalNegative (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1, FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1, uint *scratchCell);

/* 0x005356B0 */
GridInfluenceDiagonalScanRegisterResult GridInfluence_SetHighDistanceBandsDiagonalPositive (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1, FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1, uint *scratchCell);

/* 0x00535B20 */
GridInfluenceDiagonalScanRegisterResult GridInfluence_ClearLowDistanceBandsDiagonalNegative (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1, FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1, uint *scratchCell);

/* 0x00535BF0 */
GridInfluenceDiagonalScanRegisterResult GridInfluence_ClearLowDistanceBandsDiagonalPositive (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1, FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1, uint *scratchCell);

/* 0x00535F70 */
GridInfluenceDiagonalScanRegisterResult GridInfluence_ClearHighDistanceBandsDiagonalNegative (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1, FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1, uint *scratchCell);

/* 0x00536040 */
GridInfluenceDiagonalScanRegisterResult GridInfluence_ClearHighDistanceBandsDiagonalPositive (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1, FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1, uint *scratchCell);

#endif /* THANDOR_WORLD_PATHING_INFLUENCE_H */
