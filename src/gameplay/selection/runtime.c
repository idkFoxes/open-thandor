/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/selection/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/selection/runtime.h>
#include <thandor/thandor.h>

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
  uint32_t childDefinitionAddress;
  ModelRuntimeSlot *childModelRuntime;
  ArmyCommandGeneration maximumValue;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  uint32_t runtimeKindOrValue;
  SelectionPanelSegmentCount totalSegmentCount;
  SelectionPanelSegmentCount totalSegmentCount_00;
  SelectionPanelSegmentCount totalSegmentCount_01;
  int activeMetricMaximum;
  int workingValue;
  int capacityOrMetric;
  uint32_t halfFilledSegments;
  ModelRuntimeSlot *armyRuntime;
  bool framebufferBusy;
  ModelRuntimeActiveTotalMetricRegisterPair activeTotalMetrics;
  SelectionPanelAdvanceEaxEdx8 topLeftAdvance;
  SelectionPanelAdvanceEaxEdx8 topRightAdvance;
  SelectionPanelAdvanceEaxEdx8 bottomLeftAdvance;
  SelectionPanelAdvanceEaxEdx8 bottomRightAdvance;
  ModelRuntimeScaleRatioRegisterPairQ12 scaleRatio;
  RuntimeGroupIndexResult groupIndexResult;
  ArmySegmentMeter armyMetrics;
  
  inGameRoot = g_InGameRuntimeRoot;
  framebufferBusy = g_GraphicsFramebufferBeginAccess();
  if (framebufferBusy) {
    return;
  }
  runtimeKindOrValue = runtimeEntry->runtimeLinkOrKind08;
  armyRuntime = runtimeEntry->modelRuntime;
  if (runtimeEntry->factionIndex == (inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex) {
    if (runtimeKindOrValue != 0) {
      if (runtimeKindOrValue == 1) {
        if (((armyRuntime->classState).classStateB8 == 1) &&
           ((((ModelRuntimeSlot *)(armyRuntime->definitionOrSavedId).savedIdOrOffset)->
             definitionValue9C_4C == 0xd ||
            (((ModelRuntimeSlot *)(armyRuntime->definitionOrSavedId).savedIdOrOffset)->
             definitionValue9C_4C == 0xb)))) {
          activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,
                              (UiNumericValue32)(activeTotalMetrics >> 0x20),(UiNumericValue32)activeTotalMetrics,0x12);
          groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
          if (groupIndexResult.notFound) {
            topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                groupIndexResult.runtimeGroupIndex,0xb);
          }
          bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextDrawY,
                     topLeftAdvance.nextDrawY,(armyRuntime->classLinkState).classState68,
                     (armyRuntime->classLinkState).classState64,0x16);
          scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextDrawY,
                     bottomLeftAdvance.nextDrawY,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                     0x19);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextDrawX,topLeftAdvance.nextDrawX,
                     panelLeft,5);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextDrawX,topRightAdvance.nextDrawX,
                     panelRight,6);
          goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
        }
      }
      else if (runtimeKindOrValue < 3) {
        childModelRuntime = ((ModelRuntimeAttachmentDescriptor *)(armyRuntime->reserved120_13F + 0x20))->
                 childModelRuntimeOrSavedOffset00;
        if (((armyRuntime->attachmentCount0C != 0) && (childModelRuntime != (ModelRuntimeSlot *)0x0)) &&
           ((childDefinitionAddress = (childModelRuntime->definitionOrSavedId).savedIdOrOffset, *(int *)(childDefinitionAddress + 0x4c) == 5 ||
            (((*(int *)(childDefinitionAddress + 0x4c) == 6 || (*(int *)(childDefinitionAddress + 0x4c) == 7)) ||
             (*(int *)(childDefinitionAddress + 0x4c) == 8)))))) {
          workingValue = *(int *)(childModelRuntime->reserved10_37 + 0x14);
          capacityOrMetric = *(int *)((childModelRuntime->definitionOrSavedId).savedIdOrOffset + 0x30);
          activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          activeMetricMaximum = (int)(activeTotalMetrics >> 0x20);
          if (activeMetricMaximum == 0) {
            topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
          }
          else {
            topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,
                                activeMetricMaximum,(UiNumericValue32)activeTotalMetrics,0x12);
          }
          groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
          if (groupIndexResult.notFound) {
            topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                groupIndexResult.runtimeGroupIndex,0xb);
          }
          bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextDrawY,
                     topLeftAdvance.nextDrawY,capacityOrMetric,capacityOrMetric - workingValue,0x16);
          scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextDrawY,
                     bottomLeftAdvance.nextDrawY,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                     0x19);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextDrawX,topLeftAdvance.nextDrawX,
                     panelLeft,5);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextDrawX,topRightAdvance.nextDrawX,
                     panelRight,6);
          goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
        }
      }
      else if (runtimeKindOrValue == 3) {
        childModelRuntime = ((ModelRuntimeAttachmentDescriptor *)(armyRuntime->reserved120_13F + 0x20))->
                 childModelRuntimeOrSavedOffset00;
        if ((armyRuntime->attachmentCount0C != 0) && (childModelRuntime != (ModelRuntimeSlot *)0x0)) {
          armyRuntime = (ModelRuntimeSlot *)(childModelRuntime->definitionOrSavedId).savedIdOrOffset;
          runtimeKindOrValue = 0xffffffff;
          workingValue = 7;
          if (armyRuntime->definitionValue9C_4C == 9) {
            do {
              if (*(uint32_t *)((childModelRuntime->classState).reserved84_A7 + workingValue * 4 + -0x24) < runtimeKindOrValue) {
                runtimeKindOrValue = *(uint32_t *)((childModelRuntime->classState).reserved84_A7 + workingValue * 4 + -0x24);
              }
              workingValue = workingValue + -1;
            } while (-1 < workingValue);
            if ((int)runtimeKindOrValue < (int)(childModelRuntime->classLinkState).classState80) {
              runtimeKindOrValue = (childModelRuntime->classLinkState).classState80;
            }
            workingValue = 0;
            maximumValue = *(ArmyCommandGeneration *)
                            ((ArmyRuntimeMovementControlState *)armyRuntime->reserved10_37 + 4);
            if (0 < (int)runtimeKindOrValue) {
              workingValue = -runtimeKindOrValue;
            }
            activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
            capacityOrMetric = (int)(activeTotalMetrics >> 0x20);
            if (capacityOrMetric == 0) {
              topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
            }
            else {
              topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,capacityOrMetric,
                                  (UiNumericValue32)activeTotalMetrics,0x12);
            }
            groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
            if (groupIndexResult.notFound) {
              topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
            }
            else {
              topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                  groupIndexResult.runtimeGroupIndex,0xb);
            }
            bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
            bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
            SelectionPanel_DrawProportionalCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextDrawY,
                       topLeftAdvance.nextDrawY,maximumValue,workingValue + maximumValue,0x16);
            scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
            SelectionPanel_DrawProportionalCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextDrawY,
                       bottomLeftAdvance.nextDrawY,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                       0x19);
            SelectionPanel_DrawSolidCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextDrawX,topLeftAdvance.nextDrawX,
                       panelLeft,5);
            SelectionPanel_DrawSolidCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextDrawX,topRightAdvance.nextDrawX,
                       panelRight,6);
            goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
          }
        }
      }
      else if ((3 < runtimeKindOrValue) &&
              (((ModelRuntimeSlot *)(armyRuntime->definitionOrSavedId).savedIdOrOffset)->
               definitionValue9C_4C == 0x16)) {
        if (((ArmyRuntimeArticulatedContactState14 *)&(armyRuntime->classState).classStateAC)->
            terrainContactMode == ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE) {
          activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,
                              (UiNumericValue32)(activeTotalMetrics >> 0x20),(UiNumericValue32)activeTotalMetrics,0x12);
          groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
          if (groupIndexResult.notFound) {
            topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                groupIndexResult.runtimeGroupIndex,0xb);
          }
          bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextDrawY,
                     topLeftAdvance.nextDrawY,(armyRuntime->classLinkState).classState68,
                     (armyRuntime->classLinkState).classState64,0x16);
          scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextDrawY,
                     bottomLeftAdvance.nextDrawY,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                     0x19);
          armyMetrics = ArmyRuntime_QueryMetric6CAndDefinitionC4Regs((ArmyRuntimeSlot *)armyRuntime);
          halfFilledSegments = armyMetrics.filledSegments >> 1;
          runtimeKindOrValue = armyMetrics.totalSegments >> 1;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextDrawX,topLeftAdvance.nextDrawX,
                     panelLeft,armyMetrics.totalSegments - runtimeKindOrValue,armyMetrics.filledSegments - halfFilledSegments,0xf);
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextDrawX,topRightAdvance.nextDrawX,
                     panelRight,runtimeKindOrValue,halfFilledSegments,0x10);
        }
        else if (((armyRuntime->classState).classStateEC & 0xc0) == 0) {
          activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          workingValue = (int)(activeTotalMetrics >> 0x20);
          if (workingValue == 0) {
            topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
          }
          else {
            topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,workingValue,
                                (UiNumericValue32)activeTotalMetrics,0x12);
          }
          groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
          if (groupIndexResult.notFound) {
            topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                groupIndexResult.runtimeGroupIndex,0xb);
          }
          bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawForwardCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextDrawY,
                     topLeftAdvance.nextDrawY,4);
          scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextDrawY,
                     bottomLeftAdvance.nextDrawY,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                     0x19);
          armyMetrics = ArmyRuntime_QueryMetric6CAndDefinitionC4Regs((ArmyRuntimeSlot *)armyRuntime);
          halfFilledSegments = armyMetrics.filledSegments >> 1;
          runtimeKindOrValue = armyMetrics.totalSegments >> 1;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextDrawX,topLeftAdvance.nextDrawX,
                     panelLeft,armyMetrics.totalSegments - runtimeKindOrValue,armyMetrics.filledSegments - halfFilledSegments,0xf);
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextDrawX,topRightAdvance.nextDrawX,
                     panelRight,runtimeKindOrValue,halfFilledSegments,0x10);
        }
        else {
          activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          workingValue = (int)(activeTotalMetrics >> 0x20);
          if (workingValue == 0) {
            topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
          }
          else {
            topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,workingValue,
                                (UiNumericValue32)activeTotalMetrics,0x12);
          }
          groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
          if (groupIndexResult.notFound) {
            topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                groupIndexResult.runtimeGroupIndex,0xb);
          }
          bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextDrawY,
                     topLeftAdvance.nextDrawY,*(uint32_t *)(armyRuntime->reserved100_117 + 4),
                     *(UiNumericValue32 *)(armyRuntime->reserved100_117 + 8),0x16);
          scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextDrawY,
                     bottomLeftAdvance.nextDrawY,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                     0x19);
          armyMetrics = ArmyRuntime_QueryMetric6CAndDefinitionC4Regs((ArmyRuntimeSlot *)armyRuntime);
          halfFilledSegments = armyMetrics.filledSegments >> 1;
          runtimeKindOrValue = armyMetrics.totalSegments >> 1;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextDrawX,topLeftAdvance.nextDrawX,
                     panelLeft,armyMetrics.totalSegments - runtimeKindOrValue,armyMetrics.filledSegments - halfFilledSegments,0xf);
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextDrawX,topRightAdvance.nextDrawX,
                     panelRight,runtimeKindOrValue,halfFilledSegments,0x10);
        }
        goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
      }
    }
    if (((armyRuntime->classState).classStateEC & 0xc0) == 0) {
      activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
      workingValue = (int)(activeTotalMetrics >> 0x20);
      if (workingValue == 0) {
        topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
      }
      else {
        topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,workingValue,
                            (UiNumericValue32)activeTotalMetrics,0x12);
      }
      groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
      if (groupIndexResult.notFound) {
        topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
      }
      else {
        topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                            groupIndexResult.runtimeGroupIndex,0xb);
      }
      bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
      bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
      SelectionPanel_DrawForwardCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextDrawY,topLeftAdvance.nextDrawY,4)
      ;
      scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextDrawY,bottomLeftAdvance.nextDrawY
                 ,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,0x19);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextDrawX,topLeftAdvance.nextDrawX,panelLeft,5
                );
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextDrawX,topRightAdvance.nextDrawX,panelRight,
                 6);
    }
    else {
      activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
      workingValue = (int)(activeTotalMetrics >> 0x20);
      if (workingValue == 0) {
        topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
      }
      else {
        topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,workingValue,
                            (UiNumericValue32)activeTotalMetrics,0x12);
      }
      groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndexCf(runtimeEntry);
      if (groupIndexResult.notFound) {
        topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
      }
      else {
        topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                            groupIndexResult.runtimeGroupIndex,0xb);
      }
      bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
      bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextDrawY,topLeftAdvance.nextDrawY,
                 *(uint32_t *)(armyRuntime->reserved100_117 + 4),
                 *(UiNumericValue32 *)(armyRuntime->reserved100_117 + 8),0x16);
      scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextDrawY,bottomLeftAdvance.nextDrawY
                 ,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,0x19);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextDrawX,topLeftAdvance.nextDrawX,panelLeft,5
                );
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextDrawX,topRightAdvance.nextDrawX,panelRight,
                 6);
    }
  }
  else {
    topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
    topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
    bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
    bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
    SelectionPanel_DrawForwardCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextDrawY,topLeftAdvance.nextDrawY,4);
    scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
    SelectionPanel_DrawProportionalCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextDrawY,bottomLeftAdvance.nextDrawY,
               (UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,0x19);
    SelectionPanel_DrawSolidCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextDrawX,topLeftAdvance.nextDrawX,panelLeft,5);
    SelectionPanel_DrawSolidCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextDrawX,topRightAdvance.nextDrawX,panelRight,6)
    ;
  }
SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn:
  g_GraphicsFramebufferEndAccess();
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
          (PlayerRuntimeId playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3)

{
  WorldOwnerListNode100 *ownerNode;
  GameEntityRuntime *entityRuntime;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  
  inGameRoot = g_InGameRuntimeRoot;
  SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  for (ownerNode = (inGameRoot->worldRuntime0A30).ownerListHead; ownerNode != (WorldOwnerListNode100 *)0x0;
      ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      entityRuntime = *(GameEntityRuntime **)((int)ownerNode->runtimePayload + 8);
      if ((*(int *)(*(int *)ownerNode->runtimePayload + 0x4c) == 0x16) &&
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
          (PlayerRuntimeId playerId,uint32_t payloadDword04,uint32_t payloadDword08,
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
          (PlayerRuntimeId playerId,uint32_t payloadDword04,CommandPayloadDword08 payloadDword08,
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
          (PlayerRuntimeId playerId,uint32_t payloadDword04,CommandPayloadDword08 payloadDword08,
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
          (PlayerRuntimeId playerId,uint32_t payloadDword04,uint32_t payloadDword08,
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
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved,Q12 deltaYQ12,Q12 deltaXQ12)

{
  uint32_t primaryEntityOffset;
  ModelRuntimeNode *modelNode;
  int classRecordAddress;
  int placementContactKind;
  int worldYQ12;
  GameEntityRuntime *target;
  int worldXQ12;
  WorldRuntimeContext *worldRuntime;
  
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  primaryEntityOffset = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->primarySelectionEntityOffset8094;
  if (primaryEntityOffset != 0) {
    target = (GameEntityRuntime *)((int)g_ArmyRuntimeRebaseBaseMinusOne + primaryEntityOffset);
    SelectionPointerArray_ContainsCf
              (target,&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    modelNode = (target->common).ownership.modelNode;
    worldYQ12 = deltaXQ12 + (modelNode->worldTransform).translation.x;
    worldXQ12 = deltaYQ12 + (modelNode->worldTransform).translation.y;
    classRecordAddress = *(int *)(target->common).ownership.definitionOrClassRecord;
    (target->common).pathCoordinate0Q12 = worldYQ12;
    (target->common).pathCoordinate1Q12 = worldXQ12;
    (target->common).trackedCoordinate0Q12 = worldYQ12;
    (target->common).trackedCoordinate1Q12 = worldXQ12;
    (target->common).damageState.trackedCoordinate0Q12 = worldYQ12;
    (target->common).damageState.trackedCoordinate1Q12 = worldXQ12;
    (modelNode->worldTransform).translation.x = worldYQ12;
    placementContactKind = *(int *)(classRecordAddress + 0x278);
    (modelNode->worldTransform).translation.y = worldXQ12;
    if (placementContactKind == 3) {
      ((modelNode->runtimePayload).armyRuntime)->movementTarget0Q12 = 0x7fffffff;
    }
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (*(Q12 *)(classRecordAddress + 0x54),worldXQ12,worldYQ12,modelNode,worldRuntime);
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
    ModelNodeRuntime_UpdateDepthBinMasks(*(DepthIntervalRadius32 *)(classRecordAddress + 0xdc),modelNode);
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
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved0,uint32_t reserved1,AngleTurn32 angleDelta)

{
  uint32_t primaryEntityOffset;
  ModelRuntimeNode *modelNodeRuntime;
  GameEntityRuntime *target;
  
  primaryEntityOffset = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->primarySelectionEntityOffset8094;
  if (primaryEntityOffset != 0) {
    target = (GameEntityRuntime *)((int)g_ArmyRuntimeRebaseBaseMinusOne + primaryEntityOffset);
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
StatusResult __thandor_eax_cf_preserve_ecx_edx
SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots)

{
  uint8_t *patchBytes;
  AssetRelativeOffset tableOffset;
  GraphicsTextureSourceAsset *loadedResource;
  int entriesRemaining;
  int blockCountOrRecordOffset;
  SelectionPlayerRuntimeBlock *playerBlockCursor;
  TextureSourceLoadResult textureLoad;
  PackageLoadResult packageLoad;
  StatusResult successStatus;
  StatusResult failureStatus;
  AssetRelativeOffset swappedSelectionDataOffset;
  uint32_t referencePayloadValue;
  
  textureLoad = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)u_gfx_panel_select_gfx_0052ce18);
  loadedResource = textureLoad.textureSource;
  if (!textureLoad.failed) {
    g_SelectionPanelTextureSource = loadedResource;
    textureLoad = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)u_gfx_panel_info_gfx_0052ce42);
    loadedResource = textureLoad.textureSource;
    if (!textureLoad.failed) {
      g_InfoPanelTextureSource = loadedResource;
      packageLoad = Package_LoadEntry((uint16_t *)u_gfx_panel_select_dat_0052ce68);
      loadedResource = packageLoad.bufferOrError;
      if (!packageLoad.failed) {
        g_SelectionPanelData = loadedResource;
        packageLoad = Package_LoadEntry((uint16_t *)u_gfx_panel_info_dat_0052ce92);
        loadedResource = packageLoad.bufferOrError;
        if (!packageLoad.failed) {
          blockCountOrRecordOffset = 8;
          playerBlockCursor = g_SelectionPlayerBlocks;
          g_InfoPanelData = loadedResource;
          do {
            for (entriesRemaining = 0x20; loadedResource = g_SelectionPanelTextureSource, entriesRemaining != 0;
                entriesRemaining = entriesRemaining + -1) {
              (playerBlockCursor->selection).entries[0] = (GameEntityRuntime *)0x0;
              playerBlockCursor = (SelectionPlayerRuntimeBlock *)((playerBlockCursor->selection).entries + 1);
            }
            playerBlockCursor = (SelectionPlayerRuntimeBlock *)&playerBlockCursor->pendingSelectionEntityOffset8098;
            blockCountOrRecordOffset = blockCountOrRecordOffset + -1;
          } while (blockCountOrRecordOffset != 0);
          g_SelectionInfoEntitySlots = entitySlots;
          tableOffset = (g_SelectionPanelTextureSource->tableDescriptor).subresourceTableOffset;
          LOCK();
          swappedSelectionDataOffset =
               *(AssetRelativeOffset *)
                (g_SelectionPanelTextureSource[2].opaqueTablePayloadBC_1FF + tableOffset + 0x110);
          *(AssetRelativeOffset *)
           (g_SelectionPanelTextureSource[2].opaqueTablePayloadBC_1FF + tableOffset + 0x110) =
               *(AssetRelativeOffset *)
                (g_SelectionPanelTextureSource[2].opaqueTablePayloadBC_1FF + tableOffset + 0xf0);
          UNLOCK();
          *(AssetRelativeOffset *)(loadedResource[2].opaqueTablePayloadBC_1FF + tableOffset + 0xf0) =
               swappedSelectionDataOffset;
          loadedResource = g_InfoPanelTextureSource;
          tableOffset = (g_InfoPanelTextureSource->tableDescriptor).subresourceTableOffset;
          referencePayloadValue =
               *(uint32_t *)((g_InfoPanelTextureSource->common).buildMetadata.
                          assetRelativeAddressAnchor28 +
                         *(int *)(g_InfoPanelTextureSource[2].opaqueTablePayloadBC_1FF +
                                 tableOffset + 0xd0) + -0x28);
          blockCountOrRecordOffset = *(int *)(g_InfoPanelTextureSource[2].opaqueTablePayloadBC_1FF + tableOffset + 0x130);
          patchBytes = (g_InfoPanelTextureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                   blockCountOrRecordOffset + -0x28;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x24) =
               referencePayloadValue;
          blockCountOrRecordOffset = *(int *)(loadedResource[3].common.buildMetadata.assetRelativeAddressAnchor28 +
                          (tableOffset - 0x1c));
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x28;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x24) =
               referencePayloadValue;
          blockCountOrRecordOffset = *(int *)(loadedResource[3].common.buildMetadata.assetRelativeAddressAnchor28 + tableOffset + 4);
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x28;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x24) =
               referencePayloadValue;
          *(uint32_t *)((int)loadedResource[3].common.buildMetadata.names.producerName + tableOffset + 0x2c) = 4;
          *(uint32_t *)((int)loadedResource[3].common.buildMetadata.names.producerName + tableOffset + 0x14) = 4;
          *(uint32_t *)((int)loadedResource[3].common.buildMetadata.names.sourceName + tableOffset + 0xc) = 4;
          *(uint32_t *)((int)loadedResource[3].common.buildMetadata.names.producerName + tableOffset + 0x34) = 4;
          patchBytes = loadedResource[2].opaqueTablePayloadBC_1FF + tableOffset + 0x100;
          patchBytes[0] = 4;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          patchBytes = loadedResource[2].opaqueTablePayloadBC_1FF + tableOffset + 0xe8;
          patchBytes[0] = 4;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          patchBytes = loadedResource[2].opaqueTablePayloadBC_1FF + tableOffset + 0x120;
          patchBytes[0] = 4;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          patchBytes = loadedResource[2].opaqueTablePayloadBC_1FF + tableOffset + 0x108;
          patchBytes[0] = 4;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          blockCountOrRecordOffset = *(int *)((int)loadedResource[3].common.buildMetadata.names.producerName + tableOffset + 0x1c);
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x28;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x24) =
               referencePayloadValue;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x20;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x1c;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x18;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x14;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x10;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0xc) =
               referencePayloadValue;
          blockCountOrRecordOffset = *(int *)((int)loadedResource[3].common.buildMetadata.names.producerName + tableOffset + 0x3c);
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x28;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x24) =
               referencePayloadValue;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x20;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x1c;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x18;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x14;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x10;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0xc) =
               referencePayloadValue;
          blockCountOrRecordOffset = *(int *)(loadedResource[2].opaqueTablePayloadBC_1FF + tableOffset + 0xf0);
          *(uint32_t *)((loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x28) =
               referencePayloadValue;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x24;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x20;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x1c;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x18;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x14;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          *(uint32_t *)((loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x10) =
               referencePayloadValue;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0xc;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          blockCountOrRecordOffset = *(int *)(loadedResource[2].opaqueTablePayloadBC_1FF + tableOffset + 0x110);
          *(uint32_t *)((loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x28) =
               referencePayloadValue;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x24;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x20;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x1c;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x18;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x14;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          *(uint32_t *)((loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0x10) =
               referencePayloadValue;
          patchBytes = (loadedResource->common).buildMetadata.assetRelativeAddressAnchor28 + blockCountOrRecordOffset + -0xc;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          successStatus.valueOrError = 0xffff0000;
          successStatus.failed = false;
          return successStatus;
        }
      }
    }
  }
  failureStatus.failed = true;
  failureStatus.valueOrError = (uint32_t)loadedResource;
  return failureStatus;
}


/* Address: 0x0052D0F0.
   Ownership: gameplay/selection/runtime.
   Purpose: Releases both panel texture sources and both package-loaded .dat tables, then clears the four global
   resource pointers.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_preserve_eax SelectionInfoPanel_ShutdownResources(void)

{
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_SelectionPanelTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_InfoPanelTextureSource);
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
WorldPositionResult __cdecl SelectionInfoEntitySlots_ComputeAverageWorldPositionRegsCf(void)

{
  ModelRuntimeNode *slotModelNode;
  int worldXAggregateQ12;
  int worldYAggregateQ12;
  int worldZAggregateQ12;
  GameEntityRuntime **selectionEntitySlotCursor;
  WorldPositionResult averagePosition;
  int selectedEntityCount;
  int selectionSlotsRemaining;
  ModelRuntimeNode *selectedModelNode;
  
  worldXAggregateQ12 = 0;
  worldYAggregateQ12 = 0;
  worldZAggregateQ12 = 0;
  selectionSlotsRemaining = 0x20;
  selectedEntityCount = 0;
  selectionEntitySlotCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntitySlotCursor != (GameEntityRuntime *)0x0) {
      slotModelNode = ((*selectionEntitySlotCursor)->common).ownership.modelNode;
      worldXAggregateQ12 = worldXAggregateQ12 + (slotModelNode->worldTransform).translation.x;
      worldYAggregateQ12 = worldYAggregateQ12 + (slotModelNode->worldTransform).translation.y;
      worldZAggregateQ12 = worldZAggregateQ12 + (slotModelNode->worldTransform).translation.z;
      selectedEntityCount = selectedEntityCount + 1;
    }
    selectionEntitySlotCursor = selectionEntitySlotCursor + 1;
    selectionSlotsRemaining = selectionSlotsRemaining + -1;
  } while (selectionSlotsRemaining != 0);
  if (selectedEntityCount != 0) {
    worldXAggregateQ12 = worldXAggregateQ12 / selectedEntityCount;
    worldYAggregateQ12 = worldYAggregateQ12 / selectedEntityCount;
    worldZAggregateQ12 = worldZAggregateQ12 / selectedEntityCount;
  }
  averagePosition.worldYQ12 = worldYAggregateQ12;
  averagePosition.worldXQ12 = worldXAggregateQ12;
  averagePosition.unresolved = selectedEntityCount == 0;
  averagePosition.worldZQ12 = worldZAggregateQ12;
  return averagePosition;
}


/* Address: 0x0052FD60.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans one exact 32-entry pointer array and clears only the first entry equal to target. EAX is
   preserved.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_RemoveFirstMatch(GameEntityRuntime *target,SelectionPointerArray32 *array)

{
  int entryIndex;

  /* REPNE SCASD over the 32 entries; the first match is cleared. */
  for (entryIndex = 0; entryIndex < 0x20; entryIndex = entryIndex + 1) {
    if (array->entries[entryIndex] == target) {
      array->entries[entryIndex] = (GameEntityRuntime *)0x0;
      return;
    }
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
  bool entryIsEmpty;
  GameEntityRuntime *currentEntry;
  
  entriesRemaining = 0x20;
  entryIsEmpty = true;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (entriesRemaining == 0) break;
    entriesRemaining = entriesRemaining + -1;
    currentEntry = *selectionEntryCursor;
    entryIsEmpty = currentEntry == (GameEntityRuntime *)0x0;
    selectionEntryCursor = selectionEntryCursor + 1;
  } while (entryIsEmpty);
  return !entryIsEmpty;
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
  int *classRecord;
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
      classRecord = (currentEntry->common).ownership.definitionOrClassRecord;
      if (ownerIndex != (currentEntry->common).ownership.ownerIndex) {
        return true;
      }
      if (*(int *)(*classRecord + 0x4c) != 0x16) {
        return true;
      }
      if (classRecord[0x1c] != 0) {
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
  GraphicsFixedVec3 *translationPtr;
  GraphicsWorldCoordinateQ12 *translationYPtr;
  GraphicsWorldCoordinateQ12 savedTranslationX;
  GraphicsWorldCoordinateQ12 savedTranslationY;
  ModelRuntimeNode *selectedModelNode;
  int class13Definition;
  uint32_t capabilityFlags;
  int entriesRemaining;
  uint8_t highBandIndex;
  GameEntityRuntime **selectionEntryCursor;
  bool testResult;
  GameEntityRuntime *selectedEntity;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  while ((selectedEntity = *selectionEntryCursor, selectedEntity == (GameEntityRuntime *)0x0 ||
         (selectedModelNode = (selectedEntity->common).ownership.modelNode,
         *(int *)(*(int *)(selectedEntity->common).ownership.definitionOrClassRecord + 0x18) == 0)))
  {
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
    if (entriesRemaining == 0) {
      entriesRemaining = 0x20;
      selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
      while ((*selectionEntryCursor == (GameEntityRuntime *)0x0 ||
             (class13Definition = *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord,
             *(int *)(class13Definition + 0x4c) != 0xd))) {
        selectionEntryCursor = selectionEntryCursor + 1;
        entriesRemaining = entriesRemaining + -1;
        if (entriesRemaining == 0) {
          return true;
        }
      }
      capabilityFlags = *(uint32_t *)(class13Definition + 0xc4);
      highBandIndex = 3;
      if (((capabilityFlags & 0x80) == 0) && (highBandIndex = 1, (capabilityFlags & 4) == 0)) {
        highBandIndex = 6;
      }
      testResult = GridScratch_TestProjectedCellMaskBandsCf(worldXQ12,worldYQ12,7,highBandIndex);
      return testResult;
    }
  }
  LOCK();
  translationPtr = &(selectedModelNode->worldTransform).translation;
  savedTranslationX = translationPtr->x;
  translationPtr->x = worldYQ12;
  UNLOCK();
  LOCK();
  translationYPtr = &(selectedModelNode->worldTransform).translation.y;
  savedTranslationY = *translationYPtr;
  *translationYPtr = worldXQ12;
  UNLOCK();
  testResult = ArmyRuntimeNode_DispatchTypedCallback((ArmyRuntimeSlot **)selectedEntity,inGameRuntime);
  (selectedModelNode->worldTransform).translation.x = savedTranslationX;
  (selectedModelNode->worldTransform).translation.y = savedTranslationY;
  return testResult;
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
  bool stateTestResult;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    armyRuntime = *selectionEntryCursor;
    if (armyRuntime != (GameEntityRuntime *)0x0) {
      stateTestResult = ArmyRuntime_TestStateField100NonnegativeCf((ArmyRuntimeSlot *)armyRuntime);
      if (stateTestResult) {
        stateTestResult = ArmyRuntime_TestStateField100ZeroCf((ArmyRuntimeSlot *)armyRuntime);
        if (!stateTestResult) {
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
  bool stateTestResult;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      stateTestResult = ArmyRuntime_TestStateField100NonnegativeCf((ArmyRuntimeSlot *)*selectionEntryCursor);
      if (stateTestResult) {
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
  /* REPNE SCASD over the 32 selection slots; CF set when the entry is not among them. */
  int slotIndex;

  for (slotIndex = 0; slotIndex < 0x20; slotIndex = slotIndex + 1) {
    if (g_SelectionInfoEntitySlots->entries[slotIndex] == entry) {
      return false;
    }
  }
  return true;
}


/* Address: 0x00530770.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection info collect attachment effect variant mask.
   Cross-module calls: ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs [gameplay/army/runtime].
*/
uint32_t __thandor_eax_preserve_ecx_edx SelectionInfo_CollectAttachmentEffectVariantMask(void)

{
  uint32_t effectVariantMask;
  int entriesRemaining;
  uint32_t entryVariantMask;
  GameEntityRuntime **selectionEntryCursor;
  
  entriesRemaining = 0x20;
  effectVariantMask = 0;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      entryVariantMask = ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs
                        (((*selectionEntryCursor)->common).ownership.definitionOrClassRecord);
      effectVariantMask = effectVariantMask | entryVariantMask;
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
uint32_t __cdecl SelectionInfo_CollectCapabilityFlags(void)

{
  uint32_t capabilityMask;
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
        capabilityMask = capabilityMask | *(uint32_t *)(currentEntityDefinition + 0xc4);
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
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2)

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
  uint32_t pairRecordsRemaining;
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
  void *class13Record;
  int entriesRemaining;
  Q12 targetWorldY;
  int selectedEntryCount;
  Q12 targetWorldX;
  int *singleClass13Entry;
  GameEntityRuntime **commandEntryCursor;
  GameEntityRuntime **selectionEntryCursor;
  bool spreadTooLarge;
  int entityDefinitionAddress;
  
  entriesRemaining = 0x20;
  spreadTooLarge = SelectionPointerArray_IsSpatialSpreadTooLargeCf(selection);
  targetWorldY = coordinateA;
  targetWorldX = coordinateB;
  commandEntryCursor = selection->entries;
  do {
    movementRuntime = (ArmyMovementRuntime *)*commandEntryCursor;
    if (movementRuntime != (ArmyMovementRuntime *)0x0) {
      if (!spreadTooLarge) {
        targetWorldX = targetWorldX - movementRuntime->classState60;
        targetWorldY = targetWorldY - movementRuntime->ownerValue64;
      }
      ArmyRuntime_QueueOrStartMoveCommandVariantA(targetWorldY,targetWorldX,movementRuntime);
      if (!spreadTooLarge) {
        targetWorldX = targetWorldX + movementRuntime->classState60;
        targetWorldY = targetWorldY + movementRuntime->ownerValue64;
      }
    }
    commandEntryCursor = commandEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  entriesRemaining = 0x20;
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
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  if ((selectedEntryCount == 1) &&
     ((GameEntityRuntime *)singleClass13Entry != (GameEntityRuntime *)0x0)) {
    class13Record = (((GameEntityRuntime *)singleClass13Entry)->common).ownership.definitionOrClassRecord;
    *(Q12 *)((int)class13Record + 0x78) = coordinateB;
    *(Q12 *)((int)class13Record + 0x7c) = coordinateA;
    *(uint32_t *)((int)class13Record + 0xec) = *(uint32_t *)((int)class13Record + 0xec) | 0x800;
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
  ModelRuntimeSlot *entryModelRuntime;
  int definitionAddress;
  void *class13Record;
  ModelRuntimeNode *modelNodeRuntime;
  int entriesRemaining;
  int selectedEntryCount;
  GameEntityRuntime *currentEntity;
  GameEntityRuntime **selectionEntryCursor;
  ModelLookupEntryResult lookupEntry;
  ModelWorldPoint localPoint;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = selectionEntries;
  do {
    currentEntity = *selectionEntryCursor;
    if ((currentEntity != (GameEntityRuntime *)0x0) &&
       ((((ModelRuntimeSlotReferenceOrSavedOffset4 *)&currentEntity->common)[6].savedIdOrOffset & 2) == 0))
    {
      ArmyRuntime_ResetMovementStateFromModel((ArmyRuntimeSlot *)currentEntity);
      ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&currentEntity->common)[0xb].savedIdOrOffset =
           ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&currentEntity->common)[0xb].savedIdOrOffset &
           0xffffffef;
      ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&currentEntity->common)[6].savedIdOrOffset =
           ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&currentEntity->common)[6].savedIdOrOffset &
           0xfffffdff;
      entryModelRuntime = ((ModelRuntimeSlotReferenceOrSavedOffset4 *)&currentEntity->common)->modelRuntime;
      if (*(int *)((entryModelRuntime->definitionOrSavedId).savedIdOrOffset + 0x4c) == 0x16) {
        *selectionEntryCursor = (GameEntityRuntime *)0x0;
        (entryModelRuntime->classState).classStateDC = 0;
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  entriesRemaining = 0x20;
  selectedEntryCount = 0;
  currentEntity = (GameEntityRuntime *)0x0;
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition
            ((SelectionPointerArray32 *)selectionEntries);
  selectionEntryCursor = selectionEntries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      selectedEntryCount = selectedEntryCount + 1;
      definitionAddress = *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord;
      if (*(int *)(definitionAddress + 0x18) != 0) {
        return;
      }
      if (*(int *)(definitionAddress + 0x4c) == 0xd) {
        currentEntity = *selectionEntryCursor;
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  if ((selectedEntryCount == 1) && (currentEntity != (GameEntityRuntime *)0x0)) {
    class13Record = (currentEntity->common).ownership.definitionOrClassRecord;
    modelNodeRuntime = (currentEntity->common).ownership.modelNode;
    *(uint32_t *)((int)class13Record + 0xec) = *(uint32_t *)((int)class13Record + 0xec) & 0xfffff7ff;
    lookupEntry = ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource)
    ;
    if (!lookupEntry.notFound) {
      localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,modelNodeRuntime);
      *(uint32_t *)((int)class13Record + 0x78) = localPoint.xQ12;
      *(uint32_t *)((int)class13Record + 0x7c) = localPoint.yQ12;
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
  PckArmyAssetIdCatalog sourceArmyAssetId;
  int sourceFactionIndex;
  GameEntityRuntime *entityRuntime;
  RuntimeToken runtimeIdentity;
  int ownerIndex;
  WorldRuntimeNode *worldNodeCursor;
  
  sourceArmyAssetId = sourceArmyRuntime->armyAssetId;
  sourceFactionIndex = sourceArmyRuntime->factionIndex;
  for (worldNodeCursor = (WorldRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      worldNodeCursor != (WorldRuntimeNode *)0x0;
      worldNodeCursor = (worldNodeCursor->common).nextNode) {
    if (((worldNodeCursor[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
        (entityRuntime = *(GameEntityRuntime **)((int)worldNodeCursor->runtimePayload + 8),
        sourceArmyAssetId == (entityRuntime->common).runtimeIdentityOrArmyAssetId)) &&
       (sourceFactionIndex == (entityRuntime->common).ownership.ownerIndex)) {
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
  bool spreadTooLarge;
  
  entriesRemaining = 0x20;
  spreadTooLarge = SelectionPointerArray_IsSpatialSpreadTooLargeCf(selection);
  do {
    movementRuntime = *(ArmyMovementRuntime **)selection;
    if (movementRuntime != (ArmyMovementRuntime *)0x0) {
      if (!spreadTooLarge) {
        coordinateB = coordinateB - movementRuntime->classState60;
        coordinateA = coordinateA - movementRuntime->ownerValue64;
      }
      ArmyRuntime_QueueWaypointOrStartMoveVariantA(coordinateA,coordinateB,movementRuntime);
      if (!spreadTooLarge) {
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
          uint16_t *commandStream,SelectionPanelCellIndex cellIndex)

{
  int rightCapStart;
  uint32_t baseSubresource;
  void *panelData;
  uint32_t textWidth;
  uint32_t leftCapWidth;
  int segmentCursor;
  int innerCursor;
  int rowCoordinate;
  uint32_t textRightBorderSubresource;
  uint32_t requiredEnd;
  uint32_t *cellFlags;
  RichTextExtentRegs textExtent;
  TextureSizeResult pieceSize;
  TextureSizeResult textLeftBorderSize;
  TextureSizeResult textRightBorderSize;
  TextureSizeResult leftCapSize;
  
  textExtent = RichTextCommandStream_MeasureRegs(g_SelectionPanelNumberTextStyle,commandStream);
  panelData = g_SelectionPanelData;
  textWidth = textExtent.widthPixels;
  cellFlags = (uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  rowCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  textLeftBorderSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
  textRightBorderSubresource = baseSubresource + 5;
  textRightBorderSize = g_GraphicsTextureSourceGetLogicalSize(textRightBorderSubresource,g_SelectionPanelTextureSource);
  leftCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  leftCapWidth = leftCapSize.logicalWidthPixels;
  requiredEnd = textWidth + pieceSize.logicalWidthPixels + textLeftBorderSize.logicalWidthPixels + textRightBorderSize.logicalWidthPixels
          + spanStartCoordinate + leftCapWidth;
  if ((uint32_t)spanEndCoordinate < requiredEnd) {
    g_SelectionPanelBlitOpaque
              (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,spanStartCoordinate,baseSubresource,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
    innerCursor = spanEndCoordinate - pieceSize.logicalWidthPixels;
    g_SelectionPanelBlitOpaque
              (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,innerCursor,baseSubresource + 2,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,innerCursor,rowCoordinate,
               spanStartCoordinate + leftCapWidth,baseSubresource + 1,g_SelectionPanelTextureSource,
               g_FramebufferAccess);
  }
  else {
    g_SelectionPanelBlitOpaque
              (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,spanStartCoordinate,baseSubresource,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    innerCursor = spanStartCoordinate + leftCapWidth;
    pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
    rightCapStart = spanEndCoordinate - pieceSize.logicalWidthPixels;
    g_SelectionPanelBlitOpaque
              (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,rightCapStart,baseSubresource + 2,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    if ((*cellFlags & 0x100) == 0) {
      if ((*cellFlags & 0x200) == 0) {
        segmentCursor = ((int)(rightCapStart - requiredEnd) >> 1) + innerCursor;
        g_SelectionPanelBlitClipped
                  (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,segmentCursor,rowCoordinate,innerCursor,baseSubresource + 1,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,segmentCursor,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
        segmentCursor = segmentCursor + pieceSize.logicalWidthPixels;
        innerCursor = textWidth + segmentCursor;
        g_SelectionPanelBlitClipped
                  (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,innerCursor,rowCoordinate,segmentCursor,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        rowCoordinate = rowCoordinate + *(int *)((int)panelData + cellIndex * 0x10 + 0xc);
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,
                   commandStream,rowCoordinate,segmentCursor);
        rowCoordinate = rowCoordinate - *(int *)((int)panelData + cellIndex * 0x10 + 0xc);
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,innerCursor,baseSubresource + 5,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 5,g_SelectionPanelTextureSource);
        g_SelectionPanelBlitClipped
                  (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,rightCapStart,rowCoordinate,
                   innerCursor + pieceSize.logicalWidthPixels,baseSubresource + 1,g_SelectionPanelTextureSource,
                   g_FramebufferAccess);
      }
      else {
        pieceSize = g_GraphicsTextureSourceGetLogicalSize(textRightBorderSubresource,g_SelectionPanelTextureSource);
        rightCapStart = rightCapStart - pieceSize.logicalWidthPixels;
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,rightCapStart,textRightBorderSubresource,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        segmentCursor = rightCapStart - textWidth;
        g_SelectionPanelBlitClipped
                  (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,rightCapStart,rowCoordinate,segmentCursor,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        rowCoordinate = rowCoordinate + *(int *)((int)panelData + cellIndex * 0x10 + 0xc);
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,
                   commandStream,rowCoordinate,segmentCursor);
        rowCoordinate = rowCoordinate - *(int *)((int)panelData + cellIndex * 0x10 + 0xc);
        pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
        segmentCursor = segmentCursor - pieceSize.logicalWidthPixels;
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,segmentCursor,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        g_SelectionPanelBlitClipped
                  (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,segmentCursor,rowCoordinate,innerCursor,baseSubresource + 1,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
      }
    }
    else {
      g_SelectionPanelBlitOpaque
                (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,innerCursor,baseSubresource + 3,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
      innerCursor = innerCursor + pieceSize.logicalWidthPixels;
      segmentCursor = textWidth + innerCursor;
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,segmentCursor,rowCoordinate,innerCursor,baseSubresource + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      rowCoordinate = rowCoordinate + *(int *)((int)panelData + cellIndex * 0x10 + 0xc);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,commandStream
                 ,rowCoordinate,innerCursor);
      rowCoordinate = rowCoordinate - *(int *)((int)panelData + cellIndex * 0x10 + 0xc);
      g_SelectionPanelBlitOpaque
                (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,segmentCursor,textRightBorderSubresource,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      pieceSize = g_GraphicsTextureSourceGetLogicalSize(textRightBorderSubresource,g_SelectionPanelTextureSource);
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,rightCapStart,rowCoordinate,
                 segmentCursor + pieceSize.logicalWidthPixels,baseSubresource + 1,g_SelectionPanelTextureSource,
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
  uint32_t cellFlags;
  void *panelData;
  uint32_t subresourceOrWidth;
  int cellDrawX;
  uint32_t spriteHeight;
  int cellDrawY;
  uint32_t *cellFlagsPtr;
  RichTextExtentRegs textExtent;
  SelectionPanelAdvanceEaxEdx8 cellAdvance;
  TextureSizeResult spriteSize;
  
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,0xf,1,value,
             (uint16_t *)&g_SelectionPanelNumberScratchUtf16);
  textExtent = RichTextCommandStream_MeasureRegs
                    (g_SelectionPanelNumberTextStyle,(uint16_t *)&g_SelectionPanelNumberScratchUtf16);
  panelData = g_SelectionPanelData;
  cellFlagsPtr = (uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  cellDrawY = drawY + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc);
  cellDrawX = drawX + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  subresourceOrWidth = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,cellDrawX,cellDrawY,subresourceOrWidth,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(subresourceOrWidth,g_SelectionPanelTextureSource);
  spriteHeight = spriteSize.logicalHeightPixels;
  subresourceOrWidth = spriteSize.logicalWidthPixels;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,
             (uint16_t *)&g_SelectionPanelNumberScratchUtf16,
             ((int)(spriteHeight - textExtent.heightPixels) >> 1) + cellDrawX,
             ((int)(subresourceOrWidth - textExtent.widthPixels) >> 1) + cellDrawY);
  cellFlags = *cellFlagsPtr;
  if ((cellFlags & 4) != 0) {
    subresourceOrWidth = 0;
  }
  if ((cellFlags & 8) != 0) {
    spriteHeight = 0;
  }
  cellAdvance.nextDrawY = subresourceOrWidth + *(int *)((int)panelData + cellIndex * 0x10 + 0xc) + drawY;
  cellAdvance.nextDrawX = spriteHeight + *(int *)((int)panelData + cellIndex * 0x10 + 0x10) + drawX;
  return cellAdvance;
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
  uint32_t cellFlags;
  void *panelData;
  uint32_t subresourceOrWidth;
  uint32_t spriteHeight;
  uint32_t *cellFlagsPtr;
  SelectionPanelAdvanceEaxEdx8 cellAdvance;
  TextureSizeResult spriteSize;
  
  panelData = g_SelectionPanelData;
  cellFlagsPtr = (uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  subresourceOrWidth = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,
             drawX + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10),
             drawY + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc),subresourceOrWidth,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(subresourceOrWidth,g_SelectionPanelTextureSource);
  spriteHeight = spriteSize.logicalHeightPixels;
  subresourceOrWidth = spriteSize.logicalWidthPixels;
  cellFlags = *cellFlagsPtr;
  if ((cellFlags & 4) != 0) {
    subresourceOrWidth = 0;
  }
  if ((cellFlags & 8) != 0) {
    spriteHeight = 0;
  }
  cellAdvance.nextDrawY = subresourceOrWidth + *(int *)((int)panelData + cellIndex * 0x10 + 0xc) + drawY;
  cellAdvance.nextDrawX = spriteHeight + *(int *)((int)panelData + cellIndex * 0x10 + 0x10) + drawX;
  return cellAdvance;
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
  int baseSubresource;
  uint32_t cellFlags;
  void *panelData;
  int meterFrame;
  int cellDrawY;
  uint32_t subresourceOrWidth;
  int cellDrawX;
  uint32_t spriteHeight;
  uint32_t *cellFlagsPtr;
  SelectionPanelAdvanceEaxEdx8 cellAdvance;
  TextureSizeResult spriteSize;
  
  panelData = g_SelectionPanelData;
  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  if (maximumValue == 0) {
    meterFrame = 0x11;
  }
  else {
    meterFrame = ((uint32_t)(currentValue * 0x20 + maximumValue) / (uint32_t)maximumValue >> 1) + 1;
  }
  cellFlagsPtr = (uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  baseSubresource = *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  cellDrawY = drawY + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc);
  cellDrawX = drawX + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  subresourceOrWidth = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,cellDrawX,cellDrawY,subresourceOrWidth,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,cellDrawX,cellDrawY,meterFrame + baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(subresourceOrWidth,g_SelectionPanelTextureSource);
  spriteHeight = spriteSize.logicalHeightPixels;
  subresourceOrWidth = spriteSize.logicalWidthPixels;
  cellFlags = *cellFlagsPtr;
  if ((cellFlags & 4) != 0) {
    subresourceOrWidth = 0;
  }
  if ((cellFlags & 8) != 0) {
    spriteHeight = 0;
  }
  cellAdvance.nextDrawY = subresourceOrWidth + *(int *)((int)panelData + cellIndex * 0x10 + 0xc) + drawY;
  cellAdvance.nextDrawX = spriteHeight + *(int *)((int)panelData + cellIndex * 0x10 + 0x10) + drawX;
  return cellAdvance;
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
  int interiorStart;
  int endCapCoordinate;
  uint32_t baseSubresource;
  int64_t interiorSpan64;
  int filledSpan;
  int fillFrame;
  int fixedDrawCoordinate;
  TextureSizeResult capSize;
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  capSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,barStartCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  interiorStart = barStartCoordinate + capSize.logicalWidthPixels;
  capSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - capSize.logicalWidthPixels;
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,endCapCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  filledSpan = endCapCoordinate - interiorStart;
  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  if ((maximumValue != 0) &&
     (interiorSpan64 = (int64_t)filledSpan, filledSpan = (int)((interiorSpan64 * currentValue) / (int64_t)maximumValue),
     maximumValue < (int)((interiorSpan64 * currentValue) % (int64_t)maximumValue) * 2)) {
    filledSpan = filledSpan + 1;
  }
  if (maximumValue == 0) {
    fillFrame = 6;
  }
  else {
    fillFrame = (int)(((int64_t)currentValue * 6) / (int64_t)maximumValue);
    if (maximumValue < (int)(((int64_t)currentValue * 6) % (int64_t)maximumValue) * 2) {
      fillFrame = fillFrame + 1;
    }
  }
  g_SelectionPanelBlitClipped
            (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,filledSpan + interiorStart,fixedDrawCoordinate,interiorStart,
             fillFrame + 2 + baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,endCapCoordinate,fixedDrawCoordinate,filledSpan + interiorStart,baseSubresource + 1,
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
  int interiorStart;
  int endCapCoordinate;
  uint32_t baseSubresource;
  int64_t interiorSpan64;
  int filledSpan;
  int fillFrame;
  int fixedDrawCoordinate;
  TextureSizeResult capSize;
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  capSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,fixedDrawCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  interiorStart = barStartCoordinate + capSize.logicalHeightPixels;
  capSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - capSize.logicalHeightPixels;
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,fixedDrawCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  filledSpan = endCapCoordinate - interiorStart;
  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  if ((maximumValue != 0) &&
     (interiorSpan64 = (int64_t)filledSpan, filledSpan = (int)((interiorSpan64 * currentValue) / (int64_t)maximumValue),
     maximumValue < (int)((interiorSpan64 * currentValue) % (int64_t)maximumValue) * 2)) {
    filledSpan = filledSpan + 1;
  }
  if (maximumValue == 0) {
    fillFrame = 6;
  }
  else {
    fillFrame = (int)(((int64_t)currentValue * 6) / (int64_t)maximumValue);
    if (maximumValue < (int)(((int64_t)currentValue * 6) % (int64_t)maximumValue) * 2) {
      fillFrame = fillFrame + 1;
    }
  }
  g_SelectionPanelBlitClipped
            (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,-0x80000000,endCapCoordinate - filledSpan,fixedDrawCoordinate,
             fillFrame + 2 + baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate - filledSpan,-0x80000000,interiorStart,fixedDrawCoordinate,baseSubresource + 1,
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
  int endCapCoordinate;
  uint32_t baseSubresource;
  int fixedDrawCoordinate;
  TextureSizeResult startCapSize;
  TextureSizeResult endCapSize;
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  startCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,barStartCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  endCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - endCapSize.logicalWidthPixels;
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,endCapCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,endCapCoordinate,fixedDrawCoordinate,
             barStartCoordinate + startCapSize.logicalWidthPixels,baseSubresource + 1,g_SelectionPanelTextureSource,
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
  int endCapCoordinate;
  uint32_t baseSubresource;
  int fixedDrawCoordinate;
  TextureSizeResult startCapSize;
  TextureSizeResult endCapSize;
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  startCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,fixedDrawCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  endCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - endCapSize.logicalHeightPixels;
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,fixedDrawCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,-0x80000000,
             barStartCoordinate + startCapSize.logicalHeightPixels,fixedDrawCoordinate,baseSubresource + 1,
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
  int interiorStart;
  uint32_t baseSubresource;
  int segmentsEnd;
  int fixedDrawCoordinate;
  uint32_t *cellFlags;
  TextureSizeResult spriteSize;
  
  cellFlags = (uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanStartCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  interiorStart = spanStartCoordinate + spriteSize.logicalWidthPixels;
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  spanEndCoordinate = spanEndCoordinate - spriteSize.logicalWidthPixels;
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanEndCoordinate,baseSubresource + 2,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
  segmentsEnd = filledSegmentCount;
  if ((*cellFlags & 0x400) != 0) {
    segmentsEnd = totalSegmentCount;
  }
  segmentsEnd = spriteSize.logicalWidthPixels * segmentsEnd + interiorStart;
  if (spanEndCoordinate < segmentsEnd) {
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,spanEndCoordinate,fixedDrawCoordinate,interiorStart,
               baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  else if ((*cellFlags & 0x100) == 0) {
    if ((*cellFlags & 0x200) == 0) {
      spanStartCoordinate = (spanEndCoordinate - segmentsEnd >> 1) + interiorStart;
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,spanStartCoordinate,fixedDrawCoordinate,interiorStart,
                 baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
        totalSegmentCount = totalSegmentCount + -1;
      }
      if ((*cellFlags & 0x400) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
          g_SelectionPanelBlitOpaque
                    (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
          spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
        }
      }
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,spanEndCoordinate,fixedDrawCoordinate,
                 spanStartCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
    }
    else {
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
        spanEndCoordinate = spanEndCoordinate - spriteSize.logicalWidthPixels;
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanEndCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount = totalSegmentCount + -1;
      }
      if ((*cellFlags & 0x400) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
          spanEndCoordinate = spanEndCoordinate - spriteSize.logicalWidthPixels;
          g_SelectionPanelBlitOpaque
                    (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanEndCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,spanEndCoordinate,fixedDrawCoordinate,interiorStart,
                 baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
    }
  }
  else {
    spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
    spanStartCoordinate = interiorStart;
    for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
      g_SelectionPanelBlitOpaque
                (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
      totalSegmentCount = totalSegmentCount + -1;
    }
    if ((*cellFlags & 0x400) != 0) {
      for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
      }
    }
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,spanEndCoordinate,fixedDrawCoordinate,
               spanStartCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
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
  int endCapCoordinate;
  uint32_t baseSubresource;
  int fixedDrawCoordinate;
  int segmentsEnd;
  uint32_t *cellFlags;
  TextureSizeResult spriteSize;
  
  cellFlags = (uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,fixedDrawCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  barStartCoordinate = barStartCoordinate + spriteSize.logicalHeightPixels;
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,fixedDrawCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
  segmentsEnd = filledSegmentCount;
  if ((*cellFlags & 0x400) != 0) {
    segmentsEnd = totalSegmentCount;
  }
  segmentsEnd = spriteSize.logicalHeightPixels * segmentsEnd + barStartCoordinate;
  if (endCapCoordinate < segmentsEnd) {
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,-0x80000000,barStartCoordinate,fixedDrawCoordinate,
               baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  else if ((*cellFlags & 0x100) == 0) {
    if ((*cellFlags & 0x200) == 0) {
      barEndCoordinate = endCapCoordinate - (endCapCoordinate - segmentsEnd >> 1);
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,-0x80000000,barEndCoordinate,fixedDrawCoordinate,
                 baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
        barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount = totalSegmentCount + -1;
      }
      if ((*cellFlags & 0x400) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
          barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
          g_SelectionPanelBlitOpaque
                    (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,-0x80000000,
                 barStartCoordinate,fixedDrawCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess)
      ;
    }
    else {
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      barEndCoordinate = endCapCoordinate;
      for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
        barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount = totalSegmentCount + -1;
      }
      if ((*cellFlags & 0x400) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
          barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
          g_SelectionPanelBlitOpaque
                    (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,-0x80000000,
                 barStartCoordinate,fixedDrawCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess)
      ;
    }
  }
  else {
    spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
    for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
      g_SelectionPanelBlitOpaque
                (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      barStartCoordinate = barStartCoordinate + spriteSize.logicalHeightPixels;
      totalSegmentCount = totalSegmentCount + -1;
    }
    if ((*cellFlags & 0x400) != 0) {
      for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        barStartCoordinate = barStartCoordinate + spriteSize.logicalHeightPixels;
      }
    }
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,-0x80000000,barStartCoordinate,fixedDrawCoordinate,
               baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
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
  bool stateIsZero;
  
  entriesRemaining = 0x20;
  do {
    runtimeState = *(ArmyRuntimeSlot **)selection;
    if (runtimeState != (ArmyRuntimeSlot *)0x0) {
      stateIsZero = ArmyRuntime_TestStateField100ZeroCf(runtimeState);
      if (!stateIsZero) {
        ArmyRuntime_ResolveCommandTarget(targetArmyRuntime,runtimeState);
        runtimeState->runtimeState98 = (uint32_t)targetArmyRuntime;
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
          (Q12 coordinateA,uint32_t coordinateB,Q12 coordinateC,SelectionPointerArray32 *selection)

{
  ArmyRuntimeSlot *runtimeState;
  int entriesRemaining;
  bool stateIsZero;
  
  entriesRemaining = 0x20;
  do {
    runtimeState = *(ArmyRuntimeSlot **)selection;
    if (runtimeState != (ArmyRuntimeSlot *)0x0) {
      stateIsZero = ArmyRuntime_TestStateField100ZeroCf(runtimeState);
      if (!stateIsZero) {
        ArmyRuntime_ApplyTargetPositionCommand(coordinateA,coordinateB,coordinateC,runtimeState);
        runtimeState->commandModeFlags = runtimeState->commandModeFlags | 0x14;
        runtimeState->movementStateFlags = runtimeState->movementStateFlags & 0xfffffdff;
        runtimeState->commandGeneration = runtimeState->commandGeneration << 2;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
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
  GameEntityCommandFlags *commandFlagsPtr;
  GameEntityRuntime *entityRuntime;
  int entriesRemaining;
  
  entriesRemaining = 0x20;
  do {
    entityRuntime = *selectionEntries;
    if ((entityRuntime != (GameEntityRuntime *)0x0) &&
       (((entityRuntime->common).commandFlags & 2) == 0)) {
      GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(entityRuntime);
      commandFlagsPtr = &(entityRuntime->common).commandFlags;
      *commandFlagsPtr = *commandFlagsPtr & 0xfffffdff;
    }
    selectionEntries = selectionEntries + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
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
  int entriesRemaining;
  
  entriesRemaining = 0x20;
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
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
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
  int entriesRemaining;
  WorldRuntimeContext *contextArg;
  
  entriesRemaining = 0x20;
  contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
  do {
    modelRuntime = *selectionEntries;
    if ((modelRuntime != (GameEntityRuntime *)0x0) &&
       (((modelRuntime->common).commandFlags & 2) == 0)) {
      ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(contextArg,(int *)modelRuntime);
    }
    selectionEntries = selectionEntries + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
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
  int entryIndex;

  /* Two REPNE SCASD passes: look for entityRuntime, and when absent store it in the first null slot. */
  for (entryIndex = 0; entryIndex < 0x20; entryIndex = entryIndex + 1) {
    if (selection->entries[entryIndex] == entityRuntime) break;
  }
  if (entryIndex == 0x20) {
    for (entryIndex = 0; entryIndex < 0x20; entryIndex = entryIndex + 1) {
      if (selection->entries[entryIndex] == (GameEntityRuntime *)0x0) {
        selection->entries[entryIndex] = entityRuntime;
        break;
      }
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
  int positionRecord;
  int averageXQ12;
  int selectedCountOrRemaining;
  int averageYQ12;
  int remainingOrEntryAddress;
  GameEntityRuntime **entryCursor;
  
  averageXQ12 = 0;
  averageYQ12 = 0;
  selectedCountOrRemaining = 0;
  remainingOrEntryAddress = 0x20;
  entryCursor = selection->entries;
  do {
    if (*entryCursor != (GameEntityRuntime *)0x0) {
      positionRecord = *(int *)((int)*entryCursor + 4);
      selectedCountOrRemaining = selectedCountOrRemaining + 1;
      averageXQ12 = averageXQ12 + *(int *)(positionRecord + 0x94);
      averageYQ12 = averageYQ12 + *(int *)(positionRecord + 0x98);
    }
    entryCursor = entryCursor + 1;
    remainingOrEntryAddress = remainingOrEntryAddress + -1;
  } while (remainingOrEntryAddress != 0);
  if (selectedCountOrRemaining != 0) {
    averageXQ12 = averageXQ12 / selectedCountOrRemaining;
    averageYQ12 = averageYQ12 / selectedCountOrRemaining;
    selectedCountOrRemaining = 0x20;
    do {
      remainingOrEntryAddress = *(int *)selection;
      if (remainingOrEntryAddress != 0) {
        positionRecord = *(int *)(remainingOrEntryAddress + 4);
        averageXQ12 = averageXQ12 - *(int *)(positionRecord + 0x94);
        averageYQ12 = averageYQ12 - *(int *)(positionRecord + 0x98);
        *(int *)(remainingOrEntryAddress + 0x60) = averageXQ12;
        *(int *)(remainingOrEntryAddress + 100) = averageYQ12;
        averageXQ12 = averageXQ12 + *(int *)(positionRecord + 0x94);
        averageYQ12 = averageYQ12 + *(int *)(positionRecord + 0x98);
      }
      selection = (SelectionPointerArray32 *)((int)selection + 4);
      selectedCountOrRemaining = selectedCountOrRemaining + -1;
    } while (selectedCountOrRemaining != 0);
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
  int entryIndex;

  /* REPNE SCASD over the 32 entries: CF clear when target was found. */
  for (entryIndex = 0; entryIndex < 0x20; entryIndex = entryIndex + 1) {
    if (array->entries[entryIndex] == target) {
      return false;
    }
  }
  return true;
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
  int entryAddress;
  int minOffset60;
  int maxOffset60;
  int maxOffset64;
  int firstEntryOrMinOffset64;
  int entriesRemaining;
  
  entriesRemaining = 0x20;
  while (firstEntryOrMinOffset64 = *(int *)selection, firstEntryOrMinOffset64 == 0) {
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining = entriesRemaining + -1;
    if (entriesRemaining == 0) {
      return false;
    }
  }
  minOffset60 = *(int *)(firstEntryOrMinOffset64 + 0x60);
  maxOffset64 = *(int *)(firstEntryOrMinOffset64 + 100);
  maxOffset60 = minOffset60;
  firstEntryOrMinOffset64 = maxOffset64;
  do {
    entryAddress = *(int *)selection;
    if (entryAddress != 0) {
      if (*(int *)(entryAddress + 0x60) < minOffset60) {
        minOffset60 = *(int *)(entryAddress + 0x60);
      }
      if (*(int *)(entryAddress + 100) < firstEntryOrMinOffset64) {
        firstEntryOrMinOffset64 = *(int *)(entryAddress + 100);
      }
      if (maxOffset60 < *(int *)(entryAddress + 0x60)) {
        maxOffset60 = *(int *)(entryAddress + 0x60);
      }
      if (maxOffset64 < *(int *)(entryAddress + 100)) {
        maxOffset64 = *(int *)(entryAddress + 100);
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  if (((maxOffset60 - minOffset60 < 0x5001) && (maxOffset64 - firstEntryOrMinOffset64 < 0x5001)) &&
     ((maxOffset60 - minOffset60) + (maxOffset64 - firstEntryOrMinOffset64) < 0x7001)) {
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
  int *classRecord;
  int markerSourceId;
  int entriesRemaining;
  int packedMarkerMatches;
  int markerSlotIndex;
  
  entriesRemaining = 0x20;
  do {
    if ((*(int **)selection != (int *)0x0) &&
       (classRecord = (int *)**(int **)selection, *(int *)(*classRecord + 0x4c) == 0x16)) {
      markerSlotIndex = 0xc;
      packedMarkerMatches = 0;
      do {
        markerSourceId = classRecord[markerSlotIndex + 0x1e];
        if (markerSourceId == g_ArmyLinkedChildAssetIdSlot0) {
          packedMarkerMatches = packedMarkerMatches + 1;
        }
        if (markerSourceId == g_ArmyLinkedChildAssetIdSlot1) {
          packedMarkerMatches = packedMarkerMatches + 0x100;
        }
        if (markerSourceId == g_ArmyLinkedChildAssetIdSlot2) {
          packedMarkerMatches = packedMarkerMatches + 0x10000;
        }
        markerSlotIndex = markerSlotIndex + -1;
      } while (-1 < markerSlotIndex);
      if ((laneMask & 1) != 0) {
        *(char *)(classRecord + 0x37) = (char)packedMarkerMatches;
        classRecord[0x2e] = valueA;
        classRecord[0x2f] = valueB;
        classRecord[0x30] = valueC;
      }
      if ((laneMask & 2) != 0) {
        *(char *)((int)classRecord + 0xdd) = (char)((uint32_t)packedMarkerMatches >> 8);
        classRecord[0x31] = valueA;
        classRecord[0x32] = valueB;
        classRecord[0x33] = valueC;
      }
      if ((laneMask & 4) != 0) {
        *(char *)((int)classRecord + 0xde) = (char)((uint32_t)packedMarkerMatches >> 0x10);
        classRecord[0x34] = valueA;
        classRecord[0x35] = valueB;
        classRecord[0x36] = valueC;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
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

