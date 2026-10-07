/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/lifecycle.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/lifecycle.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/version.h>

#include <array>
#include <initializer_list>

/* Module data. */

uint32_t g_FrontendRuntimeFlags = 0;

GraphicsTextureSet *g_FrontendCentralTextureSet = nullptr;

GraphicsPaletteAsset *g_FrontendCentralPaletteAsset = nullptr;

GraphicsTextureSourceAsset *g_FrontendMenuTextureSource = nullptr;

RuntimeSpinLockValue g_FrontendStateTickSpinLock = 0;

SoundVoiceSet *g_FrontendMusicVoiceSet = nullptr;

uint16_t g_FrontendMusic00SamPathUtf16[18] = {'s', 'o', 'u', 'n', 'd', '\\', 'm', 'u', 's', 'i', 'c', '0', '0', '.', 's', 'a', 'm', 0}; /* L"sound\\music00.sam" */

static UiRootCallbacks g_FrontendUiRootCallbacks = {
    .frameUpdate = UI_SLOT(FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState),
    .keyboardFallback = UI_SLOT(FrontendRuntime_DispatchCommandByCodeAndModifierFlags)};

/* row pointer table of the frontend network backend list (display
   names), one entry per network backend; Frontend_Init fills it and hands it to the backend list control. The
   original addresses it on its own, right after the control offset tables, and reserves 256 entries. */
static Ptr32<uint16_t> g_FrontendNetworkBackendNameRows[256] = {};

static void *g_FrontendCentralRomAsset = nullptr;

static WorldObjectRecord *g_FrontendWorldObjectRecords = nullptr;

static uint16_t g_GfxTexturenZentraleGfxPathUtf16[26] = {'g', 'f', 'x', '\\', 't', 'e', 'x', 't', 'u', 'r', 'e', 'n', '\\', 'z', 'e', 'n', 't', 'r', 'a', 'l', 'e', '.', 'g', 'f', 'x', 0}; /* L"gfx\\texturen\\zentrale.gfx" */

static uint16_t g_GfxTexturenZentralePalPathUtf16[26] = {'g', 'f', 'x', '\\', 't', 'e', 'x', 't', 'u', 'r', 'e', 'n', '\\', 'z', 'e', 'n', 't', 'r', 'a', 'l', 'e', '.', 'p', 'a', 'l', 0}; /* L"gfx\\texturen\\zentrale.pal" */

static uint16_t g_SoundMenue01SamPathUtf16[18] = {'s', 'o', 'u', 'n', 'd', '\\', 'm', 'e', 'n', 'u', 'e', '0', '1', '.', 's', 'a', 'm', 0}; /* L"sound\\menue01.sam" */

static uint16_t g_GfxPanelMenueGfxPathUtf16[20] = {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 'm', 'e', 'n', 'u', 'e', '.', 'g', 'f', 'x', 0}; /* L"gfx\\panel\\menue.gfx" */

/* Menu sounds of Frontend_Init: counts the two digits of "sound\menue01.sam" from 01 up to 99 into the
   voice-set table slots 1..99 and stops at the first file that does not exist. Returns 0, or the voice-set
   creation error. */
static uint32_t FrontendInit_LoadMenuSounds()
{
  SoundVoiceSet **voiceSetSlot;
  SoundSampleAsset *loadedSample;
  SoundVoiceSet *menuVoiceSet;
  uint32_t voiceSetError;

  g_SoundMenue01SamPathUtf16[FRONTEND_MENU_SOUND_PATH_TENS_DIGIT] = L'0';
  g_SoundMenue01SamPathUtf16[FRONTEND_MENU_SOUND_PATH_ONES_DIGIT] = L'1';
  voiceSetSlot = &g_FrontendMenuSoundVoiceSets[1];
  while ((uint16_t)g_SoundMenue01SamPathUtf16[FRONTEND_MENU_SOUND_PATH_TENS_DIGIT] < L'9' + 1) {
    while ((uint16_t)g_SoundMenue01SamPathUtf16[FRONTEND_MENU_SOUND_PATH_ONES_DIGIT] < L'9' + 1) {
      if (!Resource_Load(g_SoundMenue01SamPathUtf16,reinterpret_cast<void **>(&loadedSample),nullptr,nullptr)) {
        return 0;
      }
      voiceSetError = g_SoundCreateSampleVoiceSet(loadedSample,&menuVoiceSet);
      if (voiceSetError != 0) {
        Resource_Release(loadedSample);
        return voiceSetError;
      }
      *voiceSetSlot = menuVoiceSet;
      Resource_Release(loadedSample);
      g_SoundMenue01SamPathUtf16[FRONTEND_MENU_SOUND_PATH_ONES_DIGIT] =
           g_SoundMenue01SamPathUtf16[FRONTEND_MENU_SOUND_PATH_ONES_DIGIT] + 1;
      voiceSetSlot++;
    }
    g_SoundMenue01SamPathUtf16[FRONTEND_MENU_SOUND_PATH_TENS_DIGIT] =
         g_SoundMenue01SamPathUtf16[FRONTEND_MENU_SOUND_PATH_TENS_DIGIT] + 1;
    g_SoundMenue01SamPathUtf16[FRONTEND_MENU_SOUND_PATH_ONES_DIGIT] = L'0';
  }
  return 0;
}

