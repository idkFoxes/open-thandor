/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/selection_panel_metrics.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/selection_panel_metrics.h>
#include <thandor/thandor.h>

/* Module data. */

/* Clip rectangle and screen bounds of one metric frame plus the advances of its four corner cells. */
typedef struct SelectionPanelMetricFrame {
  UiPixelCoordinate clipBottom;
  UiPixelCoordinate clipRight;
  UiPixelCoordinate clipTop;
  UiPixelCoordinate clipLeft;
  UiPixelCoordinate panelBottom;
  UiPixelCoordinate panelRight;
  UiPixelCoordinate panelTop;
  UiPixelCoordinate panelLeft;
  SelectionPanelCellAdvance topLeft;
  SelectionPanelCellAdvance topRight;
  SelectionPanelCellAdvance bottomLeft;
  SelectionPanelCellAdvance bottomRight;
} SelectionPanelMetricFrame;

/* Four plain corner cells (entities of other factions). */
static void SelectionPanelMetrics_DrawPlainCorners(SelectionPanelMetricFrame *frame)
{
  frame->topLeft = SelectionPanel_DrawIconCellAndAdvance
                     (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelTop,
                      frame->panelLeft,SELECTION_PANEL_CELL_CORNER_TOP_LEFT);
  frame->topRight = SelectionPanel_DrawIconCellAndAdvance
                      (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelTop,
                       frame->panelRight,SELECTION_PANEL_CELL_CORNER_TOP_RIGHT);
  frame->bottomLeft = SelectionPanel_DrawIconCellAndAdvance
                        (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelBottom,
                         frame->panelLeft,SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT);
  frame->bottomRight = SelectionPanel_DrawIconCellAndAdvance
                         (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelBottom,
                          frame->panelRight,SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT);
}

/* Corners of an own entity: the hierarchy energy meter top left (the plain corner instead when the hierarchy has
   no energy demand and alwaysDrawMeter is false), the group number top right (the plain corner when the entity is
   in no group) and the plain bottom corners. */
static void SelectionPanelMetrics_DrawOwnCorners
          (SelectionPanelMetricFrame *frame,RuntimeModelFactionPrefix *runtimeEntry,Bool8 alwaysDrawMeter)
{
  ModelHierarchyEnergyDemand energyDemand;
  uint32_t groupNumber;

  energyDemand = ModelRuntime_QueryHierarchyEnergyDemand(runtimeEntry);
  if (!alwaysDrawMeter && energyDemand.totalQ4 == 0) {
    frame->topLeft = SelectionPanel_DrawIconCellAndAdvance
                       (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelTop,
                        frame->panelLeft,SELECTION_PANEL_CELL_CORNER_TOP_LEFT);
  }
  else {
    frame->topLeft = SelectionPanel_DrawSteppedMeterCellAndAdvance
                       (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelTop,
                        frame->panelLeft,(UiNumericValue32)energyDemand.totalQ4,
                        (UiNumericValue32)energyDemand.activeQ4,SELECTION_PANEL_CELL_HIERARCHY_METER);
  }
  groupNumber = GameFactionRuntime_FindRuntimeGroupNumber(runtimeEntry);
  if (groupNumber == 0) {
    frame->topRight = SelectionPanel_DrawIconCellAndAdvance
                        (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelTop,
                         frame->panelRight,SELECTION_PANEL_CELL_CORNER_TOP_RIGHT);
  }
  else {
    frame->topRight = SelectionPanel_DrawNumberCellAndAdvance
                        (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelTop,
                         frame->panelRight,groupNumber,SELECTION_PANEL_CELL_GROUP_NUMBER);
  }
  frame->bottomLeft = SelectionPanel_DrawIconCellAndAdvance
                        (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelBottom,
                         frame->panelLeft,SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT);
  frame->bottomRight = SelectionPanel_DrawIconCellAndAdvance
                         (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelBottom,
                          frame->panelRight,SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT);
}

