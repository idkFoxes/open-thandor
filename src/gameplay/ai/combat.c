/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/combat.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/combat.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/ai/combat. */

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
  AiCombatTargetSelectionResult selectionResult;
  ArmyCommandGeneration candidateCommandGenerationBase;
  AiCommandGenerationRightShiftBits selectedCommandGenerationRightShiftBits;

  if (((armyRuntime->commandModeFlags & ARMY_COMMAND_MODE_INTERRUPTED) == 0) &&
     (((int)armyRuntime->commandGeneration < 1 ||
      ((armyRuntime->commandModeFlags &
       (ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_TARGET_POSITION)) == 0)))) {
    selectionResult = AiCombatTarget_SelectBestCandidate(worldRuntime,armyRuntime);
    selectedCommandGenerationRightShiftBits =
         g_AiCombatTargetSelectedCommandGenerationRightShiftBits;
    candidateCommandGenerationBase = g_AiCommandGenerationCandidateBase;
    selectedTargetArmyRuntime = selectionResult.targetArmyRuntime;
    if (selectedTargetArmyRuntime == armyRuntime->commandTargetArmyRuntime) {
      armyRuntime->commandGeneration = g_AiCommandGenerationRetainedTarget;
    }
    /* the qword test is the sign of sourceClassCount (TEST EDX,EDX / JLE): count > 0 */
    else if ((selectedTargetArmyRuntime == NULL) &&
            (selectionResult.sourceClassCount != 0 &&
             -1 < THANDOR_BITCAST(AiCombatTargetSelectionResult, int64_t, selectionResult))) {
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
void __fastcall AiUnitGroup_AssignCollectedEntitiesToBestTarget(void)

{
  uint32_t targetClassIndex;
  GameEntityRuntime *targetRuntime;
  ArmyCommandGeneration assignedCommandGeneration;
  uint32_t remainingOrBestScore;
  int targetCandidateRecordsRemaining;
  uint32_t accumulatedScaleRatio;
  ArmyRuntimeSlot *targetArmy;
  AiTargetWorkspaceEntry *targetCandidateRecordCursor;
  ArmyRuntimeSlot **collectedArmyCursor;
  ModelRuntimeScaleRatioRegisterPairQ12 collectedHierarchyScaleRatioPairQ12;
  ModelRuntimeSlot *candidateTargetModelRuntime;

  if (1 < g_AiCollectedEntityCount) {
    accumulatedScaleRatio = 0;
    remainingOrBestScore = g_AiCollectedEntityCount;
    /* The collected-army cursor shares its variable with the chosen target (the original keeps both in ESI):
       "->modelRuntimeOrSavedOffset.modelRuntime" (offset 0) reads the army pointer at the cursor,
       "&->modelNodeRuntime" (offset 4) steps to the next one. */
    targetArmy = (ArmyRuntimeSlot *)g_AiWorkspace14CollectedArmies;
    do {
      collectedHierarchyScaleRatioPairQ12 =
           ModelRuntime_QueryHierarchyScaleRatioQ12Regs
                     ((RuntimeModelFactionPrefix *)(targetArmy->modelRuntimeOrSavedOffset).modelRuntime);
      assignedCommandGeneration = g_AiCommandGenerationCandidateBase;
      THANDOR_PART(uint32_t, collectedHierarchyScaleRatioPairQ12, 4) =
           (uint32_t)(collectedHierarchyScaleRatioPairQ12 >> 32);
      if ((THANDOR_PART(uint32_t, collectedHierarchyScaleRatioPairQ12, 4) != 0) &&
         (accumulatedScaleRatio = accumulatedScaleRatio + (uint32_t)((int)collectedHierarchyScaleRatioPairQ12 << 8) /
                          THANDOR_PART(uint32_t, collectedHierarchyScaleRatioPairQ12, 4),
          accumulatedScaleRatio > AI_UNIT_GROUP_ATTACK_STRENGTH - 1)) {
        remainingOrBestScore = 0;
        targetCandidateRecordsRemaining = g_AiWorkspace07Count;
        targetCandidateRecordCursor = g_AiWorkspace07Targets;
        if ((g_AiWorkspace07Count == 0) &&
           (targetCandidateRecordsRemaining = g_AiWorkspace03Count,
           targetCandidateRecordCursor = (AiTargetWorkspaceEntry *)g_AiWorkspace03UnseenHostiles,
           g_AiWorkspace03Count == 0)) {
          return;
        }
        do {
          candidateTargetModelRuntime = targetCandidateRecordCursor->modelRuntime;
          /* ties go to the later candidate */
          if ((candidateTargetModelRuntime != NULL) &&
             (targetClassIndex = candidateTargetModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId,
              remainingOrBestScore <= g_AiCombatTargetClassBaseScores[targetClassIndex])) {
            remainingOrBestScore = g_AiCombatTargetClassBaseScores[targetClassIndex];
            /* the chosen target model runtime shares the variable (ESI) with the collected-army cursor */
            targetArmy = (ArmyRuntimeSlot *)candidateTargetModelRuntime;
          }
          targetCandidateRecordCursor++;
          targetCandidateRecordsRemaining--;
        } while (targetCandidateRecordsRemaining != 0);
        if (remainingOrBestScore == 0) {
          return;
        }
        /* the target model's owning army */
        targetRuntime = ((ModelRuntimeSlot *)targetArmy)->ownerArmyRuntimeOrSavedOffset.entityRuntime;
        remainingOrBestScore = g_AiCollectedEntityCount;
        collectedArmyCursor = g_AiWorkspace14CollectedArmies;
        do {
          targetArmy = *collectedArmyCursor;
          ArmyRuntime_ResolveCommandTargetAndRoute(targetRuntime,targetArmy);
          targetArmy->assignedTargetArmyRuntime = (uint32_t)targetRuntime;
          targetArmy->commandModeFlags = targetArmy->commandModeFlags | ARMY_COMMAND_MODE_INTERRUPTED;
          targetArmy->aiUnitFlags = targetArmy->aiUnitFlags | 1;
          targetArmy->movementStateFlags = targetArmy->movementStateFlags & ~ARMY_MOVEMENT_ROUTED;
          targetArmy->aiUnitState = 8;
          targetArmy->commandGeneration = assignedCommandGeneration;
          collectedArmyCursor++;
          remainingOrBestScore--;
        } while (remainingOrBestScore != 0);
        return;
      }
      targetArmy = (ArmyRuntimeSlot *)&targetArmy->modelNodeRuntime;
      remainingOrBestScore--;
    } while (remainingOrBestScore != 0);
  }
  return;
}


/* Address: 0x005372C0.
   Picks the best target for sourceArmyRuntime among the armies of the world owner list: sums the source's
   eight class counters (+0x100); a positive sum searches armies of other factions, a negative one other armies
   of the own faction (skipping entities flagged 0x400), zero searches nothing. Candidates must carry the
   source faction's bit (2 << 2 * faction) in +0x50 and are scored by AiCombatTarget_EvaluateCandidateScore
   within depth-bin masks around the source (radius +0x4C plus 4.0). Returns the best army (or NULL) and the sum.
   Only called by AiCombatDecision_UpdateTargetAssignment.
   Original quirk: a zero sum returns a stack leftover instead of the sum (see the body); the C returns the
   world runtime pointer, the positive value the traced original paths leave there.
*/
AiCombatTargetSelectionResult
AiCombatTarget_SelectBestCandidate
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  ArmyRuntimeSlot *candidateArmyRuntime;
  DepthBinMask32 sourceDepthMask1;
  DepthBinMask32 sourceDepthMask0;
  AiCandidateScore32 candidateScore;
  int classIndexOrFaction;
  Q12 searchRadiusQ12;
  AiCandidateScore32 currentBestScore;
  AiSourceClassCount sourceClassCount;
  AiCombatTargetSelectionResult targetSelectionResult;
  AiSourceClassCount returnedSourceClassCount;
  ArmyRuntimeSlot *bestCandidateArmyRuntime;
  WorldOwnerListNode *ownerNodeCursor;
  ModelRuntimeNode *sourceModelNode;
  GameEntityRuntime *candidateEntityRuntime;
  FactionRuntimeIndex sourceFactionIndex;
  
  sourceClassCount = 0;
  ownerNodeCursor = worldRuntime->ownerListHead;
  classIndexOrFaction = 7;
  bestCandidateArmyRuntime = NULL;
  /* the eight class counters at +0x100 */
  do {
    sourceClassCount = sourceClassCount + sourceArmyRuntime->targetClassShotDamage[classIndexOrFaction];
    classIndexOrFaction--;
  } while (-1 < classIndexOrFaction);
  /* Original quirk: with a zero sum the original skips MOV [EBP-0x10],ESI (0x00537302) and returns in EDX
     (0x00537403) whatever an earlier call left in that stack slot, 36 bytes below
     AiCombatDecision_UpdateTargetAssignment's return address. The call just before at the same depth is
     ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive (0x0051D2C0): for an army with child
     entities each child's frame pushes EBX = worldRuntime there (0x0052A7D8), otherwise the class handler's
     frame leaves e.g. the movement length ([EBP-8], 0x0052094B) or a pushed pointer / return address. The
     value is class and state dependent, but every traced path leaves a positive one (the movement length is
     0 only when the unit stands exactly on its movement point), and the caller only tests EDX > 0
     (0x00537013), so the C returns the most common of them, the world runtime pointer (positive:
     /LARGEADDRESSAWARE:NO). */
  returnedSourceClassCount = (AiSourceClassCount)(uintptr_t)worldRuntime;
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
    for (; returnedSourceClassCount = sourceClassCount, ownerNodeCursor != NULL;
        ownerNodeCursor = ownerNodeCursor->nextNode) {
      if (ownerNodeCursor->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        candidateEntityRuntime = ownerNodeCursor->runtimePayload;
        candidateArmyRuntime = (candidateEntityRuntime->common).ownership.runtimeLink;
        if ((((-1 < sourceClassCount) ||
             (((candidateEntityRuntime->common).runtimeFlags & ARMY_MODEL_STATE_NO_REGENERATION) == 0)) &&
            (((candidateEntityRuntime->common).runtimeFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0)) &&
           (classIndexOrFaction = candidateArmyRuntime->factionIndex, classIndexOrFaction != 0)) {
          /* Non-positive class counts look for other armies of the own faction, positive ones for armies
             of other factions. */
          if ((sourceClassCount < 1) ?
              ((classIndexOrFaction == sourceArmyRuntime->factionIndex) &&
               (sourceArmyRuntime != candidateArmyRuntime)) :
              (classIndexOrFaction != sourceArmyRuntime->factionIndex)) {
            /* bit 1 of the source faction's 2-bit field in the candidate's +0x50 */
            if (((candidateArmyRuntime->terrainOccupancyMask0 &
                 2 << ((char)sourceFactionIndex * 2 & 31U)) != 0) &&
               (candidateScore =
                     AiCombatTarget_EvaluateCandidateScore
                               (currentBestScore,sourceClassCount,sourceDepthMask0,sourceDepthMask1,
                                candidateArmyRuntime,sourceArmyRuntime),
               currentBestScore < candidateScore)) {
              g_AiCombatTargetSelectedCommandGenerationRightShiftBits =
                   g_AiCombatTargetCurrentCommandGenerationRightShiftBits;
              currentBestScore = candidateScore;
              bestCandidateArmyRuntime = candidateArmyRuntime;
            }
          }
        }
      }
    }
  }
  targetSelectionResult.sourceClassCount = returnedSourceClassCount;
  targetSelectionResult.targetArmyRuntime = bestCandidateArmyRuntime;
  return targetSelectionResult;
}


/* Address: 0x00537060.
   Scores candidateArmyRuntime as a target for sourceArmyRuntime; 0 rejects it. Requires overlapping depth-bin
   masks, a faction relation that fits the search (not friendly for sourceClassCount > 0, friendly otherwise,
   where only damaged candidates count) and a horizontal distance within the source radius (+0x4C) plus 2.0.
   The score adds weighted terms for the remaining clearance, the class base score, the two armies' class
   counters and the condition deficit (1.0 - ModelRuntime_QueryHierarchyScaleRatioQ12Regs, whose EDX is the
   Q12 unity the counter and deficit terms are divided by); a score above currentBestScore is then
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
  int64_t clearanceSquaredOrWeight;
  int reachDeltaOrSourceCounter;
  int deltaXOrCandidateCounter;
  uint32_t radialClearanceQ12;
  ModelRuntimeSlot *sourceWeaponModelRuntime;
  ModelRuntimeNode *candidateAimModelNode;
  int classBaseScore;
  bool testPassed;
  ModelRuntimeScaleRatioRegisterPairQ12 hierarchyScaleRatioPairQ12;
  uint32_t candidateScore;
  uint32_t sourceRadiusQ12;
  ModelRuntimeSlot *sourceModelRuntime;
  ModelRuntimeNode *candidateModelNode;
  
  candidateFactionIndex = candidateArmyRuntime->factionIndex;
  candidateModelNode = candidateArmyRuntime->modelNodeRuntime;
  testPassed = DepthBinMasks_Overlap
                    (candidateModelNode->depthBinMaskFar,candidateModelNode->depthBinMaskNear,sourceDepthMask0,
                     sourceDepthMask1);
  if (testPassed) {
    if (sourceClassCount < 1) {
      testPassed = GameFactionRuntime_TestCapabilityBitClear(candidateFactionIndex,sourceArmyRuntime->factionIndex);
      if (testPassed) {
        return 0;
      }
    }
    else {
      testPassed = GameFactionRuntime_TestCapabilityBitClear(candidateFactionIndex,sourceArmyRuntime->factionIndex);
      if (!testPassed) {
        return 0;
      }
    }
    /* clearance^2 = (radius + 2.0)^2 - dx^2 - dy^2 in 64 bits; negative rejects */
    reachDeltaOrSourceCounter = sourceArmyRuntime->weaponRangeQ12 + 2 * Q12_ONE;
    deltaXOrCandidateCounter = (sourceArmyRuntime->modelNodeRuntime->worldTransform).translation.x -
            (candidateModelNode->worldTransform).translation.x;
    clearanceSquaredOrWeight = (int64_t)reachDeltaOrSourceCounter * (int64_t)reachDeltaOrSourceCounter -
                              (int64_t)deltaXOrCandidateCounter * (int64_t)deltaXOrCandidateCounter;
    if (-1 < clearanceSquaredOrWeight) {
      reachDeltaOrSourceCounter = (sourceArmyRuntime->modelNodeRuntime->worldTransform).translation.y -
              (candidateModelNode->worldTransform).translation.y;
      clearanceSquaredOrWeight =
          clearanceSquaredOrWeight - (int64_t)reachDeltaOrSourceCounter * (int64_t)reachDeltaOrSourceCounter;
      if (-1 < clearanceSquaredOrWeight) {
        radialClearanceQ12 =
             FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)clearanceSquaredOrWeight >> 32),
                                 (UInt64Half32)clearanceSquaredOrWeight);
        clearanceSquaredOrWeight = (int64_t)(int)g_AiCombatTargetRadialClearanceWeight;
        sourceRadiusQ12 = sourceArmyRuntime->weaponRangeQ12;
        candidateDefinition = (((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                definitionOrSavedId).runtimeDefinition;
        /* class counters (+0x100): the candidate's for the source's class and the source's for the
           candidate's class; a zero candidate counter selects the shorter command time (shift 2) */
        reachDeltaOrSourceCounter = candidateArmyRuntime->targetClassShotDamage
                        [(((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                         definitionOrSavedId).runtimeDefinition->targetClassIndex];
        deltaXOrCandidateCounter = sourceArmyRuntime->targetClassShotDamage[candidateDefinition->targetClassIndex];
        g_AiCombatTargetCurrentCommandGenerationRightShiftBits = 0;
        if (reachDeltaOrSourceCounter == 0) {
          g_AiCombatTargetCurrentCommandGenerationRightShiftBits = 2;
        }
        if (deltaXOrCandidateCounter != 0) {
          /* a negative source counter counts by its magnitude and ignores the candidate's counter */
          if (deltaXOrCandidateCounter < 0) {
            reachDeltaOrSourceCounter = 0;
            deltaXOrCandidateCounter = -deltaXOrCandidateCounter;
          }
          classBaseScore =
               g_AiCombatTargetClassBaseScores
                 [candidateDefinition->runtimeClassId] *
               g_AiCombatTargetClassBaseScoreMultiplier;
          hierarchyScaleRatioPairQ12 =
               ModelRuntime_QueryHierarchyScaleRatioQ12Regs
                         ((RuntimeModelFactionPrefix *)candidateArmyRuntime);
          THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4) = (uint32_t)(hierarchyScaleRatioPairQ12 >> 32);
          /* low half = condition ratio, high half = Q12 unity; a friendly search needs condition < 1.0 */
          if ((-1 < sourceClassCount) ||
             ((uint32_t)hierarchyScaleRatioPairQ12 < THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4))) {
            candidateScore =
                 (int)(((int)radialClearanceQ12 * clearanceSquaredOrWeight) / (int64_t)(int)sourceRadiusQ12) +
                classBaseScore +
                 (int)(((int64_t)(int)g_AiCombatTargetSourceCounterCountWeight * (int64_t)reachDeltaOrSourceCounter) /
                      (int64_t)(int)THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4)) +
                 (int)(((int64_t)(int)g_AiCombatTargetCandidateCounterCountWeight * (int64_t)deltaXOrCandidateCounter) /
                      (int64_t)(int)THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4)) +
                 (int)(((int64_t)(int)g_AiCombatTargetScaleDeficitWeight *
                       (int64_t)
                       (int)(THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4) -
                            (uint32_t)hierarchyScaleRatioPairQ12)) /
                      (int64_t)(int)THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4));
            if (reachDeltaOrSourceCounter == 0) {
              candidateScore = candidateScore >> 2;
            }
            if (currentBestScore < (int)candidateScore) {
              sourceModelRuntime = (sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
              candidateAimModelNode = candidateArmyRuntime->modelNodeRuntime;
              sourceWeaponModelRuntime =
                   sourceModelRuntime->attachments[0].childModelRuntimeOrSavedOffset;
              if ((sourceModelRuntime->attachmentCount != 0) &&
                 ((candidateDefinition = (((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                           definitionOrSavedId).runtimeDefinition,
                  sourceWeaponModelRuntime != NULL ||
                  ((sourceWeaponModelRuntime =
                         sourceModelRuntime->attachments[1].childModelRuntimeOrSavedOffset,
                   sourceModelRuntime->attachmentCount != 1 &&
                   (sourceWeaponModelRuntime != NULL)))))) {
                /* the weapon is the first or second attachment; aircraft are aimed at their first child
                   node, all targets at the definition's height offset (+0x50) above the node */
                if (candidateDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
                  candidateAimModelNode = candidateAimModelNode->childNodes[0];
                }
                testPassed = ArmyWeaponRuntime_TestTargetLineOfFire
                                  (candidateDefinition->aimHeightOffsetQ12 +
                                   (candidateAimModelNode->worldTransform).translation.z,
                                   (candidateAimModelNode->worldTransform).translation.y,
                                   (candidateAimModelNode->worldTransform).translation.x,
                                   &g_InGameRuntimeRoot->worldRuntime,
                                   sourceWeaponModelRuntime);
                if ((testPassed) &&
                   (candidateScore = candidateScore >> 2,
                   (((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                    definitionOrSavedId).runtimeDefinition->accelerationPerTick == 0)) {
                  return 0;
                }
              }
            }
            return candidateScore;
          }
        }
      }
    }
  }
  return 0;
}

