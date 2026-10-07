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

/* Byte offsets of the dwords in the persistent settings image (PersistentSettings_Read/Write/WriteBlock), the
   layout of the original thandor.dat; open-thandor saves them as named keys in thandor.ini (persistent.cpp).
   Named as they are found. */
inline constexpr auto PERSISTENT_SETTING_ADAPTER_INDEX = 0x00; /* graphics adapter chosen in the display settings */
inline constexpr auto PERSISTENT_SETTING_DISPLAY_WIDTH = 0x04;
inline constexpr auto PERSISTENT_SETTING_DISPLAY_HEIGHT = 0x08;
inline constexpr auto PERSISTENT_SETTING_BITS_PER_PIXEL = 0x0C;
/* Offsets below found from the PersistentSettings_Read/Write call sites (settings pages, session, world);
   they match the fields of PersistentSettingsImage in core/settings/types.h. */
inline constexpr auto PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE = 0x10; /* default 0x20 */
inline constexpr auto PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION = 0x14; /* default 0x40, written as 2 * grid half size */
inline constexpr auto PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT = 0x18; /* default 0x10, "shading depth" */
inline constexpr auto PERSISTENT_SETTING_SHADING_ENABLED = 0x1C; /* 0/1, default 1 */
inline constexpr auto PERSISTENT_SETTING_SOUND_OPTION_FLAGS = 0x20; /* bit0 effects, bit1 music, bit2 reverse stereo; default 3 */
inline constexpr auto PERSISTENT_SETTING_EFFECTS_GAIN = 0x24; /* Q15, default 0x8000 */
inline constexpr auto PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN = 0x28; /* Q15, default 0x8000 */
inline constexpr auto PERSISTENT_SETTING_MUSIC_GAIN = 0x2C; /* Q15, default 0x8000 */
inline constexpr auto PERSISTENT_SETTING_TEXTURE_QUALITY = 0x30; /* PersistentTextureQualityLevel 0..2 */
inline constexpr auto PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD = 0x34; /* Q8, default 0x10000 */
inline constexpr auto PERSISTENT_SETTING_LOCALE_COUNTRY_CODE = 0x38; /* mirrors g_LocaleCountryCodeOverride */
inline constexpr auto PERSISTENT_SETTING_NETWORK_PLAYER_COUNT = 0x3C; /* default 4 */
inline constexpr auto PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS = 0x40; /* automatic zoom/rotation off, side panel hidden */
inline constexpr auto PERSISTENT_SETTING_GAME_SPEED_PERCENT = 0x44; /* default 100 */
inline constexpr auto PERSISTENT_SETTING_CAMERA_SCROLL_STEP = 0x48; /* default 0x20 */
inline constexpr auto PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN = 0x4C; /* Q15, default 0x8000 */
inline constexpr auto PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS = 0x5C; /* link rotation zoom/tilt, hide panel */
inline constexpr auto PERSISTENT_SETTING_PLAYER_NAME = 0x60; /* UTF-16, PERSISTENT_SETTINGS_NAME_BYTES long */
inline constexpr auto PERSISTENT_SETTING_GAME_NAME = 0x88; /* UTF-16, PERSISTENT_SETTINGS_NAME_BYTES long */
inline constexpr auto PERSISTENT_SETTINGS_NAME_BYTES = 0x28; /* 20 UTF-16 code units */
/* Not in the original (open-thandor additions in the free tail of the image; the original game never reads them and
   writes the whole image back unchanged, so the file stays compatible). A file without them reads 0, the defaults. */
inline constexpr auto PERSISTENT_SETTING_RENDERER = 0xB0; /* PERSISTENT_RENDERER_*: the display settings' renderer */
inline constexpr auto PERSISTENT_SETTING_DISPLAY_MODE_KIND = 0xB4; /* PERSISTENT_DISPLAY_MODE_*: fullscreen, borderless or window */
inline constexpr auto PERSISTENT_SETTING_GPU_RASTERIZATION = 0xB8; /* PERSISTENT_GPU_RASTERIZATION_*: how the GPU renderers draw */
inline constexpr auto PERSISTENT_SETTING_UI_SCALE = 0xBC; /* PERSISTENT_UI_SCALE_* or 1..3: the GPU renderers' UI scale */
inline constexpr auto PERSISTENT_SETTING_VSYNC = 0xC0; /* PERSISTENT_VSYNC_*: vsync of the presents (all renderers) */
inline constexpr auto PERSISTENT_SETTING_FRAME_LIMIT = 0xC4; /* frames per second the presents are capped to, 0 = no limit */
inline constexpr auto PERSISTENT_RENDERER_VULKAN = 0; /* default: SDL_GPU on Vulkan */
inline constexpr auto PERSISTENT_RENDERER_DIRECT3D12 = 1; /* SDL_GPU on Direct3D 12 */
inline constexpr auto PERSISTENT_RENDERER_SOFTWARE = 2; /* the software rasterizer, presented through an SDL_Renderer */
inline constexpr auto PERSISTENT_RENDERER_COUNT = 3;
inline constexpr auto PERSISTENT_DISPLAY_MODE_FULLSCREEN = 0; /* default "Vollbild": exclusive fullscreen in the display mode (or the
                                                 closest larger one), letterboxed */
inline constexpr auto PERSISTENT_DISPLAY_MODE_BORDERLESS = 1; /* "Vollbildfenster": borderless window over the whole display, letterboxed */
inline constexpr auto PERSISTENT_DISPLAY_MODE_WINDOW = 2; /* "Fenster": normal window in the size of the display mode */
inline constexpr auto PERSISTENT_DISPLAY_MODE_COUNT = 3;
inline constexpr auto PERSISTENT_GPU_RASTERIZATION_SMOOTH = 0; /* default: sub-pixel corners, perspective-correct textures (as the
                                                 original's Direct3D renderer) */
