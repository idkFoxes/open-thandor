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
   Draws the metric frame of one selected entity around its projected screen bounds (panelTop..panelBottom,
   panelLeft..panelRight): four corner cells (hierarchy meter, group number), a bar along the top and bottom edge
   and bars or segment rows on the left and right edge, all from the SELECTION_PANEL_CELL_* layout. What the bars
   show depends on the entity kind (runtimeLinkOrKind08) and its definition class; entities of other factions
   only get the empty frame. Called by SelectionOverlay_RenderSelectedArmyMetrics,
   SelectionOverlay_RenderArmyMetricsForEntity (gameplay/selection/overlay.c) and
   UiArmyMetricsPanel_DrawTextureMetricsAndChildren (ui/controls/text.c).
*/
void SelectionPanel_RenderArmyRuntimeMetrics
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate panelBottom,UiPixelCoordinate panelRight,
          UiPixelCoordinate panelTop,UiPixelCoordinate panelLeft,
          RuntimeModelFactionPrefix *runtimeEntry)

{
  uint32_t childDefinitionAddress;
  ModelRuntimeSlot *childModelRuntime;
  ArmyCommandGeneration maximumValue;
  InGameRuntimeRoot *inGameRoot;
  uint32_t runtimeKindOrValue;
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
  if (runtimeEntry->factionIndex == (inGameRoot->worldRuntime).activeFactionRuntimeIndex) {
    /* kinds 1-3 and class-0x16 entities of a higher kind get special bars; every other case (including a
       special case whose condition fails) ends in the generic frame after this block */
    if (runtimeKindOrValue != 0) {
      if (runtimeKindOrValue == 1) {
        /* a factory (class 13) or production building (class 11) that is building: its build progress */
        if (((armyRuntime->classState).behaviorState == ARMY_FACTORY_STATE_BUILDING) &&
           ((armyRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13 ||
            (armyRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11)))) {
          activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,
                              (UiNumericValue32)(activeTotalMetrics >> 0x20),(UiNumericValue32)activeTotalMetrics,SELECTION_PANEL_CELL_HIERARCHY_METER);
          groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndex(runtimeEntry);
          if (groupIndexResult.notFound) {
            topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,SELECTION_PANEL_CELL_CORNER_TOP_RIGHT);
          }
          else {
            topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                groupIndexResult.runtimeGroupIndex,SELECTION_PANEL_CELL_GROUP_NUMBER);
          }
          bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT);
          bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextX,
                     topLeftAdvance.nextX,(armyRuntime->classLinkState).classState68,
                     (armyRuntime->classLinkState).classState64,SELECTION_PANEL_CELL_TOP_BAR);
          scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextX,
                     bottomLeftAdvance.nextX,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                     SELECTION_PANEL_CELL_BOTTOM_BAR);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextY,topLeftAdvance.nextY,
                     panelLeft,SELECTION_PANEL_CELL_LEFT_BAR);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextY,topRightAdvance.nextY,
                     panelRight,SELECTION_PANEL_CELL_RIGHT_BAR);
          goto EndFramebufferAccess;
        }
      }
      else if (runtimeKindOrValue < 3) {
        childModelRuntime = armyRuntime->attachments[0].childModelRuntimeOrSavedOffset;
        if (((armyRuntime->attachmentCount != 0) && (childModelRuntime != NULL)) &&
           ((childDefinitionAddress = (childModelRuntime->definitionOrSavedId).savedIdOrOffset,
            ((ModelDefinition *)childDefinitionAddress)->runtimeClassId == MODEL_RUNTIME_CLASS_05 ||
            (((((ModelDefinition *)childDefinitionAddress)->runtimeClassId == MODEL_RUNTIME_CLASS_06 ||
               (((ModelDefinition *)childDefinitionAddress)->runtimeClassId == MODEL_RUNTIME_CLASS_07)) ||
             (((ModelDefinition *)childDefinitionAddress)->runtimeClassId == MODEL_RUNTIME_CLASS_08)))))) {
          /* a weapon: its reload countdown against the weapon definition's reload ticks */
          workingValue = ((ModelRuntimeWeaponAimStateView *)childModelRuntime)->attachmentReloadCountdownTicks;
          capacityOrMetric = ((ModelRuntimeWeaponAimStateView *)childModelRuntime)->modelDefinition->attachmentReloadTicks;
          activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          activeMetricMaximum = (int)(activeTotalMetrics >> 0x20);
          if (activeMetricMaximum == 0) {
            topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,SELECTION_PANEL_CELL_CORNER_TOP_LEFT);
          }
          else {
            topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,
                                activeMetricMaximum,(UiNumericValue32)activeTotalMetrics,SELECTION_PANEL_CELL_HIERARCHY_METER);
          }
          groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndex(runtimeEntry);
          if (groupIndexResult.notFound) {
            topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,SELECTION_PANEL_CELL_CORNER_TOP_RIGHT);
          }
          else {
            topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                groupIndexResult.runtimeGroupIndex,SELECTION_PANEL_CELL_GROUP_NUMBER);
          }
          bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT);
          bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextX,
                     topLeftAdvance.nextX,capacityOrMetric,capacityOrMetric - workingValue,SELECTION_PANEL_CELL_TOP_BAR);
          scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextX,
                     bottomLeftAdvance.nextX,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                     SELECTION_PANEL_CELL_BOTTOM_BAR);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextY,topLeftAdvance.nextY,
                     panelLeft,SELECTION_PANEL_CELL_LEFT_BAR);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextY,topRightAdvance.nextY,
                     panelRight,SELECTION_PANEL_CELL_RIGHT_BAR);
          goto EndFramebufferAccess;
        }
      }
      else if (runtimeKindOrValue == 3) {
        childModelRuntime = armyRuntime->attachments[0].childModelRuntimeOrSavedOffset;
        if ((armyRuntime->attachmentCount != 0) && (childModelRuntime != NULL)) {
          armyRuntime = (ModelRuntimeSlot *)(childModelRuntime->definitionOrSavedId).savedIdOrOffset;
          runtimeKindOrValue = 0xffffffff;
          workingValue = 7;
          /* armyRuntime holds the child's definition from here on */
          if (((ModelDefinition *)armyRuntime)->runtimeClassId == MODEL_RUNTIME_CLASS_09) {
            /* unsigned minimum of the child's eight attachment reload ticks, then at least classState80 (signed) */
            do {
              if (((ModelRuntimeWeaponAimStateView *)childModelRuntime)->attachmentReloadTicks[workingValue] < runtimeKindOrValue) {
                runtimeKindOrValue = ((ModelRuntimeWeaponAimStateView *)childModelRuntime)->attachmentReloadTicks[workingValue];
              }
              workingValue--;
            } while (-1 < workingValue);
            if ((int)runtimeKindOrValue < (int)(childModelRuntime->classLinkState).classState80) {
              runtimeKindOrValue = (childModelRuntime->classLinkState).classState80;
            }
            /* top bar: maximumValue minus the positive part of that clamped minimum */
            workingValue = 0;
            /* armyRuntime holds the child's (weapon) definition here */
            maximumValue = ((ArmyWeaponDefinitionView *)armyRuntime)->attachmentReloadTicks;
            if (0 < (int)runtimeKindOrValue) {
              workingValue = -runtimeKindOrValue;
            }
            activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
            capacityOrMetric = (int)(activeTotalMetrics >> 0x20);
            if (capacityOrMetric == 0) {
              topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,SELECTION_PANEL_CELL_CORNER_TOP_LEFT);
            }
            else {
              topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,capacityOrMetric,
                                  (UiNumericValue32)activeTotalMetrics,SELECTION_PANEL_CELL_HIERARCHY_METER);
            }
            groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndex(runtimeEntry);
            if (groupIndexResult.notFound) {
              topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,SELECTION_PANEL_CELL_CORNER_TOP_RIGHT);
            }
            else {
              topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                  groupIndexResult.runtimeGroupIndex,SELECTION_PANEL_CELL_GROUP_NUMBER);
            }
            bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT);
            bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT);
            SelectionPanel_DrawProportionalCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextX,
                       topLeftAdvance.nextX,maximumValue,workingValue + maximumValue,SELECTION_PANEL_CELL_TOP_BAR);
            scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
            SelectionPanel_DrawProportionalCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextX,
                       bottomLeftAdvance.nextX,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                       SELECTION_PANEL_CELL_BOTTOM_BAR);
            SelectionPanel_DrawSolidCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextY,topLeftAdvance.nextY,
                       panelLeft,SELECTION_PANEL_CELL_LEFT_BAR);
            SelectionPanel_DrawSolidCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextY,topRightAdvance.nextY,
                       panelRight,SELECTION_PANEL_CELL_RIGHT_BAR);
            goto EndFramebufferAccess;
          }
        }
      }
      else if ((3 < runtimeKindOrValue) &&
              (armyRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22)) {
        if (((ArmyRuntimeArticulatedContactState *)&(armyRuntime->classState).classStateAC)->
            terrainContactMode == ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE) {
          activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,
                              (UiNumericValue32)(activeTotalMetrics >> 0x20),(UiNumericValue32)activeTotalMetrics,SELECTION_PANEL_CELL_HIERARCHY_METER);
          groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndex(runtimeEntry);
          if (groupIndexResult.notFound) {
            topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,SELECTION_PANEL_CELL_CORNER_TOP_RIGHT);
          }
          else {
            topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                groupIndexResult.runtimeGroupIndex,SELECTION_PANEL_CELL_GROUP_NUMBER);
          }
          bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT);
          bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextX,
                     topLeftAdvance.nextX,(armyRuntime->classLinkState).classState68,
                     (armyRuntime->classLinkState).classState64,SELECTION_PANEL_CELL_TOP_BAR);
          scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextX,
                     bottomLeftAdvance.nextX,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                     SELECTION_PANEL_CELL_BOTTOM_BAR);
          armyMetrics = ArmyRuntime_GetLinkedChildSlotMeterRegs((ArmyRuntimeSlot *)armyRuntime);
          halfFilledSegments = armyMetrics.filledSegments >> 1;
          runtimeKindOrValue = armyMetrics.totalSegments >> 1;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextY,topLeftAdvance.nextY,
                     panelLeft,armyMetrics.totalSegments - runtimeKindOrValue,armyMetrics.filledSegments - halfFilledSegments,SELECTION_PANEL_CELL_LEFT_SEGMENTS);
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextY,topRightAdvance.nextY,
                     panelRight,runtimeKindOrValue,halfFilledSegments,SELECTION_PANEL_CELL_RIGHT_SEGMENTS);
        }
        else if (((armyRuntime->classState).stateFlags & (ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_RESEARCH_UNPAID)) == 0) {
          activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          workingValue = (int)(activeTotalMetrics >> 0x20);
          if (workingValue == 0) {
            topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,SELECTION_PANEL_CELL_CORNER_TOP_LEFT);
          }
          else {
            topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,workingValue,
                                (UiNumericValue32)activeTotalMetrics,SELECTION_PANEL_CELL_HIERARCHY_METER);
          }
          groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndex(runtimeEntry);
          if (groupIndexResult.notFound) {
            topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,SELECTION_PANEL_CELL_CORNER_TOP_RIGHT);
          }
          else {
            topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                groupIndexResult.runtimeGroupIndex,SELECTION_PANEL_CELL_GROUP_NUMBER);
          }
          bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT);
          bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT);
          SelectionPanel_DrawForwardCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextX,
                     topLeftAdvance.nextX,SELECTION_PANEL_CELL_TOP_BAR_EMPTY);
          scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextX,
                     bottomLeftAdvance.nextX,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                     SELECTION_PANEL_CELL_BOTTOM_BAR);
          armyMetrics = ArmyRuntime_GetLinkedChildSlotMeterRegs((ArmyRuntimeSlot *)armyRuntime);
          halfFilledSegments = armyMetrics.filledSegments >> 1;
          runtimeKindOrValue = armyMetrics.totalSegments >> 1;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextY,topLeftAdvance.nextY,
                     panelLeft,armyMetrics.totalSegments - runtimeKindOrValue,armyMetrics.filledSegments - halfFilledSegments,SELECTION_PANEL_CELL_LEFT_SEGMENTS);
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextY,topRightAdvance.nextY,
                     panelRight,runtimeKindOrValue,halfFilledSegments,SELECTION_PANEL_CELL_RIGHT_SEGMENTS);
        }
        else {
          activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
          workingValue = (int)(activeTotalMetrics >> 0x20);
          if (workingValue == 0) {
            topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,SELECTION_PANEL_CELL_CORNER_TOP_LEFT);
          }
          else {
            topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,workingValue,
                                (UiNumericValue32)activeTotalMetrics,SELECTION_PANEL_CELL_HIERARCHY_METER);
          }
          groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndex(runtimeEntry);
          if (groupIndexResult.notFound) {
            topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,SELECTION_PANEL_CELL_CORNER_TOP_RIGHT);
          }
          else {
            topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                                groupIndexResult.runtimeGroupIndex,SELECTION_PANEL_CELL_GROUP_NUMBER);
          }
          bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT);
          bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextX,
                     topLeftAdvance.nextX,armyRuntime->researchDurationTicks,
                     armyRuntime->researchElapsedTicks,SELECTION_PANEL_CELL_TOP_BAR);
          scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextX,
                     bottomLeftAdvance.nextX,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,
                     SELECTION_PANEL_CELL_BOTTOM_BAR);
          armyMetrics = ArmyRuntime_GetLinkedChildSlotMeterRegs((ArmyRuntimeSlot *)armyRuntime);
          halfFilledSegments = armyMetrics.filledSegments >> 1;
          runtimeKindOrValue = armyMetrics.totalSegments >> 1;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextY,topLeftAdvance.nextY,
                     panelLeft,armyMetrics.totalSegments - runtimeKindOrValue,armyMetrics.filledSegments - halfFilledSegments,SELECTION_PANEL_CELL_LEFT_SEGMENTS);
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextY,topRightAdvance.nextY,
                     panelRight,runtimeKindOrValue,halfFilledSegments,SELECTION_PANEL_CELL_RIGHT_SEGMENTS);
        }
        goto EndFramebufferAccess;
      }
    }
    if (((armyRuntime->classState).stateFlags & (ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_RESEARCH_UNPAID)) == 0) {
      activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
      workingValue = (int)(activeTotalMetrics >> 0x20);
      if (workingValue == 0) {
        topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,SELECTION_PANEL_CELL_CORNER_TOP_LEFT);
      }
      else {
        topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,workingValue,
                            (UiNumericValue32)activeTotalMetrics,SELECTION_PANEL_CELL_HIERARCHY_METER);
      }
      groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndex(runtimeEntry);
      if (groupIndexResult.notFound) {
        topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,SELECTION_PANEL_CELL_CORNER_TOP_RIGHT);
      }
      else {
        topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                            groupIndexResult.runtimeGroupIndex,SELECTION_PANEL_CELL_GROUP_NUMBER);
      }
      bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT);
      bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT);
      SelectionPanel_DrawForwardCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextX,topLeftAdvance.nextX,
                 SELECTION_PANEL_CELL_TOP_BAR_EMPTY);
      scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextX,bottomLeftAdvance.nextX
                 ,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,SELECTION_PANEL_CELL_BOTTOM_BAR);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextY,topLeftAdvance.nextY,panelLeft,
                 SELECTION_PANEL_CELL_LEFT_BAR);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextY,topRightAdvance.nextY,panelRight,
                 SELECTION_PANEL_CELL_RIGHT_BAR);
    }
    else {
      activeTotalMetrics = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(runtimeEntry);
      workingValue = (int)(activeTotalMetrics >> 0x20);
      if (workingValue == 0) {
        topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,SELECTION_PANEL_CELL_CORNER_TOP_LEFT);
      }
      else {
        topLeftAdvance = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,workingValue,
                            (UiNumericValue32)activeTotalMetrics,SELECTION_PANEL_CELL_HIERARCHY_METER);
      }
      groupIndexResult = GameFactionRuntime_FindRuntimeGroupIndex(runtimeEntry);
      if (groupIndexResult.notFound) {
        topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,SELECTION_PANEL_CELL_CORNER_TOP_RIGHT);
      }
      else {
        topRightAdvance = SelectionPanel_DrawNumberCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,
                            groupIndexResult.runtimeGroupIndex,SELECTION_PANEL_CELL_GROUP_NUMBER);
      }
      bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT);
      bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextX,topLeftAdvance.nextX,
                 armyRuntime->researchDurationTicks,
                 armyRuntime->researchElapsedTicks,SELECTION_PANEL_CELL_TOP_BAR);
      scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextX,bottomLeftAdvance.nextX
                 ,(UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,SELECTION_PANEL_CELL_BOTTOM_BAR);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextY,topLeftAdvance.nextY,panelLeft,
                 SELECTION_PANEL_CELL_LEFT_BAR);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextY,topRightAdvance.nextY,panelRight,
                 SELECTION_PANEL_CELL_RIGHT_BAR);
    }
  }
  else {
    topLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,SELECTION_PANEL_CELL_CORNER_TOP_LEFT);
    topRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,SELECTION_PANEL_CELL_CORNER_TOP_RIGHT);
    bottomLeftAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT);
    bottomRightAdvance = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT);
    SelectionPanel_DrawForwardCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,panelTop,topRightAdvance.nextX,topLeftAdvance.nextX,SELECTION_PANEL_CELL_TOP_BAR_EMPTY);
    scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(runtimeEntry);
    SelectionPanel_DrawProportionalCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,panelBottom,bottomRightAdvance.nextX,bottomLeftAdvance.nextX,
               (UiNumericValue32)(scaleRatio >> 0x20),(UiNumericValue32)scaleRatio,SELECTION_PANEL_CELL_BOTTOM_BAR);
    SelectionPanel_DrawSolidCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,bottomLeftAdvance.nextY,topLeftAdvance.nextY,panelLeft,SELECTION_PANEL_CELL_LEFT_BAR);
    SelectionPanel_DrawSolidCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,bottomRightAdvance.nextY,topRightAdvance.nextY,panelRight,
               SELECTION_PANEL_CELL_RIGHT_BAR);
  }
