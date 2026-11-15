/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/hotkeys.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/hotkeys.h>
#include <thandor/ui/core/key_dispatch.h>
#include <thandor/ui/core/key_binding_tables.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>
#include <thandor/ui/ingame/key_bindings_overlay.h>
#include <thandor/ui/ingame/objective_overlay.h>

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
    ToggleObjectiveOverlay = 16,  /* not in the original: Z (key binding objective_toggle, spare records only) */
};
static_assert(sizeof(UiKeyCommandRecord<InGameHotkeyAction>) == 0xC, "a key command record keeps the original 12 bytes");

/* The 15 command records of the original (with the default key bindings). The dispatcher scans the working table
   g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30 built from them (ui/core/key_binding_tables.h). */
static constexpr UiKeyCommandRecord<InGameHotkeyAction> g_InGameHotkeyOriginalRecords[15] = {
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
    /* 14 */ {.commandCode = 0x3007A, .modifierClassFlags = KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT, .action = InGameHotkeyAction::CheatToggleFastBuild}};

/* The working table: the 15 records rewritten from the key bindings (InGameHotkeys_ApplyBindings), 45 spare records
   for further chords (10 editable actions with one default chord: 4 more each; objective_toggle, which has no
   original record: all 5) and the terminator record (commandCode 0) that ends the dispatcher's scan (the original's
   other two dwords were NOP fill, never read). */
static constexpr std::size_t INGAME_HOTKEY_TABLE_CAPACITY = 15 + 45 + 1;
static std::array<UiKeyCommandRecord<InGameHotkeyAction>, INGAME_HOTKEY_TABLE_CAPACITY>
    g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30 =
        KeyBindingTable_Make<InGameHotkeyAction, INGAME_HOTKEY_TABLE_CAPACITY>(g_InGameHotkeyOriginalRecords);

uint32_t InGameHotkeys_ApplyBindings()
{
  return KeyBindingTable_Apply(g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30,
                               g_InGameHotkeyOriginalRecords,KeyBindingSource::InGameHotkeys);
}

uint32_t InGameHotkeys_FindBinding(uint32_t keyCode,UiKeyboardStateMask heldFlags)
{
  const UiKeyCommandRecord<InGameHotkeyAction> *record = UiCommandDispatch_Find(
      g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30.data(), keyCode, heldFlags,
      UiKeyModifierRule::ShiftRequiredByClass);
  return (record == nullptr) ? 0 : static_cast<uint32_t>(record->action);
}

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
  bool localSession = !Any(g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK);

  /* project change: no hotkey while the key binding overlay is open (keys the panel passes on end here) */
  if (InGameKeyBindingsOverlay_IsOpen()) {
    return false;
  }

  /* A record without modifier class matches only without Ctrl and Alt; otherwise exactly the named
     combination (Ctrl, Alt, or both) must be held. Shift is ignored, unless a rebound key's class has Shift: then
     Shift must be held (no default record has Shift, so the defaults match as in the original). */
  const UiKeyCommandRecord<InGameHotkeyAction> *record = UiCommandDispatch_Find(
      g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30.data(), (uint32_t)commandCode,
      modifierFlags, UiKeyModifierRule::ShiftRequiredByClass);
  if (record == nullptr) {
    return false;
  }
  InGameHotkeyAction target = record->action;
  switch (target) {
  case InGameHotkeyAction::ToggleObjectiveOverlay: /* project change: Z, the objective overlay on/off (display only) */
    ObjectiveOverlay_ToggleSetting();
    break;
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
        reinterpret_cast<uint32_t *>(UiTextEdit_Text(&image->chatInputTextEdit).data())[i] = 0;
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
      reinterpret_cast<uint32_t *>(UiTextEdit_Text(&image->messageTextEdit).data())[i] = 0;
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