/* Top bar between the two top corners, filled to currentValue of maximumValue. */
static void SelectionPanelMetrics_DrawTopBar
          (SelectionPanelMetricFrame *frame,UiNumericValue32 maximumValue,UiNumericValue32 currentValue)
{
  SelectionPanel_DrawProportionalCappedBar
            (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelTop,
             frame->topRight.nextX,frame->topLeft.nextX,maximumValue,currentValue,SELECTION_PANEL_CELL_TOP_BAR);
}

/* Empty top bar between the two top corners. */
static void SelectionPanelMetrics_DrawEmptyTopBar(SelectionPanelMetricFrame *frame)
{
  SelectionPanel_DrawForwardCappedBar
            (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelTop,
             frame->topRight.nextX,frame->topLeft.nextX,SELECTION_PANEL_CELL_TOP_BAR_EMPTY);
}

/* Bottom bar between the two bottom corners: the hierarchy's condition ratio against Q12_ONE. */
static void SelectionPanelMetrics_DrawConditionBar
          (SelectionPanelMetricFrame *frame,RuntimeModelFactionPrefix *runtimeEntry)
{
  Q12 conditionRatioQ12;

  conditionRatioQ12 = ModelRuntime_QueryHierarchyConditionRatioQ12(runtimeEntry);
  SelectionPanel_DrawProportionalCappedBar
            (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->panelBottom,
             frame->bottomRight.nextX,frame->bottomLeft.nextX,(UiNumericValue32)Q12_ONE,
             (UiNumericValue32)conditionRatioQ12,SELECTION_PANEL_CELL_BOTTOM_BAR);
}

/* Plain bars on the left and right edge. */
static void SelectionPanelMetrics_DrawSolidSides(SelectionPanelMetricFrame *frame)
{
  SelectionPanel_DrawSolidCappedBar
            (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->bottomLeft.nextY,
             frame->topLeft.nextY,frame->panelLeft,SELECTION_PANEL_CELL_LEFT_BAR);
  SelectionPanel_DrawSolidCappedBar
            (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->bottomRight.nextY,
             frame->topRight.nextY,frame->panelRight,SELECTION_PANEL_CELL_RIGHT_BAR);
}

/* Segment rows on the left and right edge showing the linked child slot meter: the right row gets the rounded
   down half of the total and of the filled segments, the left row the rest. */
static void SelectionPanelMetrics_DrawSegmentedSides(SelectionPanelMetricFrame *frame,ModelRuntimeSlot *armyRuntime)
{
  ArmySegmentMeter slotMeter;
  uint32_t rightFilledSegments;
  uint32_t rightTotalSegments;

  slotMeter = ArmyRuntime_GetLinkedChildSlotMeter((ModelRuntimeLinkedChildSpawnAndBuildView *)armyRuntime);
  rightFilledSegments = slotMeter.filledSegments >> 1;
  rightTotalSegments = slotMeter.totalSegments >> 1;
  SelectionPanel_DrawSegmentedCappedBar
            (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->bottomLeft.nextY,
             frame->topLeft.nextY,frame->panelLeft,slotMeter.totalSegments - rightTotalSegments,
             slotMeter.filledSegments - rightFilledSegments,SELECTION_PANEL_CELL_LEFT_SEGMENTS);
  SelectionPanel_DrawSegmentedCappedBar
            (frame->clipBottom,frame->clipRight,frame->clipTop,frame->clipLeft,frame->bottomRight.nextY,
             frame->topRight.nextY,frame->panelRight,rightTotalSegments,rightFilledSegments,
             SELECTION_PANEL_CELL_RIGHT_SEGMENTS);
}

/* Corners, top and bottom bar of an own entity without kind-specific bars: the top bar shows the research
   progress while researching (or while research is unpaid), otherwise it stays empty. */
