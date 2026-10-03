/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/shots/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_SHOTS_RUNTIME_H
#define THANDOR_WORLD_SHOTS_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/shots/runtime. */
#define SHOT_RUNTIME_SLOT_COUNT 0x1000 /* g_ShotRuntimeSlots, 0x40-byte slots */
#define SHOT_RUNTIME_POOL_BYTES 0x40000 /* SHOT_RUNTIME_SLOT_COUNT * sizeof(ShotRuntimeSlot) */
/* Functions are grouped by semantic ownership. */

void ShotRuntime_ApplyArmyHitRelationAndNotifications(ModelRuntimeSlot *targetModelRuntime,ShotRuntimeSlot *shotRuntime);

Bool8 ShotRuntime_InitGraphicsResources(uint16_t *mutableBasePath,uint32_t *outError);

void ShotRuntime_ShutdownGraphicsResources(void);

ShotDefinition *ShotRuntime_FindDefinitionById(PckShotDefinitionIdCatalog definitionId);

void ShotRuntime_RebaseSlotsAfterLoad(void);

void ShotRuntimePool_CreateProjectileFromDefinition
          (ShotTargetModelReference targetModelReference,ArmyRuntimeSlot *ownerArmyRuntime,
          Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,Q12 launchWorldZQ12,
          Q12 launchWorldYQ12,Q12 launchWorldXQ12,ShotDefinition *shotDefinition,
          WorldRuntimeContext *worldRuntime);

void ShotRuntime_PostImpactRelationNotificationNoOp(ShotRuntimeSlot *shotRuntime,WorldRuntimeContext *worldRuntime);

extern GraphicsTextureSet *g_ShotTextureSet;
extern GraphicsPaletteAsset *g_ShotPalette;
extern ShotRuntimeSlot *g_ShotRuntimeSlots;
extern uint8_t *g_ShotRuntimeRebaseBaseMinusOne;

#endif /* THANDOR_WORLD_SHOTS_RUNTIME_H */
