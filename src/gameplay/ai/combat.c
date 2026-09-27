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
   Ownership: gameplay/ai/combat.
   Purpose: Runs target selection for an eligible AI-controlled entity. It preserves the current target when still
   best, clears or delays selection when no valid candidate remains, and otherwise assigns the selected target
   while updating decision-state flags and cooldown.
   Local calls: AiCombatTarget_SelectBestCandidate.
   Cross-module calls: ArmyRuntime_ResolveCommandTarget [gameplay/army/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
AiCombatDecision_UpdateTargetAssignment
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyRuntimeSlot *selectedTargetArmyRuntime;
  AiCombatTargetSelectionResult selectionResult;
  ArmyCommandGeneration candidateCommandGenerationBase;
  AiCommandGenerationRightShiftBits selectedCommandGenerationRightShiftBits;
  
  if (((armyRuntime->commandModeFlags & 4) == 0) &&
     (((int)armyRuntime->commandGeneration < 1 || ((armyRuntime->commandModeFlags & 3) == 0)))) {
    selectionResult = AiCombatTarget_SelectBestCandidate(worldRuntime,armyRuntime);
    selectedCommandGenerationRightShiftBits =
         g_AiCombatTargetSelectedCommandGenerationRightShiftBits;
    candidateCommandGenerationBase = g_AiCommandGenerationCandidateBase;
    selectedTargetArmyRuntime = selectionResult.targetArmyRuntime;
    if (selectedTargetArmyRuntime == armyRuntime->commandTargetArmyRuntime) {
      armyRuntime->commandGeneration = g_AiCommandGenerationRetainedTarget;
    }
    else if ((selectedTargetArmyRuntime == (ArmyRuntimeSlot *)0x0) &&
            (selectionResult.sourceClassCount != 0 && -1 < THANDOR_BITCAST(AiCombatTargetSelectionResult, int64_t, selectionResult))) {
      ArmyRuntime_ResolveCommandTarget((ArmyRuntimeSlot *)armyRuntime->runtimeState98,armyRuntime);
      if ((armyRuntime->commandModeFlags & 1) == 0) {
        armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | 4;
      }
    }
    else {
      ArmyRuntime_ResolveCommandTarget(selectedTargetArmyRuntime,armyRuntime);
      armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | 8;
      armyRuntime->commandGeneration =
           candidateCommandGenerationBase >> ((uint8_t)selectedCommandGenerationRightShiftBits & 0x1f);
    }
  }
  return;
}


