/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/settings/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_SETTINGS_TYPES_H
#define THANDOR_CORE_SETTINGS_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/audio/spatial/types.h>
#include <thandor/movie/runtime/types.h>

typedef struct PersistentSettingsRuntime PersistentSettingsRuntime, *PPersistentSettingsRuntime;
typedef struct PersistentSettingsImage PersistentSettingsImage, *PPersistentSettingsImage;

enum {
    PERSISTENT_TOGGLE_DISABLED=0,
    PERSISTENT_TOGGLE_ENABLED=1
};
using PersistentToggleState = int;

enum {
    PERSISTENT_MOUSE_LINK_ROTATION_ZOOM=1,
    PERSISTENT_MOUSE_LINK_ROTATION_TILT=2,
    PERSISTENT_UI_HIDE_PANEL=4
};
using PersistentMouseLinkPanelOptionFlags = int;

enum {
    PERSISTENT_MAP_AUTOMATIC_ZOOM_OFF=1,
    PERSISTENT_MAP_AUTOMATIC_ROTATION_OFF=2,
    PERSISTENT_MOUSE_RIGHT_BUTTON_DOES_NOT_SCROLL=4
};
using PersistentMapMouseOptionFlags = int;

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

enum {
    SOUND_OPTIONS_EFFECTS_ENABLED=1,
    SOUND_OPTIONS_MUSIC_ENABLED=2,
    SOUND_OPTIONS_REVERSE_STEREO=4
};
using PersistentSoundOptionFlags = int;

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
    uint8_t reservedBC_C7[12]; 
};

#endif /* THANDOR_CORE_SETTINGS_TYPES_H */
