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
#define TEXT_ID_PROJECT_COUNT 4

Bool8 TextResourcePage_LoadCompatibilityAliases(uint32_t levelTitleIndex,uint16_t *path);

Bool8 TextResourcePage_Load(TextResourcePageIndex pageIndex,uint16_t *path,uintptr_t *outLocaleBlockOrError);

void TextResourceOverride_Register(TextResourceId resourceId,uint16_t *text);

Bool8 TextResource_TryResolve(TextResourceId resourceId,uint16_t **outText);

/* TextResource_TryResolve without the found flag (a missing text gives TEXT_RESOURCE_MISSING_SENTINEL_0x33) */
uint16_t *TextResource_Resolve(TextResourceId resourceId);

extern TextResourceOverrideTable *g_TextResourceOverrides;

#endif /* THANDOR_ASSETS_TEXT_RESOURCES_H */
