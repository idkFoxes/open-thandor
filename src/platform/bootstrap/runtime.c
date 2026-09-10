#include <thandor/platform/bootstrap/runtime.h>

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
  ATOM AVar1;
  HANDLE pvVar2;
  HWND pHVar3;
  undefined2 extraout_var;
  int nHeight;
  int nWidth;
  dword arg1;
  dword arg0;
  undefined1 uVar4;
  HMENU hMenu;
  HINSTANCE hInstance;
  dword arg2;
  LPVOID lpParam;
  dword arg3;
  
  g_hInstance = GetModuleHandleA((LPCSTR)0x0);
  pvVar2 = GetCurrentProcess();
  SetPriorityClass(pvVar2,0x100);
  pvVar2 = GetCurrentThread();
  SetThreadPriority(pvVar2,0);
  CommandLine_Parse();
  pHVar3 = FindWindowA(sz_MainWindowClass,(LPCSTR)0x0);
  if (pHVar3 == (HWND)0x0) {
    g_MainWindowClassInstanceHandle = (undefined *)g_hInstance;
    g_MainWindowClassIconHandle = (undefined *)LoadIconA(g_hInstance,(LPCSTR)0x1);
    g_MainWindowClassCursorHandle =
         (undefined *)LoadCursorA((HINSTANCE)0x0,&k_LowAddressLiteral00007F00);
    AVar1 = RegisterClassA((WNDCLASSA *)&g_MainMessage.pointY);
    if (CONCAT22(extraout_var,AVar1) != 0) {
      lpParam = (LPVOID)0x0;
      hMenu = (HMENU)0x0;
      pHVar3 = (HWND)0x0;
      hInstance = g_hInstance;
      nHeight = GetSystemMetrics(1);
      nWidth = GetSystemMetrics(0);
      g_MainWindow = CreateWindowExA(8,sz_MainWindowClass,sz_MainWindowTitle,0x80080000,0,0,nWidth,
                                     nHeight,pHVar3,hMenu,hInstance,lpParam);
      if (g_MainWindow != (HWND)0x0) {
        ShowWindow(g_MainWindow,1);
        UpdateWindow(g_MainWindow);
        ArenaHeap_Init();
        FileSystem_Init();
        Locale_Init();
        ErrorSystem_Init();
        uVar4 = 0;
        if (g_CpuFeatureFlags == 0) {
          uVar4 = 1;
          (*g_FatalErrorPrimaryDispatchCf)();
        }
        DynAPI_Bootstrap();
        (*g_FatalErrorPrimaryDispatchCf)();
        TimerSystem_Init();
        (*g_FatalErrorPrimaryDispatchCf)();
        Graphics_Init();
        (*g_FatalErrorPrimaryDispatchCf)();
        DirectInputMouse_Init();
        (*g_FatalErrorPrimaryDispatchCf)();
        DirectSound_Init();
        if ((bool)uVar4) {
          CommandLine_FindOption(6,s_SOUND_00582f28);
          if (!(bool)uVar4) {
            (*g_FatalErrorPrimaryDispatchCf)();
          }
        }
        Network_Init();
        (*g_FatalErrorPrimaryDispatchCf)();
        PersistentSettings_Load();
        arg3 = 0x280;
        arg2 = 0x1e0;
        arg1 = PersistentSettings_ReadDword(0x10,0xc);
        arg0 = PersistentSettings_ReadDword(0,0);
        if (g_GraphicsAdapterCount <= arg0) {
          arg0 = 0;
        }
        (*g_GraphicsDisplayModeHook)(arg0,arg1,arg2,arg3);
        (*g_FatalErrorPrimaryDispatchCf)();
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
   Ownership: platform/bootstrap/runtime.
   Purpose: Clears the 0x100-byte auxiliary state and the first 0x3A00 bytes of the faction image, initializes
   eight 0x740-byte runtime records with verified defaults, allocates a fresh 0x38000-byte stat table, replaces the
   prior allocation, zeroes it, and writes a terminal 0xFFFFFFFF dword.
   [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Initializes faction economy defaults: Xenite/Tritium storage
   limits are 0xFA0 Q4; baseline Energy supply and initial Energy generation-capacity ceiling are both 0x280 Q4
   (40). These are separate resource/fuel/utility domains.
*/
void GameData_ResetDefaults(void)

{
  FactionCapabilityFlags *pFVar1;
  void *memory;
  undefined4 *puVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  GameFactionRuntimeImage *pGVar6;
  dword *pdVar7;
  bool bVar8;
  
  iVar3 = 0x40;
  pdVar7 = g_GameDataAuxState.pairPressureMatrix8x8;
  for (; iVar3 != 0; iVar3 = iVar3 + -1) {
    *pdVar7 = 0;
    pdVar7 = pdVar7 + 1;
  }
  pGVar6 = &g_GameFactionRuntimeImage;
  for (iVar3 = 0xe80; iVar3 != 0; iVar3 = iVar3 + -1) {
    pGVar6->records[0].xeniteCurrentQ4 = 0;
    pGVar6 = (GameFactionRuntimeImage *)&pGVar6->records[0].xeniteStorageLimitQ4;
  }
  pGVar6 = &g_GameFactionRuntimeImage;
  iVar3 = 8;
  uVar5 = 1;
  uVar4 = 0x1111111f;
  do {
    pFVar1 = &pGVar6->records[0].capabilityFlags;
    *pFVar1 = *pFVar1 | uVar5;
    pdVar7 = pGVar6->records[0].technologyMasks256Bits;
    *pdVar7 = *pdVar7 | 1;
    pFVar1 = &pGVar6->records[0].capabilityFlags;
    *pFVar1 = *pFVar1 | 1;
    pGVar6->records[0].packedRelationStates = uVar4;
    pGVar6->records[0].relationCapabilityState = 0;
    pGVar6->records[0].primaryAnchorYQ12 = -0xc000;
    pGVar6->records[0].primaryAnchorXQ12 = 0;
    pGVar6->records[0].secondaryAnchorYQ12 = -0xc000;
    pGVar6->records[0].secondaryAnchorXQ12 = 0;
    pGVar6->records[0].relationTransitionTick = 0x11;
    pGVar6->records[0].energyGenerationCapacityQ4 = 0x280;
    pGVar6->records[0].baselineEnergySupplyQ4 = 0x280;
    pGVar6->records[0].xeniteStorageLimitQ4 = 4000;
    pGVar6->records[0].tritiumStorageLimitQ4 = 4000;
    pGVar6->records[0].terrainContributionScaleQ8 = 0x100;
    uVar5 = uVar5 * 2;
    uVar4 = uVar4 << 4 | uVar4 >> 0x1c;
    bVar8 = (GameFactionRuntimeImage *)0xfffff8bf < pGVar6;
    pGVar6 = (GameFactionRuntimeImage *)(pGVar6->records + 1);
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  puVar2 = (*g_MemoryApi.alloc)(0x38000);
  memory = g_GameStatTableImage;
  if (!bVar8) {
    LOCK();
    UNLOCK();
    g_GameStatTableImage = puVar2;
    (*g_MemoryApi.free)(memory);
    for (iVar3 = 0xe000; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar2 = 0;
      puVar2 = puVar2 + 1;
    }
    puVar2[-1] = 0xffffffff;
    g_GameFactionRuntimeImage.tail.periodicClockTick = 0;
  }
  return;
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
undefined8 GameData_LoadExternalTables(void)

{
  void *memory;
  undefined4 in_EAX;
  void *pvVar1;
  dword *pdVar2;
  undefined4 in_ECX;
  int iVar3;
  undefined4 in_EDX;
  dword *pdVar4;
  undefined4 unaff_EDI;
  dword *pdVar5;
  undefined1 uVar6;
  
  uVar6 = false;
  iVar3 = 0x40;
  pdVar2 = g_GameDataAuxState.pairPressureMatrix8x8;
  for (; iVar3 != 0; iVar3 = iVar3 + -1) {
    *pdVar2 = 0;
    pdVar2 = pdVar2 + 1;
  }
  Package_LoadEntryIntoBuffer
            (0x3a20,(byte *)&g_GameFactionRuntimeImage,(word *)u_daten_hex_0050e054);
  if (!(bool)uVar6) {
    pvVar1 = Package_LoadEntry((word *)u_stat_hex_0050e082);
    memory = g_GameStatTableImage;
    if (!(bool)uVar6) {
      LOCK();
      UNLOCK();
      g_GameStatTableImage = pvVar1;
      (*g_MemoryApi.free)(memory);
      pdVar2 = Package_LoadEntry((word *)u_oldunit_hex_0050e094);
      if ((bool)uVar6) {
        pdVar2 = g_OldUnitPrimaryTable;
        for (iVar3 = 0x1000; iVar3 != 0; iVar3 = iVar3 + -1) {
          *pdVar2 = 0;
          pdVar2 = pdVar2 + 1;
        }
        pdVar2 = g_OldUnitSecondaryTable;
        for (iVar3 = 0x40; iVar3 != 0; iVar3 = iVar3 + -1) {
          *pdVar2 = 0;
          pdVar2 = pdVar2 + 1;
        }
        g_OldUnitRecordCount = 0;
      }
      else {
        g_OldUnitRecordCount = *pdVar2;
        pdVar5 = g_OldUnitPrimaryTable;
        pdVar4 = pdVar2;
        for (iVar3 = 0x1000; pdVar4 = pdVar4 + 1, iVar3 != 0; iVar3 = iVar3 + -1) {
          *pdVar5 = *pdVar4;
          pdVar5 = pdVar5 + 1;
        }
        pdVar5 = g_OldUnitSecondaryTable;
        for (iVar3 = 0x40; iVar3 != 0; iVar3 = iVar3 + -1) {
          *pdVar5 = *pdVar4;
          pdVar4 = pdVar4 + 1;
          pdVar5 = pdVar5 + 1;
        }
        Resource_Release(pdVar2);
      }
      return CONCAT44(unaff_EDI,in_ECX);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00573BC0.
   Ownership: platform/bootstrap/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Returns resolved
   function pointer in EAX.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
void * DynAPI_Resolve(void **destination,HINSTANCE module,char *procedureName)

{
  FARPROC resolvedProcedure;
  dword modulesRemaining;
  DynamicModuleEntry *moduleEntryCursor;
  
  Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)procedureName);
  resolvedProcedure = GetProcAddress(module,procedureName);
  if (resolvedProcedure != (FARPROC)0x0) {
    *destination = resolvedProcedure;
    return resolvedProcedure;
  }
  moduleEntryCursor = g_DynamicModules;
  g_FatalErrorDetail1Utf16[0] = 0;
  modulesRemaining = g_DynamicModuleCount;
  while( true ) {
    if (modulesRemaining == 0) {
      g_FatalErrorDetail1Utf16[0] = 0;
      return (void *)0x10;
    }
    if (module == moduleEntryCursor->module) break;
    moduleEntryCursor = moduleEntryCursor + 1;
    modulesRemaining = modulesRemaining - 1;
  }
  Text_CopyNarrowToUtf16Cf(0x100,g_FatalErrorDetail1Utf16,(byte *)moduleEntryCursor->name);
  return (void *)0x10;
}

/* Address: 0x00573C50.
   Ownership: platform/bootstrap/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Returns HMODULE in
   EAX.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
HINSTANCE DynDLL_Load(char *moduleName)

{
  HINSTANCE loadedModule;
  dword moduleSlotIndex;
  
  Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)moduleName);
  if ((g_BootstrapApiBindings[0].destination != (void **)dynapi_9) && (g_DynamicModuleCount < 0x10))
  {
    loadedModule = (HINSTANCE)(*(code *)g_BootstrapApiBindings[0].destination)(moduleName);
    moduleSlotIndex = g_DynamicModuleCount;
    if (loadedModule != (HINSTANCE)0x0) {
      g_DynamicModules[g_DynamicModuleCount].module = loadedModule;
      g_DynamicModules[moduleSlotIndex].name = moduleName;
      g_DynamicModuleCount = g_DynamicModuleCount + 1;
      return loadedModule;
    }
  }
  return (HINSTANCE)0x11;
}

/* Address: 0x00573CD0.
   Ownership: platform/bootstrap/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
dword DynDLL_Unload(char *moduleName)

{
  dword dVar1;
  DynamicModuleEntry *moduleEntryCursor;
  
  moduleEntryCursor = g_DynamicModules;
  dVar1 = g_DynamicModuleCount;
  do {
    if (dVar1 == 0) {
DynDLL_ReportModuleNotLoaded:
      Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)moduleName);
      return 0xf;
    }
    if (moduleName == moduleEntryCursor->name) {
      dVar1 = (*(code *)g_BootstrapApiBindings[1].destination)(moduleEntryCursor->module);
      if (dVar1 != 0) {
        return dVar1;
      }
      goto DynDLL_ReportModuleNotLoaded;
    }
    moduleEntryCursor = moduleEntryCursor + 1;
    dVar1 = dVar1 - 1;
  } while( true );
}

/* Address: 0x00573D40.
   Ownership: platform/bootstrap/runtime.
   Purpose: Handles bootstrap api resolve binding by destination.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
undefined8 BootstrapApi_ResolveBindingByDestination(void **param_1)

{
  void *pvVar1;
  dword dVar2;
  undefined4 in_EDX;
  DynamicApiBinding *pDVar3;
  
  pDVar3 = g_BootstrapApiBindings;
  dVar2 = g_DynamicModuleCount;
  do {
    if (dVar2 == 0) {
LAB_00573d6a:
      return CONCAT44(in_EDX,0xf);
    }
    if (param_1 == pDVar3->destination) {
      Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)param_1);
      pvVar1 = (void *)(*(code *)g_BootstrapApiBindings[0].destination)(param_1,pDVar3);
      if (pvVar1 != (void *)0x0) {
        *param_1 = pvVar1;
        return CONCAT44(in_EDX,0xf);
      }
      goto LAB_00573d6a;
    }
    pDVar3 = pDVar3 + 1;
    dVar2 = dVar2 - 1;
  } while( true );
}

/* Address: 0x00573EB0.
   Ownership: platform/bootstrap/runtime.
   Purpose: Releases all cached dynamic modules.
*/
void __cdecl DynDLL_UnloadAll(void)

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
      (*(code *)g_BootstrapApiBindings[1].destination)(loadedModule);
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
LRESULT MainWindowProc(HWND hwnd,Win32WindowMessageId message,WPARAM wParam,LPARAM lParam)

{
  ushort uVar1;
  HANDLE pvVar2;
  LRESULT LVar3;
  
  if ((message == 2) || (message == 0x10)) {
    g_WindowDestroyDepth = g_WindowDestroyDepth + 1;
  }
  else if (message == 0x1c) {
    g_AppActive = wParam;
    if (wParam == 0) {
      pvVar2 = GetCurrentProcess();
      SetPriorityClass(pvVar2,0x20);
      if (g_MouseDevice != (IDirectInputDeviceA *)0x0) {
        (*g_MouseDevice->lpVtbl->Unacquire)(g_MouseDevice);
      }
      if (g_WindowDestroyDepth == 0) {
        (*(code *)g_GraphicsBackendRefreshActiveAdapterCf)();
      }
    }
    else {
      pvVar2 = GetCurrentProcess();
      SetPriorityClass(pvVar2,0x100);
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
        uVar1 = GetKeyState(0x90);
        if ((uVar1 & 1) != 0) {
          g_KeyboardStateMask = g_KeyboardStateMask | 0x10000;
        }
        uVar1 = GetKeyState(0x91);
        if ((uVar1 & 1) != 0) {
          g_KeyboardStateMask = g_KeyboardStateMask | 0x20000;
        }
        uVar1 = GetKeyState(0x14);
        if ((uVar1 & 1) != 0) {
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
      LVar3 = DefWindowProcA(hwnd,message,wParam,lParam);
      return LVar3;
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
  dword arg3;
  dword arg2;
  dword arg1;
  dword arg0;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar1;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 uVar2;
  
  (*g_GraphicsCursorSetFrame)(0);
  (*g_FatalErrorPrimaryDispatchCf)();
  GameRuntime_InitializeSpatialAudioAndRenderingCf();
  (*g_FatalErrorPrimaryDispatchCf)();
  Game_LoadCoreAssets();
  (*g_FatalErrorPrimaryDispatchCf)();
  Game_PlayIntroMovies();
  (*g_FatalErrorPrimaryDispatchCf)();
  PersistentSettings_Load();
  arg3 = PersistentSettings_ReadDword(0x280,4);
  arg2 = PersistentSettings_ReadDword(0x1e0,8);
  arg1 = PersistentSettings_ReadDword(0x10,0xc);
  if (((arg3 != 0x280) || (arg2 != 0x1e0)) ||
     (uVar1 = extraout_ECX, uVar2 = extraout_EDX, arg1 != 0x10)) {
    arg0 = PersistentSettings_ReadDword(0,0);
    if (g_GraphicsAdapterCount <= arg0) {
      arg0 = 0;
    }
    (*g_GraphicsDisplayModeHook)(arg0,arg1,arg2,arg3);
    (*g_FatalErrorPrimaryDispatchCf)();
    PersistentSettings_WriteDword(g_ActiveGraphicsAdapterIndex,0);
    uVar1 = extraout_ECX_00;
    uVar2 = extraout_EDX_00;
  }
  Frontend_MainLoop(uVar1,uVar2,1);
  (*g_FatalErrorPrimaryDispatchCf)();
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
void __cdecl GameRuntime_InitializeSpatialAudioAndRenderingCf(void)

{
  undefined1 in_CF;
  
  SpatialSoundPool_Init();
  if (!(bool)in_CF) {
    TerrainByteClampLookup_Initialize();
    if (!(bool)in_CF) {
      GraphicsIntensityClampTable_InitializeCf();
      if (!(bool)in_CF) {
        SoftwareRenderer_InstallDisplayModeHook();
        if (!(bool)in_CF) {
          GraphicsPrimitiveQueue_AllocateGlobalPool(0xa000);
        }
      }
    }
  }
  return;
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
  wchar_t wVar1;
  int iVar2;
  dword dVar3;
  DirectSoundVoiceSet *pDVar4;
  void *pvVar5;
  word *pwVar6;
  AudioMixerGainQ15 AVar7;
  MovieAudioGainQ15 MVar8;
  MovieAudioGainQ15 MVar9;
  dword extraout_EAX;
  FncModuleHeader *pFVar10;
  GraphicsTextureSourceAsset *pGVar11;
  RecentTextHistorySlot *pRVar12;
  dword *pdVar13;
  RomRegistrySlot *pRVar14;
  FrontendSessionDiscoveryRecordB0 **ppFVar15;
  FrontendSessionDiscoveryRecordB0 *pFVar16;
  undefined *puVar17;
  float *pfVar18;
  SelectionPlayerRuntimeBlock *pSVar19;
  FrontendPlayerRuntimeRecord *pFVar20;
  byte *pbVar21;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 extraout_ECX_04;
  undefined4 extraout_ECX_05;
  uint uVar22;
  uint extraout_ECX_06;
  uint extraout_ECX_07;
  uint extraout_ECX_08;
  uint extraout_ECX_09;
  dword arg0;
  undefined4 extraout_EDX;
  void *allocation;
  undefined4 extraout_EDX_00;
  void *allocation_00;
  undefined4 extraout_EDX_01;
  void *allocation_01;
  undefined4 extraout_EDX_02;
  void *allocation_02;
  undefined4 extraout_EDX_03;
  void *allocation_03;
  undefined4 extraout_EDX_04;
  void *allocation_04;
  undefined4 extraout_EDX_05;
  void *allocation_05;
  undefined4 extraout_EDX_06;
  dword arg1;
  FrontendPlayerRuntimeRecord **playerRuntimePointerTableWriteCursor;
  undefined1 uVar23;
  bool bVar24;
  undefined8 uVar25;
  
  if ((g_MemoryApi.alloc == ArenaHeap_Alloc) &&
     (iVar2 = (*(code *)g_BootstrapApiBindings[5].destination)
                        (0x80000002,s_Software_Planet4_Thandor_00572e20,0,0x20019,
                         &g_InstallRegistryKeyHandle), iVar2 == 0)) {
    iVar2 = (*(code *)g_BootstrapApiBindings[6].destination)
                      (g_InstallRegistryKeyHandle,&s_InstallRegistryValueNameCD,0,
                       &g_InstallRegistryValueType,&g_InstallRegistryValueDataA,
                       &g_InstallRegistryValueDataCapacityBytes);
    if ((iVar2 == 0) && (g_InstallRegistryValueType == 1)) {
      Text_CopyNarrowToUtf16Cf
                (0x200,(word *)&g_InstallDirectoryScratchUtf16,&g_InstallRegistryValueDataA);
      WidePath_CombineDirectoryAndLeaf
                (g_LooseMoviePathPrefix.codeUnits,(word *)u_Thandor_00572e10,
                 (word *)&g_InstallDirectoryScratchUtf16);
    }
    (*(code *)g_BootstrapApiBindings[7].destination)(g_InstallRegistryKeyHandle);
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
    uVar23 = g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[0] < 0x30;
  } while (!(bool)uVar23);
  dVar3 = Package_Mount((word *)u_daten_pck_00572e56);
  if (!(bool)uVar23) {
    g_DataPackageHandle = dVar3;
  }
  dVar3 = Package_Mount((word *)u_modelle_pck_00572e6a);
  if (!(bool)uVar23) {
    g_ModelPackageHandle = dVar3;
  }
  dVar3 = Package_Mount((word *)u_graphik_pck_00572e82);
  if (!(bool)uVar23) {
    g_GraphicsPackageHandle = dVar3;
  }
  dVar3 = Package_Mount((word *)u_sound_pck_00572e9a);
  if (!(bool)uVar23) {
    g_SoundPackageHandle = dVar3;
  }
  dVar3 = Package_Mount((word *)u_filme_pck_00572eae);
  if (!(bool)uVar23) {
    g_MoviePackageHandle = dVar3;
  }
  dVar3 = Package_Mount((word *)u_level_pck_00572ec2);
  if (!(bool)uVar23) {
    g_LevelPackageHandle = dVar3;
  }
  uVar25 = Resource_Load(extraout_ECX,extraout_EDX,(word *)u_sound_button0_sam_00572f06);
  if ((bool)uVar23) {
    return (dword)(SoundSampleAsset *)uVar25;
  }
  pDVar4 = (*g_SoundCreateSampleVoiceSet)((SoundSampleAsset *)uVar25);
  uVar25 = CONCAT44(allocation,pDVar4);
  if (!(bool)uVar23) {
    g_UiButtonSoundVoiceSets7[0] = (undefined *)Resource_Release(allocation);
    uVar25 = Resource_Load(extraout_ECX_00,extraout_EDX_00,(word *)u_sound_button1_sam_00572f2a);
    if ((bool)uVar23) {
      return (dword)(SoundSampleAsset *)uVar25;
    }
    pDVar4 = (*g_SoundCreateSampleVoiceSet)((SoundSampleAsset *)uVar25);
    uVar25 = CONCAT44(allocation_00,pDVar4);
    if (!(bool)uVar23) {
      g_UiButtonSoundVoiceSets7[1] = (undefined *)Resource_Release(allocation_00);
      uVar25 = Resource_Load(extraout_ECX_01,extraout_EDX_01,(word *)u_sound_button2_sam_00572f4e);
      if ((bool)uVar23) {
        return (dword)(SoundSampleAsset *)uVar25;
      }
      pDVar4 = (*g_SoundCreateSampleVoiceSet)((SoundSampleAsset *)uVar25);
      uVar25 = CONCAT44(allocation_01,pDVar4);
      if (!(bool)uVar23) {
        g_UiButtonSoundVoiceSets7[2] = (undefined *)Resource_Release(allocation_01);
        uVar25 = Resource_Load(extraout_ECX_02,extraout_EDX_02,(word *)u_sound_button3_sam_00572f72)
        ;
        if ((bool)uVar23) {
          return (dword)(SoundSampleAsset *)uVar25;
        }
        pDVar4 = (*g_SoundCreateSampleVoiceSet)((SoundSampleAsset *)uVar25);
        uVar25 = CONCAT44(allocation_02,pDVar4);
        if (!(bool)uVar23) {
          g_UiButtonSoundVoiceSets7[3] = (undefined *)Resource_Release(allocation_02);
          uVar25 = Resource_Load(extraout_ECX_03,extraout_EDX_03,
                                 (word *)u_sound_button4_sam_00572f96);
          if ((bool)uVar23) {
            return (dword)(SoundSampleAsset *)uVar25;
          }
          pDVar4 = (*g_SoundCreateSampleVoiceSet)((SoundSampleAsset *)uVar25);
          uVar25 = CONCAT44(allocation_03,pDVar4);
          if (!(bool)uVar23) {
            g_UiButtonSoundVoiceSets7[4] = (undefined *)Resource_Release(allocation_03);
            uVar25 = Resource_Load(extraout_ECX_04,extraout_EDX_04,
                                   (word *)u_sound_button5_sam_00572fba);
            if ((bool)uVar23) {
              return (dword)(SoundSampleAsset *)uVar25;
            }
            pDVar4 = (*g_SoundCreateSampleVoiceSet)((SoundSampleAsset *)uVar25);
            uVar25 = CONCAT44(allocation_04,pDVar4);
            if (!(bool)uVar23) {
              g_UiButtonSoundVoiceSets7[5] = (undefined *)Resource_Release(allocation_04);
              uVar25 = Resource_Load(extraout_ECX_05,extraout_EDX_05,
                                     (word *)u_sound_button6_sam_00572fde);
              if ((bool)uVar23) {
                return (dword)(SoundSampleAsset *)uVar25;
              }
              pDVar4 = (*g_SoundCreateSampleVoiceSet)((SoundSampleAsset *)uVar25);
              uVar25 = CONCAT44(allocation_05,pDVar4);
              if (!(bool)uVar23) {
                g_UiButtonSoundVoiceSets7[6] = (undefined *)Resource_Release(allocation_05);
                do {
                  do {
                    pvVar5 = (void *)(*g_FileSystemOpenCf)(0,(word *)(u_Dscreen00_pcx_00572e3a + 1))
                    ;
                    if ((bool)uVar23)
                    goto Game_LoadCoreAssets_BindDebugOverlayTextAndContinueRemainingAssetLoad;
                    u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + L'\x01';
                    (*g_FileSystemClose)(pvVar5);
                    wVar1 = u_Dscreen00_pcx_00572e3a[7];
                    uVar23 = (ushort)u_Dscreen00_pcx_00572e3a[8] < 0x39;
                  } while ((ushort)u_Dscreen00_pcx_00572e3a[8] < 0x3a);
                  u_Dscreen00_pcx_00572e3a[7] = u_Dscreen00_pcx_00572e3a[7] + L'\x01';
                  u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + L'\xfff6';
                  uVar23 = (ushort)u_Dscreen00_pcx_00572e3a[7] < 0x39;
                } while ((ushort)u_Dscreen00_pcx_00572e3a[7] < 0x3a);
                u_Dscreen00_pcx_00572e3a[7] = wVar1 + L'\xfff7';
Game_LoadCoreAssets_BindDebugOverlayTextAndContinueRemainingAssetLoad:
                uVar22 = 0x112;
                do {
                  pwVar6 = TextResource_Resolve(uVar22);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (0,g_FrontendDebugOverlayTextSlot00Utf16,pwVar6);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (1,g_FrontendDebugOverlayTextSlot01Utf16,(word *)uVar25);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (2,g_FrontendDebugOverlayTextSlot02Utf16,(word *)uVar25);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (3,g_FrontendDebugOverlayTextSlot03Utf16,(word *)uVar25);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (4,g_FrontendDebugOverlayTextSlot04Utf16,(word *)uVar25);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (5,g_FrontendDebugOverlayTextSlot05Utf16,(word *)uVar25);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (6,g_FrontendDebugOverlayTextSlot06Utf16,(word *)uVar25);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (7,g_FrontendDebugOverlayTextSlot07Utf16,(word *)uVar25);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (8,g_FrontendDebugOverlayTextSlot08Utf16,(word *)uVar25);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (9,g_FrontendDebugOverlayTextSlot09Utf16,(word *)uVar25);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (10,g_FrontendDebugOverlayTextSlot10Utf16,(word *)uVar25);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (0xb,g_FrontendDebugOverlayTextSlot11Utf16,(word *)uVar25);
                  uVar25 = RichTextCommandStream_PatchPayloadBySelector
                                     (0xc,g_FrontendDebugOverlayTextSlot12Utf16,(word *)uVar25);
                  RichTextCommandStream_PatchPayloadBySelector
                            (0xd,g_FrontendDebugOverlayTextSlot13Utf16,(word *)uVar25);
                  uVar23 = extraout_ECX_06 < 0x117;
                  uVar22 = extraout_ECX_06;
                } while (extraout_ECX_06 < 0x118);
                UiActionHandlers_SetPageCf
                          (0x10,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage10);
                UiActionHandlers_SetPageCf
                          (0x11,(UiActionHandlerPage *)&g_InGameUiCommandModeActionHandlers30);
                UiActionHandlers_SetPageCf
                          (0x12,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage12);
                UiActionHandlers_SetPageCf
                          (0x20,(UiActionHandlerPage *)&g_FrontendUiActionHandlersPage20);
                dVar3 = TextResourcePage_Load(0xff,(word *)u_texte_neterror_str_0050f104);
                if ((bool)uVar23) {
                  return dVar3;
                }
                dVar3 = TextResourcePage_Load(0x18,(word *)u_texte_help_str_00563170);
                if ((bool)uVar23) {
                  return dVar3;
                }
                dVar3 = TextResourcePage_Load(0x20,(word *)u_texte_hilfe_str_00545b34);
                if ((bool)uVar23) {
                  return dVar3;
                }
                dVar3 = TextResourcePage_Load(0x21,(word *)u_texte_menue_str_00545ba0);
                if ((bool)uVar23) {
                  return dVar3;
                }
                dVar3 = TextResourcePage_Load(0x30,(word *)u_texte_techno_str_0050dec4);
                if ((bool)uVar23) {
                  return dVar3;
                }
                dVar3 = TextResourcePage_Load(0x22,(word *)u_texte_level_str_00545bc0);
                if ((bool)uVar23) {
                  return dVar3;
                }
                dVar3 = TextResourcePage_Load(0x23,(word *)u_texte_inhalt_str_00545be0);
                if ((bool)uVar23) {
                  return dVar3;
                }
                dVar3 = TextResourcePage_Load(0x24,(word *)u_texte_tastatur_str_005631b8);
                if ((bool)uVar23) {
                  return dVar3;
                }
                pwVar6 = TextResource_Resolve(0x2402);
                RichTextCommandStream_BindTextureSource(g_CursorSourceAsset,pwVar6);
                uVar22 = PersistentSettings_ReadDword(3,0x20);
                AVar7 = 0;
                if ((uVar22 & 1) != 0) {
                  AVar7 = PersistentSettings_ReadDword(0x8000,0x24);
                  uVar22 = extraout_ECX_07;
                }
                MVar8 = 0;
                g_UiSoundGainQ15 = AVar7;
                g_SoundEffectsGainQ15 = AVar7;
                if ((uVar22 & 1) != 0) {
                  MVar8 = PersistentSettings_ReadDword(0x8000,0x28);
                  uVar22 = extraout_ECX_08;
                }
                MVar9 = 0;
                g_MovieDefaultAudioGainQ15 = MVar8;
                if ((uVar22 & 1) != 0) {
                  MVar9 = PersistentSettings_ReadDword(0x8000,0x4c);
                  uVar22 = extraout_ECX_09;
                }
                g_ReverseStereoMask = 0;
                uVar23 = 0;
                if ((uVar22 & 4) != 0) {
                  g_ReverseStereoMask = 0xffffffff;
                }
                g_MovieAlternateAudioGainQ15 = MVar9;
                g_ModelLodDepthThresholdQ8 = PersistentSettings_ReadDword(g_ReverseStereoMask,0x34);
                AiRuntime_InitWorkspace();
                if ((bool)uVar23) {
                  return extraout_EAX;
                }
                pFVar10 = Package_LoadEntry((word *)u_engine_pcx_fnc_00573028);
                if ((bool)uVar23) {
                  return (dword)pFVar10;
                }
                pFVar10 = (FncModuleHeader *)FncModule_LoadAndRelocateCf(pFVar10);
                uVar25 = CONCAT44(extraout_EDX_06,pFVar10);
                if (!(bool)uVar23) {
                  g_PcxFunctionModule = pFVar10;
                  uVar25 = FncModule_GetExportByIndexCf(3,pFVar10);
                  if (!(bool)uVar23) {
                    g_PcxFunctionExport3 = (void *)uVar25;
                    uVar25 = FncModule_GetExportByIndexCf(2,g_PcxFunctionModule);
                    if (!(bool)uVar23) {
                      g_PcxFunctionExport2 = (void *)uVar25;
                      Resource_Release((void *)((ulonglong)uVar25 >> 0x20));
                      pGVar11 = (*g_GraphicsTextureSourceLoadPackageAsset)
                                          (arg0,arg1,(word *)u_gfx_panel_stat_gfx_00573002);
                      if ((bool)uVar23) {
                        return (dword)pGVar11;
                      }
                      g_InGameStatusPanelTextureSource = (undefined *)pGVar11;
                      pRVar12 = (*g_MemoryApi.alloc)(0x800);
                      if ((bool)uVar23) {
                        return (dword)pRVar12;
                      }
                      g_RecentTextSlotStorage = pRVar12;
                      pdVar13 = (*g_MemoryApi.alloc)(0x100);
                      if ((bool)uVar23) {
                        return (dword)pdVar13;
                      }
                      g_OldUnitSecondaryTable = pdVar13;
                      pdVar13 = (*g_MemoryApi.alloc)(0x4000);
                      if ((bool)uVar23) {
                        return (dword)pdVar13;
                      }
                      g_OldUnitPrimaryTable = pdVar13;
                      pvVar5 = (*g_MemoryApi.alloc)(0x400);
                      if ((bool)uVar23) {
                        return (dword)pvVar5;
                      }
                      g_FrontendPlayerListRow1 = (int)pvVar5 + 0x80;
                      g_FrontendPlayerListRow2 = (int)pvVar5 + 0x100;
                      g_FrontendPlayerListRow3 = (int)pvVar5 + 0x180;
                      g_FrontendPlayerListRow4 = (int)pvVar5 + 0x200;
                      g_FrontendPlayerListRow5 = (int)pvVar5 + 0x280;
                      g_FrontendPlayerListRow6 = (int)pvVar5 + 0x300;
                      bVar24 = 0xffffff7f < g_FrontendPlayerListRow6;
                      g_FrontendPlayerListRow7 = (int)pvVar5 + 0x380;
                      g_FrontendPlayerListRows = (dword)pvVar5;
                      pRVar14 = (*g_MemoryApi.alloc)(0x800);
                      if (bVar24) {
                        return (dword)pRVar14;
                      }
                      g_RomRegistrySlots = pRVar14;
                      ppFVar15 = (*g_MemoryApi.alloc)(0x80);
                      if (bVar24) {
                        return (dword)ppFVar15;
                      }
                      g_FrontendSessionListRows = ppFVar15;
                      pFVar16 = (*g_MemoryApi.alloc)(0x1600);
                      if (bVar24) {
                        return (dword)pFVar16;
                      }
                      g_FrontendSessionDiscoveryRecords = pFVar16;
                      puVar17 = (*g_MemoryApi.alloc)(0x2000);
                      if (bVar24) {
                        return (dword)puVar17;
                      }
                      g_InGameFactionStatusTextScratchUtf16 = puVar17;
                      g_InGameFactionStatusTextScratchUtf16Mirror = puVar17;
                      puVar17 = (*g_MemoryApi.alloc)(0x160);
                      if (bVar24) {
                        return (dword)puVar17;
                      }
                      g_InGamePlayerListTextScratchUtf16 = puVar17;
                      pfVar18 = (*g_MemoryApi.alloc)(0x6000);
                      if (bVar24) {
                        return (dword)pfVar18;
                      }
                      g_WorldMotionSplineMatrixWorkspaces[1] = pfVar18 + 0x400;
                      g_WorldMotionSplineMatrixWorkspaces[2] = pfVar18 + 0x800;
                      g_WorldMotionSplineMatrixWorkspaces[3] = pfVar18 + 0xc00;
                      g_WorldMotionSplineMatrixWorkspaces[4] = pfVar18 + 0x1000;
                      bVar24 = (float *)0xffffefff < g_WorldMotionSplineMatrixWorkspaces[4];
                      g_WorldMotionSplineMatrixWorkspaces[5] = pfVar18 + 0x1400;
                      g_WorldMotionSplineMatrixWorkspaces[0] = pfVar18;
                      pfVar18 = (*g_MemoryApi.alloc)(0x300);
                      if (bVar24) {
                        return (dword)pfVar18;
                      }
                      g_WorldMotionSplineCoefficientTables[1] = pfVar18 + 0x20;
                      g_WorldMotionSplineCoefficientTables[2] = pfVar18 + 0x40;
                      g_WorldMotionSplineCoefficientTables[3] = pfVar18 + 0x60;
                      g_WorldMotionSplineCoefficientTables[4] = pfVar18 + 0x80;
                      bVar24 = (float *)0xffffff7f < g_WorldMotionSplineCoefficientTables[4];
                      g_WorldMotionSplineCoefficientTables[5] = pfVar18 + 0xa0;
                      g_WorldMotionSplineCoefficientTables[0] = pfVar18;
                      pSVar19 = (*g_MemoryApi.alloc)(0x408c0);
                      if (bVar24) {
                        return (dword)pSVar19;
                      }
                      g_SelectionPlayerBlocks = pSVar19;
                      pvVar5 = (*g_MemoryApi.alloc)(0x1300);
                      if (bVar24) {
                        return (dword)pvVar5;
                      }
                      g_FrontendLocalPlayerPcxPreview = pvVar5;
                      pvVar5 = (*g_MemoryApi.alloc)(0x4000);
                      if (bVar24) {
                        return (dword)pvVar5;
                      }
                      g_TerrainRegionCollectionEntries = pvVar5;
                      pvVar5 = (*g_MemoryApi.alloc)(800);
                      if (bVar24) {
                        return (dword)pvVar5;
                      }
                      g_FrontendPlayerMessageBuffers = pvVar5;
                      pFVar20 = (*g_MemoryApi.alloc)(0x9d80);
                      if (bVar24) {
                        return (dword)pFVar20;
                      }
                      playerRuntimePointerTableWriteCursor =
                           (FrontendPlayerRuntimeRecord **)&g_FrontendPlayerRuntimeRecordPointers32;
                      g_FrontendPlayerRuntimeBlockCount = 1;
                      g_LocalPlayerRuntimeId = 0;
                      g_FrontendPlayerRuntimeBlocks = pFVar20;
                      (pFVar20->playerName).textUtf16[0] = 0;
                      (pFVar20->playerName).textUtf16[1] = 0;
                      pFVar20->playerRuntimeId = 0;
                      (pFVar20->factionAssignment).roleStateFlags = 0;
                      pFVar20->snapshotTransferFlags = 0;
                      iVar2 = 0x20;
                      do {
                        *playerRuntimePointerTableWriteCursor = pFVar20;
                        playerRuntimePointerTableWriteCursor =
                             playerRuntimePointerTableWriteCursor + 1;
                        bVar24 = (FrontendPlayerRuntimeRecord *)0xffffec4f < pFVar20;
                        pFVar20 = pFVar20 + 1;
                        iVar2 = iVar2 + -1;
                      } while (iVar2 != 0);
                      pbVar21 = (*g_MemoryApi.alloc)(0xe00);
                      if (bVar24) {
                        return (dword)pbVar21;
                      }
                      g_CoreAssetScratchSlice1 = pbVar21 + 0x200;
                      g_CoreAssetScratchSlice2 = pbVar21 + 0x400;
                      g_CoreAssetScratchSlice3 = pbVar21 + 0x600;
                      g_CoreAssetScratchSlice4 = pbVar21 + 0x800;
                      g_CoreAssetScratchSlice5 = pbVar21 + 0xa00;
                      g_CoreAssetScratchSlice6 = pbVar21 + 0xc00;
                      g_CoreAssetScratchSlice0 = pbVar21;
                      for (iVar2 = 0x380; iVar2 != 0; iVar2 = iVar2 + -1) {
                        pbVar21[0] = 0;
                        pbVar21[1] = 0;
                        pbVar21[2] = 0;
                        pbVar21[3] = 0;
                        pbVar21 = pbVar21 + 4;
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
  dVar3 = (dword)uVar25;
  Resource_Release((void *)((ulonglong)uVar25 >> 0x20));
  return dVar3;
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
void Game_PlayIntroMovies(void)

{
  dword dVar1;
  MovieRuntime *arg7;
  uint uVar2;
  dword arg0;
  uint extraout_ECX;
  int extraout_EDX;
  undefined1 in_CF;
  undefined1 uVar3;
  bool bVar4;
  qword qVar5;
  MovieFrameDimensionsEdxEax8 MVar6;
  
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  }
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  }
  (*g_CommandLineFindOption)(8,s_NOINTRO_00573064);
  uVar3 = 1;
  if ((bool)in_CF) {
    while (Movie_Open(1,(word *)u_flm_intro0_flm_00573046), !(bool)uVar3) {
      arg7 = Movie_AdvanceFrame();
      if ((bool)uVar3) {
        Movie_Close();
        return;
      }
      g_IntroMoviePendingTicks = 0;
      UiFrame_FlushInputAndResetPendingTicks();
      (*g_TimerRegisterPeriodic)(arg0,IntroMovie_TimerTick);
      while( true ) {
        (*g_Win32PumpMessages)();
        qVar5 = (*g_KeyboardReadEvent)();
        if (!(bool)uVar3) break;
        uVar2 = (*g_GraphicsCursorConsumeEvent)();
        if ((!(bool)uVar3) && (uVar3 = uVar2 < 4, !(bool)uVar3))
        goto GameIntroMovies_StopCurrentPlayback;
        uVar3 = 0;
        if (g_IntroMoviePendingTicks != 0) {
          do {
            bVar4 = false;
            Movie_AdvanceFrame();
            dVar1 = g_FramebufferHeight;
            uVar3 = true;
            if (bVar4) goto GameIntroMovies_StopCurrentPlayback;
            g_IntroMoviePendingTicks = g_IntroMoviePendingTicks - 1;
          } while ((g_IntroMoviePendingTicks != 0) && (extraout_EDX != 1));
          bVar4 = (g_FramebufferHeight >> 1 & 1) != 0;
          (*g_GraphicsFramebufferBeginAccess)();
          uVar3 = true;
          if (bVar4) goto GameIntroMovies_StopCurrentPlayback;
          MVar6 = Movie_GetFrameDimensions();
          uVar2 = (int)((dVar1 - extraout_ECX) - (int)(MVar6 >> 0x20)) >> 1;
          uVar3 = CARRY4(uVar2,extraout_ECX >> 1);
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (g_FramebufferHeight,g_FramebufferWidth,0,0,uVar2 + (extraout_ECX >> 1),
                     (int)(g_FramebufferWidth - (int)MVar6) >> 1,0,
                     (GraphicsTextureSourceAsset *)arg7,g_FramebufferAccess);
          (*g_GraphicsFramebufferEndAccess)();
          (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
        }
      }
      uVar3 = (uint)qVar5 < 0x10000;
      if ((uint)qVar5 == 0x10000) {
        u_flm_intro0_flm_00573046[9] = L'8';
      }
GameIntroMovies_StopCurrentPlayback:
      (*g_TimerUnregisterPeriodic)(IntroMovie_TimerTick);
      Movie_Close();
      u_flm_intro0_flm_00573046[9] = u_flm_intro0_flm_00573046[9] + L'\x01';
    }
  }
  return;
}

/* Address: 0x00573DB0.
   Ownership: platform/bootstrap/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Resolves the
   bootstrap API table.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
dword __cdecl DynAPI_Bootstrap(void)

{
  void **in_EAX;
  HINSTANCE hModule;
  DynamicApiBinding *bindingCursor;
  void **lpProcName;
  char *moduleName;
  dword moduleSlotIndex;
  
  bindingCursor = g_BootstrapApiBindings;
  do {
    if (bindingCursor->destination == (void **)0x0) {
      return (dword)in_EAX;
    }
    lpProcName = bindingCursor->destination;
    hModule = GetModuleHandleA(bindingCursor->moduleName);
    if (hModule == (HMODULE)0x0) {
      if (bindingCursor->destination == (void **)dynapi_9) {
        Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)bindingCursor->moduleName);
        return 0xf;
      }
      hModule = (HINSTANCE)
                (*(code *)g_BootstrapApiBindings[0].destination)(bindingCursor->moduleName);
      moduleSlotIndex = g_DynamicModuleCount;
      if (hModule == (HINSTANCE)0x0) {
        Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)bindingCursor->moduleName);
        return 0x11;
      }
      g_DynamicModuleCount = g_DynamicModuleCount + 1;
      moduleName = bindingCursor->moduleName;
      g_DynamicModules[moduleSlotIndex].module = hModule;
      g_DynamicModules[moduleSlotIndex].name = moduleName;
      lpProcName = bindingCursor->destination;
    }
    in_EAX = (void **)GetProcAddress(hModule,(LPCSTR)lpProcName);
    if (in_EAX == (void **)0x0) {
      Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)bindingCursor->destination);
      Text_CopyNarrowToUtf16Cf(0x100,g_FatalErrorDetail1Utf16,(byte *)bindingCursor->moduleName);
      return 0x10;
    }
    bindingCursor->destination = in_EAX;
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
void CommandLine_FindOption(CommandLineOptionLengthBytes length,char *option)

{
  dword compareBytesRemaining;
  int optionBufferCapacityRemaining;
  char *pcVar1;
  char *optionBufferCursor;
  char *storedOptionCompareCursor;
  bool comparedBytesEqual;
  char currentOptionBufferByte;
  
  optionBufferCursor = g_CommandLine.optionBuffer;
  do {
    if (*optionBufferCursor == '\0') {
      return;
    }
    comparedBytesEqual = false;
    compareBytesRemaining = length;
    pcVar1 = option;
    storedOptionCompareCursor = optionBufferCursor;
    do {
      if (compareBytesRemaining == 0) break;
      compareBytesRemaining = compareBytesRemaining - 1;
      comparedBytesEqual = *pcVar1 == *storedOptionCompareCursor;
      pcVar1 = pcVar1 + 1;
      storedOptionCompareCursor = storedOptionCompareCursor + 1;
    } while (comparedBytesEqual);
    if (comparedBytesEqual) {
      return;
    }
    optionBufferCapacityRemaining = (int)sz_MainWindowTitle - (int)optionBufferCursor;
    pcVar1 = optionBufferCursor;
    do {
      optionBufferCursor = pcVar1;
      if (optionBufferCapacityRemaining == 0) break;
      optionBufferCapacityRemaining = optionBufferCapacityRemaining + -1;
      optionBufferCursor = pcVar1 + 1;
      currentOptionBufferByte = *pcVar1;
      pcVar1 = optionBufferCursor;
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
void __cdecl CommandLine_Parse(void)

{
  byte *pbVar1;
  CommandLineArgumentMirrorState500 *pCVar2;
  byte bVar3;
  byte *pbVar4;
  char *pcVar5;
  char *pcVar6;
  CommandLineArgumentMirrorState500 *pCVar7;
  char *pcVar8;
  
  g_CommandLineFindOption = CommandLine_FindOption;
  pbVar4 = (byte *)GetCommandLineA();
  pCVar2 = &g_CommandLine;
  if (*pbVar4 == 0x22) {
    pbVar4 = pbVar4 + 1;
    do {
      pCVar7 = pCVar2;
      bVar3 = *pbVar4;
      pCVar7->executablePath[0] = bVar3;
      pbVar4 = pbVar4 + 1;
      if (bVar3 == 0) {
        g_CommandLine.executablePath[0] = '\0';
        goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
      }
      pCVar2 = (CommandLineArgumentMirrorState500 *)(pCVar7->executablePath + 1);
    } while (bVar3 != 0x22);
    pCVar7->executablePath[0] = '\0';
    pcVar6 = g_CommandLine.optionBuffer;
  }
  else {
    do {
      pCVar7 = pCVar2;
      bVar3 = *pbVar4;
      pCVar7->executablePath[0] = bVar3;
      pbVar4 = pbVar4 + 1;
      if (bVar3 == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
      pCVar2 = (CommandLineArgumentMirrorState500 *)(pCVar7->executablePath + 1);
    } while (bVar3 != 0x20);
    pCVar7->executablePath[0] = '\0';
    pcVar6 = g_CommandLine.optionBuffer;
  }
CommandLine_Parse_ContinueScanningNextOptionOrArgument:
  do {
    do {
      while( true ) {
        bVar3 = *pbVar4;
        pbVar4 = pbVar4 + 1;
        if ((bVar3 != 0x2f) && (bVar3 != 0x2d)) break;
        do {
          while( true ) {
            pcVar5 = pcVar6;
            bVar3 = *pbVar4;
            if ((0x60 < bVar3) && (bVar3 < 0x7b)) {
              bVar3 = bVar3 - 0x20;
            }
            *pcVar5 = bVar3;
            pbVar4 = pbVar4 + 1;
            pcVar6 = pcVar5 + 1;
            if (bVar3 == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
            if (bVar3 != 0x22) break;
            do {
              bVar3 = *pbVar4;
              *pcVar6 = bVar3;
              pbVar4 = pbVar4 + 1;
              pcVar6 = pcVar6 + 1;
              if (bVar3 == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
            } while (bVar3 != 0x22);
          }
        } while (bVar3 != 0x20);
        *pcVar5 = 0;
      }
      if (bVar3 == 0) {
CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn:
        Text_CopyNarrowToUtf16Cf
                  (0x200,g_CommandLineWideArguments.argument1,(byte *)g_CommandLine.argument1);
        Text_CopyNarrowToUtf16Cf
                  (0x200,g_CommandLineWideArguments.argument2,(byte *)g_CommandLine.argument2);
        Text_CopyNarrowToUtf16Cf
                  (0x200,g_CommandLineWideArguments.argument3,(byte *)g_CommandLine.argument3);
        return;
      }
    } while (bVar3 == 0x20);
    if (bVar3 == 0x22) {
      pbVar1 = pbVar4;
      if (g_CommandLine.argument1[0] == '\0') {
        pcVar5 = g_CommandLine.argument1;
CommandLine_Parse_CopyQuotedArgumentToNextAvailableSlot:
        do {
          pcVar8 = pcVar5;
          pbVar4 = pbVar1;
          bVar3 = *pbVar4;
          if ((0x60 < bVar3) && (bVar3 < 0x7b)) {
            bVar3 = bVar3 - 0x20;
          }
          *pcVar8 = bVar3;
          if (bVar3 == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
          pbVar1 = pbVar4 + 1;
          pcVar5 = pcVar8 + 1;
        } while (bVar3 != 0x22);
        *pcVar8 = 0;
      }
      else {
        pcVar5 = g_CommandLine.argument2;
        if ((g_CommandLine.argument2[0] == '\0') ||
           (pcVar5 = g_CommandLine.argument3, g_CommandLine.argument3[0] == '\0'))
        goto CommandLine_Parse_CopyQuotedArgumentToNextAvailableSlot;
        do {
          bVar3 = *pbVar4;
          pbVar4 = pbVar4 + 1;
          if (bVar3 == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
        } while (bVar3 != 0x22);
      }
      goto CommandLine_Parse_ContinueScanningNextOptionOrArgument;
    }
    if ((0x60 < bVar3) && (bVar3 < 0x7b)) {
      bVar3 = bVar3 - 0x20;
    }
    pbVar1 = pbVar4;
    if (g_CommandLine.argument1[0] == '\0') {
      pcVar5 = g_CommandLine.argument1 + 1;
      g_CommandLine.argument1[0] = bVar3;
    }
    else if (g_CommandLine.argument2[0] == '\0') {
      pcVar5 = g_CommandLine.argument2 + 1;
      g_CommandLine.argument2[0] = bVar3;
    }
    else {
      if (g_CommandLine.argument3[0] != '\0') {
        do {
          bVar3 = *pbVar4;
          pbVar4 = pbVar4 + 1;
          if (bVar3 == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
        } while (bVar3 != 0x20);
        goto CommandLine_Parse_ContinueScanningNextOptionOrArgument;
      }
      pcVar5 = g_CommandLine.argument3 + 1;
      g_CommandLine.argument3[0] = bVar3;
    }
    do {
      pcVar8 = pcVar5;
      pbVar4 = pbVar1;
      bVar3 = *pbVar4;
      if ((0x60 < bVar3) && (bVar3 < 0x7b)) {
        bVar3 = bVar3 - 0x20;
      }
      *pcVar8 = bVar3;
      if (bVar3 == 0) goto CommandLine_Parse_FinalizeUtf16ArgumentsAndReturn;
      pbVar1 = pbVar4 + 1;
      pcVar5 = pcVar8 + 1;
    } while (bVar3 != 0x20);
    *pcVar8 = 0;
  } while( true );
}
