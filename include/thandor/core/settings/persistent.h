/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/settings/persistent.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_SETTINGS_PERSISTENT_H
#define THANDOR_CORE_SETTINGS_PERSISTENT_H

#include <thandor/core/settings/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/settings/persistent. */
/* Byte offsets of the dwords in the persistent settings image (PersistentSettings_Read/Write/WriteBlock), the
   layout of the original thandor.dat; open-thandor saves them as named keys in thandor.ini (persistent.cpp).
   Named as they are found. */
#define PERSISTENT_SETTING_ADAPTER_INDEX 0x00 /* graphics adapter chosen in the display settings */
#define PERSISTENT_SETTING_DISPLAY_WIDTH 0x04
#define PERSISTENT_SETTING_DISPLAY_HEIGHT 0x08
#define PERSISTENT_SETTING_BITS_PER_PIXEL 0x0C
/* Offsets below found from the PersistentSettings_Read/Write call sites (settings pages, session, world);
   they match the fields of PersistentSettingsImage in core/settings/types.h. */
#define PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE 0x10 /* default 0x20 */
#define PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION 0x14 /* default 0x40, written as 2 * grid half size */
#define PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT 0x18 /* default 0x10, "shading depth" */
#define PERSISTENT_SETTING_SHADING_ENABLED 0x1C /* 0/1, default 1 */
#define PERSISTENT_SETTING_SOUND_OPTION_FLAGS 0x20 /* bit0 effects, bit1 music, bit2 reverse stereo; default 3 */
#define PERSISTENT_SETTING_EFFECTS_GAIN 0x24 /* Q15, default 0x8000 */
#define PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN 0x28 /* Q15, default 0x8000 */
#define PERSISTENT_SETTING_MUSIC_GAIN 0x2C /* Q15, default 0x8000 */
#define PERSISTENT_SETTING_TEXTURE_QUALITY 0x30 /* PersistentTextureQualityLevel 0..2 */
#define PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD 0x34 /* Q8, default 0x10000 */
#define PERSISTENT_SETTING_LOCALE_COUNTRY_CODE 0x38 /* mirrors g_LocaleCountryCodeOverride */
#define PERSISTENT_SETTING_NETWORK_PLAYER_COUNT 0x3C /* default 4 */
#define PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS 0x40 /* right button does not scroll, automatic zoom/rotation off */
#define PERSISTENT_SETTING_GAME_SPEED_PERCENT 0x44 /* default 100 */
#define PERSISTENT_SETTING_CAMERA_SCROLL_STEP 0x48 /* default 0x20 */
#define PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN 0x4C /* Q15, default 0x8000 */
#define PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS 0x5C /* link rotation zoom/tilt, hide panel */
#define PERSISTENT_SETTING_PLAYER_NAME 0x60 /* UTF-16, PERSISTENT_SETTINGS_NAME_BYTES long */
#define PERSISTENT_SETTING_GAME_NAME 0x88 /* UTF-16, PERSISTENT_SETTINGS_NAME_BYTES long */
#define PERSISTENT_SETTING_RENDERER 0xB0 /* 0 Vulkan (default), 1 DirectX 12, 2 Software; open-thandor addition */
#define PERSISTENT_SETTING_DISPLAY_MODE_KIND 0xB4 /* 0 fullscreen (default), 1 borderless, 2 window; open-thandor addition */
#define PERSISTENT_SETTINGS_NAME_BYTES 0x28 /* 20 UTF-16 code units */
/* Not in the original (open-thandor additions in the free tail of the image; the original game never reads them and
   writes the whole image back unchanged, so the file stays compatible). A file without them reads 0, the defaults. */
#define PERSISTENT_SETTING_RENDERER 0xB0 /* PERSISTENT_RENDERER_*: the display settings' renderer */
#define PERSISTENT_SETTING_DISPLAY_MODE_KIND 0xB4 /* PERSISTENT_DISPLAY_MODE_*: fullscreen, borderless or window */
#define PERSISTENT_RENDERER_VULKAN 0 /* default: SDL_GPU on Vulkan */
#define PERSISTENT_RENDERER_DIRECT3D12 1 /* SDL_GPU on Direct3D 12 */
#define PERSISTENT_RENDERER_SOFTWARE 2 /* the software rasterizer, presented through an SDL_Renderer */
#define PERSISTENT_RENDERER_COUNT 3
#define PERSISTENT_DISPLAY_MODE_FULLSCREEN 0 /* default "Vollbild": exclusive fullscreen in the display mode (or the
                                                 closest larger one), letterboxed */
