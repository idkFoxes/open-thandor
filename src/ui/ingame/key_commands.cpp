/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/key_commands.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/key_commands.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/ui/core/key_dispatch.h>

/* Module data. */

/* Actions of the in-game key command table (InGameUiRuntime_DispatchCommandByCodeAndModifierFlags). */
enum class InGameKeyCommandAction : uint32_t {
    RecallGroup = 1,              /* 1..8 */
    AddGroupToSelection = 2,      /* Shift+1..8 */
    StoreSelectionAsGroup = 3,    /* Ctrl+1..8, Alt+1..8 */
    AddSelectionToGroup = 4,      /* Ctrl+Shift+1..8, Alt+Shift+1..8 */
    NotificationAdvance = 5,      /* Space */
    NotificationCancel = 6,       /* Backspace */
    CameraToNotification = 7,     /* Alt+Space */
    CameraToSelection = 8,        /* Numpad 5 */
    CameraToClass11Model = 9,     /* B */
    SelectionResetMovement = 10,  /* S, Shift+Alt+S */
    SelectionStopMovement = 11,   /* Shift+S */
    SelectionCancelTargets = 12,  /* Alt+S */
    UpgradePageToggle = 13,       /* F */
    SelectOwnedClass16 = 14,      /* A */
    SelectionSelfDestruct = 15,   /* Alt+D */
    FreeCameraToggle = 16,        /* Alt+C */
    WrappedStatusTextToggle = 17, /* O */
    CheatOccupancyToggle = 18,    /* Ctrl+Alt+V */
    InfoTextNext = 19,            /* Ctrl+I; not in the original table, no original address */
};
static_assert(sizeof(UiKeyCommandRecord<InGameKeyCommandAction>) == 0xC, "a key command record keeps the original 12 bytes");

/* 63 key command records of the original, Ctrl+I (added, see InGameKeyCommandAction::InfoTextNext) and the terminator record
   (commandCode 0) that ends the dispatcher's scan */
