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
            (selectionResult.sourceClassCount != 0 && -1 < THANDOR_BITCAST(AiCombatTargetSelectionResult, int64_t, selectionResult))) {
      ArmyRuntime_ResolveCommandTarget((ArmyRuntimeSlot *)armyRuntime->runtimeState98,armyRuntime);
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
           candidateCommandGenerationBase >> ((uint8_t)selectedCommandGenerationRightShiftBits & 0x1f);
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
  ArmyRuntimeSlot *collectedOrTargetArmy;
  AiTargetWorkspaceEntry *targetCandidateRecordCursor;
  ArmyRuntimeSlot **collectedArmyCursor;
  ModelRuntimeScaleRatioRegisterPairQ12 collectedHierarchyScaleRatioPairQ12;
  ArmyRuntimeSlot *candidateTargetArmy;

  if (1 < g_AiCollectedEntityCount) {
    accumulatedScaleRatio = 0;
    remainingOrBestScore = g_AiCollectedEntityCount;
    /* walks the collected army pointers: "->modelRuntimeOrSavedOffset.modelRuntime" (offset 0) reads the
       pointer at the cursor, "&->modelNodeRuntime" (offset 4) steps to the next one */
    collectedOrTargetArmy = (ArmyRuntimeSlot *)g_AiWorkspaceBuffer14_Size0100;
    do {
      collectedHierarchyScaleRatioPairQ12 =
           ModelRuntime_QueryHierarchyScaleRatioQ12Regs
                     ((RuntimeModelFactionPrefix10 *)
                      (collectedOrTargetArmy->modelRuntimeOrSavedOffset).modelRuntime);
      assignedCommandGeneration = g_AiCommandGenerationCandidateBase;
      THANDOR_PART(uint32_t, collectedHierarchyScaleRatioPairQ12, 4) =
           (uint32_t)(collectedHierarchyScaleRatioPairQ12 >> 0x20);
      if ((THANDOR_PART(uint32_t, collectedHierarchyScaleRatioPairQ12, 4) != 0) &&
         (accumulatedScaleRatio = accumulatedScaleRatio + (uint32_t)((int)collectedHierarchyScaleRatioPairQ12 << 8) /
                          THANDOR_PART(uint32_t, collectedHierarchyScaleRatioPairQ12, 4), 0x1ff < accumulatedScaleRatio)) {
        remainingOrBestScore = 0;
        targetCandidateRecordsRemaining = g_AiWorkspace07Count;
        targetCandidateRecordCursor = g_AiWorkspaceBuffer07_Size0400;
        if ((g_AiWorkspace07Count == 0) &&
           (targetCandidateRecordsRemaining = g_AiWorkspace03Count,
           targetCandidateRecordCursor = (AiTargetWorkspaceEntry *)g_AiWorkspaceBuffer03_Size1000,
           g_AiWorkspace03Count == 0)) {
          return;
        }
        do {
          candidateTargetArmy = targetCandidateRecordCursor->armyRuntime;
          /* ties go to the later candidate */
          if ((candidateTargetArmy != NULL) &&
             (targetClassIndex = candidateTargetArmy->modelRuntimeOrSavedOffset.modelRuntime->definitionValue9C_4C
             , remainingOrBestScore <= *(uint32_t *)(&g_AiCombatTargetClassBaseScoreTable24 + targetClassIndex * 4))) {
            remainingOrBestScore = *(uint32_t *)(&g_AiCombatTargetClassBaseScoreTable24 + targetClassIndex * 4);
            collectedOrTargetArmy = candidateTargetArmy;
          }
          targetCandidateRecordCursor++;
          targetCandidateRecordsRemaining--;
        } while (targetCandidateRecordsRemaining != 0);
        if (remainingOrBestScore == 0) {
          return;
        }
        targetRuntime = collectedOrTargetArmy->linkedEntityRuntime;
        remainingOrBestScore = g_AiCollectedEntityCount;
        collectedArmyCursor = g_AiWorkspaceBuffer14_Size0100;
        do {
          collectedOrTargetArmy = *collectedArmyCursor;
          ArmyRuntime_ResolveCommandTargetAndRoute(targetRuntime,collectedOrTargetArmy);
          collectedOrTargetArmy->runtimeState98 = (uint32_t)targetRuntime;
          collectedOrTargetArmy->commandModeFlags = collectedOrTargetArmy->commandModeFlags | 4;
          collectedOrTargetArmy->runtimeState94 = collectedOrTargetArmy->runtimeState94 | 1;
          collectedOrTargetArmy->movementStateFlags = collectedOrTargetArmy->movementStateFlags & ~0x200;
          collectedOrTargetArmy->runtimeState8C = 8;
          collectedOrTargetArmy->commandGeneration = assignedCommandGeneration;
          collectedArmyCursor++;
          remainingOrBestScore--;
        } while (remainingOrBestScore != 0);
        return;
      }
      collectedOrTargetArmy = (ArmyRuntimeSlot *)&collectedOrTargetArmy->modelNodeRuntime;
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
  WorldRuntimeNode *ownerNodeCursor;
  ModelRuntimeNode *sourceModelNode;
  GameEntityRuntime *candidateEntityRuntime;
  FactionRuntimeIndex sourceFactionIndex;
  
  sourceClassCount = 0;
  ownerNodeCursor = (WorldRuntimeNode *)worldRuntime->ownerListHead;
  classIndexOrFaction = 7;
  bestCandidateArmyRuntime = NULL;
  /* the eight class counters at +0x100 */
  do {
    sourceClassCount = sourceClassCount + sourceArmyRuntime->targetClassCounters100[classIndexOrFaction];
    classIndexOrFaction--;
  } while (-1 < classIndexOrFaction);
  /* With a zero sum the original returns an EDX never written in this call (stack slot [EBP-0x10]);
     returnedSourceClassCount is likewise left unset. */
  if (sourceClassCount != 0) {
    sourceModelNode = sourceArmyRuntime->modelNodeRuntime;
    searchRadiusQ12 = sourceArmyRuntime->runtimeState4C + 0x4000;
    sourceDepthMask1 =
         DepthInterval_BuildBinMask(searchRadiusQ12,(sourceModelNode->worldTransform).translation.x)
    ;
    sourceDepthMask0 =
         DepthInterval_BuildBinMask(searchRadiusQ12,(sourceModelNode->worldTransform).translation.y)
    ;
    sourceFactionIndex = sourceArmyRuntime->factionIndex;
    currentBestScore = 0;
    for (; returnedSourceClassCount = sourceClassCount, ownerNodeCursor != NULL;
        ownerNodeCursor = (ownerNodeCursor->common).nextNode) {
      /* the owner node's dword at +0xA4 must be zero */
      if (ownerNodeCursor[2].common.nextNode == NULL) {
        candidateEntityRuntime = ownerNodeCursor->runtimePayload;
        candidateArmyRuntime = (candidateEntityRuntime->common).ownership.runtimeLink;
        if ((((-1 < sourceClassCount) ||
             (((candidateEntityRuntime->common).runtimeFlags & 0x400) == 0)) &&
            (((candidateEntityRuntime->common).runtimeFlags & 8) == 0)) &&
           (classIndexOrFaction = candidateArmyRuntime->factionIndex, classIndexOrFaction != 0)) {
          /* Non-positive class counts look for other armies of the own faction, positive ones for armies
             of other factions. */
          if ((sourceClassCount < 1) ?
              ((classIndexOrFaction == sourceArmyRuntime->factionIndex) &&
               (sourceArmyRuntime != candidateArmyRuntime)) :
              (classIndexOrFaction != sourceArmyRuntime->factionIndex)) {
            /* bit 1 of the source faction's 2-bit field in the candidate's +0x50 */
            if (((candidateArmyRuntime->terrainOccupancyMask0 &
                 2 << ((char)sourceFactionIndex * '\x02' & 0x1fU)) != 0) &&
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
  uint32_t factionOrDefinitionAddress;
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
  
  factionOrDefinitionAddress = candidateArmyRuntime->factionIndex;
  candidateModelNode = candidateArmyRuntime->modelNodeRuntime;
  testPassed = DepthBinMasks_Overlap
                    (candidateModelNode->depthBinMaskFar,candidateModelNode->depthBinMaskNear,sourceDepthMask0,
                     sourceDepthMask1);
  if (testPassed) {
    if (sourceClassCount < 1) {
      testPassed = GameFactionRuntime_TestCapabilityBitClear(factionOrDefinitionAddress,sourceArmyRuntime->factionIndex);
      if (testPassed) {
        return 0;
      }
    }
    else {
      testPassed = GameFactionRuntime_TestCapabilityBitClear(factionOrDefinitionAddress,sourceArmyRuntime->factionIndex);
      if (!testPassed) {
        return 0;
      }
    }
    /* clearance^2 = (radius + 2.0)^2 - dx^2 - dy^2 in 64 bits; negative rejects */
    reachDeltaOrSourceCounter = sourceArmyRuntime->runtimeState4C + 0x2000;
    deltaXOrCandidateCounter = (sourceArmyRuntime->modelNodeRuntime->worldTransform).translation.x -
            (candidateModelNode->worldTransform).translation.x;
    clearanceSquaredOrWeight = (int64_t)reachDeltaOrSourceCounter * (int64_t)reachDeltaOrSourceCounter - (int64_t)deltaXOrCandidateCounter * (int64_t)deltaXOrCandidateCounter;
    if (-1 < clearanceSquaredOrWeight) {
      reachDeltaOrSourceCounter = (sourceArmyRuntime->modelNodeRuntime->worldTransform).translation.y -
              (candidateModelNode->worldTransform).translation.y;
      clearanceSquaredOrWeight = clearanceSquaredOrWeight - (int64_t)reachDeltaOrSourceCounter * (int64_t)reachDeltaOrSourceCounter;
      if (-1 < clearanceSquaredOrWeight) {
        radialClearanceQ12 =
             FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)clearanceSquaredOrWeight >> 32),(UInt64Half32)clearanceSquaredOrWeight);
        clearanceSquaredOrWeight = (int64_t)(int)g_AiCombatTargetRadialClearanceWeight;
        sourceRadiusQ12 = sourceArmyRuntime->runtimeState4C;
        factionOrDefinitionAddress = (((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                definitionOrSavedId).savedIdOrOffset;
        /* class counters (+0x100): the candidate's for the source's class and the source's for the
           candidate's class; a zero candidate counter selects the shorter command time (shift 2) */
        reachDeltaOrSourceCounter = candidateArmyRuntime->targetClassCounters100
                        [(((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                         definitionOrSavedId).runtimeDefinition->targetClassIndex5C];
        deltaXOrCandidateCounter = sourceArmyRuntime->targetClassCounters100[((ModelDefinitionRuntimeSemanticView280 *)factionOrDefinitionAddress)->targetClassIndex5C];
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
          classBaseScore = *(int *)(&g_AiCombatTargetClassBaseScoreTable24 + ((ModelDefinitionRuntimeSemanticView280 *)factionOrDefinitionAddress)->runtimeClassId4C * 4) *
                  g_AiCombatTargetClassBaseScoreMultiplier;
          hierarchyScaleRatioPairQ12 =
               ModelRuntime_QueryHierarchyScaleRatioQ12Regs
                         ((RuntimeModelFactionPrefix10 *)candidateArmyRuntime);
          THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4) = (uint32_t)(hierarchyScaleRatioPairQ12 >> 32);
          /* low half = condition ratio, high half = Q12 unity; a friendly search needs condition < 1.0 */
          if ((-1 < sourceClassCount) ||
             ((uint32_t)hierarchyScaleRatioPairQ12 < THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4))) {
            candidateScore =
                 (int)(((int)radialClearanceQ12 * clearanceSquaredOrWeight) / (int64_t)(int)sourceRadiusQ12) + classBaseScore +
                 (int)(((int64_t)(int)g_AiCombatTargetSourceCounterCountWeight * (int64_t)reachDeltaOrSourceCounter) /
                      (int64_t)(int)THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4)) +
                 (int)(((int64_t)(int)g_AiCombatTargetCandidateCounterCountWeight * (int64_t)deltaXOrCandidateCounter) /
                      (int64_t)(int)THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4)) +
                 (int)(((int64_t)(int)g_AiCombatTargetScaleDeficitWeight *
                       (int64_t)
                       (int)(THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4) - (uint32_t)hierarchyScaleRatioPairQ12)) /
                      (int64_t)(int)THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4));
            if (reachDeltaOrSourceCounter == 0) {
              candidateScore = candidateScore >> 2;
            }
            if (currentBestScore < (int)candidateScore) {
              sourceModelRuntime = (sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
              candidateAimModelNode = candidateArmyRuntime->modelNodeRuntime;
              sourceWeaponModelRuntime =
                   sourceModelRuntime->attachments140[0].childModelRuntimeOrSavedOffset00;
              if ((sourceModelRuntime->attachmentCount0C != 0) &&
                 ((factionOrDefinitionAddress = (((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                           definitionOrSavedId).savedIdOrOffset,
                  sourceWeaponModelRuntime != NULL ||
                  ((sourceWeaponModelRuntime =
                         sourceModelRuntime->attachments140[1].childModelRuntimeOrSavedOffset00,
                   sourceModelRuntime->attachmentCount0C != 1 &&
                   (sourceWeaponModelRuntime != NULL)))))) {
                /* the weapon is the first or second attachment; aircraft are aimed at their first child
                   node, all targets at the definition's height offset (+0x50) above the node */
                if (((ModelDefinitionRuntimeSemanticView280 *)factionOrDefinitionAddress)->runtimeClassId4C == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
                  candidateAimModelNode = candidateAimModelNode->childNodes[0];
                }
                testPassed = ArmyWeaponRuntime_TestTargetLineOfFire
                                  (((ModelDefinitionRuntimeSemanticView280 *)factionOrDefinitionAddress)->aimHeightOffsetQ12 +
                                   (candidateAimModelNode->worldTransform).translation.z,
                                   (candidateAimModelNode->worldTransform).translation.y,
                                   (candidateAimModelNode->worldTransform).translation.x,
                                   &g_InGameRuntimeRoot->worldRuntime0A30,
                                   (ArmyRuntimeSlot *)sourceWeaponModelRuntime);
                if ((testPassed) &&
                   (candidateScore = candidateScore >> 2,
                   (((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                    definitionOrSavedId).runtimeDefinition->runtimeValue18 == 0)) {
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

