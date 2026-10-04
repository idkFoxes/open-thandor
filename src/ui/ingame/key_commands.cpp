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

/* 63 key command records and the terminator record (commandCode 0) that ends the dispatcher's scan */
static UiCommandDispatchRecord g_InGameCommandDispatchRecords[64] = {
    /*  0 */ {.commandCode = 0x30073, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567F60},
    /*  1 */ {.commandCode = 0x30073, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567FC0},
    /*  2 */ {.commandCode = 0x30073, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x568020},
    /*  3 */ {.commandCode = 0x30073, .continuationEntryAddress = 0x567F60},
    /*  4 */ {.commandCode = 0x30062, .continuationEntryAddress = 0x567ED0},
    /*  5 */ {.commandCode = 0x30061, .continuationEntryAddress = 0x5680F0},
    /*  6 */ {.commandCode = 0x30031, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /*  7 */ {.commandCode = 0x30032, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /*  8 */ {.commandCode = 0x30033, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /*  9 */ {.commandCode = 0x30034, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /* 10 */ {.commandCode = 0x30035, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /* 11 */ {.commandCode = 0x30036, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /* 12 */ {.commandCode = 0x30037, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /* 13 */ {.commandCode = 0x30038, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /* 14 */ {.commandCode = 0x30031, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 15 */ {.commandCode = 0x30032, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 16 */ {.commandCode = 0x30033, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 17 */ {.commandCode = 0x30034, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 18 */ {.commandCode = 0x30035, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 19 */ {.commandCode = 0x30036, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 20 */ {.commandCode = 0x30037, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 21 */ {.commandCode = 0x30038, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 22 */ {.commandCode = 0x30031, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 23 */ {.commandCode = 0x30032, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 24 */ {.commandCode = 0x30033, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 25 */ {.commandCode = 0x30034, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 26 */ {.commandCode = 0x30035, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 27 */ {.commandCode = 0x30036, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 28 */ {.commandCode = 0x30037, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 29 */ {.commandCode = 0x30038, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 30 */ {.commandCode = 0x30031, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 31 */ {.commandCode = 0x30032, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 32 */ {.commandCode = 0x30033, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 33 */ {.commandCode = 0x30034, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 34 */ {.commandCode = 0x30035, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 35 */ {.commandCode = 0x30036, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 36 */ {.commandCode = 0x30037, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 37 */ {.commandCode = 0x30038, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 38 */ {.commandCode = 0x30031, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 39 */ {.commandCode = 0x30032, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 40 */ {.commandCode = 0x30033, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 41 */ {.commandCode = 0x30034, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 42 */ {.commandCode = 0x30035, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 43 */ {.commandCode = 0x30036, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 44 */ {.commandCode = 0x30037, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 45 */ {.commandCode = 0x30038, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 46 */ {.commandCode = 0x30031, .continuationEntryAddress = 0x567CC0},
    /* 47 */ {.commandCode = 0x30032, .continuationEntryAddress = 0x567CC0},
    /* 48 */ {.commandCode = 0x30033, .continuationEntryAddress = 0x567CC0},
    /* 49 */ {.commandCode = 0x30034, .continuationEntryAddress = 0x567CC0},
    /* 50 */ {.commandCode = 0x30035, .continuationEntryAddress = 0x567CC0},
    /* 51 */ {.commandCode = 0x30036, .continuationEntryAddress = 0x567CC0},
    /* 52 */ {.commandCode = 0x30037, .continuationEntryAddress = 0x567CC0},
    /* 53 */ {.commandCode = 0x30038, .continuationEntryAddress = 0x567CC0},
    /* 54 */ {.commandCode = 0x20, .continuationEntryAddress = 0x567E00},
    /* 55 */ {.commandCode = 0x20, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567E40},
    /* 56 */ {.commandCode = 0x10003, .continuationEntryAddress = 0x567E20},
    /* 57 */ {.commandCode = 0x30066, .continuationEntryAddress = 0x568080},
    /* 58 */ {.commandCode = 0x3006F, .continuationEntryAddress = 0x5681A0},
    /* 59 */ {.commandCode = 0x30076, .modifierClassFlags = 0x3C, .continuationEntryAddress = 0x5681B0},
    /* 60 */ {.commandCode = 0x30063, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x568190},
    /* 61 */ {.commandCode = 0x10015, .continuationEntryAddress = 0x567EA0},
    /* 62 */ {.commandCode = 0x30064, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x568130},
    /* 63 */ {.commandCode = 0x0, .modifierClassFlags = 0x90909090, .continuationEntryAddress = 0x90909090}}; /* commandCode 0, the rest is the original's NOP fill */

uint32_t g_UiCommandRuntimeFlags = 0;

/* Continuation addresses stored in g_InGameCommandDispatchRecords (entry points
   inside the original function; the rewritten dispatcher below switches on them). */
enum InGameKeyCommandContinuation {
  INGAME_KEY_RECALL_GROUP = 0x567cc0,                /* 1..8 */
  INGAME_KEY_ADD_GROUP_TO_SELECTION = 0x567d10,      /* Shift+1..8 */
  INGAME_KEY_STORE_SELECTION_AS_GROUP = 0x567d60,    /* Ctrl+1..8, Alt+1..8 */
  INGAME_KEY_ADD_SELECTION_TO_GROUP = 0x567db0,      /* Ctrl+Shift+1..8, Alt+Shift+1..8 */
  INGAME_KEY_NOTIFICATION_ADVANCE = 0x567e00,        /* Space */
  INGAME_KEY_NOTIFICATION_CANCEL = 0x567e20,         /* Backspace */
  INGAME_KEY_CAMERA_TO_NOTIFICATION = 0x567e40,      /* Alt+Space */
  INGAME_KEY_CAMERA_TO_SELECTION = 0x567ea0,         /* Numpad 5 */
  INGAME_KEY_CAMERA_TO_CLASS11_MODEL = 0x567ed0,     /* B */
  INGAME_KEY_SELECTION_RESET_MOVEMENT = 0x567f60,    /* S, Shift+Alt+S */
  INGAME_KEY_SELECTION_STOP_MOVEMENT = 0x567fc0,     /* Shift+S */
  INGAME_KEY_SELECTION_CANCEL_TARGETS = 0x568020,    /* Alt+S */
  INGAME_KEY_UPGRADE_PAGE_TOGGLE = 0x568080,         /* F */
  INGAME_KEY_SELECT_OWNED_CLASS16 = 0x5680f0,        /* A */
  INGAME_KEY_SELECTION_SELF_DESTRUCT = 0x568130,     /* Alt+D */
  INGAME_KEY_FREE_CAMERA_TOGGLE = 0x568190,          /* Alt+C */
  INGAME_KEY_WRAPPED_STATUS_TEXT_TOGGLE = 0x5681a0,  /* O */
  INGAME_KEY_CHEAT_OCCUPANCY_TOGGLE = 0x5681b0       /* Ctrl+Alt+V */
};

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
   Returns true when no record matches: the field is the keyboardFallback of the world view's
   pointer context (FrontendModelPointerContext_KeyboardEvent), which then passes the key on, so keys such as Esc
   reach the in-game root's hotkeys. A matched record returns false, also when the command is blocked.
*/
Bool8 InGameUiRuntime_DispatchCommandByCodeAndModifierFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          WorldRuntimeContext *world)

{
  /* Each dispatch record names its handler by continuationEntryAddress, which only serves as the case
     label of the switch below. world is the world view (worldRuntime). */
  UiCommandDispatchRecord *record; /* g_InGameCommandDispatchRecords ends at the terminator record [63] */
  uint32_t target;
  Bool8 localSession =
       (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL;
  Bool8 commandsBlocked =
       (g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) != 0;

  record = UiCommandDispatch_Find(g_InGameCommandDispatchRecords,commandCode,modifierFlags,
                                  UiKeyModifierRule::ExactWithShift);
  if (record == nullptr) {
    return true; /* not a world view key: the pointer context passes it on (Esc reaches the root's hotkeys) */
  }
  target = (uint32_t)record->continuationEntryAddress;
  switch (target) {
  case INGAME_KEY_RECALL_GROUP:
  case INGAME_KEY_ADD_GROUP_TO_SELECTION:
  case INGAME_KEY_STORE_SELECTION_AS_GROUP:
  case INGAME_KEY_ADD_SELECTION_TO_GROUP: {
    /* FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh mode bits: 1 = the selection is the
       source and the group the destination, 2 = merge instead of replace */
    uint32_t transferMode = (target == INGAME_KEY_RECALL_GROUP) ? 0 :
                            (target == INGAME_KEY_ADD_GROUP_TO_SELECTION) ? 2 :
                            (target == INGAME_KEY_STORE_SELECTION_AS_GROUP) ? 1 : 3;
    uint32_t groupIndex = commandCode - KEYBOARD_KEY_CODE_CHAR('1');
    if (commandsBlocked) {
      break;
    }
    InGameCommand_Issue<FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh>
              (world->activeFactionRuntimeIndex,transferMode,groupIndex);
    break;
  }
  case INGAME_KEY_NOTIFICATION_ADVANCE:
    InGameTargetingContext_AdvanceOrResolveTarget
              ((InGameTargetingRootTraversalView *)
               THANDOR_UI_SIBLING(world,InGameUiImage,worldView,notificationTargetButton));
    break;
  case INGAME_KEY_NOTIFICATION_CANCEL:
    InGameTargetingContext_CancelAndRestoreState
              ((InGameTargetingRootTraversalView *)
               THANDOR_UI_SIBLING(world,InGameUiImage,worldView,notificationTargetButton));
    break;
  case INGAME_KEY_CAMERA_TO_NOTIFICATION: {
    /* the in-game root that holds this world view */
    InGameRuntimeRoot *root = (InGameRuntimeRoot *)
         ((uint8_t *)world - offsetof(InGameRuntimeRoot,worldRuntime));
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
  case INGAME_KEY_CAMERA_TO_SELECTION: {
    FixedVectorQ12 center;
    if (!SelectionInfoEntitySlots_ComputeAverageWorldPosition(&center)) {
      break;
    }
    WorldRuntime_PointCameraAtTarget
              ((world->motion).pitchAngle,(world->motion).headingAngle,(world->motion).committedDistanceQ12,
               center.zQ12,center.yQ12,center.xQ12,world);
    break;
  }
  case INGAME_KEY_CAMERA_TO_CLASS11_MODEL: {
    /* the first model on the owner list that belongs to the active faction and whose definition has
       runtime class 11 */
    WorldOwnerListNode *ownerNode = world->ownerListHead;
    uint32_t faction = world->activeFactionRuntimeIndex;
    for (; ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
      uint8_t *modelRuntime;
      if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
        continue;
      }
      modelRuntime = (uint8_t *)ownerNode->runtimePayload;
      if ((faction == (uint32_t)((ModelRuntimeSlot *)modelRuntime)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
          (((ModelRuntimeSlot *)modelRuntime)->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11)) {
        WorldRuntime_PointCameraAtTarget
                  ((world->motion).pitchAngle,(world->motion).headingAngle,(world->motion).committedDistanceQ12,
                   ownerNode->worldZQ12,ownerNode->worldYQ12,ownerNode->worldXQ12,world);
        break;
      }
    }
    break;
  }
  case INGAME_KEY_SELECTION_RESET_MOVEMENT:
  case INGAME_KEY_SELECTION_STOP_MOVEMENT:
  case INGAME_KEY_SELECTION_CANCEL_TARGETS:
  case INGAME_KEY_SELECTION_SELF_DESTRUCT: {
    if (commandsBlocked || SelectionInfo_AllEntriesEmptyOrMatchOwner(world->activeFactionRuntimeIndex)) {
      break;
    }
    /* in-game commands 0xE10, 0xE30, 0xE50 and 0xE70 */
    if (target == INGAME_KEY_SELECTION_RESET_MOVEMENT) {
      InGameCommand_Issue<PlayerSelection_ResetMovementPruneAndRecenterEntries>(0,0,0);
    }
    else if (target == INGAME_KEY_SELECTION_STOP_MOVEMENT) {
      InGameCommand_Issue<PlayerSelection_StopMovement>(0,0,0);
    }
    else if (target == INGAME_KEY_SELECTION_CANCEL_TARGETS) {
      InGameCommand_Issue<PlayerSelection_CancelTargets>(0,0,0);
    }
    else {
      InGameCommand_Issue<PlayerSelection_SelfDestruct>(0,0,0);
    }
    break;
  }
  case INGAME_KEY_UPGRADE_PAGE_TOGGLE: {
    UiSpriteButtonControl *upgradeButton = (UiSpriteButtonControl *)
         THANDOR_UI_SIBLING(world,InGameUiImage,worldView,singleSelectionUpgradeButton);
    if (commandsBlocked || (((upgradeButton->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
      break;
    }
    /* only while the single-selection page is shown */
    if (UiPageStack_ActivePageIndex
              ((UiPageStackControl *)THANDOR_UI_SIBLING(world,InGameUiImage,worldView,selectionDetailPageStack))
        != 1) {
      break;
    }
        if ((((upgradeButton->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) && (upgradeButton->activationSound != nullptr)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,upgradeButton->activationSound,nullptr);
    }
    InGameTechnologyPanel_ToggleForSelection((UiNodeBase *)world);
    break;
  }
  case INGAME_KEY_SELECT_OWNED_CLASS16:
    if (commandsBlocked) {
      break;
    }
    InGameCommand_Issue<InGameSelection_SelectAllOwnAircraftPads>(0,0,0);
    break;
  case INGAME_KEY_FREE_CAMERA_TOGGLE:
    /* no pitch and distance clamps in the world motion code */
    world->runtimeFlags = world->runtimeFlags ^ WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA;
    break;
  case INGAME_KEY_WRAPPED_STATUS_TEXT_TOGGLE:
    THANDOR_UI_SIBLING(world,InGameUiImage,worldView,worldViewWrappedStatusText)->nodeFlags =
         THANDOR_UI_SIBLING(world,InGameUiImage,worldView,worldViewWrappedStatusText)->nodeFlags ^
         UI_NODE_SUPPRESSED;
    break;
  case INGAME_KEY_CHEAT_OCCUPANCY_TOGGLE:
    if (!localSession || ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED) == 0)) {
      break;
    }
    /* the flag an ended local faction gets, whose simulation step also sets occupancy bit 0 on every cell */
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED;
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED) == 0) {
      FieldGrid_ClearOccupancyMaskByteBit0AllCells(world->activeFactionRuntimeIndex,world->fieldGrid);
    }
    else {
      FieldGrid_SetOccupancyMaskByteBit0AllCells(world->activeFactionRuntimeIndex,world->fieldGrid);
    }
    FieldGrid_ClassifyCellFlagsToRuntimeByte(world->activeFactionRuntimeIndex,world->fieldGrid);
    break;
  default:
    Thandor_Log("InGameUi dispatch: unhandled continuation %08x",target);
    break;
  }
  return false;
}

/* The world view's fieldRegion.clearTransientStateCallback: resets the notification target button's cursor
   frame to 0 when it still shows frame 0x1B.
*/
void InGameUiRuntime_ResetNotificationButtonCursor(void *worldView)

{
  UiImageActionControl *notificationButton;

  notificationButton = (UiImageActionControl *)
       THANDOR_UI_SIBLING(worldView,InGameUiImage,worldView,notificationTargetButton);
  if (notificationButton->cursorFrame == INGAME_NOTIFICATION_CURSOR_CANCEL) {
    notificationButton->cursorFrame = 0;
  }
  return;
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
  if ((((g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) &&
      (((world->interaction).nodeFlags & 8) == 0)) &&
     ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0)) {
    if ((world->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) == 0) {
      if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) == 0) {
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
  return;
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
      payloadKind = (InGameNotificationPayloadKind)
           THANDOR_ATOMIC_EXCHANGE(&(queueSlot->payload).payloadKind,payloadKind);
    }
  }
  return;
}