/* Menu music: loads sound\music00.sam and plays it looping at the saved music gain (g_FrontendMusicVoiceSet /
   g_FrontendMusicActiveBuffer). Failures leave the menu silent. Used by Frontend_Init (when music is enabled)
   and by FrontendAudioSettings_SetMusicEnabled (when music is switched on). */
void FrontendMusic_StartMenuMusic()
{
  uint32_t musicGain;
  SoundSampleAsset *loadedSample;
  SoundVoiceSet *musicVoiceSet;
  SoundVoice *musicBuffer;

  musicBuffer = g_FrontendMusicActiveBuffer;
  if (Resource_Load(g_FrontendMusic00SamPathUtf16,reinterpret_cast<void **>(&loadedSample),nullptr,nullptr)) {
    if (g_SoundCreateSampleVoiceSet(loadedSample,&musicVoiceSet) != 0) {
      Resource_Release(loadedSample);
    }
    else {
      g_FrontendMusicVoiceSet = musicVoiceSet;
      Resource_Release(loadedSample);
      musicGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MUSIC_GAIN);
      if (!g_SoundPlayLooping(musicGain,musicGain,musicVoiceSet,&musicBuffer)) {
        g_SoundReleaseSampleVoiceSet(musicVoiceSet);
        g_FrontendMusicVoiceSet = nullptr;
        musicBuffer = g_FrontendMusicActiveBuffer;
      }
    }
  }
  g_FrontendMusicActiveBuffer = musicBuffer;
}

/* Fills the frontend's network-backend list with the backends' display names (0x100 bytes apart); the row
   pointer table is g_FrontendNetworkBackendNameRows. */
static void FrontendInit_FillNetworkBackendList(FrontendRootResourceSlots *frontendUiState)
{
  uint32_t backendCount;
  uint32_t backendIndex;
  uint16_t *backendDisplayName;

  backendCount = g_NetworkBackendInstanceCount;
  if (g_NetworkBackendInstanceCount == 0) {
    return;
  }
  backendDisplayName = g_NetworkBackendInstanceTable->displayNameUtf16;
  for (backendIndex = 0; backendIndex < backendCount; backendIndex++) {
    g_FrontendNetworkBackendNameRows[backendIndex] = backendDisplayName;
    backendDisplayName = backendDisplayName + 128; /* 0x100 bytes */
  }
  UiPointerList_InitializeMeasuredTextRows /* networkProtocolList is a text list: the same prefix as a pointer list */
            (backendCount,reinterpret_cast<Ptr32<void> *>(g_FrontendNetworkBackendNameRows),
             reinterpret_cast<UiPointerListControl *>(&FrontendUi_Image(frontendUiState)->networkProtocolList));
}

/* Adapters from the generic model-pointer callback slots (FrontendModelPointerContext, graphics/render/types.h) to
   the menu room's handlers, whose parameters are typed differently: the hit metric is an int in the slot and a
   uint32_t in the handlers, the pointer context arrives as FrontendModelPointerHitContext * and the handlers take it
   as the FrontendPointerSceneRuntimeView of the same node (or as an unused uint32_t), the pointed model node as
   void * (hover) or as its 32-bit address (FrontendCallbackArgument5; every address is below 2 GB, core/ptr32.h).
   The press/drag/release slots return a value nobody reads (FrontendModelPointerContext_NonRight*), so the
   adapters of the void handlers return 0. */
