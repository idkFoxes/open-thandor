/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/combat.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/combat.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/ai/combat. */

/* Source class count AiCombatTarget_SelectBestCandidate reports for a zero class counter sum, in place of the
   original's positive stack leftover (see the quirk there). */
#define AI_SOURCE_CLASS_COUNT_ZERO_SUM_LEFTOVER 1

/* Address: 0x00536FC0.
   Per-step AI target choice of a non-neutral army, called by ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers
   (the army entry of the primaryUpdate phase of g_RuntimeMaintenanceCallbackPhases). Skipped while an
   interrupted command is pending or a target command is still running (commandGeneration > 0). Keeps the
   current target (re-stamping commandGeneration), falls back to the stored group-attack target (+0x98) when
   nothing was found although the army has hostile class counters, or else commands the selected target.
*/
void AiCombatDecision_UpdateTargetAssignment(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyRuntimeSlot *selectedTargetArmyRuntime;
  AiSourceClassCount sourceClassCount;
  ArmyCommandGeneration candidateCommandGenerationBase;
  AiCommandGenerationRightShiftBits selectedCommandGenerationRightShiftBits;

  if (((armyRuntime->commandModeFlags & ARMY_COMMAND_MODE_INTERRUPTED) == 0) &&
     (((int)armyRuntime->commandGeneration < 1 ||
      ((armyRuntime->commandModeFlags &
       (ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_TARGET_POSITION)) == 0)))) {
    selectedTargetArmyRuntime =
         AiCombatTarget_SelectBestCandidate(worldRuntime,armyRuntime,&sourceClassCount);
    selectedCommandGenerationRightShiftBits =
         g_AiCombatTargetSelectedCommandGenerationRightShiftBits;
    candidateCommandGenerationBase = g_AiCommandGenerationCandidateBase;
    if (selectedTargetArmyRuntime == armyRuntime->commandTargetArmyRuntime) {
      armyRuntime->commandGeneration = g_AiCommandGenerationRetainedTarget;
    }
    /* TEST EDX,EDX / JLE on the returned sum: count > 0 */
    else if ((selectedTargetArmyRuntime == NULL) && (0 < sourceClassCount)) {
      ArmyRuntime_ResolveCommandTarget((ArmyRuntimeSlot *)armyRuntime->assignedTargetArmyRuntime,armyRuntime);
      if ((armyRuntime->commandModeFlags & ARMY_COMMAND_MODE_TARGET_ARMY) == 0) {
        armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | ARMY_COMMAND_MODE_INTERRUPTED;
      }
    }
    else {
      ArmyRuntime_ResolveCommandTarget(selectedTargetArmyRuntime,armyRuntime);
      armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | ARMY_COMMAND_MODE_AI_COMBAT_TARGET;
      /* shift 2 (a quarter of the time) when the target's class counter for this army's class is zero,
         see AiCombatTarget_EvaluateCandidateScore */
      armyRuntime->commandGeneration =
           candidateCommandGenerationBase >> ((uint8_t)selectedCommandGenerationRightShiftBits & 31);
    }
  }
}


/* Address: 0x0053B8B0.
   Group attack: when at least two armies were collected, sums their hierarchy scale ratios (x256 each) until
   the group is strong enough (sum >= 0x200), then picks the target with the highest class base score from
   workspace 07 (or workspace 03 when 07 is empty) and sends every collected army to attack it. The target
   class is read with a single dereference from the target model's +0x4C, not from its type field.
*/
void AiUnitGroup_AssignCollectedEntitiesToBestTarget(void)