static void SelectionPanelMetrics_DrawResearchOrIdleBars
          (SelectionPanelMetricFrame *frame,RuntimeModelFactionPrefix *runtimeEntry,ModelRuntimeSlot *researchSource)
{
  Bool8 researching;

  researching = ((researchSource->classState).stateFlags &
                 (ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_RESEARCH_UNPAID)) != 0;
  SelectionPanelMetrics_DrawOwnCorners(frame,runtimeEntry,false);
  if (researching) {
    SelectionPanelMetrics_DrawTopBar
              (frame,researchSource->researchDurationTicks,researchSource->researchElapsedTicks);
  }
  else {
    SelectionPanelMetrics_DrawEmptyTopBar(frame);
  }
  SelectionPanelMetrics_DrawConditionBar(frame,runtimeEntry);
}

/* Kind 1: a factory (class 13) or production building (class 11) that is building shows its build progress.
   Returns false (nothing drawn) for every other kind-1 entity. */
static Bool8 SelectionPanelMetrics_DrawBuildProgressFrame
          (SelectionPanelMetricFrame *frame,RuntimeModelFactionPrefix *runtimeEntry,ModelRuntimeSlot *armyRuntime)
{
  if ((armyRuntime->classState).behaviorState != ARMY_FACTORY_STATE_BUILDING) {
    return false;
  }
  if ((armyRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_13) &&
      (armyRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_11)) {
    return false;
  }
  SelectionPanelMetrics_DrawOwnCorners(frame,runtimeEntry,true);
  SelectionPanelMetrics_DrawTopBar
            (frame,(UiNumericValue32)(armyRuntime->classLinkState).classState68,
             (UiNumericValue32)(armyRuntime->classLinkState).classState64);
  SelectionPanelMetrics_DrawConditionBar(frame,runtimeEntry);
  SelectionPanelMetrics_DrawSolidSides(frame);
  return true;
}

/* Kind 2: a weapon (first child of class 5-8) shows its reload countdown against the weapon definition's reload
   ticks. Returns false (nothing drawn) when there is no such child. */
static Bool8 SelectionPanelMetrics_DrawWeaponReloadFrame
          (SelectionPanelMetricFrame *frame,RuntimeModelFactionPrefix *runtimeEntry,ModelRuntimeSlot *armyRuntime)
{
  ModelRuntimeSlot *childModelRuntime;
  ModelDefinition *childDefinition;
  ModelRuntimeWeaponAimStateView *weapon;
  int reloadCountdownTicks;
  int reloadTicks;

  childModelRuntime = armyRuntime->attachments[0].childModelRuntimeOrSavedOffset;
  if ((armyRuntime->attachmentCount == 0) || (childModelRuntime == nullptr)) {
    return false;
  }
  childDefinition = (childModelRuntime->definitionOrSavedId).runtimeDefinition;
  if ((childDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_05) &&
      (childDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_06) &&
      (childDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_07) &&
      (childDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_08)) {
    return false;
  }
  weapon = (ModelRuntimeWeaponAimStateView *)childModelRuntime;
  reloadCountdownTicks = weapon->attachmentReloadCountdownTicks;
  reloadTicks = (int)weapon->modelDefinition->attachmentReloadTicks;
  SelectionPanelMetrics_DrawOwnCorners(frame,runtimeEntry,false);
  SelectionPanelMetrics_DrawTopBar(frame,reloadTicks,reloadTicks - reloadCountdownTicks);
  SelectionPanelMetrics_DrawConditionBar(frame,runtimeEntry);
  SelectionPanelMetrics_DrawSolidSides(frame);
  return true;
}

/* Kind 3: a class-9 first child (eight launch slots) shows the weapon definition's reload ticks minus the positive
   part of its smallest slot countdown (unsigned minimum of the eight, then at least classState80, signed).
   Returns false (nothing drawn) otherwise; *researchSource then names what the generic frame reads its research
   state from. */
