/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/scenario/catalog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_SCENARIO_CATALOG_H
#define THANDOR_ASSETS_SCENARIO_CATALOG_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/scenario/catalog. */
/* Functions are grouped by semantic ownership. */

/* Scenario catalog (g_ScenarioCatalog): ScenarioCatalogHeader followed by the level, campaign and save
   records (0x100 bytes each). Every counted record advances the following section offsets and
   g_ScenarioCatalogUsedBytes by 0x104, although the records are packed 0x100 apart. */
#define SCENARIO_CATALOG_CAPACITY 0x30000
#define SCENARIO_CATALOG_HEADER_SIZE 0x18
#define SCENARIO_CATALOG_RECORD_SIZE 0x100
#define SCENARIO_CATALOG_RECORD_STRIDE 0x104
/* Text resource ids: a level title is 0x2230 + its title index, a campaign title 0x2220 + its index (the
   level text page aliases, see TextResourcePage_LoadCompatibilityAliases). */
#define TEXT_ID_LEVEL_TITLE_BASE 0x2230
#define TEXT_ID_CAMPAIGN_TITLE_BASE 0x2220
/* A level's description text is 0x230010 + 0x10 * its title index (record +0x70), registered by
   TextResourcePage_LoadCompatibilityAliases. 0x215D fills a scenario description box while no row is selected. */
#define TEXT_ID_LEVEL_DESCRIPTION_BASE 0x230010
#define TEXT_ID_LEVEL_DESCRIPTION_STRIDE 0x10 /* description + TEXT_LEVEL_EXTRA_LINE_COUNT lines per level */
#define TEXT_ID_SCENARIO_DESCRIPTION_EMPTY 0x215D
/* A campaign's description text is 0x230000 + its title index (record +0x50). */
#define TEXT_ID_CAMPAIGN_DESCRIPTION_BASE 0x230000
/* Saved-game description: a template whose rich-text payload selectors 0 and 1 receive the texts of the save
   record's +0x90 and +0x70 ids (ScenarioCatalog_SelectSavedGameAndShowDescription). */
#define TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE 0x215E
/* Text id bases of the other single-game list columns (index at level record +0x50, +0x60 and +0x80). */
#define TEXT_ID_LEVEL_COLUMN50_BASE 0x220A
#define TEXT_ID_LEVEL_COLUMN60_BASE 0x2200
#define TEXT_ID_LEVEL_COLUMN80_BASE 0x2205

/* Campaign asset (level\<name>.cgn, g_FrontendLoadedCampaignAsset): a 0x200-byte header followed by
   levelRecordCount level records of 0x180 bytes. The original walks the records with a cursor that starts at
   the asset base and advances by 0x180, reading the fields at cursor + 0x200 + field offset; the port keeps
   that cursor and reads ((CampaignAsset *)cursor)->levels[0] (= record i of the real asset). */
typedef struct CampaignLevelRecord {
    int32_t successorLevelIds[8];        /* +0x000 next level id per end selection (g_EndMovieSelectionIndex), <0 = end */
    int32_t endMovieNumbersVariant[8];   /* +0x020 end movie number per end selection, nonzero g_EndMovieVariantIndex */
    int32_t endMovieNumbers[8];          /* +0x040 end movie number per end selection, g_EndMovieVariantIndex 0 */
    int32_t exitZoneCenterX[8];          /* +0x060 per faction: carry-over exit zone centre X (Q12) */
    int32_t exitZoneCenterY[8];          /* +0x080 per faction: exit zone centre Y */
    int32_t exitZoneRadius[8];           /* +0x0A0 per faction: exit zone radius (<= 0 = none) */
    int32_t exitZoneDestinationX[8];     /* +0x0C0 per faction: where the carried units appear, X */
    int32_t exitZoneDestinationY[8];     /* +0x0E0 per faction: destination Y */
    int32_t levelId;                     /* +0x100 */
    uint32_t carryOverMask;              /* +0x104 one carry-over bit per end selection */
    uint32_t skipMask;                   /* +0x108 one skip bit per end selection */
    uint16_t levelFileName[58];          /* +0x10C UTF-16 level file name (level\<name>.lev) */
} CampaignLevelRecord;

