/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/scenario/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/assets/scenario/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 0040AFC0 g_ExecutableDirectoryUtf16 */
__declspec(align(16)) uint16_t g_ExecutableDirectoryUtf16[256] = {0};

/* 0050D9C0 g_ScenarioCatalog */
__declspec(align(16)) ScenarioCatalogHeader *g_ScenarioCatalog = 0;

/* 0050D9C4 g_ScenarioCatalogUsedBytes */
__declspec(align(4)) uint32_t g_ScenarioCatalogUsedBytes = 0;

/* 0050D9C8 u_save___sve_0050d9c8 */
__declspec(align(8)) uint16_t u_save___sve_0050d9c8[11] = L"save\\*.sve";

/* 0050DA0E u_level_level_dat_0050da0e */
__declspec(align(4)) uint16_t u_level_level_dat_0050da0e[16] = L"level\\level.dat";

/* 0050DA2E g_ScenarioLevelDataPathTemplateUtf16 */
__declspec(align(4)) ScenarioLevelDataPathTemplate24 g_ScenarioLevelDataPathTemplateUtf16 = {
    .prefixCodeUnits = {0x6C, 0x65, 0x76, 0x65, 0x6C, 0x5C, 0x6C, 0x65, 0x76, 0x65, 0x6C},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = L".dat"};

/* 0050DA52 u_level_campagne_dat_0050da52 */
__declspec(align(4)) uint16_t u_level_campagne_dat_0050da52[19] = L"level\\campagne.dat";

/* 0050DA78 g_ScenarioCampaignDataPathTemplateUtf16 */
__declspec(align(8)) ScenarioCampaignDataPathTemplate2A g_ScenarioCampaignDataPathTemplateUtf16 = {
    .prefixCodeUnits = {0x6C, 0x65, 0x76, 0x65, 0x6C, 0x5C, 0x63, 0x61, 0x6D, 0x70, 0x61, 0x67, 0x6E, 0x65},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = L".dat"};

/* 0050DAAC u_level_0050daac */
__declspec(align(4)) uint16_t u_level_0050daac[6] = L"level";

/* 0050DAB8 u_level_0050dab8 */
__declspec(align(8)) uint16_t u_level_0050dab8[6] = L"level";

/* 00530C28 g_LevelResourcePathScratchUtf16 */
__declspec(align(8)) uint16_t g_LevelResourcePathScratchUtf16[256] = {0};

/* 00545780 g_FrontendLoadedLevelAsset */
__declspec(align(16)) FrontendLoadedLevelAsset *g_FrontendLoadedLevelAsset = 0;

/* 00545918 g_FrontendScenarioTransferState */
__declspec(align(8)) uint32_t g_FrontendScenarioTransferState = 0;

/* 00545A98 g_FrontendScenarioMapOptionHandlerTable */
__declspec(align(8)) ScenarioCatalogRefreshSelectedRecordCallback *g_FrontendScenarioMapOptionHandlerTable[3] = {
    /* 0 */ (void *)ScenarioCatalog_SelectSavedGameAndShowDescription,
    /* 1 */ (void *)ScenarioCatalog_SelectLevelAndShowDescription,
    /* 2 */ (void *)ScenarioCatalog_SelectCampaignAndShowDescription};

/* 00545C72 g_FrontendScenarioPathScratchUtf16 */
__declspec(align(4)) uint16_t g_FrontendScenarioPathScratchUtf16[256] = {0};
