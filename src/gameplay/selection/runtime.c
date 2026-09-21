/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/selection/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/selection/runtime.h>

/* Implementation ownership: gameplay/selection/runtime. */

/* Address: 0x0052E350.
   Ownership: gameplay/selection/runtime.
   Purpose: Begins framebuffer access and composes the complete army and model-runtime selection panel from
   hierarchy metrics, status flags, icons, number cells, bars, and segmented indicators before ending access. Typed
   parameters: p2 clipTop→UiPixelCoordinate_V297, p3 clipLeft→UiPixelCoordinate_V297, p4
   clipBottom→UiPixelCoordinate_V297, p5 clipRight→UiPixelCoordinate_V297. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p6 panelBottom→UiPixelCoordinate_V297, p7 panelRight→UiPixelCoordinate_V297, p8
   panelTop→UiPixelCoordinate_V297, p9 panelLeft→UiPixelCoordinate_V297. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs, SelectionPanel_DrawIconCellAndAdvanceRegs,
   SelectionPanel_DrawNumberCellAndAdvanceRegs, SelectionPanel_DrawProportionalCappedBar,
   SelectionPanel_DrawSolidCappedBar, SelectionPanel_DrawSegmentedCappedBar, SelectionPanel_DrawForwardCappedBar.
   Cross-module calls: ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs [world/model/runtime],
   GameFactionRuntime_FindRuntimeGroupIndexCf [gameplay/faction/runtime],
   ModelRuntime_QueryHierarchyScaleRatioQ12Regs [world/model/runtime], ArmyRuntime_QueryMetric6CAndDefinitionC4Regs
   [gameplay/army/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_RenderArmyRuntimeMetrics
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate panelBottom,UiPixelCoordinate panelRight,
          UiPixelCoordinate panelTop,UiPixelCoordinate panelLeft,
          RuntimeModelFactionPrefix10 *runtimeEntry)

{
  dword dVar1;
  ModelRuntimeSlot *pMVar2;
  ArmyCommandGeneration maximumValue;
  InGameRuntimeRootImageC3E4 *pIVar3;
  uint uVar4;
  SelectionPanelSegmentCount totalSegmentCount;
  SelectionPanelSegmentCount totalSegmentCount_00;
  SelectionPanelSegmentCount totalSegmentCount_01;
  int maximumValue_00;
  int iVar5;
  int iVar6;
  uint uVar7;
  ModelRuntimeSlot *armyRuntime;
  bool bVar8;
  ModelRuntimeActiveTotalMetricRegisterPair MVar9;
  SelectionPanelAdvanceEaxEdx8 SVar10;
  SelectionPanelAdvanceEaxEdx8 SVar11;
  SelectionPanelAdvanceEaxEdx8 SVar12;
  SelectionPanelAdvanceEaxEdx8 SVar13;
  ModelRuntimeScaleRatioRegisterPairQ12 MVar14;
  FactionRuntimeGroupIndexEaxCf5 FVar15;
  ArmyMetric6CDefinitionC4Regs8 AVar16;
  
  pIVar3 = g_InGameRuntimeRoot;
  bVar8 = (*g_GraphicsFramebufferBeginAccess)();
  if (bVar8) {
    return;
  }
  uVar4 = runtimeEntry->runtimeLinkOrKind08;
  armyRuntime = runtimeEntry->modelRuntime;
  if (runtimeEntry->factionIndex == (pIVar3->worldRuntime0A30).activeFactionRuntimeIndex) {
    if (uVar4 != 0) {
      if (uVar4 == 1) {
        if (((armyRuntime->classState).classStateB8 == 1) &&
           ((((ModelRuntimeSlot *)(armyRuntime->definitionOrSavedId).savedIdOrOffset)->
             definitionValue9C_4C == 0xd ||
            (((ModelRuntimeSlot *)(armyRuntime->definitionOrSavedId).savedIdOrOffset)->
             definitionValue9C_4C == 0xb)))) {
          MVar9 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          SVar10 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,
                              (UiNumericValue32)(MVar9 >> 0x20),(UiNumericValue32)MVar9,0x12);
          FVar15 = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
          if (FVar15.carry) {
            SVar11 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            SVar11 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                FVar15.runtimeGroupIndex,0xb);
          }
          SVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          SVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,SVar11.nextDrawY,
                     SVar10.nextDrawY,(armyRuntime->classLinkState).classState68,
                     (armyRuntime->classLinkState).classState64,0x16);
          MVar14 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,SVar13.nextDrawY,
                     SVar12.nextDrawY,(UiNumericValue32)(MVar14 >> 0x20),(UiNumericValue32)MVar14,
                     0x19);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,SVar12.nextDrawX,SVar10.nextDrawX,
                     panelLeft,5);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,SVar13.nextDrawX,SVar11.nextDrawX,
                     panelRight,6);
          goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
        }
      }
      else if (uVar4 < 3) {
        pMVar2 = ((ModelRuntimeAttachmentDescriptor *)(armyRuntime->reserved120_13F + 0x20))->
                 childModelRuntimeOrSavedOffset00;
        if (((armyRuntime->attachmentCount0C != 0) && (pMVar2 != (ModelRuntimeSlot *)0x0)) &&
           ((dVar1 = (pMVar2->definitionOrSavedId).savedIdOrOffset, *(int *)(dVar1 + 0x4c) == 5 ||
            (((*(int *)(dVar1 + 0x4c) == 6 || (*(int *)(dVar1 + 0x4c) == 7)) ||
             (*(int *)(dVar1 + 0x4c) == 8)))))) {
          iVar5 = *(int *)(pMVar2->reserved10_37 + 0x14);
          iVar6 = *(int *)((pMVar2->definitionOrSavedId).savedIdOrOffset + 0x30);
          MVar9 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          maximumValue_00 = (int)(MVar9 >> 0x20);
          if (maximumValue_00 == 0) {
            SVar10 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
          }
          else {
            SVar10 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,
                                maximumValue_00,(UiNumericValue32)MVar9,0x12);
          }
          FVar15 = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
          if (FVar15.carry) {
            SVar11 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            SVar11 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                FVar15.runtimeGroupIndex,0xb);
          }
          SVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          SVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,SVar11.nextDrawY,
                     SVar10.nextDrawY,iVar6,iVar6 - iVar5,0x16);
          MVar14 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,SVar13.nextDrawY,
                     SVar12.nextDrawY,(UiNumericValue32)(MVar14 >> 0x20),(UiNumericValue32)MVar14,
                     0x19);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,SVar12.nextDrawX,SVar10.nextDrawX,
                     panelLeft,5);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,SVar13.nextDrawX,SVar11.nextDrawX,
                     panelRight,6);
          goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
        }
      }
      else if (uVar4 == 3) {
        pMVar2 = ((ModelRuntimeAttachmentDescriptor *)(armyRuntime->reserved120_13F + 0x20))->
                 childModelRuntimeOrSavedOffset00;
        if ((armyRuntime->attachmentCount0C != 0) && (pMVar2 != (ModelRuntimeSlot *)0x0)) {
          armyRuntime = (ModelRuntimeSlot *)(pMVar2->definitionOrSavedId).savedIdOrOffset;
          uVar4 = 0xffffffff;
          iVar5 = 7;
          if (armyRuntime->definitionValue9C_4C == 9) {
            do {
              if (*(uint *)((pMVar2->classState).reserved84_A7 + iVar5 * 4 + -0x24) < uVar4) {
                uVar4 = *(uint *)((pMVar2->classState).reserved84_A7 + iVar5 * 4 + -0x24);
              }
              iVar5 = iVar5 + -1;
            } while (-1 < iVar5);
            if ((int)uVar4 < (int)(pMVar2->classLinkState).classState80) {
              uVar4 = (pMVar2->classLinkState).classState80;
            }
            iVar5 = 0;
            maximumValue = *(ArmyCommandGeneration *)
                            ((ArmyRuntimeMovementControlState *)armyRuntime->reserved10_37 + 4);
            if (0 < (int)uVar4) {
              iVar5 = -uVar4;
            }
            MVar9 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
            iVar6 = (int)(MVar9 >> 0x20);
            if (iVar6 == 0) {
              SVar10 = SelectionPanel_DrawIconCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
            }
            else {
              SVar10 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,iVar6,
                                  (UiNumericValue32)MVar9,0x12);
            }
            FVar15 = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
            if (FVar15.carry) {
              SVar11 = SelectionPanel_DrawIconCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
            }
            else {
              SVar11 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                  FVar15.runtimeGroupIndex,0xb);
            }
            SVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
            SVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
            SelectionPanel_DrawProportionalCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,panelTop,SVar11.nextDrawY,
                       SVar10.nextDrawY,maximumValue,iVar5 + maximumValue,0x16);
            MVar14 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
            SelectionPanel_DrawProportionalCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,panelBottom,SVar13.nextDrawY,
                       SVar12.nextDrawY,(UiNumericValue32)(MVar14 >> 0x20),(UiNumericValue32)MVar14,
                       0x19);
            SelectionPanel_DrawSolidCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,SVar12.nextDrawX,SVar10.nextDrawX,
                       panelLeft,5);
            SelectionPanel_DrawSolidCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,SVar13.nextDrawX,SVar11.nextDrawX,
                       panelRight,6);
            goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
          }
        }
      }
      else if ((3 < uVar4) &&
              (((ModelRuntimeSlot *)(armyRuntime->definitionOrSavedId).savedIdOrOffset)->
               definitionValue9C_4C == 0x16)) {
        if (((ArmyRuntimeArticulatedContactState14 *)&(armyRuntime->classState).classStateAC)->
            terrainContactMode == ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE) {
          MVar9 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          SVar10 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,
                              (UiNumericValue32)(MVar9 >> 0x20),(UiNumericValue32)MVar9,0x12);
          FVar15 = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
          if (FVar15.carry) {
            SVar11 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            SVar11 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                FVar15.runtimeGroupIndex,0xb);
          }
          SVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          SVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,SVar11.nextDrawY,
                     SVar10.nextDrawY,(armyRuntime->classLinkState).classState68,
                     (armyRuntime->classLinkState).classState64,0x16);
          MVar14 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,SVar13.nextDrawY,
                     SVar12.nextDrawY,(UiNumericValue32)(MVar14 >> 0x20),(UiNumericValue32)MVar14,
                     0x19);
          AVar16 = ArmyRuntime_QueryMetric6CAndDefinitionC4Regs((ArmyRuntimeSlot *)armyRuntime);
          uVar7 = AVar16.ebx >> 1;
          uVar4 = AVar16.ecx >> 1;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,SVar12.nextDrawX,SVar10.nextDrawX,
                     panelLeft,AVar16.ecx - uVar4,AVar16.ebx - uVar7,0xf);
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,SVar13.nextDrawX,SVar11.nextDrawX,
                     panelRight,uVar4,uVar7,0x10);
        }
        else if (((armyRuntime->classState).classStateEC & 0xc0) == 0) {
          MVar9 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          iVar5 = (int)(MVar9 >> 0x20);
          if (iVar5 == 0) {
            SVar10 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
          }
          else {
            SVar10 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,iVar5,
                                (UiNumericValue32)MVar9,0x12);
          }
          FVar15 = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
          if (FVar15.carry) {
            SVar11 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            SVar11 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                FVar15.runtimeGroupIndex,0xb);
          }
          SVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          SVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawForwardCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,SVar11.nextDrawY,
                     SVar10.nextDrawY,4);
          MVar14 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,SVar13.nextDrawY,
                     SVar12.nextDrawY,(UiNumericValue32)(MVar14 >> 0x20),(UiNumericValue32)MVar14,
                     0x19);
          AVar16 = ArmyRuntime_QueryMetric6CAndDefinitionC4Regs((ArmyRuntimeSlot *)armyRuntime);
          uVar7 = AVar16.ebx >> 1;
          uVar4 = AVar16.ecx >> 1;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,SVar12.nextDrawX,SVar10.nextDrawX,
                     panelLeft,AVar16.ecx - uVar4,AVar16.ebx - uVar7,0xf);
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,SVar13.nextDrawX,SVar11.nextDrawX,
                     panelRight,uVar4,uVar7,0x10);
        }
        else {
          MVar9 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          iVar5 = (int)(MVar9 >> 0x20);
          if (iVar5 == 0) {
            SVar10 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
          }
          else {
            SVar10 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,iVar5,
                                (UiNumericValue32)MVar9,0x12);
          }
          FVar15 = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
          if (FVar15.carry) {
            SVar11 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            SVar11 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                FVar15.runtimeGroupIndex,0xb);
          }
          SVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          SVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,SVar11.nextDrawY,
                     SVar10.nextDrawY,*(dword *)(armyRuntime->reserved100_117 + 4),
                     *(UiNumericValue32 *)(armyRuntime->reserved100_117 + 8),0x16);
          MVar14 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,SVar13.nextDrawY,
                     SVar12.nextDrawY,(UiNumericValue32)(MVar14 >> 0x20),(UiNumericValue32)MVar14,
                     0x19);
          AVar16 = ArmyRuntime_QueryMetric6CAndDefinitionC4Regs((ArmyRuntimeSlot *)armyRuntime);
          uVar7 = AVar16.ebx >> 1;
          uVar4 = AVar16.ecx >> 1;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,SVar12.nextDrawX,SVar10.nextDrawX,
                     panelLeft,AVar16.ecx - uVar4,AVar16.ebx - uVar7,0xf);
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,SVar13.nextDrawX,SVar11.nextDrawX,
                     panelRight,uVar4,uVar7,0x10);
        }
        goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
      }
    }
    if (((armyRuntime->classState).classStateEC & 0xc0) == 0) {
      MVar9 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
      iVar5 = (int)(MVar9 >> 0x20);
      if (iVar5 == 0) {
        SVar10 = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
      }
      else {
        SVar10 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,iVar5,
                            (UiNumericValue32)MVar9,0x12);
      }
      FVar15 = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
      if (FVar15.carry) {
        SVar11 = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
      }
      else {
        SVar11 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                            FVar15.runtimeGroupIndex,0xb);
      }
      SVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
      SVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
      SelectionPanel_DrawForwardCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelTop,SVar11.nextDrawY,SVar10.nextDrawY,4)
      ;
      MVar14 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelBottom,SVar13.nextDrawY,SVar12.nextDrawY
                 ,(UiNumericValue32)(MVar14 >> 0x20),(UiNumericValue32)MVar14,0x19);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,SVar12.nextDrawX,SVar10.nextDrawX,panelLeft,5
                );
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,SVar13.nextDrawX,SVar11.nextDrawX,panelRight,
                 6);
    }
    else {
      MVar9 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
      iVar5 = (int)(MVar9 >> 0x20);
      if (iVar5 == 0) {
        SVar10 = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
      }
      else {
        SVar10 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,iVar5,
                            (UiNumericValue32)MVar9,0x12);
      }
      FVar15 = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
      if (FVar15.carry) {
        SVar11 = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
      }
      else {
        SVar11 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                            FVar15.runtimeGroupIndex,0xb);
      }
      SVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
      SVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelTop,SVar11.nextDrawY,SVar10.nextDrawY,
                 *(dword *)(armyRuntime->reserved100_117 + 4),
                 *(UiNumericValue32 *)(armyRuntime->reserved100_117 + 8),0x16);
      MVar14 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelBottom,SVar13.nextDrawY,SVar12.nextDrawY
                 ,(UiNumericValue32)(MVar14 >> 0x20),(UiNumericValue32)MVar14,0x19);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,SVar12.nextDrawX,SVar10.nextDrawX,panelLeft,5
                );
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,SVar13.nextDrawX,SVar11.nextDrawX,panelRight,
                 6);
    }
  }
  else {
    SVar10 = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
    SVar11 = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
    SVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
    SVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
    SelectionPanel_DrawForwardCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,panelTop,SVar11.nextDrawY,SVar10.nextDrawY,4);
    MVar14 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
    SelectionPanel_DrawProportionalCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,panelBottom,SVar13.nextDrawY,SVar12.nextDrawY,
               (UiNumericValue32)(MVar14 >> 0x20),(UiNumericValue32)MVar14,0x19);
    SelectionPanel_DrawSolidCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,SVar12.nextDrawX,SVar10.nextDrawX,panelLeft,5);
    SelectionPanel_DrawSolidCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,SVar13.nextDrawX,SVar11.nextDrawX,panelRight,6)
    ;
  }
SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn:
  (*g_GraphicsFramebufferEndAccess)();
  return;
}


/* Address: 0x0055FA20.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles in game selection rebuild owned class16 selection.
   Local calls: SelectionPointerArray_Clear32, SelectionPointerArray_InsertUniqueAndRecenter.
   Cross-module calls: InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime], UiCatalogGroup48_RebuildGrid
   [ui/ingame/technology].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSelection_RebuildOwnedClass16Selection
          (PlayerRuntimeId playerRuntimeId,dword callbackArg1,dword callbackArg2,dword callbackArg3)

{
  WorldOwnerListNode100 *pWVar1;
  GameEntityRuntime *entityRuntime;
  InGameRuntimeRootImageC3E4 *pIVar2;
  
  pIVar2 = g_InGameRuntimeRoot;
  SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  for (pWVar1 = (pIVar2->worldRuntime0A30).ownerListHead; pWVar1 != (WorldOwnerListNode100 *)0x0;
      pWVar1 = pWVar1->nextNode) {
    if (pWVar1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      entityRuntime = *(GameEntityRuntime **)((int)pWVar1->runtimePayload + 8);
      if ((*(int *)(*(int *)pWVar1->runtimePayload + 0x4c) == 0x16) &&
         ((entityRuntime->common).ownership.ownerIndex ==
          g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->primaryEntityOrFactionToken8080))
      {
        SelectionPointerArray_InsertUniqueAndRecenter
                  (entityRuntime,&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection)
        ;
      }
    }
  }
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
  return;
}


/* Address: 0x0055FB30.
   Ownership: gameplay/selection/runtime.
   Purpose: Replaces one player selection with the army runtime identified by the rebased index and refreshes local
   UI when applicable. Kept distinct from frontend slot indices, faction runtime indices, network endpoint
   identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage,
   body bytes, control flow, and executable data remain unchanged. Typed parameters: p3
   armyRuntimeIndex→RuntimeToken.
   Local calls: SelectionPointerArray_Clear32, SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity.
   Cross-module calls: InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime], UiCatalogGroup48_RebuildGrid
   [ui/ingame/technology].
*/
void __thandor_preserve_eax
InGamePlayerSelection_ReplaceWithArmyRuntimeIndex
          (PlayerRuntimeId playerId,dword payloadDword04,dword payloadDword08,
          RuntimeToken armyRuntimeIndex)

