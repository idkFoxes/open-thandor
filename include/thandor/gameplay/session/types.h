/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_TYPES_H
#define THANDOR_GAMEPLAY_SESSION_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/package/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/faction/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>

/* Types (split out by tools/dev/split_types.py). */

typedef union ResourceRegistrationRuntimePayloadReference4 ResourceRegistrationRuntimePayloadReference4, *PResourceRegistrationRuntimePayloadReference4;
typedef struct WorldObjectRecord WorldObjectRecord, *PWorldObjectRecord;
typedef struct WorldObjectRecordCommon WorldObjectRecordCommon, *PWorldObjectRecordCommon;
typedef struct LevelAssetRuntimePrefix LevelAssetRuntimePrefix, *PLevelAssetRuntimePrefix;
typedef struct LevelAssetHeader LevelAssetHeader, *PLevelAssetHeader;
typedef struct LevelPlayerSlotRecord LevelPlayerSlotRecord, *PLevelPlayerSlotRecord;
typedef struct LevelAssetPathOffsets LevelAssetPathOffsets, *PLevelAssetPathOffsets;
typedef struct LevelAssetResourceTables LevelAssetResourceTables, *PLevelAssetResourceTables;
typedef struct LevelWorldSettings LevelWorldSettings, *PLevelWorldSettings;
typedef struct ResourceRegistrationRuntimeImageSavedView ResourceRegistrationRuntimeImageSavedView, *PResourceRegistrationRuntimeImageSavedView;
typedef struct ResourceRegistrationRecordSavedView ResourceRegistrationRecordSavedView, *PResourceRegistrationRecordSavedView;
typedef struct ResourceRegistrationRecord ResourceRegistrationRecord, *PResourceRegistrationRecord;
typedef union ResourceRegistrationPointerOrSavedOffset4 ResourceRegistrationPointerOrSavedOffset4, *PResourceRegistrationPointerOrSavedOffset4;
typedef struct InGameFieldImageSaveContext58 InGameFieldImageSaveContext58, *PInGameFieldImageSaveContext58;
typedef struct PckArchiveHeader PckArchiveHeader, *PPckArchiveHeader;
typedef struct ResourceRegistrationRuntimeImage ResourceRegistrationRuntimeImage, *PResourceRegistrationRuntimeImage;
typedef struct InGameLevelSaveWorldView InGameLevelSaveWorldView, *PInGameLevelSaveWorldView;
typedef struct InGameLevelRuntimeGlobalBlock20 InGameLevelRuntimeGlobalBlock20, *PInGameLevelRuntimeGlobalBlock20;
typedef struct LevelInitialArmyPlacementRecord20 LevelInitialArmyPlacementRecord20, *PLevelInitialArmyPlacementRecord20;
typedef struct ArmyRuntimeSlot ArmyRuntimeSlot;
typedef struct EffectRuntimeSlot EffectRuntimeSlot;
typedef struct FieldGridAsset FieldGridAsset;
typedef struct InGameLevelConditionStorage InGameLevelConditionStorage;
typedef struct ShotRuntimeSlot ShotRuntimeSlot;
typedef struct SpriteAssetHeader SpriteAssetHeader;

typedef uint32_t LevelLightingCycleDurationTicks;
typedef uint32_t LevelFactionRelationGroupMasks32;
typedef uint32_t LevelMusicSampleNumber;
typedef uint32_t LevelEffectSampleNumber;
typedef uint32_t LevelRuntimePrefixByteSizeAndInitialArmyPlacementOffset;
typedef uint32_t LevelPlayerSlotByteOffset32;

typedef uint32_t WorldObjectAllocationFlags;

union ResourceRegistrationRuntimePayloadReference4 {
    Ptr32<struct ArmyRuntimeSlot> armyRuntime;
    Ptr32<struct EffectRuntimeSlot> effectRuntime;
    Ptr32<struct ShotRuntimeSlot> shotRuntime;
    uint32_t savedOffset;
    uint32_t raw;
};

