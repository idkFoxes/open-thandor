/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/platform/bootstrap/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/test_aids.h>
#include <thandor/platform/debug/movie_player.h>

/* Implementation ownership: platform/bootstrap/runtime. */

/* Address: 0x00585D40.
   Process entry: raises the process to real-time priority, creates the full-screen main window (only
   one instance may run), initialises every subsystem, sets the initial 640x480 display mode from the
   saved adapter and colour depth, runs the game and shuts down. Any failed step ends in the
   fatal-error dispatcher; a missing sound device is tolerated when -SOUND is not on the command line.
*/
void __cdecl ProcessEntry(void)

{
  HANDLE processOrThreadHandle;
  HINSTANCE windowInstance;
  int screenHeight;
  int screenWidth;
  uint32_t networkResult;
  uint32_t bootstrapError;
  uint32_t graphicsError;
  uint32_t bitsPerPixel;
  uint32_t adapterIndex;
  uint32_t mouseInitError;
  StatusResult soundResult;
  FatalErrorCheckResult fatalResult;
  DisplayModeResult displayModeResult;
  CommandLineOptionResult soundOption;
  uint32_t displayWidth;
  uint32_t displayHeight;

  g_hInstance = GetModuleHandleA(NULL);
  processOrThreadHandle = GetCurrentProcess();
  SetPriorityClass(processOrThreadHandle,REALTIME_PRIORITY_CLASS);
  processOrThreadHandle = GetCurrentThread();
  SetThreadPriority(processOrThreadHandle,THREAD_PRIORITY_NORMAL);
  CommandLine_Parse();
#ifdef THANDOR_TEST_AIDS
  if ((FindWindowA(sz_MainWindowClass,NULL) == NULL) || Thandor_TestAidAllowSecondInstance()) {
#else
  if (FindWindowA(sz_MainWindowClass,NULL) == NULL) {
#endif
    g_MainWindowClass.instance = g_hInstance;
    g_MainWindowClass.icon = LoadIconA(g_hInstance,MAKEINTRESOURCEA(1));
    g_MainWindowClass.cursor = LoadCursorA(NULL,IDC_ARROW);
    if (RegisterClassA((WNDCLASSA *)&g_MainWindowClass) != 0) { /* the original tests the 16-bit ATOM in AX */
      windowInstance = g_hInstance; /* read before the GetSystemMetrics calls, as in the original */
      screenHeight = GetSystemMetrics(SM_CYSCREEN);
      screenWidth = GetSystemMetrics(SM_CXSCREEN);
#ifdef THANDOR_TEST_AIDS
      if (Thandor_TestAidWindowed()) {
        /* test aid (not in the original): a normal window instead of the full-screen topmost popup */
        g_MainWindow = (HWND)Thandor_TestAidCreateWindowedMainWindow(sz_MainWindowClass,sz_MainWindowTitle,
                                                                     windowInstance);
      }
      else {
        g_MainWindow = CreateWindowExA(WS_EX_TOPMOST,sz_MainWindowClass,sz_MainWindowTitle,WS_POPUP | WS_SYSMENU,
                                       0,0,screenWidth,screenHeight,NULL,NULL,windowInstance,NULL);
      }
#else
      g_MainWindow = CreateWindowExA(WS_EX_TOPMOST,sz_MainWindowClass,sz_MainWindowTitle,WS_POPUP | WS_SYSMENU,
                                     0,0,screenWidth,screenHeight,NULL,NULL,windowInstance,NULL);
#endif
      if (g_MainWindow != NULL) {
        ShowWindow(g_MainWindow,SW_SHOWNORMAL);
        UpdateWindow(g_MainWindow);
        ArenaHeap_Init();
        FileSystem_Init();
        Locale_Init();
        ErrorSystem_Init();
        if (g_CpuFeatureFlags == 0) {
          FatalError_ExitIfFailed(FATAL_ERROR_CPU_WITHOUT_MMX,true);
        }
        bootstrapError = DynAPI_Bootstrap();
        fatalResult = FatalError_ExitIfFailed(bootstrapError,bootstrapError != 0);
        /* TimerSystem_Init only installs the timer procs and always clears CF */
        TimerSystem_Init();
        fatalResult = FatalError_ExitIfFailed(fatalResult.valueOrError,false);
        graphicsError = Graphics_Init();
        FatalError_ExitIfFailed(graphicsError,graphicsError != 0);
        if (!DirectInputMouse_Init(&mouseInitError)) {
          FatalError_ExitIfFailed(mouseInitError,true);
        }
        soundResult = DirectSound_Init();
        Thandor_Log("DirectSound_Init: %s", soundResult.failed ? "failed (continuing without sound)" : "ok");
        if (soundResult.failed) {
          /* without a sound device the game only stops when -SOUND demands sound */
          soundOption = CommandLine_FindOption(sizeof g_CommandLineOptionSound,g_CommandLineOptionSound);
          if (!soundOption.notFound) {
            FatalError_ExitIfFailed(soundResult.valueOrError,true);
          }
        }
        networkResult = Network_Init();
        /* Network_Init leaves with CF clear on every path, failures included (CLC at 0x00584DD6 and
           0x00584DE0), so a missing WinSock is never fatal: the check below always passes. */
        FatalError_ExitIfFailed(networkResult,false);
        PersistentSettings_Load();
        displayWidth = GAME_START_DISPLAY_WIDTH;
        displayHeight = GAME_START_DISPLAY_HEIGHT;
        bitsPerPixel = PersistentSettings_Read(PERSISTENT_DEFAULT_BITS_PER_PIXEL,PERSISTENT_SETTING_BITS_PER_PIXEL);
        adapterIndex = PersistentSettings_Read(PERSISTENT_DEFAULT_ADAPTER_INDEX,PERSISTENT_SETTING_ADAPTER_INDEX);
        if (g_GraphicsAdapterCount <= adapterIndex) {
          adapterIndex = 0;
        }
        displayModeResult =
             g_GraphicsSetDisplayMode(adapterIndex,bitsPerPixel,displayHeight,displayWidth);
        FatalError_ExitIfFailed(displayModeResult.valueOrError,displayModeResult.failed);
        UiRuntime_Initialize();
        Game_Run();
        Runtime_Shutdown();
        DestroyWindow(g_MainWindow);
      }
    }
  }
  ExitProcess(0);
}


/* Address: 0x00512E70.
   Resets the game data to the defaults of a new game: clears the auxiliary state and the eight faction
   records, gives every faction its own capability bit, the base technology, a rotated relation pattern
   (0xF for itself, 1 for everyone else) and the starting economy limits, and replaces the stat table with a
   fresh zeroed one. Returns 0, or the allocator's (non-zero) error code when the stat table cannot be
   allocated (the old one then stays).
*/
uint32_t GameData_ResetDefaults(void)

{
  FactionCapabilityFlags *capabilityFlagsSlot;
  void *previousStatTable;
  int remainingCount;
  uint32_t relationStatePattern;
  uint32_t factionBit;
  GameFactionRuntimeRecord *factionRecord;
  uint32_t *dwordCursor;
  uint32_t *statTableCursor;
  ArenaAllocResult allocResult;

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
  allocResult = g_MemoryApi.alloc(GAME_STAT_TABLE_BYTES);
  previousStatTable = g_GameStatTableImage;
  if (allocResult.failed) {
    return allocResult.payloadOrError;
  }
  LOCK();
  UNLOCK();
  g_GameStatTableImage = (void *)allocResult.payloadOrError;
  g_MemoryApi.free(previousStatTable);
  statTableCursor = (uint32_t *)allocResult.payloadOrError;
  for (remainingCount = GAME_STAT_TABLE_BYTES / 4; remainingCount != 0; remainingCount--) {
    *statTableCursor = 0;
    statTableCursor++;
  }
  statTableCursor[-1] = UINT32_MAX; /* end marker */
  g_GameFactionRuntimeImage.tail.periodicClockTick = 0;
  return 0;
}


/* Address: 0x00512F60.
   Loads the game data of a level or savegame from the mounted packages: daten.hex is the faction image,
   stat.hex replaces the stat table and oldunit.hex (record count, primary table, secondary table) fills the
   old-unit tables; without oldunit.hex both tables and the count are cleared. CF is set when daten.hex or
   stat.hex cannot be loaded.
*/
bool GameData_LoadExternalTables(void)

{
  void *previousStatTable;
  uint32_t *oldUnitBufferOrCursor;
  int remainingCount;
  uint32_t *sourceCursor;
  uint32_t *destinationCursor;
  PackageLoadResult packageEntry;

  remainingCount = sizeof g_GameDataAuxState.pairPressureMatrix8x8 / 4;
  oldUnitBufferOrCursor = g_GameDataAuxState.pairPressureMatrix8x8;
  for (; remainingCount != 0; remainingCount--) {
    *oldUnitBufferOrCursor = 0;
    oldUnitBufferOrCursor++;
  }
  if (Package_LoadEntryIntoBuffer
                    (GAME_FACTION_IMAGE_BYTES,(uint8_t *)&g_GameFactionRuntimeImage,
                     (uint16_t *)u_daten_hex_0050e054,NULL)) {
    packageEntry = Package_LoadEntry((uint16_t *)u_stat_hex_0050e082);
    previousStatTable = g_GameStatTableImage;
    if (!packageEntry.failed) {
      LOCK();
      UNLOCK();
      g_GameStatTableImage = packageEntry.bufferOrError;
      g_MemoryApi.free(previousStatTable);
      packageEntry = Package_LoadEntry((uint16_t *)u_oldunit_hex_0050e094);
      oldUnitBufferOrCursor = packageEntry.bufferOrError;
      if (packageEntry.failed) {
        oldUnitBufferOrCursor = g_OldUnitPrimaryTable;
        for (remainingCount = OLD_UNIT_PRIMARY_TABLE_BYTES / 4; remainingCount != 0; remainingCount--) {
          *oldUnitBufferOrCursor = 0;
          oldUnitBufferOrCursor++;
        }
        oldUnitBufferOrCursor = g_OldUnitSecondaryTable;
        for (remainingCount = OLD_UNIT_SECONDARY_TABLE_BYTES / 4; remainingCount != 0; remainingCount--) {
          *oldUnitBufferOrCursor = 0;
          oldUnitBufferOrCursor++;
        }
        g_OldUnitRecordCount = 0;
      }
      else {
        g_OldUnitRecordCount = *oldUnitBufferOrCursor;
        destinationCursor = g_OldUnitPrimaryTable;
        sourceCursor = oldUnitBufferOrCursor;
        /* the source advances before each copy, so the first dword (the count) is skipped */
        for (remainingCount = OLD_UNIT_PRIMARY_TABLE_BYTES / 4; sourceCursor++, remainingCount != 0;
            remainingCount--) {
          *destinationCursor = *sourceCursor;
          destinationCursor++;
        }
        destinationCursor = g_OldUnitSecondaryTable;
        for (remainingCount = OLD_UNIT_SECONDARY_TABLE_BYTES / 4; remainingCount != 0; remainingCount--) {
          *destinationCursor = *sourceCursor;
          sourceCursor++;
          destinationCursor++;
        }
        Resource_Release(oldUnitBufferOrCursor);
      }
      return false;
    }
  }
  return true;
}


/* Address: 0x00573BC0.
   Resolves procedureName in module with GetProcAddress and stores it in *destination; returns 0. The name is
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
  if (resolvedProcedure != NULL) {
    *destination = resolvedProcedure;
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


/* Address: 0x00573C50.
   Loads the DLL moduleName with the bound LoadLibraryA and records it in g_DynamicModules so that
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
    if (loadedModule != NULL) {
      g_DynamicModules[g_DynamicModuleCount].module = loadedModule;
      g_DynamicModules[moduleSlotIndex].name = moduleName;
      g_DynamicModuleCount++;
      return loadedModule;
    }
  }
  return NULL;
}


/* Address: 0x00573CD0.
   Frees a DLL loaded by DynDLL_Load; the module is found by its name pointer (not by comparing text), as the
   Glide backend passes the same name string it loaded with. Returns FreeLibrary's non-zero result, or
   FATAL_ERROR_LOADER_MODULE_MISSING with the name in g_PackageLastErrorPath. The table entry stays in place.
   The original also reports success/failure in CF (CLC at 0x00573D2A, STC at 0x00573D12); no caller reads it:
   Glide3_InitAndEnumerate follows with CLC/STC, GraphicsGlide3_ApplyDisplayModeAndInitializeResources with STC,
   and Glide3_Shutdown passes it out unchanged, but its callers overwrite the flags first (ADD at 0x00578B5D,
   TEST at 0x00579582, XOR EAX,EAX at 0x00586102 after GraphicsBackend_ShutdownGlideOnDeactivate). The EAX
   result is likewise discarded (POP EAX or ignored) by all of them.
*/
uint32_t DynDLL_Unload(char *moduleName)

{
  uint32_t modulesRemainingOrResult; /* one register in the original: the loop count, then FreeLibrary's result */
  DynamicModuleEntry *moduleEntryCursor;

  moduleEntryCursor = g_DynamicModules;
  modulesRemainingOrResult = g_DynamicModuleCount;
  for (; modulesRemainingOrResult != 0; modulesRemainingOrResult--) {
    if (moduleName == moduleEntryCursor->name) {
      modulesRemainingOrResult = ((BootstrapFreeLibraryProc)g_BootstrapApiBindings[BOOTSTRAP_API_FREE_LIBRARY].destination)(moduleEntryCursor->module);
      if (modulesRemainingOrResult != 0) {
        return modulesRemainingOrResult;
      }
      break;
    }
    moduleEntryCursor++;
  }
  /* module not loaded, or FreeLibrary failed */
  Text_CopyNarrowToUtf16(256,g_PackageLastErrorPath,(uint8_t *)moduleName);
  return FATAL_ERROR_LOADER_MODULE_MISSING;
}

/* Address: 0x00573D40.
   Looks up the g_BootstrapApiBindings entry whose destination slot equals destination (a slot that still
   holds its name string), passes that name to the bound LoadLibraryA and stores the result in the slot.
   Returns true on success; false when no entry matches or the call fails (the name is left in
   g_PackageLastErrorPath). The original's EAX was FATAL_ERROR_LOADER_MODULE_MISSING on both paths, so it
   carried no information. No caller found in src/ or src/generated/image_data.c (only the function map lists it).
*/
bool BootstrapApi_ResolveBindingByDestination(void **destination)

{
  void *resolvedProcedure;
  uint32_t remainingCount;
  DynamicApiBinding *bindingCursor;

  bindingCursor = g_BootstrapApiBindings;
  /* the original bounds the walk with the loaded-module count, not with the size of the binding table */
  remainingCount = g_DynamicModuleCount;
  for (; remainingCount != 0; remainingCount--) {
    if (destination == bindingCursor->destination) {
      Text_CopyNarrowToUtf16(256,g_PackageLastErrorPath,(uint8_t *)destination);
      /* The original pushes ESI (the binding cursor) only to preserve it across the call: binding slot 0
         (LoadLibraryA) gets the destination argument as its single argument, and the result is stored
         into the matching binding (MOV [ESI],EDX after POP ESI). */
      resolvedProcedure =
           (void *)((BootstrapLoadLibraryAProc)g_BootstrapApiBindings[BOOTSTRAP_API_LOAD_LIBRARY_A].destination)((char *)destination);
      if (resolvedProcedure != NULL) {
        bindingCursor->destination = (void **)resolvedProcedure;
        return true;
      }
      break;
    }
    bindingCursor++;
  }
  return false;
}


/* Address: 0x00573EB0.
   Frees every DLL recorded in g_DynamicModules with the bound FreeLibrary at shutdown; each slot is cleared
   before the call so a module is never freed twice. The count is left unchanged.
*/
void DynDLL_UnloadAll(void)

{
  uint32_t modulesRemaining;
  DynamicModuleEntry *moduleEntryCursor;
  HINSTANCE loadedModule;

  moduleEntryCursor = g_DynamicModules;
  for (modulesRemaining = g_DynamicModuleCount; modulesRemaining != 0;
      modulesRemaining = modulesRemaining - 1) {
    if (moduleEntryCursor->module != NULL) {
      loadedModule = moduleEntryCursor->module;
      moduleEntryCursor->module = NULL;
      ((BootstrapFreeLibraryProc)g_BootstrapApiBindings[BOOTSTRAP_API_FREE_LIBRARY].destination)(loadedModule);
    }
    moduleEntryCursor++;
  }
  return;
}


/* Address: 0x00585F50.
   Window procedure of the main window (g_MainWindowClass.windowProc, registered by ProcessEntry). Counts
   WM_CLOSE/WM_DESTROY, hides the cursor and forwards keys and characters to the keyboard layer. On
   WM_ACTIVATEAPP it drops to normal priority, releases the mouse and lets the graphics backend give up the
   display when deactivated, and on reactivation returns to real-time priority, reacquires the mouse, restores
   the display mode and reseeds the lock-key state.
*/
LRESULT __stdcall MainWindowProc(HWND hwnd,Win32WindowMessageId message,WPARAM wParam,LPARAM lParam)

{
  uint16_t keyState;
  HANDLE currentProcess;
  LRESULT defaultResult;

  if ((message == WM_DESTROY) || (message == WM_CLOSE)) {
    g_WindowDestroyDepth++;
  }
  else if (message == WM_ACTIVATEAPP) {
    g_AppActive = wParam;
    if (wParam == 0) {
      currentProcess = GetCurrentProcess();
      SetPriorityClass(currentProcess,NORMAL_PRIORITY_CLASS);
      if (g_MouseDevice != NULL) {
        g_MouseDevice->lpVtbl->Unacquire(g_MouseDevice);
      }
      /* not while the window is being closed or destroyed */
      if (g_WindowDestroyDepth == 0) {
        g_GraphicsBackendRefreshActiveAdapter();
      }
    }
    else {
      currentProcess = GetCurrentProcess();
      SetPriorityClass(currentProcess,REALTIME_PRIORITY_CLASS);
      if (g_MouseDevice != NULL) {
        g_MouseDevice->lpVtbl->Acquire(g_MouseDevice);
      }
      /* only when a display mode was set; the red+green+blue bit count is rounded up to a multiple of 16
         (a 5-5-5 format asks for 16 bits per pixel) */
      if (-1 < (int)g_ActiveGraphicsAdapterIndex) {
        g_GraphicsSetDisplayMode
                  (g_ActiveGraphicsAdapterIndex,
                   (g_SoftwarePixelFormatConfig.redBitCount +
                   g_SoftwarePixelFormatConfig.greenBitCount +
                   g_SoftwarePixelFormatConfig.blueBitCount + 0xf) & 0xfffffff0,g_FramebufferHeight,
                   g_FramebufferWidth);
      }
      if (g_MouseDevice != NULL) {
        g_KeyboardStateMask = 0;
        /* bit 0 of GetKeyState is the toggle state of a lock key */
        keyState = GetKeyState(VK_NUMLOCK);
        if ((keyState & 1) != 0) {
          g_KeyboardStateMask = g_KeyboardStateMask | KEYBOARD_STATE_NUM_LOCK;
        }
        keyState = GetKeyState(VK_SCROLL);
        if ((keyState & 1) != 0) {
          g_KeyboardStateMask = g_KeyboardStateMask | KEYBOARD_STATE_SCROLL_LOCK;
        }
        keyState = GetKeyState(VK_CAPITAL);
        if ((keyState & 1) != 0) {
          g_KeyboardStateMask = g_KeyboardStateMask | KEYBOARD_STATE_CAPS_LOCK;
        }
        g_KeyboardFlushEvents();
      }
    }
  }
  else if (message == WM_SETCURSOR) {
    SetCursor(NULL);
  }
  else if ((message == WM_KEYDOWN) || (message == WM_SYSKEYDOWN)) {
    Keyboard_OnKeyDown(wParam);
  }
  else if ((message == WM_KEYUP) || (message == WM_SYSKEYUP)) {
    Keyboard_OnKeyUp(wParam);
  }
  else {
    if ((message != WM_CHAR) && (message != WM_SYSCHAR)) {
      defaultResult = DefWindowProcA(hwnd,message,wParam,lParam);
      return defaultResult;
    }
    Keyboard_OnChar(wParam);
  }
  return 0;
}


/* Address: 0x00587370.
   Sets CPU_FEATURE_MMX in g_CpuFeatureFlags when CPUID reports MMX; ProcessEntry refuses to run without it
   (FATAL_ERROR_CPU_WITHOUT_MMX). The constant return value 5 has no known use.
*/
uint32_t __cdecl CPU_DetectFeatures(void)

{
  int cpuidVersionInfo;

  cpuidVersionInfo = cpuid_Version_info(CPUID_LEAF_VERSION_INFO);
  /* offset 8 of the CPUID result is EDX */
  if ((*(uint32_t *)(cpuidVersionInfo + 8) & CPUID_EDX_MMX) != 0) {
    g_CpuFeatureFlags = g_CpuFeatureFlags | CPU_FEATURE_MMX;
  }
  return 5;
}

/* Address: 0x00573070.
   Runs the game once the subsystems are up: shows the first cursor frame, initialises spatial audio and
   rendering, loads the core assets and plays the intro movies (each failure is fatal). It then switches
   from the 640x480x16 start mode to the saved display mode if that differs, runs the frontend main loop and
   finally closes and cleans up the network backend.
*/
void __cdecl Game_Run(void)

{
  uint32_t renderingInitError;
  uint32_t loadResultOrWidth; /* Game_LoadCoreAssets result, later the saved display width */
  uint32_t displayHeight;
  uint32_t bitsPerPixel;
  uint32_t adapterIndex;
  bool introMoviesFailed;
  CursorFrameResult cursorFrameResult;
  FatalErrorCheckResult fatalResult;
  DisplayModeResult displayModeResult;
  uint32_t mainLoopError;

  cursorFrameResult = g_GraphicsCursorSetFrame(0);
  FatalError_ExitIfFailed(cursorFrameResult.errorCode,cursorFrameResult.failed);
  renderingInitError = GameRuntime_InitializeSpatialAudioAndRendering();
  FatalError_ExitIfFailed(renderingInitError,renderingInitError != 0);
  loadResultOrWidth = Game_LoadCoreAssets();
  Thandor_Log("Game_LoadCoreAssets -> 0x%08X", loadResultOrWidth);
  /* 0 with CF clear on success, an error code with CF set otherwise */
  fatalResult = FatalError_ExitIfFailed(loadResultOrWidth,loadResultOrWidth != 0);
  /* keeps EAX: a movie that cannot start is reported with the previous value */
  introMoviesFailed = Game_PlayIntroMovies();
  FatalError_ExitIfFailed(fatalResult.valueOrError,introMoviesFailed);
  PersistentSettings_Load();
  /* ProcessEntry started in 640x480x16; switch only when the saved mode differs */
  loadResultOrWidth = PersistentSettings_Read(GAME_START_DISPLAY_WIDTH,PERSISTENT_SETTING_DISPLAY_WIDTH);
  displayHeight = PersistentSettings_Read(GAME_START_DISPLAY_HEIGHT,PERSISTENT_SETTING_DISPLAY_HEIGHT);
  bitsPerPixel = PersistentSettings_Read(PERSISTENT_DEFAULT_BITS_PER_PIXEL,PERSISTENT_SETTING_BITS_PER_PIXEL);
  if (loadResultOrWidth != GAME_START_DISPLAY_WIDTH || displayHeight != GAME_START_DISPLAY_HEIGHT ||
      bitsPerPixel != PERSISTENT_DEFAULT_BITS_PER_PIXEL) {
    adapterIndex = PersistentSettings_Read(PERSISTENT_DEFAULT_ADAPTER_INDEX,PERSISTENT_SETTING_ADAPTER_INDEX);
    if (g_GraphicsAdapterCount <= adapterIndex) {
      adapterIndex = 0;
    }
    displayModeResult = g_GraphicsSetDisplayMode(adapterIndex,bitsPerPixel,displayHeight,loadResultOrWidth);
    FatalError_ExitIfFailed(displayModeResult.valueOrError,displayModeResult.failed);
    PersistentSettings_Write(g_ActiveGraphicsAdapterIndex,PERSISTENT_SETTING_ADAPTER_INDEX);
  }
  if (!Frontend_MainLoop(1,&mainLoopError)) {
    FatalError_ExitIfFailed(mainLoopError,true);
  }
  g_NetworkBackendSlot3(); /* close */
  g_NetworkBackendSlot1(); /* cleanup */
  return;
}


/* Address: 0x0050BB10.
   Game_Run's first startup step: initialises the spatial-sound pool, the terrain and intensity clamp tables,
   the software renderer's display-mode hook and the global primitive queue (0xA000 packets), in that order.
   Stops at the first step that fails and returns its (non-zero) error code; returns 0 when all succeed.
   The original returned g_PrimitiveQueueStorage in EAX on success; its only caller (Game_Run) discards it.
*/
uint32_t __cdecl GameRuntime_InitializeSpatialAudioAndRendering(void)

{
  StatusResult step;
  uint32_t poolError;
  uint32_t stepError;

  if (!SpatialSoundPool_Init(&poolError)) {
    return poolError;
  }
  step = TerrainByteClampLookup_Initialize();
  if (step.failed) {
    return step.valueOrError;
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


/* Address: 0x00573140.
   Loads everything the frontend needs once at startup: takes the CD path from the registry, mounts the patch,
   level and core packages, creates the seven UI button sounds, moves the screenshot name past the existing
   screen??.pcx files, loads the text pages, applies the sound settings, binds the PCX codec module and
   allocates the fixed runtime buffers. Returns 0, or the error code of the first failing step (the caller
   treats non-zero as failure).
*/
uint32_t __cdecl Game_LoadCoreAssets(void)

{
  wchar_t screenshotTensDigit;
  int statusOrCount;
  SoundSampleAsset *loadedResource; /* a button sample, later the engine\pcx.fnc package buffer */
  FncModuleHeader *module; /* a voice set or the PCX module; the error code on the failure paths */
  uint16_t *textBuffer;
  uint32_t soundOptionsOrBufferBase;
  AudioMixerGainQ15 uiSoundGain;
  MovieAudioGainQ15 movieGain;
  MovieAudioGainQ15 alternateMovieGain;
  float *splineBuffer;
  FrontendPlayerRuntimeRecord *playerRecordCursor;
  uint8_t *scratchCursor;
  TextResourceId resourceId;
  FrontendPlayerRuntimeRecord **playerRuntimePointerTableWriteCursor;
  uint32_t aiInitError;
  uint32_t packageHandle; /* mounted package handle (set on failure too, but then unused) */
  SampleVoiceSetResult voiceSetResult;
  FileSystemOpenResult openResult;
  uint16_t *resolvedText;
  uint32_t textPageError; /* the failing text page's error code */
  PackageLoadResult pcxModuleEntry;
  bool pcxModuleLoaded;
  uint32_t fncError;
  void *pcxExport;
  TextureSourceLoadResult panelTextureResult;
  ArenaAllocResult allocResult;
  uint32_t loadErrorCode;
  
  /* HKLM\Software\Planet4\Thandor "CD": movies are looked up under <CD>\Thandor first */
  if ((g_MemoryApi.alloc == ArenaHeap_Alloc) &&
     (statusOrCount = ((BootstrapRegOpenKeyExAProc)g_BootstrapApiBindings[BOOTSTRAP_API_REG_OPEN_KEY_EX_A].destination)
                        (HKEY_LOCAL_MACHINE,s_Software_Planet4_Thandor_00572e20,0,KEY_READ,
                         &g_InstallRegistryKeyHandle), statusOrCount == ERROR_SUCCESS)) {
    statusOrCount = ((BootstrapRegQueryValueExAProc)g_BootstrapApiBindings[BOOTSTRAP_API_REG_QUERY_VALUE_EX_A].destination)
                      (g_InstallRegistryKeyHandle,&s_InstallRegistryValueNameCD,0,
                       &g_InstallRegistryValueType,&g_InstallRegistryValueDataA,
                       &g_InstallRegistryValueDataCapacityBytes);
    if ((statusOrCount == ERROR_SUCCESS) && (g_InstallRegistryValueType == REG_SZ)) {
      Text_CopyNarrowToUtf16
                (sizeof g_InstallDirectoryScratchUtf16,g_InstallDirectoryScratchUtf16,&g_InstallRegistryValueDataA);
      WidePath_CombineDirectoryAndLeaf
                (g_LooseMoviePathPrefix.codeUnits,(uint16_t *)u_Thandor_00572e10,g_InstallDirectoryScratchUtf16);
    }
    ((BootstrapRegCloseKeyProc)g_BootstrapApiBindings[BOOTSTRAP_API_REG_CLOSE_KEY].destination)(g_InstallRegistryKeyHandle);
  }
  {
    /* open-thandor: the full-length movies from the CD (Ende*.flm, Intro2.flm) live in the
       flm folder of the game directory, so the CD is no longer needed. Movie_Open looks under
       g_LooseMoviePathPrefix before the packages, which only hold still-image stand-ins for
       these movies; point the prefix at the game directory when that folder exists. */
    static const uint16_t flmLeaf[4] = {'f','l','m',0};
    static uint16_t localFlmPath[256];
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
  /* patchNN.pck and then levelNN.pck, NN counting down to "00". decimalDigits.codeUnits[0] is the tens
     digit, [1] the ones digit; adding UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP to the packed pair decrements the
     tens digit and turns the ones digit from '0' - 1 back into '9'. The patch count starts at "00", so only
     patch00.pck is tried. */
  g_PatchArchivePathTemplateUtf16.decimalDigits.packedDigits = UTF16_DIGIT_PAIR('0','0');
  do {
    do {
      Package_Mount(g_PatchArchivePathTemplateUtf16.prefixCodeUnits,NULL);
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
  if (Package_Mount((uint16_t *)u_daten_pck_00572e56,&packageHandle)) {
    g_DataPackageHandle = packageHandle;
  }
  if (Package_Mount((uint16_t *)u_modelle_pck_00572e6a,&packageHandle)) {
    g_ModelPackageHandle = packageHandle;
  }
  if (Package_Mount((uint16_t *)u_graphik_pck_00572e82,&packageHandle)) {
    g_GraphicsPackageHandle = packageHandle;
  }
  if (Package_Mount((uint16_t *)u_sound_pck_00572e9a,&packageHandle)) {
    g_SoundPackageHandle = packageHandle;
  }
  if (Package_Mount((uint16_t *)u_filme_pck_00572eae,&packageHandle)) {
    g_MoviePackageHandle = packageHandle;
  }
  if (Package_Mount((uint16_t *)u_level_pck_00572ec2,&packageHandle)) {
    g_LevelPackageHandle = packageHandle;
  }
  if (!Resource_Load((uint16_t *)u_sound_button0_sam_00572f06,(void **)&loadedResource,NULL,&loadErrorCode)) {
    return loadErrorCode;
  }
  voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
  module = (FncModuleHeader *)voiceSetResult.voiceSet;
  if (!voiceSetResult.failed) {
    Resource_Release(loadedResource);
    g_UiButtonSoundVoiceSets7[0] = (DirectSoundVoiceSet *)module;
    if (!Resource_Load((uint16_t *)u_sound_button1_sam_00572f2a,(void **)&loadedResource,NULL,&loadErrorCode)) {
      return loadErrorCode;
    }
    voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
    module = (FncModuleHeader *)voiceSetResult.voiceSet;
    if (!voiceSetResult.failed) {
      Resource_Release(loadedResource);
      g_UiButtonSoundVoiceSets7[1] = (DirectSoundVoiceSet *)module;
      if (!Resource_Load((uint16_t *)u_sound_button2_sam_00572f4e,(void **)&loadedResource,NULL,&loadErrorCode)) {
        return loadErrorCode;
      }
      voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
      module = (FncModuleHeader *)voiceSetResult.voiceSet;
      if (!voiceSetResult.failed) {
        Resource_Release(loadedResource);
        g_UiButtonSoundVoiceSets7[2] = (DirectSoundVoiceSet *)module;
        if (!Resource_Load((uint16_t *)u_sound_button3_sam_00572f72,(void **)&loadedResource,NULL,&loadErrorCode)) {
          return loadErrorCode;
        }
        voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
        module = (FncModuleHeader *)voiceSetResult.voiceSet;
        if (!voiceSetResult.failed) {
          Resource_Release(loadedResource);
          g_UiButtonSoundVoiceSets7[3] = (DirectSoundVoiceSet *)module;
          if (!Resource_Load((uint16_t *)u_sound_button4_sam_00572f96,(void **)&loadedResource,NULL,&loadErrorCode)) {
            return loadErrorCode;
          }
          voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
          module = (FncModuleHeader *)voiceSetResult.voiceSet;
          if (!voiceSetResult.failed) {
            Resource_Release(loadedResource);
            g_UiButtonSoundVoiceSets7[4] = (DirectSoundVoiceSet *)module;
            if (!Resource_Load((uint16_t *)u_sound_button5_sam_00572fba,(void **)&loadedResource,NULL,&loadErrorCode)) {
              return loadErrorCode;
            }
            voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
            module = (FncModuleHeader *)voiceSetResult.voiceSet;
            if (!voiceSetResult.failed) {
              Resource_Release(loadedResource);
              g_UiButtonSoundVoiceSets7[5] = (DirectSoundVoiceSet *)module;
              if (!Resource_Load((uint16_t *)u_sound_button6_sam_00572fde,(void **)&loadedResource,NULL,&loadErrorCode)) {
                return loadErrorCode;
              }
              voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
              module = (FncModuleHeader *)voiceSetResult.voiceSet;
              if (!voiceSetResult.failed) {
                Resource_Release(loadedResource);
                g_UiButtonSoundVoiceSets7[6] = (DirectSoundVoiceSet *)module;
                /* u_Dscreen00_pcx_00572e3a + 1 is "screen00.pcx" ([7] tens digit, [8] ones digit): count up
                   to the first screenshot file that does not exist yet */
                do {
                  do {
                    openResult = g_FileSystemOpen(0,(uint16_t *)(u_Dscreen00_pcx_00572e3a + 1));
                    if (openResult.failed)
                      goto bindDebugOverlayTexts;
                    u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + 1;
                    g_FileSystemClose((void *)openResult.handleOrError);
                    screenshotTensDigit = u_Dscreen00_pcx_00572e3a[7];
                  } while ((uint16_t)u_Dscreen00_pcx_00572e3a[8] < '9' + 1);
                  u_Dscreen00_pcx_00572e3a[7] = u_Dscreen00_pcx_00572e3a[7] + 1;
                  u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] - 10;
                } while ((uint16_t)u_Dscreen00_pcx_00572e3a[7] < '9' + 1);
                /* all 100 names exist: the tens digit goes back to '0' (the last seen '9' - 9) */
                u_Dscreen00_pcx_00572e3a[7] = screenshotTensDigit - 9;
bindDebugOverlayTexts:
                /* bind placeholders 0..13 of the world view info texts to the debug-overlay text slots */
                resourceId = TEXT_ID_WORLD_VIEW_INFO_FIRST;
                do {
                  resolvedText = TextResource_Resolve(resourceId);
                  textBuffer = resolvedText;
                  resourceId = resourceId + 1;
                  RichTextCommandStream_PatchPayloadBySelector
                            (0,g_FrontendDebugOverlayTextSlot00Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (1,g_FrontendDebugOverlayTextSlot01Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (2,g_FrontendDebugOverlayTextSlot02Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (3,g_FrontendDebugOverlayTextSlot03Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (4,g_FrontendDebugOverlayTextSlot04Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (5,g_FrontendDebugOverlayTextSlot05Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (6,g_FrontendDebugOverlayTextSlot06Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (7,g_FrontendDebugOverlayTextSlot07Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (8,g_FrontendDebugOverlayTextSlot08Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (9,g_FrontendDebugOverlayTextSlot09Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (10,g_FrontendDebugOverlayTextSlot10Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (11,g_FrontendDebugOverlayTextSlot11Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (12,g_FrontendDebugOverlayTextSlot12Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (13,g_FrontendDebugOverlayTextSlot13Utf16,textBuffer);
                } while (resourceId < TEXT_ID_WORLD_VIEW_INFO_LAST + 1);
                UiActionHandlers_SetPage
                          (UI_ACTION_PAGE_INGAME,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage10);
                UiActionHandlers_SetPage
                          (UI_ACTION_PAGE_INGAME_COMMAND_MODE,
                           (UiActionHandlerPage *)&g_InGameUiActionHandlersPage11);
                UiActionHandlers_SetPage
                          (UI_ACTION_PAGE_INGAME_MENU,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage12);
                UiActionHandlers_SetPage
                          (UI_ACTION_PAGE_FRONTEND,(UiActionHandlerPage *)&g_FrontendUiActionHandlersPage20);
                if (!TextResourcePage_Load(GAME_TEXT_PAGE_NETERROR,(uint16_t *)u_texte_neterror_str_0050f104,&textPageError)) {
                  return textPageError;
                }
                if (!TextResourcePage_Load(GAME_TEXT_PAGE_HELP,(uint16_t *)u_texte_help_str_00563170,&textPageError)) {
                  return textPageError;
                }
                if (!TextResourcePage_Load(GAME_TEXT_PAGE_HILFE,(uint16_t *)u_texte_hilfe_str_00545b34,&textPageError)) {
                  return textPageError;
                }
                if (!TextResourcePage_Load(GAME_TEXT_PAGE_MENUE,(uint16_t *)u_texte_menue_str_00545ba0,&textPageError)) {
                  return textPageError;
                }
                if (!TextResourcePage_Load(GAME_TEXT_PAGE_TECHNO,(uint16_t *)u_texte_techno_str_0050dec4,&textPageError)) {
                  return textPageError;
                }
                if (!TextResourcePage_Load(GAME_TEXT_PAGE_LEVEL,(uint16_t *)u_texte_level_str_00545bc0,&textPageError)) {
                  return textPageError;
                }
                if (!TextResourcePage_Load(GAME_TEXT_PAGE_INHALT,(uint16_t *)u_texte_inhalt_str_00545be0,&textPageError)) {
                  return textPageError;
                }
                if (!TextResourcePage_Load(GAME_TEXT_PAGE_TASTATUR,(uint16_t *)u_texte_tastatur_str_005631b8,&textPageError)) {
                  return textPageError;
                }
                resolvedText = TextResource_Resolve(TEXT_ID_MOUSE_HELP);
                RichTextCommandStream_BindTextureSource(g_CursorSourceAsset,resolvedText);
                /* sound effects off: every gain is 0 */
                soundOptionsOrBufferBase =
                     PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
                uiSoundGain = 0;
                if ((soundOptionsOrBufferBase & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
                  uiSoundGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN);
                }
                movieGain = 0;
                g_UiSoundGainQ15 = uiSoundGain;
                g_SoundEffectsGainQ15 = uiSoundGain;
                if ((soundOptionsOrBufferBase & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
                  movieGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
                }
                alternateMovieGain = 0;
                g_MovieDefaultAudioGainQ15 = movieGain;
                if ((soundOptionsOrBufferBase & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
                  alternateMovieGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
                }
                g_ReverseStereoMask = 0;
                if ((soundOptionsOrBufferBase & PERSISTENT_SOUND_OPTION_REVERSE_STEREO) != 0) {
                  g_ReverseStereoMask = 0xffffffff;
                }
                g_MovieAlternateAudioGainQ15 = alternateMovieGain;
                /* the original passes the reverse-stereo mask (0 or 0xFFFFFFFF) as the default here, still in
                   EAX from the store above (PUSH EAX at 0x00573674) */
                g_ModelLodDepthThresholdQ8 =
                     PersistentSettings_Read(g_ReverseStereoMask,PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD);
                if (!AiRuntime_InitWorkspace(&aiInitError)) {
                  return aiInitError;
                }
                pcxModuleEntry = Package_LoadEntry((uint16_t *)u_engine_pcx_fnc_00573028);
                if (pcxModuleEntry.failed) {
                  return (uint32_t)pcxModuleEntry.bufferOrError;
                }
                pcxModuleLoaded = FncModule_LoadAndRelocate(pcxModuleEntry.bufferOrError,&module,&fncError);
                if (!pcxModuleLoaded) {
                  module = (FncModuleHeader *)fncError; /* the error code for the failure exit below */
                }
                loadedResource = (SoundSampleAsset *)pcxModuleEntry.bufferOrError;
                if (pcxModuleLoaded) {
                  g_PcxFunctionModule = module;
                  fncError = FncModule_GetExportByIndex(3,module,&pcxExport);
                  module = (FncModuleHeader *)fncError;
                  if (fncError == 0) {
                    g_PcxFunctionExport3 = (PcxEncodeProc *)pcxExport;
                    fncError = FncModule_GetExportByIndex(2,g_PcxFunctionModule,&pcxExport);
                    module = (FncModuleHeader *)fncError;
                    if (fncError == 0) {
                      g_PcxFunctionExport2 = (PcxDecodeProc *)pcxExport;
                      /* EDX still holds the engine\pcx.fnc package buffer. */
                      Resource_Release((SoundSampleAsset *)pcxModuleEntry.bufferOrError);
                      panelTextureResult = g_GraphicsTextureSourceLoadPackageAsset
                                         ((uint16_t *)u_gfx_panel_stat_gfx_00573002);
                      if (panelTextureResult.failed) {
                        return (uint32_t)panelTextureResult.textureSource;
                      }
                      g_InGameStatusPanelTextureSource = panelTextureResult.textureSource;
                      allocResult = g_MemoryApi.alloc
                                              (RECENT_TEXT_HISTORY_SLOT_COUNT * sizeof(RecentTextHistorySlot));
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_RecentTextSlotStorage = (RecentTextHistorySlot *)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(OLD_UNIT_SECONDARY_TABLE_BYTES);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_OldUnitSecondaryTable = (uint32_t *)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(OLD_UNIT_PRIMARY_TABLE_BYTES);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_OldUnitPrimaryTable = (uint32_t *)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc
                                              (FRONTEND_PLAYER_LIST_ROW_COUNT * FRONTEND_PLAYER_LIST_ROW_BYTES);
                      soundOptionsOrBufferBase = allocResult.payloadOrError;
                      if (allocResult.failed) {
                        return soundOptionsOrBufferBase;
                      }
                      g_FrontendPlayerListRow1 = soundOptionsOrBufferBase + 1 * FRONTEND_PLAYER_LIST_ROW_BYTES;
                      g_FrontendPlayerListRow2 = soundOptionsOrBufferBase + 2 * FRONTEND_PLAYER_LIST_ROW_BYTES;
                      g_FrontendPlayerListRow3 = soundOptionsOrBufferBase + 3 * FRONTEND_PLAYER_LIST_ROW_BYTES;
                      g_FrontendPlayerListRow4 = soundOptionsOrBufferBase + 4 * FRONTEND_PLAYER_LIST_ROW_BYTES;
                      g_FrontendPlayerListRow5 = soundOptionsOrBufferBase + 5 * FRONTEND_PLAYER_LIST_ROW_BYTES;
                      g_FrontendPlayerListRow6 = soundOptionsOrBufferBase + 6 * FRONTEND_PLAYER_LIST_ROW_BYTES;
                      g_FrontendPlayerListRow7 = soundOptionsOrBufferBase + 7 * FRONTEND_PLAYER_LIST_ROW_BYTES;
                      g_FrontendPlayerListRows = soundOptionsOrBufferBase;
                      allocResult = g_MemoryApi.alloc(ROM_REGISTRY_SLOT_COUNT * sizeof(RomRegistrySlot));
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_RomRegistrySlots = (RomRegistrySlot *)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc
                                              (FRONTEND_SESSION_LIST_CAPACITY * sizeof(FrontendSessionDiscoveryRecord *));
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_FrontendSessionListRows = (FrontendSessionDiscoveryRecord **)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc
                                              (FRONTEND_SESSION_LIST_CAPACITY * sizeof(FrontendSessionDiscoveryRecord));
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_FrontendSessionDiscoveryRecords =
                           (FrontendSessionDiscoveryRecord *)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(INGAME_FACTION_STATUS_TEXT_BYTES);
                      textBuffer = (uint16_t *)allocResult.payloadOrError;
                      if (allocResult.failed) {
                        return (uint32_t)textBuffer;
                      }
                      g_InGameFactionStatusTextScratchUtf16 = textBuffer;
                      g_InGameFactionStatusTextScratchUtf16Mirror = textBuffer;
                      allocResult = g_MemoryApi.alloc(INGAME_PLAYER_LIST_TEXT_BYTES);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_InGamePlayerListTextScratchUtf16 = (uint16_t *)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(WORLD_MOTION_SPLINE_CHANNEL_COUNT *
                                                      CUBIC_SPLINE_MATRIX_FLOATS * sizeof(float));
                      splineBuffer = (float *)allocResult.payloadOrError;
                      if (allocResult.failed) {
                        return (uint32_t)splineBuffer;
                      }
                      g_WorldMotionSplineMatrixWorkspaces[1] = splineBuffer + 1 * CUBIC_SPLINE_MATRIX_FLOATS;
                      g_WorldMotionSplineMatrixWorkspaces[2] = splineBuffer + 2 * CUBIC_SPLINE_MATRIX_FLOATS;
                      g_WorldMotionSplineMatrixWorkspaces[3] = splineBuffer + 3 * CUBIC_SPLINE_MATRIX_FLOATS;
                      g_WorldMotionSplineMatrixWorkspaces[4] = splineBuffer + 4 * CUBIC_SPLINE_MATRIX_FLOATS;
                      g_WorldMotionSplineMatrixWorkspaces[5] = splineBuffer + 5 * CUBIC_SPLINE_MATRIX_FLOATS;
                      g_WorldMotionSplineMatrixWorkspaces[0] = splineBuffer;
                      /* one coefficient vector of CUBIC_SPLINE_MATRIX_ORDER floats per channel */
                      allocResult = g_MemoryApi.alloc(WORLD_MOTION_SPLINE_CHANNEL_COUNT *
                                                      CUBIC_SPLINE_MATRIX_ORDER * sizeof(float));
                      splineBuffer = (float *)allocResult.payloadOrError;
                      if (allocResult.failed) {
                        return (uint32_t)splineBuffer;
                      }
                      g_WorldMotionSplineCoefficientTables[1] = splineBuffer + 1 * CUBIC_SPLINE_MATRIX_ORDER;
                      g_WorldMotionSplineCoefficientTables[2] = splineBuffer + 2 * CUBIC_SPLINE_MATRIX_ORDER;
                      g_WorldMotionSplineCoefficientTables[3] = splineBuffer + 3 * CUBIC_SPLINE_MATRIX_ORDER;
                      g_WorldMotionSplineCoefficientTables[4] = splineBuffer + 4 * CUBIC_SPLINE_MATRIX_ORDER;
                      g_WorldMotionSplineCoefficientTables[5] = splineBuffer + 5 * CUBIC_SPLINE_MATRIX_ORDER;
                      g_WorldMotionSplineCoefficientTables[0] = splineBuffer;
                      allocResult = g_MemoryApi.alloc
                                              (SELECTION_PLAYER_BLOCK_COUNT * sizeof(SelectionPlayerRuntimeBlock));
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_SelectionPlayerBlocks = (SelectionPlayerRuntimeBlock *)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(FRONTEND_SNAPSHOT_PAYLOAD_BYTES);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_FrontendLocalPlayerPcxPreview = allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(TERRAIN_REGION_COLLECTION_CAPACITY * 8); /* 8-byte records */
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_TerrainRegionCollectionEntries = allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(800);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_FrontendPlayerMessageBuffers = allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc
                                    (FRONTEND_PLAYER_RUNTIME_RECORD_ALLOC_COUNT * sizeof(FrontendPlayerRuntimeRecord));
                      playerRecordCursor = (FrontendPlayerRuntimeRecord *)allocResult.payloadOrError;
                      if (allocResult.failed) {
                        return (uint32_t)playerRecordCursor;
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
                      statusOrCount = 32;
                      do {
                        *playerRuntimePointerTableWriteCursor = playerRecordCursor;
                        playerRuntimePointerTableWriteCursor++;
                        playerRecordCursor++;
                        statusOrCount--;
                      } while (statusOrCount != 0);
                      allocResult = g_MemoryApi.alloc
                                    (CORE_ASSET_SCRATCH_SLICE_COUNT * CORE_ASSET_SCRATCH_SLICE_BYTES);
                      scratchCursor = (uint8_t *)allocResult.payloadOrError;
                      if (allocResult.failed) {
                        return (uint32_t)scratchCursor;
                      }
                      g_CoreAssetScratchSlice1 = scratchCursor + 1 * CORE_ASSET_SCRATCH_SLICE_BYTES;
                      g_CoreAssetScratchSlice2 = scratchCursor + 2 * CORE_ASSET_SCRATCH_SLICE_BYTES;
                      g_CoreAssetScratchSlice3 = scratchCursor + 3 * CORE_ASSET_SCRATCH_SLICE_BYTES;
                      g_CoreAssetScratchSlice4 = scratchCursor + 4 * CORE_ASSET_SCRATCH_SLICE_BYTES;
                      g_CoreAssetScratchSlice5 = scratchCursor + 5 * CORE_ASSET_SCRATCH_SLICE_BYTES;
                      g_CoreAssetScratchSlice6 = scratchCursor + 6 * CORE_ASSET_SCRATCH_SLICE_BYTES;
                      g_CoreAssetScratchSlice0 = scratchCursor;
                      for (statusOrCount = CORE_ASSET_SCRATCH_SLICE_COUNT * CORE_ASSET_SCRATCH_SLICE_BYTES / 4;
                           statusOrCount != 0; statusOrCount--) {
                        scratchCursor[0] = 0;
                        scratchCursor[1] = 0;
                        scratchCursor[2] = 0;
                        scratchCursor[3] = 0;
                        scratchCursor += 4;
                      }
                      return 0;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  Resource_Release(loadedResource);
  return (uint32_t)module;
}


/* Address: 0x005739D0.
   Plays the intro movies flm\intro0.flm, intro1.flm, ... until one cannot be opened, unless -NOINTRO is given.
   Each movie runs at its own rate from IntroMovie_TimerTick, centred on the screen;
   a key or mouse-button release skips to the next one, Escape skips all of them (the number jumps to 9).
   CF is set only when the first frame of an opened movie cannot be decoded.
*/
bool Game_PlayIntroMovies(void)

{
  uint32_t frameHeightSnapshot;
  uint32_t playbackRateHz;
  uint32_t quarterFrameHeight;
  int frameAdvanceBudget;
  bool accessFailed;
  MovieFrameDimensionsEdxEax8 frameDimensions;
  MovieOpenResult openResult;
  MovieRuntime *introMovie;
  bool frameDecoded;
  KeyboardEventResult keyEvent;
  CommandLineOptionResult noIntroOption;
  CursorEventResult cursorEvent;
  
    {
    const char *exportMovies = getenv("OPEN_THANDOR_MOVIEEXPORT");
    const char *debugMovie = getenv("OPEN_THANDOR_MOVIE");
    if ((exportMovies != NULL) && (exportMovies[0] != 0)) {
      char names[256];
      char *name;
      strncpy(names, exportMovies, sizeof names - 1);
      names[sizeof names - 1] = 0;
      for (name = strtok(names, ","); name != NULL; name = strtok(NULL, ",")) {
        DebugMovie_ExportOne(name);
      }
      ExitProcess(0);
    }
    if ((debugMovie != NULL) && (debugMovie[0] != 0)) {
      DebugMovie_Run(debugMovie);
    }
  }
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
  noIntroOption = g_CommandLineFindOption(sizeof g_CommandLineOptionNoIntro,g_CommandLineOptionNoIntro);
  if (noIntroOption.notFound) {
    while (!(openResult = Movie_Open(1,(uint16_t *)u_flm_intro0_flm_00573046)).failed) {
      if (!Movie_AdvanceFrame(&introMovie,NULL)) {
        Movie_Close();
        return true;
      }
      g_IntroMoviePendingTicks = 0;
      UiFrame_FlushInputAndResetPendingTicks();
      playbackRateHz = openResult.playbackRateHz; /* PUSH ECX: rate left by Movie_Open (AdvanceFrame preserves ECX) */
      g_TimerRegisterPeriodic(playbackRateHz,IntroMovie_TimerTick);
      while( true ) {
        g_Win32PumpMessages();
        keyEvent = g_KeyboardReadEvent();
        if (!keyEvent.queueEmpty) {
          /* [9] is the digit of "flm\intro0.flm"; Escape moves on to intro9 (normally absent, which ends the
             intros) */
          if (keyEvent.eventCode == KEYBOARD_KEY_CODE_ESCAPE) {
            u_flm_intro0_flm_00573046[9] = L'8';
          }
          break;
        }
        cursorEvent = g_GraphicsCursorConsumeEvent();
        /* event types above RIGHT_PRESS are the button releases */
        if (!cursorEvent.queueEmpty && RIGHT_PRESS < cursorEvent.eventType) break;
        if (g_IntroMoviePendingTicks != 0) {
          /* catch up at most three frames per pass */
          frameAdvanceBudget = 3;
          do {
            frameDecoded = Movie_AdvanceFrame(NULL,NULL);
            frameHeightSnapshot = g_FramebufferHeight;
            if (!frameDecoded) break;
            g_IntroMoviePendingTicks--;
          } while ((g_IntroMoviePendingTicks != 0) && (--frameAdvanceBudget != 0));
          if (!frameDecoded) break;
          quarterFrameHeight = g_FramebufferHeight >> 2;
          accessFailed = g_GraphicsFramebufferBeginAccess();
          if (accessFailed) break;
          frameDimensions = Movie_GetFrameDimensions(); /* EDX:EAX = height:width */
          /* y = (H - H/4 - frameHeight) / 2 + H/8, i.e. vertically centred; the source is the movie
             returned by the first Movie_AdvanceFrame */
          g_GraphicsTextureSourceBlitSourceAlpha
                    (g_FramebufferHeight,g_FramebufferWidth,0,0,
                     ((int)((frameHeightSnapshot - quarterFrameHeight) - (int)(frameDimensions >> 32)) >> 1) +
                     (frameHeightSnapshot >> 3),
                     (int)(g_FramebufferWidth - (int)frameDimensions) >> 1,0,
                     (GraphicsTextureSourceAsset *)introMovie,g_FramebufferAccess);
          g_GraphicsFramebufferEndAccess();
          g_GraphicsFramebufferPresent(g_FramebufferAccess);
        }
      }
      /* stop playback: key, mouse button release, movie end or framebuffer loss */
      g_TimerUnregisterPeriodic(IntroMovie_TimerTick);
      Movie_Close();
      u_flm_intro0_flm_00573046[9] = u_flm_intro0_flm_00573046[9] + 1;
    }
  }
  return false;
}


/* Address: 0x00573DB0.
   Binds the bootstrap API table: every entry starts out holding a procedure name and its DLL name and
   has the name replaced by the resolved procedure address. DLLs that are not mapped yet are loaded with
   the table's first entry (LoadLibraryA, resolved first) and recorded in g_DynamicModules. On failure the
   DLL/procedure name is stored for the fatal-error message and a FATAL_ERROR_* code is returned; 0 when
   every entry is bound. (The original left the last resolved procedure in EAX on success; ProcessEntry only
   passes it through the fatal-error handler, which ignores it.)
*/
uint32_t DynAPI_Bootstrap(void)

{
  void **resolvedProcedure;
  HINSTANCE module;
  DynamicApiBinding *bindingCursor;
  void **procedureName; /* the unresolved destination slot still holds the procedure name */
  char *moduleName;
  uint32_t moduleSlotIndex;

  for (bindingCursor = g_BootstrapApiBindings; bindingCursor->destination != NULL; bindingCursor++) {
    procedureName = bindingCursor->destination;
    module = GetModuleHandleA(bindingCursor->moduleName);
    if (module == NULL) {
      /* dynapi_9 is the string "LoadLibraryA": without its module nothing can be loaded */
      if (bindingCursor->destination == (void **)dynapi_9) {
        Text_CopyNarrowToUtf16(256,g_PackageLastErrorPath,(uint8_t *)bindingCursor->moduleName);
        return FATAL_ERROR_LOADER_MODULE_MISSING;
      }
      module = ((BootstrapLoadLibraryAProc)g_BootstrapApiBindings[BOOTSTRAP_API_LOAD_LIBRARY_A].destination)(bindingCursor->moduleName);
      moduleSlotIndex = g_DynamicModuleCount;
      if (module == NULL) {
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
    if (resolvedProcedure == NULL) {
      Text_CopyNarrowToUtf16(256,g_PackageLastErrorPath,(uint8_t *)bindingCursor->destination);
      Text_CopyNarrowToUtf16(256,g_FatalErrorDetail1Utf16,(uint8_t *)bindingCursor->moduleName);
      return FATAL_ERROR_DLL_PROCEDURE_MISSING;
    }
    bindingCursor->destination = resolvedProcedure;
  }
  return 0;
}


/* Address: 0x00586110.
   Looks up a command-line option (stored uppercased without its '/' or '-' by CommandLine_Parse) in
   g_CommandLine.optionBuffer, a list of NUL-terminated strings ending with an empty one. Only the first
   length bytes are compared (case-sensitive): a length including the NUL asks for an exact match, a shorter
   one for a prefix such as an option name followed by its value. Returns CF clear and the stored option in
   EBX when found, CF set otherwise; EAX is preserved.
   Original register convention: result in EBX, CF set on failure; EAX, ECX and EDX preserved.
*/
CommandLineOptionResult CommandLine_FindOption(CommandLineOptionLengthBytes length,char *option)

{
  uint32_t compareBytesRemaining;
  int bytesToBufferEnd;
  char *compareOrScanCursor; /* the REPE CMPSB source, then the REPNE SCASB cursor */
  char *storedOption;
  char *storedOptionCompareCursor;
  bool comparedBytesEqual;
  CommandLineOptionResult foundResult;
  CommandLineOptionResult notFoundResult;
  char scannedByte;

  storedOption = g_CommandLine.optionBuffer;
  while (*storedOption != '\0') {
    /* REPE CMPSB: with length 0 ZF stays as left by the compare with 0 above, i.e. clear */
    comparedBytesEqual = false;
    compareBytesRemaining = length;
    compareOrScanCursor = option;
    storedOptionCompareCursor = storedOption;
    do {
      if (compareBytesRemaining == 0) break;
      compareBytesRemaining--;
      comparedBytesEqual = *compareOrScanCursor == *storedOptionCompareCursor;
      compareOrScanCursor++;
      storedOptionCompareCursor++;
    } while (comparedBytesEqual);
    if (comparedBytesEqual) {
      foundResult.notFound = false;
      foundResult.option = (uint8_t *)storedOption;
      return foundResult;
    }
    /* REPNE SCASB to the byte after the NUL; sz_MainWindowTitle directly follows optionBuffer and so
       marks the end of the buffer */
    bytesToBufferEnd = sz_MainWindowTitle - storedOption;
    compareOrScanCursor = storedOption;
    do {
      storedOption = compareOrScanCursor;
      if (bytesToBufferEnd == 0) break;
      bytesToBufferEnd--;
      storedOption = compareOrScanCursor + 1;
      scannedByte = *compareOrScanCursor;
      compareOrScanCursor = storedOption;
    } while (scannedByte != '\0');
  }
  notFoundResult.notFound = true;
  notFoundResult.option = NULL; /* EBX not written; all callers read it only with CF clear */
  return notFoundResult;
}


/* Address: 0x00586170.
   Splits the process command line (GetCommandLineA) into g_CommandLine and installs CommandLine_FindOption
   as the lookup hook: the executable path without quotes, up to three positional arguments (quotes
   removed, further ones skipped) and every '/' or '-' option without its prefix, NUL-separated in
   optionBuffer (quoted parts kept verbatim with their quotes). Everything else is uppercased (ASCII a-z
   only). The 256-byte buffers are not bounds-checked. The arguments are also stored as UTF-16.
*/
void CommandLine_Parse(void)

{
  uint8_t *commandLineNext;
  char *pathWriteNext;
  uint8_t currentChar;
  uint8_t *commandLineCursor;
  char *textWriteCursor;
  char *optionWriteNext;
  char *pathWriteCursor;
  char *argumentWriteCursor;

  g_CommandLineFindOption = CommandLine_FindOption;
  commandLineCursor = (uint8_t *)GetCommandLineA();
  pathWriteNext = g_CommandLine.executablePath;
  if (*commandLineCursor == '"') {
    commandLineCursor++;
    do {
      pathWriteCursor = pathWriteNext;
      currentChar = *commandLineCursor;
      *pathWriteCursor = currentChar;
      commandLineCursor++;
      if (currentChar == '\0') {
        /* no closing quote: the path is discarded */
        g_CommandLine.executablePath[0] = '\0';
        goto copyWideArguments;
      }
      pathWriteNext = pathWriteCursor + 1;
    } while (currentChar != '"');
    *pathWriteCursor = '\0';
    optionWriteNext = g_CommandLine.optionBuffer;
  }
  else {
    do {
      pathWriteCursor = pathWriteNext;
      currentChar = *commandLineCursor;
      *pathWriteCursor = currentChar;
      commandLineCursor++;
      if (currentChar == '\0') goto copyWideArguments;
      pathWriteNext = pathWriteCursor + 1;
    } while (currentChar != ' ');
    *pathWriteCursor = '\0';
    optionWriteNext = g_CommandLine.optionBuffer;
  }
  /* options and positional arguments, until the terminating NUL */
  for (;;) {
    currentChar = *commandLineCursor;
    commandLineCursor++;
    if ((currentChar == '/') || (currentChar == '-')) {
      /* option: copied uppercased up to the next space; quoted parts verbatim */
      do {
        while( true ) {
          textWriteCursor = optionWriteNext;
          currentChar = *commandLineCursor;
          if (('a' - 1 < currentChar) && (currentChar < 'z' + 1)) {
            currentChar = currentChar - ('a' - 'A');
          }
          *textWriteCursor = currentChar;
          commandLineCursor++;
          optionWriteNext = textWriteCursor + 1;
          if (currentChar == '\0') goto copyWideArguments;
          if (currentChar != '"') break;
          do {
            currentChar = *commandLineCursor;
            *optionWriteNext = currentChar;
            commandLineCursor++;
            optionWriteNext++;
            if (currentChar == '\0') goto copyWideArguments;
          } while (currentChar != '"');
        }
      } while (currentChar != ' ');
      *textWriteCursor = '\0';
      continue;
    }
    if (currentChar == '\0') break;
    if (currentChar == ' ') continue;
    if (currentChar == '"') {
      /* quoted positional argument: into the first free slot, or skipped when all three are used */
      commandLineNext = commandLineCursor;
      if (g_CommandLine.argument1[0] == '\0') {
        textWriteCursor = g_CommandLine.argument1;
      }
      else if (g_CommandLine.argument2[0] == '\0') {
        textWriteCursor = g_CommandLine.argument2;
      }
      else if (g_CommandLine.argument3[0] == '\0') {
        textWriteCursor = g_CommandLine.argument3;
      }
      else {
        do {
          currentChar = *commandLineCursor;
          commandLineCursor++;
          if (currentChar == '\0') goto copyWideArguments;
        } while (currentChar != '"');
        continue;
      }
      do {
        argumentWriteCursor = textWriteCursor;
        commandLineCursor = commandLineNext;
        currentChar = *commandLineCursor;
        if (('a' - 1 < currentChar) && (currentChar < 'z' + 1)) {
          currentChar = currentChar - ('a' - 'A');
        }
        *argumentWriteCursor = currentChar;
        if (currentChar == '\0') goto copyWideArguments;
        commandLineNext = commandLineCursor + 1;
        textWriteCursor = argumentWriteCursor + 1;
      } while (currentChar != '"');
      *argumentWriteCursor = '\0';
      continue;
    }
    /* unquoted positional argument */
    if (('a' - 1 < currentChar) && (currentChar < 'z' + 1)) {
      currentChar = currentChar - ('a' - 'A');
    }
    commandLineNext = commandLineCursor;
    if (g_CommandLine.argument1[0] == '\0') {
      textWriteCursor = g_CommandLine.argument1 + 1;
      g_CommandLine.argument1[0] = currentChar;
    }
    else if (g_CommandLine.argument2[0] == '\0') {
      textWriteCursor = g_CommandLine.argument2 + 1;
      g_CommandLine.argument2[0] = currentChar;
    }
    else {
      if (g_CommandLine.argument3[0] != '\0') {
        do {
          currentChar = *commandLineCursor;
          commandLineCursor++;
          if (currentChar == '\0') goto copyWideArguments;
        } while (currentChar != ' ');
        continue;
      }
      textWriteCursor = g_CommandLine.argument3 + 1;
      g_CommandLine.argument3[0] = currentChar;
    }
    do {
      argumentWriteCursor = textWriteCursor;
      commandLineCursor = commandLineNext;
      currentChar = *commandLineCursor;
      if (('a' - 1 < currentChar) && (currentChar < 'z' + 1)) {
        currentChar = currentChar - ('a' - 'A');
      }
      *argumentWriteCursor = currentChar;
      if (currentChar == '\0') goto copyWideArguments;
      commandLineNext = commandLineCursor + 1;
      textWriteCursor = argumentWriteCursor + 1;
    } while (currentChar != ' ');
    *argumentWriteCursor = '\0';
  }
copyWideArguments:
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

