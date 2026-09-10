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
void AiCombatDecision_UpdateTargetAssignment
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  GameEntityRuntime *resolvedTarget;
  byte extraout_CL;
  AiCombatTargetSelectionResult64 AVar2;
  
  if (((armyRuntime->commandModeFlags & 4) == 0) &&
     (((int)armyRuntime->commandGeneration < 1 || ((armyRuntime->commandModeFlags & 3) == 0)))) {
    AVar2 = AiCombatTarget_SelectBestCandidate(worldRuntime,armyRuntime);
    uVar1 = g_AiCommandGenerationCandidateBase;
    resolvedTarget = (GameEntityRuntime *)AVar2;
    if (resolvedTarget == (GameEntityRuntime *)armyRuntime->commandTargetArmyRuntime) {
      armyRuntime->commandGeneration = g_AiCommandGenerationRetainedTarget;
    }
    else if ((resolvedTarget == (GameEntityRuntime *)0x0) &&
            ((int)(AVar2 >> 0x20) != 0 && -1 < (longlong)AVar2)) {
      ArmyRuntime_ResolveCommandTarget((GameEntityRuntime *)armyRuntime->runtimeState98,armyRuntime)
      ;
      if ((armyRuntime->commandModeFlags & 1) == 0) {
        armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | 4;
      }
    }
    else {
      ArmyRuntime_ResolveCommandTarget(resolvedTarget,armyRuntime);
      armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | 8;
      armyRuntime->commandGeneration = uVar1 >> (extraout_CL & 0x1f);
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
  ArmyCommandModeFlags *pAVar1;
  ArmyMovementStateFlags *pAVar2;
  dword *pdVar3;
  int iVar4;
  GameEntityRuntime *targetRuntime;
  GameEntityRuntime *armyRuntime;
  int extraout_ECX;
  int targetCandidateRecordsRemaining;
  int extraout_ECX_00;
  ArmyCommandGeneration extraout_EDX;
  uint uVar5;
  ArmyRuntimeSlot *armyRuntime7;
  AiTargetWorkspaceEntry *targetCandidateRecordCursor;
  GameEntityRuntime **ppGVar6;
  ModelRuntimeScaleRatioRegisterPairQ12 collectedHierarchyScaleRatioPairQ12;
  byte *candidateArmyRuntime;
  ArmyRuntimeSlot *armyRuntime4;
  
  if (1 < g_AiCollectedEntityCount) {
    uVar5 = 0;
    armyRuntime7 = (ArmyRuntimeSlot *)g_AiWorkspaceBuffer14_Size0100;
    do {
      collectedHierarchyScaleRatioPairQ12 =
           ModelRuntime_QueryHierarchyScaleRatioQ12Regs(armyRuntime7->definitionOrAsset);
      collectedHierarchyScaleRatioPairQ12._4_4_ =
           (uint)(collectedHierarchyScaleRatioPairQ12 >> 0x20);
      if ((collectedHierarchyScaleRatioPairQ12._4_4_ != 0) &&
         (uVar5 = uVar5 + (uint)((int)collectedHierarchyScaleRatioPairQ12 << 8) /
                          collectedHierarchyScaleRatioPairQ12._4_4_, 0x1ff < uVar5)) {
        uVar5 = 0;
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
             (iVar4 = *(int *)((int)armyRuntime4->definitionOrAsset + 0x4c),
             uVar5 <= *(uint *)(&g_AiCombatTargetClassBaseScoreTable24 + iVar4 * 4))) {
            uVar5 = *(uint *)(&g_AiCombatTargetClassBaseScoreTable24 + iVar4 * 4);
            armyRuntime7 = armyRuntime4;
          }
          targetCandidateRecordCursor = targetCandidateRecordCursor + 1;
          targetCandidateRecordsRemaining = targetCandidateRecordsRemaining + -1;
        } while (targetCandidateRecordsRemaining != 0);
        if (uVar5 == 0) {
          return;
        }
        targetRuntime = armyRuntime7->linkedEntityRuntime;
        ppGVar6 = g_AiWorkspaceBuffer14_Size0100;
        do {
          armyRuntime = *ppGVar6;
          ArmyRuntime_ResolveCommandTargetAndRoute(targetRuntime,(ArmyRuntimeSlot *)armyRuntime);
          *(GameEntityRuntime **)((int)(armyRuntime->common).reserved68_9F + 0x30) = targetRuntime;
          pAVar1 = &(armyRuntime->common).commandTarget.targetFlags;
          *pAVar1 = *pAVar1 | 4;
          pdVar3 = (dword *)((int)(armyRuntime->common).reserved68_9F + 0x2c);
          *pdVar3 = *pdVar3 | 1;
          pAVar2 = &(armyRuntime->common).commandFlags;
          *pAVar2 = *pAVar2 & 0xfffffdff;
          *(dword *)((int)(armyRuntime->common).reserved68_9F + 0x24) = 8;
          (armyRuntime->common).commandTarget.commandGeneration = extraout_EDX;
          ppGVar6 = ppGVar6 + 1;
        } while (extraout_ECX_00 != 1);
        return;
      }
      armyRuntime7 = (ArmyRuntimeSlot *)&armyRuntime7->modelNodeRuntime;
    } while (extraout_ECX != 1);
  }
  return;
}

