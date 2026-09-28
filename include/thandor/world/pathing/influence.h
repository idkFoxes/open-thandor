/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/influence.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_PATHING_INFLUENCE_H
#define THANDOR_WORLD_PATHING_INFLUENCE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/pathing/influence. */

/* Byte offset of the model radius in a model definition record (definitionOrClassRecord); 0 = no grid influence.
   Placement and selection read the same field. */
#define MODEL_DEFINITION_RADIUS_OFFSET 0xdc
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
void GridInfluence_ClearDistanceBandsAndRefreshEntities(WorldOwnerListNode100 *entityListHead);

/* 0x00535330 */
void GridInfluence_SetLowDistanceBandsAroundWorldPoint(FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00535780 */
void GridInfluence_SetHighDistanceBandsAroundWorldPoint(FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00535CC0 */
void GridInfluence_ClearLowDistanceBandsAroundWorldPoint(FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00536110 */
void GridInfluence_ClearHighDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00535190 */
int GridInfluence_SetLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

/* 0x00535260 */
int GridInfluence_SetLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

/* 0x005355E0 */
int GridInfluence_SetHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

/* 0x005356B0 */
int GridInfluence_SetHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

/* 0x00535B20 */
int GridInfluence_ClearLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

/* 0x00535BF0 */
int GridInfluence_ClearLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

/* 0x00535F70 */
int GridInfluence_ClearHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

/* 0x00536040 */
int GridInfluence_ClearHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

#endif /* THANDOR_WORLD_PATHING_INFLUENCE_H */
