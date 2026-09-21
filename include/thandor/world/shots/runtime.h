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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0052CC60 */
void __thandor_void_preserve_eax_ecx_edx
ShotRuntime_ApplyArmyHitRelationAndNotifications
          (ArmyRuntimeSlot *targetArmyRuntime,ShotRuntimeSlot *shotRuntime);

/* 0x0052B540 */
StatusValueEaxCf5 ShotRuntime_InitGraphicsResources(word *mutableBasePath);

/* 0x0052B5C0 */
void __thandor_void_preserve_eax_ecx ShotRuntime_ShutdownGraphicsResources(void);

/* 0x0052B660 */
ShotDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ShotRuntime_FindDefinitionByIdCf(PckShotDefinitionIdCatalog definitionId);

/* 0x0052B750 */
void __thandor_void_preserve_eax_ecx_edx ShotRuntime_RebaseSlotsAfterLoad(void);

/* 0x0052BDB0 */
void __thandor_void_preserve_eax_ecx_edx
ShotRuntimePool_CreateProjectileFromDefinition
          (ShotRuntimeState14 shotRuntimeState14,ArmyRuntimeSlot *ownerArmyRuntime,
          Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,Q12 launchWorldZQ12,
          Q12 launchWorldYQ12,Q12 launchWorldXQ12,ShotDefinition *shotDefinition,
          WorldRuntimeContext *worldRuntime);

/* 0x00514710 */
void __thandor_void_preserve_eax_ecx_edx
ShotRuntime_PostImpactRelationNotificationNoOp
          (ShotRuntimeSlot *shotRuntime,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_WORLD_SHOTS_RUNTIME_H */
