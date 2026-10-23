/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/settings/persistent.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <thandor/core/settings/persistent.h>
#include <thandor/thandor.h>
#include <thandor/core/bytes.h>
#include <thandor/platform/bootstrap/image.h>

/*
open-thandor: the settings live in a readable thandor.ini instead of the original's 200-byte thandor.dat.
The game code still works on the 200-byte image through PersistentSettings_Read/Write/WriteBlock/
GetRegionOrFallback; only loading and saving changed:
- Load builds the image from thandor.ini (current directory, then the executable directory). Each dword the
  ini sets is marked present; Read and GetRegionOrFallback return the caller's default for dwords that are not
  present, exactly like the original for bytes beyond the end of a short thandor.dat.
- Without a thandor.ini an existing thandor.dat is loaded as before (migration; its loaded bytes are the
  present ones, except the dwords the original never used, 0x50..0x5B, and values outside the key table's
  ranges) and the image is marked dirty, so the next Flush writes thandor.ini. thandor.dat is never
  written or deleted.
- Flush writes thandor.ini (when dirty, as before) with every key that is present or was written since the
  load, in the fixed order of the key table below, and with what the loaded thandor.ini had beyond the table
  (unknown keys, foreign sections; PersistentSettingsIniExtras), so keys of a newer version survive a run of an
  older one.
Without either file the original kept no image (all defaults, nothing saved); now an empty image (nothing
present, so all defaults) is kept, so settings changed in the menus are saved to a new thandor.ini.
*/

/* Module data. */

static PersistentSettingsRuntime g_PersistentSettings = {.path = {'t', 'h', 'a', 'n', 'd', 'o', 'r', '.', 'd', 'a', 't'}};

uint32_t g_LocaleCountryCodeOverride = 0;

/* open-thandor: the ini file name and the path it was found at (or will be written to) */
static const uint16_t s_PersistentSettingsIniLeaf[] = {'t', 'h', 'a', 'n', 'd', 'o', 'r', '.', 'i', 'n', 'i', 0};
/* THANDOR_PATH_CAPACITY units: with a long game directory a 0x100-unit copy cut <exe dir>\thandor.ini off, and the
   flush then wrote the settings to the cut name */
static uint16_t s_PersistentSettingsIniPath[THANDOR_PATH_CAPACITY];
/* bit i: the dword at byte offset 4 * i was loaded (Read returns it instead of the default) */
static uint64_t s_PersistentSettingsPresentMask;
/* bit i: the dword at byte offset 4 * i was written since the load (Flush saves it) */
static uint64_t s_PersistentSettingsWrittenMask;

#define PERSISTENT_SETTINGS_DWORD_COUNT (PERSISTENT_SETTINGS_IMAGE_BYTES / 4)
constexpr auto PERSISTENT_SETTINGS_INI_MAX_BYTES = 0x10000; /* written ini text: the full key set needs about 3 KB,
                                                           the kept unknown lines at most 32 KB */
constexpr auto PERSISTENT_SETTINGS_GAIN_FULL = 0x8000; /* Q15 gain of 100 % */

/* thandor.ini key table: one entry per key, in the order they are written. */

enum {
  INI_KIND_UINT,  /* unsigned decimal */
  INI_KIND_INT,   /* signed decimal */
  INI_KIND_HEX,   /* option bits, written as 0x... */
  INI_KIND_BOOL,  /* the whole dword 0/1, written as false/true */
  INI_KIND_BIT,   /* one bit (mask) of a dword shared by several keys; defaultValue = the dword when keys are missing */
  INI_KIND_GAIN,  /* Q15 volume, decimal; the comment also shows the percentage */
  INI_KIND_ENUM,  /* one of names[] (index = value) */
  INI_KIND_NAME,  /* PERSISTENT_SETTINGS_NAME_BYTES of UTF-16, written as quoted UTF-8 */
  INI_KIND_RAW    /* reserved dword of the original file, written as 0x... and only when nonzero */
};

struct PersistentIniKey {
  uint32_t offset;
  uint32_t kind;
  const char *section;
  const char *key;
  const char *comment;
  uint32_t mask;         /* INI_KIND_BIT */
  uint32_t defaultValue; /* INI_KIND_BIT */
  const char *const *names;
  uint32_t nameCount;
  const struct PersistentIniRange *range; /* the accepted values; nullptr: those of the kind (see below) */
};

/* open-thandor: a value outside this range is not loaded (the game's default is used, one log line), see
   PersistentIni_ValueAllowed. The limits are those the readers can work with and include every value the
   game's own menus write (the slider ranges of the settings pages, the listed display modes). */
struct PersistentIniRange {
  int64_t minimum;
  int64_t maximum;
};

/* the display modes start at 640x480 (ListDisplayModes); 16384 is far beyond any display */
static const PersistentIniRange s_IniRangeDisplayWidth = {640, 16384};
static const PersistentIniRange s_IniRangeDisplayHeight = {480, 16384};
/* nothing reads the adapter index any more (SdlVideo_SaveAdapterIndex writes 0); the original's DirectDraw
   adapter list was short */
static const PersistentIniRange s_IniRangeAdapter = {0, 15};
/* polygonDetailSlider / modelDetailSlider 0x4000..0x40000 */
static const PersistentIniRange s_IniRangeModelDetail = {0x4000, 0x40000};
/* larger values were read as no limit by LoadFramePacing; the menu offers 0, 60, 120 and 144 */
static const PersistentIniRange s_IniRangeFrameLimit = {0, PERSISTENT_FRAME_LIMIT_MAX};
/* gameSpeedSlider 80..120 */
static const PersistentIniRange s_IniRangeGameSpeed = {80, 120};
/* scrollSpeedSlider 8..128 (options page and in-game) */
static const PersistentIniRange s_IniRangeScrollStep = {8, 128};
/* maxPlayersSlider 2..8 */
static const PersistentIniRange s_IniRangePlayers = {2, 8};

static const char *const s_IniRendererNames[] = {"vulkan", "d3d12", "software"};
static const char *const s_IniDisplayModeNames[] = {"fullscreen", "borderless", "window"};
static const char *const s_IniTextureQualityNames[] = {"high", "medium", "low"};
static const char *const s_IniGpuRasterizationNames[] = {"smooth", "exact"};
static const char *const s_IniUiScaleNames[] = {"auto", "1", "2", "3"};
static const char *const s_IniVsyncNames[] = {"on", "off"};

#define INI_ENUM(names) names, (uint32_t)(sizeof names / sizeof names[0])
#define INI_RANGE(range) nullptr, 0, &range

