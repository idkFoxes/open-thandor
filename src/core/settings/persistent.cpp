/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/settings/persistent.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/core/settings/persistent.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/*
open-thandor: the settings live in a readable thandor.ini instead of the original's 200-byte thandor.dat.
The game code still works on the 200-byte image through PersistentSettings_Read/Write/WriteBlock/
GetRegionOrFallback; only loading and saving changed:
- Load builds the image from thandor.ini (current directory, then the executable directory). Each dword the
  ini sets is marked present; Read and GetRegionOrFallback return the caller's default for dwords that are not
  present, exactly like the original for bytes beyond the end of a short thandor.dat.
- Without a thandor.ini an existing thandor.dat is loaded as before (migration; its loaded bytes are the
  present ones) and the image is marked dirty, so the next Flush writes thandor.ini. thandor.dat is never
  written or deleted.
- Flush writes thandor.ini (when dirty, as before) with every key that is present or was written since the
  load, in the fixed order of the key table below.
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
#define PERSISTENT_SETTINGS_INI_MAX_BYTES 0x4000 /* written ini text; the full key set needs about 3 KB */
#define PERSISTENT_SETTINGS_INI_MAX_LINE 256
#define PERSISTENT_SETTINGS_GAIN_FULL 0x8000 /* Q15 gain of 100 % */

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

typedef struct PersistentIniKey {
  uint32_t offset;
  uint32_t kind;
  const char *section;
  const char *key;
  const char *comment;
  uint32_t mask;         /* INI_KIND_BIT */
  uint32_t defaultValue; /* INI_KIND_BIT */
  const char *const *names;
  uint32_t nameCount;
} PersistentIniKey;

static const char *const s_IniRendererNames[] = {"vulkan", "d3d12", "software"};
static const char *const s_IniDisplayModeNames[] = {"fullscreen", "borderless", "window"};
static const char *const s_IniTextureQualityNames[] = {"high", "medium", "low"};
static const char *const s_IniGpuRasterizationNames[] = {"smooth", "exact"};

#define INI_ENUM(names) names, (uint32_t)(sizeof names / sizeof names[0])

