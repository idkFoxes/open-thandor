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

/* The influence radius is ModelDefinition.footprintRadius (+0xDC); 0 = no grid influence.
   Placement and selection read the same field. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

void GridInfluence_AddLowDistanceBands(GameEntityRuntime *entityRuntime);

void GridInfluence_RemoveLowDistanceBands(GameEntityRuntime *entityRuntime);

void GridInfluence_AddHighDistanceBands(GameEntityRuntime *entityRuntime);

void GridInfluence_RemoveHighDistanceBands(GameEntityRuntime *entityRuntime);

void GridInfluence_AddNoOp(GameEntityRuntime *entityRuntime);

void GridInfluence_RemoveNoOp(GameEntityRuntime *entityRuntime);

void GridInfluence_ClearDistanceBandsAndRefreshEntities(WorldOwnerListNode *entityListHead);

void GridInfluence_SetLowDistanceBandsAroundWorldPoint(FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

void GridInfluence_SetHighDistanceBandsAroundWorldPoint(FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

void GridInfluence_ClearLowDistanceBandsAroundWorldPoint(FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

void GridInfluence_ClearHighDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

int GridInfluence_SetLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

int GridInfluence_SetLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

int GridInfluence_SetHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

int GridInfluence_SetHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

int GridInfluence_ClearLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

int GridInfluence_ClearLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

int GridInfluence_ClearHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

int GridInfluence_ClearHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell);

#endif /* THANDOR_WORLD_PATHING_INFLUENCE_H */
