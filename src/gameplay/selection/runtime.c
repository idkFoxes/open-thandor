/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/selection/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/selection/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/selection/runtime. */

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
          (SelectionPanelMetricFrame *frame,RuntimeModelFactionPrefix *runtimeEntry,bool alwaysDrawMeter)
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
  bool researching;

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
static bool SelectionPanelMetrics_DrawBuildProgressFrame
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
static bool SelectionPanelMetrics_DrawWeaponReloadFrame
          (SelectionPanelMetricFrame *frame,RuntimeModelFactionPrefix *runtimeEntry,ModelRuntimeSlot *armyRuntime)
{
  ModelRuntimeSlot *childModelRuntime;
  ModelDefinition *childDefinition;
  ModelRuntimeWeaponAimStateView *weapon;
  int reloadCountdownTicks;
  int reloadTicks;

  childModelRuntime = armyRuntime->attachments[0].childModelRuntimeOrSavedOffset;
  if ((armyRuntime->attachmentCount == 0) || (childModelRuntime == NULL)) {
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
static bool SelectionPanelMetrics_DrawSlotReloadFrame
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
  if ((armyRuntime->attachmentCount == 0) || (childModelRuntime == NULL)) {
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
static bool SelectionPanelMetrics_DrawKindFrame
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
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate panelBottom,UiPixelCoordinate panelRight,
          UiPixelCoordinate panelTop,UiPixelCoordinate panelLeft,
          RuntimeModelFactionPrefix *runtimeEntry)

{
  InGameRuntimeRoot *inGameRoot;
  uint32_t runtimeKind;
  ModelRuntimeSlot *armyRuntime;
  ModelRuntimeSlot *researchSource;
  bool framebufferBusy;
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
           MODEL_RUNTIME_CLASS_22) &&
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
   In-game command handler INGAME_COMMAND_MOVE (plain click on the ground): sends the player's
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
  ModelDefinition *definition;
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
    definition = *(ModelDefinition **)(target->common).ownership.definitionOrClassRecord;
    (target->common).pathCoordinate0Q12 = newWorldXQ12;
    (target->common).pathCoordinate1Q12 = newWorldYQ12;
    (target->common).trackedCoordinate0Q12 = newWorldXQ12;
    (target->common).trackedCoordinate1Q12 = newWorldYQ12;
    (target->common).damageState.trackedCoordinate0Q12 = newWorldXQ12;
    (target->common).damageState.trackedCoordinate1Q12 = newWorldYQ12;
    (modelNode->worldTransform).translation.x = newWorldXQ12;
    placementContactKind = definition->placementContactKindIndex;
    (modelNode->worldTransform).translation.y = newWorldYQ12;
    if (placementContactKind == ARMY_PLACEMENT_CONTACT_KIND_ARTICULATED_SUSPENSION) {
      ((modelNode->runtimePayload).armyRuntime)->movementTarget0Q12 = INT32_MAX;
    }
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (definition->placementHeightOffsetQ12,newWorldYQ12,newWorldXQ12,modelNode,worldRuntime);
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
    ModelNodeRuntime_UpdateDepthBinMasks(definition->footprintRadius,modelNode);
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
         angleDelta + (modelNodeRuntime->modelPayload).worldRotationAngle2 & FIXED_ANGLE16_MASK;
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  }
  return;
}


/* Empties the 32 selection entries of all eight player blocks. */
static void SelectionInfoPanel_ClearAllPlayerSelections(void)

{
  int blockIndex;
  int entryIndex;

  for (blockIndex = 0; blockIndex < 8; blockIndex++) {
    for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
      g_SelectionPlayerBlocks[blockIndex].selection.entries[entryIndex] = NULL;
    }
  }
}


/* The subresource entry table of a loaded texture source. */
static GraphicsTextureSourceEntry *SelectionInfoPanel_TextureEntries(GraphicsTextureSourceAsset *textureSource)

{
  return (GraphicsTextureSourceEntry *)
         ((uint8_t *)textureSource + (textureSource->tableDescriptor).subresourceTableOffset);
}


/* Stores a dword at byteOffset inside a texture-source record (records are not aligned). */
static void SelectionInfoPanel_SetRecordDword(uint8_t *record,int byteOffset,uint32_t value)

{
  *(uint32_t *)(record + byteOffset) = value;
}


/* Rewrites a two-dword info.gfx record to { 0, referencePayloadValue }. */
static void SelectionInfoPanel_PatchShortRecord(uint8_t *record,uint32_t referencePayloadValue)

{
  SelectionInfoPanel_SetRecordDword(record,0,0);
  SelectionInfoPanel_SetRecordDword(record,4,referencePayloadValue);
}


/* Rewrites an eight-dword info.gfx record to { first, second, 0xFFFF0000 x4, first, second }. */
static void SelectionInfoPanel_PatchLongRecord(uint8_t *record,uint32_t first,uint32_t second)

{
  int byteOffset;

  SelectionInfoPanel_SetRecordDword(record,0,first);
  SelectionInfoPanel_SetRecordDword(record,4,second);
  for (byteOffset = 8; byteOffset <= 20; byteOffset += 4) {
    SelectionInfoPanel_SetRecordDword(record,byteOffset,0xFFFF0000u);
  }
  SelectionInfoPanel_SetRecordDword(record,24,first);
  SelectionInfoPanel_SetRecordDword(record,28,second);
}


/* select.gfx: swaps the data offsets of subresources 45 and 46 (an XCHG in the original). */
static void SelectionInfoPanel_PatchSelectionTexture(GraphicsTextureSourceAsset *selectionTextureSource)

{
  GraphicsTextureSourceEntry *entries;
  AssetRelativeOffset swappedDataOffset;

  entries = SelectionInfoPanel_TextureEntries(selectionTextureSource);
  swappedDataOffset = entries[46].dataOffset;
  entries[46].dataOffset = entries[45].dataOffset;
  entries[45].dataOffset = swappedDataOffset;
}


