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
   Ownership: platform/bootstrap/runtime.
   Purpose: Process entry; terminates with ExitProcess.
   Local calls: CommandLine_Parse, DynAPI_Bootstrap, CommandLine_FindOption, Game_Run.
   Cross-module calls: ArenaHeap_Init [core/memory/allocator], FileSystem_Init [platform/filesystem/win32],
   Locale_Init [platform/system/time_locale], ErrorSystem_Init [core/error/runtime], TimerSystem_Init
   [platform/system/time_locale], Graphics_Init [graphics/core/runtime].
*/
void __cdecl ProcessEntry(void)

{
  ATOM windowClassAtom;
  HANDLE processOrThreadHandle;
  HWND windowHandle;
  int nHeight;
  int nWidth;
  dword initResultOrBitDepth;
  StatusValueEaxCf5 soundResult;
  dword adapterIndex;
  bool carryOrSoundFailed;
  bool carryIn;
  StatusValueEaxCf5 statusResult;
  FatalErrorEaxCf5 fatalResult;
  DisplayModeEaxCf5 displayModeResult;
  CommandLineFindOptionEbxCf5 soundOption;
  HMENU hMenu;
  HINSTANCE hInstance;
  dword displayHeight;
  LPVOID lpParam;
  dword displayWidth;
  
  g_hInstance = GetModuleHandleA((LPCSTR)0x0);
  processOrThreadHandle = GetCurrentProcess();
  SetPriorityClass(processOrThreadHandle,0x100);
  processOrThreadHandle = GetCurrentThread();
  SetThreadPriority(processOrThreadHandle,0);
  CommandLine_Parse();
  windowHandle = FindWindowA(sz_MainWindowClass,(LPCSTR)0x0);
  if (windowHandle == (HWND)0x0) {
    g_MainWindowClass.instance = g_hInstance;
    g_MainWindowClass.icon = LoadIconA(g_hInstance,(LPCSTR)1);
    g_MainWindowClass.cursor = LoadCursorA((HINSTANCE)0x0,(LPCSTR)0x7f00); /* IDC_ARROW */
    windowClassAtom = RegisterClassA((WNDCLASSA *)&g_MainWindowClass);
    if (windowClassAtom != 0) { /* the original tests the 16-bit ATOM in AX */
      lpParam = (LPVOID)0x0;
      hMenu = (HMENU)0x0;
      windowHandle = (HWND)0x0;
      hInstance = g_hInstance;
      nHeight = GetSystemMetrics(1);
      nWidth = GetSystemMetrics(0);
      g_MainWindow = CreateWindowExA(8,sz_MainWindowClass,sz_MainWindowTitle,0x80080000,0,0,nWidth,
                                     nHeight,windowHandle,hMenu,hInstance,lpParam);
      if (g_MainWindow != (HWND)0x0) {
        ShowWindow(g_MainWindow,1);
        UpdateWindow(g_MainWindow);
        ArenaHeap_Init();
        FileSystem_Init();
        Locale_Init();
        ErrorSystem_Init();
        if (g_CpuFeatureFlags == 0) {
          (*g_FatalErrorPrimaryDispatchCf)(0x51,true);
        }
        statusResult = DynAPI_Bootstrap();
        fatalResult = (*g_FatalErrorPrimaryDispatchCf)(statusResult.valueOrError,statusResult.carry);
        carryOrSoundFailed = fatalResult.carry;
        /* TimerSystem_Init only installs the timer procs and always clears CF */
        TimerSystem_Init();
        fatalResult = (*g_FatalErrorPrimaryDispatchCf)(fatalResult.eax,false);
        statusResult = Graphics_Init();
        (*g_FatalErrorPrimaryDispatchCf)(statusResult.valueOrError,statusResult.carry);
        statusResult = DirectInputMouse_Init();
        (*g_FatalErrorPrimaryDispatchCf)(statusResult.valueOrError,statusResult.carry);
        soundResult = DirectSound_Init();
        carryOrSoundFailed = soundResult.carry;
        Thandor_Log("DirectSound_Init: %s", carryOrSoundFailed ? "failed (continuing without sound)" : "ok");
        carryIn = false;
        if (carryOrSoundFailed) {
          soundOption = CommandLine_FindOption(6,s_SOUND_00582f28);
          carryIn = soundOption.carry;
          if (!carryIn) {
            fatalResult = (*g_FatalErrorPrimaryDispatchCf)(soundResult.valueOrError,true);
            carryIn = fatalResult.carry;
          }
        }
        initResultOrBitDepth = Network_Init();
        /* Network_Init returns 0 with CF clear (xor eax,eax) on success and an error code with CF
           set otherwise; Ghidra dropped its CF and passed the stale carry of the sound block. */
        (*g_FatalErrorPrimaryDispatchCf)(initResultOrBitDepth,initResultOrBitDepth != 0);
        PersistentSettings_Load();
        displayWidth = 0x280;
        displayHeight = 0x1e0;
        initResultOrBitDepth = PersistentSettings_ReadDword(0x10,0xc);
        adapterIndex = PersistentSettings_ReadDword(0,0);
        if (g_GraphicsAdapterCount <= adapterIndex) {
          adapterIndex = 0;
        }
        displayModeResult = (*g_GraphicsDisplayModeHook)(adapterIndex,initResultOrBitDepth,displayHeight,displayWidth);
        (*g_FatalErrorPrimaryDispatchCf)(displayModeResult.eax,displayModeResult.carry);
        UiRuntime_Initialize();
        Game_Run();
        Runtime_Shutdown();
        DestroyWindow(g_MainWindow);
      }
    }
  }
                    // WARNING: Subroutine does not return
  ExitProcess(0);
}