static const PersistentIniKey s_PersistentIniKeys[] = {
  {PERSISTENT_SETTING_ADAPTER_INDEX, INI_KIND_UINT, "display", "adapter",
   "graphics adapter, 0 = the first (default 0)", 0, 0, INI_RANGE(s_IniRangeAdapter)},
  {PERSISTENT_SETTING_DISPLAY_WIDTH, INI_KIND_UINT, "display", "width",
   "screen width in pixels (default 640)", 0, 0, INI_RANGE(s_IniRangeDisplayWidth)},
  {PERSISTENT_SETTING_DISPLAY_HEIGHT, INI_KIND_UINT, "display", "height",
   "screen height in pixels (default 480)", 0, 0, INI_RANGE(s_IniRangeDisplayHeight)},
  {PERSISTENT_SETTING_BITS_PER_PIXEL, INI_KIND_UINT, "display", "bits_per_pixel",
   "colour depth in bits, always 32 (an older 16 is read as 32)", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_RENDERER, INI_KIND_ENUM, "display", "renderer",
   "renderer: vulkan (default), d3d12 or software", 0, 0, INI_ENUM(s_IniRendererNames)},
  {PERSISTENT_SETTING_DISPLAY_MODE_KIND, INI_KIND_ENUM, "display", "display_mode",
   "fullscreen (default), borderless or window", 0, 0, INI_ENUM(s_IniDisplayModeNames)},

  {PERSISTENT_SETTING_SHADING_ENABLED, INI_KIND_BOOL, "graphics", "shading",
   "terrain shading (default true)", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE, INI_KIND_UINT, "graphics", "shading_grid_half_size",
   "shading grid half size (default 32)", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION, INI_KIND_UINT, "graphics", "shading_texture_size",
   "shading texture size, 2 x shading_grid_half_size (default 64)", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT, INI_KIND_UINT, "graphics", "shading_depth",
   "shading depth (default 16)", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_TEXTURE_QUALITY, INI_KIND_ENUM, "graphics", "texture_quality",
   "texture quality: high, medium or low", 0, 0, INI_ENUM(s_IniTextureQualityNames)},
  {PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD, INI_KIND_INT, "graphics", "model_detail",
   "model detail distance, 8.8 fixed point (default 65536)", 0, 0, INI_RANGE(s_IniRangeModelDetail)},
  {PERSISTENT_SETTING_GPU_RASTERIZATION, INI_KIND_ENUM, "graphics", "gpu_rasterization",
   "Vulkan / DirectX 12 triangles: smooth (default; sub-pixel, perspective-correct like the original's Direct3D) "
   "or exact (the software renderer's look)", 0, 0, INI_ENUM(s_IniGpuRasterizationNames)},
  {PERSISTENT_SETTING_UI_SCALE, INI_KIND_ENUM, "graphics", "ui_scale",
   "Vulkan / DirectX 12 UI scale: auto (default; the largest whole factor that keeps the UI at least 1280x720) "
   "or 1, 2, 3; the UI resolution is the display mode divided by it, the window / fullscreen size stays the mode", 0, 0,
   INI_ENUM(s_IniUiScaleNames)},
  {PERSISTENT_SETTING_VSYNC, INI_KIND_ENUM, "graphics", "vsync",
   "vsync: on (default; frames wait for the display's refresh, no tearing) or off", 0, 0, INI_ENUM(s_IniVsyncNames)},
  {PERSISTENT_SETTING_FRAME_LIMIT, INI_KIND_UINT, "graphics", "frame_limit",
   "frame rate limit in frames per second: 0 (default, no limit), 60, 120, 144; at least 60 recommended (below 60 "
   "the game runs slower, its steps wait for drawn frames)", 0, 0, INI_RANGE(s_IniRangeFrameLimit)},

  {PERSISTENT_SETTING_SOUND_OPTION_FLAGS, INI_KIND_BIT, "sound", "effects",
   "sound effects (default true)", ToBits(PERSISTENT_SOUND_OPTION_EFFECTS), ToBits(PERSISTENT_SOUND_OPTION_DEFAULT), nullptr, 0},
  {PERSISTENT_SETTING_SOUND_OPTION_FLAGS, INI_KIND_BIT, "sound", "music",
   "music (default true)", ToBits(PERSISTENT_SOUND_OPTION_MUSIC), ToBits(PERSISTENT_SOUND_OPTION_DEFAULT), nullptr, 0},
  {PERSISTENT_SETTING_SOUND_OPTION_FLAGS, INI_KIND_BIT, "sound", "reverse_stereo",
   "swap the left and right channel (default false)", ToBits(PERSISTENT_SOUND_OPTION_REVERSE_STEREO),
   ToBits(PERSISTENT_SOUND_OPTION_DEFAULT), nullptr, 0},
  {PERSISTENT_SETTING_EFFECTS_GAIN, INI_KIND_GAIN, "sound", "effects_volume",
   "sound effects volume", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_MUSIC_GAIN, INI_KIND_GAIN, "sound", "music_volume",
   "music volume", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN, INI_KIND_GAIN, "sound", "movie_volume",
   "movie volume", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN, INI_KIND_GAIN, "sound", "movie_alternate_volume",
   "second movie volume", 0, 0, nullptr, 0},

  {PERSISTENT_SETTING_GAME_SPEED_PERCENT, INI_KIND_UINT, "game", "speed_percent",
   "game speed in percent (default 100)", 0, 0, INI_RANGE(s_IniRangeGameSpeed)},
  {PERSISTENT_SETTING_CAMERA_SCROLL_STEP, INI_KIND_INT, "game", "scroll_step",
   "map scroll speed (default 32)", 0, 0, INI_RANGE(s_IniRangeScrollStep)},
  {PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS, INI_KIND_HEX, "game", "map_mouse_options",
   "bits: 0x1 automatic zoom off, 0x2 automatic rotation off, 0x4 right button does not scroll / side panel "
   "hidden (default 0x0)", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS, INI_KIND_HEX, "game", "link_panel_options",
   "bits: 0x1 link rotation with zoom, 0x2 link rotation with tilt, 0x4 hide panel (default 0x0)", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_PLAYER_NAME, INI_KIND_NAME, "game", "player_name",
   "player name, at most 20 characters", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_GAME_NAME, INI_KIND_NAME, "game", "game_name",
   "name of a hosted network game, at most 20 characters", 0, 0, nullptr, 0},
  {PERSISTENT_SETTING_LOCALE_COUNTRY_CODE, INI_KIND_UINT, "game", "language_country_code",
   "text language as telephone country code (49 German, 44 English, ...), 0 = from Windows", 0, 0, nullptr, 0},

  {PERSISTENT_SETTING_NETWORK_PLAYER_COUNT, INI_KIND_UINT, "network", "players",
   "player count of a hosted network game (default 4)", 0, 0, INI_RANGE(s_IniRangePlayers)},

  {0x50, INI_KIND_RAW, "reserved", "dword_50", "unused in the original settings file, kept", 0, 0, nullptr, 0},
  {0x54, INI_KIND_RAW, "reserved", "dword_54", "unused in the original settings file, kept", 0, 0, nullptr, 0},
  {0x58, INI_KIND_RAW, "reserved", "dword_58", "unused in the original settings file, kept", 0, 0, nullptr, 0},
};

#define PERSISTENT_INI_KEY_COUNT (sizeof s_PersistentIniKeys / sizeof s_PersistentIniKeys[0])

/* Whether value may be loaded for key; logs why not. Without a range of its own a key accepts what its kind can
   hold: a switch 0/1, a gain 0..0x8000 (the volume sliders' range; more would amplify past full scale), an
   enumeration only its names (index 0..nameCount - 1, also given as number), the others any 32-bit value. Not
   checked here: names, the option bits and the reserved dwords (any value is harmless), the shading values
   (they must match each other, PersistentSettings_ValidateShading) and the colour depth (normalized). */
static bool PersistentIni_ValueAllowed(const PersistentIniKey *key, uint32_t value)
{
  int64_t number = key->kind == INI_KIND_INT ? (int64_t)(int32_t)value : (int64_t)value;
  int64_t minimum = key->kind == INI_KIND_INT ? INT32_MIN : 0;
  int64_t maximum = key->kind == INI_KIND_INT ? INT32_MAX : UINT32_MAX;

  switch (key->kind) {
  case INI_KIND_BOOL:
    maximum = 1;
    break;
  case INI_KIND_GAIN:
    maximum = PERSISTENT_SETTINGS_GAIN_FULL;
    break;
  case INI_KIND_ENUM:
    maximum = (int64_t)key->nameCount - 1;
    break;
  default:
    break;
  }
  if (key->range != nullptr) {
    minimum = key->range->minimum;
    maximum = key->range->maximum;
  }
  if (number < minimum || number > maximum) {
    Thandor_Log("settings: [%s] %s = %lld not %lld..%lld, default used", key->section, key->key, (long long)number,
                (long long)minimum, (long long)maximum);
    return false;
  }
  return true;
}

static uint64_t PersistentSettings_DwordMask(uint32_t offset, uint32_t byteCount);
static uint32_t PersistentIni_GetDword(const uint8_t *image, uint32_t offset);

/* Width and height form one display mode: when one of them is not allowed, the other is dropped too, so the
   default mode is used and not a mix of the default and the saved one. Returns the reduced presentMask. */
static uint64_t PersistentIni_DropIncompleteMode(uint64_t presentMask, uint64_t rejectedMask)
{
  uint64_t modeMask = PersistentSettings_DwordMask(PERSISTENT_SETTING_DISPLAY_WIDTH, 4) |
                      PersistentSettings_DwordMask(PERSISTENT_SETTING_DISPLAY_HEIGHT, 4);

  if ((rejectedMask & modeMask) != 0) {
    presentMask &= ~modeMask;
  }
  return presentMask;
}

/* Drops the loaded values the key table does not allow from presentMask (thandor.dat, whose bytes are not
   parsed); returns the reduced mask. */