struct WorldObjectRecordCommon {
    uint8_t reserved00_07[8]; 
    Ptr32<struct WorldRuntimeContext> ownerWorld; 
    uint8_t reserved0C_4B[64]; 
    WorldObjectAllocationFlags allocationFlags; 
};

struct WorldObjectRecord {
    struct WorldObjectRecordCommon common; 
    uint8_t classPayload[176]; 
};

enum {
    RUNTIME_REGISTRATION_RECORD_ALLOCATED=1073741824
};
typedef int RuntimeRegistrationRecordFlags;

typedef uint32_t LevelCampaignAssociationIndex;

typedef uint32_t LevelAssetRelativeByteOffset;

typedef uint32_t LevelPackedHeadingPitch;

typedef uint32_t LevelStartCameraMagnitudeQ12;

typedef uint32_t InGameLoadedResourcePointerCount;

typedef uint32_t LevelPlayerAiClassOrMode;

typedef int LevelStartCameraCoordinateQ12;

typedef uint32_t LevelAssetRecordCount;

typedef uint32_t OldUnitRecordCount;

struct LevelAssetResourceTables {
    LevelRuntimePrefixByteSizeAndInitialArmyPlacementOffset runtimePrefixByteSizeAndInitialArmyPlacementOffset; // Dual-use LEV header value: exact byte count copied from file start into mutable level runtime storage, and file-relative offset of the 0x20-byte initial army placement table.
    LevelAssetRecordCount armyAssetPathCount; // ARM path-record count.
    LevelAssetRelativeByteOffset armyAssetPathTableOffset; // ARM path table offset.
    LevelAssetRecordCount modelAssetPathCount; // MDL path-record count.
    LevelAssetRelativeByteOffset modelAssetPathTableOffset; // MDL path table offset.
    LevelAssetRecordCount effectAssetPathCount; // EFF path-record count.
    LevelAssetRelativeByteOffset effectAssetPathTableOffset; // EFF path table offset.
    LevelAssetRecordCount shotAssetPathCount; // SHT path-record count.
    LevelAssetRelativeByteOffset shotAssetPathTableOffset; // SHT path table offset.
};

struct LevelAssetPathOffsets {
    AssetRelativeOffset levelPathOffset;
    AssetRelativeOffset groundTextureBasePathOffset;
    AssetRelativeOffset surfaceTextureBasePathOffset;
    AssetRelativeOffset skyTextureBasePathOffset;
    AssetRelativeOffset armyTextureBasePathOffset;
    AssetRelativeOffset shotTextureBasePathOffset;
    AssetRelativeOffset effectTextureBasePathOffset;
    AssetRelativeOffset endingMovieBasePathOffset;
    AssetRelativeOffset soundBasePathOffset;
    AssetRelativeOffset technologyPathOffset;
};

struct LevelAssetHeader {
    struct GeneratedAssetCommonPrefix common;
    struct LevelAssetPathOffsets pathOffsets;
    LevelAssetRecordCount initialArmyPlacementRecordCount; // Number of 0x20-byte initial army placement records at resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset.
    struct LevelAssetResourceTables resourceTables; // Counts and UTF-16 path-table offsets for ARM, MDL, EFF, and SHT assets.
    uint16_t levelFileNameUtf16[56]; // UTF-16 level file base name (level\<name>.lev); "t00_tu..." marks the tutorial levels.
    UiTextResourceId titleTextResourceIndex; // Localized level title resource identifier.
    uint8_t opaque174_18F[28]; // Level-header region without closed field evidence.
    LevelCampaignAssociationIndex campaignAssociationIndex; // Campaign association index.
    uint8_t opaque194_1FF[108]; // Level-header tail without closed field evidence.
};