static const PersistentIniKey s_PersistentIniKeys[] = {
  {PERSISTENT_SETTING_ADAPTER_INDEX, INI_KIND_UINT, "display", "adapter",
   "graphics adapter, 0 = the first (default 0)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_DISPLAY_WIDTH, INI_KIND_UINT, "display", "width",
   "screen width in pixels (default 640)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_DISPLAY_HEIGHT, INI_KIND_UINT, "display", "height",
   "screen height in pixels (default 480)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_BITS_PER_PIXEL, INI_KIND_UINT, "display", "bits_per_pixel",
   "colour depth in bits, always 32 (an older 16 is read as 32)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_RENDERER, INI_KIND_ENUM, "display", "renderer",
   "renderer: vulkan (default), d3d12 or software", 0, 0, INI_ENUM(s_IniRendererNames)},
  {PERSISTENT_SETTING_DISPLAY_MODE_KIND, INI_KIND_ENUM, "display", "display_mode",
   "fullscreen (default), borderless or window", 0, 0, INI_ENUM(s_IniDisplayModeNames)},

  {PERSISTENT_SETTING_SHADING_ENABLED, INI_KIND_BOOL, "graphics", "shading",
   "terrain shading (default true)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE, INI_KIND_UINT, "graphics", "shading_grid_half_size",
   "shading grid half size (default 32)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION, INI_KIND_UINT, "graphics", "shading_texture_size",
   "shading texture size, 2 x shading_grid_half_size (default 64)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT, INI_KIND_UINT, "graphics", "shading_depth",
   "shading depth (default 16)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_TEXTURE_QUALITY, INI_KIND_ENUM, "graphics", "texture_quality",
   "texture quality: high, medium or low", 0, 0, INI_ENUM(s_IniTextureQualityNames)},
  {PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD, INI_KIND_INT, "graphics", "model_detail",
   "model detail distance, 8.8 fixed point (default 65536)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_GPU_RASTERIZATION, INI_KIND_ENUM, "graphics", "gpu_rasterization",
   "Vulkan / DirectX 12 triangles: smooth (default; sub-pixel, perspective-correct like the original's Direct3D) "
   "or exact (the software renderer's look)", 0, 0, INI_ENUM(s_IniGpuRasterizationNames)},

  {PERSISTENT_SETTING_SOUND_OPTION_FLAGS, INI_KIND_BIT, "sound", "effects",
   "sound effects (default true)", PERSISTENT_SOUND_OPTION_EFFECTS, PERSISTENT_SOUND_OPTION_DEFAULT, NULL, 0},
  {PERSISTENT_SETTING_SOUND_OPTION_FLAGS, INI_KIND_BIT, "sound", "music",
   "music (default true)", PERSISTENT_SOUND_OPTION_MUSIC, PERSISTENT_SOUND_OPTION_DEFAULT, NULL, 0},
  {PERSISTENT_SETTING_SOUND_OPTION_FLAGS, INI_KIND_BIT, "sound", "reverse_stereo",
   "swap the left and right channel (default false)", PERSISTENT_SOUND_OPTION_REVERSE_STEREO,
   PERSISTENT_SOUND_OPTION_DEFAULT, NULL, 0},
  {PERSISTENT_SETTING_EFFECTS_GAIN, INI_KIND_GAIN, "sound", "effects_volume",
   "sound effects volume", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_MUSIC_GAIN, INI_KIND_GAIN, "sound", "music_volume",
   "music volume", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN, INI_KIND_GAIN, "sound", "movie_volume",
   "movie volume", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN, INI_KIND_GAIN, "sound", "movie_alternate_volume",
   "second movie volume", 0, 0, NULL, 0},

  {PERSISTENT_SETTING_GAME_SPEED_PERCENT, INI_KIND_UINT, "game", "speed_percent",
   "game speed in percent (default 100)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_CAMERA_SCROLL_STEP, INI_KIND_INT, "game", "scroll_step",
   "map scroll speed (default 32)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS, INI_KIND_HEX, "game", "map_mouse_options",
   "bits: 0x1 automatic zoom off, 0x2 automatic rotation off, 0x4 right button does not scroll / side panel "
   "hidden (default 0x0)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS, INI_KIND_HEX, "game", "link_panel_options",
   "bits: 0x1 link rotation with zoom, 0x2 link rotation with tilt, 0x4 hide panel (default 0x0)", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_PLAYER_NAME, INI_KIND_NAME, "game", "player_name",
   "player name, at most 20 characters", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_GAME_NAME, INI_KIND_NAME, "game", "game_name",
   "name of a hosted network game, at most 20 characters", 0, 0, NULL, 0},
  {PERSISTENT_SETTING_LOCALE_COUNTRY_CODE, INI_KIND_UINT, "game", "language_country_code",
   "text language as telephone country code (49 German, 44 English, ...), 0 = from Windows", 0, 0, NULL, 0},

  {PERSISTENT_SETTING_NETWORK_PLAYER_COUNT, INI_KIND_UINT, "network", "players",
   "player count of a hosted network game (default 4)", 0, 0, NULL, 0},

  {0x50, INI_KIND_RAW, "reserved", "dword_50", "unused in the original settings file, kept", 0, 0, NULL, 0},
  {0x54, INI_KIND_RAW, "reserved", "dword_54", "unused in the original settings file, kept", 0, 0, NULL, 0},
  {0x58, INI_KIND_RAW, "reserved", "dword_58", "unused in the original settings file, kept", 0, 0, NULL, 0},
  {0xBC, INI_KIND_RAW, "reserved", "dword_bc", "unused in the original settings file, kept", 0, 0, NULL, 0},
  {0xC0, INI_KIND_RAW, "reserved", "dword_c0", "unused in the original settings file, kept", 0, 0, NULL, 0},
  {0xC4, INI_KIND_RAW, "reserved", "dword_c4", "unused in the original settings file, kept", 0, 0, NULL, 0},
};