static uint64_t PersistentSettings_DropDisallowed(const uint8_t *image, uint64_t presentMask)
{
  uint64_t rejectedMask = 0;
  uint32_t index;

  for (index = 0; index < PERSISTENT_INI_KEY_COUNT; index++) {
    const PersistentIniKey *key = &s_PersistentIniKeys[index];
    uint64_t mask = PersistentSettings_DwordMask(key->offset, 4);

    if (key->kind == INI_KIND_NAME || key->kind == INI_KIND_BIT || key->kind == INI_KIND_HEX ||
        key->kind == INI_KIND_RAW || (presentMask & mask) != mask) {
      continue;
    }
    if (!PersistentIni_ValueAllowed(key, PersistentIni_GetDword(image, key->offset))) {
      presentMask &= ~mask;
      rejectedMask |= mask;
    }
  }
  return PersistentIni_DropIncompleteMode(presentMask, rejectedMask);
}

/* Bits of the dwords [offset, offset + byteCount) in a present/written mask. */
static uint64_t PersistentSettings_DwordMask(uint32_t offset, uint32_t byteCount)
{
  uint64_t mask = 0;
  uint32_t dword;

  for (dword = offset / 4; dword < (offset + byteCount + 3) / 4 && dword < PERSISTENT_SETTINGS_DWORD_COUNT;
       dword++) {
    mask |= (uint64_t)1 << dword;
  }
  return mask;
}

/* Whether every dword of [offset, offset + byteCount) was loaded (the original: within the file length). */
static bool PersistentSettings_IsPresent(uint32_t offset, uint32_t byteCount)
{
  uint64_t mask;

  if (offset + byteCount > PERSISTENT_SETTINGS_IMAGE_BYTES) {
    return false;
  }
  mask = PersistentSettings_DwordMask(offset, byteCount);
  return (s_PersistentSettingsPresentMask & mask) == mask;
}

static uint32_t PersistentIni_GetDword(const uint8_t *image, uint32_t offset)
{
  return Thandor_LoadU32(image + offset);
}

static void PersistentIni_SetDword(uint8_t *image, uint32_t offset, uint32_t value)
{
  Thandor_StoreU32(image + offset, value);
}

/* The ini text being written: at most capacity - 1 characters (the caller's buffer keeps room for the NUL);
   what does not fit is silently dropped. */
struct PersistentIniWriter {
  std::string text;
  uint32_t capacity;
};

static void PersistentIni_Append(PersistentIniWriter *writer, const char *format, ...)
{
  va_list args;
  va_list measureArgs;
  int written;

  if (writer->text.size() + 1 >= writer->capacity) {
    return;
  }
  va_start(args, format);
  va_copy(measureArgs, args);
  written = vsnprintf(nullptr, 0, format, measureArgs);
  va_end(measureArgs);
  if (written > 0) {
    std::vector<char> formatted((size_t)written + 1);
    vsnprintf(formatted.data(), formatted.size(), format, args);
    writer->text.append(formatted.data(), (size_t)written);
    if (writer->text.size() >= writer->capacity) {
      writer->text.resize(writer->capacity - 1);
    }
  }
  va_end(args);
}

/* One code point as NUL-terminated UTF-8 into encoded (5 bytes). */
static void PersistentIni_EncodeUtf8(uint32_t codePoint, char *encoded)
{
  if (codePoint < 0x80) {
    encoded[0] = (char)codePoint;
    encoded[1] = 0;
  }
  else if (codePoint < 0x800) {
    encoded[0] = (char)(0xC0 | (codePoint >> 6));
    encoded[1] = (char)(0x80 | (codePoint & 0x3F));
    encoded[2] = 0;
  }
  else if (codePoint < 0x10000) {
    encoded[0] = (char)(0xE0 | (codePoint >> 12));
    encoded[1] = (char)(0x80 | ((codePoint >> 6) & 0x3F));
    encoded[2] = (char)(0x80 | (codePoint & 0x3F));
    encoded[3] = 0;
  }
  else {
    encoded[0] = (char)(0xF0 | (codePoint >> 18));
    encoded[1] = (char)(0x80 | ((codePoint >> 12) & 0x3F));
    encoded[2] = (char)(0x80 | ((codePoint >> 6) & 0x3F));
    encoded[3] = (char)(0x80 | (codePoint & 0x3F));
    encoded[4] = 0;
  }
}

/* UTF-16 name field -> UTF-8 (up to the first NUL; a lone surrogate is encoded like a code point). */
static void PersistentIni_AppendName(PersistentIniWriter *writer, const uint8_t *field)
{
  uint32_t unitCount = PERSISTENT_SETTINGS_NAME_BYTES / 2;
  uint32_t index = 0;
  char encoded[5];

  while (index < unitCount) {
    uint16_t unit;
    uint32_t codePoint;

    unit = Thandor_LoadU16(field + index * 2);
    if (unit == 0) {
      break;
    }
    codePoint = unit;
    index++;
    if (unit >= 0xD800 && unit < 0xDC00 && index < unitCount) {
      uint16_t low;

      low = Thandor_LoadU16(field + index * 2);
      if (low >= 0xDC00 && low < 0xE000) {
        codePoint = 0x10000 + (((uint32_t)unit - 0xD800) << 10) + (low - 0xDC00);
        index++;
      }
    }
    PersistentIni_EncodeUtf8(codePoint, encoded);
    PersistentIni_Append(writer, "%s", encoded);
  }
}

static bool PersistentIni_Equals(const char *begin, const char *end, const char *name);

/* The kept extras entry of section [begin, end) (known or foreign), or nullptr. */
static const PersistentSettingsIniExtras::Section *PersistentIni_FindExtras(const PersistentSettingsIniExtras *extras,
                                                                           const char *begin, const char *end)
{
  if (extras == nullptr) {
    return nullptr;
  }
  for (const PersistentSettingsIniExtras::Section &section : extras->sections) {
    if (PersistentIni_Equals(begin, end, section.name.c_str())) {
      return &section;
    }
  }
  return nullptr;
}

/* Writes one key of the table (its comment and its line). */
static void PersistentIni_AppendKey(PersistentIniWriter *writer, const uint8_t *image, const PersistentIniKey *key,
                                    uint32_t value)
{
  if (key->kind == INI_KIND_GAIN) {
    uint32_t tenths = (uint32_t)(((uint64_t)value * 1000 + PERSISTENT_SETTINGS_GAIN_FULL / 2) /
                                 PERSISTENT_SETTINGS_GAIN_FULL);
    PersistentIni_Append(writer, "; %s, 0..32768 = 0..100 %% (default 32768); here %u.%u %%\r\n",
                         key->comment, tenths / 10, tenths % 10);
  }
  else {
    PersistentIni_Append(writer, "; %s\r\n", key->comment);
  }
  PersistentIni_Append(writer, "%s = ", key->key);
  switch (key->kind) {
  case INI_KIND_INT:
    PersistentIni_Append(writer, "%d", (int32_t)value);
    break;
  case INI_KIND_HEX:
  case INI_KIND_RAW:
    PersistentIni_Append(writer, "0x%X", value);
    break;
  case INI_KIND_BOOL:
    if (value <= 1) {
      PersistentIni_Append(writer, "%s", value != 0 ? "true" : "false");
    }
    else {
      PersistentIni_Append(writer, "%u", value);
    }
    break;
  case INI_KIND_BIT:
    PersistentIni_Append(writer, "%s", (value & key->mask) != 0 ? "true" : "false");
    break;
  case INI_KIND_ENUM:
    if (value < key->nameCount) {
      PersistentIni_Append(writer, "%s", key->names[value]);
    }
    else {
      PersistentIni_Append(writer, "%u", value);
    }
    break;
  case INI_KIND_NAME:
    PersistentIni_Append(writer, "\"");
    PersistentIni_AppendName(writer, image + key->offset);
    PersistentIni_Append(writer, "\"");
    break;
  default: /* INI_KIND_UINT, INI_KIND_GAIN */
    PersistentIni_Append(writer, "%u", value);
    break;
  }
  PersistentIni_Append(writer, "\r\n");
}

/* Writes the kept lines of an extras section. */
static void PersistentIni_AppendExtraLines(PersistentIniWriter *writer, const PersistentSettingsIniExtras::Section *section)
{
  for (const std::string &line : section->lines) {
    PersistentIni_Append(writer, "%s\r\n", line.c_str());
  }
}

