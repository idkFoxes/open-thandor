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

/* g_ModelRuntimeSlots: a 0x400000-byte pool of 0x200-byte ModelRuntimeSlot entries (ModelRuntimePool_Init) */
#define MODEL_RUNTIME_SLOT_COUNT 0x2000
#define MODEL_RUNTIME_POOL_BYTES 0x400000 /* MODEL_RUNTIME_SLOT_COUNT * sizeof(ModelRuntimeSlot) */
/* pointer slots in g_ModelDefinitionRegistry */
#define MODEL_DEFINITION_REGISTRY_SLOT_COUNT 0x300
/* ModelRuntimeNode.runtimeFlags bits */
#define MODEL_NODE_FLAG_TRANSFORM_DIRTY 0x1 /* local translation/rotation changed: world transform is rebuilt */
#define MODEL_NODE_FLAG_RENDERED 0x2 /* drawn in the current frame (set by the model renderers) */
#define MODEL_NODE_FLAG_RAY_TRANSPARENT 0x2000 /* skipped by ModelRuntime_RaycastCandidateListNearest; set at
                                                 creation when the definition has flag 0x100 at +0x68 */
/* ModelRuntimeNode.runtimeFlags bit: skipped by every model pass of the world view
   (FrontendModelPointerContext_RenderWorldViewQueuesClipped); no writer with a constant mask found */
#define MODEL_NODE_FLAG_HIDDEN 0x40
/* ModelDefinition flags dword (+0x68, modelFlags) bit that makes the model's root node ray transparent */
#define MODEL_DEFINITION_FLAG_RAY_TRANSPARENT 0x100
/* ModelDefinition flags (+0x68) bit: the energy demand of directly attached models counts
   (ModelRuntimeHierarchy_ComputeEnergyDemand, ArmyAssetHierarchy_SumEnergyFrom) */
#define MODEL_DEFINITION_FLAG_COUNT_ATTACHED_ENERGY 0x80
/* ModelRuntimeNode.runtimeFlags bit: ModelNodeRuntime_UpdateStateTintRecursive fades the alpha to 0 */
#define MODEL_NODE_FLAG_FORCE_TRANSPARENT 0x1000
/* ModelRuntimeNode.runtimeFlags bits set at creation (ModelNodeRuntime_CreateHierarchyRecursive) and read by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped and the pointer and box selection */
#define MODEL_NODE_FLAG_FACTION_OWNED 0x20       /* the owning army's factionIndex is not 0; required for picking */
#define MODEL_NODE_FLAG_SHADING_PASS 0x100       /* drawn into the generated-texture shading pass */
#define MODEL_NODE_FLAG_DRAW_BEFORE_TERRAIN 0x200 /* drawn in the model pass before the terrain pass */
/* ModelDefinition.modelFlags (+0x68) bits copied into the node flags at creation */
#define MODEL_DEFINITION_FLAG_NOT_REMEMBERED 0x10    /* -> TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED */
#define MODEL_DEFINITION_FLAG_DRAW_BEFORE_TERRAIN 0x20 /* -> MODEL_NODE_FLAG_DRAW_BEFORE_TERRAIN; the army runtime
                                                         also skips the field grid height stamp for it */
#define MODEL_DEFINITION_FLAG_NO_SHADING_PASS 0x40   /* clear -> MODEL_NODE_FLAG_SHADING_PASS */
/* Aim tolerance of ModelNodeRuntime_SmoothYaw/PitchTowardTarget: outsideTolerance beyond +-0x3FF */
#define MODEL_AIM_TOLERANCE_ANGLE16 0x3ff
/* ModelDefinition.variantModelDefinitionIds[] entries (ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive) */
#define MODEL_TECHNOLOGY_VARIANT_COUNT 6
/* nearest distance of a ray that hit nothing (ModelRuntime_RaycastCandidateListNearest) */
#define MODEL_RAYCAST_NO_HIT_DISTANCE 0x7fffffff

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00529360 */
bool ModelRuntimePool_RepairDeferredChild
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeAttachmentIndex attachmentIndex,PckModelDefinitionIdCatalog childDefinitionId,
          ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldRuntime,
          ModelRuntimeSlot **outChildModelRuntime);

/* 0x004BDDB0 */
void ModelRuntime_CullAndRenderHierarchyRecursive(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BE270 */
void ModelRuntime_RenderHierarchyRecursiveAlternatePath(ModelRuntimeNode *modelNode);

/* 0x0050B440 */
bool ModelRuntime_RaycastCandidateListNearest
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 maximumDistanceQ12,Q12 originZQ12
          ,Q12 originYQ12,Q12 originXQ12,WorldOwnerRuntimeClassId requiredOwnerId,
          ModelRuntimeNode *excludedNode,WorldRuntimeContext *worldRuntime,Q12 *outNearestDistanceQ12,
          ModelRuntimeNode **outNearestModelNode);

/* 0x0051C260 */
Q12 ModelRuntime_QueryHierarchyConditionRatioQ12(RuntimeModelFactionPrefix *runtimeEntry);

/* 0x0051C280 */
int ModelRuntime_QueryActiveHierarchyMetric(ArmyRuntimeSlot *armyRuntime);

/* 0x0051C2A0 */
ModelHierarchyEnergyDemand
ModelRuntime_QueryHierarchyEnergyDemand(RuntimeModelFactionPrefix *runtimeEntry);

/* 0x00528A40 */
uint32_t __cdecl ModelRuntimePool_Init(void);

/* 0x00528A70 */
void ModelRuntimePool_ShutdownAndReleaseDefinitions(void);

/* 0x00528B30 */
void __cdecl ModelRuntimePool_UnrebaseBeforeSave(void);

/* 0x00528CF0 */
void ModelRuntimePool_RebaseAfterLoad(void);

/* 0x00529560 */
void ModelRuntimePool_DestroyHierarchyAndDetach(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

/* 0x00529690 */
void ModelRuntime_EmitProjectilesFromAttachmentPoints
          (ShotTargetModelReference targetModelReference,Q12 targetWorldZQ12,Q12 targetWorldYQ12,
          Q12 targetWorldXQ12,ShotDefinition *shotDefinition,ModelRuntimeNode *modelNodeRuntime,
          MdlSerializedNodeHeader *definitionNode,WorldRuntimeContext *worldRuntime);

/* 0x00529140 */
uint32_t ModelRuntimePool_CreateInstanceByDefinitionId
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ArmyRuntimeSlot *armyRuntime,PckModelDefinitionIdCatalog modelDefinitionId,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot **outModelRuntime);

#endif /* THANDOR_WORLD_MODEL_RUNTIME_H */