static uint32_t FrontendMenuRoomSlot_UpdatePointerContextAndSceneView
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,int hitMetric,
          ModelRuntimeNode *pointedModelNode,FrontendModelPointerHitContext *pointerContext)

{
  return FrontendRuntime_UpdatePointerContextAndSceneView
                   (callbackArgument1,callbackArgument2,callbackArgument3,(uint32_t)hitMetric,pointedModelNode,
                    UiNode_As<FrontendPointerSceneRuntimeView>(&pointerContext->base));
}

static uint32_t FrontendMenuRoomSlot_PressNoOp
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,int hitMetric,
          ModelRuntimeNode *pointedModelNode,FrontendModelPointerHitContext *pointerContext)

{
  FrontendMenuRoom_PressNoOp(callbackArgument1,callbackArgument2,callbackArgument3,(uint32_t)hitMetric,
                             static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pointedModelNode)),static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pointerContext)));
  return 0;
}

static uint32_t FrontendMenuRoomSlot_DragNoOp
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,int hitMetric,
          ModelRuntimeNode *pointedModelNode,FrontendModelPointerHitContext *pointerContext)

{
  FrontendMenuRoom_DragNoOp(callbackArgument1,callbackArgument2,callbackArgument3,(uint32_t)hitMetric,
                            static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pointedModelNode)),static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pointerContext)));
  return 0;
}

static uint32_t FrontendMenuRoomSlot_ExecuteClickedRomAction
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,int hitMetric,
          ModelRuntimeNode *pointedModelNode,FrontendModelPointerHitContext *pointerContext)

{
  FrontendMenuRoom_ExecuteClickedRomAction
            (callbackArgument1,callbackArgument2,callbackArgument3,(uint32_t)hitMetric,
             static_cast<FrontendCallbackArgument5>(reinterpret_cast<uintptr_t>(pointedModelNode)),static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pointerContext)));
  return 0;
}

static void FrontendMenuRoomSlot_StopCameraFlight(FrontendModelPointerContext *pointerContext)

{
  FrontendMenuRoom_StopCameraFlight(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pointerContext)));
}

/* Installs the menu room's handlers on its 3D pointer-context control (menuRoomModelView), through the
   FrontendMenuRoomSlot_ adapters above where the handler's parameters differ from the slot's. */
static void FrontendInit_InstallMenuRoomPointerCallbacks(FrontendModelPointerContext *pointerContext)
{
  pointerContext->keyboardFallback = UI_SLOT(FrontendRuntime_DispatchCommandByCodeAndModifierFlags);
  pointerContext->hoverCursorCallback = FrontendMenuRoomSlot_UpdatePointerContextAndSceneView;
  pointerContext->heldButtonCursorCallback = FrontendMenuRoomSlot_UpdatePointerContextAndSceneView;
  pointerContext->buttonPressCallback = FrontendMenuRoomSlot_PressNoOp;
  pointerContext->buttonDragCallback = FrontendMenuRoomSlot_DragNoOp;
  pointerContext->buttonReleaseCallback = FrontendMenuRoomSlot_ExecuteClickedRomAction;
  pointerContext->clearTransientStateCallback = 0;
  pointerContext->rightClickCallback = FrontendMenuRoomSlot_StopCameraFlight;
  pointerContext->renderSpinLock = &g_FrontendStateTickSpinLock;
  pointerContext->renderSpinLockReleaseCallback = Frontend_StateTick;
}

/* Copies one saved name (PERSISTENT_SETTINGS_NAME_BYTES, 10 dwords) dword by dword. */
static void FrontendInit_CopyNameDwords(uint32_t *destination,const uint32_t *source)
{
  int remainingDwords;

  for (remainingDwords = PERSISTENT_SETTINGS_NAME_BYTES / sizeof(uint32_t); remainingDwords != 0;
       remainingDwords--) {
    *destination = *source;
    source++;
    destination++;
  }
}

