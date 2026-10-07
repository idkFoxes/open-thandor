/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/hotkeys.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/hotkeys.h>
#include <thandor/ui/core/key_dispatch.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

/* Actions of the in-game root's hotkey table (InGameHotkeys_DispatchCommandByFlags). */
enum class InGameHotkeyAction : uint32_t {
    LeaveGame = 1,                /* Alt+Q */
    OpenChatLine = 2,             /* Enter */
    ToggleMessageWindow = 3,      /* C */
    OpenGameMenu = 4,             /* Esc */
    ToggleMissionObjectives = 5,  /* F1 */
    OpenSavePage = 6,             /* F2 */
    OpenQuitPage = 7,             /* Alt+F4 */
    TogglePause = 8,              /* P */
    SlowerGameSpeed = 9,          /* Alt+G */
    FasterGameSpeed = 10,         /* G */
    ToggleSidePanel = 11,         /* Tab */
    Screenshot = 12,              /* Alt+P */
    CheatAddXenite = 13,          /* Ctrl+Alt+X */
    CheatAddEnergy = 14,          /* Ctrl+Alt+E */
    CheatToggleFastBuild = 15,    /* Ctrl+Alt+Z */
};
static_assert(sizeof(UiKeyCommandRecord<InGameHotkeyAction>) == 0xC, "a key command record keeps the original 12 bytes");

/* 15 command records and the terminator
   record (commandCode 0) that ends the dispatcher's scan */
static const UiKeyCommandRecord<InGameHotkeyAction> g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30[16] = {
    /*  0 */ {.commandCode = 0x30071, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameHotkeyAction::LeaveGame},
    /*  1 */ {.commandCode = 0x10001, .action = InGameHotkeyAction::OpenChatLine},
    /*  2 */ {.commandCode = 0x30063, .action = InGameHotkeyAction::ToggleMessageWindow},
    /*  3 */ {.commandCode = 0x10000, .action = InGameHotkeyAction::OpenGameMenu},
    /*  4 */ {.commandCode = 0x20001, .action = InGameHotkeyAction::ToggleMissionObjectives},
    /*  5 */ {.commandCode = 0x20002, .action = InGameHotkeyAction::OpenSavePage},
    /*  6 */ {.commandCode = 0x20004, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameHotkeyAction::OpenQuitPage},
    /*  7 */ {.commandCode = 0x30070, .action = InGameHotkeyAction::TogglePause},
    /*  8 */ {.commandCode = 0x30067, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameHotkeyAction::SlowerGameSpeed},
    /*  9 */ {.commandCode = 0x30067, .action = InGameHotkeyAction::FasterGameSpeed},
    /* 10 */ {.commandCode = 0x10002, .action = InGameHotkeyAction::ToggleSidePanel},
    /* 11 */ {.commandCode = 0x30070, .modifierClassFlags = KEYBOARD_STATE_ALT, .action = InGameHotkeyAction::Screenshot},
    /* 12 */ {.commandCode = 0x30078, .modifierClassFlags = KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT, .action = InGameHotkeyAction::CheatAddXenite},
    /* 13 */ {.commandCode = 0x30065, .modifierClassFlags = KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT, .action = InGameHotkeyAction::CheatAddEnergy},
    /* 14 */ {.commandCode = 0x3007A, .modifierClassFlags = KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT, .action = InGameHotkeyAction::CheatToggleFastBuild},
    /* 15 */ {.commandCode = 0x0}}; /* terminator: commandCode 0 (the original's other two dwords were NOP fill, never read) */

/* Keyboard fallback of the in-game UI root: looks the key up in the hotkey table (key code plus required Ctrl/Alt
   combination) and runs its action: chat, message window, menus, save, pause, game speed, side panel,
   screenshot, leaving the game and the three cheat keys (only while cheats are enabled).
*/
bool InGameHotkeys_DispatchCommandByFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          InGameRuntimeRootFrameView *inGameRoot)

