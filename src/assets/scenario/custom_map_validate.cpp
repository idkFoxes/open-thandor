/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/scenario/custom_map_validate.cpp
 * Project code (not in the original game)
 */

/* Validation of user-made maps (.lev + expanded .fld) before the level loaders see them. Works on the raw bytes
   with explicit little-endian reads at the file offsets of docs/level_format.md and docs/field_grid_format.md, so
   it never depends on struct layout and never reads outside [data, data + size).

   What the loaders already bound themselves (checked here again only where a stricter rule is cheap, so a map is
   rejected with a reason instead of failing in the middle of the load):
     - LEV magic / converter version, prefix size >= 0x800 and <= the image, the nine resource paths terminated
       LEVEL_IMAGE_PATH_SUFFIX_UNITS units before the image end, local faction 1..7 (NewLevel_ValidateImage),
       active faction count 1..7 (InGameLevelRuntime_LoadResourcesAfterDefaultReset), assignable count 1..active
       (FrontendPlayer / task assignment), the field path offset <= 0xFFFF and terminated
       (FrontendLevelAsset_LoadedImageFits);
     - the level script: faction operands, trigger factions / conditions, postfix operands and end token
       (InGameLevelScript_Validate);
     - the loaded resource count (INGAME_LOADED_RESOURCE_CAPACITY), extension writes (WidePath_SetExtensionCode is
       bounded by 256 units), unknown army asset ids (FATAL_ERROR_ARMY_ID_NOT_FOUND), a full army pool;
     - FLD magic / converter, sides >= FIELD_GRID_MIN_SIDE_CELLS and header + cells <= the asset size
       (FieldGrid_ValidateLoadedImage), the map-edge flags (rebuilt at load), title text ids (a missing text
       resolves to the empty string), light angles (masked to 16 bits), the lighting cycle period.
   Checked here because the loaders use the value unchecked (out of bounds or overflow with a malformed map):
     - the EFF/SHT/MDL/ARM path tables: offset, count and every record (the loader walks them, rewrites each
       extension in place and advances by 32 units);
     - the placement table: offset + count * 0x20 inside the file (NewLevel_SpawnInitialArmies reads it unchecked),
       the owner faction < 8 (it indexes factionLifecycleStates), positions inside the field grid;
     - intro notification movie id: "flm\movie%03d.flm" gets the id written into a 17-unit array, so id + 4 must
       stay below 1000; music / level effect sample numbers: written with 2 digits into an 18-unit array (< 100);
     - player slot colour offsets: colorIndex = offset + faction selects the faction graphics (suffix letter) and
       the minimap faction colour (a palette index, 0x20 + 4 * bank + colorIndex);
     - end trigger end movie selection < 8 (CampaignLevelRecord.endMovieNumbers[8]);
     - FLD cells: world X/Y on the lattice the PCK decoder generates, material ids 0..25 with their texture set
       flagged, only known flag bits, heights in range, and zero in the runtime fields the loader does not reset
       but reads as indices (fog-of-war lighting index dword, occupancy mask, resource extraction descriptor).
   Paths: UTF-16, terminated, no '.', ':', '/', wildcards (except '?' in the sound pattern), no leading, doubled
   or trailing backslash, and a fixed directory prefix per field (gfx\, flm\, sound\, engine\ or level\, mdl\,
   arm\, sht\, eff\, each list also level\; the field grid path level\ or maps\). With '.' banned there is no ".." and no extension. */

#include <thandor/assets/scenario/custom_map_validate.h>
#include <thandor/thandor.h>
#include <thandor/gameplay/army/pool.h>
#include <thandor/gameplay/session/level_new.h>
#include <thandor/gameplay/session/level_script.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <iterator>