/* Address: 0x00512E70.
   Ownership: platform/bootstrap/runtime.
   Purpose: Clears the 0x100-byte auxiliary state and the first 0x3A00 bytes of the faction image, initializes
   eight 0x740-byte runtime records with verified defaults, allocates a fresh 0x38000-byte stat table, replaces the
   prior allocation, zeroes it, and writes a terminal 0xFFFFFFFF dword.
   [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Initializes faction economy defaults: Xenite/Tritium storage
   limits are 0xFA0 Q4; baseline Energy supply and initial Energy generation-capacity ceiling are both 0x280 Q4
   (40). These are separate resource/fuel/utility domains.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx GameData_ResetDefaults(void)

{
  FactionCapabilityFlags *capabilityFlagsSlot;
  void *memory;
  int remainingCount;
  uint relationStatePattern;
  uint factionBit;
  GameFactionRuntimeImage *factionRecordCursor;
  dword *dwordCursor;
  dword *statTableCursor;
  ArenaAllocEaxCf5 allocResult;
  StatusValueEaxCf5 status;
  
  remainingCount = 0x40;
  dwordCursor = g_GameDataAuxState.pairPressureMatrix8x8;
  for (; remainingCount != 0; remainingCount = remainingCount + -1) {
    *dwordCursor = 0;
    dwordCursor = dwordCursor + 1;
  }
  factionRecordCursor = &g_GameFactionRuntimeImage;
  for (remainingCount = 0xe80; remainingCount != 0; remainingCount = remainingCount + -1) {
    factionRecordCursor->records[0].xeniteCurrentQ4 = 0;
    factionRecordCursor = (GameFactionRuntimeImage *)&factionRecordCursor->records[0].xeniteStorageLimitQ4;
  }
  factionRecordCursor = &g_GameFactionRuntimeImage;
  remainingCount = 8;
  factionBit = 1;
  relationStatePattern = 0x1111111f;
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
    factionRecordCursor->records[0].energyGenerationCapacityQ4 = 0x280;
    factionRecordCursor->records[0].baselineEnergySupplyQ4 = 0x280;
    factionRecordCursor->records[0].xeniteStorageLimitQ4 = 4000;
    factionRecordCursor->records[0].tritiumStorageLimitQ4 = 4000;
    factionRecordCursor->records[0].terrainContributionScaleQ8 = 0x100;
    factionBit = factionBit * 2;
    relationStatePattern = relationStatePattern << 4 | relationStatePattern >> 0x1c;
    factionRecordCursor = (GameFactionRuntimeImage *)(factionRecordCursor->records + 1);
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  allocResult = (*g_MemoryApi.alloc)(0x38000);
  memory = g_GameStatTableImage;
  if (!allocResult.carry) {
    LOCK();
    UNLOCK();
    g_GameStatTableImage = (void *)allocResult.eax;
    (*g_MemoryApi.free)(memory);
    statTableCursor = (dword *)allocResult.eax;
    for (remainingCount = 0xe000; remainingCount != 0; remainingCount = remainingCount + -1) {
      *statTableCursor = 0;
      statTableCursor = statTableCursor + 1;
    }
    statTableCursor[-1] = 0xffffffff;
    g_GameFactionRuntimeImage.tail.periodicClockTick = 0;
    allocResult.eax = 0;
    allocResult.carry = false;
  }
  status.valueOrError = allocResult.eax;
  status.carry = allocResult.carry;
  return status;
}


/* Address: 0x00512F60.
   Ownership: platform/bootstrap/runtime.
   Purpose: Clears the auxiliary state, loads exactly 0x3A20 bytes from daten.hex into the faction image, replaces
   the heap-backed stat table with stat.hex, and imports oldunit.hex into fixed 0x4000-byte primary and 0x100-byte
   secondary buffers. A missing oldunit file clears both buffers and the count; earlier load failures return with
   CF set.
   Cross-module calls: Package_LoadEntryIntoBuffer [assets/package/runtime], Package_LoadEntry
   [assets/package/runtime], Resource_Release [assets/resource/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx GameData_LoadExternalTables(void)

{
  void *memory;
  dword *oldUnitBufferOrCursor;
  int remainingCount;
  dword *sourceCursor;
  dword *destinationCursor;
  StatusValueEaxCf5 loadStatus;
  PackageLoadEntryEaxCf5 packageEntry;
  
  remainingCount = 0x40;
  oldUnitBufferOrCursor = g_GameDataAuxState.pairPressureMatrix8x8;
  for (; remainingCount != 0; remainingCount = remainingCount + -1) {
    *oldUnitBufferOrCursor = 0;
    oldUnitBufferOrCursor = oldUnitBufferOrCursor + 1;
  }
  loadStatus = Package_LoadEntryIntoBuffer
                    (0x3a20,(byte *)&g_GameFactionRuntimeImage,(word *)u_daten_hex_0050e054);
  if (!loadStatus.carry) {
    packageEntry = Package_LoadEntry((word *)u_stat_hex_0050e082);
    memory = g_GameStatTableImage;
    if (!packageEntry.carry) {
      LOCK();
      UNLOCK();
      g_GameStatTableImage = packageEntry.bufferOrError;
      (*g_MemoryApi.free)(memory);
      packageEntry = Package_LoadEntry((word *)u_oldunit_hex_0050e094);
      oldUnitBufferOrCursor = packageEntry.bufferOrError;
      if (packageEntry.carry) {
        oldUnitBufferOrCursor = g_OldUnitPrimaryTable;
        for (remainingCount = 0x1000; remainingCount != 0; remainingCount = remainingCount + -1) {
          *oldUnitBufferOrCursor = 0;
          oldUnitBufferOrCursor = oldUnitBufferOrCursor + 1;
        }
        oldUnitBufferOrCursor = g_OldUnitSecondaryTable;
        for (remainingCount = 0x40; remainingCount != 0; remainingCount = remainingCount + -1) {
          *oldUnitBufferOrCursor = 0;
          oldUnitBufferOrCursor = oldUnitBufferOrCursor + 1;
        }
        g_OldUnitRecordCount = 0;
      }
      else {
        g_OldUnitRecordCount = *oldUnitBufferOrCursor;
        destinationCursor = g_OldUnitPrimaryTable;
        sourceCursor = oldUnitBufferOrCursor;
        for (remainingCount = 0x1000; sourceCursor = sourceCursor + 1, remainingCount != 0; remainingCount = remainingCount + -1) {
          *destinationCursor = *sourceCursor;
          destinationCursor = destinationCursor + 1;
        }
        destinationCursor = g_OldUnitSecondaryTable;
        for (remainingCount = 0x40; remainingCount != 0; remainingCount = remainingCount + -1) {
          *destinationCursor = *sourceCursor;
          sourceCursor = sourceCursor + 1;
          destinationCursor = destinationCursor + 1;
        }
        Resource_Release(oldUnitBufferOrCursor);
      }
      return false;
    }
  }
  return true;
}


/* Address: 0x00573BC0.
   Ownership: platform/bootstrap/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Returns resolved
   function pointer in EAX.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
DynApiResolveEaxCf5 __thandor_eax_cf_preserve_ecx_edx
DynAPI_Resolve(void **destination,HINSTANCE module,char *procedureName)

{
  FARPROC resolvedProcedure;
  dword modulesRemaining;
  DynamicModuleEntry *moduleEntryCursor;
  DynApiResolveEaxCf5 successResult;
  DynApiResolveEaxCf5 failureResult;
  
  Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)procedureName);
  resolvedProcedure = GetProcAddress(module,procedureName);
  if (resolvedProcedure != (FARPROC)0x0) {
    *destination = resolvedProcedure;
    successResult.carry = false;
    successResult.procedureOrError = resolvedProcedure;
    return successResult;
  }
  moduleEntryCursor = g_DynamicModules;
  g_FatalErrorDetail1Utf16[0] = 0;
  modulesRemaining = g_DynamicModuleCount;
  for (; modulesRemaining != 0; modulesRemaining = modulesRemaining - 1) {
    if (module == moduleEntryCursor->module) {
      /* name the module in the error detail */
      Text_CopyNarrowToUtf16Cf(0x100,g_FatalErrorDetail1Utf16,(byte *)moduleEntryCursor->name);
      break;
    }
    moduleEntryCursor = moduleEntryCursor + 1;
  }
  failureResult.carry = true;
  failureResult.procedureOrError = (void *)0x10;
  return failureResult;
}


/* Address: 0x00573C50.
   Ownership: platform/bootstrap/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Returns HMODULE in
   EAX.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
DynDllLoadEaxCf5 __thandor_eax_cf_preserve_ecx_edx DynDLL_Load(char *moduleName)

{
  HINSTANCE loadedModule;
  DynDllLoadEaxCf5 successResult;
  DynDllLoadEaxCf5 failureResult;
  dword moduleSlotIndex;
  
  Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)moduleName);
  if ((g_BootstrapApiBindings[0].destination != (void **)dynapi_9) && (g_DynamicModuleCount < 0x10))
  {
    loadedModule = (HINSTANCE)((BootstrapLoadLibraryAProc)g_BootstrapApiBindings[0].destination)(moduleName);
    moduleSlotIndex = g_DynamicModuleCount;
    if (loadedModule != (HINSTANCE)0x0) {
      g_DynamicModules[g_DynamicModuleCount].module = loadedModule;
      g_DynamicModules[moduleSlotIndex].name = moduleName;
      g_DynamicModuleCount = g_DynamicModuleCount + 1;
      successResult.carry = false;
      successResult.moduleOrError = loadedModule;
      return successResult;
    }
  }
  failureResult.carry = true;
  failureResult.moduleOrError = (HINSTANCE)0x11;
  return failureResult;
}


/* Address: 0x00573CD0.
   Ownership: platform/bootstrap/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
dword DynDLL_Unload(char *moduleName)

{
  dword modulesRemainingOrResult;
  DynamicModuleEntry *moduleEntryCursor;
  
  moduleEntryCursor = g_DynamicModules;
  modulesRemainingOrResult = g_DynamicModuleCount;
  for (; modulesRemainingOrResult != 0; modulesRemainingOrResult = modulesRemainingOrResult - 1) {
    if (moduleName == moduleEntryCursor->name) {
      modulesRemainingOrResult = ((BootstrapFreeLibraryProc)g_BootstrapApiBindings[1].destination)(moduleEntryCursor->module);
      if (modulesRemainingOrResult != 0) {
        return modulesRemainingOrResult;
      }
      break;
    }
    moduleEntryCursor = moduleEntryCursor + 1;
  }
  /* module not loaded, or FreeLibrary failed */
  Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)moduleName);
  return 0xf;
}

/* Address: 0x00573D40.
   Ownership: platform/bootstrap/runtime.
   Purpose: Handles bootstrap api resolve binding by destination.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
BootstrapApi_ResolveBindingByDestination(void **destination)

{
  void *resolvedProcedure;
  dword remainingCount;
  DynamicApiBinding *bindingCursor;
  StatusValueEaxCf5 failureResult;
  StatusValueEaxCf5 successResult;
  
  bindingCursor = g_BootstrapApiBindings;
  remainingCount = g_DynamicModuleCount;
  for (; remainingCount != 0; remainingCount = remainingCount - 1) {
    if (destination == bindingCursor->destination) {
      Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)destination);
      /* The original pushes ESI (the binding cursor) only to preserve it across the call: binding slot 0
         (LoadLibraryA) gets the destination argument as its single argument, and the result is stored
         into the matching binding (MOV [ESI],EDX after POP ESI). The function is unreferenced. */
      resolvedProcedure =
           (void *)((BootstrapLoadLibraryAProc)g_BootstrapApiBindings[0].destination)((char *)destination);
      if (resolvedProcedure != (void *)0x0) {
        bindingCursor->destination = (void **)resolvedProcedure;
        successResult.valueOrError = 0xf;
        successResult.carry = false;
        return successResult;
      }
      break;
    }
    bindingCursor = bindingCursor + 1;
  }
  failureResult.carry = true;
  failureResult.valueOrError = 0xf;
  return failureResult;
}


