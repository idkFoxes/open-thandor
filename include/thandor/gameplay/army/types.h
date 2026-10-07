/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_TYPES_H
#define THANDOR_GAMEPLAY_ARMY_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/army/types.h>
#include <thandor/assets/sprite/types.h>
#include <thandor/audio/spatial/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/faction/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/world/camera/types.h>
#include <thandor/world/terrain/types.h>

struct ArmySegmentMeter;
struct ModelWorldPoint;
struct ArmyRuntimeSlot;
struct GameEntityRuntime;
struct ArmyRuntimeMovementControlState;
struct ArmyRuntimeArticulatedContactState;
struct ArmyRuntimeLinkedChildOverloadedState;
struct ArmyRuntimeLinkedChildSpawnParameters;
struct ArmyRuntimeLinkedChildPendingCounts;
union EffectDefinitionReferenceOrSavedId;
union ShotDefinitionReferenceOrSavedId;
struct GameEntityRuntimeCommon;
union GameEntityRuntimeClassPayload;
union ArmyRuntimeContactRadiusOrLinkedSlotMask;
union ArmyRuntimeCoordinateCommandOrHistoryValue;
struct ModelResource;
struct ModelRuntimeSlot;
struct GameEntityOwnershipState10;
struct GameEntityCommandTargetState;
struct GameEntityDamageState2C;
struct GameEntityPathingAndImpactState10;
struct GameEntityTechnologyPayload;
struct GameEntityImpactOwnerLinksPayloadFC;
union ModelDefinitionReferenceOrSavedId;
union ModelRuntimeNodeReferenceOrSavedOffset4;
union ArmyRuntimeReferenceOrSavedOffset;
union ModelRuntimeSlotReferenceOrSavedOffset;
struct ModelRuntimeClassLinkState;
struct ModelRuntimeSlotClassState;
struct ModelRuntimeAttachmentDescriptor;
union GameEntityDamageCounterOrTerminalReference4;
struct GameEntityPathingReferenceState8;
struct GameEntityImpactReactionBytes8;
union ModelRuntimeSlotLinkOrState;
union ModelRuntimeArmyLinkOrState;
union SpriteAssetReferenceOrSavedId;
struct ArmyRuntimeOrderHandlerMatrix11x24;
struct MdlSerializedNodeHeader;
struct ArmyArticulatedRuntimeSlotView;
struct ArmyRuntimeLinkedChildMaskSlotView;
struct ArmyRuntimeLinkedChildMaskArticulatedContactState;
struct ArmyRuntimeLinkedChildSlotMaskState;
struct ArmyGraphicsBinding;
struct ArmyMovementRuntime;
struct WorldPointXYQ12;
struct RuntimeMaintenanceAudioRefreshCallbacks;
struct RuntimeMaintenancePrimaryUpdateCallbacks;
struct RuntimeMaintenanceTerrainStateRefreshCallbacks;
struct RuntimeMaintenanceCallbackPhasesTyped;
struct RuntimeMaintenanceOccupancyRebuildCallbacks;
struct ArmyPlacementContactCallbackTable5;
struct FixedVectorQ12;
struct PathingDestination;
struct ArmyRuntimeClassUpdate21DefinitionView;
struct ModelRuntimePlacementValidationView;
struct ArmyWeaponDefinitionView;
struct RuntimeCollisionQueryView;
struct ModelRuntimeUpdateView;
struct ModelDefinitionClass14PlacementView;
struct ModelRuntimePlacementClass14View;
struct ModelRuntimeClass14UpdateView;
struct FixedLengthAngle;
struct TerrainPlacementResult;
struct ModelRuntimeLinkedChildSpawnInheritedState;
struct ModelDefinitionVerticalDeploymentView;
struct ModelRuntimeTimedEffectsUpdateView;
struct ModelDefinitionTimedEffectsUpdateView;
struct ModelDefinitionLinkedChildStateView;
struct ModelRuntimeTimedTargetState;
struct ModelDefinitionDestroyEffectsView;
struct ModelRuntimeDestroyEffectsView;
struct ModelRuntimeGroundMovementTrackView;
struct ModelDefinitionGroundMovementTrackView;
struct ModelRuntimeLinkedChildSpawnAndBuildView;
struct ModelRuntimeLinkedChildBuildState;
struct ModelRuntimeLinkedChildPendingSpawnCounts;
struct ModelRuntimeGroundMovementSteeringView;
struct ModelDefinitionGroundMovementSteeringView;
struct ModelRuntimeTimedTargetLinkState;
struct ModelDefinitionTimedTargetProjectileView;
struct ModelDefinitionTimedTargetParameters;
struct ModelRuntimeTimedTargetProjectileView;
struct ModelRuntimeWeaponAimStateView;
struct ModelRuntimeVerticalDeploymentView;
struct ModelRuntimeVerticalDeploymentLinkState;
struct ModelRuntimeClass21State;
struct ModelDefinitionArticulatedMovementView;
struct ModelRuntimeClass21UpdateView;
struct ModelRuntimeArticulatedMovementDefinitionView;
struct EffectDefinition;
struct GraphicsPaletteAsset;
struct GraphicsTextureSet;
struct ModelAttachmentTransformRecord;
struct ModelDefinition;
struct ModelDefinitionRecordPrefix;
struct ModelRuntimeNode;
struct ShotDefinition;
struct ShotRuntimeSlot;
struct WorldOwnerListNode;
struct WorldRuntimeContext;

struct ArmySegmentMeter {
    uint32_t filledSegments; // segments shown as filled
    uint32_t totalSegments; // segments of the whole meter
};

struct ModelWorldPoint {
    uint32_t xQ12;
    uint32_t yQ12;
    uint32_t zQ12;
};
using ArmyPlacementContactKindIndex32 = uint32_t;
using WeaponAimCountdownTicks = int;

using ArmyMovementStateFlags = uint32_t;

using ArmyCommandModeFlags = uint32_t;

using ArmyCommandGeneration = uint32_t;

using ArmyRuntimeFlags = uint32_t;

using ArmyRuntimeTimer = uint32_t;

using ArmySelectionMetric = int;

using AngleTurn16Stored32 = int;

using ModelRuntimeFlags = uint32_t;

using ModelTextureSubresourceIndex = int;

using ModelTextureOffsetTexel = int;

using ArmyTurnVelocityAngle16 = int;

enum {
    ARMY_TERRAIN_CONTACT_ACQUIRE_OR_INITIALIZE_CONTACT_SLOT=0,
    ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE=1
};
using ArmyTerrainContactDispatchMode = int;

enum {
    SHT_0000_SLRAY0=0,
    SHT_0001_SSFBA0=1,
    SHT_0002_SPMGW0=2,
    SHT_0003_SPFLK0=3,
    SHT_0004_SPKAN0=4,
    SHT_0005_SSFBB0=5,
    SHT_0006_SIIOA0=6,
    SHT_0007_SSFBC0=7,
    SHT_0008_SSFBC0=8,
    SHT_0009_SPMGW0=9,
    SHT_0010_SLRAB0=10,
    SHT_0011_SRMIA0=11,
    SHT_0012_SRMIB0=12,
    SHT_0013_SRMIC0=13,
    SHT_0014_SPMGW0=14,
    SHT_0015_SPHAU0=15,
    SHT_0016_SPHAU0=16,
    SHT_0017_SPKAB0=17,
    SHT_0018_SLRAB0=18,
    SHT_0019_SLTRM0=19,
    SHT_0020_SRMID0=20,
    SHT_0021_SRECR0=21,
    SHT_0022_SPMGW0=22,
    SHT_0023_SPMGW0=23,
    SHT_0024_SPMGW0=24,
    SHT_0025_SPMGW0=25,
    SHT_0026_SLRAY0=26,
    SHT_0027_SPFLK0=27,
    SHT_0028_SPMGW0=28,
    SHT_0029_SPMGW0=29,
    SHT_0030_SPMGW0=30,
    SHT_0031_SRMIB0=31,
    SHT_0032_SRMIB0=32,
    SHT_0033_SIIOB0=33,
    SHT_1000_SLRAY0=1000,
    SHT_1001_SSFBA0=1001,
    SHT_1002_SPMGW0=1002,
    SHT_1003_SPFLK0=1003,
    SHT_1004_SPKAN0=1004,
    SHT_1005_SSFBB0=1005,
    SHT_1006_SIIOA0=1006,
    SHT_1007_SSFBC0=1007,
    SHT_1008_SSFBC0=1008,
    SHT_1009_SPMGW0=1009,
    SHT_1010_SLRAB0=1010,
    SHT_1011_SRMIA0=1011,
    SHT_1012_SRMIB0=1012,
    SHT_1013_SRMIC0=1013,
    SHT_1014_SPMGW0=1014,
    SHT_1015_SPHAU0=1015,
    SHT_1016_SPHAU0=1016,
    SHT_1017_SPKAB0=1017,
    SHT_1018_SLRAB0=1018,
    SHT_1019_SLTRM0=1019,
    SHT_1020_SRMID0=1020,
    SHT_1021_SRECR0=1021,
    SHT_1022_SPMGW0=1022,
    SHT_1023_SPMGW0=1023,
    SHT_1024_SPMGW0=1024,
    SHT_1025_SPMGW0=1025,
    SHT_1026_SLRAY0=1026,
    SHT_1027_SPFLK0=1027,
    SHT_1028_SPMGW0=1028,
    SHT_1029_SPMGW0=1029,
    SHT_1030_SPMGW0=1030,
    SHT_1031_SRMIB0=1031,
    SHT_1032_SRMIB0=1032,
    SHT_1033_SIIOB0=1033,
    SHT_2000_SLRAY0=2000,
    SHT_2001_SSFBA0=2001,
    SHT_2002_SPMGW0=2002,
    SHT_2003_SPFLK0=2003,
    SHT_2004_SPKAN0=2004,
    SHT_2005_SSFBB0=2005,
    SHT_2006_SIIOA0=2006,
    SHT_2007_SSFBC0=2007,
    SHT_2008_SSFBC0=2008,
    SHT_2009_SPMGW0=2009,
    SHT_2010_SLRAB0=2010,
    SHT_2011_SRMIA0=2011,
    SHT_2012_SRMIB0=2012,
    SHT_2013_SRMIC0=2013,
    SHT_2014_SPMGW0=2014,
    SHT_2015_SPHAU0=2015,
    SHT_2016_SPHAU0=2016,
    SHT_2017_SPKAB0=2017,
    SHT_2018_SLRAB0=2018,
    SHT_2019_SLTRM0=2019,
    SHT_2020_SRMID0=2020,
    SHT_2021_SRECR0=2021,
    SHT_2022_SPMGW0=2022,
    SHT_2023_SPMGW0=2023,
    SHT_2024_SPMGW0=2024,
    SHT_2025_SPMGW0=2025,
    SHT_2026_SLRAY0=2026,
    SHT_2027_SPFLK0=2027,
    SHT_2028_SPMGW0=2028,
    SHT_2029_SPMGW0=2029,
    SHT_2030_SPMGW0=2030,
    SHT_2031_SRMIB0=2031,
    SHT_2032_SRMIB0=2032,
    SHT_2033_SIIOB0=2033,
    SHT_3000_SLRAY0=3000,
    SHT_3001_SSFBA0=3001,
    SHT_3002_SPMGW0=3002,
    SHT_3003_SPFLK0=3003,
    SHT_3004_SPKAN0=3004,
    SHT_3005_SSFBB0=3005,
    SHT_3006_SIIOA0=3006,
    SHT_3007_SSFBC0=3007,
    SHT_3008_SSFBC0=3008,
    SHT_3009_SPMGW0=3009,
    SHT_3010_SLRAB0=3010,
    SHT_3011_SRMIA0=3011,
    SHT_3012_SRMIB0=3012,
    SHT_3013_SRMIC0=3013,
    SHT_3014_SPMGW0=3014,
    SHT_3015_SPHAU0=3015,
    SHT_3016_SPHAU0=3016,
    SHT_3017_SPKAB0=3017,
    SHT_3018_SLRAB0=3018,
    SHT_3019_SLTRM0=3019,
    SHT_3020_SRMID0=3020,
    SHT_3021_SRECR0=3021,
    SHT_3022_SPMGW0=3022,
    SHT_3023_SPMGW0=3023,
    SHT_3024_SPMGW0=3024,
    SHT_3025_SPMGW0=3025,
    SHT_3026_SLRAY0=3026,
    SHT_3027_SPFLK0=3027,
    SHT_3028_SPMGW0=3028,
    SHT_3029_SPMGW0=3029,
    SHT_3030_SPMGW0=3030,
    SHT_3031_SRMIB0=3031,
    SHT_3032_SRMIB0=3032,
    SHT_3033_SIIOB0=3033
};
using PckShotDefinitionIdCatalog = int;

using GameEntityCommandFlags = uint32_t;

using RuntimeToken = uint32_t;

using GameEntityRuntimeFlags = uint32_t;

using GameEntityCommandState = int;

using OwnedNestedResourceFlag = uint32_t;

using ModelMeshGroupCount = uint32_t;

using ModelPackedLookupTableRelativeOffset = uint32_t;

using ModelPackedLookupTableEntryCount = uint32_t;

using ModelMeshGroupRelativeOffset = uint32_t;

using ModelPackedGeometryRecordCount = uint32_t;

enum {
    MODEL_RESOURCE_DISABLE_PROJECTED_HIT_TEST=4
};
using ModelResourceHitTestFlags = int;

using GameEntityCommandTargetFlags = uint32_t;

using TechnologyResearchDurationQ5 = uint32_t;

using TechnologyEnergyCostQ4 = uint32_t;

using TechnologyXeniteCostQ4 = uint32_t;

enum {
    EFFECT_TRANSITION_SPAWN_LINKED_EFFECT_AFTER_COUNTDOWN=0,
    EFFECT_TRANSITION_ADVANCE_PERIODIC_EMISSION_AND_COMPLETION_ACTION=1,
    EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION=2,
    EFFECT_TRANSITION_NO_ADDITIONAL_ACTION=3,
    EFFECT_TRANSITION_ADVANCE_TERRAIN_RELATIVE_MOTION_AND_TERMINATE_ON_CONTACT=4
};
using EffectLifecycleTransitionKind = int;

using PlayerRuntimeId = uint32_t;

using ModelChildNodeIndex = uint32_t;

