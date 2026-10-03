/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/bootstrap/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_BOOTSTRAP_RUNTIME_H
#define THANDOR_PLATFORM_BOOTSTRAP_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: platform/bootstrap/runtime. */

/* Game data tables (GameData_ResetDefaults, GameData_LoadExternalTables, Game_LoadCoreAssets). */
#define GAME_FACTION_IMAGE_BYTES 0x3A20 /* sizeof(GameFactionRuntimeImage): 8 records of 0x740 bytes + 0x20 tail;
                                           daten.hex holds exactly this image */
#define GAME_STAT_TABLE_BYTES 0x38000 /* g_GameStatTableImage (stat.hex); the last dword is 0xFFFFFFFF */
#define OLD_UNIT_PRIMARY_TABLE_BYTES 0x4000 /* g_OldUnitPrimaryTable (oldunit.hex after the record count) */
#define OLD_UNIT_SECONDARY_TABLE_BYTES 0x100 /* g_OldUnitSecondaryTable (oldunit.hex after the primary table) */
/* Buffers Game_LoadCoreAssets allocates */
#define FRONTEND_PLAYER_LIST_ROW_COUNT 8 /* entries of g_FrontendPlayerListRows */
#define FRONTEND_PLAYER_LIST_ROW_BYTES 0x80 /* one row buffer of the lobby player list */
#define INGAME_FACTION_STATUS_TEXT_BYTES 0x2000 /* g_InGameFactionStatusTextScratchUtf16 */
#define INGAME_PLAYER_LIST_TEXT_BYTES 0x160 /* g_InGamePlayerListTextScratchUtf16 */
#define SELECTION_PLAYER_BLOCK_COUNT 8 /* g_SelectionPlayerBlocks, see g_SelectionPlayerRuntimeBlockPointers[8] */
#define FRONTEND_PLAYER_RUNTIME_RECORD_ALLOC_COUNT 8 /* records in g_FrontendPlayerRuntimeBlocks (0x9D80 bytes) */
#define CORE_ASSET_SCRATCH_SLICE_COUNT 7 /* g_CoreAssetScratchSlice0..6, one zeroed allocation */
#define CORE_ASSET_SCRATCH_SLICE_BYTES 0x200
/* CPU_DetectFeatures */
#define CPUID_LEAF_VERSION_INFO 1
#define CPUID_EDX_MMX 0x00800000 /* CPUID leaf 1, EDX bit 23 */
#define CPU_FEATURE_MMX 0x1 /* bit of g_CpuFeatureFlags */
/* DynDLL_Load: capacity of g_DynamicModules */
#define DYNAMIC_MODULE_CAPACITY 16
/* ProcessEntry sets this display mode first (with PERSISTENT_DEFAULT_BITS_PER_PIXEL); Game_Run switches to the
   saved mode when it differs */
#define GAME_START_DISPLAY_WIDTH 640
#define GAME_START_DISPLAY_HEIGHT 480
/* GameRuntime_InitializeSpatialAudioAndRendering: packets of the global primitive queue */
#define GAME_PRIMITIVE_QUEUE_PACKET_COUNT 0xA000
/* Text pages Game_LoadCoreAssets loads (TextResourcePage_Load), named after their files in texte\ */
#define GAME_TEXT_PAGE_NETERROR 0xFF /* neterror.str */
#define GAME_TEXT_PAGE_HELP 0x18 /* help.str */
#define GAME_TEXT_PAGE_HILFE 0x20 /* hilfe.str */
#define GAME_TEXT_PAGE_MENUE 0x21 /* menue.str */
#define GAME_TEXT_PAGE_LEVEL 0x22 /* level.str */
#define GAME_TEXT_PAGE_INHALT 0x23 /* inhalt.str */
#define GAME_TEXT_PAGE_TASTATUR 0x24 /* tastatur.str */
#define GAME_TEXT_PAGE_TECHNO 0x30 /* techno.str (page 0x30 is later the loaded level's text page) */
/* Text 2 of tastatur.str: the mouse help whose inline icons come from the cursor texture */
#define TEXT_ID_MOUSE_HELP 0x2402
/* Slots of g_BootstrapApiBindings, bound by DynAPI_Bootstrap (order of the image data table) */
#define BOOTSTRAP_API_LOAD_LIBRARY_A 0 /* KERNEL32 */
#define BOOTSTRAP_API_FREE_LIBRARY 1 /* KERNEL32 */
#define BOOTSTRAP_API_TIME_SET_EVENT 2 /* WINMM */
#define BOOTSTRAP_API_TIME_KILL_EVENT 3 /* WINMM */
#define BOOTSTRAP_API_MCI_SEND_COMMAND_A 4 /* WINMM */
#define BOOTSTRAP_API_REG_OPEN_KEY_EX_A 5 /* ADVAPI32 */
#define BOOTSTRAP_API_REG_QUERY_VALUE_EX_A 6 /* ADVAPI32 */
#define BOOTSTRAP_API_REG_CLOSE_KEY 7 /* ADVAPI32 */

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00585D40 */
void __cdecl ProcessEntry(void);

