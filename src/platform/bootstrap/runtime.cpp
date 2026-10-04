/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/platform/bootstrap/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

THANDOR_ALIGN(4) CommandLineFindOptionProc *g_CommandLineFindOption = nullptr;

static CommandLineWideArguments g_CommandLineWideArguments = {0};

static uint32_t g_CpuFeatureFlags = 0;

static uint16_t g_TexteTechnoStrPathUtf16[17] = {'t', 'e', 'x', 't', 'e', '\\', 't', 'e', 'c', 'h', 'n', 'o', '.', 's', 't', 'r', 0}; /* L"texte\\techno.str" */

static uint16_t g_TexteNeterrorStrPathUtf16[19] = {'t', 'e', 'x', 't', 'e', '\\', 'n', 'e', 't', 'e', 'r', 'r', 'o', 'r', '.', 's', 't', 'r', 0}; /* L"texte\\neterror.str" */

static uint16_t g_TexteHilfeStrPathUtf16[16] = {'t', 'e', 'x', 't', 'e', '\\', 'h', 'i', 'l', 'f', 'e', '.', 's', 't', 'r', 0}; /* L"texte\\hilfe.str" */

static uint16_t g_TexteMenueStrPathUtf16[16] = {'t', 'e', 'x', 't', 'e', '\\', 'm', 'e', 'n', 'u', 'e', '.', 's', 't', 'r', 0}; /* L"texte\\menue.str" */

static uint16_t g_TexteLevelStrPathUtf16[16] = {'t', 'e', 'x', 't', 'e', '\\', 'l', 'e', 'v', 'e', 'l', '.', 's', 't', 'r', 0}; /* L"texte\\level.str" */

static uint16_t g_TexteInhaltStrPathUtf16[17] = {'t', 'e', 'x', 't', 'e', '\\', 'i', 'n', 'h', 'a', 'l', 't', '.', 's', 't', 'r', 0}; /* L"texte\\inhalt.str" */

static uint16_t g_TexteHelpStrPathUtf16[15] = {'t', 'e', 'x', 't', 'e', '\\', 'h', 'e', 'l', 'p', '.', 's', 't', 'r', 0}; /* L"texte\\help.str" */

static uint16_t g_TexteTastaturStrPathUtf16[19] = {'t', 'e', 'x', 't', 'e', '\\', 't', 'a', 's', 't', 'a', 't', 'u', 'r', '.', 's', 't', 'r', 0}; /* L"texte\\tastatur.str" */

static uintptr_t g_DataPackageHandle = 0;

static uintptr_t g_ModelPackageHandle = 0;

static uintptr_t g_GraphicsPackageHandle = 0;

static uintptr_t g_MoviePackageHandle = 0;

static uintptr_t g_LevelPackageHandle = 0;

/* HKEY (pointer-sized on x64) */
static uintptr_t g_InstallRegistryKeyHandle = 0;

/* uint32_t: RegQueryValueExA lpcbData for the install "CD" value, initially 256 (size of g_InstallRegistryValueDataA) */
static uint32_t g_InstallRegistryValueDataCapacityBytes = 256;

static uint32_t g_InstallRegistryValueType = 0;

/* RegQueryValueExA data buffer for the install "CD" value (capacity g_InstallRegistryValueDataCapacityBytes) */
static uint8_t g_InstallRegistryValueDataA[256] = {0};

static uint16_t g_InstallDirectoryScratchUtf16[256] = {0};

static uint16_t g_ThandorWindowTitleUtf16[8] = {'T', 'h', 'a', 'n', 'd', 'o', 'r', 0}; /* L"Thandor" */

static char g_SoftwarePlanet4ThandorAscii[25] = "Software\\Planet4\\Thandor";

/* registry value "CD" */
static char g_InstallRegistryValueNameCD[3] = "CD";

static uint16_t g_DatenPckPathUtf16[10] = {'d', 'a', 't', 'e', 'n', '.', 'p', 'c', 'k', 0}; /* L"daten.pck" */

static uint16_t g_ModellePckPathUtf16[12] = {'m', 'o', 'd', 'e', 'l', 'l', 'e', '.', 'p', 'c', 'k', 0}; /* L"modelle.pck" */

static uint16_t g_GraphikPckPathUtf16[12] = {'g', 'r', 'a', 'p', 'h', 'i', 'k', '.', 'p', 'c', 'k', 0}; /* L"graphik.pck" */

static uint16_t g_SoundPckPathUtf16[10] = {'s', 'o', 'u', 'n', 'd', '.', 'p', 'c', 'k', 0}; /* L"sound.pck" */

static uint16_t g_FilmePckPathUtf16[10] = {'f', 'i', 'l', 'm', 'e', '.', 'p', 'c', 'k', 0}; /* L"filme.pck" */

static uint16_t g_LevelPckPathUtf16[10] = {'l', 'e', 'v', 'e', 'l', '.', 'p', 'c', 'k', 0}; /* L"level.pck" */

static PatchArchivePathTemplate18 g_PatchArchivePathTemplateUtf16 = {
    .prefixCodeUnits = {0x70, 0x61, 0x74, 0x63, 0x68},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = {'.', 'p', 'c', 'k', 0}}; /* L".pck" */

static LevelArchivePathTemplate18 g_LevelArchivePathTemplateUtf16 = {
    .prefixCodeUnits = {0x6C, 0x65, 0x76, 0x65, 0x6C},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = {'.', 'p', 'c', 'k', 0}}; /* L".pck" */

static uint16_t g_SoundButton0SamPathUtf16[18] = {'s', 'o', 'u', 'n', 'd', '\\', 'b', 'u', 't', 't', 'o', 'n', '0', '.', 's', 'a', 'm', 0}; /* L"sound\\button0.sam" */

static uint16_t g_SoundButton1SamPathUtf16[18] = {'s', 'o', 'u', 'n', 'd', '\\', 'b', 'u', 't', 't', 'o', 'n', '1', '.', 's', 'a', 'm', 0}; /* L"sound\\button1.sam" */

static uint16_t g_SoundButton2SamPathUtf16[18] = {'s', 'o', 'u', 'n', 'd', '\\', 'b', 'u', 't', 't', 'o', 'n', '2', '.', 's', 'a', 'm', 0}; /* L"sound\\button2.sam" */

static uint16_t g_SoundButton3SamPathUtf16[18] = {'s', 'o', 'u', 'n', 'd', '\\', 'b', 'u', 't', 't', 'o', 'n', '3', '.', 's', 'a', 'm', 0}; /* L"sound\\button3.sam" */

static uint16_t g_SoundButton4SamPathUtf16[18] = {'s', 'o', 'u', 'n', 'd', '\\', 'b', 'u', 't', 't', 'o', 'n', '4', '.', 's', 'a', 'm', 0}; /* L"sound\\button4.sam" */

static uint16_t g_SoundButton5SamPathUtf16[18] = {'s', 'o', 'u', 'n', 'd', '\\', 'b', 'u', 't', 't', 'o', 'n', '5', '.', 's', 'a', 'm', 0}; /* L"sound\\button5.sam" */

static uint16_t g_SoundButton6SamPathUtf16[18] = {'s', 'o', 'u', 'n', 'd', '\\', 'b', 'u', 't', 't', 'o', 'n', '6', '.', 's', 'a', 'm', 0}; /* L"sound\\button6.sam" */

static uint16_t g_GfxPanelStatGfxPathUtf16[19] = {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 's', 't', 'a', 't', '.', 'g', 'f', 'x', 0}; /* L"gfx\\panel\\stat.gfx" */

static uint16_t g_FlmIntro0FlmPathUtf16[15] = {'f', 'l', 'm', '\\', 'i', 'n', 't', 'r', 'o', '0', '.', 'f', 'l', 'm', 0}; /* L"flm\\intro0.flm" */

static char g_CommandLineOptionNoIntro[8] = "NOINTRO";

static DynamicModuleEntry g_DynamicModules[16] = {};

static uint32_t g_DynamicModuleCount = 0;

static char g_Kernel32ModuleName[9] = "KERNEL32";

static char g_Advapi32ModuleName[9] = "ADVAPI32";

static char dynapi_9[13] = "LoadLibraryA";

static char g_BootstrapApiName_FreeLibrary[12] = "FreeLibrary";

static char g_BootstrapApiName_RegOpenKeyExA[14] = "RegOpenKeyExA";

static char g_BootstrapApiName_RegQueryValueExA[17] = "RegQueryValueExA";

static char g_BootstrapApiName_RegCloseKey[12] = "RegCloseKey";

static char g_CommandLineOptionSound[6] = "SOUND";

static CommandLineArgumentMirrorState500 g_CommandLine = {0};

static char sz_MainWindowTitle[15] = " thandor  (TG)";

WidePathBuffer256 g_LooseMoviePathPrefix = {0};

uint16_t g_DatenHexPathUtf16[10] = {'d', 'a', 't', 'e', 'n', '.', 'h', 'e', 'x', 0}; /* L"daten.hex" */

uint16_t g_StatHexPathUtf16[9] = {'s', 't', 'a', 't', '.', 'h', 'e', 'x', 0}; /* L"stat.hex" */

void *g_GameStatTableImage = nullptr;

GameDataAuxState g_GameDataAuxState = {0};

UPtr32 g_FrontendPlayerListRows[8] = {
    0, /* row 0 */
    0, /* row 1 */
    0, /* row 2 */
    0, /* row 3 */
    0, /* row 4 */
    0, /* row 5 */
    0, /* row 6 */
    0, /* row 7 */
};

uint32_t g_IntroMoviePendingTicks = 0;

/* "screen00.pcx" with its two-digit counter at code units 6 and 7 */
uint16_t g_ScreenshotFileNameUtf16[13] = {'s', 'c', 'r', 'e', 'e', 'n', '0', '0', '.', 'p', 'c', 'x', 0}; /* L"screen00.pcx" */

