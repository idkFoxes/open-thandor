/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/scenario/custom_project.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_ASSETS_SCENARIO_CUSTOM_PROJECT_H
#define THANDOR_ASSETS_SCENARIO_CUSTOM_PROJECT_H

/* Reading a map editor project (maps\<name>.thmap, docs/custom_maps.md) with the editor's project-format reader
   (third_party/map_editor_project_format): a ZIP with map.ini at the root ([map], format=1) or a legacy stored PCK
   with editor/project.txt (THANDOR_MAP_EDITOR_PROJECT_V1..V3). This is only the archive level: entries present and
   of the right type, the LEV's field grid path equal to the project's field entry, the FLD's size; the game's own
   checks (CustomMap_Validate, the field grid check) run in custom_maps.cpp on the bytes returned here.
   Native memory only (std containers), never the arena. The reader's exceptions stay inside CustomProject_Load. */

#include <stdint.h>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

/* Number of STR strings a project's level text holds for the game: 0 = title, 1 = description, 2.. = the further
   lines of the level's description block (TEXT_ID_LEVEL_DESCRIPTION_BASE + 0x10 * index + line - 1). */
inline constexpr uint32_t CUSTOM_PROJECT_TEXT_STRINGS = 16;

struct CustomProject {
  bool zip = false;               /* ZIP project (else legacy PCK) */
  std::vector<uint8_t> lev;       /* the level entry */
  std::vector<uint8_t> fld;       /* the field grid entry (expanded; converter 0x00070001 of old editors -> 0x00060006) */
  std::string levelEntry;         /* normalized entry paths ("level/<x>.lev", "level/<x>.fld"): lowercase, '/' */
  std::string fieldEntry;
  std::string displayName;        /* manifest display_name (UTF-8; empty when missing) */
  std::string author;             /* manifest author (UTF-8; empty when missing) */
  /* Terrain files the game may load from the project while its map runs (normalized path -> bytes): the
     gfx/boden/ entries with extension .gfx, .pal or .dat, plus the ground textures a ZIP project replaces by
     textures/terrain/a..z.tga (converted to .gfx). Everything else of the archive is not served. */
  std::map<std::string,std::vector<uint8_t>> servedAssets;
  /* The level text (string_entry), when the project has one: per country code of the STR's locale blocks (and
     key 0 = the block the editor prefers, 44 or the first) the raw UTF-16 units of the first
     CUSTOM_PROJECT_TEXT_STRINGS strings (missing strings empty). Unfiltered: may carry rich-text commands. */
  std::map<uint32_t,std::vector<std::vector<uint16_t>>> texts;
};

/* Reads file into *outProject. Returns false with a short English reason (for the log) when the archive cannot be
   read or does not hold a usable project. */
bool CustomProject_Load(const std::filesystem::path &file,CustomProject *outProject,std::string *outReason);

/* As CustomProject_Load, from the bytes of a project file (the self-test). */
bool CustomProject_LoadBytes(std::vector<uint8_t> bytes,CustomProject *outProject,std::string *outReason);

/* The size limit of a project file (larger files are rejected before they are read). */
uint64_t CustomProject_MaxFileBytes();

#endif /* THANDOR_ASSETS_SCENARIO_CUSTOM_PROJECT_H */
