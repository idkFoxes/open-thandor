/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/weapons.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_WEAPONS_H
#define THANDOR_GAMEPLAY_ARMY_WEAPONS_H

#include <thandor/assets/effect/types.h>
#include <thandor/assets/shot/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/model/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/weapons. */

/* Functions are grouped by semantic ownership. */

void ArmyRuntime_SetNonzeroActionVector
          (Q12 actionVector0,Q12 actionVector2,Q12 actionVector1,ArmyRuntimeSlot *armyRuntime);

Bool8
ArmyRuntime_ResolveShotAimPoint
          (Q12 sourceWorldZQ12,Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,
          ShotDefinition *shotDefinition,GameEntityRuntime *targetState,GraphicsFixedVec3 *outAimPoint);

Bool8 ArmyRuntime_TestHasNoWeaponDamage(ArmyRuntimeSlot *armyRuntime);

Bool8 ArmyRuntime_TestWeaponDamageNonnegative(ArmyRuntimeSlot *armyRuntime);

void ArmyRuntimeClass_SelectProjectileTargetNode (ModelRuntimeTimedTargetProjectileView *modelRuntime, WorldOwnerListNode *candidateNode);

void ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedTargetProjectileView *modelRuntime);

Bool8 ArmyRuntime_ResolveShotLaunchFromModelAttachment
          (ShotTargetModelReference targetModelReference,Q12 targetWorldXQ12,Q12 targetWorldYQ12,
          Q12 targetWorldZQ12,SprAttachmentSelectorOrdinal attachmentSelectorOrdinal,
          ShotDefinition *shotDefinition,ModelRuntimeNode *modelNode,
          MdlSerializedNodeHeader *definitionNode,WorldRuntimeContext *worldRuntime);

void ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
          (EffectCreationFlagBits effectFlags,Q12 worldZQ12,Q12 worldYQ12,Q12 worldXQ12,
          ModelAttachmentOrdinal modelPointOrdinal,PckEffectDefinitionIdCatalog effectDefinitionId,
          void *sourceRuntime,void *modelPointTable,WorldRuntimeContext *worldContext);

void ArmyRuntime_ProcessReadyAttachmentChannels(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntime_UpdateTimedShotAndEffectEmitters
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

void ModelRuntime_EmitProjectilesFromAttachmentPoints
          (ShotTargetModelReference targetModelReference,Q12 targetWorldZQ12,Q12 targetWorldYQ12,
          Q12 targetWorldXQ12,ShotDefinition *shotDefinition,ModelRuntimeNode *modelNodeRuntime,
          MdlSerializedNodeHeader *definitionNode,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_WEAPONS_H */
