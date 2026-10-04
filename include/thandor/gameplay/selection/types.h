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

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct SelectionInfoEntitySlots SelectionInfoEntitySlots, *PSelectionInfoEntitySlots;
typedef struct SelectionPlayerPairRecord SelectionPlayerPairRecord, *PSelectionPlayerPairRecord;
typedef struct SelectionPointerArray32 SelectionPointerArray32, *PSelectionPointerArray32;
typedef struct SelectionPlayerRuntimeBlock SelectionPlayerRuntimeBlock, *PSelectionPlayerRuntimeBlock;
typedef struct GameEntityRuntime GameEntityRuntime;

typedef uint32_t SelectionMarkerCoordinateValue32;

typedef int SelectionMarkerIndex;

typedef uint32_t SelectionMarkerLaneMask;

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
    struct SelectionPlayerPairRecord markedCells[4096]; // +0x80: field cells marked with the editor region tool (PlayerPairList_InsertUnique).
    uint32_t factionIndex; // +0x8080: the player's faction (FactionRuntimeIndex).
    uint32_t markedCellCount;
    Ptr32<int> terrainHeightScratchPlane;
    Ptr32<uint32_t> terrainMaterialEditPlane;
    uint32_t placementFactionIndex; // +0x8090: faction of the next army placed in the editor (PlayerRuntime_SetPlacementFaction).
    uint32_t placedArmyToken; // +0x8094: army placed/picked in the editor, as an offset from g_ArmyRuntimeRebaseBaseMinusOne; moved and turned by the placement commands.
    UPtr32 pendingPlacementArmyAsset; // +0x8098: ArmyAssetRecordPrefix * taken from the faction's army stock for placement, 0 when none.
    uint32_t chatRecipientMaskAndWriteOffset; // +0x809C: chat recipient mask (bits 8+faction, 16+player) and, in the low byte, the write offset in chatStagingText.
    UPtr32 technologyPageBuilding; // +0x80A0: building (ModelRuntimeSlot *) whose technology page the player has open.
    uint32_t heldResearchUnpaidFlag; // +0x80A4: its ARMY_MODEL_STATE_RESEARCH_UNPAID bit, taken away while the page is open.
    uint32_t sessionFlags;
    InGameSimulationStepBatchTicks simulationStepTicks; 
    union {
        uint8_t reserved80B0_8117[104];
        struct {
            uint8_t reserved80B0_80BF[16];
            uint8_t chatStagingText[48]; /* +0x80C0 chat line being received (INGAME_COMMAND_CHAT_APPEND), 8-bit text */
            uint16_t playerNameUtf16[20]; /* +0x80F0 the player's name (chat sender) */
        };
    };
};

typedef uint32_t SelectionPlayerPairKey;

#endif /* THANDOR_GAMEPLAY_SELECTION_TYPES_H */