/* Writes the ini text of every key whose dword is in presentMask, section by section in the order the sections
   first appear in the key table; after the keys of a section its kept unknown keys, and after the last section
   the kept foreign sections (in file order, so a rewrite keeps them where they were relative to each other; the
   game's own sections come first). Returns the text length (no NUL counted). */
uint32_t PersistentSettings_FormatIniWithExtras(const uint8_t *image, uint64_t presentMask,
                                                const PersistentSettingsIniExtras *extras, char *out,
                                                uint32_t capacity)
{
  PersistentIniWriter writer = {std::string(), capacity};
  uint32_t sectionIndex;
  uint32_t index;

  if (capacity == 0) {
    return 0;
  }
  PersistentIni_Append(&writer,
      "; Open Thandor settings. The game rewrites this file when a setting changes in its menus.\r\n"
      "; A missing key means the game's default; unknown keys and sections are kept as they are.\r\n");
  for (sectionIndex = 0; sectionIndex < PERSISTENT_INI_KEY_COUNT; sectionIndex++) {
    const char *section = s_PersistentIniKeys[sectionIndex].section;
    const PersistentSettingsIniExtras::Section *kept;
    bool headerWritten = false;

    for (index = 0; index < sectionIndex && strcmp(s_PersistentIniKeys[index].section, section) != 0; index++) {
    }
    if (index < sectionIndex) {
      continue; /* written with its first key */
    }
    for (index = sectionIndex; index < PERSISTENT_INI_KEY_COUNT; index++) {
      const PersistentIniKey *key = &s_PersistentIniKeys[index];
      uint32_t byteCount = key->kind == INI_KIND_NAME ? PERSISTENT_SETTINGS_NAME_BYTES : 4;
      uint64_t mask = PersistentSettings_DwordMask(key->offset, byteCount);
      uint32_t value;

      if (strcmp(key->section, section) != 0 || (presentMask & mask) != mask) {
        continue;
      }
      value = PersistentIni_GetDword(image, key->offset);
      if (key->kind == INI_KIND_RAW && value == 0) {
        continue;
      }
      if (!headerWritten) {
        PersistentIni_Append(&writer, "\r\n[%s]\r\n", section);
        headerWritten = true;
      }
      PersistentIni_AppendKey(&writer, image, key, value);
    }
    kept = PersistentIni_FindExtras(extras, section, section + strlen(section));
    if (kept != nullptr && kept->known && !kept->lines.empty()) {
      if (!headerWritten) {
        PersistentIni_Append(&writer, "\r\n[%s]\r\n", section);
      }
      PersistentIni_AppendExtraLines(&writer, kept);
    }
  }
  if (extras != nullptr) {
    for (const PersistentSettingsIniExtras::Section &kept : extras->sections) {
      if (!kept.known) {
        PersistentIni_Append(&writer, "\r\n[%s]\r\n", kept.name.c_str());
        PersistentIni_AppendExtraLines(&writer, &kept);
      }
    }
  }
  std::copy(writer.text.begin(), writer.text.end(), out);
  out[writer.text.size()] = 0;
  return (uint32_t)writer.text.size();
}

/* open-thandor: the unknown keys and foreign sections of the loaded thandor.ini (written back by every Flush). */
static PersistentSettingsIniExtras s_PersistentSettingsIniExtras;

uint32_t PersistentSettings_FormatIni(const uint8_t *image, uint64_t presentMask, char *out, uint32_t capacity)
{
  return PersistentSettings_FormatIniWithExtras(image, presentMask, &s_PersistentSettingsIniExtras, out, capacity);
}

static bool PersistentIni_IsSpace(char c)
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

/* Trims [*begin, *end) in place. */
static void PersistentIni_Trim(const char **begin, const char **end)
{
  while (*begin < *end && PersistentIni_IsSpace(**begin)) {
    (*begin)++;
  }
  while (*end > *begin && PersistentIni_IsSpace((*end)[-1])) {
    (*end)--;
  }
}

static bool PersistentIni_Equals(const char *begin, const char *end, const char *name)
{
  size_t length = (size_t)(end - begin);

  return strlen(name) == length && _strnicmp(begin, name, length) == 0;
}

/* Parses an integer (decimal, -decimal or 0x hex) filling all of [begin, end). */
static bool PersistentIni_ParseNumber(const char *begin, const char *end, uint32_t *outValue)
{
  char text[64];
  char *parseEnd;
  size_t length = (size_t)(end - begin);
  bool negative = false;
  const char *digits;
  unsigned long long magnitude;

  if (length == 0 || length >= sizeof text) {
    return false;
  }
  std::copy(begin, end, text);
  text[length] = 0;
  digits = text;
  if (*digits == '-' || *digits == '+') {
    negative = *digits == '-';
    digits++;
  }
  if (*digits < '0' || *digits > '9') {
    return false;
  }
  if (digits[0] == '0' && (digits[1] == 'x' || digits[1] == 'X')) {
    magnitude = strtoull(digits + 2, &parseEnd, 16);
    if (parseEnd == digits + 2) {
      return false;
    }
  }
  else {
    magnitude = strtoull(digits, &parseEnd, 10);
  }
  if (*parseEnd != 0 || magnitude > 0xFFFFFFFFull) {
    return false;
  }
  *outValue = negative ? (uint32_t)(0u - (uint32_t)magnitude) : (uint32_t)magnitude;
  return true;
}

static bool PersistentIni_ParseBool(const char *begin, const char *end, uint32_t *outValue)
{
  static const char *const trueNames[] = {"true", "yes", "on"};
  static const char *const falseNames[] = {"false", "no", "off"};
  uint32_t index;

  for (index = 0; index < 3; index++) {
    if (PersistentIni_Equals(begin, end, trueNames[index])) {
      *outValue = 1;
      return true;
    }
    if (PersistentIni_Equals(begin, end, falseNames[index])) {
      *outValue = 0;
      return true;
    }
  }
  return PersistentIni_ParseNumber(begin, end, outValue);
}

/* Q15 gain: a plain number, or a percentage like "50%" or "37.5 %". */
static bool PersistentIni_ParseGain(const char *begin, const char *end, uint32_t *outValue)
{
  const char *cursor;
  uint32_t whole = 0;
  uint32_t tenths = 0;
  uint32_t digitCount = 0;

  if (end > begin && end[-1] == '%') {
    end--;
    PersistentIni_Trim(&begin, &end);
    for (cursor = begin; cursor < end && *cursor >= '0' && *cursor <= '9'; cursor++) {
      if (++digitCount > 5) {
        return false;
      }
      whole = whole * 10 + (uint32_t)(*cursor - '0');
    }
    if (cursor < end && (*cursor == '.' || *cursor == ',')) {
      cursor++;
      if (cursor < end && *cursor >= '0' && *cursor <= '9') {
        tenths = (uint32_t)(*cursor - '0');
        cursor++;
      }
      while (cursor < end && *cursor >= '0' && *cursor <= '9') {
        cursor++;
      }
    }
    if (digitCount == 0 || cursor != end) {
      return false;
    }
    *outValue = (uint32_t)(((uint64_t)(whole * 10 + tenths) * PERSISTENT_SETTINGS_GAIN_FULL + 500) / 1000);
    return true;
  }
  return PersistentIni_ParseNumber(begin, end, outValue);
}

/* UTF-8 (invalid bytes taken as Latin-1) -> the zero-padded UTF-16 name field; stops before a character that
   no longer fits. */
