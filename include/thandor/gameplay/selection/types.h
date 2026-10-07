/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/selection/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SELECTION_TYPES_H
#define THANDOR_GAMEPLAY_SELECTION_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>

struct SelectionInfoEntitySlots;
struct SelectionPlayerPairRecord;
struct SelectionPointerArray32;
struct SelectionPlayerRuntimeBlock;
struct GameEntityRuntime;

using SelectionMarkerCoordinateValue32 = uint32_t;

using SelectionMarkerIndex = int;

using SelectionMarkerLaneMask = uint32_t;

struct SelectionInfoEntitySlots {
    Ptr32<struct GameEntityRuntime> entries[32]; 
};

struct SelectionPlayerPairRecord {
    uint32_t pairKey; 
    uint32_t pairValue; 
};

struct SelectionPointerArray32 {
    Ptr32<struct GameEntityRuntime> entries[32]; 
};

struct SelectionPlayerRuntimeBlock {
    struct SelectionPointerArray32 selection; 
    struct SelectionPlayerPairRecord markedCells[4096]; // field cells marked with the editor region tool (PlayerPairList_InsertUnique).
    uint32_t factionIndex; // the player's faction (FactionRuntimeIndex).
    uint32_t markedCellCount;
    Ptr32<int> terrainHeightScratchPlane;
    Ptr32<uint32_t> terrainMaterialEditPlane;
    uint32_t placementFactionIndex; // faction of the next army placed in the editor (PlayerRuntime_SetPlacementFaction).
    uint32_t placedArmyToken; // army placed/picked in the editor, as an offset from g_ArmyRuntimeRebaseBaseMinusOne; moved and turned by the placement commands.
    UPtr32 pendingPlacementArmyAsset; // ArmyAssetRecordPrefix * taken from the faction's army stock for placement, 0 when none.
    uint32_t chatRecipientMaskAndWriteOffset; // chat recipient mask (bits 8+faction, 16+player) and, in the low byte, the write offset in chatStagingText.
    UPtr32 technologyPageBuilding; // building (a ModelRuntimeSlot) whose technology page the player has open.
    uint32_t heldResearchUnpaidFlag; // its ARMY_MODEL_STATE_RESEARCH_UNPAID bit, taken away while the page is open.
    uint32_t sessionFlags;
    InGameSimulationStepBatchTicks simulationStepTicks; 
    union {
        uint8_t reserved80B0_8117[104];
        struct {
            uint8_t reserved80B0_80BF[16];
            uint8_t chatStagingText[48]; /* chat line being received (INGAME_COMMAND_CHAT_APPEND), 8-bit text */
            uint16_t playerNameUtf16[20]; /* the player's name (chat sender) */
        };
    };
};

using SelectionPlayerPairKey = uint32_t;

#endif /* THANDOR_GAMEPLAY_SELECTION_TYPES_H */
