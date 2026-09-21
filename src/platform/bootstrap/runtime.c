/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

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
  dword errorOrValue;
  dword dVar4;
  dword errorOrValue_00;
  dword arg0;
  bool bVar5;
  undefined1 carryIn;
  StatusValueEaxCf5 SVar6;
  FatalErrorEaxCf5 FVar7;
  DisplayModeEaxCf5 DVar8;
  CommandLineFindOptionEbxCf5 CVar9;
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
    g_MainWindowClassInstanceHandle = g_hInstance;
    g_MainWindowClassIconHandle = LoadIconA(g_hInstance,(LPCSTR)0x1);
    g_MainWindowClassCursorHandle = LoadCursorA((HINSTANCE)0x0,&k_LowAddressLiteral00007F00);
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
        if (g_CpuFeatureFlags == 0) {
          (*g_FatalErrorPrimaryDispatchCf)(0x51,true);
        }
        SVar6 = DynAPI_Bootstrap();
        FVar7 = (*g_FatalErrorPrimaryDispatchCf)(SVar6.valueOrError,SVar6.carry);
        bVar5 = FVar7.carry;
        TimerSystem_Init();
        FVar7 = (*g_FatalErrorPrimaryDispatchCf)(errorOrValue,bVar5);
        bVar5 = FVar7.carry;
        dVar4 = Graphics_Init();
        (*g_FatalErrorPrimaryDispatchCf)(dVar4,bVar5);
        SVar6 = DirectInputMouse_Init();
        (*g_FatalErrorPrimaryDispatchCf)(SVar6.valueOrError,SVar6.carry);
        bVar5 = DirectSound_Init();
        carryIn = 0;
        if (bVar5) {
          CVar9 = CommandLine_FindOption(6,s_SOUND_00582f28);
          carryIn = CVar9.carry;
          if (!(bool)carryIn) {
            FVar7 = (*g_FatalErrorPrimaryDispatchCf)(errorOrValue_00,true);
            carryIn = FVar7.carry;
          }
        }
        dVar4 = Network_Init();
        (*g_FatalErrorPrimaryDispatchCf)(dVar4,(bool)carryIn);
        PersistentSettings_Load();
        arg3 = 0x280;
        arg2 = 0x1e0;
        dVar4 = PersistentSettings_ReadDword(0x10,0xc);
        arg0 = PersistentSettings_ReadDword(0,0);
        if (g_GraphicsAdapterCount <= arg0) {
          arg0 = 0;
        }
        DVar8 = (*g_GraphicsDisplayModeHook)(arg0,dVar4,arg2,arg3);
        (*g_FatalErrorPrimaryDispatchCf)(DVar8.eax,DVar8.carry);
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
  FactionCapabilityFlags *pFVar1;
  void *memory;
  int iVar2;
  uint uVar3;
  uint uVar4;
  GameFactionRuntimeImage *pGVar5;
  dword *pdVar6;
  undefined4 *puVar7;
  ArenaAllocEaxCf5 AVar8;
  StatusValueEaxCf5 SVar9;
  
  iVar2 = 0x40;
  pdVar6 = g_GameDataAuxState.pairPressureMatrix8x8;
  for (; iVar2 != 0; iVar2 = iVar2 + -1) {
    *pdVar6 = 0;
    pdVar6 = pdVar6 + 1;
  }
  pGVar5 = &g_GameFactionRuntimeImage;
  for (iVar2 = 0xe80; iVar2 != 0; iVar2 = iVar2 + -1) {
    pGVar5->records[0].xeniteCurrentQ4 = 0;
    pGVar5 = (GameFactionRuntimeImage *)&pGVar5->records[0].xeniteStorageLimitQ4;
  }
  pGVar5 = &g_GameFactionRuntimeImage;
  iVar2 = 8;
  uVar4 = 1;
  uVar3 = 0x1111111f;
  do {
    pFVar1 = &pGVar5->records[0].capabilityFlags;
    *pFVar1 = *pFVar1 | uVar4;
    pdVar6 = pGVar5->records[0].technologyMasks256Bits;
    *pdVar6 = *pdVar6 | 1;
    pFVar1 = &pGVar5->records[0].capabilityFlags;
    *pFVar1 = *pFVar1 | 1;
    pGVar5->records[0].packedRelationStates = uVar3;
    pGVar5->records[0].relationCapabilityState = 0;
    pGVar5->records[0].primaryAnchorYQ12 = -0xc000;
    pGVar5->records[0].primaryAnchorXQ12 = 0;
    pGVar5->records[0].secondaryAnchorYQ12 = -0xc000;
    pGVar5->records[0].secondaryAnchorXQ12 = 0;
    pGVar5->records[0].relationTransitionTick = 0x11;
    pGVar5->records[0].energyGenerationCapacityQ4 = 0x280;
    pGVar5->records[0].baselineEnergySupplyQ4 = 0x280;
    pGVar5->records[0].xeniteStorageLimitQ4 = 4000;
    pGVar5->records[0].tritiumStorageLimitQ4 = 4000;
    pGVar5->records[0].terrainContributionScaleQ8 = 0x100;
    uVar4 = uVar4 * 2;
    uVar3 = uVar3 << 4 | uVar3 >> 0x1c;
    pGVar5 = (GameFactionRuntimeImage *)(pGVar5->records + 1);
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  AVar8 = (*g_MemoryApi.alloc)(0x38000);
  memory = g_GameStatTableImage;
  if (!AVar8.carry) {
    LOCK();
    UNLOCK();
    g_GameStatTableImage = (undefined4 *)AVar8.eax;
    (*g_MemoryApi.free)(memory);
    puVar7 = (undefined4 *)AVar8.eax;
    for (iVar2 = 0xe000; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar7 = 0;
      puVar7 = puVar7 + 1;
    }
    puVar7[-1] = 0xffffffff;
    g_GameFactionRuntimeImage.tail.periodicClockTick = 0;
    AVar8.eax = 0;
    AVar8.carry = false;
  }
  SVar9.valueOrError = AVar8.eax;
  SVar9.carry = AVar8.carry;
  return SVar9;
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
  dword *pdVar1;
  int iVar2;
  dword *pdVar3;
  dword *pdVar4;
  StatusValueEaxCf5 SVar5;
  PackageLoadEntryEaxCf5 PVar6;
  
  iVar2 = 0x40;
  pdVar1 = g_GameDataAuxState.pairPressureMatrix8x8;
  for (; iVar2 != 0; iVar2 = iVar2 + -1) {
    *pdVar1 = 0;
    pdVar1 = pdVar1 + 1;
  }
  SVar5 = Package_LoadEntryIntoBuffer
                    (0x3a20,(byte *)&g_GameFactionRuntimeImage,(word *)u_daten_hex_0050e054);
  if (!SVar5.carry) {
    PVar6 = Package_LoadEntry((word *)u_stat_hex_0050e082);
    memory = g_GameStatTableImage;
    if (!PVar6.carry) {
      LOCK();
      UNLOCK();
      g_GameStatTableImage = PVar6.bufferOrError;
      (*g_MemoryApi.free)(memory);
      PVar6 = Package_LoadEntry((word *)u_oldunit_hex_0050e094);
      pdVar1 = PVar6.bufferOrError;
      if (PVar6.carry) {
        pdVar1 = g_OldUnitPrimaryTable;
        for (iVar2 = 0x1000; iVar2 != 0; iVar2 = iVar2 + -1) {
          *pdVar1 = 0;
          pdVar1 = pdVar1 + 1;
        }
        pdVar1 = g_OldUnitSecondaryTable;
        for (iVar2 = 0x40; iVar2 != 0; iVar2 = iVar2 + -1) {
          *pdVar1 = 0;
          pdVar1 = pdVar1 + 1;
        }
        g_OldUnitRecordCount = 0;
      }
      else {
        g_OldUnitRecordCount = *pdVar1;
        pdVar4 = g_OldUnitPrimaryTable;
        pdVar3 = pdVar1;
        for (iVar2 = 0x1000; pdVar3 = pdVar3 + 1, iVar2 != 0; iVar2 = iVar2 + -1) {
          *pdVar4 = *pdVar3;
          pdVar4 = pdVar4 + 1;
        }
        pdVar4 = g_OldUnitSecondaryTable;
        for (iVar2 = 0x40; iVar2 != 0; iVar2 = iVar2 + -1) {
          *pdVar4 = *pdVar3;
          pdVar3 = pdVar3 + 1;
          pdVar4 = pdVar4 + 1;
        }
        Resource_Release(pdVar1);
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
  DynApiResolveEaxCf5 DVar1;
  DynApiResolveEaxCf5 DVar2;
  
  Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)procedureName);
  resolvedProcedure = GetProcAddress(module,procedureName);
  if (resolvedProcedure != (FARPROC)0x0) {
    *destination = resolvedProcedure;
    DVar1.carry = false;
    DVar1.procedureOrError = resolvedProcedure;
    return DVar1;
  }
  moduleEntryCursor = g_DynamicModules;
  g_FatalErrorDetail1Utf16[0] = 0;
  modulesRemaining = g_DynamicModuleCount;
  do {
    if (modulesRemaining == 0) {
LAB_00573c3e:
      DVar2.carry = true;
      DVar2.procedureOrError = (void *)0x10;
      return DVar2;
    }
    if (module == moduleEntryCursor->module) {
      Text_CopyNarrowToUtf16Cf(0x100,g_FatalErrorDetail1Utf16,(byte *)moduleEntryCursor->name);
      goto LAB_00573c3e;
    }
    moduleEntryCursor = moduleEntryCursor + 1;
    modulesRemaining = modulesRemaining - 1;
  } while( true );
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
  DynDllLoadEaxCf5 DVar1;
  DynDllLoadEaxCf5 DVar2;
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
      DVar1.carry = false;
      DVar1.moduleOrError = loadedModule;
      return DVar1;
    }
  }
  DVar2.carry = true;
  DVar2.moduleOrError = (HINSTANCE)0x11;
  return DVar2;
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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
BootstrapApi_ResolveBindingByDestination(void **destination)