static void PersistentIni_ParseName(const char *begin, const char *end, uint8_t *field)
{
  uint32_t unitCount = PERSISTENT_SETTINGS_NAME_BYTES / 2;
  uint32_t written = 0;
  const uint8_t *cursor = reinterpret_cast<const uint8_t *>(begin);
  const uint8_t *limit = reinterpret_cast<const uint8_t *>(end);

  std::fill_n(field, PERSISTENT_SETTINGS_NAME_BYTES, 0);
  while (cursor < limit) {
    uint32_t codePoint = *cursor;
    uint32_t extra = 0;
    uint32_t index;
    uint16_t units[2];
    uint32_t units16;

    if (codePoint >= 0xF0 && codePoint < 0xF5) {
      extra = 3;
      codePoint &= 0x07;
    }
    else if (codePoint >= 0xE0 && codePoint < 0xF0) {
      extra = 2;
      codePoint &= 0x0F;
    }
    else if (codePoint >= 0xC2 && codePoint < 0xE0) {
      extra = 1;
      codePoint &= 0x1F;
    }
    if (extra != 0) {
      if ((uint32_t)(limit - cursor) <= extra) {
        extra = 0;
      }
      for (index = 1; index <= extra; index++) {
        if ((cursor[index] & 0xC0) != 0x80) {
          extra = 0;
          break;
        }
        codePoint = (codePoint << 6) | (cursor[index] & 0x3F);
      }
      if (extra == 0) {
        codePoint = *cursor; /* not UTF-8: Latin-1 */
      }
    }
    cursor += 1 + extra;
    if (codePoint >= 0x10000) {
      units[0] = (uint16_t)(0xD800 + ((codePoint - 0x10000) >> 10));
      units[1] = (uint16_t)(0xDC00 + ((codePoint - 0x10000) & 0x3FF));
      units16 = 2;
    }
    else {
      units[0] = (uint16_t)codePoint;
      units16 = 1;
    }
    if (units[0] == 0 || written + units16 > unitCount) {
      break;
    }
    for (index = 0; index < units16; index++) {
      Thandor_StoreU16(field + (written + index) * 2, units[index]);
    }
    written += units16;
  }
}

const uint16_t *PersistentSettings_IniPathUtf16()
{
  return s_PersistentSettingsIniPath;
}

/* Upper bound of the kept unknown lines (PersistentSettingsIniExtras::byteCount), so that the rewritten file
   still fits PERSISTENT_SETTINGS_INI_MAX_BYTES with every key of the table. */
constexpr size_t PERSISTENT_SETTINGS_INI_EXTRAS_MAX_BYTES = 0x8000;

/* open-thandor: an editor may save thandor.ini as UTF-16 (Notepad's Unicode); parsed as bytes every key would be
   missed and the next rewrite would replace the file with defaults. UTF-16 is recognised by its byte order mark,
   or without one when every second byte of the start is 0 (the keys are ASCII, and UTF-8 text has no zero
   bytes); it is converted to UTF-8 into converted (a lone surrogate becomes U+FFFD, a NUL ends the text).
   False when the text is not UTF-16. */
static bool PersistentIni_DecodeUtf16(const char *text, uint32_t length, std::string *converted)
{
  const uint8_t *bytes = reinterpret_cast<const uint8_t *>(text);
  uint32_t probe = std::min<uint32_t>(length & ~1u, 64);
  bool bigEndian;
  uint32_t offset = 0;
  uint32_t index;
  char encoded[5];

  if (length >= 2 && bytes[0] == 0xFF && bytes[1] == 0xFE) {
    bigEndian = false;
    offset = 2;
  }
  else if (length >= 2 && bytes[0] == 0xFE && bytes[1] == 0xFF) {
    bigEndian = true;
    offset = 2;
  }
  else {
    bool zeroHigh = probe >= 4; /* little endian: the odd bytes are 0 */
    bool zeroLow = probe >= 4;  /* big endian: the even bytes are 0 */

    for (index = 0; index < probe; index += 2) {
      zeroHigh = zeroHigh && bytes[index] != 0 && bytes[index + 1] == 0;
      zeroLow = zeroLow && bytes[index] == 0 && bytes[index + 1] != 0;
    }
    if (!zeroHigh && !zeroLow) {
      return false;
    }
    bigEndian = zeroLow;
  }
  converted->clear();
  for (index = offset; index + 1 < length; index += 2) {
    uint32_t unit = bigEndian ? ((uint32_t)bytes[index] << 8 | bytes[index + 1])
                              : ((uint32_t)bytes[index + 1] << 8 | bytes[index]);
    uint32_t codePoint = unit;

    if (unit == 0) {
      break;
    }
    if (unit >= 0xD800 && unit < 0xE000) {
      codePoint = 0xFFFD;
      if (unit < 0xDC00 && index + 3 < length) {
        uint32_t low = bigEndian ? ((uint32_t)bytes[index + 2] << 8 | bytes[index + 3])
                                 : ((uint32_t)bytes[index + 3] << 8 | bytes[index + 2]);

        if (low >= 0xDC00 && low < 0xE000) {
          codePoint = 0x10000 + ((unit - 0xD800) << 10) + (low - 0xDC00);
          index += 2;
        }
      }
    }
    PersistentIni_EncodeUtf8(codePoint, encoded);
    converted->append(encoded);
  }
  return true;
}

/* Whether [begin, end) names a section of the key table. */
static bool PersistentIni_IsKnownSection(const char *begin, const char *end)
{
  uint32_t index;

  for (index = 0; index < PERSISTENT_INI_KEY_COUNT; index++) {
    if (PersistentIni_Equals(begin, end, s_PersistentIniKeys[index].section)) {
      return true;
    }
  }
  return false;
}

/* The extras entry of section [begin, end), created (at the end, so foreign sections keep their file order)
   when there is none; sections that appear twice share one entry. Returns its index. */
static size_t PersistentIni_ExtrasSection(PersistentSettingsIniExtras *extras, const char *begin, const char *end,
                                          bool known)
{
  size_t index;

  for (index = 0; index < extras->sections.size(); index++) {
    if (PersistentIni_Equals(begin, end, extras->sections[index].name.c_str())) {
      return index;
    }
  }
  extras->sections.push_back({std::string(begin, end), known, {}});
  return extras->sections.size() - 1;
}

/* Keeps one line in an extras section, unless the bound is reached (then the line is dropped, logged once). */
static void PersistentIni_KeepLine(PersistentSettingsIniExtras *extras, size_t section, const char *begin,
                                   const char *end, bool *overflowLogged)
{
  size_t length = (size_t)(end - begin);

  if (extras->byteCount + length + 2 > PERSISTENT_SETTINGS_INI_EXTRAS_MAX_BYTES) {
    if (!*overflowLogged) {
      Thandor_Log("settings: more than %u bytes of unknown keys and sections, the rest is not kept",
                  (unsigned)PERSISTENT_SETTINGS_INI_EXTRAS_MAX_BYTES);
      *overflowLogged = true;
    }
    return;
  }
  extras->sections[section].lines.emplace_back(begin, end);
  extras->byteCount += length + 2; /* and the line end */
}

/* Parses ini text into image (which the caller zeroed); returns the mask of the dwords the text set. Values that
   do not parse are ignored. With extras, the lines this version does not know are collected for the rewrite:
   - in a section of the key table, every key line whose key is not in the table, together with the comment
     lines directly above it (the comments above known keys are dropped: the game writes its own there, and a
     user comment could not be placed again reliably once keys are added or removed);
   - every line of a foreign section (comments included), with its header.
   A key the table knows is never kept as an extra, also when its value does not parse, so the rewrite never
   writes it twice. Lines before the first section header (the game's own file comment) are not kept. */