/* info.gfx: rewrites the records of several sequences (the records at the dataOffset of subresources
   0x2D..0x33; their layout is not typed) and sets the heights of subresources 45, 46, 50 and 51 to 4.
   referencePayloadValue is the first dword of the record of subresource 0x2C. */
static void SelectionInfoPanel_PatchInfoTexture(GraphicsTextureSourceAsset *infoTextureSource)

{
  uint8_t *base;
  GraphicsTextureSourceEntry *entries;
  uint32_t referencePayloadValue;

  base = (uint8_t *)infoTextureSource;
  entries = SelectionInfoPanel_TextureEntries(infoTextureSource);
  referencePayloadValue = *(uint32_t *)(base + entries[44].dataOffset);
  SelectionInfoPanel_PatchShortRecord(base + entries[47].dataOffset,referencePayloadValue);
  SelectionInfoPanel_PatchShortRecord(base + entries[48].dataOffset,referencePayloadValue);
  SelectionInfoPanel_PatchShortRecord(base + entries[49].dataOffset,referencePayloadValue);
  entries[50].pixelHeight = 4;
  entries[50].logicalHeight = 4;
  entries[51].pixelHeight = 4;
  entries[51].logicalHeight = 4;
  entries[45].pixelHeight = 4;
  entries[45].logicalHeight = 4;
  entries[46].pixelHeight = 4;
  entries[46].logicalHeight = 4;
  SelectionInfoPanel_PatchLongRecord(base + entries[50].dataOffset,0,referencePayloadValue);
  SelectionInfoPanel_PatchLongRecord(base + entries[51].dataOffset,0,referencePayloadValue);
  SelectionInfoPanel_PatchLongRecord(base + entries[45].dataOffset,referencePayloadValue,0);
  SelectionInfoPanel_PatchLongRecord(base + entries[46].dataOffset,referencePayloadValue,0);
}


/* Address: 0x0052CEE0.
   Loads the selection and information panel graphics (gfx\panel\select.gfx, info.gfx) and their 0x1A4-byte .dat
   tables, empties the selection of all eight player blocks and stores the caller's entity-slot table. Then it
   patches sequence descriptors inside the loaded textures (swaps two select.gfx entries, rewrites frames of
   info.gfx) - the exact meaning of these patches is not known. Returns true on success; on failure returns false
   with the failing loader's error in *outError.
*/
bool SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots,uint32_t *outError)

{
  GraphicsTextureSourceAsset *selectionTextureSource;
  GraphicsTextureSourceAsset *infoTextureSource;
  GraphicsTextureSourceAsset *selectionPanelData;
  GraphicsTextureSourceAsset *infoPanelData;
  uint32_t loadErrorCode;

  selectionTextureSource =
       g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)u_gfx_panel_select_gfx_0052ce18,&loadErrorCode);
  if (selectionTextureSource == NULL) {
    *outError = loadErrorCode;
    return false;
  }
  g_SelectionPanelTextureSource = selectionTextureSource;
  infoTextureSource =
       g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)u_gfx_panel_info_gfx_0052ce42,&loadErrorCode);
  if (infoTextureSource == NULL) {
    *outError = loadErrorCode;
    return false;
  }
  g_InfoPanelTextureSource = infoTextureSource;
  selectionPanelData = Package_LoadEntry((uint16_t *)u_gfx_panel_select_dat_0052ce68,&loadErrorCode);
  if (selectionPanelData == NULL) {
    *outError = loadErrorCode;
    return false;
  }
  g_SelectionPanelData = selectionPanelData;
  infoPanelData = Package_LoadEntry((uint16_t *)u_gfx_panel_info_dat_0052ce92,&loadErrorCode);
  if (infoPanelData == NULL) {
    *outError = loadErrorCode;
    return false;
  }
  g_InfoPanelData = infoPanelData;
  SelectionInfoPanel_ClearAllPlayerSelections();
  g_SelectionInfoEntitySlots = entitySlots;
  SelectionInfoPanel_PatchSelectionTexture(g_SelectionPanelTextureSource);
  SelectionInfoPanel_PatchInfoTexture(g_InfoPanelTextureSource);
  return true;
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
   Writes the average world position (model node translation) of the local selection's entities to
   *outPosition and returns true; returns false when the selection is empty (*outPosition is then all 0).
*/
bool SelectionInfoEntitySlots_ComputeAverageWorldPosition(FixedVectorQ12 *outPosition)

{
  ModelRuntimeNode *slotModelNode;
  int worldXAggregateQ12;
  int worldYAggregateQ12;
  int worldZAggregateQ12;
  GameEntityRuntime **selectionEntitySlotCursor;
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
  outPosition->xQ12 = worldXAggregateQ12;
  outPosition->yQ12 = worldYAggregateQ12;
  outPosition->zQ12 = worldZAggregateQ12;
  return selectedEntityCount != 0;
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
  int entryIndex;
  int activeEntryCount;
  GameEntityRuntime *currentEntry;

  activeEntryCount = 0;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    currentEntry = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (currentEntry == NULL) {
      continue;
    }
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
  return activeEntryCount == 0;
}


/* Address: 0x0052FEB0.
   Returns false (CF clear) when the local selection can take a ground position order: some entity's
   definition has a non-zero dword +0x18, or the selection is a single entity of definition class 0x0D (13).
   True otherwise; the world input then ignores the ground click.
*/
bool SelectionInfo_TestAnyActiveOrSingleClass13(void)

{
  int entryIndex;
  int selectedEntryCount;
  GameEntityRuntime *selectedEntry;
  bool selectedEntryIsClass13;
  ModelDefinition *selectedDefinition;

  selectedEntryCount = 0;
  selectedEntryIsClass13 = false;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (selectedEntry == NULL) {
      continue;
    }
    selectedEntryCount++;
    selectedDefinition = *(ModelDefinition **)(selectedEntry->common).ownership.definitionOrClassRecord;
    if (selectedDefinition->accelerationPerTick != 0) {
      return false;
    }
    selectedEntryIsClass13 = selectedDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13;
  }
  if ((selectedEntryCount == 1) && (selectedEntryIsClass13)) {
    return false;
  }
  return true;
}