struct LevelPlayerSlotRecord {
    LevelStartCameraCoordinateQ12 startCameraXQ12; 
    LevelStartCameraCoordinateQ12 startCameraYQ12; 
    LevelStartCameraCoordinateQ12 startCameraZQ12; 
    LevelStartCameraMagnitudeQ12 startCameraMagnitudeQ12; 
    LevelPackedHeadingPitch packedHeadingLow16PitchHigh16; 
    XeniteAmountQ4 startXeniteQ4; 
    TritiumAmountQ4 startTritiumQ4; 
    LevelPlayerAiClassOrMode aiClassOrMode; 
};

struct LevelWorldSettings {
    uint32_t packedFieldRegionOriginYHigh16XLow16;
    PackedArgb32 terrainRampStepColorArgb;
    PackedArgb32 terrainBaseColorArgb;
    LevelLightingCycleDurationTicks terrainLightingCycleDurationTicks; // Modulo period for WorldLightingRuntime_UpdateInterpolatedTerrainLighting; zero disables periodic interpolation.
    uint32_t packedFieldRegionHeightHigh16WidthLow16;
    PackedArgb32 terrainLightingColor128Argb;
    PackedArgb32 terrainSecondaryColorArgb;
    uint32_t reserved1C;
    PackedArgb32 terrainLightingColor130Argb;
    PackedArgb32 terrainLightingColor134Argb;
    PackedArgb32 terrainLightingColor138Argb;
    PackedArgb32 terrainLightingColor13CArgb;
    uint32_t assignableFactionCount; // Count of initially assignable factions. Frontend faction assignment loops consume this count and wrap factionAssignmentIndex modulo it.
    uint32_t activeFactionCount; // Total active faction count copied into GameFactionRuntimeImageTail and used as the upper faction index/count.
    GameRelationUiFlags relationUiFlags; // Copied directly into GameFactionRuntimeImage.tail.relationUiFlags during level initialization.
    LevelFactionRelationGroupMasks32 relationState8FactionGroupMasks; // Packed faction groups whose pairwise members are transitioned with state arguments 8/8 during level initialization.
    LevelFactionRelationGroupMasks32 relationState4FactionGroupMasks; // Packed faction groups whose pairwise members are transitioned with state arguments 4/4 during level initialization.
    InGameNotificationMovieId introNotificationMovieId; /* +0x44: first of the five level intro notification movies (0 = none) */
    uint32_t alternatePackedFieldRegionOriginYHigh16XLow16; // Second field-region origin pair interpolated against packedFieldRegionOriginYHigh16XLow16.
    uint32_t alternatePackedFieldRegionHeightHigh16WidthLow16; // Second field-region dimension pair interpolated against packedFieldRegionHeightHigh16WidthLow16.
    PackedArgb32 alternateTerrainRampStepColorArgb; // Second endpoint for terrain ramp-step colour (+0x120) interpolation.
    PackedArgb32 alternateTerrainBaseColorArgb; // Second endpoint for terrain base colour (+0x124) interpolation.
    PackedArgb32 alternateTerrainLightingColor128Argb; // Second endpoint for terrain lighting color +0x128 interpolation.
    PackedArgb32 alternateTerrainSecondaryColorArgb; // Second endpoint for terrain secondary colour (+0x12C) interpolation.
    PackedArgb32 alternateTerrainLightingColor130Argb; // Second endpoint for terrain lighting color +0x130 interpolation.
    PackedArgb32 alternateTerrainLightingColor134Argb; // Second endpoint for terrain lighting color +0x134 interpolation.
    PackedArgb32 alternateTerrainLightingColor138Argb; // Second endpoint for terrain lighting color +0x138 interpolation.
    PackedArgb32 alternateTerrainLightingColor13CArgb; // Second endpoint for terrain lighting color +0x13C interpolation.
    LevelMusicSampleNumber musicSampleNumbers[4]; // Four level music sample selectors, indexed 0..3 by end-game music selection and level loading.
    LevelEffectSampleNumber effectSampleNumbers[4]; // Four level-effect sample selectors, indexed 0..3 by level loading.
};