uint64_t PersistentSettings_ParseIniWithExtras(const char *text, uint32_t length, uint8_t *image,
                                               PersistentSettingsIniExtras *extras)
{
  const char *cursor = text;
  const char *textEnd = text + length;
  const char *sectionBegin = nullptr;
  const char *sectionEnd = nullptr;
  bool sectionKnown = false;
  size_t sectionExtras = 0; /* extras entry of a foreign section */
  std::vector<std::pair<const char *, const char *>> pendingComments; /* above the next key of a known section */
  bool overflowLogged = false;
  uint64_t presentMask = 0;
  uint64_t rejectedMask = 0; /* dwords whose value was out of range */
  uint64_t bitsGivenMask = 0; /* dwords of INI_KIND_BIT keys that got their default before the first bit */

  std::string converted;

  if (extras != nullptr) {
    extras->sections.clear();
    extras->byteCount = 0;
  }
  if (PersistentIni_DecodeUtf16(text, length, &converted)) {
    Thandor_Log("settings: thandor.ini is UTF-16, read as text (written back as UTF-8)");
    text = converted.data();
    length = (uint32_t)converted.size();
    cursor = text;
    textEnd = text + length;
  }
  if (length >= 3 && (uint8_t)text[0] == 0xEF && (uint8_t)text[1] == 0xBB && (uint8_t)text[2] == 0xBF) {
    cursor += 3;
  }
  while (cursor < textEnd) {
    const char *lineBegin = cursor;
    const char *lineEnd;
    const char *equals;
    const char *keyBegin;
    const char *keyEnd;
    const char *valueBegin;
    const char *valueEnd;
    uint32_t index;
    bool keyKnown = false;

    /* a line ends at LF, CR LF or a lone CR (old Mac editors); the empty line between CR and LF is skipped */
    while (cursor < textEnd && *cursor != '\n' && *cursor != '\r') {
      cursor++;
    }
    lineEnd = cursor;
    if (cursor < textEnd) {
      cursor++;
    }
    PersistentIni_Trim(&lineBegin, &lineEnd);
    if (lineBegin == lineEnd) {
      continue;
    }
    if (*lineBegin == ';' || *lineBegin == '#') {
      if (extras != nullptr && sectionBegin != nullptr) {
        if (sectionKnown) {
          pendingComments.emplace_back(lineBegin, lineEnd);
        }
        else {
          PersistentIni_KeepLine(extras, sectionExtras, lineBegin, lineEnd, &overflowLogged);
        }
      }
      continue;
    }
    if (*lineBegin == '[') {
      const char *close = static_cast<const char *>(memchr(lineBegin, ']', (size_t)(lineEnd - lineBegin)));

      pendingComments.clear();
      if (close == nullptr) {
        /* a header without ']': its keys belong to no known section, so they are neither read (the section
           before it used to run on and take them) nor kept, up to the next valid header */
        Thandor_Log("settings: section header %.*s without ']', its keys are ignored", (int)(lineEnd - lineBegin),
                    lineBegin);
        sectionBegin = nullptr;
        sectionEnd = nullptr;
        continue;
      }
      sectionBegin = lineBegin + 1;
      sectionEnd = close;
      PersistentIni_Trim(&sectionBegin, &sectionEnd);
      sectionKnown = PersistentIni_IsKnownSection(sectionBegin, sectionEnd);
      if (extras != nullptr && !sectionKnown) {
        /* created at once, so an empty foreign section is kept too */
        sectionExtras = PersistentIni_ExtrasSection(extras, sectionBegin, sectionEnd, false);
      }
      continue;
    }
    if (sectionBegin == nullptr) {
      continue;
    }
    if (!sectionKnown) {
      if (extras != nullptr) {
        PersistentIni_KeepLine(extras, sectionExtras, lineBegin, lineEnd, &overflowLogged);
      }
      continue;
    }
    equals = static_cast<const char *>(memchr(lineBegin, '=', (size_t)(lineEnd - lineBegin)));
    if (equals == nullptr) {
      pendingComments.clear();
      continue;
    }
    keyBegin = lineBegin;
    keyEnd = equals;
    PersistentIni_Trim(&keyBegin, &keyEnd);
    valueBegin = equals + 1;
    valueEnd = lineEnd;
    PersistentIni_Trim(&valueBegin, &valueEnd);
    for (index = 0; index < PERSISTENT_INI_KEY_COUNT; index++) {
      const PersistentIniKey *key = &s_PersistentIniKeys[index];
      uint32_t value = 0;
      bool parsed = false;

      if (!PersistentIni_Equals(sectionBegin, sectionEnd, key->section) ||
          !PersistentIni_Equals(keyBegin, keyEnd, key->key)) {
        continue;
      }
      keyKnown = true;
      if (key->kind == INI_KIND_NAME) {
        const char *nameBegin = valueBegin;
        const char *nameEnd = valueEnd;

        /* quoted (as the game writes it): the name is up to the last quote, so a quote inside the name stays,
           and a comment after the closing quote is cut off. Unquoted names keep the whole value, ';' and '#'
           included: a hand-written name may contain them, and the game never writes a name unquoted. */
        if (nameEnd - nameBegin >= 2 && *nameBegin == '"') {
          const char *closeQuote = nameEnd - 1;
          const char *rest;

          while (closeQuote > nameBegin && *closeQuote != '"') {
            closeQuote--;
          }
          rest = closeQuote + 1;
          while (rest < nameEnd && PersistentIni_IsSpace(*rest)) {
            rest++;
          }
          if (closeQuote > nameBegin && (rest == nameEnd || *rest == ';' || *rest == '#')) {
            nameBegin++;
            nameEnd = closeQuote;
          }
        }
        PersistentIni_ParseName(nameBegin, nameEnd, image + key->offset);
        presentMask |= PersistentSettings_DwordMask(key->offset, PERSISTENT_SETTINGS_NAME_BYTES);
        break;
      }
      {
        /* a trailing comment after the value */
        const char *numberEnd = valueBegin;

        while (numberEnd < valueEnd && *numberEnd != ';' && *numberEnd != '#') {
          numberEnd++;
        }
        PersistentIni_Trim(&valueBegin, &numberEnd);
        valueEnd = numberEnd;
      }
      switch (key->kind) {
      case INI_KIND_BOOL:
      case INI_KIND_BIT:
        parsed = PersistentIni_ParseBool(valueBegin, valueEnd, &value);
        break;
      case INI_KIND_GAIN:
        parsed = PersistentIni_ParseGain(valueBegin, valueEnd, &value);
        break;
      case INI_KIND_ENUM: {
        uint32_t nameIndex;

        for (nameIndex = 0; nameIndex < key->nameCount; nameIndex++) {
          if (PersistentIni_Equals(valueBegin, valueEnd, key->names[nameIndex])) {
            value = nameIndex;
            parsed = true;
          }
        }
        if (!parsed) {
          parsed = PersistentIni_ParseNumber(valueBegin, valueEnd, &value);
        }
        break;
      }
      default:
        parsed = PersistentIni_ParseNumber(valueBegin, valueEnd, &value);
        break;
      }
      /* only the signed keys take a negative number (it used to wrap, -5 became 0xFFFFFFFB) */
      if (parsed && valueBegin < valueEnd && *valueBegin == '-' && key->kind != INI_KIND_INT &&
          key->kind != INI_KIND_RAW) {
        Thandor_Log("settings: [%s] %s = %.*s is negative, default used", key->section, key->key,
                    (int)(valueEnd - valueBegin), valueBegin);
        rejectedMask |= PersistentSettings_DwordMask(key->offset, 4);
        parsed = false;
      }
      if (parsed && key->kind != INI_KIND_BIT && key->kind != INI_KIND_HEX && key->kind != INI_KIND_RAW &&
          !PersistentIni_ValueAllowed(key, value)) {
        rejectedMask |= PersistentSettings_DwordMask(key->offset, 4);
        parsed = false;
      }
      if (parsed) {
        uint64_t mask = PersistentSettings_DwordMask(key->offset, 4);

        if (key->kind == INI_KIND_BIT) {
          uint32_t flags;

          if ((bitsGivenMask & mask) == 0) {
            PersistentIni_SetDword(image, key->offset, key->defaultValue);
            bitsGivenMask |= mask;
          }
          flags = PersistentIni_GetDword(image, key->offset);
          flags = value != 0 ? (flags | key->mask) : (flags & ~key->mask);
          PersistentIni_SetDword(image, key->offset, flags);
        }
        else {
          PersistentIni_SetDword(image, key->offset, value);
        }
        presentMask |= mask;
      }
      break;
    }
    if (!keyKnown && extras != nullptr) {
      size_t section = PersistentIni_ExtrasSection(extras, sectionBegin, sectionEnd, true);

      for (const std::pair<const char *, const char *> &comment : pendingComments) {
        PersistentIni_KeepLine(extras, section, comment.first, comment.second, &overflowLogged);
      }
      PersistentIni_KeepLine(extras, section, lineBegin, lineEnd, &overflowLogged);
    }
    pendingComments.clear();
  }
  return PersistentIni_DropIncompleteMode(presentMask, rejectedMask);
}

uint64_t PersistentSettings_ParseIni(const char *text, uint32_t length, uint8_t *image)
{
  return PersistentSettings_ParseIniWithExtras(text, length, image, nullptr);
}

/* Saves the settings: mirrors g_LocaleCountryCodeOverride into the image and, when anything changed since the
   last load or save, writes thandor.ini (open-thandor; the original wrote the 200-byte image to thandor.dat)
   with every key that was loaded or written. The save result is not checked.
*/
void PersistentSettings_Flush()