{
  uint32_t collectedCount;
  uint32_t collectedIndex;
  ArmyRuntimeSlot **collectedArmies;
  uint32_t accumulatedScaleRatio;
  Q12 collectedConditionRatioQ12;
  ArmyCommandGeneration assignedCommandGeneration;
  uint32_t targetCandidateCount;
  uint32_t targetCandidateIndex;
  AiTargetWorkspaceEntry *targetCandidateRecords;
  ModelRuntimeSlot *candidateTargetModelRuntime;
  uint32_t candidateClassScore;
  uint32_t bestScore;
  ModelRuntimeSlot *bestTargetModelRuntime;
  GameEntityRuntime *targetRuntime;
  ArmyRuntimeSlot *collectedArmy;

  if (g_AiCollectedEntityCount < 2) {
    return;
  }
  collectedCount = g_AiCollectedEntityCount;
  collectedArmies = g_AiWorkspace14CollectedArmies;
  accumulatedScaleRatio = 0;
  for (collectedIndex = 0; collectedIndex < collectedCount; collectedIndex++) {
    collectedConditionRatioQ12 =
         ModelRuntime_QueryHierarchyConditionRatioQ12((RuntimeModelFactionPrefix *)collectedArmies[collectedIndex]);
    accumulatedScaleRatio =
         accumulatedScaleRatio + (uint32_t)(collectedConditionRatioQ12 << 8) / (uint32_t)Q12_ONE;
    if (accumulatedScaleRatio >= AI_UNIT_GROUP_ATTACK_STRENGTH) {
      break;
    }
  }
  if (collectedIndex == collectedCount) {
    return;
  }
  assignedCommandGeneration = g_AiCommandGenerationCandidateBase;

  /* the group is strong enough: pick the target */
  targetCandidateCount = g_AiWorkspace07Count;
  targetCandidateRecords = g_AiWorkspace07Targets;
  if (g_AiWorkspace07Count == 0) {
    targetCandidateCount = g_AiWorkspace03Count;
    targetCandidateRecords = (AiTargetWorkspaceEntry *)g_AiWorkspace03UnseenHostiles;
    if (g_AiWorkspace03Count == 0) {
      return;
    }
  }
  bestScore = 0;
  bestTargetModelRuntime = NULL;
  for (targetCandidateIndex = 0; targetCandidateIndex < targetCandidateCount; targetCandidateIndex++) {
    candidateTargetModelRuntime = targetCandidateRecords[targetCandidateIndex].modelRuntime;
    if (candidateTargetModelRuntime == NULL) {
      continue;
    }
    candidateClassScore = g_AiCombatTargetClassBaseScores
                            [candidateTargetModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId];
    /* ties go to the later candidate */
    if (bestScore <= candidateClassScore) {
      bestScore = candidateClassScore;
      bestTargetModelRuntime = candidateTargetModelRuntime;
    }
  }
  if (bestScore == 0) {
    return;
  }

  /* send every collected army against the target model's owning army */
  targetRuntime = bestTargetModelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
  collectedCount = g_AiCollectedEntityCount;
  collectedArmies = g_AiWorkspace14CollectedArmies;
  for (collectedIndex = 0; collectedIndex < collectedCount; collectedIndex++) {
    collectedArmy = collectedArmies[collectedIndex];
    ArmyRuntime_ResolveCommandTargetAndRoute(targetRuntime,collectedArmy);
    collectedArmy->assignedTargetArmyRuntime = (uint32_t)targetRuntime;
    collectedArmy->commandModeFlags = collectedArmy->commandModeFlags | ARMY_COMMAND_MODE_INTERRUPTED;
    collectedArmy->aiUnitFlags = collectedArmy->aiUnitFlags | 1;
    collectedArmy->movementStateFlags = collectedArmy->movementStateFlags & ~ARMY_MOVEMENT_ROUTED;
    collectedArmy->aiUnitState = 8;
    collectedArmy->commandGeneration = assignedCommandGeneration;
  }
}


/* Address: 0x005372C0.
   Picks the best target for sourceArmyRuntime among the armies of the world owner list: sums the source's
   eight class counters (+0x100); a positive sum searches armies of other factions, a negative one other armies
   of the own faction (skipping entities flagged 0x400), zero searches nothing. Candidates must carry the
   source faction's bit (2 << 2 * faction) in +0x50 and are scored by AiCombatTarget_EvaluateCandidateScore
   within depth-bin masks around the source (radius +0x4C plus 4.0). Returns the best army (or NULL) and stores
   the sum in *outSourceClassCount (the original returned the pair in EAX:EDX).
   Only called by AiCombatDecision_UpdateTargetAssignment.
   Original quirk: a zero sum stores a stack leftover instead of the sum (see the body); the C stores the
   positive constant AI_SOURCE_CLASS_COUNT_ZERO_SUM_LEFTOVER, since every traced original path leaves a positive
   value there and the caller only tests the sign.
*/
ArmyRuntimeSlot *AiCombatTarget_SelectBestCandidate
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *sourceArmyRuntime,
          AiSourceClassCount *outSourceClassCount)