enum {
    MDL_0100_UNTERBAU1=100,
    MDL_0101_UNTERBAU1=101,
    MDL_0102_UNTERBAU1=102,
    MDL_0103_UNTERBAU1=103,
    MDL_0111_UNTERBAU1=111,
    MDL_0112_UNTERBAU1=112,
    MDL_0113_UNTERBAU1=113,
    MDL_0120_UNTERBAU1=120,
    MDL_0121_UNTERBAU1=121,
    MDL_0122_UNTERBAU1=122,
    MDL_0130_UNTERBAU1=130,
    MDL_0131_UNTERBAU1=131,
    MDL_0132_UNTERBAU1=132,
    MDL_0133_UNTERBAU1=133,
    MDL_0141_UNTERBAU1=141,
    MDL_0142_UNTERBAU1=142,
    MDL_0143_UNTERBAU1=143,
    MDL_0150_UNTERBAU1=150,
    MDL_0151_UNTERBAU1=151,
    MDL_0152_UNTERBAU1=152,
    MDL_0153_UNTERBAU1=153,
    MDL_0200_AUFBAU1=200,
    MDL_0201_AUFBAU1=201,
    MDL_0202_AUFBAU1=202,
    MDL_0210_AUFBAU1=210,
    MDL_0211_AUFBAU1=211,
    MDL_0212_AUFBAU1=212,
    MDL_0213_AUFBAU1=213,
    MDL_0214_AUFBAU1=214,
    MDL_0215_AUFBAU1=215,
    MDL_0216_AUFBAU1=216,
    MDL_0217_AUFBAU1=217,
    MDL_0218_AUFBAU1=218,
    MDL_0219_AUFBAU1=219,
    MDL_0220_AUFBAU1=220,
    MDL_0221_AUFBAU1=221,
    MDL_0222_AUFBAU1=222,
    MDL_0223_AUFBAU1=223,
    MDL_0224_AUFBAU1=224,
    MDL_0225_AUFBAU1=225,
    MDL_0226_AUFBAU1=226,
    MDL_0227_AUFBAU1=227,
    MDL_0228_AUFBAU1=228,
    MDL_0229_AUFBAU1=229,
    MDL_0230_AUFBAU1=230,
    MDL_0231_AUFBAU1=231,
    MDL_0232_AUFBAU1=232,
    MDL_0233_AUFBAU1=233,
    MDL_0300_BUILDING1=300,
    MDL_0301_BUILDING1=301,
    MDL_0302_BUILDING1=302,
    MDL_0303_BUILDING1=303,
    MDL_0305_BUILDING1=305,
    MDL_0306_BUILDING1=306,
    MDL_0307_BUILDING1=307,
    MDL_0308_BUILDING1=308,
    MDL_0309_BUILDING1=309,
    MDL_0310_BUILDING1=310,
    MDL_0311_BUILDING1=311,
    MDL_0312_BUILDING1=312,
    MDL_0314_BUILDING1=314,
    MDL_0315_BUILDING1=315,
    MDL_0316_BUILDING1=316,
    MDL_0317_BUILDING1=317,
    MDL_0318_BUILDING1=318,
    MDL_0319_BUILDING1=319,
    MDL_0320_BUILDING1=320,
    MDL_0321_BUILDING1=321,
    MDL_0322_BUILDING1=322,
    MDL_0323_BUILDING1=323,
    MDL_0324_BUILDING1=324,
    MDL_0325_BUILDING1=325,
    MDL_0326_BUILDING1=326,
    MDL_0327_BUILDING1=327,
    MDL_0328_BUILDING1=328,
    MDL_0329_BUILDING1=329,
    MDL_0400_RUINEN=400,
    MDL_0401_RUINEN=401,
    MDL_0402_RUINEN=402,
    MDL_0403_RUINEN=403,
    MDL_0404_RUINEN=404,
    MDL_0405_RUINEN=405,
    MDL_0406_RUINEN=406,
    MDL_0407_RUINEN=407,
    MDL_0408_RUINEN=408,
    MDL_0409_RUINEN=409,
    MDL_0410_RUINEN=410,
    MDL_0411_RUINEN=411,
    MDL_0412_RUINEN=412,
    MDL_0413_RUINEN=413,
    MDL_0414_RUINEN=414,
    MDL_0415_RUINEN=415,
    MDL_0416_RUINEN=416,
    MDL_0417_RUINEN=417,
    MDL_0418_RUINEN=418,
    MDL_0419_RUINEN=419,
    MDL_0420_RUINEN=420,
    MDL_0421_RUINEN=421,
    MDL_0422_RUINEN=422,
    MDL_0423_RUINEN=423,
    MDL_0424_RUINEN=424,
    MDL_0425_RUINEN=425,
    MDL_0426_RUINEN=426,
    MDL_0427_RUINEN=427,
    MDL_0428_RUINEN=428,
    MDL_0429_RUINEN=429,
    MDL_0430_RUINEN=430,
    MDL_0431_RUINEN=431,
    MDL_0432_RUINEN=432,
    MDL_0433_RUINEN=433,
    MDL_0434_RUINEN=434,
    MDL_0435_RUINEN=435,
    MDL_0436_RUINEN=436,
    MDL_0437_RUINEN=437,
    MDL_0438_RUINEN=438,
    MDL_0439_RUINEN=439,
    MDL_0440_RUINEN=440,
    MDL_0500_LBAUM=500,
    MDL_0501_LBAUM=501,
    MDL_0502_LBAUM=502,
    MDL_0503_LBAUM=503,
    MDL_0504_LBAUM=504,
    MDL_0505_LBAUM=505,
    MDL_0506_LBAUM=506,
    MDL_0507_LBAUM=507,
    MDL_0508_LBAUM=508,
    MDL_0540_NBAUM=540,
    MDL_0541_NBAUM=541,
    MDL_0542_NBAUM=542,
    MDL_0543_NBAUM=543,
    MDL_0544_NBAUM=544,
    MDL_0545_NBAUM=545,
    MDL_0546_NBAUM=546,
    MDL_0547_NBAUM=547,
    MDL_0548_NBAUM=548,
    MDL_0549_NBAUM=549,
    MDL_0550_NBAUM=550,
    MDL_0551_NBAUM=551,
    MDL_0552_NBAUM=552,
    MDL_0553_NBAUM=553,
    MDL_0554_NBAUM=554,
    MDL_0555_NBAUM=555,
    MDL_0556_NBAUM=556,
    MDL_0557_NBAUM=557,
    MDL_0558_NBAUM=558,
    MDL_0559_NBAUM=559,
    MDL_0560_NBAUM=560,
    MDL_0561_NBAUM=561,
    MDL_0562_NBAUM=562,
    MDL_0563_NBAUM=563,
    MDL_0580_PALMEN=580,
    MDL_0581_PALMEN=581,
    MDL_0582_PALMEN=582,
    MDL_0583_PALMEN=583,
    MDL_0584_PALMEN=584,
    MDL_0585_PALMEN=585,
    MDL_0586_PALMEN=586,
    MDL_0587_PALMEN=587,
    MDL_0588_PALMEN=588,
    MDL_0620_STEIN=620,
    MDL_0621_STEIN=621,
    MDL_0622_STEIN=622,
    MDL_0623_STEIN=623,
    MDL_0624_STEIN=624,
    MDL_0625_STEIN=625,
    MDL_0626_STEIN=626,
    MDL_0627_STEIN=627,
    MDL_0628_STEIN=628,
    MDL_0629_STEIN=629,
    MDL_0630_STEIN=630,
    MDL_0631_STEIN=631,
    MDL_0632_STEIN=632,
    MDL_0633_STEIN=633,
    MDL_0634_STEIN=634,
    MDL_0635_STEIN=635,
    MDL_0636_STEIN=636,
    MDL_0637_STEIN=637,
    MDL_0638_STEIN=638,
    MDL_0639_STEIN=639,
    MDL_0640_STEIN=640,
    MDL_0641_STEIN=641,
    MDL_0642_STEIN=642,
    MDL_0647_STEIN=647,
    MDL_0648_STEIN=648,
    MDL_0649_STEIN=649,
    MDL_0650_STEIN=650,
    MDL_0651_STEIN=651,
    MDL_0652_STEIN=652,
    MDL_0653_STEIN=653,
    MDL_0654_STEIN=654,
    MDL_0655_STEIN=655,
    MDL_0656_STEIN=656,
    MDL_0657_STEIN=657,
    MDL_0658_STEIN=658,
    MDL_0660_BUSCH=660,
    MDL_0661_BUSCH=661,
    MDL_0662_BUSCH=662,
    MDL_0663_BUSCH=663,
    MDL_0664_BUSCH=664,
    MDL_0665_BUSCH=665,
    MDL_0710_FARNE=710,
    MDL_0711_FARNE=711,
    MDL_0712_FARNE=712,
    MDL_0713_FARNE=713,
    MDL_0750_KAKTUS=750,
    MDL_0751_KAKTUS=751,
    MDL_0752_KAKTUS=752,
    MDL_0800_ROHSTOFF=800,
    MDL_0801_ROHSTOFF=801,
    MDL_0802_ROHSTOFF=802,
    MDL_0803_ROHSTOFF=803,
    MDL_0804_ROHSTOFF=804,
    MDL_0805_ROHSTOFF=805,
    MDL_0806_ROHSTOFF=806,
    MDL_0807_ROHSTOFF=807,
    MDL_0808_ROHSTOFF=808,
    MDL_0809_ROHSTOFF=809,
    MDL_0810_ROHSTOFF=810,
    MDL_0811_ROHSTOFF=811,
    MDL_0812_ROHSTOFF=812,
    MDL_0850_ROHSTOFF=850,
    MDL_0851_ROHSTOFF=851,
    MDL_0852_ROHSTOFF=852,
    MDL_0853_ROHSTOFF=853,
    MDL_0854_ROHSTOFF=854,
    MDL_0855_ROHSTOFF=855,
    MDL_1100_UNTERBAU2=1100,
    MDL_1101_UNTERBAU2=1101,
    MDL_1102_UNTERBAU2=1102,
    MDL_1103_UNTERBAU2=1103,
    MDL_1111_UNTERBAU2=1111,
    MDL_1112_UNTERBAU2=1112,
    MDL_1113_UNTERBAU2=1113,
    MDL_1120_UNTERBAU2=1120,
    MDL_1121_UNTERBAU2=1121,
    MDL_1122_UNTERBAU2=1122,
    MDL_1130_UNTERBAU2=1130,
    MDL_1131_UNTERBAU2=1131,
    MDL_1132_UNTERBAU2=1132,
    MDL_1133_UNTERBAU2=1133,
    MDL_1141_UNTERBAU2=1141,
    MDL_1142_UNTERBAU2=1142,
    MDL_1143_UNTERBAU2=1143,
    MDL_1150_UNTERBAU2=1150,
    MDL_1151_UNTERBAU2=1151,
    MDL_1152_UNTERBAU2=1152,
    MDL_1153_UNTERBAU2=1153,
    MDL_1200_AUFBAU2=1200,
    MDL_1201_AUFBAU2=1201,
    MDL_1202_AUFBAU2=1202,
    MDL_1210_AUFBAU2=1210,
    MDL_1211_AUFBAU2=1211,
    MDL_1212_AUFBAU2=1212,
    MDL_1213_AUFBAU2=1213,
    MDL_1214_AUFBAU2=1214,
    MDL_1215_AUFBAU2=1215,
    MDL_1216_AUFBAU2=1216,
    MDL_1217_AUFBAU2=1217,
    MDL_1218_AUFBAU2=1218,
    MDL_1219_AUFBAU2=1219,
    MDL_1220_AUFBAU2=1220,
    MDL_1221_AUFBAU2=1221,
    MDL_1222_AUFBAU2=1222,
    MDL_1223_AUFBAU2=1223,
    MDL_1224_AUFBAU2=1224,
    MDL_1225_AUFBAU2=1225,
    MDL_1226_AUFBAU2=1226,
    MDL_1227_AUFBAU2=1227,
    MDL_1228_AUFBAU2=1228,
    MDL_1229_AUFBAU2=1229,
    MDL_1230_AUFBAU2=1230,
    MDL_1231_AUFBAU2=1231,
    MDL_1232_AUFBAU2=1232,
    MDL_1233_AUFBAU2=1233,
    MDL_1300_BUILDING2=1300,
    MDL_1301_BUILDING2=1301,
    MDL_1302_BUILDING2=1302,
    MDL_1303_BUILDING2=1303,
    MDL_1305_BUILDING2=1305,
    MDL_1306_BUILDING2=1306,
    MDL_1307_BUILDING2=1307,
    MDL_1308_BUILDING2=1308,
    MDL_1309_BUILDING2=1309,
    MDL_1310_BUILDING2=1310,
    MDL_1311_BUILDING2=1311,
    MDL_1312_BUILDING2=1312,
    MDL_1314_BUILDING2=1314,
    MDL_1315_BUILDING2=1315,
    MDL_1316_BUILDING2=1316,
    MDL_1317_BUILDING2=1317,
    MDL_1318_BUILDING2=1318,
    MDL_1319_BUILDING2=1319,
    MDL_1320_BUILDING2=1320,
    MDL_1321_BUILDING2=1321,
    MDL_1322_BUILDING2=1322,
    MDL_1323_BUILDING2=1323,
    MDL_1324_BUILDING2=1324,
    MDL_1325_BUILDING2=1325,
    MDL_1326_BUILDING2=1326,
    MDL_1327_BUILDING2=1327,
    MDL_1328_BUILDING2=1328,
    MDL_1329_BUILDING2=1329,
    MDL_2100_UNTERBAU3=2100,
    MDL_2101_UNTERBAU3=2101,
    MDL_2102_UNTERBAU3=2102,
    MDL_2103_UNTERBAU3=2103,
    MDL_2111_UNTERBAU3=2111,
    MDL_2112_UNTERBAU3=2112,
    MDL_2113_UNTERBAU3=2113,
    MDL_2120_UNTERBAU3=2120,
    MDL_2121_UNTERBAU3=2121,
    MDL_2122_UNTERBAU3=2122,
    MDL_2130_UNTERBAU3=2130,
    MDL_2131_UNTERBAU3=2131,
    MDL_2132_UNTERBAU3=2132,
    MDL_2133_UNTERBAU3=2133,
    MDL_2141_UNTERBAU3=2141,
    MDL_2142_UNTERBAU3=2142,
    MDL_2143_UNTERBAU3=2143,
    MDL_2150_UNTERBAU3=2150,
    MDL_2151_UNTERBAU3=2151,
    MDL_2152_UNTERBAU3=2152,
    MDL_2153_UNTERBAU3=2153,
    MDL_2200_AUFBAU3=2200,
    MDL_2201_AUFBAU3=2201,
    MDL_2202_AUFBAU3=2202,
    MDL_2210_AUFBAU3=2210,
    MDL_2211_AUFBAU3=2211,
    MDL_2212_AUFBAU3=2212,
    MDL_2213_AUFBAU3=2213,
    MDL_2214_AUFBAU3=2214,
    MDL_2215_AUFBAU3=2215,
    MDL_2216_AUFBAU3=2216,
    MDL_2217_AUFBAU3=2217,
    MDL_2218_AUFBAU3=2218,
    MDL_2219_AUFBAU3=2219,
    MDL_2220_AUFBAU3=2220,
    MDL_2221_AUFBAU3=2221,
    MDL_2222_AUFBAU3=2222,
    MDL_2223_AUFBAU3=2223,
    MDL_2224_AUFBAU3=2224,
    MDL_2225_AUFBAU3=2225,
    MDL_2226_AUFBAU3=2226,
    MDL_2227_AUFBAU3=2227,
    MDL_2228_AUFBAU3=2228,
    MDL_2229_AUFBAU3=2229,
    MDL_2230_AUFBAU3=2230,
    MDL_2231_AUFBAU3=2231,
    MDL_2232_AUFBAU3=2232,
    MDL_2233_AUFBAU3=2233,
    MDL_2300_BUILDING3=2300,
    MDL_2301_BUILDING3=2301,
    MDL_2302_BUILDING3=2302,
    MDL_2303_BUILDING3=2303,
    MDL_2305_BUILDING3=2305,
    MDL_2306_BUILDING3=2306,
    MDL_2307_BUILDING3=2307,
    MDL_2308_BUILDING3=2308,
    MDL_2309_BUILDING3=2309,
    MDL_2310_BUILDING3=2310,
    MDL_2311_BUILDING3=2311,
    MDL_2312_BUILDING3=2312,
    MDL_2314_BUILDING3=2314,
    MDL_2315_BUILDING3=2315,
    MDL_2316_BUILDING3=2316,
    MDL_2317_BUILDING3=2317,
    MDL_2318_BUILDING3=2318,
    MDL_2319_BUILDING3=2319,
    MDL_2320_BUILDING3=2320,
    MDL_2321_BUILDING3=2321,
    MDL_2322_BUILDING3=2322,
    MDL_2323_BUILDING3=2323,
    MDL_2324_BUILDING3=2324,
    MDL_2325_BUILDING3=2325,
    MDL_2326_BUILDING3=2326,
    MDL_2327_BUILDING3=2327,
    MDL_2328_BUILDING3=2328,
    MDL_2329_BUILDING3=2329
};
using PckModelDefinitionIdCatalog = int;

struct ModelResource {
    uint8_t reserved00_AF[176]; 
    ModelMeshGroupCount meshGroupCount; 
    uint8_t reservedB4_BF[12]; 
    Q12 localBoundsX0Q12; 
    Q12 localBoundsX1Q12; 
    Q12 localBoundsY0Q12; 
    Q12 localBoundsY1Q12; 
    Q12 localBoundsZ0Q12; 
    Q12 localBoundsZ1Q12; 
    Q12 boundingRadiusQ12; 
    Q12 placementHeightOffsetQ12; 
    Q12 lightingScaleQ12; 
    ModelPackedLookupTableRelativeOffset packedLookupTableRelativeOffset; 
    ModelPackedLookupTableEntryCount packedLookupTableEntryCount;
    union {
        uint8_t reservedEC_1FF[276];
        struct {
            int shadowMeshGroupOffset; /* resource-relative offset of the mesh group the shadow pass draws, 0 = none */
            uint8_t reservedF0_1FF[272];
        };
    };
    ModelMeshGroupRelativeOffset firstMeshGroupRelativeOffset;
    ModelPackedGeometryRecordCount packedGeometryRecordCount; 
    uint8_t reserved208_20B[4]; 
    ModelResourceHitTestFlags hitTestFlags20C; 

    /* The packed point table: packedLookupTableEntryCount records at the resource-relative byte offset
       packedLookupTableRelativeOffset (step 13 X5; replaces the byte-offset casts to ModelPackedPointRecord at the
       users, same address arithmetic). */
    ModelPackedPointRecord *packedPointRecords()
    {
        return reinterpret_cast<ModelPackedPointRecord *>(reinterpret_cast<uint8_t *>(this) +
                                                          packedLookupTableRelativeOffset);
    }
};

union GameEntityDamageCounterOrTerminalReference4 {
    int countdownOrState; 
    Ptr32<struct GameEntityRuntime> terminalEntity; 
    uint32_t raw; 
};

struct GameEntityTechnologyPayload {
    TechnologyResearchDurationQ5 appliedResearchDurationQ5; 
    uint32_t entityValue24; 
    TechnologyEnergyCostQ4 energyCostQ4; // [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Energy requirement Q4 for active research. Starting research adds this demand to the research ArmyRuntime load; completion removes it. It is not Tritium stock.
    TechnologyXeniteCostQ4 xeniteCostQ4; // [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Xenite requirement Q4. Starting research pays this once from faction Xenite stock before research enters its active Energy-demand phase.
    uint8_t reserved14_FB[236]; 
};

struct ArmyRuntimeLinkedChildSpawnParameters {
    uint32_t parameter0; 
    uint32_t parameter1; 
    uint32_t parameter2; 
};

struct GameEntityOwnershipState10 {
    Ptr32<void> definitionOrClassRecord; 
    Ptr32<struct ModelRuntimeNode> modelNode; 
    Ptr32<void> runtimeLink;
    FactionRuntimeIndex ownerIndex;

    /* Typed views of the two untyped references (step 13 X3; they replace the C-style T * casts of ownership.<field> and
       compile to the same load). GameEntityRuntime is a view laid over more than one record, so what +0x0 and
       +0x8 point to depends on the record under it; the caller picks the view, as it did with the cast:
       - modelRuntime(): +0x0 as a ModelRuntimeSlot (the army entities of selection, commands, weapons, AI:
         ArmyRuntimeSlot.modelRuntimeOrSavedOffset);
       - modelDefinition(): +0x0 as a ModelDefinition (pathing route and grid influence handlers);
       - linkedArmyRuntime() / linkedModelRuntime(): +0x8 as an ArmyRuntimeSlot (route, damage, AI) or as the
         ModelRuntimeSlot the grid influence handlers keep their stored point in.
       Not constexpr: a Ptr32 field holds an address as an integer. */
    struct ModelRuntimeSlot *modelRuntime() const
    {
        return static_cast<struct ModelRuntimeSlot *>(definitionOrClassRecord.get());
    }
    struct ModelDefinition *modelDefinition() const
    {
        return static_cast<struct ModelDefinition *>(definitionOrClassRecord.get());
    }
    struct ArmyRuntimeSlot *linkedArmyRuntime() const
    {
        return static_cast<struct ArmyRuntimeSlot *>(runtimeLink.get());
    }
    struct ModelRuntimeSlot *linkedModelRuntime() const
    {
        return static_cast<struct ModelRuntimeSlot *>(runtimeLink.get());
    }
};

union ModelRuntimeArmyLinkOrState {
    Ptr32<struct ArmyRuntimeSlot> armyRuntime; 
    uint32_t classState; 
    uint32_t serializedOffset; 
};

union ArmyRuntimeCoordinateCommandOrHistoryValue {
    int signedValue; 
    Q12 coordinateOrTargetQ12; 
    AngleTurn32 headingOrTurnValue; 
    uint32_t raw; 
};

struct GameEntityImpactOwnerLinksPayloadFC {
    uint8_t reserved00_3B[60];
    /* +0x140 / +0x160 of the model runtime this GameEntityRuntime view is laid over: the child model runtimes of
       its attachment descriptors 0 and 1 (ModelRuntimeSlot.attachments[i].childModelRuntimeOrSavedOffset) */
    Ptr32<struct ModelRuntimeSlot> attachment0ChildModelRuntime;
    uint8_t reserved40_5B[28];
    Ptr32<struct ModelRuntimeSlot> attachment1ChildModelRuntime;
    uint8_t reserved60_FB[156]; 
};

union GameEntityRuntimeClassPayload {
    uint8_t opaque[252]; 
    struct GameEntityTechnologyPayload technology; 
    struct GameEntityImpactOwnerLinksPayloadFC impactOwnerLinks; 
};

struct GameEntityImpactReactionBytes8 {
    uint8_t state08; 
    uint8_t reactionCode09; 
    uint8_t state0A; 
    uint8_t state0B; 
    uint8_t reserved0C_0F[4]; 
};

struct GameEntityPathingReferenceState8 {
    Ptr32<struct GameEntityRuntime> overlappingEntity; 
    Ptr32<void> secondaryPathingReference; 
};

struct GameEntityPathingAndImpactState10 {
    struct GameEntityPathingReferenceState8 pathingReferences; 
    struct GameEntityImpactReactionBytes8 impactReaction; 
};

struct GameEntityDamageState2C {
    uint32_t reserved00; // Unresolved damage-state prefix.
    union GameEntityDamageCounterOrTerminalReference4 counterOrTerminalReference; // Movement countdown/state or terminal entity reference selected by the active path.
    int remainingIntegrity; // Impact-subtracted integrity clamped against definition maximum.
    uint8_t reserved0C_1B[16]; // Unresolved damage/relation bytes.
    uint32_t factionVisibilityBits1C; /* at entity +0x50: two bits per faction, bit 1 = visible to that faction */
    uint8_t reserved20_23[4];
    Q12 trackedCoordinate0Q12; // First verified tracked coordinate mirrored from pathCoordinate0Q12/model transform; neutral axis name retained because callers disagree on X/Y naming.
    Q12 trackedCoordinate1Q12; // Second verified tracked coordinate mirrored from pathCoordinate1Q12/model transform; neutral axis name retained because callers disagree on X/Y naming.
};

struct GameEntityCommandTargetState {
    Ptr32<struct GameEntityRuntime> targetEntity; 
    Q12 targetWorldXQ12; 
    Q12 targetWorldYQ12; 
    Q12 targetWorldZQ12; 
    GameEntityCommandTargetFlags targetFlags; 
    ArmyCommandGeneration commandGeneration; 
};

