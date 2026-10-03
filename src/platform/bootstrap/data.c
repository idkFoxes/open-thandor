/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/platform/bootstrap/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 00402028 g_CommandLineWideArguments */
__declspec(align(8)) CommandLineWideArguments g_CommandLineWideArguments = {0};

/* 004027C4 g_CpuFeatureFlags */
__declspec(align(4)) uint32_t g_CpuFeatureFlags = 0;

/* 00407514 g_PackageLastErrorPath */
__declspec(align(4)) uint16_t g_PackageLastErrorPath[256] = {0};

/* 00407714 g_FatalErrorDetail1Utf16 */
__declspec(align(4)) uint16_t g_FatalErrorDetail1Utf16[256] = {0};

/* 00417214 g_KeyboardReadEvent */
__declspec(align(4)) KeyboardReadEventProc *g_KeyboardReadEvent = (void *)Keyboard_ReadNextEvent;

/* 004A6DA4 g_LooseMoviePathPrefix */
__declspec(align(4)) WidePathBuffer256 g_LooseMoviePathPrefix = {0};

/* 004D11C0 g_SoftwareRasterScanState */
__declspec(align(16)) SoftwareRasterScanState g_SoftwareRasterScanState = {0};

/* 0050B5C0 g_ReverseStereoMask */
__declspec(align(16)) int32_t g_ReverseStereoMask = 0;

/* 0050D9DE g_UnreferencedLevelPatternUtf16: UTF-16 L"level\\*.lev" after the save pattern; no code reference found */
__declspec(align(4)) uint16_t g_UnreferencedLevelPatternUtf16[12] = L"level\\*.lev";

/* 0050D9F6 g_UnreferencedCampaignPatternUtf16: UTF-16 L"level\\*.cgn"; no code reference found */
__declspec(align(4)) uint16_t g_UnreferencedCampaignPatternUtf16[12] = L"level\\*.cgn";

/* 0050DEC4 u_texte_techno_str_0050dec4 */
__declspec(align(4)) uint16_t u_texte_techno_str_0050dec4[17] = L"texte\\techno.str";

/* 0050E054 u_daten_hex_0050e054 */
__declspec(align(4)) uint16_t u_daten_hex_0050e054[10] = L"daten.hex";

/* 0050E082 u_stat_hex_0050e082 */
__declspec(align(4)) uint16_t u_stat_hex_0050e082[9] = L"stat.hex";

/* 0050F104 u_texte_neterror_str_0050f104 */
__declspec(align(4)) uint16_t u_texte_neterror_str_0050f104[19] = L"texte\\neterror.str";

/* 00512D6C g_GameStatTableImage */
__declspec(align(4)) void *g_GameStatTableImage = 0;

/* 00512D70 g_GameDataAuxState */
__declspec(align(16)) GameDataAuxState g_GameDataAuxState = {0};

/* 0051B394 g_UnreferencedArmyTexturePathUtf16: UTF-16 L"army0000.gfx" after g_AiCommandGenerationRetainedTarget; no code reference found */
__declspec(align(4)) uint16_t g_UnreferencedArmyTexturePathUtf16[13] = L"army0000.gfx";

/* 0051B3AE g_UnreferencedArmyTag: char "ARMY" after the army0000.gfx string; no code reference found; followed by 0x90 fill */
__declspec(align(4)) char g_UnreferencedArmyTag[5] = "ARMY";

/* 0054591C g_FrontendPlayerMessageBuffers */
__declspec(align(4)) uint32_t g_FrontendPlayerMessageBuffers = 0;

/* 00545B34 u_texte_hilfe_str_00545b34 */
__declspec(align(4)) uint16_t u_texte_hilfe_str_00545b34[16] = L"texte\\hilfe.str";

/* 00545BA0 u_texte_menue_str_00545ba0 */
__declspec(align(16)) uint16_t u_texte_menue_str_00545ba0[16] = L"texte\\menue.str";

/* 00545BC0 u_texte_level_str_00545bc0 */
__declspec(align(16)) uint16_t u_texte_level_str_00545bc0[16] = L"texte\\level.str";

/* 00545BE0 u_texte_inhalt_str_00545be0 */
__declspec(align(16)) uint16_t u_texte_inhalt_str_00545be0[17] = L"texte\\inhalt.str";