/* 0x00512E70: 0, or the allocator's error code */
uint32_t GameData_ResetDefaults(void);

/* 0x00512F60 */
bool GameData_LoadExternalTables(void);

/* 0x00573BC0: 0 when resolved into *destination, else FATAL_ERROR_DLL_PROCEDURE_MISSING */
uint32_t DynAPI_Resolve(void **destination,HINSTANCE module,char *procedureName);

/* 0x00573C50: the loaded module, or NULL (callers report FATAL_ERROR_DLL_LOAD_FAILED) */
HINSTANCE DynDLL_Load(char *moduleName);

/* 0x00573CD0 */
uint32_t DynDLL_Unload(char *moduleName);

/* 0x00573D40: true when the binding was found and loaded */
bool BootstrapApi_ResolveBindingByDestination(void **destination);

/* 0x00573EB0 */
void DynDLL_UnloadAll(void);

/* 0x00585F50 */
LRESULT __stdcall MainWindowProc(HWND hwnd,Win32WindowMessageId message,WPARAM wParam,LPARAM lParam);

/* 0x00587370 */
uint32_t __cdecl CPU_DetectFeatures(void);

/* 0x00573070 */
void __cdecl Game_Run(void);

/* 0x0050BB10: 0, or the error code of the first failing step */
uint32_t __cdecl GameRuntime_InitializeSpatialAudioAndRendering(void);

/* 0x00573140 */
uint32_t __cdecl Game_LoadCoreAssets(void);

/* 0x005739D0 */
bool Game_PlayIntroMovies(void);

/* 0x00573DB0: 0, or a FATAL_ERROR_* code */
uint32_t DynAPI_Bootstrap(void);

/* 0x00586110 */
uint8_t *CommandLine_FindOption(CommandLineOptionLengthBytes length,char *option);

/* 0x00586170 */
void CommandLine_Parse(void);

/*
g_BootstrapApiBindings (0x00573F74) is resolved at startup from {name, module} pairs; each slot then
holds the __stdcall entry of that Win32 API. Calls must use these types: the Ghidra `code` type is
cdecl and would leave the stack unbalanced after every call.
*/
typedef HINSTANCE (__stdcall *BootstrapLoadLibraryAProc)(char *moduleName);                 /* [0] */
typedef BOOL (__stdcall *BootstrapFreeLibraryProc)(HINSTANCE module);                        /* [1] */
typedef uint32_t (__stdcall *BootstrapTimeSetEventProc)(uint32_t delayMs, uint32_t resolutionMs, void *callback,
                                                     uint32_t user, uint32_t flags);             /* [2] */
typedef uint32_t (__stdcall *BootstrapTimeKillEventProc)(uint32_t timerId);                        /* [3] */
typedef uint32_t (__stdcall *BootstrapMciSendCommandAProc)(uint32_t device, uint32_t message, uint32_t flags,
                                                        uint32_t params);                     /* [4] */
typedef long (__stdcall *BootstrapRegOpenKeyExAProc)(uint32_t key, char *subKey, uint32_t options, uint32_t access,
                                                     void *result);                       /* [5] */
typedef long (__stdcall *BootstrapRegQueryValueExAProc)(uint32_t key, void *valueName, uint32_t *reserved,
                                                        void *type, void *data, void *size); /* [6] */
typedef long (__stdcall *BootstrapRegCloseKeyProc)(uint32_t key);                               /* [7] */

#endif /* THANDOR_PLATFORM_BOOTSTRAP_RUNTIME_H */
