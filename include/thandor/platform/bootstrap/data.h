/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/bootstrap/data.h
 */

#ifndef THANDOR_PLATFORM_BOOTSTRAP_DATA_H
#define THANDOR_PLATFORM_BOOTSTRAP_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern CommandLineWideArguments g_CommandLineWideArguments; /* 00402028 g_CommandLineWideArguments */

extern uint32_t g_CpuFeatureFlags; /* 004027C4 g_CpuFeatureFlags */

extern uint16_t g_PackageLastErrorPath[256]; /* 00407514 g_PackageLastErrorPath */

extern uint16_t g_FatalErrorDetail1Utf16[256]; /* 00407714 g_FatalErrorDetail1Utf16 */

extern KeyboardReadEventProc *g_KeyboardReadEvent; /* 00417214 g_KeyboardReadEvent */

extern WidePathBuffer256 g_LooseMoviePathPrefix; /* 004A6DA4 g_LooseMoviePathPrefix */

extern SoftwareRasterScanState g_SoftwareRasterScanState; /* 004D11C0 g_SoftwareRasterScanState */

extern int32_t g_ReverseStereoMask; /* 0050B5C0 g_ReverseStereoMask */

extern uint16_t g_UnreferencedLevelPatternUtf16[12]; /* 0050D9DE g_UnreferencedLevelPatternUtf16: UTF-16 L"level\\*.lev" after the save pattern; no code reference found */

extern uint16_t g_UnreferencedCampaignPatternUtf16[12]; /* 0050D9F6 g_UnreferencedCampaignPatternUtf16: UTF-16 L"level\\*.cgn"; no code reference found */

extern uint16_t u_texte_techno_str_0050dec4[17]; /* 0050DEC4 u_texte_techno_str_0050dec4 */

extern uint16_t u_daten_hex_0050e054[10]; /* 0050E054 u_daten_hex_0050e054 */

extern uint16_t u_stat_hex_0050e082[9]; /* 0050E082 u_stat_hex_0050e082 */

extern uint16_t u_texte_neterror_str_0050f104[19]; /* 0050F104 u_texte_neterror_str_0050f104 */

extern void *g_GameStatTableImage; /* 00512D6C g_GameStatTableImage */

extern GameDataAuxState g_GameDataAuxState; /* 00512D70 g_GameDataAuxState */

extern uint16_t g_UnreferencedArmyTexturePathUtf16[13]; /* 0051B394 g_UnreferencedArmyTexturePathUtf16: UTF-16 L"army0000.gfx" after g_AiCommandGenerationRetainedTarget; no code reference found */

extern char g_UnreferencedArmyTag[5]; /* 0051B3AE g_UnreferencedArmyTag: char "ARMY" after the army0000.gfx string; no code reference found; followed by 0x90 fill */

extern uint32_t g_FrontendPlayerMessageBuffers; /* 0054591C g_FrontendPlayerMessageBuffers */

extern uint16_t u_texte_hilfe_str_00545b34[16]; /* 00545B34 u_texte_hilfe_str_00545b34 */

extern uint16_t u_texte_menue_str_00545ba0[16]; /* 00545BA0 u_texte_menue_str_00545ba0 */

extern uint16_t u_texte_level_str_00545bc0[16]; /* 00545BC0 u_texte_level_str_00545bc0 */

extern uint16_t u_texte_inhalt_str_00545be0[17]; /* 00545BE0 u_texte_inhalt_str_00545be0 */

extern uint32_t g_FrontendPlayerListRows[8]; /* 0054DDB0 g_FrontendPlayerListRows: pointers (as uint32_t) to the eight 0x80-byte lobby player list rows */

extern uint16_t *g_InGameFactionStatusTextScratchUtf16; /* 0054FBD4 g_InGameFactionStatusTextScratchUtf16 */

extern uint16_t u_texte_help_str_00563170[15]; /* 00563170 u_texte_help_str_00563170 */

extern uint16_t u_texte_tastatur_str_005631b8[19]; /* 005631B8 u_texte_tastatur_str_005631b8 */

