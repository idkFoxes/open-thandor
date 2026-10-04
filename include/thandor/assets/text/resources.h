/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/text/resources.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_TEXT_RESOURCES_H
#define THANDOR_ASSETS_TEXT_RESOURCES_H

#include <thandor/assets/text/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* Text resource ids: a compact id (bits 16-23 zero) is page << 8 | index, an extended id page << 16 | index
   with a 16-bit index. TEXT_RESOURCE_ID_NONE resolves to the shared empty string; FontRuntime_Init
   fills the whole override table with it. */
#define TEXT_RESOURCE_ID_NONE 0xFFFFFFFF
#define TEXT_RESOURCE_OVERRIDE_CAPACITY 0x1000 /* entries of TextResourceOverrideTable */
/* Page 0x30 holds the text of the loaded level (its .str entry, see TextResourcePage_LoadCompatibilityAliases):
   index 0 the title, 1 the description, 2..15 fourteen further description lines. */
#define TEXT_RESOURCE_PAGE_LEVEL 0x30
#define TEXT_ID_LEVEL_PAGE_TITLE 0x3000
#define TEXT_ID_LEVEL_PAGE_DESCRIPTION 0x3001
#define TEXT_ID_LEVEL_PAGE_EXTRA_LINES 0x3002
#define TEXT_LEVEL_EXTRA_LINE_COUNT 14
/* Not in the original: texts of open-thandor's own controls (literal German, no locale data), resolved by
   TextResource_TryResolve before the override table. Page 0x7F (extended ids) is not used by the game data. */
#define TEXT_ID_PROJECT_BASE 0x7F0000
#define TEXT_ID_DISPLAY_MODE_KIND_TITLE (TEXT_ID_PROJECT_BASE + 0) /* "Anzeigemodus:" */
#define TEXT_ID_DISPLAY_MODE_KIND_WINDOW (TEXT_ID_PROJECT_BASE + 1) /* "Fenster" */
#define TEXT_ID_DISPLAY_MODE_KIND_BORDERLESS (TEXT_ID_PROJECT_BASE + 2) /* "Vollbildfenster" */
#define TEXT_ID_DISPLAY_MODE_KIND_FULLSCREEN (TEXT_ID_PROJECT_BASE + 3) /* "Vollbild" */
/* the options page's renamed "Graphik" button and display settings page title, and the advanced settings page */
#define TEXT_ID_OPTIONS_DISPLAY_BUTTON (TEXT_ID_PROJECT_BASE + 4) /* "Anzeige" */
#define TEXT_ID_FRONTEND_DISPLAY_TITLE (TEXT_ID_PROJECT_BASE + 5) /* "Anzeigeeinstellungen" */
#define TEXT_ID_OPTIONS_ADVANCED_BUTTON (TEXT_ID_PROJECT_BASE + 6) /* "Erweitert" */
#define TEXT_ID_ADVANCED_SETTINGS_TITLE (TEXT_ID_PROJECT_BASE + 7) /* "Erweiterte Einstellungen" */
#define TEXT_ID_ADVANCED_EDGES_TITLE (TEXT_ID_PROJECT_BASE + 8) /* "3D-Kanten:" */
#define TEXT_ID_ADVANCED_EDGES_SMOOTH (TEXT_ID_PROJECT_BASE + 9) /* "Glatt" */
#define TEXT_ID_ADVANCED_EDGES_EXACT (TEXT_ID_PROJECT_BASE + 10) /* "Original" */
#define TEXT_ID_ADVANCED_UI_SCALE_TITLE (TEXT_ID_PROJECT_BASE + 11) /* "UI-Skalierung:" */
#define TEXT_ID_ADVANCED_UI_SCALE_AUTO (TEXT_ID_PROJECT_BASE + 12) /* "Auto" */
#define TEXT_ID_ADVANCED_UI_SCALE_1 (TEXT_ID_PROJECT_BASE + 13) /* "1x" (2x, 3x follow) */
#define TEXT_ID_ADVANCED_FRAME_LIMIT_TITLE (TEXT_ID_PROJECT_BASE + 16) /* "Bildratenbegrenzung:" */
#define TEXT_ID_ADVANCED_FRAME_LIMIT_OFF (TEXT_ID_PROJECT_BASE + 17) /* "Aus" (30, 60, 120, 144 Bilder/s follow) */
#define TEXT_ID_ADVANCED_VSYNC (TEXT_ID_PROJECT_BASE + 22) /* "VSync" */
#define TEXT_ID_ADVANCED_NOTE_SOFTWARE (TEXT_ID_PROJECT_BASE + 23) /* edges and UI scale need a GPU renderer */
#define TEXT_ID_ADVANCED_NOTE_UI_SCALE (TEXT_ID_PROJECT_BASE + 24) /* the UI scale applies with the next mode switch */
#define TEXT_ID_PROJECT_COUNT 25

Bool8 TextResourcePage_LoadCompatibilityAliases(uint32_t levelTitleIndex,uint16_t *path);

Bool8 TextResourcePage_Load(TextResourcePageIndex pageIndex,uint16_t *path,uintptr_t *outLocaleBlockOrError);

void TextResourceOverride_Register(TextResourceId resourceId,uint16_t *text);

Bool8 TextResource_TryResolve(TextResourceId resourceId,uint16_t **outText);

/* TextResource_TryResolve without the found flag (a missing text gives TEXT_RESOURCE_MISSING_SENTINEL_0x33) */
uint16_t *TextResource_Resolve(TextResourceId resourceId);

extern TextResourceOverrideTable *g_TextResourceOverrides;

#endif /* THANDOR_ASSETS_TEXT_RESOURCES_H */