static Bool8 SelectionPanelMetrics_DrawSlotReloadFrame
          (SelectionPanelMetricFrame *frame,RuntimeModelFactionPrefix *runtimeEntry,ModelRuntimeSlot *armyRuntime,
          ModelRuntimeSlot **researchSource)
{
  ModelRuntimeSlot *childModelRuntime;
  ArmyWeaponDefinitionView *childDefinition;
  ModelRuntimeWeaponAimStateView *launcher;
  uint32_t minimumReloadTicks;
  uint32_t remainingReloadTicks;
  uint32_t maximumValue;
  int slotIndex;

  childModelRuntime = armyRuntime->attachments[0].childModelRuntimeOrSavedOffset;
  if ((armyRuntime->attachmentCount == 0) || (childModelRuntime == nullptr)) {
    return false;
  }
  childDefinition = (ArmyWeaponDefinitionView *)(childModelRuntime->definitionOrSavedId).runtimeDefinition;
  /* Original quirk: when the child is not class 9, the generic frame reads the research flags and ticks at the
     child's definition instead of the entity's own runtime. */
  *researchSource = (ModelRuntimeSlot *)childDefinition;
  if (((ModelDefinition *)childDefinition)->runtimeClassId != MODEL_RUNTIME_CLASS_09) {
    return false;
  }
  launcher = (ModelRuntimeWeaponAimStateView *)childModelRuntime;
  minimumReloadTicks = UINT32_MAX;
  for (slotIndex = 7; slotIndex >= 0; slotIndex--) {
    if (launcher->attachmentReloadTicks[slotIndex] < minimumReloadTicks) {
      minimumReloadTicks = launcher->attachmentReloadTicks[slotIndex];
    }
  }
  if ((int)minimumReloadTicks < (int)(childModelRuntime->classLinkState).classState80) {
    minimumReloadTicks = (childModelRuntime->classLinkState).classState80;
  }
  remainingReloadTicks = 0;
  maximumValue = childDefinition->attachmentReloadTicks;
  if (0 < (int)minimumReloadTicks) {
    remainingReloadTicks = minimumReloadTicks;
  }
  SelectionPanelMetrics_DrawOwnCorners(frame,runtimeEntry,false);
  SelectionPanelMetrics_DrawTopBar
            (frame,(UiNumericValue32)maximumValue,(UiNumericValue32)(maximumValue - remainingReloadTicks));
  SelectionPanelMetrics_DrawConditionBar(frame,runtimeEntry);
  SelectionPanelMetrics_DrawSolidSides(frame);
  return true;
}

/* Class-22 entities of a kind above 3: segment rows for the linked child slots on the left and right edge; on top
   the class-state progress while the terrain contact advances, otherwise the research progress or the empty bar. */
static void SelectionPanelMetrics_DrawClass22Frame
          (SelectionPanelMetricFrame *frame,RuntimeModelFactionPrefix *runtimeEntry,ModelRuntimeSlot *armyRuntime)
{
  if (((ArmyRuntimeArticulatedContactState *)&(armyRuntime->classState).classStateAC)->
      terrainContactMode == ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE) {
    SelectionPanelMetrics_DrawOwnCorners(frame,runtimeEntry,true);
    SelectionPanelMetrics_DrawTopBar
              (frame,(UiNumericValue32)(armyRuntime->classLinkState).classState68,
               (UiNumericValue32)(armyRuntime->classLinkState).classState64);
    SelectionPanelMetrics_DrawConditionBar(frame,runtimeEntry);
  }
  else {
    SelectionPanelMetrics_DrawResearchOrIdleBars(frame,runtimeEntry,armyRuntime);
  }
  SelectionPanelMetrics_DrawSegmentedSides(frame,armyRuntime);
}

/* Kind-specific frame of an own entity (kinds 1-3, and class-22 entities of a higher kind). Returns false when
   nothing was drawn and the entity gets the generic frame, read from *researchSource. */
