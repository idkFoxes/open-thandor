/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/faction/carryover.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_FACTION_CARRYOVER_H
#define THANDOR_GAMEPLAY_FACTION_CARRYOVER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/faction/carryover. */

/* g_OldUnitPrimaryTable: 0x20-byte carry-over unit records (OLD_UNIT_PRIMARY_TABLE_BYTES / 0x20). */
#define OLD_UNIT_PRIMARY_RECORD_CAPACITY 0x200

/* Functions are grouped by semantic ownership. */

void OldUnitRuntime_RebuildScenarioReplayTables(void);

void OldUnitRuntime_MergeMasksAndReplayRecords(void);

void OldUnitRuntime_ResetPendingTables(void);

extern uint32_t *g_OldUnitSecondaryTable;
extern uint32_t *g_OldUnitPrimaryTable;
extern OldUnitRecordCount g_OldUnitRecordCount; /* followed by 8 bytes 0x90 fill (dropped) */

#endif /* THANDOR_GAMEPLAY_FACTION_CARRYOVER_H */