/* Address: 0x00573EB0.
   Ownership: platform/bootstrap/runtime.
   Purpose: Releases all cached dynamic modules.
*/
void __thandor_void_preserve_eax_ecx_edx DynDLL_UnloadAll(void)

{
  dword modulesRemaining;
  DynamicModuleEntry *moduleEntryCursor;
  HINSTANCE loadedModule;
  
  moduleEntryCursor = g_DynamicModules;
  for (modulesRemaining = g_DynamicModuleCount; modulesRemaining != 0;
      modulesRemaining = modulesRemaining - 1) {
    if (moduleEntryCursor->module != (HINSTANCE)0x0) {
      loadedModule = moduleEntryCursor->module;
      moduleEntryCursor->module = (HINSTANCE)0x0;
      ((BootstrapFreeLibraryProc)g_BootstrapApiBindings[1].destination)(loadedModule);
    }
    moduleEntryCursor = moduleEntryCursor + 1;
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
  ushort keyState;
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
        (*g_MouseDevice->lpVtbl->Unacquire)(g_MouseDevice);
      }
      if (g_WindowDestroyDepth == 0) {
        (*g_GraphicsBackendRefreshActiveAdapterCf)();
      }
    }
    else {
      currentProcess = GetCurrentProcess();
      SetPriorityClass(currentProcess,0x100);
      if (g_MouseDevice != (IDirectInputDeviceA *)0x0) {
        (*g_MouseDevice->lpVtbl->Acquire)(g_MouseDevice);
      }
      if (-1 < (int)g_ActiveGraphicsAdapterIndex) {
        (*g_GraphicsDisplayModeHook)
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
        (*g_KeyboardFlushEvents)();
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
   Ownership: platform/bootstrap/runtime.
   Purpose: Executes CPUID leaf 1, sets g_CpuFeatureFlags bit 0 when EDX bit 23 reports MMX support, and returns
   constant 5.
*/
dword __cdecl CPU_DetectFeatures(void)

{
  int cpuidVersionInfo;
  
  cpuidVersionInfo = cpuid_Version_info(1);
  if ((*(uint *)(cpuidVersionInfo + 8) & 0x800000) != 0) {
    g_CpuFeatureFlags = g_CpuFeatureFlags | 1;
  }
  return 5;
}

/* Address: 0x00573070.
   Ownership: platform/bootstrap/runtime.
   Purpose: Handles game run.
   Local calls: GameRuntime_InitializeSpatialAudioAndRenderingCf, Game_LoadCoreAssets, Game_PlayIntroMovies.
   Cross-module calls: PersistentSettings_Load [core/settings/persistent], PersistentSettings_ReadDword
   [core/settings/persistent], PersistentSettings_WriteDword [core/settings/persistent], Frontend_MainLoop
   [ui/frontend/runtime].
*/
void __cdecl Game_Run(void)

{
  StatusValueEaxCf5 renderingInitResult;
  dword loadResultOrWidth;
  dword displayHeight;
  dword bitDepth;
  dword adapterIndex;
  bool dispatchCarry;
  GraphicsCursorFrameEaxCf5 cursorFrameResult;
  FatalErrorEaxCf5 fatalResult;
  DisplayModeEaxCf5 displayModeResult;
  FrontendMainLoopEaxCf5 mainLoopResult;
  
  cursorFrameResult = (*g_GraphicsCursorSetFrame)(0);
  fatalResult = (*g_FatalErrorPrimaryDispatchCf)(cursorFrameResult.eax,cursorFrameResult.carry);
  dispatchCarry = fatalResult.carry;
  renderingInitResult = GameRuntime_InitializeSpatialAudioAndRenderingCf();
  fatalResult = (*g_FatalErrorPrimaryDispatchCf)(renderingInitResult.valueOrError,renderingInitResult.carry);
  dispatchCarry = fatalResult.carry;
  loadResultOrWidth = Game_LoadCoreAssets();
  Thandor_Log("Game_LoadCoreAssets -> 0x%08X", loadResultOrWidth);
  /* 0 with CF clear on success, an error code with CF set otherwise */
  fatalResult = (*g_FatalErrorPrimaryDispatchCf)(loadResultOrWidth,loadResultOrWidth != 0);
  /* keeps EAX: a movie that cannot start is reported with the previous value */
  dispatchCarry = Game_PlayIntroMovies();
  (*g_FatalErrorPrimaryDispatchCf)(fatalResult.eax,dispatchCarry);
  PersistentSettings_Load();
  loadResultOrWidth = PersistentSettings_ReadDword(0x280,4);
  displayHeight = PersistentSettings_ReadDword(0x1e0,8);
  bitDepth = PersistentSettings_ReadDword(0x10,0xc);
  if (((loadResultOrWidth != 0x280) || (displayHeight != 0x1e0)) || (bitDepth != 0x10)) {
    adapterIndex = PersistentSettings_ReadDword(0,0);
    if (g_GraphicsAdapterCount <= adapterIndex) {
      adapterIndex = 0;
    }
    displayModeResult = (*g_GraphicsDisplayModeHook)(adapterIndex,bitDepth,displayHeight,loadResultOrWidth);
    (*g_FatalErrorPrimaryDispatchCf)(displayModeResult.eax,displayModeResult.carry);
    PersistentSettings_WriteDword(g_ActiveGraphicsAdapterIndex,0);
  }
  mainLoopResult = Frontend_MainLoop(1);
  (*g_FatalErrorPrimaryDispatchCf)(mainLoopResult.errorOrValue,mainLoopResult.carry);
  (*g_NetworkBackendSlot3)();
  (*g_NetworkBackendSlot1)();
  return;
}


/* Address: 0x0050BB10.
   Ownership: platform/bootstrap/runtime.
   Purpose: Runs the exact ordered startup chain for the spatial-sound pool, two rendering lookup-table
   allocations, the software-renderer display-mode hook, and the global primitive queue with capacity 0xA000. Each
   step runs only after CF-clear success from the preceding step; the final CF reports the first failure or final
   allocation result.
   Cross-module calls: SpatialSoundPool_Init [audio/spatial/runtime], TerrainByteClampLookup_Initialize
   [world/terrain/visuals], GraphicsIntensityClampTable_InitializeCf [graphics/render/shading],
   SoftwareRenderer_InstallDisplayModeHook [graphics/backend/software], GraphicsPrimitiveQueue_AllocateGlobalPool
   [graphics/render/primitives].
*/
StatusValueEaxCf5 __cdecl GameRuntime_InitializeSpatialAudioAndRenderingCf(void)

{
  StatusValueEaxCf5 step;
  
  step = SpatialSoundPool_Init();
  if (step.carry) {
    return step;
  }
  step = TerrainByteClampLookup_Initialize();
  if (step.carry) {
    return step;
  }
  step = GraphicsIntensityClampTable_InitializeCf();
  if (step.carry) {
    return step;
  }
  step = SoftwareRenderer_InstallDisplayModeHook();
  if (step.carry) {
    return step;
  }
  return GraphicsPrimitiveQueue_AllocateGlobalPool(0xa000);
}


/* Address: 0x00573140.
   Ownership: platform/bootstrap/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string], WidePath_CombineDirectoryAndLeaf
   [core/text/path], Package_Mount [assets/package/runtime], LevelPackage_ValidateAndMount
   [assets/package/runtime], Resource_Load [assets/resource/runtime], Resource_Release [assets/resource/runtime].
*/
dword __cdecl Game_LoadCoreAssets(void)

{
  wchar_t screenshotTensDigit;
  int statusOrCount;
  SoundSampleAsset *loadedResource;
  FncModuleHeader *module;
  word *textBuffer;
  dword settingsOrBufferBase;
  AudioMixerGainQ15 uiSoundGain;
  MovieAudioGainQ15 movieGain;
  MovieAudioGainQ15 alternateMovieGain;
  float *splineBuffer;
  FrontendPlayerRuntimeRecord *playerRecordCursor;
  byte *scratchCursor;
  TextResourceId resourceId;
  FrontendPlayerRuntimeRecord **playerRuntimePointerTableWriteCursor;
  StatusValueEaxCf5 status;
  SoundCreateSampleVoiceSetEaxCf5 voiceSetResult;
  FileSystemOpenEaxCf5 openResult;
  TextResourceResolveEaxCf5 textResolveResult;
  TextResourceLoadEaxCf5 textPageLoadResult;
  PackageLoadEntryEaxCf5 pcxModuleEntry;
  FncModuleLoadEaxCf5 moduleLoadResult;
  GraphicsTextureSourceLoadEaxCf5 panelTextureResult;
  ArenaAllocEaxCf5 allocResult;
  ResourceLoadEaxEcxCf9 resourceLoadResult;
  
  if ((g_MemoryApi.alloc == ArenaHeap_Alloc) &&
     (statusOrCount = ((BootstrapRegOpenKeyExAProc)g_BootstrapApiBindings[5].destination)
                        (0x80000002,s_Software_Planet4_Thandor_00572e20,0,0x20019,
                         &g_InstallRegistryKeyHandle), statusOrCount == 0)) {
    statusOrCount = ((BootstrapRegQueryValueExAProc)g_BootstrapApiBindings[6].destination)
                      (g_InstallRegistryKeyHandle,&s_InstallRegistryValueNameCD,0,
                       &g_InstallRegistryValueType,&g_InstallRegistryValueDataA,
                       &g_InstallRegistryValueDataCapacityBytes);
    if ((statusOrCount == 0) && (g_InstallRegistryValueType == 1)) {
      Text_CopyNarrowToUtf16Cf
                (0x200,(word *)&g_InstallDirectoryScratchUtf16,&g_InstallRegistryValueDataA);
      WidePath_CombineDirectoryAndLeaf
                (g_LooseMoviePathPrefix.codeUnits,(word *)u_Thandor_00572e10,
                 (word *)&g_InstallDirectoryScratchUtf16);
    }
    ((BootstrapRegCloseKeyProc)g_BootstrapApiBindings[7].destination)(g_InstallRegistryKeyHandle);
  }
  {
    /* open-thandor: the full-length movies from the CD (Ende*.flm, Intro2.flm) live in the
       flm folder of the game directory, so the CD is no longer needed. Movie_Open looks under
       g_LooseMoviePathPrefix before the packages, which only hold still-image stand-ins for
       these movies; point the prefix at the game directory when that folder exists. */
    static const word flmLeaf[4] = {'f','l','m',0};
    static word localFlmPath[0x100];
    char narrow[0x100];
    int k;
    WidePath_CombineDirectoryAndLeaf
              (localFlmPath,(word *)flmLeaf,(word *)&g_ExecutableDirectoryUtf16);
    if (Thandor_DirectoryExistsW(localFlmPath)) {
      word *directory = (word *)&g_ExecutableDirectoryUtf16;
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
  g_PatchArchivePathTemplateUtf16.decimalDigits.packedDigits = 0x300030;
  do {
    do {
      Package_Mount(g_PatchArchivePathTemplateUtf16.prefixCodeUnits);
      g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[1] =
           g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[1] - 1;
    } while (0x2f < g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[1]);
    g_PatchArchivePathTemplateUtf16.decimalDigits.packedDigits =
         g_PatchArchivePathTemplateUtf16.decimalDigits.packedDigits + 0x9ffff;
  } while (0x2f < g_PatchArchivePathTemplateUtf16.decimalDigits.codeUnits[0]);
  g_LevelArchivePathTemplateUtf16.decimalDigits.packedDigits = 0x390039;
  do {
    do {
      LevelPackage_ValidateAndMount(g_LevelArchivePathTemplateUtf16.prefixCodeUnits);
      g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[1] =
           g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[1] - 1;
    } while (0x2f < g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[1]);
    g_LevelArchivePathTemplateUtf16.decimalDigits.packedDigits =
         g_LevelArchivePathTemplateUtf16.decimalDigits.packedDigits + 0x9ffff;
  } while (0x2f < g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[0]);
  status = Package_Mount((word *)u_daten_pck_00572e56);
  if (!status.carry) {
    g_DataPackageHandle = status.valueOrError;
  }
  status = Package_Mount((word *)u_modelle_pck_00572e6a);
  if (!status.carry) {
    g_ModelPackageHandle = status.valueOrError;
  }
  status = Package_Mount((word *)u_graphik_pck_00572e82);
  if (!status.carry) {
    g_GraphicsPackageHandle = status.valueOrError;
  }
  status = Package_Mount((word *)u_sound_pck_00572e9a);
  if (!status.carry) {
    g_SoundPackageHandle = status.valueOrError;
  }
  status = Package_Mount((word *)u_filme_pck_00572eae);
  if (!status.carry) {
    g_MoviePackageHandle = status.valueOrError;
  }
  status = Package_Mount((word *)u_level_pck_00572ec2);
  if (!status.carry) {
    g_LevelPackageHandle = status.valueOrError;
  }
  resourceLoadResult = Resource_Load((word *)u_sound_button0_sam_00572f06);
  loadedResource = (SoundSampleAsset *)resourceLoadResult.eax;
  if (resourceLoadResult.carry) {
    return (dword)loadedResource;
  }
  voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedResource);
  module = (FncModuleHeader *)voiceSetResult.eax;
  if (!voiceSetResult.carry) {
    Resource_Release(loadedResource);
    g_UiButtonSoundVoiceSets7[0] = (DirectSoundVoiceSet *)module;
    resourceLoadResult = Resource_Load((word *)u_sound_button1_sam_00572f2a);
    loadedResource = (SoundSampleAsset *)resourceLoadResult.eax;
    if (resourceLoadResult.carry) {
      return (dword)loadedResource;
    }
    voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedResource);
    module = (FncModuleHeader *)voiceSetResult.eax;
    if (!voiceSetResult.carry) {
      Resource_Release(loadedResource);
      g_UiButtonSoundVoiceSets7[1] = (DirectSoundVoiceSet *)module;
      resourceLoadResult = Resource_Load((word *)u_sound_button2_sam_00572f4e);
      loadedResource = (SoundSampleAsset *)resourceLoadResult.eax;
      if (resourceLoadResult.carry) {
        return (dword)loadedResource;
      }
      voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedResource);
      module = (FncModuleHeader *)voiceSetResult.eax;
      if (!voiceSetResult.carry) {
        Resource_Release(loadedResource);
        g_UiButtonSoundVoiceSets7[2] = (DirectSoundVoiceSet *)module;
        resourceLoadResult = Resource_Load((word *)u_sound_button3_sam_00572f72);
        loadedResource = (SoundSampleAsset *)resourceLoadResult.eax;
        if (resourceLoadResult.carry) {
          return (dword)loadedResource;
        }
        voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedResource);
        module = (FncModuleHeader *)voiceSetResult.eax;
        if (!voiceSetResult.carry) {
          Resource_Release(loadedResource);
          g_UiButtonSoundVoiceSets7[3] = (DirectSoundVoiceSet *)module;
          resourceLoadResult = Resource_Load((word *)u_sound_button4_sam_00572f96);
          loadedResource = (SoundSampleAsset *)resourceLoadResult.eax;
          if (resourceLoadResult.carry) {
            return (dword)loadedResource;
          }
          voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedResource);
          module = (FncModuleHeader *)voiceSetResult.eax;
          if (!voiceSetResult.carry) {
            Resource_Release(loadedResource);
            g_UiButtonSoundVoiceSets7[4] = (DirectSoundVoiceSet *)module;
            resourceLoadResult = Resource_Load((word *)u_sound_button5_sam_00572fba);
            loadedResource = (SoundSampleAsset *)resourceLoadResult.eax;
            if (resourceLoadResult.carry) {
              return (dword)loadedResource;
            }
            voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedResource);
            module = (FncModuleHeader *)voiceSetResult.eax;
            if (!voiceSetResult.carry) {
              Resource_Release(loadedResource);
              g_UiButtonSoundVoiceSets7[5] = (DirectSoundVoiceSet *)module;
              resourceLoadResult = Resource_Load((word *)u_sound_button6_sam_00572fde);
              loadedResource = (SoundSampleAsset *)resourceLoadResult.eax;
              if (resourceLoadResult.carry) {
                return (dword)loadedResource;
              }
              voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedResource);
              module = (FncModuleHeader *)voiceSetResult.eax;
              if (!voiceSetResult.carry) {
                Resource_Release(loadedResource);
                g_UiButtonSoundVoiceSets7[6] = (DirectSoundVoiceSet *)module;
                do {
                  do {
                    openResult = (*g_FileSystemOpenCf)(0,(word *)(u_Dscreen00_pcx_00572e3a + 1));
                    if (openResult.carry)
                    goto Game_LoadCoreAssets_BindDebugOverlayTextAndContinueRemainingAssetLoad;
                    u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + L'\x01';
                    (*g_FileSystemClose)((void *)openResult.eax);
                    screenshotTensDigit = u_Dscreen00_pcx_00572e3a[7];
                  } while ((ushort)u_Dscreen00_pcx_00572e3a[8] < 0x3a);
                  u_Dscreen00_pcx_00572e3a[7] = u_Dscreen00_pcx_00572e3a[7] + L'\x01';
                  u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + L'\xfff6';
                } while ((ushort)u_Dscreen00_pcx_00572e3a[7] < 0x3a);
                u_Dscreen00_pcx_00572e3a[7] = screenshotTensDigit + L'\xfff7';
Game_LoadCoreAssets_BindDebugOverlayTextAndContinueRemainingAssetLoad:
                resourceId = 0x112;
                do {
                  textResolveResult = TextResource_Resolve(resourceId);
                  textBuffer = textResolveResult.eax;
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
                UiActionHandlers_SetPageCf
                          (0x10,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage10);
                UiActionHandlers_SetPageCf
                          (0x11,(UiActionHandlerPage *)&g_InGameUiCommandModeActionHandlers30);
                UiActionHandlers_SetPageCf
                          (0x12,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage12);
                UiActionHandlers_SetPageCf
                          (0x20,(UiActionHandlerPage *)&g_FrontendUiActionHandlersPage20);
                textPageLoadResult = TextResourcePage_Load(0xff,(word *)u_texte_neterror_str_0050f104);
                if (textPageLoadResult.carry) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x18,(word *)u_texte_help_str_00563170);
                if (textPageLoadResult.carry) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x20,(word *)u_texte_hilfe_str_00545b34);
                if (textPageLoadResult.carry) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x21,(word *)u_texte_menue_str_00545ba0);
                if (textPageLoadResult.carry) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x30,(word *)u_texte_techno_str_0050dec4);
                if (textPageLoadResult.carry) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x22,(word *)u_texte_level_str_00545bc0);
                if (textPageLoadResult.carry) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x23,(word *)u_texte_inhalt_str_00545be0);
                if (textPageLoadResult.carry) {
                  return textPageLoadResult.errorOrValue;
                }
                textPageLoadResult = TextResourcePage_Load(0x24,(word *)u_texte_tastatur_str_005631b8);
                if (textPageLoadResult.carry) {
                  return textPageLoadResult.errorOrValue;
                }
                textResolveResult = TextResource_Resolve(0x2402);
                RichTextCommandStream_BindTextureSource(g_CursorSourceAsset,textResolveResult.eax);
                settingsOrBufferBase = PersistentSettings_ReadDword(3,0x20);
                uiSoundGain = 0;
                if ((settingsOrBufferBase & 1) != 0) {
                  uiSoundGain = PersistentSettings_ReadDword(0x8000,0x24);
                }
                movieGain = 0;
                g_UiSoundGainQ15 = uiSoundGain;
                g_SoundEffectsGainQ15 = uiSoundGain;
                if ((settingsOrBufferBase & 1) != 0) {
                  movieGain = PersistentSettings_ReadDword(0x8000,0x28);
                }
                alternateMovieGain = 0;
                g_MovieDefaultAudioGainQ15 = movieGain;
                if ((settingsOrBufferBase & 1) != 0) {
                  alternateMovieGain = PersistentSettings_ReadDword(0x8000,0x4c);
                }
                g_ReverseStereoMask = 0;
                if ((settingsOrBufferBase & 4) != 0) {
                  g_ReverseStereoMask = 0xffffffff;
                }
                g_MovieAlternateAudioGainQ15 = alternateMovieGain;
                g_ModelLodDepthThresholdQ8 = PersistentSettings_ReadDword(g_ReverseStereoMask,0x34);
                status = AiRuntime_InitWorkspace();
                if (status.carry) {
                  return status.valueOrError;
                }
                pcxModuleEntry = Package_LoadEntry((word *)u_engine_pcx_fnc_00573028);
                if (pcxModuleEntry.carry) {
                  return (dword)pcxModuleEntry.bufferOrError;
                }
                moduleLoadResult = FncModule_LoadAndRelocateCf(pcxModuleEntry.bufferOrError);
                module = (FncModuleHeader *)moduleLoadResult.moduleBase;
                loadedResource = (SoundSampleAsset *)pcxModuleEntry.bufferOrError;
                if (!moduleLoadResult.carry) {
                  g_PcxFunctionModule = module;
                  status = FncModule_GetExportByIndexCf(3,module);
                  module = (FncModuleHeader *)status.valueOrError;
                  if (!status.carry) {
                    g_PcxFunctionExport3 = (PcxEncodeProc *)module;
                    status = FncModule_GetExportByIndexCf(2,g_PcxFunctionModule);
                    module = (FncModuleHeader *)status.valueOrError;
                    if (!status.carry) {
                      g_PcxFunctionExport2 = (PcxDecodeProc *)module;
                      /* EDX still holds the engine\pcx.fnc package buffer. */
                      Resource_Release((SoundSampleAsset *)pcxModuleEntry.bufferOrError);
                      panelTextureResult = (*g_GraphicsTextureSourceLoadPackageAsset)
                                         ((word *)u_gfx_panel_stat_gfx_00573002);
                      if (panelTextureResult.carry) {
                        return (dword)panelTextureResult.eax;
                      }
                      g_InGameStatusPanelTextureSource = panelTextureResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(0x800);
                      if (allocResult.carry) {
                        return (dword)(RecentTextHistorySlot *)allocResult.eax;
                      }
                      g_RecentTextSlotStorage = (RecentTextHistorySlot *)allocResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(0x100);
                      if (allocResult.carry) {
                        return (dword)(dword *)allocResult.eax;
                      }
                      g_OldUnitSecondaryTable = (dword *)allocResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(0x4000);
                      if (allocResult.carry) {
                        return (dword)(dword *)allocResult.eax;
                      }
                      g_OldUnitPrimaryTable = (dword *)allocResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(0x400);
                      settingsOrBufferBase = allocResult.eax;
                      if (allocResult.carry) {
                        return settingsOrBufferBase;
                      }
                      g_FrontendPlayerListRow1 = settingsOrBufferBase + 0x80;
                      g_FrontendPlayerListRow2 = settingsOrBufferBase + 0x100;
                      g_FrontendPlayerListRow3 = settingsOrBufferBase + 0x180;
                      g_FrontendPlayerListRow4 = settingsOrBufferBase + 0x200;
                      g_FrontendPlayerListRow5 = settingsOrBufferBase + 0x280;
                      g_FrontendPlayerListRow6 = settingsOrBufferBase + 0x300;
                      g_FrontendPlayerListRow7 = settingsOrBufferBase + 0x380;
                      g_FrontendPlayerListRows = settingsOrBufferBase;
                      allocResult = (*g_MemoryApi.alloc)(0x800);
                      if (allocResult.carry) {
                        return (dword)(RomRegistrySlot *)allocResult.eax;
                      }
                      g_RomRegistrySlots = (RomRegistrySlot *)allocResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(0x80);
                      if (allocResult.carry) {
                        return (dword)(FrontendSessionDiscoveryRecordB0 **)allocResult.eax;
                      }
                      g_FrontendSessionListRows = (FrontendSessionDiscoveryRecordB0 **)allocResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(0x1600);
                      if (allocResult.carry) {
                        return (dword)(FrontendSessionDiscoveryRecordB0 *)allocResult.eax;
                      }
                      g_FrontendSessionDiscoveryRecords =
                           (FrontendSessionDiscoveryRecordB0 *)allocResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(0x2000);
                      textBuffer = (word *)allocResult.eax;
                      if (allocResult.carry) {
                        return (dword)textBuffer;
                      }
                      g_InGameFactionStatusTextScratchUtf16 = textBuffer;
                      g_InGameFactionStatusTextScratchUtf16Mirror = textBuffer;
                      allocResult = (*g_MemoryApi.alloc)(0x160);
                      if (allocResult.carry) {
                        return (dword)(word *)allocResult.eax;
                      }
                      g_InGamePlayerListTextScratchUtf16 = (word *)allocResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(0x6000);
                      splineBuffer = (float *)allocResult.eax;
                      if (allocResult.carry) {
                        return (dword)splineBuffer;
                      }
                      g_WorldMotionSplineMatrixWorkspaces[1] = splineBuffer + 0x400;
                      g_WorldMotionSplineMatrixWorkspaces[2] = splineBuffer + 0x800;
                      g_WorldMotionSplineMatrixWorkspaces[3] = splineBuffer + 0xc00;
                      g_WorldMotionSplineMatrixWorkspaces[4] = splineBuffer + 0x1000;
                      g_WorldMotionSplineMatrixWorkspaces[5] = splineBuffer + 0x1400;
                      g_WorldMotionSplineMatrixWorkspaces[0] = splineBuffer;
                      allocResult = (*g_MemoryApi.alloc)(0x300);
                      splineBuffer = (float *)allocResult.eax;
                      if (allocResult.carry) {
                        return (dword)splineBuffer;
                      }
                      g_WorldMotionSplineCoefficientTables[1] = splineBuffer + 0x20;
                      g_WorldMotionSplineCoefficientTables[2] = splineBuffer + 0x40;
                      g_WorldMotionSplineCoefficientTables[3] = splineBuffer + 0x60;
                      g_WorldMotionSplineCoefficientTables[4] = splineBuffer + 0x80;
                      g_WorldMotionSplineCoefficientTables[5] = splineBuffer + 0xa0;
                      g_WorldMotionSplineCoefficientTables[0] = splineBuffer;
                      allocResult = (*g_MemoryApi.alloc)(0x408c0);
                      if (allocResult.carry) {
                        return (dword)(SelectionPlayerRuntimeBlock *)allocResult.eax;
                      }
                      g_SelectionPlayerBlocks = (SelectionPlayerRuntimeBlock *)allocResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(0x1300);
                      if (allocResult.carry) {
                        return allocResult.eax;
                      }
                      g_FrontendLocalPlayerPcxPreview = allocResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(0x4000);
                      if (allocResult.carry) {
                        return allocResult.eax;
                      }
                      g_TerrainRegionCollectionEntries = allocResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(800);
                      if (allocResult.carry) {
                        return allocResult.eax;
                      }
                      g_FrontendPlayerMessageBuffers = allocResult.eax;
                      allocResult = (*g_MemoryApi.alloc)(0x9d80);
                      playerRecordCursor = (FrontendPlayerRuntimeRecord *)allocResult.eax;
                      if (allocResult.carry) {
                        return (dword)playerRecordCursor;
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
                        playerRuntimePointerTableWriteCursor =
                             playerRuntimePointerTableWriteCursor + 1;
                        playerRecordCursor = playerRecordCursor + 1;
                        statusOrCount = statusOrCount + -1;
                      } while (statusOrCount != 0);
                      allocResult = (*g_MemoryApi.alloc)(0xe00);
                      scratchCursor = (byte *)allocResult.eax;
                      if (allocResult.carry) {
                        return (dword)scratchCursor;
                      }
                      g_CoreAssetScratchSlice1 = scratchCursor + 0x200;
                      g_CoreAssetScratchSlice2 = scratchCursor + 0x400;
                      g_CoreAssetScratchSlice3 = scratchCursor + 0x600;
                      g_CoreAssetScratchSlice4 = scratchCursor + 0x800;
                      g_CoreAssetScratchSlice5 = scratchCursor + 0xa00;
                      g_CoreAssetScratchSlice6 = scratchCursor + 0xc00;
                      g_CoreAssetScratchSlice0 = scratchCursor;
                      for (statusOrCount = 0x380; statusOrCount != 0; statusOrCount = statusOrCount + -1) {
                        scratchCursor[0] = 0;
                        scratchCursor[1] = 0;
                        scratchCursor[2] = 0;
                        scratchCursor[3] = 0;
                        scratchCursor = scratchCursor + 4;
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
  return (dword)module;
}


/* Debug tool: movie test player.
   OPEN_THANDOR_MOVIE=<name>  plays flm\<name>.flm,
   OPEN_THANDOR_MOVIE=all     plays every name listed in movies.txt (one per line, working dir).
   Each movie runs at its own rate for at most 10 seconds; a key or mouse click skips to the next.
   The name and frame counter are drawn top left. OPEN_THANDOR_MOVIE_STRETCH=1 draws full screen
   with the end-movie bilinear stretch. The process exits after the last movie. */

static const byte g_DebugFont5x7[][8] = {
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
  dword *fb = (dword *)g_FramebufferAccess;
  int scale = 1;
  int length = (int)strlen(text);
  int boxWidth = length * 6 * scale + 2 * scale;
  int boxHeight = 9 * scale;
  dword pitch;
  dword bpp;
  byte *pixels;
  int x;
  int y;
  int c;
  if (fb == NULL) {
    return;
  }
  pitch = fb[0];
  bpp = fb[2];
  pixels = (byte *)(uintptr_t)fb[3];
  if ((pixels == NULL) || ((bpp != 4) && (bpp != 2))) {
    return;
  }
  if (x0 + boxWidth > (int)g_FramebufferWidth) boxWidth = (int)g_FramebufferWidth - x0;
#define DEBUG_PUT(px, py, white)                                                          \
  do {                                                                                    \
    if (bpp == 4) ((dword *)pixels)[(py) * pitch + (px)] = (white) ? 0xffffff40 : 0xff000000; \
    else ((word *)pixels)[(py) * pitch + (px)] = (white) ? 0xffe8 : 0;                    \
  } while (0)
  for (y = 0; y < boxHeight; y++) {
    for (x = 0; x < boxWidth; x++) {
      DEBUG_PUT(x0 + x, y0 + y, 0);
    }
  }
  for (c = 0; c < length; c++) {
    char ch = text[c];
    const byte *glyph = NULL;
    unsigned g;
    if ((ch >= 'A') && (ch <= 'Z')) ch = (char)(ch - 'A' + 'a');
    for (g = 0; g < sizeof g_DebugFont5x7 / sizeof g_DebugFont5x7[0]; g++) {
      if (g_DebugFont5x7[g][0] == (byte)ch) { glyph = g_DebugFont5x7[g] + 1; break; }
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

static void DebugMovie_ClearScreen(void)
{
  if (!(*g_GraphicsFramebufferBeginAccess)()) {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0,
               0xff000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  }
}

/* Plays one movie; returns after the end, 10 seconds, or a key/click. */
static void DebugMovie_PlayOne(const char *name, int index, int count, int stretch)
{
  word path[0x40];
  char label[0x80];
  int n = 0;
  int i;
  unsigned start;
  MovieOpenEaxCf5 opened;
  MovieAdvanceFrameEaxCf5 frame;
  path[n++] = 'f'; path[n++] = 'l'; path[n++] = 'm'; path[n++] = '\\';
  for (i = 0; name[i] != 0 && n < 0x38; i++) path[n++] = (word)name[i];
  path[n++] = '.'; path[n++] = 'f'; path[n++] = 'l'; path[n++] = 'm';
  path[n] = 0;
  DebugMovie_ClearScreen();
  DebugMovie_ClearScreen();
  opened = Movie_Open(1,path);
  if (opened.carry) {
    Thandor_Log("debug movie %d/%d %s: Movie_Open failed (eax=%08x)", index, count, name, opened.eax);
    sprintf(label, "Video %d/%d: %s.flm - OEFFNEN FEHLGESCHLAGEN", index, count, name);
    if (!(*g_GraphicsFramebufferBeginAccess)()) {
      DebugMovie_DrawText(8, 8, label);
      (*g_GraphicsFramebufferEndAccess)();
      (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
    }
    Thandor_SleepMs(1500);
    return;
  }
  frame = Movie_AdvanceFrame();
  if (frame.carry) {
    Thandor_Log("debug movie %d/%d %s: first frame failed", index, count, name);
    Movie_Close();
    return;
  }
  Thandor_Log("debug movie %d/%d %s: playing, %u frames at %u Hz", index, count, name,
              g_ActiveMovie->fileHeader->frameCount, opened.playbackRateHzEcx);
  g_IntroMoviePendingTicks = 0;
  UiFrame_FlushInputAndResetPendingTicks();
  (*g_TimerRegisterPeriodic)(opened.playbackRateHzEcx,IntroMovie_TimerTick);
  start = Thandor_TickCount();
  for (;;) {
    KeyboardEventEaxEdxCf9 key;
    GraphicsCursorInputEventRegsCf21 cursor;
    (*g_Win32PumpMessages)();
    key = (*g_KeyboardReadEvent)();
    if (!key.carry) break;
    cursor = (*g_GraphicsCursorConsumeEvent)();
    if ((!cursor.carry) && (3 < cursor.eventCode)) break;
    if (Thandor_TickCount() - start > 10000) break;
    if (g_IntroMoviePendingTicks != 0) {
      int burst = 3;
      MovieAdvanceFrameEaxCf5 next;
      int ended = 0;
      do {
        next = Movie_AdvanceFrame();
        if (next.carry) { ended = 1; break; }
        g_IntroMoviePendingTicks = g_IntroMoviePendingTicks - 1;
      } while ((g_IntroMoviePendingTicks != 0) && (--burst != 0));
      if (ended) break;
      if ((*g_GraphicsFramebufferBeginAccess)()) break;
      if (stretch) {
        (*g_GraphicsTextureSourceStretchDirectColorBilinear)
                  (g_FramebufferHeight,g_FramebufferWidth,0,0,0,
                   (GraphicsTextureSourceAsset *)frame.eax,g_FramebufferAccess);
      }
      else {
        MovieFrameDimensionsEdxEax8 size = Movie_GetFrameDimensions();
        dword height = g_FramebufferHeight;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (g_FramebufferHeight,g_FramebufferWidth,0,0,
                   ((int)((height - (height >> 2)) - (int)(size >> 0x20)) >> 1) + (height >> 3),
                   (int)(g_FramebufferWidth - (int)size) >> 1,0,
                   (GraphicsTextureSourceAsset *)frame.eax,g_FramebufferAccess);
      }
      sprintf(label, "Video %d/%d: %s.flm  Frame %u/%u", index, count, name,
              g_ActiveMovie->currentFrameIndex, g_ActiveMovie->fileHeader->frameCount);
      DebugMovie_DrawText(8, 8, label);
      (*g_GraphicsFramebufferEndAccess)();
      (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
    }
  }
  Thandor_Log("debug movie %d/%d %s: stopped at frame %u/%u after %u ms", index, count, name,
              g_ActiveMovie->currentFrameIndex, g_ActiveMovie->fileHeader->frameCount,
              Thandor_TickCount() - start);
  (*g_TimerUnregisterPeriodic)(IntroMovie_TimerTick);
  Movie_Close();
}

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
   Ownership: platform/bootstrap/runtime.
   Purpose: Clears and presents the framebuffer, honors the NOINTRO option, plays sequentially numbered files
   beginning with flm\intro0.flm, advances at most three timer-pending frames per loop, centers the active
   MovieRuntime, and aborts on keyboard or qualifying cursor input. Escape writes digit 8 before cleanup so the
   next increment exits through intro9. CF reports completion versus movie-open/decode failure.
   Cross-module calls: Movie_Open [movie/runtime/playback], Movie_AdvanceFrame [movie/runtime/playback],
   Movie_Close [movie/runtime/playback], UiFrame_FlushInputAndResetPendingTicks [ui/controls/layout],
   Movie_GetFrameDimensions [movie/runtime/playback].
*/
bool __thandor_cf_preserve_eax_ecx_edx Game_PlayIntroMovies(void)

{
  dword frameHeightSnapshot;
  dword playbackRateHz;
  uint quarterFrameHeight;
  int frameAdvanceBudget;
  bool accessFailed;
  MovieFrameDimensionsEdxEax8 frameDimensions;
  MovieOpenEaxCf5 openResult;
  MovieAdvanceFrameEaxCf5 firstFrameResult;
  MovieAdvanceFrameEaxCf5 advanceResult;
  KeyboardEventEaxEdxCf9 keyEvent;
  CommandLineFindOptionEbxCf5 noIntroOption;
  GraphicsCursorInputEventRegsCf21 cursorEvent;
  
    {
    const char *debugMovie = getenv("OPEN_THANDOR_MOVIE");
    if ((debugMovie != NULL) && (debugMovie[0] != 0)) {
      DebugMovie_Run(debugMovie);
    }
  }
  accessFailed = (*g_GraphicsFramebufferBeginAccess)();
  if (!accessFailed) {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  }
  accessFailed = (*g_GraphicsFramebufferBeginAccess)();
  if (!accessFailed) {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  }
  noIntroOption = (*g_CommandLineFindOption)(8,s_NOINTRO_00573064);
  if (noIntroOption.carry) {
    while( true ) {
      openResult = Movie_Open(1,(word *)u_flm_intro0_flm_00573046);
      if (openResult.carry) break;
      firstFrameResult = Movie_AdvanceFrame();
      if (firstFrameResult.carry) {
        Movie_Close();
        return true;
      }
      g_IntroMoviePendingTicks = 0;
      UiFrame_FlushInputAndResetPendingTicks();
      playbackRateHz = openResult.playbackRateHzEcx; /* PUSH ECX: rate left by Movie_Open (AdvanceFrame preserves ECX) */
      (*g_TimerRegisterPeriodic)(playbackRateHz,IntroMovie_TimerTick);
      while( true ) {
        (*g_Win32PumpMessages)();
        keyEvent = (*g_KeyboardReadEvent)();
        if (!keyEvent.carry) break;
        cursorEvent = (*g_GraphicsCursorConsumeEvent)();
        if ((!cursorEvent.carry) && (3 < cursorEvent.eventCode)) goto GameIntroMovies_StopCurrentPlayback;
        if (g_IntroMoviePendingTicks != 0) {
          frameAdvanceBudget = 3;
          do {
            advanceResult = Movie_AdvanceFrame();
            frameHeightSnapshot = g_FramebufferHeight;
            if (advanceResult.carry) goto GameIntroMovies_StopCurrentPlayback;
            g_IntroMoviePendingTicks = g_IntroMoviePendingTicks - 1;
          } while ((g_IntroMoviePendingTicks != 0) && (frameAdvanceBudget = frameAdvanceBudget + -1, frameAdvanceBudget != 0));
          quarterFrameHeight = g_FramebufferHeight >> 2;
          accessFailed = (*g_GraphicsFramebufferBeginAccess)();
          if (accessFailed) goto GameIntroMovies_StopCurrentPlayback;
          frameDimensions = Movie_GetFrameDimensions();
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (g_FramebufferHeight,g_FramebufferWidth,0,0,
                     ((int)((frameHeightSnapshot - quarterFrameHeight) - (int)(frameDimensions >> 0x20)) >> 1) + (frameHeightSnapshot >> 3),
                     (int)(g_FramebufferWidth - (int)frameDimensions) >> 1,0,
                     (GraphicsTextureSourceAsset *)firstFrameResult.eax,g_FramebufferAccess);
          (*g_GraphicsFramebufferEndAccess)();
          (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
        }
      }
      if (keyEvent.eventCode == 0x10000) {
        u_flm_intro0_flm_00573046[9] = L'8';
      }
GameIntroMovies_StopCurrentPlayback:
      (*g_TimerUnregisterPeriodic)(IntroMovie_TimerTick);
      Movie_Close();
      u_flm_intro0_flm_00573046[9] = u_flm_intro0_flm_00573046[9] + L'\x01';
    }
  }
  return false;
}


/* Address: 0x00573DB0.
   Ownership: platform/bootstrap/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Resolves the
   bootstrap API table.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx DynAPI_Bootstrap(void)

{
  /* EAX at the table end: the last resolved procedure (the table is never empty; incoming EAX otherwise) */
  void **resolvedProcedure = (void **)0x0;
  HINSTANCE hModule;
  DynamicApiBinding *bindingCursor;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 loadFailedResult;
  StatusValueEaxCf5 moduleUnavailableResult;
  StatusValueEaxCf5 procedureMissingResult;
  void **lpProcName;
  char *moduleName;
  dword moduleSlotIndex;
  
  bindingCursor = g_BootstrapApiBindings;
  do {
    if (bindingCursor->destination == (void **)0x0) {
      successResult.carry = false;
      successResult.valueOrError = (dword)resolvedProcedure;
      return successResult;
    }
    lpProcName = bindingCursor->destination;
    hModule = GetModuleHandleA(bindingCursor->moduleName);
    if (hModule == (HMODULE)0x0) {
      if (bindingCursor->destination == (void **)dynapi_9) {
        Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)bindingCursor->moduleName);
        moduleUnavailableResult.carry = true;
        moduleUnavailableResult.valueOrError = 0xf;
        return moduleUnavailableResult;
      }
      hModule = (HINSTANCE)
                ((BootstrapLoadLibraryAProc)g_BootstrapApiBindings[0].destination)(bindingCursor->moduleName);
      moduleSlotIndex = g_DynamicModuleCount;
      if (hModule == (HINSTANCE)0x0) {
        Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)bindingCursor->moduleName);
        loadFailedResult.carry = true;
        loadFailedResult.valueOrError = 0x11;
        return loadFailedResult;
      }
      g_DynamicModuleCount = g_DynamicModuleCount + 1;
      moduleName = bindingCursor->moduleName;
      g_DynamicModules[moduleSlotIndex].module = hModule;
      g_DynamicModules[moduleSlotIndex].name = moduleName;
      lpProcName = bindingCursor->destination;
    }
    resolvedProcedure = (void **)GetProcAddress(hModule,(LPCSTR)lpProcName);
    if (resolvedProcedure == (void **)0x0) {
      Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)bindingCursor->destination);
      Text_CopyNarrowToUtf16Cf(0x100,g_FatalErrorDetail1Utf16,(byte *)bindingCursor->moduleName);
      procedureMissingResult.carry = true;
      procedureMissingResult.valueOrError = 0x10;
      return procedureMissingResult;
    }
    bindingCursor->destination = resolvedProcedure;
    bindingCursor = bindingCursor + 1;
  } while( true );
}


/* Address: 0x00586110.
   Ownership: platform/bootstrap/runtime.
   Purpose: Scans CommandLineState.optionBuffer as consecutive NUL-terminated strings, bounded by the 0x100-byte
   buffer end. Compares exactly length bytes, so the match is a case-sensitive prefix and does not require the
   stored entry to end at length. CF clear means found and EBX points to the matching stored option. CF set means
   not found. EAX is preserved and is not a scalar result.
*/
CommandLineFindOptionEbxCf5 __thandor_ebx_cf_preserve_eax_ecx_edx
CommandLine_FindOption(CommandLineOptionLengthBytes length,char *option)

{
  dword compareBytesRemaining;
  int optionBufferCapacityRemaining;
  char *compareOrScanCursor;
  char *optionBufferCursor;
  char *storedOptionCompareCursor;
  bool comparedBytesEqual;
  CommandLineFindOptionEbxCf5 foundResult;
  CommandLineFindOptionEbxCf5 notFoundResult;
  char currentOptionBufferByte;
  
  optionBufferCursor = g_CommandLine.optionBuffer;
  do {
    if (*optionBufferCursor == '\0') {
      notFoundResult.carry = true;
      notFoundResult.ebx = (byte *)0; /* EBX not written; all callers read it only with CF clear */
      return notFoundResult;
    }
    comparedBytesEqual = false;
    compareBytesRemaining = length;
    compareOrScanCursor = option;
    storedOptionCompareCursor = optionBufferCursor;
    do {
      if (compareBytesRemaining == 0) break;
      compareBytesRemaining = compareBytesRemaining - 1;
      comparedBytesEqual = *compareOrScanCursor == *storedOptionCompareCursor;
      compareOrScanCursor = compareOrScanCursor + 1;
      storedOptionCompareCursor = storedOptionCompareCursor + 1;
    } while (comparedBytesEqual);
    if (comparedBytesEqual) {
      foundResult.carry = false;
      foundResult.ebx = (byte *)optionBufferCursor;
      return foundResult;
    }
    optionBufferCapacityRemaining = (int)sz_MainWindowTitle - (int)optionBufferCursor;
    compareOrScanCursor = optionBufferCursor;
    do {
      optionBufferCursor = compareOrScanCursor;
      if (optionBufferCapacityRemaining == 0) break;
      optionBufferCapacityRemaining = optionBufferCapacityRemaining + -1;
      optionBufferCursor = compareOrScanCursor + 1;
      currentOptionBufferByte = *compareOrScanCursor;
      compareOrScanCursor = optionBufferCursor;
    } while (currentOptionBufferByte != '\0');
  } while( true );
}


/* Address: 0x00586170.
   Ownership: platform/bootstrap/runtime.
   Purpose: Installs CommandLine_FindOption, reads GetCommandLineA, stores a quote-stripped executable path, up to
   three positional arguments, and slash/dash options. Positional text and unquoted option text are uppercased only
   for ASCII a-z. Quoted positional delimiters are removed; quoted segments inside options are retained and copied
   verbatim. Extra positional arguments are skipped. The fixed 256-byte buffers have no explicit bounds checks.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
void __thandor_void_preserve_eax_ecx_edx CommandLine_Parse(void)

{
  byte *commandLineNext;
  CommandLineArgumentMirrorState500 *pathWriteNext;
  byte currentChar;
  byte *commandLineCursor;
  char *textWriteCursor;
  char *optionWriteNext;
  CommandLineArgumentMirrorState500 *pathWriteCursor;
  char *argumentWriteCursor;
  
  g_CommandLineFindOption = CommandLine_FindOption;
  commandLineCursor = (byte *)GetCommandLineA();
  pathWriteNext = &g_CommandLine;
  if (*commandLineCursor == 0x22) {
    commandLineCursor = commandLineCursor + 1;
    do {
      pathWriteCursor = pathWriteNext;
      currentChar = *commandLineCursor;
      pathWriteCursor->executablePath[0] = currentChar;
      commandLineCursor = commandLineCursor + 1;
      if (currentChar == 0) {
        g_CommandLine.executablePath[0] = '\0';
        goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
      }
      pathWriteNext = (CommandLineArgumentMirrorState500 *)(pathWriteCursor->executablePath + 1);
    } while (currentChar != 0x22);
    pathWriteCursor->executablePath[0] = '\0';
    optionWriteNext = g_CommandLine.optionBuffer;
  }
  else {
    do {
      pathWriteCursor = pathWriteNext;
      currentChar = *commandLineCursor;
      pathWriteCursor->executablePath[0] = currentChar;
      commandLineCursor = commandLineCursor + 1;
      if (currentChar == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
      pathWriteNext = (CommandLineArgumentMirrorState500 *)(pathWriteCursor->executablePath + 1);
    } while (currentChar != 0x20);
    pathWriteCursor->executablePath[0] = '\0';
    optionWriteNext = g_CommandLine.optionBuffer;
  }
  /* options and positional arguments, until the terminating NUL */
  for (;;) {
    currentChar = *commandLineCursor;
    commandLineCursor = commandLineCursor + 1;
    if ((currentChar == 0x2f) || (currentChar == 0x2d)) {
      /* '/' or '-' option: copied uppercased up to the next space; quoted parts verbatim */
      do {
        while( true ) {
          textWriteCursor = optionWriteNext;
          currentChar = *commandLineCursor;
          if ((0x60 < currentChar) && (currentChar < 0x7b)) {
            currentChar = currentChar - 0x20;
          }
          *textWriteCursor = currentChar;
          commandLineCursor = commandLineCursor + 1;
          optionWriteNext = textWriteCursor + 1;
          if (currentChar == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
          if (currentChar != 0x22) break;
          do {
            currentChar = *commandLineCursor;
            *optionWriteNext = currentChar;
            commandLineCursor = commandLineCursor + 1;
            optionWriteNext = optionWriteNext + 1;
            if (currentChar == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
          } while (currentChar != 0x22);
        }
      } while (currentChar != 0x20);
      *textWriteCursor = 0;
      continue;
    }
    if (currentChar == 0) break;
    if (currentChar == 0x20) continue;
    if (currentChar == 0x22) {
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
          commandLineCursor = commandLineCursor + 1;
          if (currentChar == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
        } while (currentChar != 0x22);
        continue;
      }
      do {
        argumentWriteCursor = textWriteCursor;
        commandLineCursor = commandLineNext;
        currentChar = *commandLineCursor;
        if ((0x60 < currentChar) && (currentChar < 0x7b)) {
          currentChar = currentChar - 0x20;
        }
        *argumentWriteCursor = currentChar;
        if (currentChar == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
        commandLineNext = commandLineCursor + 1;
        textWriteCursor = argumentWriteCursor + 1;
      } while (currentChar != 0x22);
      *argumentWriteCursor = 0;
      continue;
    }
    /* unquoted positional argument */
    if ((0x60 < currentChar) && (currentChar < 0x7b)) {
      currentChar = currentChar - 0x20;
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
          commandLineCursor = commandLineCursor + 1;
          if (currentChar == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
        } while (currentChar != 0x20);
        continue;
      }
      textWriteCursor = g_CommandLine.argument3 + 1;
      g_CommandLine.argument3[0] = currentChar;
    }
    do {
      argumentWriteCursor = textWriteCursor;
      commandLineCursor = commandLineNext;
      currentChar = *commandLineCursor;
      if ((0x60 < currentChar) && (currentChar < 0x7b)) {
        currentChar = currentChar - 0x20;
      }
      *argumentWriteCursor = currentChar;
      if (currentChar == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
      commandLineNext = commandLineCursor + 1;
      textWriteCursor = argumentWriteCursor + 1;
    } while (currentChar != 0x20);
    *argumentWriteCursor = 0;
  }
CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn:
  Text_CopyNarrowToUtf16Cf
            (0x200,g_CommandLineWideArguments.argument1,(byte *)g_CommandLine.argument1);
  Text_CopyNarrowToUtf16Cf
            (0x200,g_CommandLineWideArguments.argument2,(byte *)g_CommandLine.argument2);
  Text_CopyNarrowToUtf16Cf
            (0x200,g_CommandLineWideArguments.argument3,(byte *)g_CommandLine.argument3);
  return;
}