struct LevelAssetRuntimePrefix {
    struct LevelAssetHeader header; // Exact serialized level header at image offset 0x000.
    struct LevelPlayerSlotRecord playerSlots[7]; // Seven exact player camera/resource records at image offset 0x200.
    struct LevelWorldSettings worldSettings; // Terrain lighting, field region, faction counts and relations, intro movie and sample selectors at image offset 0x2E0.
};

enum {
    RESOURCE_DOMAIN_ARMY_RUNTIME=0,
    RESOURCE_DOMAIN_SHOT_RUNTIME=1,
    RESOURCE_DOMAIN_EFFECT_RUNTIME=2
};
typedef int ResourceRegistrationDomainIndex;

struct ResourceRegistrationRecordSavedView {
    uint32_t primarySavedIdOrOffset; 
    uint32_t secondarySavedIdOrOffset; 
    uint32_t ownerRuntimeSavedOffset; 
    uint8_t reserved000C_002F[36]; 
    uint32_t paletteAssetSavedIdOrOffset; 
    uint32_t textureSetSavedIdOrOffset; 
    uint8_t reserved0038_003F[8]; 
    uint32_t spriteAssetSavedIdOrOffset; 
    uint8_t reserved0044_0047[4]; 
    uint32_t runtimePayloadSavedOffset; 
    RuntimeRegistrationRecordFlags flags; 
    uint8_t reserved0050_005B[12]; 
    uint32_t auxiliarySavedIdOrOffset; 
    uint8_t reserved0060_00A3[68]; 
    ResourceRegistrationDomainIndex domainIndex; 
    uint8_t reserved00A8_00C3[28]; 
    uint32_t nestedBaseSavedOffset; 
    uint32_t nestedCount; 
    uint32_t nestedSavedOffsets[13]; 
};

union ResourceRegistrationPointerOrSavedOffset4 {
    Ptr32<void> runtimePointer; 
    uint32_t savedIdOrOffset; 
    uint32_t raw; 
};

struct ResourceRegistrationRuntimeImageSavedView {
    uint8_t reserved0000_004F[80]; 
    uint32_t factionAssignmentIndex; 
    uint8_t reserved0054_0057[4]; 
    Ptr32<struct ResourceRegistrationRecordSavedView> records; 
    uint8_t reserved005C_00AB[80]; 
    uint32_t recordCount; 
    uint8_t reserved00B0_00D7[40]; 
    Ptr32<struct ResourceRegistrationRecord> tailRecord; 
};

struct ResourceRegistrationRecord {
    union ResourceRegistrationPointerOrSavedOffset4 primaryPointerOrSavedOffset; 
    union ResourceRegistrationPointerOrSavedOffset4 secondaryPointerOrSavedOffset; 
    union ResourceRegistrationPointerOrSavedOffset4 ownerRuntimeOrSavedOffset; 
    uint8_t reserved000C_002F[36]; 
    Ptr32<struct GraphicsPaletteAsset> paletteAsset; 
    Ptr32<struct GraphicsTextureSet> textureSet; 
    uint8_t reserved0038_003F[8]; 
    Ptr32<struct SpriteAssetHeader> spriteAsset; 
    uint8_t reserved0044_0047[4]; 
    union ResourceRegistrationRuntimePayloadReference4 runtimePayload; 
    RuntimeRegistrationRecordFlags flags; 
    uint8_t reserved0050_005B[12]; 
    union ResourceRegistrationPointerOrSavedOffset4 auxiliaryPointerOrSavedOffset; 
    uint8_t reserved0060_00A3[68]; 
    ResourceRegistrationDomainIndex domainIndex; 
    uint8_t reserved00A8_00C3[28]; 
    union ResourceRegistrationPointerOrSavedOffset4 nestedBasePointerOrSavedOffset; 
    uint32_t nestedCount; 
    union ResourceRegistrationPointerOrSavedOffset4 nestedPointersOrSavedOffsets[13]; 
};

