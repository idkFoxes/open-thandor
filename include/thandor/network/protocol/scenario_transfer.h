/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/scenario_transfer.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_SCENARIO_TRANSFER_H
#define THANDOR_NETWORK_PROTOCOL_SCENARIO_TRANSFER_H

#include <thandor/core/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/core/contracts.h>

/* g_FrontendScenarioTransferState: which asset a network client expects next in the transfer mailbox
   (handled by FrontendScenarioTransfer_ProcessReceivedAsset). */
inline constexpr auto SCENARIO_TRANSFER_NONE = 0;
inline constexpr auto SCENARIO_TRANSFER_CATALOG = 1; /* scenario catalog */
inline constexpr auto SCENARIO_TRANSFER_LEVEL = 2; /* level asset */
inline constexpr auto SCENARIO_TRANSFER_FIELD_GRID = 3; /* field grid of the loaded level */
inline constexpr auto SCENARIO_TRANSFER_CAMPAIGN_BUNDLE = 4; /* level + campaign + field grid */
inline constexpr auto SCENARIO_TRANSFER_LEVEL_BUNDLE = 5; /* level + field grid (every value >= 5) */

/* Header of a SCENARIO_TRANSFER_CAMPAIGN_BUNDLE packet; the three encoded images follow it. */
struct ScenarioCampaignBundleHeader {
    uint32_t levelDecodedBytes;          /* +0x00 */
    uint32_t campaignDecodedBytes;       /* +0x04 */
    uint32_t fieldGridDecodedBytes;      /* +0x08 */
    uint32_t levelEncodedBytes;          /* +0x0C */
    uint32_t campaignEncodedBytes;       /* +0x10 */
    uint32_t fieldGridEncodedBytes;      /* +0x14 */
};

/* Header of a SCENARIO_TRANSFER_LEVEL_BUNDLE packet (built by Frontend_MainLoop); the two encoded
   images follow it. */
struct ScenarioLevelBundleHeader {
    uint32_t levelDecodedBytes;          /* +0x00 */
    uint32_t fieldGridDecodedBytes;      /* +0x04 */
    uint32_t levelEncodedBytes;          /* +0x08 */
    uint32_t fieldGridEncodedBytes;      /* +0x0C */
};

void FrontendScenarioTransfer_ProcessReceivedAsset();

void FrontendScenarioTransfer_ReleaseLoadedLevelAsset();

extern uint32_t g_FrontendScenarioTransferState;

/* DwordBlock64Array_ContainsExactRecord: dwords per compared record */
inline constexpr auto DWORD_BLOCK64_RECORD_DWORDS = 0x40;

Bool8 DwordBlock64Array_ContainsExactRecord
          (DwordBlockRecordCount recordCount,uint32_t *recordArray,uint32_t *candidateRecord);

#endif /* THANDOR_NETWORK_PROTOCOL_SCENARIO_TRANSFER_H */