/* Fallback of SelectionInfo_TestPositionCommandAtWorldPoint: the first class-0x0D entity of the local selection
   tests the grid cell mask bands selected by its capability flags (0x80 -> band 3, 4 -> band 1, else 6);
   true when there is no such entity. */
static bool SelectionInfo_TestClass13CellBandsAtWorldPoint(Q12 worldXQ12,Q12 worldYQ12)

{
  GameEntityRuntime *selectedEntity;
  ModelDefinition *class13Definition;
  uint32_t capabilityFlags;
  uint8_t highBandIndex;
  int entryIndex;

  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntity = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (selectedEntity == NULL) {
      continue;
    }
    class13Definition = *(ModelDefinition **)(selectedEntity->common).ownership.definitionOrClassRecord;
    if (class13Definition->runtimeClassId != MODEL_RUNTIME_CLASS_13) {
      continue;
    }
    capabilityFlags = class13Definition->classParameterC4;
    if ((capabilityFlags & 0x80) != 0) {
      highBandIndex = 3;
    }
    else if ((capabilityFlags & 4) != 0) {
      highBandIndex = 1;
    }
    else {
      highBandIndex = 6;
    }
    return GridScratch_TestProjectedCellMaskBands(worldXQ12,worldYQ12,7,highBandIndex);
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
  GraphicsWorldCoordinateQ12 savedTranslationX;
  GraphicsWorldCoordinateQ12 savedTranslationY;
  ModelRuntimeNode *selectedModelNode;
  int entryIndex;
  bool testResult;
  GameEntityRuntime *selectedEntity;

  selectedEntity = NULL;
  selectedModelNode = NULL;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntity = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (selectedEntity == NULL) {
      continue;
    }
    selectedModelNode = (selectedEntity->common).ownership.modelNode;
    if (((ModelRuntimeSlot *)(selectedEntity->common).ownership.definitionOrClassRecord)->definitionOrSavedId.
        runtimeDefinition->accelerationPerTick != 0) {
      break;
    }
  }
  if (entryIndex == SELECTION_ENTRY_CAPACITY) {
    return SelectionInfo_TestClass13CellBandsAtWorldPoint(worldXQ12,worldYQ12);
  }
  /* swap the point in (an XCHG in the original); note that translation.x receives worldYQ12 and
     translation.y worldXQ12 */
  savedTranslationX = (selectedModelNode->worldTransform).translation.x;
  (selectedModelNode->worldTransform).translation.x = worldYQ12;
  savedTranslationY = (selectedModelNode->worldTransform).translation.y;
  (selectedModelNode->worldTransform).translation.y = worldXQ12;
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
   Tests whether entry is missing from the local selection: true (CF set) when absent, false (CF clear) when it is selected.
*/
bool SelectionInfo_IsEntryAbsent(GameEntityRuntime *entry)

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
   ArmyRuntime_GetAttachmentEffectVariantMask).
*/
uint32_t SelectionInfo_CollectAttachmentEffectVariantMask(void)

{
  uint32_t effectVariantMask;
  int entryIndex;
  GameEntityRuntime *selectedEntry;

  effectVariantMask = 0;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (selectedEntry != NULL) {
      effectVariantMask |=
           ArmyRuntime_GetAttachmentEffectVariantMask((selectedEntry->common).ownership.definitionOrClassRecord);
    }
  }
  return effectVariantMask;
}


/* Address: 0x005307C0.
   Returns the OR of the capability flags of the local selection: definition class 0x16 contributes 8, class
   0x0D the capability dword +0xC4 of its definition; other classes contribute nothing.
*/
uint32_t __cdecl SelectionInfo_CollectCapabilityFlags(void)