{
  ArmyRuntimeSlot *sourceArmyRuntime;
  
  if (armyRuntimeIndex != 0) {
    sourceArmyRuntime = (ArmyRuntimeSlot *)(armyRuntimeIndex + (int)g_ArmyRuntimeRebaseBaseMinusOne)
    ;
    SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
    if (sourceArmyRuntime->modelNodeRuntime != (ModelRuntimeNode *)0x0) {
      SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity
                (sourceArmyRuntime,&g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
      if (playerId == g_LocalPlayerRuntimeId) {
        InGameSelectionDetailPanel_Rebuild();
        UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
      }
    }
  }
  return;
}


/* Address: 0x0055FE70.
   Ownership: gameplay/selection/runtime.
   Purpose: Forwards command payload coordinates to position-command variant B using the player selection array.
   Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset
   identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage, body bytes, control
   flow, and executable data remain unchanged. Typed parameters: p2 payloadDword08→CommandPayloadDword08_V343, p3
   payloadDword0C→CommandPayloadDword0C_V343.
   Local calls: SelectionPointerArray_ApplyPositionCommandVariantB.
*/
void __thandor_preserve_eax_edx
InGamePlayerSelection_ApplyPositionCommandVariantB
          (PlayerRuntimeId playerId,dword payloadDword04,CommandPayloadDword08 payloadDword08,
          CommandPayloadDword0C payloadDword0C)

{
  SelectionPointerArray_ApplyPositionCommandVariantB
            (payloadDword08,payloadDword0C,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}


/* Address: 0x0055FEA0.
   Ownership: gameplay/selection/runtime.
   Purpose: Forwards command payload coordinates to SelectionPointerArray_ApplyPositionCommand for one player
   selection array. Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity,
   and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage, body
   bytes, control flow, and executable data remain unchanged. Typed parameters: p2
   payloadDword08→CommandPayloadDword08_V343, p3 payloadDword0C→CommandPayloadDword0C_V343.
   Local calls: SelectionPointerArray_ApplyPositionCommand.
*/
void __thandor_void_preserve_eax_ecx_edx
InGamePlayerSelection_ApplyPositionCommand
          (PlayerRuntimeId playerId,dword payloadDword04,CommandPayloadDword08 payloadDword08,
          CommandPayloadDword0C payloadDword0C)

{
  SelectionPointerArray_ApplyPositionCommand
            (payloadDword08,payloadDword0C,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}


/* Address: 0x0055FED0.
   Ownership: gameplay/selection/runtime.
   Purpose: Selects the army runtime identified by the rebased index for one player when the runtime is eligible.
   Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset
   identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage, body bytes, control
   flow, and executable data remain unchanged. Typed parameters: p3 armyRuntimeIndex→RuntimeToken.
   Local calls: SelectionPointerArray_ApplyArmyRuntimeTarget.
*/
void __thandor_preserve_eax
InGamePlayerSelection_SelectArmyRuntimeIndex
          (PlayerRuntimeId playerId,dword payloadDword04,dword payloadDword08,
          RuntimeToken armyRuntimeIndex)

{
  if ((armyRuntimeIndex != 0) &&
     (((ArmyRuntimeSlot *)(armyRuntimeIndex + (int)g_ArmyRuntimeRebaseBaseMinusOne))->
      modelNodeRuntime != (ModelRuntimeNode *)0x0)) {
    SelectionPointerArray_ApplyArmyRuntimeTarget
              ((ArmyRuntimeSlot *)(armyRuntimeIndex + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  }
  return;
}


/* Address: 0x0055FF10.
   Ownership: gameplay/selection/runtime.
   Purpose: Forwards a four-dword in-game command to the target-position helper using one player selection array.
   Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset
   identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage, body bytes, control
   flow, and executable data remain unchanged. Typed parameters: p1 payloadDword04→CommandPayloadDword04_V343, p2
   payloadDword08→CommandPayloadDword08_V343, p3 payloadDword0C→CommandPayloadDword0C_V343.
   Local calls: SelectionPointerArray_ApplyTargetPositionCommand.
*/
void __thandor_void_preserve_eax_ecx_edx
InGamePlayerSelection_ApplyTargetPositionCommand
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  SelectionPointerArray_ApplyTargetPositionCommand
            (payloadDword04,payloadDword08,payloadDword0C,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}


/* Address: 0x0055FF40.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles player selection reset movement prune and recenter entries.
   Local calls: SelectionRuntime_ResetMovementPruneAndRecenterEntries.
*/
void __thandor_preserve_eax
PlayerSelection_ResetMovementPruneAndRecenterEntries
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  SelectionRuntime_ResetMovementPruneAndRecenterEntries
            ((GameEntityRuntime **)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}


/* Address: 0x0055FF60.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles player selection reset movement anchors and clear flag200 for eligible entries.
   Local calls: SelectionRuntime_ResetMovementAnchorsAndClearFlag200ForEligibleEntries.
*/
void __thandor_preserve_eax
PlayerSelection_ResetMovementAnchorsAndClearFlag200ForEligibleEntries
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  SelectionRuntime_ResetMovementAnchorsAndClearFlag200ForEligibleEntries
            ((GameEntityRuntime **)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}


/* Address: 0x0055FF80.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles player selection interrupt targets and clear flag10 for eligible entries.
   Local calls: SelectionRuntime_InterruptTargetsAndClearFlag10ForEligibleEntries.
*/
void __thandor_preserve_eax
PlayerSelection_InterruptTargetsAndClearFlag10ForEligibleEntries
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  SelectionRuntime_InterruptTargetsAndClearFlag10ForEligibleEntries
            ((GameEntityRuntime **)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}


/* Address: 0x0055FFA0.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles player selection apply flags418 unless bit8 to eligible entries.
   Local calls: SelectionRuntime_ApplyFlags418UnlessBit8ToEligibleEntries.
*/
void __thandor_preserve_eax
PlayerSelection_ApplyFlags418UnlessBit8ToEligibleEntries
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  SelectionRuntime_ApplyFlags418UnlessBit8ToEligibleEntries
            ((GameEntityRuntime **)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}


/* Address: 0x0055FFC0.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles in game selection apply type16 marker coordinates variant1.
   Local calls: SelectionPointerArray_ApplyType16MarkerCoordinates.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSelection_ApplyType16MarkerCoordinatesVariant1
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (1,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return;
}


/* Address: 0x0055FFF0.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles in game selection apply type16 marker coordinates variant2.
   Local calls: SelectionPointerArray_ApplyType16MarkerCoordinates.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSelection_ApplyType16MarkerCoordinatesVariant2
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (2,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return;
}


/* Address: 0x00562050.
   Ownership: gameplay/selection/runtime.
   Purpose: Resolves the selected player primary entity, reapplies its current position to the entity and command-
   state coordinate fields, invokes the type-specific position callback, and refreshes dependent selection state.
   Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset
   identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body bytes,
   control flow, and executable data remain unchanged.
   Local calls: SelectionPointerArray_ContainsCf.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ModelNodeRuntime_UpdateDepthBinMasks [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerRuntime_ReissuePrimarySelectionPosition
          (PlayerRuntimeId playerRuntimeId,dword reserved,Q12 deltaYQ12,Q12 deltaXQ12)

{
  dword dVar1;
  ModelRuntimeNode *modelNode;
  int iVar2;
  int iVar3;
  int worldYQ12;
  GameEntityRuntime *target;
  int worldXQ12;
  WorldRuntimeContext *worldRuntime;
  
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  dVar1 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->primarySelectionEntityOffset8094;
  if (dVar1 != 0) {
    target = (GameEntityRuntime *)((int)g_ArmyRuntimeRebaseBaseMinusOne + dVar1);
    SelectionPointerArray_ContainsCf
              (target,&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    modelNode = (target->common).ownership.modelNode;
    worldYQ12 = deltaXQ12 + (modelNode->worldTransform).translation.x;
    worldXQ12 = deltaYQ12 + (modelNode->worldTransform).translation.y;
    iVar2 = *(int *)(target->common).ownership.definitionOrClassRecord;
    (target->common).pathCoordinate0Q12 = worldYQ12;
    (target->common).pathCoordinate1Q12 = worldXQ12;
    (target->common).trackedCoordinate0Q12 = worldYQ12;
    (target->common).trackedCoordinate1Q12 = worldXQ12;
    (target->common).damageState.trackedCoordinate0Q12 = worldYQ12;
    (target->common).damageState.trackedCoordinate1Q12 = worldXQ12;
    (modelNode->worldTransform).translation.x = worldYQ12;
    iVar3 = *(int *)(iVar2 + 0x278);
    (modelNode->worldTransform).translation.y = worldXQ12;
    if (iVar3 == 3) {
      ((modelNode->runtimePayload).armyRuntime)->movementTarget0Q12 = 0x7fffffff;
    }
    (*g_ArmyPlacementContactKindDispatchTable.callbacks[iVar3])
              (*(Q12 *)(iVar2 + 0x54),worldXQ12,worldYQ12,modelNode,worldRuntime);
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
    ModelNodeRuntime_UpdateDepthBinMasks(*(DepthIntervalRadius32 *)(iVar2 + 0xdc),modelNode);
  }
  return;
}


/* Address: 0x00562220.
   Ownership: gameplay/selection/runtime.
   Purpose: Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK
   asset identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body
   bytes, control flow, and executable data remain unchanged.
   Local calls: SelectionPointerArray_ContainsCf.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerRuntime_AdvancePrimarySelectionCycle
          (PlayerRuntimeId playerRuntimeId,dword reserved0,dword reserved1,AngleTurn32 angleDelta)

{
  dword dVar1;
  ModelRuntimeNode *modelNodeRuntime;
  GameEntityRuntime *target;
  
  dVar1 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->primarySelectionEntityOffset8094;
  if (dVar1 != 0) {
    target = (GameEntityRuntime *)((int)g_ArmyRuntimeRebaseBaseMinusOne + dVar1);
    SelectionPointerArray_ContainsCf
              (target,&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    modelNodeRuntime = (target->common).ownership.modelNode;
    (modelNodeRuntime->modelPayload).worldRotationAngle2 =
         angleDelta + (modelNodeRuntime->modelPayload).worldRotationAngle2 & 0xffff;
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  }
  return;
}


/* Address: 0x0052CEE0.
   Ownership: gameplay/selection/runtime.
   Purpose: Loads gfx\panel\select.gfx and info.gfx plus their 0x1A4-byte .dat tables, clears eight fixed runtime
   blocks, stores the caller's entity-slot table, and patches verified texture-source sequence descriptors used by
   the selection and information panels. The .dat record semantics remain opaque.
   Cross-module calls: Package_LoadEntry [assets/package/runtime].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots)

{
  byte *pbVar1;
  AssetRelativeOffset AVar2;
  GraphicsTextureSourceAsset *pGVar3;
  int iVar4;
  int iVar5;
  SelectionPlayerRuntimeBlock *pSVar6;
  GraphicsTextureSourceLoadEaxCf5 GVar7;
  PackageLoadEntryEaxCf5 PVar8;
  StatusValueEaxCf5 SVar9;
  StatusValueEaxCf5 SVar10;
  AssetRelativeOffset swappedSelectionDataOffset;
  dword referencePayloadValue;
  
  GVar7 = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_select_gfx_0052ce18);
  pGVar3 = GVar7.eax;
  if (!GVar7.carry) {
    g_SelectionPanelTextureSource = pGVar3;
    GVar7 = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_info_gfx_0052ce42);
    pGVar3 = GVar7.eax;
    if (!GVar7.carry) {
      g_InfoPanelTextureSource = pGVar3;
      PVar8 = Package_LoadEntry((word *)u_gfx_panel_select_dat_0052ce68);
      pGVar3 = PVar8.bufferOrError;
      if (!PVar8.carry) {
        g_SelectionPanelData = pGVar3;
        PVar8 = Package_LoadEntry((word *)u_gfx_panel_info_dat_0052ce92);
        pGVar3 = PVar8.bufferOrError;
        if (!PVar8.carry) {
          iVar5 = 8;
          pSVar6 = g_SelectionPlayerBlocks;
          g_InfoPanelData = pGVar3;
          do {
            for (iVar4 = 0x20; pGVar3 = g_SelectionPanelTextureSource, iVar4 != 0;
                iVar4 = iVar4 + -1) {
              (pSVar6->selection).entries[0] = (GameEntityRuntime *)0x0;
              pSVar6 = (SelectionPlayerRuntimeBlock *)((pSVar6->selection).entries + 1);
            }
            pSVar6 = (SelectionPlayerRuntimeBlock *)&pSVar6->pendingSelectionEntityOffset8098;
            iVar5 = iVar5 + -1;
          } while (iVar5 != 0);
          g_SelectionInfoEntitySlots = entitySlots;
          AVar2 = (g_SelectionPanelTextureSource->tableDescriptor).subresourceTableOffset;
          LOCK();
          swappedSelectionDataOffset =
               *(AssetRelativeOffset *)
                (g_SelectionPanelTextureSource[2].opaqueTablePayloadBC_1FF + AVar2 + 0x110);
          *(AssetRelativeOffset *)
           (g_SelectionPanelTextureSource[2].opaqueTablePayloadBC_1FF + AVar2 + 0x110) =
               *(AssetRelativeOffset *)
                (g_SelectionPanelTextureSource[2].opaqueTablePayloadBC_1FF + AVar2 + 0xf0);
          UNLOCK();
          *(AssetRelativeOffset *)(pGVar3[2].opaqueTablePayloadBC_1FF + AVar2 + 0xf0) =
               swappedSelectionDataOffset;
          pGVar3 = g_InfoPanelTextureSource;
          AVar2 = (g_InfoPanelTextureSource->tableDescriptor).subresourceTableOffset;
          referencePayloadValue =
               *(dword *)((g_InfoPanelTextureSource->common).buildMetadata.
                          assetRelativeAddressAnchor28 +
                         *(int *)(g_InfoPanelTextureSource[2].opaqueTablePayloadBC_1FF +
                                 AVar2 + 0xd0) + -0x28);
          iVar5 = *(int *)(g_InfoPanelTextureSource[2].opaqueTablePayloadBC_1FF + AVar2 + 0x130);
          pbVar1 = (g_InfoPanelTextureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                   iVar5 + -0x28;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(dword *)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x24) =
               referencePayloadValue;
          iVar5 = *(int *)(pGVar3[3].common.buildMetadata.assetRelativeAddressAnchor28 +
                          (AVar2 - 0x1c));
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x28;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(dword *)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x24) =
               referencePayloadValue;
          iVar5 = *(int *)(pGVar3[3].common.buildMetadata.assetRelativeAddressAnchor28 + AVar2 + 4);
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x28;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(dword *)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x24) =
               referencePayloadValue;
          *(undefined4 *)((int)pGVar3[3].common.buildMetadata.names.producerName + AVar2 + 0x2c) = 4
          ;
          *(undefined4 *)((int)pGVar3[3].common.buildMetadata.names.producerName + AVar2 + 0x14) = 4
          ;
          *(undefined4 *)((int)pGVar3[3].common.buildMetadata.names.sourceName + AVar2 + 0xc) = 4;
          *(undefined4 *)((int)pGVar3[3].common.buildMetadata.names.producerName + AVar2 + 0x34) = 4
          ;
          pbVar1 = pGVar3[2].opaqueTablePayloadBC_1FF + AVar2 + 0x100;
          pbVar1[0] = 4;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          pbVar1 = pGVar3[2].opaqueTablePayloadBC_1FF + AVar2 + 0xe8;
          pbVar1[0] = 4;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          pbVar1 = pGVar3[2].opaqueTablePayloadBC_1FF + AVar2 + 0x120;
          pbVar1[0] = 4;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          pbVar1 = pGVar3[2].opaqueTablePayloadBC_1FF + AVar2 + 0x108;
          pbVar1[0] = 4;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          iVar5 = *(int *)((int)pGVar3[3].common.buildMetadata.names.producerName + AVar2 + 0x1c);
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x28;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(dword *)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x24) =
               referencePayloadValue;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x20;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x1c;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x18;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x14;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x10;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(dword *)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0xc) =
               referencePayloadValue;
          iVar5 = *(int *)((int)pGVar3[3].common.buildMetadata.names.producerName + AVar2 + 0x3c);
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x28;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(dword *)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x24) =
               referencePayloadValue;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x20;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x1c;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x18;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x14;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x10;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(dword *)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0xc) =
               referencePayloadValue;
          iVar5 = *(int *)(pGVar3[2].opaqueTablePayloadBC_1FF + AVar2 + 0xf0);
          *(dword *)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x28) =
               referencePayloadValue;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x24;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x20;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x1c;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x18;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x14;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          *(dword *)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x10) =
               referencePayloadValue;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0xc;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          iVar5 = *(int *)(pGVar3[2].opaqueTablePayloadBC_1FF + AVar2 + 0x110);
          *(dword *)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x28) =
               referencePayloadValue;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x24;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x20;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x1c;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x18;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x14;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          *(dword *)((pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x10) =
               referencePayloadValue;
          pbVar1 = (pGVar3->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0xc;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          SVar9.valueOrError = 0xffff0000;
          SVar9.carry = false;
          return SVar9;
        }
      }
    }
  }
  SVar10.carry = true;
  SVar10.valueOrError = (dword)pGVar3;
  return SVar10;
}


/* Address: 0x0052D0F0.
   Ownership: gameplay/selection/runtime.
   Purpose: Releases both panel texture sources and both package-loaded .dat tables, then clears the four global
   resource pointers.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_preserve_eax SelectionInfoPanel_ShutdownResources(void)

{
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_SelectionPanelTextureSource);
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_InfoPanelTextureSource);
  Resource_Release(g_SelectionPanelData);
  Resource_Release(g_InfoPanelData);
  g_SelectionPanelTextureSource = (GraphicsTextureSourceAsset *)0x0;
  g_InfoPanelTextureSource = (GraphicsTextureSourceAsset *)0x0;
  g_SelectionPanelData = (void *)0x0;
  g_InfoPanelData = (void *)0x0;
  return;
}


/* Address: 0x0052FB20.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the first 32 pointers in each of eight exact 0x8118-byte player blocks and clears every entry
   equal to target. All incoming general registers are restored.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerBlocks_RemovePointer(GameEntityRuntime *target)

{
  int entriesRemainingInBlock;
  int playerBlocksRemaining;
  SelectionPlayerRuntimeBlock *currentSelectionEntry;
  SelectionPlayerRuntimeBlock *selectionEntryCursor;
  
  playerBlocksRemaining = 8;
  entriesRemainingInBlock = 0x20;
  selectionEntryCursor = g_SelectionPlayerBlocks;
  do {
    do {
      currentSelectionEntry = selectionEntryCursor;
      if (target == (currentSelectionEntry->selection).entries[0]) {
        (currentSelectionEntry->selection).entries[0] = (GameEntityRuntime *)0x0;
      }
      entriesRemainingInBlock = entriesRemainingInBlock + -1;
      selectionEntryCursor =
           (SelectionPlayerRuntimeBlock *)((currentSelectionEntry->selection).entries + 1);
    } while (entriesRemainingInBlock != 0);
    entriesRemainingInBlock = 0x20;
    playerBlocksRemaining = playerBlocksRemaining + -1;
    selectionEntryCursor =
         (SelectionPlayerRuntimeBlock *)&currentSelectionEntry->packedSelectionState809C;
  } while (playerBlocksRemaining != 0);
  return;
}


/* Address: 0x0052FB70.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the 32 global selection-info entity slots, sums entity position fields +0x94/+0x98/+0x9C, and
   returns their signed averages in EAX/ECX/EDX. CF is set when no slot is populated and clear on success.
*/
WorldPositionEaxEcxEdxCf13 __cdecl SelectionInfoEntitySlots_ComputeAverageWorldPositionRegsCf(void)

{
  ModelRuntimeNode *pMVar1;
  int worldXAggregateQ12;
  int iVar2;
  int worldZAggregateQ12;
  GameEntityRuntime **selectionEntitySlotCursor;
  WorldPositionEaxEcxEdxCf13 WVar3;
  int selectedEntityCount;
  int selectionSlotsRemaining;
  ModelRuntimeNode *selectedModelNode;
  
  worldXAggregateQ12 = 0;
  iVar2 = 0;
  worldZAggregateQ12 = 0;
  selectionSlotsRemaining = 0x20;
  selectedEntityCount = 0;
  selectionEntitySlotCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntitySlotCursor != (GameEntityRuntime *)0x0) {
      pMVar1 = ((*selectionEntitySlotCursor)->common).ownership.modelNode;
      worldXAggregateQ12 = worldXAggregateQ12 + (pMVar1->worldTransform).translation.x;
      iVar2 = iVar2 + (pMVar1->worldTransform).translation.y;
      worldZAggregateQ12 = worldZAggregateQ12 + (pMVar1->worldTransform).translation.z;
      selectedEntityCount = selectedEntityCount + 1;
    }
    selectionEntitySlotCursor = selectionEntitySlotCursor + 1;
    selectionSlotsRemaining = selectionSlotsRemaining + -1;
  } while (selectionSlotsRemaining != 0);
  if (selectedEntityCount != 0) {
    worldXAggregateQ12 = worldXAggregateQ12 / selectedEntityCount;
    iVar2 = iVar2 / selectedEntityCount;
    worldZAggregateQ12 = worldZAggregateQ12 / selectedEntityCount;
  }
  WVar3.worldYQ12 = iVar2;
  WVar3.worldXQ12 = worldXAggregateQ12;
  WVar3.carry = selectedEntityCount == 0;
  WVar3.worldZQ12 = worldZAggregateQ12;
  return WVar3;
}


/* Address: 0x0052FD60.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans one exact 32-entry pointer array and clears only the first entry equal to target. EAX is
   preserved.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_RemoveFirstMatch(GameEntityRuntime *target,SelectionPointerArray32 *array)

{
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  undefined1 in_ZF;
  
  entriesRemaining = 0x20;
  do {
    selectionEntryCursor = array->entries;
    if (entriesRemaining == 0) break;
    entriesRemaining = entriesRemaining + -1;
    selectionEntryCursor = (GameEntityRuntime **)((int)array + 4);
    in_ZF = target == array->entries[0];
    array = (SelectionPointerArray32 *)selectionEntryCursor;
  } while (!(bool)in_ZF);
  if ((bool)in_ZF) {
    selectionEntryCursor[-1] = (GameEntityRuntime *)0x0;
  }
  return;
}


/* Address: 0x0052FDC0.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the global exact 32-entry selection array. CF set means at least one entry is non-null; CF clear
   means all entries are null. EAX is restored before return.
*/
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_HasAnyEntryCf(void)

{
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  bool bVar1;
  GameEntityRuntime *currentEntry;
  
  entriesRemaining = 0x20;
  bVar1 = true;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (entriesRemaining == 0) break;
    entriesRemaining = entriesRemaining + -1;
    currentEntry = *selectionEntryCursor;
    bVar1 = currentEntry == (GameEntityRuntime *)0x0;
    selectionEntryCursor = selectionEntryCursor + 1;
  } while (bVar1);
  return !bVar1;
}


/* Address: 0x0052FDE0.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the global 32-entry selection array. Null entries are accepted; every non-null entry must have
   owner/index dword +0x0C equal to ownerIndex. CF clear means all entries satisfy the condition, and CF set means
   the first mismatch. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or
   codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SelectionInfo_AllEntriesEmptyOrMatchOwnerCf(FactionRuntimeIndex ownerIndex)

{
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  while ((*selectionEntryCursor == (GameEntityRuntime *)0x0 ||
         (ownerIndex == ((*selectionEntryCursor)->common).ownership.ownerIndex))) {
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
    if (entriesRemaining == 0) {
      return false;
    }
  }
  return true;
}


/* Address: 0x0052FE30.
   Ownership: gameplay/selection/runtime.
   Purpose: Validates the global 32-entry selection array for one owner index. Every non-null entry must match
   ownerIndex, resolve to nested type 0x16, and at least one matching nested object must have a positive dword at
   +0x70. CF clear means the complete condition holds; CF set means mismatch or no active entry. EAX is preserved.
   It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed
   ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SelectionInfo_ValidateOwnerType16AndAnyActiveCf(FactionRuntimeIndex ownerIndex)

{
  int *piVar1;
  int entriesRemaining;
  int activeEntryCount;
  GameEntityRuntime **selectionEntryCursor;
  GameEntityRuntime *currentEntry;
  
  entriesRemaining = 0x20;
  activeEntryCount = 0;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    currentEntry = *selectionEntryCursor;
    if (currentEntry != (GameEntityRuntime *)0x0) {
      piVar1 = (currentEntry->common).ownership.definitionOrClassRecord;
      if (ownerIndex != (currentEntry->common).ownership.ownerIndex) {
        return true;
      }
      if (*(int *)(*piVar1 + 0x4c) != 0x16) {
        return true;
      }
      if (piVar1[0x1c] != 0) {
        activeEntryCount = activeEntryCount + 1;
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
    if (entriesRemaining == 0) {
      if (activeEntryCount == 0) {
        return true;
      }
      return false;
    }
  } while( true );
}


/* Address: 0x0052FEB0.
   Ownership: gameplay/selection/runtime.
   Purpose: Returns through CF whether the current selection contains an active definition or the exact single-
   class-13 fallback condition.
*/
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_TestAnyActiveOrSingleClass13Cf(void)

{
  int entriesRemaining;
  int selectedEntryCount;
  GameEntityRuntime **selectionEntryCursor;
  bool selectedEntryIsClass13;
  int selectedDefinitionRecordAddress;
  
  entriesRemaining = 0x20;
  selectedEntryCount = 0;
  selectedEntryIsClass13 = false;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      selectedEntryCount = selectedEntryCount + 1;
      selectedDefinitionRecordAddress =
           *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord;
      if (*(int *)(selectedDefinitionRecordAddress + 0x18) != 0) {
        return false;
      }
      selectedEntryIsClass13 = *(int *)(selectedDefinitionRecordAddress + 0x4c) == 0xd;
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  if ((selectedEntryCount == 1) && (selectedEntryIsClass13)) {
    return false;
  }
  return true;
}


/* Address: 0x0052FF30.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection info test position command at world point carry-flag result.
   Cross-module calls: GridScratch_TestProjectedCellMaskBandsCf [world/pathing/grid],
   ArmyRuntimeNode_DispatchTypedCallback [gameplay/army/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
SelectionInfo_TestPositionCommandAtWorldPointCf
          (Q12 worldXQ12,Q12 worldYQ12,WorldRuntimeContext *inGameRuntime)

{
  GraphicsFixedVec3 *pGVar1;
  GraphicsWorldCoordinateQ12 *pGVar2;
  GraphicsWorldCoordinateQ12 GVar3;
  GraphicsWorldCoordinateQ12 GVar4;
  ModelRuntimeNode *pMVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  byte highBandIndex;
  GameEntityRuntime **ppGVar9;
  bool bVar10;
  GameEntityRuntime *selectedEntity;
  
  iVar8 = 0x20;
  ppGVar9 = g_SelectionInfoEntitySlots->entries;
  while ((selectedEntity = *ppGVar9, selectedEntity == (GameEntityRuntime *)0x0 ||
         (pMVar5 = (selectedEntity->common).ownership.modelNode,
         *(int *)(*(int *)(selectedEntity->common).ownership.definitionOrClassRecord + 0x18) == 0)))
  {
    ppGVar9 = ppGVar9 + 1;
    iVar8 = iVar8 + -1;
    if (iVar8 == 0) {
      iVar8 = 0x20;
      ppGVar9 = g_SelectionInfoEntitySlots->entries;
      while ((*ppGVar9 == (GameEntityRuntime *)0x0 ||
             (iVar6 = *(int *)((*ppGVar9)->common).ownership.definitionOrClassRecord,
             *(int *)(iVar6 + 0x4c) != 0xd))) {
        ppGVar9 = ppGVar9 + 1;
        iVar8 = iVar8 + -1;
        if (iVar8 == 0) {
          return true;
        }
      }
      uVar7 = *(uint *)(iVar6 + 0xc4);
      highBandIndex = 3;
      if (((uVar7 & 0x80) == 0) && (highBandIndex = 1, (uVar7 & 4) == 0)) {
        highBandIndex = 6;
      }
      bVar10 = GridScratch_TestProjectedCellMaskBandsCf(worldXQ12,worldYQ12,7,highBandIndex);
      return bVar10;
    }
  }
  LOCK();
  pGVar1 = &(pMVar5->worldTransform).translation;
  GVar3 = pGVar1->x;
  pGVar1->x = worldYQ12;
  UNLOCK();
  LOCK();
  pGVar2 = &(pMVar5->worldTransform).translation.y;
  GVar4 = *pGVar2;
  *pGVar2 = worldXQ12;
  UNLOCK();
  bVar10 = ArmyRuntimeNode_DispatchTypedCallback((ArmyRuntimeSlot **)selectedEntity,inGameRuntime);
  (pMVar5->worldTransform).translation.x = GVar3;
  (pMVar5->worldTransform).translation.y = GVar4;
  return bVar10;
}


/* Address: 0x00530050.
   Ownership: gameplay/selection/runtime.
   Purpose: Returns through CF when no selected runtime reports a positive state value at offset 0x100.
   Cross-module calls: ArmyRuntime_TestStateField100NonnegativeCf [gameplay/army/runtime],
   ArmyRuntime_TestStateField100ZeroCf [gameplay/army/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_TestAllStateField100NonpositiveCf(void)

{
  GameEntityRuntime *armyRuntime;
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  bool bVar1;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    armyRuntime = *selectionEntryCursor;
    if (armyRuntime != (GameEntityRuntime *)0x0) {
      bVar1 = ArmyRuntime_TestStateField100NonnegativeCf((ArmyRuntimeSlot *)armyRuntime);
      if (bVar1) {
        bVar1 = ArmyRuntime_TestStateField100ZeroCf((ArmyRuntimeSlot *)armyRuntime);
        if (!bVar1) {
          return false;
        }
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return true;
}


/* Address: 0x005300A0.
   Ownership: gameplay/selection/runtime.
   Purpose: Returns through CF when any selected runtime reports a nonnegative state value at offset 0x100.
   Cross-module calls: ArmyRuntime_TestStateField100NonnegativeCf [gameplay/army/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_TestAnyStateField100NonnegativeCf(void)

{
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  bool bVar1;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      bVar1 = ArmyRuntime_TestStateField100NonnegativeCf((ArmyRuntimeSlot *)*selectionEntryCursor);
      if (bVar1) {
        return true;
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return false;
}


/* Address: 0x005300E0.
   Ownership: gameplay/selection/runtime.
   Purpose: Returns the first non-null pointer in the fixed 32-entry selection-info array, or null when every slot
   is empty.
*/
GameEntityRuntime * __cdecl SelectionInfo_GetFirstEntry(void)

{
  GameEntityRuntime *firstEntry;
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  GameEntityRuntime **nextSelectionEntryCursor;
  bool currentEntryIsEmpty;
  
  entriesRemaining = 0x20;
  firstEntry = (GameEntityRuntime *)0x0;
  currentEntryIsEmpty = true;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    nextSelectionEntryCursor = selectionEntryCursor;
    if (entriesRemaining == 0) break;
    entriesRemaining = entriesRemaining + -1;
    nextSelectionEntryCursor = selectionEntryCursor + 1;
    currentEntryIsEmpty = *selectionEntryCursor == (GameEntityRuntime *)0x0;
    selectionEntryCursor = nextSelectionEntryCursor;
  } while (currentEntryIsEmpty);
  if (!currentEntryIsEmpty) {
    firstEntry = nextSelectionEntryCursor[-1];
  }
  return firstEntry;
}

/* Address: 0x00530100.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the fixed 32-entry selection-info array for entry. CF is clear when found and set when absent;
   EAX is preserved.
*/
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_FindEntryCf(GameEntityRuntime *entry)

{
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  byte in_CF;
  undefined1 in_ZF;
  GameEntityRuntime *currentEntry;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (entriesRemaining == 0) break;
    entriesRemaining = entriesRemaining + -1;
    in_CF = 0;
    currentEntry = *selectionEntryCursor;
    in_ZF = entry == currentEntry;
    selectionEntryCursor = selectionEntryCursor + 1;
  } while (!(bool)in_ZF);
  return (in_CF & 2) != 0 || !(bool)in_ZF;
}