EndFramebufferAccess:
  g_GraphicsFramebufferEndAccess();
  return;
}


/* Address: 0x0055FA20.
   In-game command handler (code 0x8F0, key A): replaces the player's selection with every world model of
   definition class 0x16 that the player owns, then rebuilds the selection panels when the player is the local
   one.
*/
void InGameSelection_SelectAllOwnAircraftPads
          (PlayerRuntimeId playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3)

{
  WorldOwnerListNode *ownerNode;
  GameEntityRuntime *entityRuntime;
  InGameRuntimeRoot *inGameRoot;

  inGameRoot = g_InGameRuntimeRoot;
  SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  for (ownerNode = (inGameRoot->worldRuntime).ownerListHead; ownerNode != NULL;
      ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      /* model payload: the ModelRuntimeSlot; its owner army is the entity */
      entityRuntime =
           (GameEntityRuntime *)((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
      if ((((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
           0x16) &&
         ((entityRuntime->common).ownership.ownerIndex ==
          g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->factionIndex))
      {
        SelectionPointerArray_InsertUniqueAndRecenter
                  (entityRuntime,&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection)
        ;
      }
    }
  }
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    InGameBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
  return;
}


/* Address: 0x0055FB30.
   In-game command handler INGAME_COMMAND_REPLACE_SELECTION: replaces the player's selection with all world
   entries matching the army at the rebased index (byte offset from g_ArmyRuntimeRebaseBaseMinusOne, 0 = none)
   and refreshes the local selection panels. Nothing is added when the army has no model node.
*/
void InGamePlayerSelection_ReplaceWithArmyRuntimeIndex
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,uint32_t unusedPayload2,
          RuntimeToken armyRuntimeIndex)

{
  ArmyRuntimeSlot *sourceArmyRuntime;

  if (armyRuntimeIndex != 0) {
    sourceArmyRuntime = (ArmyRuntimeSlot *)(armyRuntimeIndex + (int)g_ArmyRuntimeRebaseBaseMinusOne);
    SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
    if (sourceArmyRuntime->modelNodeRuntime != NULL) {
      SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity
                (sourceArmyRuntime,&g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
      if (playerId == g_LocalPlayerRuntimeId) {
        InGameSelectionDetailPanel_Rebuild();
        InGameBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
      }
    }
  }
  return;
}


/* Address: 0x0055FE70.
   In-game command handler INGAME_COMMAND_POSITION_VARIANT_B (plain click on the ground): sends the player's
   selection to the world point, each entry keeping its formation offset unless the selection is spread too wide.
   A lone class-0x0D entry takes the point into its definition record instead and the selection is cleared.
*/
void InGamePlayerSelection_ApplyMoveCommand
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,CommandPayload worldXQ12,
          CommandPayload worldYQ12)

{
  SelectionPointerArray_ApplyMoveCommand
            (worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}


/* Address: 0x0055FEA0.
   In-game command handler INGAME_COMMAND_POSITION (Shift/Alt-click on the ground): queues the world point as a
   waypoint for every entry of the player's selection (formation offsets as in the plain move).
*/
void InGamePlayerSelection_ApplyPositionCommand
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,CommandPayload worldXQ12,
          CommandPayload worldYQ12)

{
  SelectionPointerArray_ApplyPositionCommand
            (worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}


/* Address: 0x0055FED0.
   In-game command handler INGAME_COMMAND_SELECT_ARMY (click on an army as an order target): makes the army at
   the rebased index the command target of every eligible entry of the player's selection. Ignored for index 0
   and for armies without a model node.
*/
void InGamePlayerSelection_SelectArmyRuntimeIndex
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,uint32_t unusedPayload2,
          RuntimeToken armyRuntimeIndex)

{
  if ((armyRuntimeIndex != 0) &&
     (((ArmyRuntimeSlot *)(armyRuntimeIndex + (int)g_ArmyRuntimeRebaseBaseMinusOne))->
      modelNodeRuntime != NULL)) {
    SelectionPointerArray_ApplyArmyRuntimeTarget
              ((ArmyRuntimeSlot *)(armyRuntimeIndex + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  }
  return;
}


/* Address: 0x0055FF10.
   In-game command handler INGAME_COMMAND_TARGET_POSITION (Ctrl-click on the ground): gives every eligible
   entry of the player's selection the terrain point (surface height, x, y) as its target position.
*/
void InGamePlayerSelection_ApplyTargetPositionCommand(PlayerRuntimeId playerId,CommandPayload surfaceHeightQ12,
          CommandPayload worldXQ12,CommandPayload worldYQ12)

{
  SelectionPointerArray_ApplyTargetPositionCommand
            (surfaceHeightQ12,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}


/* Address: 0x0055FF40.
   In-game command handler 0xE10 (key S, stop): resets the movement of the player's selection, drops its
   class-0x16 entries and recenters the formation offsets (SelectionRuntime_ResetMovementPruneAndRecenterEntries).
*/
void PlayerSelection_ResetMovementPruneAndRecenterEntries(PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3)

{
  SelectionRuntime_ResetMovementPruneAndRecenterEntries
            ((GameEntityRuntime **)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}


/* Address: 0x0055FF60.
   In-game command handler 0xE30 (Shift+S): resets the movement anchors of the eligible entries of the player's
   selection and clears their command flag 0x200. The block pointer doubles as its selection array (first
   member).
*/
void PlayerSelection_StopMovement
          (PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3)

{
  SelectionRuntime_StopMovement
            ((GameEntityRuntime **)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}


/* Address: 0x0055FF80.
   In-game command handler 0xE50 (Alt+S): interrupts the active targets of the eligible entries of the player's
   selection and clears flag 0x10 of their dword +0x2C.
*/
void PlayerSelection_CancelTargets
          (PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3)

{
  SelectionRuntime_CancelTargets
            ((GameEntityRuntime **)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}


/* Address: 0x0055FFA0.
   In-game command handler 0xE70 (Alt+D): applies the model hierarchy flags 0x418 to the eligible entries of the
   player's selection (SelectionRuntime_SelfDestruct).
*/
void PlayerSelection_SelfDestruct
          (PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3)

{
  SelectionRuntime_SelfDestruct
            ((GameEntityRuntime **)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}


/* Address: 0x0055FFC0.
   Pointer-mode handler for lane 1 (g_InGamePointerModeHandlers[1], chosen in gameplay/input/world.c when the
   modifier mask (no modifier = 7, Shift = 1) and the attachment variant mask leave 1; networked as command code
   0xE90): stores the pointed world point and preview heading as marker lane 1 of every class-0x16 entity in
   the player's selection (SelectionPointerArray_SetAircraftPadTargets).
*/
void InGameSelection_SetAircraftPadTargetLane1
          (SelectionMarkerIndex playerRuntimeId,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (1,heading16,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  return;
}


/* Address: 0x0055FFF0.
   Pointer-mode handler for lane 2 (g_InGamePointerModeHandlers[2]: modifier mask & attachment variant mask
   == 2, Alt = 2; networked as command code 0xEC0): like InGameSelection_SetAircraftPadTargetLane1, for marker lane 2.
*/
void InGameSelection_SetAircraftPadTargetLane2
          (SelectionMarkerIndex playerRuntimeId,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (2,heading16,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  return;
}


/* Address: 0x00562050.
   In-game command handler 0x2F20: moves the player's primary selected model (block +0x8094, rebased offset, 0 =
   none) by a pointer-drag delta, writes the new point into its path and tracked coordinates and the model
   transform, and lets the definition's placement contact kind (+0x278) set its height before the transforms and
   depth bins are rebuilt. Sent by InGameUiCommand_UpdateInteractionByMode (ui/ingame/runtime.c) while the
   pointer drags with bit 0x4 of g_CursorButtonState set.
*/
void SelectionPlayerRuntime_MovePrimarySelectionBy
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved,Q12 deltaYQ12,Q12 deltaXQ12)

{
  uint32_t primaryEntityOffset;
  ModelRuntimeNode *modelNode;
  int definitionAddress;
  int placementContactKind;
  int newWorldXQ12;
  GameEntityRuntime *target;
  int newWorldYQ12;
  WorldRuntimeContext *worldRuntime;

  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  primaryEntityOffset = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placedArmyToken;
  if (primaryEntityOffset != 0) {
    target = (GameEntityRuntime *)((int)g_ArmyRuntimeRebaseBaseMinusOne + primaryEntityOffset);
    /* the result is ignored: the primary entity is moved whether or not it is still selected */
    SelectionPointerArray_Contains
              (target,&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    modelNode = (target->common).ownership.modelNode;
    newWorldXQ12 = deltaXQ12 + (modelNode->worldTransform).translation.x;
    newWorldYQ12 = deltaYQ12 + (modelNode->worldTransform).translation.y;
    definitionAddress = *(int *)(target->common).ownership.definitionOrClassRecord;
    (target->common).pathCoordinate0Q12 = newWorldXQ12;
    (target->common).pathCoordinate1Q12 = newWorldYQ12;
    (target->common).trackedCoordinate0Q12 = newWorldXQ12;
    (target->common).trackedCoordinate1Q12 = newWorldYQ12;
    (target->common).damageState.trackedCoordinate0Q12 = newWorldXQ12;
    (target->common).damageState.trackedCoordinate1Q12 = newWorldYQ12;
    (modelNode->worldTransform).translation.x = newWorldXQ12;
    placementContactKind = ((ModelDefinition *)definitionAddress)->placementContactKindIndex;
    (modelNode->worldTransform).translation.y = newWorldYQ12;
    if (placementContactKind == ARMY_PLACEMENT_CONTACT_KIND_ARTICULATED_SUSPENSION) {
      ((modelNode->runtimePayload).armyRuntime)->movementTarget0Q12 = 0x7fffffff;
    }
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (((ModelDefinition *)definitionAddress)->placementHeightOffsetQ12,newWorldYQ12,newWorldXQ12,modelNode,
               worldRuntime);
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
    ModelNodeRuntime_UpdateDepthBinMasks(((ModelDefinition *)definitionAddress)->footprintRadius,modelNode);
  }
  return;
}


/* Address: 0x00562220.
   In-game command handler 0x30F0: turns the player's primary selected model (block +0x8094) by angleDelta
   (16-bit angle, wraps) and rebuilds its transforms. Sent by InGameUiCommand_UpdateInteractionByMode
   (ui/ingame/runtime.c) with the horizontal pointer drag * 64.
*/
void SelectionPlayerRuntime_RotatePrimarySelectionBy
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved0,uint32_t reserved1,AngleTurn32 angleDelta)

{
  uint32_t primaryEntityOffset;
  ModelRuntimeNode *modelNodeRuntime;
  GameEntityRuntime *target;

  primaryEntityOffset = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placedArmyToken;
  if (primaryEntityOffset != 0) {
    target = (GameEntityRuntime *)((int)g_ArmyRuntimeRebaseBaseMinusOne + primaryEntityOffset);
    /* the result is ignored, as in SelectionPlayerRuntime_MovePrimarySelectionBy */
    SelectionPointerArray_Contains
              (target,&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    modelNodeRuntime = (target->common).ownership.modelNode;
    (modelNodeRuntime->modelPayload).worldRotationAngle2 =
         angleDelta + (modelNodeRuntime->modelPayload).worldRotationAngle2 & 0xffff;
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  }
  return;
}


/* Address: 0x0052CEE0.
   Loads the selection and information panel graphics (gfx\panel\select.gfx, info.gfx) and their 0x1A4-byte .dat
   tables, empties the selection of all eight player blocks and stores the caller's entity-slot table. Then it
   patches sequence descriptors inside the loaded textures (swaps two select.gfx entries, rewrites frames of
   info.gfx) - the exact meaning of these patches is not known. On failure returns the failing loader's error.
*/
StatusResult SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots)

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
          blockCountOrRecordOffset = 8; /* player blocks */
          playerBlockCursor = g_SelectionPlayerBlocks;
          g_InfoPanelData = loadedResource;
          do {
            /* clear the 32 selection entries; the cursor then skips the rest of the 0x8118-byte block */
            for (entriesRemaining = 0x20; loadedResource = g_SelectionPanelTextureSource, entriesRemaining != 0;
                entriesRemaining--) {
              (playerBlockCursor->selection).entries[0] = NULL;
              playerBlockCursor = (SelectionPlayerRuntimeBlock *)((playerBlockCursor->selection).entries + 1);
            }
            playerBlockCursor = (SelectionPlayerRuntimeBlock *)&playerBlockCursor->pendingPlacementArmyAsset;
            blockCountOrRecordOffset--;
          } while (blockCountOrRecordOffset != 0);
          g_SelectionInfoEntitySlots = entitySlots;
          tableOffset = (g_SelectionPanelTextureSource->tableDescriptor).subresourceTableOffset;
          /* select.gfx: swap the data offsets of subresources 0x2E and 0x2D (XCHG in the original) */
          LOCK();
          swappedSelectionDataOffset =
               ((GraphicsTextureSourceEntry *)((uint8_t *)g_SelectionPanelTextureSource + tableOffset))[0x2e].dataOffset;
          ((GraphicsTextureSourceEntry *)((uint8_t *)g_SelectionPanelTextureSource + tableOffset))[0x2e].dataOffset =
               ((GraphicsTextureSourceEntry *)((uint8_t *)g_SelectionPanelTextureSource + tableOffset))[0x2d].dataOffset;
          UNLOCK();
          ((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x2d].dataOffset =
               swappedSelectionDataOffset;
          /* info.gfx: rewrite records of several sequences (the records at the dataOffset of subresources
             0x2C..0x33; their layout is not typed); referencePayloadValue is the first dword of the record of
             subresource 0x2C */
          loadedResource = g_InfoPanelTextureSource;
          tableOffset = (g_InfoPanelTextureSource->tableDescriptor).subresourceTableOffset;
          referencePayloadValue =
               *(uint32_t *)((uint8_t *)g_InfoPanelTextureSource +
                            ((GraphicsTextureSourceEntry *)((uint8_t *)g_InfoPanelTextureSource + tableOffset))[0x2c].
                            dataOffset);
          blockCountOrRecordOffset =
               ((GraphicsTextureSourceEntry *)((uint8_t *)g_InfoPanelTextureSource + tableOffset))[0x2f].dataOffset;
          patchBytes = (uint8_t *)g_InfoPanelTextureSource + blockCountOrRecordOffset;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((uint8_t *)loadedResource + blockCountOrRecordOffset + 0x4) =
               referencePayloadValue;
          blockCountOrRecordOffset =
               ((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x30].dataOffset;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((uint8_t *)loadedResource + blockCountOrRecordOffset + 0x4) =
               referencePayloadValue;
          blockCountOrRecordOffset =
               ((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x31].dataOffset;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((uint8_t *)loadedResource + blockCountOrRecordOffset + 0x4) =
               referencePayloadValue;
          ((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x32].pixelHeight = 4;
          ((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x32].logicalHeight = 4;
          ((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x33].pixelHeight = 4;
          ((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x33].logicalHeight = 4;
          patchBytes =
               (uint8_t *)&((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x2d].pixelHeight;
          patchBytes[0] = 4;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          patchBytes =
               (uint8_t *)&((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x2d].logicalHeight;
          patchBytes[0] = 4;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          patchBytes =
               (uint8_t *)&((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x2e].pixelHeight;
          patchBytes[0] = 4;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          patchBytes =
               (uint8_t *)&((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x2e].logicalHeight;
          patchBytes[0] = 4;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          blockCountOrRecordOffset =
               ((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x32].dataOffset;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((uint8_t *)loadedResource + blockCountOrRecordOffset + 0x4) =
               referencePayloadValue;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x8;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0xc;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x10;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x14;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x18;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((uint8_t *)loadedResource + blockCountOrRecordOffset + 0x1c) =
               referencePayloadValue;
          blockCountOrRecordOffset =
               ((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x33].dataOffset;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((uint8_t *)loadedResource + blockCountOrRecordOffset + 0x4) =
               referencePayloadValue;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x8;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0xc;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x10;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x14;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x18;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          *(uint32_t *)((uint8_t *)loadedResource + blockCountOrRecordOffset + 0x1c) =
               referencePayloadValue;
          blockCountOrRecordOffset =
               ((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x2d].dataOffset;
          *(uint32_t *)((uint8_t *)loadedResource + blockCountOrRecordOffset) =
               referencePayloadValue;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x4;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x8;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0xc;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x10;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x14;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          *(uint32_t *)((uint8_t *)loadedResource + blockCountOrRecordOffset + 0x18) =
               referencePayloadValue;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x1c;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          blockCountOrRecordOffset =
               ((GraphicsTextureSourceEntry *)((uint8_t *)loadedResource + tableOffset))[0x2e].dataOffset;
          *(uint32_t *)((uint8_t *)loadedResource + blockCountOrRecordOffset) =
               referencePayloadValue;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x4;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0;
          patchBytes[3] = 0;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x8;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0xc;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x10;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x14;
          patchBytes[0] = 0;
          patchBytes[1] = 0;
          patchBytes[2] = 0xff;
          patchBytes[3] = 0xff;
          *(uint32_t *)((uint8_t *)loadedResource + blockCountOrRecordOffset + 0x18) =
               referencePayloadValue;
          patchBytes = (uint8_t *)loadedResource + blockCountOrRecordOffset + 0x1c;
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
   Counterpart of SelectionInfoPanel_InitResources: releases both panel textures and both .dat tables and clears
   the four resource pointers.
*/
void SelectionInfoPanel_ShutdownResources(void)

{
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_SelectionPanelTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_InfoPanelTextureSource);
  Resource_Release(g_SelectionPanelData);
  Resource_Release(g_InfoPanelData);
  g_SelectionPanelTextureSource = NULL;
  g_InfoPanelTextureSource = NULL;
  g_SelectionPanelData = NULL;
  g_InfoPanelData = NULL;
  return;
}


/* Address: 0x0052FB20.
   Removes an entity from the selections of all eight players (every matching entry of each player block's
   32-entry selection becomes NULL), so no selection keeps pointing at an entity that is being destroyed.
*/
void SelectionPlayerBlocks_RemovePointer(GameEntityRuntime *target)

{
  int entriesRemainingInBlock;
  int playerBlocksRemaining;
  SelectionPlayerRuntimeBlock *currentSelectionEntry;
  SelectionPlayerRuntimeBlock *selectionEntryCursor;

  /* The cursor is typed as a block but walks the selection entries one pointer at a time: entries[0] of the
     cursor is the current entry. */
  playerBlocksRemaining = 8;
  entriesRemainingInBlock = SELECTION_ENTRY_CAPACITY;
  selectionEntryCursor = g_SelectionPlayerBlocks;
  do {
    do {
      currentSelectionEntry = selectionEntryCursor;
      if (target == (currentSelectionEntry->selection).entries[0]) {
        (currentSelectionEntry->selection).entries[0] = NULL;
      }
      entriesRemainingInBlock--;
      selectionEntryCursor =
           (SelectionPlayerRuntimeBlock *)((currentSelectionEntry->selection).entries + 1);
    } while (entriesRemainingInBlock != 0);
    entriesRemainingInBlock = SELECTION_ENTRY_CAPACITY;
    playerBlocksRemaining--;
    /* from the last entry (+0x7C) to the next block: 0x7C + 0x809C = 0x8118 = sizeof(SelectionPlayerRuntimeBlock) */
    selectionEntryCursor =
         (SelectionPlayerRuntimeBlock *)&currentSelectionEntry->chatRecipientMaskAndWriteOffset;
  } while (playerBlocksRemaining != 0);
}


/* Address: 0x0052FB70.
   Returns the average world position (model node translation) of the local selection's entities; unresolved
   (CF) when the selection is empty, with all coordinates 0.
*/
WorldPositionResult __cdecl SelectionInfoEntitySlots_ComputeAverageWorldPositionRegs(void)

{
  ModelRuntimeNode *slotModelNode;
  int worldXAggregateQ12;
  int worldYAggregateQ12;
  int worldZAggregateQ12;
  GameEntityRuntime **selectionEntitySlotCursor;
  WorldPositionResult averagePosition;
  int selectedEntityCount;
  int selectionSlotsRemaining;

  worldXAggregateQ12 = 0;
  worldYAggregateQ12 = 0;
  worldZAggregateQ12 = 0;
  selectionSlotsRemaining = SELECTION_ENTRY_CAPACITY;
  selectedEntityCount = 0;
  selectionEntitySlotCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntitySlotCursor != NULL) {
      slotModelNode = ((*selectionEntitySlotCursor)->common).ownership.modelNode;
      worldXAggregateQ12 = worldXAggregateQ12 + (slotModelNode->worldTransform).translation.x;
      worldYAggregateQ12 = worldYAggregateQ12 + (slotModelNode->worldTransform).translation.y;
      worldZAggregateQ12 = worldZAggregateQ12 + (slotModelNode->worldTransform).translation.z;
      selectedEntityCount++;
    }
    selectionEntitySlotCursor++;
    selectionSlotsRemaining--;
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
   Removes an entity from one 32-entry selection array: only the first matching entry is set to NULL
   (an entity is in a selection at most once).
*/
void SelectionPointerArray_RemoveFirstMatch(GameEntityRuntime *target,SelectionPointerArray32 *array)

{
  int entryIndex;

  /* REPNE SCASD over the 32 entries; the first match is cleared. */
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    if (array->entries[entryIndex] == target) {
      array->entries[entryIndex] = NULL;
      return;
    }
  }
}


/* Address: 0x0052FDC0.
   Returns true (CF set) when the local selection holds at least one entity, false when it is empty.
*/
bool SelectionInfo_HasAnyEntry(void)

{
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  bool entryIsEmpty;
  GameEntityRuntime *currentEntry;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  entryIsEmpty = true;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  /* REPE SCASD against 0 in the original */
  do {
    if (entriesRemaining == 0) break;
    entriesRemaining--;
    currentEntry = *selectionEntryCursor;
    entryIsEmpty = currentEntry == NULL;
    selectionEntryCursor++;
  } while (entryIsEmpty);
  return !entryIsEmpty;
}


/* Address: 0x0052FDE0.
   Returns false (CF clear) when every entity of the local selection belongs to the faction ownerIndex (an empty
   selection passes), true as soon as one belongs to another faction.
*/
bool SelectionInfo_AllEntriesEmptyOrMatchOwner(FactionRuntimeIndex ownerIndex)

{
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  while ((*selectionEntryCursor == NULL ||
         (ownerIndex == ((*selectionEntryCursor)->common).ownership.ownerIndex))) {
    selectionEntryCursor++;
    entriesRemaining--;
    if (entriesRemaining == 0) {
      return false;
    }
  }
  return true;
}


/* Address: 0x0052FE30.
   Returns false (CF clear) when the local selection consists only of class-0x16 entities of faction ownerIndex
   and at least one of them has a non-zero dword +0x70 in its runtime record; true otherwise (also for an empty
   selection).
*/
bool SelectionInfo_TestNotOwnAircraftPadsWithAircraft(FactionRuntimeIndex ownerIndex)

{
  ModelRuntimeSlot *classRecord;
  int entriesRemaining;
  int activeEntryCount;
  GameEntityRuntime **selectionEntryCursor;
  GameEntityRuntime *currentEntry;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  activeEntryCount = 0;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    currentEntry = *selectionEntryCursor;
    if (currentEntry != NULL) {
      classRecord = (currentEntry->common).ownership.definitionOrClassRecord; /* the model runtime */
      if (ownerIndex != (currentEntry->common).ownership.ownerIndex) {
        return true;
      }
      if (classRecord->definitionOrSavedId.runtimeDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_22) {
        return true;
      }
      if (classRecord->classLinkState.classState70 != 0) {
        activeEntryCount++;
      }
    }
    selectionEntryCursor++;
    entriesRemaining--;
    if (entriesRemaining == 0) {
      if (activeEntryCount == 0) {
        return true;
      }
      return false;
    }
  } while( true );
}


/* Address: 0x0052FEB0.
   Returns false (CF clear) when the local selection can take a ground position order: some entity's
   definition has a non-zero dword +0x18, or the selection is a single entity of definition class 0x0D (13).
   True otherwise; the world input then ignores the ground click.
*/
bool SelectionInfo_TestAnyActiveOrSingleClass13(void)

{
  int entriesRemaining;
  int selectedEntryCount;
  GameEntityRuntime **selectionEntryCursor;
  bool selectedEntryIsClass13;
  int selectedDefinitionRecordAddress;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  selectedEntryCount = 0;
  selectedEntryIsClass13 = false;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != NULL) {
      selectedEntryCount++;
      selectedDefinitionRecordAddress =
           *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord;
      if (((ModelDefinition *)selectedDefinitionRecordAddress)->accelerationPerTick != 0) {
        return false;
      }
      selectedEntryIsClass13 = ((ModelDefinition *)selectedDefinitionRecordAddress)->runtimeClassId == MODEL_RUNTIME_CLASS_13;
    }
    selectionEntryCursor++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  if ((selectedEntryCount == 1) && (selectedEntryIsClass13)) {
    return false;
  }
  return true;
}


/* Address: 0x0052FF30.
   Tests whether the local selection could be ordered to a world point (CF = result of the test). The first
   entity whose definition has a non-zero dword +0x18 is temporarily moved to the point and asked through its
   typed callback; without such an entity the first class-0x0D entity tests the grid cell mask bands selected by
   its capability flags (0x80 -> band 3, 4 -> band 1, else 6). CF is set when neither exists.
*/
bool SelectionInfo_TestPositionCommandAtWorldPoint(Q12 worldXQ12,Q12 worldYQ12,WorldRuntimeContext *inGameRuntime)

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
  
  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  while ((selectedEntity = *selectionEntryCursor, selectedEntity == NULL ||
         (selectedModelNode = (selectedEntity->common).ownership.modelNode,
         ((ModelRuntimeSlot *)(selectedEntity->common).ownership.definitionOrClassRecord)->definitionOrSavedId.
         runtimeDefinition->accelerationPerTick == 0)))
  {
    selectionEntryCursor++;
    entriesRemaining--;
    if (entriesRemaining == 0) {
      entriesRemaining = SELECTION_ENTRY_CAPACITY;
      selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
      while ((*selectionEntryCursor == NULL ||
             (class13Definition = *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord,
             ((ModelDefinition *)class13Definition)->runtimeClassId != MODEL_RUNTIME_CLASS_13))) {
        selectionEntryCursor++;
        entriesRemaining--;
        if (entriesRemaining == 0) {
          return true;
        }
      }
      capabilityFlags = ((ModelDefinition *)class13Definition)->classParameterC4;
      highBandIndex = 3;
      if (((capabilityFlags & 0x80) == 0) && (highBandIndex = 1, (capabilityFlags & 4) == 0)) {
        highBandIndex = 6;
      }
      testResult = GridScratch_TestProjectedCellMaskBands(worldXQ12,worldYQ12,7,highBandIndex);
      return testResult;
    }
  }
  /* XCHG in the original; note that translation.x receives worldYQ12 and translation.y worldXQ12 */
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
   Returns false (CF clear) as soon as one entity of the local selection passes
   ArmyRuntime_TestWeaponDamageNonnegative but fails ArmyRuntime_TestHasNoWeaponDamage (its state value at
   +0x100 is positive); true when none does.
*/
bool SelectionInfo_TestNoEntryHasWeaponDamage(void)

{
  GameEntityRuntime *armyRuntime;
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  bool stateTestResult;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    armyRuntime = *selectionEntryCursor;
    if (armyRuntime != NULL) {
      stateTestResult = ArmyRuntime_TestWeaponDamageNonnegative((ArmyRuntimeSlot *)armyRuntime);
      if (stateTestResult) {
        stateTestResult = ArmyRuntime_TestHasNoWeaponDamage((ArmyRuntimeSlot *)armyRuntime);
        if (!stateTestResult) {
          return false;
        }
      }
    }
    selectionEntryCursor++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return true;
}


/* Address: 0x005300A0.
   Returns true (CF set) when ArmyRuntime_TestWeaponDamageNonnegative holds for any entity of the local
   selection, false otherwise.
*/
bool SelectionInfo_TestAnyEntryWeaponDamageNonnegative(void)

{
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  bool stateTestResult;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != NULL) {
      stateTestResult = ArmyRuntime_TestWeaponDamageNonnegative((ArmyRuntimeSlot *)*selectionEntryCursor);
      if (stateTestResult) {
        return true;
      }
    }
    selectionEntryCursor++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return false;
}


/* Address: 0x005300E0.
   Returns the first entity of the local player's selection (the first non-NULL entry), or NULL when nothing is
   selected; the in-game panels use it as the representative of the selection.
*/
GameEntityRuntime * __cdecl SelectionInfo_GetFirstEntry(void)

{
  GameEntityRuntime *firstEntry;
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  GameEntityRuntime **nextSelectionEntryCursor;
  bool currentEntryIsEmpty;

  /* REPE SCASD against 0 in the original */
  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  firstEntry = NULL;
  currentEntryIsEmpty = true;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    nextSelectionEntryCursor = selectionEntryCursor;
    if (entriesRemaining == 0) break;
    entriesRemaining--;
    nextSelectionEntryCursor = selectionEntryCursor + 1;
    currentEntryIsEmpty = *selectionEntryCursor == NULL;
    selectionEntryCursor = nextSelectionEntryCursor;
  } while (currentEntryIsEmpty);
  if (!currentEntryIsEmpty) {
    firstEntry = nextSelectionEntryCursor[-1];
  }
  return firstEntry;
}

/* Address: 0x00530100.
   Tests whether entry is part of the local selection: false (CF clear) when found, true when absent.
*/
bool SelectionInfo_FindEntry(GameEntityRuntime *entry)

{
  /* REPNE SCASD over the 32 selection slots in the original */
  int slotIndex;

  for (slotIndex = 0; slotIndex < SELECTION_ENTRY_CAPACITY; slotIndex++) {
    if (g_SelectionInfoEntitySlots->entries[slotIndex] == entry) {
      return false;
    }
  }
  return true;
}


/* Address: 0x00530770.
   Returns the OR of the attachment effect variant masks of all entities in the local selection (per entity from
   ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs).
*/
uint32_t SelectionInfo_CollectAttachmentEffectVariantMask(void)

{
  uint32_t effectVariantMask;
  int entriesRemaining;
  uint32_t entryVariantMask;
  GameEntityRuntime **selectionEntryCursor;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  effectVariantMask = 0;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != NULL) {
      entryVariantMask = ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs
                        (((*selectionEntryCursor)->common).ownership.definitionOrClassRecord);
      effectVariantMask = effectVariantMask | entryVariantMask;
    }
    selectionEntryCursor++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return effectVariantMask;
}


/* Address: 0x005307C0.
   Returns the OR of the capability flags of the local selection: definition class 0x16 contributes 8, class
   0x0D the capability dword +0xC4 of its definition; other classes contribute nothing.
*/
uint32_t __cdecl SelectionInfo_CollectCapabilityFlags(void)

{
  uint32_t capabilityMask;
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  int currentEntityDefinition;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  capabilityMask = 0;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != NULL) {
      currentEntityDefinition =
           *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord;
      if (((ModelDefinition *)currentEntityDefinition)->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
        capabilityMask = capabilityMask | 8;
      }
      else if (((ModelDefinition *)currentEntityDefinition)->runtimeClassId == MODEL_RUNTIME_CLASS_13) {
        capabilityMask = capabilityMask | ((ModelDefinition *)currentEntityDefinition)->classParameterC4;
      }
    }
    selectionEntryCursor++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return capabilityMask;
}

/* Address: 0x00561000.
   In-game command handler 0x1ED0: empties the player's marked-cell list (the field cells collected by
   PlayerPairList_InsertRange) and, for the local player, the in-game root's copy of its count (+0xBA4). Sent by
   InGameUiCommand_BeginInteractionByMode and InGameUiCommand_ResetInteractionByMode (ui/ingame/runtime.c).
*/
void SelectionPlayerRuntime_ClearTerrainEditSelectionState
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2)

{
  InGameRuntimeRoot *inGameRuntimeRoot;
  
  inGameRuntimeRoot = g_InGameRuntimeRoot;
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCellCount = 0;
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    inGameRuntimeRoot->localPlayerMarkedCellCount = 0;
  }
  return;
}


/* Address: 0x00571020.
   Tells whether the field cell (worldXQ12, worldYQ12) is in the player's marked-cell list (see
   PlayerPairList_InsertUnique): CF clear (false) when listed, CF set (true) when not. Used by the FieldGrid cell
   updates in world/terrain/grid.c.
*/
bool SelectionPlayerPairList_ContainsPair(SelectionPlayerPairValue worldYQ12,SelectionPlayerPairKey worldXQ12,
          PlayerRuntimeId playerRuntimeId)

{
  uint32_t pairRecordsRemaining;
  SelectionPlayerPairRecord *pairRecordCursor;

  pairRecordsRemaining = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCellCount;
  pairRecordCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCells;
  while( true ) {
    if (pairRecordsRemaining == 0) {
      return true;
    }
    if ((worldXQ12 == pairRecordCursor->pairKey) && (worldYQ12 == pairRecordCursor->pairValue)) break;
    pairRecordCursor++;
    pairRecordsRemaining--;
  }
  return false;
}


/* Address: 0x005302B0.
   Move command for a selection (ArmyRuntime_StartRoutedMoveCommand per entity): each entity is sent to
   the target shifted by its offset from the selection's centre, so the group keeps its formation, unless the
   selection is spread too widely, then all go to the target itself. If the selection is exactly one class-0xD
   entity (a production structure, cf. gameplay/faction/runtime.c), the target becomes its point at model
   runtime +0x78/+0x7C (flag 0x800 at +0xEC) and the selection is cleared.
*/
void SelectionPointerArray_ApplyMoveCommand
          (Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection)

{
  ArmyMovementRuntime *movementRuntime;
  void *class13Record;
  int entriesRemaining;
  Q12 entryTargetY;
  int selectedEntryCount;
  Q12 entryTargetX;
  int *singleClass13Entry;
  GameEntityRuntime **commandEntryCursor;
  GameEntityRuntime **selectionEntryCursor;
  bool spreadTooLarge;
  int entityDefinitionAddress;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  spreadTooLarge = SelectionPointerArray_IsSpatialSpreadTooLarge(selection);
  entryTargetY = targetWorldY;
  entryTargetX = targetWorldX;
  commandEntryCursor = selection->entries;
  do {
    movementRuntime = (ArmyMovementRuntime *)*commandEntryCursor;
    if (movementRuntime != NULL) {
      /* classState60/ownerValue64 are the entity's selection offsets (common +0x60/+0x64, centre - position)
         written by SelectionPointerArray_RecenterOffsetsAroundAveragePosition */
      if (!spreadTooLarge) {
        entryTargetX = entryTargetX - movementRuntime->classState60;
        entryTargetY = entryTargetY - movementRuntime->ownerValue64;
      }
      ArmyRuntime_StartRoutedMoveCommand(entryTargetY,entryTargetX,movementRuntime);
      if (!spreadTooLarge) {
        entryTargetX = entryTargetX + movementRuntime->classState60;
        entryTargetY = entryTargetY + movementRuntime->ownerValue64;
      }
    }
    commandEntryCursor = commandEntryCursor + 1;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  selectedEntryCount = 0;
  singleClass13Entry = NULL;
  selectionEntryCursor = selection->entries;
  do {
    if (*selectionEntryCursor != NULL) {
      selectedEntryCount++;
      entityDefinitionAddress =
           *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord;
      if (((ModelDefinition *)entityDefinitionAddress)->accelerationPerTick != 0) {
        return;
      }
      if (((ModelDefinition *)entityDefinitionAddress)->runtimeClassId == MODEL_RUNTIME_CLASS_13) {
        singleClass13Entry = (int *)*selectionEntryCursor;
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  if ((selectedEntryCount == 1) &&
     ((GameEntityRuntime *)singleClass13Entry != NULL)) {
    class13Record = (((GameEntityRuntime *)singleClass13Entry)->common).ownership.definitionOrClassRecord;
    ((ModelRuntimeSlot *)class13Record)->classLinkState.classState78 = targetWorldX;
    ((ModelRuntimeSlot *)class13Record)->classLinkState.classState7C = targetWorldY;
    ((ModelRuntimeSlot *)class13Record)->classState.stateFlags =
         ((ModelRuntimeSlot *)class13Record)->classState.stateFlags | ARMY_MODEL_STATE_RALLY_POINT_SET;
    SelectionPointerArray_Clear32(selection);
  }
}


/* Address: 0x00530420.
   Stops the selected entities: every entity without command flag 0x2 has its movement reset to its current
   model position and command-mode bit 0x10 and movement bit 0x200 cleared; class-0x16 entities are dropped
   from the selection. The formation offsets are then recomputed, and a selection of exactly one class-0xD
   entity gets its point at model runtime +0x78/+0x7C reset to its model's lookup point (1,5) (flag 0x800
   cleared) and the selection cleared.
*/
void SelectionRuntime_ResetMovementPruneAndRecenterEntries(GameEntityRuntime **selectionEntries)

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

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  selectionEntryCursor = selectionEntries;
  do {
    currentEntity = *selectionEntryCursor;
    if ((currentEntity != NULL) &&
       ((((ArmyRuntimeSlot *)currentEntity)->movementStateFlags & ARMY_MOVEMENT_LOCKED) == 0))
    {
      ArmyRuntime_ResetMovementStateFromModel((ArmyRuntimeSlot *)currentEntity);
      ((ArmyRuntimeSlot *)currentEntity)->commandModeFlags =
           ((ArmyRuntimeSlot *)currentEntity)->commandModeFlags & 0xffffffef;
      ((ArmyRuntimeSlot *)currentEntity)->movementStateFlags =
           ((ArmyRuntimeSlot *)currentEntity)->movementStateFlags & ~ARMY_MOVEMENT_ROUTED;
      entryModelRuntime = ((ArmyRuntimeSlot *)currentEntity)->modelRuntimeOrSavedOffset.modelRuntime;
      if ((entryModelRuntime->definitionOrSavedId).runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
        *selectionEntryCursor = NULL;
        (entryModelRuntime->classState).classStateDC = 0;
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  selectedEntryCount = 0;
  currentEntity = NULL;
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition
            ((SelectionPointerArray32 *)selectionEntries);
  selectionEntryCursor = selectionEntries;
  do {
    if (*selectionEntryCursor != NULL) {
      selectedEntryCount++;
      definitionAddress = *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord;
      if (((ModelDefinition *)definitionAddress)->accelerationPerTick != 0) {
        return;
      }
      if (((ModelDefinition *)definitionAddress)->runtimeClassId == MODEL_RUNTIME_CLASS_13) {
        currentEntity = *selectionEntryCursor;
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  if ((selectedEntryCount == 1) && (currentEntity != NULL)) {
    class13Record = (currentEntity->common).ownership.definitionOrClassRecord;
    modelNodeRuntime = (currentEntity->common).ownership.modelNode;
    ((ModelRuntimeSlot *)class13Record)->classState.stateFlags =
         ((ModelRuntimeSlot *)class13Record)->classState.stateFlags & ~ARMY_MODEL_STATE_RALLY_POINT_SET;
    lookupEntry = ModelLookupTable_ContainsPackedKey(1,5,(modelNodeRuntime->modelPayload).modelResource);
    if (!lookupEntry.notFound) {
      localPoint = ModelNodeRuntime_TransformLocalPointRegs(lookupEntry.entry,modelNodeRuntime);
      ((ModelRuntimeSlot *)class13Record)->classLinkState.classState78 = localPoint.xQ12;
      ((ModelRuntimeSlot *)class13Record)->classLinkState.classState7C = localPoint.yQ12;
      SelectionPointerArray_Clear32((SelectionPointerArray32 *)selectionEntries);
    }
  }
}


/* Address: 0x0052FCE0.
   Adds to a selection every entity in the world of the same army type (army asset id) and faction as
   sourceArmyRuntime, i.e. "select all units of this kind"; each insertion recomputes the formation offsets.
*/
void SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity
          (ArmyRuntimeSlot *sourceArmyRuntime,SelectionPointerArray32 *selection)

{
  PckArmyAssetIdCatalog sourceArmyAssetId;
  int sourceFactionIndex;
  GameEntityRuntime *entityRuntime;
  WorldRuntimeNode *worldNodeCursor;

  sourceArmyAssetId = sourceArmyRuntime->armyAssetId;
  sourceFactionIndex = sourceArmyRuntime->factionIndex;
  /* world owner list; worldNodeCursor[2].common.nextNode is the ownerClassId dword at +0xA4
     (0 = WORLD_OWNER_RUNTIME_MODEL) */
  for (worldNodeCursor = (WorldRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime).ownerListHead;
      worldNodeCursor != NULL;
      worldNodeCursor = (worldNodeCursor->common).nextNode) {
    if (((worldNodeCursor[2].common.nextNode == NULL) &&
        (entityRuntime = *(GameEntityRuntime **)((int)worldNodeCursor->runtimePayload + 8),
        sourceArmyAssetId == (entityRuntime->common).runtimeIdentityOrArmyAssetId)) &&
       (sourceFactionIndex == (entityRuntime->common).ownership.ownerIndex)) {
      SelectionPointerArray_InsertUniqueAndRecenter(entityRuntime,selection);
    }
  }
}


/* Address: 0x005303A0.
   Waypoint move for a selection (ArmyRuntime_AppendWaypointOrStartMove per entity): like
   SelectionPointerArray_ApplyMoveCommand each entity gets the target shifted by its formation
   offset, unless the selection is spread too widely, but without the class-0xD special case.
*/
void SelectionPointerArray_ApplyPositionCommand(Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection)

{
  ArmyMovementRuntime *movementRuntime;
  int entriesRemaining;
  bool spreadTooLarge;

  /* selection is advanced as a cursor over its entries; the targets are shifted per entry and restored */
  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  spreadTooLarge = SelectionPointerArray_IsSpatialSpreadTooLarge(selection);
  do {
    movementRuntime = *(ArmyMovementRuntime **)selection;
    if (movementRuntime != NULL) {
      if (!spreadTooLarge) {
        targetWorldX = targetWorldX - movementRuntime->classState60;
        targetWorldY = targetWorldY - movementRuntime->ownerValue64;
      }
      ArmyRuntime_AppendWaypointOrStartMove(targetWorldY,targetWorldX,movementRuntime);
      if (!spreadTooLarge) {
        targetWorldX = targetWorldX + movementRuntime->classState60;
        targetWorldY = targetWorldY + movementRuntime->ownerValue64;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining--;
  } while (entriesRemaining != 0);
}


/* Address: 0x0052D150.
   Draws a horizontal capped bar from spanStartCoordinate to spanEndCoordinate at row fixedCoordinate with a rich
   text label inside: start cap (base sprite), fill (+1), end cap (+2), text left/right border (+3/+5) and text
   background (+4). The label is placed at the start, the end or the centre (SELECTION_PANEL_CELL_FLAG_ALIGN_*)
   and left out when the span is too short. No caller was found in src/ or src/generated/image_data.c.
*/
void SelectionPanel_DrawHorizontalNumberTextCappedBar
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
  cellFlags = (uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                           SELECTION_PANEL_CELL_FLAGS);
  rowCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                             SELECTION_PANEL_CELL_OFFSET_Y);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                  SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
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
              (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,innerCursor,rowCoordinate,
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
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_START) == 0) {
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_END) == 0) {
        segmentCursor = ((int)(rightCapStart - requiredEnd) >> 1) + innerCursor;
        g_SelectionPanelBlitClipped
                  (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,segmentCursor,rowCoordinate,innerCursor,baseSubresource + 1,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,segmentCursor,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
        segmentCursor = segmentCursor + pieceSize.logicalWidthPixels;
        innerCursor = textWidth + segmentCursor;
        g_SelectionPanelBlitClipped
                  (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,innerCursor,rowCoordinate,segmentCursor,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        /* in this cell the X offset field shifts the text vertically */
        rowCoordinate = rowCoordinate + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                                 SELECTION_PANEL_CELL_OFFSET_X);
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,
                   commandStream,rowCoordinate,segmentCursor);
        rowCoordinate = rowCoordinate - *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                                 SELECTION_PANEL_CELL_OFFSET_X);
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,innerCursor,baseSubresource + 5,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 5,g_SelectionPanelTextureSource);
        g_SelectionPanelBlitClipped
                  (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,rightCapStart,rowCoordinate,
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
                  (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,rightCapStart,rowCoordinate,segmentCursor,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        rowCoordinate = rowCoordinate + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                                 SELECTION_PANEL_CELL_OFFSET_X);
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,
                   commandStream,rowCoordinate,segmentCursor);
        rowCoordinate = rowCoordinate - *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                                 SELECTION_PANEL_CELL_OFFSET_X);
        pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
        segmentCursor = segmentCursor - pieceSize.logicalWidthPixels;
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,segmentCursor,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        g_SelectionPanelBlitClipped
                  (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,segmentCursor,rowCoordinate,innerCursor,baseSubresource + 1,
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
                (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,segmentCursor,rowCoordinate,innerCursor,baseSubresource + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      rowCoordinate = rowCoordinate + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                               SELECTION_PANEL_CELL_OFFSET_X);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,commandStream,
                 rowCoordinate,innerCursor);
      rowCoordinate = rowCoordinate - *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                               SELECTION_PANEL_CELL_OFFSET_X);
      g_SelectionPanelBlitOpaque
                (clipTop,clipLeft,clipBottom,clipRight,rowCoordinate,segmentCursor,textRightBorderSubresource,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      pieceSize = g_GraphicsTextureSourceGetLogicalSize(textRightBorderSubresource,g_SelectionPanelTextureSource);
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,rightCapStart,rowCoordinate,
                 segmentCursor + pieceSize.logicalWidthPixels,baseSubresource + 1,g_SelectionPanelTextureSource,
                 g_FramebufferAccess);
    }
  }
  return;
}

/* Address: 0x0052D600.
   Draws a number cell: the cell's sprite at (originY, originX) plus the cell offsets, with value formatted as
   signed decimal text centred on it. Returns the coordinates after the cell (see SelectionPanelAdvanceEaxEdx8;
   SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_* keep an axis at the origin plus offset). Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the group number.
*/
SelectionPanelAdvanceEaxEdx8 SelectionPanel_DrawNumberCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate originY,UiPixelCoordinate originX,
          SelectionPanelNumericValue32 value,SelectionPanelCellIndex cellIndex)

{
  uint32_t cellFlags;
  void *panelData;
  uint32_t subresourceOrWidth;
  int cellY;
  uint32_t spriteHeight;
  int cellX;
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
  cellFlagsPtr = (uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                              SELECTION_PANEL_CELL_FLAGS);
  cellX = originX + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                             SELECTION_PANEL_CELL_OFFSET_X);
  cellY = originY + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                             SELECTION_PANEL_CELL_OFFSET_Y);
  subresourceOrWidth = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                     SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,cellY,cellX,subresourceOrWidth,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(subresourceOrWidth,g_SelectionPanelTextureSource);
  spriteHeight = spriteSize.logicalHeightPixels;
  subresourceOrWidth = spriteSize.logicalWidthPixels;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,
             (uint16_t *)&g_SelectionPanelNumberScratchUtf16,
             ((int)(spriteHeight - textExtent.heightPixels) >> 1) + cellY,
             ((int)(subresourceOrWidth - textExtent.widthPixels) >> 1) + cellX);
  cellFlags = *cellFlagsPtr;
  if ((cellFlags & SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_X) != 0) {
    subresourceOrWidth = 0;
  }
  if ((cellFlags & SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_Y) != 0) {
    spriteHeight = 0;
  }
  cellAdvance.nextX = subresourceOrWidth + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                                   SELECTION_PANEL_CELL_OFFSET_X) + originX;
  cellAdvance.nextY = spriteHeight + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                             SELECTION_PANEL_CELL_OFFSET_Y) + originY;
  return cellAdvance;
}


/* Address: 0x0052D6F0.
   Draws an icon cell: the cell's sprite at (originY, originX) plus the cell offsets. Returns the coordinates
   after the cell (see SelectionPanel_DrawNumberCellAndAdvanceRegs). Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the frame corners.
*/
SelectionPanelAdvanceEaxEdx8 SelectionPanel_DrawIconCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate originY,UiPixelCoordinate originX,
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
  cellFlagsPtr = (uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                              SELECTION_PANEL_CELL_FLAGS);
  subresourceOrWidth = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                     SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,
             originY + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                SELECTION_PANEL_CELL_OFFSET_Y),
             originX + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                SELECTION_PANEL_CELL_OFFSET_X),subresourceOrWidth,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(subresourceOrWidth,g_SelectionPanelTextureSource);
  spriteHeight = spriteSize.logicalHeightPixels;
  subresourceOrWidth = spriteSize.logicalWidthPixels;
  cellFlags = *cellFlagsPtr;
  if ((cellFlags & SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_X) != 0) {
    subresourceOrWidth = 0;
  }
  if ((cellFlags & SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_Y) != 0) {
    spriteHeight = 0;
  }
  cellAdvance.nextX = subresourceOrWidth + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                                   SELECTION_PANEL_CELL_OFFSET_X) + originX;
  cellAdvance.nextY = spriteHeight + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                             SELECTION_PANEL_CELL_OFFSET_Y) + originY;
  return cellAdvance;
}


/* Address: 0x0052D770.
   Draws a meter cell: the cell's base sprite at (originY, originX) plus the cell offsets and over it frame
   1..17 of the meter (currentValue clamped to 0..maximumValue, rounded to sixteenths; 17 when maximumValue is
   0). Returns the coordinates after the cell (see SelectionPanel_DrawNumberCellAndAdvanceRegs). Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the hierarchy meter.
*/
SelectionPanelAdvanceEaxEdx8 SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate originY,UiPixelCoordinate originX,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex)

{
  int baseSubresource;
  uint32_t cellFlags;
  void *panelData;
  int meterFrame;
  int cellX;
  uint32_t subresourceOrWidth;
  int cellY;
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
    meterFrame = 17;
  }
  else {
    /* round(16 * current / maximum) + 1 */
    meterFrame = ((uint32_t)(currentValue * 32 + maximumValue) / (uint32_t)maximumValue >> 1) + 1;
  }
  cellFlagsPtr = (uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                              SELECTION_PANEL_CELL_FLAGS);
  baseSubresource = *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                             SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  cellX = originX + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                             SELECTION_PANEL_CELL_OFFSET_X);
  cellY = originY + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                             SELECTION_PANEL_CELL_OFFSET_Y);
  subresourceOrWidth = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                     SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,cellY,cellX,subresourceOrWidth,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  g_SelectionPanelBlitOpaque
            (clipTop,clipLeft,clipBottom,clipRight,cellY,cellX,meterFrame + baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(subresourceOrWidth,g_SelectionPanelTextureSource);
  spriteHeight = spriteSize.logicalHeightPixels;
  subresourceOrWidth = spriteSize.logicalWidthPixels;
  cellFlags = *cellFlagsPtr;
  if ((cellFlags & SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_X) != 0) {
    subresourceOrWidth = 0;
  }
  if ((cellFlags & SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_Y) != 0) {
    spriteHeight = 0;
  }
  cellAdvance.nextX = subresourceOrWidth + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                                   SELECTION_PANEL_CELL_OFFSET_X) + originX;
  cellAdvance.nextY = spriteHeight + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                             SELECTION_PANEL_CELL_OFFSET_Y) + originY;
  return cellAdvance;
}


