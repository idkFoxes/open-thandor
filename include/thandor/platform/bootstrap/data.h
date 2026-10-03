/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/bootstrap/data.h
 */

#ifndef THANDOR_PLATFORM_BOOTSTRAP_DATA_H
#define THANDOR_PLATFORM_BOOTSTRAP_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern CommandLineWideArguments g_CommandLineWideArguments;

extern uint32_t g_CpuFeatureFlags;

extern uint16_t g_PackageLastErrorPath[256];

extern uint16_t g_FatalErrorDetail1Utf16[256];

extern KeyboardReadEventProc *g_KeyboardReadEvent;

extern WidePathBuffer256 g_LooseMoviePathPrefix;

extern SoftwareRasterScanState g_SoftwareRasterScanState;

extern int32_t g_ReverseStereoMask;

extern uint16_t g_UnreferencedLevelPatternUtf16[12]; /* UTF-16 L"level\\*.lev" after the save pattern; no code reference found */

extern uint16_t g_UnreferencedCampaignPatternUtf16[12]; /* UTF-16 L"level\\*.cgn"; no code reference found */

extern uint16_t u_texte_techno_str_0050dec4[17];

extern uint16_t u_daten_hex_0050e054[10];

extern uint16_t u_stat_hex_0050e082[9];

extern uint16_t u_texte_neterror_str_0050f104[19];

extern void *g_GameStatTableImage;

extern GameDataAuxState g_GameDataAuxState;

extern uint16_t g_UnreferencedArmyTexturePathUtf16[13]; /* UTF-16 L"army0000.gfx" after g_AiCommandGenerationRetainedTarget; no code reference found */

extern char g_UnreferencedArmyTag[5]; /* char "ARMY" after the army0000.gfx string; no code reference found; followed by 0x90 fill */

extern uint32_t g_FrontendPlayerMessageBuffers;

extern uint16_t u_texte_hilfe_str_00545b34[16];

extern uint16_t u_texte_menue_str_00545ba0[16];

extern uint16_t u_texte_level_str_00545bc0[16];

extern uint16_t u_texte_inhalt_str_00545be0[17];

extern uint32_t g_FrontendPlayerListRows[8]; /* pointers (as uint32_t) to the eight 0x80-byte lobby player list rows */

extern uint16_t *g_InGameFactionStatusTextScratchUtf16;

extern uint16_t u_texte_help_str_00563170[15];

extern uint16_t u_texte_tastatur_str_005631b8[19];

extern uint16_t g_FrontendDebugOverlayTextSlot00Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot01Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot02Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot03Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot04Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot05Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot06Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot07Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot08Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot09Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot12Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot13Utf16[32]; /* owns the unnamed 0x20 bytes after its first 16 units in the original (elapsed time can exceed 16 units) */

extern uint32_t g_DataPackageHandle;

extern uint32_t g_ModelPackageHandle;

extern uint32_t g_GraphicsPackageHandle;

extern uint32_t g_MoviePackageHandle;

extern uint32_t g_LevelPackageHandle;

extern uint32_t g_IntroMoviePendingTicks;

extern uint32_t g_InstallRegistryKeyHandle;

extern uint32_t g_InstallRegistryValueDataCapacityBytes; /* uint32_t: RegQueryValueExA lpcbData for the install "CD" value, initially 256 (size of g_InstallRegistryValueDataA); platform/bootstrap/runtime.c */

extern uint32_t g_InstallRegistryValueType;

extern uint8_t g_InstallRegistryValueDataA[256]; /* RegQueryValueExA data buffer for the install "CD" value (capacity g_InstallRegistryValueDataCapacityBytes) */

extern uint16_t g_InstallDirectoryScratchUtf16[256];

extern uint16_t u_Thandor_00572e10[8];

extern char s_Software_Planet4_Thandor_00572e20[25];

extern char g_InstallRegistryValueNameCD[3]; /* registry value "CD" */

/* "screen00.pcx" with its two-digit counter at code units 6 and 7 */
extern uint16_t g_ScreenshotFileNameUtf16[13];

extern uint16_t u_daten_pck_00572e56[10];

extern uint16_t u_modelle_pck_00572e6a[12];

extern uint16_t u_graphik_pck_00572e82[12];

extern uint16_t u_sound_pck_00572e9a[10];

extern uint16_t u_filme_pck_00572eae[10];

extern uint16_t u_level_pck_00572ec2[10];

extern PatchArchivePathTemplate18 g_PatchArchivePathTemplateUtf16;

extern LevelArchivePathTemplate18 g_LevelArchivePathTemplateUtf16;

extern uint16_t u_sound_button0_sam_00572f06[18];

extern uint16_t u_sound_button1_sam_00572f2a[18];

extern uint16_t u_sound_button2_sam_00572f4e[18];

extern uint16_t u_sound_button3_sam_00572f72[18];

extern uint16_t u_sound_button4_sam_00572f96[18];

extern uint16_t u_sound_button5_sam_00572fba[18];

extern uint16_t u_sound_button6_sam_00572fde[18];

extern uint16_t u_gfx_panel_stat_gfx_00573002[19];

extern uint16_t u_flm_intro0_flm_00573046[15];

extern char g_CommandLineOptionNoIntro[8];

extern DynamicModuleEntry g_DynamicModules[16];

extern uint32_t g_DynamicModuleCount;

extern DynamicApiBinding g_BootstrapApiBindings[9]; /* 8 bindings + the all-zero terminator [8] that ends the DynAPI_Bootstrap scan */

extern char g_Kernel32ModuleName[9];

extern char g_WinmmModuleName[6];

extern char g_Advapi32ModuleName[9];

extern char dynapi_9[13];

extern char g_BootstrapApiName_FreeLibrary[12];

extern char g_BootstrapApiName_RegOpenKeyExA[14];

extern char g_BootstrapApiName_RegQueryValueExA[17];

extern char g_BootstrapApiName_RegCloseKey[12];

extern char g_BootstrapApiName_timeSetEvent[13];

extern char g_BootstrapApiName_timeKillEvent[14];

extern char g_BootstrapApiName_mciSendCommandA[16];

extern char g_CommandLineOptionSound[6];

extern HINSTANCE g_hInstance;

extern HWND g_MainWindow;

extern uint32_t g_AppActive; /* uint32_t: WM_ACTIVATEAPP wParam (application active flag), initially 1; platform/bootstrap/runtime.c */

extern Win32MainMessageStorage g_MainMessageStorage;

extern CommandLineArgumentMirrorState500 g_CommandLine;

extern char sz_MainWindowTitle[15];

extern char sz_MainWindowClass[17];

#endif