{
  uint32_t capabilityMask;
  int entryIndex;
  GameEntityRuntime *selectedEntry;
  ModelDefinition *entityDefinition;

  capabilityMask = 0;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (selectedEntry == NULL) {
      continue;
    }
    entityDefinition = *(ModelDefinition **)(selectedEntry->common).ownership.definitionOrClassRecord;
    if (entityDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
      capabilityMask = capabilityMask | 8;
    }
    else if (entityDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) {
      capabilityMask = capabilityMask | entityDefinition->classParameterC4;
    }
  }
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
  for (; pairRecordsRemaining != 0; pairRecordsRemaining--) {
    if ((worldXQ12 == pairRecordCursor->pairKey) && (worldYQ12 == pairRecordCursor->pairValue)) {
      return false;
    }
    pairRecordCursor++;
  }
  return true;
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
  ModelRuntimeSlot *class13Record;
  int entryIndex;
  Q12 entryTargetY;
  int selectedEntryCount;
  Q12 entryTargetX;
  GameEntityRuntime *selectedEntry;
  GameEntityRuntime *singleClass13Entry;
  bool spreadTooLarge;
  ModelDefinition *entityDefinition;

  spreadTooLarge = SelectionPointerArray_IsSpatialSpreadTooLarge(selection);
  entryTargetY = targetWorldY;
  entryTargetX = targetWorldX;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    movementRuntime = (ArmyMovementRuntime *)selection->entries[entryIndex];
    if (movementRuntime == NULL) {
      continue;
    }
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
  selectedEntryCount = 0;
  singleClass13Entry = NULL;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = selection->entries[entryIndex];
    if (selectedEntry == NULL) {
      continue;
    }
    selectedEntryCount++;
    entityDefinition = *(ModelDefinition **)(selectedEntry->common).ownership.definitionOrClassRecord;
    if (entityDefinition->accelerationPerTick != 0) {
      return;
    }
    if (entityDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) {
      singleClass13Entry = selectedEntry;
    }
  }
  if ((selectedEntryCount == 1) && (singleClass13Entry != NULL)) {
    class13Record = (ModelRuntimeSlot *)(singleClass13Entry->common).ownership.definitionOrClassRecord;
    class13Record->classLinkState.classState78 = targetWorldX;
    class13Record->classLinkState.classState7C = targetWorldY;
    class13Record->classState.stateFlags = class13Record->classState.stateFlags | ARMY_MODEL_STATE_RALLY_POINT_SET;
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
  ModelDefinition *entityDefinition;
  ModelRuntimeSlot *class13Record;
  ModelRuntimeNode *modelNodeRuntime;
  int entryIndex;
  int selectedEntryCount;
  ArmyRuntimeSlot *entryArmy;
  GameEntityRuntime *selectedEntry;
  GameEntityRuntime *singleClass13Entry;
  ModelPackedPointRecord *anchorRecord;
  ModelWorldPoint localPoint;

  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    entryArmy = (ArmyRuntimeSlot *)selectionEntries[entryIndex];
    if ((entryArmy == NULL) || ((entryArmy->movementStateFlags & ARMY_MOVEMENT_LOCKED) != 0)) {
      continue;
    }
    ArmyRuntime_ResetMovementStateFromModel(entryArmy);
    entryArmy->commandModeFlags = entryArmy->commandModeFlags & ~(uint32_t)ARMY_COMMAND_MODE_SELECTION_ORDER;
    entryArmy->movementStateFlags = entryArmy->movementStateFlags & ~ARMY_MOVEMENT_ROUTED;
    entryModelRuntime = entryArmy->modelRuntimeOrSavedOffset.modelRuntime;
    if ((entryModelRuntime->definitionOrSavedId).runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
      selectionEntries[entryIndex] = NULL;
      (entryModelRuntime->classState).classStateDC = 0;
    }
  }
  selectedEntryCount = 0;
  singleClass13Entry = NULL;
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition
            ((SelectionPointerArray32 *)selectionEntries);
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = selectionEntries[entryIndex];
    if (selectedEntry == NULL) {
      continue;
    }
    selectedEntryCount++;
    entityDefinition = *(ModelDefinition **)(selectedEntry->common).ownership.definitionOrClassRecord;
    if (entityDefinition->accelerationPerTick != 0) {
      return;
    }
    if (entityDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) {
      singleClass13Entry = selectedEntry;
    }
  }
  if ((selectedEntryCount == 1) && (singleClass13Entry != NULL)) {
    class13Record = (ModelRuntimeSlot *)(singleClass13Entry->common).ownership.definitionOrClassRecord;
    modelNodeRuntime = (singleClass13Entry->common).ownership.modelNode;
    class13Record->classState.stateFlags = class13Record->classState.stateFlags & ~ARMY_MODEL_STATE_RALLY_POINT_SET;
    if (ModelLookupTable_FindPackedPoint(1,5,(modelNodeRuntime->modelPayload).modelResource,&anchorRecord)) {
      localPoint = ModelNodeRuntime_TransformLocalPoint(anchorRecord,modelNodeRuntime);
      class13Record->classLinkState.classState78 = localPoint.xQ12;
      class13Record->classLinkState.classState7C = localPoint.yQ12;
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
  WorldOwnerListNode *ownerNode;

  sourceArmyAssetId = sourceArmyRuntime->armyAssetId;
  sourceFactionIndex = sourceArmyRuntime->factionIndex;
  for (ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead; ownerNode != NULL;
      ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    /* model payload: the ModelRuntimeSlot; its owner army is the entity */
    entityRuntime =
         (GameEntityRuntime *)((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    if ((sourceArmyAssetId == (entityRuntime->common).runtimeIdentityOrArmyAssetId) &&
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
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate fixedCoordinate,
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
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize pieceSize;
  GraphicsTextureLogicalSize textLeftBorderSize;
  GraphicsTextureLogicalSize textRightBorderSize;
  GraphicsTextureLogicalSize leftCapSize;
  
  textExtent = RichTextCommandStream_MeasureLine(g_SelectionPanelNumberTextStyle,commandStream);
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
              (clipBottom,clipRight,clipTop,clipLeft,rowCoordinate,spanStartCoordinate,baseSubresource,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
    innerCursor = spanEndCoordinate - pieceSize.logicalWidthPixels;
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,rowCoordinate,innerCursor,baseSubresource + 2,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,innerCursor,rowCoordinate,
               spanStartCoordinate + leftCapWidth,baseSubresource + 1,g_SelectionPanelTextureSource,
               g_FramebufferAccess);
  }
  else {
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,rowCoordinate,spanStartCoordinate,baseSubresource,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    innerCursor = spanStartCoordinate + leftCapWidth;
    pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
    rightCapStart = spanEndCoordinate - pieceSize.logicalWidthPixels;
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,rowCoordinate,rightCapStart,baseSubresource + 2,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_START) == 0) {
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_END) == 0) {
        segmentCursor = ((int)(rightCapStart - requiredEnd) >> 1) + innerCursor;
        g_SelectionPanelBlitClipped
                  (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,segmentCursor,rowCoordinate,innerCursor,baseSubresource + 1,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,rowCoordinate,segmentCursor,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
        segmentCursor = segmentCursor + pieceSize.logicalWidthPixels;
        innerCursor = textWidth + segmentCursor;
        g_SelectionPanelBlitClipped
                  (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,innerCursor,rowCoordinate,segmentCursor,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        /* in this cell the X offset field shifts the text vertically */
        rowCoordinate = rowCoordinate + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                                 SELECTION_PANEL_CELL_OFFSET_X);
        RichTextCommandStream_DrawSingleLine
                  (clipBottom,clipRight,clipTop,clipLeft,g_SelectionPanelNumberTextStyle,
                   commandStream,rowCoordinate,segmentCursor);
        rowCoordinate = rowCoordinate - *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                                 SELECTION_PANEL_CELL_OFFSET_X);
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,rowCoordinate,innerCursor,baseSubresource + 5,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 5,g_SelectionPanelTextureSource);
        g_SelectionPanelBlitClipped
                  (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,rightCapStart,rowCoordinate,
                   innerCursor + pieceSize.logicalWidthPixels,baseSubresource + 1,g_SelectionPanelTextureSource,
                   g_FramebufferAccess);
      }
      else {
        pieceSize = g_GraphicsTextureSourceGetLogicalSize(textRightBorderSubresource,g_SelectionPanelTextureSource);
        rightCapStart = rightCapStart - pieceSize.logicalWidthPixels;
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,rowCoordinate,rightCapStart,textRightBorderSubresource,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        segmentCursor = rightCapStart - textWidth;
        g_SelectionPanelBlitClipped
                  (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,rightCapStart,rowCoordinate,segmentCursor,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        rowCoordinate = rowCoordinate + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                                 SELECTION_PANEL_CELL_OFFSET_X);
        RichTextCommandStream_DrawSingleLine
                  (clipBottom,clipRight,clipTop,clipLeft,g_SelectionPanelNumberTextStyle,
                   commandStream,rowCoordinate,segmentCursor);
        rowCoordinate = rowCoordinate - *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                                 SELECTION_PANEL_CELL_OFFSET_X);
        pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
        segmentCursor = segmentCursor - pieceSize.logicalWidthPixels;
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,rowCoordinate,segmentCursor,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        g_SelectionPanelBlitClipped
                  (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,segmentCursor,rowCoordinate,innerCursor,baseSubresource + 1,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
      }
    }
    else {
      g_SelectionPanelBlitOpaque
                (clipBottom,clipRight,clipTop,clipLeft,rowCoordinate,innerCursor,baseSubresource + 3,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      pieceSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
      innerCursor = innerCursor + pieceSize.logicalWidthPixels;
      segmentCursor = textWidth + innerCursor;
      g_SelectionPanelBlitClipped
                (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,segmentCursor,rowCoordinate,innerCursor,baseSubresource + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      rowCoordinate = rowCoordinate + *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                               SELECTION_PANEL_CELL_OFFSET_X);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_SelectionPanelNumberTextStyle,commandStream,
                 rowCoordinate,innerCursor);
      rowCoordinate = rowCoordinate - *(int *)((int)panelData + cellIndex * SELECTION_PANEL_CELL_SIZE +
                                               SELECTION_PANEL_CELL_OFFSET_X);
      g_SelectionPanelBlitOpaque
                (clipBottom,clipRight,clipTop,clipLeft,rowCoordinate,segmentCursor,textRightBorderSubresource,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      pieceSize = g_GraphicsTextureSourceGetLogicalSize(textRightBorderSubresource,g_SelectionPanelTextureSource);
      g_SelectionPanelBlitClipped
                (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,rightCapStart,rowCoordinate,
                 segmentCursor + pieceSize.logicalWidthPixels,baseSubresource + 1,g_SelectionPanelTextureSource,
                 g_FramebufferAccess);
    }
  }
  return;
}

