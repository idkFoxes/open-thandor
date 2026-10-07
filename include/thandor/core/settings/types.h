/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/settings/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_SETTINGS_TYPES_H
#define THANDOR_CORE_SETTINGS_TYPES_H

#include <stdint.h>
#include <thandor/core/flags.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/audio/spatial/types.h>
#include <thandor/movie/runtime/types.h>

struct PersistentSettingsRuntime;
struct PersistentSettingsImage;

using PersistentToggleState = int; /* 0 off, 1 on */

/* PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS (in-game gameplay settings page), mirrored to
   WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM / _LINK_ROTATION_TILT / _HIDE_PANEL. Flag enum classes (step 13) over the
   uint32_t the settings dword API reads and writes; the file dwords keep their bits (also unnamed ones). */
enum class PersistentMouseLinkPanelOptionFlags : uint32_t {
    PERSISTENT_LINK_OPTION_ROTATION_ZOOM = 0x1,
    PERSISTENT_LINK_OPTION_ROTATION_TILT = 0x2,
    PERSISTENT_LINK_OPTION_HIDE_PANEL = 0x4
};
THANDOR_FLAG_ENUM(PersistentMouseLinkPanelOptionFlags);
using enum PersistentMouseLinkPanelOptionFlags;

/* PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS (gameplay settings page, InGameRuntime_UpdateCursorGridAndViewScaleCache). */
enum class PersistentMapMouseOptionFlags : uint32_t {
    PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF = 0x1,
    PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF = 0x2,
    PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN = 0x4 /* checkbox text "right button does not scroll" */
};
THANDOR_FLAG_ENUM(PersistentMapMouseOptionFlags);
using enum PersistentMapMouseOptionFlags;

using PersistentSettingsValue = uint32_t;

using PersistentDisplayAdapterIndex = uint32_t;

using PersistentDisplayDimensionPixels = uint32_t;

using PersistentShadingSubresourceCount = uint32_t;

using NetworkPlayerCount = uint32_t;

using PersistentModelLodDepthQ8 = int;

using PersistentShadingTextureDimension = uint32_t;

using LocaleCountryCode = uint32_t;

using CameraScrollStepPixels = int;

using PersistentSettingsMutationCount = uint32_t;

using GameSpeedPercent = uint32_t;

using PersistentColorDepthBits = uint32_t;

using PersistentSettingsByteOffset = uint32_t;

using PersistentSettingsByteCount = uint32_t;

using PersistentShadingGridHalfSize = uint32_t;

/* PERSISTENT_SETTING_SOUND_OPTION_FLAGS (Game_LoadCoreAssets). */
enum class PersistentSoundOptionFlags : uint32_t {
    PERSISTENT_SOUND_OPTION_EFFECTS = 0x1,
    PERSISTENT_SOUND_OPTION_MUSIC = 0x2,
    PERSISTENT_SOUND_OPTION_REVERSE_STEREO = 0x4,
    PERSISTENT_SOUND_OPTION_DEFAULT = 0x3 /* effects and music on */
};
THANDOR_FLAG_ENUM(PersistentSoundOptionFlags);
using enum PersistentSoundOptionFlags;

enum {
    TEXTURE_QUALITY_HIGH=0,
    TEXTURE_QUALITY_MEDIUM=1,
    TEXTURE_QUALITY_LOW=2
};
using PersistentTextureQualityLevel = int;

struct PersistentSettingsRuntime {
    Ptr32<struct PersistentSettingsImage> image; 
    PersistentSettingsByteCount loadedByteCount; 
    PersistentSettingsMutationCount dirtyWriteCount; 
    uint16_t path[256]; 
};

struct PersistentSettingsImage {
    PersistentDisplayAdapterIndex graphicsAdapterIndex; 
    PersistentDisplayDimensionPixels displayWidth; 
    PersistentDisplayDimensionPixels displayHeight; 
    PersistentColorDepthBits displayBitsPerPixel; 
    PersistentShadingGridHalfSize shadingGridHalfSize; 
    PersistentShadingTextureDimension shadingTextureDimension; 
    PersistentShadingSubresourceCount shadingTextureSubresourceCount; 
    PersistentToggleState shadingEnabled; 
    PersistentSoundOptionFlags soundOptionFlags; 
    SpatialSoundGainQ15 soundEffectsGainQ15; 
    MovieAudioGainQ15 movieDefaultAudioGainQ15; 
    SpatialSoundGainQ15 musicGainQ15; 
    PersistentTextureQualityLevel textureQualityLevel; 
    PersistentModelLodDepthQ8 modelLodDepthThresholdQ8; 
    LocaleCountryCode localeCountryCodeOverride; 
    NetworkPlayerCount networkPlayerCount; 
    PersistentMapMouseOptionFlags mapMouseOptionFlags; 
    GameSpeedPercent gameSpeedPercent; 
    CameraScrollStepPixels cameraScrollStep; 
    MovieAudioGainQ15 movieAlternateAudioGainQ15; 
    uint8_t reserved50_5B[12]; 
    PersistentMouseLinkPanelOptionFlags mouseLinkPanelOptionFlags; 
    uint16_t playerName[20]; 
    uint16_t gameName[20]; 
    uint32_t renderer; /* open-thandor: PERSISTENT_SETTING_RENDERER (0 = Vulkan) */
    uint32_t displayModeKind; /* open-thandor: PERSISTENT_SETTING_DISPLAY_MODE_KIND (0 = fullscreen) */
    uint32_t gpuRasterization; /* open-thandor: PERSISTENT_SETTING_GPU_RASTERIZATION (0 = smooth) */
    uint32_t uiScale; /* open-thandor: PERSISTENT_SETTING_UI_SCALE (0 = auto) */
    uint32_t vsync; /* open-thandor: PERSISTENT_SETTING_VSYNC (0 = on) */
    uint32_t frameLimit; /* open-thandor: PERSISTENT_SETTING_FRAME_LIMIT (0 = no limit) */
};

#endif /* THANDOR_CORE_SETTINGS_TYPES_H */