/* 0054DDB0 g_FrontendPlayerListRows */
__declspec(align(16)) uint32_t g_FrontendPlayerListRows[8] = {
    0, /* 0054DDB0 row 0 */
    0, /* 0054DDB4 row 1 */
    0, /* 0054DDB8 row 2 */
    0, /* 0054DDBC row 3 */
    0, /* 0054DDC0 row 4 */
    0, /* 0054DDC4 row 5 */
    0, /* 0054DDC8 row 6 */
    0, /* 0054DDCC row 7 */
};

/* 0054FBD4 g_InGameFactionStatusTextScratchUtf16 */
__declspec(align(4)) uint16_t *g_InGameFactionStatusTextScratchUtf16 = 0;

/* 00563170 u_texte_help_str_00563170 */
__declspec(align(16)) uint16_t u_texte_help_str_00563170[15] = L"texte\\help.str";

/* 005631B8 u_texte_tastatur_str_005631b8 */
__declspec(align(8)) uint16_t u_texte_tastatur_str_005631b8[19] = L"texte\\tastatur.str";

/* 00563334 g_FrontendDebugOverlayTextSlot00Utf16 */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot00Utf16[16] = {0};

/* 00563354 g_FrontendDebugOverlayTextSlot01Utf16 */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot01Utf16[16] = {0};

/* 00563374 g_FrontendDebugOverlayTextSlot02Utf16 */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot02Utf16[16] = {0};

/* 00563394 g_FrontendDebugOverlayTextSlot03Utf16 */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot03Utf16[16] = {0};

/* 005633B4 g_FrontendDebugOverlayTextSlot04Utf16 */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot04Utf16[16] = {0};

/* 005633D4 g_FrontendDebugOverlayTextSlot05Utf16 */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot05Utf16[16] = {0};

/* 005633F4 g_FrontendDebugOverlayTextSlot06Utf16 */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot06Utf16[16] = {0};

/* 00563414 g_FrontendDebugOverlayTextSlot07Utf16 */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot07Utf16[16] = {0};

/* 00563434 g_FrontendDebugOverlayTextSlot08Utf16 */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot08Utf16[16] = {0};

/* 00563454 g_FrontendDebugOverlayTextSlot09Utf16 */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot09Utf16[16] = {0};

/* 005634B4 g_FrontendDebugOverlayTextSlot12Utf16 */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot12Utf16[16] = {0};

/* 005634D4 g_FrontendDebugOverlayTextSlot13Utf16: 32 units, the last slot owns the unnamed 0x20 bytes up to
   0x563514. It receives the locale-formatted elapsed time (hours, time separator, minutes, AM/PM designator),
   which can run past 16 units. */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot13Utf16[32] = {0};

/* 00572AC0 g_DataPackageHandle */
__declspec(align(16)) uint32_t g_DataPackageHandle = 0;

/* 00572AC4 g_ModelPackageHandle */
__declspec(align(4)) uint32_t g_ModelPackageHandle = 0;

/* 00572AC8 g_GraphicsPackageHandle */
__declspec(align(8)) uint32_t g_GraphicsPackageHandle = 0;

/* 00572AD0 g_MoviePackageHandle */
__declspec(align(16)) uint32_t g_MoviePackageHandle = 0;

/* 00572AD4 g_LevelPackageHandle */
__declspec(align(4)) uint32_t g_LevelPackageHandle = 0;

/* 00572B00 g_IntroMoviePendingTicks */
__declspec(align(16)) uint32_t g_IntroMoviePendingTicks = 0;

/* 00572B04 g_InstallRegistryKeyHandle */
__declspec(align(4)) uint32_t g_InstallRegistryKeyHandle = 0;

/* 00572B08 g_InstallRegistryValueDataCapacityBytes: uint32_t: RegQueryValueExA lpcbData for the install "CD" value, initially 256 (size of g_InstallRegistryValueDataA); platform/bootstrap/runtime.c */
__declspec(align(8)) uint32_t g_InstallRegistryValueDataCapacityBytes = 256;

/* 00572B0C g_InstallRegistryValueType */
__declspec(align(4)) uint32_t g_InstallRegistryValueType = 0;

/* 00572B10 g_InstallRegistryValueDataA */
__declspec(align(16)) uint8_t g_InstallRegistryValueDataA[256] = {0};

/* 00572C10 g_InstallDirectoryScratchUtf16 */
__declspec(align(16)) uint16_t g_InstallDirectoryScratchUtf16[256] = {0};

