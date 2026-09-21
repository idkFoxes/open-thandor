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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00527330 */
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_AddLowDistanceBands(GameEntityRuntime *entityRuntime);

/* 0x00527380 */
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_RemoveLowDistanceBands(GameEntityRuntime *entityRuntime);

/* 0x00528070 */
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_AddHighDistanceBands(GameEntityRuntime *entityRuntime);

/* 0x005280D0 */
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_RemoveHighDistanceBands(GameEntityRuntime *entityRuntime);

/* 0x00527B50 */
void __thandor_void_preserve_eax_ecx_edx GridInfluence_AddNoOp(GameEntityRuntime *entityRuntime);

/* 0x00527B60 */
void __thandor_void_preserve_eax_ecx_edx GridInfluence_RemoveNoOp(GameEntityRuntime *entityRuntime);

/* 0x00535A30 */
void __thandor_preserve_eax
GridInfluence_ClearDistanceBandsAndRefreshEntities(WorldOwnerListNode100 *entityListHead);

/* 0x00535330 */
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetLowDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00535780 */
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetHighDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00535CC0 */
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearLowDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00536110 */
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearHighDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00535190 */
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell);

/* 0x00535260 */
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell);

/* 0x005355E0 */
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell);

/* 0x005356B0 */
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell);

/* 0x00535B20 */
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell);

/* 0x00535BF0 */
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell);

/* 0x00535F70 */
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell);

/* 0x00536040 */
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell);

#endif /* THANDOR_WORLD_PATHING_INFLUENCE_H */
