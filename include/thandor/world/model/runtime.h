/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/model/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_MODEL_RUNTIME_H
#define THANDOR_WORLD_MODEL_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/model/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00529360 */
ModelNodeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelRuntimePool_RepairDeferredChild
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeAttachmentIndex attachmentIndex,PckModelDefinitionIdCatalog childDefinitionId,
          ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldRuntime);

/* 0x004BDDB0 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntime_CullAndRenderHierarchyRecursive(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BE270 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntime_RenderHierarchyRecursiveAlternatePath(ModelRuntimeNode *modelNode);

/* 0x0050B440 */
ModelRaycastNearestHitEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
ModelRuntime_RaycastCandidateListNearestCf
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 maximumDistanceQ12,Q12 originZQ12
          ,Q12 originYQ12,Q12 originXQ12,ModelRuntimeClassId requiredOwnerId,
          ModelRuntimeNode *excludedNode,WorldRuntimeContext *worldRuntime);

/* 0x0051C240 */
Q12 __thandor_eax_preserve_ecx_edx
ModelRuntime_QueryHierarchyScaleRatioQ12(RuntimeModelFactionPrefix10 *runtimeEntry);

/* 0x0051C260 */
ModelRuntimeScaleRatioRegisterPairQ12 __thandor_eax_edx_cf_preserve_ecx
ModelRuntime_QueryHierarchyScaleRatioQ12Regs(RuntimeModelFactionPrefix10 *runtimeEntry);

/* 0x0051C280 */
int __thandor_eax_preserve_ecx_edx
ModelRuntime_QueryActiveHierarchyMetric(ArmyRuntimeSlot *modelRuntimeHolder);

/* 0x0051C2A0 */
ModelRuntimeActiveTotalMetricRegisterPair
ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(RuntimeModelFactionPrefix10 *runtimeEntry);

/* 0x00528A40 */
StatusValueEaxCf5 __cdecl ModelRuntimePool_Init(void);

/* 0x00528A70 */
void __thandor_void_preserve_eax_ecx_edx ModelRuntimePool_ShutdownAndReleaseDefinitions(void);

/* 0x00528B30 */
void __cdecl ModelRuntimePool_UnrebaseBeforeSave(void);

/* 0x00528CF0 */
void __thandor_void_preserve_eax_ecx_edx ModelRuntimePool_RebaseAfterLoad(void);

/* 0x00529560 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimePool_DestroyHierarchyAndDetach
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

/* 0x00529690 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntime_EmitProjectilesFromAttachmentPoints
          (ShotRuntimeState14 shotRuntimeState14,Q12 targetWorldZQ12,Q12 targetWorldYQ12,
          Q12 targetWorldXQ12,ShotDefinition *shotDefinition,ModelRuntimeNode *modelNodeRuntime,
          MdlSerializedNodeHeader38 *definitionNode,WorldRuntimeContext *worldRuntime);

/* 0x00529140 */
ModelNodeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelRuntimePool_CreateInstanceByDefinitionIdCf
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ArmyRuntimeSlot *armyRuntime,PckModelDefinitionIdCatalog modelDefinitionId,
          WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_WORLD_MODEL_RUNTIME_H */