/* Builds the frontend (menu) at the ROM record initialRomRecordId: clears the screen, loads the central
   texture set, palette, menu sounds (sound\menueNN.sam until the first missing one), engine\zentrale.rom and
   the menu music, creates the 0x5954-byte frontend root from its template and pushes it on the UI root stack,
   installs the 3D menu-room callbacks and activates the record's camera transition. It then reports this
   player ready and draws frames until every player is ready (network sessions wait here for the peers).
   Returns true on success (the root is g_FrontendRootNode); false with the failing call's error in *outError.
   FrontendRuntime_ShutdownAndReleaseResources undoes it.
*/
Bool8 Frontend_Init(RomRecordId initialRomRecordId,uint32_t *outError)

{
  uint32_t settingValue;
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendPlayerRuntimeBlockCount remainingBlockCount;
  GraphicsTextureSet *centralTextureSet;
  GraphicsPaletteAsset *centralPaletteAsset;
  uint32_t centralResourceErrorCode;
  uint32_t error;
  void *centralRomAsset;
  uint32_t romLoadErrorCode;
  void *allocPayload;
  uint32_t *zeroCursor;
  uint32_t *templateDwords;
  uint32_t *rootDwords;
  int remainingDwords;
  FrontendRootResourceSlots *frontendUiState;
  WorldRuntimeContext *worldRuntime;
  uint32_t *savedPlayerName;

  settingValue = PersistentSettings_Read(0,PERSISTENT_SETTING_TEXTURE_QUALITY);
  g_TextureDownsampleShift = settingValue >> 1;
  /* network session: every player starts not ready */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
    playerBlock = g_FrontendPlayerRuntimeBlocks;
    remainingBlockCount = g_FrontendPlayerRuntimeBlockCount;
    do {
      playerBlock->factionAssignment.readyOrWaitState = 0;
      playerBlock->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
      playerBlock++;
      remainingBlockCount--;
    } while (remainingBlockCount != 0);
  }
  if (!g_GraphicsFramebufferBeginAccess()) {
    g_GraphicsFramebufferFillRectArgb
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,UI_ARGB_OPAQUE_BLACK,g_FramebufferAccess); /* opaque black */
    g_GraphicsFramebufferEndAccess();
  }
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  g_CursorVisibilityToken++;
  g_FrontendRomTransitionTargetRecordId = 0;
  g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
  g_FrontendRuntimeFlags = FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS;
  g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
  g_FrontendStateTickSpinLock = 0;
  g_TimerRegisterPeriodic(FRONTEND_PERIODIC_TIMER_HZ,FrontendRuntime_TimerCountdownTick);
  UiRuntime_SetSynchronizationHooks(Frontend_StateTick,&g_FrontendStateTickSpinLock);
  centralTextureSet = g_GraphicsTextureSetLoadPackage(g_GfxTexturenZentraleGfxPathUtf16,
                                                      &centralResourceErrorCode);
  if (centralTextureSet == nullptr) {
    *outError = centralResourceErrorCode;
    return false;
  }
  g_FrontendCentralTextureSet = centralTextureSet;
  centralPaletteAsset = g_GraphicsPaletteAssetLoadPackage(g_GfxTexturenZentralePalPathUtf16,
                                                          &centralResourceErrorCode);
  if (centralPaletteAsset == nullptr) {
    *outError = centralResourceErrorCode;
    return false;
  }
  g_FrontendCentralPaletteAsset = centralPaletteAsset;
  RichTextCommandStream_PatchPayloadBySelector
            (0,g_FrontendNetworkEndpointTextUtf16,TextResource_Resolve(TEXT_ID_NETWORK_ADDRESS_TEMPLATE));
  error = FrontendInit_LoadMenuSounds();
  if (error != 0) {
    *outError = error;
    return false;
  }
  centralRomAsset = Package_LoadEntry(g_EngineZentraleRomPathUtf16,&romLoadErrorCode);
  if (centralRomAsset == nullptr) {
    *outError = romLoadErrorCode;
    return false;
  }
  g_FrontendCentralRomAsset = centralRomAsset;
  error = RomAsset_PrepareRecords(static_cast<RomAssetHeader *>(centralRomAsset));
  if (error != 0) {
    *outError = error;
    return false;
  }
  error = g_MemoryApi.alloc(FRONTEND_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord),&allocPayload);
  if (error != 0) {
    *outError = error;
    return false;
  }
  /* the world object records of the 3D menu room, zeroed */
  g_FrontendWorldObjectRecords = static_cast<WorldObjectRecord *>(allocPayload);
  zeroCursor = static_cast<uint32_t *>(allocPayload);
  for (remainingDwords = FRONTEND_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord) / 4; remainingDwords != 0;
       remainingDwords--) {
    *zeroCursor = 0;
    zeroCursor++;
  }
  error = g_MemoryApi.alloc(sizeof(FrontendUiImage),&allocPayload);
  if (error != 0) {
    *outError = error;
    return false;
  }
  frontendUiState = static_cast<FrontendRootResourceSlots *>(allocPayload);
  worldRuntime = FrontendModelPointerContext_AsWorldRuntime(&FrontendUi_Image(frontendUiState)->menuRoomModelView);
  /* the template image and the new root are copied dword by dword */
  templateDwords = reinterpret_cast<uint32_t *>(&g_FrontendRootInitializationTemplate);
  g_FrontendRootNode = FrontendUi_Image(frontendUiState);
  rootDwords = reinterpret_cast<uint32_t *>(frontendUiState);
  for (remainingDwords = sizeof(FrontendUiImage) / 4; remainingDwords != 0; remainingDwords--) {
    *rootDwords = *templateDwords;
    templateDwords++;
    rootDwords++;
  }
  FrontendMenu_BindSharedResources(frontendUiState);
  UiRootStack_Push(&g_FrontendUiRootCallbacks,&FrontendUi_Image(frontendUiState)->frontendRoot.root);
  if ((PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS) &
       PERSISTENT_SOUND_OPTION_MUSIC) != 0) {
    FrontendMusic_StartMenuMusic();
  }
  FrontendInit_FillNetworkBackendList(frontendUiState);
  /* the 3D pointer-context control of the menu room is the world runtime menuRoomModelView */
  FrontendInit_InstallMenuRoomPointerCallbacks(&FrontendUi_Image(frontendUiState)->menuRoomModelView);
  RecentTextHistory_SortAndBuildPointerList
            (5,reinterpret_cast<RecentTextHistoryPointerList *> /* lineCount and textLines are the list's count and entries */
               (&FrontendUi_Image(frontendUiState)->chatMessageHistory.lineCount));
  WorldRuntime_SetTerrainLightingConfiguration(0,0,0xffffffff,0,0,0,0,0,worldRuntime);
  WorldRuntime_AttachObjectArray(FRONTEND_WORLD_OBJECT_RECORD_COUNT,g_FrontendWorldObjectRecords,worldRuntime);
  if (RomRuntime_BuildAllRegistryNodeTrees(worldRuntime)) {
    /* The only failure source of RomRuntime_BuildAllRegistryNodeTrees is WorldObjectArray_AllocateFreeRecord's
       FATAL_ERROR_GENERAL_FAILURE (object array full), passed up unchanged through
       RomRuntime_BuildNodeTreeRecursive. */
    *outError = FATAL_ERROR_GENERAL_FAILURE;
    return false;
  }
  error = FrontendRomTransition_ActivateRecordById(initialRomRecordId,worldRuntime);
  if (error != 0) {
    *outError = error;
    return false;
  }
  /* Saved player name into the name field and g_FrontendLocalPlayerNameUtf16 (which is also the fallback),
     saved game name into the game-name field (10 dwords = 0x28 bytes each). */
  savedPlayerName = static_cast<uint32_t *>(PersistentSettings_GetRegionOrFallback
                      (PERSISTENT_SETTINGS_NAME_BYTES,g_FrontendLocalPlayerNameUtf16,PERSISTENT_SETTING_PLAYER_NAME));
  /* the name fields' UTF-16 text buffers, copied as dwords */
  FrontendInit_CopyNameDwords
            (reinterpret_cast<uint32_t *>(FrontendUi_Image(frontendUiState)->playerNameEdit.textBuffer),
             savedPlayerName);
  FrontendInit_CopyNameDwords(reinterpret_cast<uint32_t *>(g_FrontendLocalPlayerNameUtf16),savedPlayerName);
  FrontendInit_CopyNameDwords
            (reinterpret_cast<uint32_t *>(FrontendUi_Image(frontendUiState)->gameNameEdit.textBuffer),
             static_cast<const uint32_t *>(PersistentSettings_GetRegionOrFallback
                       (PERSISTENT_SETTINGS_NAME_BYTES,g_FrontendLocalPlayerNameUtf16,PERSISTENT_SETTING_GAME_NAME)));
  settingValue = PersistentSettings_Read(4,PERSISTENT_SETTING_NETWORK_PLAYER_COUNT);
  FrontendUi_Image(frontendUiState)->maxPlayersSlider.value = settingValue;
  UiFrame_FlushInputAndResetPendingTicks();
  g_SpinLockAcquire(&g_FrontendStateTickSpinLock);
  WorldMotionSpline_ClearCachedDerivatives();
  g_TimerRegisterPeriodic(FRONTEND_ROM_TRANSITION_TIMER_HZ,FrontendRomTransition_AdvanceElapsedTicks);
  FrontendCommand_Issue<FrontendPlayerRuntime_RecordReadyAndUpdateWaitState>(0,0,0);
  g_SpinLockRelease(&g_FrontendStateTickSpinLock);
  do {
    UiNode_InvalidateRoot(&FrontendUi_Image(frontendUiState)->frontendRoot.root.base);
    UiFrame_Update(0);
    UiFrame_Draw();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
    Frontend_StateTick();
  } while ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) != 0);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  UiFrame_FlushInputAndResetPendingTicks();
  return true;
}

