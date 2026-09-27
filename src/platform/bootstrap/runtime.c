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
  uint32_t bitsPerPixel;
  uint32_t adapterIndex;
  StatusResult statusResult;
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
  if (FindWindowA(sz_MainWindowClass,NULL) == NULL) {
    g_MainWindowClass.instance = g_hInstance;
    g_MainWindowClass.icon = LoadIconA(g_hInstance,MAKEINTRESOURCEA(1));
    g_MainWindowClass.cursor = LoadCursorA(NULL,IDC_ARROW);
    if (RegisterClassA((WNDCLASSA *)&g_MainWindowClass) != 0) { /* the original tests the 16-bit ATOM in AX */
      windowInstance = g_hInstance; /* read before the GetSystemMetrics calls, as in the original */
      screenHeight = GetSystemMetrics(SM_CYSCREEN);
      screenWidth = GetSystemMetrics(SM_CXSCREEN);
      g_MainWindow = CreateWindowExA(WS_EX_TOPMOST,sz_MainWindowClass,sz_MainWindowTitle,WS_POPUP | WS_SYSMENU,
                                     0,0,screenWidth,screenHeight,NULL,NULL,windowInstance,NULL);
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
        statusResult = DynAPI_Bootstrap();
        fatalResult = FatalError_ExitIfFailed(statusResult.valueOrError,statusResult.failed);
        /* TimerSystem_Init only installs the timer procs and always clears CF */
        TimerSystem_Init();
        fatalResult = FatalError_ExitIfFailed(fatalResult.valueOrError,false);
        statusResult = Graphics_Init();
        FatalError_ExitIfFailed(statusResult.valueOrError,statusResult.failed);
        statusResult = DirectInputMouse_Init();
        FatalError_ExitIfFailed(statusResult.valueOrError,statusResult.failed);
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
        displayWidth = 640;
        displayHeight = 480;
        bitsPerPixel = PersistentSettings_Read(16,PERSISTENT_SETTING_BITS_PER_PIXEL);
        adapterIndex = PersistentSettings_Read(0,PERSISTENT_SETTING_ADAPTER_INDEX);
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
   fresh zeroed one. CF is set when the stat table cannot be allocated (the old one then stays).
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx GameData_ResetDefaults(void)

{
  FactionCapabilityFlags *capabilityFlagsSlot;
  void *previousStatTable;
  int remainingCount;
  uint32_t relationStatePattern;
  uint32_t factionBit;
  GameFactionRuntimeImage *factionRecordCursor;
  uint32_t *dwordCursor;
  uint32_t *statTableCursor;
  ArenaAllocResult allocResult;
  StatusResult status;

  remainingCount = 0x40;
  dwordCursor = g_GameDataAuxState.pairPressureMatrix8x8;
  for (; remainingCount != 0; remainingCount--) {
    *dwordCursor = 0;
    dwordCursor++;
  }
  /* clears the eight records (0x3A00 bytes) dword by dword, not the image tail */
  factionRecordCursor = &g_GameFactionRuntimeImage;
  for (remainingCount = 0xe80; remainingCount != 0; remainingCount--) {
    factionRecordCursor->records[0].xeniteCurrentQ4 = 0;
    factionRecordCursor = (GameFactionRuntimeImage *)&factionRecordCursor->records[0].xeniteStorageLimitQ4;
  }
  factionRecordCursor = &g_GameFactionRuntimeImage;
  remainingCount = 8;
  factionBit = 1;
  relationStatePattern = 0x1111111f; /* one nibble per faction, rotated by one nibble per record */
  do {
    capabilityFlagsSlot = &factionRecordCursor->records[0].capabilityFlags;
    *capabilityFlagsSlot = *capabilityFlagsSlot | factionBit;
    dwordCursor = factionRecordCursor->records[0].technologyMasks256Bits;
    *dwordCursor = *dwordCursor | 1;
    capabilityFlagsSlot = &factionRecordCursor->records[0].capabilityFlags;
    *capabilityFlagsSlot = *capabilityFlagsSlot | 1;
    factionRecordCursor->records[0].packedRelationStates = relationStatePattern;
    factionRecordCursor->records[0].relationCapabilityState = 0;
    factionRecordCursor->records[0].primaryAnchorYQ12 = -0xc000;
    factionRecordCursor->records[0].primaryAnchorXQ12 = 0;
    factionRecordCursor->records[0].secondaryAnchorYQ12 = -0xc000;
    factionRecordCursor->records[0].secondaryAnchorXQ12 = 0;
    factionRecordCursor->records[0].relationTransitionTick = 0x11;
    factionRecordCursor->records[0].energyGenerationCapacityQ4 = 0x280; /* 40.0 */
    factionRecordCursor->records[0].baselineEnergySupplyQ4 = 0x280; /* 40.0 */
    factionRecordCursor->records[0].xeniteStorageLimitQ4 = 4000; /* 250.0 */
    factionRecordCursor->records[0].tritiumStorageLimitQ4 = 4000; /* 250.0 */
    factionRecordCursor->records[0].terrainContributionScaleQ8 = 0x100; /* 1.0 */
    factionBit = factionBit * 2;
    relationStatePattern = relationStatePattern << 4 | relationStatePattern >> 0x1c;
    factionRecordCursor = (GameFactionRuntimeImage *)(factionRecordCursor->records + 1);
    remainingCount--;
  } while (remainingCount != 0);
  allocResult = g_MemoryApi.alloc(GAME_STAT_TABLE_BYTES);
  previousStatTable = g_GameStatTableImage;
  if (!allocResult.failed) {
    LOCK();
    UNLOCK();
    g_GameStatTableImage = (void *)allocResult.payloadOrError;
    g_MemoryApi.free(previousStatTable);
    statTableCursor = (uint32_t *)allocResult.payloadOrError;
    for (remainingCount = GAME_STAT_TABLE_BYTES / 4; remainingCount != 0; remainingCount--) {
      *statTableCursor = 0;
      statTableCursor++;
    }
    statTableCursor[-1] = 0xffffffff; /* end marker */
    g_GameFactionRuntimeImage.tail.periodicClockTick = 0;
    allocResult.payloadOrError = 0;
    allocResult.failed = false;
  }
  status.valueOrError = allocResult.payloadOrError;
  status.failed = allocResult.failed;
  return status;
}


/* Address: 0x00512F60.
   Loads the game data of a level or savegame from the mounted packages: daten.hex is the faction image,
   stat.hex replaces the stat table and oldunit.hex (record count, primary table, secondary table) fills the
   old-unit tables; without oldunit.hex both tables and the count are cleared. CF is set when daten.hex or
   stat.hex cannot be loaded.
*/
bool __thandor_cf_preserve_eax_ecx_edx GameData_LoadExternalTables(void)

{
  void *previousStatTable;
  uint32_t *oldUnitBufferOrCursor;
  int remainingCount;
  uint32_t *sourceCursor;
  uint32_t *destinationCursor;
  StatusResult loadStatus;
  PackageLoadResult packageEntry;

  remainingCount = 0x40;
  oldUnitBufferOrCursor = g_GameDataAuxState.pairPressureMatrix8x8;
  for (; remainingCount != 0; remainingCount--) {
    *oldUnitBufferOrCursor = 0;
    oldUnitBufferOrCursor++;
  }
  loadStatus = Package_LoadEntryIntoBuffer
                    (GAME_FACTION_IMAGE_BYTES,(uint8_t *)&g_GameFactionRuntimeImage,
                     (uint16_t *)u_daten_hex_0050e054);
  if (!loadStatus.failed) {
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
   Resolves procedureName in module with GetProcAddress, stores it in *destination and returns it with CF
   clear. The name is left in g_PackageLastErrorPath and, on failure, the name of the module (when it is one
   of g_DynamicModules) in g_FatalErrorDetail1Utf16 for the fatal-error message; CF set with
   FATAL_ERROR_DLL_PROCEDURE_MISSING.
*/
DynApiResolveResult __thandor_eax_cf_preserve_ecx_edx
DynAPI_Resolve(void **destination,HINSTANCE module,char *procedureName)

{
  FARPROC resolvedProcedure;
  uint32_t modulesRemaining;
  DynamicModuleEntry *moduleEntryCursor;
  DynApiResolveResult successResult;
  DynApiResolveResult failureResult;

  Text_CopyNarrowToUtf16(0x100,g_PackageLastErrorPath,(uint8_t *)procedureName);
  resolvedProcedure = GetProcAddress(module,procedureName);
  if (resolvedProcedure != NULL) {
    *destination = resolvedProcedure;
    successResult.failed = false;
    successResult.procedureOrError = resolvedProcedure;
    return successResult;
  }
  moduleEntryCursor = g_DynamicModules;
  g_FatalErrorDetail1Utf16[0] = 0;
  modulesRemaining = g_DynamicModuleCount;
  for (; modulesRemaining != 0; modulesRemaining = modulesRemaining - 1) {
    if (module == moduleEntryCursor->module) {
      /* name the module in the error detail */
      Text_CopyNarrowToUtf16(0x100,g_FatalErrorDetail1Utf16,(uint8_t *)moduleEntryCursor->name);
      break;
    }
    moduleEntryCursor = moduleEntryCursor + 1;
  }
  failureResult.failed = true;
  failureResult.procedureOrError = (void *)FATAL_ERROR_DLL_PROCEDURE_MISSING;
  return failureResult;
}


/* Address: 0x00573C50.
   Loads the DLL moduleName with the bound LoadLibraryA and records it in g_DynamicModules so that
   DynDLL_UnloadAll frees it; returns the module with CF clear. Fails with CF set and FATAL_ERROR_DLL_LOAD_FAILED
   (the name left in g_PackageLastErrorPath) when LoadLibraryA is not bound yet, the table is full or the load
   fails.
*/
DllLoadResult __thandor_eax_cf_preserve_ecx_edx DynDLL_Load(char *moduleName)

{
  HINSTANCE loadedModule;
  DllLoadResult successResult;
  DllLoadResult failureResult;
  uint32_t moduleSlotIndex;

  Text_CopyNarrowToUtf16(0x100,g_PackageLastErrorPath,(uint8_t *)moduleName);
  /* dynapi_9 is the string "LoadLibraryA": the slot still holds the name until DynAPI_Bootstrap binds it */
  if ((g_BootstrapApiBindings[0].destination != (void **)dynapi_9) &&
      (g_DynamicModuleCount < DYNAMIC_MODULE_CAPACITY))
  {
    loadedModule = (HINSTANCE)((BootstrapLoadLibraryAProc)g_BootstrapApiBindings[0].destination)(moduleName);
    moduleSlotIndex = g_DynamicModuleCount;
    if (loadedModule != NULL) {
      g_DynamicModules[g_DynamicModuleCount].module = loadedModule;
      g_DynamicModules[moduleSlotIndex].name = moduleName;
      g_DynamicModuleCount++;
      successResult.failed = false;
      successResult.moduleOrError = loadedModule;
      return successResult;
    }
  }
  failureResult.failed = true;
  failureResult.moduleOrError = (HINSTANCE)FATAL_ERROR_DLL_LOAD_FAILED;
  return failureResult;
}


/* Address: 0x00573CD0.
   Frees a DLL loaded by DynDLL_Load; the module is found by its name pointer (not by comparing text), as the
   Glide backend passes the same name string it loaded with. Returns FreeLibrary's non-zero result, or
   FATAL_ERROR_LOADER_MODULE_MISSING with the name in g_PackageLastErrorPath. The table entry stays in place.
   The original also reports success/failure in CF (clear/set); the callers ignore both.
*/
uint32_t DynDLL_Unload(char *moduleName)

{
  uint32_t modulesRemainingOrResult; /* one register in the original: the loop count, then FreeLibrary's result */
  DynamicModuleEntry *moduleEntryCursor;

  moduleEntryCursor = g_DynamicModules;
  modulesRemainingOrResult = g_DynamicModuleCount;
  for (; modulesRemainingOrResult != 0; modulesRemainingOrResult--) {
    if (moduleName == moduleEntryCursor->name) {
      /* g_BootstrapApiBindings[1] is FreeLibrary */
      modulesRemainingOrResult = ((BootstrapFreeLibraryProc)g_BootstrapApiBindings[1].destination)(moduleEntryCursor->module);
      if (modulesRemainingOrResult != 0) {
        return modulesRemainingOrResult;
      }
      break;
    }
    moduleEntryCursor++;
  }
  /* module not loaded, or FreeLibrary failed */
  Text_CopyNarrowToUtf16(0x100,g_PackageLastErrorPath,(uint8_t *)moduleName);
  return FATAL_ERROR_LOADER_MODULE_MISSING;
}

/* Address: 0x00573D40.
   Ownership: platform/bootstrap/runtime.
   Purpose: Handles bootstrap api resolve binding by destination.
   Cross-module calls: Text_CopyNarrowToUtf16 [core/text/string].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
BootstrapApi_ResolveBindingByDestination(void **destination)

{
  void *resolvedProcedure;
  uint32_t remainingCount;
  DynamicApiBinding *bindingCursor;
  StatusResult failureResult;
  StatusResult successResult;
  
  bindingCursor = g_BootstrapApiBindings;
  remainingCount = g_DynamicModuleCount;
  for (; remainingCount != 0; remainingCount = remainingCount - 1) {
    if (destination == bindingCursor->destination) {
      Text_CopyNarrowToUtf16(0x100,g_PackageLastErrorPath,(uint8_t *)destination);
      /* The original pushes ESI (the binding cursor) only to preserve it across the call: binding slot 0
         (LoadLibraryA) gets the destination argument as its single argument, and the result is stored
         into the matching binding (MOV [ESI],EDX after POP ESI). The function is unreferenced. */
      resolvedProcedure =
           (void *)((BootstrapLoadLibraryAProc)g_BootstrapApiBindings[0].destination)((char *)destination);
      if (resolvedProcedure != (void *)0x0) {
        bindingCursor->destination = (void **)resolvedProcedure;
        successResult.valueOrError = 0xf;
        successResult.failed = false;
        return successResult;
      }
      break;
    }
    bindingCursor = bindingCursor + 1;
  }
  failureResult.failed = true;
  failureResult.valueOrError = 0xf;
  return failureResult;
}


/* Address: 0x00573EB0.
   Frees every DLL recorded in g_DynamicModules with the bound FreeLibrary at shutdown; each slot is cleared
   before the call so a module is never freed twice. The count is left unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx DynDLL_UnloadAll(void)

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
      ((BootstrapFreeLibraryProc)g_BootstrapApiBindings[1].destination)(loadedModule);
    }
    moduleEntryCursor++;
  }
  return;
}


/* Address: 0x00585F50.
   Ownership: platform/bootstrap/runtime.
   Purpose: Win32 WNDPROC. Typed parameters: p1 message→Win32WindowMessageId_V343. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: Keyboard_OnKeyDown [platform/input/devices], Keyboard_OnKeyUp [platform/input/devices],
   Keyboard_OnChar [platform/input/devices].
*/
LRESULT __stdcall MainWindowProc(HWND hwnd,Win32WindowMessageId message,WPARAM wParam,LPARAM lParam)

{
  uint16_t keyState;
  HANDLE currentProcess;
  LRESULT defaultResult;
  
  if ((message == 2) || (message == 0x10)) {
    g_WindowDestroyDepth = g_WindowDestroyDepth + 1;
  }
  else if (message == 0x1c) {
    g_AppActive = wParam;
    if (wParam == 0) {
      currentProcess = GetCurrentProcess();
      SetPriorityClass(currentProcess,0x20);
      if (g_MouseDevice != (IDirectInputDeviceA *)0x0) {
        g_MouseDevice->lpVtbl->Unacquire(g_MouseDevice);
      }
      if (g_WindowDestroyDepth == 0) {
        g_GraphicsBackendRefreshActiveAdapter();
      }
    }
    else {
      currentProcess = GetCurrentProcess();
      SetPriorityClass(currentProcess,0x100);
      if (g_MouseDevice != (IDirectInputDeviceA *)0x0) {
        g_MouseDevice->lpVtbl->Acquire(g_MouseDevice);
      }
      if (-1 < (int)g_ActiveGraphicsAdapterIndex) {
        g_GraphicsSetDisplayMode
                  (g_ActiveGraphicsAdapterIndex,
                   g_SoftwarePixelFormatConfig.redBitCount +
                   g_SoftwarePixelFormatConfig.greenBitCount +
                   g_SoftwarePixelFormatConfig.blueBitCount + 0xf & 0xfffffff0,g_FramebufferHeight,
                   g_FramebufferWidth);
      }
      if (g_MouseDevice != (IDirectInputDeviceA *)0x0) {
        g_KeyboardStateMask = 0;
        keyState = GetKeyState(0x90);
        if ((keyState & 1) != 0) {
          g_KeyboardStateMask = g_KeyboardStateMask | 0x10000;
        }
        keyState = GetKeyState(0x91);
        if ((keyState & 1) != 0) {
          g_KeyboardStateMask = g_KeyboardStateMask | 0x20000;
        }
        keyState = GetKeyState(0x14);
        if ((keyState & 1) != 0) {
          g_KeyboardStateMask = g_KeyboardStateMask | 0x40000;
        }
        g_KeyboardFlushEvents();
      }
    }
  }
  else if (message == 0x20) {
    SetCursor((HCURSOR)0x0);
  }
  else if ((message == 0x100) || (message == 0x104)) {
    Keyboard_OnKeyDown(wParam);
  }
  else if ((message == 0x101) || (message == 0x105)) {
    Keyboard_OnKeyUp(wParam);
  }
  else {
    if ((message != 0x102) && (message != 0x106)) {
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
  StatusResult renderingInitResult;
  uint32_t loadResultOrWidth; /* Game_LoadCoreAssets result, later the saved display width */
  uint32_t displayHeight;
  uint32_t bitsPerPixel;
  uint32_t adapterIndex;
  bool introMoviesFailed;
  CursorFrameResult cursorFrameResult;
  FatalErrorCheckResult fatalResult;
  DisplayModeResult displayModeResult;
  FrontendMainLoopResult mainLoopResult;

  cursorFrameResult = g_GraphicsCursorSetFrame(0);
  FatalError_ExitIfFailed(cursorFrameResult.errorCode,cursorFrameResult.failed);
  renderingInitResult = GameRuntime_InitializeSpatialAudioAndRendering();
  FatalError_ExitIfFailed(renderingInitResult.valueOrError,renderingInitResult.failed);
  loadResultOrWidth = Game_LoadCoreAssets();
  Thandor_Log("Game_LoadCoreAssets -> 0x%08X", loadResultOrWidth);
  /* 0 with CF clear on success, an error code with CF set otherwise */
  fatalResult = FatalError_ExitIfFailed(loadResultOrWidth,loadResultOrWidth != 0);
  /* keeps EAX: a movie that cannot start is reported with the previous value */
  introMoviesFailed = Game_PlayIntroMovies();
  FatalError_ExitIfFailed(fatalResult.valueOrError,introMoviesFailed);
  PersistentSettings_Load();
  /* ProcessEntry started in 640x480x16; switch only when the saved mode differs */
  loadResultOrWidth = PersistentSettings_Read(640,PERSISTENT_SETTING_DISPLAY_WIDTH);
  displayHeight = PersistentSettings_Read(480,PERSISTENT_SETTING_DISPLAY_HEIGHT);
  bitsPerPixel = PersistentSettings_Read(16,PERSISTENT_SETTING_BITS_PER_PIXEL);
  if (((loadResultOrWidth != 640) || (displayHeight != 480)) || (bitsPerPixel != 16)) {
    adapterIndex = PersistentSettings_Read(0,PERSISTENT_SETTING_ADAPTER_INDEX);
    if (g_GraphicsAdapterCount <= adapterIndex) {
      adapterIndex = 0;
    }
    displayModeResult = g_GraphicsSetDisplayMode(adapterIndex,bitsPerPixel,displayHeight,loadResultOrWidth);
    FatalError_ExitIfFailed(displayModeResult.valueOrError,displayModeResult.failed);
    PersistentSettings_Write(g_ActiveGraphicsAdapterIndex,PERSISTENT_SETTING_ADAPTER_INDEX);
  }
  mainLoopResult = Frontend_MainLoop(1);
  FatalError_ExitIfFailed(mainLoopResult.errorOrValue,mainLoopResult.failed);
  g_NetworkBackendSlot3(); /* close */
  g_NetworkBackendSlot1(); /* cleanup */
  return;
}


/* Address: 0x0050BB10.
   Game_Run's first startup step: initialises the spatial-sound pool, the terrain and intensity clamp tables,
   the software renderer's display-mode hook and the global primitive queue (0xA000 packets), in that order.
   Stops at the first step that fails and returns its result.
*/
StatusResult __cdecl GameRuntime_InitializeSpatialAudioAndRendering(void)

{
  StatusResult step;
  
  step = SpatialSoundPool_Init();
  if (step.failed) {
    return step;
  }
  step = TerrainByteClampLookup_Initialize();
  if (step.failed) {
    return step;
  }
  step = GraphicsIntensityClampTable_Initialize();
  if (step.failed) {
    return step;
  }
  step = SoftwareRenderer_InstallDisplayModeHook();
  if (step.failed) {
    return step;
  }
  return GraphicsPrimitiveQueue_AllocateGlobalPool(0xa000);
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
  FncModuleHeader *module; /* a voice set or a PCX export; the error code on the failure paths */
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
  StatusResult status;
  SampleVoiceSetResult voiceSetResult;
  FileSystemOpenResult openResult;
  TextResolveResult textResolveResult;
  TextPageLoadResult textPageLoadResult;
  PackageLoadResult pcxModuleEntry;
  FncModuleLoadResult moduleLoadResult;
  TextureSourceLoadResult panelTextureResult;
  ArenaAllocResult allocResult;
  ResourceLoadResult resourceLoadResult;
  
  /* HKLM\Software\Planet4\Thandor "CD": movies are looked up under <CD>\Thandor first */
  if ((g_MemoryApi.alloc == ArenaHeap_Alloc) &&
     (statusOrCount = ((BootstrapRegOpenKeyExAProc)g_BootstrapApiBindings[5].destination)
                        (HKEY_LOCAL_MACHINE,s_Software_Planet4_Thandor_00572e20,0,KEY_READ,
                         &g_InstallRegistryKeyHandle), statusOrCount == ERROR_SUCCESS)) {
    statusOrCount = ((BootstrapRegQueryValueExAProc)g_BootstrapApiBindings[6].destination)
                      (g_InstallRegistryKeyHandle,&s_InstallRegistryValueNameCD,0,
                       &g_InstallRegistryValueType,&g_InstallRegistryValueDataA,
                       &g_InstallRegistryValueDataCapacityBytes);
    if ((statusOrCount == ERROR_SUCCESS) && (g_InstallRegistryValueType == REG_SZ)) {
      Text_CopyNarrowToUtf16
                (0x200,(uint16_t *)&g_InstallDirectoryScratchUtf16,&g_InstallRegistryValueDataA);
      WidePath_CombineDirectoryAndLeaf
                (g_LooseMoviePathPrefix.codeUnits,(uint16_t *)u_Thandor_00572e10,
                 (uint16_t *)&g_InstallDirectoryScratchUtf16);
    }
    ((BootstrapRegCloseKeyProc)g_BootstrapApiBindings[7].destination)(g_InstallRegistryKeyHandle);
  }
  {
    /* open-thandor: the full-length movies from the CD (Ende*.flm, Intro2.flm) live in the
       flm folder of the game directory, so the CD is no longer needed. Movie_Open looks under
       g_LooseMoviePathPrefix before the packages, which only hold still-image stand-ins for
       these movies; point the prefix at the game directory when that folder exists. */
    static const uint16_t flmLeaf[4] = {'f','l','m',0};
    static uint16_t localFlmPath[0x100];
    char narrow[0x100];
    int k;
    WidePath_CombineDirectoryAndLeaf
              (localFlmPath,(uint16_t *)flmLeaf,(uint16_t *)&g_ExecutableDirectoryUtf16);
    if (Thandor_DirectoryExistsW(localFlmPath)) {
      uint16_t *directory = (uint16_t *)&g_ExecutableDirectoryUtf16;
      for (k = 0; (k < 0xff) && (directory[k] != 0); k++) {
        g_LooseMoviePathPrefix.codeUnits[k] = directory[k];
      }
      g_LooseMoviePathPrefix.codeUnits[k] = 0;
    }
    for (k = 0; (k < 0xff) && (g_LooseMoviePathPrefix.codeUnits[k] != 0); k++) {
      narrow[k] = (char)g_LooseMoviePathPrefix.codeUnits[k];
    }
    narrow[k] = 0;
    Thandor_Log("movie CD path: \"%s\"", narrow);
  }
  /* patchNN.pck and then levelNN.pck, NN counting down to "00". decimalDigits.codeUnits[0] is the tens
     digit, [1] the ones digit; adding 0x9FFFF to the packed pair decrements the tens digit and, through the
     carry, turns the ones digit from '0' - 1 back into '9'. The patch count starts at "00", so only
     patch00.pck is tried. */
  g_PatchArchivePathTemplateUtf16.decimalDigits.packedDigits = 0x300030; /* "00" */
  do {
    do {
      Package_Mount(g_PatchArchivePathTemplateUtf16.prefixCodeUnits);
      g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[1] =
           g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[1] - 1;
    } while (0x2f < g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[1]);
    g_PatchArchivePathTemplateUtf16.decimalDigits.packedDigits =
         g_PatchArchivePathTemplateUtf16.decimalDigits.packedDigits + 0x9ffff;
  } while (0x2f < g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[0]);
  g_LevelArchivePathTemplateUtf16.decimalDigits.packedDigits = 0x390039; /* "99" */
  do {
    do {
      LevelPackage_ValidateAndMount(g_LevelArchivePathTemplateUtf16.prefixCodeUnits);
      g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[1] =
           g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[1] - 1;
    } while (0x2f < g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[1]);
    g_LevelArchivePathTemplateUtf16.decimalDigits.packedDigits =
         g_LevelArchivePathTemplateUtf16.decimalDigits.packedDigits + 0x9ffff;
  } while (0x2f < g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[0]);
  status = Package_Mount((uint16_t *)u_daten_pck_00572e56);
  if (!status.failed) {
    g_DataPackageHandle = status.valueOrError;
  }
  status = Package_Mount((uint16_t *)u_modelle_pck_00572e6a);
  if (!status.failed) {
    g_ModelPackageHandle = status.valueOrError;
  }
  status = Package_Mount((uint16_t *)u_graphik_pck_00572e82);
  if (!status.failed) {
    g_GraphicsPackageHandle = status.valueOrError;
  }
  status = Package_Mount((uint16_t *)u_sound_pck_00572e9a);
  if (!status.failed) {
    g_SoundPackageHandle = status.valueOrError;
  }
  status = Package_Mount((uint16_t *)u_filme_pck_00572eae);
  if (!status.failed) {
    g_MoviePackageHandle = status.valueOrError;
  }
  status = Package_Mount((uint16_t *)u_level_pck_00572ec2);
  if (!status.failed) {
    g_LevelPackageHandle = status.valueOrError;
  }
  resourceLoadResult = Resource_Load((uint16_t *)u_sound_button0_sam_00572f06);
  loadedResource = (SoundSampleAsset *)resourceLoadResult.bufferOrError;
  if (resourceLoadResult.failed) {
    return (uint32_t)loadedResource;
  }
  voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
  module = (FncModuleHeader *)voiceSetResult.voiceSet;
  if (!voiceSetResult.failed) {
    Resource_Release(loadedResource);
    g_UiButtonSoundVoiceSets7[0] = (DirectSoundVoiceSet *)module;
    resourceLoadResult = Resource_Load((uint16_t *)u_sound_button1_sam_00572f2a);
    loadedResource = (SoundSampleAsset *)resourceLoadResult.bufferOrError;
    if (resourceLoadResult.failed) {
      return (uint32_t)loadedResource;
    }
    voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
    module = (FncModuleHeader *)voiceSetResult.voiceSet;
    if (!voiceSetResult.failed) {
      Resource_Release(loadedResource);
      g_UiButtonSoundVoiceSets7[1] = (DirectSoundVoiceSet *)module;
      resourceLoadResult = Resource_Load((uint16_t *)u_sound_button2_sam_00572f4e);
      loadedResource = (SoundSampleAsset *)resourceLoadResult.bufferOrError;
      if (resourceLoadResult.failed) {
        return (uint32_t)loadedResource;
      }
      voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
      module = (FncModuleHeader *)voiceSetResult.voiceSet;
      if (!voiceSetResult.failed) {
        Resource_Release(loadedResource);
        g_UiButtonSoundVoiceSets7[2] = (DirectSoundVoiceSet *)module;
        resourceLoadResult = Resource_Load((uint16_t *)u_sound_button3_sam_00572f72);
        loadedResource = (SoundSampleAsset *)resourceLoadResult.bufferOrError;
        if (resourceLoadResult.failed) {
          return (uint32_t)loadedResource;
        }
        voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
        module = (FncModuleHeader *)voiceSetResult.voiceSet;
        if (!voiceSetResult.failed) {
          Resource_Release(loadedResource);
          g_UiButtonSoundVoiceSets7[3] = (DirectSoundVoiceSet *)module;
          resourceLoadResult = Resource_Load((uint16_t *)u_sound_button4_sam_00572f96);
          loadedResource = (SoundSampleAsset *)resourceLoadResult.bufferOrError;
          if (resourceLoadResult.failed) {
            return (uint32_t)loadedResource;
          }
          voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
          module = (FncModuleHeader *)voiceSetResult.voiceSet;
          if (!voiceSetResult.failed) {
            Resource_Release(loadedResource);
            g_UiButtonSoundVoiceSets7[4] = (DirectSoundVoiceSet *)module;
            resourceLoadResult = Resource_Load((uint16_t *)u_sound_button5_sam_00572fba);
            loadedResource = (SoundSampleAsset *)resourceLoadResult.bufferOrError;
            if (resourceLoadResult.failed) {
              return (uint32_t)loadedResource;
            }
            voiceSetResult = g_SoundCreateSampleVoiceSet(loadedResource);
            module = (FncModuleHeader *)voiceSetResult.voiceSet;
            if (!voiceSetResult.failed) {
              Resource_Release(loadedResource);
              g_UiButtonSoundVoiceSets7[5] = (DirectSoundVoiceSet *)module;
              resourceLoadResult = Resource_Load((uint16_t *)u_sound_button6_sam_00572fde);
              loadedResource = (SoundSampleAsset *)resourceLoadResult.bufferOrError;
              if (resourceLoadResult.failed) {
                return (uint32_t)loadedResource;
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
                    goto Game_LoadCoreAssets_BindDebugOverlayTextAndContinueRemainingAssetLoad;
                    u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + 1;
                    g_FileSystemClose((void *)openResult.handleOrError);
                    screenshotTensDigit = u_Dscreen00_pcx_00572e3a[7];
                  } while ((uint16_t)u_Dscreen00_pcx_00572e3a[8] < 0x3a);
                  u_Dscreen00_pcx_00572e3a[7] = u_Dscreen00_pcx_00572e3a[7] + 1;
                  u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + L'\xfff6'; /* -10 */
                } while ((uint16_t)u_Dscreen00_pcx_00572e3a[7] < 0x3a);
                /* all 100 names exist: the tens digit goes back to '0' (the last seen '9' - 9) */
                u_Dscreen00_pcx_00572e3a[7] = screenshotTensDigit + L'\xfff7';
Game_LoadCoreAssets_BindDebugOverlayTextAndContinueRemainingAssetLoad:
                /* bind placeholders 0..13 of texts 0x112..0x117 to the debug-overlay text slots */
                resourceId = 0x112;
                do {
                  textResolveResult = TextResource_Resolve(resourceId);
                  textBuffer = textResolveResult.text;
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
                            (0xb,g_FrontendDebugOverlayTextSlot11Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (0xc,g_FrontendDebugOverlayTextSlot12Utf16,textBuffer);
                  RichTextCommandStream_PatchPayloadBySelector
                            (0xd,g_FrontendDebugOverlayTextSlot13Utf16,textBuffer);
                } while (resourceId < 0x118);
                UiActionHandlers_SetPage
                          (0x10,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage10);
                UiActionHandlers_SetPage
                          (0x11,(UiActionHandlerPage *)&g_InGameUiCommandModeActionHandlers30);
                UiActionHandlers_SetPage
                          (0x12,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage12);
                UiActionHandlers_SetPage
                          (0x20,(UiActionHandlerPage *)&g_FrontendUiActionHandlersPage20);
                textPageLoadResult = TextResourcePage_Load(0xff,(uint16_t *)u_texte_neterror_str_0050f104);
                if (textPageLoadResult.failed) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x18,(uint16_t *)u_texte_help_str_00563170);
                if (textPageLoadResult.failed) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x20,(uint16_t *)u_texte_hilfe_str_00545b34);
                if (textPageLoadResult.failed) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x21,(uint16_t *)u_texte_menue_str_00545ba0);
                if (textPageLoadResult.failed) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x30,(uint16_t *)u_texte_techno_str_0050dec4);
                if (textPageLoadResult.failed) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x22,(uint16_t *)u_texte_level_str_00545bc0);
                if (textPageLoadResult.failed) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x23,(uint16_t *)u_texte_inhalt_str_00545be0);
                if (textPageLoadResult.failed) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x24,(uint16_t *)u_texte_tastatur_str_005631b8);
                if (textPageLoadResult.failed) {
                  return textPageLoadResult.errorOrValue;
                }
                textResolveResult = TextResource_Resolve(0x2402);
                RichTextCommandStream_BindTextureSource(g_CursorSourceAsset,textResolveResult.text);
                /* sound effects off: every gain is 0 */
                soundOptionsOrBufferBase =
                     PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
                uiSoundGain = 0;
                if ((soundOptionsOrBufferBase & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
                  uiSoundGain = PersistentSettings_Read(0x8000,PERSISTENT_SETTING_EFFECTS_GAIN);
                }
                movieGain = 0;
                g_UiSoundGainQ15 = uiSoundGain;
                g_SoundEffectsGainQ15 = uiSoundGain;
                if ((soundOptionsOrBufferBase & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
                  movieGain = PersistentSettings_Read(0x8000,PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN);
                }
                alternateMovieGain = 0;
                g_MovieDefaultAudioGainQ15 = movieGain;
                if ((soundOptionsOrBufferBase & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
                  alternateMovieGain = PersistentSettings_Read(0x8000,PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN);
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
                status = AiRuntime_InitWorkspace();
                if (status.failed) {
                  return status.valueOrError;
                }
                pcxModuleEntry = Package_LoadEntry((uint16_t *)u_engine_pcx_fnc_00573028);
                if (pcxModuleEntry.failed) {
                  return (uint32_t)pcxModuleEntry.bufferOrError;
                }
                moduleLoadResult = FncModule_LoadAndRelocate(pcxModuleEntry.bufferOrError);
                module = (FncModuleHeader *)moduleLoadResult.moduleBase;
                loadedResource = (SoundSampleAsset *)pcxModuleEntry.bufferOrError;
                if (!moduleLoadResult.failed) {
                  g_PcxFunctionModule = module;
                  status = FncModule_GetExportByIndex(3,module);
                  module = (FncModuleHeader *)status.valueOrError;
                  if (!status.failed) {
                    g_PcxFunctionExport3 = (PcxEncodeProc *)module;
                    status = FncModule_GetExportByIndex(2,g_PcxFunctionModule);
                    module = (FncModuleHeader *)status.valueOrError;
                    if (!status.failed) {
                      g_PcxFunctionExport2 = (PcxDecodeProc *)module;
                      /* EDX still holds the engine\pcx.fnc package buffer. */
                      Resource_Release((SoundSampleAsset *)pcxModuleEntry.bufferOrError);
                      panelTextureResult = g_GraphicsTextureSourceLoadPackageAsset
                                         ((uint16_t *)u_gfx_panel_stat_gfx_00573002);
                      if (panelTextureResult.failed) {
                        return (uint32_t)panelTextureResult.textureSource;
                      }
                      g_InGameStatusPanelTextureSource = panelTextureResult.textureSource;
                      allocResult = g_MemoryApi.alloc(0x800);
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
                      allocResult = g_MemoryApi.alloc(0x400);
                      soundOptionsOrBufferBase = allocResult.payloadOrError;
                      if (allocResult.failed) {
                        return soundOptionsOrBufferBase;
                      }
                      g_FrontendPlayerListRow1 = soundOptionsOrBufferBase + 0x80;
                      g_FrontendPlayerListRow2 = soundOptionsOrBufferBase + 0x100;
                      g_FrontendPlayerListRow3 = soundOptionsOrBufferBase + 0x180;
                      g_FrontendPlayerListRow4 = soundOptionsOrBufferBase + 0x200;
                      g_FrontendPlayerListRow5 = soundOptionsOrBufferBase + 0x280;
                      g_FrontendPlayerListRow6 = soundOptionsOrBufferBase + 0x300;
                      g_FrontendPlayerListRow7 = soundOptionsOrBufferBase + 0x380;
                      g_FrontendPlayerListRows = soundOptionsOrBufferBase;
                      allocResult = g_MemoryApi.alloc(0x800);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_RomRegistrySlots = (RomRegistrySlot *)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(0x80);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_FrontendSessionListRows = (FrontendSessionDiscoveryRecordB0 **)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(0x1600);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_FrontendSessionDiscoveryRecords =
                           (FrontendSessionDiscoveryRecordB0 *)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(0x2000);
                      textBuffer = (uint16_t *)allocResult.payloadOrError;
                      if (allocResult.failed) {
                        return (uint32_t)textBuffer;
                      }
                      g_InGameFactionStatusTextScratchUtf16 = textBuffer;
                      g_InGameFactionStatusTextScratchUtf16Mirror = textBuffer;
                      allocResult = g_MemoryApi.alloc(0x160);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_InGamePlayerListTextScratchUtf16 = (uint16_t *)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(0x6000);
                      splineBuffer = (float *)allocResult.payloadOrError;
                      if (allocResult.failed) {
                        return (uint32_t)splineBuffer;
                      }
                      g_WorldMotionSplineMatrixWorkspaces[1] = splineBuffer + 0x400;
                      g_WorldMotionSplineMatrixWorkspaces[2] = splineBuffer + 0x800;
                      g_WorldMotionSplineMatrixWorkspaces[3] = splineBuffer + 0xc00;
                      g_WorldMotionSplineMatrixWorkspaces[4] = splineBuffer + 0x1000;
                      g_WorldMotionSplineMatrixWorkspaces[5] = splineBuffer + 0x1400;
                      g_WorldMotionSplineMatrixWorkspaces[0] = splineBuffer;
                      allocResult = g_MemoryApi.alloc(0x300);
                      splineBuffer = (float *)allocResult.payloadOrError;
                      if (allocResult.failed) {
                        return (uint32_t)splineBuffer;
                      }
                      g_WorldMotionSplineCoefficientTables[1] = splineBuffer + 0x20;
                      g_WorldMotionSplineCoefficientTables[2] = splineBuffer + 0x40;
                      g_WorldMotionSplineCoefficientTables[3] = splineBuffer + 0x60;
                      g_WorldMotionSplineCoefficientTables[4] = splineBuffer + 0x80;
                      g_WorldMotionSplineCoefficientTables[5] = splineBuffer + 0xa0;
                      g_WorldMotionSplineCoefficientTables[0] = splineBuffer;
                      allocResult = g_MemoryApi.alloc(0x408c0);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_SelectionPlayerBlocks = (SelectionPlayerRuntimeBlock *)allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(0x1300);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_FrontendLocalPlayerPcxPreview = allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(0x4000);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_TerrainRegionCollectionEntries = allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(800);
                      if (allocResult.failed) {
                        return allocResult.payloadOrError;
                      }
                      g_FrontendPlayerMessageBuffers = allocResult.payloadOrError;
                      allocResult = g_MemoryApi.alloc(0x9d80);
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
                      statusOrCount = 0x20;
                      do {
                        *playerRuntimePointerTableWriteCursor = playerRecordCursor;
                        playerRuntimePointerTableWriteCursor++;
                        playerRecordCursor++;
                        statusOrCount--;
                      } while (statusOrCount != 0);
                      allocResult = g_MemoryApi.alloc(0xe00);
                      scratchCursor = (uint8_t *)allocResult.payloadOrError;
                      if (allocResult.failed) {
                        return (uint32_t)scratchCursor;
                      }
                      g_CoreAssetScratchSlice1 = scratchCursor + 0x200;
                      g_CoreAssetScratchSlice2 = scratchCursor + 0x400;
                      g_CoreAssetScratchSlice3 = scratchCursor + 0x600;
                      g_CoreAssetScratchSlice4 = scratchCursor + 0x800;
                      g_CoreAssetScratchSlice5 = scratchCursor + 0xa00;
                      g_CoreAssetScratchSlice6 = scratchCursor + 0xc00;
                      g_CoreAssetScratchSlice0 = scratchCursor;
                      for (statusOrCount = 0x380; statusOrCount != 0; statusOrCount--) {
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


/* Debug tool: movie test player.
   OPEN_THANDOR_MOVIE=<name>  plays flm\<name>.flm,
   OPEN_THANDOR_MOVIE=all     plays every name listed in movies.txt (one per line, working dir).
   Each movie runs at its own rate for at most 10 seconds; a key or mouse click skips to the next.
   The name and frame counter are drawn top left. OPEN_THANDOR_MOVIE_STRETCH=1 draws full screen
   with the end-movie bilinear stretch. The process exits after the last movie. */

static const uint8_t g_DebugFont5x7[][8] = {
  /* char, 7 rows of 5 bits */
  {' ',0x00,0x00,0x00,0x00,0x00,0x00,0x00},{'.',0x00,0x00,0x00,0x00,0x00,0x0c,0x0c},
  {'/',0x01,0x02,0x02,0x04,0x08,0x08,0x10},{':',0x00,0x0c,0x0c,0x00,0x0c,0x0c,0x00},
  {'-',0x00,0x00,0x00,0x1f,0x00,0x00,0x00},{'_',0x00,0x00,0x00,0x00,0x00,0x00,0x1f},
  {'0',0x0e,0x11,0x13,0x15,0x19,0x11,0x0e},{'1',0x04,0x0c,0x04,0x04,0x04,0x04,0x0e},
  {'2',0x0e,0x11,0x01,0x02,0x04,0x08,0x1f},{'3',0x1f,0x02,0x04,0x02,0x01,0x11,0x0e},
  {'4',0x02,0x06,0x0a,0x12,0x1f,0x02,0x02},{'5',0x1f,0x10,0x1e,0x01,0x01,0x11,0x0e},
  {'6',0x06,0x08,0x10,0x1e,0x11,0x11,0x0e},{'7',0x1f,0x01,0x02,0x04,0x08,0x08,0x08},
  {'8',0x0e,0x11,0x11,0x0e,0x11,0x11,0x0e},{'9',0x0e,0x11,0x11,0x0f,0x01,0x02,0x0c},
  {'a',0x0e,0x11,0x11,0x1f,0x11,0x11,0x11},{'b',0x1e,0x11,0x11,0x1e,0x11,0x11,0x1e},
  {'c',0x0e,0x11,0x10,0x10,0x10,0x11,0x0e},{'d',0x1c,0x12,0x11,0x11,0x11,0x12,0x1c},
  {'e',0x1f,0x10,0x10,0x1e,0x10,0x10,0x1f},{'f',0x1f,0x10,0x10,0x1e,0x10,0x10,0x10},
  {'g',0x0e,0x11,0x10,0x17,0x11,0x11,0x0f},{'h',0x11,0x11,0x11,0x1f,0x11,0x11,0x11},
  {'i',0x0e,0x04,0x04,0x04,0x04,0x04,0x0e},{'j',0x07,0x02,0x02,0x02,0x02,0x12,0x0c},
  {'k',0x11,0x12,0x14,0x18,0x14,0x12,0x11},{'l',0x10,0x10,0x10,0x10,0x10,0x10,0x1f},
  {'m',0x11,0x1b,0x15,0x15,0x11,0x11,0x11},{'n',0x11,0x11,0x19,0x15,0x13,0x11,0x11},
  {'o',0x0e,0x11,0x11,0x11,0x11,0x11,0x0e},{'p',0x1e,0x11,0x11,0x1e,0x10,0x10,0x10},
  {'q',0x0e,0x11,0x11,0x11,0x15,0x12,0x0d},{'r',0x1e,0x11,0x11,0x1e,0x14,0x12,0x11},
  {'s',0x0f,0x10,0x10,0x0e,0x01,0x01,0x1e},{'t',0x1f,0x04,0x04,0x04,0x04,0x04,0x04},
  {'u',0x11,0x11,0x11,0x11,0x11,0x11,0x0e},{'v',0x11,0x11,0x11,0x11,0x11,0x0a,0x04},
  {'w',0x11,0x11,0x11,0x15,0x15,0x15,0x0a},{'x',0x11,0x11,0x0a,0x04,0x0a,0x11,0x11},
  {'y',0x11,0x11,0x11,0x0a,0x04,0x04,0x04},{'z',0x1f,0x01,0x02,0x04,0x08,0x10,0x1f},
};

/* Draws text at 1x scale with a black box behind it; framebuffer: [0] pitch in pixels,
   [2] bytes per pixel, [3] pixels. */
static void DebugMovie_DrawText(int x0, int y0, const char *text)
{
  uint32_t *fb = (uint32_t *)g_FramebufferAccess;
  int scale = 1;
  int length = (int)strlen(text);
  int boxWidth = length * 6 * scale + 2 * scale;
  int boxHeight = 9 * scale;
  uint32_t pitch;
  uint32_t bpp;
  uint8_t *pixels;
  int x;
  int y;
  int c;
  if (fb == NULL) {
    return;
  }
  pitch = fb[0];
  bpp = fb[2];
  pixels = (uint8_t *)(uintptr_t)fb[3];
  if ((pixels == NULL) || ((bpp != 4) && (bpp != 2))) {
    return;
  }
  if (x0 + boxWidth > (int)g_FramebufferWidth) boxWidth = (int)g_FramebufferWidth - x0;
#define DEBUG_PUT(px, py, white)                                                          \
  do {                                                                                    \
    if (bpp == 4) ((uint32_t *)pixels)[(py) * pitch + (px)] = (white) ? 0xffffff40 : 0xff000000; \
    else ((uint16_t *)pixels)[(py) * pitch + (px)] = (white) ? 0xffe8 : 0;                    \
  } while (0)
  for (y = 0; y < boxHeight; y++) {
    for (x = 0; x < boxWidth; x++) {
      DEBUG_PUT(x0 + x, y0 + y, 0);
    }
  }
  for (c = 0; c < length; c++) {
    char ch = text[c];
    const uint8_t *glyph = NULL;
    unsigned g;
    if ((ch >= 'A') && (ch <= 'Z')) ch = (char)(ch - 'A' + 'a');
    for (g = 0; g < sizeof g_DebugFont5x7 / sizeof g_DebugFont5x7[0]; g++) {
      if (g_DebugFont5x7[g][0] == (uint8_t)ch) { glyph = g_DebugFont5x7[g] + 1; break; }
    }
    if (glyph == NULL) continue;
    for (y = 0; y < 7 * scale; y++) {
      for (x = 0; x < 5 * scale; x++) {
        int px = x0 + scale + c * 6 * scale + x;
        if (px >= (int)g_FramebufferWidth) break;
        if ((glyph[y / scale] >> (4 - x / scale)) & 1) {
          DEBUG_PUT(px, y0 + scale + y, 1);
        }
      }
    }
  }
#undef DEBUG_PUT
}

/* Fills the framebuffer with opaque black and presents it (called twice to clear both page buffers). */
static void DebugMovie_ClearScreen(void)
{
  if (!g_GraphicsFramebufferBeginAccess()) {
    g_GraphicsFramebufferFillRectArgb
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0,
               0xff000000,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
  }
}

/* Plays one movie; returns after the end, 10 seconds, or a key/click. */
static void DebugMovie_PlayOne(const char *name, int index, int count, int stretch)
{
  uint16_t path[0x40];
  char label[0x80];
  int pathLength = 0;
  int nameIndex;
  unsigned start;
  MovieOpenResult opened;
  MovieFrameResult frame;
  /* UTF-16 path "flm\<name>.flm"; the name is cut so ".flm" and the terminator still fit */
  path[pathLength++] = 'f'; path[pathLength++] = 'l'; path[pathLength++] = 'm'; path[pathLength++] = '\\';
  for (nameIndex = 0; name[nameIndex] != 0 && pathLength < 0x38; nameIndex++) {
    path[pathLength++] = (uint16_t)name[nameIndex];
  }
  path[pathLength++] = '.'; path[pathLength++] = 'f'; path[pathLength++] = 'l'; path[pathLength++] = 'm';
  path[pathLength] = 0;
  DebugMovie_ClearScreen();
  DebugMovie_ClearScreen();
  opened = Movie_Open(1,path);
  if (opened.failed) {
    Thandor_Log("debug movie %d/%d %s: Movie_Open failed (eax=%08x)", index, count, name, opened.frameCountOrError);
    sprintf(label, "Video %d/%d: %s.flm - OEFFNEN FEHLGESCHLAGEN", index, count, name);
    if (!g_GraphicsFramebufferBeginAccess()) {
      DebugMovie_DrawText(8, 8, label);
      g_GraphicsFramebufferEndAccess();
      g_GraphicsFramebufferPresent(g_FramebufferAccess);
    }
    Thandor_SleepMs(1500);
    return;
  }
  frame = Movie_AdvanceFrame();
  if (frame.ended) {
    Thandor_Log("debug movie %d/%d %s: first frame failed", index, count, name);
    Movie_Close();
    return;
  }
  Thandor_Log("debug movie %d/%d %s: playing, %u frames at %u Hz", index, count, name,
              g_ActiveMovie->fileHeader->frameCount, opened.playbackRateHz);
  g_IntroMoviePendingTicks = 0;
  UiFrame_FlushInputAndResetPendingTicks();
  g_TimerRegisterPeriodic(opened.playbackRateHz,IntroMovie_TimerTick);
  start = Thandor_TickCount();
  for (;;) {
    KeyboardEventResult key;
    CursorEventResult cursor;
    g_Win32PumpMessages();
    key = g_KeyboardReadEvent();
    if (!key.queueEmpty) break;
    cursor = g_GraphicsCursorConsumeEvent();
    if ((!cursor.queueEmpty) && (3 < cursor.eventType)) break;
    if (Thandor_TickCount() - start > 10000) break;
    if (g_IntroMoviePendingTicks != 0) {
      int burst = 3;
      MovieFrameResult next;
      int ended = 0;
      do {
        next = Movie_AdvanceFrame();
        if (next.ended) { ended = 1; break; }
        g_IntroMoviePendingTicks--;
      } while ((g_IntroMoviePendingTicks != 0) && (--burst != 0));
      if (ended) break;
      if (g_GraphicsFramebufferBeginAccess()) break;
      if (stretch) {
        g_GraphicsTextureSourceStretchDirectColorBilinear
                  (g_FramebufferHeight,g_FramebufferWidth,0,0,0,
                   (GraphicsTextureSourceAsset *)frame.movieOrError,g_FramebufferAccess);
      }
      else {
        MovieFrameDimensionsEdxEax8 size = Movie_GetFrameDimensions();
        uint32_t height = g_FramebufferHeight;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (g_FramebufferHeight,g_FramebufferWidth,0,0,
                   ((int)((height - (height >> 2)) - (int)(size >> 0x20)) >> 1) + (height >> 3),
                   (int)(g_FramebufferWidth - (int)size) >> 1,0,
                   (GraphicsTextureSourceAsset *)frame.movieOrError,g_FramebufferAccess);
      }
      sprintf(label, "Video %d/%d: %s.flm  Frame %u/%u", index, count, name,
              g_ActiveMovie->currentFrameIndex, g_ActiveMovie->fileHeader->frameCount);
      DebugMovie_DrawText(8, 8, label);
      g_GraphicsFramebufferEndAccess();
      g_GraphicsFramebufferPresent(g_FramebufferAccess);
    }
  }
  Thandor_Log("debug movie %d/%d %s: stopped at frame %u/%u after %u ms", index, count, name,
              g_ActiveMovie->currentFrameIndex, g_ActiveMovie->fileHeader->frameCount,
              Thandor_TickCount() - start);
  g_TimerUnregisterPeriodic(IntroMovie_TimerTick);
  Movie_Close();
}

/* Debug tool: OPEN_THANDOR_MOVIEEXPORT=<name>[,<name>...] decodes flm\<name>.flm frame by frame (as fast
   as the stream allows) and writes moviedump\<name>.rgb (32-bit BGRA frames, top-down), moviedump\<name>.wav
   (the chosen audio track as the DirectSound buffer holds it) and moviedump\<name>.txt (width height
   frames rate). The process exits afterwards. */
static void DebugMovie_ExportOne(const char *name)
{
  uint16_t path[0x40];
  char fileName[0x80];
  int pathLength = 0;
  int nameIndex;
  uint32_t frames = 0;
  uint32_t width;
  uint32_t height;
  MovieOpenResult opened;
  MovieFrameResult frame;
  FILE *video;
  FILE *info;
  /* L"flm\<name>.flm", the name cut so that the extension and terminator still fit */
  path[pathLength++] = 'f'; path[pathLength++] = 'l'; path[pathLength++] = 'm'; path[pathLength++] = '\\';
  for (nameIndex = 0; name[nameIndex] != 0 && pathLength < 0x38; nameIndex++) {
    path[pathLength++] = (uint16_t)name[nameIndex];
  }
  path[pathLength++] = '.'; path[pathLength++] = 'f'; path[pathLength++] = 'l'; path[pathLength++] = 'm';
  path[pathLength] = 0;
  CreateDirectoryA("moviedump", NULL);
  opened = Movie_Open(1,path);
  if (opened.failed) {
    Thandor_Log("movie export %s: Movie_Open failed (eax=%08x)", name, opened.frameCountOrError);
    return;
  }
  width = g_ActiveMovie->fileHeader->widthPixels;
  height = g_ActiveMovie->fileHeader->heightPixels;
  if (g_ActiveMovie->audioVoiceSet != NULL && g_ActiveMovie->audioVoiceSet->voices[0] != NULL) {
    IDirectSoundBuffer *buffer = g_ActiveMovie->audioVoiceSet->voices[0];
    WAVEFORMATEX format;
    uint32_t formatBytes = 0;
    void *part1 = NULL;
    void *part2 = NULL;
    uint32_t bytes1 = 0;
    uint32_t bytes2 = 0;
    memset(&format, 0, sizeof format);
    buffer->lpVtbl->GetFormat(buffer, &format, sizeof format, &formatBytes);
    if (buffer->lpVtbl->Lock(buffer, 0, 0, &part1, &bytes1, &part2, &bytes2, DSBLOCK_ENTIREBUFFER) == 0) {
      FILE *wav;
      sprintf(fileName, "moviedump\\%s.wav", name);
      wav = fopen(fileName, "wb");
      if (wav != NULL) {
        uint32_t dataBytes = bytes1 + bytes2;
        uint32_t riffBytes = 36 + dataBytes;
        uint32_t fmtBytes = 16;
        fwrite("RIFF", 1, 4, wav); fwrite(&riffBytes, 4, 1, wav);
        fwrite("WAVEfmt ", 1, 8, wav); fwrite(&fmtBytes, 4, 1, wav);
        fwrite(&format, 1, 16, wav);
        fwrite("data", 1, 4, wav); fwrite(&dataBytes, 4, 1, wav);
        fwrite(part1, 1, bytes1, wav);
        if (part2 != NULL) fwrite(part2, 1, bytes2, wav);
        fclose(wav);
      }
      buffer->lpVtbl->Unlock(buffer, part1, bytes1, part2, bytes2);
      Thandor_Log("movie export %s: audio %u Hz, %u ch, %u bit, %u bytes", name, format.nSamplesPerSec,
                  format.nChannels, format.wBitsPerSample, bytes1 + bytes2);
    }
  }
  sprintf(fileName, "moviedump\\%s.rgb", name);
  video = fopen(fileName, "wb");
  for (;;) {
    int attempts = 0;
    do {
      frame = Movie_AdvanceFrame();
      if (!frame.ended) break;
      Thandor_SleepMs(5); /* the refill worker may not have loaded the next frame yet */
    } while (++attempts < 200 && g_ActiveMovie != NULL &&
             g_ActiveMovie->currentFrameIndex < g_ActiveMovie->fileHeader->frameCount);
    if (frame.ended) break;
    if (video != NULL) fwrite(g_ActiveMovie->argbPixels, 4, width * height, video);
    frames++;
  }
  if (video != NULL) fclose(video);
  sprintf(fileName, "moviedump\\%s.txt", name);
  info = fopen(fileName, "w");
  if (info != NULL) {
    fprintf(info, "%u %u %u %u\n", width, height, frames, opened.playbackRateHz);
    fclose(info);
  }
  Thandor_Log("movie export %s: %ux%u, %u frames at %u Hz", name, width, height, frames, opened.playbackRateHz);
  Movie_Close();
}

/* Debug tool: OPEN_THANDOR_MOVIE=<name> plays flm\<name>.flm, OPEN_THANDOR_MOVIE=all plays every name
   listed in movies.txt (up to 256, one per line) one after another, each with a frame counter overlay.
   OPEN_THANDOR_MOVIE_STRETCH=1 stretches the frames to the screen. The process exits afterwards. */
static void DebugMovie_Run(const char *which)
{
  const char *stretchValue = getenv("OPEN_THANDOR_MOVIE_STRETCH");
  int stretch = (stretchValue != NULL) && (stretchValue[0] == '1');
  if (strcmp(which, "all") == 0) {
    static char names[256][24];
    int count = 0;
    int i;
    FILE *list = fopen("movies.txt", "r");
    if (list == NULL) {
      Thandor_Log("debug movie: movies.txt not found");
      ExitProcess(1);
    }
    while ((count < 256) && (fgets(names[count], sizeof names[count], list) != NULL)) {
      char *end = names[count] + strlen(names[count]);
      while ((end > names[count]) && ((end[-1] == '\n') || (end[-1] == '\r') || (end[-1] == ' '))) *--end = 0;
      if (names[count][0] != 0) count++;
    }
    fclose(list);
    /* OPEN_THANDOR_MOVIE_START=<n> resumes the list at movie n (1-based). */
    i = (getenv("OPEN_THANDOR_MOVIE_START") != NULL) ? atoi(getenv("OPEN_THANDOR_MOVIE_START")) - 1 : 0;
    if (i < 0) i = 0;
    for (; i < count; i++) {
      DebugMovie_PlayOne(names[i], i + 1, count, stretch);
    }
  }
  else {
    DebugMovie_PlayOne(which, 1, 1, stretch);
  }
  Thandor_Log("debug movie: finished");
  ExitProcess(0);
}

/* Address: 0x005739D0.
   Plays the intro movies flm\intro0.flm, intro1.flm, ... until one cannot be opened, unless -NOINTRO is given.
   Each movie runs at its own rate from IntroMovie_TimerTick, centred on the screen;
   a key or mouse-button release skips to the next one, Escape skips all of them (the number jumps to 9).
   CF is set only when the first frame of an opened movie cannot be decoded.
*/
bool __thandor_cf_preserve_eax_ecx_edx Game_PlayIntroMovies(void)

{
  uint32_t frameHeightSnapshot;
  uint32_t playbackRateHz;
  uint32_t quarterFrameHeight;
  int frameAdvanceBudget;
  bool accessFailed;
  MovieFrameDimensionsEdxEax8 frameDimensions;
  MovieOpenResult openResult;
  MovieFrameResult firstFrameResult;
  MovieFrameResult advanceResult;
  KeyboardEventResult keyEvent;
  CommandLineOptionResult noIntroOption;
  CursorEventResult cursorEvent;
  
    {
    const char *exportMovies = getenv("OPEN_THANDOR_MOVIEEXPORT");
    const char *debugMovie = getenv("OPEN_THANDOR_MOVIE");
    if ((exportMovies != NULL) && (exportMovies[0] != 0)) {
      char names[0x100];
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
    while( true ) {
      openResult = Movie_Open(1,(uint16_t *)u_flm_intro0_flm_00573046);
      if (openResult.failed) break;
      firstFrameResult = Movie_AdvanceFrame();
      if (firstFrameResult.ended) {
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
        if (!keyEvent.queueEmpty) break;
        cursorEvent = g_GraphicsCursorConsumeEvent();
        /* event types above RIGHT_PRESS are the button releases */
        if ((!cursorEvent.queueEmpty) && (3 < cursorEvent.eventType)) goto GameIntroMovies_StopCurrentPlayback;
        if (g_IntroMoviePendingTicks != 0) {
          /* catch up at most three frames per pass */
          frameAdvanceBudget = 3;
          do {
            advanceResult = Movie_AdvanceFrame();
            frameHeightSnapshot = g_FramebufferHeight;
            if (advanceResult.ended) goto GameIntroMovies_StopCurrentPlayback;
            g_IntroMoviePendingTicks--;
          } while ((g_IntroMoviePendingTicks != 0) && (--frameAdvanceBudget != 0));
          quarterFrameHeight = g_FramebufferHeight >> 2;
          accessFailed = g_GraphicsFramebufferBeginAccess();
          if (accessFailed) goto GameIntroMovies_StopCurrentPlayback;
          frameDimensions = Movie_GetFrameDimensions(); /* EDX:EAX = height:width */
          /* y = (H - H/4 - frameHeight) / 2 + H/8, i.e. vertically centred; the source is the movie
             returned by the first Movie_AdvanceFrame */
          g_GraphicsTextureSourceBlitSourceAlpha
                    (g_FramebufferHeight,g_FramebufferWidth,0,0,
                     ((int)((frameHeightSnapshot - quarterFrameHeight) - (int)(frameDimensions >> 0x20)) >> 1) + (frameHeightSnapshot >> 3),
                     (int)(g_FramebufferWidth - (int)frameDimensions) >> 1,0,
                     (GraphicsTextureSourceAsset *)firstFrameResult.movieOrError,g_FramebufferAccess);
          g_GraphicsFramebufferEndAccess();
          g_GraphicsFramebufferPresent(g_FramebufferAccess);
        }
      }
      /* [9] is the digit of "flm\intro0.flm"; Escape moves on to intro9 (normally absent, which ends the intros) */
      if (keyEvent.eventCode == KEYBOARD_KEY_CODE_ESCAPE) {
        u_flm_intro0_flm_00573046[9] = L'8';
      }
GameIntroMovies_StopCurrentPlayback:
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
   DLL/procedure name is stored for the fatal-error message and a FATAL_ERROR_* code is returned with CF set.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx DynAPI_Bootstrap(void)

{
  /* EAX at the table end: the last resolved procedure (the table is never empty; incoming EAX otherwise) */
  void **resolvedProcedure = NULL;
  HINSTANCE module;
  DynamicApiBinding *bindingCursor;
  StatusResult successResult;
  StatusResult loadFailedResult;
  StatusResult moduleUnavailableResult;
  StatusResult procedureMissingResult;
  void **procedureName; /* the unresolved destination slot still holds the procedure name */
  char *moduleName;
  uint32_t moduleSlotIndex;

  bindingCursor = g_BootstrapApiBindings;
  do {
    if (bindingCursor->destination == NULL) {
      successResult.failed = false;
      successResult.valueOrError = (uint32_t)resolvedProcedure;
      return successResult;
    }
    procedureName = bindingCursor->destination;
    module = GetModuleHandleA(bindingCursor->moduleName);
    if (module == NULL) {
      /* dynapi_9 is the string "LoadLibraryA": without its module nothing can be loaded */
      if (bindingCursor->destination == (void **)dynapi_9) {
        Text_CopyNarrowToUtf16(0x100,g_PackageLastErrorPath,(uint8_t *)bindingCursor->moduleName);
        moduleUnavailableResult.failed = true;
        moduleUnavailableResult.valueOrError = FATAL_ERROR_LOADER_MODULE_MISSING;
        return moduleUnavailableResult;
      }
      module = ((BootstrapLoadLibraryAProc)g_BootstrapApiBindings[0].destination)(bindingCursor->moduleName);
      moduleSlotIndex = g_DynamicModuleCount;
      if (module == NULL) {
        Text_CopyNarrowToUtf16(0x100,g_PackageLastErrorPath,(uint8_t *)bindingCursor->moduleName);
        loadFailedResult.failed = true;
        loadFailedResult.valueOrError = FATAL_ERROR_DLL_LOAD_FAILED;
        return loadFailedResult;
      }
      g_DynamicModuleCount++;
      moduleName = bindingCursor->moduleName;
      g_DynamicModules[moduleSlotIndex].module = module;
      g_DynamicModules[moduleSlotIndex].name = moduleName;
      procedureName = bindingCursor->destination;
    }
    resolvedProcedure = (void **)GetProcAddress(module,(LPCSTR)procedureName);
    if (resolvedProcedure == NULL) {
      Text_CopyNarrowToUtf16(0x100,g_PackageLastErrorPath,(uint8_t *)bindingCursor->destination);
      Text_CopyNarrowToUtf16(0x100,g_FatalErrorDetail1Utf16,(uint8_t *)bindingCursor->moduleName);
      procedureMissingResult.failed = true;
      procedureMissingResult.valueOrError = FATAL_ERROR_DLL_PROCEDURE_MISSING;
      return procedureMissingResult;
    }
    bindingCursor->destination = resolvedProcedure;
    bindingCursor++;
  } while( true );
}


/* Address: 0x00586110.
   Looks up a command-line option (stored uppercased without its '/' or '-' by CommandLine_Parse) in
   g_CommandLine.optionBuffer, a list of NUL-terminated strings ending with an empty one. Only the first
   length bytes are compared (case-sensitive): a length including the NUL asks for an exact match, a shorter
   one for a prefix such as an option name followed by its value. Returns CF clear and the stored option in
   EBX when found, CF set otherwise; EAX is preserved.
*/
CommandLineOptionResult __thandor_ebx_cf_preserve_eax_ecx_edx
CommandLine_FindOption(CommandLineOptionLengthBytes length,char *option)

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
  do {
    if (*storedOption == '\0') {
      notFoundResult.notFound = true;
      notFoundResult.option = NULL; /* EBX not written; all callers read it only with CF clear */
      return notFoundResult;
    }
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
    bytesToBufferEnd = (int)sz_MainWindowTitle - (int)storedOption;
    compareOrScanCursor = storedOption;
    do {
      storedOption = compareOrScanCursor;
      if (bytesToBufferEnd == 0) break;
      bytesToBufferEnd--;
      storedOption = compareOrScanCursor + 1;
      scannedByte = *compareOrScanCursor;
      compareOrScanCursor = storedOption;
    } while (scannedByte != '\0');
  } while( true );
}


/* Address: 0x00586170.
   Splits the process command line (GetCommandLineA) into g_CommandLine and installs CommandLine_FindOption
   as the lookup hook: the executable path without quotes, up to three positional arguments (quotes
   removed, further ones skipped) and every '/' or '-' option without its prefix, NUL-separated in
   optionBuffer (quoted parts kept verbatim with their quotes). Everything else is uppercased (ASCII a-z
   only). The 256-byte buffers are not bounds-checked. The arguments are also stored as UTF-16.
*/
void __thandor_void_preserve_eax_ecx_edx CommandLine_Parse(void)

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
        goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
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
      if (currentChar == '\0') goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
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
          if (currentChar == '\0') goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
          if (currentChar != '"') break;
          do {
            currentChar = *commandLineCursor;
            *optionWriteNext = currentChar;
            commandLineCursor++;
            optionWriteNext++;
            if (currentChar == '\0') goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
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
          if (currentChar == '\0') goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
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
        if (currentChar == '\0') goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
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
          if (currentChar == '\0') goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
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
      if (currentChar == '\0') goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
      commandLineNext = commandLineCursor + 1;
      textWriteCursor = argumentWriteCursor + 1;
    } while (currentChar != ' ');
    *argumentWriteCursor = '\0';
  }
CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn:
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