/* Address: 0x005372C0.
   Ownership: gameplay/ai/combat.
   Purpose: Best combat target over the candidate set scored by AiCombatTarget_EvaluateCandidateScore.
   Local calls: AiCombatTarget_EvaluateCandidateScore.
   Cross-module calls: DepthInterval_BuildBinMask [graphics/render/primitives].
*/
AiCombatTargetSelectionResult64
AiCombatTarget_SelectBestCandidate
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  ArmyRuntimeSlot *candidateArmyRuntime;
  uint uVar1;
  int iVar2;
  uint extraout_ECX;
  Q12 searchRadiusQ12;
  AiCandidateScore32 currentBestScore;
  int sourceClassCount;
  DepthBinMaskEaxPreservedEdxCarrier64 xDepthMaskPair;
  DepthBinMaskEaxPreservedEdxCarrier64 yDepthMaskPair;
  longlong candidateScorePair;
  int returnedSourceClassCount;
  ArmyRuntimeSlot *bestCandidateArmyRuntime;
  WorldRuntimeNode *ownerNodeCursor;
  ModelRuntimeNode *sourceModelNode;
  void *candidateRuntimePayload;
  
  sourceClassCount = 0;
  ownerNodeCursor = worldRuntime->ownerListHead;
  iVar2 = 7;
  bestCandidateArmyRuntime = (ArmyRuntimeSlot *)0x0;
  do {
    sourceClassCount = sourceClassCount + *(int *)(sourceArmyRuntime->reservedF8_FF + iVar2 * 4 + 8)
    ;
    iVar2 = iVar2 + -1;
  } while (-1 < iVar2);
  if (sourceClassCount != 0) {
    sourceModelNode = sourceArmyRuntime->modelNodeRuntime;
    searchRadiusQ12 = sourceArmyRuntime->runtimeState4C + 0x4000;
    xDepthMaskPair =
         DepthInterval_BuildBinMask(searchRadiusQ12,(sourceModelNode->worldTransform).translation.x)
    ;
    yDepthMaskPair =
         DepthInterval_BuildBinMask
                   ((DepthIntervalRadius32)(xDepthMaskPair >> 0x20),
                    (sourceModelNode->worldTransform).translation.y);
    currentBestScore = 0;
    uVar1 = 2 << ((char)sourceArmyRuntime->factionIndex * '\x02' & 0x1fU);
    for (; returnedSourceClassCount = sourceClassCount, ownerNodeCursor != (WorldRuntimeNode *)0x0;
        ownerNodeCursor = (ownerNodeCursor->common).nextNode) {
      if (ownerNodeCursor[2].common.nextNode == (WorldRuntimeNode *)0x0) {
        candidateRuntimePayload = ownerNodeCursor->runtimePayload;
        candidateArmyRuntime = *(ArmyRuntimeSlot **)((int)candidateRuntimePayload + 8);
        if ((((-1 < sourceClassCount) ||
             ((*(uint *)((int)candidateRuntimePayload + 0xec) & 0x400) == 0)) &&
            ((*(uint *)((int)candidateRuntimePayload + 0xec) & 8) == 0)) &&
           (iVar2 = candidateArmyRuntime->factionIndex, iVar2 != 0)) {
          if (sourceClassCount < 1) {
            if ((iVar2 == sourceArmyRuntime->factionIndex) &&
               (sourceArmyRuntime != candidateArmyRuntime))
            goto AiCombatTarget_SelectBestCandidate_EvaluateRelationEligibleCandidateAndUpdateBest;
          }
          else if (iVar2 != sourceArmyRuntime->factionIndex) {
AiCombatTarget_SelectBestCandidate_EvaluateRelationEligibleCandidateAndUpdateBest:
            if (((candidateArmyRuntime->terrainOccupancyMask0 & uVar1) != 0) &&
               (candidateScorePair._0_4_ =
                     AiCombatTarget_EvaluateCandidateScore
                               (currentBestScore,sourceClassCount,(DepthBinMask32)yDepthMaskPair,
                                (DepthBinMask32)xDepthMaskPair,candidateArmyRuntime,
                                sourceArmyRuntime), uVar1 = extraout_ECX,
               currentBestScore = candidateScorePair._4_4_,
               candidateScorePair._4_4_ < (AiCandidateScore32)candidateScorePair)) {
              g_AiCombatTargetSelectedCommandGenerationShift =
                   g_AiCombatTargetCurrentCommandGenerationShift;
              currentBestScore = (AiCandidateScore32)candidateScorePair;
              bestCandidateArmyRuntime = candidateArmyRuntime;
            }
          }
        }
      }
    }
  }
  return CONCAT44(returnedSourceClassCount,bestCandidateArmyRuntime);
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
AiCandidateScore32
AiCombatTarget_EvaluateCandidateScore
          (AiCandidateScore32 currentBestScore,AiSourceClassCount sourceClassCount,
          DepthBinMask32 sourceDepthMask0,DepthBinMask32 sourceDepthMask1,
          ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime)

