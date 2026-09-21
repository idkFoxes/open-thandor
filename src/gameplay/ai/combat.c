/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/combat.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/combat.h>

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
  AiCombatTargetSelectionResult AVar1;
  ArmyCommandGeneration candidateCommandGenerationBase;
  AiCommandGenerationRightShiftBits selectedCommandGenerationRightShiftBits;
  
  if (((armyRuntime->commandModeFlags & 4) == 0) &&
     (((int)armyRuntime->commandGeneration < 1 || ((armyRuntime->commandModeFlags & 3) == 0)))) {
    AVar1 = AiCombatTarget_SelectBestCandidate(worldRuntime,armyRuntime);
    selectedCommandGenerationRightShiftBits =
         g_AiCombatTargetSelectedCommandGenerationRightShiftBits;
    candidateCommandGenerationBase = g_AiCommandGenerationCandidateBase;
    selectedTargetArmyRuntime = AVar1.targetArmyRuntime;
    if (selectedTargetArmyRuntime == armyRuntime->commandTargetArmyRuntime) {
      armyRuntime->commandGeneration = g_AiCommandGenerationRetainedTarget;
    }
    else if ((selectedTargetArmyRuntime == (ArmyRuntimeSlot *)0x0) &&
            (AVar1.sourceClassCount != 0 && -1 < (longlong)AVar1)) {
      ArmyRuntime_ResolveCommandTarget((ArmyRuntimeSlot *)armyRuntime->runtimeState98,armyRuntime);
      if ((armyRuntime->commandModeFlags & 1) == 0) {
        armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | 4;
      }
    }
    else {
      ArmyRuntime_ResolveCommandTarget(selectedTargetArmyRuntime,armyRuntime);
      armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | 8;
      armyRuntime->commandGeneration =
           candidateCommandGenerationBase >> ((byte)selectedCommandGenerationRightShiftBits & 0x1f);
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
  dword dVar1;
  GameEntityRuntime *targetRuntime;
  ArmyCommandGeneration AVar2;
  uint uVar3;
  int targetCandidateRecordsRemaining;
  uint uVar4;
  ArmyRuntimeSlot *armyRuntime7;
  AiTargetWorkspaceEntry *targetCandidateRecordCursor;
  ArmyRuntimeSlot **ppAVar5;
  ModelRuntimeScaleRatioRegisterPairQ12 collectedHierarchyScaleRatioPairQ12;
  byte *candidateArmyRuntime;
  ArmyRuntimeSlot *armyRuntime4;
  
  if (1 < g_AiCollectedEntityCount) {
    uVar4 = 0;
    uVar3 = g_AiCollectedEntityCount;
    armyRuntime7 = (ArmyRuntimeSlot *)g_AiWorkspaceBuffer14_Size0100;
    do {
      collectedHierarchyScaleRatioPairQ12 =
           ModelRuntime_QueryHierarchyScaleRatioQ12Regs
                     ((RuntimeModelFactionPrefix10 *)
                      (armyRuntime7->modelRuntimeOrSavedOffset).modelRuntime);
      AVar2 = g_AiCommandGenerationCandidateBase;
      collectedHierarchyScaleRatioPairQ12._4_4_ =
           (uint)(collectedHierarchyScaleRatioPairQ12 >> 0x20);
      if ((collectedHierarchyScaleRatioPairQ12._4_4_ != 0) &&
         (uVar4 = uVar4 + (uint)((int)collectedHierarchyScaleRatioPairQ12 << 8) /
                          collectedHierarchyScaleRatioPairQ12._4_4_, 0x1ff < uVar4)) {
        uVar3 = 0;
        targetCandidateRecordsRemaining = g_AiWorkspace07Count;
        targetCandidateRecordCursor = g_AiWorkspaceBuffer07_Size0400;
        if ((g_AiWorkspace07Count == 0) &&
           (targetCandidateRecordsRemaining = g_AiWorkspace03Count,
           targetCandidateRecordCursor = (AiTargetWorkspaceEntry *)g_AiWorkspaceBuffer03_Size1000,
           g_AiWorkspace03Count == 0)) {
          return;
        }
        do {
          armyRuntime4 = targetCandidateRecordCursor->armyRuntime;
          if ((armyRuntime4 != (ArmyRuntimeSlot *)0x0) &&
             (dVar1 = ((armyRuntime4->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C
             , uVar3 <= *(uint *)(&g_AiCombatTargetClassBaseScoreTable24 + dVar1 * 4))) {
            uVar3 = *(uint *)(&g_AiCombatTargetClassBaseScoreTable24 + dVar1 * 4);
            armyRuntime7 = armyRuntime4;
          }
          targetCandidateRecordCursor = targetCandidateRecordCursor + 1;
          targetCandidateRecordsRemaining = targetCandidateRecordsRemaining + -1;
        } while (targetCandidateRecordsRemaining != 0);
        if (uVar3 == 0) {
          return;
        }
        targetRuntime = armyRuntime7->linkedEntityRuntime;
        uVar3 = g_AiCollectedEntityCount;
        ppAVar5 = g_AiWorkspaceBuffer14_Size0100;
        do {
          armyRuntime7 = *ppAVar5;
          ArmyRuntime_ResolveCommandTargetAndRoute(targetRuntime,armyRuntime7);
          armyRuntime7->runtimeState98 = (dword)targetRuntime;
          armyRuntime7->commandModeFlags = armyRuntime7->commandModeFlags | 4;
          armyRuntime7->runtimeState94 = armyRuntime7->runtimeState94 | 1;
          armyRuntime7->movementStateFlags = armyRuntime7->movementStateFlags & 0xfffffdff;
          armyRuntime7->runtimeState8C = 8;
          armyRuntime7->commandGeneration = AVar2;
          ppAVar5 = ppAVar5 + 1;
          uVar3 = uVar3 - 1;
        } while (uVar3 != 0);
        return;
      }
      armyRuntime7 = (ArmyRuntimeSlot *)&armyRuntime7->modelNodeRuntime;
      uVar3 = uVar3 - 1;
    } while (uVar3 != 0);
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
  int iVar1;
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
  iVar1 = 7;
  bestCandidateArmyRuntime = (ArmyRuntimeSlot *)0x0;
  do {
    sourceClassCount = sourceClassCount + *(int *)(sourceArmyRuntime->reservedF8_FF + iVar1 * 4 + 8)
    ;
    iVar1 = iVar1 + -1;
  } while (-1 < iVar1);
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
           (iVar1 = candidateArmyRuntime->factionIndex, iVar1 != 0)) {
          if (sourceClassCount < 1) {
            if ((iVar1 == sourceArmyRuntime->factionIndex) &&
               (sourceArmyRuntime != candidateArmyRuntime))
            goto AiCombatTarget_SelectBestCandidate_EvaluateRelationEligibleCandidateAndUpdateBest;
          }
          else if (iVar1 != sourceArmyRuntime->factionIndex) {
AiCombatTarget_SelectBestCandidate_EvaluateRelationEligibleCandidateAndUpdateBest:
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
  dword dVar1;
  longlong lVar2;
  int iVar3;
  int iVar4;
  dword radialClearanceQ12;
  ModelRuntimeSlot *sourceWeaponModelRuntime;
  ModelRuntimeNode *candidateAimModelNode;
  int iVar5;
  bool bVar6;
  ModelRuntimeScaleRatioRegisterPairQ12 hierarchyScaleRatioPairQ12;
  uint candidateScore;
  dword sourceRadiusQ12;
  ModelRuntimeSlot *sourceModelRuntime;
  ModelRuntimeNode *modelNode1;
  
  dVar1 = candidateArmyRuntime->factionIndex;
  modelNode1 = candidateArmyRuntime->modelNodeRuntime;
  bVar6 = DepthBinMasks_OverlapCf
                    (modelNode1->depthBinMaskFar,modelNode1->depthBinMaskNear,sourceDepthMask0,
                     sourceDepthMask1);
  if (bVar6) {
    if (sourceClassCount < 1) {
      bVar6 = GameFactionRuntime_TestCapabilityBitClearCf(dVar1,sourceArmyRuntime->factionIndex);
      if (bVar6) {
        return 0;
      }
    }
    else {
      bVar6 = GameFactionRuntime_TestCapabilityBitClearCf(dVar1,sourceArmyRuntime->factionIndex);
      if (!bVar6) {
        return 0;
      }
    }
    iVar3 = sourceArmyRuntime->runtimeState4C + 0x2000;
    iVar4 = (sourceArmyRuntime->modelNodeRuntime->worldTransform).translation.x -
            (modelNode1->worldTransform).translation.x;
    lVar2 = (longlong)iVar3 * (longlong)iVar3 - (longlong)iVar4 * (longlong)iVar4;
    if (-1 < lVar2) {
      iVar3 = (sourceArmyRuntime->modelNodeRuntime->worldTransform).translation.y -
              (modelNode1->worldTransform).translation.y;
      lVar2 = lVar2 - (longlong)iVar3 * (longlong)iVar3;
      if (-1 < lVar2) {
        radialClearanceQ12 =
             FixedMath_UInt64Sqrt((UInt64Half32)((ulonglong)lVar2 >> 0x20),(UInt64Half32)lVar2);
        lVar2 = (longlong)g_AiCombatTargetRadialClearanceWeight;
        sourceRadiusQ12 = sourceArmyRuntime->runtimeState4C;
        dVar1 = (((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                definitionOrSavedId).savedIdOrOffset;
        iVar3 = *(int *)(candidateArmyRuntime->reservedF8_FF +
                        *(int *)((((sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                                 definitionOrSavedId).savedIdOrOffset + 0x5c) * 4 + 8);
        iVar4 = *(int *)(sourceArmyRuntime->reservedF8_FF + *(int *)(dVar1 + 0x5c) * 4 + 8);
        g_AiCombatTargetCurrentCommandGenerationRightShiftBits = 0;
        if (iVar3 == 0) {
          g_AiCombatTargetCurrentCommandGenerationRightShiftBits = 2;
        }
        if (iVar4 != 0) {
          if (iVar4 < 0) {
            iVar3 = 0;
            iVar4 = -iVar4;
          }
          iVar5 = *(int *)(&g_AiCombatTargetClassBaseScoreTable24 + *(int *)(dVar1 + 0x4c) * 4) *
                  g_AiCombatTargetClassBaseScoreMultiplier;
          hierarchyScaleRatioPairQ12 =
               ModelRuntime_QueryHierarchyScaleRatioQ12Regs
                         ((RuntimeModelFactionPrefix10 *)candidateArmyRuntime);
          hierarchyScaleRatioPairQ12._4_4_ = (uint)(hierarchyScaleRatioPairQ12 >> 0x20);
          if ((-1 < sourceClassCount) ||
             ((uint)hierarchyScaleRatioPairQ12 < hierarchyScaleRatioPairQ12._4_4_)) {
            candidateScore =
                 (int)(((int)radialClearanceQ12 * lVar2) / (longlong)(int)sourceRadiusQ12) + iVar5 +
                 (int)(((longlong)g_AiCombatTargetSourceCounterCountWeight * (longlong)iVar3) /
                      (longlong)(int)hierarchyScaleRatioPairQ12._4_4_) +
                 (int)(((longlong)g_AiCombatTargetCandidateCounterCountWeight * (longlong)iVar4) /
                      (longlong)(int)hierarchyScaleRatioPairQ12._4_4_) +
                 (int)(((longlong)g_AiCombatTargetScaleDeficitWeight *
                       (longlong)
                       (int)(hierarchyScaleRatioPairQ12._4_4_ - (uint)hierarchyScaleRatioPairQ12)) /
                      (longlong)(int)hierarchyScaleRatioPairQ12._4_4_);
            if (iVar3 == 0) {
              candidateScore = candidateScore >> 2;
            }
            if (currentBestScore < (int)candidateScore) {
              sourceModelRuntime = (sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
              candidateAimModelNode = candidateArmyRuntime->modelNodeRuntime;
              sourceWeaponModelRuntime =
                   sourceModelRuntime->attachments140[0].childModelRuntimeOrSavedOffset00;
              if ((sourceModelRuntime->attachmentCount0C != 0) &&
                 ((dVar1 = (((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                           definitionOrSavedId).savedIdOrOffset,
                  sourceWeaponModelRuntime != (ModelRuntimeSlot *)0x0 ||
                  ((sourceWeaponModelRuntime =
                         sourceModelRuntime->attachments140[1].childModelRuntimeOrSavedOffset00,
                   sourceModelRuntime->attachmentCount0C != 1 &&
                   (sourceWeaponModelRuntime != (ModelRuntimeSlot *)0x0)))))) {
                if (*(int *)(dVar1 + 0x4c) == 0x15) {
                  candidateAimModelNode = candidateAimModelNode->childNodes[0];
                }
                bVar6 = ArmyWeaponRuntime_TestTargetLineOfFireCf
                                  (*(int *)(dVar1 + 0x50) +
                                   (candidateAimModelNode->worldTransform).translation.z,
                                   (candidateAimModelNode->worldTransform).translation.y,
                                   (candidateAimModelNode->worldTransform).translation.x,
                                   &g_InGameRuntimeRoot->worldRuntime0A30,
                                   (ArmyRuntimeSlot *)sourceWeaponModelRuntime);
                if ((bVar6) &&
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