typedef struct CampaignAsset {
    enum AssetMagic magic;               /* +0x00 */
    PckDecodedByteCount decodedSizeBytes; /* +0x04 allocation/decoded size of the whole asset */
    uint8_t reserved08_B3[0xac];
    int32_t firstLevelId;                /* +0xB4 */
    int32_t levelRecordCount;            /* +0xB8 */
    uint8_t reservedBC_C3[8];
    int32_t currentLevelId;              /* +0xC4 */
    uint8_t reservedC8_1FF[0x138];
    CampaignLevelRecord levels[1];       /* +0x200, levelRecordCount records */
} CampaignAsset;

/* Index of the "Choose game" page (gameSelectPage) in the frontend page stack. */
#define FRONTEND_PAGE_STACK_CHOOSE_GAME 10

/* Tabs of the scenario-selection page (index into scenarioCatalogRebuildCallbacks and
   g_FrontendScenarioMapOptionHandlerTable). */
#define SCENARIO_SELECTION_TAB_SAVED_GAMES 0
#define SCENARIO_SELECTION_TAB_SINGLE_GAMES 1
#define SCENARIO_SELECTION_TAB_CAMPAIGNS 2

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

void FrontendScenarioSelection_SelectOrStartSavedGame(UiPointerListControl *listControl);

void FrontendScenarioSelection_SelectOrStartLevel(UiPointerListControl *listControl);

void FrontendScenarioSelection_SelectOrStartCampaign(UiPointerListControl *listControl);

void FrontendScenarioSelectionPage_InitializeAndApplyMapOption
          (FrontendScenarioSelectionPageView *scenarioSelectionPage);

void FrontendScenarioPage_OpenSaveRecordsAndRefresh(UiNodeBase *sourceNode);

void FrontendScenarioPage_OpenLevelRecordsAndRefresh(UiNodeBase *sourceNode);

void FrontendScenarioPage_OpenCampaignRecordsAndRefresh(UiNodeBase *sourceNode);

void FrontendScenarioAction_StartFieldGridLoad(void *source);

void ScenarioCatalog_Rebuild(void);

void ScenarioCatalog_RequestRomTransitionStopCallback(uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,
                                                 uint32_t unusedArg3);

void FrontendScenarioTransfer_ProcessReceivedAsset(void);

void FrontendScenarioSession_LoadOrRequestFieldGrid(uint32_t playerRuntimeId);

void FrontendScenarioSession_LoadOrRequestCampaignBundle
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t selectedRecordIndex);

void ScenarioCatalog_RebuildSaveRecordListPage
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3);

void ScenarioCatalog_RebuildLevelRecordListPage
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3);

void ScenarioCatalog_RebuildCampaignRecordListPage
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3);

ScenarioCatalogRecordCount ScenarioCatalog_MergeRecordsByName
          (ScenarioCatalogSourceByteCount sourceByteCount,ScenarioCatalogRecord *sourceRecords,
          ScenarioCatalogRecordCount existingRecordCount,ScenarioCatalogRecord *destinationRecords);

void FrontendScenarioSession_LoadOrRequestLevelAsset
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t selectedRowIndex);

void ScenarioCatalog_SelectSavedGameAndShowDescription
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex);

void ScenarioCatalog_SelectCampaignAndShowDescription
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex);

void FrontendScenarioSelection_ActivateSelectedRecord(FrontendScenarioSelectionControlAddress32 selectionControl);

void ScenarioCatalog_SelectLevelAndShowDescription
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex);

extern ScenarioCatalogHeader *g_ScenarioCatalog;
extern uint32_t g_ScenarioCatalogUsedBytes;
extern uint16_t u_save___sve_0050d9c8[11];
extern uint16_t u_level_0050daac[6];
extern uint16_t g_LevelResourcePathScratchUtf16[256];
extern FrontendLoadedLevelAsset *g_FrontendLoadedLevelAsset;
extern uint32_t g_FrontendScenarioTransferState;
extern uint16_t g_FrontendScenarioPathScratchUtf16[256]; /* level/campaign/save path (level\<name>.lev etc.) built for Package_LoadEntry */

extern uint32_t g_FrontendLoadedCampaignAsset;

extern uint16_t g_UnreferencedLevelPatternUtf16[12]; /* UTF-16 L"level\\*.lev" after the save pattern; no code reference found */
extern uint16_t g_UnreferencedCampaignPatternUtf16[12]; /* UTF-16 L"level\\*.cgn"; no code reference found */

#endif /* THANDOR_ASSETS_SCENARIO_CATALOG_H */