{
  static char iniText[PERSISTENT_SETTINGS_INI_MAX_BYTES];
  uint32_t iniLength;

  if (g_PersistentSettings.image != nullptr) {
    PersistentSettings_Write(g_LocaleCountryCodeOverride,PERSISTENT_SETTING_LOCALE_COUNTRY_CODE);
    if (g_PersistentSettings.dirtyWriteCount != 0) {
      iniLength = PersistentSettings_FormatIni
                    (Thandor_Bytes(g_PersistentSettings.image.get()),
                     s_PersistentSettingsPresentMask | s_PersistentSettingsWrittenMask,iniText,sizeof iniText);
      FileSystem_WriteBufferToPath(iniLength,iniText,s_PersistentSettingsIniPath);
      g_PersistentSettings.dirtyWriteCount = 0;
    }
  }
}


/* open-thandor: opens thandor.ini in the current directory, else in the executable directory, reads it and
   builds the image from it. Sets s_PersistentSettingsIniPath to the path found. False when there is no
   readable thandor.ini.
*/
static bool PersistentSettings_LoadIni(PersistentSettingsImage *image)

{
  void *fileHandle;
  uint32_t fileSize;
  uint32_t index;
  uint64_t presentMask;

  if (g_FileSystemOpen(FileSystemOpenFlags::FILESYSTEM_OPEN_NONE,s_PersistentSettingsIniPath,&fileHandle) != 0) {
    WidePath_CombineDirectoryAndLeaf
              (g_FileSystemCombinedPathScratchUtf16,const_cast<uint16_t *>(s_PersistentSettingsIniLeaf) /* only read */,
               g_ExecutableDirectoryUtf16);
    if (g_FileSystemOpen(FileSystemOpenFlags::FILESYSTEM_OPEN_NONE,g_FileSystemCombinedPathScratchUtf16,&fileHandle) != 0) {
      return false;
    }
    for (index = 0; index < THANDOR_PATH_CAPACITY - 1 && g_FileSystemCombinedPathScratchUtf16[index] != 0; index++) {
      s_PersistentSettingsIniPath[index] = g_FileSystemCombinedPathScratchUtf16[index];
    }
    s_PersistentSettingsIniPath[index] = 0;
  }
  if (!g_FileSystemGetSize(fileHandle,&fileSize) || fileSize > 0x100000) {
    g_FileSystemClose(fileHandle);
    return false;
  }
  std::vector<char> text(fileSize + 1); /* the file and a NUL */
  if (fileSize != 0 && g_FileSystemReadExact(fileSize,text.data(),fileHandle) != 0) {
    g_FileSystemClose(fileHandle);
    return false;
  }
  g_FileSystemClose(fileHandle);
  presentMask = PersistentSettings_ParseIniWithExtras(text.data(),fileSize,Thandor_Bytes(image),
                                                     &s_PersistentSettingsIniExtras);
  s_PersistentSettingsPresentMask = presentMask;
  if ((presentMask & PersistentSettings_DwordMask(PERSISTENT_SETTING_LOCALE_COUNTRY_CODE,4)) != 0) {
    g_LocaleCountryCodeOverride = image->localeCountryCodeOverride;
  }
  g_PersistentSettings.image = image;
  g_PersistentSettings.loadedByteCount = PERSISTENT_SETTINGS_IMAGE_BYTES;
  g_PersistentSettings.dirtyWriteCount = 0;
  Thandor_Log("settings: loaded thandor.ini");
  return true;
}


/* Loads the settings into a fresh zeroed 200-byte image. open-thandor: from thandor.ini when there is one
   (see the top of this file); otherwise from the original thandor.dat as before, marking the image dirty so
   that the next Flush writes thandor.ini. A shorter thandor.dat leaves the rest zero and not present, so
   every PersistentSettings_Read beyond the loaded bytes falls back to its default. When the file is not found
   at its path it is looked up in the executable directory. Without any settings file the image stays empty
   (all defaults). open-thandor: the image is then passed to PersistentSettings_NormalizeColorDepth.
*/
static void PersistentSettings_LoadImage()

{
  uint32_t *clearCursor;
  void *fileHandle;
  uint32_t byteCount;
  PersistentSettingsImage *image;
  uint32_t pathByteCount;
  uint32_t fileSize;
  uint32_t dword;

  Resource_Release(g_PersistentSettings.image);
  g_PersistentSettings.image = nullptr;
  s_PersistentSettingsPresentMask = 0;
  s_PersistentSettingsWrittenMask = 0;
  s_PersistentSettingsIniExtras = PersistentSettingsIniExtras();
  std::copy(std::begin(s_PersistentSettingsIniLeaf),std::end(s_PersistentSettingsIniLeaf),s_PersistentSettingsIniPath);
  if (g_MemoryApi.alloc(PERSISTENT_SETTINGS_IMAGE_BYTES,reinterpret_cast<void **>(&clearCursor)) /* the payload as dwords */ != 0) {
    return;
  }
  clearCursor = std::fill_n(clearCursor,PERSISTENT_SETTINGS_IMAGE_BYTES / 4,0);
  image = reinterpret_cast<PersistentSettingsImage *>(clearCursor - PERSISTENT_SETTINGS_IMAGE_BYTES / 4);
  if (PersistentSettings_LoadIni(image)) {
    return;
  }
  /* no thandor.ini: an empty image (nothing present), unless thandor.dat loads below */
  g_PersistentSettings.image = image;
  g_PersistentSettings.loadedByteCount = 0;
  g_PersistentSettings.dirtyWriteCount = 0;
  if (g_FileSystemOpen(FileSystemOpenFlags::FILESYSTEM_OPEN_NONE,g_PersistentSettings.path,&fileHandle) != 0) {
    WidePath_CombineDirectoryAndLeaf
              (g_FileSystemCombinedPathScratchUtf16,g_PersistentSettings.path,
               g_ExecutableDirectoryUtf16);
    if (g_FileSystemOpen(FileSystemOpenFlags::FILESYSTEM_OPEN_NONE,g_FileSystemCombinedPathScratchUtf16,&fileHandle) != 0) {
      Thandor_Log("settings: no thandor.ini or thandor.dat, defaults");
      return;
    }
    /* The original continues with the return value of this path copy (the byte count, or
       FATAL_ERROR_GENERAL_FAILURE on overflow) as the file handle instead of the handle from the open above:
       GetFileSize and CloseHandle then run on an arbitrary handle number (on the second load in Game_Run
       possibly an open PCK package, which gets read as settings and closed under its owner), the real handle
       leaks and the migration practically always fails. Fixed here because of that: the real handle is used,
       so a thandor.dat in the executable directory is migrated like one at its plain path. */
    (void)RichTextCommandStream_CopyExpanded
           (sizeof g_PersistentSettings.path,g_PersistentSettings.path,
            g_FileSystemCombinedPathScratchUtf16,&pathByteCount);
  }
  if (g_FileSystemGetSize(fileHandle,&fileSize)) {
    byteCount = PERSISTENT_SETTINGS_IMAGE_BYTES;
    if (fileSize < PERSISTENT_SETTINGS_IMAGE_BYTES) {
      byteCount = fileSize;
    }
    if (g_FileSystemReadExact(byteCount,image,fileHandle) == 0) {
      g_FileSystemClose(fileHandle);
      /* the country code override is only taken over when the file contains it */
      if (byteCount >= PERSISTENT_SETTING_LOCALE_COUNTRY_CODE + 4) {
        g_LocaleCountryCodeOverride = image->localeCountryCodeOverride;
      }
      for (dword = 0; (dword + 1) * 4 <= byteCount; dword++) {
        s_PersistentSettingsPresentMask |= (uint64_t)1 << dword;
      }
      /* the dwords the original never used are not taken over: their bytes are whatever the original left there,
         and open-thandor may give these offsets a meaning (they would load as garbage settings) */
      std::fill_n(Thandor_Bytes(image) + PERSISTENT_SETTINGS_DAT_UNUSED_OFFSET,PERSISTENT_SETTINGS_DAT_UNUSED_BYTES,0);
      s_PersistentSettingsPresentMask &=
          ~PersistentSettings_DwordMask(PERSISTENT_SETTINGS_DAT_UNUSED_OFFSET,PERSISTENT_SETTINGS_DAT_UNUSED_BYTES);
      s_PersistentSettingsPresentMask =
          PersistentSettings_DropDisallowed(Thandor_Bytes(image), s_PersistentSettingsPresentMask);
      g_PersistentSettings.loadedByteCount = byteCount;
      g_PersistentSettings.dirtyWriteCount = 1; /* the next Flush writes thandor.ini */
      Thandor_Log("settings: migrated %u bytes of thandor.dat, thandor.ini follows on the next save", byteCount);
      return;
    }
  }
  g_FileSystemClose(fileHandle);
  /* the failed read may have left bytes behind */
  std::fill_n(Thandor_Bytes(image),PERSISTENT_SETTINGS_IMAGE_BYTES,0);
}

