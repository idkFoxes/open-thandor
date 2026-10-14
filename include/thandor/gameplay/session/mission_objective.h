/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/mission_objective.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_GAMEPLAY_SESSION_MISSION_OBJECTIVE_H
#define THANDOR_GAMEPLAY_SESSION_MISSION_OBJECTIVE_H

/* open-thandor (objective overlay, docs/plans/objective_overlay.md 3): the objective of the local faction in the
   running level, for the overlay's two lines. Pure display: nothing here writes game state, draws random numbers or
   allocates from the arena; the live values are read from the counters the level script evaluates
   (level_script.cpp InGameScheduledCondition_Holds). */

#include <cstddef>
#include <cstdint>
#include <thandor/assets/text/types.h>
#include <thandor/gameplay/session/types.h>

/* code units of MissionObjective.line1 and of a sensible progress buffer, terminator included */
inline constexpr int MISSION_OBJECTIVE_LINE_UNITS = 96;

/* Where line 1 came from. */
enum class MissionObjectiveSource : uint8_t {
    NONE = 0,       /* no victory trigger for the local faction: the overlay stays hidden (plan 3c) */
    SHORT_TEXT = 1, /* the hand-written short objective (TextResource_MissionObjective, plan 3a) */
    GENERATED = 2   /* generated from the victory trigger's condition (plan 3b) */
};

/* What line 2 shows (MissionObjective_FormatProgress); the operands are the raw level-script operands. */
enum class MissionObjectiveProgressKind : uint8_t {
    NONE = 0,
    ENEMY_UNITS = 1,     /* models of the factions in factionMask ("Gegnerische Einheiten: 37"); faction is the
                            first of them */
    ASSET_REMAINING = 2, /* models of army asset armyAssetId owned by faction ("Verbleibend: 2") */
    XENITE = 3,          /* Xenite of faction / target (Q4, shown in whole units) */
    TRITIUM = 4,         /* Tritium of faction / target (Q4) */
    TRITIUM_RATE = 5,    /* Tritium extraction rate of faction / target (Q4 per economy run, shown per minute) */
    ASSET_COUNT = 6,     /* models of armyAssetId owned by faction / target ("2 / 5") */
    TERRAIN_PERCENT = 7, /* occupied cells in % (occupancy byte cellByteOffset) / target % */
    COUNTDOWN = 8        /* remaining seconds of condition conditionIndex (operand 1, read live; "Noch 4:30") */
};

struct MissionObjectiveProgress {
    MissionObjectiveProgressKind kind = MissionObjectiveProgressKind::NONE;
    uint8_t conditionIndex = 0; /* the condition record the values come from */
    uint8_t factionMask = 0;    /* ENEMY_UNITS: bit f = faction f */
    uint32_t faction = 0;
    uint32_t armyAssetId = 0;
    uint32_t target = 0;
    uint32_t cellByteOffset = 0;
};

struct MissionObjective {
    MissionObjectiveSource source = MissionObjectiveSource::NONE;
    uint32_t localFaction = 0;
    uint8_t triggerIndex = 0;  /* the first active victory trigger of the local faction */
    uint32_t moreTriggers = 0; /* further victory triggers of the local faction with another condition; a generated
                                  line 1 already ends in "(+n weitere)" */
    TextResourceId shortTextId = 0xFFFFFFFF; /* SHORT_TEXT: the id line 1 was resolved from, else TEXT_RESOURCE_ID_NONE */
    uint16_t line1[MISSION_OBJECTIVE_LINE_UNITS] = {}; /* plain UTF-16 (no rich-text commands), terminated; empty for
                                                          NONE. Not cut to the overlay width (the overlay does that) */
    MissionObjectiveProgress progress;               /* the first measurable part of the victory condition */
    const InGameLevelConditionStorage *storage = nullptr; /* the storage Build read; FormatProgress reads it again */
};

/* Builds the objective of localFaction (1..7) from levelConditionStorage (the running level's
   g_InGameLevelRuntimeGlobalBlock.conditionStorage, or a copied LEV prefix): the first active end trigger with
   movie variant 1 (victory) of that faction, as the campaign map reads them. Line 1 is the hand-written short
   objective of the level when TextResource_MissionObjective has one, else generated from the trigger's condition
   (plan 3b; a BOOLEAN_POSTFIX_EXPRESSION gives "A und B" / "A oder B" or "Mehrere Ziele (F1)"). Texts follow the
   project text language at the time of the call (build again after a language switch); unit type and faction names
   come from the loaded model and faction colour texts, with "Einheiten" / the faction number when those are not
   available. Read-only: no game state is written. source NONE (line 1 empty) when there is no such trigger or the
   storage is nullptr. */
MissionObjective MissionObjective_Build(const InGameLevelConditionStorage *levelConditionStorage,uint32_t localFaction);

/* Line 2 of the objective (plain UTF-16, terminated, cut to capacity code units) from the live values: faction
   resources (g_GameFactionRuntimeImage), model counts over the world owner list, the occupied share of the field
   grid, the countdown's remaining operand. Read-only. Returns false (out empty) when the objective has no progress
   kind, or the values need the running world and there is none (g_InGameRuntimeRoot is nullptr). */
bool MissionObjective_FormatProgress(const MissionObjective *objective,uint16_t *out,size_t capacity);

#endif /* THANDOR_GAMEPLAY_SESSION_MISSION_OBJECTIVE_H */