struct GameEntityRuntimeCommon {
    struct GameEntityOwnershipState10 ownership; // Definition, model, runtime-link, and owner prefix.
    uint8_t reserved10_17[8]; // Unresolved common state.
    GameEntityCommandFlags commandFlags; // Selection-command and transient flags.
    struct GameEntityCommandTargetState commandTarget; // Typed entity command-target state used by target resolution, routing, shot aiming, and selection commands.
    struct GameEntityDamageState2C damageState; // Impact-mutated integrity and terminal entity state.
    Q12 selectionOffsetXQ12; // Selection-relative X offset.
    Q12 selectionOffsetYQ12; // Selection-relative Y offset.
    uint8_t reserved68_77[16]; // Unresolved common entity state before the tracked-coordinate pair.
    Q12 trackedCoordinate0Q12; // First verified tracked coordinate mirrored with path/model coordinate 0.
    Q12 trackedCoordinate1Q12; // Second verified tracked coordinate mirrored with path/model coordinate 1.
    uint8_t reserved80_8B[12]; // Same bytes as ArmyRuntimeSlot.aiSiteScoreWeight .. aiSecondaryWorkspaceScoreWeight.
    int aiCommandCooldownTicks; // same dword as ArmyRuntimeSlot.aiUnitState: set to AI_UNIT_COMMANDED_STATE (8) on an AI order, counted down per AI tick of busy units.
    uint8_t reserved90_9F[16]; // Same bytes as ArmyRuntimeSlot.occupancyMarkRadius .. depthBinClass.
    RuntimeToken runtimeIdentityOrArmyAssetId; // Runtime identity or army asset identifier.
    uint8_t reservedA4_B7[20]; // Unresolved prefix retained.
    Q12 pathCoordinate0Q12; // First stored path/aim coordinate, consumed by AI distance, shot aim and overlay routines; axis naming differs between callers.
    Q12 pathCoordinate1Q12; // Second stored path/aim coordinate; paired with +0xB8.
    uint8_t reservedC0_EB[44]; // Remaining path state not subdivided.
    GameEntityRuntimeFlags runtimeFlags; // Runtime eligibility and lifecycle flags.
    struct GameEntityPathingAndImpactState10 pathingAndImpactState; // Pathing-reference prefix plus impact-reaction tail.
    GameEntityCommandState commandState; // Signed command/state value.
};

struct GameEntityRuntime {
    struct GameEntityRuntimeCommon common; 
    union GameEntityRuntimeClassPayload classPayload; 
};

union ArmyRuntimeReferenceOrSavedOffset {
    Ptr32<struct ArmyRuntimeSlot> armyRuntime;
    uint32_t savedIdOrOffset;
    uint32_t raw;
    Ptr32<struct GameEntityRuntime> entityRuntime; /* the same army under its GameEntityRuntime view (command target, owner) */
    /* ModelRuntimeSlotClassState.linkedArmyRuntimeOrSavedOffset (+0xF0) holds a model runtime, not an army: the
       factory model a new unit leaves (ArmyRuntimeClass_UpdateUnitFactory), the class-23 platform and the unit
       docked on it (ArmyRuntime_HandleCollisionPartner) */
    Ptr32<struct ModelRuntimeSlot> modelRuntime;
};

struct ArmyRuntimeMovementControlState {
    Q12 movementAdvancePerTickQ12; 
    ArmyTurnVelocityAngle16 turnVelocityAngle16; 
};

union ArmyRuntimeContactRadiusOrLinkedSlotMask {
    UQ12 contactRadiusQ12; 
    uint32_t linkedChildSlotMask; 
    uint32_t raw; 
};

struct ArmyRuntimeArticulatedContactState {
    ArmyTerrainContactDispatchMode terrainContactMode; 
    Q12 lateralOffsetQ12; 
    union ArmyRuntimeContactRadiusOrLinkedSlotMask contactRadiusOrLinkedSlotMask; 
    Q12 fallbackPosition0Q12; 
    Q12 fallbackPosition1Q12; 
};

struct ArmyRuntimeLinkedChildPendingCounts {
    uint8_t slot0; 
    uint8_t slot1; 
    uint8_t slot2; 
    uint8_t reserved03; 
};

struct ArmyRuntimeLinkedChildOverloadedState {
    union ArmyRuntimeCoordinateCommandOrHistoryValue primaryCoordinateCommandOrHistory; 
    union ArmyRuntimeCoordinateCommandOrHistoryValue secondaryCoordinateCommandOrHistory; 
    union ArmyRuntimeCoordinateCommandOrHistoryValue leftHeadingCommandOrSpawnValue; 
    union ArmyRuntimeCoordinateCommandOrHistoryValue rightHeadingCommandOrSpawnValue; 
};

union ShotDefinitionReferenceOrSavedId {
    Ptr32<struct ShotDefinition> definition; 
    PckShotDefinitionIdCatalog savedId; 
    uint32_t raw; 
};

union ModelRuntimeSlotLinkOrState {
    Ptr32<struct ModelRuntimeSlot> modelRuntime; // Class-selected model-runtime link.
    uint32_t classState; // Class-selected scalar state.
    uint32_t serializedOffset; // Saved model-pool offset.
    int signedScalarState; // Signed 32-bit scalar interpretation for polymorphic class state when machine code performs arithmetic, signed comparison, or class-index use rather than pointer dereference.
};

struct ModelRuntimeSlotClassState {
    uint8_t reserved84_A7[36]; // Unresolved class-specific state.
    uint32_t classStateA8; // Class-discriminated runtime state.
    uint32_t classStateAC; // Class-discriminated runtime state.
    int32_t classStateB0; // Class-discriminated signed runtime state.
    int32_t classStateB4; // Class-discriminated signed runtime state.
    uint32_t behaviorState; // class state machine or flags: ARMY_FACTORY_STATE_* (factories), ARMY_AIRCRAFT_STATE_* (aircraft), bit 0 = army linked (class 23 deployment), bits 1/2/4 (class 17 banking)
    uint32_t classStateBC; // Class-discriminated; class 13: bit 0 = exit point (+0x78/+0x7C) not stored yet.
    uint32_t classStateC0; // Class-discriminated dword at ModelRuntimeSlot +0xC0; verified selector input in class-14 placement/runtime paths.
    uint8_t reservedC4_C7[4]; // Unresolved class-specific bytes C4-C7.
    uint32_t classStateC8; // Class-discriminated dword at ModelRuntimeSlot +0xC8; verified packed runtime contribution in class-14 update path.
    uint8_t reservedCC_CF[4]; // Unresolved class-specific bytes CC-CF.
    int32_t classStateD0; // Class-discriminated signed runtime state.
    uint8_t reservedD4_DB[8]; // Unresolved class-specific state.
    uint32_t classStateDC; // Class-discriminated runtime state.
    uint32_t effectEmitterPointIndex; // Next model effect point of the timed effect emitter (ArmyRuntime_UpdateTimedShotAndEffectEmitters).
    uint32_t shotEmitterTimerTicks; // Timed shot emitter countdown; constructor sets 1.
    uint32_t effectEmitterTimerTicks; // Timed effect emitter countdown; constructor sets 1, 0x7FFFFFFF = never.
    uint32_t stateFlags; // ARMY_MODEL_STATE_* bits (gameplay/army/pool.h); constructor-cleared.
    union ArmyRuntimeReferenceOrSavedOffset linkedArmyRuntimeOrSavedOffset; // Live army pointer or serialized pool offset.
    uint32_t energyLoadQ4; // Energy demand: the definition's energyLoadQ4 plus loads held while building/researching.
    uint32_t healthRegenerationDelayTicks; // Counts down to the next health step; damage sets it to 0x200.
    uint32_t dismantleTickCountdown; // 12-tick period of the Xenite refund while dismantling.
};

/* ModelRuntimeSlot +0x60..+0x83: class-discriminated words (the class views name them per class). Examples:
   factories (ArmyRuntimeClass_UpdateUnitFactory / _UpdateStructureFactory): +0x64 build elapsed ticks,
   +0x68 build required ticks, +0x74 energy load of the build, unit factory +0x78/+0x7C exit point X/Y;
   aircraft (ArmyRuntimeClass_UpdateAircraft): +0x60 home pad model, +0x64..+0x80 flight timers and headings;
   class 17 banking: +0x60 bank value (0x4000 = level), +0x64 bank heading, +0x68 last waypoint X;
   ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters: +0x64..+0x80 per-key counters. The links at +0x60
   and +0x6C are rebased as model / army pointers for the classes whose rebase callbacks say so. */
struct ModelRuntimeClassLinkState {
    union ModelRuntimeSlotLinkOrState modelLinkOrState;
    uint32_t classState64;
    uint32_t classState68; 
    union ModelRuntimeArmyLinkOrState armyLinkOrState; 
    uint32_t classState70; 
    uint32_t classState74; 
    uint32_t classState78; 
    uint32_t classState7C; 
    uint32_t classState80; 
};

union ModelRuntimeNodeReferenceOrSavedOffset4 {
    Ptr32<struct ModelRuntimeNode> modelNode; 
    uint32_t savedIdOrOffset; 
    uint32_t raw; 
};

struct ModelRuntimeAttachmentDescriptor {
    Ptr32<struct ModelRuntimeSlot> childModelRuntimeOrSavedOffset; 
    Ptr32<struct ModelAttachmentTransformRecord> sourceTransform; 
    Ptr32<struct ModelRuntimeNode> parentModelNodeOrSavedOffset; 
    ModelChildNodeIndex childNodeIndex; 
    AngleTurn32 childLocalRotationAngle0; 
    AngleTurn32 childLocalRotationAngle1; 
    AngleTurn32 childLocalRotationAngle2; 
    uint32_t reserved1C; 
};

union ModelDefinitionReferenceOrSavedId {
    Ptr32<struct ModelDefinitionRecordPrefix> definition;
    uint32_t savedIdOrOffset;
    uint32_t raw;
    Ptr32<struct ModelDefinition> runtimeDefinition; /* same live pointer, full definition field view */
};

union ModelRuntimeSlotReferenceOrSavedOffset {
    Ptr32<struct ModelRuntimeSlot> modelRuntime;
    uint32_t savedIdOrOffset;
    uint32_t raw;
};

struct ArmyRuntimeSlot {
    union ModelRuntimeSlotReferenceOrSavedOffset modelRuntimeOrSavedOffset; // Live ModelRuntimeSlot reference; serialized save image stores the model-pool-relative offset. Verified by the create/save/rebase paths.
    Ptr32<struct ModelRuntimeNode> modelNodeRuntime;
    Ptr32<struct GameEntityRuntime> linkedEntityRuntime; // Linked GameEntityRuntime state copied from the army asset record and dereferenced by movement, command, placement, and class callbacks.
    FactionRuntimeIndex factionIndex;
    struct ArmyRuntimeMovementControlState movementControl; // Typed per-tick movement advance and signed turn-velocity state used by runtime-update and projected-sound callbacks.
    ArmyMovementStateFlags movementStateFlags;
    Ptr32<struct ArmyRuntimeSlot> commandTargetArmyRuntime;
    Q12 commandCoordinate0Q12;
    Q12 commandCoordinate1Q12;
    Q12 commandCoordinate2Q12;
    ArmyCommandModeFlags commandModeFlags;
    ArmyCommandGeneration commandGeneration;
    Q12 actionVector0Q12;
    Q12 actionVector1Q12;
    Q12 actionVector2Q12;
    uint32_t runtimeState40;
    uint32_t visibilityRadius; // largest ModelDefinition.visibilityRadius of the army's models; radius of its terrain occlusion (visibility) mask
    uint32_t visibilityHeightOffset; // largest model height + ModelDefinition.visibilityHeightOffset above the root node (visibility reference height)
    uint32_t weaponRangeQ12; // Largest shot selection range of the army's weapons (ArmyRuntime_RebuildDerivedSelectionMetrics).
    FieldGridRegionMask terrainOccupancyMask0; // First mask supplied to TerrainOccupancyMask_ResolveRuntimeClassFlags.
    FieldGridRegionMask terrainOccupancyMask1; // Second mask supplied to TerrainOccupancyMask_ResolveRuntimeClassFlags.
    Q12 movementPosition0Q12;
    Q12 movementPosition1Q12;
    uint32_t classState60; // no army code reads these (the former users were model runtimes typed as ArmyRuntimeSlot)
    uint32_t classState64;
    uint32_t classState68;
    Ptr32<struct ArmyRuntimeSlot> linkedArmyRuntimeOrSavedOffset;
    Q12 fallbackWorldYQ12;
    Q12 fallbackWorldXQ12;
    Q12 movementTarget0Q12;
    Q12 movementTarget1Q12;
    uint32_t aiSiteScoreWeight; // AI weight of general (workspace 05) sites, copied from the army asset record
    uint32_t aiFactionAnchorScoreWeight; // AI weight of the faction anchor points, copied from the army asset record
    uint32_t aiSecondaryWorkspaceScoreWeight; // AI weight of secondary workspace sites, copied from the army asset record
    uint32_t aiUnitState; // AI command state (AI_UNIT_COMMANDED_STATE when the AI gave an order), 0 on creation
    uint32_t occupancyMarkRadius; // largest ModelDefinition.occupancyMarkRadius; radius of occupancy bit 1 around the army
    uint32_t aiUnitFlags; // AI unit flags (AI_UNIT_STATE94_GROUP_ASSIGNED)
    uint32_t assignedTargetArmyRuntime; // ArmyRuntimeSlot * given as target by the AI or the player's selection; pool offset in saves
    ModelRuntimeClassId depthBinClass;
    PckArmyAssetIdCatalog armyAssetId;
    uint32_t movementRetryCountdown; // ArmyMovementRuntime.retryCountdown; counted down by ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers
    uint32_t runtimeStateA8;
    struct ArmyRuntimeArticulatedContactState articulatedContact; // Terrain-contact and articulated movement state.
    struct ArmyRuntimeLinkedChildOverloadedState linkedChildOverloadedState; // Mixed coordinate, command, heading, spawn, and state-history overlay.
    struct ArmyRuntimeLinkedChildSpawnParameters linkedChildSpawnParameters; // Third linked-child spawn parameter triplet.
    struct ArmyRuntimeLinkedChildPendingCounts linkedChildPendingCounts; // Three independently decremented pending child counters.
    uint8_t reservedE0_EB[12];
    ArmyRuntimeFlags runtimeFlags;
    Ptr32<struct ArmyRuntimeSlot> linkedArmyRuntime;
    ArmyRuntimeTimer runtimeTimer;
    uint8_t reservedF8_FF[8];
    union {
        struct {
            int stateOrTechnologyId;
            uint32_t runtimeState104;
            ArmySelectionMetric selectionMetric0;
            ArmySelectionMetric selectionMetric1;
            ArmySelectionMetric selectionMetric2;
            ArmySelectionMetric selectionMetric3;
            ArmySelectionMetric selectionMetric4;
            ArmySelectionMetric selectionMetric5;
        };
        int targetClassShotDamage[8]; /* per target class (definition targetClassIndex): summed shot damage of the army's weapons */
    };
};

struct ModelRuntimeSlot {
    union ModelDefinitionReferenceOrSavedId definitionOrSavedId;
    union ModelRuntimeNodeReferenceOrSavedOffset4 rootModelNodeOrSavedOffset;
    union ArmyRuntimeReferenceOrSavedOffset ownerArmyRuntimeOrSavedOffset;
    uint32_t attachmentCount;
    union {
        uint8_t classPrefixState[40]; /* class-specific state, cleared by the constructor (weapon aim turn velocities and firing countdowns, ground movement control, class 13 timing at +0x10) */
        struct {
            uint8_t reserved10_2F[32];
            uint32_t effectModelFlags; /* bit 1 (value 2): an effect model drawn with the army graphics of binding 0 */
            uint8_t reserved34_37[4];
        };
        struct { /* moving classes 1, 2, 17, 18, 19 (ground, tracked, banking, water movement;
                    ModelRuntimeGroundMovementSteeringView / ModelRuntimeGroundMovementTrackView) */
            struct ArmyRuntimeMovementControlState movementControl; /* advance per tick (Q12) and signed turn velocity */
        };
        struct { /* turret classes 5..8 (ModelRuntimeWeaponAimStateView) */
            uint8_t turretReserved10_13[4];
            ArmyTurnVelocityAngle16 yawTurnVelocityAngle16; /* signed yaw turn velocity */
            ArmyTurnVelocityAngle16 pitchTurnVelocityAngle16; /* signed pitch turn velocity */
        };
    };
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset;
    uint32_t health; /* current health; starts at ModelDefinition.maximumHealth */
    /* +0x40 destruction effect channels: once health is gone, channel i spawns the definition's effect
       (+0x80 + 8 * i) at the model points with key i << 4 | 3 when its timer (from +0x84 + 8 * i) is 0
       (ArmyRuntime_ProcessReadyAttachmentChannels) */
    uint32_t destructionEffectTimers[8];
    struct ModelRuntimeClassLinkState classLinkState;
    struct ModelRuntimeSlotClassState classState;
    uint32_t researchTechnologyId; /* technology (TechnologyId) being researched */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4; /* Energy held while researching */
    int researchXeniteCostQ4; /* Xenite still to pay before research starts */
    int damageEffectCooldownTicks; /* ticks to the next damage smoke/fire effect (ArmyRuntime_EmitDamageThresholdEffect) */
    int damageEffectPointIndex; /* next damage emitter point of the model; -1 wraps to the first; constructor-cleared */
    uint32_t classState11C;
    uint8_t reserved120_13F[32];
    struct ModelRuntimeAttachmentDescriptor attachments[6];
};

union EffectDefinitionReferenceOrSavedId {
    Ptr32<struct EffectDefinition> definition; 
    PckEffectDefinitionIdCatalog savedId; 
    uint32_t raw; 
};

using AssetRegistryId = uint32_t;

union SpriteAssetReferenceOrSavedId {
    Ptr32<struct SpriteAssetHeader> spriteAsset; // Live relocated sprite pointer.
    AssetRegistryId savedId; // Serialized sprite registry id.
    uint32_t raw; // Raw exact dword view.
    Ptr32<struct ModelResource> modelResource; // typed model/render view of relocated sprite asset pointer
};

using ArmyGraphicsAssetAddress32 = intptr_t; /* address of a faction graphics texture source asset, pointer-sized (5f) */

using SprAttachmentSelectorOrdinal = uint32_t;

using ArmyPlacementDispatchArg2 = uint32_t;

using ArmyPlacementDispatchArg3 = uint32_t;

using ArmyPlacementDispatchArg0 = uint32_t;

using MdlNodeFlags = uint32_t;

using FactionProgressAmountQ4 = uint32_t;

using PlacementFactionIndex = uint32_t;

using PlacedArmyToken = uint32_t;

using ArmyPlacementDispatchArg7 = uint32_t;

using ArmyPlacementClearancePaddingQ12 = Q12;

using ArmyPlacementMode = uint32_t;

using ArmyMoveAuxiliaryValue0 = uint32_t;

using ArmyMoveAuxiliaryValue1 = uint32_t;

using ImpactDamageValue32 = int;

using PlayerStateLookupValue1 = int;

using PlayerStateLookupValue0 = int;

/* cos(angle) and sin(angle), scaled (FixedMath_SinCosScaled). */
struct FixedSinCos {
    int32_t cosValue;
    int32_t sinValue;
};

using FixedVectorStateAddress32 = intptr_t; /* address of a ModelRuntimeNode, pointer-sized (5f) */

using WorldMotionValue74 = uint32_t;

using WorldMotionValue70 = uint32_t;

using ShotTargetModelReference = uint32_t; /* ModelRuntimeSlot * of the shot's target (0 = none), stored in ShotRuntimeSlot +0x14 */

using SoundAssetIndex = uint32_t;

using MdlChildCount = uint32_t;

using FixedVectorStepMultiplier32 = int;

using ArmyPlacementCollisionFilterFlags = uint32_t;

using ModelAttachmentOrdinal = uint32_t;

using ArmyPlacementCandidateCount = uint32_t;

using DamageAmount32 = int;

struct MdlSerializedNodeHeader {
    AssetRecordByteCount nodeByteSize;
    MdlNodeFlags nodeFlags; 
    AngleTurn32 localRotationAngle0;
    AngleTurn32 localRotationAngle1;
    AngleTurn32 localRotationAngle2;
    MdlChildCount childCount;
    SerializedRelativeByteOffset childSerializedOffsets[6]; 
    union SpriteAssetReferenceOrSavedId spriteAssetReference; 
    OwnedNestedResourceFlag ownedNestedResourcePresent;
};