#define PERSISTENT_INI_KEY_COUNT (sizeof s_PersistentIniKeys / sizeof s_PersistentIniKeys[0])

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
  uint32_t value;

  memcpy(&value, image + offset, 4);
  return value;
}

static void PersistentIni_SetDword(uint8_t *image, uint32_t offset, uint32_t value)
{
  memcpy(image + offset, &value, 4);
}

/* Appends to the ini text being written (silently truncated at the capacity). */
typedef struct PersistentIniWriter {
  char *out;
  uint32_t capacity;
  uint32_t length;
} PersistentIniWriter;

static void PersistentIni_Append(PersistentIniWriter *writer, const char *format, ...)
{
  va_list args;
  int written;

  if (writer->length + 1 >= writer->capacity) {
    return;
  }
  va_start(args, format);
  written = vsnprintf(writer->out + writer->length, writer->capacity - writer->length, format, args);
  va_end(args);
  if (written > 0) {
    writer->length += (uint32_t)written;
    if (writer->length >= writer->capacity) {
      writer->length = writer->capacity - 1;
    }
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

    memcpy(&unit, field + index * 2, 2);
    if (unit == 0) {
      break;
    }
    codePoint = unit;
    index++;
    if (unit >= 0xD800 && unit < 0xDC00 && index < unitCount) {
      uint16_t low;

      memcpy(&low, field + index * 2, 2);
      if (low >= 0xDC00 && low < 0xE000) {
        codePoint = 0x10000 + (((uint32_t)unit - 0xD800) << 10) + (low - 0xDC00);
        index++;
      }
    }
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
    PersistentIni_Append(writer, "%s", encoded);
  }
}

/* Writes the ini text of every key whose dword is in presentMask; returns the text length (no NUL counted). */
uint32_t PersistentSettings_FormatIni(const uint8_t *image, uint64_t presentMask, char *out, uint32_t capacity)
{
  PersistentIniWriter writer = {out, capacity, 0};
  const char *section = NULL;
  uint32_t index;

  if (capacity == 0) {
    return 0;
  }
  out[0] = 0;
  PersistentIni_Append(&writer,
      "; Open Thandor settings. The game rewrites this file when a setting changes in its menus.\r\n"
      "; A missing key means the game's default; unknown keys are ignored.\r\n");
  for (index = 0; index < PERSISTENT_INI_KEY_COUNT; index++) {
    const PersistentIniKey *key = &s_PersistentIniKeys[index];
    uint32_t byteCount = key->kind == INI_KIND_NAME ? PERSISTENT_SETTINGS_NAME_BYTES : 4;
    uint64_t mask = PersistentSettings_DwordMask(key->offset, byteCount);
    uint32_t value;

    if ((presentMask & mask) != mask) {
      continue;
    }
    value = PersistentIni_GetDword(image, key->offset);
    if (key->kind == INI_KIND_RAW && value == 0) {
      continue;
    }
    if (section == NULL || strcmp(section, key->section) != 0) {
      section = key->section;
      PersistentIni_Append(&writer, "\r\n[%s]\r\n", section);
    }
    if (key->kind == INI_KIND_GAIN) {
      uint32_t tenths = (uint32_t)(((uint64_t)value * 1000 + PERSISTENT_SETTINGS_GAIN_FULL / 2) /
                                   PERSISTENT_SETTINGS_GAIN_FULL);
      PersistentIni_Append(&writer, "; %s, 0..32768 = 0..100 %% (default 32768); here %u.%u %%\r\n",
                           key->comment, tenths / 10, tenths % 10);
    }
    else {
      PersistentIni_Append(&writer, "; %s\r\n", key->comment);
    }
    PersistentIni_Append(&writer, "%s = ", key->key);
    switch (key->kind) {
    case INI_KIND_INT:
      PersistentIni_Append(&writer, "%d", (int32_t)value);
      break;
    case INI_KIND_HEX:
    case INI_KIND_RAW:
      PersistentIni_Append(&writer, "0x%X", value);
      break;
    case INI_KIND_BOOL:
      if (value <= 1) {
        PersistentIni_Append(&writer, "%s", value != 0 ? "true" : "false");
      }
      else {
        PersistentIni_Append(&writer, "%u", value);
      }
      break;
    case INI_KIND_BIT:
      PersistentIni_Append(&writer, "%s", (value & key->mask) != 0 ? "true" : "false");
      break;
    case INI_KIND_ENUM:
      if (value < key->nameCount) {
        PersistentIni_Append(&writer, "%s", key->names[value]);
      }
      else {
        PersistentIni_Append(&writer, "%u", value);
      }
      break;
    case INI_KIND_NAME:
      PersistentIni_Append(&writer, "\"");
      PersistentIni_AppendName(&writer, image + key->offset);
      PersistentIni_Append(&writer, "\"");
      break;
    default: /* INI_KIND_UINT, INI_KIND_GAIN */
      PersistentIni_Append(&writer, "%u", value);
      break;
    }
    PersistentIni_Append(&writer, "\r\n");
  }
  return writer.length;
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
  memcpy(text, begin, length);
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
  const uint8_t *cursor = (const uint8_t *)begin;
  const uint8_t *limit = (const uint8_t *)end;

  memset(field, 0, PERSISTENT_SETTINGS_NAME_BYTES);
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
    memcpy(field + written * 2, units, units16 * 2);
    written += units16;
  }
}