/* Called by Frontend_Init for the freshly copied frontend UI: loads gfx\panel\menue.gfx as the texture of the
   menu panels (also kept in g_FrontendMenuTextureSource) and gives the buttons their click sounds, button
   sound voice sets 3 to 6 by control kind. Nothing is bound when the texture cannot be loaded.
*/
void FrontendMenu_BindSharedResources(FrontendRootResourceSlots *frontendUiState)

{
  FrontendUiImage *ui = FrontendUi_Image(frontendUiState);
  SoundVoiceSet *buttonVoiceSet;
  SoundVoiceSet *buttonVoiceSet5;
  GraphicsTextureSourceAsset *menuTexture;
  int controlIndex;

  menuTexture = g_GraphicsTextureSourceLoadPackageAsset(g_GfxPanelMenueGfxPathUtf16,nullptr);
  if (menuTexture != nullptr) {
    g_FrontendMenuTextureSource = menuTexture;
    frontendUiState->menuTextureSource_485C = menuTexture;
    frontendUiState->menuTextureSource_4F30 = menuTexture;
    frontendUiState->menuTextureSource_53D8 = menuTexture;
    frontendUiState->menuTextureSource_5720 = menuTexture;
    frontendUiState->menuTextureSource_2670 = menuTexture;
    frontendUiState->menuTextureSource_2D0C = menuTexture;
    frontendUiState->menuTextureSource_3738 = menuTexture;
    frontendUiState->menuTextureSource_3E64 = menuTexture;
    frontendUiState->menuTextureSource_24F8 = menuTexture;
    frontendUiState->menuTextureSource_1C8C = menuTexture;
    frontendUiState->menuTextureSource_0AE4 = menuTexture;
    frontendUiState->menuTextureSource_05E0 = menuTexture;
    buttonVoiceSet = g_UiButtonSoundVoiceSets7[3];
    frontendUiState->buttonVoiceSet3_0644 = g_UiButtonSoundVoiceSets7[3];
    frontendUiState->buttonVoiceSet3_0764 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_06A4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0704 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0B48 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0BA8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0C68 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1CF0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1D50 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1DB0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1E10 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1E70 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_255C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_25BC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_26D4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2790 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_27F0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2850 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2D70 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2DD0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_379C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_3EC8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_491C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_497C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_49DC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_4FF0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_5050 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_5498 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_54F8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_5558 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_57E0 = buttonVoiceSet;
    buttonVoiceSet = g_UiButtonSoundVoiceSets7[4];
    frontendUiState->buttonVoiceSet4_2AE0 = g_UiButtonSoundVoiceSets7[4];
    frontendUiState->buttonVoiceSet4_2B40 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2BF4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2C54 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2CB4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2EE0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2F48 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2FB0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3018 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3080 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_313C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_31A4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_320C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3274 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_32DC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3344 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_33AC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3414 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_347C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_34E4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_35A0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3608 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3670 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_36D8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3858 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_390C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3974 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_39DC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3A44 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3AAC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3B14 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3D4C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3DAC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3E0C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3F84 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3FE4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_4044 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_28B0 = buttonVoiceSet;
    controlIndex = 7;
    /* the seven faction, player and selection-row controls of the faction setup page (entries 1..7) */
    do {
      FrontendUi_Image(frontendUiState)->NodeAt<UiFramedTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[controlIndex - 1])->activationSound =
           buttonVoiceSet;
      FrontendUi_Image(frontendUiState)->NodeAt<UiFramedTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[controlIndex - 1])->activationSound =
           buttonVoiceSet;
      FrontendUi_Image(frontendUiState)->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[controlIndex - 1])->activationSound =
           buttonVoiceSet;
      buttonVoiceSet5 = g_UiButtonSoundVoiceSets7[5];
      controlIndex--;
    } while (controlIndex != 0);
    frontendUiState->buttonVoiceSet5_3C98 = g_UiButtonSoundVoiceSets7[5];
    frontendUiState->buttonVoiceSet5_41C0 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_433C = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_44B8 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_4634 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_514C = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_5210 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_0A8C = buttonVoiceSet5;
    buttonVoiceSet = g_UiButtonSoundVoiceSets7[6];
    frontendUiState->buttonVoiceSet6_2024 = g_UiButtonSoundVoiceSets7[6];
    frontendUiState->buttonVoiceSet6_21EC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_23CC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4AD4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4BD0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_5654 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4DC4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4EB0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_50BC = buttonVoiceSet;
    /* not in the original: the advanced settings page (background, and the sounds of the options page's buttons
       and of the display settings page's choices) */
    ui->advancedSettingsPage.textureSource = menuTexture;
    ui->advancedSettingsButton.activationSound =
         g_UiButtonSoundVoiceSets7[3];
    ui->advancedSettingsBackButton.activationSound =
         g_UiButtonSoundVoiceSets7[3];
    for (UiTextButtonControl *choice : {&ui->advancedEdgesSmooth,
                                        &ui->advancedEdgesExact,
                                        &ui->advancedUiScaleAuto,
                                        &ui->advancedUiScale1,
                                        &ui->advancedUiScale2,
                                        &ui->advancedUiScale3,
                                        &ui->advancedFrameLimitOff,
                                        &ui->advancedFrameLimit60,
                                        &ui->advancedFrameLimit120,
                                        &ui->advancedFrameLimit144,
                                        &ui->advancedVsyncCheckbox}) {
      choice->activationSound = g_UiButtonSoundVoiceSets7[4];
    }
  }
}

