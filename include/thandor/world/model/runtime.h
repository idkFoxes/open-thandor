/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/model/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_MODEL_RUNTIME_H
#define THANDOR_WORLD_MODEL_RUNTIME_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/model/types.h>
#include <thandor/core/contracts.h>

/* g_ModelRuntimeSlots: a 0x400000-byte pool of 0x200-byte ModelRuntimeSlot entries (ModelRuntimePool_Init) */
inline constexpr int MODEL_RUNTIME_SLOT_COUNT = 0x2000;
inline constexpr int MODEL_RUNTIME_POOL_BYTES = 0x400000; /* MODEL_RUNTIME_SLOT_COUNT * sizeof(ModelRuntimeSlot) */
/* pointer slots in g_ModelDefinitionRegistry */
inline constexpr int MODEL_DEFINITION_REGISTRY_SLOT_COUNT = 0x300;
/* The ModelRuntimeNode.runtimeFlags bits MODEL_NODE_FLAG_* are ModelRuntimeFlags (gameplay/army/types.h). */
/* The ModelDefinition.modelFlags bits MODEL_DEFINITION_FLAG_* are ModelDefinitionFlags (gameplay/army/types.h). */

bool ModelRuntimePool_RepairDeferredChild
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeAttachmentIndex attachmentIndex,PckModelDefinitionIdCatalog childDefinitionId,
          ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldRuntime,
          ModelRuntimeSlot **outChildModelRuntime);

uint32_t ModelRuntimePool_Init();

void ModelRuntimePool_ShutdownAndReleaseDefinitions();

void ModelRuntimePool_UnrebaseBeforeSave();

void ModelRuntimePool_RebaseAfterLoad();

void ModelRuntimePool_DestroyHierarchyAndDetach(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

uint32_t ModelRuntimePool_CreateInstanceByDefinitionId
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ArmyRuntimeSlot *armyRuntime,PckModelDefinitionIdCatalog modelDefinitionId,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot **outModelRuntime);

extern ModelRuntimeSlot *g_ModelRuntimeSlots;
extern intptr_t g_ModelRuntimeRebaseDelta; /* model runtime pool base - 1 */

#endif /* THANDOR_WORLD_MODEL_RUNTIME_H */