/* 5 bindings, then the all-zero terminator [5] that ends the DynAPI_Bootstrap scan. The original also bound
   WINMM's timeSetEvent, timeKillEvent (its periodic timers, now SDL timers) and mciSendCommandA (never called). */
DynamicApiBinding g_BootstrapApiBindings[6] = {
        /* 0 */ {.destination = THANDOR_PTR(&dynapi_9), .moduleName = THANDOR_PTR(g_Kernel32ModuleName)},
        /* 1 */ {.destination = THANDOR_PTR(g_BootstrapApiName_FreeLibrary), .moduleName = THANDOR_PTR(g_Kernel32ModuleName)},
        /* 2 */ {.destination = THANDOR_PTR(g_BootstrapApiName_RegOpenKeyExA), .moduleName = THANDOR_PTR(g_Advapi32ModuleName)},
        /* 3 */ {
        .destination = THANDOR_PTR(g_BootstrapApiName_RegQueryValueExA),
        .moduleName = THANDOR_PTR(g_Advapi32ModuleName)},
        /* 4 */ {.destination = THANDOR_PTR(g_BootstrapApiName_RegCloseKey), .moduleName = THANDOR_PTR(g_Advapi32ModuleName)},
        /* 5: terminator */ {}};

HWND g_MainWindow = nullptr;

/* Implementation ownership: platform/bootstrap/runtime. */

/* ProcessEntry once the main window exists: initialises every subsystem, sets the initial 640x480 display
   mode from the saved adapter and colour depth, runs the game and shuts down. Any failed step ends in the
   fatal-error dispatcher; a missing sound device is tolerated when -SOUND is not on the command line. The
   platform backend is SDL3 (src/platform/sdl3), in place of the original's DirectDraw, DirectInput and DirectSound.
*/
static void ProcessEntry_RunGame()

{
  uint32_t networkResult;
  uint32_t bootstrapError;
  uint32_t graphicsError;
  uint32_t bitsPerPixel;
  uint32_t adapterIndex;
  uint32_t mouseInitError;
  uint32_t soundError;
  uint32_t checkedValue;
  uint32_t displayModeError;
  uint32_t displayWidth;
  uint32_t displayHeight;

  ArenaHeap_Init();
  FileSystem_Init();
  Locale_Init();
  ErrorSystem_Init();
  if (g_CpuFeatureFlags == 0) {
    FatalError_ExitIfFailed(FATAL_ERROR_CPU_WITHOUT_MMX,true);
  }
  bootstrapError = DynAPI_Bootstrap();
  checkedValue = FatalError_ExitIfFailed(bootstrapError,bootstrapError != 0);
  /* the original's TimerSystem_Init installed the WinMM timers and the Win32 message pump here (it cannot fail);
     the SDL timers and the SDL event pump take their place */
  SdlPlatform_InstallTimersAndPump();
  checkedValue = FatalError_ExitIfFailed(checkedValue,false);
  graphicsError = SdlVideo_Init();
  FatalError_ExitIfFailed(graphicsError,graphicsError != 0);
  if (!SdlInput_Init(&mouseInitError)) {
    FatalError_ExitIfFailed(mouseInitError,true);
  }
  soundError = SdlAudio_Init();
  Thandor_Log("SdlAudio_Init: %s", soundError != 0 ? "failed (continuing without sound)" : "ok");
  if (soundError != 0) {
    /* without a sound device the game only stops when -SOUND demands sound */
    if (CommandLine_FindOption(sizeof g_CommandLineOptionSound,g_CommandLineOptionSound) != nullptr) {
      FatalError_ExitIfFailed(soundError,true);
    }
  }
  networkResult = Network_Init();
  /* the original Network_Init reports success on every path, failures included, so a missing
     WinSock is never fatal: the check below always passes. */
  FatalError_ExitIfFailed(networkResult,false);
  PersistentSettings_Load();
  displayWidth = GAME_START_DISPLAY_WIDTH;
  displayHeight = GAME_START_DISPLAY_HEIGHT;
  bitsPerPixel = PersistentSettings_Read(PERSISTENT_DEFAULT_BITS_PER_PIXEL,PERSISTENT_SETTING_BITS_PER_PIXEL);
  /* the adapters are the renderers; the choice is kept in PERSISTENT_SETTING_RENDERER, not in the original's
     adapter index (SdlVideo_SavedAdapterIndex) */
  adapterIndex = SdlVideo_SavedAdapterIndex();
  if (!g_GraphicsSetDisplayMode(adapterIndex,bitsPerPixel,displayHeight,displayWidth,&displayModeError)) {
    FatalError_ExitIfFailed(displayModeError,true);
  }
  UiRuntime_Initialize();
  Game_Run();
  Runtime_Shutdown();
}


/* Process entry: raises the process to real-time priority, creates the full-screen main window (only
   one instance may run), runs the game (ProcessEntry_RunGame) and ends the process.
*/
void __cdecl ProcessEntry()

{
  HANDLE processHandle;
  HANDLE threadHandle;

  processHandle = GetCurrentProcess();
  SetPriorityClass(processHandle,DebugHook_ProcessPriorityClass(REALTIME_PRIORITY_CLASS));
  threadHandle = GetCurrentThread();
  SetThreadPriority(threadHandle,THREAD_PRIORITY_NORMAL);
  CommandLine_Parse();
  /* the window has SDL's class, so the running instance is found by its title */
  if ((FindWindowA(nullptr,sz_MainWindowTitle) == nullptr) || DebugHook_AllowSecondInstance()) {
    if (SdlPlatform_CreateMainWindow(sz_MainWindowTitle)) {
      ProcessEntry_RunGame();
      SdlPlatform_Quit();
    }
  }
  ExitProcess(0);
}


/* Resets the game data to the defaults of a new game: clears the auxiliary state and the eight faction
   records, gives every faction its own capability bit, the base technology, a rotated relation pattern
   (0xF for itself, 1 for everyone else) and the starting economy limits, and replaces the stat table with a
   fresh zeroed one. Returns 0, or the allocator's (non-zero) error code when the stat table cannot be
   allocated (the old one then stays).
*/
uint32_t GameData_ResetDefaults()

{
  FactionCapabilityFlags *capabilityFlagsSlot;
  void *previousStatTable;
  int remainingCount;
  uint32_t relationStatePattern;
  uint32_t factionBit;
  GameFactionRuntimeRecord *factionRecord;
  uint32_t *dwordCursor;
  uint32_t *statTableCursor;
  uint32_t allocError;
  void *allocPayload;

  remainingCount = sizeof g_GameDataAuxState.pairPressureMatrix8x8 / 4;
  dwordCursor = g_GameDataAuxState.pairPressureMatrix8x8;
  for (; remainingCount != 0; remainingCount--) {
    *dwordCursor = 0;
    dwordCursor++;
  }
  /* clears the eight records (0x3A00 bytes) dword by dword, not the image tail */
  dwordCursor = (uint32_t *)g_GameFactionRuntimeImage.records;
  for (remainingCount = sizeof g_GameFactionRuntimeImage.records / 4; remainingCount != 0; remainingCount--) {
    *dwordCursor = 0;
    dwordCursor++;
  }
  factionRecord = g_GameFactionRuntimeImage.records;
  remainingCount = sizeof g_GameFactionRuntimeImage.records / sizeof g_GameFactionRuntimeImage.records[0];
  factionBit = 1;
  relationStatePattern = FACTION_RELATION_DEFAULT_PATTERN; /* one nibble per faction, rotated by one nibble per record */
  do {
    capabilityFlagsSlot = &factionRecord->capabilityFlags;
    *capabilityFlagsSlot = *capabilityFlagsSlot | factionBit;
    dwordCursor = factionRecord->technologyMasks256Bits;
    *dwordCursor = *dwordCursor | 1;
    capabilityFlagsSlot = &factionRecord->capabilityFlags;
    *capabilityFlagsSlot = *capabilityFlagsSlot | 1;
    factionRecord->packedRelationStates = relationStatePattern;
    factionRecord->relationCapabilityState = 0;
    factionRecord->primaryAnchorYQ12 = -12 * Q12_ONE;
    factionRecord->primaryAnchorXQ12 = 0;
    factionRecord->secondaryAnchorYQ12 = -12 * Q12_ONE;
    factionRecord->secondaryAnchorXQ12 = 0;
    factionRecord->relationTransitionTick = 17;
    factionRecord->energyGenerationCapacityQ4 = 40 << Q4_SHIFT;
    factionRecord->baselineEnergySupplyQ4 = 40 << Q4_SHIFT;
    factionRecord->xeniteStorageLimitQ4 = 4000; /* 250.0 */
    factionRecord->tritiumStorageLimitQ4 = 4000; /* 250.0 */
    factionRecord->terrainContributionScaleQ8 = Q8_ONE;
    factionBit = factionBit * 2;
    relationStatePattern = relationStatePattern << 4 | relationStatePattern >> 28; /* rotate left by a nibble */
    factionRecord++;
    remainingCount--;
  } while (remainingCount != 0);
  allocError = g_MemoryApi.alloc(GAME_STAT_TABLE_BYTES,&allocPayload);
  previousStatTable = g_GameStatTableImage;
  if (allocError != 0) {
    return allocError;
  }
  LOCK();
  UNLOCK();
  g_GameStatTableImage = allocPayload;
  g_MemoryApi.free(previousStatTable);
  statTableCursor = (uint32_t *)allocPayload;
  for (remainingCount = GAME_STAT_TABLE_BYTES / 4; remainingCount != 0; remainingCount--) {
    *statTableCursor = 0;
    statTableCursor++;
  }
  statTableCursor[-1] = UINT32_MAX; /* end marker */
  g_GameFactionRuntimeImage.tail.periodicClockTick = 0;
  return 0;
}