/* Tears down what Frontend_Init built, before a session starts, before the menu is rebuilt and when the game
   quits: removes the frame hooks and frontend timers, saves the root's state snapshot and pops/frees the
   frontend root, releases the ROM registry, world objects, central ROM, textures, palette, menu sounds and
   music, and flushes pending input.
*/
void FrontendRuntime_ShutdownAndReleaseResources()

{
  UiRootNode *root;
  int voiceSetsRemaining;
  SoundVoiceSet **voiceSetCursor;

  UiRuntime_SetSynchronizationHooks(nullptr,nullptr);
  g_TimerUnregisterPeriodic(FrontendRuntime_TimerCountdownTick);
  g_TimerUnregisterPeriodic(FrontendRomTransition_AdvanceElapsedTicks);
  root = &g_FrontendRootNode->frontendRoot.root;
  g_CursorVisibilityToken--;
  if (g_FrontendRootNode != nullptr) {
    FrontendTeardown_SaveStatusTextAndHostAddress(&g_FrontendRootNode->frontendRoot.root);
    UiRootStack_Pop(root);
    g_MemoryApi.free(root);
    g_FrontendRootNode = nullptr;
  }
  FrontendRomRegistry_ClearAndReleaseNestedResources();
  g_MemoryApi.free(g_FrontendWorldObjectRecords);
  g_FrontendWorldObjectRecords = nullptr;
  Resource_Release(g_FrontendCentralRomAsset);
  g_FrontendCentralRomAsset = nullptr;
  GraphicsShadingRuntime_ClearRecordTable();
  g_GraphicsTextureSetReleasePackage(g_FrontendCentralTextureSet);
  g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(g_FrontendCentralPaletteAsset);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_FrontendMenuTextureSource);
  g_FrontendCentralTextureSet = nullptr;
  g_FrontendCentralPaletteAsset = nullptr;
  g_FrontendMenuTextureSource = nullptr;
  /* all 100 menu sound slots (Frontend_Init fills 1..99) */
  voiceSetCursor = g_FrontendMenuSoundVoiceSets;
  voiceSetsRemaining = 100;
  do {
    if (*voiceSetCursor != nullptr) {
      g_SoundReleaseSampleVoiceSet(*voiceSetCursor);
    }
    *voiceSetCursor = nullptr;
    voiceSetCursor++;
    voiceSetsRemaining--;
  } while (voiceSetsRemaining != 0);
  g_SoundStopVoice(g_FrontendMusicActiveBuffer);
  g_SoundReleaseSampleVoiceSet(g_FrontendMusicVoiceSet);
  g_FrontendMusicActiveBuffer = nullptr;
  g_FrontendMusicVoiceSet = nullptr;
  SpriteAssetRegistry_Reset();
  UiFrame_FlushInputAndResetPendingTicks();
}

