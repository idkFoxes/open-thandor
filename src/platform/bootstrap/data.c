/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/platform/bootstrap/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(8)) CommandLineWideArguments g_CommandLineWideArguments = {0};

__declspec(align(4)) uint32_t g_CpuFeatureFlags = 0;

__declspec(align(4)) uint16_t g_PackageLastErrorPath[256] = {0};

__declspec(align(4)) uint16_t g_FatalErrorDetail1Utf16[256] = {0};

__declspec(align(4)) KeyboardReadEventProc *g_KeyboardReadEvent = (void *)Keyboard_ReadNextEvent;

__declspec(align(4)) WidePathBuffer256 g_LooseMoviePathPrefix = {0};

__declspec(align(16)) SoftwareRasterScanState g_SoftwareRasterScanState = {0};

__declspec(align(16)) int32_t g_ReverseStereoMask = 0;

/* UTF-16 L"level\\*.lev" after the save pattern; no code reference found */
__declspec(align(4)) uint16_t g_UnreferencedLevelPatternUtf16[12] = L"level\\*.lev";

/* UTF-16 L"level\\*.cgn"; no code reference found */
__declspec(align(4)) uint16_t g_UnreferencedCampaignPatternUtf16[12] = L"level\\*.cgn";

__declspec(align(4)) uint16_t u_texte_techno_str_0050dec4[17] = L"texte\\techno.str";

__declspec(align(4)) uint16_t u_daten_hex_0050e054[10] = L"daten.hex";

__declspec(align(4)) uint16_t u_stat_hex_0050e082[9] = L"stat.hex";

__declspec(align(4)) uint16_t u_texte_neterror_str_0050f104[19] = L"texte\\neterror.str";

__declspec(align(4)) void *g_GameStatTableImage = 0;

__declspec(align(16)) GameDataAuxState g_GameDataAuxState = {0};

/* UTF-16 L"army0000.gfx" after g_AiCommandGenerationRetainedTarget; no code reference found */
__declspec(align(4)) uint16_t g_UnreferencedArmyTexturePathUtf16[13] = L"army0000.gfx";

/* char "ARMY" after the army0000.gfx string; no code reference found; followed by 0x90 fill */
__declspec(align(4)) char g_UnreferencedArmyTag[5] = "ARMY";

__declspec(align(4)) uint32_t g_FrontendPlayerMessageBuffers = 0;

__declspec(align(4)) uint16_t u_texte_hilfe_str_00545b34[16] = L"texte\\hilfe.str";

__declspec(align(16)) uint16_t u_texte_menue_str_00545ba0[16] = L"texte\\menue.str";

__declspec(align(16)) uint16_t u_texte_level_str_00545bc0[16] = L"texte\\level.str";

__declspec(align(16)) uint16_t u_texte_inhalt_str_00545be0[17] = L"texte\\inhalt.str";

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

__declspec(align(4)) uint16_t *g_InGameFactionStatusTextScratchUtf16 = 0;

__declspec(align(16)) uint16_t u_texte_help_str_00563170[15] = L"texte\\help.str";

__declspec(align(8)) uint16_t u_texte_tastatur_str_005631b8[19] = L"texte\\tastatur.str";

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot00Utf16[16] = {0};

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot01Utf16[16] = {0};

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot02Utf16[16] = {0};

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot03Utf16[16] = {0};

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot04Utf16[16] = {0};

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot05Utf16[16] = {0};

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot06Utf16[16] = {0};

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot07Utf16[16] = {0};

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot08Utf16[16] = {0};

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot09Utf16[16] = {0};

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot12Utf16[16] = {0};

/* 32 units, the last slot owns the unnamed 0x20 bytes up to
   0x563514. It receives the locale-formatted elapsed time (hours, time separator, minutes, AM/PM designator),
   which can run past 16 units. */
__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot13Utf16[32] = {0};

__declspec(align(16)) uint32_t g_DataPackageHandle = 0;

__declspec(align(4)) uint32_t g_ModelPackageHandle = 0;

__declspec(align(8)) uint32_t g_GraphicsPackageHandle = 0;

__declspec(align(16)) uint32_t g_MoviePackageHandle = 0;

__declspec(align(4)) uint32_t g_LevelPackageHandle = 0;

__declspec(align(16)) uint32_t g_IntroMoviePendingTicks = 0;

__declspec(align(4)) uint32_t g_InstallRegistryKeyHandle = 0;

/* uint32_t: RegQueryValueExA lpcbData for the install "CD" value, initially 256 (size of g_InstallRegistryValueDataA); platform/bootstrap/runtime.c */
__declspec(align(8)) uint32_t g_InstallRegistryValueDataCapacityBytes = 256;

__declspec(align(4)) uint32_t g_InstallRegistryValueType = 0;

__declspec(align(16)) uint8_t g_InstallRegistryValueDataA[256] = {0};

__declspec(align(16)) uint16_t g_InstallDirectoryScratchUtf16[256] = {0};