struct ArmyArticulatedRuntimeSlotView {
    Ptr32<void> definitionOrAsset; 
    Ptr32<struct ModelRuntimeNode> modelNodeRuntime; 
    Ptr32<struct GameEntityRuntime> linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    Ptr32<struct ArmyRuntimeSlot> commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    uint32_t runtimeState40; 
    uint32_t runtimeState44; 
    uint32_t runtimeState48; 
    uint32_t runtimeState4C; 
    uint8_t reserved50_57[8]; 
    Q12 movementPosition0Q12; 
    Q12 movementPosition1Q12; 
    uint32_t classState60; 
    uint32_t ownerValue64; 
    uint32_t ownerValue68; 
    Ptr32<struct ArmyRuntimeSlot> linkedArmyRuntimeOrSavedOffset; 
    Q12 fallbackWorldYQ12; 
    Q12 fallbackWorldXQ12; 
    Q12 movementTarget0Q12; 
    Q12 movementTarget1Q12; 
    uint32_t definitionClassValue80; 
    uint32_t definitionClassValue84; 
    uint32_t definitionClassValue88; 
    uint32_t runtimeState8C; 
    uint32_t runtimeState90; 
    uint32_t runtimeState94; 
    uint32_t runtimeState98; 
    int articulatedCoordinateOrState9C; 
    int articulatedHeightOrStateA0; 
    uint32_t runtimeStateA4; 
    uint32_t runtimeStateA8; 
    struct ArmyRuntimeArticulatedContactState articulatedContact; 
    struct ArmyRuntimeLinkedChildOverloadedState linkedChildOverloadedState; 
    struct ArmyRuntimeLinkedChildSpawnParameters linkedChildSpawnParameters; 
    struct ArmyRuntimeLinkedChildPendingCounts linkedChildPendingCounts; 
    uint8_t reservedE0_EB[12]; 
    ArmyRuntimeFlags runtimeFlags; 
    Ptr32<struct ArmyRuntimeSlot> linkedArmyRuntime; 
    ArmyRuntimeTimer runtimeTimer; 
    uint8_t reservedF8_FF[8]; 
    int stateOrTechnologyId; 
    uint32_t runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

struct ArmyRuntimeLinkedChildSlotMaskState {
    uint32_t linkedChildSlotMask; 
};

struct ArmyRuntimeLinkedChildMaskArticulatedContactState {
    ArmyTerrainContactDispatchMode terrainContactMode; 
    Q12 lateralOffsetQ12; 
    struct ArmyRuntimeLinkedChildSlotMaskState linkedChildSlotMaskState; 
    Q12 fallbackPosition0Q12; 
    Q12 fallbackPosition1Q12; 
};

struct ArmyRuntimeLinkedChildMaskSlotView {
    Ptr32<void> definitionOrAsset; 
    Ptr32<struct ModelRuntimeNode> modelNodeRuntime; 
    Ptr32<struct GameEntityRuntime> linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    Ptr32<struct ArmyRuntimeSlot> commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    uint32_t runtimeState40; 
    uint32_t visibilityRadius; 
    uint32_t visibilityHeightOffset; 
    uint32_t weaponRangeQ12; 
    uint8_t reserved50_57[8]; 
    Q12 movementPosition0Q12; 
    Q12 movementPosition1Q12; 
    uint32_t classState60; 
    uint32_t classState64; 
    uint32_t classState68; 
    Ptr32<struct ArmyRuntimeSlot> linkedArmyRuntimeOrSavedOffset; 
    Q12 fallbackWorldYQ12; 
    Q12 fallbackWorldXQ12; 
    Q12 movementTarget0Q12; 
    Q12 movementTarget1Q12; 
    uint32_t aiSiteScoreWeight; 
    uint32_t aiFactionAnchorScoreWeight; 
    uint32_t aiSecondaryWorkspaceScoreWeight; 
    uint32_t aiUnitState; 
    uint32_t occupancyMarkRadius; 
    uint32_t aiUnitFlags; 
    uint32_t assignedTargetArmyRuntime; 
    ModelRuntimeClassId depthBinClass; 
    PckArmyAssetIdCatalog armyAssetId; 
    uint32_t movementRetryCountdown; 
    uint32_t runtimeStateA8; 
    struct ArmyRuntimeLinkedChildMaskArticulatedContactState articulatedContact; 
    struct ArmyRuntimeLinkedChildOverloadedState linkedChildOverloadedState; 
    struct ArmyRuntimeLinkedChildSpawnParameters linkedChildSpawnParameters; 
    struct ArmyRuntimeLinkedChildPendingCounts linkedChildPendingCounts; 
    uint8_t reservedE0_EB[12]; 
    ArmyRuntimeFlags runtimeFlags; 
    Ptr32<struct ArmyRuntimeSlot> linkedArmyRuntime; 
    ArmyRuntimeTimer runtimeTimer; 
    uint8_t reservedF8_FF[8]; 
    int stateOrTechnologyId; 
    uint32_t runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

struct ArmyGraphicsBinding {
    Ptr32<struct GraphicsTextureSet> textureSet;
    Ptr32<struct GraphicsPaletteAsset> paletteAsset; 
};

using ArmyWaypointCount = uint32_t;

using ArmyMovementRetryCountdown = uint32_t;

struct WorldPointXYQ12 {
    Q12 worldXQ12; 
    Q12 worldYQ12; 
};

struct ArmyMovementRuntime {
    Ptr32<struct GameEntityRuntime> entityRuntime; 
    Ptr32<struct ModelRuntimeNode> modelNodeRuntime; 
    Ptr32<struct GameEntityRuntime> linkedEntityRuntime; 
    FactionRuntimeIndex factionIndex; 
    struct ArmyRuntimeMovementControlState movementControl; 
    ArmyMovementStateFlags movementStateFlags; 
    Ptr32<struct ArmyRuntimeSlot> commandTargetArmyRuntime; 
    Q12 commandCoordinate0Q12; 
    Q12 commandCoordinate1Q12; 
    Q12 commandCoordinate2Q12; 
    ArmyCommandModeFlags commandModeFlags; 
    ArmyCommandGeneration commandGeneration; 
    Q12 actionVector0Q12; 
    Q12 actionVector1Q12; 
    Q12 actionVector2Q12; 
    uint32_t runtimeState40; 
    uint32_t runtimeState44; 
    uint32_t runtimeState48; 
    uint32_t runtimeState4C; 
    uint8_t reserved50_57[8]; 
    Q12 movementWorldXQ12; 
    Q12 movementWorldYQ12; 
    uint32_t classState60; 
    uint32_t ownerValue64; 
    uint32_t ownerValue68; 
    Ptr32<struct ArmyRuntimeSlot> linkedArmyRuntimeOrSavedOffset; 
    Q12 lastCheckedWorldXQ12; 
    Q12 lastCheckedWorldYQ12; 
    Q12 movementTargetWorldXQ12; 
    Q12 movementTargetWorldYQ12; 
    uint32_t definitionClassValue80; 
    uint32_t definitionClassValue84; 
    uint32_t definitionClassValue88; 
    uint32_t runtimeState8C; 
    uint32_t runtimeState90; 
    uint32_t runtimeState94; 
    uint32_t runtimeState98; 
    ModelRuntimeClassId depthBinClass; 
    PckArmyAssetIdCatalog armyAssetId; 
    ArmyMovementRetryCountdown retryCountdown; 
    ArmyWaypointCount queuedWaypointCount; 
    uint8_t unresolvedMovementPrefix[12]; 
    struct WorldPointXYQ12 fallbackPosition; 
    struct WorldPointXYQ12 queuedWaypoints[8]; 
    int stateOrTechnologyId; 
    uint32_t runtimeState104; 
    ArmySelectionMetric selectionMetric0; 
    ArmySelectionMetric selectionMetric1; 
    ArmySelectionMetric selectionMetric2; 
    ArmySelectionMetric selectionMetric3; 
    ArmySelectionMetric selectionMetric4; 
    ArmySelectionMetric selectionMetric5; 
};

struct RuntimeMaintenanceAudioRefreshCallbacks {
    Ptr32<void (struct WorldRuntimeContext *, struct WorldOwnerListNode *)> army; // the owner-list node of a model (ArmyRuntimeMaintenance_DispatchClassMethodDRecursive)
    Ptr32<void (struct WorldRuntimeContext *, struct ModelRuntimeNode *)> shot; 
    Ptr32<void (struct WorldRuntimeContext *, void *)> effect; 
};

struct RuntimeMaintenancePrimaryUpdateCallbacks {
    Ptr32<void (struct WorldRuntimeContext *, struct WorldOwnerListNode *)> army; // Every-tick primary update army slot.
    Ptr32<void (struct WorldRuntimeContext *, struct ModelRuntimeNode *)> shot; // Every-tick primary update shot slot.
    Ptr32<void (struct WorldRuntimeContext *, struct ModelRuntimeNode *)> effect; // Every-tick primary update effect slot.
};

struct RuntimeMaintenanceTerrainStateRefreshCallbacks {
    Ptr32<void (struct WorldRuntimeContext *, struct ModelRuntimeNode *)> army; 
    Ptr32<void (struct WorldRuntimeContext *, struct ModelRuntimeNode *)> shot; 
    Ptr32<void (struct WorldRuntimeContext *, struct ModelRuntimeNode *)> effect; 
};

struct RuntimeMaintenanceOccupancyRebuildCallbacks {
    Ptr32<void (struct WorldRuntimeContext *, struct WorldOwnerListNode *)> army; // Occupancy rebuild army slot.
    Ptr32<void (struct WorldRuntimeContext *, void *)> shot; // Occupancy rebuild shot slot.
    Ptr32<void (struct WorldRuntimeContext *, void *)> effect; // Occupancy rebuild effect slot.
};

struct RuntimeMaintenanceCallbackPhasesTyped {
    struct RuntimeMaintenancePrimaryUpdateCallbacks primaryUpdate; 
    struct RuntimeMaintenanceTerrainStateRefreshCallbacks terrainStateRefresh; 
    struct RuntimeMaintenanceOccupancyRebuildCallbacks occupancyRebuild; 
    struct RuntimeMaintenanceAudioRefreshCallbacks audioRefresh; 
};

using MdlReloadTicks = uint32_t;

struct ArmyPlacementContactCallbackTable5 {
    Ptr32<void (Q12, Q12, Q12, struct ModelRuntimeNode *, struct WorldRuntimeContext *)> callbacks[5]; 
};

/* Energy demand of a model and its directly attached models (ModelRuntimeHierarchy_ComputeEnergyDemand) */
struct ModelHierarchyEnergyDemand {
    uint32_t activeQ4; /* demand of the models not switched off */
    uint32_t totalQ4;  /* demand of all counted models */
};

/* Q12 vector returned by the fixed-point rotation helpers. */
struct FixedVectorQ12 {
    Q12 xQ12;
    Q12 yQ12;
    Q12 zQ12;
};

struct PathingDestination {
    Q12 primaryWorldXQ12; // primary X
    Q12 primaryWorldYQ12; // primary Y
    Q12 fallbackWorldXQ12; // fallback X
    Q12 fallbackWorldYQ12; // fallback Y
};

/* runtimeUpdate, classMethodD (sound update) and classCommand of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes
   are called with a model runtime (ModelRuntimeSlot, first dword = its ModelDefinition), never with an army:
   ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive, ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
   and ArmyRuntime_DispatchClassCommand pick the slot by the definition's runtimeClassId. The implementations take
   the ModelRuntimeSlot or the ModelRuntime*View of their class (same object, class-specific field names). */
typedef void ModelRuntimeClassCallback(struct WorldRuntimeContext *worldRuntime, struct ModelRuntimeSlot *modelRuntime);

struct ArmyRuntimeOrderHandlerMatrix11x24 {
    Ptr32<ModelRuntimeClassCallback> runtimeUpdate[24];
    Ptr32<ModelRuntimeClassCallback> classMethodD[24];
    Ptr32<void (struct ModelRuntimeSlot *)> modelUnrebase[24];
    Ptr32<void (struct ModelRuntimeSlot *)> modelRebaseOrLoadRepair[24];
    Ptr32<void (struct ModelDefinitionRecordPrefix *, struct ModelRuntimeSlot *)> modelClassInitialize[24];
    Ptr32<void (struct ModelDefinitionRecordPrefix *, struct ModelRuntimeSlot *)> modelReleaseOrCommit[24];
    Ptr32<Bool8 (struct WorldRuntimeContext *, struct ModelRuntimePlacementValidationView *)> placementValidation[24]; // 24 placement validators returning a bool. Split from generic world/army callbacks.
    Ptr32<Bool8 (uint32_t, uint32_t, uint32_t, uint32_t, int, int, struct ModelDefinitionRecordPrefix *, uint32_t, struct WorldRuntimeContext *, uint32_t *outPlacementValue)> placementAssetClassDispatch[24]; // true = accepted, *outPlacementValue set only then
    Ptr32<ModelRuntimeClassCallback> classCommand[24];
    Ptr32<void (struct GameEntityRuntime *)> gridInfluenceAdd[24];
    Ptr32<void (struct GameEntityRuntime *)> gridInfluenceRemove[24];
};

struct ArmyRuntimeClassUpdate21DefinitionView {
    uint8_t opaqueGap0000_000B[12]; // Not yet named.
    Q12 movementStepQ12;
    uint8_t opaqueGap0010_0013[4]; // Not yet named.
    int arcCoefficient;
    uint8_t opaqueGap0018_002B[20]; // Not yet named.
    PckEffectDefinitionIdCatalog modelPointEffectId;
    int modelPointStep;
    uint8_t opaqueGap0034_0047[20]; // Not yet named.
    uint32_t worldPointAllowedContext;
    uint8_t opaqueGap004C_0053[8]; // Not yet named.
    Q12 placementHeightOffsetQ12; // ModelDefinition.placementHeightOffsetQ12
    uint8_t opaqueGap0058_005F[8]; // Not yet named.
    Q12 maximumHealth;
    Ptr32<void> rootNode;
    uint8_t opaqueGap0068_00BF[88]; // Not yet named.
    uint32_t phaseInitial;
    uint32_t phaseDuration;
    int travelStepCount;
    int verticalArcCoefficient;
    uint8_t opaqueGap00D0_00DB[12]; // Not yet named.
    DepthIntervalRadius32 footprintRadius;
    uint8_t opaqueGap00E0_018F[176]; // Not yet named.
    Ptr32<struct EffectDefinition> removalEffect;
    uint8_t opaqueGap0194_026B[216]; // Not yet named.
    SoundAssetIndex terrainSoundAssetIndex;
    uint8_t opaqueGap0270_0277[8]; // Not yet named.
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Shared placement-contact callback dispatch index; value vocabulary 0..4 documented by the placement callback table.
};

struct ModelRuntimePlacementValidationView {
    Ptr32<struct ModelDefinition> modelDefinition; // Live placement-validation phase: dispatcher reads the slot's first field as ModelDefinition and selects class via its runtimeClassId; targets read placement fields including footprintRadius, placementFlags, footprintRadiusClass, terrainTraversalClass and placementContactKindIndex.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live placement-validation phase root ModelRuntimeNode; targets read world transform fields from this pointer.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live placement-validation phase owner ArmyRuntimeSlot; targets read factionIndex and pass owner/runtime context onward.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    uint8_t classPrefixState[40]; // Unresolved common runtime state.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeClassLinkState classLinkState; // Class-specific model/army pointer-or-offset overlays and adjacent state.
    struct ModelRuntimeSlotClassState classState; // First grouped model-runtime class-state region derived from the 24x11 callback matrix.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ArmyWeaponDefinitionView {
    AssetRecordByteCount byteSize;
    uint32_t flags;
    PckModelDefinitionIdCatalog definitionId;
    int movementSpeed; // ModelDefinition.movementSpeed
    ArmyTurnVelocityAngle16 yawTurnRateLimitAnglePerTick; // Positive magnitude cap for the signed yaw turn velocity used by ModelNodeRuntime_SmoothYawTowardTarget.
    ArmyTurnVelocityAngle16 pitchTurnRateLimitAnglePerTick; // Positive magnitude cap for the signed pitch turn velocity used by ModelNodeRuntime_SmoothPitchTowardTarget.
    int accelerationPerTick; // ModelDefinition.accelerationPerTick
    ArmyTurnVelocityAngle16 yawTurnRateAccelerationAnglePerTick; // Per-tick acceleration/deceleration step applied to yaw turn velocity.
    ArmyTurnVelocityAngle16 pitchTurnRateAccelerationAnglePerTick; // Per-tick acceleration/deceleration step applied to pitch turn velocity.
    AngleTurn32 minimumPitchAngle; // Lower clamped pitch target for articulated weapon aiming.
    AngleTurn32 maximumPitchAngle; // Upper clamped pitch target for articulated weapon aiming.
    Ptr32<struct ShotDefinition> shotDefinition; // Pointer consumed by the shot aim, launch-angle and launch-resolution helpers.
    uint32_t attachmentReloadTicks; // Value copied into one of eight selector reload slots when a launch is attempted.
    int localRotationAngle2StepPerTick; // Signed per-tick increment applied to the articulated child node localRotationAngle2 during the class-7/8 weapon recoil/aim state.
    uint32_t sharedInterShotTicks; // Value copied into the shared firing gate after a successful launch.
    FixedMathScale32 backwardStepScale; // Scale supplied to FixedVector_StepBackwardAlongOwnDirection for the class-7/8 articulated weapon recoil step.
    Q12 postLaunchVector0Q12; // Q12 value passed to the post-launch action-vector update.
    Q12 postLaunchVector1Q12; // Q12 value passed to the post-launch action-vector update.
    uint8_t opaqueGap0048_0063[28]; // Not yet named.
    Ptr32<struct MdlSerializedNodeHeader> rootNode; // Live MDL root node pointer after ModelDefinition_RegisterAndResolveReferences rebases ModelDefinition.rootNodeOffsetOrPointer.
};

struct RuntimeCollisionQueryView {
    Ptr32<struct ModelDefinition> modelDefinition; // Common collision-query layout: the first field is dereferenced directly for ModelDefinition.footprintRadius.
    Ptr32<struct ModelRuntimeNode> modelNodeRuntime; // Common collision-query layout: the second field supplies model-node depth bins.
    uint8_t reserved0008_00EF[232];
    Ptr32<void> linkedRuntime; // the model runtime this unit is linked to (ModelRuntimeSlotClassState.linkedArmyRuntimeOrSavedOffset), excluded from the collision test.
};

struct ModelRuntimeUpdateView {
    Ptr32<struct ModelDefinition> modelDefinition; // Live runtime-update phase definition pointer.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live root ModelRuntimeNode.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live owning ArmyRuntimeSlot.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    uint8_t classPrefixState[40]; // Unresolved common runtime state.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeClassLinkState classLinkState; // Class-specific model/army pointer-or-offset overlays and adjacent state.
    struct ModelRuntimeSlotClassState classState; // First grouped model-runtime class-state region derived from the 24x11 callback matrix.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ModelDefinitionClass14PlacementView {
    AssetRecordByteCount byteSize;
    uint32_t flags;
    PckModelDefinitionIdCatalog definitionId;
    uint8_t reserved00C_023[24]; // Not yet named.
    uint32_t runtimeValue24;
    uint32_t runtimeValue28;
    union ShotDefinitionReferenceOrSavedId shotDefinitionReference;
    uint8_t reserved030_047[24]; // Not yet named.
    uint32_t visibilityRadius;
    ModelRuntimeClassId runtimeClassId; // 24-way model/army runtime callback class selector; consumed by placement, grid-influence, maintenance, and class-method dispatch tables.
    uint8_t field10_0x50;
    uint8_t field11_0x51;
    uint8_t field12_0x52;
    uint8_t field13_0x53;
    Q12 placementHeightOffsetQ12; // Q12 height offset passed as the first argument to the five ArmyPlacementContact callbacks.
    union EffectDefinitionReferenceOrSavedId waterEmitterEffectDefinitionReference;
    uint8_t reserved05C_05F[4]; // Not yet named.
    uint32_t maximumHealth; /* maximum health */
    uint32_t rootNodeOffsetOrPointer;
    uint32_t modelFlags;
    uint8_t field20_0x6c;
    uint8_t field21_0x6d;
    uint8_t field22_0x6e;
    uint8_t field23_0x6f;
    uint8_t field24_0x70;
    uint8_t field25_0x71;
    uint8_t field26_0x72;
    uint8_t field27_0x73;
    uint8_t field28_0x74;
    uint8_t field29_0x75;
    uint8_t field30_0x76;
    uint8_t field31_0x77;
    SpatialSoundGainQ15 positionedSoundGainQ15; // Q15 gain passed with positionedSoundMaximumDistanceQ12 to positioned-sound playback/update helpers.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // Q12 maximum positioned-sound distance paired with positionedSoundGainQ15.
    union EffectDefinitionReferenceOrSavedId destructionEffect0;
    uint32_t destructionEffectDelayTicks0;
    union EffectDefinitionReferenceOrSavedId destructionEffect1;
    uint32_t destructionEffectDelayTicks1;
    union EffectDefinitionReferenceOrSavedId destructionEffect2;
    uint32_t destructionEffectDelayTicks2;
    union EffectDefinitionReferenceOrSavedId destructionEffect3;
    uint32_t destructionEffectDelayTicks3;
    union EffectDefinitionReferenceOrSavedId destructionEffect4;
    uint32_t destructionEffectDelayTicks4;
    union EffectDefinitionReferenceOrSavedId destructionEffect5;
    uint32_t destructionEffectDelayTicks5;
    union EffectDefinitionReferenceOrSavedId destructionEffect6;
    uint32_t destructionEffectDelayTicks6;
    union EffectDefinitionReferenceOrSavedId destructionEffect7;
    uint32_t destructionEffectDelayTicks7;
    uint32_t resourceFieldSupportSelector; // Runtime class 14 only. Slot 14 placement callback uses 0x800 << low5(selector) as required positive FieldGrid support bit; selector 0=>0x0800 Xenite, 1=>0x1000 Tritium. Do not generalize this name to other runtime classes.
    uint8_t field51_0xc4;
    uint8_t field52_0xc5;
    uint8_t field53_0xc6;
    uint8_t field54_0xc7;
    int claimedCellTag; /* stored << 24 into the resource cell the extractor claims */
    uint8_t field59_0xcc;
    uint8_t field60_0xcd;
    uint8_t field61_0xce;
    uint8_t field62_0xcf;
    uint8_t field63_0xd0;
    uint8_t field64_0xd1;
    uint8_t field65_0xd2;
    uint8_t field66_0xd3;
    uint8_t field67_0xd4;
    uint8_t field68_0xd5;
    uint8_t field69_0xd6;
    uint8_t field70_0xd7;
    uint8_t field71_0xd8;
    uint8_t field72_0xd9;
    uint8_t field73_0xda;
    uint8_t field74_0xdb;
    uint32_t footprintRadius;
    uint8_t reserved0E0_167[136]; // Not yet named.
    union ShotDefinitionReferenceOrSavedId emitterShotDefinitionReference;
    uint8_t reserved16C_173[8]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId emitterEffectDefinitionReference;
    uint8_t reserved178_187[16]; // Not yet named.
    uint32_t buildEnergyLoadQ4;
    uint32_t energyLoadQ4; /* Energy demand (copied to the model runtime's energyLoadQ4) */
    union EffectDefinitionReferenceOrSavedId removalEffectDefinitionReference;
    uint8_t reserved194_197[4]; // Not yet named.
    uint32_t waterDamageThreshold;
    uint8_t reserved19C_19F[4]; // Not yet named.
    uint32_t footprintRadiusCopy; // Grid-derived runtime value selected through footprintRadiusClass; not an Effect definition reference.
    uint8_t reserved1A4_1A7[4]; // Not yet named.
    uint32_t placementFlags;
    uint8_t reserved1AC_1B7[12]; // Not yet named.
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex;
    uint8_t reserved1C0_253[148]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId damageEffectDefinitionReference;
    uint8_t reserved258_25F[8]; // Not yet named.
    uint32_t footprintRadiusClass;
    uint32_t terrainTraversalClass;
    uint32_t traversalSecondaryThreshold;
    uint8_t reserved26C_277[12]; // Not yet named.
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Placement-contact callback dispatch index. Observed vocabulary: 0 terrain height; 1 water-surface height; 2 terrain height+normal; 3 articulated suspension; 4 top-surface height. Kept as a 32-bit index typedef rather than enum storage.
    uint32_t occupancyMarkRadius;
};

struct ModelRuntimePlacementClass14View {
    Ptr32<struct ModelDefinitionClass14PlacementView> modelDefinition; // Class-14 placement-validation definition view; placementValidation slot 14 targets ArmyPlacement_TestGridOccupancyMask.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live placement-validation phase root ModelRuntimeNode; targets read world transform fields from this pointer.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live placement-validation phase owner ArmyRuntimeSlot; targets read factionIndex and pass owner/runtime context onward.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    uint8_t classPrefixState[40]; // Unresolved common runtime state.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeClassLinkState classLinkState; // Class-specific model/army pointer-or-offset overlays and adjacent state.
    struct ModelRuntimeSlotClassState classState; // First grouped model-runtime class-state region derived from the 24x11 callback matrix.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ModelRuntimeClass14UpdateView {
    Ptr32<struct ModelDefinitionClass14PlacementView> modelDefinition; // Runtime class-14 definition; resourceFieldSupportSelector is the verified resource-field selector for this class.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live root ModelRuntimeNode.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live owning ArmyRuntimeSlot.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    uint8_t classPrefixState[40]; // Unresolved common runtime state.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeClassLinkState classLinkState; // Class-specific model/army pointer-or-offset overlays and adjacent state.
    struct ModelRuntimeSlotClassState classState; // First grouped model-runtime class-state region derived from the 24x11 callback matrix.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct FixedLengthAngle {
    uint32_t length; // Result of FixedMath_Length2.
    AngleTurn32 angle; // Masked low-16 angle from FixedMath_Atan2Angle16.
};

struct TerrainPlacementResult {
    int value; // Callback result value; callers use it only on paths proven against the original.
    Bool8 rejected; // Callback status: true when the placement is rejected.
};

struct ModelRuntimeLinkedChildSpawnInheritedState {
    WorldMotionValue70 inheritedValue70; // First inherited state word passed to ArmyRuntimeSpawner_CreateLinkedChildInstance.
    WorldMotionValue74 inheritedValue74; // Second inherited state word passed to ArmyRuntimeSpawner_CreateLinkedChildInstance.
    WorldMotionValue78 inheritedValue78; // Third inherited state word passed to ArmyRuntimeSpawner_CreateLinkedChildInstance.
};

struct ModelDefinitionVerticalDeploymentView {
    AssetRecordByteCount byteSize;
    uint32_t flags;
    PckModelDefinitionIdCatalog definitionId;
    Q12 verticalDeploymentStepQ12PerTick; // Signed local-Z displacement applied per simulation tick while deploying/retracting the child model.
    uint8_t field4_0x10;
    uint8_t field5_0x11;
    uint8_t field6_0x12;
    uint8_t field7_0x13;
    uint8_t field8_0x14;
    uint8_t field9_0x15;
    uint8_t field10_0x16;
    uint8_t field11_0x17;
    uint8_t field12_0x18;
    uint8_t field13_0x19;
    uint8_t field14_0x1a;
    uint8_t field15_0x1b;
    uint8_t field16_0x1c;
    uint8_t field17_0x1d;
    uint8_t field18_0x1e;
    uint8_t field19_0x1f;
    uint8_t field20_0x20;
    uint8_t field21_0x21;
    uint8_t field22_0x22;
    uint8_t field23_0x23;
    Q12 deploymentTravelLimitQ12; // Class-23 deployment travel threshold used to select endpoint transitions and sounds.
    uint32_t runtimeValue28;
    union ShotDefinitionReferenceOrSavedId shotDefinitionReference;
    uint8_t reserved030_047[24]; // Not yet named.
    uint32_t visibilityRadius;
    ModelRuntimeClassId runtimeClassId; // 24-way model/army runtime callback class selector; consumed by placement, grid-influence, maintenance, and class-method dispatch tables.
    uint8_t field30_0x50;
    uint8_t field31_0x51;
    uint8_t field32_0x52;
    uint8_t field33_0x53;
    Q12 placementHeightOffsetQ12; // Q12 height offset passed as the first argument to the five ArmyPlacementContact callbacks.
    union EffectDefinitionReferenceOrSavedId waterEmitterEffectDefinitionReference;
    uint8_t reserved05C_05F[4]; // Not yet named.
    uint32_t maximumHealth; /* maximum health */
    uint32_t rootNodeOffsetOrPointer;
    uint32_t modelFlags;
    uint8_t field40_0x6c;
    uint8_t field41_0x6d;
    uint8_t field42_0x6e;
    uint8_t field43_0x6f;
    uint8_t field44_0x70;
    uint8_t field45_0x71;
    uint8_t field46_0x72;
    uint8_t field47_0x73;
    uint8_t field48_0x74;
    uint8_t field49_0x75;
    uint8_t field50_0x76;
    uint8_t field51_0x77;
    SpatialSoundGainQ15 positionedSoundGainQ15; // Q15 gain passed with positionedSoundMaximumDistanceQ12 to positioned-sound playback/update helpers.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // Q12 maximum positioned-sound distance paired with positionedSoundGainQ15.
    union EffectDefinitionReferenceOrSavedId destructionEffect0;
    uint32_t destructionEffectDelayTicks0;
    union EffectDefinitionReferenceOrSavedId destructionEffect1;
    uint32_t destructionEffectDelayTicks1;
    union EffectDefinitionReferenceOrSavedId destructionEffect2;
    uint32_t destructionEffectDelayTicks2;
    union EffectDefinitionReferenceOrSavedId destructionEffect3;
    uint32_t destructionEffectDelayTicks3;
    union EffectDefinitionReferenceOrSavedId destructionEffect4;
    uint32_t destructionEffectDelayTicks4;
    union EffectDefinitionReferenceOrSavedId destructionEffect5;
    uint32_t destructionEffectDelayTicks5;
    union EffectDefinitionReferenceOrSavedId destructionEffect6;
    uint32_t destructionEffectDelayTicks6;
    union EffectDefinitionReferenceOrSavedId destructionEffect7;
    uint32_t destructionEffectDelayTicks7;
    uint8_t reserved0C0_0DB[28]; // Not yet named.
    uint32_t footprintRadius;
    uint8_t reserved0E0_167[136]; // Not yet named.
    union ShotDefinitionReferenceOrSavedId emitterShotDefinitionReference;
    uint8_t reserved16C_173[8]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId emitterEffectDefinitionReference;
    uint8_t reserved178_187[16]; // Not yet named.
    uint32_t buildEnergyLoadQ4;
    uint32_t energyLoadQ4; /* Energy demand (copied to the model runtime's energyLoadQ4) */
    union EffectDefinitionReferenceOrSavedId removalEffectDefinitionReference;
    uint8_t reserved194_197[4]; // Not yet named.
    uint32_t waterDamageThreshold;
    uint8_t reserved19C_19F[4]; // Not yet named.
    uint32_t footprintRadiusCopy; // Grid-derived runtime value selected through footprintRadiusClass; not an Effect definition reference.
    uint8_t reserved1A4_1A7[4]; // Not yet named.
    uint32_t placementFlags;
    uint8_t reserved1AC_1B7[12]; // Not yet named.
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex;
    uint8_t reserved1C0_253[148]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId damageEffectDefinitionReference;
    uint8_t reserved258_25F[8]; // Not yet named.
    uint32_t footprintRadiusClass;
    uint32_t terrainTraversalClass;
    uint32_t traversalSecondaryThreshold;
    SoundAssetIndex deploymentSoundAssetIndex; // Positioned one-shot sound asset index used at class-23 deployment endpoints.
    uint8_t field96_0x270;
    uint8_t field97_0x271;
    uint8_t field98_0x272;
    uint8_t field99_0x273;
    uint8_t field100_0x274;
    uint8_t field101_0x275;
    uint8_t field102_0x276;
    uint8_t field103_0x277;
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Placement-contact callback dispatch index. Observed vocabulary: 0 terrain height; 1 water-surface height; 2 terrain height+normal; 3 articulated suspension; 4 top-surface height. Kept as a 32-bit index typedef rather than enum storage.
    uint32_t occupancyMarkRadius;
};

struct ModelRuntimeTimedEffectsUpdateView {
    Ptr32<struct ModelDefinitionTimedEffectsUpdateView> modelDefinition; // Class-specific definition overlay containing the timed-effects update gate.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live root ModelRuntimeNode.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live owning ArmyRuntimeSlot.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    uint8_t classPrefixState[40]; // Unresolved common runtime state.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeClassLinkState classLinkState; // Class-specific model/army pointer-or-offset overlays and adjacent state.
    struct ModelRuntimeSlotClassState classState; // First grouped model-runtime class-state region derived from the 24x11 callback matrix.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ModelDefinitionTimedEffectsUpdateView {
    AssetRecordByteCount byteSize;
    uint32_t flags;
    PckModelDefinitionIdCatalog definitionId;
    uint8_t reserved00C_023[24]; // Not yet named.
    uint32_t runtimeValue24;
    uint32_t runtimeValue28;
    union ShotDefinitionReferenceOrSavedId shotDefinitionReference;
    uint8_t reserved030_047[24]; // Not yet named.
    uint32_t visibilityRadius;
    ModelRuntimeClassId runtimeClassId; // 24-way model/army runtime callback class selector; consumed by placement, grid-influence, maintenance, and class-method dispatch tables.
    uint8_t field10_0x50;
    uint8_t field11_0x51;
    uint8_t field12_0x52;
    uint8_t field13_0x53;
    Q12 placementHeightOffsetQ12; // Q12 height offset passed as the first argument to the five ArmyPlacementContact callbacks.
    union EffectDefinitionReferenceOrSavedId waterEmitterEffectDefinitionReference;
    uint8_t reserved05C_05F[4]; // Not yet named.
    uint32_t maximumHealth; /* maximum health */
    uint32_t rootNodeOffsetOrPointer;
    uint32_t modelFlags;
    uint8_t field20_0x6c;
    uint8_t field21_0x6d;
    uint8_t field22_0x6e;
    uint8_t field23_0x6f;
    uint8_t field24_0x70;
    uint8_t field25_0x71;
    uint8_t field26_0x72;
    uint8_t field27_0x73;
    uint8_t field28_0x74;
    uint8_t field29_0x75;
    uint8_t field30_0x76;
    uint8_t field31_0x77;
    SpatialSoundGainQ15 positionedSoundGainQ15; // Q15 gain passed with positionedSoundMaximumDistanceQ12 to positioned-sound playback/update helpers.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // Q12 maximum positioned-sound distance paired with positionedSoundGainQ15.
    union EffectDefinitionReferenceOrSavedId destructionEffect0;
    uint32_t destructionEffectDelayTicks0;
    union EffectDefinitionReferenceOrSavedId destructionEffect1;
    uint32_t destructionEffectDelayTicks1;
    union EffectDefinitionReferenceOrSavedId destructionEffect2;
    uint32_t destructionEffectDelayTicks2;
    union EffectDefinitionReferenceOrSavedId destructionEffect3;
    uint32_t destructionEffectDelayTicks3;
    union EffectDefinitionReferenceOrSavedId destructionEffect4;
    uint32_t destructionEffectDelayTicks4;
    union EffectDefinitionReferenceOrSavedId destructionEffect5;
    uint32_t destructionEffectDelayTicks5;
    union EffectDefinitionReferenceOrSavedId destructionEffect6;
    uint32_t destructionEffectDelayTicks6;
    union EffectDefinitionReferenceOrSavedId destructionEffect7;
    uint32_t destructionEffectDelayTicks7;
    uint8_t reserved0C0_0DB[28]; // Not yet named.
    uint32_t footprintRadius;
    uint8_t reserved0E0_167[136]; // Not yet named.
    union ShotDefinitionReferenceOrSavedId emitterShotDefinitionReference;
    uint8_t reserved16C_173[8]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId emitterEffectDefinitionReference;
    uint8_t reserved178_187[16]; // Not yet named.
    uint32_t buildEnergyLoadQ4;
    uint32_t energyLoadQ4; /* Energy demand (copied to the model runtime's energyLoadQ4) */
    union EffectDefinitionReferenceOrSavedId removalEffectDefinitionReference;
    uint8_t reserved194_197[4]; // Not yet named.
    uint32_t waterDamageThreshold;
    uint8_t reserved19C_19F[4]; // Not yet named.
    uint32_t footprintRadiusCopy; // Grid-derived runtime value selected through footprintRadiusClass; not an Effect definition reference.
    uint8_t reserved1A4_1A7[4]; // Not yet named.
    uint32_t placementFlags;
    uint8_t reserved1AC_1B7[12]; // Not yet named.
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex;
    uint8_t field69_0x1c0;
    uint8_t field70_0x1c1;
    uint8_t field71_0x1c2;
    uint8_t field72_0x1c3;
    uint8_t field73_0x1c4;
    uint8_t field74_0x1c5;
    uint8_t field75_0x1c6;
    uint8_t field76_0x1c7;
    int timedEffectsRequireStateBit40; // Exact machine semantics: 0 allows timed-emitter and animated-subnode updates whenever stateFlags bits 0/3 are clear; nonzero additionally requires runtime stateFlags bit 0x40.
    uint8_t field78_0x1cc;
    uint8_t field79_0x1cd;
    uint8_t field80_0x1ce;
    uint8_t field81_0x1cf;
    uint8_t field82_0x1d0;
    uint8_t field83_0x1d1;
    uint8_t field84_0x1d2;
    uint8_t field85_0x1d3;
    uint8_t field86_0x1d4;
    uint8_t field87_0x1d5;
    uint8_t field88_0x1d6;
    uint8_t field89_0x1d7;
    uint8_t field90_0x1d8;
    uint8_t field91_0x1d9;
    uint8_t field92_0x1da;
    uint8_t field93_0x1db;
    uint8_t field94_0x1dc;
    uint8_t field95_0x1dd;
    uint8_t field96_0x1de;
    uint8_t field97_0x1df;
    uint8_t field98_0x1e0;
    uint8_t field99_0x1e1;
    uint8_t field100_0x1e2;
    uint8_t field101_0x1e3;
    uint8_t field102_0x1e4;
    uint8_t field103_0x1e5;
    uint8_t field104_0x1e6;
    uint8_t field105_0x1e7;
    uint8_t field106_0x1e8;
    uint8_t field107_0x1e9;
    uint8_t field108_0x1ea;
    uint8_t field109_0x1eb;
    uint8_t field110_0x1ec;
    uint8_t field111_0x1ed;
    uint8_t field112_0x1ee;
    uint8_t field113_0x1ef;
    uint8_t field114_0x1f0;
    uint8_t field115_0x1f1;
    uint8_t field116_0x1f2;
    uint8_t field117_0x1f3;
    uint8_t field118_0x1f4;
    uint8_t field119_0x1f5;
    uint8_t field120_0x1f6;
    uint8_t field121_0x1f7;
    uint8_t field122_0x1f8;
    uint8_t field123_0x1f9;
    uint8_t field124_0x1fa;
    uint8_t field125_0x1fb;
    uint8_t field126_0x1fc;
    uint8_t field127_0x1fd;
    uint8_t field128_0x1fe;
    uint8_t field129_0x1ff;
    uint8_t field130_0x200;
    uint8_t field131_0x201;
    uint8_t field132_0x202;
    uint8_t field133_0x203;
    uint8_t field134_0x204;
    uint8_t field135_0x205;
    uint8_t field136_0x206;
    uint8_t field137_0x207;
    uint8_t field138_0x208;
    uint8_t field139_0x209;
    uint8_t field140_0x20a;
    uint8_t field141_0x20b;
    uint8_t field142_0x20c;
    uint8_t field143_0x20d;
    uint8_t field144_0x20e;
    uint8_t field145_0x20f;
    uint8_t field146_0x210;
    uint8_t field147_0x211;
    uint8_t field148_0x212;
    uint8_t field149_0x213;
    uint8_t field150_0x214;
    uint8_t field151_0x215;
    uint8_t field152_0x216;
    uint8_t field153_0x217;
    uint8_t field154_0x218;
    uint8_t field155_0x219;
    uint8_t field156_0x21a;
    uint8_t field157_0x21b;
    uint8_t field158_0x21c;
    uint8_t field159_0x21d;
    uint8_t field160_0x21e;
    uint8_t field161_0x21f;
    uint8_t field162_0x220;
    uint8_t field163_0x221;
    uint8_t field164_0x222;
    uint8_t field165_0x223;
    uint8_t field166_0x224;
    uint8_t field167_0x225;
    uint8_t field168_0x226;
    uint8_t field169_0x227;
    uint8_t field170_0x228;
    uint8_t field171_0x229;
    uint8_t field172_0x22a;
    uint8_t field173_0x22b;
    uint8_t field174_0x22c;
    uint8_t field175_0x22d;
    uint8_t field176_0x22e;
    uint8_t field177_0x22f;
    uint8_t field178_0x230;
    uint8_t field179_0x231;
    uint8_t field180_0x232;
    uint8_t field181_0x233;
    uint8_t field182_0x234;
    uint8_t field183_0x235;
    uint8_t field184_0x236;
    uint8_t field185_0x237;
    uint8_t field186_0x238;
    uint8_t field187_0x239;
    uint8_t field188_0x23a;
    uint8_t field189_0x23b;
    uint8_t field190_0x23c;
    uint8_t field191_0x23d;
    uint8_t field192_0x23e;
    uint8_t field193_0x23f;
    uint8_t field194_0x240;
    uint8_t field195_0x241;
    uint8_t field196_0x242;
    uint8_t field197_0x243;
    uint8_t field198_0x244;
    uint8_t field199_0x245;
    uint8_t field200_0x246;
    uint8_t field201_0x247;
    uint8_t field202_0x248;
    uint8_t field203_0x249;
    uint8_t field204_0x24a;
    uint8_t field205_0x24b;
    uint8_t field206_0x24c;
    uint8_t field207_0x24d;
    uint8_t field208_0x24e;
    uint8_t field209_0x24f;
    uint8_t field210_0x250;
    uint8_t field211_0x251;
    uint8_t field212_0x252;
    uint8_t field213_0x253;
    union EffectDefinitionReferenceOrSavedId damageEffectDefinitionReference;
    uint8_t reserved258_25F[8]; // Not yet named.
    uint32_t footprintRadiusClass;
    uint32_t terrainTraversalClass;
    uint32_t traversalSecondaryThreshold;
    uint8_t reserved26C_277[12]; // Not yet named.
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Placement-contact callback dispatch index. Observed vocabulary: 0 terrain height; 1 water-surface height; 2 terrain height+normal; 3 articulated suspension; 4 top-surface height. Kept as a 32-bit index typedef rather than enum storage.
    uint32_t occupancyMarkRadius;
};

struct ModelDefinitionLinkedChildStateView {
    AssetRecordByteCount byteSize;
    uint32_t flags;
    PckModelDefinitionIdCatalog definitionId;
    ModelTextureOffsetTexel linkedChildTextureVStepPerTick; // Signed per-tick primary texture-V step used by the linked-child transition animation.
    uint8_t field4_0x10;
    uint8_t field5_0x11;
    uint8_t field6_0x12;
    uint8_t field7_0x13;
    uint8_t field8_0x14;
    uint8_t field9_0x15;
    uint8_t field10_0x16;
    uint8_t field11_0x17;
    uint8_t field12_0x18;
    uint8_t field13_0x19;
    uint8_t field14_0x1a;
    uint8_t field15_0x1b;
    uint8_t field16_0x1c;
    uint8_t field17_0x1d;
    uint8_t field18_0x1e;
    uint8_t field19_0x1f;
    uint8_t field20_0x20;
    uint8_t field21_0x21;
    uint8_t field22_0x22;
    uint8_t field23_0x23;
    uint32_t runtimeValue24;
    uint32_t runtimeValue28;
    union ShotDefinitionReferenceOrSavedId shotDefinitionReference;
    uint8_t reserved030_047[24]; // Not yet named.
    uint32_t visibilityRadius;
    ModelRuntimeClassId runtimeClassId; // 24-way model/army runtime callback class selector; consumed by placement, grid-influence, maintenance, and class-method dispatch tables.
    uint8_t field30_0x50;
    uint8_t field31_0x51;
    uint8_t field32_0x52;
    uint8_t field33_0x53;
    Q12 placementHeightOffsetQ12; // Q12 height offset passed as the first argument to the five ArmyPlacementContact callbacks.
    union EffectDefinitionReferenceOrSavedId waterEmitterEffectDefinitionReference;
    uint8_t reserved05C_05F[4]; // Not yet named.
    uint32_t maximumHealth; /* maximum health */
    uint32_t rootNodeOffsetOrPointer;
    uint32_t modelFlags;
    uint8_t field40_0x6c;
    uint8_t field41_0x6d;
    uint8_t field42_0x6e;
    uint8_t field43_0x6f;
    uint8_t field44_0x70;
    uint8_t field45_0x71;
    uint8_t field46_0x72;
    uint8_t field47_0x73;
    uint8_t field48_0x74;
    uint8_t field49_0x75;
    uint8_t field50_0x76;
    uint8_t field51_0x77;
    SpatialSoundGainQ15 positionedSoundGainQ15; // Q15 gain passed with positionedSoundMaximumDistanceQ12 to positioned-sound playback/update helpers.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // Q12 maximum positioned-sound distance paired with positionedSoundGainQ15.
    union EffectDefinitionReferenceOrSavedId destructionEffect0;
    uint32_t destructionEffectDelayTicks0;
    union EffectDefinitionReferenceOrSavedId destructionEffect1;
    uint32_t destructionEffectDelayTicks1;
    union EffectDefinitionReferenceOrSavedId destructionEffect2;
    uint32_t destructionEffectDelayTicks2;
    union EffectDefinitionReferenceOrSavedId destructionEffect3;
    uint32_t destructionEffectDelayTicks3;
    union EffectDefinitionReferenceOrSavedId destructionEffect4;
    uint32_t destructionEffectDelayTicks4;
    union EffectDefinitionReferenceOrSavedId destructionEffect5;
    uint32_t destructionEffectDelayTicks5;
    union EffectDefinitionReferenceOrSavedId destructionEffect6;
    uint32_t destructionEffectDelayTicks6;
    union EffectDefinitionReferenceOrSavedId destructionEffect7;
    uint32_t destructionEffectDelayTicks7;
    uint8_t field70_0xc0;
    uint8_t field71_0xc1;
    uint8_t field72_0xc2;
    uint8_t field73_0xc3;
    FactionArmyAssetCount linkedChildSlotCapacity; // Maximum linked-child asset slots scanned/stored by this class.
    Q12 linkedChildTranslationStepQ12PerTick; // Per-tick local-Z step for linked-child deployment/retraction.
    Q12 linkedChildTranslationLimitQ12; // Upper local-Z endpoint for the linked-child transition.
    uint8_t field77_0xd0;
    uint8_t field78_0xd1;
    uint8_t field79_0xd2;
    uint8_t field80_0xd3;
    uint8_t field81_0xd4;
    uint8_t field82_0xd5;
    uint8_t field83_0xd6;
    uint8_t field84_0xd7;
    uint8_t field85_0xd8;
    uint8_t field86_0xd9;
    uint8_t field87_0xda;
    uint8_t field88_0xdb;
    uint32_t footprintRadius;
    uint8_t reserved0E0_167[136]; // Not yet named.
    union ShotDefinitionReferenceOrSavedId emitterShotDefinitionReference;
    uint8_t reserved16C_173[8]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId emitterEffectDefinitionReference;
    uint8_t reserved178_187[16]; // Not yet named.
    uint32_t buildEnergyLoadQ4;
    uint32_t energyLoadQ4; /* Energy demand (copied to the model runtime's energyLoadQ4) */
    union EffectDefinitionReferenceOrSavedId removalEffectDefinitionReference;
    uint8_t reserved194_197[4]; // Not yet named.
    uint32_t waterDamageThreshold;
    uint8_t reserved19C_19F[4]; // Not yet named.
    uint32_t footprintRadiusCopy; // Grid-derived runtime value selected through footprintRadiusClass; not an Effect definition reference.
    uint8_t reserved1A4_1A7[4]; // Not yet named.
    uint32_t placementFlags;
    uint8_t reserved1AC_1B7[12]; // Not yet named.
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex;
    uint8_t reserved1C0_253[148]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId damageEffectDefinitionReference;
    uint8_t reserved258_25F[8]; // Not yet named.
    uint32_t footprintRadiusClass;
    uint32_t terrainTraversalClass;
    uint32_t traversalSecondaryThreshold;
    SoundAssetIndex linkedChildTransitionSoundAssetIndex; // First positioned sound selector used by linked-child transition state changes.
    SoundAssetIndex linkedChildTransitionEndSoundAssetIndex; // Second positioned sound selector used after the texture-V transition reaches its upper endpoint.
    uint8_t field115_0x274;
    uint8_t field116_0x275;
    uint8_t field117_0x276;
    uint8_t field118_0x277;
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Placement-contact callback dispatch index. Observed vocabulary: 0 terrain height; 1 water-surface height; 2 terrain height+normal; 3 articulated suspension; 4 top-surface height. Kept as a 32-bit index typedef rather than enum storage.
    uint32_t occupancyMarkRadius;
};

struct ModelRuntimeTimedTargetState {
    uint8_t opaque10_23[20]; // Class-local runtime state not interpreted by this timed-target family.
    int targetProjectileReloadCountdownTicks; // Signed simulation-tick countdown decremented by g_InGameSimulationStepTicks and reloaded from ModelDefinition.reloadTicks.
    uint8_t opaque28_2B[4]; // Class-local dword not interpreted by this family.
    uint8_t opaque2C_2F[4]; // Class-local dword left unresolved; the owner-list callback's shot definition comes from modelDefinition->shotDefinitionReference, not from this runtime field.
    uint8_t opaque30_37[8]; // Class-local runtime tail not interpreted by this family.
};

struct ModelDefinitionDestroyEffectsView {
    AssetRecordByteCount byteSize;
    uint32_t flags;
    PckModelDefinitionIdCatalog definitionId;
    Q12 verticalTranslationStepQ12PerTick; // Signed world-Z translation step applied each simulation tick before destroying the model hierarchy at the travel limit.
    uint8_t field4_0x10;
    uint8_t field5_0x11;
    uint8_t field6_0x12;
    uint8_t field7_0x13;
    uint8_t field8_0x14;
    uint8_t field9_0x15;
    uint8_t field10_0x16;
    uint8_t field11_0x17;
    uint8_t field12_0x18;
    uint8_t field13_0x19;
    uint8_t field14_0x1a;
    uint8_t field15_0x1b;
    uint8_t field16_0x1c;
    uint8_t field17_0x1d;
    uint8_t field18_0x1e;
    uint8_t field19_0x1f;
    uint8_t field20_0x20;
    uint8_t field21_0x21;
    uint8_t field22_0x22;
    uint8_t field23_0x23;
    uint32_t runtimeValue24;
    uint32_t runtimeValue28;
    union ShotDefinitionReferenceOrSavedId shotDefinitionReference;
    uint8_t reserved030_047[24]; // Not yet named.
    uint32_t visibilityRadius;
    ModelRuntimeClassId runtimeClassId; // 24-way model/army runtime callback class selector; consumed by placement, grid-influence, maintenance, and class-method dispatch tables.
    uint8_t field30_0x50;
    uint8_t field31_0x51;
    uint8_t field32_0x52;
    uint8_t field33_0x53;
    Q12 placementHeightOffsetQ12; // Q12 height offset passed as the first argument to the five ArmyPlacementContact callbacks.
    union EffectDefinitionReferenceOrSavedId waterEmitterEffectDefinitionReference;
    uint8_t reserved05C_05F[4]; // Not yet named.
    uint32_t maximumHealth; /* maximum health */
    uint32_t rootNodeOffsetOrPointer;
    uint32_t modelFlags;
    uint8_t field40_0x6c;
    uint8_t field41_0x6d;
    uint8_t field42_0x6e;
    uint8_t field43_0x6f;
    uint8_t field44_0x70;
    uint8_t field45_0x71;
    uint8_t field46_0x72;
    uint8_t field47_0x73;
    uint8_t field48_0x74;
    uint8_t field49_0x75;
    uint8_t field50_0x76;
    uint8_t field51_0x77;
    SpatialSoundGainQ15 positionedSoundGainQ15; // Q15 gain passed with positionedSoundMaximumDistanceQ12 to positioned-sound playback/update helpers.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // Q12 maximum positioned-sound distance paired with positionedSoundGainQ15.
    union EffectDefinitionReferenceOrSavedId destructionEffect0;
    uint32_t destructionEffectDelayTicks0;
    union EffectDefinitionReferenceOrSavedId destructionEffect1;
    uint32_t destructionEffectDelayTicks1;
    union EffectDefinitionReferenceOrSavedId destructionEffect2;
    uint32_t destructionEffectDelayTicks2;
    union EffectDefinitionReferenceOrSavedId destructionEffect3;
    uint32_t destructionEffectDelayTicks3;
    union EffectDefinitionReferenceOrSavedId destructionEffect4;
    uint32_t destructionEffectDelayTicks4;
    union EffectDefinitionReferenceOrSavedId destructionEffect5;
    uint32_t destructionEffectDelayTicks5;
    union EffectDefinitionReferenceOrSavedId destructionEffect6;
    uint32_t destructionEffectDelayTicks6;
    union EffectDefinitionReferenceOrSavedId destructionEffect7;
    uint32_t destructionEffectDelayTicks7;
    uint8_t reserved0C0_0DB[28]; // Not yet named.
    uint32_t footprintRadius;
    uint8_t reserved0E0_167[136]; // Not yet named.
    union ShotDefinitionReferenceOrSavedId emitterShotDefinitionReference;
    uint8_t reserved16C_173[8]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId emitterEffectDefinitionReference;
    uint8_t reserved178_187[16]; // Not yet named.
    uint32_t buildEnergyLoadQ4;
    uint32_t energyLoadQ4; /* Energy demand (copied to the model runtime's energyLoadQ4) */
    union EffectDefinitionReferenceOrSavedId removalEffectDefinitionReference;
    uint8_t reserved194_197[4]; // Not yet named.
    uint32_t waterDamageThreshold;
    uint8_t reserved19C_19F[4]; // Not yet named.
    uint32_t footprintRadiusCopy; // Grid-derived runtime value selected through footprintRadiusClass; not an Effect definition reference.
    uint8_t reserved1A4_1A7[4]; // Not yet named.
    uint32_t placementFlags;
    uint8_t reserved1AC_1B7[12]; // Not yet named.
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex;
    uint8_t reserved1C0_253[148]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId damageEffectDefinitionReference;
    uint8_t reserved258_25F[8]; // Not yet named.
    uint32_t footprintRadiusClass;
    uint32_t terrainTraversalClass;
    uint32_t traversalSecondaryThreshold;
    uint8_t reserved26C_277[12]; // Not yet named.
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Placement-contact callback dispatch index. Observed vocabulary: 0 terrain height; 1 water-surface height; 2 terrain height+normal; 3 articulated suspension; 4 top-surface height. Kept as a 32-bit index typedef rather than enum storage.
    uint32_t occupancyMarkRadius;
};

struct ModelRuntimeDestroyEffectsView {
    Ptr32<struct ModelDefinitionDestroyEffectsView> modelDefinition; // Class-specific destroy/effects definition overlay.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live root ModelRuntimeNode.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live owning ArmyRuntimeSlot.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    uint8_t classPrefixState[40]; // Unresolved common runtime state.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeClassLinkState classLinkState; // Class-specific model/army pointer-or-offset overlays and adjacent state.
    struct ModelRuntimeSlotClassState classState; // First grouped model-runtime class-state region derived from the 24x11 callback matrix.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ModelRuntimeGroundMovementTrackView {
    Ptr32<struct ModelDefinitionGroundMovementTrackView> modelDefinition; // Ground-movement track-animation definition overlay.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live root ModelRuntimeNode.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live owning ArmyRuntimeSlot.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    struct ArmyRuntimeMovementControlState movementControl; // Model-runtime movement step and signed turn-rate state used by the ground-movement callbacks.
    uint8_t reserved18_37[32]; // Unresolved common runtime state following the recovered movement-control pair.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeClassLinkState classLinkState; // Class-specific model/army pointer-or-offset overlays and adjacent state.
    struct ModelRuntimeSlotClassState classState; // First grouped model-runtime class-state region derived from the 24x11 callback matrix.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ModelDefinitionGroundMovementTrackView {
    AssetRecordByteCount byteSize;
    uint32_t flags;
    PckModelDefinitionIdCatalog definitionId;
    Q12 movementStepQ12PerTick; // Per-tick planar movement step; also used as the short-range movement threshold scale in the banking variant.
    ArmyTurnVelocityAngle16 turnRateLimitAnglePerTick; // Positive magnitude cap for the signed per-tick heading turn rate.
    uint32_t trackTextureUScalePerDistance; // Signed scale converting traveled planar distance into animated track texture-U offset.
    uint8_t field6_0x18;
    uint8_t field7_0x19;
    uint8_t field8_0x1a;
    uint8_t field9_0x1b;
    ArmyTurnVelocityAngle16 turnRateAccelerationAnglePerTick; // Per-tick signed turn-rate acceleration toward the configured turn-rate limit.
    uint8_t field11_0x20;
    uint8_t field12_0x21;
    uint8_t field13_0x22;
    uint8_t field14_0x23;
    uint32_t runtimeValue24;
    uint32_t runtimeValue28;
    union ShotDefinitionReferenceOrSavedId shotDefinitionReference;
    uint8_t reserved030_047[24]; // Not yet named.
    uint32_t visibilityRadius;
    ModelRuntimeClassId runtimeClassId; // 24-way model/army runtime callback class selector; consumed by placement, grid-influence, maintenance, and class-method dispatch tables.
    uint8_t field21_0x50;
    uint8_t field22_0x51;
    uint8_t field23_0x52;
    uint8_t field24_0x53;
    Q12 placementHeightOffsetQ12; // Q12 height offset passed as the first argument to the five ArmyPlacementContact callbacks.
    union EffectDefinitionReferenceOrSavedId waterEmitterEffectDefinitionReference;
    uint8_t reserved05C_05F[4]; // Not yet named.
    uint32_t maximumHealth; /* maximum health */
    uint32_t rootNodeOffsetOrPointer;
    uint32_t modelFlags;
    uint8_t field31_0x6c;
    uint8_t field32_0x6d;
    uint8_t field33_0x6e;
    uint8_t field34_0x6f;
    uint8_t field35_0x70;
    uint8_t field36_0x71;
    uint8_t field37_0x72;
    uint8_t field38_0x73;
    uint8_t field39_0x74;
    uint8_t field40_0x75;
    uint8_t field41_0x76;
    uint8_t field42_0x77;
    SpatialSoundGainQ15 positionedSoundGainQ15; // Q15 gain passed with positionedSoundMaximumDistanceQ12 to positioned-sound playback/update helpers.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // Q12 maximum positioned-sound distance paired with positionedSoundGainQ15.
    union EffectDefinitionReferenceOrSavedId destructionEffect0;
    uint32_t destructionEffectDelayTicks0;
    union EffectDefinitionReferenceOrSavedId destructionEffect1;
    uint32_t destructionEffectDelayTicks1;
    union EffectDefinitionReferenceOrSavedId destructionEffect2;
    uint32_t destructionEffectDelayTicks2;
    union EffectDefinitionReferenceOrSavedId destructionEffect3;
    uint32_t destructionEffectDelayTicks3;
    union EffectDefinitionReferenceOrSavedId destructionEffect4;
    uint32_t destructionEffectDelayTicks4;
    union EffectDefinitionReferenceOrSavedId destructionEffect5;
    uint32_t destructionEffectDelayTicks5;
    union EffectDefinitionReferenceOrSavedId destructionEffect6;
    uint32_t destructionEffectDelayTicks6;
    union EffectDefinitionReferenceOrSavedId destructionEffect7;
    uint32_t destructionEffectDelayTicks7;
    Q12 headingErrorInterpolationDistanceQ12; // Distance below which the allowed heading error interpolates from the near-angle limit toward the far-angle limit; banking uses the same distance as its heading-hold threshold.
    AngleTurn32 farHeadingErrorLimitAngle; // Allowed heading-error angle at or beyond the interpolation distance.
    AngleTurn32 nearHeadingErrorLimitAngle; // Allowed heading-error angle at zero distance, linearly blended toward the far limit.
    uint8_t field64_0xcc;
    uint8_t field65_0xcd;
    uint8_t field66_0xce;
    uint8_t field67_0xcf;
    uint8_t field68_0xd0;
    uint8_t field69_0xd1;
    uint8_t field70_0xd2;
    uint8_t field71_0xd3;
    uint8_t field72_0xd4;
    uint8_t field73_0xd5;
    uint8_t field74_0xd6;
    uint8_t field75_0xd7;
    uint8_t field76_0xd8;
    uint8_t field77_0xd9;
    uint8_t field78_0xda;
    uint8_t field79_0xdb;
    uint32_t footprintRadius;
    uint8_t reserved0E0_167[136]; // Not yet named.
    union ShotDefinitionReferenceOrSavedId emitterShotDefinitionReference;
    uint8_t reserved16C_173[8]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId emitterEffectDefinitionReference;
    uint8_t reserved178_187[16]; // Not yet named.
    uint32_t buildEnergyLoadQ4;
    uint32_t energyLoadQ4; /* Energy demand (copied to the model runtime's energyLoadQ4) */
    union EffectDefinitionReferenceOrSavedId removalEffectDefinitionReference;
    int waterDamageMultiplier; // Signed multiplier applied to FieldGrid_InterpolateWaterDelta; arithmetic shift right by 7 produces the damage value.
    int waterDamageThreshold; // Signed interpolated-water-delta threshold; damage is evaluated only when the sampled delta is greater than this value.
    uint8_t reserved19C_19F[4]; // Not yet named.
    uint32_t footprintRadiusCopy; // Grid-derived runtime value selected through footprintRadiusClass; not an Effect definition reference.
    uint8_t reserved1A4_1A7[4]; // Not yet named.
    uint32_t placementFlags;
    uint8_t reserved1AC_1B7[12]; // Not yet named.
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex;
    uint8_t reserved1C0_253[148]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId damageEffectDefinitionReference;
    uint8_t reserved258_25F[8]; // Not yet named.
    uint32_t footprintRadiusClass;
    uint32_t terrainTraversalClass;
    uint32_t traversalSecondaryThreshold;
    uint8_t reserved26C_277[12]; // Not yet named.
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Placement-contact callback dispatch index. Observed vocabulary: 0 terrain height; 1 water-surface height; 2 terrain height+normal; 3 articulated suspension; 4 top-surface height. Kept as a 32-bit index typedef rather than enum storage.
    uint32_t occupancyMarkRadius;
};

struct ModelRuntimeLinkedChildPendingSpawnCounts {
    uint8_t slot0; // Pending spawn-attempt countdown for linked-child slot 0.
    uint8_t slot1; // Pending spawn-attempt countdown for linked-child slot 1.
    uint8_t slot2; // Pending spawn-attempt countdown for linked-child slot 2.
    uint8_t reserved03; // Unused/preserved fourth byte in this function family.
};

struct ModelRuntimeLinkedChildBuildState {
    PckArmyAssetIdCatalog selectedSecondaryArmyAssetId; // Secondary Army asset id selected from the faction queue and committed when its build interval completes.
    uint32_t secondaryArmyAssetBuildElapsedTicks; // Simulation ticks elapsed while the selected secondary Army asset is being completed.
    uint32_t secondaryArmyAssetBuildRequiredTicks; // Required build interval copied from the selected secondary Army asset record (ArmyAssetRecord.buildTicks), adjusted by fast-build mode.
    FactionArmyAssetCount completedSecondaryArmyAssetCount; // Number of completed secondary Army asset ids already stored in completedSecondaryArmyAssetIds.
    uint32_t classState70; // Class-local completion counter/state; incremented after a secondary Army asset id is stored.
    uint32_t selectedSecondaryArmyAssetValue; // Class-local value copied from the selected secondary Army asset's ArmyAssetRecord.energyLoadQ4 and accumulated in energyLoadQ4 while the build is active.
};

struct ModelRuntimeLinkedChildSpawnAndBuildView {
    Ptr32<struct ModelDefinitionLinkedChildStateView> modelDefinition; // Class-specific linked-child definition overlay.
    union ModelRuntimeNodeReferenceOrSavedOffset4 rootModelNodeOrSavedOffset; // Live model node or saved offset.
    union ArmyRuntimeReferenceOrSavedOffset ownerArmyRuntimeOrSavedOffset; // Live army runtime or saved offset.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    uint8_t classPrefixState[40]; // Unresolved common runtime state.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeLinkedChildBuildState linkedChildBuildState; // Class-local secondary-Army build and completed-slot counters.
    PckArmyAssetIdCatalog completedSecondaryArmyAssetIds[13]; // Thirteen dword slots from +0x78 through +0xAB. The definition slot-capacity bounds the reverse scan; +0xAC is independent state.
    uint32_t secondaryArmyAssetBuildState; // Two-state secondary-Army selection/build-completion state (0 idle/select, 1 building).
    uint32_t linkedChildTransitionState; // Seven-way linked-child transition/spawn state used by the switch in ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode.
    uint8_t opaqueB4_B7[4]; // Class-local dword not interpreted by this callback.
    struct ModelRuntimeLinkedChildSpawnInheritedState linkedChildSpawnInheritedState[3]; // Three contiguous inherited-state triplets for linked-child spawn slots 0, 1, and 2.
    struct ModelRuntimeLinkedChildPendingSpawnCounts linkedChildPendingSpawnCounts; // Three byte countdowns consumed before attempting linked-child spawns.
    uint32_t effectEmitterPointIndex;
    uint32_t shotEmitterTimerTicks;
    uint32_t effectEmitterTimerTicks;
    uint32_t linkedChildRuntimeFlags; // Class runtime flags tested for transition, inhibit, and build/spawn state bits.
    union ArmyRuntimeReferenceOrSavedOffset linkedArmyRuntimeOrSavedOffset;
    uint32_t energyLoadQ4; // Accumulator adjusted by the selected secondary Army asset's ArmyAssetRecord.energyLoadQ4 while active.
    uint32_t healthRegenerationDelayTicks;
    uint32_t dismantleTickCountdown;
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ModelRuntimeGroundMovementSteeringView {
    Ptr32<struct ModelDefinitionGroundMovementSteeringView> modelDefinition; // Ground-movement steering definition overlay.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live root ModelRuntimeNode.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live owning ArmyRuntimeSlot.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    struct ArmyRuntimeMovementControlState movementControl; // Model-runtime movement step and signed turn-rate state used by the ground-movement callbacks.
    uint8_t reserved18_37[32]; // Unresolved common runtime state following the recovered movement-control pair.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeClassLinkState classLinkState; // Class-specific model/army pointer-or-offset overlays and adjacent state.
    struct ModelRuntimeSlotClassState classState; // First grouped model-runtime class-state region derived from the 24x11 callback matrix.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ModelDefinitionGroundMovementSteeringView {
    AssetRecordByteCount byteSize;
    uint32_t flags;
    PckModelDefinitionIdCatalog definitionId;
    Q12 movementStepQ12PerTick; // Per-tick planar movement step; also used as the short-range movement threshold scale in the banking variant.
    ArmyTurnVelocityAngle16 turnRateLimitAnglePerTick; // Positive magnitude cap for the signed per-tick heading turn rate.
    uint8_t field5_0x14;
    uint8_t field6_0x15;
    uint8_t field7_0x16;
    uint8_t field8_0x17;
    uint8_t field9_0x18;
    uint8_t field10_0x19;
    uint8_t field11_0x1a;
    uint8_t field12_0x1b;
    ArmyTurnVelocityAngle16 turnRateAccelerationAnglePerTick; // Per-tick signed turn-rate acceleration toward the configured turn-rate limit.
    uint8_t field14_0x20;
    uint8_t field15_0x21;
    uint8_t field16_0x22;
    uint8_t field17_0x23;
    uint32_t runtimeValue24;
    uint32_t runtimeValue28;
    union ShotDefinitionReferenceOrSavedId shotDefinitionReference;
    uint8_t reserved030_047[24]; // Not yet named.
    uint32_t visibilityRadius;
    ModelRuntimeClassId runtimeClassId; // 24-way model/army runtime callback class selector; consumed by placement, grid-influence, maintenance, and class-method dispatch tables.
    uint8_t field24_0x50;
    uint8_t field25_0x51;
    uint8_t field26_0x52;
    uint8_t field27_0x53;
    Q12 placementHeightOffsetQ12; // Q12 height offset passed as the first argument to the five ArmyPlacementContact callbacks.
    union EffectDefinitionReferenceOrSavedId waterEmitterEffectDefinitionReference;
    uint8_t reserved05C_05F[4]; // Not yet named.
    uint32_t maximumHealth; /* maximum health */
    uint32_t rootNodeOffsetOrPointer;
    uint32_t modelFlags;
    uint8_t field34_0x6c;
    uint8_t field35_0x6d;
    uint8_t field36_0x6e;
    uint8_t field37_0x6f;
    uint8_t field38_0x70;
    uint8_t field39_0x71;
    uint8_t field40_0x72;
    uint8_t field41_0x73;
    uint8_t field42_0x74;
    uint8_t field43_0x75;
    uint8_t field44_0x76;
    uint8_t field45_0x77;
    SpatialSoundGainQ15 positionedSoundGainQ15; // Q15 gain passed with positionedSoundMaximumDistanceQ12 to positioned-sound playback/update helpers.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // Q12 maximum positioned-sound distance paired with positionedSoundGainQ15.
    union EffectDefinitionReferenceOrSavedId destructionEffect0;
    uint32_t destructionEffectDelayTicks0;
    union EffectDefinitionReferenceOrSavedId destructionEffect1;
    uint32_t destructionEffectDelayTicks1;
    union EffectDefinitionReferenceOrSavedId destructionEffect2;
    uint32_t destructionEffectDelayTicks2;
    union EffectDefinitionReferenceOrSavedId destructionEffect3;
    uint32_t destructionEffectDelayTicks3;
    union EffectDefinitionReferenceOrSavedId destructionEffect4;
    uint32_t destructionEffectDelayTicks4;
    union EffectDefinitionReferenceOrSavedId destructionEffect5;
    uint32_t destructionEffectDelayTicks5;
    union EffectDefinitionReferenceOrSavedId destructionEffect6;
    uint32_t destructionEffectDelayTicks6;
    union EffectDefinitionReferenceOrSavedId destructionEffect7;
    uint32_t destructionEffectDelayTicks7;
    Q12 headingErrorInterpolationDistanceQ12; // Distance below which the allowed heading error interpolates from the near-angle limit toward the far-angle limit; banking uses the same distance as its heading-hold threshold.
    AngleTurn32 farHeadingErrorLimitAngle; // Allowed heading-error angle at or beyond the interpolation distance.
    AngleTurn32 nearHeadingErrorLimitAngle; // Allowed heading-error angle at zero distance, linearly blended toward the far limit.
    uint8_t field67_0xcc;
    uint8_t field68_0xcd;
    uint8_t field69_0xce;
    uint8_t field70_0xcf;
    uint8_t field71_0xd0;
    uint8_t field72_0xd1;
    uint8_t field73_0xd2;
    uint8_t field74_0xd3;
    uint8_t field75_0xd4;
    uint8_t field76_0xd5;
    uint8_t field77_0xd6;
    uint8_t field78_0xd7;
    uint8_t field79_0xd8;
    uint8_t field80_0xd9;
    uint8_t field81_0xda;
    uint8_t field82_0xdb;
    uint32_t footprintRadius;
    uint8_t reserved0E0_167[136]; // Not yet named.
    union ShotDefinitionReferenceOrSavedId emitterShotDefinitionReference;
    uint8_t reserved16C_173[8]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId emitterEffectDefinitionReference;
    uint8_t reserved178_187[16]; // Not yet named.
    uint32_t buildEnergyLoadQ4;
    uint32_t energyLoadQ4; /* Energy demand (copied to the model runtime's energyLoadQ4) */
    union EffectDefinitionReferenceOrSavedId removalEffectDefinitionReference;
    int waterDamageMultiplier; // Signed multiplier applied to FieldGrid_InterpolateWaterDelta; arithmetic shift right by 7 produces the damage value.
    int waterDamageThreshold; // Signed interpolated-water-delta threshold; damage is evaluated only when the sampled delta is greater than this value.
    uint8_t reserved19C_19F[4]; // Not yet named.
    uint32_t footprintRadiusCopy; // Grid-derived runtime value selected through footprintRadiusClass; not an Effect definition reference.
    uint8_t reserved1A4_1A7[4]; // Not yet named.
    uint32_t placementFlags;
    uint8_t reserved1AC_1B7[12]; // Not yet named.
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex;
    uint8_t reserved1C0_253[148]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId damageEffectDefinitionReference;
    uint8_t reserved258_25F[8]; // Not yet named.
    uint32_t footprintRadiusClass;
    uint32_t terrainTraversalClass;
    uint32_t traversalSecondaryThreshold;
    uint8_t reserved26C_277[12]; // Not yet named.
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Placement-contact callback dispatch index. Observed vocabulary: 0 terrain height; 1 water-surface height; 2 terrain height+normal; 3 articulated suspension; 4 top-surface height. Kept as a 32-bit index typedef rather than enum storage.
    uint32_t occupancyMarkRadius;
};

struct ModelRuntimeTimedTargetLinkState {
    Ptr32<struct ModelRuntimeSlot> selectedTargetModelRuntime; // Eligible class-10 ModelRuntimeSlot selected from the owner-list traversal.
    Ptr32<struct ShotRuntimeSlot> matchingActiveShotRuntime; // ShotRuntimeSlot whose live ShotDefinition equals targetShotDefinition2C; non-null suppresses a new emission.
    uint32_t classState68; // Class state.
    union ModelRuntimeArmyLinkOrState armyLinkOrState; // Army pointer, saved offset, or class state.
    uint32_t classState70; // Class state.
    uint32_t classState74; // Class state.
    uint32_t classState78; // Class state.
    uint32_t classState7C; // Class state.
    uint32_t classState80; // Class state.
};

struct ModelDefinitionTimedTargetParameters {
    MdlReloadTicks reloadTicks; // MDL +0x30 reload/cadence interval added to the runtime countdown after a target is accepted and a projectile is emitted.
    uint8_t opaque34_47[20]; // Class-local MDL parameters not required by the timed-target callback; preserved without semantic assertion.
};

struct ModelDefinitionTimedTargetProjectileView {
    AssetRecordByteCount byteSize;
    uint32_t flags;
    PckModelDefinitionIdCatalog definitionId;
    uint8_t reserved00C_023[24]; // Not yet named.
    uint32_t runtimeValue24;
    uint32_t runtimeValue28;
    union ShotDefinitionReferenceOrSavedId shotDefinitionReference;
    struct ModelDefinitionTimedTargetParameters timedTargetParameters; // Class-20 timed-target projectile parameters rooted at MDL +0x30.
    uint32_t visibilityRadius;
    ModelRuntimeClassId runtimeClassId; // 24-way model/army runtime callback class selector; consumed by placement, grid-influence, maintenance, and class-method dispatch tables.
    uint8_t field10_0x50;
    uint8_t field11_0x51;
    uint8_t field12_0x52;
    uint8_t field13_0x53;
    Q12 placementHeightOffsetQ12; // Q12 height offset passed as the first argument to the five ArmyPlacementContact callbacks.
    union EffectDefinitionReferenceOrSavedId waterEmitterEffectDefinitionReference;
    uint8_t reserved05C_05F[4]; // Not yet named.
    uint32_t maximumHealth; /* maximum health */
    uint32_t rootNodeOffsetOrPointer;
    uint32_t modelFlags;
    uint8_t field20_0x6c;
    uint8_t field21_0x6d;
    uint8_t field22_0x6e;
    uint8_t field23_0x6f;
    uint8_t field24_0x70;
    uint8_t field25_0x71;
    uint8_t field26_0x72;
    uint8_t field27_0x73;
    uint8_t field28_0x74;
    uint8_t field29_0x75;
    uint8_t field30_0x76;
    uint8_t field31_0x77;
    SpatialSoundGainQ15 positionedSoundGainQ15; // Q15 gain passed with positionedSoundMaximumDistanceQ12 to positioned-sound playback/update helpers.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // Q12 maximum positioned-sound distance paired with positionedSoundGainQ15.
    union EffectDefinitionReferenceOrSavedId destructionEffect0;
    uint32_t destructionEffectDelayTicks0;
    union EffectDefinitionReferenceOrSavedId destructionEffect1;
    uint32_t destructionEffectDelayTicks1;
    union EffectDefinitionReferenceOrSavedId destructionEffect2;
    uint32_t destructionEffectDelayTicks2;
    union EffectDefinitionReferenceOrSavedId destructionEffect3;
    uint32_t destructionEffectDelayTicks3;
    union EffectDefinitionReferenceOrSavedId destructionEffect4;
    uint32_t destructionEffectDelayTicks4;
    union EffectDefinitionReferenceOrSavedId destructionEffect5;
    uint32_t destructionEffectDelayTicks5;
    union EffectDefinitionReferenceOrSavedId destructionEffect6;
    uint32_t destructionEffectDelayTicks6;
    union EffectDefinitionReferenceOrSavedId destructionEffect7;
    uint32_t destructionEffectDelayTicks7;
    uint8_t reserved0C0_0DB[28]; // Not yet named.
    uint32_t footprintRadius;
    uint8_t reserved0E0_167[136]; // Not yet named.
    union ShotDefinitionReferenceOrSavedId emitterShotDefinitionReference;
    uint8_t reserved16C_173[8]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId emitterEffectDefinitionReference;
    uint8_t reserved178_187[16]; // Not yet named.
    uint32_t buildEnergyLoadQ4;
    uint32_t energyLoadQ4; /* Energy demand (copied to the model runtime's energyLoadQ4) */
    union EffectDefinitionReferenceOrSavedId removalEffectDefinitionReference;
    uint8_t reserved194_197[4]; // Not yet named.
    uint32_t waterDamageThreshold;
    uint8_t reserved19C_19F[4]; // Not yet named.
    uint32_t footprintRadiusCopy; // Grid-derived runtime value selected through footprintRadiusClass; not an Effect definition reference.
    uint8_t reserved1A4_1A7[4]; // Not yet named.
    uint32_t placementFlags;
    uint8_t reserved1AC_1B7[12]; // Not yet named.
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex;
    uint8_t reserved1C0_253[148]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId damageEffectDefinitionReference;
    uint8_t reserved258_25F[8]; // Not yet named.
    uint32_t footprintRadiusClass;
    uint32_t terrainTraversalClass;
    uint32_t traversalSecondaryThreshold;
    uint8_t reserved26C_277[12]; // Not yet named.
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Placement-contact callback dispatch index. Observed vocabulary: 0 terrain height; 1 water-surface height; 2 terrain height+normal; 3 articulated suspension; 4 top-surface height. Kept as a 32-bit index typedef rather than enum storage.
    uint32_t occupancyMarkRadius;
};

struct ModelRuntimeTimedTargetProjectileView {
    Ptr32<struct ModelDefinitionTimedTargetProjectileView> modelDefinition; // Class-20 timed-target definition overlay.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live root ModelRuntimeNode.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live owning ArmyRuntimeSlot.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    struct ModelRuntimeTimedTargetState timedTargetState; // Timed-target reload and ShotDefinition state.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeTimedTargetLinkState timedTargetLinkState; // Traversal outputs and adjacent class-link state.
    struct ModelRuntimeSlotClassState classState; // First grouped model-runtime class-state region derived from the 24x11 callback matrix.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ModelRuntimeWeaponAimStateView {
    Ptr32<struct ArmyWeaponDefinitionView> modelDefinition; // Weapon-aim definition used by the shared smoothing and firing family.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live root ModelRuntimeNode.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live owning ArmyRuntimeSlot.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    uint8_t reserved10_13[4]; // Unresolved class-local dword before the recovered aim velocities.
    ArmyTurnVelocityAngle16 yawTurnVelocityAngle16; // Signed current yaw turn velocity maintained by ModelNodeRuntime_SmoothYawTowardTarget.
    ArmyTurnVelocityAngle16 pitchTurnVelocityAngle16; // Signed current pitch turn velocity maintained by ModelNodeRuntime_SmoothPitchTowardTarget.
    uint8_t reserved1C_1F[4]; // Unresolved class-local dword between aim velocity and firing-state fields.
    uint32_t alternatingAttachmentSequence; // Monotonic firing sequence whose low bit selects attachment 0 versus 1 in weapon-update variant B.
    WeaponAimCountdownTicks attachmentReloadCountdownTicks; // Signed countdown controlling the articulated child rotation/reload phase; decremented by simulation ticks and clamped to zero.
    WeaponAimCountdownTicks attachment0BackwardStepCountdownTicks; // Signed duration countdown for attachment-0 backward-vector stepping; decremented by simulation ticks and clamped to zero.
    WeaponAimCountdownTicks attachment1BackwardStepCountdownTicks; // Signed duration countdown for attachment-1 backward-vector stepping in variant B; decremented by simulation ticks and clamped to zero.
    uint8_t reserved30_37[8]; // Unresolved tail of the class-local aim/firing state prefix.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    uint32_t attachmentReloadTicks[8]; // Eight class-9 attachment countdowns at +0x60..+0x7C.
    uint32_t sharedInterShotTicks; // Class-9 shared inter-shot countdown at +0x80.
    struct ModelRuntimeSlotClassState classState; // First grouped model-runtime class-state region derived from the 24x11 callback matrix.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ModelRuntimeVerticalDeploymentLinkState {
    Q12 deploymentTravelQ12; // Signed accumulated deployment travel. Raw code compares it with deploymentTravelLimitQ12 and adds/subtracts verticalDeploymentStepQ12PerTick times simulation ticks in lockstep with child local-Z.
    uint32_t collisionRetryCountdown; // Per-tick countdown. When it reaches zero, the class retries the linked-runtime collision test and reloads this field with 8 ticks.
    uint32_t classState68; // Class state.
    union ModelRuntimeArmyLinkOrState armyLinkOrState; // Army pointer, saved offset, or class state.
    uint32_t classState70; // Class state.
    uint32_t classState74; // Class state.
    uint32_t classState78; // Class state.
    uint32_t classState7C; // Class state.
    uint32_t classState80; // Class state.
};

struct ModelRuntimeVerticalDeploymentView {
    Ptr32<struct ModelDefinitionVerticalDeploymentView> modelDefinition; // Class-23 vertical-deployment definition overlay.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live root ModelRuntimeNode.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live owning ArmyRuntimeSlot.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    uint8_t classPrefixState[40]; // Unresolved common runtime state.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeVerticalDeploymentLinkState deploymentState; // Class-23 vertical-deployment scalar travel and collision retry state.
    struct ModelRuntimeSlotClassState classState; // First grouped model-runtime class-state region derived from the 24x11 callback matrix.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ModelRuntimeClass21State {
    Q12 trajectoryTerrainReferenceHeightQ12; // Terrain-relative trajectory reference height. Computed from the maximum sampled top-surface height plus 0x800 Q12 and twice the saved child local-Z; consumed by the class-21 vertical arc state.
    uint8_t field1_0x4;
    uint8_t field2_0x5;
    uint8_t field3_0x6;
    uint8_t field4_0x7;
    uint8_t field5_0x8;
    uint8_t field6_0x9;
    uint8_t field7_0xa;
    uint8_t field8_0xb;
    uint8_t field9_0xc;
    uint8_t field10_0xd;
    uint8_t field11_0xe;
    uint8_t field12_0xf;
    uint8_t field13_0x10;
    uint8_t field14_0x11;
    uint8_t field15_0x12;
    uint8_t field16_0x13;
    uint8_t field17_0x14;
    uint8_t field18_0x15;
    uint8_t field19_0x16;
    uint8_t field20_0x17;
    uint8_t field21_0x18;
    uint8_t field22_0x19;
    uint8_t field23_0x1a;
    uint8_t field24_0x1b;
    uint8_t field25_0x1c;
    uint8_t field26_0x1d;
    uint8_t field27_0x1e;
    uint8_t field28_0x1f;
    uint8_t field29_0x20;
    uint8_t field30_0x21;
    uint8_t field31_0x22;
    uint8_t field32_0x23;
    uint32_t classStateA8; // Class-discriminated runtime state.
    uint32_t classStateAC; // Class-discriminated runtime state.
    int32_t classStateB0; // Class-discriminated signed runtime state.
    int32_t classStateB4; // Class-discriminated signed runtime state.
    uint32_t behaviorState; // Class-discriminated runtime flags or value.
    uint8_t reservedBC_BF[4]; // Unresolved class-specific bytes BC-BF.
    uint32_t classStateC0; // Class-discriminated dword at ModelRuntimeSlot +0xC0; verified selector input in class-14 placement/runtime paths.
    uint8_t reservedC4_C7[4]; // Unresolved class-specific bytes C4-C7.
    uint32_t classStateC8; // Class-discriminated dword at ModelRuntimeSlot +0xC8; verified packed runtime contribution in class-14 update path.
    uint8_t reservedCC_CF[4]; // Unresolved class-specific bytes CC-CF.
    int32_t classStateD0; // Class-discriminated signed runtime state.
    uint8_t reservedD4_DB[8]; // Unresolved class-specific state.
    uint32_t classStateDC; // Class-discriminated runtime state.
    uint32_t effectEmitterPointIndex; // Next model effect point of the timed effect emitter (ArmyRuntime_UpdateTimedShotAndEffectEmitters).
    uint32_t shotEmitterTimerTicks; // Timed shot emitter countdown; constructor sets 1.
    uint32_t effectEmitterTimerTicks; // Timed effect emitter countdown; constructor sets 1, 0x7FFFFFFF = never.
    uint32_t stateFlags; // ARMY_MODEL_STATE_* bits (gameplay/army/pool.h); constructor-cleared.
    union ArmyRuntimeReferenceOrSavedOffset linkedArmyRuntimeOrSavedOffset; // Live army pointer or serialized pool offset.
    uint32_t energyLoadQ4; // Energy demand: the definition's energyLoadQ4 plus loads held while building/researching.
    uint32_t healthRegenerationDelayTicks; // Counts down to the next health step; damage sets it to 0x200.
    uint32_t dismantleTickCountdown; // 12-tick period of the Xenite refund while dismantling.
};

struct ModelDefinitionArticulatedMovementView {
    AssetRecordByteCount byteSize;
    uint32_t flags;
    PckModelDefinitionIdCatalog definitionId;
    uint8_t field3_0xc;
    uint8_t field4_0xd;
    uint8_t field5_0xe;
    uint8_t field6_0xf;
    uint8_t field7_0x10;
    uint8_t field8_0x11;
    uint8_t field9_0x12;
    uint8_t field10_0x13;
    uint8_t field11_0x14;
    uint8_t field12_0x15;
    uint8_t field13_0x16;
    uint8_t field14_0x17;
    Q12 movementAdvanceDeltaQ12PerTick; // Signed amount added to or subtracted from movementAdvancePerTickQ12 while articulated contact transition bits are active.
    uint8_t field16_0x1c;
    uint8_t field17_0x1d;
    uint8_t field18_0x1e;
    uint8_t field19_0x1f;
    uint8_t field20_0x20;
    uint8_t field21_0x21;
    uint8_t field22_0x22;
    uint8_t field23_0x23;
    uint32_t runtimeValue24;
    uint32_t runtimeValue28;
    union ShotDefinitionReferenceOrSavedId shotDefinitionReference;
    uint8_t reserved030_047[24]; // Not yet named.
    uint32_t visibilityRadius;
    ModelRuntimeClassId runtimeClassId; // 24-way model/army runtime callback class selector; consumed by placement, grid-influence, maintenance, and class-method dispatch tables.
    uint8_t field30_0x50;
    uint8_t field31_0x51;
    uint8_t field32_0x52;
    uint8_t field33_0x53;
    Q12 placementHeightOffsetQ12; // Q12 height offset passed as the first argument to the five ArmyPlacementContact callbacks.
    union EffectDefinitionReferenceOrSavedId waterEmitterEffectDefinitionReference;
    uint8_t reserved05C_05F[4]; // Not yet named.
    uint32_t maximumHealth; /* maximum health */
    uint32_t rootNodeOffsetOrPointer;
    uint32_t modelFlags;
    uint8_t field40_0x6c;
    uint8_t field41_0x6d;
    uint8_t field42_0x6e;
    uint8_t field43_0x6f;
    uint8_t field44_0x70;
    uint8_t field45_0x71;
    uint8_t field46_0x72;
    uint8_t field47_0x73;
    uint8_t field48_0x74;
    uint8_t field49_0x75;
    uint8_t field50_0x76;
    uint8_t field51_0x77;
    SpatialSoundGainQ15 positionedSoundGainQ15; // Q15 gain passed with positionedSoundMaximumDistanceQ12 to positioned-sound playback/update helpers.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // Q12 maximum positioned-sound distance paired with positionedSoundGainQ15.
    union EffectDefinitionReferenceOrSavedId destructionEffect0;
    uint32_t destructionEffectDelayTicks0;
    union EffectDefinitionReferenceOrSavedId destructionEffect1;
    uint32_t destructionEffectDelayTicks1;
    union EffectDefinitionReferenceOrSavedId destructionEffect2;
    uint32_t destructionEffectDelayTicks2;
    union EffectDefinitionReferenceOrSavedId destructionEffect3;
    uint32_t destructionEffectDelayTicks3;
    union EffectDefinitionReferenceOrSavedId destructionEffect4;
    uint32_t destructionEffectDelayTicks4;
    union EffectDefinitionReferenceOrSavedId destructionEffect5;
    uint32_t destructionEffectDelayTicks5;
    union EffectDefinitionReferenceOrSavedId destructionEffect6;
    uint32_t destructionEffectDelayTicks6;
    union EffectDefinitionReferenceOrSavedId destructionEffect7;
    uint32_t destructionEffectDelayTicks7;
    uint8_t reserved0C0_0DB[28]; // Not yet named.
    uint32_t footprintRadius;
    uint8_t reserved0E0_167[136]; // Not yet named.
    union ShotDefinitionReferenceOrSavedId emitterShotDefinitionReference;
    uint8_t reserved16C_173[8]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId emitterEffectDefinitionReference;
    uint8_t reserved178_187[16]; // Not yet named.
    uint32_t buildEnergyLoadQ4;
    uint32_t energyLoadQ4; /* Energy demand (copied to the model runtime's energyLoadQ4) */
    union EffectDefinitionReferenceOrSavedId removalEffectDefinitionReference;
    int waterDamageMultiplier; // Signed multiplier applied to FieldGrid_InterpolateWaterDelta; arithmetic shift right by 7 produces the damage value.
    int waterDamageThreshold; // Signed interpolated-water-delta threshold; damage is evaluated only when the sampled delta is greater than this value.
    uint8_t reserved19C_19F[4]; // Not yet named.
    uint32_t footprintRadiusCopy; // Grid-derived runtime value selected through footprintRadiusClass; not an Effect definition reference.
    uint8_t reserved1A4_1A7[4]; // Not yet named.
    uint32_t placementFlags;
    uint8_t reserved1AC_1B7[12]; // Not yet named.
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex;
    uint8_t reserved1C0_253[148]; // Not yet named.
    union EffectDefinitionReferenceOrSavedId damageEffectDefinitionReference;
    uint8_t reserved258_25F[8]; // Not yet named.
    uint32_t footprintRadiusClass;
    uint32_t terrainTraversalClass;
    uint32_t traversalSecondaryThreshold;
    uint8_t reserved26C_277[12]; // Not yet named.
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Placement-contact callback dispatch index. Observed vocabulary: 0 terrain height; 1 water-surface height; 2 terrain height+normal; 3 articulated suspension; 4 top-surface height. Kept as a 32-bit index typedef rather than enum storage.
    uint32_t occupancyMarkRadius;
};

struct ModelRuntimeClass21UpdateView {
    Ptr32<struct ArmyRuntimeClassUpdate21DefinitionView> modelDefinition; // Class-21 update definition overlay with recovered ballistic/phase/model-point fields.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live root ModelRuntimeNode.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live owning ArmyRuntimeSlot.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    uint8_t classPrefixState[40]; // Unresolved common runtime state.
    union ModelRuntimeSlotReferenceOrSavedOffset linkedModelRuntimeOrSavedOffset; // Live model runtime or saved offset.
    uint32_t health; // Current health; starts at ModelDefinition.maximumHealth.
    uint32_t destructionEffectTimers[8]; // destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeClassLinkState classLinkState; // Class-specific model/army pointer-or-offset overlays and adjacent state.
    struct ModelRuntimeClass21State class21State; // Class-21 trajectory/state-machine fields.
    uint32_t researchTechnologyId; /* see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; // Constructor-cleared class state.
    uint32_t classState11C; // Constructor-cleared class state.
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};

struct ModelRuntimeArticulatedMovementDefinitionView {
    Ptr32<struct ModelDefinitionArticulatedMovementView> modelDefinition; // Articulated-movement definition overlay with movement-advance and water-damage parameters.
    Ptr32<struct ModelRuntimeNode> rootModelNode; // Live root ModelRuntimeNode.
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; // Live owning ArmyRuntimeSlot.
    uint32_t attachmentCount; // Valid attachment descriptors in attachments.
    struct ArmyRuntimeMovementControlState movementControl; // Typed per-tick movement advance and signed turn-velocity state used by runtime-update and projected-sound callbacks.
    ArmyMovementStateFlags movementStateFlags;
    Ptr32<struct ArmyRuntimeSlot> commandTargetArmyRuntime;
    Q12 commandCoordinate0Q12;
    Q12 commandCoordinate1Q12;
    Q12 commandCoordinate2Q12;
    ArmyCommandModeFlags commandModeFlags;
    ArmyCommandGeneration commandGeneration;
    Q12 actionVector0Q12;
    Q12 actionVector1Q12;
    Q12 actionVector2Q12;
    uint32_t runtimeState40;
    uint32_t runtimeState44;
    uint32_t runtimeState48;
    uint32_t runtimeState4C;
    uint8_t reserved50_57[8];
    Q12 movementPosition0Q12;
    Q12 movementPosition1Q12;
    uint32_t stepStartHeading; // Class-3 walker: body heading at step start
    uint32_t stepEndHeading; // Class-3 walker: body heading at step end
    uint32_t leftFootGroundNormal; // Class-3 walker: left foot ground normal now (packed elevation << 16 | azimuth); target in fallbackWorldYQ12
    Ptr32<struct ArmyRuntimeSlot> linkedArmyRuntimeOrSavedOffset;
    Q12 fallbackWorldYQ12;
    Q12 fallbackWorldXQ12;
    Q12 movementTarget0Q12;
    Q12 movementTarget1Q12;
    uint32_t leftFootYQ12; // Class-3 walker: left foot Y (X is movementTarget0Q12)
    uint32_t rightFootYQ12; // Class-3 walker: right foot Y (X is movementTarget1Q12)
    uint32_t leftFootZQ12; // Class-3 walker: left foot Z
    uint32_t rightFootZQ12; // Class-3 walker: right foot Z
    uint32_t leftStepTargetXQ12; // Class-3 walker: left foot step target X
    uint32_t rightStepTargetXQ12; // Class-3 walker: right foot step target X
    uint32_t leftStepTargetYQ12; // Class-3 walker: left foot step target Y
    int rightStepTargetYQ12; // Class-3 walker: right foot step target Y
    int leftStepTargetZQ12; // Class-3 walker: left foot step target Z
    uint32_t rightStepTargetZQ12; // Class-3 walker: right foot step target Z
    uint32_t leftStepProgressQ12; // Class-3 walker: left step progress (0x1000 = step done); the right one is articulatedContact.terrainContactMode
    struct ArmyRuntimeArticulatedContactState articulatedContact; // Terrain-contact and articulated movement state.
    struct ArmyRuntimeLinkedChildOverloadedState linkedChildOverloadedState; // Mixed coordinate, command, heading, spawn, and state-history overlay.
    struct ArmyRuntimeLinkedChildSpawnParameters linkedChildSpawnParameters; // Third linked-child spawn parameter triplet.
    struct ArmyRuntimeLinkedChildPendingCounts linkedChildPendingCounts; // Three independently decremented pending child counters.
    uint8_t reservedE0_EB[12];
    ArmyRuntimeFlags runtimeFlags;
    Ptr32<struct ModelRuntimeSlot> linkedModelRuntime; // ModelRuntimeSlotClassState.linkedArmyRuntimeOrSavedOffset: the linked model runtime (e.g. a class-23 platform the walker docked on).
    ArmyRuntimeTimer runtimeTimer;
    uint8_t reservedF8_FF[8];
    int stateOrTechnologyId;
    uint32_t runtimeState104;
    ArmySelectionMetric selectionMetric0;
    ArmySelectionMetric selectionMetric1;
    ArmySelectionMetric selectionMetric2;
    ArmySelectionMetric selectionMetric3;
    ArmySelectionMetric selectionMetric4;
    ArmySelectionMetric selectionMetric5;
    uint8_t reserved120_13F[32]; // Unresolved state before fixed attachment descriptors.
    struct ModelRuntimeAttachmentDescriptor attachments[6]; // Six fixed 0x20-byte attachment descriptors.
};
using TerrainClassOverlayCallback = Bool8 (uint32_t cellFlagMask, int cellValue, int radiusWorldUnits, Q12 worldYQ12, Q12 worldXQ12, FieldGridAsset * fieldGrid);

#endif /* THANDOR_GAMEPLAY_ARMY_TYPES_H */