/* Address: 0x0053B8B0.
   Ownership: gameplay/ai/combat.
   Purpose: GROUP ATTACK also SINGLE-derefs instance+0x4C — falsifies the docs note that the group-attack priority
   path used the type field (ai-economy.md corrected 2026-07-31).
   Cross-module calls: ModelRuntime_QueryHierarchyScaleRatioQ12Regs [world/model/runtime],
   ArmyRuntime_ResolveCommandTargetAndRoute [gameplay/army/movement].
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
  uint8_t *candidateArmyRuntime;
  ArmyRuntimeSlot *candidateTargetArmy;
  
  if (1 < g_AiCollectedEntityCount) {
    accumulatedScaleRatio = 0;
    remainingOrBestScore = g_AiCollectedEntityCount;
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
          if ((candidateTargetArmy != (ArmyRuntimeSlot *)0x0) &&
             (targetClassIndex = ((candidateTargetArmy->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C
             , remainingOrBestScore <= *(uint32_t *)(&g_AiCombatTargetClassBaseScoreTable24 + targetClassIndex * 4))) {
            remainingOrBestScore = *(uint32_t *)(&g_AiCombatTargetClassBaseScoreTable24 + targetClassIndex * 4);
            collectedOrTargetArmy = candidateTargetArmy;
          }
          targetCandidateRecordCursor = targetCandidateRecordCursor + 1;
          targetCandidateRecordsRemaining = targetCandidateRecordsRemaining + -1;
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
          collectedOrTargetArmy->movementStateFlags = collectedOrTargetArmy->movementStateFlags & 0xfffffdff;
          collectedOrTargetArmy->runtimeState8C = 8;
          collectedOrTargetArmy->commandGeneration = assignedCommandGeneration;
          collectedArmyCursor = collectedArmyCursor + 1;
          remainingOrBestScore = remainingOrBestScore - 1;
        } while (remainingOrBestScore != 0);
        return;
      }
      collectedOrTargetArmy = (ArmyRuntimeSlot *)&collectedOrTargetArmy->modelNodeRuntime;
      remainingOrBestScore = remainingOrBestScore - 1;
    } while (remainingOrBestScore != 0);
  }
  return;
}


/* Address: 0x005372C0.
   Ownership: gameplay/ai/combat.
   Purpose: Best combat target over the candidate set scored by AiCombatTarget_EvaluateCandidateScore.
   Local calls: AiCombatTarget_EvaluateCandidateScore.
   Cross-module calls: DepthInterval_BuildBinMask [graphics/render/primitives].
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
  bestCandidateArmyRuntime = (ArmyRuntimeSlot *)0x0;
  do {
    sourceClassCount = sourceClassCount + *(int *)(sourceArmyRuntime->reservedF8_FF + classIndexOrFaction * 4 + 8)
    ;
    classIndexOrFaction = classIndexOrFaction + -1;
  } while (-1 < classIndexOrFaction);
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
    for (; returnedSourceClassCount = sourceClassCount, ownerNodeCursor != (WorldRuntimeNode *)0x0;
        ownerNodeCursor = (ownerNodeCursor->common).nextNode) {
      if (ownerNodeCursor[2].common.nextNode == (WorldRuntimeNode *)0x0) {
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
   Ownership: gameplay/ai/combat.
   Purpose: Evaluates one opposing runtime entity using depth-mask overlap, faction relation state, horizontal
   separation, configured class weights, health or readiness values, and a verified line-of-fire or reachability
   test. It returns a positive score for an acceptable target or zero for rejection. The CORRECT double-deref
   contrast case: reads (*definitionOrAsset+0x5C) — live type fields, unlike the single-deref military dispatch /
   income / group-attack paths. Typed parameters: p4 sourceDepthMask0→DepthBinMask32_V338, p5
   sourceDepthMask1→DepthBinMask32_V338. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Cross-module calls: DepthBinMasks_OverlapCf [graphics/render/primitives],
   GameFactionRuntime_TestCapabilityBitClearCf [gameplay/faction/runtime], FixedMath_UInt64Sqrt [core/math/fixed],
   ModelRuntime_QueryHierarchyScaleRatioQ12Regs [world/model/runtime], ArmyWeaponRuntime_TestTargetLineOfFireCf
   [gameplay/army/combat].
*/
AiCandidateScore32 __thandor_eax_preserve_ecx_edx
AiCombatTarget_EvaluateCandidateScore
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
  testPassed = DepthBinMasks_OverlapCf
                    (candidateModelNode->depthBinMaskFar,candidateModelNode->depthBinMaskNear,sourceDepthMask0,
                     sourceDepthMask1);
  if (testPassed) {
    if (sourceClassCount < 1) {
      testPassed = GameFactionRuntime_TestCapabilityBitClearCf(factionOrDefinitionAddress,sourceArmyRuntime->factionIndex);
      if (testPassed) {
        return 0;
      }
    }
    else {
      testPassed = GameFactionRuntime_TestCapabilityBitClearCf(factionOrDefinitionAddress,sourceArmyRuntime->factionIndex);
      if (!testPassed) {
        return 0;
      }
    }
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
             FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)clearanceSquaredOrWeight >> 0x20),(UInt64Half32)clearanceSquaredOrWeight);
        clearanceSquaredOrWeight = (int64_t)(int)g_AiCombatTargetRadialClearanceWeight;
        sourceRadiusQ12 = sourceArmyRuntime->runtimeState4C;
        factionOrDefinitionAddress = (((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                definitionOrSavedId).savedIdOrOffset;
        reachDeltaOrSourceCounter = *(int *)(candidateArmyRuntime->reservedF8_FF +
                        *(int *)((((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                                 definitionOrSavedId).savedIdOrOffset + 0x5c) * 4 + 8);
        deltaXOrCandidateCounter = *(int *)(sourceArmyRuntime->reservedF8_FF + *(int *)(factionOrDefinitionAddress + 0x5c) * 4 + 8);
        g_AiCombatTargetCurrentCommandGenerationRightShiftBits = 0;
        if (reachDeltaOrSourceCounter == 0) {
          g_AiCombatTargetCurrentCommandGenerationRightShiftBits = 2;
        }
        if (deltaXOrCandidateCounter != 0) {
          if (deltaXOrCandidateCounter < 0) {
            reachDeltaOrSourceCounter = 0;
            deltaXOrCandidateCounter = -deltaXOrCandidateCounter;
          }
          classBaseScore = *(int *)(&g_AiCombatTargetClassBaseScoreTable24 + *(int *)(factionOrDefinitionAddress + 0x4c) * 4) *
                  g_AiCombatTargetClassBaseScoreMultiplier;
          hierarchyScaleRatioPairQ12 =
               ModelRuntime_QueryHierarchyScaleRatioQ12Regs
                         ((RuntimeModelFactionPrefix10 *)candidateArmyRuntime);
          THANDOR_PART(uint32_t, hierarchyScaleRatioPairQ12, 4) = (uint32_t)(hierarchyScaleRatioPairQ12 >> 0x20);
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
                  sourceWeaponModelRuntime != (ModelRuntimeSlot *)0x0 ||
                  ((sourceWeaponModelRuntime =
                         sourceModelRuntime->attachments140[1].childModelRuntimeOrSavedOffset00,
                   sourceModelRuntime->attachmentCount0C != 1 &&
                   (sourceWeaponModelRuntime != (ModelRuntimeSlot *)0x0)))))) {
                if (*(int *)(factionOrDefinitionAddress + 0x4c) == 0x15) {
                  candidateAimModelNode = candidateAimModelNode->childNodes[0];
                }
                testPassed = ArmyWeaponRuntime_TestTargetLineOfFireCf
                                  (*(int *)(factionOrDefinitionAddress + 0x50) +
                                   (candidateAimModelNode->worldTransform).translation.z,
                                   (candidateAimModelNode->worldTransform).translation.y,
                                   (candidateAimModelNode->worldTransform).translation.x,
                                   &g_InGameRuntimeRoot->worldRuntime0A30,
                                   (ArmyRuntimeSlot *)sourceWeaponModelRuntime);
                if ((testPassed) &&
                   (candidateScore = candidateScore >> 2,
                   *(int *)((((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                            definitionOrSavedId).savedIdOrOffset + 0x18) == 0)) {
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