/* Address: 0x0052D850.
   Draws a horizontal value bar in row fixedCoordinate: start cap (base sprite) at barStartCoordinate, end cap
   (+2) ending at barEndCoordinate, and between them a filled part of rounded currentValue / maximumValue of the
   width (currentValue clamped to 0..maximumValue) in fill colour +3..+9 (by rounded sixths of the value, full
   when maximumValue is 0), the rest in the plain fill (+1). Called by SelectionPanel_RenderArmyRuntimeMetrics
   for the top and bottom edge.
*/
void SelectionPanel_DrawProportionalCappedBar
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
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_Y);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
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
  /* filled length and fill frame are rounded to the nearest integer (remainder * 2 > maximum rounds up) */
  if ((maximumValue != 0) &&
     (interiorSpan64 = (int64_t)filledSpan, filledSpan = (int)((interiorSpan64 * currentValue) / (int64_t)maximumValue),
     maximumValue < (int)((interiorSpan64 * currentValue) % (int64_t)maximumValue) * 2)) {
    filledSpan++;
  }
  if (maximumValue == 0) {
    fillFrame = 6;
  }
  else {
    fillFrame = (int)(((int64_t)currentValue * 6) / (int64_t)maximumValue);
    if (maximumValue < (int)(((int64_t)currentValue * 6) % (int64_t)maximumValue) * 2) {
      fillFrame++;
    }
  }
  g_SelectionPanelBlitClipped
            (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,filledSpan + interiorStart,fixedDrawCoordinate,interiorStart,
             fillFrame + 2 + baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,endCapCoordinate,fixedDrawCoordinate,filledSpan + interiorStart,baseSubresource + 1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  return;
}


