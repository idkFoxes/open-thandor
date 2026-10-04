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

/* 15 command records and the terminator
   record (commandCode 0) that ends the dispatcher's scan */
static const UiCommandDispatchRecord g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30[16] = {
    /*  0 */ {.commandCode = 0x30071, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567870},
    /*  1 */ {.commandCode = 0x10001, .continuationEntryAddress = 0x567270},
    /*  2 */ {.commandCode = 0x30063, .continuationEntryAddress = 0x5674B0},
    /*  3 */ {.commandCode = 0x10000, .continuationEntryAddress = 0x567410},
    /*  4 */ {.commandCode = 0x20001, .continuationEntryAddress = 0x567460},
    /*  5 */ {.commandCode = 0x20002, .continuationEntryAddress = 0x5673A0},
    /*  6 */ {.commandCode = 0x20004, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567340},
    /*  7 */ {.commandCode = 0x30070, .continuationEntryAddress = 0x5675E0},
    /*  8 */ {.commandCode = 0x30067, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567660},
    /*  9 */ {.commandCode = 0x30067, .continuationEntryAddress = 0x567620},
    /* 10 */ {.commandCode = 0x10002, .continuationEntryAddress = 0x5676A0},
    /* 11 */ {.commandCode = 0x30070, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x5677E0},
    /* 12 */ {.commandCode = 0x30078, .modifierClassFlags = 0x3C, .continuationEntryAddress = 0x567200},
    /* 13 */ {.commandCode = 0x30065, .modifierClassFlags = 0x3C, .continuationEntryAddress = 0x567230},
    /* 14 */ {.commandCode = 0x3007A, .modifierClassFlags = 0x3C, .continuationEntryAddress = 0x5671E0},
    /* 15 */ {.commandCode = 0x0, .modifierClassFlags = 0x90909090, .continuationEntryAddress = 0x90909090}}; /* commandCode 0, the rest is the original's NOP fill */

/* Implementation ownership: ui/ingame/hotkeys. */

/* Keyboard fallback of the in-game UI root: looks the key up in the hotkey table (key code plus required Ctrl/Alt
   combination) and runs its action: chat, message window, menus, save, pause, game speed, side panel,
   screenshot, leaving the game and the three cheat keys (only while cheats are enabled).
*/
Bool8 InGameHotkeys_DispatchCommandByFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          InGameRuntimeRootFrameView *inGameRoot)

