/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/scenario_transfer.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_SCENARIO_TRANSFER_H
#define THANDOR_NETWORK_PROTOCOL_SCENARIO_TRANSFER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/protocol/scenario_transfer. */

/* g_FrontendScenarioTransferState: which asset a network client expects next in the transfer mailbox
   (handled by FrontendScenarioTransfer_ProcessReceivedAsset). */
#define SCENARIO_TRANSFER_NONE 0
#define SCENARIO_TRANSFER_CATALOG 1            /* scenario catalog */
#define SCENARIO_TRANSFER_LEVEL 2              /* level asset */
#define SCENARIO_TRANSFER_FIELD_GRID 3         /* field grid of the loaded level */
#define SCENARIO_TRANSFER_CAMPAIGN_BUNDLE 4    /* level + campaign + field grid */
#define SCENARIO_TRANSFER_LEVEL_BUNDLE 5       /* level + field grid (every value >= 5) */

/* Header of a SCENARIO_TRANSFER_CAMPAIGN_BUNDLE packet; the three encoded images follow it. */
typedef struct ScenarioCampaignBundleHeader {
    uint32_t levelDecodedBytes;          /* +0x00 */
    uint32_t campaignDecodedBytes;       /* +0x04 */
    uint32_t fieldGridDecodedBytes;      /* +0x08 */
    uint32_t levelEncodedBytes;          /* +0x0C */
    uint32_t campaignEncodedBytes;       /* +0x10 */
    uint32_t fieldGridEncodedBytes;      /* +0x14 */
} ScenarioCampaignBundleHeader;

/* Header of a SCENARIO_TRANSFER_LEVEL_BUNDLE packet (built by Frontend_MainLoop); the two encoded
   images follow it. */
typedef struct ScenarioLevelBundleHeader {
    uint32_t levelDecodedBytes;          /* +0x00 */
    uint32_t fieldGridDecodedBytes;      /* +0x04 */
    uint32_t levelEncodedBytes;          /* +0x08 */
    uint32_t fieldGridEncodedBytes;      /* +0x0C */
} ScenarioLevelBundleHeader;

/* Functions are grouped by semantic ownership. */

void FrontendScenarioTransfer_ProcessReceivedAsset(void);

void FrontendScenarioTransfer_ReleaseLoadedLevelAsset(void);

extern uint32_t g_FrontendScenarioTransferState;

/* DwordBlock64Array_ContainsExactRecord: dwords per compared record */
#define DWORD_BLOCK64_RECORD_DWORDS 0x40

Bool8 DwordBlock64Array_ContainsExactRecord
          (DwordBlockRecordCount recordCount,uint32_t *recordArray,uint32_t *candidateRecord);

#endif /* THANDOR_NETWORK_PROTOCOL_SCENARIO_TRANSFER_H */