/* Address: 0x0052D9A0.
   Vertical counterpart of SelectionPanel_DrawProportionalCappedBar: caps at barStartCoordinate and
   barEndCoordinate in column fixedCoordinate, the filled part grows upwards from the end cap. No caller was found
   in src/ or src/generated/image_data.c.
*/
void SelectionPanel_DrawVerticalProportionalCappedBar
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
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_X);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
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
  /* filled length and fill frame are rounded to the nearest integer (remainder * 2 > maximum rounds up) */
  if ((maximumValue != 0) &&
     (interiorSpan64 = (int64_t)filledSpan, filledSpan = (int)((interiorSpan64 * currentValue) / (int64_t)maximumValue),
     maximumValue < (int)((interiorSpan64 * currentValue) % (int64_t)maximumValue) * 2)) {
    filledSpan++;
  }
  if (maximumValue == 0) {
    fillFrame = 6;
  }
  else {
    fillFrame = (int)(((int64_t)currentValue * 6) / (int64_t)maximumValue);
    if (maximumValue < (int)(((int64_t)currentValue * 6) % (int64_t)maximumValue) * 2) {
      fillFrame++;
    }
  }
  g_SelectionPanelBlitClipped
            (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,endCapCoordinate - filledSpan,fixedDrawCoordinate,
             fillFrame + 2 + baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate - filledSpan,GRAPHICS_TILED_BLIT_ONE_TILE,interiorStart,fixedDrawCoordinate,baseSubresource + 1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  return;
}