/* The SELECTION_PANEL_CELL_SIZE-byte record of cellIndex in select.dat (fields SELECTION_PANEL_CELL_*). */
static uint8_t *SelectionPanel_GetCellRecord(SelectionPanelCellIndex cellIndex)

{
  return (uint8_t *)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE;
}


/* Shared tail of the cell draw functions: the coordinates after a cell drawn at (cellY, cellX) with a sprite of
   spriteSize; SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_* keep an axis at the cell position. */
static SelectionPanelCellAdvance SelectionPanel_AdvancePastCell
          (uint8_t *cell,int cellY,int cellX,GraphicsTextureLogicalSize spriteSize)

{
  uint32_t cellFlags;
  uint32_t advanceWidth;
  uint32_t advanceHeight;
  SelectionPanelCellAdvance cellAdvance;

  advanceWidth = spriteSize.logicalWidthPixels;
  advanceHeight = spriteSize.logicalHeightPixels;
  cellFlags = *(uint32_t *)(cell + SELECTION_PANEL_CELL_FLAGS);
  if ((cellFlags & SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_X) != 0) {
    advanceWidth = 0;
  }
  if ((cellFlags & SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_Y) != 0) {
    advanceHeight = 0;
  }
  cellAdvance.nextX = advanceWidth + cellX;
  cellAdvance.nextY = advanceHeight + cellY;
  return cellAdvance;
}


/* Address: 0x0052D600.
   Draws a number cell: the cell's sprite at (originY, originX) plus the cell offsets, with value formatted as
   signed decimal text centred on it. Returns the coordinates after the cell (see SelectionPanelCellAdvance;
   SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_* keep an axis at the origin plus offset). Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the group number.
*/
SelectionPanelCellAdvance SelectionPanel_DrawNumberCellAndAdvance
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate originY,UiPixelCoordinate originX,
          SelectionPanelNumericValue32 value,SelectionPanelCellIndex cellIndex)

{
  uint8_t *cell;
  uint32_t subresource;
  int cellY;
  int cellX;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize spriteSize;

  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,15,1,value,
             g_SelectionPanelNumberScratchUtf16);
  textExtent = RichTextCommandStream_MeasureLine
                    (g_SelectionPanelNumberTextStyle,g_SelectionPanelNumberScratchUtf16);
  cell = SelectionPanel_GetCellRecord(cellIndex);
  cellX = originX + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_X);
  cellY = originY + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_Y);
  subresource = *(uint32_t *)(cell + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,cellY,cellX,subresource,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(subresource,g_SelectionPanelTextureSource);
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,g_SelectionPanelNumberTextStyle,
             g_SelectionPanelNumberScratchUtf16,
             ((int)(spriteSize.logicalHeightPixels - textExtent.heightPixels) >> 1) + cellY,
             ((int)(spriteSize.logicalWidthPixels - textExtent.widthPixels) >> 1) + cellX);
  return SelectionPanel_AdvancePastCell(cell,cellY,cellX,spriteSize);
}


/* Address: 0x0052D6F0.
   Draws an icon cell: the cell's sprite at (originY, originX) plus the cell offsets. Returns the coordinates
   after the cell (see SelectionPanel_DrawNumberCellAndAdvance). Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the frame corners.
*/
SelectionPanelCellAdvance SelectionPanel_DrawIconCellAndAdvance
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate originY,UiPixelCoordinate originX,
          SelectionPanelCellIndex cellIndex)