/* Address: 0x00530770.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection info collect attachment effect variant mask.
   Cross-module calls: ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs [gameplay/army/runtime].
*/
dword __thandor_eax_preserve_ecx_edx SelectionInfo_CollectAttachmentEffectVariantMask(void)

{
  dword effectVariantMask;
  int entriesRemaining;
  uint uVar1;
  GameEntityRuntime **selectionEntryCursor;
  
  entriesRemaining = 0x20;
  effectVariantMask = 0;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      uVar1 = ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs
                        (((*selectionEntryCursor)->common).ownership.definitionOrClassRecord);
      effectVariantMask = effectVariantMask | uVar1;
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return effectVariantMask;
}


/* Address: 0x005307C0.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the fixed 32-entry selection-info array. Entries whose nested type is 0x16 contribute flag 0x08;
   type 0x0D entries contribute their nested capability dword at +0xC4. Returns the OR-combined mask.
*/
dword __cdecl SelectionInfo_CollectCapabilityFlags(void)

{
  dword capabilityMask;
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  int currentEntityDefinition;
  
  entriesRemaining = 0x20;
  capabilityMask = 0;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      currentEntityDefinition =
           *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord;
      if (*(int *)(currentEntityDefinition + 0x4c) == 0x16) {
        capabilityMask = capabilityMask | 8;
      }
      else if (*(int *)(currentEntityDefinition + 0x4c) == 0xd) {
        capabilityMask = capabilityMask | *(uint *)(currentEntityDefinition + 0xc4);
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return capabilityMask;
}

/* Address: 0x00561000.
   Ownership: gameplay/selection/runtime.
   Purpose: Clears the selected player terrain-edit state field and clears the matching local root field when the
   selected player is the local player. Kept distinct from frontend slot indices, faction runtime indices, network
   endpoint identity, and PCK asset identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling
   convention, storage, body bytes, control flow, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerRuntime_ClearTerrainEditSelectionState
          (PlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          dword reservedZero2)

{
  InGameRuntimeRootImageC3E4 *inGameRuntimeRoot;
  
  inGameRuntimeRoot = g_InGameRuntimeRoot;
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->activePairCount8084 = 0;
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    inGameRuntimeRoot->localPlayerPairCount0BA4 = 0;
  }
  return;
}


/* Address: 0x00571020.
   Ownership: gameplay/selection/runtime.
   Purpose: CF=0 reports an existing pair and CF=1 reports absence. Archived binary body 00571020-0057108B; EAX,
   ECX, and EDX are preserved or incidental caller state rather than synthetic parameters or normal returns.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SelectionPlayerPairList_ContainsPairCf
          (SelectionPlayerPairValue pairValue,SelectionPlayerPairKey pairKey,
          PlayerRuntimeId playerRuntimeId)

{
  dword pairRecordsRemaining;
  SelectionPlayerPairRecord *pairRecordCursor;
  
  pairRecordsRemaining = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->activePairCount8084
  ;
  pairRecordCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pairRecords80_807F;
  while( true ) {
    if (pairRecordsRemaining == 0) {
      return true;
    }
    if ((pairKey == pairRecordCursor->pairKey) && (pairValue == pairRecordCursor->pairValue)) break;
    pairRecordCursor = pairRecordCursor + 1;
    pairRecordsRemaining = pairRecordsRemaining - 1;
  }
  return false;
}


/* Address: 0x005302B0.
   Ownership: gameplay/selection/runtime.
   Purpose: Typed parameters: p0 coordinateA→Q12, p1 coordinateB→Q12. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: SelectionPointerArray_IsSpatialSpreadTooLargeCf, SelectionPointerArray_Clear32.
   Cross-module calls: ArmyRuntime_QueueOrStartMoveCommandVariantA [gameplay/army/movement].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyPositionCommandVariantB
          (Q12 coordinateA,Q12 coordinateB,SelectionPointerArray32 *selection)

{
  ArmyMovementRuntime *movementRuntime;
  void *pvVar1;
  int iVar2;
  Q12 targetWorldY;
  int selectedEntryCount;
  Q12 targetWorldX;
  int *singleClass13Entry;
  GameEntityRuntime **ppGVar3;
  GameEntityRuntime **selectionEntryCursor;
  bool bVar4;
  int entityDefinitionAddress;
  
  iVar2 = 0x20;
  bVar4 = SelectionPointerArray_IsSpatialSpreadTooLargeCf(selection);
  targetWorldY = coordinateA;
  targetWorldX = coordinateB;
  ppGVar3 = selection->entries;
  do {
    movementRuntime = (ArmyMovementRuntime *)*ppGVar3;
    if (movementRuntime != (ArmyMovementRuntime *)0x0) {
      if (!bVar4) {
        targetWorldX = targetWorldX - movementRuntime->classState60;
        targetWorldY = targetWorldY - movementRuntime->ownerValue64;
      }
      ArmyRuntime_QueueOrStartMoveCommandVariantA(targetWorldY,targetWorldX,movementRuntime);
      if (!bVar4) {
        targetWorldX = targetWorldX + movementRuntime->classState60;
        targetWorldY = targetWorldY + movementRuntime->ownerValue64;
      }
    }
    ppGVar3 = ppGVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  iVar2 = 0x20;
  selectedEntryCount = 0;
  singleClass13Entry = (int *)0x0;
  selectionEntryCursor = selection->entries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      selectedEntryCount = selectedEntryCount + 1;
      entityDefinitionAddress =
           *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord;
      if (*(int *)(entityDefinitionAddress + 0x18) != 0) {
        return;
      }
      if (*(int *)(entityDefinitionAddress + 0x4c) == 0xd) {
        singleClass13Entry = (int *)*selectionEntryCursor;
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  if ((selectedEntryCount == 1) &&
     ((GameEntityRuntime *)singleClass13Entry != (GameEntityRuntime *)0x0)) {
    pvVar1 = (((GameEntityRuntime *)singleClass13Entry)->common).ownership.definitionOrClassRecord;
    *(Q12 *)((int)pvVar1 + 0x78) = coordinateB;
    *(Q12 *)((int)pvVar1 + 0x7c) = coordinateA;
    *(uint *)((int)pvVar1 + 0xec) = *(uint *)((int)pvVar1 + 0xec) | 0x800;
    SelectionPointerArray_Clear32(selection);
  }
  return;
}


/* Address: 0x00530420.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection runtime reset movement prune and recenter entries.
   Local calls: SelectionPointerArray_RecenterOffsetsAroundAveragePosition, SelectionPointerArray_Clear32.
   Cross-module calls: ArmyRuntime_ResetMovementStateFromModel [gameplay/army/movement],
   ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions], ModelNodeRuntime_TransformLocalPointRegs
   [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionRuntime_ResetMovementPruneAndRecenterEntries(GameEntityRuntime **selectionEntries)

{
  ModelRuntimeSlot *pMVar1;
  int iVar2;
  void *pvVar3;
  ModelRuntimeNode *modelNodeRuntime;
  int iVar4;
  int iVar5;
  GameEntityRuntime *pGVar6;
  GameEntityRuntime **ppGVar7;
  ModelLookupEntryEaxCf5 MVar8;
  ModelLocalPointRegs12 MVar9;
  
  iVar4 = 0x20;
  ppGVar7 = selectionEntries;
  do {
    pGVar6 = *ppGVar7;
    if ((pGVar6 != (GameEntityRuntime *)0x0) &&
       ((((ModelRuntimeSlotReferenceOrSavedOffset4 *)&pGVar6->common)[6].savedIdOrOffset & 2) == 0))
    {
      ArmyRuntime_ResetMovementStateFromModel((ArmyRuntimeSlot *)pGVar6);
      ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&pGVar6->common)[0xb].savedIdOrOffset =
           ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&pGVar6->common)[0xb].savedIdOrOffset &
           0xffffffef;
      ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&pGVar6->common)[6].savedIdOrOffset =
           ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&pGVar6->common)[6].savedIdOrOffset &
           0xfffffdff;
      pMVar1 = ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&pGVar6->common)->modelRuntime;
      if (*(int *)((pMVar1->definitionOrSavedId).savedIdOrOffset + 0x4c) == 0x16) {
        *ppGVar7 = (GameEntityRuntime *)0x0;
        (pMVar1->classState).classStateDC = 0;
      }
    }
    ppGVar7 = ppGVar7 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  iVar4 = 0x20;
  iVar5 = 0;
  pGVar6 = (GameEntityRuntime *)0x0;
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition
            ((SelectionPointerArray32 *)selectionEntries);
  ppGVar7 = selectionEntries;
  do {
    if (*ppGVar7 != (GameEntityRuntime *)0x0) {
      iVar5 = iVar5 + 1;
      iVar2 = *(int *)((*ppGVar7)->common).ownership.definitionOrClassRecord;
      if (*(int *)(iVar2 + 0x18) != 0) {
        return;
      }
      if (*(int *)(iVar2 + 0x4c) == 0xd) {
        pGVar6 = *ppGVar7;
      }
    }
    ppGVar7 = ppGVar7 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  if ((iVar5 == 1) && (pGVar6 != (GameEntityRuntime *)0x0)) {
    pvVar3 = (pGVar6->common).ownership.definitionOrClassRecord;
    modelNodeRuntime = (pGVar6->common).ownership.modelNode;
    *(uint *)((int)pvVar3 + 0xec) = *(uint *)((int)pvVar3 + 0xec) & 0xfffff7ff;
    MVar8 = ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource)
    ;
    if (!MVar8.carry) {
      MVar9 = ModelNodeRuntime_TransformLocalPointRegs(MVar8.entry,modelNodeRuntime);
      *(dword *)((int)pvVar3 + 0x78) = MVar9.eax;
      *(dword *)((int)pvVar3 + 0x7c) = MVar9.ecx;
      SelectionPointerArray_Clear32((SelectionPointerArray32 *)selectionEntries);
    }
  }
  return;
}


/* Address: 0x0052FCE0.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection pointer array add world entries matching runtime identity.
   Local calls: SelectionPointerArray_InsertUniqueAndRecenter.
*/
void __thandor_void_preserve_ecx_edx
SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity
          (ArmyRuntimeSlot *sourceArmyRuntime,SelectionPointerArray32 *selection)

{
  PckArmyAssetIdCatalog PVar1;
  int iVar2;
  GameEntityRuntime *entityRuntime;
  RuntimeToken runtimeIdentity;
  int ownerIndex;
  WorldRuntimeNode *worldNodeCursor;
  
  PVar1 = sourceArmyRuntime->armyAssetId;
  iVar2 = sourceArmyRuntime->factionIndex;
  for (worldNodeCursor = (WorldRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      worldNodeCursor != (WorldRuntimeNode *)0x0;
      worldNodeCursor = (worldNodeCursor->common).nextNode) {
    if (((worldNodeCursor[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
        (entityRuntime = *(GameEntityRuntime **)((int)worldNodeCursor->runtimePayload + 8),
        PVar1 == (entityRuntime->common).runtimeIdentityOrArmyAssetId)) &&
       (iVar2 == (entityRuntime->common).ownership.ownerIndex)) {
      SelectionPointerArray_InsertUniqueAndRecenter(entityRuntime,selection);
    }
  }
  return;
}


/* Address: 0x005303A0.
   Ownership: gameplay/selection/runtime.
   Purpose: Probes the fixed selection spread, then applies the existing position command to each of 32 possible
   entries. EAX and CF-derived behavior are preserved. Typed parameters: p0 coordinateA→Q12, p1 coordinateB→Q12.
   Calling convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: SelectionPointerArray_IsSpatialSpreadTooLargeCf.
   Cross-module calls: ArmyRuntime_QueueWaypointOrStartMoveVariantA [gameplay/army/movement].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyPositionCommand
          (Q12 coordinateA,Q12 coordinateB,SelectionPointerArray32 *selection)

{
  ArmyMovementRuntime *movementRuntime;
  int entriesRemaining;
  bool bVar1;
  
  entriesRemaining = 0x20;
  bVar1 = SelectionPointerArray_IsSpatialSpreadTooLargeCf(selection);
  do {
    movementRuntime = *(ArmyMovementRuntime **)selection;
    if (movementRuntime != (ArmyMovementRuntime *)0x0) {
      if (!bVar1) {
        coordinateB = coordinateB - movementRuntime->classState60;
        coordinateA = coordinateA - movementRuntime->ownerValue64;
      }
      ArmyRuntime_QueueWaypointOrStartMoveVariantA(coordinateA,coordinateB,movementRuntime);
      if (!bVar1) {
        coordinateB = coordinateB + movementRuntime->classState60;
        coordinateA = coordinateA + movementRuntime->ownerValue64;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return;
}


/* Address: 0x0052D150.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws the recovered horizontal number/text capped selection bar.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawHorizontalNumberTextCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate spanEndCoordinate,UiPixelCoordinate spanStartCoordinate,
          word *commandStream,SelectionPanelCellIndex cellIndex)

{
  int iVar1;
  dword arg0;
  void *pvVar2;
  dword dVar3;
  dword dVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  dword arg0_00;
  uint uVar8;
  uint *puVar9;
  RichTextExtentRegs RVar10;
  GraphicsTextureSizeEaxEdxCf9 GVar11;
  GraphicsTextureSizeEaxEdxCf9 GVar12;
  GraphicsTextureSizeEaxEdxCf9 GVar13;
  GraphicsTextureSizeEaxEdxCf9 GVar14;
  
  RVar10 = RichTextCommandStream_MeasureRegs(g_SelectionPanelNumberTextStyle,commandStream);
  pvVar2 = g_SelectionPanelData;
  dVar3 = RVar10.widthPixels;
  puVar9 = (uint *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  iVar7 = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  arg0 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
  GVar12 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 3,g_SelectionPanelTextureSource);
  arg0_00 = arg0 + 5;
  GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0_00,g_SelectionPanelTextureSource);
  GVar14 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_SelectionPanelTextureSource);
  dVar4 = GVar14.logicalWidthPixels;
  uVar8 = dVar3 + GVar11.logicalWidthPixels + GVar12.logicalWidthPixels + GVar13.logicalWidthPixels
          + spanStartCoordinate + dVar4;
  if ((uint)spanEndCoordinate < uVar8) {
    (*g_SelectionPanelBlitOpaque)
              (clipTop,clipLeft,clipBottom,clipRight,iVar7,spanStartCoordinate,arg0,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
    iVar6 = spanEndCoordinate - GVar11.logicalWidthPixels;
    (*g_SelectionPanelBlitOpaque)
              (clipTop,clipLeft,clipBottom,clipRight,iVar7,iVar6,arg0 + 2,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    (*g_SelectionPanelBlitClipped)
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar6,iVar7,
               spanStartCoordinate + dVar4,arg0 + 1,g_SelectionPanelTextureSource,
               g_FramebufferAccess);
  }
  else {
    (*g_SelectionPanelBlitOpaque)
              (clipTop,clipLeft,clipBottom,clipRight,iVar7,spanStartCoordinate,arg0,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    iVar6 = spanStartCoordinate + dVar4;
    GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
    iVar1 = spanEndCoordinate - GVar11.logicalWidthPixels;
    (*g_SelectionPanelBlitOpaque)
              (clipTop,clipLeft,clipBottom,clipRight,iVar7,iVar1,arg0 + 2,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    if ((*puVar9 & 0x100) == 0) {
      if ((*puVar9 & 0x200) == 0) {
        iVar5 = ((int)(iVar1 - uVar8) >> 1) + iVar6;
        (*g_SelectionPanelBlitClipped)
                  (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar5,iVar7,iVar6,arg0 + 1,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar7,iVar5,arg0 + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 3,g_SelectionPanelTextureSource);
        iVar5 = iVar5 + GVar11.logicalWidthPixels;
        iVar6 = dVar3 + iVar5;
        (*g_SelectionPanelBlitClipped)
                  (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar6,iVar7,iVar5,arg0 + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        iVar7 = iVar7 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0xc);
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,
                   commandStream,iVar7,iVar5);
        iVar7 = iVar7 - *(int *)((int)pvVar2 + cellIndex * 0x10 + 0xc);
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar7,iVar6,arg0 + 5,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 5,g_SelectionPanelTextureSource);
        (*g_SelectionPanelBlitClipped)
                  (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar1,iVar7,
                   iVar6 + GVar11.logicalWidthPixels,arg0 + 1,g_SelectionPanelTextureSource,
                   g_FramebufferAccess);
      }
      else {
        GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0_00,g_SelectionPanelTextureSource);
        iVar1 = iVar1 - GVar11.logicalWidthPixels;
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar7,iVar1,arg0_00,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        iVar5 = iVar1 - dVar3;
        (*g_SelectionPanelBlitClipped)
                  (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar1,iVar7,iVar5,arg0 + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        iVar7 = iVar7 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0xc);
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,
                   commandStream,iVar7,iVar5);
        iVar7 = iVar7 - *(int *)((int)pvVar2 + cellIndex * 0x10 + 0xc);
        GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 3,g_SelectionPanelTextureSource);
        iVar5 = iVar5 - GVar11.logicalWidthPixels;
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar7,iVar5,arg0 + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        (*g_SelectionPanelBlitClipped)
                  (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar5,iVar7,iVar6,arg0 + 1,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
      }
    }
    else {
      (*g_SelectionPanelBlitOpaque)
                (clipTop,clipLeft,clipBottom,clipRight,iVar7,iVar6,arg0 + 3,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 3,g_SelectionPanelTextureSource);
      iVar6 = iVar6 + GVar11.logicalWidthPixels;
      iVar5 = dVar3 + iVar6;
      (*g_SelectionPanelBlitClipped)
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar5,iVar7,iVar6,arg0 + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      iVar7 = iVar7 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0xc);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,commandStream
                 ,iVar7,iVar6);
      iVar7 = iVar7 - *(int *)((int)pvVar2 + cellIndex * 0x10 + 0xc);
      (*g_SelectionPanelBlitOpaque)
                (clipTop,clipLeft,clipBottom,clipRight,iVar7,iVar5,arg0_00,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0_00,g_SelectionPanelTextureSource);
      (*g_SelectionPanelBlitClipped)
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar1,iVar7,
                 iVar5 + GVar11.logicalWidthPixels,arg0 + 1,g_SelectionPanelTextureSource,
                 g_FramebufferAccess);
    }
  }
  return;
}

/* Address: 0x0052D600.
   Ownership: gameplay/selection/runtime.
   Purpose: Formats a signed number, measures and draws it centered over the selected panel-cell sprite, and
   returns the next layout coordinates while respecting horizontal and vertical suppression flags. Typed
   parameters: p4 drawX→UiPixelCoordinate_V297, p5 drawY→UiPixelCoordinate_V297, p7
   cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p6
   value→SelectionPanelNumericValue32_V343. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext],
   RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
SelectionPanelAdvanceEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
SelectionPanel_DrawNumberCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          SelectionPanelNumericValue32 value,SelectionPanelCellIndex cellIndex)

{
  uint uVar1;
  void *pvVar2;
  dword dVar3;
  int arg4;
  dword dVar4;
  int arg5;
  uint *puVar5;
  RichTextExtentRegs RVar6;
  SelectionPanelAdvanceEaxEdx8 SVar7;
  GraphicsTextureSizeEaxEdxCf9 GVar8;
  
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,0xf,1,value,
             (word *)&g_SelectionPanelNumberScratchUtf16);
  RVar6 = RichTextCommandStream_MeasureRegs
                    (g_SelectionPanelNumberTextStyle,(word *)&g_SelectionPanelNumberScratchUtf16);
  pvVar2 = g_SelectionPanelData;
  puVar5 = (uint *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  arg5 = drawY + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc);
  arg4 = drawX + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  dVar3 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,dVar3,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  GVar8 = (*g_GraphicsTextureSourceGetLogicalSize)(dVar3,g_SelectionPanelTextureSource);
  dVar4 = GVar8.logicalHeightPixels;
  dVar3 = GVar8.logicalWidthPixels;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,
             (word *)&g_SelectionPanelNumberScratchUtf16,
             ((int)(dVar4 - RVar6.heightPixels) >> 1) + arg4,
             ((int)(dVar3 - RVar6.widthPixels) >> 1) + arg5);
  uVar1 = *puVar5;
  if ((uVar1 & 4) != 0) {
    dVar3 = 0;
  }
  if ((uVar1 & 8) != 0) {
    dVar4 = 0;
  }
  SVar7.nextDrawY = dVar3 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0xc) + drawY;
  SVar7.nextDrawX = dVar4 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0x10) + drawX;
  return SVar7;
}


/* Address: 0x0052D6F0.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws the selected panel icon cell and returns the next layout coordinates while respecting the
   verified horizontal and vertical suppression flags. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1
   clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4
   drawX→UiPixelCoordinate_V297, p5 drawY→UiPixelCoordinate_V297, p6 cellIndex→SelectionPanelCellIndex_V342.
   Calling convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
SelectionPanelAdvanceEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
SelectionPanel_DrawIconCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          SelectionPanelCellIndex cellIndex)

{
  uint uVar1;
  void *pvVar2;
  dword dVar3;
  dword dVar4;
  uint *puVar5;
  SelectionPanelAdvanceEaxEdx8 SVar6;
  GraphicsTextureSizeEaxEdxCf9 GVar7;
  
  pvVar2 = g_SelectionPanelData;
  puVar5 = (uint *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  dVar3 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,
             drawX + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10),
             drawY + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc),dVar3,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  GVar7 = (*g_GraphicsTextureSourceGetLogicalSize)(dVar3,g_SelectionPanelTextureSource);
  dVar4 = GVar7.logicalHeightPixels;
  dVar3 = GVar7.logicalWidthPixels;
  uVar1 = *puVar5;
  if ((uVar1 & 4) != 0) {
    dVar3 = 0;
  }
  if ((uVar1 & 8) != 0) {
    dVar4 = 0;
  }
  SVar6.nextDrawY = dVar3 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0xc) + drawY;
  SVar6.nextDrawX = dVar4 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0x10) + drawX;
  return SVar6;
}


/* Address: 0x0052D770.
   Ownership: gameplay/selection/runtime.
   Purpose: Clamps the current and maximum values, chooses the stepped meter subresource frame, draws the base and
   meter frame, and returns the next layout coordinates. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1
   clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4
   drawX→UiPixelCoordinate_V297, p5 drawY→UiPixelCoordinate_V297, p6 maximumValue→UiNumericValue32_V308, p7
   currentValue→UiNumericValue32_V308, p8 cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
SelectionPanelAdvanceEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex)

{
  int iVar1;
  uint uVar2;
  void *pvVar3;
  int iVar4;
  int arg5;
  dword dVar5;
  int arg4;
  dword dVar6;
  uint *puVar7;
  SelectionPanelAdvanceEaxEdx8 SVar8;
  GraphicsTextureSizeEaxEdxCf9 GVar9;
  
  pvVar3 = g_SelectionPanelData;
  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  if (maximumValue == 0) {
    iVar4 = 0x11;
  }
  else {
    iVar4 = ((uint)(currentValue * 0x20 + maximumValue) / (uint)maximumValue >> 1) + 1;
  }
  puVar7 = (uint *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  iVar1 = *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  arg5 = drawY + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc);
  arg4 = drawX + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  dVar5 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,dVar5,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,iVar4 + iVar1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  GVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(dVar5,g_SelectionPanelTextureSource);
  dVar6 = GVar9.logicalHeightPixels;
  dVar5 = GVar9.logicalWidthPixels;
  uVar2 = *puVar7;
  if ((uVar2 & 4) != 0) {
    dVar5 = 0;
  }
  if ((uVar2 & 8) != 0) {
    dVar6 = 0;
  }
  SVar8.nextDrawY = dVar5 + *(int *)((int)pvVar3 + cellIndex * 0x10 + 0xc) + drawY;
  SVar8.nextDrawX = dVar6 + *(int *)((int)pvVar3 + cellIndex * 0x10 + 0x10) + drawX;
  return SVar8;
}


/* Address: 0x0052D850.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws left and right cap sprites, clamps current against maximum, rounds the proportional interior
   width and six-step fill frame, and draws the two interior bar spans. Typed parameters: p2
   clipTop→UiPixelCoordinate_V297, p3 clipLeft→UiPixelCoordinate_V297, p4 clipBottom→UiPixelCoordinate_V297, p5
   clipRight→UiPixelCoordinate_V297, p7 barEndCoordinate→UiPixelCoordinate_V297, p8
   barStartCoordinate→UiPixelCoordinate_V297, p9 maximumValue→UiNumericValue32_V308, p10
   currentValue→UiNumericValue32_V308, p11 cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawProportionalCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate barEndCoordinate,UiPixelCoordinate barStartCoordinate,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex)

{
  int arg7;
  int arg5;
  dword arg0;
  longlong lVar1;
  int iVar2;
  int iVar3;
  int arg4;
  GraphicsTextureSizeEaxEdxCf9 GVar4;
  
  arg4 = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  arg0 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  GVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_SelectionPanelTextureSource);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,barStartCoordinate,arg0,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  arg7 = barStartCoordinate + GVar4.logicalWidthPixels;
  GVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
  arg5 = barEndCoordinate - GVar4.logicalWidthPixels;
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,arg0 + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  iVar2 = arg5 - arg7;
  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  if ((maximumValue != 0) &&
     (lVar1 = (longlong)iVar2, iVar2 = (int)((lVar1 * currentValue) / (longlong)maximumValue),
     maximumValue < (int)((lVar1 * currentValue) % (longlong)maximumValue) * 2)) {
    iVar2 = iVar2 + 1;
  }
  if (maximumValue == 0) {
    iVar3 = 6;
  }
  else {
    iVar3 = (int)(((longlong)currentValue * 6) / (longlong)maximumValue);
    if (maximumValue < (int)(((longlong)currentValue * 6) % (longlong)maximumValue) * 2) {
      iVar3 = iVar3 + 1;
    }
  }
  (*g_SelectionPanelBlitClipped)
            (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar2 + arg7,arg4,arg7,
             iVar3 + 2 + arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  (*g_SelectionPanelBlitClipped)
            (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,arg5,arg4,iVar2 + arg7,arg0 + 1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  return;
}


/* Address: 0x0052D9A0.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws the recovered vertical proportional capped selection bar.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawVerticalProportionalCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex)

{
  int arg6;
  int arg4;
  dword arg0;
  longlong lVar1;
  int iVar2;
  int iVar3;
  int arg5;
  GraphicsTextureSizeEaxEdxCf9 GVar4;
  
  arg5 = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc);
  arg0 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  GVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_SelectionPanelTextureSource);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,arg5,arg0,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  arg6 = barStartCoordinate + GVar4.logicalHeightPixels;
  GVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
  arg4 = barEndCoordinate - GVar4.logicalHeightPixels;
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,arg0 + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  iVar2 = arg4 - arg6;
  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  if ((maximumValue != 0) &&
     (lVar1 = (longlong)iVar2, iVar2 = (int)((lVar1 * currentValue) / (longlong)maximumValue),
     maximumValue < (int)((lVar1 * currentValue) % (longlong)maximumValue) * 2)) {
    iVar2 = iVar2 + 1;
  }
  if (maximumValue == 0) {
    iVar3 = 6;
  }
  else {
    iVar3 = (int)(((longlong)currentValue * 6) / (longlong)maximumValue);
    if (maximumValue < (int)(((longlong)currentValue * 6) % (longlong)maximumValue) * 2) {
      iVar3 = iVar3 + 1;
    }
  }
  (*g_SelectionPanelBlitClipped)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,-0x80000000,arg4 - iVar2,arg5,
             iVar3 + 2 + arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  (*g_SelectionPanelBlitClipped)
            (clipTop,clipLeft,clipBottom,clipRight,arg4 - iVar2,-0x80000000,arg6,arg5,arg0 + 1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  return;
}

/* Address: 0x0052DAF0.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws the two cap sprites and fills the span between them with the verified middle bar subresource.
   Typed parameters: p2 clipTop→UiPixelCoordinate_V297, p3 clipLeft→UiPixelCoordinate_V297, p4
   clipBottom→UiPixelCoordinate_V297, p5 clipRight→UiPixelCoordinate_V297, p7
   barEndCoordinate→UiPixelCoordinate_V297, p8 barStartCoordinate→UiPixelCoordinate_V297, p9
   cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawForwardCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate barEndCoordinate,UiPixelCoordinate barStartCoordinate,
          SelectionPanelCellIndex cellIndex)

{
  int arg5;
  dword arg0;
  int arg4;
  GraphicsTextureSizeEaxEdxCf9 GVar1;
  GraphicsTextureSizeEaxEdxCf9 GVar2;
  
  arg4 = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  arg0 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  GVar1 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_SelectionPanelTextureSource);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,barStartCoordinate,arg0,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  GVar2 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
  arg5 = barEndCoordinate - GVar2.logicalWidthPixels;
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,arg0 + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  (*g_SelectionPanelBlitClipped)
            (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,arg5,arg4,
             barStartCoordinate + GVar1.logicalWidthPixels,arg0 + 1,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  return;
}


/* Address: 0x0052DBC0.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws the selected left and right cap sprites and fills the complete interior span with the verified
   middle bar subresource. Typed parameters: p2 clipTop→UiPixelCoordinate_V297, p3 clipLeft→UiPixelCoordinate_V297,
   p4 clipBottom→UiPixelCoordinate_V297, p5 clipRight→UiPixelCoordinate_V297, p6
   barEndCoordinate→UiPixelCoordinate_V297, p7 barStartCoordinate→UiPixelCoordinate_V297, p9
   cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawSolidCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          SelectionPanelCellIndex cellIndex)

{
  int arg4;
  dword arg0;
  int arg5;
  GraphicsTextureSizeEaxEdxCf9 GVar1;
  GraphicsTextureSizeEaxEdxCf9 GVar2;
  
  arg5 = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc);
  arg0 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  GVar1 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_SelectionPanelTextureSource);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,arg5,arg0,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  GVar2 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
  arg4 = barEndCoordinate - GVar2.logicalHeightPixels;
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,arg0 + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  (*g_SelectionPanelBlitClipped)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,-0x80000000,
             barStartCoordinate + GVar1.logicalHeightPixels,arg5,arg0 + 1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  return;
}


/* Address: 0x0052DC90.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws the recovered horizontal segmented capped selection bar.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawHorizontalSegmentedCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate spanEndCoordinate,UiPixelCoordinate spanStartCoordinate,
          SelectionPanelSegmentCount totalSegmentCount,SelectionPanelSegmentCount filledSegmentCount
          ,SelectionPanelCellIndex cellIndex)

{
  int arg7;
  dword arg0;
  int iVar1;
  int arg4;
  uint *puVar2;
  GraphicsTextureSizeEaxEdxCf9 GVar3;
  
  puVar2 = (uint *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  arg4 = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  arg0 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_SelectionPanelTextureSource);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,spanStartCoordinate,arg0,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  arg7 = spanStartCoordinate + GVar3.logicalWidthPixels;
  GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
  spanEndCoordinate = spanEndCoordinate - GVar3.logicalWidthPixels;
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,spanEndCoordinate,arg0 + 2,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 3,g_SelectionPanelTextureSource);
  iVar1 = filledSegmentCount;
  if ((*puVar2 & 0x400) != 0) {
    iVar1 = totalSegmentCount;
  }
  iVar1 = GVar3.logicalWidthPixels * iVar1 + arg7;
  if (spanEndCoordinate < iVar1) {
    (*g_SelectionPanelBlitClipped)
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,spanEndCoordinate,arg4,arg7,
               arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  else if ((*puVar2 & 0x100) == 0) {
    if ((*puVar2 & 0x200) == 0) {
      spanStartCoordinate = (spanEndCoordinate - iVar1 >> 1) + arg7;
      (*g_SelectionPanelBlitClipped)
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,spanStartCoordinate,arg4,arg7,
                 arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
      GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,arg4,spanStartCoordinate,arg0 + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        spanStartCoordinate = spanStartCoordinate + GVar3.logicalWidthPixels;
        totalSegmentCount = totalSegmentCount + -1;
      }
      if ((*puVar2 & 0x400) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
          (*g_SelectionPanelBlitOpaque)
                    (clipTop,clipLeft,clipBottom,clipRight,arg4,spanStartCoordinate,arg0 + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
          spanStartCoordinate = spanStartCoordinate + GVar3.logicalWidthPixels;
        }
      }
      (*g_SelectionPanelBlitClipped)
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,spanEndCoordinate,arg4,
                 spanStartCoordinate,arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
    }
    else {
      GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
        spanEndCoordinate = spanEndCoordinate - GVar3.logicalWidthPixels;
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,arg4,spanEndCoordinate,arg0 + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount = totalSegmentCount + -1;
      }
      if ((*puVar2 & 0x400) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
          spanEndCoordinate = spanEndCoordinate - GVar3.logicalWidthPixels;
          (*g_SelectionPanelBlitOpaque)
                    (clipTop,clipLeft,clipBottom,clipRight,arg4,spanEndCoordinate,arg0 + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      (*g_SelectionPanelBlitClipped)
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,spanEndCoordinate,arg4,arg7,
                 arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
    }
  }
  else {
    GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 4,g_SelectionPanelTextureSource);
    spanStartCoordinate = arg7;
    for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
      (*g_SelectionPanelBlitOpaque)
                (clipTop,clipLeft,clipBottom,clipRight,arg4,spanStartCoordinate,arg0 + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      spanStartCoordinate = spanStartCoordinate + GVar3.logicalWidthPixels;
      totalSegmentCount = totalSegmentCount + -1;
    }
    if ((*puVar2 & 0x400) != 0) {
      for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,arg4,spanStartCoordinate,arg0 + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        spanStartCoordinate = spanStartCoordinate + GVar3.logicalWidthPixels;
      }
    }
    (*g_SelectionPanelBlitClipped)
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,spanEndCoordinate,arg4,
               spanStartCoordinate,arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  return;
}

/* Address: 0x0052DFF0.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws a capped segmented bar, emits optional marker and segment sprites according to the panel flags
   and counts, and fills the remaining span. Typed parameters: p2 clipTop→UiPixelCoordinate_V297, p3
   clipLeft→UiPixelCoordinate_V297, p4 clipBottom→UiPixelCoordinate_V297, p5 clipRight→UiPixelCoordinate_V297, p6
   barEndCoordinate→UiPixelCoordinate_V297, p7 barStartCoordinate→UiPixelCoordinate_V297, p11
   cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p9
   totalSegmentCount→SelectionPanelSegmentCount_V343, p10 filledSegmentCount→SelectionPanelSegmentCount_V343.
   Calling convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawSegmentedCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          SelectionPanelSegmentCount totalSegmentCount,SelectionPanelSegmentCount filledSegmentCount
          ,SelectionPanelCellIndex cellIndex)

{
  int arg4;
  dword arg0;
  int arg5;
  int iVar1;
  uint *puVar2;
  GraphicsTextureSizeEaxEdxCf9 GVar3;
  
  puVar2 = (uint *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  arg5 = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc);
  arg0 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_SelectionPanelTextureSource);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,arg5,arg0,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  barStartCoordinate = barStartCoordinate + GVar3.logicalHeightPixels;
  GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
  arg4 = barEndCoordinate - GVar3.logicalHeightPixels;
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,arg0 + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 3,g_SelectionPanelTextureSource);
  iVar1 = filledSegmentCount;
  if ((*puVar2 & 0x400) != 0) {
    iVar1 = totalSegmentCount;
  }
  iVar1 = GVar3.logicalHeightPixels * iVar1 + barStartCoordinate;
  if (arg4 < iVar1) {
    (*g_SelectionPanelBlitClipped)
              (clipTop,clipLeft,clipBottom,clipRight,arg4,-0x80000000,barStartCoordinate,arg5,
               arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  else if ((*puVar2 & 0x100) == 0) {
    if ((*puVar2 & 0x200) == 0) {
      barEndCoordinate = arg4 - (arg4 - iVar1 >> 1);
      (*g_SelectionPanelBlitClipped)
                (clipTop,clipLeft,clipBottom,clipRight,arg4,-0x80000000,barEndCoordinate,arg5,
                 arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
      GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
        barEndCoordinate = barEndCoordinate - GVar3.logicalHeightPixels;
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,arg5,arg0 + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount = totalSegmentCount + -1;
      }
      if ((*puVar2 & 0x400) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
          barEndCoordinate = barEndCoordinate - GVar3.logicalHeightPixels;
          (*g_SelectionPanelBlitOpaque)
                    (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,arg5,arg0 + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      (*g_SelectionPanelBlitClipped)
                (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,-0x80000000,
                 barStartCoordinate,arg5,arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess)
      ;
    }
    else {
      GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 4,g_SelectionPanelTextureSource);
      barEndCoordinate = arg4;
      for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
        barEndCoordinate = barEndCoordinate - GVar3.logicalHeightPixels;
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,arg5,arg0 + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount = totalSegmentCount + -1;
      }
      if ((*puVar2 & 0x400) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
          barEndCoordinate = barEndCoordinate - GVar3.logicalHeightPixels;
          (*g_SelectionPanelBlitOpaque)
                    (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,arg5,arg0 + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      (*g_SelectionPanelBlitClipped)
                (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,-0x80000000,
                 barStartCoordinate,arg5,arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess)
      ;
    }
  }
  else {
    GVar3 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 4,g_SelectionPanelTextureSource);
    for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
      (*g_SelectionPanelBlitOpaque)
                (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,arg5,arg0 + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      barStartCoordinate = barStartCoordinate + GVar3.logicalHeightPixels;
      totalSegmentCount = totalSegmentCount + -1;
    }
    if ((*puVar2 & 0x400) != 0) {
      for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,arg5,arg0 + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        barStartCoordinate = barStartCoordinate + GVar3.logicalHeightPixels;
      }
    }
    (*g_SelectionPanelBlitClipped)
              (clipTop,clipLeft,clipBottom,clipRight,arg4,-0x80000000,barStartCoordinate,arg5,
               arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  return;
}


/* Address: 0x00530130.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection pointer array apply army runtime target.
   Cross-module calls: ArmyRuntime_TestStateField100ZeroCf [gameplay/army/runtime],
   ArmyRuntime_ResolveCommandTarget [gameplay/army/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyArmyRuntimeTarget
          (ArmyRuntimeSlot *targetArmyRuntime,SelectionPointerArray32 *selection)

{
  ArmyRuntimeSlot *runtimeState;
  int entriesRemaining;
  bool bVar1;
  
  entriesRemaining = 0x20;
  do {
    runtimeState = *(ArmyRuntimeSlot **)selection;
    if (runtimeState != (ArmyRuntimeSlot *)0x0) {
      bVar1 = ArmyRuntime_TestStateField100ZeroCf(runtimeState);
      if (!bVar1) {
        ArmyRuntime_ResolveCommandTarget(targetArmyRuntime,runtimeState);
        runtimeState->runtimeState98 = (dword)targetArmyRuntime;
        runtimeState->commandModeFlags = runtimeState->commandModeFlags | 0x14;
        runtimeState->movementStateFlags = runtimeState->movementStateFlags & 0xfffffdff;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return;
}


/* Address: 0x00530190.
   Ownership: gameplay/selection/runtime.
   Purpose: Applies the target-position command tuple to every eligible selected runtime. Typed parameters: p0
   coordinateA→Q12, p2 coordinateC→Q12. Calling convention, complete VariableStorage serialization, function bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ArmyRuntime_TestStateField100ZeroCf [gameplay/army/runtime],
   ArmyRuntime_ApplyTargetPositionCommand [gameplay/army/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyTargetPositionCommand
          (Q12 coordinateA,dword coordinateB,Q12 coordinateC,SelectionPointerArray32 *selection)

{
  ArmyRuntimeSlot *runtimeState;
  int iVar1;
  bool bVar2;
  
  iVar1 = 0x20;
  do {
    runtimeState = *(ArmyRuntimeSlot **)selection;
    if (runtimeState != (ArmyRuntimeSlot *)0x0) {
      bVar2 = ArmyRuntime_TestStateField100ZeroCf(runtimeState);
      if (!bVar2) {
        ArmyRuntime_ApplyTargetPositionCommand(coordinateA,coordinateB,coordinateC,runtimeState);
        runtimeState->commandModeFlags = runtimeState->commandModeFlags | 0x14;
        runtimeState->movementStateFlags = runtimeState->movementStateFlags & 0xfffffdff;
        runtimeState->commandGeneration = runtimeState->commandGeneration << 2;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}


/* Address: 0x00530540.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection runtime reset movement anchors and clear flag200 for eligible entries.
   Cross-module calls: GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel
   [gameplay/faction/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionRuntime_ResetMovementAnchorsAndClearFlag200ForEligibleEntries
          (GameEntityRuntime **selectionEntries)

{
  GameEntityCommandFlags *pGVar1;
  GameEntityRuntime *entityRuntime;
  int iVar2;
  
  iVar2 = 0x20;
  do {
    entityRuntime = *selectionEntries;
    if ((entityRuntime != (GameEntityRuntime *)0x0) &&
       (((entityRuntime->common).commandFlags & 2) == 0)) {
      GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(entityRuntime);
      pGVar1 = &(entityRuntime->common).commandFlags;
      *pGVar1 = *pGVar1 & 0xfffffdff;
    }
    selectionEntries = selectionEntries + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}


/* Address: 0x005305A0.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection runtime interrupt targets and clear flag10 for eligible entries.
   Cross-module calls: ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration [gameplay/army/movement].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionRuntime_InterruptTargetsAndClearFlag10ForEligibleEntries
          (GameEntityRuntime **selectionEntries)

{
  GameEntityRuntime *armyRuntime;
  int iVar1;
  
  iVar1 = 0x20;
  do {
    armyRuntime = *selectionEntries;
    if ((armyRuntime != (GameEntityRuntime *)0x0) &&
       ((((ModelRuntimeSlotReferenceOrSavedOffset4 *)&armyRuntime->common)[6].savedIdOrOffset & 2)
        == 0)) {
      ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration((ArmyRuntimeSlot *)armyRuntime);
      ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&armyRuntime->common)[0xb].savedIdOrOffset =
           ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&armyRuntime->common)[0xb].savedIdOrOffset &
           0xffffffef;
    }
    selectionEntries = selectionEntries + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}


/* Address: 0x00530600.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection runtime apply flags418 unless bit8 to eligible entries.
   Cross-module calls: ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx
SelectionRuntime_ApplyFlags418UnlessBit8ToEligibleEntries(GameEntityRuntime **selectionEntries)

{
  GameEntityRuntime *modelRuntime;
  int iVar1;
  WorldRuntimeContext *contextArg;
  
  iVar1 = 0x20;
  contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
  do {
    modelRuntime = *selectionEntries;
    if ((modelRuntime != (GameEntityRuntime *)0x0) &&
       (((modelRuntime->common).commandFlags & 2) == 0)) {
      ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(contextArg,(int *)modelRuntime);
    }
    selectionEntries = selectionEntries + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}


/* Address: 0x0052FCA0.
   Ownership: gameplay/selection/runtime.
   Purpose: Inserts a pointer into the first free slot of a 32-entry selection array when absent, then recomputes
   relative offsets around the average position.
   Local calls: SelectionPointerArray_RecenterOffsetsAroundAveragePosition.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_InsertUniqueAndRecenter
          (GameEntityRuntime *entityRuntime,SelectionPointerArray32 *selection)

{
  int iVar1;
  GameEntityRuntime **ppGVar2;
  GameEntityRuntime **ppGVar3;
  undefined1 in_ZF;
  bool bVar4;
  
  iVar1 = 0x20;
  ppGVar2 = selection->entries;
  do {
    if (iVar1 == 0) break;
    iVar1 = iVar1 + -1;
    in_ZF = entityRuntime == *ppGVar2;
    ppGVar2 = ppGVar2 + 1;
  } while (!(bool)in_ZF);
  if (!(bool)in_ZF) {
    bVar4 = true;
    iVar1 = 0x20;
    ppGVar2 = selection->entries;
    do {
      ppGVar3 = ppGVar2;
      if (iVar1 == 0) break;
      iVar1 = iVar1 + -1;
      ppGVar3 = ppGVar2 + 1;
      bVar4 = *ppGVar2 == (GameEntityRuntime *)0x0;
      ppGVar2 = ppGVar3;
    } while (!bVar4);
    if (bVar4) {
      ppGVar3[-1] = entityRuntime;
    }
  }
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition(selection);
  return;
}


/* Address: 0x0052FBF0.
   Ownership: gameplay/selection/runtime.
   Purpose: Averages the positions referenced by up to 32 selection pointers and stores each selection record
   offset relative to that average.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_RecenterOffsetsAroundAveragePosition(SelectionPointerArray32 *selection)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  GameEntityRuntime **ppGVar6;
  
  iVar2 = 0;
  iVar4 = 0;
  iVar3 = 0;
  iVar5 = 0x20;
  ppGVar6 = selection->entries;
  do {
    if (*ppGVar6 != (GameEntityRuntime *)0x0) {
      iVar1 = *(int *)((int)*ppGVar6 + 4);
      iVar3 = iVar3 + 1;
      iVar2 = iVar2 + *(int *)(iVar1 + 0x94);
      iVar4 = iVar4 + *(int *)(iVar1 + 0x98);
    }
    ppGVar6 = ppGVar6 + 1;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  if (iVar3 != 0) {
    iVar2 = iVar2 / iVar3;
    iVar4 = iVar4 / iVar3;
    iVar3 = 0x20;
    do {
      iVar5 = *(int *)selection;
      if (iVar5 != 0) {
        iVar1 = *(int *)(iVar5 + 4);
        iVar2 = iVar2 - *(int *)(iVar1 + 0x94);
        iVar4 = iVar4 - *(int *)(iVar1 + 0x98);
        *(int *)(iVar5 + 0x60) = iVar2;
        *(int *)(iVar5 + 100) = iVar4;
        iVar2 = iVar2 + *(int *)(iVar1 + 0x94);
        iVar4 = iVar4 + *(int *)(iVar1 + 0x98);
      }
      selection = (SelectionPointerArray32 *)((int)selection + 4);
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  return;
}


/* Address: 0x0052FD90.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans one exact 32-entry pointer array for target. CF clear means found, CF set means absent, and EAX
   is preserved.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SelectionPointerArray_ContainsCf(GameEntityRuntime *target,SelectionPointerArray32 *array)

{
  int entriesRemaining;
  undefined1 in_ZF;
  
  entriesRemaining = 0x20;
  do {
    if (entriesRemaining == 0) break;
    entriesRemaining = entriesRemaining + -1;
    in_ZF = target == array->entries[0];
    array = (SelectionPointerArray32 *)((int)array + 4);
  } while (!(bool)in_ZF);
  if (!(bool)in_ZF) {
    return true;
  }
  return false;
}


/* Address: 0x005301F0.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the fixed 32-entry selection pointer array and computes bounds from entity coordinates +0x60 and
   +0x64. CF is set when either span exceeds 0x5000 or their sum exceeds 0x7000; CF is clear for an empty or
   compact selection. EAX is preserved.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SelectionPointerArray_IsSpatialSpreadTooLargeCf(SelectionPointerArray32 *selection)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int local_20;
  
  local_20 = 0x20;
  while (iVar5 = *(int *)selection, iVar5 == 0) {
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    local_20 = local_20 + -1;
    if (local_20 == 0) {
      return false;
    }
  }
  iVar2 = *(int *)(iVar5 + 0x60);
  iVar4 = *(int *)(iVar5 + 100);
  iVar3 = iVar2;
  iVar5 = iVar4;
  do {
    iVar1 = *(int *)selection;
    if (iVar1 != 0) {
      if (*(int *)(iVar1 + 0x60) < iVar2) {
        iVar2 = *(int *)(iVar1 + 0x60);
      }
      if (*(int *)(iVar1 + 100) < iVar5) {
        iVar5 = *(int *)(iVar1 + 100);
      }
      if (iVar3 < *(int *)(iVar1 + 0x60)) {
        iVar3 = *(int *)(iVar1 + 0x60);
      }
      if (iVar4 < *(int *)(iVar1 + 100)) {
        iVar4 = *(int *)(iVar1 + 100);
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    local_20 = local_20 + -1;
  } while (local_20 != 0);
  if (((iVar3 - iVar2 < 0x5001) && (iVar4 - iVar5 < 0x5001)) &&
     ((iVar3 - iVar2) + (iVar4 - iVar5) < 0x7001)) {
    return false;
  }
  return true;
}


/* Address: 0x00530650.
   Ownership: gameplay/selection/runtime.
   Purpose: For each nonnull entry in the fixed 32-entry array whose nested type is 0x16, scans the exact 13-dword
   marker-source region, packs matches for three global marker IDs into three bytes, and conditionally writes
   marker bytes plus coordinate triples at +0xB8 through +0xD8 according to lane mask bits 1, 2, and 4. EAX is
   preserved. Typed parameters: p1 valueC→SelectionMarkerCoordinateValue32_V342, p2
   valueB→SelectionMarkerCoordinateValue32_V342, p3 valueA→SelectionMarkerCoordinateValue32_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged. Typed parameters: p0 laneMask→SelectionMarkerLaneMask_V343.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyType16MarkerCoordinates
          (SelectionMarkerLaneMask laneMask,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA,
          SelectionPointerArray32 *selection)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar3 = 0x20;
  do {
    if ((*(int **)selection != (int *)0x0) &&
       (piVar1 = (int *)**(int **)selection, *(int *)(*piVar1 + 0x4c) == 0x16)) {
      iVar5 = 0xc;
      iVar4 = 0;
      do {
        iVar2 = piVar1[iVar5 + 0x1e];
        if (iVar2 == g_ArmyLinkedChildAssetIdSlot0) {
          iVar4 = iVar4 + 1;
        }
        if (iVar2 == g_ArmyLinkedChildAssetIdSlot1) {
          iVar4 = iVar4 + 0x100;
        }
        if (iVar2 == g_ArmyLinkedChildAssetIdSlot2) {
          iVar4 = iVar4 + 0x10000;
        }
        iVar5 = iVar5 + -1;
      } while (-1 < iVar5);
      if ((laneMask & 1) != 0) {
        *(char *)(piVar1 + 0x37) = (char)iVar4;
        piVar1[0x2e] = valueA;
        piVar1[0x2f] = valueB;
        piVar1[0x30] = valueC;
      }
      if ((laneMask & 2) != 0) {
        *(char *)((int)piVar1 + 0xdd) = (char)((uint)iVar4 >> 8);
        piVar1[0x31] = valueA;
        piVar1[0x32] = valueB;
        piVar1[0x33] = valueC;
      }
      if ((laneMask & 4) != 0) {
        *(char *)((int)piVar1 + 0xde) = (char)((uint)iVar4 >> 0x10);
        piVar1[0x34] = valueA;
        piVar1[0x35] = valueB;
        piVar1[0x36] = valueC;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}


/* Address: 0x0052FB00.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection pointer array clear32.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_Clear32(SelectionPointerArray32 *array)

{
  int entriesRemaining;
  
  for (entriesRemaining = 0x20; entriesRemaining != 0; entriesRemaining = entriesRemaining + -1) {
    array->entries[0] = 0;
    array = (SelectionPointerArray32 *)((int)array + 4);
  }
  return;
}

