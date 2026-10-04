/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/error/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_ERROR_RUNTIME_H
#define THANDOR_CORE_ERROR_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/error/runtime. */

/* The two fatal-error handlers, installed by ErrorSystem_Init. Both take a value and a failure flag and
   return the value unchanged; with the flag set they first treat the value as an error code or message
   and handle it (so callers can wrap a computation: x = FatalError_ExitIfFailed(value, failed)).
   FatalError_ExitIfFailed: always FatalError_Exit, which shows the message box, shuts down and exits.
   FatalError_ReportIfFailed: FatalError_Exit until the UI error state exists
   (ErrorRuntime_InstallUiHandlerAndAllocateState), then FatalErrorRuntime_DispatchPendingError, which
   shows the error in a modal in-game dialog and returns. */
#define FatalError_ExitIfFailed(valueOrError, failed) (g_FatalErrorExitHandler((valueOrError), (failed)))
#define FatalError_ReportIfFailed(valueOrError, failed) (g_FatalErrorReportHandler((valueOrError), (failed)))

/* Error codes handed to the fatal-error dispatcher (FatalError_ExitIfFailed); the code selects
   the message text. Named as they are found. */
#define FATAL_ERROR_CPU_WITHOUT_MMX 0x51 /* ProcessEntry: CPUID reports no MMX (see CPU_DetectFeatures) */
/* DLL binding (DynAPI_Bootstrap, DynAPI_Resolve, DynDLL_Load); the DLL/procedure name is left in
   g_PackageLastErrorPath */
#define FATAL_ERROR_LOADER_MODULE_MISSING 0x0F /* the module of LoadLibraryA itself is not mapped */
#define FATAL_ERROR_DLL_PROCEDURE_MISSING 0x10 /* GetProcAddress failed */
#define FATAL_ERROR_DLL_LOAD_FAILED 0x11 /* LoadLibraryA failed */
/* Subsystem startup (Graphics_Init, DirectSound_Init); names follow the failing step */
#define FATAL_ERROR_DIRECTDRAW_NO_ADAPTER 0x17 /* DirectDrawEnumerateA failed or listed no adapter */
#define FATAL_ERROR_DIRECTDRAW_NO_DISPLAY_MODE 0x18 /* no adapter reported a usable display mode */
#define FATAL_ERROR_DIRECTSOUND_SETUP 0x29 /* primary buffer setup failed; the stage number is left in
                                              g_PackageLastErrorPath */
#define FATAL_ERROR_DIRECTINPUT_SETUP 0x25 /* DirectInputMouse_Init: DirectInputCreateA or a mouse-device
                                              setup call failed; the stage number (0..4) is left in
                                              g_PackageLastErrorPath */
/* Generic failure code returned as a failure by many helpers (package mount/lookup, PCK codec, text copies,
   runtime pools); InGameRuntime_RunSessionUntilExit returns it when the UI root stack runs empty */
#define FATAL_ERROR_GENERAL_FAILURE 0x14
/* Out of memory: the arena allocation failed (Package_LoadEntry, Resource_Load and the FileSystem whole-file
   loaders then leave the requested byte count in g_FatalErrorDetail1Utf16); the package loaders also return
   it for entries whose packed size exceeds PACKAGE_SCRATCH_BUFFER_BYTES */
#define FATAL_ERROR_OUT_OF_MEMORY 0x05
/* Network socket setup/send failed (NetworkFallback_OpenAndBindUdpSocket, NetworkFallback_SendDatagram); the
   WSAGetLastError code is left in g_PackageLastErrorPath */
#define FATAL_ERROR_NETWORK_SOCKET 0x2A
/* No network backend: the default g_NetworkBackendSlot0/Slot2 entries (NetworkBackendFallback_Slot0/2, left in
   place when Network_Init could not start WinSock) return it; the frontend reports it when no backend opens */
#define FATAL_ERROR_NETWORK_UNAVAILABLE 0x2B
/* DirectSound_CreateSampleVoiceSet: the asset is not a 'sam' of format version 0x10000 (a failing
   secondary-buffer step there returns FATAL_ERROR_DIRECTSOUND_SETUP) */
#define FATAL_ERROR_SOUND_SAMPLE_INVALID 0x4A
/* Arena heap (core/memory/allocator): no free block is large enough (ArenaHeap_Alloc,
   ArenaHeap_AllocLargestFreeBlock; the largest free payload size is left in g_PackageLastErrorPath).
   A corrupt block chain returns ARENA_HEAP_CORRUPT instead. */
#define FATAL_ERROR_ARENA_EXHAUSTED 0x12
/* Win32 file layer (platform/filesystem/win32, the g_FileSystem* table); the path is left in
   g_PackageLastErrorPath. Named after the operations that return them. */