/* Not in the original (open-thandor): "Open Thandor 1.0.6" (THANDOR_PRODUCT_VERSION_STRING) at the bottom right of
   the main menu, in the small grey game font with its shadow. Shown while the frontend is the front root and the
   menu room shows the entry hall (ROM record FRONTEND_ROM_RECORD_MAIN_MENU) with no dialog page open: centred in
   the black bar below the room, or just above the bottom edge where the room reaches it. Called by UiFrame_Draw
   after the roots, before the tooltip. */
void FrontendVersionLabel_Draw()
{
  constexpr uint32_t kStyle = (0u << TEXT_STYLE_FONT_SHIFT) | (2u << TEXT_STYLE_PALETTE_SHIFT) | TEXT_STYLE_ALIGN_RIGHT;
  constexpr int kMargin = 8;
  static std::array<uint16_t, sizeof THANDOR_PRODUCT_VERSION_STRING> s_textUtf16 = [] {
    std::array<uint16_t, sizeof THANDOR_PRODUCT_VERSION_STRING> utf16 = {};
    for (std::size_t index = 0; index < utf16.size(); index++) {
      utf16[index] = static_cast<unsigned char>(THANDOR_PRODUCT_VERSION_STRING[index]); /* ASCII; ends with the 0 */
    }
    return utf16;
  }();
  FrontendUiImage *const frontendRoot = g_FrontendRootNode;
  uint32_t lineHeight;
  int barTop;
  int lineTop;

  if ((frontendRoot == nullptr) || (g_UiRootNode != &frontendRoot->frontendRoot.root) || (g_FrontendActiveRomRecord == nullptr) ||
      (g_FrontendActiveRomRecord->recordId != FRONTEND_ROM_RECORD_MAIN_MENU) ||
      (UiPageStack_ActivePageIndex(UiLayoutContainerControl_AsPageStack(&frontendRoot->frontendViewModeStack)) != 0) ||
      (UiPageStack_ActivePageIndex(UiLayoutContainerControl_AsPageStack(&frontendRoot->frontendPageStack)) !=
       FRONTEND_PAGE_MAIN)) {
    return;
  }
  const FrontendModelPointerContext *room =
       &frontendRoot->menuRoomModelView;
  if ((room->contextFlags & FRONTEND_MENU_ROOM_RENDER_SUPPRESSED) != 0) {
    return;
  }
  FontGlyph_GetLogicalSizeForStyle(kStyle,0,&lineHeight);
  barTop = room->base.bottom;
  if ((barTop < 0) || ((int)g_FramebufferHeight - barTop < (int)lineHeight + 4)) {
    lineTop = (int)g_FramebufferHeight - (int)lineHeight - kMargin;
  }
  else {
    lineTop = barTop + ((int)g_FramebufferHeight - barTop - (int)lineHeight) / 2;
  }
  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  RichTextCommandStream_DrawSingleLine
            (g_FramebufferHeight,g_FramebufferWidth,0,0,kStyle,s_textUtf16.data(),lineTop,
             (int)g_FramebufferWidth - kMargin);
  g_GraphicsFramebufferEndAccess();
}