{
  /* The record table holds the original game's continuation addresses inside this function; they are only
     used as keys here, each continuation is one case of the switch below. rt is the runtime root. */
  uint8_t *rt = (uint8_t *)inGameRoot;
  Bool8 localSession = (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == 0;

  /* A record without modifier class matches only without Ctrl and Alt; otherwise exactly the named
     combination (Ctrl, Alt, or both) must be held. Shift is ignored. */
  const UiCommandDispatchRecord *record = UiCommandDispatch_Find(
      g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30, (uint32_t)commandCode,
      (uint32_t)modifierFlags, UiKeyModifierRule::ExactShiftIgnored);
  if (record == nullptr) {
    return false;
  }
  uint32_t target = (uint32_t)record->continuationEntryAddress;
  switch (target) {
  case 0x5671e0: /* Ctrl+Alt+Z, cheat: toggle fast build and research */
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED) != 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD;
    }
    break;
  case 0x567200: /* Ctrl+Alt+X, cheat: +1000 Xenite (xeniteCurrentQ4 += 1000 << Q4_SHIFT) */
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED) != 0) {
      g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(rt,
           worldView))->activeFactionRuntimeIndex].xeniteCurrentQ4 += 1000 << Q4_SHIFT;
    }
    break;
  case 0x567230: /* Ctrl+Alt+E, cheat: +100 energy supply and capacity (Q4) */
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED) != 0) {
      g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(rt,
           worldView))->activeFactionRuntimeIndex].baselineEnergySupplyQ4 += 100 << Q4_SHIFT;
      g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(rt,
           worldView))->activeFactionRuntimeIndex].energyGenerationCapacityQ4 += 100 << Q4_SHIFT;
    }
    break;
  case 0x567270: /* Enter: open the chat line */
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(rt,chatInputPageStack));
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->cursorIndex = 0;
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->selectionStart = 0;
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->selectionEnd = 0;
    if (!localSession) {
      UiNodeBase *recipientTab;
      int i;
      for (i = 0; i < 24; i++) {
        ((uint32_t *)((InGameCommandTextEditControlCC *)INGAME_UI(rt,chatInputTextEdit))->textBuffer)[i] = 0;
      }
      /* Original quirk: the result is not tested; with no tab selected this is the last tab */
      UiSelectableGroup_FindVisibleSelected(&recipientTab,nullptr,3,INGAME_UI(rt,messageRecipientAllTab),
                                            INGAME_UI(rt,messageRecipientGroupsTab),
                                            INGAME_UI(rt,messageRecipientPlayersTab));
      g_InGameUiActionHandlersPage10.handlers[((UiSelectableControl *)recipientTab)->actionId & 0xff]
                (recipientTab);
    }
    UiKeyboardFocus_Set(INGAME_UI(rt,chatInputTextEdit));
    break;
  case 0x567340: /* Alt+F4: game menu on the quit page */
  case 0x5673a0: /* F2: game menu on the save page (local games only) */
  case 0x567410: /* Esc: game menu */
  case 0x567460: { /* F1: mission objectives */
    UiSelectableControl *toggle;
    if ((target == 0x5673a0) && !localSession) {
      break;
    }
    toggle = (UiSelectableControl *)(target == 0x567460 ? INGAME_UI(rt,missionObjectivesButton) :
                                                     INGAME_UI(rt,inGameMenuButton));
    UiSelectableControl_SetSelected(1,toggle);
    if (((toggle->stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) &&
        (((UiSpriteButtonControl *)toggle)->activationSound != nullptr)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,
                            ((UiSpriteButtonControl *)toggle)->activationSound,nullptr);
    }
    if (target == 0x567460) {
      InGameMissionHelpPage_Toggle((UiNodeBase *)toggle);
      break;
    }
    InGameSettingsPage_ToggleAndSynchronizeControls(toggle);
    if (target == 0x567340) {
      InGameQuitMenu_OpenAndRefreshButtons((InGameCommandPanelSourceAddress32)INGAME_UI(rt,
           gameMenuQuitButton));
    }
    else if (target == 0x5673a0) {
      InGameSaveGamePage_RebuildCatalog(INGAME_UI(rt,gameMenuSaveButton));
    }
    break;
  }
  case 0x5674b0: { /* C: toggle the message window (network games only) */
    UiPageStackControl *stack;
    uint32_t index;
    UiNodeBase *recipientTab;
    int i;
    if (localSession) {
      break;
    }
    stack = (UiPageStackControl *)INGAME_UI(rt,gameWindowPageStack);
    index = (UiPageStack_ActivePageIndex(stack) == 1) ? 0 : 1;
    UiPageStack_SetActiveIndex(index,stack);
    INGAME_UI(rt,worldView)->nodeFlags =
         INGAME_UI(rt,worldView)->nodeFlags & ~UI_NODE_SUPPRESSED;
    if (index != 1) {
      break;
    }
    INGAME_UI(rt,worldView)->nodeFlags =
         INGAME_UI(rt,worldView)->nodeFlags | UI_NODE_SUPPRESSED;
    UiKeyboardFocus_ReleaseNode(INGAME_UI(rt,worldView));
    ((UiTextEditControl *)INGAME_UI(rt,messageTextEdit))->cursorIndex = 0;
    ((UiTextEditControl *)INGAME_UI(rt,messageTextEdit))->selectionStart = 0;
    ((UiTextEditControl *)INGAME_UI(rt,messageTextEdit))->selectionEnd = 0;
    for (i = 0; i < 24; i++) {
      ((uint32_t *)((InGameCommandTextEditControlCC *)INGAME_UI(rt,messageTextEdit))->textBuffer)[i] = 0;
    }
    /* Original quirk: the result is not tested; with no tab selected this is the last tab */
    UiSelectableGroup_FindVisibleSelected(&recipientTab,nullptr,3,INGAME_UI(rt,messageRecipientAllTab),
                                          INGAME_UI(rt,messageRecipientGroupsTab),
                                          INGAME_UI(rt,messageRecipientPlayersTab));
    g_InGameUiActionHandlersPage10.handlers[((UiSelectableControl *)recipientTab)->actionId & 0xff]
              (recipientTab);
    g_KeyboardFlushEvents();
    break;
  }
  case 0x5675e0: /* P: pause */
    InGameCommand_Issue<InGameCommand_TogglePauseRequest>(0,0,0);
    break;
  case 0x567620: /* G: faster */
  case 0x567660: /* Alt+G: slower */ {
    int step = (target == 0x567620) ? 1 : -1;
    InGameCommand_Issue<InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks>(0,0,step);
    break;
  }
  case 0x5676a0: { /* Tab: hide or show the side panel; bit 2 of the map/mouse settings remembers it */
    uint32_t settings = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
    UiPageStackControl *stack = (UiPageStackControl *)INGAME_UI(rt,sidePanelStack);
    if (UiPageStack_ActivePageIndex(stack) != 0) {
      UiPageStack_SetActiveIndex(0,stack);
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,resourceBarModeStack));
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,gamePanelsModeStack));
      INGAME_UI(rt,worldViewArea)->rightOffset = INGAME_UI(rt,sidePanelFrameLeftEdge)->leftOffset;
      settings = settings & ~PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN;
    }
    else {
      UiPageStack_SetActiveIndex(1,stack);
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,resourceBarModeStack));
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,gamePanelsModeStack));
      INGAME_UI(rt,worldViewArea)->rightOffset = 0;
      settings = settings | PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN;
    }
    UiContainer_LayoutChildren((UiNodeBase *)rt);
    PersistentSettings_Write(settings,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
    break;
  }
  case 0x5677e0: /* Alt+P: screenshot to the next numbered PCX file */
    Screenshot_SaveFramebufferAsPcx();
    break;
  case 0x567870: /* Alt+Q: leave the game (not as host) */
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != 0) {
      break;
    }
    InGameCommand_Issue<InGameCommand_HandlePlayerDeparture>(0,0,0);
    break;
  default:
    Thandor_Log("EndGameResults dispatch: unhandled continuation %08x",target);
    break;
  }
  return false;
}