#define FATAL_ERROR_FILE_ACCESS_FAILED 0x01 /* CreateFileA, DeleteFileA, MoveFileA, CopyFileA or GetFileTime failed */
#define FATAL_ERROR_FILE_READ_FAILED 0x06 /* ReadFile transferred fewer bytes than requested */
#define FATAL_ERROR_FILE_WRITE_INCOMPLETE 0x07 /* WriteFile succeeded but wrote fewer bytes than requested */
#define FATAL_ERROR_FILE_WRITE_FAILED 0x08 /* WriteFile failed; also CreateDirectoryA */
#define FATAL_ERROR_FILE_SEEK_FAILED 0x09 /* SetFilePointer failed */
#define FATAL_ERROR_SET_DIRECTORY_FAILED 0x0A /* SetCurrentDirectoryA failed */
#define FATAL_ERROR_REMOVE_DIRECTORY_FAILED 0x0B /* RemoveDirectoryA failed */
/* Level loading (gameplay/session/level.c); the level path is left in g_PackageLastErrorPath */
#define FATAL_ERROR_LEVEL_ASSET_INVALID 0x39 /* not a 'lev' asset of converter version 0x70001 */
#define FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES 0x3A /* the EFF/SHT/MDL/ARM lists name 0x200 or more files */
#define FATAL_ERROR_TECHNOLOGY_ASSET_INVALID 0x4F /* the level's technology file is not a 'tec' asset of
                                                     converter version 0x20000 */
/* Terrain visuals (world/terrain/visuals.c) */
#define FATAL_ERROR_FIELD_ASSET_INVALID 0x38 /* TerrainVisualResources_Load*: the field grid is not an 'fld' asset of
                                                converter version 0x60006 */
/* Model definitions (assets/model/definitions.c) */
#define FATAL_ERROR_MODEL_ASSET_INVALID 0x3D /* ModelAsset_PrepareRecords: not an 'mdl' asset of converter version
                                                0x8000A */
#define FATAL_ERROR_MODEL_DEFINITION_MISSING 0x3E /* ModelDefinitionRegistry_FindById: the id is not in the
                                                     768-slot registry */
#define FATAL_ERROR_MODEL_REGISTRY_FULL 0x3F /* ModelDefinition_RegisterAndResolveReferences: all 768 slots are
                                                taken (768 is left in g_PackageLastErrorPath) */
#define FATAL_ERROR_MODEL_ID_DUPLICATE 0x4B /* ModelDefinition_RegisterAndResolveReferences: the id is already
                                               registered (the id is left in g_PackageLastErrorPath) */
#define FATAL_ERROR_SPRITE_ASSET_INVALID 0x36 /* SpriteAsset_RegisterAndRelocatePointers: not an 'spr' asset of
                                                 converter version 0x20007 */
#define FATAL_ERROR_PALETTE_ASSET_INVALID 0x35 /* GraphicsPaletteAsset_Validate: not a 'pal' asset */
/* ROM registry (assets/rom/runtime) */
#define FATAL_ERROR_ROM_REGISTRY_FULL 0x3B /* RomAssetRecord_RegisterAndRelocate: all 256 slots are taken; the
                                              path "engine\zentrale.rom" is left in g_PackageLastErrorPath */
#define FATAL_ERROR_ROM_RECORD_NOT_REGISTERED 0x3C /* RomRegistry_FindRecordById/FindSlotValueByRecordId miss */
/* Shot and effect catalogs (assets/shot/catalog.c, assets/effect/catalog.c). An invalid asset leaves its path,
   the other codes leave the offending definition id (or registry index / slot count) in g_PackageLastErrorPath. */
#define FATAL_ERROR_SHOT_ASSET_INVALID 0x43 /* not a 'sht' asset of converter version 0x60006 */
#define FATAL_ERROR_SHOT_ID_NOT_FOUND 0x44 /* ShotDefinitionRegistry_FindByIdWithError: id not registered */
#define FATAL_ERROR_SHOT_REGISTRY_FULL 0x45 /* all 256 shot-definition registry slots are taken */
#define FATAL_ERROR_SHOT_TERRAIN_MATERIAL_INVALID 0x46 /* a shot names a terrain material that is out of
                                                          range or not loaded */
