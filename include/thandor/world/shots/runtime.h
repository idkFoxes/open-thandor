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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0052CC60 */
void ShotRuntime_ApplyArmyHitRelationAndNotifications(ModelRuntimeSlot *targetModelRuntime,ShotRuntimeSlot *shotRuntime);

/* 0x0052B540 */
bool ShotRuntime_InitGraphicsResources(uint16_t *mutableBasePath,uint32_t *outError);

/* 0x0052B5C0 */
void ShotRuntime_ShutdownGraphicsResources(void);

/* 0x0052B660 */
ShotDefinition *ShotRuntime_FindDefinitionById(PckShotDefinitionIdCatalog definitionId);

/* 0x0052B750 */
void ShotRuntime_RebaseSlotsAfterLoad(void);

/* 0x0052BDB0 */
void ShotRuntimePool_CreateProjectileFromDefinition
          (ShotTargetModelReference targetModelReference,ArmyRuntimeSlot *ownerArmyRuntime,
          Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,Q12 launchWorldZQ12,
          Q12 launchWorldYQ12,Q12 launchWorldXQ12,ShotDefinition *shotDefinition,
          WorldRuntimeContext *worldRuntime);

/* 0x00514710 */
void ShotRuntime_PostImpactRelationNotificationNoOp(ShotRuntimeSlot *shotRuntime,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_WORLD_SHOTS_RUNTIME_H */