static Bool8 SelectionPanelMetrics_DrawKindFrame
          (SelectionPanelMetricFrame *frame,RuntimeModelFactionPrefix *runtimeEntry,uint32_t runtimeKind,
          ModelRuntimeSlot *armyRuntime,ModelRuntimeSlot **researchSource)
{
  if (runtimeKind == 1) {
    return SelectionPanelMetrics_DrawBuildProgressFrame(frame,runtimeEntry,armyRuntime);
  }
  if (runtimeKind == 2) {
    return SelectionPanelMetrics_DrawWeaponReloadFrame(frame,runtimeEntry,armyRuntime);
  }
  if (runtimeKind == 3) {
    return SelectionPanelMetrics_DrawSlotReloadFrame(frame,runtimeEntry,armyRuntime,researchSource);
  }
  if ((3 < runtimeKind) &&
      (armyRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22)) {
    SelectionPanelMetrics_DrawClass22Frame(frame,runtimeEntry,armyRuntime);
    return true;
  }
  return false;
}

/* Draws the metric frame of one selected entity around its projected screen bounds (panelTop..panelBottom,
   panelLeft..panelRight): four corner cells (hierarchy meter, group number), a bar along the top and bottom edge
   and bars or segment rows on the left and right edge, all from the SELECTION_PANEL_CELL_* layout. What the bars
   show depends on the entity kind (runtimeLinkOrKind08) and its definition class; entities of other factions
   only get the empty frame. Called by SelectionOverlay_RenderSelectedArmyMetrics,
   SelectionOverlay_RenderArmyMetricsForEntity (ui/ingame/selection_overlay.cpp) and
   UiArmyMetricsPanel_DrawTextureMetricsAndChildren (ui/controls/panels.cpp).
*/
void SelectionPanel_RenderArmyRuntimeMetrics
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate panelBottom,UiPixelCoordinate panelRight,
          UiPixelCoordinate panelTop,UiPixelCoordinate panelLeft,
          RuntimeModelFactionPrefix *runtimeEntry)

{
  InGameRuntimeRoot *inGameRoot;
  uint32_t runtimeKind;
  ModelRuntimeSlot *armyRuntime;
  ModelRuntimeSlot *researchSource;
  Bool8 framebufferBusy;
  SelectionPanelMetricFrame frame;

  inGameRoot = g_InGameRuntimeRoot;
  framebufferBusy = g_GraphicsFramebufferBeginAccess();
  if (framebufferBusy) {
    return;
  }
  runtimeKind = runtimeEntry->runtimeLinkOrKind08;
  armyRuntime = runtimeEntry->modelRuntime;
  frame.clipBottom = clipBottom;
  frame.clipRight = clipRight;
  frame.clipTop = clipTop;
  frame.clipLeft = clipLeft;
  frame.panelBottom = panelBottom;
  frame.panelRight = panelRight;
  frame.panelTop = panelTop;
  frame.panelLeft = panelLeft;
  if (runtimeEntry->factionIndex == (inGameRoot->worldRuntime).activeFactionRuntimeIndex) {
    /* kinds 1-3 and class-22 entities of a higher kind get special bars; every other case (including a special
       case whose condition fails) gets the generic frame */
    researchSource = armyRuntime;
    if (!SelectionPanelMetrics_DrawKindFrame(&frame,runtimeEntry,runtimeKind,armyRuntime,&researchSource)) {
      SelectionPanelMetrics_DrawResearchOrIdleBars(&frame,runtimeEntry,researchSource);
      SelectionPanelMetrics_DrawSolidSides(&frame);
    }
  }
  else {
    SelectionPanelMetrics_DrawPlainCorners(&frame);
    SelectionPanelMetrics_DrawEmptyTopBar(&frame);
    SelectionPanelMetrics_DrawConditionBar(&frame,runtimeEntry);
    SelectionPanelMetrics_DrawSolidSides(&frame);
  }
  g_GraphicsFramebufferEndAccess();
}