{
  void *pvVar1;
  dword dVar2;
  DynamicApiBinding *pDVar3;
  StatusValueEaxCf5 SVar4;
  StatusValueEaxCf5 SVar5;
  
  pDVar3 = g_BootstrapApiBindings;
  dVar2 = g_DynamicModuleCount;
  do {
    if (dVar2 == 0) {
LAB_00573d6a:
      SVar4.carry = true;
      SVar4.valueOrError = 0xf;
      return SVar4;
    }
    if (destination == pDVar3->destination) {
      Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)destination);
      pvVar1 = (void *)(*(code *)g_BootstrapApiBindings[0].destination)(destination,pDVar3);
      if (pvVar1 != (void *)0x0) {
        *destination = pvVar1;
        SVar5.valueOrError = 0xf;
        SVar5.carry = false;
        return SVar5;
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
        (*g_GraphicsBackendRefreshActiveAdapterCf)();
      }
    }
    else {
      pvVar2 = GetCurrentProcess();
      SetPriorityClass(pvVar2,0x100);
      if (g_MouseDevice != (IDirectInputDeviceA *)0x0) {
        (*g_MouseDevice->lpVtbl->Acquire)(g_MouseDevice);
      }
      if (-1 < g_ActiveGraphicsAdapterIndex) {
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
  dword errorOrValue;
  dword dVar1;
  dword arg2;
  dword arg1;
  dword arg0;
  bool bVar2;
  GraphicsCursorFrameEaxCf5 GVar3;
  FatalErrorEaxCf5 FVar4;
  DisplayModeEaxCf5 DVar5;
  FrontendMainLoopEaxCf5 FVar6;
  
  GVar3 = (*g_GraphicsCursorSetFrame)(0);
  FVar4 = (*g_FatalErrorPrimaryDispatchCf)(GVar3.eax,GVar3.carry);
  bVar2 = FVar4.carry;
  GameRuntime_InitializeSpatialAudioAndRenderingCf();
  FVar4 = (*g_FatalErrorPrimaryDispatchCf)(errorOrValue,bVar2);
  bVar2 = FVar4.carry;
  dVar1 = Game_LoadCoreAssets();
  FVar4 = (*g_FatalErrorPrimaryDispatchCf)(dVar1,bVar2);
  bVar2 = FVar4.carry;
  Game_PlayIntroMovies();
  (*g_FatalErrorPrimaryDispatchCf)(FVar4.eax,bVar2);
  PersistentSettings_Load();
  dVar1 = PersistentSettings_ReadDword(0x280,4);
  arg2 = PersistentSettings_ReadDword(0x1e0,8);
  arg1 = PersistentSettings_ReadDword(0x10,0xc);
  if (((dVar1 != 0x280) || (arg2 != 0x1e0)) || (arg1 != 0x10)) {
    arg0 = PersistentSettings_ReadDword(0,0);
    if (g_GraphicsAdapterCount <= arg0) {
      arg0 = 0;
    }
    DVar5 = (*g_GraphicsDisplayModeHook)(arg0,arg1,arg2,dVar1);
    (*g_FatalErrorPrimaryDispatchCf)(DVar5.eax,DVar5.carry);
    PersistentSettings_WriteDword(g_ActiveGraphicsAdapterIndex,0);
  }
  FVar6 = Frontend_MainLoop(1);
  (*g_FatalErrorPrimaryDispatchCf)(FVar6.errorOrValue,FVar6.carry);
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
  bool bVar1;
  
  bVar1 = SpatialSoundPool_Init();
  if (!bVar1) {
    bVar1 = TerrainByteClampLookup_Initialize();
    if (!bVar1) {
      bVar1 = GraphicsIntensityClampTable_InitializeCf();
      if (!bVar1) {
        bVar1 = SoftwareRenderer_InstallDisplayModeHook();
        if (!bVar1) {
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
  SoundSampleAsset *arg0;
  FncModuleHeader *module;
  word *pwVar3;
  dword dVar4;
  AudioMixerGainQ15 AVar5;
  MovieAudioGainQ15 MVar6;
  MovieAudioGainQ15 MVar7;
  float *pfVar8;
  FrontendPlayerRuntimeRecord *pFVar9;
  byte *pbVar10;
  TextResourceId resourceId;
  SoundSampleAsset *allocation;
  FrontendPlayerRuntimeRecord **playerRuntimePointerTableWriteCursor;
  StatusValueEaxCf5 SVar11;
  SoundCreateSampleVoiceSetEaxCf5 SVar12;
  FileSystemOpenEaxCf5 FVar13;
  TextResourceResolveEaxCf5 TVar14;
  TextResourceLoadEaxCf5 TVar15;
  PackageLoadEntryEaxCf5 PVar16;
  FncModuleLoadEaxCf5 FVar17;
  GraphicsTextureSourceLoadEaxCf5 GVar18;
  ArenaAllocEaxCf5 AVar19;
  ResourceLoadEaxEcxCf9 RVar20;
  
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
  } while (0x2f < g_LevelArchivePathTemplateUtf16.decimalDigits.codeUnits[0]);
  SVar11 = Package_Mount((word *)u_daten_pck_00572e56);
  if (!SVar11.carry) {
    g_DataPackageHandle = SVar11.valueOrError;
  }
  SVar11 = Package_Mount((word *)u_modelle_pck_00572e6a);
  if (!SVar11.carry) {
    g_ModelPackageHandle = SVar11.valueOrError;
  }
  SVar11 = Package_Mount((word *)u_graphik_pck_00572e82);
  if (!SVar11.carry) {
    g_GraphicsPackageHandle = SVar11.valueOrError;
  }
  SVar11 = Package_Mount((word *)u_sound_pck_00572e9a);
  if (!SVar11.carry) {
    g_SoundPackageHandle = SVar11.valueOrError;
  }
  SVar11 = Package_Mount((word *)u_filme_pck_00572eae);
  if (!SVar11.carry) {
    g_MoviePackageHandle = SVar11.valueOrError;
  }
  SVar11 = Package_Mount((word *)u_level_pck_00572ec2);
  if (!SVar11.carry) {
    g_LevelPackageHandle = SVar11.valueOrError;
  }
  RVar20 = Resource_Load((word *)u_sound_button0_sam_00572f06);
  arg0 = (SoundSampleAsset *)RVar20.eax;
  if (RVar20.carry) {
    return (dword)arg0;
  }
  SVar12 = (*g_SoundCreateSampleVoiceSet)(arg0);
  module = (FncModuleHeader *)SVar12.eax;
  if (!SVar12.carry) {
    Resource_Release(arg0);
    g_UiButtonSoundVoiceSets7[0] = (DirectSoundVoiceSet *)module;
    RVar20 = Resource_Load((word *)u_sound_button1_sam_00572f2a);
    arg0 = (SoundSampleAsset *)RVar20.eax;
    if (RVar20.carry) {
      return (dword)arg0;
    }
    SVar12 = (*g_SoundCreateSampleVoiceSet)(arg0);
    module = (FncModuleHeader *)SVar12.eax;
    if (!SVar12.carry) {
      Resource_Release(arg0);
      g_UiButtonSoundVoiceSets7[1] = (DirectSoundVoiceSet *)module;
      RVar20 = Resource_Load((word *)u_sound_button2_sam_00572f4e);
      arg0 = (SoundSampleAsset *)RVar20.eax;
      if (RVar20.carry) {
        return (dword)arg0;
      }
      SVar12 = (*g_SoundCreateSampleVoiceSet)(arg0);
      module = (FncModuleHeader *)SVar12.eax;
      if (!SVar12.carry) {
        Resource_Release(arg0);
        g_UiButtonSoundVoiceSets7[2] = (DirectSoundVoiceSet *)module;
        RVar20 = Resource_Load((word *)u_sound_button3_sam_00572f72);
        arg0 = (SoundSampleAsset *)RVar20.eax;
        if (RVar20.carry) {
          return (dword)arg0;
        }
        SVar12 = (*g_SoundCreateSampleVoiceSet)(arg0);
        module = (FncModuleHeader *)SVar12.eax;
        if (!SVar12.carry) {
          Resource_Release(arg0);
          g_UiButtonSoundVoiceSets7[3] = (DirectSoundVoiceSet *)module;
          RVar20 = Resource_Load((word *)u_sound_button4_sam_00572f96);
          arg0 = (SoundSampleAsset *)RVar20.eax;
          if (RVar20.carry) {
            return (dword)arg0;
          }
          SVar12 = (*g_SoundCreateSampleVoiceSet)(arg0);
          module = (FncModuleHeader *)SVar12.eax;
          if (!SVar12.carry) {
            Resource_Release(arg0);
            g_UiButtonSoundVoiceSets7[4] = (DirectSoundVoiceSet *)module;
            RVar20 = Resource_Load((word *)u_sound_button5_sam_00572fba);
            arg0 = (SoundSampleAsset *)RVar20.eax;
            if (RVar20.carry) {
              return (dword)arg0;
            }
            SVar12 = (*g_SoundCreateSampleVoiceSet)(arg0);
            module = (FncModuleHeader *)SVar12.eax;
            if (!SVar12.carry) {
              Resource_Release(arg0);
              g_UiButtonSoundVoiceSets7[5] = (DirectSoundVoiceSet *)module;
              RVar20 = Resource_Load((word *)u_sound_button6_sam_00572fde);
              arg0 = (SoundSampleAsset *)RVar20.eax;
              if (RVar20.carry) {
                return (dword)arg0;
              }
              SVar12 = (*g_SoundCreateSampleVoiceSet)(arg0);
              module = (FncModuleHeader *)SVar12.eax;
              if (!SVar12.carry) {
                Resource_Release(arg0);
                g_UiButtonSoundVoiceSets7[6] = (DirectSoundVoiceSet *)module;
                do {
                  do {
                    FVar13 = (*g_FileSystemOpenCf)(0,(word *)(u_Dscreen00_pcx_00572e3a + 1));
                    if (FVar13.carry)
                    goto Game_LoadCoreAssets_BindDebugOverlayTextAndContinueRemainingAssetLoad;
                    u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + L'\x01';
                    (*g_FileSystemClose)((void *)FVar13.eax);
                    wVar1 = u_Dscreen00_pcx_00572e3a[7];
                  } while ((ushort)u_Dscreen00_pcx_00572e3a[8] < 0x3a);
                  u_Dscreen00_pcx_00572e3a[7] = u_Dscreen00_pcx_00572e3a[7] + L'\x01';
                  u_Dscreen00_pcx_00572e3a[8] = u_Dscreen00_pcx_00572e3a[8] + L'\xfff6';
                } while ((ushort)u_Dscreen00_pcx_00572e3a[7] < 0x3a);
                u_Dscreen00_pcx_00572e3a[7] = wVar1 + L'\xfff7';
Game_LoadCoreAssets_BindDebugOverlayTextAndContinueRemainingAssetLoad:
                resourceId = 0x112;
                do {
                  TVar14 = TextResource_Resolve(resourceId);
                  pwVar3 = TVar14.eax;
                  resourceId = resourceId + 1;
                  RichTextCommandStream_PatchPayloadBySelector
                            (0,g_FrontendDebugOverlayTextSlot00Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (1,g_FrontendDebugOverlayTextSlot01Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (2,g_FrontendDebugOverlayTextSlot02Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (3,g_FrontendDebugOverlayTextSlot03Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (4,g_FrontendDebugOverlayTextSlot04Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (5,g_FrontendDebugOverlayTextSlot05Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (6,g_FrontendDebugOverlayTextSlot06Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (7,g_FrontendDebugOverlayTextSlot07Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (8,g_FrontendDebugOverlayTextSlot08Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (9,g_FrontendDebugOverlayTextSlot09Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (10,g_FrontendDebugOverlayTextSlot10Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (0xb,g_FrontendDebugOverlayTextSlot11Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (0xc,g_FrontendDebugOverlayTextSlot12Utf16,pwVar3);
                  RichTextCommandStream_PatchPayloadBySelector
                            (0xd,g_FrontendDebugOverlayTextSlot13Utf16,pwVar3);
                } while (resourceId < 0x118);
                UiActionHandlers_SetPageCf
                          (0x10,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage10);
                UiActionHandlers_SetPageCf
                          (0x11,(UiActionHandlerPage *)&g_InGameUiCommandModeActionHandlers30);
                UiActionHandlers_SetPageCf
                          (0x12,(UiActionHandlerPage *)&g_InGameUiActionHandlersPage12);
                UiActionHandlers_SetPageCf
                          (0x20,(UiActionHandlerPage *)&g_FrontendUiActionHandlersPage20);
                TVar15 = TextResourcePage_Load(0xff,(word *)u_texte_neterror_str_0050f104);
                if (TVar15.carry) {
                  return TVar15.errorOrValue;
                }
                TVar15 = TextResourcePage_Load(0x18,(word *)u_texte_help_str_00563170);
                if (TVar15.carry) {
                  return TVar15.errorOrValue;
                }
                TVar15 = TextResourcePage_Load(0x20,(word *)u_texte_hilfe_str_00545b34);
                if (TVar15.carry) {
                  return TVar15.errorOrValue;
                }
                TVar15 = TextResourcePage_Load(0x21,(word *)u_texte_menue_str_00545ba0);
                if (TVar15.carry) {
                  return TVar15.errorOrValue;
                }
                TVar15 = TextResourcePage_Load(0x30,(word *)u_texte_techno_str_0050dec4);
                if (TVar15.carry) {
                  return TVar15.errorOrValue;
                }
                TVar15 = TextResourcePage_Load(0x22,(word *)u_texte_level_str_00545bc0);
                if (TVar15.carry) {
                  return TVar15.errorOrValue;
                }
                TVar15 = TextResourcePage_Load(0x23,(word *)u_texte_inhalt_str_00545be0);
                if (TVar15.carry) {
                  return TVar15.errorOrValue;
                }
                TVar15 = TextResourcePage_Load(0x24,(word *)u_texte_tastatur_str_005631b8);
                if (TVar15.carry) {
                  return TVar15.errorOrValue;
                }
                TVar14 = TextResource_Resolve(0x2402);
                RichTextCommandStream_BindTextureSource(g_CursorSourceAsset,TVar14.eax);
                dVar4 = PersistentSettings_ReadDword(3,0x20);
                AVar5 = 0;
                if ((dVar4 & 1) != 0) {
                  AVar5 = PersistentSettings_ReadDword(0x8000,0x24);
                }
                MVar6 = 0;
                g_UiSoundGainQ15 = AVar5;
                g_SoundEffectsGainQ15 = AVar5;
                if ((dVar4 & 1) != 0) {
                  MVar6 = PersistentSettings_ReadDword(0x8000,0x28);
                }
                MVar7 = 0;
                g_MovieDefaultAudioGainQ15 = MVar6;
                if ((dVar4 & 1) != 0) {
                  MVar7 = PersistentSettings_ReadDword(0x8000,0x4c);
                }
                g_ReverseStereoMask = 0;
                if ((dVar4 & 4) != 0) {
                  g_ReverseStereoMask = 0xffffffff;
                }
                g_MovieAlternateAudioGainQ15 = MVar7;
                g_ModelLodDepthThresholdQ8 = PersistentSettings_ReadDword(g_ReverseStereoMask,0x34);
                SVar11 = AiRuntime_InitWorkspace();
                if (SVar11.carry) {
                  return SVar11.valueOrError;
                }
                PVar16 = Package_LoadEntry((word *)u_engine_pcx_fnc_00573028);
                if (PVar16.carry) {
                  return (dword)PVar16.bufferOrError;
                }
                FVar17 = FncModule_LoadAndRelocateCf(PVar16.bufferOrError);
                module = (FncModuleHeader *)FVar17.moduleBase;
                arg0 = allocation;
                if (!FVar17.carry) {
                  g_PcxFunctionModule = module;
                  SVar11 = FncModule_GetExportByIndexCf(3,module);
                  module = (FncModuleHeader *)SVar11.valueOrError;
                  if (!SVar11.carry) {
                    g_PcxFunctionExport3 = (PcxEncodeProc *)module;
                    SVar11 = FncModule_GetExportByIndexCf(2,g_PcxFunctionModule);
                    module = (FncModuleHeader *)SVar11.valueOrError;
                    if (!SVar11.carry) {
                      g_PcxFunctionExport2 = (PcxDecodeProc *)module;
                      Resource_Release(allocation);
                      GVar18 = (*g_GraphicsTextureSourceLoadPackageAsset)
                                         ((word *)u_gfx_panel_stat_gfx_00573002);
                      if (GVar18.carry) {
                        return (dword)GVar18.eax;
                      }
                      g_InGameStatusPanelTextureSource = GVar18.eax;
                      AVar19 = (*g_MemoryApi.alloc)(0x800);
                      if (AVar19.carry) {
                        return (dword)(RecentTextHistorySlot *)AVar19.eax;
                      }
                      g_RecentTextSlotStorage = (RecentTextHistorySlot *)AVar19.eax;
                      AVar19 = (*g_MemoryApi.alloc)(0x100);
                      if (AVar19.carry) {
                        return (dword)(dword *)AVar19.eax;
                      }
                      g_OldUnitSecondaryTable = (dword *)AVar19.eax;
                      AVar19 = (*g_MemoryApi.alloc)(0x4000);
                      if (AVar19.carry) {
                        return (dword)(dword *)AVar19.eax;
                      }
                      g_OldUnitPrimaryTable = (dword *)AVar19.eax;
                      AVar19 = (*g_MemoryApi.alloc)(0x400);
                      dVar4 = AVar19.eax;
                      if (AVar19.carry) {
                        return dVar4;
                      }
                      g_FrontendPlayerListRow1 = dVar4 + 0x80;
                      g_FrontendPlayerListRow2 = dVar4 + 0x100;
                      g_FrontendPlayerListRow3 = dVar4 + 0x180;
                      g_FrontendPlayerListRow4 = dVar4 + 0x200;
                      g_FrontendPlayerListRow5 = dVar4 + 0x280;
                      g_FrontendPlayerListRow6 = dVar4 + 0x300;
                      g_FrontendPlayerListRow7 = dVar4 + 0x380;
                      g_FrontendPlayerListRows = dVar4;
                      AVar19 = (*g_MemoryApi.alloc)(0x800);
                      if (AVar19.carry) {
                        return (dword)(RomRegistrySlot *)AVar19.eax;
                      }
                      g_RomRegistrySlots = (RomRegistrySlot *)AVar19.eax;
                      AVar19 = (*g_MemoryApi.alloc)(0x80);
                      if (AVar19.carry) {
                        return (dword)(FrontendSessionDiscoveryRecordB0 **)AVar19.eax;
                      }
                      g_FrontendSessionListRows = (FrontendSessionDiscoveryRecordB0 **)AVar19.eax;
                      AVar19 = (*g_MemoryApi.alloc)(0x1600);
                      if (AVar19.carry) {
                        return (dword)(FrontendSessionDiscoveryRecordB0 *)AVar19.eax;
                      }
                      g_FrontendSessionDiscoveryRecords =
                           (FrontendSessionDiscoveryRecordB0 *)AVar19.eax;
                      AVar19 = (*g_MemoryApi.alloc)(0x2000);
                      pwVar3 = (word *)AVar19.eax;
                      if (AVar19.carry) {
                        return (dword)pwVar3;
                      }
                      g_InGameFactionStatusTextScratchUtf16 = pwVar3;
                      g_InGameFactionStatusTextScratchUtf16Mirror = pwVar3;
                      AVar19 = (*g_MemoryApi.alloc)(0x160);
                      if (AVar19.carry) {
                        return (dword)(word *)AVar19.eax;
                      }
                      g_InGamePlayerListTextScratchUtf16 = (word *)AVar19.eax;
                      AVar19 = (*g_MemoryApi.alloc)(0x6000);
                      pfVar8 = (float *)AVar19.eax;
                      if (AVar19.carry) {
                        return (dword)pfVar8;
                      }
                      g_WorldMotionSplineMatrixWorkspaces[1] = pfVar8 + 0x400;
                      g_WorldMotionSplineMatrixWorkspaces[2] = pfVar8 + 0x800;
                      g_WorldMotionSplineMatrixWorkspaces[3] = pfVar8 + 0xc00;
                      g_WorldMotionSplineMatrixWorkspaces[4] = pfVar8 + 0x1000;
                      g_WorldMotionSplineMatrixWorkspaces[5] = pfVar8 + 0x1400;
                      g_WorldMotionSplineMatrixWorkspaces[0] = pfVar8;
                      AVar19 = (*g_MemoryApi.alloc)(0x300);
                      pfVar8 = (float *)AVar19.eax;
                      if (AVar19.carry) {
                        return (dword)pfVar8;
                      }
                      g_WorldMotionSplineCoefficientTables[1] = pfVar8 + 0x20;
                      g_WorldMotionSplineCoefficientTables[2] = pfVar8 + 0x40;
                      g_WorldMotionSplineCoefficientTables[3] = pfVar8 + 0x60;
                      g_WorldMotionSplineCoefficientTables[4] = pfVar8 + 0x80;
                      g_WorldMotionSplineCoefficientTables[5] = pfVar8 + 0xa0;
                      g_WorldMotionSplineCoefficientTables[0] = pfVar8;
                      AVar19 = (*g_MemoryApi.alloc)(0x408c0);
                      if (AVar19.carry) {
                        return (dword)(SelectionPlayerRuntimeBlock *)AVar19.eax;
                      }
                      g_SelectionPlayerBlocks = (SelectionPlayerRuntimeBlock *)AVar19.eax;
                      AVar19 = (*g_MemoryApi.alloc)(0x1300);
                      if (AVar19.carry) {
                        return AVar19.eax;
                      }
                      g_FrontendLocalPlayerPcxPreview = AVar19.eax;
                      AVar19 = (*g_MemoryApi.alloc)(0x4000);
                      if (AVar19.carry) {
                        return AVar19.eax;
                      }
                      g_TerrainRegionCollectionEntries = AVar19.eax;
                      AVar19 = (*g_MemoryApi.alloc)(800);
                      if (AVar19.carry) {
                        return AVar19.eax;
                      }
                      g_FrontendPlayerMessageBuffers = AVar19.eax;
                      AVar19 = (*g_MemoryApi.alloc)(0x9d80);
                      pFVar9 = (FrontendPlayerRuntimeRecord *)AVar19.eax;
                      if (AVar19.carry) {
                        return (dword)pFVar9;
                      }
                      playerRuntimePointerTableWriteCursor =
                           (FrontendPlayerRuntimeRecord **)&g_FrontendPlayerRuntimeRecordPointers32;
                      g_FrontendPlayerRuntimeBlockCount = 1;
                      g_LocalPlayerRuntimeId = 0;
                      g_FrontendPlayerRuntimeBlocks = pFVar9;
                      (pFVar9->playerName).textUtf16[0] = 0;
                      (pFVar9->playerName).textUtf16[1] = 0;
                      pFVar9->playerRuntimeId = 0;
                      (pFVar9->factionAssignment).roleStateFlags = 0;
                      pFVar9->snapshotTransferFlags = 0;
                      iVar2 = 0x20;
                      do {
                        *playerRuntimePointerTableWriteCursor = pFVar9;
                        playerRuntimePointerTableWriteCursor =
                             playerRuntimePointerTableWriteCursor + 1;
                        pFVar9 = pFVar9 + 1;
                        iVar2 = iVar2 + -1;
                      } while (iVar2 != 0);
                      AVar19 = (*g_MemoryApi.alloc)(0xe00);
                      pbVar10 = (byte *)AVar19.eax;
                      if (AVar19.carry) {
                        return (dword)pbVar10;
                      }
                      g_CoreAssetScratchSlice1 = pbVar10 + 0x200;
                      g_CoreAssetScratchSlice2 = pbVar10 + 0x400;
                      g_CoreAssetScratchSlice3 = pbVar10 + 0x600;
                      g_CoreAssetScratchSlice4 = pbVar10 + 0x800;
                      g_CoreAssetScratchSlice5 = pbVar10 + 0xa00;
                      g_CoreAssetScratchSlice6 = pbVar10 + 0xc00;
                      g_CoreAssetScratchSlice0 = pbVar10;
                      for (iVar2 = 0x380; iVar2 != 0; iVar2 = iVar2 + -1) {
                        pbVar10[0] = 0;
                        pbVar10[1] = 0;
                        pbVar10[2] = 0;
                        pbVar10[3] = 0;
                        pbVar10 = pbVar10 + 4;
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
  Resource_Release(arg0);
  return (dword)module;
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
void __thandor_void_preserve_eax_ecx_edx Game_PlayIntroMovies(void)

{
  dword dVar1;
  dword arg0;
  uint uVar2;
  int iVar3;
  bool bVar4;
  MovieFrameDimensionsEdxEax8 MVar5;
  MovieOpenEaxCf5 MVar6;
  MovieAdvanceFrameEaxCf5 MVar7;
  MovieAdvanceFrameEaxCf5 MVar8;
  KeyboardEventEaxEdxCf9 KVar9;
  CommandLineFindOptionEbxCf5 CVar10;
  GraphicsCursorInputEventRegsCf21 GVar11;
  
  bVar4 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar4) {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  }
  bVar4 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar4) {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  }
  CVar10 = (*g_CommandLineFindOption)(8,s_NOINTRO_00573064);
  if (CVar10.carry) {
    while( true ) {
      MVar6 = Movie_Open(1,(word *)u_flm_intro0_flm_00573046);
      if (MVar6.carry) break;
      MVar7 = Movie_AdvanceFrame();
      if (MVar7.carry) {
        Movie_Close();
        return;
      }
      g_IntroMoviePendingTicks = 0;
      UiFrame_FlushInputAndResetPendingTicks();
      (*g_TimerRegisterPeriodic)(arg0,IntroMovie_TimerTick);
      while( true ) {
        (*g_Win32PumpMessages)();
        KVar9 = (*g_KeyboardReadEvent)();
        if (!KVar9.carry) break;
        GVar11 = (*g_GraphicsCursorConsumeEvent)();
        if ((!GVar11.carry) && (3 < GVar11.eventCode)) goto GameIntroMovies_StopCurrentPlayback;
        if (g_IntroMoviePendingTicks != 0) {
          iVar3 = 3;
          do {
            MVar8 = Movie_AdvanceFrame();
            dVar1 = g_FramebufferHeight;
            if (MVar8.carry) goto GameIntroMovies_StopCurrentPlayback;
            g_IntroMoviePendingTicks = g_IntroMoviePendingTicks - 1;
          } while ((g_IntroMoviePendingTicks != 0) && (iVar3 = iVar3 + -1, iVar3 != 0));
          uVar2 = g_FramebufferHeight >> 2;
          bVar4 = (*g_GraphicsFramebufferBeginAccess)();
          if (bVar4) goto GameIntroMovies_StopCurrentPlayback;
          MVar5 = Movie_GetFrameDimensions();
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (g_FramebufferHeight,g_FramebufferWidth,0,0,
                     ((int)((dVar1 - uVar2) - (int)(MVar5 >> 0x20)) >> 1) + (dVar1 >> 3),
                     (int)(g_FramebufferWidth - (int)MVar5) >> 1,0,
                     (GraphicsTextureSourceAsset *)MVar7.eax,g_FramebufferAccess);
          (*g_GraphicsFramebufferEndAccess)();
          (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
        }
      }
      if (KVar9.eventCode == 0x10000) {
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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx DynAPI_Bootstrap(void)

{
  void **in_EAX;
  HINSTANCE hModule;
  DynamicApiBinding *bindingCursor;
  StatusValueEaxCf5 SVar1;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  StatusValueEaxCf5 SVar4;
  void **lpProcName;
  char *moduleName;
  dword moduleSlotIndex;
  
  bindingCursor = g_BootstrapApiBindings;
  do {
    if (bindingCursor->destination == (void **)0x0) {
      SVar1.carry = false;
      SVar1.valueOrError = (dword)in_EAX;
      return SVar1;
    }
    lpProcName = bindingCursor->destination;
    hModule = GetModuleHandleA(bindingCursor->moduleName);
    if (hModule == (HMODULE)0x0) {
      if (bindingCursor->destination == (void **)dynapi_9) {
        Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)bindingCursor->moduleName);
        SVar3.carry = true;
        SVar3.valueOrError = 0xf;
        return SVar3;
      }
      hModule = (HINSTANCE)
                (*(code *)g_BootstrapApiBindings[0].destination)(bindingCursor->moduleName);
      moduleSlotIndex = g_DynamicModuleCount;
      if (hModule == (HINSTANCE)0x0) {
        Text_CopyNarrowToUtf16Cf(0x100,g_PackageLastErrorPath,(byte *)bindingCursor->moduleName);
        SVar2.carry = true;
        SVar2.valueOrError = 0x11;
        return SVar2;
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
      SVar4.carry = true;
      SVar4.valueOrError = 0x10;
      return SVar4;
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
CommandLineFindOptionEbxCf5 __thandor_ebx_cf_preserve_eax_ecx_edx
CommandLine_FindOption(CommandLineOptionLengthBytes length,char *option)

{
  dword compareBytesRemaining;
  int optionBufferCapacityRemaining;
  byte *in_EBX;
  char *pcVar1;
  char *optionBufferCursor;
  char *storedOptionCompareCursor;
  bool comparedBytesEqual;
  CommandLineFindOptionEbxCf5 CVar2;
  CommandLineFindOptionEbxCf5 CVar3;
  char currentOptionBufferByte;
  
  optionBufferCursor = g_CommandLine.optionBuffer;
  do {
    if (*optionBufferCursor == '\0') {
      CVar3.carry = true;
      CVar3.ebx = in_EBX;
      return CVar3;
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
      CVar2.carry = false;
      CVar2.ebx = (byte *)optionBufferCursor;
      return CVar2;
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
void __thandor_void_preserve_eax_ecx_edx CommandLine_Parse(void)

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

