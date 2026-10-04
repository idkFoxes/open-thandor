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
                                                 creation when ModelDefinition.modelFlags has bit 0x100 */
/* ModelRuntimeNode.runtimeFlags bit: skipped by every model pass of the world view
   (FrontendModelPointerContext_RenderWorldViewQueuesClipped); no writer with a constant mask found */
#define MODEL_NODE_FLAG_HIDDEN 0x40
/* ModelDefinition.modelFlags bit that makes the model's root node ray transparent */
#define MODEL_DEFINITION_FLAG_RAY_TRANSPARENT 0x100
/* ModelDefinition.modelFlags bit: the energy demand of directly attached models counts
   (ModelRuntimeHierarchy_ComputeEnergyDemand, ArmyAssetHierarchy_SumEnergyFrom) */
#define MODEL_DEFINITION_FLAG_COUNT_ATTACHED_ENERGY 0x80
/* ModelRuntimeNode.runtimeFlags bit: ModelNodeRuntime_UpdateStateTintRecursive fades the alpha to 0 */
#define MODEL_NODE_FLAG_FORCE_TRANSPARENT 0x1000
/* ModelRuntimeNode.runtimeFlags bits set at creation (ModelNodeRuntime_CreateHierarchyRecursive) and read by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped and the pointer and box selection */
#define MODEL_NODE_FLAG_FACTION_OWNED 0x20       /* the owning army's factionIndex is not 0; required for picking */
#define MODEL_NODE_FLAG_SHADING_PASS 0x100       /* drawn into the generated-texture shading pass */
#define MODEL_NODE_FLAG_DRAW_BEFORE_TERRAIN 0x200 /* drawn in the model pass before the terrain pass */
/* ModelDefinition.modelFlags bits copied into the node flags at creation */
#define MODEL_DEFINITION_FLAG_NOT_REMEMBERED 0x10    /* -> TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED */
#define MODEL_DEFINITION_FLAG_DRAW_BEFORE_TERRAIN 0x20 /* -> MODEL_NODE_FLAG_DRAW_BEFORE_TERRAIN; the army runtime
                                                         also skips the field grid height stamp for it */
#define MODEL_DEFINITION_FLAG_NO_SHADING_PASS 0x40   /* clear -> MODEL_NODE_FLAG_SHADING_PASS */

/* Functions are grouped by semantic ownership. */

Bool8 ModelRuntimePool_RepairDeferredChild
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeAttachmentIndex attachmentIndex,PckModelDefinitionIdCatalog childDefinitionId,
          ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldRuntime,
          ModelRuntimeSlot **outChildModelRuntime);

uint32_t __cdecl ModelRuntimePool_Init(void);

void ModelRuntimePool_ShutdownAndReleaseDefinitions(void);

void __cdecl ModelRuntimePool_UnrebaseBeforeSave(void);

void ModelRuntimePool_RebaseAfterLoad(void);

void ModelRuntimePool_DestroyHierarchyAndDetach(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ModelRuntime_EmitProjectilesFromAttachmentPoints
          (ShotTargetModelReference targetModelReference,Q12 targetWorldZQ12,Q12 targetWorldYQ12,
          Q12 targetWorldXQ12,ShotDefinition *shotDefinition,ModelRuntimeNode *modelNodeRuntime,
          MdlSerializedNodeHeader *definitionNode,WorldRuntimeContext *worldRuntime);

uint32_t ModelRuntimePool_CreateInstanceByDefinitionId
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ArmyRuntimeSlot *armyRuntime,PckModelDefinitionIdCatalog modelDefinitionId,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot **outModelRuntime);

extern ModelRuntimeSlot *g_ModelRuntimeSlots;
extern intptr_t g_ModelRuntimeRebaseDelta; /* model runtime pool base - 1 */

#endif /* THANDOR_WORLD_MODEL_RUNTIME_H */