extern uint16_t g_FrontendDebugOverlayTextSlot00Utf16[16]; /* 00563334 g_FrontendDebugOverlayTextSlot00Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot01Utf16[16]; /* 00563354 g_FrontendDebugOverlayTextSlot01Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot02Utf16[16]; /* 00563374 g_FrontendDebugOverlayTextSlot02Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot03Utf16[16]; /* 00563394 g_FrontendDebugOverlayTextSlot03Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot04Utf16[16]; /* 005633B4 g_FrontendDebugOverlayTextSlot04Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot05Utf16[16]; /* 005633D4 g_FrontendDebugOverlayTextSlot05Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot06Utf16[16]; /* 005633F4 g_FrontendDebugOverlayTextSlot06Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot07Utf16[16]; /* 00563414 g_FrontendDebugOverlayTextSlot07Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot08Utf16[16]; /* 00563434 g_FrontendDebugOverlayTextSlot08Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot09Utf16[16]; /* 00563454 g_FrontendDebugOverlayTextSlot09Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot12Utf16[16]; /* 005634B4 g_FrontendDebugOverlayTextSlot12Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot13Utf16[32]; /* 005634D4 g_FrontendDebugOverlayTextSlot13Utf16: owns the unnamed 0x20 bytes up to 0x563514 (elapsed time can exceed 16 units) */

extern uint32_t g_DataPackageHandle; /* 00572AC0 g_DataPackageHandle */

extern uint32_t g_ModelPackageHandle; /* 00572AC4 g_ModelPackageHandle */

extern uint32_t g_GraphicsPackageHandle; /* 00572AC8 g_GraphicsPackageHandle */

extern uint32_t g_MoviePackageHandle; /* 00572AD0 g_MoviePackageHandle */

extern uint32_t g_LevelPackageHandle; /* 00572AD4 g_LevelPackageHandle */

extern uint32_t g_IntroMoviePendingTicks; /* 00572B00 g_IntroMoviePendingTicks */

extern uint32_t g_InstallRegistryKeyHandle; /* 00572B04 g_InstallRegistryKeyHandle */

extern uint32_t g_InstallRegistryValueDataCapacityBytes; /* 00572B08 g_InstallRegistryValueDataCapacityBytes: uint32_t: RegQueryValueExA lpcbData for the install "CD" value, initially 256 (size of g_InstallRegistryValueDataA); platform/bootstrap/runtime.c */

extern uint32_t g_InstallRegistryValueType; /* 00572B0C g_InstallRegistryValueType */

extern uint8_t g_InstallRegistryValueDataA[256]; /* 00572B10 g_InstallRegistryValueDataA: RegQueryValueExA data buffer for the install "CD" value (capacity g_InstallRegistryValueDataCapacityBytes) */

extern uint16_t g_InstallDirectoryScratchUtf16[256]; /* 00572C10 g_InstallDirectoryScratchUtf16 */

extern uint16_t u_Thandor_00572e10[8]; /* 00572E10 u_Thandor_00572e10 */

extern char s_Software_Planet4_Thandor_00572e20[25]; /* 00572E20 s_Software_Planet4_Thandor_00572e20 */

extern char g_InstallRegistryValueNameCD[3]; /* 00572E39 s_InstallRegistryValueNameCD: registry value "CD" */

/* 00572E3C g_ScreenshotFileNameUtf16: "screen00.pcx" with its two-digit counter at code units 6 and 7 */
extern uint16_t g_ScreenshotFileNameUtf16[13];

extern uint16_t u_daten_pck_00572e56[10]; /* 00572E56 u_daten_pck_00572e56 */

extern uint16_t u_modelle_pck_00572e6a[12]; /* 00572E6A u_modelle_pck_00572e6a */

extern uint16_t u_graphik_pck_00572e82[12]; /* 00572E82 u_graphik_pck_00572e82 */

extern uint16_t u_sound_pck_00572e9a[10]; /* 00572E9A u_sound_pck_00572e9a */

extern uint16_t u_filme_pck_00572eae[10]; /* 00572EAE u_filme_pck_00572eae */

extern uint16_t u_level_pck_00572ec2[10]; /* 00572EC2 u_level_pck_00572ec2 */

extern PatchArchivePathTemplate18 g_PatchArchivePathTemplateUtf16; /* 00572ED6 g_PatchArchivePathTemplateUtf16 */

extern LevelArchivePathTemplate18 g_LevelArchivePathTemplateUtf16; /* 00572EEE g_LevelArchivePathTemplateUtf16 */

