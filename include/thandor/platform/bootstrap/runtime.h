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
/* CPU_DetectFeatures */
#define CPUID_LEAF_VERSION_INFO 1
#define CPUID_EDX_MMX 0x00800000 /* CPUID leaf 1, EDX bit 23 */
#define CPU_FEATURE_MMX 0x1 /* bit of g_CpuFeatureFlags */
/* DynDLL_Load: capacity of g_DynamicModules */
#define DYNAMIC_MODULE_CAPACITY 16

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00585D40 */
void __cdecl ProcessEntry(void);

/* 0x00512E70 */
StatusResult GameData_ResetDefaults(void);

/* 0x00512F60 */
bool GameData_LoadExternalTables(void);

/* 0x00573BC0 */
DynApiResolveResult DynAPI_Resolve(void **destination,HINSTANCE module,char *procedureName);

/* 0x00573C50 */
DllLoadResult DynDLL_Load(char *moduleName);

/* 0x00573CD0 */
uint32_t DynDLL_Unload(char *moduleName);

/* 0x00573D40 */
StatusResult BootstrapApi_ResolveBindingByDestination(void **destination);

/* 0x00573EB0 */
void DynDLL_UnloadAll(void);

/* 0x00585F50 */
LRESULT __stdcall MainWindowProc(HWND hwnd,Win32WindowMessageId message,WPARAM wParam,LPARAM lParam);

/* 0x00587370 */
uint32_t __cdecl CPU_DetectFeatures(void);

/* 0x00573070 */
void __cdecl Game_Run(void);

/* 0x0050BB10 */
StatusResult __cdecl GameRuntime_InitializeSpatialAudioAndRendering(void);

/* 0x00573140 */
uint32_t __cdecl Game_LoadCoreAssets(void);

/* 0x005739D0 */
bool Game_PlayIntroMovies(void);

/* 0x00573DB0 */
StatusResult DynAPI_Bootstrap(void);

/* 0x00586110 */
CommandLineOptionResult CommandLine_FindOption(CommandLineOptionLengthBytes length,char *option);

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