/* 00572E10 u_Thandor_00572e10 */
__declspec(align(16)) uint16_t u_Thandor_00572e10[8] = L"Thandor";

/* 00572E20 s_Software_Planet4_Thandor_00572e20 */
__declspec(align(16)) char s_Software_Planet4_Thandor_00572e20[25] = "Software\\Planet4\\Thandor";

/* 00572E39 s_InstallRegistryValueNameCD */
__declspec(align(4)) char g_InstallRegistryValueNameCD[3] = "CD";

/* 00572E3C g_ScreenshotFileNameUtf16 */
__declspec(align(4)) uint16_t g_ScreenshotFileNameUtf16[13] = L"screen00.pcx";

/* 00572E56 u_daten_pck_00572e56 */
__declspec(align(4)) uint16_t u_daten_pck_00572e56[10] = L"daten.pck";

/* 00572E6A u_modelle_pck_00572e6a */
__declspec(align(4)) uint16_t u_modelle_pck_00572e6a[12] = L"modelle.pck";

/* 00572E82 u_graphik_pck_00572e82 */
__declspec(align(4)) uint16_t u_graphik_pck_00572e82[12] = L"graphik.pck";

/* 00572E9A u_sound_pck_00572e9a */
__declspec(align(4)) uint16_t u_sound_pck_00572e9a[10] = L"sound.pck";

/* 00572EAE u_filme_pck_00572eae */
__declspec(align(4)) uint16_t u_filme_pck_00572eae[10] = L"filme.pck";

/* 00572EC2 u_level_pck_00572ec2 */
__declspec(align(4)) uint16_t u_level_pck_00572ec2[10] = L"level.pck";

/* 00572ED6 g_PatchArchivePathTemplateUtf16 */
__declspec(align(4)) PatchArchivePathTemplate18 g_PatchArchivePathTemplateUtf16 = {
    .prefixCodeUnits = {0x70, 0x61, 0x74, 0x63, 0x68},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = L".pck"};

/* 00572EEE g_LevelArchivePathTemplateUtf16 */
__declspec(align(4)) LevelArchivePathTemplate18 g_LevelArchivePathTemplateUtf16 = {
    .prefixCodeUnits = {0x6C, 0x65, 0x76, 0x65, 0x6C},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = L".pck"};

/* 00572F06 u_sound_button0_sam_00572f06 */
__declspec(align(4)) uint16_t u_sound_button0_sam_00572f06[18] = L"sound\\button0.sam";

/* 00572F2A u_sound_button1_sam_00572f2a */
__declspec(align(4)) uint16_t u_sound_button1_sam_00572f2a[18] = L"sound\\button1.sam";

/* 00572F4E u_sound_button2_sam_00572f4e */
__declspec(align(4)) uint16_t u_sound_button2_sam_00572f4e[18] = L"sound\\button2.sam";

/* 00572F72 u_sound_button3_sam_00572f72 */
__declspec(align(4)) uint16_t u_sound_button3_sam_00572f72[18] = L"sound\\button3.sam";

/* 00572F96 u_sound_button4_sam_00572f96 */
__declspec(align(4)) uint16_t u_sound_button4_sam_00572f96[18] = L"sound\\button4.sam";

/* 00572FBA u_sound_button5_sam_00572fba */
__declspec(align(4)) uint16_t u_sound_button5_sam_00572fba[18] = L"sound\\button5.sam";

/* 00572FDE u_sound_button6_sam_00572fde */
__declspec(align(4)) uint16_t u_sound_button6_sam_00572fde[18] = L"sound\\button6.sam";

/* 00573002 u_gfx_panel_stat_gfx_00573002 */
__declspec(align(4)) uint16_t u_gfx_panel_stat_gfx_00573002[19] = L"gfx\\panel\\stat.gfx";

/* 00573046 u_flm_intro0_flm_00573046 */
__declspec(align(4)) uint16_t u_flm_intro0_flm_00573046[15] = L"flm\\intro0.flm";

/* 00573064 g_CommandLineOptionNoIntro */
__declspec(align(4)) char g_CommandLineOptionNoIntro[8] = "NOINTRO";

/* 00573EF0 g_DynamicModules */
__declspec(align(16)) DynamicModuleEntry g_DynamicModules[16] = {0};

/* 00573F70 g_DynamicModuleCount */
__declspec(align(16)) uint32_t g_DynamicModuleCount = 0;