inline constexpr auto PERSISTENT_GPU_RASTERIZATION_EXACT = 1; /* the software rasterizer's triangles (pixel-snapped, affine) */
inline constexpr auto PERSISTENT_GPU_RASTERIZATION_COUNT = 2;
inline constexpr auto PERSISTENT_UI_SCALE_AUTO = 0; /* default: the largest integer scale that keeps the UI (mode / scale) at least 1280x720 */
inline constexpr auto PERSISTENT_UI_SCALE_MAX = 3; /* values 1..3: that scale */
inline constexpr auto PERSISTENT_VSYNC_ON = 0; /* default */
inline constexpr auto PERSISTENT_VSYNC_OFF = 1;
inline constexpr auto PERSISTENT_VSYNC_COUNT = 2;
inline constexpr auto PERSISTENT_FRAME_LIMIT_OFF = 0; /* default: no frame rate limit */
inline constexpr auto PERSISTENT_FRAME_LIMIT_MAX = 1000; /* larger saved values are read as no limit; the menu offers 60, 120, 144 */
inline constexpr auto PERSISTENT_SETTINGS_IMAGE_BYTES = 200; /* size of the settings file and of the in-memory image */
/* The bits of PERSISTENT_SETTING_SOUND_OPTION_FLAGS, _MAP_MOUSE_OPTION_FLAGS and _MOUSE_LINK_PANEL_OPTION_FLAGS are
   the flag enum classes PersistentSoundOptionFlags, PersistentMapMouseOptionFlags and
   PersistentMouseLinkPanelOptionFlags (settings/types.h); the typed accessors below read and write those dwords. */
/* Default values passed to PersistentSettings_Read (the settings' defaults listed above) */
inline constexpr auto PERSISTENT_DEFAULT_GAIN_Q15 = 0x8000; /* full volume, all four gain settings */
inline constexpr auto PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE = 0x20;
inline constexpr auto PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION = 0x40;
inline constexpr auto PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT = 0x10;
inline constexpr auto PERSISTENT_DEFAULT_MODEL_LOD_DEPTH_THRESHOLD = 0x10000;
inline constexpr auto PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP = 0x20;
inline constexpr auto PERSISTENT_DEFAULT_BITS_PER_PIXEL = 32; /* the only colour depth (the original's default was 16) */
inline constexpr auto PERSISTENT_DEFAULT_ADAPTER_INDEX = 0;

void PersistentSettings_Flush();

void PersistentSettings_Load();

uint32_t PersistentSettings_Read(PersistentSettingsValue defaultValue,
          PersistentSettingsByteOffset settingsOffsetBytes);

void * PersistentSettings_GetRegionOrFallback(PersistentSettingsByteCount regionByteCount,void *fallback,
          PersistentSettingsByteOffset settingsOffsetBytes);

void PersistentSettings_WriteBlock(PersistentSettingsByteCount regionByteCount,uint32_t *source,
          PersistentSettingsByteOffset settingsOffsetBytes);

void PersistentSettings_Write(PersistentSettingsValue value,PersistentSettingsByteOffset settingsOffsetBytes);

/* Typed accessors of the three option-flag dwords (Read/Write at the same offsets with the same defaults). */
inline PersistentSoundOptionFlags PersistentSettings_ReadSoundOptions()
{
  return FromBits<PersistentSoundOptionFlags>(
      PersistentSettings_Read(ToBits(PERSISTENT_SOUND_OPTION_DEFAULT),PERSISTENT_SETTING_SOUND_OPTION_FLAGS));
}
inline void PersistentSettings_WriteSoundOptions(PersistentSoundOptionFlags flags)
{
  PersistentSettings_Write(ToBits(flags),PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
}
inline PersistentMapMouseOptionFlags PersistentSettings_ReadMapMouseOptions()
{
  return FromBits<PersistentMapMouseOptionFlags>(PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS));
}
inline void PersistentSettings_WriteMapMouseOptions(PersistentMapMouseOptionFlags flags)
{
  PersistentSettings_Write(ToBits(flags),PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
}
inline PersistentMouseLinkPanelOptionFlags PersistentSettings_ReadMouseLinkPanelOptions()
{
  return FromBits<PersistentMouseLinkPanelOptionFlags>(
      PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS));
}
inline void PersistentSettings_WriteMouseLinkPanelOptions(PersistentMouseLinkPanelOptionFlags flags)
{
  PersistentSettings_Write(ToBits(flags),PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
}
/* Not in the original: Write for open-thandor's own settings chosen in its menus; the value is read back at once
   (Write leaves a dword that was not loaded reading its default). */
void PersistentSettings_WriteChosen(PersistentSettingsValue value,PersistentSettingsByteOffset settingsOffsetBytes);

/* open-thandor: the settings are saved as thandor.ini (thandor.dat is only read, as a migration source).
   FormatIni writes the ini text of the keys whose dwords are in presentMask (bit i = byte offset 4 * i) in a
   fixed key order and returns the text length; ParseIni parses ini text into image (zeroed by the caller) and
   returns the mask of the dwords it set (unknown keys and values that do not parse are ignored). */
uint32_t PersistentSettings_FormatIni(const uint8_t *image, uint64_t presentMask, char *out, uint32_t capacity);

uint64_t PersistentSettings_ParseIni(const char *text, uint32_t length, uint8_t *image);

extern uint32_t g_LocaleCountryCodeOverride;

#endif /* THANDOR_CORE_SETTINGS_PERSISTENT_H */