{
  ArmyRuntimeSlot *candidateArmyRuntime;
  DepthBinMask32 sourceDepthMask1;
  DepthBinMask32 sourceDepthMask0;
  AiCandidateScore32 candidateScore;
  int classIndex;
  FactionRuntimeIndex candidateFactionIndex;
  bool relationFitsSearch;
  Q12 searchRadiusQ12;
  AiCandidateScore32 currentBestScore;
  AiSourceClassCount sourceClassCount;
  AiSourceClassCount returnedSourceClassCount;
  ArmyRuntimeSlot *bestCandidateArmyRuntime;
  WorldOwnerListNode *ownerNodeCursor;
  ModelRuntimeNode *sourceModelNode;
  GameEntityRuntime *candidateEntityRuntime;
  FactionRuntimeIndex sourceFactionIndex;

  sourceClassCount = 0;
  ownerNodeCursor = worldRuntime->ownerListHead;
  bestCandidateArmyRuntime = NULL;
  /* the eight class counters at +0x100, summed from the last one down */
  for (classIndex = 7; classIndex >= 0; classIndex--) {
    sourceClassCount = sourceClassCount + sourceArmyRuntime->targetClassShotDamage[classIndex];
  }
  /* Original quirk: with a zero sum the original skips MOV [EBP-0x10],ESI (0x00537302) and returns in EDX
     (0x00537403) whatever an earlier call left in that stack slot, 36 bytes below
     AiCombatDecision_UpdateTargetAssignment's return address. The call just before at the same depth is
     ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive (0x0051D2C0): for an army with child
     entities each child's frame pushes EBX = worldRuntime there (0x0052A7D8), otherwise the class handler's
     frame leaves e.g. the movement length ([EBP-8], 0x0052094B) or a pushed pointer / return address. The
     value is class and state dependent, but every traced path leaves a positive one (the movement length is
     0 only when the unit stands exactly on its movement point), and the caller only tests EDX > 0
     (0x00537013). The C formerly returned the most common of them, the world runtime pointer (positive:
     /LARGEADDRESSAWARE:NO); since the only use of the value is that sign test (the zero-sum path finds no
     candidate, and AiCombatDecision_UpdateTargetAssignment reads sourceClassCount nowhere else), any positive
     constant gives the same decisions and stays positive on 64-bit. */
  returnedSourceClassCount = AI_SOURCE_CLASS_COUNT_ZERO_SUM_LEFTOVER;
  if (sourceClassCount != 0) {
    sourceModelNode = sourceArmyRuntime->modelNodeRuntime;
    searchRadiusQ12 = sourceArmyRuntime->weaponRangeQ12 + 4 * Q12_ONE;
    sourceDepthMask1 =
         DepthInterval_BuildBinMask(searchRadiusQ12,(sourceModelNode->worldTransform).translation.x)
    ;
    sourceDepthMask0 =
         DepthInterval_BuildBinMask(searchRadiusQ12,(sourceModelNode->worldTransform).translation.y)
    ;
    sourceFactionIndex = sourceArmyRuntime->factionIndex;
    currentBestScore = 0;
    returnedSourceClassCount = sourceClassCount;
    for (; ownerNodeCursor != NULL; ownerNodeCursor = ownerNodeCursor->nextNode) {
      if (ownerNodeCursor->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
        continue;
      }
      candidateEntityRuntime = ownerNodeCursor->runtimePayload;
      candidateArmyRuntime = (candidateEntityRuntime->common).ownership.runtimeLink;
      /* a negative sum skips entities flagged 0x400 */
      if ((sourceClassCount < 0) &&
          (((candidateEntityRuntime->common).runtimeFlags & ARMY_MODEL_STATE_NO_REGENERATION) != 0)) {
        continue;
      }
      if (((candidateEntityRuntime->common).runtimeFlags & ARMY_RUNTIME_FLAG_DESTROYED) != 0) {
        continue;
      }
      candidateFactionIndex = candidateArmyRuntime->factionIndex;
      if (candidateFactionIndex == 0) {
        continue;
      }
      /* Non-positive class counts look for other armies of the own faction, positive ones for armies
         of other factions. */
      if (sourceClassCount < 1) {
        relationFitsSearch = (candidateFactionIndex == sourceArmyRuntime->factionIndex) &&
                             (sourceArmyRuntime != candidateArmyRuntime);
      }
      else {
        relationFitsSearch = candidateFactionIndex != sourceArmyRuntime->factionIndex;
      }
      if (!relationFitsSearch) {
        continue;
      }
      /* bit 1 of the source faction's 2-bit field in the candidate's +0x50 */
      if ((candidateArmyRuntime->terrainOccupancyMask0 & 2 << ((char)sourceFactionIndex * 2 & 31U)) == 0) {
        continue;
      }
      candidateScore =
           AiCombatTarget_EvaluateCandidateScore
                     (currentBestScore,sourceClassCount,sourceDepthMask0,sourceDepthMask1,
                      candidateArmyRuntime,sourceArmyRuntime);
      if (currentBestScore < candidateScore) {
        g_AiCombatTargetSelectedCommandGenerationRightShiftBits =
             g_AiCombatTargetCurrentCommandGenerationRightShiftBits;
        currentBestScore = candidateScore;
        bestCandidateArmyRuntime = candidateArmyRuntime;
      }
    }
  }
  *outSourceClassCount = returnedSourceClassCount;
  return bestCandidateArmyRuntime;
}


