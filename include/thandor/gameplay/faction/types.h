/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/faction/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_FACTION_TYPES_H
#define THANDOR_GAMEPLAY_FACTION_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/army/types.h>
#include <thandor/core/types.h>

typedef struct AiCandidateWorkspaceEntry AiCandidateWorkspaceEntry, *PAiCandidateWorkspaceEntry;
typedef struct GameFactionRuntimeRecord GameFactionRuntimeRecord, *PGameFactionRuntimeRecord;
typedef struct AiFactionCandidateCacheState AiFactionCandidateCacheState, *PAiFactionCandidateCacheState;
typedef struct GameFactionRuntimeImage GameFactionRuntimeImage, *PGameFactionRuntimeImage;
typedef struct GameFactionRuntimeImageTail GameFactionRuntimeImageTail, *PGameFactionRuntimeImageTail;
typedef struct ArmyRuntimeSlot ArmyRuntimeSlot;

using GameRelationUiFlags = uint32_t;

enum /* FactionRuntimeLifecycleObservedState, stored in 1 byte(s) */ {
    FACTION_RUNTIME_LIFECYCLE_INACTIVE=0, /* slot unused, or absorbed by a merge (state 11 relation) */
    FACTION_RUNTIME_LIFECYCLE_ACTIVE=1,
    FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING=2,
    FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED=3
};
using FactionRuntimeLifecycleObservedState = uint8_t;

enum {
    FACTION_RELATION_MERGE=11
};
using FactionRelationState = int;

using FactionRuntimeFlags = uint32_t;

using TerrainExploredPercent = uint32_t;

using FactionActiveMask = uint32_t;

using FactionNotificationCodeBase = uint32_t;

using FactionPackedRelationStates = uint32_t;

using FactionRelationCounter = int;

using FactionAnchorCooldownTicks = uint32_t;

using FactionProgressScore = int;

using FactionArmyAssetCount = uint32_t;

using FactionAiPressureScore = int;

using FactionCapabilityFlags = uint32_t;

using FactionRelationTick = uint32_t;

using FactionRelationCapabilityState = uint32_t;

using FactionResourceScoreComponent = int;

using FactionTechnologyCount = uint32_t;

using FactionContributionScaleQ8 = uint32_t;

using FactionRelationStateNibble = uint32_t;

using XeniteAmountQ4 = uint32_t;

using TritiumAmountQ4 = uint32_t;

struct AiCandidateWorkspaceEntry {
    uint32_t weightedScoreAndKind; 
    uint32_t entityIdAndMultiplicity; 
};

using EnergyAmountQ4 = uint32_t;

using ResourceExtractionRateQ4PerTick = uint32_t;

struct AiFactionCandidateCacheState {
    struct AiCandidateWorkspaceEntry savedEntries[3]; 
    uint32_t savedEntryCount; 
    uint32_t cacheReuseState; 
};

