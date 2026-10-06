/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/loaded_session.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/loaded_session.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>
#include <thandor/assets/record_bytes.h>

/* Module data. */

SelectionPlayerRuntimeBlock *g_SelectionPlayerBlocks = nullptr;

/* Failure exit of InGameRuntime_InitializeLoadedSession: closes the level movie, releases the level entry and
   unmounts the save package (levelAsset is NULL and saveHandle 0 when they were not loaded yet), stores the error
   in *outError and returns false.
*/
static Bool8 InGameLoadedSession_Fail(FrontendLoadedLevelAsset *levelAsset,EngineFileHandle saveHandle,uint32_t error,
                                     uint32_t *outError)

{
  Movie_Close();
  Resource_Release(levelAsset);
  Package_Unmount(saveHandle);
  *outError = error;
  return false;
}

/* Sets the text of the in-game template's save-name edit (saveNameEdit, 32 code units) to the session name of a
   mounted save package: the UTF-16 string at offset 256 of the package header (scanned for at most 36
   characters), without its four-character file extension and cut to 31 characters. Without a terminator the name
   stays empty.
*/
static void InGameLoadedSession_ReadSessionName(EngineFileHandle saveHandle)

{
  uint8_t *headerBuffer;
  uint16_t *nameStart;
  uint16_t *scanEnd;
  uint16_t *sessionNameCursor;
  uint16_t *sourceCursor;
  uint32_t copyCount;
  int remainingCount;
  Bool8 terminatorFound;

  headerBuffer = g_PackageScratchBuffer;
  sessionNameCursor = reinterpret_cast<UiRequiredTextEditControl *>
                        (&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    *sessionNameCursor = 0;
    sessionNameCursor++;
  }
  g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,THANDOR_PTR(saveHandle));
  g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,headerBuffer,THANDOR_PTR(saveHandle));
  nameStart = Asset_RecordAt<uint16_t>(headerBuffer,256);
  terminatorFound = false;
  scanEnd = nameStart;
  for (remainingCount = 36; remainingCount != 0 && !terminatorFound; remainingCount--) {
    terminatorFound = *scanEnd == 0;
    scanEnd++;
  }
  if (!terminatorFound) {
    return;
  }
  /* scanEnd is just past the terminator: the four characters before the terminator (the file extension) are cut
     off */
  scanEnd[-3] = 0;
  scanEnd[-2] = 0;
  scanEnd[-5] = 0;
  scanEnd[-4] = 0;
  copyCount = (uint32_t)(scanEnd - nameStart);
  if (31 < copyCount) {
    copyCount = 31;
  }
  sessionNameCursor = reinterpret_cast<UiRequiredTextEditControl *>
                        (&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
  sourceCursor = nameStart;
  for (; copyCount != 0; copyCount--) {
    *sessionNameCursor = *sourceCursor;
    sourceCursor++;
    sessionNameCursor++;
  }
}

/* Resets the session state for a loaded game: clears all selection blocks and sets up only block 0 (local player 0
   with the saved faction), resets the end-movie, tick and ready state, and starts the periodic step timer and the
   UI synchronisation hooks.
*/
static void InGameLoadedSession_ResetSessionState(uint32_t savedFactionIndex)

