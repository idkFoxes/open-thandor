/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_DEBUG_TYPES_H
#define THANDOR_PLATFORM_DEBUG_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/gameplay/session/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

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

#endif /* THANDOR_PLATFORM_DEBUG_TYPES_H */