{
  uint8_t *cell;
  uint32_t subresource;
  int cellY;
  int cellX;
  GraphicsTextureLogicalSize spriteSize;

  cell = SelectionPanel_GetCellRecord(cellIndex);
  subresource = *(uint32_t *)(cell + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  cellY = originY + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_Y);
  cellX = originX + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_X);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,cellY,cellX,subresource,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(subresource,g_SelectionPanelTextureSource);
  return SelectionPanel_AdvancePastCell(cell,cellY,cellX,spriteSize);
}


/* Address: 0x0052D770.
   Draws a meter cell: the cell's base sprite at (originY, originX) plus the cell offsets and over it frame
   1..17 of the meter (currentValue clamped to 0..maximumValue, rounded to sixteenths; 17 when maximumValue is
   0). Returns the coordinates after the cell (see SelectionPanel_DrawNumberCellAndAdvance). Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the hierarchy meter.
*/
SelectionPanelCellAdvance SelectionPanel_DrawSteppedMeterCellAndAdvance
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate originY,UiPixelCoordinate originX,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex)

{
  uint8_t *cell;
  int baseSubresource;
  int meterFrame;
  int cellX;
  int cellY;
  GraphicsTextureLogicalSize spriteSize;

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
  cell = SelectionPanel_GetCellRecord(cellIndex);
  baseSubresource = *(int *)(cell + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  cellX = originX + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_X);
  cellY = originY + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_Y);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,cellY,cellX,baseSubresource,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,cellY,cellX,meterFrame + baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  return SelectionPanel_AdvancePastCell(cell,cellY,cellX,spriteSize);
}


/* numerator / maximumValue rounded to the nearest integer: the quotient is rounded up when twice the remainder
   (as a 32-bit int) exceeds maximumValue. */
static int SelectionPanel_DivideRounded(int64_t numerator,UiNumericValue32 maximumValue)

{
  int quotient;

  quotient = (int)(numerator / (int64_t)maximumValue);
  if (maximumValue < (int)(numerator % (int64_t)maximumValue) * 2) {
    quotient++;
  }
  return quotient;
}


/* Address: 0x0052D850.
   Draws a horizontal value bar in row fixedCoordinate: start cap (base sprite) at barStartCoordinate, end cap
   (+2) ending at barEndCoordinate, and between them a filled part of rounded currentValue / maximumValue of the
   width (currentValue clamped to 0..maximumValue) in fill colour +3..+9 (by rounded sixths of the value, full
   when maximumValue is 0), the rest in the plain fill (+1). Called by SelectionPanel_RenderArmyRuntimeMetrics
   for the top and bottom edge.
*/
void SelectionPanel_DrawProportionalCappedBar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate barEndCoordinate,UiPixelCoordinate barStartCoordinate,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex)

{
  int interiorStart;
  int endCapCoordinate;
  uint32_t baseSubresource;
  int filledSpan;
  int fillFrame;
  int fixedDrawCoordinate;
  uint8_t *cell;
  GraphicsTextureLogicalSize capSize;

  cell = SelectionPanel_GetCellRecord(cellIndex);
  fixedDrawCoordinate = fixedCoordinate + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_Y);
  baseSubresource = *(uint32_t *)(cell + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  capSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,barStartCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  interiorStart = barStartCoordinate + capSize.logicalWidthPixels;
  capSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - capSize.logicalWidthPixels;
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,endCapCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  filledSpan = endCapCoordinate - interiorStart;
  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  /* filled length and fill frame are rounded to the nearest integer (remainder * 2 > maximum rounds up);
     without a maximum the whole interior is filled with frame 6 */
  if (maximumValue == 0) {
    fillFrame = 6;
  }
  else {
    filledSpan = SelectionPanel_DivideRounded((int64_t)filledSpan * currentValue,maximumValue);
    fillFrame = SelectionPanel_DivideRounded((int64_t)currentValue * 6,maximumValue);
  }
  g_SelectionPanelBlitClipped
            (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,filledSpan + interiorStart,fixedDrawCoordinate,interiorStart,
             fillFrame + 2 + baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,endCapCoordinate,fixedDrawCoordinate,filledSpan + interiorStart,baseSubresource + 1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  return;
}


/* Address: 0x0052D9A0.
   Vertical counterpart of SelectionPanel_DrawProportionalCappedBar: caps at barStartCoordinate and
   barEndCoordinate in column fixedCoordinate, the filled part grows upwards from the end cap. No caller was found
   in src/ or src/generated/image_data.c.
*/
void SelectionPanel_DrawVerticalProportionalCappedBar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex)

{
  int interiorStart;
  int endCapCoordinate;
  uint32_t baseSubresource;
  int filledSpan;
  int fillFrame;
  int fixedDrawCoordinate;
  uint8_t *cell;
  GraphicsTextureLogicalSize capSize;

  cell = SelectionPanel_GetCellRecord(cellIndex);
  fixedDrawCoordinate = fixedCoordinate + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_X);
  baseSubresource = *(uint32_t *)(cell + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  capSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,barStartCoordinate,fixedDrawCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  interiorStart = barStartCoordinate + capSize.logicalHeightPixels;
  capSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - capSize.logicalHeightPixels;
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,fixedDrawCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  filledSpan = endCapCoordinate - interiorStart;
  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  /* filled length and fill frame are rounded to the nearest integer (remainder * 2 > maximum rounds up);
     without a maximum the whole interior is filled with frame 6 */
  if (maximumValue == 0) {
    fillFrame = 6;
  }
  else {
    filledSpan = SelectionPanel_DivideRounded((int64_t)filledSpan * currentValue,maximumValue);
    fillFrame = SelectionPanel_DivideRounded((int64_t)currentValue * 6,maximumValue);
  }
  g_SelectionPanelBlitClipped
            (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,endCapCoordinate - filledSpan,fixedDrawCoordinate,
             fillFrame + 2 + baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate - filledSpan,GRAPHICS_TILED_BLIT_ONE_TILE,interiorStart,fixedDrawCoordinate,baseSubresource + 1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  return;
}

/* Address: 0x0052DAF0.
   Draws a horizontal bar without a value in row fixedCoordinate: start cap (base sprite) at barStartCoordinate,
   end cap (+2) ending at barEndCoordinate and the plain fill (+1) between them. Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the top edge when there is no value to show.
*/
void SelectionPanel_DrawForwardCappedBar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate barEndCoordinate,UiPixelCoordinate barStartCoordinate,
          SelectionPanelCellIndex cellIndex)