namespace {

/* ---- LEV layout (docs/level_format.md) ------------------------------------------------------------------------ */
constexpr uint32_t LEV_OFFSET_MAGIC = 0x000;
constexpr uint32_t LEV_OFFSET_ALLOCATION_SIZE = 0x004;
constexpr uint32_t LEV_OFFSET_FORMAT_VERSION = 0x008;
constexpr uint32_t LEV_OFFSET_CONVERTER_VERSION = 0x00C;
constexpr uint32_t LEV_OFFSET_PATH_OFFSETS = 0x0B0; /* ten path offsets, field grid path first */
constexpr uint32_t LEV_OFFSET_PLACEMENT_COUNT = 0x0D8;
constexpr uint32_t LEV_OFFSET_PREFIX_SIZE = 0x0DC; /* also the placement table offset */
constexpr uint32_t LEV_OFFSET_TABLES = 0x0E0; /* (count, offset) pairs: ARM, MDL, EFF, SHT */
constexpr uint32_t LEV_OFFSET_TITLE_INDEX = 0x170;
constexpr uint32_t LEV_OFFSET_PLAYER_SLOTS = 0x200;
constexpr uint32_t LEV_PLAYER_SLOT_BYTES = 0x20;
constexpr uint32_t LEV_PLAYER_SLOT_COUNT = 7;
constexpr uint32_t LEV_OFFSET_ASSIGNABLE_FACTIONS = 0x310;
constexpr uint32_t LEV_OFFSET_ACTIVE_FACTIONS = 0x314;
constexpr uint32_t LEV_OFFSET_INTRO_MOVIE = 0x324;
constexpr uint32_t LEV_OFFSET_MUSIC_SAMPLES = 0x350;
constexpr uint32_t LEV_OFFSET_EFFECT_SAMPLES = 0x360;
constexpr uint32_t LEV_OFFSET_CONDITIONS = 0x380;
constexpr uint32_t LEV_CONDITION_BYTES = 0x10;
constexpr uint32_t LEV_OFFSET_TRIGGERS = LEV_OFFSET_CONDITIONS + INGAME_SCHEDULED_CONDITION_COUNT * LEV_CONDITION_BYTES;
constexpr uint32_t LEV_TRIGGER_BYTES = 8;
constexpr uint32_t LEV_FIXED_PREFIX_BYTES = 0x800; /* sizeof(InGameLevelConditionStorage) */
constexpr uint32_t LEV_PATH_RECORD_BYTES = LEVEL_ASSET_PATH_RECORD_UNITS * sizeof(uint16_t);
/* units WidePath_SetExtensionCode writes from the terminator of a path without extension: '.', 3 letters, NUL */
constexpr uint32_t LEV_EXTENSION_UNITS = 5;
constexpr uint32_t LEV_PLACEMENT_BYTES = 0x20;
constexpr uint32_t LEV_FORMAT_VERSION = 1;
constexpr uint32_t LEV_FACTION_SLOTS = 8; /* neutral 0 + factions 1..7: the faction arrays of the runtime image */
constexpr uint32_t LEV_TITLE_INDEX_LIMIT = 0xD0; /* TEXT_ID_LEVEL_TITLE_BASE + index stays in text page 0x22xx */
constexpr uint32_t LEV_INTRO_MOVIE_COUNT = 5; /* ids n..n+4 are queued */
constexpr uint32_t LEV_NOTIFICATION_MOVIE_LIMIT = 1000; /* three digits of flm\movie%03d.flm */
constexpr uint32_t LEV_SAMPLE_NUMBER_LIMIT = 100; /* two digits of sound\music%02d.sam / level%02d.sam */
constexpr uint32_t LEV_START_RESOURCE_LIMIT_Q4 = 1000000u << 4; /* stock 0..104000 */
constexpr uint32_t LEV_COLOR_INDEX_ACTIVE_MAX = 8; /* stock colour indexes of existing factions: 1..8 */
constexpr uint32_t LEV_COLOR_INDEX_ANY_MAX = 15; /* minimap faction colours 0x20 + 4 * bank + colorIndex */
constexpr uint32_t LEV_END_MOVIE_SELECTION_COUNT = 8; /* CampaignLevelRecord.endMovieNumbers[8] */
constexpr int64_t LEV_CAMERA_MARGIN_Q12 = (int64_t)64 << Q12_SHIFT; /* stock cameras stand up to ~30 units off the grid */
constexpr int64_t LEV_CAMERA_MIN_Z_Q12 = -((int64_t)64 << Q12_SHIFT);
constexpr int64_t LEV_CAMERA_MAX_Z_Q12 = (int64_t)256 << Q12_SHIFT;

enum LevPathField : uint32_t {
  LEV_PATH_FIELD_GRID, LEV_PATH_GROUND, LEV_PATH_SURFACE, LEV_PATH_SKY, LEV_PATH_ARMY, LEV_PATH_SHOT, LEV_PATH_EFFECT,
  LEV_PATH_MOVIE, LEV_PATH_SOUND, LEV_PATH_TECHNOLOGY, LEV_PATH_COUNT
};
const char *const s_LevPathNames[LEV_PATH_COUNT] = {"field grid", "ground texture", "surface texture",
                                                    "sky texture",  "army texture",   "shot texture",
                                                    "effect texture", "movie",        "sound",
                                                    "technology"};
/* allowed directory prefixes (lower case, compared case-insensitively), at most two per field */
const char *const s_LevPathPrefixes[LEV_PATH_COUNT][2] = {
    {"level\\", "maps\\"}, {"gfx\\", nullptr},    {"gfx\\", nullptr},   {"gfx\\", nullptr},
    {"gfx\\", nullptr},    {"gfx\\", nullptr},    {"gfx\\", nullptr},   {"flm\\", nullptr},
    {"sound\\", nullptr},  {"engine\\", "level\\"}};

enum LevTable : uint32_t { LEV_TABLE_ARM, LEV_TABLE_MDL, LEV_TABLE_EFF, LEV_TABLE_SHT, LEV_TABLE_COUNT };
const char *const s_LevTableNames[LEV_TABLE_COUNT] = {"ARM", "MDL", "EFF", "SHT"};
/* plus level\ (the tutorial levels list level\tutorial\armunit00 etc.) */
const char *const s_LevTablePrefixes[LEV_TABLE_COUNT] = {"arm\\", "mdl\\", "eff\\", "sht\\"};

/* The army asset ids of enum PckArmyAssetIdCatalog (include/thandor/core/types.h) as inclusive ranges. */
struct ArmyIdRange {
  uint16_t first;
  uint16_t last;
};
constexpr ArmyIdRange s_ArmyAssetIdRanges[] = {
    {1, 3},     {10, 19},   {30, 39},   {50, 50},   {60, 67},   {80, 89},   {100, 105}, {110, 117},
    {130, 141}, {150, 157}, {170, 172}, {180, 188}, {200, 208}, {220, 224}, {240, 240}, {242, 242},
    {244, 244}, {250, 252}, {260, 275}, {280, 285}, {295, 295}, {300, 306}, {310, 310}, {318, 318},
    {320, 323}, {330, 333}, {340, 340}, {345, 345}, {350, 352}, {357, 366}, {370, 377}, {380, 382},
    {390, 392}, {395, 395}, {400, 440}, {500, 507}, {540, 563}, {580, 588}, {620, 642}, {647, 658},
    {660, 665}, {710, 713}, {750, 752}, {800, 803}, {805, 808}, {810, 811}, {814, 815}, {850, 855}};

/* ---- FLD layout (docs/field_grid_format.md) ------------------------------------------------------------------- */
constexpr uint32_t FLD_OFFSET_MAGIC = 0x00;
constexpr uint32_t FLD_OFFSET_ALLOCATION_SIZE = 0x04;
constexpr uint32_t FLD_OFFSET_FORMAT_VERSION = 0x08;
constexpr uint32_t FLD_OFFSET_CONVERTER_VERSION = 0x0C;
constexpr uint32_t FLD_OFFSET_FIELD_FLAGS = 0xB0;
constexpr uint32_t FLD_OFFSET_RUNTIME_FLAGS = 0xB4;
constexpr uint32_t FLD_OFFSET_WIDTH = 0xB8;
constexpr uint32_t FLD_OFFSET_HEIGHT = 0xBC;
constexpr uint32_t FLD_FORMAT_VERSION = 1;
constexpr uint32_t FLD_CELL_BYTES = 0x80;
constexpr uint32_t FLD_CELL_WORLD_X = 0x40;
constexpr uint32_t FLD_CELL_WORLD_Y = 0x44;
constexpr uint32_t FLD_CELL_HEIGHT = 0x48;
constexpr uint32_t FLD_CELL_WATER_DELTA = 0x4C;
constexpr uint32_t FLD_CELL_FLAGS = 0x50;
/* runtime fields the load does not reset but reads as indices: they must be zero in the file */
constexpr uint32_t FLD_CELL_ZERO_FIELDS[] = {0x68, 0x70, 0x74, 0x7C};
constexpr int64_t FLD_LATTICE_COLUMN_X = 0x901; /* worldX = column * 0x901 + row * 0x480 */
constexpr int64_t FLD_LATTICE_ROW_X = 0x480;
constexpr int64_t FLD_LATTICE_ROW_Y = -1999; /* worldY = row * -1999 */
constexpr uint32_t FLD_MATERIAL_COUNT = 26; /* gfx\boden\soila..z */
constexpr uint32_t FLD_RUNTIME_FLAGS_ALLOWED = 1; /* FIELD_GRID_RUNTIME_SURFACE_DIRTY */
constexpr int32_t FLD_HEIGHT_LIMIT_Q12 = 32 << Q12_SHIFT; /* stock heights: |h| <= 14.2 units */
constexpr int32_t FLD_WATER_DELTA_LIMIT_Q12 = 128 << Q12_SHIFT; /* stock water deltas: |d| <= 64.7 units */
/* material byte, random variant, Xenite / Tritium, map edges, editor mark, unresolved, fluid exclusions */
constexpr uint32_t FLD_CELL_FLAGS_ALLOWED = 0x000000FFu | 0x00000700u | 0x00000800u | 0x00001000u | 0x00002000u |
                                            0x00004000u | 0x00008000u | 0x08000000u | 0x10000000u | 0x20000000u |
                                            0x40000000u | 0x80000000u;

/* ---- helpers --------------------------------------------------------------------------------------------------- */

struct Reason {
  char *text;
  size_t size;
};

bool Fail(Reason &reason, const char *format, ...)
{
  if ((reason.text != nullptr) && (reason.size != 0)) {
    va_list args;
    va_start(args, format);
    vsnprintf(reason.text, reason.size, format, args);
    va_end(args);
  }
  return false;
}

uint32_t LoadU32(const uint8_t *data, size_t offset)
{
  return (uint32_t)data[offset] | (uint32_t)data[offset + 1] << 8 | (uint32_t)data[offset + 2] << 16 |
         (uint32_t)data[offset + 3] << 24;
}

int32_t LoadS32(const uint8_t *data, size_t offset)
{
  return (int32_t)LoadU32(data, offset);
}

uint16_t LoadU16(const uint8_t *data, size_t offset)
{
  return (uint16_t)(data[offset] | data[offset + 1] << 8);
}

/* A byte span [begin, end) of the level prefix that one path or table owns (no two may overlap). */
struct Span {
  uint32_t begin;
  uint32_t end;
  const char *name;
};

struct Spans {
  Span items[(uint32_t)LEV_PATH_COUNT + (uint32_t)LEV_TABLE_COUNT];
  size_t count = 0;
};

/* Checks one UTF-16 path at byte offset offset of the level, which may use the bytes [offset, limit): terminated
   with suffixUnits units from the terminator on (inside limit), at most maxUnits long, only
   allowed characters and one of the prefixes. *outEndBytes receives the end of the bytes the path and its
   in-place suffix writes use. */
bool CheckPath(const uint8_t *lev, uint32_t offset, uint32_t limit, uint32_t maxUnits, uint32_t suffixUnits,
               const char *const *prefixes, size_t prefixCount, bool allowWildcard, const char *name,
               uint32_t *outEndBytes, Reason &reason)
{
  if ((offset % 2) != 0) {
    return Fail(reason, "%s path offset 0x%X is odd", name, offset);
  }
  if ((offset < LEV_FIXED_PREFIX_BYTES) || (offset >= limit)) {
    return Fail(reason, "%s path offset 0x%X outside 0x%X..0x%X", name, offset, LEV_FIXED_PREFIX_BYTES, limit);
  }
  const uint32_t unitsAvailable = (limit - offset) / sizeof(uint16_t);
  uint32_t length = 0;
  while ((length < unitsAvailable) && (LoadU16(lev, offset + length * sizeof(uint16_t)) != 0)) {
    length++;
  }
  if (length == unitsAvailable) {
    return Fail(reason, "%s path at 0x%X not terminated", name, offset);
  }
  if (unitsAvailable - length < suffixUnits) {
    return Fail(reason, "%s path at 0x%X has no room for its extension", name, offset);
  }
  if ((length == 0) || (length > maxUnits)) {
    return Fail(reason, "%s path length %u outside 1..%u", name, length, maxUnits);
  }
  uint16_t previous = '\\'; /* a leading backslash counts as doubled */
  for (uint32_t i = 0; i < length; i++) {
    const uint16_t unit = LoadU16(lev, offset + i * sizeof(uint16_t));
    const bool latin1Letter = (unit >= 0xC0) && (unit <= 0xFF) && (unit != 0xD7) && (unit != 0xF7);
    const bool ascii = (unit >= 0x20) && (unit < 0x7F);
    if ((!ascii && !latin1Letter) || (unit == '.') || (unit == ':') || (unit == '/') || (unit == '*') ||
        (unit == '"') || (unit == '<') || (unit == '>') || (unit == '|') || ((unit == '?') && !allowWildcard)) {
      return Fail(reason, "%s path at 0x%X: character 0x%X not allowed", name, offset, unit);
    }
    if ((unit == '\\') && (previous == '\\')) {
      return Fail(reason, "%s path at 0x%X: leading or doubled backslash", name, offset);
    }
    previous = unit;
  }
  if (previous == '\\') {
    return Fail(reason, "%s path at 0x%X ends in a backslash", name, offset);
  }
  bool prefixFound = false;
  for (size_t p = 0; (p < prefixCount) && !prefixFound; p++) {
    if (prefixes[p] == nullptr) {
      continue;
    }
    const size_t prefixLength = strlen(prefixes[p]);
    if (prefixLength >= length) {
      continue;
    }
    prefixFound = true;
    for (size_t i = 0; i < prefixLength; i++) {
      uint16_t unit = LoadU16(lev, offset + i * sizeof(uint16_t));
      if ((unit >= 'A') && (unit <= 'Z')) {
        unit = (uint16_t)(unit + ('a' - 'A'));
      }
      if (unit != (uint8_t)prefixes[p][i]) {
        prefixFound = false;
        break;
      }
    }
  }
  if (!prefixFound) {
    return Fail(reason, "%s path at 0x%X must start with %s%s%s", name, offset, prefixes[0],
                (prefixCount > 1 && prefixes[1] != nullptr) ? " or " : "",
                (prefixCount > 1 && prefixes[1] != nullptr) ? prefixes[1] : "");
  }
  *outEndBytes = offset + (length + suffixUnits) * (uint32_t)sizeof(uint16_t);
  return true;
}

bool AddSpan(Spans &spans, uint32_t begin, uint32_t end, const char *name)
{
  if (begin >= end) {
    return true; /* empty table */
  }
  spans.items[spans.count++] = Span{begin, end, name};
  return true;
}

/* Grid position of a world point (Q12): true when it lies on the cell array (the border ring included; stock
   maps place scenery there). The lattice: worldX = column * 0x901 + row * 0x480, worldY = row * -1999. */
bool WorldPointOnGrid(int64_t worldX, int64_t worldY, uint32_t width, uint32_t height)
{
  const int64_t rowScaled = -worldY; /* row * 1999 */
  if ((rowScaled < 0) || (rowScaled > (int64_t)(height - 1) * -FLD_LATTICE_ROW_Y)) {
    return false;
  }
  /* column * 0x901 * 1999 = worldX * 1999 - row * 1999 * 0x480 */
  const int64_t columnScaled = worldX * -FLD_LATTICE_ROW_Y - rowScaled * FLD_LATTICE_ROW_X;
  return (columnScaled >= 0) && (columnScaled <= (int64_t)(width - 1) * FLD_LATTICE_COLUMN_X * -FLD_LATTICE_ROW_Y);
}

/* ---- FLD ------------------------------------------------------------------------------------------------------- */

bool ValidateField(const uint8_t *fld, size_t fldSize, uint32_t *outWidth, uint32_t *outHeight, Reason &reason)
{
  if (fld == nullptr) {
    return Fail(reason, "fld: no data");
  }
  if (fldSize < FIELD_GRID_HEADER_BYTES) {
    return Fail(reason, "fld: %llu bytes, shorter than the 0x%X-byte header", (unsigned long long)fldSize, FIELD_GRID_HEADER_BYTES);
  }
  if (LoadU32(fld, FLD_OFFSET_MAGIC) != (uint32_t)ASSET_MAGIC_FLD) {
    return Fail(reason, "fld: bad magic 0x%08X", LoadU32(fld, FLD_OFFSET_MAGIC));
  }
  if (LoadU32(fld, FLD_OFFSET_CONVERTER_VERSION) != (uint32_t)PCK_CONVERTER_FLD_SHT_00060006) {
    return Fail(reason, "fld: converter version 0x%08X", LoadU32(fld, FLD_OFFSET_CONVERTER_VERSION));
  }
  if (LoadU32(fld, FLD_OFFSET_FORMAT_VERSION) != FLD_FORMAT_VERSION) {
    return Fail(reason, "fld: format version %u", LoadU32(fld, FLD_OFFSET_FORMAT_VERSION));
  }
  const uint32_t width = LoadU32(fld, FLD_OFFSET_WIDTH);
  const uint32_t height = LoadU32(fld, FLD_OFFSET_HEIGHT);
  if ((width < CUSTOM_MAP_FIELD_MIN_SIDE_CELLS) || (width > CUSTOM_MAP_FIELD_MAX_SIDE_CELLS) ||
      (height < CUSTOM_MAP_FIELD_MIN_SIDE_CELLS) || (height > CUSTOM_MAP_FIELD_MAX_SIDE_CELLS) || (width % 4 != 3) ||
      (height % 4 != 3)) {
    return Fail(reason, "fld: grid %ux%u, sides must be 4k+3 in %u..%u", width, height,
                CUSTOM_MAP_FIELD_MIN_SIDE_CELLS, CUSTOM_MAP_FIELD_MAX_SIDE_CELLS);
  }
  const uint64_t expectedSize = FIELD_GRID_HEADER_BYTES + (uint64_t)width * height * FLD_CELL_BYTES;
  if ((uint64_t)fldSize != expectedSize) {
    return Fail(reason, "fld: %llu bytes, a %ux%u grid needs %llu", (unsigned long long)fldSize, width, height,
                (unsigned long long)expectedSize);
  }
  if (LoadU32(fld, FLD_OFFSET_ALLOCATION_SIZE) != expectedSize) {
    return Fail(reason, "fld: allocation size %u != file size %llu", LoadU32(fld, FLD_OFFSET_ALLOCATION_SIZE),
                (unsigned long long)expectedSize);
  }
  const uint32_t fieldFlags = LoadU32(fld, FLD_OFFSET_FIELD_FLAGS);
  if ((fieldFlags >> FLD_MATERIAL_COUNT) != 0) {
    return Fail(reason, "fld: material flags 0x%08X name materials above %u", fieldFlags, FLD_MATERIAL_COUNT - 1);
  }
  if ((LoadU32(fld, FLD_OFFSET_RUNTIME_FLAGS) & ~FLD_RUNTIME_FLAGS_ALLOWED) != 0) {
    return Fail(reason, "fld: runtime flags 0x%08X", LoadU32(fld, FLD_OFFSET_RUNTIME_FLAGS));
  }
  for (uint32_t row = 0; row < height; row++) {
    for (uint32_t column = 0; column < width; column++) {
      const size_t cell = FIELD_GRID_HEADER_BYTES + ((size_t)row * width + column) * FLD_CELL_BYTES;
      if ((LoadS32(fld, cell + FLD_CELL_WORLD_X) != (int64_t)column * FLD_LATTICE_COLUMN_X + (int64_t)row * FLD_LATTICE_ROW_X) ||
          (LoadS32(fld, cell + FLD_CELL_WORLD_Y) != (int64_t)row * FLD_LATTICE_ROW_Y)) {
        return Fail(reason, "fld: cell (%u,%u) world position not on the grid lattice", column, row);
      }
      const int32_t terrainHeight = LoadS32(fld, cell + FLD_CELL_HEIGHT);
      const int32_t waterDelta = LoadS32(fld, cell + FLD_CELL_WATER_DELTA);
      if ((terrainHeight < -FLD_HEIGHT_LIMIT_Q12) || (terrainHeight > FLD_HEIGHT_LIMIT_Q12) ||
          (waterDelta < -FLD_WATER_DELTA_LIMIT_Q12) || (waterDelta > FLD_WATER_DELTA_LIMIT_Q12)) {
        return Fail(reason, "fld: cell (%u,%u) height %d / water delta %d outside +-%d / +-%d (Q12)", column, row,
                    terrainHeight, waterDelta, FLD_HEIGHT_LIMIT_Q12, FLD_WATER_DELTA_LIMIT_Q12);
      }
      const uint32_t flags = LoadU32(fld, cell + FLD_CELL_FLAGS);
      const uint32_t material = flags & 0xFF;
      if ((flags & ~FLD_CELL_FLAGS_ALLOWED) != 0) {
        return Fail(reason, "fld: cell (%u,%u) unknown flag bits 0x%08X", column, row, flags & ~FLD_CELL_FLAGS_ALLOWED);
      }
      if ((material >= FLD_MATERIAL_COUNT) || ((fieldFlags & (1u << material)) == 0)) {
        return Fail(reason, "fld: cell (%u,%u) material %u not in the material flags 0x%08X", column, row, material,
                    fieldFlags);
      }
      for (uint32_t zeroField : FLD_CELL_ZERO_FIELDS) {
        if (LoadU32(fld, cell + zeroField) != 0) {
          return Fail(reason, "fld: cell (%u,%u) runtime field +0x%02X not zero", column, row, zeroField);
        }
      }
    }
  }
  *outWidth = width;
  *outHeight = height;
  return true;
}

/* ---- LEV: level script ------------------------------------------------------------------------------------------ */

/* Faction operands per condition kind (as InGameScheduledCondition_FactionOperandCount); -1 = unknown kind. */
int ConditionFactionOperands(uint32_t kind)
{
  switch (kind) {
  case INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED:
  case INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED:
  case INGAME_SCHEDULED_CONDITION_NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A:
  case INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION:
    return 0;
  case INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8:
    return 2;
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY:
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_COMMAND_GROUP_A_ARMY:
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY_OF_ASSET:
  case INGAME_SCHEDULED_CONDITION_XENITE_AT_LEAST:
  case INGAME_SCHEDULED_CONDITION_TRITIUM_AT_LEAST:
  case INGAME_SCHEDULED_CONDITION_TRITIUM_EXTRACTION_RATE_AT_LEAST:
  case INGAME_SCHEDULED_CONDITION_ARMY_OF_ASSET_COUNT_AT_LEAST:
  case INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST: /* op0: byte in the 8-byte mask */
  case INGAME_SCHEDULED_CONDITION_XENITE_STORAGE_LIMIT_AT_MOST_0FA0:
    return 1;
  default:
    return -1;
  }
}

bool ValidateScript(const uint8_t *lev, Reason &reason)
{
  for (uint32_t index = 0; index < INGAME_SCHEDULED_CONDITION_COUNT; index++) {
    const uint32_t record = LEV_OFFSET_CONDITIONS + index * LEV_CONDITION_BYTES;
    const uint32_t kind = lev[record] & INGAME_SCHEDULED_CONDITION_KIND_MASK;
    const int factionOperands = ConditionFactionOperands(kind);
    if (factionOperands < 0) {
      return Fail(reason, "script: condition %u has unknown kind %u", index, kind);
    }
    for (int operand = 0; operand < factionOperands; operand++) {
      const uint32_t faction = LoadU32(lev, record + 4 + 4 * (uint32_t)operand);
      if (faction >= LEV_FACTION_SLOTS) {
        return Fail(reason, "script: condition %u faction operand %u out of range", index, faction);
      }
    }
    if (kind != INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION) {
      continue;
    }
    bool ended = false;
    for (uint32_t byte = 1; (byte < LEV_CONDITION_BYTES) && !ended; byte++) {
      const uint8_t token = lev[record + byte];
      if (token == INGAME_CONDITION_TOKEN_END) {
        ended = true;
      }
      else if ((token < INGAME_CONDITION_TOKEN_NOT) && (token >= INGAME_SCHEDULED_CONDITION_COUNT)) {
        return Fail(reason, "script: condition %u postfix operand %u out of range", index, token);
      }
    }
    if (!ended) {
      return Fail(reason, "script: condition %u postfix expression without end token", index);
    }
  }
  for (uint32_t index = 0; index < INGAME_END_CONDITION_TRIGGER_COUNT; index++) {
    const uint32_t record = LEV_OFFSET_TRIGGERS + index * LEV_TRIGGER_BYTES;
    const uint8_t stateFlags = lev[record + 0];
    const uint8_t endMovieSelection = lev[record + 5];
    const uint8_t faction = lev[record + 4];
    const uint8_t condition = lev[record + 6];
    if (stateFlags > (uint8_t)INGAME_END_CONDITION_TRIGGER_ACTIVE) {
      return Fail(reason, "script: end trigger %u state flags %u", index, stateFlags);
    }
    if ((stateFlags != 0) && ((faction < 1) || (faction >= LEV_FACTION_SLOTS) ||
                              (condition >= INGAME_SCHEDULED_CONDITION_COUNT) ||
                              (endMovieSelection >= LEV_END_MOVIE_SELECTION_COUNT))) {
      return Fail(reason, "script: end trigger %u faction %u / condition %u / end movie %u out of range", index,
                  faction, condition, endMovieSelection);
    }
  }
  return true;
}

/* ---- LEV ------------------------------------------------------------------------------------------------------- */

bool ValidateLevel(const uint8_t *lev, size_t levSize, uint32_t width, uint32_t height, Reason &reason)
{
  if (lev == nullptr) {
    return Fail(reason, "lev: no data");
  }
  if (levSize < LEV_FIXED_PREFIX_BYTES) {
    return Fail(reason, "lev: %llu bytes, shorter than the 0x%X-byte fixed part", (unsigned long long)levSize, LEV_FIXED_PREFIX_BYTES);
  }
  if (LoadU32(lev, LEV_OFFSET_MAGIC) != (uint32_t)ASSET_MAGIC_LEV) {
    return Fail(reason, "lev: bad magic 0x%08X", LoadU32(lev, LEV_OFFSET_MAGIC));
  }
  if (LoadU32(lev, LEV_OFFSET_CONVERTER_VERSION) != (uint32_t)PCK_CONVERTER_LEV_00070001) {
    return Fail(reason, "lev: converter version 0x%08X", LoadU32(lev, LEV_OFFSET_CONVERTER_VERSION));
  }
  if (LoadU32(lev, LEV_OFFSET_FORMAT_VERSION) != LEV_FORMAT_VERSION) {
    return Fail(reason, "lev: format version %u", LoadU32(lev, LEV_OFFSET_FORMAT_VERSION));
  }
  if (LoadU32(lev, LEV_OFFSET_ALLOCATION_SIZE) != levSize) {
    return Fail(reason, "lev: allocation size %u != file size %llu", LoadU32(lev, LEV_OFFSET_ALLOCATION_SIZE), (unsigned long long)levSize);
  }
  const uint32_t prefix = LoadU32(lev, LEV_OFFSET_PREFIX_SIZE);
  const uint32_t placementCount = LoadU32(lev, LEV_OFFSET_PLACEMENT_COUNT);
  if ((prefix < LEV_FIXED_PREFIX_BYTES) || (prefix > CUSTOM_MAP_LEVEL_MAX_PREFIX_BYTES) || ((prefix % 4) != 0)) {
    return Fail(reason, "lev: prefix size 0x%X not a multiple of 4 in 0x%X..0x%X", prefix, LEV_FIXED_PREFIX_BYTES,
                CUSTOM_MAP_LEVEL_MAX_PREFIX_BYTES);
  }
  if (placementCount > CUSTOM_MAP_LEVEL_MAX_PLACEMENTS) {
    return Fail(reason, "lev: %u placements, at most %u", placementCount, CUSTOM_MAP_LEVEL_MAX_PLACEMENTS);
  }
  if ((uint64_t)prefix + (uint64_t)placementCount * LEV_PLACEMENT_BYTES != levSize) {
    return Fail(reason, "lev: prefix 0x%X + %u placements * 0x20 != file size %llu", prefix, placementCount, (unsigned long long)levSize);
  }

  /* resource paths: inside [0x800, prefix), terminated, prefix per field */
  Spans spans;
  for (uint32_t field = 0; field < LEV_PATH_COUNT; field++) {
    const uint32_t offset = LoadU32(lev, LEV_OFFSET_PATH_OFFSETS + 4 * field);
    uint32_t end = 0;
    /* the field grid path only gets ".fld" (FrontendLevelAsset_LoadedImageFits), the others also a suffix letter */
    const uint32_t suffixUnits = (field == LEV_PATH_FIELD_GRID) ? LEV_EXTENSION_UNITS : LEVEL_IMAGE_PATH_SUFFIX_UNITS;
    if (!CheckPath(lev, offset, prefix, CUSTOM_MAP_LEVEL_MAX_PATH_UNITS, suffixUnits, s_LevPathPrefixes[field],
                   std::size(s_LevPathPrefixes[field]), field == LEV_PATH_SOUND, s_LevPathNames[field], &end,
                   reason)) {
      return false;
    }
    AddSpan(spans, offset, end, s_LevPathNames[field]);
  }

  /* EFF/SHT/MDL/ARM file lists: 0x40-byte records inside [0x800, prefix) */
  uint32_t totalRecords = 0;
  for (uint32_t table = 0; table < LEV_TABLE_COUNT; table++) {
    const uint32_t count = LoadU32(lev, LEV_OFFSET_TABLES + 8 * table);
    const uint32_t offset = LoadU32(lev, LEV_OFFSET_TABLES + 8 * table + 4);
    if (count > CUSTOM_MAP_LEVEL_MAX_TABLE_RECORDS) {
      return Fail(reason, "lev: %s list has %u files, at most %u", s_LevTableNames[table], count,
                  CUSTOM_MAP_LEVEL_MAX_TABLE_RECORDS);
    }
    totalRecords += count;
    if (count == 0) {
      continue;
    }
    const uint64_t end = (uint64_t)offset + (uint64_t)count * LEV_PATH_RECORD_BYTES;
    if ((offset < LEV_FIXED_PREFIX_BYTES) || (end > prefix) || ((offset % 2) != 0)) {
      return Fail(reason, "lev: %s list 0x%X..0x%llX outside 0x%X..0x%X", s_LevTableNames[table], offset,
                  (unsigned long long)end, LEV_FIXED_PREFIX_BYTES, prefix);
    }
    for (uint32_t record = 0; record < count; record++) {
      const uint32_t recordOffset = offset + record * LEV_PATH_RECORD_BYTES;
      const char *const prefixes[2] = {s_LevTablePrefixes[table], "level\\"};
      uint32_t pathEnd = 0;
      /* the extension is written inside the record: the record end is the limit */
      if (!CheckPath(lev, recordOffset, recordOffset + LEV_PATH_RECORD_BYTES, LEVEL_ASSET_PATH_RECORD_UNITS,
                     LEV_EXTENSION_UNITS, prefixes, 2, false, s_LevTableNames[table], &pathEnd, reason)) {
        return false;
      }
    }
    AddSpan(spans, offset, (uint32_t)end, s_LevTableNames[table]);
  }
  if (totalRecords > (uint32_t)INGAME_LOADED_RESOURCE_CAPACITY) {
    return Fail(reason, "lev: %u listed files, at most %d", totalRecords, INGAME_LOADED_RESOURCE_CAPACITY);
  }
  std::sort(spans.items, spans.items + spans.count,
            [](const Span &a, const Span &b) { return a.begin < b.begin; });
  for (size_t i = 1; i < spans.count; i++) {
    if (spans.items[i].begin < spans.items[i - 1].end) {
      return Fail(reason, "lev: %s at 0x%X overlaps %s at 0x%X", spans.items[i].name, spans.items[i].begin,
                  spans.items[i - 1].name, spans.items[i - 1].begin);
    }
  }

  /* header, factions, player slots, samples */
  const uint32_t titleIndex = LoadU32(lev, LEV_OFFSET_TITLE_INDEX);
  if (titleIndex >= LEV_TITLE_INDEX_LIMIT) {
    return Fail(reason, "lev: title text index %u, at most %u", titleIndex, LEV_TITLE_INDEX_LIMIT - 1);
  }
  const uint32_t assignable = LoadU32(lev, LEV_OFFSET_ASSIGNABLE_FACTIONS);
  const uint32_t active = LoadU32(lev, LEV_OFFSET_ACTIVE_FACTIONS);
  if ((assignable < 1) || (assignable > active) || (active > LEV_PLAYER_SLOT_COUNT)) {
    return Fail(reason, "lev: faction counts assignable %u / active %u, need 1 <= assignable <= active <= 7",
                assignable, active);
  }
  const int64_t gridMaxX = (int64_t)(width - 1) * FLD_LATTICE_COLUMN_X + (int64_t)(height - 1) * FLD_LATTICE_ROW_X;
  const int64_t gridMinY = (int64_t)(height - 1) * FLD_LATTICE_ROW_Y;
  for (uint32_t slot = 0; slot < LEV_PLAYER_SLOT_COUNT; slot++) {
    const uint32_t record = LEV_OFFSET_PLAYER_SLOTS + slot * LEV_PLAYER_SLOT_BYTES;
    const int64_t cameraX = LoadS32(lev, record + 0x00);
    const int64_t cameraY = LoadS32(lev, record + 0x04);
    const int64_t cameraZ = LoadS32(lev, record + 0x08);
    const uint32_t faction = slot + 1;
    if ((cameraX < -LEV_CAMERA_MARGIN_Q12) || (cameraX > gridMaxX + LEV_CAMERA_MARGIN_Q12) ||
        (cameraY < gridMinY - LEV_CAMERA_MARGIN_Q12) || (cameraY > LEV_CAMERA_MARGIN_Q12) ||
        (cameraZ < LEV_CAMERA_MIN_Z_Q12) || (cameraZ > LEV_CAMERA_MAX_Z_Q12)) {
      return Fail(reason, "lev: faction %u start camera far outside the map", faction);
    }
    if ((LoadU32(lev, record + 0x14) > LEV_START_RESOURCE_LIMIT_Q4) ||
        (LoadU32(lev, record + 0x18) > LEV_START_RESOURCE_LIMIT_Q4)) {
      return Fail(reason, "lev: faction %u start resources above %u", faction, LEV_START_RESOURCE_LIMIT_Q4 >> 4);
    }
    const int64_t colorIndex = (int64_t)LoadS32(lev, record + 0x1C) + faction;
    const int64_t colorMin = (faction <= active) ? 1 : 0;
    const int64_t colorMax = (faction <= active) ? LEV_COLOR_INDEX_ACTIVE_MAX : LEV_COLOR_INDEX_ANY_MAX;
    if ((colorIndex < colorMin) || (colorIndex > colorMax)) {
      return Fail(reason, "lev: faction %u colour offset %d gives colour %lld outside %lld..%lld", faction,
                  LoadS32(lev, record + 0x1C), (long long)colorIndex, (long long)colorMin, (long long)colorMax);
    }
  }
  const uint32_t introMovie = LoadU32(lev, LEV_OFFSET_INTRO_MOVIE);
  if ((introMovie != 0) && (introMovie > LEV_NOTIFICATION_MOVIE_LIMIT - LEV_INTRO_MOVIE_COUNT)) {
    return Fail(reason, "lev: intro movie id %u, at most %u", introMovie,
                LEV_NOTIFICATION_MOVIE_LIMIT - LEV_INTRO_MOVIE_COUNT);
  }
  for (uint32_t i = 0; i < 4; i++) {
    if ((LoadU32(lev, LEV_OFFSET_MUSIC_SAMPLES + 4 * i) >= LEV_SAMPLE_NUMBER_LIMIT) ||
        (LoadU32(lev, LEV_OFFSET_EFFECT_SAMPLES + 4 * i) >= LEV_SAMPLE_NUMBER_LIMIT)) {
      return Fail(reason, "lev: music / effect sample number above %u", LEV_SAMPLE_NUMBER_LIMIT - 1);
    }
  }
  if (!ValidateScript(lev, reason)) {
    return false;
  }

  /* initial army placements */
  for (uint32_t i = 0; i < placementCount; i++) {
    const size_t record = (size_t)prefix + (size_t)i * LEV_PLACEMENT_BYTES;
    const uint32_t assetId = LoadU32(lev, record + 0x00);
    const uint32_t faction = LoadU32(lev, record + 0x04);
    const int32_t worldX = LoadS32(lev, record + 0x08);
    const int32_t worldY = LoadS32(lev, record + 0x0C);
    const uint32_t rotation = LoadU32(lev, record + 0x10);
    bool knownAsset = false;
    for (const ArmyIdRange &range : s_ArmyAssetIdRanges) {
      knownAsset = knownAsset || ((assetId >= range.first) && (assetId <= range.last));
    }
    if (!knownAsset) {
      return Fail(reason, "lev: placement %u has unknown army asset id %u", i, assetId);
    }
    if (faction >= LEV_FACTION_SLOTS) {
      return Fail(reason, "lev: placement %u owner faction %u outside 0..7", i, faction);
    }
    if (!WorldPointOnGrid(worldX, worldY, width, height)) {
      return Fail(reason, "lev: placement %u at (%d, %d) outside the %ux%u grid", i, worldX, worldY, width, height);
    }
    if ((rotation > 0xFFFFu) && (rotation < 0xFFFF0000u)) {
      return Fail(reason, "lev: placement %u rotation 0x%08X is not a 16-bit angle", i, rotation);
    }
  }
  return true;
}

} // namespace

bool CustomMap_Validate(const uint8_t *lev, size_t levSize, const uint8_t *fld, size_t fldSize, char *reason,
                        size_t reasonSize)
{
  Reason reasonOut{reason, reasonSize};
  uint32_t width = 0;
  uint32_t height = 0;

  if ((reason != nullptr) && (reasonSize != 0)) {
    reason[0] = 0;
  }
  /* the grid first: the level's positions are checked against its size */
  return ValidateField(fld, fldSize, &width, &height, reasonOut) &&
         ValidateLevel(lev, levSize, width, height, reasonOut);
}
