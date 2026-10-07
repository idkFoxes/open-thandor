/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/faction/economy.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_FACTION_ECONOMY_H
#define THANDOR_GAMEPLAY_FACTION_ECONOMY_H

#include <thandor/core/contracts.h>

/* Faction statistics table sampling (InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState):
   row simulationTick >> 7 is written while these tick bits are clear (8 of every 128 steps) */
inline constexpr int INGAME_STAT_SAMPLE_TICK_MASK = 0x78;

/* FieldGridCell.resourceExtractionDescriptor (ArmyRuntime claim, collected into g_TerrainRegionCollectionEntries):
   support bit | faction << 13 | claimedCellTag (share) << 24 */
inline constexpr int RESOURCE_EXTRACTION_FACTION_SHIFT = 13;
inline constexpr int RESOURCE_EXTRACTION_FACTION_MASK = 0x7FF; /* after the shift: bits 13..23 */
inline constexpr int RESOURCE_EXTRACTION_SHARE_SHIFT = 24;

void InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState();

#endif /* THANDOR_GAMEPLAY_FACTION_ECONOMY_H */