/* Loads the game data of a level or savegame from the mounted packages: daten.hex is the faction image,
   stat.hex replaces the stat table and oldunit.hex (record count, primary table, secondary table) fills the
   old-unit tables; without oldunit.hex both tables and the count are cleared. Returns true (failure) when
   daten.hex or stat.hex cannot be loaded, false otherwise.
*/
Bool8 GameData_LoadExternalTables()

{
  void *previousStatTable;
  uint32_t *oldUnitBuffer;
  uint32_t *clearCursor;
  int remainingCount;
  uint32_t *sourceCursor;
  uint32_t *destinationCursor;
  void *statTable;

  clearCursor = g_GameDataAuxState.pairPressureMatrix8x8;
  for (remainingCount = sizeof g_GameDataAuxState.pairPressureMatrix8x8 / 4; remainingCount != 0;
      remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  if (!Package_LoadEntryIntoBuffer
                    (GAME_FACTION_IMAGE_BYTES,(uint8_t *)&g_GameFactionRuntimeImage,
                     (uint16_t *)g_DatenHexPathUtf16,nullptr)) {
    return true;
  }
  statTable = Package_LoadEntry((uint16_t *)g_StatHexPathUtf16,nullptr);
  previousStatTable = g_GameStatTableImage;
  if (statTable == nullptr) {
    return true;
  }
  LOCK();
  UNLOCK();
  g_GameStatTableImage = statTable;
  g_MemoryApi.free(previousStatTable);
  oldUnitBuffer = (uint32_t *)Package_LoadEntry((uint16_t *)g_OldunitHexPathUtf16,nullptr);
  if (oldUnitBuffer == nullptr) {
    clearCursor = g_OldUnitPrimaryTable;
    for (remainingCount = OLD_UNIT_PRIMARY_TABLE_BYTES / 4; remainingCount != 0; remainingCount--) {
      *clearCursor = 0;
      clearCursor++;
    }
    clearCursor = g_OldUnitSecondaryTable;
    for (remainingCount = OLD_UNIT_SECONDARY_TABLE_BYTES / 4; remainingCount != 0; remainingCount--) {
      *clearCursor = 0;
      clearCursor++;
    }
    g_OldUnitRecordCount = 0;
    return false;
  }
  /* oldunit.hex: the record count, then the primary and the secondary table */
  g_OldUnitRecordCount = *oldUnitBuffer;
  sourceCursor = oldUnitBuffer + 1;
  destinationCursor = g_OldUnitPrimaryTable;
  for (remainingCount = OLD_UNIT_PRIMARY_TABLE_BYTES / 4; remainingCount != 0; remainingCount--) {
    *destinationCursor = *sourceCursor;
    sourceCursor++;
    destinationCursor++;
  }
  destinationCursor = g_OldUnitSecondaryTable;
  for (remainingCount = OLD_UNIT_SECONDARY_TABLE_BYTES / 4; remainingCount != 0; remainingCount--) {
    *destinationCursor = *sourceCursor;
    sourceCursor++;
    destinationCursor++;
  }
  Resource_Release(oldUnitBuffer);
  return false;
}


/* Resolves procedureName in module with GetProcAddress and stores it in *destination; returns 0. The name is
   left in g_PackageLastErrorPath and, on failure, the name of the module (when it is one of g_DynamicModules)
   in g_FatalErrorDetail1Utf16 for the fatal-error message; returns FATAL_ERROR_DLL_PROCEDURE_MISSING then
   (*destination untouched).
*/
uint32_t DynAPI_Resolve(void **destination,HINSTANCE module,char *procedureName)

{
  FARPROC resolvedProcedure;
  uint32_t modulesRemaining;
  DynamicModuleEntry *moduleEntryCursor;

  Text_CopyNarrowToUtf16(256,g_PackageLastErrorPath,(uint8_t *)procedureName);
  resolvedProcedure = GetProcAddress(module,procedureName);
  if (resolvedProcedure != nullptr) {
    *destination = (void *)resolvedProcedure;
    return 0;
  }
  moduleEntryCursor = g_DynamicModules;
  g_FatalErrorDetail1Utf16[0] = 0;
  modulesRemaining = g_DynamicModuleCount;
  for (; modulesRemaining != 0; modulesRemaining = modulesRemaining - 1) {
    if (module == moduleEntryCursor->module) {
      /* name the module in the error detail */
      Text_CopyNarrowToUtf16(256,g_FatalErrorDetail1Utf16,(uint8_t *)moduleEntryCursor->name);
      break;
    }
    moduleEntryCursor = moduleEntryCursor + 1;
  }
  return FATAL_ERROR_DLL_PROCEDURE_MISSING;
}


/* Loads the DLL moduleName with the bound LoadLibraryA and records it in g_DynamicModules so that
   DynDLL_UnloadAll frees it; returns the (non-NULL) module. Returns NULL (the name left in
   g_PackageLastErrorPath) when LoadLibraryA is not bound yet, the table is full or the load fails; the
   original reported FATAL_ERROR_DLL_LOAD_FAILED then, which the callers now supply themselves.
*/
HINSTANCE DynDLL_Load(char *moduleName)

{
  HINSTANCE loadedModule;
  uint32_t moduleSlotIndex;

  Text_CopyNarrowToUtf16(256,g_PackageLastErrorPath,(uint8_t *)moduleName);
  /* dynapi_9 is the string "LoadLibraryA": the slot still holds the name until DynAPI_Bootstrap binds it */
  if ((g_BootstrapApiBindings[BOOTSTRAP_API_LOAD_LIBRARY_A].destination != (void **)dynapi_9) &&
      (g_DynamicModuleCount < DYNAMIC_MODULE_CAPACITY))
  {
    loadedModule = (HINSTANCE)((BootstrapLoadLibraryAProc)g_BootstrapApiBindings[BOOTSTRAP_API_LOAD_LIBRARY_A].destination)(moduleName);
    moduleSlotIndex = g_DynamicModuleCount;
    if (loadedModule != nullptr) {
      g_DynamicModules[g_DynamicModuleCount].module = loadedModule;
      g_DynamicModules[moduleSlotIndex].name = moduleName;
      g_DynamicModuleCount++;
      return loadedModule;
    }
  }
  return nullptr;
}


/* Frees every DLL recorded in g_DynamicModules with the bound FreeLibrary at shutdown; each slot is cleared
   before the call so a module is never freed twice. The count is left unchanged.
*/
void DynDLL_UnloadAll()

{
  uint32_t modulesRemaining;
  DynamicModuleEntry *moduleEntryCursor;
  HINSTANCE loadedModule;

  moduleEntryCursor = g_DynamicModules;
  for (modulesRemaining = g_DynamicModuleCount; modulesRemaining != 0;
      modulesRemaining = modulesRemaining - 1) {
    if (moduleEntryCursor->module != nullptr) {
      loadedModule = moduleEntryCursor->module;
      moduleEntryCursor->module = nullptr;
      ((BootstrapFreeLibraryProc)g_BootstrapApiBindings[BOOTSTRAP_API_FREE_LIBRARY].destination)(loadedModule);
    }
    moduleEntryCursor++;
  }
  return;
}


/* Sets CPU_FEATURE_MMX in g_CpuFeatureFlags when CPUID reports MMX; ProcessEntry refuses to run without it
   (FATAL_ERROR_CPU_WITHOUT_MMX). The constant return value 5 has no known use.
*/
uint32_t __cdecl CPU_DetectFeatures()

{
  intptr_t cpuidVersionInfo;

  cpuidVersionInfo = cpuid_Version_info(CPUID_LEAF_VERSION_INFO);
  /* offset 8 of the CPUID result is EDX */
  if ((*(uint32_t *)(cpuidVersionInfo + 8) & CPUID_EDX_MMX) != 0) {
    g_CpuFeatureFlags = g_CpuFeatureFlags | CPU_FEATURE_MMX;
  }
  return 5;
}

/* Runs the game once the subsystems are up: shows the first cursor frame, initialises spatial audio and
   rendering, loads the core assets and plays the intro movies (each failure is fatal). It then switches
   from the 640x480 start mode to the saved display mode, runs the frontend main loop and
   finally closes and cleans up the network backend.
*/
void __cdecl Game_Run()

{
  uint32_t renderingInitError;
  uint32_t loadResult;
  uint32_t displayWidth;
  uint32_t displayHeight;
  uint32_t bitsPerPixel;
  uint32_t adapterIndex;
  Bool8 introMoviesFailed;
  Bool8 cursorFrameSet;
  uint32_t checkedValue;
  uint32_t displayModeError;
  uint32_t mainLoopError;

  cursorFrameSet = g_GraphicsCursorSetFrame(0);
  FatalError_ExitIfFailed(FATAL_ERROR_CURSOR_FRAME_OUT_OF_RANGE,!cursorFrameSet);
  renderingInitError = GameRuntime_InitializeSpatialAudioAndRendering();
  FatalError_ExitIfFailed(renderingInitError,renderingInitError != 0);
  loadResult = Game_LoadCoreAssets();
  Thandor_Log("Game_LoadCoreAssets -> 0x%08X", loadResult);
  /* 0 on success, an error code otherwise */
  checkedValue = FatalError_ExitIfFailed(loadResult,loadResult != 0);
  /* a movie that cannot start is reported with the previous checked value */
  introMoviesFailed = Game_PlayIntroMovies();
  FatalError_ExitIfFailed(checkedValue,introMoviesFailed);
  PersistentSettings_Load();
  /* ProcessEntry started in 640x480. The original switched only when the saved mode differed from 640x480x16;
     the colour depth is always 32 bits now (never 16), so the saved mode is always set (the same mode switches,
     and with them the same allocations, as before) */
  displayWidth = PersistentSettings_Read(GAME_START_DISPLAY_WIDTH,PERSISTENT_SETTING_DISPLAY_WIDTH);
  displayHeight = PersistentSettings_Read(GAME_START_DISPLAY_HEIGHT,PERSISTENT_SETTING_DISPLAY_HEIGHT);
  bitsPerPixel = PersistentSettings_Read(PERSISTENT_DEFAULT_BITS_PER_PIXEL,PERSISTENT_SETTING_BITS_PER_PIXEL);
  adapterIndex = SdlVideo_SavedAdapterIndex();
  if (!g_GraphicsSetDisplayMode(adapterIndex,bitsPerPixel,displayHeight,displayWidth,&displayModeError)) {
    FatalError_ExitIfFailed(displayModeError,true);
  }
  SdlVideo_SaveAdapterIndex(g_ActiveGraphicsAdapterIndex);
  if (!Frontend_MainLoop(1,&mainLoopError)) {
    FatalError_ExitIfFailed(mainLoopError,true);
  }
  g_NetworkBackendSlot3(); /* close */
  g_NetworkBackendSlot1(); /* cleanup */
  return;
}


/* Game_Run's first startup step: initialises the spatial-sound pool, the terrain and intensity clamp tables,
   the software renderer's display-mode hook and the global primitive queue (0xA000 packets), in that order.
   Stops at the first step that fails and returns its (non-zero) error code; returns 0 when all succeed.
   The original returned g_PrimitiveQueueStorage on success; its only caller (Game_Run) discards it.
*/
uint32_t __cdecl GameRuntime_InitializeSpatialAudioAndRendering()

{
  uint32_t poolError;
  uint32_t stepError;

  if (!SpatialSoundPool_Init(&poolError)) {
    return poolError;
  }
  if (!TerrainByteClampLookup_Initialize(&stepError)) {
    return stepError;
  }
  stepError = GraphicsIntensityClampTable_Initialize();
  if (stepError != 0) {
    return stepError;
  }
  stepError = SoftwareRenderer_InstallDisplayModeHook();
  if (stepError != 0) {
    return stepError;
  }
  return GraphicsPrimitiveQueue_AllocateGlobalPool(GAME_PRIMITIVE_QUEUE_PACKET_COUNT);
}


/* HKLM\Software\Planet4\Thandor "CD": movies are looked up under <CD>\Thandor first. Only done with the
   arena heap allocator. The 32-bit setup of 1999 wrote the key, so the 64-bit build reads the 32-bit registry view
   (WOW6432Node) like the original did. */
static void CoreAssets_ReadCdPathFromRegistry()

{
  int status;

  if (g_MemoryApi.alloc != ArenaHeap_Alloc) {
    return;
  }
  status = ((BootstrapRegOpenKeyExAProc)g_BootstrapApiBindings[BOOTSTRAP_API_REG_OPEN_KEY_EX_A].destination)
             ((uintptr_t)(intptr_t)(int32_t)HKEY_LOCAL_MACHINE,g_SoftwarePlanet4ThandorAscii,0,KEY_READ | KEY_WOW64_32KEY,
              &g_InstallRegistryKeyHandle); /* predefined keys are sign-extended 32-bit values on x64 */
  if (status != ERROR_SUCCESS) {
    return;
  }
  status = ((BootstrapRegQueryValueExAProc)g_BootstrapApiBindings[BOOTSTRAP_API_REG_QUERY_VALUE_EX_A].destination)
             (g_InstallRegistryKeyHandle,g_InstallRegistryValueNameCD,nullptr,
              &g_InstallRegistryValueType,g_InstallRegistryValueDataA,
              &g_InstallRegistryValueDataCapacityBytes);
  if ((status == ERROR_SUCCESS) && (g_InstallRegistryValueType == REG_SZ)) {
    /* open-thandor: the directory is cut short enough that "<CD>\Thandor" fits the 0x100-unit
       g_LooseMoviePathPrefix (a CD path of 0xF8 characters or more ran past it; the copy keeps at most
       capacity / 2 - 2 characters, here 0xF7) */
    Text_CopyNarrowToUtf16
              (sizeof g_InstallDirectoryScratchUtf16 - (sizeof g_ThandorWindowTitleUtf16 - sizeof(uint16_t)),
               g_InstallDirectoryScratchUtf16,g_InstallRegistryValueDataA);
    WidePath_CombineDirectoryAndLeaf
              (g_LooseMoviePathPrefix.codeUnits,(uint16_t *)g_ThandorWindowTitleUtf16,g_InstallDirectoryScratchUtf16);
  }
  ((BootstrapRegCloseKeyProc)g_BootstrapApiBindings[BOOTSTRAP_API_REG_CLOSE_KEY].destination)(g_InstallRegistryKeyHandle);
}


/* open-thandor: the full-length movies from the CD (Ende*.flm, Intro2.flm) live in the flm folder of the
   game directory, so the CD is no longer needed. Movie_Open looks under g_LooseMoviePathPrefix before the
   packages, which only hold still-image stand-ins for these movies; point the prefix at the game directory
   when that folder exists. */
static void CoreAssets_UseLocalMovieFolder()

{
  static const uint16_t flmLeaf[4] = {'f','l','m',0};
  static uint16_t localFlmPath[THANDOR_PATH_CAPACITY]; /* <exe dir>\flm: up to 0x104 units */
  char narrow[256];
  int k;

  WidePath_CombineDirectoryAndLeaf
            (localFlmPath,(uint16_t *)flmLeaf,g_ExecutableDirectoryUtf16);
  if (Thandor_DirectoryExistsW(localFlmPath)) {
    uint16_t *directory = g_ExecutableDirectoryUtf16;
    for (k = 0; (k < 255) && (directory[k] != 0); k++) {
      g_LooseMoviePathPrefix.codeUnits[k] = directory[k];
    }
    g_LooseMoviePathPrefix.codeUnits[k] = 0;
  }
  for (k = 0; (k < 255) && (g_LooseMoviePathPrefix.codeUnits[k] != 0); k++) {
    narrow[k] = (char)g_LooseMoviePathPrefix.codeUnits[k];
  }
  narrow[k] = 0;
  Thandor_Log("movie CD path: \"%s\"", narrow);
}


/* Mounts patchNN.pck, levelNN.pck and the fixed core packages, keeping the core package handles. */
static void CoreAssets_MountPackages()

{
  uintptr_t packageHandle; /* mounted package handle (set on failure too, but then unused) */

  /* patchNN.pck and then levelNN.pck, NN counting down to "00". decimalDigits.codeUnits[0] is the tens
     digit, [1] the ones digit; adding UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP to the packed pair decrements the
     tens digit and turns the ones digit from '0' - 1 back into '9'. The patch count starts at "00", so only
     patch00.pck is tried. */
  g_PatchArchivePathTemplateUtf16.decimalDigits.packedDigits = UTF16_DIGIT_PAIR('0','0');
  do {
    do {
      Package_Mount(g_PatchArchivePathTemplateUtf16.prefixCodeUnits,nullptr);
      g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[1] =
           g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[1] - 1;
    } while ('0' - 1 < g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[1]);
    g_PatchArchivePathTemplateUtf16.decimalDigits.packedDigits =
         g_PatchArchivePathTemplateUtf16.decimalDigits.packedDigits + UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP;
  } while ('0' - 1 < g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[0]);
  g_LevelArchivePathTemplateUtf16.decimalDigits.packedDigits = UTF16_DIGIT_PAIR('9','9');
  do {
    do {
      LevelPackage_ValidateAndMount(g_LevelArchivePathTemplateUtf16.prefixCodeUnits);
      g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[1] =
           g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[1] - 1;
    } while ('0' - 1 < g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[1]);
    g_LevelArchivePathTemplateUtf16.decimalDigits.packedDigits =
         g_LevelArchivePathTemplateUtf16.decimalDigits.packedDigits + UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP;
  } while ('0' - 1 < g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[0]);
  if (Package_Mount((uint16_t *)g_DatenPckPathUtf16,&packageHandle)) {
    g_DataPackageHandle = packageHandle;
  }
  if (Package_Mount((uint16_t *)g_ModellePckPathUtf16,&packageHandle)) {
    g_ModelPackageHandle = packageHandle;
  }
  if (Package_Mount((uint16_t *)g_GraphikPckPathUtf16,&packageHandle)) {
    g_GraphicsPackageHandle = packageHandle;
  }
  if (Package_Mount((uint16_t *)g_SoundPckPathUtf16,&packageHandle)) {
    g_SoundPackageHandle = packageHandle;
  }
  if (Package_Mount((uint16_t *)g_FilmePckPathUtf16,&packageHandle)) {
    g_MoviePackageHandle = packageHandle;
  }
  if (Package_Mount((uint16_t *)g_LevelPckPathUtf16,&packageHandle)) {
    g_LevelPackageHandle = packageHandle;
  }
}


/* Loads one UI button sample and creates its voice set in *voiceSetSlot; the sample is released again
   either way once loaded. Returns false with the error code in *error when the sample cannot be loaded or
   the voice set cannot be created. */
static Bool8 CoreAssets_LoadButtonSound(uint16_t *samplePath,DirectSoundVoiceSet **voiceSetSlot,uint32_t *error)

{
  SoundSampleAsset *sample;
  DirectSoundVoiceSet *voiceSet;
  uint32_t voiceSetError;

  if (!Resource_Load(samplePath,(void **)&sample,nullptr,error)) {
    return false;
  }
  voiceSetError = g_SoundCreateSampleVoiceSet(sample,&voiceSet);
  Resource_Release(sample);
  if (voiceSetError != 0) {
    *error = voiceSetError;
    return false;
  }
  *voiceSetSlot = voiceSet;
  return true;
}


/* g_ScreenshotFileNameUtf16 is "screen00.pcx" ([6] tens digit, [7] ones digit): counts up to the first
   screenshot file that does not exist yet. */
static void CoreAssets_AdvanceScreenshotName()

{
  wchar_t screenshotTensDigit;
  void *screenshotFile;

  do {
    do {
      if (g_FileSystemOpen(0,g_ScreenshotFileNameUtf16,&screenshotFile) != 0) {
        return;
      }
      g_ScreenshotFileNameUtf16[7] = g_ScreenshotFileNameUtf16[7] + 1;
      g_FileSystemClose(screenshotFile);
      screenshotTensDigit = g_ScreenshotFileNameUtf16[6];
    } while ((uint16_t)g_ScreenshotFileNameUtf16[7] < '9' + 1);
    g_ScreenshotFileNameUtf16[6] = g_ScreenshotFileNameUtf16[6] + 1;
    g_ScreenshotFileNameUtf16[7] = g_ScreenshotFileNameUtf16[7] - 10;
  } while ((uint16_t)g_ScreenshotFileNameUtf16[6] < '9' + 1);
  /* all 100 names exist: the tens digit goes back to '0' (the last seen '9' - 9) */
  g_ScreenshotFileNameUtf16[6] = screenshotTensDigit - 9;
}


/* Advances the two-digit counter of g_ScreenshotFileNameUtf16 ("screen00.pcx", [6] tens digit, [7] ones
   digit) after a screenshot was written, wrapping from 99 to 00. */
void Screenshot_AdvanceFileName()

{
  uint16_t *digitHigh = &g_ScreenshotFileNameUtf16[6];
  uint16_t *digitLow = &g_ScreenshotFileNameUtf16[7];

  (*digitLow)++;
  if (*digitLow > '9') {
    (*digitHigh)++;
    *digitLow = *digitLow - 10;
    if (*digitHigh > '9') {
      *digitHigh = *digitHigh - 10;
    }
  }
}

/* Screenshot command of the in-game and end-movie keyboard dispatchers: captures the whole framebuffer,
   writes it as PCX to g_ScreenshotFileNameUtf16 and advances the name's counter. A failed capture or encoding
   writes nothing and keeps the name. */
void Screenshot_SaveFramebufferAsPcx()

{
  GraphicsCapturedTextureSourceAsset *capture;
  void *pcxBytes;
  uint32_t pcxByteCount;
  uint32_t pcxError;

  capture = g_GraphicsFramebufferCaptureRegion(g_FramebufferHeight,g_FramebufferWidth,0,0);
  if (capture == nullptr) {
    return;
  }
  if (!Pcx_EncodeCapture(capture,&pcxBytes,&pcxByteCount,&pcxError)) {
    g_MemoryApi.free(capture);
    return;
  }
  FileSystem_WriteBufferToPath(pcxByteCount,pcxBytes,g_ScreenshotFileNameUtf16);
  g_MemoryApi.free(pcxBytes);
  g_MemoryApi.free(capture);
  Screenshot_AdvanceFileName();
}


/* Binds placeholders 0..13 of the world view info texts to the debug-overlay text slots and installs the
   in-game and frontend UI action handler pages. */
static void CoreAssets_BindDebugOverlayTextsAndUiPages()

{
  TextResourceId resourceId;
  uint16_t *resolvedText;

  for (resourceId = TEXT_ID_WORLD_VIEW_INFO_FIRST; resourceId < TEXT_ID_WORLD_VIEW_INFO_LAST + 1;
      resourceId = resourceId + 1) {
    resolvedText = TextResource_Resolve(resourceId);
    RichTextCommandStream_PatchPayloadBySelector(0,g_FrontendDebugOverlayTextSlot00Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(1,g_FrontendDebugOverlayTextSlot01Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(2,g_FrontendDebugOverlayTextSlot02Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(3,g_FrontendDebugOverlayTextSlot03Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(4,g_FrontendDebugOverlayTextSlot04Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(5,g_FrontendDebugOverlayTextSlot05Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(6,g_FrontendDebugOverlayTextSlot06Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(7,g_FrontendDebugOverlayTextSlot07Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(8,g_FrontendDebugOverlayTextSlot08Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(9,g_FrontendDebugOverlayTextSlot09Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(10,g_FrontendDebugOverlayTextSlot10Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(11,g_FrontendDebugOverlayTextSlot11Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(12,g_FrontendDebugOverlayTextSlot12Utf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(13,g_FrontendDebugOverlayTextSlot13Utf16,resolvedText);
  }
  UiActionHandlers_SetPage
            (UI_ACTION_PAGE_INGAME,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage10);
  UiActionHandlers_SetPage
            (UI_ACTION_PAGE_INGAME_COMMAND_MODE,
             (UiActionHandlerPage *)&g_InGameUiActionHandlersPage11);
  UiActionHandlers_SetPage
            (UI_ACTION_PAGE_INGAME_MENU,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage12);
  UiActionHandlers_SetPage
            (UI_ACTION_PAGE_FRONTEND,(UiActionHandlerPage *)&g_FrontendUiActionHandlersPage20);
}


/* Loads the eight text pages; returns false with the failing page's error code in *error. */
static Bool8 CoreAssets_LoadTextPages(uintptr_t *error)

{
  return TextResourcePage_Load(GAME_TEXT_PAGE_NETERROR,(uint16_t *)g_TexteNeterrorStrPathUtf16,error) &&
         TextResourcePage_Load(GAME_TEXT_PAGE_HELP,(uint16_t *)g_TexteHelpStrPathUtf16,error) &&
         TextResourcePage_Load(GAME_TEXT_PAGE_HILFE,(uint16_t *)g_TexteHilfeStrPathUtf16,error) &&
         TextResourcePage_Load(GAME_TEXT_PAGE_MENUE,(uint16_t *)g_TexteMenueStrPathUtf16,error) &&
         TextResourcePage_Load(GAME_TEXT_PAGE_TECHNO,(uint16_t *)g_TexteTechnoStrPathUtf16,error) &&
         TextResourcePage_Load(GAME_TEXT_PAGE_LEVEL,(uint16_t *)g_TexteLevelStrPathUtf16,error) &&
         TextResourcePage_Load(GAME_TEXT_PAGE_INHALT,(uint16_t *)g_TexteInhaltStrPathUtf16,error) &&
         TextResourcePage_Load(GAME_TEXT_PAGE_TASTATUR,(uint16_t *)g_TexteTastaturStrPathUtf16,error);
}


/* Applies the saved sound options: the effect and movie gains (all 0 with sound effects off), the
   reverse-stereo mask and the model LOD depth threshold. */
static void CoreAssets_ApplySoundSettings()

{
  uint32_t soundOptions;
  AudioMixerGainQ15 uiSoundGain;
  MovieAudioGainQ15 movieGain;
  MovieAudioGainQ15 alternateMovieGain;

  soundOptions = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  uiSoundGain = 0;
  if ((soundOptions & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
    uiSoundGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN);
  }
  movieGain = 0;
  g_UiSoundGainQ15 = uiSoundGain;
  g_SoundEffectsGainQ15 = uiSoundGain;
  if ((soundOptions & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
    movieGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
  }
  alternateMovieGain = 0;
  g_MovieDefaultAudioGainQ15 = movieGain;
  if ((soundOptions & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
    alternateMovieGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
  }
  g_ReverseStereoMask = 0;
  if ((soundOptions & PERSISTENT_SOUND_OPTION_REVERSE_STEREO) != 0) {
    g_ReverseStereoMask = 0xffffffff;
  }
  g_MovieAlternateAudioGainQ15 = alternateMovieGain;
  /* the original passes the reverse-stereo mask (0 or 0xFFFFFFFF) as the default here, a leftover
     value from the store above */
  g_ModelLodDepthThresholdQ8 =
       PersistentSettings_Read(g_ReverseStereoMask,PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD);
}


/* Allocates the fixed runtime buffers (text history, old-unit tables, frontend lists, spline workspaces,
   player records, scratch slices) and sets up the pointers into them. Returns 0, or the error code of the
   first failing allocation. */
static uint32_t CoreAssets_AllocateRuntimeBuffers()

{
  uint16_t *textBuffer;
  uintptr_t playerListBase;
  float *splineBuffer;
  FrontendPlayerRuntimeRecord *playerRecordCursor;
  Ptr32<FrontendPlayerRuntimeRecord> *playerRuntimePointerTableWriteCursor;
  uint8_t *scratchCursor;
  int remainingCount;
  uint32_t allocError;
  void *allocPayload;

  allocError = g_MemoryApi.alloc
                   (RECENT_TEXT_HISTORY_SLOT_COUNT * sizeof(RecentTextHistorySlot),
                    (void **)&g_RecentTextSlotStorage);
  if (allocError != 0) {
    return allocError;
  }
  allocError = g_MemoryApi.alloc(OLD_UNIT_SECONDARY_TABLE_BYTES,(void **)&g_OldUnitSecondaryTable);
  if (allocError != 0) {
    return allocError;
  }
  allocError = g_MemoryApi.alloc(OLD_UNIT_PRIMARY_TABLE_BYTES,(void **)&g_OldUnitPrimaryTable);
  if (allocError != 0) {
    return allocError;
  }
  allocError = g_MemoryApi.alloc
                   (FRONTEND_PLAYER_LIST_ROW_COUNT * FRONTEND_PLAYER_LIST_ROW_BYTES,&allocPayload);
  if (allocError != 0) {
    return allocError;
  }
  playerListBase = (uintptr_t)allocPayload;
  g_FrontendPlayerListRows[1] = playerListBase + 1 * FRONTEND_PLAYER_LIST_ROW_BYTES;
  g_FrontendPlayerListRows[2] = playerListBase + 2 * FRONTEND_PLAYER_LIST_ROW_BYTES;
  g_FrontendPlayerListRows[3] = playerListBase + 3 * FRONTEND_PLAYER_LIST_ROW_BYTES;
  g_FrontendPlayerListRows[4] = playerListBase + 4 * FRONTEND_PLAYER_LIST_ROW_BYTES;
  g_FrontendPlayerListRows[5] = playerListBase + 5 * FRONTEND_PLAYER_LIST_ROW_BYTES;
  g_FrontendPlayerListRows[6] = playerListBase + 6 * FRONTEND_PLAYER_LIST_ROW_BYTES;
  g_FrontendPlayerListRows[7] = playerListBase + 7 * FRONTEND_PLAYER_LIST_ROW_BYTES;
  g_FrontendPlayerListRows[0] = playerListBase;
  allocError = g_MemoryApi.alloc
                   (ROM_REGISTRY_SLOT_COUNT * sizeof(RomRegistrySlot),
                    (void **)&g_RomRegistrySlots);
  if (allocError != 0) {
    return allocError;
  }
  allocError = g_MemoryApi.alloc
                   (FRONTEND_SESSION_LIST_CAPACITY * sizeof(Ptr32<FrontendSessionDiscoveryRecord>),
                    (void **)&g_FrontendSessionListRows);
  if (allocError != 0) {
    return allocError;
  }
  allocError = g_MemoryApi.alloc
                   (FRONTEND_SESSION_LIST_CAPACITY * sizeof(FrontendSessionDiscoveryRecord),
                    (void **)&g_FrontendSessionDiscoveryRecords);
  if (allocError != 0) {
    return allocError;
  }
  allocError = g_MemoryApi.alloc(INGAME_FACTION_STATUS_TEXT_BYTES,(void **)&textBuffer);
  if (allocError != 0) {
    return allocError;
  }
  g_InGameFactionStatusTextScratchUtf16 = textBuffer;
  /* the in-game template's wrapped world view status text shows the same buffer */
  ((UiWrappedTextControl *)&g_InGameRuntimeDefaultImageTemplate.worldViewWrappedStatusText)->text = textBuffer;
  allocError = g_MemoryApi.alloc
                   (INGAME_PLAYER_LIST_TEXT_BYTES,(void **)&g_InGamePlayerListTextScratchUtf16);
  if (allocError != 0) {
    return allocError;
  }
  allocError = g_MemoryApi.alloc(WORLD_MOTION_SPLINE_CHANNEL_COUNT *
                                 CUBIC_SPLINE_MATRIX_FLOATS * sizeof(float),
                                 (void **)&splineBuffer);
  if (allocError != 0) {
    return allocError;
  }
  g_WorldMotionSplineMatrixWorkspaces[1] = splineBuffer + 1 * CUBIC_SPLINE_MATRIX_FLOATS;
  g_WorldMotionSplineMatrixWorkspaces[2] = splineBuffer + 2 * CUBIC_SPLINE_MATRIX_FLOATS;
  g_WorldMotionSplineMatrixWorkspaces[3] = splineBuffer + 3 * CUBIC_SPLINE_MATRIX_FLOATS;
  g_WorldMotionSplineMatrixWorkspaces[4] = splineBuffer + 4 * CUBIC_SPLINE_MATRIX_FLOATS;
  g_WorldMotionSplineMatrixWorkspaces[5] = splineBuffer + 5 * CUBIC_SPLINE_MATRIX_FLOATS;
  g_WorldMotionSplineMatrixWorkspaces[0] = splineBuffer;
  /* one coefficient vector of CUBIC_SPLINE_MATRIX_ORDER floats per channel */
  allocError = g_MemoryApi.alloc(WORLD_MOTION_SPLINE_CHANNEL_COUNT *
                                 CUBIC_SPLINE_MATRIX_ORDER * sizeof(float),
                                 (void **)&splineBuffer);
  if (allocError != 0) {
    return allocError;
  }
  g_WorldMotionSplineCoefficientTables[1] = splineBuffer + 1 * CUBIC_SPLINE_MATRIX_ORDER;
  g_WorldMotionSplineCoefficientTables[2] = splineBuffer + 2 * CUBIC_SPLINE_MATRIX_ORDER;
  g_WorldMotionSplineCoefficientTables[3] = splineBuffer + 3 * CUBIC_SPLINE_MATRIX_ORDER;
  g_WorldMotionSplineCoefficientTables[4] = splineBuffer + 4 * CUBIC_SPLINE_MATRIX_ORDER;
  g_WorldMotionSplineCoefficientTables[5] = splineBuffer + 5 * CUBIC_SPLINE_MATRIX_ORDER;
  g_WorldMotionSplineCoefficientTables[0] = splineBuffer;
  allocError = g_MemoryApi.alloc
                   (SELECTION_PLAYER_BLOCK_COUNT * sizeof(SelectionPlayerRuntimeBlock),
                    (void **)&g_SelectionPlayerBlocks);
  if (allocError != 0) {
    return allocError;
  }
  allocError = g_MemoryApi.alloc(FRONTEND_SNAPSHOT_PAYLOAD_BYTES,&allocPayload);
  if (allocError != 0) {
    return allocError;
  }
  g_FrontendLocalPlayerPcxPreview = (uintptr_t)allocPayload;
  allocError = g_MemoryApi.alloc(TERRAIN_REGION_COLLECTION_CAPACITY * 8,&allocPayload); /* 8-byte records */
  if (allocError != 0) {
    return allocError;
  }
  g_TerrainRegionCollectionEntries = (uintptr_t)allocPayload;
  allocError = g_MemoryApi.alloc(800,&allocPayload);
  if (allocError != 0) {
    return allocError;
  }
  g_FrontendPlayerMessageBuffers = (uintptr_t)allocPayload;
  allocError = g_MemoryApi.alloc
                   (FRONTEND_PLAYER_RUNTIME_RECORD_ALLOC_COUNT * sizeof(FrontendPlayerRuntimeRecord),
                    (void **)&playerRecordCursor);
  if (allocError != 0) {
    return allocError;
  }
  playerRuntimePointerTableWriteCursor =
       g_FrontendPlayerRuntimeRecordPointers32;
  g_FrontendPlayerRuntimeBlockCount = 1;
  g_LocalPlayerRuntimeId = 0;
  g_FrontendPlayerRuntimeBlocks = playerRecordCursor;
  (playerRecordCursor->playerName).textUtf16[0] = 0;
  (playerRecordCursor->playerName).textUtf16[1] = 0;
  playerRecordCursor->playerRuntimeId = 0;
  (playerRecordCursor->factionAssignment).roleStateFlags = 0;
  playerRecordCursor->snapshotTransferFlags = 0;
  /* pointers to 32 consecutive records, although only the first
     FRONTEND_PLAYER_RUNTIME_RECORD_ALLOC_COUNT are allocated.
     Original quirk: entries 8..31 point past the buffer (the original
     also allocates 0x9D80 and loops 0x20 times with stride 0x13B0). Harmless:
     the table is only the hostLobbyPlayerList row table, whose rowCount is
     capped by maxPlayersSlider (range 2..8), so only rows 0..7 are used. */
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    *playerRuntimePointerTableWriteCursor = playerRecordCursor;
    playerRuntimePointerTableWriteCursor++;
    playerRecordCursor++;
  }
  allocError = g_MemoryApi.alloc
                   (CORE_ASSET_SCRATCH_SLICE_COUNT * CORE_ASSET_SCRATCH_SLICE_BYTES,
                    (void **)&scratchCursor);
  if (allocError != 0) {
    return allocError;
  }
  /* the slices are the tooltip texts of the seven technology area tabs of the in-game template */
  g_InGameRuntimeDefaultImageTemplate.technologyAreaTab2_prefix.tooltipText =
       (uint16_t *)(scratchCursor + 1 * CORE_ASSET_SCRATCH_SLICE_BYTES);
  g_InGameRuntimeDefaultImageTemplate.technologyAreaTab3_prefix.tooltipText =
       (uint16_t *)(scratchCursor + 2 * CORE_ASSET_SCRATCH_SLICE_BYTES);
  g_InGameRuntimeDefaultImageTemplate.technologyAreaTab4_prefix.tooltipText =
       (uint16_t *)(scratchCursor + 3 * CORE_ASSET_SCRATCH_SLICE_BYTES);
  g_InGameRuntimeDefaultImageTemplate.technologyAreaTab5_prefix.tooltipText =
       (uint16_t *)(scratchCursor + 4 * CORE_ASSET_SCRATCH_SLICE_BYTES);
  g_InGameRuntimeDefaultImageTemplate.technologyAreaTab6_prefix.tooltipText =
       (uint16_t *)(scratchCursor + 5 * CORE_ASSET_SCRATCH_SLICE_BYTES);
  g_InGameRuntimeDefaultImageTemplate.technologyAreaTab7_prefix.tooltipText =
       (uint16_t *)(scratchCursor + 6 * CORE_ASSET_SCRATCH_SLICE_BYTES);
  g_InGameRuntimeDefaultImageTemplate.technologyAreaTab1_prefix.tooltipText = (uint16_t *)scratchCursor;
  for (remainingCount = CORE_ASSET_SCRATCH_SLICE_COUNT * CORE_ASSET_SCRATCH_SLICE_BYTES / 4;
       remainingCount != 0; remainingCount--) {
    scratchCursor[0] = 0;
    scratchCursor[1] = 0;
    scratchCursor[2] = 0;
    scratchCursor[3] = 0;
    scratchCursor += 4;
  }
  return 0;
}


/* Loads everything the frontend needs once at startup: takes the CD path from the registry, mounts the patch,
   level and core packages, creates the seven UI button sounds, moves the screenshot name past the existing
   screen??.pcx files, loads the text pages, applies the sound settings and allocates the fixed runtime
   buffers. (The original also loaded and bound the PCX codec module engine\pcx.fnc here; open-thandor
   reads and writes PCX in C instead.) Returns 0, or the error code of the first failing step (the caller
   treats non-zero as failure).
*/
uint32_t __cdecl Game_LoadCoreAssets()

{
  uint32_t aiInitError;
  uint32_t buttonSoundError;
  uintptr_t textPageError; /* the failing text page's error code */
  GraphicsTextureSourceAsset *panelTexture;
  uint32_t panelTextureError;

  CoreAssets_ReadCdPathFromRegistry();
  CoreAssets_UseLocalMovieFolder();
  CoreAssets_MountPackages();
  if (!CoreAssets_LoadButtonSound
         ((uint16_t *)g_SoundButton0SamPathUtf16,&g_UiButtonSoundVoiceSets7[0],&buttonSoundError) ||
      !CoreAssets_LoadButtonSound
         ((uint16_t *)g_SoundButton1SamPathUtf16,&g_UiButtonSoundVoiceSets7[1],&buttonSoundError) ||
      !CoreAssets_LoadButtonSound
         ((uint16_t *)g_SoundButton2SamPathUtf16,&g_UiButtonSoundVoiceSets7[2],&buttonSoundError) ||
      !CoreAssets_LoadButtonSound
         ((uint16_t *)g_SoundButton3SamPathUtf16,&g_UiButtonSoundVoiceSets7[3],&buttonSoundError) ||
      !CoreAssets_LoadButtonSound
         ((uint16_t *)g_SoundButton4SamPathUtf16,&g_UiButtonSoundVoiceSets7[4],&buttonSoundError) ||
      !CoreAssets_LoadButtonSound
         ((uint16_t *)g_SoundButton5SamPathUtf16,&g_UiButtonSoundVoiceSets7[5],&buttonSoundError) ||
      !CoreAssets_LoadButtonSound
         ((uint16_t *)g_SoundButton6SamPathUtf16,&g_UiButtonSoundVoiceSets7[6],&buttonSoundError)) {
    return buttonSoundError;
  }
  CoreAssets_AdvanceScreenshotName();
  CoreAssets_BindDebugOverlayTextsAndUiPages();
  if (!CoreAssets_LoadTextPages(&textPageError)) {
    return (uint32_t)textPageError;
  }
  RichTextCommandStream_BindTextureSource(g_CursorSourceAsset,TextResource_Resolve(TEXT_ID_MOUSE_HELP));
  CoreAssets_ApplySoundSettings();
  if (!AiRuntime_InitWorkspace(&aiInitError)) {
    return aiInitError;
  }
  /* engine\pcx.fnc (machine code in ENGINE.PCK) is no longer loaded: PCX files are read and
     written in C, graphics/resources/pcx_read.cpp and pcx_write.cpp. */
  panelTexture = g_GraphicsTextureSourceLoadPackageAsset
                     ((uint16_t *)g_GfxPanelStatGfxPathUtf16,&panelTextureError);
  if (panelTexture == nullptr) {
    return panelTextureError;
  }
  ((UiImagePanelControl *)&g_InGameRuntimeDefaultImageTemplate.resultsScreenPanel)->textureSource = panelTexture;
  return CoreAssets_AllocateRuntimeBuffers();
}


/* Pumps the window messages and checks for a skip request: a key press (Escape also moves the movie number
   to 8, so the caller's increment reaches intro9, normally absent, which ends the intros) or a mouse-button
   release. The mouse event is only read when no key event was pending. */
static Bool8 IntroMovie_PollSkipRequest()

{
  uint32_t keyCode;
  uint32_t keyStateMask;
  CursorPointerEvent cursorEvent;

  g_Win32PumpMessages();
  if (g_KeyboardReadEvent(&keyCode,&keyStateMask)) {
    /* [9] is the digit of "flm\intro0.flm" */
    if (keyCode == KEYBOARD_KEY_CODE_ESCAPE) {
      g_FlmIntro0FlmPathUtf16[9] = L'8';
    }
    return true;
  }
  /* event types above RIGHT_PRESS are the button releases */
  return g_GraphicsCursorConsumeEvent(&cursorEvent) && (RIGHT_PRESS < cursorEvent.eventType);
}


/* Decodes up to three pending movie frames (one per pending timer tick) and blits the current frame
   centred into the framebuffer. Returns false when a frame cannot be decoded (the movie ended) or the
   framebuffer cannot be accessed, which stops playback. */
static Bool8 IntroMovie_PresentPendingFrames(MovieRuntime *introMovie)

{
  uint32_t frameHeightSnapshot;
  uint32_t quarterFrameHeight;
  int frameAdvanceBudget;
  MovieFrameDimensions frameDimensions;

  /* catch up at most three frames per pass */
  for (frameAdvanceBudget = 3; frameAdvanceBudget != 0; frameAdvanceBudget--) {
    if (!Movie_AdvanceFrame(nullptr,nullptr)) {
      return false;
    }
    frameHeightSnapshot = g_FramebufferHeight;
    g_IntroMoviePendingTicks--;
    if (g_IntroMoviePendingTicks == 0) break;
  }
  quarterFrameHeight = g_FramebufferHeight >> 2;
  if (g_GraphicsFramebufferBeginAccess()) {
    return false;
  }
  frameDimensions = Movie_GetFrameDimensions();
  /* y = (H - H/4 - frameHeight) / 2 + H/8, i.e. vertically centred; the source is the movie
     returned by the first Movie_AdvanceFrame */
  g_GraphicsTextureSourceBlitSourceAlpha
            (g_FramebufferHeight,g_FramebufferWidth,0,0,
             ((int)((frameHeightSnapshot - quarterFrameHeight) - (int)frameDimensions.height) >> 1) +
             (frameHeightSnapshot >> 3),
             (int)(g_FramebufferWidth - (int)frameDimensions.width) >> 1,0,
             (GraphicsTextureSourceAsset *)introMovie,g_FramebufferAccess);
  g_GraphicsFramebufferEndAccess();
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
  return true;
}


/* Plays the intro movies flm\intro0.flm, intro1.flm, ... until one cannot be opened, unless -NOINTRO is given.
   Each movie runs at its own rate from IntroMovie_TimerTick, centred on the screen;
   a key or mouse-button release skips to the next one, Escape skips all of them (the number jumps to 9).
   Returns true only when the first frame of an opened movie cannot be decoded.
*/
Bool8 Game_PlayIntroMovies()

{
  uint32_t playbackRateHz;
  Bool8 accessFailed;
  MovieRuntime *introMovie;

  DebugHook_BeforeIntroMovies();
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    g_GraphicsFramebufferFillRectArgb
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
  }
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    g_GraphicsFramebufferFillRectArgb
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
  }
  if (g_CommandLineFindOption(sizeof g_CommandLineOptionNoIntro,g_CommandLineOptionNoIntro) == nullptr) {
    /* playbackRateHz: the rate from Movie_Open, passed on to TimerRegisterPeriodic */
    while (Movie_Open(1,(uint16_t *)g_FlmIntro0FlmPathUtf16,&playbackRateHz,nullptr)) {
      if (!Movie_AdvanceFrame(&introMovie,nullptr)) {
        Movie_Close();
        return true;
      }
      g_IntroMoviePendingTicks = 0;
      UiFrame_FlushInputAndResetPendingTicks();
      g_TimerRegisterPeriodic(playbackRateHz,IntroMovie_TimerTick);
      while (!IntroMovie_PollSkipRequest()) {
        if ((g_IntroMoviePendingTicks != 0) && !IntroMovie_PresentPendingFrames(introMovie)) break;
      }
      /* stop playback: key, mouse button release, movie end or framebuffer loss */
      g_TimerUnregisterPeriodic(IntroMovie_TimerTick);
      Movie_Close();
      g_FlmIntro0FlmPathUtf16[9] = g_FlmIntro0FlmPathUtf16[9] + 1;
    }
  }
  return false;
}


/* Binds the bootstrap API table: every entry starts out holding a procedure name and its DLL name and
   has the name replaced by the resolved procedure address. DLLs that are not mapped yet are loaded with
   the table's first entry (LoadLibraryA, resolved first) and recorded in g_DynamicModules. On failure the
   DLL/procedure name is stored for the fatal-error message and a FATAL_ERROR_* code is returned; 0 when
   every entry is bound. (The original returned the last resolved procedure on success; ProcessEntry only
   passes it through the fatal-error handler, which ignores it.)
*/
uint32_t DynAPI_Bootstrap()

{
  void **resolvedProcedure;
  HINSTANCE module;
  DynamicApiBinding *bindingCursor;
  void **procedureName; /* the unresolved destination slot still holds the procedure name */
  char *moduleName;
  uint32_t moduleSlotIndex;

  for (bindingCursor = g_BootstrapApiBindings; bindingCursor->destination != nullptr; bindingCursor++) {
    procedureName = bindingCursor->destination;
    module = GetModuleHandleA(bindingCursor->moduleName);
    if (module == nullptr) {
      /* dynapi_9 is the string "LoadLibraryA": without its module nothing can be loaded */
      if (bindingCursor->destination == (void **)dynapi_9) {
        Text_CopyNarrowToUtf16(256,g_PackageLastErrorPath,(uint8_t *)bindingCursor->moduleName);
        return FATAL_ERROR_LOADER_MODULE_MISSING;
      }
      module = ((BootstrapLoadLibraryAProc)g_BootstrapApiBindings[BOOTSTRAP_API_LOAD_LIBRARY_A].destination)(bindingCursor->moduleName);
      moduleSlotIndex = g_DynamicModuleCount;
      if (module == nullptr) {
        Text_CopyNarrowToUtf16(256,g_PackageLastErrorPath,(uint8_t *)bindingCursor->moduleName);
        return FATAL_ERROR_DLL_LOAD_FAILED;
      }
      g_DynamicModuleCount++;
      moduleName = bindingCursor->moduleName;
      g_DynamicModules[moduleSlotIndex].module = module;
      g_DynamicModules[moduleSlotIndex].name = moduleName;
      procedureName = bindingCursor->destination;
    }
    resolvedProcedure = (void **)GetProcAddress(module,(LPCSTR)procedureName);
    if (resolvedProcedure == nullptr) {
      Text_CopyNarrowToUtf16(256,g_PackageLastErrorPath,(uint8_t *)bindingCursor->destination);
      Text_CopyNarrowToUtf16(256,g_FatalErrorDetail1Utf16,(uint8_t *)bindingCursor->moduleName);
      return FATAL_ERROR_DLL_PROCEDURE_MISSING;
    }
    bindingCursor->destination = resolvedProcedure;
  }
  return 0;
}


/* Looks up a command-line option (stored uppercased without its '/' or '-' by CommandLine_Parse) in
   g_CommandLine.optionBuffer, a list of NUL-terminated strings ending with an empty one. Only the first
   length bytes are compared (case-sensitive): a length including the NUL asks for an exact match, a shorter
   one for a prefix such as an option name followed by its value. Returns the stored option (never NULL)
   when found, NULL otherwise.
*/
uint8_t *CommandLine_FindOption(CommandLineOptionLengthBytes length,char *option)

{
  uint32_t compareBytesRemaining;
  int bytesToBufferEnd;
  char *optionCompareCursor;
  char *scanCursor;
  char *storedOption;
  char *storedOptionCompareCursor;
  Bool8 comparedBytesEqual;
  char scannedByte;

  storedOption = g_CommandLine.optionBuffer;
  while (*storedOption != '\0') {
    /* compare up to length bytes, stopping at the first difference; length 0 never matches */
    comparedBytesEqual = false;
    optionCompareCursor = option;
    storedOptionCompareCursor = storedOption;
    for (compareBytesRemaining = length; compareBytesRemaining != 0; compareBytesRemaining--) {
      comparedBytesEqual = *optionCompareCursor == *storedOptionCompareCursor;
      optionCompareCursor++;
      storedOptionCompareCursor++;
      if (!comparedBytesEqual) break;
    }
    if (comparedBytesEqual) {
      return (uint8_t *)storedOption;
    }
    /* skip to the byte after the NUL, at most to the end of optionBuffer (the original used the address of
       sz_MainWindowTitle, which directly followed optionBuffer in its image) */
    bytesToBufferEnd = (int)(g_CommandLine.optionBuffer + sizeof g_CommandLine.optionBuffer - storedOption);
    scanCursor = storedOption;
    while (bytesToBufferEnd != 0) {
      bytesToBufferEnd--;
      scannedByte = *scanCursor;
      scanCursor++;
      if (scannedByte == '\0') break;
    }
    storedOption = scanCursor;
  }
  return nullptr;
}


/* ASCII a-z to A-Z, every other byte unchanged. */
static uint8_t CommandLine_UppercaseAscii(uint8_t character)

{
  if ((character >= 'a') && (character <= 'z')) {
    character = character - ('a' - 'A');
  }
  return character;
}


/* First positional argument slot still empty, NULL when all three are used. */
static char *CommandLine_FindFreeArgumentSlot()

{
  if (g_CommandLine.argument1[0] == '\0') {
    return g_CommandLine.argument1;
  }
  if (g_CommandLine.argument2[0] == '\0') {
    return g_CommandLine.argument2;
  }
  if (g_CommandLine.argument3[0] == '\0') {
    return g_CommandLine.argument3;
  }
  return nullptr;
}


/* Skips the command line up to and including terminator. Returns the position after it, or NULL when the
   command line ends first. */
static uint8_t *CommandLine_SkipPast(uint8_t *cursor,uint8_t terminator)

{
  uint8_t currentChar;

  do {
    currentChar = *cursor;
    cursor++;
    if (currentChar == '\0') {
      return nullptr;
    }
  } while (currentChar != terminator);
  return cursor;
}


/* Copies the command line uppercased into destination up to terminator, which is stored as the NUL.
   Returns the position OF the terminator (not after it), or NULL when the command line ends first (its NUL
   copied as well). */
static uint8_t *CommandLine_CopyUppercasedUntil(uint8_t *cursor,char *destination,uint8_t terminator)

{
  uint8_t currentChar;

  currentChar = CommandLine_UppercaseAscii(*cursor);
  while (currentChar != terminator) {
    *destination = currentChar;
    if (currentChar == '\0') {
      return nullptr;
    }
    destination++;
    cursor++;
    currentChar = CommandLine_UppercaseAscii(*cursor);
  }
  *destination = '\0';
  return cursor;
}


/* Copies the executable path (the first word, or the quoted part without its quotes) into
   g_CommandLine.executablePath. Returns the position after it, or NULL when the command line ends there
   (a quoted path without its closing quote is discarded). */
static uint8_t *CommandLine_CopyExecutablePath(uint8_t *cursor)

{
  char *pathWrite;
  uint8_t currentChar;

  pathWrite = g_CommandLine.executablePath;
  if (*cursor == '"') {
    cursor++;
    currentChar = *cursor;
    cursor++;
    while (currentChar != '"') {
      if (currentChar == '\0') {
        *pathWrite = '\0';
        /* no closing quote: the path is discarded */
        g_CommandLine.executablePath[0] = '\0';
        return nullptr;
      }
      *pathWrite = currentChar;
      pathWrite++;
      currentChar = *cursor;
      cursor++;
    }
  }
  else {
    currentChar = *cursor;
    cursor++;
    while (currentChar != ' ') {
      *pathWrite = currentChar;
      if (currentChar == '\0') {
        return nullptr;
      }
      pathWrite++;
      currentChar = *cursor;
      cursor++;
    }
  }
  *pathWrite = '\0';
  return cursor;
}


/* Copies one option (cursor just after its '/' or '-') uppercased up to the next space to *optionWrite,
   quoted parts verbatim with their quotes, and NUL-terminates it. Returns the position after the space, or
   NULL when the command line ends inside the option (its NUL copied as well). */
static uint8_t *CommandLine_CopyOption(uint8_t *cursor,char **optionWrite)

{
  char *write;
  uint8_t currentChar;

  write = *optionWrite;
  currentChar = CommandLine_UppercaseAscii(*cursor);
  cursor++;
  while (currentChar != ' ') {
    *write = currentChar;
    write++;
    if (currentChar == '\0') {
      return nullptr;
    }
    if (currentChar == '"') {
      do {
        currentChar = *cursor;
        *write = currentChar;
        cursor++;
        write++;
        if (currentChar == '\0') {
          return nullptr;
        }
      } while (currentChar != '"');
    }
    currentChar = CommandLine_UppercaseAscii(*cursor);
    cursor++;
  }
  *write = '\0';
  *optionWrite = write + 1;
  return cursor;
}


/* Copies a quoted positional argument (cursor just after its opening quote) uppercased into the first free
   slot, or skips it when all three are used. Returns where parsing continues, or NULL when the command line
   ends inside the argument. */
static uint8_t *CommandLine_CopyQuotedArgument(uint8_t *cursor)

{
  char *argumentSlot;

  argumentSlot = CommandLine_FindFreeArgumentSlot();
  if (argumentSlot == nullptr) {
    return CommandLine_SkipPast(cursor,'"');
  }
  /* Original quirk: parsing continues ON the closing quote, which is then read again as the opening quote
     of a further quoted argument. */
  return CommandLine_CopyUppercasedUntil(cursor,argumentSlot,'"');
}


/* Copies an unquoted positional argument (firstChar already read, cursor after it) uppercased into the first
   free slot up to the next space, or skips it when all three are used. Returns where parsing continues, or
   NULL when the command line ends inside the argument. */
static uint8_t *CommandLine_CopyArgument(uint8_t firstChar,uint8_t *cursor)

{
  char *argumentSlot;

  argumentSlot = CommandLine_FindFreeArgumentSlot();
  if (argumentSlot == nullptr) {
    return CommandLine_SkipPast(cursor,' ');
  }
  argumentSlot[0] = CommandLine_UppercaseAscii(firstChar);
  /* continues on the space after the argument, which is skipped next */
  return CommandLine_CopyUppercasedUntil(cursor,argumentSlot + 1,' ');
}


/* Splits the process command line (GetCommandLineA) into g_CommandLine and installs CommandLine_FindOption
   as the lookup hook: the executable path without quotes, up to three positional arguments (quotes
   removed, further ones skipped) and every '/' or '-' option without its prefix, NUL-separated in
   optionBuffer (quoted parts kept verbatim with their quotes). Everything else is uppercased (ASCII a-z
   only). The 256-byte buffers are not bounds-checked. The arguments are also stored as UTF-16.
*/
void CommandLine_Parse()

{
  uint8_t *commandLineCursor;
  char *optionWrite;
  uint8_t currentChar;

  g_CommandLineFindOption = CommandLine_FindOption;
  commandLineCursor = CommandLine_CopyExecutablePath((uint8_t *)GetCommandLineA());
  optionWrite = g_CommandLine.optionBuffer;
  /* options and positional arguments, until the terminating NUL (a NULL cursor: it ended inside one) */
  while (commandLineCursor != nullptr) {
    currentChar = *commandLineCursor;
    commandLineCursor++;
    if (currentChar == '\0') {
      break;
    }
    if ((currentChar == '/') || (currentChar == '-')) {
      commandLineCursor = CommandLine_CopyOption(commandLineCursor,&optionWrite);
    }
    else if (currentChar == '"') {
      commandLineCursor = CommandLine_CopyQuotedArgument(commandLineCursor);
    }
    else if (currentChar != ' ') {
      commandLineCursor = CommandLine_CopyArgument(currentChar,commandLineCursor);
    }
  }
  Text_CopyNarrowToUtf16
            (sizeof g_CommandLineWideArguments.argument1,g_CommandLineWideArguments.argument1,
             (uint8_t *)g_CommandLine.argument1);
  Text_CopyNarrowToUtf16
            (sizeof g_CommandLineWideArguments.argument2,g_CommandLineWideArguments.argument2,
             (uint8_t *)g_CommandLine.argument2);
  Text_CopyNarrowToUtf16
            (sizeof g_CommandLineWideArguments.argument3,g_CommandLineWideArguments.argument3,
             (uint8_t *)g_CommandLine.argument3);
  return;
}