__declspec(align(16)) uint16_t u_Thandor_00572e10[8] = L"Thandor";

__declspec(align(16)) char s_Software_Planet4_Thandor_00572e20[25] = "Software\\Planet4\\Thandor";

__declspec(align(4)) char g_InstallRegistryValueNameCD[3] = "CD";

__declspec(align(4)) uint16_t g_ScreenshotFileNameUtf16[13] = L"screen00.pcx";

__declspec(align(4)) uint16_t u_daten_pck_00572e56[10] = L"daten.pck";

__declspec(align(4)) uint16_t u_modelle_pck_00572e6a[12] = L"modelle.pck";

__declspec(align(4)) uint16_t u_graphik_pck_00572e82[12] = L"graphik.pck";

__declspec(align(4)) uint16_t u_sound_pck_00572e9a[10] = L"sound.pck";

__declspec(align(4)) uint16_t u_filme_pck_00572eae[10] = L"filme.pck";

__declspec(align(4)) uint16_t u_level_pck_00572ec2[10] = L"level.pck";

__declspec(align(4)) PatchArchivePathTemplate18 g_PatchArchivePathTemplateUtf16 = {
    .prefixCodeUnits = {0x70, 0x61, 0x74, 0x63, 0x68},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = L".pck"};

__declspec(align(4)) LevelArchivePathTemplate18 g_LevelArchivePathTemplateUtf16 = {
    .prefixCodeUnits = {0x6C, 0x65, 0x76, 0x65, 0x6C},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = L".pck"};

__declspec(align(4)) uint16_t u_sound_button0_sam_00572f06[18] = L"sound\\button0.sam";

__declspec(align(4)) uint16_t u_sound_button1_sam_00572f2a[18] = L"sound\\button1.sam";

__declspec(align(4)) uint16_t u_sound_button2_sam_00572f4e[18] = L"sound\\button2.sam";

__declspec(align(4)) uint16_t u_sound_button3_sam_00572f72[18] = L"sound\\button3.sam";

__declspec(align(4)) uint16_t u_sound_button4_sam_00572f96[18] = L"sound\\button4.sam";

__declspec(align(4)) uint16_t u_sound_button5_sam_00572fba[18] = L"sound\\button5.sam";

__declspec(align(4)) uint16_t u_sound_button6_sam_00572fde[18] = L"sound\\button6.sam";

__declspec(align(4)) uint16_t u_gfx_panel_stat_gfx_00573002[19] = L"gfx\\panel\\stat.gfx";

__declspec(align(4)) uint16_t u_flm_intro0_flm_00573046[15] = L"flm\\intro0.flm";

__declspec(align(4)) char g_CommandLineOptionNoIntro[8] = "NOINTRO";

__declspec(align(16)) DynamicModuleEntry g_DynamicModules[16] = {0};

__declspec(align(16)) uint32_t g_DynamicModuleCount = 0;

/* 8 bindings, then the all-zero terminator [8] that ends the DynAPI_Bootstrap scan */
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

__declspec(align(4)) char g_Kernel32ModuleName[9] = "KERNEL32";

__declspec(align(4)) char g_WinmmModuleName[6] = "WINMM";

__declspec(align(4)) char g_Advapi32ModuleName[9] = "ADVAPI32";

__declspec(align(4)) char dynapi_9[13] = "LoadLibraryA";

__declspec(align(4)) char g_BootstrapApiName_FreeLibrary[12] = "FreeLibrary";

__declspec(align(4)) char g_BootstrapApiName_RegOpenKeyExA[14] = "RegOpenKeyExA";

__declspec(align(4)) char g_BootstrapApiName_RegQueryValueExA[17] = "RegQueryValueExA";

__declspec(align(4)) char g_BootstrapApiName_RegCloseKey[12] = "RegCloseKey";

__declspec(align(4)) char g_BootstrapApiName_timeSetEvent[13] = "timeSetEvent";

__declspec(align(16)) char g_BootstrapApiName_timeKillEvent[14] = "timeKillEvent";

__declspec(align(4)) char g_BootstrapApiName_mciSendCommandA[16] = "mciSendCommandA";

__declspec(align(8)) char g_CommandLineOptionSound[6] = "SOUND";

__declspec(align(16)) HINSTANCE g_hInstance = 0;

__declspec(align(4)) HWND g_MainWindow = 0;

/* uint32_t: WM_ACTIVATEAPP wParam (application active flag), initially 1; platform/bootstrap/runtime.c */
__declspec(align(4)) uint32_t g_AppActive = 1;

__declspec(align(16)) Win32MainMessageStorage g_MainMessageStorage = {
    .overlay = {.windowClass = {.style = 3, .windowProc = (void *)MainWindowProc, .className = (void *)&sz_MainWindowClass}}};

__declspec(align(4)) CommandLineArgumentMirrorState500 g_CommandLine = {0};

__declspec(align(4)) char sz_MainWindowTitle[15] = " thandor  (TG)";

__declspec(align(4)) char sz_MainWindowClass[17] = "thandorCLASS(TG)";