{
  longlong lVar1;
  int iVar2;
  int iVar3;
  dword radialClearanceQ12;
  dword capabilityBitIndex;
  int extraout_ECX;
  ArmyRuntimeSlot *sourceWeaponRuntime;
  ModelRuntimeNode *candidateAimModelNode;
  bool bVar4;
  ModelRuntimeScaleRatioRegisterPairQ12 hierarchyScaleRatioPairQ12;
  uint candidateScore;
  dword sourceRadiusQ12;
  void *sourceDefinitionOrAsset;
  ModelRuntimeNode *modelNode1;
  
  bVar4 = &stack0xffffffe8 < (undefined1 *)0x4;
  modelNode1 = candidateArmyRuntime->modelNodeRuntime;
  DepthBinMasks_OverlapCf
            (modelNode1->depthBinMaskFar,modelNode1->depthBinMaskNear,sourceDepthMask0,
             sourceDepthMask1);
  if (bVar4) {
    bVar4 = false;
    if (sourceClassCount < 1) {
      GameFactionRuntime_TestCapabilityBitClearCf
                (capabilityBitIndex,sourceArmyRuntime->factionIndex);
      if (bVar4) {
        return 0;
      }
    }
    else {
      GameFactionRuntime_TestCapabilityBitClearCf
                (capabilityBitIndex,sourceArmyRuntime->factionIndex);
      if (!bVar4) {
        return 0;
      }
    }
    iVar2 = sourceArmyRuntime->runtimeState4C + 0x2000;
    iVar3 = (sourceArmyRuntime->modelNodeRuntime->worldTransform).translation.x -
            (modelNode1->worldTransform).translation.x;
    lVar1 = (longlong)iVar2 * (longlong)iVar2 - (longlong)iVar3 * (longlong)iVar3;
    if (-1 < lVar1) {
      iVar2 = (sourceArmyRuntime->modelNodeRuntime->worldTransform).translation.y -
              (modelNode1->worldTransform).translation.y;
      lVar1 = lVar1 - (longlong)iVar2 * (longlong)iVar2;
      if (-1 < lVar1) {
        radialClearanceQ12 =
             FixedMath_UInt64Sqrt((UInt64Half32)((ulonglong)lVar1 >> 0x20),(UInt64Half32)lVar1);
        lVar1 = (longlong)g_AiCombatTargetRadialClearanceWeight;
        sourceRadiusQ12 = sourceArmyRuntime->runtimeState4C;
        iVar2 = *(int *)(candidateArmyRuntime->reservedF8_FF +
                        *(int *)(*(int *)sourceArmyRuntime->definitionOrAsset + 0x5c) * 4 + 8);
        g_AiCombatTargetCurrentCommandGenerationShift = 0;
        if (iVar2 == 0) {
          g_AiCombatTargetCurrentCommandGenerationShift = 2;
        }
        if (*(int *)(sourceArmyRuntime->reservedF8_FF +
                    *(int *)(*(int *)candidateArmyRuntime->definitionOrAsset + 0x5c) * 4 + 8) != 0)
        {
          if (*(int *)(sourceArmyRuntime->reservedF8_FF +
                      *(int *)(*(int *)candidateArmyRuntime->definitionOrAsset + 0x5c) * 4 + 8) < 0)
          {
            iVar2 = 0;
          }
          iVar3 = *(int *)(&g_AiCombatTargetClassBaseScoreTable24 +
                          *(int *)(*(int *)candidateArmyRuntime->definitionOrAsset + 0x4c) * 4) *
                  g_AiCombatTargetClassBaseScoreMultiplier;
          hierarchyScaleRatioPairQ12 =
               ModelRuntime_QueryHierarchyScaleRatioQ12Regs(candidateArmyRuntime);
          hierarchyScaleRatioPairQ12._4_4_ = (uint)(hierarchyScaleRatioPairQ12 >> 0x20);
          if ((-1 < sourceClassCount) ||
             ((uint)hierarchyScaleRatioPairQ12 < hierarchyScaleRatioPairQ12._4_4_)) {
            candidateScore =
                 (int)(((int)radialClearanceQ12 * lVar1) / (longlong)(int)sourceRadiusQ12) + iVar3 +
                 (int)(((longlong)g_AiCombatTargetSourceCounterCountWeight * (longlong)iVar2) /
                      (longlong)(int)hierarchyScaleRatioPairQ12._4_4_) +
                 (int)(((longlong)g_AiCombatTargetCandidateCounterCountWeight *
                       (longlong)extraout_ECX) / (longlong)(int)hierarchyScaleRatioPairQ12._4_4_) +
                 (int)(((longlong)g_AiCombatTargetScaleDeficitWeight *
                       (longlong)
                       (int)(hierarchyScaleRatioPairQ12._4_4_ - (uint)hierarchyScaleRatioPairQ12)) /
                      (longlong)(int)hierarchyScaleRatioPairQ12._4_4_);
            if (iVar2 == 0) {
              candidateScore = candidateScore >> 2;
            }
            if (currentBestScore < (int)candidateScore) {
              sourceDefinitionOrAsset = sourceArmyRuntime->definitionOrAsset;
              candidateAimModelNode = candidateArmyRuntime->modelNodeRuntime;
              sourceWeaponRuntime = *(ArmyRuntimeSlot **)((int)sourceDefinitionOrAsset + 0x140);
              if ((*(int *)((int)sourceDefinitionOrAsset + 0xc) != 0) &&
                 ((sourceWeaponRuntime != (ArmyRuntimeSlot *)0x0 ||
                  ((sourceWeaponRuntime =
                         *(ArmyRuntimeSlot **)((int)sourceDefinitionOrAsset + 0x160),
                   *(int *)((int)sourceDefinitionOrAsset + 0xc) != 1 &&
                   (sourceWeaponRuntime != (ArmyRuntimeSlot *)0x0)))))) {
                if (*(int *)(*(int *)candidateArmyRuntime->definitionOrAsset + 0x4c) == 0x15) {
                  candidateAimModelNode = candidateAimModelNode->childNodes[0];
                }
                bVar4 = (InGameRuntimeRootImageC3E4 *)0xfffff5cf < g_InGameRuntimeRoot;
                ArmyWeaponRuntime_TestTargetLineOfFireCf
                          (*(int *)(*(int *)candidateArmyRuntime->definitionOrAsset + 0x50) +
                           (candidateAimModelNode->worldTransform).translation.z,
                           (candidateAimModelNode->worldTransform).translation.y,
                           (candidateAimModelNode->worldTransform).translation.x,
                           &g_InGameRuntimeRoot->worldRuntime0A30,sourceWeaponRuntime);
                if ((bVar4) &&
                   (candidateScore = candidateScore >> 2,
                   *(int *)(*(int *)sourceArmyRuntime->definitionOrAsset + 0x18) == 0)) {
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