struct InGameFieldImageSaveContext58 {
    uint8_t opaqueRuntimePrefix00_53[84]; 
    Ptr32<struct FieldGridAsset> fieldGridAsset; 
};

typedef uint32_t PckArchiveByteCount;

typedef uint32_t PckArchiveVersion;

typedef uint32_t PckArchiveFormat;

typedef uint32_t PckPackedDate;

typedef uint32_t PckPackedTime;

struct PckArchiveHeader {
    uint8_t magic[4]; 
    PckArchiveByteCount archiveSize; 
    PckArchiveVersion version; 
    PckArchiveFormat format; 
    PckPackedTime timeValue0; // Time/date pairs, time first (see AssetBuildTimestampSet).
    PckPackedDate dateValue0;
    PckPackedTime timeValue1;
    PckPackedDate dateValue1;
    PckPackedTime timeValue2;
    PckPackedDate dateValue2;
    uint8_t reserved28_2F[8];
    uint16_t producerName[32];
    uint16_t sourceName[32];
    PckEntryCount entryCount;
    uint8_t reservedB4_FF[76];
    char unusedText[256]; // Zero-terminated text that is empty in every stock archive and never read; InGameSaveGame_CreatePackage clears its first byte like every other asset writer.
};

struct ResourceRegistrationRuntimeImage {
    uint8_t reserved0000_004F[80]; 
    uint32_t factionAssignmentIndex; 
    uint8_t reserved0054_0057[4]; 
    Ptr32<struct ResourceRegistrationRecord> records; 
    uint8_t reserved005C_00AB[80]; 
    uint32_t recordCount; 
    uint8_t reserved00B0_00D7[40]; 
    Ptr32<struct ResourceRegistrationRecord> tailRecord; 
};

typedef uint64_t ResourceRegistrationImagePair;

/* One runtime save-segment image: the block to write and its byte size. */
typedef struct RuntimeHexSegmentImage {
    Ptr32<uint32_t> image;
    uint32_t byteSize;
} RuntimeHexSegmentImage;

struct InGameLevelSaveWorldView {
    struct WorldRuntimeContext worldRuntime;
    uint8_t reserved15C_177[28];
    int32_t lightAzimuthAngle; // InGameRuntimeRoot.lightAzimuthAngle
    int32_t lightElevationAngle; // InGameRuntimeRoot.lightElevationAngle
};

struct InGameLevelRuntimeGlobalBlock20 {
    Ptr32<struct InGameLevelConditionStorage> conditionStorage; // Allocated mutable level-image/schedule storage.
    LevelPlayerSlotByteOffset32 playerSlotByteOffsets[7]; // Seven 0x20-byte player-slot offsets; machine code indexes these through selectors 1..7 from the block base.
};

struct LevelInitialArmyPlacementRecord20 {
    PckArmyAssetIdCatalog armyAssetId;
    FactionRuntimeIndex factionIndex;
    Q12 worldYQ12;
    Q12 worldXQ12;
    AngleTurn32 orientationAngle;
    uint8_t zeroPadding[12]; // Written as zero by the level saver, ignored by the loader.
};

/* Level script: condition schedule and end triggers (InGameLevelConditionStorage). */

typedef struct InGameEndConditionTriggerRecord8 InGameEndConditionTriggerRecord8, *PInGameEndConditionTriggerRecord8;
typedef struct InGameConditionSchedule InGameConditionSchedule, *PInGameConditionSchedule;
typedef struct InGameScheduledConditionRecord10 InGameScheduledConditionRecord10, *PInGameScheduledConditionRecord10;
typedef union InGameScheduledConditionStatusAndKind4 InGameScheduledConditionStatusAndKind4, *PInGameScheduledConditionStatusAndKind4;
typedef union InGameScheduledConditionPayload0C InGameScheduledConditionPayload0C, *PInGameScheduledConditionPayload0C;
typedef struct InGameEndConditionTriggerRecord8ReferenceView InGameEndConditionTriggerRecord8ReferenceView, *PInGameEndConditionTriggerRecord8ReferenceView;
typedef struct InGameLevelConditionStorage InGameLevelConditionStorage, *PInGameLevelConditionStorage;