{
  int endCapCoordinate;
  uint32_t baseSubresource;
  int fixedDrawCoordinate;
  GraphicsTextureLogicalSize startCapSize;
  GraphicsTextureLogicalSize endCapSize;
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_Y);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  startCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,barStartCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  endCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - endCapSize.logicalWidthPixels;
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,endCapCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,endCapCoordinate,fixedDrawCoordinate,
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
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          SelectionPanelCellIndex cellIndex)

{
  int endCapCoordinate;
  uint32_t baseSubresource;
  int fixedDrawCoordinate;
  GraphicsTextureLogicalSize startCapSize;
  GraphicsTextureLogicalSize endCapSize;
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_X);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  startCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,barStartCoordinate,fixedDrawCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  endCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - endCapSize.logicalHeightPixels;
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,fixedDrawCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,
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
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate spanEndCoordinate,UiPixelCoordinate spanStartCoordinate,
          SelectionPanelSegmentCount totalSegmentCount,SelectionPanelSegmentCount filledSegmentCount
          ,SelectionPanelCellIndex cellIndex)

{
  int interiorStart;
  uint32_t baseSubresource;
  int segmentsEnd;
  int fixedDrawCoordinate;
  uint32_t *cellFlags;
  GraphicsTextureLogicalSize spriteSize;
  
  cellFlags = (uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_FLAGS);
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_Y);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,spanStartCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  interiorStart = spanStartCoordinate + spriteSize.logicalWidthPixels;
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  spanEndCoordinate = spanEndCoordinate - spriteSize.logicalWidthPixels;
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,spanEndCoordinate,baseSubresource + 2,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
  segmentsEnd = filledSegmentCount;
  if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
    segmentsEnd = totalSegmentCount;
  }
  segmentsEnd = spriteSize.logicalWidthPixels * segmentsEnd + interiorStart;
  if (spanEndCoordinate < segmentsEnd) {
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,spanEndCoordinate,fixedDrawCoordinate,interiorStart,
               baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  else if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_START) == 0) {
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_END) == 0) {
      spanStartCoordinate = (spanEndCoordinate - segmentsEnd >> 1) + interiorStart;
      g_SelectionPanelBlitClipped
                (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,spanStartCoordinate,fixedDrawCoordinate,interiorStart,
                 baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount--) {
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
        totalSegmentCount--;
      }
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount--) {
          g_SelectionPanelBlitOpaque
                    (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
          spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
        }
      }
      g_SelectionPanelBlitClipped
                (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,spanEndCoordinate,fixedDrawCoordinate,
                 spanStartCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
    }
    else {
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount--) {
        spanEndCoordinate = spanEndCoordinate - spriteSize.logicalWidthPixels;
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,spanEndCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount--;
      }
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount--) {
          spanEndCoordinate = spanEndCoordinate - spriteSize.logicalWidthPixels;
          g_SelectionPanelBlitOpaque
                    (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,spanEndCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      g_SelectionPanelBlitClipped
                (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,spanEndCoordinate,fixedDrawCoordinate,interiorStart,
                 baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
    }
  }
  else {
    spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
    spanStartCoordinate = interiorStart;
    for (; filledSegmentCount != 0; filledSegmentCount--) {
      g_SelectionPanelBlitOpaque
                (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
      totalSegmentCount--;
    }
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
      for (; totalSegmentCount != 0; totalSegmentCount--) {
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,spanStartCoordinate,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        spanStartCoordinate = spanStartCoordinate + spriteSize.logicalWidthPixels;
      }
    }
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,spanEndCoordinate,fixedDrawCoordinate,
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
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          SelectionPanelSegmentCount totalSegmentCount,SelectionPanelSegmentCount filledSegmentCount
          ,SelectionPanelCellIndex cellIndex)

{
  int endCapCoordinate;
  uint32_t baseSubresource;
  int fixedDrawCoordinate;
  int segmentsEnd;
  uint32_t *cellFlags;
  GraphicsTextureLogicalSize spriteSize;
  
  cellFlags = (uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_FLAGS);
  fixedDrawCoordinate = fixedCoordinate + *(int *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_X);
  baseSubresource = *(uint32_t *)((int)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,barStartCoordinate,fixedDrawCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  barStartCoordinate = barStartCoordinate + spriteSize.logicalHeightPixels;
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,fixedDrawCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
  segmentsEnd = filledSegmentCount;
  if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
    segmentsEnd = totalSegmentCount;
  }
  segmentsEnd = spriteSize.logicalHeightPixels * segmentsEnd + barStartCoordinate;
  if (endCapCoordinate < segmentsEnd) {
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,barStartCoordinate,fixedDrawCoordinate,
               baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  else if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_START) == 0) {
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_END) == 0) {
      barEndCoordinate = endCapCoordinate - (endCapCoordinate - segmentsEnd >> 1);
      g_SelectionPanelBlitClipped
                (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,barEndCoordinate,fixedDrawCoordinate,
                 baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount--) {
        barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount--;
      }
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount--) {
          barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
          g_SelectionPanelBlitOpaque
                    (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      g_SelectionPanelBlitClipped
                (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,
                 barStartCoordinate,fixedDrawCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess)
      ;
    }
    else {
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      barEndCoordinate = endCapCoordinate;
      for (; filledSegmentCount != 0; filledSegmentCount--) {
        barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount--;
      }
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount--) {
          barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
          g_SelectionPanelBlitOpaque
                    (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      g_SelectionPanelBlitClipped
                (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,
                 barStartCoordinate,fixedDrawCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess)
      ;
    }
  }
  else {
    spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
    for (; filledSegmentCount != 0; filledSegmentCount--) {
      g_SelectionPanelBlitOpaque
                (clipBottom,clipRight,clipTop,clipLeft,barStartCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      barStartCoordinate = barStartCoordinate + spriteSize.logicalHeightPixels;
      totalSegmentCount--;
    }
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
      for (; totalSegmentCount != 0; totalSegmentCount--) {
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,barStartCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        barStartCoordinate = barStartCoordinate + spriteSize.logicalHeightPixels;
      }
    }
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,barStartCoordinate,fixedDrawCoordinate,
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
        runtimeState->commandModeFlags = runtimeState->commandModeFlags |
                                       (ARMY_COMMAND_MODE_SELECTION_ORDER | ARMY_COMMAND_MODE_INTERRUPTED);
        runtimeState->movementStateFlags = runtimeState->movementStateFlags & ~ARMY_MOVEMENT_ROUTED;
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
        runtimeState->commandModeFlags = runtimeState->commandModeFlags |
                                       (ARMY_COMMAND_MODE_SELECTION_ORDER | ARMY_COMMAND_MODE_INTERRUPTED);
        runtimeState->movementStateFlags = runtimeState->movementStateFlags & ~ARMY_MOVEMENT_ROUTED;
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
       (((entityRuntime->common).commandFlags & ARMY_MOVEMENT_LOCKED) == 0)) {
      GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(entityRuntime);
      commandFlagsPtr = &(entityRuntime->common).commandFlags;
      *commandFlagsPtr = *commandFlagsPtr & ~(uint32_t)ARMY_MOVEMENT_ROUTED;
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
           ((ArmyRuntimeSlot *)armyRuntime)->commandModeFlags & ~(uint32_t)ARMY_COMMAND_MODE_SELECTION_ORDER;
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
      ModelRuntimeHierarchy_MarkDestroyedRecursive(contextArg,(ArmyRuntimeSlot *)modelRuntime);
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
  ModelRuntimeNode *modelNode;
  GameEntityRuntime *entry;
  int averageXQ12;
  int averageYQ12;
  int selectedCount;
  int entryIndex;

  averageXQ12 = 0;
  averageYQ12 = 0;
  selectedCount = 0;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    entry = selection->entries[entryIndex];
    if (entry != NULL) {
      modelNode = (entry->common).ownership.modelNode;
      selectedCount++;
      averageXQ12 = averageXQ12 + modelNode->worldTransform.translation.x;
      averageYQ12 = averageYQ12 + modelNode->worldTransform.translation.y;
    }
  }
  if (selectedCount == 0) {
    return;
  }
  averageXQ12 = averageXQ12 / selectedCount;
  averageYQ12 = averageYQ12 / selectedCount;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    entry = selection->entries[entryIndex];
    if (entry != NULL) {
      modelNode = (entry->common).ownership.modelNode;
      (entry->common).selectionOffsetXQ12 = averageXQ12 - modelNode->worldTransform.translation.x;
      (entry->common).selectionOffsetYQ12 = averageYQ12 - modelNode->worldTransform.translation.y;
    }
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
  GameEntityRuntime *entry;
  int minOffsetX;
  int maxOffsetX;
  int minOffsetY;
  int maxOffsetY;
  int entryIndex;

  /* the first non-empty entry seeds the bounds */
  entryIndex = 0;
  while (selection->entries[entryIndex] == NULL) {
    entryIndex++;
    if (entryIndex == SELECTION_ENTRY_CAPACITY) {
      return false;
    }
  }
  entry = selection->entries[entryIndex];
  minOffsetX = entry->common.selectionOffsetXQ12;
  maxOffsetY = entry->common.selectionOffsetYQ12;
  maxOffsetX = minOffsetX;
  minOffsetY = maxOffsetY;
  for (; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    entry = selection->entries[entryIndex];
    if (entry == NULL) {
      continue;
    }
    if (entry->common.selectionOffsetXQ12 < minOffsetX) {
      minOffsetX = entry->common.selectionOffsetXQ12;
    }
    if (entry->common.selectionOffsetYQ12 < minOffsetY) {
      minOffsetY = entry->common.selectionOffsetYQ12;
    }
    if (maxOffsetX < entry->common.selectionOffsetXQ12) {
      maxOffsetX = entry->common.selectionOffsetXQ12;
    }
    if (maxOffsetY < entry->common.selectionOffsetYQ12) {
      maxOffsetY = entry->common.selectionOffsetYQ12;
    }
  }
  if (((maxOffsetX - minOffsetX < 5 * Q12_ONE + 1) && (maxOffsetY - minOffsetY < 5 * Q12_ONE + 1)) &&
     ((maxOffsetX - minOffsetX) + (maxOffsetY - minOffsetY) < 7 * Q12_ONE + 1)) {
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
  GameEntityRuntime *selectedEntry;
  int markerSourceId;
  int entryIndex;
  int packedMarkerMatches;
  int markerSlotIndex;

  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = selection->entries[entryIndex];
    if (selectedEntry == NULL) {
      continue;
    }
    /* entry -> model runtime (dword 0) -> definition */
    padRuntime = *(ModelRuntimeLinkedChildSpawnAndBuildView **)selectedEntry;
    if (padRuntime->modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
      /* one match counter per byte: lane 1 in bits 0-7, lane 2 in bits 8-15, lane 4 in bits 16-23 */
      packedMarkerMatches = 0;
      for (markerSlotIndex = 12; markerSlotIndex >= 0; markerSlotIndex--) {
        markerSourceId = padRuntime->completedSecondaryArmyAssetIds[markerSlotIndex];
        if (markerSourceId == g_ArmyLinkedChildAssetIdSlot0) {
          packedMarkerMatches = packedMarkerMatches + SELECTION_PACKED_LANE_ONE(0);
        }
        if (markerSourceId == g_ArmyLinkedChildAssetIdSlot1) {
          packedMarkerMatches = packedMarkerMatches + SELECTION_PACKED_LANE_ONE(1);
        }
        if (markerSourceId == g_ArmyLinkedChildAssetIdSlot2) {
          packedMarkerMatches = packedMarkerMatches + SELECTION_PACKED_LANE_ONE(2);
        }
      }
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
  }
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