struct GameFactionRuntimeRecord {
    XeniteAmountQ4 xeniteCurrentQ4; 
    XeniteAmountQ4 xeniteStorageLimitQ4; 
    ResourceExtractionRateQ4PerTick xeniteExtractionRateQ4PerTick; 
    XeniteAmountQ4 xeniteExtractedTotalQ4; 
    TritiumAmountQ4 tritiumCurrentQ4; 
    TritiumAmountQ4 tritiumStorageLimitQ4; 
    ResourceExtractionRateQ4PerTick tritiumExtractionRateQ4PerTick; 
    TritiumAmountQ4 tritiumExtractedTotalQ4; 
    EnergyAmountQ4 baselineEnergySupplyQ4; 
    EnergyAmountQ4 energyGenerationCapacityQ4; 
    EnergyDemandQ4 suppliedEnergyDemandQ4; 
    EnergyDemandQ4 unpoweredEnergyDemandQ4; 
    FactionArmyAssetCount primaryArmyAssetCount; 
    FactionArmyAssetCount secondaryArmyAssetCount; 
    uint32_t colorIndex; // Faction colour: colour name text TEXT_ID_FACTION_NAME_BASE + colorIndex, graphics suffix of the faction's palette/sprites, minimap panel colour variant. Set per level as slot colour offset + faction index.
    FactionCapabilityFlags capabilityFlags; 
    FactionPackedRelationStates packedRelationStates; 
    FactionRelationCapabilityState relationCapabilityState; 
    FactionContributionScaleQ8 terrainContributionScaleQ8; 
    FactionRelationTick relationTransitionTick; 
    GraphicsWorldCoordinateQ12 primaryAnchorYQ12; 
    GraphicsWorldCoordinateQ12 primaryAnchorXQ12; 
    FactionAnchorCooldownTicks primaryAnchorCooldown; 
    GraphicsWorldCoordinateQ12 secondaryAnchorYQ12; 
    GraphicsWorldCoordinateQ12 secondaryAnchorXQ12; 
    FactionAnchorCooldownTicks anchorCooldown0; 
    FactionAnchorCooldownTicks anchorCooldown1; 
    FactionAnchorCooldownTicks anchorCooldown2; 
    FactionRuntimeFlags runtimeFlags; 
    FactionAiPressureScore maximumAiPressure; 
    uint8_t reserved78_87[16]; 
    FactionProgressScore combinedProgressScore; 
    FactionProgressScore activeArmyContribution; 
    FactionProgressScore economyProgressScore; 
    FactionProgressScore relationScore; 
    TerrainExploredPercent exploredTerrainPercent; 
    FactionTechnologyCount unlockedTechnologyCountBeyondBaseline; 
    FactionResourceScoreComponent primaryResourceComponent; 
    FactionResourceScoreComponent secondaryResourceComponent; 
    FactionRelationCounter relationCounterA; 
    FactionRelationCounter relationCounterB; 
    FactionRelationCounter relationCounterC; 
    FactionRelationCounter relationCounterD; 
    FactionRelationCounter relationCounterE; 
    FactionRelationCounter relationCounterF; 
    struct AiFactionCandidateCacheState candidateCache; 
    uint32_t secondaryArmyAssetPointersOrIds[64]; 
    uint32_t primaryArmyAssetPointersOrIds[64]; 
    Ptr32<struct ArmyRuntimeSlot> runtimeGroupMembers8x32[256]; 
    uint32_t technologyMasks256Bits[8]; 
    uint32_t relationStateTicks[8]; 
    int32_t aiPressureValues[8]; 
};

using InGameSimulationTick = uint32_t;

using InGamePresentationTick = uint32_t;

using InGamePeriodicClockTick = uint32_t;

using GameSpeedQ8 = uint32_t;

struct GameFactionRuntimeImageTail {
    uint32_t activeFactionCount; // Total active faction count copied from the loaded level runtime tail; faction loops and owner-faction UI cycling use it as their upper index/count.
    FactionRuntimeLifecycleObservedState factionLifecycleStates[8]; // Observed states: 1 active, 2 ending pending, 3 ended/transitioned. Zero remains deliberately unnamed.
    InGameSimulationTick simulationTick; // Monotonic simulation tick used for phased maintenance scheduling.
    InGamePresentationTick presentationTick; // Presentation/update cadence counter used by end-game HUD and terrain-composite refresh scheduling.
    InGamePeriodicClockTick periodicClockTick; // Periodic real-time clock tick used by the displayed elapsed-time conversion.
    GameSpeedQ8 gameSpeedQ8; // Runtime game-speed multiplier converted from the frontend percentage into unsigned Q8 form.
    GameRelationUiFlags relationUiFlags; // Relation and UI visibility flags loaded from the level-condition runtime.
};

struct GameFactionRuntimeImage {
    struct GameFactionRuntimeRecord records[8]; 
    struct GameFactionRuntimeImageTail tail; 
};

using SelectionPlayerPairValue = uint32_t;

#endif /* THANDOR_GAMEPLAY_FACTION_TYPES_H */
