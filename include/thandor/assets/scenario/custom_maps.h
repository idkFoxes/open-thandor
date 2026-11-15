/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/scenario/custom_maps.h
 */

#ifndef THANDOR_ASSETS_SCENARIO_CUSTOM_MAPS_H
#define THANDOR_ASSETS_SCENARIO_CUSTOM_MAPS_H

/* Not in the original: custom maps (docs/custom_maps.md), single player only. Every map editor project
   maps\<name>.thmap next to the executable (a ZIP with map.ini or a legacy stored PCK with editor/project.txt,
   custom_project.h) whose level and field grid pass the checks becomes a row of the "Single game" list in a local
   game. Custom maps never enter g_ScenarioCatalog, so a network host neither lists nor sends them. Everything here
   is native memory and native file calls (no arena): without a maps folder nothing changes.

   Text ids: a custom map gets the title index CUSTOM_MAP_TITLE_INDEX_BASE + slot (slot = hash of the lowercased name
   modulo CUSTOM_MAP_TITLE_INDEX_SLOTS, probing on), which the loaded .lev copy carries instead of its own; its title
   (TEXT_ID_LEVEL_TITLE_BASE + index, compact pages 0x25..0x2C) and description lines
   (TEXT_ID_LEVEL_DESCRIPTION_BASE + 0x10 * index + line) resolve through CustomMaps_TryResolveText. No game text page
   uses these ids. The slot of a name stays the same while the set of maps does not collide, so a saved game keeps
   its title.

   The level path of a custom map is maps\<name>.lev, which exists nowhere on disk: while the map is loaded or runs,
   its project is active (CustomMaps_ActivateLevelPath / CustomMaps_ActivateIdentifier) and the package loader takes
   that level, the project's field grid (level\<x>.fld, the path the level names) and its terrain files
   (gfx\boden\...) from it (CustomMaps_FindServedAsset). Without an active project nothing is served, so a project
   never shadows an original file outside its own map. The project is released when the session ends, when the
   scenario selection is built and when any other level is loaded.
   The loaded .lev copy of a custom map carries the identifier "maps\<name>." instead of its file's: the save game
   stores that copy, so a loaded save (and the restart after it) finds the project again.
   While "Original Thandor" is on (OT_FEATURE_CUSTOM_MAPS, core/settings/original_mode.h) maps\ is neither scanned
   nor listed (from the next list build on); a saved game of a custom map still loads and restarts (the paths above
   do not need the scan), its title then shows the fallback text. */

#include <stdint.h>
#include <string>
#include <thandor/core/ptr32.h>

struct ScenarioCatalogHeader;
struct ScenarioCatalogDisplayRecord;
struct FrontendLoadedLevelAsset;

inline constexpr int CUSTOM_MAP_LIMIT = 200;          /* further projects are logged and not listed */
inline constexpr int CUSTOM_MAP_NAME_MAX_UNITS = 30;  /* "<name>." fits the 32-unit catalog identifier */
inline constexpr uint32_t CUSTOM_MAP_TITLE_INDEX_BASE = 0x2D0; /* title id 0x2500 */
inline constexpr uint32_t CUSTOM_MAP_TITLE_INDEX_SLOTS = 0x800;
static_assert(CUSTOM_MAP_LIMIT <= (int)CUSTOM_MAP_TITLE_INDEX_SLOTS);

/* Scans maps\*.thmap again (called by ScenarioCatalog_Rebuild after the stock records): every project is checked
   (name, collision with a stock level, the archive, the field grid path, the game's field grid check,
   CustomMap_Validate) and listed or logged with the reason. catalog gives the stock identifiers. Releases the
   active project. */
void CustomMaps_Rebuild(const ScenarioCatalogHeader *catalog);

/* The number of listed custom maps, and the name of listed map index (nullptr past the end; the self-test). */
uint32_t CustomMaps_Count();
const char *CustomMaps_ListedName(uint32_t index);

/* The row pointers of the "Single game" list: stockRows (stockCount rows) followed by one display record per custom
   map with its column texts resolved. Returns stockRows unchanged when there is no custom map; otherwise a native
   array that stays valid until the next call. *outCount receives the row count. */
Ptr32<ScenarioCatalogDisplayRecord> *CustomMaps_ListRows(Ptr32<ScenarioCatalogDisplayRecord> *stockRows,
                                                         uint32_t stockCount,uint32_t *outCount);

/* When record is a custom map row: writes "maps\<name>." (the caller sets the extension) and returns true. */
bool CustomMaps_LevelPathOfRecord(uint16_t *destination,const void *record);

/* When identifier (a level header's levelFileNameUtf16) is a custom map's "maps\<name>.": writes "maps\<name>."
   and returns true. */
bool CustomMaps_LevelPathOfIdentifier(uint16_t *destination,const uint16_t *identifier);

/* True when identifier (terminated, at most 56 units) starts with "maps\" (a loaded custom map). */
bool CustomMaps_IsCustomIdentifier(const uint16_t *identifier);

/* After a level was loaded from levelPath: when the path is a custom map's maps\<name>.lev, sets the loaded
   copy's identifier to "maps\<name>." and its title index to the map's custom index. Other levels are left alone. */
void CustomMaps_PrepareLoadedLevel(FrontendLoadedLevelAsset *level,const uint16_t *levelPath);

/* Whether the level at levelPath (relative, with extension) can be loaded again: a custom map must still pass every
   check of the scan, any other level must exist in a mounted package or as a loose file. Logs the reason. */
bool CustomMaps_LevelPathPlayable(const uint16_t *levelPath);

/* Before a level is loaded from levelPath (relative, with extension): for a custom map's maps\<name>.lev makes its
   project the active one (read and checked again unless it is active and unchanged; false with a log line when it
   is gone or fails), for any other level releases the active project (true). */
bool CustomMaps_ActivateLevelPath(const uint16_t *levelPath);

/* At the start of a session on a level whose header identifier is identifier: as CustomMaps_ActivateLevelPath for
   a custom map's "maps\<name>." in a local game; releases the active project otherwise (a network game). */
bool CustomMaps_ActivateIdentifier(const uint16_t *identifier);

/* Releases the active project (the end of a session). */
void CustomMaps_Deactivate();

/* The package loader's hook: when a project is active and path (relative, terminated) is one of its served files
   (case ignored, '\' = '/'), stores its bytes and size and returns true. Never true without an active project. */
bool CustomMaps_FindServedAsset(const uint16_t *path,const uint8_t **outData,uint32_t *outBytes);

/* The self-test: the checks of the scan on maps\<name>.thmap (without the name and collision rules). */
bool CustomMaps_CheckProjectForTest(const std::string &name,std::string *outReason);

/* The text of a custom map title or description id in the current language (see above); false for every other id. */
bool CustomMaps_TryResolveText(uint32_t resourceId,uint16_t **outText);

/* Rewrites the list titles of the custom map rows for the current text language (Game_ApplyLanguage). */
void CustomMaps_RefreshLanguage();

#endif /* THANDOR_ASSETS_SCENARIO_CUSTOM_MAPS_H */