{
  /* Each record names one action, one case of the switch below (the original jumped to a continuation inside
     this function). image is the in-game UI image
     of the runtime root. */
  InGameUiImage *image = InGameUi_Image(inGameRoot);
  Bool8 localSession = !Any(g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK);

  /* A record without modifier class matches only without Ctrl and Alt; otherwise exactly the named
     combination (Ctrl, Alt, or both) must be held. Shift is ignored. */
  const UiKeyCommandRecord<InGameHotkeyAction> *record = UiCommandDispatch_Find(
      g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30, (uint32_t)commandCode,
      modifierFlags, UiKeyModifierRule::ExactShiftIgnored);
  if (record == nullptr) {
    return false;
  }
  InGameHotkeyAction target = record->action;
  switch (target) {
  case InGameHotkeyAction::CheatToggleFastBuild: /* Ctrl+Alt+Z, cheat: toggle fast build and research */
    if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED)) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD;
    }
    break;
  case InGameHotkeyAction::CheatAddXenite: /* Ctrl+Alt+X, cheat: +1000 Xenite (xeniteCurrentQ4 += 1000 << Q4_SHIFT) */
    if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED)) {
      g_GameFactionRuntimeImage.records[reinterpret_cast<WorldRuntimeContext *>(
           &image->worldView)->activeFactionRuntimeIndex].xeniteCurrentQ4 += 1000 << Q4_SHIFT;
    }
    break;
  case InGameHotkeyAction::CheatAddEnergy: /* Ctrl+Alt+E, cheat: +100 energy supply and capacity (Q4) */
    if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED)) {
      g_GameFactionRuntimeImage.records[reinterpret_cast<WorldRuntimeContext *>(
           &image->worldView)->activeFactionRuntimeIndex].baselineEnergySupplyQ4 += 100 << Q4_SHIFT;
      g_GameFactionRuntimeImage.records[reinterpret_cast<WorldRuntimeContext *>(
           &image->worldView)->activeFactionRuntimeIndex].energyGenerationCapacityQ4 += 100 << Q4_SHIFT;
    }
    break;
  case InGameHotkeyAction::OpenChatLine: /* Enter: open the chat line */
    UiPageStack_SetActiveIndex(1,UiLayoutContainerControl_AsPageStack(&image->chatInputPageStack));
    image->chatInputTextEdit.cursorIndex = 0;
    image->chatInputTextEdit.selectionStart = 0;
    image->chatInputTextEdit.selectionEnd = 0;
    if (!localSession) {
      UiNodeBase *recipientTab;
      int i;
      for (i = 0; i < 24; i++) {
        reinterpret_cast<uint32_t *>(
            reinterpret_cast<InGameCommandTextEditControlCC *>(&image->chatInputTextEdit)->textBuffer)[i] = 0;
      }
      /* Original quirk: the result is not tested; with no tab selected this is the last tab */
      UiSelectableGroup_FindVisibleSelected(&recipientTab,nullptr,3,&image->messageRecipientAllTab.selectable.base,
                                            &image->messageRecipientGroupsTab.selectable.base,
                                            &image->messageRecipientPlayersTab.selectable.base);
      g_InGameUiActionHandlersPage10.handlers[reinterpret_cast<UiSelectableControl *>(recipientTab)->actionId & 0xff]
                (recipientTab);
    }
    UiKeyboardFocus_Set(&image->chatInputTextEdit.base);
    break;
  case InGameHotkeyAction::OpenQuitPage: /* Alt+F4: game menu on the quit page */
  case InGameHotkeyAction::OpenSavePage: /* F2: game menu on the save page (local games only) */
  case InGameHotkeyAction::OpenGameMenu: /* Esc: game menu */
  case InGameHotkeyAction::ToggleMissionObjectives: { /* F1: mission objectives */
    UiSpriteButtonControl *toggleButton;
    UiSelectableControl *toggle;
    if ((target == InGameHotkeyAction::OpenSavePage) && !localSession) {
      break;
    }
    toggleButton = (target == InGameHotkeyAction::ToggleMissionObjectives) ? &image->missionObjectivesButton : &image->inGameMenuButton;
    toggle = &toggleButton->selectable;
    UiSelectableControl_SetSelected(1,toggle);
    if (Any(toggle->stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) &&
        (toggleButton->activationSound != nullptr)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,
                            toggleButton->activationSound,nullptr);
    }
    if (target == InGameHotkeyAction::ToggleMissionObjectives) {
      InGameMissionHelpPage_Toggle(&toggle->base);
      break;
    }
    InGameSettingsPage_ToggleAndSynchronizeControls(toggle);
    if (target == InGameHotkeyAction::OpenQuitPage) {
      InGameQuitMenu_OpenAndRefreshButtons(
           reinterpret_cast<InGameCommandPanelSourceAddress32>(&image->gameMenuQuitButton));
    }
    else if (target == InGameHotkeyAction::OpenSavePage) {
      InGameSaveGamePage_RebuildCatalog(&image->gameMenuSaveButton.selectable.base);
    }
    break;
  }
  case InGameHotkeyAction::ToggleMessageWindow: { /* C: toggle the message window (network games only) */
    UiPageStackControl *stack;
    uint32_t index;
    UiNodeBase *recipientTab;
    int i;
    if (localSession) {
      break;
    }
    stack = UiLayoutContainerControl_AsPageStack(&image->gameWindowPageStack);
    index = (UiPageStack_ActivePageIndex(stack) == 1) ? 0 : 1;
    UiPageStack_SetActiveIndex(index,stack);
    image->worldView.base.nodeFlags =
         image->worldView.base.nodeFlags & ~UI_NODE_SUPPRESSED;
    if (index != 1) {
      break;
    }
    image->worldView.base.nodeFlags =
         image->worldView.base.nodeFlags | UI_NODE_SUPPRESSED;
    UiKeyboardFocus_ReleaseNode(&image->worldView.base);
    image->messageTextEdit.cursorIndex = 0;
    image->messageTextEdit.selectionStart = 0;
    image->messageTextEdit.selectionEnd = 0;
    for (i = 0; i < 24; i++) {
      reinterpret_cast<uint32_t *>(
          reinterpret_cast<InGameCommandTextEditControlCC *>(&image->messageTextEdit)->textBuffer)[i] = 0;
    }
    /* Original quirk: the result is not tested; with no tab selected this is the last tab */
    UiSelectableGroup_FindVisibleSelected(&recipientTab,nullptr,3,&image->messageRecipientAllTab.selectable.base,
                                          &image->messageRecipientGroupsTab.selectable.base,
                                          &image->messageRecipientPlayersTab.selectable.base);
    g_InGameUiActionHandlersPage10.handlers[reinterpret_cast<UiSelectableControl *>(recipientTab)->actionId & 0xff]
              (recipientTab);
    g_KeyboardFlushEvents();
    break;
  }
  case InGameHotkeyAction::TogglePause: /* P: pause */
    InGameCommand_Issue<InGameCommand_TogglePauseRequest>(0,0,0);
    break;
  case InGameHotkeyAction::FasterGameSpeed: /* G: faster */
  case InGameHotkeyAction::SlowerGameSpeed: /* Alt+G: slower */ {
    int step = (target == InGameHotkeyAction::FasterGameSpeed) ? 1 : -1;
    InGameCommand_Issue<InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks>(0,0,step);
    break;
  }
  case InGameHotkeyAction::ToggleSidePanel: { /* Tab: hide or show the side panel; bit 2 of the map/mouse settings remembers it */
    PersistentMapMouseOptionFlags settings = PersistentSettings_ReadMapMouseOptions();
    UiPageStackControl *stack = UiLayoutContainerControl_AsPageStack(&image->sidePanelStack);
    if (UiPageStack_ActivePageIndex(stack) != 0) {
      UiPageStack_SetActiveIndex(0,stack);
      UiPageStack_SetActiveIndex(0,UiLayoutContainerControl_AsPageStack(&image->resourceBarModeStack));
      UiPageStack_SetActiveIndex(0,UiLayoutContainerControl_AsPageStack(&image->gamePanelsModeStack));
      image->worldViewArea.base.rightOffset = image->sidePanelFrameLeftEdge.base.leftOffset;
      settings = settings & ~PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN;
    }
    else {
      UiPageStack_SetActiveIndex(1,stack);
      UiPageStack_SetActiveIndex(0,UiLayoutContainerControl_AsPageStack(&image->resourceBarModeStack));
      UiPageStack_SetActiveIndex(0,UiLayoutContainerControl_AsPageStack(&image->gamePanelsModeStack));
      image->worldViewArea.base.rightOffset = 0;
      settings = settings | PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN;
    }
    UiContainer_LayoutChildren(&image->inGameRootPanel.root.base);
    PersistentSettings_WriteMapMouseOptions(settings);
    break;
  }
  case InGameHotkeyAction::Screenshot: /* Alt+P: screenshot to the next numbered PCX file */
    Screenshot_SaveFramebufferAsPcx();
    break;
  case InGameHotkeyAction::LeaveGame: /* Alt+Q: leave the game (not as host) */
    if (Any(g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST)) {
      break;
    }
    InGameCommand_Issue<InGameCommand_HandlePlayerDeparture>(0,0,0);
    break;
  default:
    Thandor_Log("EndGameResults dispatch: unhandled action %u",static_cast<uint32_t>(target));
    break;
  }
  return false;
}