/* Address: 0x0052DAF0.
   Draws a horizontal bar without a value in row fixedCoordinate: start cap (base sprite) at barStartCoordinate,
   end cap (+2) ending at barEndCoordinate and the plain fill (+1) between them. Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the top edge when there is no value to show.
*/
void SelectionPanel_DrawForwardCappedBar
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
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_Y);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
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
            (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,endCapCoordinate,fixedDrawCoordinate,
             barStartCoordinate + startCapSize.logicalWidthPixels,baseSubresource + 1,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  return;
}


/* Address: 0x0052DBC0.
   Vertical counterpart of SelectionPanel_DrawForwardCappedBar: start cap (base sprite) at barStartCoordinate,
   end cap (+2) ending at barEndCoordinate and the plain fill (+1) between them, in column fixedCoordinate.
   Called by SelectionPanel_RenderArmyRuntimeMetrics for the left and right edge.
*/
void SelectionPanel_DrawSolidCappedBar
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
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_X);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
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
            (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,
             barStartCoordinate + startCapSize.logicalHeightPixels,fixedDrawCoordinate,baseSubresource + 1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  return;
}


/* Address: 0x0052DC90.
   Horizontal counterpart of SelectionPanel_DrawSegmentedCappedBar: caps at spanStartCoordinate and
   spanEndCoordinate in row fixedCoordinate, filledSegmentCount full segments (+4), with
   SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS the rest of totalSegmentCount as empty segments (+3), aligned
   by SELECTION_PANEL_CELL_FLAG_ALIGN_*, and the plain fill (+1) around them; only the fill when the segments do
   not fit. No caller was found in src/ or src/generated/image_data.c.
*/
void SelectionPanel_DrawHorizontalSegmentedCappedBar
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
  
  cellFlags = (uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_FLAGS);
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_Y);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
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
  if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
    segmentsEnd = totalSegmentCount;
  }
  segmentsEnd = spriteSize.logicalWidthPixels * segmentsEnd + interiorStart;
  if (spanEndCoordinate < segmentsEnd) {
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,spanEndCoordinate,fixedDrawCoordinate,interiorStart,
               baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  else if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_START) == 0) {
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_END) == 0) {
      spanStartCoordinate = (spanEndCoordinate - segmentsEnd >> 1) + interiorStart;
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,spanStartCoordinate,fixedDrawCoordinate,interiorStart,
                 baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount--) {
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
        totalSegmentCount--;
      }
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount--) {
          g_SelectionPanelBlitOpaque
                    (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
          spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
        }
      }
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,spanEndCoordinate,fixedDrawCoordinate,
                 spanStartCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
    }
    else {
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount--) {
        spanEndCoordinate = spanEndCoordinate - spriteSize.logicalWidthPixels;
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanEndCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount--;
      }
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount--) {
          spanEndCoordinate = spanEndCoordinate - spriteSize.logicalWidthPixels;
          g_SelectionPanelBlitOpaque
                    (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanEndCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,spanEndCoordinate,fixedDrawCoordinate,interiorStart,
                 baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
    }
  }
  else {
    spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
    spanStartCoordinate = interiorStart;
    for (; filledSegmentCount != 0; filledSegmentCount--) {
      g_SelectionPanelBlitOpaque
                (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
      totalSegmentCount--;
    }
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
      for (; totalSegmentCount != 0; totalSegmentCount--) {
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
      }
    }
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,spanEndCoordinate,fixedDrawCoordinate,
               spanStartCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  return;
}