/* open-thandor: the game runs in 32-bit colour only. A colour depth other than 32 from an older thandor.ini or
   thandor.dat (the original's 16) is read as 32 and written as 32 by the next PersistentSettings_Flush. */
static void PersistentSettings_NormalizeColorDepth()
{
  if ((g_PersistentSettings.image == nullptr) ||
      !PersistentSettings_IsPresent(PERSISTENT_SETTING_BITS_PER_PIXEL, 4) ||
      (PersistentIni_GetDword(Thandor_Bytes(g_PersistentSettings.image.get()), PERSISTENT_SETTING_BITS_PER_PIXEL) ==
       PERSISTENT_DEFAULT_BITS_PER_PIXEL)) {
    return;
  }
  PersistentIni_SetDword(Thandor_Bytes(g_PersistentSettings.image.get()), PERSISTENT_SETTING_BITS_PER_PIXEL,
                         PERSISTENT_DEFAULT_BITS_PER_PIXEL);
  s_PersistentSettingsWrittenMask |= PersistentSettings_DwordMask(PERSISTENT_SETTING_BITS_PER_PIXEL, 4);
  g_PersistentSettings.dirtyWriteCount++; /* the next Flush writes it */
}

/* Largest shading depth (stored as a quarter of the depth the menus name) the menus offer: 128 / 4. */
constexpr uint32_t PERSISTENT_SHADING_SUBRESOURCE_COUNT_MAX = 32;

/* open-thandor: the shading settings go unchecked into GraphicsShadingRuntime_InitializeGeneratedTexture (session
   start and the in-game shading option). The menus only write the grid half sizes 32, 64 and 128 with a texture
   size of twice that and the depths 32, 64 and 128 (stored as 8, 16 and 32). A hand-edited thandor.ini (or
   thandor.dat) could give a texture size of 0 (division by zero in the grid step), a grid half size that is no
   multiple of 32 or a texture size other than twice it (the filter and the tile walk run past the scratch grid
   and the textures), or a depth of 0 or one so large that the texture asset size overflows. Such values are
   dropped from the loaded image here (one log line each), so PersistentSettings_Read returns the defaults;
   grid half size and texture size only together, as they must match. Values the game writes pass unchanged. */
static void PersistentSettings_ValidateShading()
{
  if (g_PersistentSettings.image == nullptr) {
    return;
  }
  const uint32_t gridHalfSize =
      PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE, PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
  const uint32_t textureDimension = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION,
                                                            PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
  const uint32_t subresourceCount = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT,
                                                            PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);

  if ((gridHalfSize != 32 && gridHalfSize != 64 && gridHalfSize != 128) || textureDimension != gridHalfSize * 2) {
    Thandor_Log("settings: shading_grid_half_size %u / shading_texture_size %u not 32/64, 64/128 or 128/256, "
                "defaults used", gridHalfSize, textureDimension);
    s_PersistentSettingsPresentMask &= ~(PersistentSettings_DwordMask(PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE, 4) |
                                         PersistentSettings_DwordMask(PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION, 4));
  }
  if (subresourceCount == 0 || subresourceCount > PERSISTENT_SHADING_SUBRESOURCE_COUNT_MAX) {
    Thandor_Log("settings: shading_depth %u not 1..%u, default used", subresourceCount,
                PERSISTENT_SHADING_SUBRESOURCE_COUNT_MAX);
    s_PersistentSettingsPresentMask &= ~PersistentSettings_DwordMask(PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT, 4);
  }
}

void PersistentSettings_Load()
{
  PersistentSettings_LoadImage();
  PersistentSettings_NormalizeColorDepth();
  PersistentSettings_ValidateShading();
}


/* Returns the setting dword at settingsOffsetBytes, or defaultValue when it was not loaded (no settings file,
   no such key in thandor.ini, or beyond the end of a short thandor.dat).
*/
uint32_t PersistentSettings_Read(PersistentSettingsValue defaultValue,
          PersistentSettingsByteOffset settingsOffsetBytes)

{
  if ((g_PersistentSettings.image != nullptr) && PersistentSettings_IsPresent(settingsOffsetBytes,4)) {
    defaultValue = *Thandor_At<PersistentSettingsValue>(g_PersistentSettings.image.get(), settingsOffsetBytes);
  }
  return defaultValue;
}


/* Returns a pointer into the settings image at settingsOffsetBytes (not a copy), or fallback when the whole
   region was not loaded. Used for the stored names.
*/
void * PersistentSettings_GetRegionOrFallback(PersistentSettingsByteCount regionByteCount,void *fallback,
          PersistentSettingsByteOffset settingsOffsetBytes)

{
  if ((g_PersistentSettings.image != nullptr) &&
     PersistentSettings_IsPresent(settingsOffsetBytes,regionByteCount)) {
    fallback = Thandor_At(g_PersistentSettings.image.get(), settingsOffsetBytes);
  }
  return fallback;
}


/* Copies a block (whole dwords only; trailing 1-3 bytes are dropped) into the settings image and marks it
   dirty, even when nothing changed. The bound is the image capacity, not the loaded part, and the block does
   not become present, so a block that was not loaded is saved but not read back until reload.
*/
void PersistentSettings_WriteBlock(PersistentSettingsByteCount regionByteCount,uint32_t *source,
          PersistentSettingsByteOffset settingsOffsetBytes)

{
  uint32_t dwordsRemaining;
  uint32_t *destination;

  if ((g_PersistentSettings.image != nullptr) &&
     (settingsOffsetBytes + regionByteCount < PERSISTENT_SETTINGS_IMAGE_BYTES + 1)) {
    destination = Thandor_At<uint32_t>(g_PersistentSettings.image.get(), settingsOffsetBytes);
    dwordsRemaining = regionByteCount >> 2;
    if (dwordsRemaining != 0) {
      s_PersistentSettingsWrittenMask |= PersistentSettings_DwordMask(settingsOffsetBytes,dwordsRemaining * 4);
      for (; dwordsRemaining != 0; dwordsRemaining--) {
        *destination = *source;
        source++;
        destination++;
      }
      g_PersistentSettings.dirtyWriteCount++;
    }
  }
}


/* Stores one setting dword in the image and marks it dirty, but only when the value actually changes (it is
   saved with the next change either way). Like WriteBlock it checks against the image capacity, and the
   dword does not become present.
*/
void PersistentSettings_Write(PersistentSettingsValue value,PersistentSettingsByteOffset settingsOffsetBytes)

{
  if ((g_PersistentSettings.image != nullptr) &&
      (settingsOffsetBytes + 4 < PERSISTENT_SETTINGS_IMAGE_BYTES + 1)) {
    s_PersistentSettingsWrittenMask |= PersistentSettings_DwordMask(settingsOffsetBytes,4);
    if (*Thandor_At<PersistentSettingsValue>(g_PersistentSettings.image.get(), settingsOffsetBytes) != value) {
      *Thandor_At<PersistentSettingsValue>(g_PersistentSettings.image.get(), settingsOffsetBytes) = value;
      g_PersistentSettings.dirtyWriteCount++;
    }
  }
}

/* Not in the original: PersistentSettings_Write for open-thandor's own settings chosen in its menus (beyond the
   original's dwords), whose new value is read back at once: the dword also becomes present. */
void PersistentSettings_WriteChosen(PersistentSettingsValue value,PersistentSettingsByteOffset settingsOffsetBytes)

{
  PersistentSettings_Write(value,settingsOffsetBytes);
  if ((g_PersistentSettings.image != nullptr) &&
      (settingsOffsetBytes + 4 < PERSISTENT_SETTINGS_IMAGE_BYTES + 1)) {
    s_PersistentSettingsPresentMask |= PersistentSettings_DwordMask(settingsOffsetBytes,4);
  }
}
