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

/* Types (split out by tools/dev/split_types.py). */

typedef struct PersistentSettingsRuntime PersistentSettingsRuntime, *PPersistentSettingsRuntime;
typedef struct PersistentSettingsImage PersistentSettingsImage, *PPersistentSettingsImage;

enum {
    PERSISTENT_TOGGLE_DISABLED=0,
    PERSISTENT_TOGGLE_ENABLED=1
};
typedef int PersistentToggleState;

enum {
    PERSISTENT_MOUSE_LINK_ROTATION_ZOOM=1,
    PERSISTENT_MOUSE_LINK_ROTATION_TILT=2,
    PERSISTENT_UI_HIDE_PANEL=4
};
typedef int PersistentMouseLinkPanelOptionFlags;

enum {
    PERSISTENT_MAP_AUTOMATIC_ZOOM_OFF=1,
    PERSISTENT_MAP_AUTOMATIC_ROTATION_OFF=2,
    PERSISTENT_MOUSE_RIGHT_BUTTON_DOES_NOT_SCROLL=4
};
typedef int PersistentMapMouseOptionFlags;

typedef uint32_t PersistentSettingsValue;

typedef uint32_t PersistentDisplayAdapterIndex;

typedef uint32_t PersistentDisplayDimensionPixels;

typedef uint32_t PersistentShadingSubresourceCount;

typedef uint32_t NetworkPlayerCount;

typedef int PersistentModelLodDepthQ8;

typedef uint32_t PersistentShadingTextureDimension;

typedef uint32_t LocaleCountryCode;

typedef int CameraScrollStepPixels;

typedef uint32_t PersistentSettingsMutationCount;

typedef uint32_t GameSpeedPercent;

typedef uint32_t PersistentColorDepthBits;

typedef uint32_t PersistentSettingsByteOffset;

typedef uint32_t PersistentSettingsByteCount;

typedef uint32_t PersistentShadingGridHalfSize;

enum {
    SOUND_OPTIONS_EFFECTS_ENABLED=1,
    SOUND_OPTIONS_MUSIC_ENABLED=2,
    SOUND_OPTIONS_REVERSE_STEREO=4
};
typedef int PersistentSoundOptionFlags;

enum {
    TEXTURE_QUALITY_HIGH=0,
    TEXTURE_QUALITY_MEDIUM=1,
    TEXTURE_QUALITY_LOW=2
};
typedef int PersistentTextureQualityLevel;

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
