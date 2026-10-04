/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/build_catalog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_BUILD_CATALOG_H
#define THANDOR_UI_INGAME_BUILD_CATALOG_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/build_catalog. */

/* Functions are grouped by semantic ownership. */

void InGameBuildCatalog_QueueOrCancelEntry(UiCatalogEntryControl *source);

void InGameSpecialBuildCatalog_QueueOrCancelEntry(UiCatalogEntryControl *source);

/* Build catalog (InGameBuildCatalog_RebuildGrid) and special build catalog (InGameSpecialBuildCatalog_RebuildGrid) */
#define BUILD_CATALOG_ENTRY_COUNT 48
#define BUILD_CATALOG_MAX_COLUMNS 8

#define SPECIAL_BUILD_CATALOG_ENTRY_COUNT 42
#define SPECIAL_BUILD_CATALOG_MAX_COLUMNS 6

/* assetFlags14 bits of a catalog record */
#define BUILD_CATALOG_ASSET_FLAG_BUILDABLE 0x1
#define BUILD_CATALOG_ASSET_FLAG_CAPABILITY_8 0x8    /* capability every model of runtime class 22 provides */

#define BUILD_CATALOG_ASSET_FLAG_SPECIAL 0x10        /* offered in the special build catalog */
#define BUILD_CATALOG_ASSET_CAPABILITY_MASK 0xEE     /* all capability bits of the normal build catalog */

void InGameBuildCatalog_RebuildGrid(UiNodeBase *node);

void InGameSpecialBuildCatalog_RebuildGrid(UiNodeBase *node);

extern UiCommandRuntimeRecordPrefix *g_UiCatalogGroup48Records[48];
extern UiCommandRuntimeRecordPrefix *g_UiCatalogGroup42Records[42];

#endif /* THANDOR_UI_INGAME_BUILD_CATALOG_H */
