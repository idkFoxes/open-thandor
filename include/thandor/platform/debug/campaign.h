/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/campaign.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_CAMPAIGN_H
#define THANDOR_PLATFORM_DEBUG_CAMPAIGN_H

#include <stdint.h>

/* Campaign test aids (developer tools, THANDOR_DEV_TOOLS), reached through thandor/platform/debug/hooks.h from
   the scenario catalog and the in-game session runtime. Environment switches:
   OPEN_THANDOR_LIST_SCENARIOS=1 logs all single games and campaigns and exits;
   OPEN_THANDOR_CAMPAIGN=<name|row> starts that campaign, OPEN_THANDOR_CAMPAIGN_LEVEL=<n> at its n-th level;
   OPEN_THANDOR_AUTOWIN=<seconds> wins each campaign level after that time (OPEN_THANDOR_AUTOWIN_LEVELS=<k>: only
   the first k sessions). The level start also logs the units a campaign carries over. */

/* Once per frame from InGameRuntime_RunSessionUntilExit: OPEN_THANDOR_AUTOWIN. */
void DebugCampaign_AutoWinTick(void);

/* Level start carry-over log, called before (afterMerge 0) and after (1) OldUnitRuntime_MergeMasksAndReplayRecords. */
void DebugCampaign_LogCarryOver(int afterMerge);

/* "Choose game" page of a local game: OPEN_THANDOR_LIST_SCENARIOS / OPEN_THANDOR_CAMPAIGN. Nonzero when a campaign
   was started. */
int DebugCampaign_ApplyScenarioOptions(void);

/* Just loaded campaign: logs its levels, OPEN_THANDOR_CAMPAIGN_LEVEL picks the first one. */
void DebugCampaign_SelectCampaignLevel(uint8_t *campaignBytes);

#endif /* THANDOR_PLATFORM_DEBUG_CAMPAIGN_H */