#define PERSISTENT_DISPLAY_MODE_BORDERLESS 1 /* "Vollbildfenster": borderless window over the whole display, letterboxed */
#define PERSISTENT_DISPLAY_MODE_WINDOW 2 /* "Fenster": normal window in the size of the display mode */
#define PERSISTENT_DISPLAY_MODE_COUNT 3
#define PERSISTENT_SETTINGS_IMAGE_BYTES 200 /* size of the settings file and of the in-memory image */
/* Bits of PERSISTENT_SETTING_SOUND_OPTION_FLAGS (Game_LoadCoreAssets) */
#define PERSISTENT_SOUND_OPTION_EFFECTS 0x1
#define PERSISTENT_SOUND_OPTION_MUSIC 0x2
#define PERSISTENT_SOUND_OPTION_REVERSE_STEREO 0x4
#define PERSISTENT_SOUND_OPTION_DEFAULT 3 /* effects and music on */
/* Bits of PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS (gameplay settings page, InGameRuntime_UpdateCursorGridAndViewScaleCache) */
#define PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF 0x1
#define PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF 0x2
#define PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN 0x4
/* Bits of PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS (in-game gameplay settings page), mirrored to
   WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM / _LINK_ROTATION_TILT / _HIDE_PANEL */
#define PERSISTENT_LINK_OPTION_ROTATION_ZOOM 0x1
#define PERSISTENT_LINK_OPTION_ROTATION_TILT 0x2
#define PERSISTENT_LINK_OPTION_HIDE_PANEL 0x4
/* Default values passed to PersistentSettings_Read (the settings' defaults listed above) */
#define PERSISTENT_DEFAULT_GAIN_Q15 0x8000 /* full volume, all four gain settings */
#define PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE 0x20
#define PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION 0x40
#define PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT 0x10
#define PERSISTENT_DEFAULT_MODEL_LOD_DEPTH_THRESHOLD 0x10000
#define PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP 0x20
#define PERSISTENT_DEFAULT_BITS_PER_PIXEL 32 /* the only colour depth (the original's default was 16) */
#define PERSISTENT_DEFAULT_ADAPTER_INDEX 0

/* Functions are grouped by semantic ownership. */

void PersistentSettings_Flush(void);

void PersistentSettings_Load(void);

uint32_t PersistentSettings_Read(PersistentSettingsValue defaultValue,
          PersistentSettingsByteOffset settingsOffsetBytes);

void * PersistentSettings_GetRegionOrFallback(PersistentSettingsByteCount regionByteCount,void *fallback,
          PersistentSettingsByteOffset settingsOffsetBytes);

void PersistentSettings_WriteBlock(PersistentSettingsByteCount regionByteCount,uint32_t *source,
          PersistentSettingsByteOffset settingsOffsetBytes);

void PersistentSettings_Write(PersistentSettingsValue value,PersistentSettingsByteOffset settingsOffsetBytes);

/* open-thandor: the settings are saved as thandor.ini (thandor.dat is only read, as a migration source).
   FormatIni writes the ini text of the keys whose dwords are in presentMask (bit i = byte offset 4 * i) in a
   fixed key order and returns the text length; ParseIni parses ini text into image (zeroed by the caller) and
   returns the mask of the dwords it set (unknown keys and values that do not parse are ignored). */
uint32_t PersistentSettings_FormatIni(const uint8_t *image, uint64_t presentMask, char *out, uint32_t capacity);

uint64_t PersistentSettings_ParseIni(const char *text, uint32_t length, uint8_t *image);

extern uint32_t g_LocaleCountryCodeOverride;

#endif /* THANDOR_CORE_SETTINGS_PERSISTENT_H */