/* Address: 0x0052DFF0.
   Draws a vertical segment row in column fixedCoordinate: caps at barStartCoordinate and barEndCoordinate,
   filledSegmentCount full segments (+4) and, with SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS, the rest of
   totalSegmentCount as empty segments (+3), stacked from the top (ALIGN_START), from the bottom (ALIGN_END) or
   upwards from the centre, with the plain fill (+1) around them; only the fill when the segments do not fit.
   Called by SelectionPanel_RenderArmyRuntimeMetrics for the left and right edge of class-0x16 entities.
*/
void SelectionPanel_DrawSegmentedCappedBar
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
  
  cellFlags = (uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_FLAGS);
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_X);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
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
  if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
    segmentsEnd = totalSegmentCount;
  }
  segmentsEnd = spriteSize.logicalHeightPixels * segmentsEnd + barStartCoordinate;
  if (endCapCoordinate < segmentsEnd) {
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,barStartCoordinate,fixedDrawCoordinate,
               baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  else if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_START) == 0) {
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_END) == 0) {
      barEndCoordinate = endCapCoordinate - (endCapCoordinate - segmentsEnd >> 1);
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,barEndCoordinate,fixedDrawCoordinate,
                 baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount--) {
        barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount--;
      }
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount--) {
          barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
          g_SelectionPanelBlitOpaque
                    (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,
                 barStartCoordinate,fixedDrawCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess)
      ;
    }
    else {
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      barEndCoordinate = endCapCoordinate;
      for (; filledSegmentCount != 0; filledSegmentCount--) {
        barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount--;
      }
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount--) {
          barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
          g_SelectionPanelBlitOpaque
                    (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      g_SelectionPanelBlitClipped
                (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,
                 barStartCoordinate,fixedDrawCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess)
      ;
    }
  }
  else {
    spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
    for (; filledSegmentCount != 0; filledSegmentCount--) {
      g_SelectionPanelBlitOpaque
                (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      barStartCoordinate = barStartCoordinate + spriteSize.logicalHeightPixels;
      totalSegmentCount--;
    }
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
      for (; totalSegmentCount != 0; totalSegmentCount--) {
        g_SelectionPanelBlitOpaque
                  (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        barStartCoordinate = barStartCoordinate + spriteSize.logicalHeightPixels;
      }
    }
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,barStartCoordinate,fixedDrawCoordinate,
               baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  return;
}


/* Address: 0x00530130.
   Orders a selection onto a target entity: every entity with a non-zero state (+0x100) gets targetArmyRuntime as
   its command target (ArmyRuntime_ResolveCommandTarget), stored again at +0x98, command-mode bits 0x14 set and
   movement bit 0x200 cleared.
*/
void SelectionPointerArray_ApplyArmyRuntimeTarget(ArmyRuntimeSlot *targetArmyRuntime,SelectionPointerArray32 *selection)

{
  ArmyRuntimeSlot *runtimeState;
  int entriesRemaining;
  bool stateIsZero;

  /* selection is advanced as a cursor over its entries */
  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  do {
    runtimeState = *(ArmyRuntimeSlot **)selection;
    if (runtimeState != NULL) {
      stateIsZero = ArmyRuntime_TestHasNoWeaponDamage(runtimeState);
      if (!stateIsZero) {
        ArmyRuntime_ResolveCommandTarget(targetArmyRuntime,runtimeState);
        runtimeState->assignedTargetArmyRuntime = (uint32_t)targetArmyRuntime;
        runtimeState->commandModeFlags = runtimeState->commandModeFlags | 0x14;
        runtimeState->movementStateFlags = runtimeState->movementStateFlags & 0xfffffdff;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining--;
  } while (entriesRemaining != 0);
}


/* Address: 0x00530190.
   Orders a selection onto a target position: every entity with a non-zero state (+0x100) gets the three
   command coordinates (ArmyRuntime_ApplyTargetPositionCommand), command-mode bits 0x14 set, movement bit 0x200
   cleared and its command generation shifted left by 2.
*/
void SelectionPointerArray_ApplyTargetPositionCommand
          (Q12 coordinateA,uint32_t coordinateB,Q12 coordinateC,SelectionPointerArray32 *selection)

{
  ArmyRuntimeSlot *runtimeState;
  int entriesRemaining;
  bool stateIsZero;

  /* selection is advanced as a cursor over its entries */
  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  do {
    runtimeState = *(ArmyRuntimeSlot **)selection;
    if (runtimeState != NULL) {
      stateIsZero = ArmyRuntime_TestHasNoWeaponDamage(runtimeState);
      if (!stateIsZero) {
        ArmyRuntime_ApplyTargetPositionCommand(coordinateA,coordinateB,coordinateC,runtimeState);
        runtimeState->commandModeFlags = runtimeState->commandModeFlags | 0x14;
        runtimeState->movementStateFlags = runtimeState->movementStateFlags & 0xfffffdff;
        runtimeState->commandGeneration = runtimeState->commandGeneration << 2;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining--;
  } while (entriesRemaining != 0);
}


/* Address: 0x00530540.
   For every selected entity without command flag 0x2: resets its movement flags and anchor coordinates to the
   current model position (GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel) and clears command
   flag 0x200.
*/
void SelectionRuntime_StopMovement(GameEntityRuntime **selectionEntries)

{
  GameEntityCommandFlags *commandFlagsPtr;
  GameEntityRuntime *entityRuntime;
  int entriesRemaining;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  do {
    entityRuntime = *selectionEntries;
    if ((entityRuntime != NULL) &&
       (((entityRuntime->common).commandFlags & 2) == 0)) {
      GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(entityRuntime);
      commandFlagsPtr = &(entityRuntime->common).commandFlags;
      *commandFlagsPtr = *commandFlagsPtr & 0xfffffdff;
    }
    selectionEntries = selectionEntries + 1;
    entriesRemaining--;
  } while (entriesRemaining != 0);
}


/* Address: 0x005305A0.
   For every selected entity without command flag 0x2: drops an active attack/follow target
   (ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration) and clears command-mode bit 0x10.
*/
void SelectionRuntime_CancelTargets(GameEntityRuntime **selectionEntries)

{
  GameEntityRuntime *armyRuntime;
  int entriesRemaining;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  do {
    armyRuntime = *selectionEntries;
    if ((armyRuntime != NULL) &&
       ((((ArmyRuntimeSlot *)armyRuntime)->movementStateFlags & ARMY_MOVEMENT_LOCKED) == 0)) {
      ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration((ArmyRuntimeSlot *)armyRuntime);
      ((ArmyRuntimeSlot *)armyRuntime)->commandModeFlags =
           ((ArmyRuntimeSlot *)armyRuntime)->commandModeFlags & 0xffffffef;
    }
    selectionEntries = selectionEntries + 1;
    entriesRemaining--;
  } while (entriesRemaining != 0);
}


/* Address: 0x00530600.
   For every selected entity without command flag 0x2: sets runtime flags 0x418 on all nodes of its model
   hierarchy that do not have flag 0x08 yet (ModelRuntimeHierarchy_MarkDestroyedRecursive).
*/
void SelectionRuntime_SelfDestruct(GameEntityRuntime **selectionEntries)

{
  GameEntityRuntime *modelRuntime;
  int entriesRemaining;
  WorldRuntimeContext *contextArg;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  contextArg = &g_InGameRuntimeRoot->worldRuntime;
  do {
    modelRuntime = *selectionEntries;
    if ((modelRuntime != NULL) &&
       (((modelRuntime->common).commandFlags & 2) == 0)) {
      ModelRuntimeHierarchy_MarkDestroyedRecursive(contextArg,(int *)modelRuntime);
    }
    selectionEntries = selectionEntries + 1;
    entriesRemaining--;
  } while (entriesRemaining != 0);
}


/* Address: 0x0052FCA0.
   Adds an entity to a 32-entry selection (into the first free entry, unless it is already in it or the
   selection is full) and recomputes every entry's formation offset from the new centre.
*/
void SelectionPointerArray_InsertUniqueAndRecenter(GameEntityRuntime *entityRuntime,SelectionPointerArray32 *selection)

{
  int entryIndex;

  /* Two REPNE SCASD passes: look for entityRuntime, and when absent store it in the first null slot. */
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    if (selection->entries[entryIndex] == entityRuntime) break;
  }
  if (entryIndex == SELECTION_ENTRY_CAPACITY) {
    for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
      if (selection->entries[entryIndex] == NULL) {
        selection->entries[entryIndex] = entityRuntime;
        break;
      }
    }
  }
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition(selection);
}


/* Address: 0x0052FBF0.
   Computes the centre (average model world X/Y) of a selection and stores for every entity its offset
   centre - position in common.selectionOffsetXQ12/YQ12 (+0x60/+0x64); move orders subtract that offset from
   the target so the group keeps its formation.
*/
void SelectionPointerArray_RecenterOffsetsAroundAveragePosition(SelectionPointerArray32 *selection)

{
  int positionRecord;
  int averageXQ12;
  int selectedCountOrRemaining;
  int averageYQ12;
  int remainingOrEntryAddress;
  GameEntityRuntime **entryCursor;

  /* positionRecord is the entity's model node */
  averageXQ12 = 0;
  averageYQ12 = 0;
  selectedCountOrRemaining = 0;
  remainingOrEntryAddress = SELECTION_ENTRY_CAPACITY;
  entryCursor = selection->entries;
  do {
    if (*entryCursor != NULL) {
      positionRecord = (int)((*entryCursor)->common).ownership.modelNode;
      selectedCountOrRemaining++;
      averageXQ12 = averageXQ12 + ((ModelRuntimeNode *)positionRecord)->worldTransform.translation.x;
      averageYQ12 = averageYQ12 + ((ModelRuntimeNode *)positionRecord)->worldTransform.translation.y;
    }
    entryCursor = entryCursor + 1;
    remainingOrEntryAddress--;
  } while (remainingOrEntryAddress != 0);
  if (selectedCountOrRemaining != 0) {
    averageXQ12 = averageXQ12 / selectedCountOrRemaining;
    averageYQ12 = averageYQ12 / selectedCountOrRemaining;
    selectedCountOrRemaining = SELECTION_ENTRY_CAPACITY;
    do {
      remainingOrEntryAddress = *(int *)selection;
      if (remainingOrEntryAddress != 0) {
        positionRecord = (int)(((GameEntityRuntime *)remainingOrEntryAddress)->common).ownership.modelNode;
        averageXQ12 = averageXQ12 - ((ModelRuntimeNode *)positionRecord)->worldTransform.translation.x;
        averageYQ12 = averageYQ12 - ((ModelRuntimeNode *)positionRecord)->worldTransform.translation.y;
        (((GameEntityRuntime *)remainingOrEntryAddress)->common).selectionOffsetXQ12 = averageXQ12;
        (((GameEntityRuntime *)remainingOrEntryAddress)->common).selectionOffsetYQ12 = averageYQ12;
        averageXQ12 = averageXQ12 + ((ModelRuntimeNode *)positionRecord)->worldTransform.translation.x;
        averageYQ12 = averageYQ12 + ((ModelRuntimeNode *)positionRecord)->worldTransform.translation.y;
      }
      selection = (SelectionPointerArray32 *)((int)selection + 4);
      selectedCountOrRemaining--;
    } while (selectedCountOrRemaining != 0);
  }
}


/* Address: 0x0052FD90.
   Tells whether target is one of the 32 entries of a selection array: CF clear (false) when found, CF set (true)
   when not. Used by FrontendPlayerSelection_ApplyEntryOrAll (ui/frontend/player.c); the primary-selection
   move/rotate handlers call it and ignore the result.
*/
bool SelectionPointerArray_Contains(GameEntityRuntime *target,SelectionPointerArray32 *array)

{
  int entryIndex;

  /* REPNE SCASD over the 32 entries: CF clear when target was found. */
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    if (array->entries[entryIndex] == target) {
      return false;
    }
  }
  return true;
}


/* Address: 0x005301F0.
   Tells the move commands whether a selection is too scattered to keep its formation: true (CF) when the
   bounding box of the entities' selection offsets (common.selectionOffsetXQ12/YQ12, +0x60/+0x64) is wider
   than 5.0 (Q12 0x5000) on either axis or the two extents add up to more than 7.0 (0x7000). An empty
   selection returns false.
*/
bool SelectionPointerArray_IsSpatialSpreadTooLarge(SelectionPointerArray32 *selection)

{
  int entryAddress;
  int minOffsetX;
  int maxOffsetX;
  int maxOffsetY;
  int firstEntryOrMinOffsetY;
  int entriesRemaining;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  /* the first non-empty entry seeds the bounds */
  while (firstEntryOrMinOffsetY = *(int *)selection, firstEntryOrMinOffsetY == 0) {
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining--;
    if (entriesRemaining == 0) {
      return false;
    }
  }
  minOffsetX = ((GameEntityRuntime *)firstEntryOrMinOffsetY)->common.selectionOffsetXQ12;
  maxOffsetY = ((GameEntityRuntime *)firstEntryOrMinOffsetY)->common.selectionOffsetYQ12;
  maxOffsetX = minOffsetX;
  firstEntryOrMinOffsetY = maxOffsetY;
  do {
    entryAddress = *(int *)selection;
    if (entryAddress != 0) {
      if (((GameEntityRuntime *)entryAddress)->common.selectionOffsetXQ12 < minOffsetX) {
        minOffsetX = ((GameEntityRuntime *)entryAddress)->common.selectionOffsetXQ12;
      }
      if (((GameEntityRuntime *)entryAddress)->common.selectionOffsetYQ12 < firstEntryOrMinOffsetY) {
        firstEntryOrMinOffsetY = ((GameEntityRuntime *)entryAddress)->common.selectionOffsetYQ12;
      }
      if (maxOffsetX < ((GameEntityRuntime *)entryAddress)->common.selectionOffsetXQ12) {
        maxOffsetX = ((GameEntityRuntime *)entryAddress)->common.selectionOffsetXQ12;
      }
      if (maxOffsetY < ((GameEntityRuntime *)entryAddress)->common.selectionOffsetYQ12) {
        maxOffsetY = ((GameEntityRuntime *)entryAddress)->common.selectionOffsetYQ12;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining--;
  } while (entriesRemaining != 0);
  if (((maxOffsetX - minOffsetX < 0x5000 + 1) && (maxOffsetY - firstEntryOrMinOffsetY < 0x5000 + 1)) &&
     ((maxOffsetX - minOffsetX) + (maxOffsetY - firstEntryOrMinOffsetY) < 0x7000 + 1)) {
    return false;
  }
  return true;
}


/* Address: 0x00530650.
   For every selected entity whose definition class is 0x16, counts how often each of the three lane asset ids
   (g_ArmyLinkedChildAssetIdSlot0/1/2, i.e. g_InGamePointerModePreviewArmyIds[1], [2] and [4]) occurs among the
   13 child asset ids at model runtime +0x78..+0xA8. For every lane bit set in laneMask (1, 2, 4) it stores that
   lane's count byte (+0xDC + lane) and the point (worldYQ12, worldXQ12, heading16) at +0xB8 + lane * 0xC.
   Called by the pointer-mode handlers InGameSelection_SetAircraftPadTargetLane1/2 and
   SelectionMarkerCoordinates_ApplyType3..7.
*/
void SelectionPointerArray_SetAircraftPadTargets
          (SelectionMarkerLaneMask laneMask,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12,
          SelectionPointerArray32 *selection)

{
  ModelRuntimeLinkedChildSpawnAndBuildView *padRuntime;
  int markerSourceId;
  int entriesRemaining;
  int packedMarkerMatches;
  int markerSlotIndex;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  do {
    /* entry -> model runtime (dword 0) -> definition */
    if ((*(int **)selection != NULL) &&
       (padRuntime = (ModelRuntimeLinkedChildSpawnAndBuildView *)**(int **)selection,
       padRuntime->modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22)) {
      markerSlotIndex = 0xc;
      /* one match counter per byte: lane 1 in bits 0-7, lane 2 in bits 8-15, lane 4 in bits 16-23 */
      packedMarkerMatches = 0;
      do {
        markerSourceId = padRuntime->completedSecondaryArmyAssetIds[markerSlotIndex];
        if (markerSourceId == g_ArmyLinkedChildAssetIdSlot0) {
          packedMarkerMatches = packedMarkerMatches + 1;
        }
        if (markerSourceId == g_ArmyLinkedChildAssetIdSlot1) {
          packedMarkerMatches = packedMarkerMatches + 0x100;
        }
        if (markerSourceId == g_ArmyLinkedChildAssetIdSlot2) {
          packedMarkerMatches = packedMarkerMatches + 0x10000;
        }
        markerSlotIndex--;
      } while (-1 < markerSlotIndex);
      if ((laneMask & 1) != 0) {
        /* the lane's match count becomes its pending launch count; the point its launch target */
        padRuntime->linkedChildPendingSpawnCounts.slot0 = (char)packedMarkerMatches;
        padRuntime->linkedChildSpawnInheritedState[0].inheritedValue70 = worldYQ12;
        padRuntime->linkedChildSpawnInheritedState[0].inheritedValue74 = worldXQ12;
        padRuntime->linkedChildSpawnInheritedState[0].inheritedValue78 = heading16;
      }
      if ((laneMask & 2) != 0) {
        padRuntime->linkedChildPendingSpawnCounts.slot1 = (char)((uint32_t)packedMarkerMatches >> 8);
        padRuntime->linkedChildSpawnInheritedState[1].inheritedValue70 = worldYQ12;
        padRuntime->linkedChildSpawnInheritedState[1].inheritedValue74 = worldXQ12;
        padRuntime->linkedChildSpawnInheritedState[1].inheritedValue78 = heading16;
      }
      if ((laneMask & 4) != 0) {
        padRuntime->linkedChildPendingSpawnCounts.slot2 = (char)((uint32_t)packedMarkerMatches >> 16);
        padRuntime->linkedChildSpawnInheritedState[2].inheritedValue70 = worldYQ12;
        padRuntime->linkedChildSpawnInheritedState[2].inheritedValue74 = worldXQ12;
        padRuntime->linkedChildSpawnInheritedState[2].inheritedValue78 = heading16;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return;
}


/* Address: 0x0052FB00.
   Empties a 32-entry selection array (all entries NULL).
*/
void SelectionPointerArray_Clear32(SelectionPointerArray32 *array)

{
  int entriesRemaining;

  /* array is advanced as a cursor over its entries (REP STOSD in the original) */
  for (entriesRemaining = SELECTION_ENTRY_CAPACITY; entriesRemaining != 0; entriesRemaining--) {
    array->entries[0] = NULL;
    array = (SelectionPointerArray32 *)((int)array + 4);
  }
}