/* Address: 0x00537060.
   Scores candidateArmyRuntime as a target for sourceArmyRuntime; 0 rejects it. Requires overlapping depth-bin
   masks, a faction relation that fits the search (not friendly for sourceClassCount > 0, friendly otherwise,
   where only damaged candidates count) and a horizontal distance within the source radius (+0x4C) plus 2.0.
   The score adds weighted terms for the remaining clearance, the class base score, the two armies' class
   counters and the condition deficit (1.0 - ModelRuntime_QueryHierarchyConditionRatioQ12; the counter and
   deficit terms are divided by the Q12 unity); a score above currentBestScore is then
   quartered when the weapon line-of-fire test returns true, and dropped unless the source definition's +0x18
   is set. The class definitions are read through two dereferences (definition+0x5C), i.e. the live type.
   Called by AiCombatTarget_SelectBestCandidate and ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate.
*/
AiCandidateScore32 AiCombatTarget_EvaluateCandidateScore
          (AiCandidateScore32 currentBestScore,AiSourceClassCount sourceClassCount,
          DepthBinMask32 sourceDepthMask0,DepthBinMask32 sourceDepthMask1,
          ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  uint32_t candidateFactionIndex;
  ModelDefinition *candidateDefinition;
  int reachQ12;
  int deltaXQ12;
  int deltaYQ12;
  int64_t clearanceSquared;
  int64_t radialClearanceWeight;
  int candidateCounterForSourceClass;
  int sourceCounterForCandidateClass;
  uint32_t radialClearanceQ12;
  ModelRuntimeSlot *sourceWeaponModelRuntime;
  ModelRuntimeNode *candidateAimModelNode;
  int classBaseScore;
  bool masksOverlap;
  bool capabilityBitClear;
  bool lineOfFireTestPassed;
  Q12 conditionRatioQ12;
  uint32_t candidateScore;
  uint32_t sourceRadiusQ12;
  ModelRuntimeSlot *sourceModelRuntime;
  ModelRuntimeNode *candidateModelNode;

  candidateFactionIndex = candidateArmyRuntime->factionIndex;
  candidateModelNode = candidateArmyRuntime->modelNodeRuntime;
  masksOverlap = DepthBinMasks_Overlap
                      (candidateModelNode->depthBinMaskFar,candidateModelNode->depthBinMaskNear,sourceDepthMask0,
                       sourceDepthMask1);
  if (!masksOverlap) {
    return 0;
  }
  capabilityBitClear =
       GameFactionRuntime_TestCapabilityBitClear(candidateFactionIndex,sourceArmyRuntime->factionIndex);
  if (sourceClassCount < 1) {
    if (capabilityBitClear) {
      return 0;
    }
  }
  else if (!capabilityBitClear) {
    return 0;
  }

  /* clearance^2 = (radius + 2.0)^2 - dx^2 - dy^2 in 64 bits; negative rejects */
  reachQ12 = sourceArmyRuntime->weaponRangeQ12 + 2 * Q12_ONE;
  deltaXQ12 = (sourceArmyRuntime->modelNodeRuntime->worldTransform).translation.x -
              (candidateModelNode->worldTransform).translation.x;
  clearanceSquared = (int64_t)reachQ12 * (int64_t)reachQ12 - (int64_t)deltaXQ12 * (int64_t)deltaXQ12;
  if (clearanceSquared < 0) {
    return 0;
  }
  deltaYQ12 = (sourceArmyRuntime->modelNodeRuntime->worldTransform).translation.y -
              (candidateModelNode->worldTransform).translation.y;
  clearanceSquared = clearanceSquared - (int64_t)deltaYQ12 * (int64_t)deltaYQ12;
  if (clearanceSquared < 0) {
    return 0;
  }
  radialClearanceQ12 =
       FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)clearanceSquared >> 32),(UInt64Half32)clearanceSquared);
  radialClearanceWeight = (int64_t)g_AiCombatTargetRadialClearanceWeight;
  sourceRadiusQ12 = sourceArmyRuntime->weaponRangeQ12;
  candidateDefinition = (((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
          definitionOrSavedId).runtimeDefinition;
  /* class counters (+0x100): the candidate's for the source's class and the source's for the
     candidate's class; a zero candidate counter selects the shorter command time (shift 2) */
  candidateCounterForSourceClass = candidateArmyRuntime->targetClassShotDamage
                  [(((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                   definitionOrSavedId).runtimeDefinition->targetClassIndex];
  sourceCounterForCandidateClass = sourceArmyRuntime->targetClassShotDamage[candidateDefinition->targetClassIndex];
  g_AiCombatTargetCurrentCommandGenerationRightShiftBits = 0;
  if (candidateCounterForSourceClass == 0) {
    g_AiCombatTargetCurrentCommandGenerationRightShiftBits = 2;
  }
  if (sourceCounterForCandidateClass == 0) {
    return 0;
  }
  /* a negative source counter counts by its magnitude and ignores the candidate's counter */
  if (sourceCounterForCandidateClass < 0) {
    candidateCounterForSourceClass = 0;
    sourceCounterForCandidateClass = -sourceCounterForCandidateClass;
  }
  classBaseScore =
       g_AiCombatTargetClassBaseScores[candidateDefinition->runtimeClassId] *
       g_AiCombatTargetClassBaseScoreMultiplier;
  conditionRatioQ12 = ModelRuntime_QueryHierarchyConditionRatioQ12((RuntimeModelFactionPrefix *)candidateArmyRuntime);
  /* a friendly search needs condition < 1.0 (unsigned compare as in the original) */
  if ((sourceClassCount < 0) && ((uint32_t)conditionRatioQ12 >= (uint32_t)Q12_ONE)) {
    return 0;
  }
  candidateScore =
       (int)(((int)radialClearanceQ12 * radialClearanceWeight) / (int64_t)(int)sourceRadiusQ12) +
      classBaseScore +
       (int)(((int64_t)g_AiCombatTargetSourceCounterCountWeight * (int64_t)candidateCounterForSourceClass) /
            (int64_t)Q12_ONE) +
       (int)(((int64_t)g_AiCombatTargetCandidateCounterCountWeight * (int64_t)sourceCounterForCandidateClass) /
            (int64_t)Q12_ONE) +
       (int)(((int64_t)g_AiCombatTargetScaleDeficitWeight *
             (int64_t)(int)((uint32_t)Q12_ONE - (uint32_t)conditionRatioQ12)) /
            (int64_t)Q12_ONE);
  if (candidateCounterForSourceClass == 0) {
    candidateScore = candidateScore >> 2;
  }
  if (currentBestScore >= (int)candidateScore) {
    return candidateScore;
  }

  /* the weapon is the first or second attachment */
  sourceModelRuntime = (sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  candidateAimModelNode = candidateArmyRuntime->modelNodeRuntime;
  if (sourceModelRuntime->attachmentCount == 0) {
    return candidateScore;
  }
  candidateDefinition = (((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
          definitionOrSavedId).runtimeDefinition;
  sourceWeaponModelRuntime = sourceModelRuntime->attachments[0].childModelRuntimeOrSavedOffset;
  if (sourceWeaponModelRuntime == NULL) {
    if (sourceModelRuntime->attachmentCount == 1) {
      return candidateScore;
    }
    sourceWeaponModelRuntime = sourceModelRuntime->attachments[1].childModelRuntimeOrSavedOffset;
    if (sourceWeaponModelRuntime == NULL) {
      return candidateScore;
    }
  }
  /* aircraft are aimed at their first child node, all targets at the definition's height offset (+0x50)
     above the node */
  if (candidateDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
    candidateAimModelNode = candidateAimModelNode->childNodes[0];
  }
  lineOfFireTestPassed = ArmyWeaponRuntime_TestTargetLineOfFire
                              (candidateDefinition->aimHeightOffsetQ12 +
                               (candidateAimModelNode->worldTransform).translation.z,
                               (candidateAimModelNode->worldTransform).translation.y,
                               (candidateAimModelNode->worldTransform).translation.x,
                               &g_InGameRuntimeRoot->worldRuntime,
                               sourceWeaponModelRuntime);
  if (lineOfFireTestPassed) {
    candidateScore = candidateScore >> 2;
    if ((((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
         definitionOrSavedId).runtimeDefinition->accelerationPerTick == 0) {
      return 0;
    }
  }
  return candidateScore;
}