{
  uint32_t *clearCursor;
  int remainingCount;

  clearCursor = reinterpret_cast<uint32_t *>(g_SelectionPlayerBlocks); /* dword clear */
  for (remainingCount = SELECTION_PLAYER_BLOCK_COUNT * sizeof(SelectionPlayerRuntimeBlock) / sizeof(uint32_t);
       remainingCount != 0; remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  g_EndMovieSelectionIndex = UINT32_MAX;
  g_EndMovieVariantIndex = 0;
  g_EndMoviePath = nullptr;
  g_LocalPlayerRuntimeId = 0;
  g_SelectionPlayerRuntimeBlockPointers[0] = g_SelectionPlayerBlocks;
  g_SelectionPlayerBlocks->factionIndex = savedFactionIndex;
  g_SelectionPlayerBlocks->simulationStepTicks = 1;
  InGameSession_ResetTickState();
  InGameSession_InstallStepTimerAndHooks();
}

/* Creates the in-game root (InGameSession_CreateRoot with the info slots of player block 0) and builds the level's
   scenario path. Returns true with the root in *outRoot; on failure returns false with the error in *outError.
*/
static Bool8 InGameLoadedSession_CreateRoot(FrontendLoadedLevelAsset *levelImage,InGameRuntimeRoot **outRoot,
                                           uint32_t *outError)

{
  /* block 0 starts with its selection slots (SelectionPointerArray32): the same 32 entity pointers */
  if (!InGameSession_CreateRoot(reinterpret_cast<SelectionInfoEntitySlots *>(g_SelectionPlayerBlocks),outRoot,outError)) {
    return false;
  }
  WidePath_CombineDirectoryAndLeaf
            (g_FrontendScenarioPathScratchUtf16,
             (levelImage->header).levelFileNameUtf16,g_ScenarioLevelDirectoryUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_LEV,g_FrontendScenarioPathScratchUtf16);
  return true;
}

/* Opens the level movie and shows its first frames, attaches the world arrays, loads the saved external tables and
   field grid with the level resources, clears the notification queue and creates the terrain texture. Returns
   true on success; on failure returns false with the error in *outError.
*/
static Bool8 InGameLoadedSession_LoadWorld(uint16_t *savePackagePath,FrontendLoadedLevelAsset *levelImage,
                                          InGameRuntimeRoot *inGameRoot,uint32_t *outError)

{
  WorldRuntimeContext *world;
  uint32_t localFactionIndex;
  void *fieldGrid;
  uint32_t packageLoadErrorCode;
  uint32_t stepError;

  world = &inGameRoot->worldRuntime;
  if (!InGameSession_OpenLoadingMovieAndAttachObjects
         (savePackagePath,reinterpret_cast<LevelAssetHeader *>(levelImage) /* the frontend view of the level image */,inGameRoot,outError)) {
    return false;
  }
  localFactionIndex = g_SelectionPlayerBlocks->factionIndex;
  world->activeFactionRuntimeIndex = (FactionRuntimeIndex)localFactionIndex;
  world->selection.activePlayerRuntimeId = 0;
  WorldRuntime_AttachAndClearDwordArray
            (INGAME_WORLD_DWORD_ARRAY_COUNT,g_InGameWorldRuntimeDwordArray256,world);
  if (GameData_LoadExternalTables()) {
    /* Original quirk: this failure reports the local player's faction index as its error code (a left-over
       intermediate value). */
    *outError = localFactionIndex;
    return false;
  }
  fieldGrid = Package_LoadEntry(g_FieldHexPathUtf16,&packageLoadErrorCode);
  if (fieldGrid == nullptr) {
    *outError = packageLoadErrorCode;
    return false;
  }
  (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid = Thandor_PointerToU32(fieldGrid); /* 32-bit format field: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
  if (!InGameLevelRuntime_LoadResourcesAfterExternalTables(levelImage,world,&stepError)) {
    *outError = stepError;
    return false;
  }
  return InGameSession_ClearNotificationsAndCreateTerrainTexture(inGameRoot,outError);
}

/* Takes the step spin lock and finishes the world while the step is held off: rebuilds the build/army/command
   grids, sets up the shading texture, mirrors the shading and mouse/panel options into the world runtime flags,
   allocates the grid scratch and rebuilds the derived terrain, influence, technology and lighting data. Returns
   true on success with the lock still held; on failure returns false with the error in *outError.
   Original quirk: the spin lock is not released on failure.
*/
static Bool8 InGameLoadedSession_FinishWorldUnderTickLock(InGameRuntimeRoot *inGameRoot,uint32_t *outError)

{
  WorldRuntimeContext *world;

  world = &inGameRoot->worldRuntime;
  /* g_InGameStateTickSpinLock is a uint32_t word; the spin lock API takes it as its int */
  g_SpinLockAcquire(reinterpret_cast<RuntimeSpinLockValue *>(&g_InGameStateTickSpinLock));
  InGameSession_RebuildUiGrids(inGameRoot);
  InGameSession_InitShadingAndMirrorViewOptions(world);
  if (!InGameSession_AllocateGridScratchAndRebuildDerived(world,outError)) {
    return false;
  }
  WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
  return true;
}

/* Continues a saved game: mounts the save package, takes the session name from its header, loads the campaign
   and level entries, and then follows the same steps as InGameRuntime_InitializeNewSession, except that the local
   player is always player 0 of a single block, the world comes from the saved external tables and field grid
   (InGameLevelRuntime_LoadResourcesAfterExternalTables) instead of a fresh level, and no intro notifications are
   queued. The package and the level entry are released again at the end. Returns true on success; on failure
   returns false and stores the error of the failing step in *outError.
*/
Bool8 InGameRuntime_InitializeLoadedSession(uint16_t *savePackagePath,uint32_t *outError)

{
  uintptr_t mountResult; /* the save package's handle, or the mount error code */
  EngineFileHandle saveHandle;
  void *campaignAsset;
  FrontendLoadedLevelAsset *levelImage;
  uint32_t packageLoadErrorCode;
  InGameRuntimeRoot *inGameRoot;
  uint32_t stepError;

  g_TextureDownsampleShift = PersistentSettings_Read(0,PERSISTENT_SETTING_TEXTURE_QUALITY);
  if (!Package_Mount(savePackagePath,&mountResult)) {
    return InGameLoadedSession_Fail(nullptr,0,(uint32_t)mountResult,outError);
  }
  saveHandle = mountResult;
  InGameLoadedSession_ReadSessionName(saveHandle);
  campaignAsset = Package_LoadEntry(g_CampagneHexPathUtf16,nullptr);
  if (campaignAsset != nullptr) {
    g_FrontendLoadedCampaignAsset = reinterpret_cast<uintptr_t>(campaignAsset);
  }
  levelImage = static_cast<FrontendLoadedLevelAsset *>(Package_LoadEntry(g_LevelHexPathUtf16,&packageLoadErrorCode));
  if (levelImage == nullptr) {
    return InGameLoadedSession_Fail(nullptr,saveHandle,packageLoadErrorCode,outError);
  }
  InGameLoadedSession_ResetSessionState(levelImage->playerSlots[6].aiClassOrMode);
  if (!InGameLoadedSession_CreateRoot(levelImage,&inGameRoot,&stepError) ||
      !InGameLoadedSession_LoadWorld(savePackagePath,levelImage,inGameRoot,&stepError) ||
      !InGameLoadedSession_FinishWorldUnderTickLock(inGameRoot,&stepError)) {
    return InGameLoadedSession_Fail(levelImage,saveHandle,stepError,outError);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags | 8;
  }
  else {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags & ~8u;
  }
  /* the spin lock taken by InGameLoadedSession_FinishWorldUnderTickLock is released here */
  InGameSession_ReportReadyAndWaitForPlayers(inGameRoot);
  Resource_Release(levelImage);
  Package_Unmount(saveHandle);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  g_TimerRegisterPeriodic(10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
  return true;
}