#define FATAL_ERROR_EFFECT_ASSET_INVALID 0x47 /* not an 'eff' asset of converter version 0x40007 */
#define FATAL_ERROR_EFFECT_ID_NOT_FOUND 0x48 /* EffectDefinitionRegistry_FindById: id not registered */
#define FATAL_ERROR_EFFECT_REGISTRY_FULL 0x49 /* all 256 effect-definition registry slots are taken */
#define FATAL_ERROR_SHOT_ID_DUPLICATE 0x4D /* a shot definition id is registered twice */
#define FATAL_ERROR_EFFECT_ID_DUPLICATE 0x4E /* an effect definition id is registered twice */
/* Movie_Open: the file is not an 'flm' of converter version 0x20001 (Movie_AdvanceFrame also returns it as a
   failure when no movie is open) */
#define FATAL_ERROR_MOVIE_INVALID 0x30
/* Army catalog (assets/army/catalog.c), same scheme as the shot/effect codes above */
#define FATAL_ERROR_ARMY_ASSET_INVALID 0x40 /* ArmyAsset_PrepareRecords: not an 'arm' asset of converter version
                                               0x20008 (the path is left in g_PackageLastErrorPath) */
#define FATAL_ERROR_ARMY_ID_NOT_FOUND 0x41 /* ArmyAssetRegistry_FindById: id not registered (the id is left in
                                              g_PackageLastErrorPath) */
#define FATAL_ERROR_ARMY_REGISTRY_FULL 0x42 /* all 768 army registry slots are taken */
#define FATAL_ERROR_ARMY_ID_DUPLICATE 0x4C /* ArmyAssetRecord_RegisterAndRelocate: an army id is registered twice
                                              (the id is left in g_PackageLastErrorPath) */
/* Display mode switch (GraphicsDirectDraw_ApplyDisplayModeAndCreateResources); the number of completed setup
   steps is left in g_PackageLastErrorPath. Named after the failing step. */
#define FATAL_ERROR_DIRECTDRAW_CREATE 0x19 /* DirectDrawCreate, SetCooperativeLevel or the IDirectDraw2 query */
#define FATAL_ERROR_DIRECTDRAW_SET_DISPLAY_MODE 0x1A /* IDirectDraw2::SetDisplayMode */
#define FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES 0x1B /* primary or back surface creation/query */
#define FATAL_ERROR_DIRECTDRAW_PIXEL_FORMAT 0x1C /* GetPixelFormat failed or reported an empty RGB mask */
/* 0x1D..0x21: unused since the software renderer is the only renderer (they were hardware renderer setup errors) */
/* GraphicsTextureSet_AllocateMetadata: an image of a texture set is not a power of two wide and high */
#define FATAL_ERROR_TEXTURE_SIZE_NOT_POWER_OF_TWO 0x2F
/* GraphicsCursor_SetFrameIndex: the frame index is not below g_CursorFrameCount (it returns false) */
#define FATAL_ERROR_CURSOR_FRAME_OUT_OF_RANGE 0x2D
/* Texture sources (graphics/resources/texture): not a 'gfx' asset or the subresource index is out of range
   (GraphicsTextureSource_ConvertPaletteEntries, GraphicsTextureSource_DecomposeSubresourceRegions) */
#define FATAL_ERROR_GFX_ASSET_INVALID 0x2C

/* The fatal-error handlers take an error code (a text id of the error page, below 0x100) or a pointer to a
   rich-text message; a value with no bits above the low byte is a code */
#define FATAL_ERROR_IS_CODE(errorOrValue) (((uintptr_t)(errorOrValue) & ~(uintptr_t)0xff) == 0)
/* FatalError_CopyRichTextToNarrow: nested rich-text streams it follows at most (deeper nesting cuts the text) */
#define FATAL_ERROR_RICHTEXT_NESTING_MAX 64

/* Functions are grouped by semantic ownership. */

void __cdecl ErrorSystem_Init(void);

Bool8 FatalErrorDialog_BlockMissedPointerPress(UiRootNode *root);

int FatalErrorDialog_BlockMissedPointerMotion(UiRootNode *root);

void FatalErrorDialog_DismissAndPopRoot(UiRootNode *rootNode);

uintptr_t FatalErrorRuntime_DispatchPendingError(uintptr_t valueOrError,Bool8 failed);

void ErrorRuntime_InstallUiHandlerAndAllocateState(void);

uintptr_t FatalError_Exit(uintptr_t valueOrError,Bool8 failed);

int FatalError_CopyRichTextToNarrow (TextOutputCapacityBytes capacityBytes,uint8_t *destination,uint16_t *source);

extern FatalErrorPassThroughProc *g_FatalErrorExitHandler;
extern FatalErrorPassThroughProc *g_FatalErrorReportHandler;
extern uint16_t g_ErrorTextIoInitializationFailed[34];

extern uint16_t g_PackageLastErrorPath[256];
extern uint16_t g_FatalErrorDetail1Utf16[256];

#endif /* THANDOR_CORE_ERROR_RUNTIME_H */
