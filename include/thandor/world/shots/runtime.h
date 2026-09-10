#ifndef THANDOR_WORLD_SHOTS_RUNTIME_H
#define THANDOR_WORLD_SHOTS_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/shots/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0052CC60 */
void ShotRuntime_ApplyArmyHitRelationAndNotifications (ArmyRuntimeSlot *targetArmyRuntime,ShotRuntimeSlot *shotRuntime);

/* 0x0052B540 */
void ShotRuntime_InitGraphicsResources(word *mutableBasePath);

/* 0x0052B5C0 */
void ShotRuntime_ShutdownGraphicsResources(void);

/* 0x0052B660 */
ShotDefinition * ShotRuntime_FindDefinitionByIdCf(PckShotDefinitionIdCatalog definitionId);

/* 0x0052B750 */
void __cdecl ShotRuntime_RebaseSlotsAfterLoad(void);

/* 0x0052BDB0 */
undefined8 ShotRuntimePool_CreateProjectileFromDefinition (ShotRuntimeState14 shotRuntimeState14,ArmyRuntimeSlot *ownerArmyRuntime, Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,Q12 launchWorldZQ12, Q12 launchWorldYQ12,Q12 launchWorldXQ12,ShotDefinition *shotDefinition, WorldRuntimeContext *worldRuntime);

/* 0x00514710 */
void ShotRuntime_PostImpactRelationNotificationNoOp (ShotRuntimeSlot *shotRuntime,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_WORLD_SHOTS_RUNTIME_H */