enum {
    INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED=0,
    INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY=2,
    INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_COMMAND_GROUP_A_ARMY=4,
    INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY_OF_ASSET=6,
    INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8=8,
    INGAME_SCHEDULED_CONDITION_XENITE_AT_LEAST=10,
    INGAME_SCHEDULED_CONDITION_TRITIUM_AT_LEAST=12,
    INGAME_SCHEDULED_CONDITION_TRITIUM_EXTRACTION_RATE_AT_LEAST=14,
    INGAME_SCHEDULED_CONDITION_ARMY_OF_ASSET_COUNT_AT_LEAST=16,
    INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST=18,
    INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED=20,
    INGAME_SCHEDULED_CONDITION_XENITE_STORAGE_LIMIT_AT_MOST_0FA0=22,
    INGAME_SCHEDULED_CONDITION_NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A=24,
    INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION=26
};
typedef int InGameScheduledConditionKind;

enum /* InGameEndConditionTriggerStateFlags, stored in 1 byte(s) */ {
    INGAME_END_CONDITION_TRIGGER_ACTIVE=1,
    INGAME_END_CONDITION_TRIGGER_PROCESSED=2
};
typedef uint8_t InGameEndConditionTriggerStateFlags;

enum {
    INGAME_SCHEDULED_CONDITION_SATISFIED=1,
    INGAME_SCHEDULED_CONDITION_KIND_MASK=254
};
typedef int InGameScheduledConditionStatusFlags;

struct InGameEndConditionTriggerRecord8 {
    uint8_t stateFlags;
    uint8_t movieVariantSelector;
    uint8_t skipArmyDisableWhenOne;
    uint8_t reserved03;
    uint8_t factionRuntimeIndex;
    uint8_t endMovieSelectionIndex;
    uint8_t conditionIndex;
    uint8_t reserved07;
};

union InGameScheduledConditionStatusAndKind4 {
    InGameScheduledConditionKind kind; 
    InGameScheduledConditionStatusFlags statusFlags;
    uint32_t raw;
    uint8_t kindAndExpression[4]; /* byte 0 kind; a BOOLEAN_POSTFIX_EXPRESSION starts at byte 1 */
};

union InGameScheduledConditionPayload0C {
    uint32_t operands[3];
    uint8_t postfixExpression[12];
};

struct InGameScheduledConditionRecord10 {
    union InGameScheduledConditionStatusAndKind4 statusAndKind; 
    union InGameScheduledConditionPayload0C payload;
};

struct InGameEndConditionTriggerRecord8ReferenceView {
    InGameEndConditionTriggerStateFlags stateFlags;
    uint8_t movieVariantSelector;
    uint8_t skipArmyDisableWhenOne;
    uint8_t reserved03;
    uint8_t factionRuntimeIndex;
    uint8_t endMovieSelectionIndex;
    uint8_t conditionIndex;
    uint8_t reserved07;
};

struct InGameConditionSchedule {
    struct InGameScheduledConditionRecord10 conditions[64];
    struct InGameEndConditionTriggerRecord8ReferenceView triggers[16];
};

struct InGameLevelConditionStorage {
    struct LevelAssetRuntimePrefix levelImage; // Mutable copy of the level runtime prefix loaded from the LEV image.
    uint8_t reserved370_37F[16]; // Never accessed.
    struct InGameConditionSchedule schedule; // 64 x 0x10 scheduled-condition records plus transition descriptors.
};

#endif /* THANDOR_GAMEPLAY_SESSION_TYPES_H */
