/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/drive_common.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/drive_common.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Accelerates a moving model (called directly by the movement class updates in drive_ground.cpp and
   drive_banking.cpp): the speed limit is the definition's movementSpeed. While the pitch (worldRotationAngle1) is
   below the first class threshold it is cut to 5/16, unless the pitch is at least the second threshold: then it
   stays full, or 5/8 when angle2 - angle0 lies between a quarter and three quarters of a turn. The advance per
   tick grows by accelerationPerTick up to that limit (and drops to it at once). When the model starts from
   standstill its start sound plays where the active faction's cell bits 0/1 are set.
*/
void ArmyRuntime_UpdateActivationMetricAndPlayStartSound(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  int previousAdvance;
  ModelDefinition *definition;
  AngleTurn32 rotationAngle1;
  SoundVoiceSet **voiceSetRef;
  uint32_t headingDelta;
  uint32_t speedLimit;
  uint32_t currentAdvance;
  uint32_t acceleratedAdvance;
  uint32_t newAdvance;
  uint32_t startSoundSlotIndex;
  Bool8 cellMasked;
  ModelRuntimeNode *rootNode;

  rootNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  rotationAngle1 = (rootNode->modelPayload).worldRotationAngle1;
  speedLimit = definition->movementSpeed;
  if ((int)rotationAngle1 < (int)definition->traversalSecondaryThreshold) {
    headingDelta = (rootNode->modelPayload).worldRotationAngle2 -
            (rootNode->modelPayload).worldRotationAngle0 & FIXED_ANGLE16_MASK;
    speedLimit = speedLimit * 5 >> 4;
    if ((int)definition->runtimeValue24 <= (int)rotationAngle1) {
      speedLimit = definition->movementSpeed;
      if ((FIXED_ANGLE16_QUARTER_TURN < headingDelta) && (headingDelta < 3 * FIXED_ANGLE16_QUARTER_TURN)) {
        speedLimit = speedLimit * 5 >> 3;
      }
    }
  }
  /* accelerate towards the limit; at or above it the advance drops to it at once */
  newAdvance = speedLimit;
  currentAdvance = (modelRuntime->movementControl).movementAdvancePerTickQ12;
  if (currentAdvance < speedLimit) {
    acceleratedAdvance = currentAdvance + definition->accelerationPerTick;
    if (acceleratedAdvance < speedLimit) {
      newAdvance = acceleratedAdvance;
    }
  }
  previousAdvance = (modelRuntime->movementControl).movementAdvancePerTickQ12;
  (modelRuntime->movementControl).movementAdvancePerTickQ12 = newAdvance;
  if ((previousAdvance == 0) && (newAdvance != 0)) {
    startSoundSlotIndex = definition->moveStartSoundSlotIndex;
    if ((startSoundSlotIndex != 0) &&
       ((startSoundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != nullptr)))) {
      voiceSetRef = (SoundVoiceSet **)worldRuntime->dwordArray[startSoundSlotIndex];
      worldPosition = &(rootNode->worldTransform).translation;
      if (voiceSetRef != nullptr) {
        cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                          ((rootNode->worldTransform).translation.y,worldPosition->x,worldRuntime)
        ;
        if (!cellMasked) {
          SpatialSound_PlayPositionedOneShot
                    (definition->positionedSoundMaximumDistanceQ12,definition->positionedSoundGainQ15,
                     worldPosition,voiceSetRef);
        }
      }
    }
  }
}

/* Reacts to the model a moving model has run into (called directly by the movement code in drive_ground.cpp,
   drive_banking.cpp and walker.cpp with the mover's model runtime and position, Y before X). A free class-23
   platform of the same faction is told to dock (behaviorState bit 0, collision retry countdown 0x20) and, once it
   is ready (bit 1), the two model runtimes are linked to each other (classState.linkedArmyRuntimeOrSavedOffset); a
   model of class 0 is run over and takes impact damage 0x100000 from the direction of the collision.
*/
void ArmyRuntime_HandleCollisionPartner(ModelRuntimeSlot *currentModelRuntime,Q12 currentWorldYQ12,Q12 currentWorldXQ12,
          ModelRuntimeSlot *collisionPartnerModelRuntime,WorldRuntimeContext *worldRuntime)

{
  uint32_t impactAngle;
  ModelRuntimeVerticalDeploymentView *platformRuntime;

  if (collisionPartnerModelRuntime == nullptr) {
    return;
  }
  if (collisionPartnerModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
      MODEL_RUNTIME_CLASS_23) {
    platformRuntime = ModelView_Cast<ModelRuntimeVerticalDeploymentView>(collisionPartnerModelRuntime);
    if ((platformRuntime->ownerArmyRuntime->factionIndex ==
         currentModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
       ((platformRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime == nullptr)) {
      (platformRuntime->classState).behaviorState |= 1;
      (platformRuntime->deploymentState).collisionRetryCountdown = 32;
      if (((platformRuntime->classState).behaviorState & 2U) != 0) {
        (platformRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = currentModelRuntime;
        (currentModelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime =
             collisionPartnerModelRuntime;
      }
    }
  }
  else if (collisionPartnerModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
           MODEL_RUNTIME_CLASS_00) {
    impactAngle = FixedMath_Atan2Angle16
                            ((collisionPartnerModelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).
                             translation.y - currentWorldYQ12,
                             (collisionPartnerModelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).
                             translation.x - currentWorldXQ12);
    ArmyRuntime_ApplyImpactDamageAndFinalizeState
              (impactAngle,ARMY_CRUSH_IMPACT_DAMAGE,collisionPartnerModelRuntime);
  }
}