extern uint16_t u_sound_button0_sam_00572f06[18]; /* 00572F06 u_sound_button0_sam_00572f06 */

extern uint16_t u_sound_button1_sam_00572f2a[18]; /* 00572F2A u_sound_button1_sam_00572f2a */

extern uint16_t u_sound_button2_sam_00572f4e[18]; /* 00572F4E u_sound_button2_sam_00572f4e */

extern uint16_t u_sound_button3_sam_00572f72[18]; /* 00572F72 u_sound_button3_sam_00572f72 */

extern uint16_t u_sound_button4_sam_00572f96[18]; /* 00572F96 u_sound_button4_sam_00572f96 */

extern uint16_t u_sound_button5_sam_00572fba[18]; /* 00572FBA u_sound_button5_sam_00572fba */

extern uint16_t u_sound_button6_sam_00572fde[18]; /* 00572FDE u_sound_button6_sam_00572fde */

extern uint16_t u_gfx_panel_stat_gfx_00573002[19]; /* 00573002 u_gfx_panel_stat_gfx_00573002 */

extern uint16_t u_flm_intro0_flm_00573046[15]; /* 00573046 u_flm_intro0_flm_00573046 */

extern char g_CommandLineOptionNoIntro[8]; /* 00573064 g_CommandLineOptionNoIntro */

extern DynamicModuleEntry g_DynamicModules[16]; /* 00573EF0 g_DynamicModules */

extern uint32_t g_DynamicModuleCount; /* 00573F70 g_DynamicModuleCount */

extern DynamicApiBinding g_BootstrapApiBindings[9]; /* 00573F74 g_BootstrapApiBindings: 8 bindings + the all-zero terminator [8] that ends the DynAPI_Bootstrap scan */

extern char g_Kernel32ModuleName[9]; /* 005744A4 sz_KERNEL32 */

extern char g_WinmmModuleName[6]; /* 005744AE sz_WINMM */

extern char g_Advapi32ModuleName[9]; /* 005744D2 sz_ADVAPI32 */

extern char dynapi_9[13]; /* 005744EC dynapi_9 */

extern char g_BootstrapApiName_FreeLibrary[12]; /* 005744FA dynapi_10 */

extern char g_BootstrapApiName_RegOpenKeyExA[14]; /* 00574506 dynapi_11 */

extern char g_BootstrapApiName_RegQueryValueExA[17]; /* 00574514 dynapi_12 */

extern char g_BootstrapApiName_RegCloseKey[12]; /* 00574526 dynapi_13 */

extern char g_BootstrapApiName_timeSetEvent[13]; /* 00574532 dynapi_14 */

extern char g_BootstrapApiName_timeKillEvent[14]; /* 00574540 dynapi_15 */

extern char g_BootstrapApiName_mciSendCommandA[16]; /* 0057454E dynapi_16 */

extern void *g_GlideTextureRefreshHandlers[3]; /* 0057ED48 g_GlideTextureRefreshHandlers: void *[3]: pointers to the GrVertex records g_GlideVertices[0..2]; no code reads it (name is historical) */

extern GlideTextureInfo g_GlideTextureInfo256Argb4444; /* 0057EDB4 g_GlideTextureInfo256Argb4444: GlideTextureInfo (GrTexInfo): {LOD 8, LOD 8, aspect 1:1, GR_TEXFMT_ARGB_4444, NULL}; not referenced by address */

extern char g_CommandLineOptionSound[6]; /* 00582F28 g_CommandLineOptionSound */

extern HINSTANCE g_hInstance; /* 005856B0 g_hInstance */

extern HWND g_MainWindow; /* 005856B4 g_MainWindow */

extern uint32_t g_AppActive; /* 005856BC g_AppActive: uint32_t: WM_ACTIVATEAPP wParam (application active flag), initially 1; platform/bootstrap/runtime.c */

extern Win32MainMessageStorage g_MainMessageStorage; /* 005856C0 g_MainMessageStorage */

extern CommandLineArgumentMirrorState500 g_CommandLine; /* 0058581C g_CommandLine */

extern char sz_MainWindowTitle[15]; /* 00585D1C sz_MainWindowTitle */

extern char sz_MainWindowClass[17]; /* 00585D2B sz_MainWindowClass */

#endif