/* Parses ini text into image (which the caller zeroed); returns the mask of the dwords the text set. Unknown
   sections and keys and values that do not parse are ignored. */
uint64_t PersistentSettings_ParseIni(const char *text, uint32_t length, uint8_t *image)
{
  const char *cursor = text;
  const char *textEnd = text + length;
  const char *sectionBegin = NULL;
  const char *sectionEnd = NULL;
  uint64_t presentMask = 0;
  uint64_t bitsGivenMask = 0; /* dwords of INI_KIND_BIT keys that got their default before the first bit */

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

    while (cursor < textEnd && *cursor != '\n') {
      cursor++;
    }
    lineEnd = cursor;
    if (cursor < textEnd) {
      cursor++;
    }
    PersistentIni_Trim(&lineBegin, &lineEnd);
    if (lineBegin == lineEnd || *lineBegin == ';' || *lineBegin == '#') {
      continue;
    }
    if (*lineBegin == '[') {
      const char *close = (const char *)memchr(lineBegin, ']', (size_t)(lineEnd - lineBegin));

      if (close != NULL) {
        sectionBegin = lineBegin + 1;
        sectionEnd = close;
        PersistentIni_Trim(&sectionBegin, &sectionEnd);
      }
      continue;
    }
    equals = (const char *)memchr(lineBegin, '=', (size_t)(lineEnd - lineBegin));
    if (equals == NULL || sectionBegin == NULL) {
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
      if (key->kind == INI_KIND_NAME) {
        const char *nameBegin = valueBegin;
        const char *nameEnd = valueEnd;

        if (nameEnd - nameBegin >= 2 && *nameBegin == '"' && nameEnd[-1] == '"') {
          nameBegin++;
          nameEnd--;
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
  }
  return presentMask;
}

/* Implementation ownership: core/settings/persistent. */

/* Saves the settings: mirrors g_LocaleCountryCodeOverride into the image and, when anything changed since the
   last load or save, writes thandor.ini (open-thandor; the original wrote the 200-byte image to thandor.dat)
   with every key that was loaded or written. The save result is not checked.
*/
void PersistentSettings_Flush(void)

{
  static char iniText[PERSISTENT_SETTINGS_INI_MAX_BYTES];
  uint32_t iniLength;

  if (g_PersistentSettings.image != NULL) {
    PersistentSettings_Write(g_LocaleCountryCodeOverride,PERSISTENT_SETTING_LOCALE_COUNTRY_CODE);
    if (g_PersistentSettings.dirtyWriteCount != 0) {
      iniLength = PersistentSettings_FormatIni
                    ((const uint8_t *)g_PersistentSettings.image,
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
  char *text;
  uint32_t index;
  uint64_t presentMask;

  if (g_FileSystemOpen(0,s_PersistentSettingsIniPath,&fileHandle) != 0) {
    WidePath_CombineDirectoryAndLeaf
              (g_FileSystemCombinedPathScratchUtf16,(uint16_t *)s_PersistentSettingsIniLeaf,
               g_ExecutableDirectoryUtf16);
    if (g_FileSystemOpen(0,g_FileSystemCombinedPathScratchUtf16,&fileHandle) != 0) {
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
  text = (char *)malloc(fileSize + 1);
  if (text == NULL || (fileSize != 0 && g_FileSystemReadExact(fileSize,text,fileHandle) != 0)) {
    free(text);
    g_FileSystemClose(fileHandle);
    return false;
  }
  g_FileSystemClose(fileHandle);
  text[fileSize] = 0;
  presentMask = PersistentSettings_ParseIni(text,fileSize,(uint8_t *)image);
  free(text);
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
static void PersistentSettings_LoadImage(void)

{
  uint32_t *clearCursor;
  void *fileHandle;
  int dwordsRemaining;
  uint32_t byteCount;
  PersistentSettingsImage *image;
  uint32_t pathByteCount;
  uint32_t fileSize;
  uint32_t dword;

  Resource_Release(g_PersistentSettings.image);
  g_PersistentSettings.image = NULL;
  s_PersistentSettingsPresentMask = 0;
  s_PersistentSettingsWrittenMask = 0;
  memcpy(s_PersistentSettingsIniPath,s_PersistentSettingsIniLeaf,sizeof s_PersistentSettingsIniLeaf);
  if (g_MemoryApi.alloc(PERSISTENT_SETTINGS_IMAGE_BYTES,(void **)&clearCursor) != 0) {
    return;
  }
  for (dwordsRemaining = PERSISTENT_SETTINGS_IMAGE_BYTES / 4; dwordsRemaining != 0; dwordsRemaining--) {
    *clearCursor = 0;
    clearCursor++;
  }
  image = (PersistentSettingsImage *)(clearCursor - PERSISTENT_SETTINGS_IMAGE_BYTES / 4);
  if (PersistentSettings_LoadIni(image)) {
    return;
  }
  /* no thandor.ini: an empty image (nothing present), unless thandor.dat loads below */
  g_PersistentSettings.image = image;
  g_PersistentSettings.loadedByteCount = 0;
  g_PersistentSettings.dirtyWriteCount = 0;
  if (g_FileSystemOpen(0,g_PersistentSettings.path,&fileHandle) != 0) {
    WidePath_CombineDirectoryAndLeaf
              (g_FileSystemCombinedPathScratchUtf16,g_PersistentSettings.path,
               g_ExecutableDirectoryUtf16);
    if (g_FileSystemOpen(0,g_FileSystemCombinedPathScratchUtf16,&fileHandle) != 0) {
      Thandor_Log("settings: no thandor.ini or thandor.dat, defaults");
      return;
    }
    /* Original quirk: the original continues with the return value of the path copy below (the byte
       count, or FATAL_ERROR_GENERAL_FAILURE on overflow) as the file handle, not the handle from this open.
       Kept as is. */
    if (!RichTextCommandStream_CopyExpanded
           (sizeof g_PersistentSettings.path,g_PersistentSettings.path,
            g_FileSystemCombinedPathScratchUtf16,&pathByteCount)) {
      pathByteCount = FATAL_ERROR_GENERAL_FAILURE;
    }
    fileHandle = THANDOR_PTR((uintptr_t)pathByteCount);
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
      g_PersistentSettings.loadedByteCount = byteCount;
      g_PersistentSettings.dirtyWriteCount = 1; /* the next Flush writes thandor.ini */
      Thandor_Log("settings: migrated %u bytes of thandor.dat, thandor.ini follows on the next save", byteCount);
      return;
    }
  }
  g_FileSystemClose(fileHandle);
  /* the failed read may have left bytes behind */
  memset(image,0,PERSISTENT_SETTINGS_IMAGE_BYTES);
}

/* open-thandor: the game runs in 32-bit colour only. A colour depth other than 32 from an older thandor.ini or
   thandor.dat (the original's 16) is read as 32 and written as 32 by the next PersistentSettings_Flush. */
static void PersistentSettings_NormalizeColorDepth(void)
{
  if ((g_PersistentSettings.image == NULL) ||
      !PersistentSettings_IsPresent(PERSISTENT_SETTING_BITS_PER_PIXEL, 4) ||
      (PersistentIni_GetDword((const uint8_t *)g_PersistentSettings.image, PERSISTENT_SETTING_BITS_PER_PIXEL) ==
       PERSISTENT_DEFAULT_BITS_PER_PIXEL)) {
    return;
  }
  PersistentIni_SetDword((uint8_t *)g_PersistentSettings.image, PERSISTENT_SETTING_BITS_PER_PIXEL,
                         PERSISTENT_DEFAULT_BITS_PER_PIXEL);
  s_PersistentSettingsWrittenMask |= PersistentSettings_DwordMask(PERSISTENT_SETTING_BITS_PER_PIXEL, 4);
  g_PersistentSettings.dirtyWriteCount++; /* the next Flush writes it */
}

void PersistentSettings_Load(void)
{
  PersistentSettings_LoadImage();
  PersistentSettings_NormalizeColorDepth();
}


/* Returns the setting dword at settingsOffsetBytes, or defaultValue when it was not loaded (no settings file,
   no such key in thandor.ini, or beyond the end of a short thandor.dat).
*/
uint32_t PersistentSettings_Read(PersistentSettingsValue defaultValue,
          PersistentSettingsByteOffset settingsOffsetBytes)

{
  if ((g_PersistentSettings.image != NULL) && PersistentSettings_IsPresent(settingsOffsetBytes,4)) {
    defaultValue = *(PersistentSettingsValue *)((uint8_t *)g_PersistentSettings.image + settingsOffsetBytes);
  }
  return defaultValue;
}


/* Returns a pointer into the settings image at settingsOffsetBytes (not a copy), or fallback when the whole
   region was not loaded. Used for the stored names.
*/
void * PersistentSettings_GetRegionOrFallback(PersistentSettingsByteCount regionByteCount,void *fallback,
          PersistentSettingsByteOffset settingsOffsetBytes)

{
  if ((g_PersistentSettings.image != NULL) &&
     PersistentSettings_IsPresent(settingsOffsetBytes,regionByteCount)) {
    fallback = (uint8_t *)g_PersistentSettings.image + settingsOffsetBytes;
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

  if ((g_PersistentSettings.image != NULL) &&
     (settingsOffsetBytes + regionByteCount < PERSISTENT_SETTINGS_IMAGE_BYTES + 1)) {
    destination = (uint32_t *)((uint8_t *)g_PersistentSettings.image + settingsOffsetBytes);
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
  if ((g_PersistentSettings.image != NULL) &&
      (settingsOffsetBytes + 4 < PERSISTENT_SETTINGS_IMAGE_BYTES + 1)) {
    s_PersistentSettingsWrittenMask |= PersistentSettings_DwordMask(settingsOffsetBytes,4);
    if (*(PersistentSettingsValue *)((uint8_t *)g_PersistentSettings.image + settingsOffsetBytes) != value) {
      *(PersistentSettingsValue *)((uint8_t *)g_PersistentSettings.image + settingsOffsetBytes) = value;
      g_PersistentSettings.dirtyWriteCount++;
    }
  }
}
