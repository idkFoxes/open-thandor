/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/hud.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_HUD_H
#define THANDOR_UI_INGAME_HUD_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/hud. */

/* Diplomacy panel texts (InGameOtherPlayerCommand_RebuildTargetEntries, see the diplomacyRow* labels in
   ui_templates.h): the player number is 0x2190 + faction index, the relation label 0x21A3 + the 4-bit
   relation state of the faction record's packedRelationStates. */
#define TEXT_ID_PLAYER_NUMBER_BASE 0x2190
#define TEXT_ID_DIPLOMATIC_RELATION_BASE 0x21A3

/* Faction status lines of the HUD (InGameHud_UpdateStatusCountersAndSessionPrompts): template with the faction
   name (selector 0), the roster (1) and the score (2); the roster text with the player list (selector 0), or
   the text used without players */
#define TEXT_ID_FACTION_STATUS_TEMPLATE 0x21D2
#define TEXT_ID_FACTION_ROSTER_TEMPLATE 0x21D3
#define TEXT_ID_FACTION_NO_ROSTER 0x21D4

/* Player status lines of a network game (InGamePanel_RebuildPlayerStatusRows), by readyOrWaitState zero or not;
   selector 0 = player name */
#define TEXT_ID_PLAYER_STATUS_STATE_ZERO 0xFF05
#define TEXT_ID_PLAYER_STATUS_STATE_SET 0xFF06

/* Functions are grouped by semantic ownership. */

void InGameMapAction_RecenterViewFromGridCoordinates(UiNodeBase *mapControl);

void InGameHud_UpdateStatusCountersAndSessionPrompts(void);

void InGamePanel_RebuildPlayerStatusRows(void *inGameRoot);

void InGameOtherPlayerCommand_RebuildTargetEntries(UiNodeBase *node);

void InGameOtherPlayerCommand_DispatchSelectedTarget(UiCommandSpriteButtonControl *control);

extern uint32_t g_RenderedFrameCountSinceDebugRefresh;

extern uint16_t *g_InGamePlayerListTextScratchUtf16;
extern InGamePlayerStatusTextSlot g_InGamePlayerStatusTextSlots[8];
extern uint16_t g_EmptyFrontendPlayerNameUtf16[1];

extern uint16_t g_FrontendDebugOverlayTextSlot10Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot11Utf16[16];
extern uint32_t g_InGameReadyStateToggleFlags;

extern uint16_t *g_InGameFactionStatusTextScratchUtf16;

void FrontendRuntime_UpdateCurrentFactionMetricCache(void);

extern uint16_t g_FrontendCurrentFactionPrimaryResourceTextUtf16[16]; /* decimal xenite amount, bound to a template text control */

#endif /* THANDOR_UI_INGAME_HUD_H */