/* 00573F74 g_BootstrapApiBindings: 8 bindings, then the all-zero terminator [8] that ends the DynAPI_Bootstrap scan */
__declspec(align(4)) DynamicApiBinding g_BootstrapApiBindings[9] = {
        /* 0 */ {.destination = (void *)&dynapi_9, .moduleName = (void *)g_Kernel32ModuleName},
        /* 1 */ {.destination = (void *)g_BootstrapApiName_FreeLibrary, .moduleName = (void *)g_Kernel32ModuleName},
        /* 2 */ {.destination = (void *)g_BootstrapApiName_timeSetEvent, .moduleName = (void *)g_WinmmModuleName},
        /* 3 */ {.destination = (void *)g_BootstrapApiName_timeKillEvent, .moduleName = (void *)g_WinmmModuleName},
        /* 4 */ {.destination = (void *)g_BootstrapApiName_mciSendCommandA, .moduleName = (void *)g_WinmmModuleName},
        /* 5 */ {.destination = (void *)g_BootstrapApiName_RegOpenKeyExA, .moduleName = (void *)g_Advapi32ModuleName},
        /* 6 */ {
        .destination = (void *)g_BootstrapApiName_RegQueryValueExA,
        .moduleName = (void *)g_Advapi32ModuleName},
        /* 7 */ {.destination = (void *)g_BootstrapApiName_RegCloseKey, .moduleName = (void *)g_Advapi32ModuleName},
        /* 8: terminator */ {0}};

/* 005744A4 sz_KERNEL32 */
__declspec(align(4)) char g_Kernel32ModuleName[9] = "KERNEL32";

/* 005744AE sz_WINMM */
__declspec(align(4)) char g_WinmmModuleName[6] = "WINMM";

/* 005744D2 sz_ADVAPI32 */
__declspec(align(4)) char g_Advapi32ModuleName[9] = "ADVAPI32";

/* 005744EC dynapi_9 */
__declspec(align(4)) char dynapi_9[13] = "LoadLibraryA";

/* 005744FA dynapi_10 */
__declspec(align(4)) char g_BootstrapApiName_FreeLibrary[12] = "FreeLibrary";

/* 00574506 dynapi_11 */
__declspec(align(4)) char g_BootstrapApiName_RegOpenKeyExA[14] = "RegOpenKeyExA";

/* 00574514 dynapi_12 */
__declspec(align(4)) char g_BootstrapApiName_RegQueryValueExA[17] = "RegQueryValueExA";

/* 00574526 dynapi_13 */
__declspec(align(4)) char g_BootstrapApiName_RegCloseKey[12] = "RegCloseKey";

/* 00574532 dynapi_14 */
__declspec(align(4)) char g_BootstrapApiName_timeSetEvent[13] = "timeSetEvent";

/* 00574540 dynapi_15 */
__declspec(align(16)) char g_BootstrapApiName_timeKillEvent[14] = "timeKillEvent";

/* 0057454E dynapi_16 */
__declspec(align(4)) char g_BootstrapApiName_mciSendCommandA[16] = "mciSendCommandA";

/* 00582F28 g_CommandLineOptionSound */
__declspec(align(8)) char g_CommandLineOptionSound[6] = "SOUND";

/* 005856B0 g_hInstance */
__declspec(align(16)) HINSTANCE g_hInstance = 0;

/* 005856B4 g_MainWindow */
__declspec(align(4)) HWND g_MainWindow = 0;

/* 005856BC g_AppActive: uint32_t: WM_ACTIVATEAPP wParam (application active flag), initially 1; platform/bootstrap/runtime.c */
__declspec(align(4)) uint32_t g_AppActive = 1;

/* 005856C0 g_MainMessageStorage */
__declspec(align(16)) Win32MainMessageStorage g_MainMessageStorage = {
    .overlay = {.windowClass = {.style = 3, .windowProc = (void *)MainWindowProc, .className = (void *)&sz_MainWindowClass}}};

/* 0058581C g_CommandLine */
__declspec(align(4)) CommandLineArgumentMirrorState500 g_CommandLine = {0};

/* 00585D1C sz_MainWindowTitle */
__declspec(align(4)) char sz_MainWindowTitle[15] = " thandor  (TG)";

/* 00585D2B sz_MainWindowClass */
__declspec(align(4)) char sz_MainWindowClass[17] = "thandorCLASS(TG)";