static UiKeyCommandRecord<InGameKeyCommandAction> g_InGameCommandDispatchRecords[65] = {
    /*  0 */ {.commandCode = 0x30073, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::SelectionResetMovement},
    /*  1 */ {.commandCode = 0x30073, .modifierClassFlags = KEYBOARD_STATE_SHIFT, .action = InGameKeyCommandAction::SelectionStopMovement},
    /*  2 */ {.commandCode = 0x30073, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::SelectionCancelTargets},
    /*  3 */ {.commandCode = 0x30073, .action = InGameKeyCommandAction::SelectionResetMovement},
    /*  4 */ {.commandCode = 0x30062, .action = InGameKeyCommandAction::CameraToClass11Model},
    /*  5 */ {.commandCode = 0x30061, .action = InGameKeyCommandAction::SelectOwnedClass16},
    /*  6 */ {.commandCode = 0x30031, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /*  7 */ {.commandCode = 0x30032, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /*  8 */ {.commandCode = 0x30033, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /*  9 */ {.commandCode = 0x30034, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 10 */ {.commandCode = 0x30035, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 11 */ {.commandCode = 0x30036, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 12 */ {.commandCode = 0x30037, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 13 */ {.commandCode = 0x30038, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 14 */ {.commandCode = 0x30031, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 15 */ {.commandCode = 0x30032, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 16 */ {.commandCode = 0x30033, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 17 */ {.commandCode = 0x30034, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 18 */ {.commandCode = 0x30035, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 19 */ {.commandCode = 0x30036, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 20 */ {.commandCode = 0x30037, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 21 */ {.commandCode = 0x30038, .modifierClassFlags = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::AddSelectionToGroup},
    /* 22 */ {.commandCode = 0x30031, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 23 */ {.commandCode = 0x30032, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 24 */ {.commandCode = 0x30033, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 25 */ {.commandCode = 0x30034, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 26 */ {.commandCode = 0x30035, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 27 */ {.commandCode = 0x30036, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 28 */ {.commandCode = 0x30037, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 29 */ {.commandCode = 0x30038, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 30 */ {.commandCode = 0x30031, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 31 */ {.commandCode = 0x30032, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 32 */ {.commandCode = 0x30033, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 33 */ {.commandCode = 0x30034, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 34 */ {.commandCode = 0x30035, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 35 */ {.commandCode = 0x30036, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 36 */ {.commandCode = 0x30037, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 37 */ {.commandCode = 0x30038, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::StoreSelectionAsGroup},
    /* 38 */ {.commandCode = 0x30031, .modifierClassFlags = KEYBOARD_STATE_SHIFT, .action = InGameKeyCommandAction::AddGroupToSelection},
    /* 39 */ {.commandCode = 0x30032, .modifierClassFlags = KEYBOARD_STATE_SHIFT, .action = InGameKeyCommandAction::AddGroupToSelection},
    /* 40 */ {.commandCode = 0x30033, .modifierClassFlags = KEYBOARD_STATE_SHIFT, .action = InGameKeyCommandAction::AddGroupToSelection},
    /* 41 */ {.commandCode = 0x30034, .modifierClassFlags = KEYBOARD_STATE_SHIFT, .action = InGameKeyCommandAction::AddGroupToSelection},
    /* 42 */ {.commandCode = 0x30035, .modifierClassFlags = KEYBOARD_STATE_SHIFT, .action = InGameKeyCommandAction::AddGroupToSelection},
    /* 43 */ {.commandCode = 0x30036, .modifierClassFlags = KEYBOARD_STATE_SHIFT, .action = InGameKeyCommandAction::AddGroupToSelection},
    /* 44 */ {.commandCode = 0x30037, .modifierClassFlags = KEYBOARD_STATE_SHIFT, .action = InGameKeyCommandAction::AddGroupToSelection},
    /* 45 */ {.commandCode = 0x30038, .modifierClassFlags = KEYBOARD_STATE_SHIFT, .action = InGameKeyCommandAction::AddGroupToSelection},
    /* 46 */ {.commandCode = 0x30031, .action = InGameKeyCommandAction::RecallGroup},
    /* 47 */ {.commandCode = 0x30032, .action = InGameKeyCommandAction::RecallGroup},
    /* 48 */ {.commandCode = 0x30033, .action = InGameKeyCommandAction::RecallGroup},
    /* 49 */ {.commandCode = 0x30034, .action = InGameKeyCommandAction::RecallGroup},
    /* 50 */ {.commandCode = 0x30035, .action = InGameKeyCommandAction::RecallGroup},
    /* 51 */ {.commandCode = 0x30036, .action = InGameKeyCommandAction::RecallGroup},
    /* 52 */ {.commandCode = 0x30037, .action = InGameKeyCommandAction::RecallGroup},
    /* 53 */ {.commandCode = 0x30038, .action = InGameKeyCommandAction::RecallGroup},
    /* 54 */ {.commandCode = 0x20, .action = InGameKeyCommandAction::NotificationAdvance},
    /* 55 */ {.commandCode = 0x20, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::CameraToNotification},
    /* 56 */ {.commandCode = 0x10003, .action = InGameKeyCommandAction::NotificationCancel},
    /* 57 */ {.commandCode = 0x30066, .action = InGameKeyCommandAction::UpgradePageToggle},
    /* 58 */ {.commandCode = 0x3006F, .action = InGameKeyCommandAction::WrappedStatusTextToggle},
    /* 59 */ {.commandCode = 0x30076, .modifierClassFlags = KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::CheatOccupancyToggle},
    /* 60 */ {.commandCode = 0x30063, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::FreeCameraToggle},
    /* 61 */ {.commandCode = 0x10015, .action = InGameKeyCommandAction::CameraToSelection},
    /* 62 */ {.commandCode = 0x30064, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameKeyCommandAction::SelectionSelfDestruct},
    /* 63 */ {.commandCode = 0x30069, .modifierClassFlags = KEYBOARD_STATE_CTRL, .action = InGameKeyCommandAction::InfoTextNext},
    /* 64 */ {.commandCode = 0x0}}; /* terminator: commandCode 0 (the original's other two dwords were NOP fill, never read) */

UiCommandRuntimeFlagMask g_UiCommandRuntimeFlags = UiCommandRuntimeFlagMask{};


/* In-game key commands (the world view's dispatchCommandCallback): the first record of
   g_InGameCommandDispatchRecords whose key code matches and whose modifier class
   (Shift / Ctrl / Alt, left or right) equals the held modifiers selects the command. Selection commands are
   ignored while the game is paused or the world input is disabled; network games queue them as player
   commands (code in brackets) instead of executing them.
     1..8                   recall selection group n (transfer mode 0)                 [0xBE0]
     Shift+1..8             add group n to the selection (mode 2)                      [0xBE0]
     Ctrl+1..8, Alt+1..8    store the selection as group n (mode 1)                    [0xBE0]
     Ctrl/Alt+Shift+1..8    add the selection to group n (mode 3)                      [0xBE0]
     S, Shift+Alt+S         stop: reset the selection's movement                       [0xE10]
     Shift+S                reset the selection's movement anchors                     [0xE30]
     Alt+S                  interrupt the selection's active targets                   [0xE50]
     Alt+D                  apply model hierarchy flags 0x418 to the selection         [0xE70]
     A                      InGameSelection_SelectAllOwnAircraftPads               [0x8F0]
     B                      camera to the first own model of runtime class 11
     Space / Backspace      advance/resolve or cancel the notification target (as the notification button)
     Alt+Space              camera to the last notification target position
     Numpad 5               camera to the centre of the selection
     F                      toggle the single-selection upgrade page (as its button)
     O                      show/hide the wrapped world-view status text
     Alt+C                  toggle the free camera (no pitch and distance clamps)
     Ctrl+Alt+V             cheat, single player only: toggle occupancy bit 0 on every field cell
     Ctrl+I                 next world view info text (InGameWorldView_ShowNextInfoText); added - the original
                            has this key only in the map editor's keyboard handler
   Returns true when no record matches: the field is the keyboardFallback of the world view's
   pointer context (FrontendModelPointerContext_KeyboardEvent), which then passes the key on, so keys such as Esc
   reach the in-game root's hotkeys. A matched record returns false, also when the command is blocked.
*/
Bool8 InGameUiRuntime_DispatchCommandByCodeAndModifierFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          WorldRuntimeContext *world)

{
  /* Each dispatch record names its action, one case of the switch below. world is the world view (worldRuntime). */
  UiKeyCommandRecord<InGameKeyCommandAction> *record; /* g_InGameCommandDispatchRecords ends at the terminator record [63] */
  InGameKeyCommandAction target;
  Bool8 localSession =
       (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL;
  Bool8 commandsBlocked =
       Any(g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED));

  record = UiCommandDispatch_Find(g_InGameCommandDispatchRecords,commandCode,modifierFlags,
                                  UiKeyModifierRule::ExactWithShift);
  if (record == nullptr) {
    return true; /* not a world view key: the pointer context passes it on (Esc reaches the root's hotkeys) */
  }
  target = record->action;
  switch (target) {
  case InGameKeyCommandAction::RecallGroup:
  case InGameKeyCommandAction::AddGroupToSelection:
  case InGameKeyCommandAction::StoreSelectionAsGroup:
  case InGameKeyCommandAction::AddSelectionToGroup: {
    /* FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh mode bits: SELECTION_TRANSFER_TO_GROUP = the
       selection is the source and the group the destination, SELECTION_TRANSFER_MERGE = merge instead of replace */
    FrontendSelectionTransferModeFlags transferMode =
        (target == InGameKeyCommandAction::RecallGroup) ? FrontendSelectionTransferModeFlags{} :
        (target == InGameKeyCommandAction::AddGroupToSelection) ? SELECTION_TRANSFER_MERGE :
        (target == InGameKeyCommandAction::StoreSelectionAsGroup) ? SELECTION_TRANSFER_TO_GROUP :
                                                                    SELECTION_TRANSFER_TO_GROUP | SELECTION_TRANSFER_MERGE;
    uint32_t groupIndex = commandCode - KEYBOARD_KEY_CODE_CHAR('1');
    if (commandsBlocked) {
      break;
    }
    InGameCommand_Issue<FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh>
              (world->activeFactionRuntimeIndex,ToBits(transferMode),groupIndex);
    break;
  }
  case InGameKeyCommandAction::NotificationAdvance:
    InGameTargetingContext_AdvanceOrResolveTarget
              (reinterpret_cast<InGameTargetingRootTraversalView *>(&THANDOR_CONTAINER_OF(world, InGameUiImage, worldView)->notificationTargetButton));
    break;
  case InGameKeyCommandAction::NotificationCancel:
    InGameTargetingContext_CancelAndRestoreState
              (reinterpret_cast<InGameTargetingRootTraversalView *>(&THANDOR_CONTAINER_OF(world, InGameUiImage, worldView)->notificationTargetButton));
    break;
  case InGameKeyCommandAction::CameraToNotification: {
    /* the in-game root that holds this world view */
    InGameRuntimeRoot *root = THANDOR_CONTAINER_OF(world, InGameRuntimeRoot, worldRuntime);
    FixedVectorQ12 point;
    if ((root->targetingWorldXQ12 == 0) ||
        (root->targetingWorldYQ12 == 0)) {
      break;
    }
    FieldGrid_GetNearestTerrainPoint(root->targetingWorldYQ12,
                                     root->targetingWorldXQ12,world->fieldGrid,&point);
    WorldRuntime_PointCameraAtTarget
              ((world->motion).pitchAngle,(world->motion).headingAngle,(world->motion).targetDistanceQ12,
               point.zQ12,root->targetingWorldYQ12,
               root->targetingWorldXQ12,world);
    break;
  }
  case InGameKeyCommandAction::CameraToSelection: {
    FixedVectorQ12 center;
    if (!SelectionInfoEntitySlots_ComputeAverageWorldPosition(&center)) {
      break;
    }
    WorldRuntime_PointCameraAtTarget
              ((world->motion).pitchAngle,(world->motion).headingAngle,(world->motion).committedDistanceQ12,
               center.zQ12,center.yQ12,center.xQ12,world);
    break;
  }
  case InGameKeyCommandAction::CameraToClass11Model: {
    /* the first model on the owner list that belongs to the active faction and whose definition has
       runtime class 11 */
    WorldOwnerListNode *ownerNode = world->ownerListHead;
    uint32_t faction = world->activeFactionRuntimeIndex;
    for (; ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
      ModelRuntimeSlot *modelRuntime;
      if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
        continue;
      }
      modelRuntime = WorldOwnerNode_ModelRuntime(ownerNode);
      if ((faction == (uint32_t)modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
          (modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11)) {
        WorldRuntime_PointCameraAtTarget
                  ((world->motion).pitchAngle,(world->motion).headingAngle,(world->motion).committedDistanceQ12,
                   ownerNode->worldZQ12,ownerNode->worldYQ12,ownerNode->worldXQ12,world);
        break;
      }
    }
    break;
  }
  case InGameKeyCommandAction::SelectionResetMovement:
  case InGameKeyCommandAction::SelectionStopMovement:
  case InGameKeyCommandAction::SelectionCancelTargets:
  case InGameKeyCommandAction::SelectionSelfDestruct: {
    if (commandsBlocked || SelectionInfo_AllEntriesEmptyOrMatchOwner(world->activeFactionRuntimeIndex)) {
      break;
    }
    /* in-game commands 0xE10, 0xE30, 0xE50 and 0xE70 */
    if (target == InGameKeyCommandAction::SelectionResetMovement) {
      InGameCommand_Issue<PlayerSelection_ResetMovementPruneAndRecenterEntries>(0,0,0);
    }
    else if (target == InGameKeyCommandAction::SelectionStopMovement) {
      InGameCommand_Issue<PlayerSelection_StopMovement>(0,0,0);
    }
    else if (target == InGameKeyCommandAction::SelectionCancelTargets) {
      InGameCommand_Issue<PlayerSelection_CancelTargets>(0,0,0);
    }
    else {
      InGameCommand_Issue<PlayerSelection_SelfDestruct>(0,0,0);
    }
    break;
  }
  case InGameKeyCommandAction::UpgradePageToggle: {
    /* a sprite button; the template member is still an untyped node */
    UiSpriteButtonControl *upgradeButton =
         reinterpret_cast<UiSpriteButtonControl *>(&THANDOR_CONTAINER_OF(world, InGameUiImage, worldView)->singleSelectionUpgradeButton);
    if (commandsBlocked || (Any((upgradeButton->selectable).base.nodeFlags & UI_NODE_SUPPRESSED))) {
      break;
    }
    /* only while the single-selection page is shown */
    if (UiPageStack_ActivePageIndex
              (UiLayoutContainerControl_AsPageStack(
                   &THANDOR_CONTAINER_OF(world, InGameUiImage, worldView)->selectionDetailPageStack))
        != 1) {
      break;
    }
        if (Any((upgradeButton->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) && (upgradeButton->activationSound != nullptr)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,upgradeButton->activationSound,nullptr);
    }
    InGameTechnologyPanel_ToggleForSelection(reinterpret_cast<UiNodeBase *>(world)); /* the world view node */
    break;
  }
  case InGameKeyCommandAction::SelectOwnedClass16:
    if (commandsBlocked) {
      break;
    }
    InGameCommand_Issue<InGameSelection_SelectAllOwnAircraftPads>(0,0,0);
    break;
  case InGameKeyCommandAction::FreeCameraToggle:
    /* no pitch and distance clamps in the world motion code */
    world->runtimeFlags = world->runtimeFlags ^ WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA;
    break;
  case InGameKeyCommandAction::WrappedStatusTextToggle:
    THANDOR_CONTAINER_OF(world, InGameUiImage, worldView)->worldViewWrappedStatusText.base.nodeFlags =
         THANDOR_CONTAINER_OF(world, InGameUiImage, worldView)->worldViewWrappedStatusText.base.nodeFlags ^
         UI_NODE_SUPPRESSED;
    break;
  case InGameKeyCommandAction::CheatOccupancyToggle:
    if (!localSession || (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED))) {
      break;
    }
    /* the flag an ended local faction gets, whose simulation step also sets occupancy bit 0 on every cell */
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED;
    if (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED)) {
      FieldGrid_ClearOccupancyMaskByteBit0AllCells(world->activeFactionRuntimeIndex,world->fieldGrid);
    }
    else {
      FieldGrid_SetOccupancyMaskByteBit0AllCells(world->activeFactionRuntimeIndex,world->fieldGrid);
    }
    FieldGrid_ClassifyCellFlagsToRuntimeByte(world->activeFactionRuntimeIndex,world->fieldGrid);
    break;
  case InGameKeyCommandAction::InfoTextNext:
    /* local display only: no command, no simulation state */
    InGameWorldView_ShowNextInfoText(&THANDOR_CONTAINER_OF(world, InGameUiImage, worldView)->worldViewCyclingInfoText);
    break;
  default:
    Thandor_Log("InGameUi dispatch: unhandled action %u",static_cast<uint32_t>(target));
    break;
  }
  return false;
}

/* Ctrl+I in the game (InGameUiRuntime_DispatchCommandByCodeAndModifierFlags) and in the map editor
   (InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlags): shows the next world view info text. The
   control's text field holds a text resource id: 0x112 is a blank (the default, so the line is off), 0x113 the
   render statistics (frames per second, draw calls, texture binds and reloads per frame), 0x114 the camera
   position, 0x115 the camera orientation, 0x116 the cursor, 0x117 the free arena bytes; after 0x117 it wraps to the
   blank. The placeholders are filled by InGameHud_UpdateStatusCountersAndSessionPrompts every tick (the
   statistics once a second). The choice is kept for the next session (InGameRuntime_SaveWorldViewInfoTextChoice).
*/
void InGameWorldView_ShowNextInfoText(UiSingleLineTextControl *infoText)

{
  uint32_t resourceId;

  resourceId = static_cast<uint32_t>(infoText->text) + 1; /* the slot holds a text id */
  if (resourceId > TEXT_ID_WORLD_VIEW_INFO_LAST) {
    resourceId = TEXT_ID_WORLD_VIEW_INFO_FIRST;
  }
  infoText->text = reinterpret_cast<uint16_t *>(static_cast<uintptr_t>(resourceId));
}

/* The world view's fieldRegion.clearTransientStateCallback: resets the notification target button's cursor
   frame to 0 when it still shows frame 0x1B.
*/
void InGameUiRuntime_ResetNotificationButtonCursor(void *worldView)

{
  UiImageActionControl *notificationButton;

  /* an image action control; the template member is still an untyped node */
  notificationButton = reinterpret_cast<UiImageActionControl *>(
       &THANDOR_CONTAINER_OF(worldView, InGameUiImage, worldView)->notificationTargetButton);
  if (notificationButton->cursorFrame == INGAME_NOTIFICATION_CURSOR_CANCEL) {
    notificationButton->cursorFrame = 0;
  }
}

/* The world view's dispatchWorldContextActionCallback, unless the game is paused or the world input is
   blocked: a running camera move (runtimeFlags 0x10) is aborted and the saved camera restored; otherwise a
   pending unit placement is dropped (GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid, network command
   0x14F0), or else the selection is cleared (network command 0xBA0). Both outcomes of the
   SelectionInfo_TestNotOwnAircraftPadsWithAircraft test clear the selection, as in the original.
*/
void InGameUiRuntime_DispatchWorldContextActionCallback(WorldRuntimeContext *world)

{
  Bool8 hasActiveOwnerType16;

  /* the original tests WORLD_INPUT_DISABLED twice */
  if (((!Any(g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED))) &&
      (!Any((world->interaction).nodeFlags & UI_NODE_SUPPRESSED))) &&
     (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED))) {
    if ((world->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) == 0) {
      if (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING)) {
        hasActiveOwnerType16 = SelectionInfo_TestNotOwnAircraftPadsWithAircraft(world->activeFactionRuntimeIndex);
        if (hasActiveOwnerType16) {
          InGameCommand_Issue<FrontendPlayerSelection_ClearAndRefreshLocalPanels>(0,0,0);
          return;
        }
        InGameCommand_Issue<FrontendPlayerSelection_ClearAndRefreshLocalPanels>(0,0,0);
      }
      else {
        InGameCommand_Issue<GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid>
                  (0,0,world->activeFactionRuntimeIndex);
      }
    }
    else {
      world->runtimeFlags = world->runtimeFlags & ~WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO;
      WorldRuntime_RestoreMotionStateFromSnapshot(world);
    }
  }
}

/* Queues an in-game notification (movie id, priority and position/orientation payload) in the four-slot
   queue g_InGameRuntimeRoot->notificationQueue, which is kept sorted by descending priority: every slot
   of lower priority is swapped (atomic exchange per dword) with the carried record, so lower entries move down one slot and the
   lowest falls out. A notification with movie id 0 is ignored.
*/
void InGameNotificationQueue_InsertPriorityRecord(InGameNotificationPayloadKind payloadKind,uint32_t payloadReserved,
          uint32_t orientationValue,AngleTurn32 orientationAngle,
          Q12 secondaryWorldCoordinateQ12,Q12 primaryWorldCoordinateQ12,
          InGameNotificationPriority priority,InGameNotificationMovieId notificationMovieId)

{
  InGameNotificationQueueRecord *queueSlot;
  uint32_t slotIndex;

  if (notificationMovieId == 0) {
    return;
  }
  /* The parameters hold the carried record: the new one first, then whatever each swap pushed out. */
  queueSlot = g_InGameRuntimeRoot->notificationQueue;
  for (slotIndex = 0; slotIndex < INGAME_NOTIFICATION_QUEUE_SLOTS; slotIndex++, queueSlot++) {
    if (queueSlot->priority < priority) {
      /* atomic exchange per dword: InGameRuntime_ProcessQueuedSessionNotificationTimer pops the queue on the timer thread */
      notificationMovieId = THANDOR_ATOMIC_EXCHANGE(&queueSlot->movieId,notificationMovieId);
      priority = THANDOR_ATOMIC_EXCHANGE(&queueSlot->priority,priority);
      primaryWorldCoordinateQ12 =
           (Q12)THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).worldXQ12,primaryWorldCoordinateQ12);
      secondaryWorldCoordinateQ12 =
           (Q12)THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).worldYQ12,secondaryWorldCoordinateQ12);
      orientationAngle =
           THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).headingAngle,orientationAngle);
      orientationValue =
           THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).orientationOrPresentationValue,orientationValue);
      payloadReserved = THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).reserved10,payloadReserved);
      payloadKind = static_cast<InGameNotificationPayloadKind>
           (THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).payloadKind,payloadKind));
    }
  }
}
