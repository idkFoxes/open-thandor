/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/hud.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/hud.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

THANDOR_ALIGN(4) uint16_t *g_InGameFactionStatusTextScratchUtf16 = nullptr;

static uint16_t g_InGameHudNumberTextUtf16[16] = {};

static int32_t g_UiAction1012PlayerIndexTextOffsets[7] = {20540, 20632, 20724, 20816, 20908, 21000, 21092};

static int32_t g_UiAction1012PlayerLabelTextOffsets[7] = {21184, 21276, 21368, 21460, 21552, 21644, 21736};

static int32_t g_UiAction1012IconImageOffsets[7] = {22472, 22564, 22656, 22748, 22840, 22932, 23024};

static int32_t g_UiAction1012StateTextOffsets[7] = {0x5544, 0x55A0, 0x55FC, 0x5658, 0x56B4, 0x5710, 0x576C};

static int32_t g_UiAction1012ControlOffsets[7] = {23116, 23240, 23364, 23488, 23612, 23736, 23860};

static int32_t g_UiAction1012SlotPageOffsets[7] = {19924, 20012, 20100, 20188, 20276, 20364, 20452};

/* uint32_t[11]: sprite subresource index (0xA9..0xAB) of the diplomacy row's relation icon per relation state */
static const uint32_t g_UiAction1012SubresourceByState[11] = {0xA9, 0xA9, 0xA9, 0xA9, 0xAA, 0xAA, 0xAA, 0xA9, 0xAB, 0xAB, 0xAB};

static uint32_t g_UiAction1012TargetPlayerIndices[7] = {};

uint16_t *g_InGamePlayerListTextScratchUtf16 = nullptr;

InGamePlayerStatusTextSlot g_InGamePlayerStatusTextSlots[8] = {};

uint16_t g_EmptyFrontendPlayerNameUtf16[1] = {};

uint32_t g_InGameReadyStateToggleFlags = 0;

uint16_t g_FrontendCurrentFactionPrimaryResourceTextUtf16[16] = {};

/* UI action 0x1000 (g_InGameUiActionHandlersPage10[0]): a click on the minimap (InGameUiImage.minimapView).
   Latches the clicked grid
   cell (selectedSourceX/YQ12 = grid column/row into sourceOriginX/YQ12), converts it to world coordinates and
   moves both camera points of the
   world runtime (the worldView node of the same in-game UI copy) by the distance to the new centre, then clears
   the field grid dirty flag.
*/
void InGameMapAction_RecenterViewFromGridCoordinates(UiNodeBase *mapControl)

{
  int64_t scaledProduct;
  int xDelta;
  int yComponent;
  
  yComponent = ((UiSelectionGeometryControl *)mapControl)->selectedSourceYQ12;
  ((UiSelectionGeometryControl *)mapControl)->sourceOriginXQ12 = ((UiSelectionGeometryControl *)mapControl)->selectedSourceXQ12;
  ((UiSelectionGeometryControl *)mapControl)->sourceOriginYQ12 = yComponent;
  /* isometric grid to world: x = (2*gx + gy) * FIELD_GRID_WORLD_COLUMN_STEP_X / 2^13,
     y = gy * FIELD_GRID_WORLD_ROW_STEP_Y / 2^12 (64-bit products) */
  scaledProduct = (int64_t)(yComponent + ((UiSelectionGeometryControl *)mapControl)->selectedSourceXQ12 * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
/* the world runtime is the worldView node of the same in-game UI copy */
#define MAP_WORLD ((WorldRuntimeContext *)THANDOR_UI_SIBLING(mapControl,InGameUiImage,minimapView,worldView))
  xDelta = (FIXED_PRODUCT_SHR(scaledProduct, Q12_SHIFT + 1)) -
          MAP_WORLD->motion.targetPositionXQ12;
  yComponent = FIXED_PRODUCT_SHR((int64_t)yComponent * FIELD_GRID_WORLD_ROW_STEP_Y, Q12_SHIFT) -
          MAP_WORLD->motion.targetPositionYQ12;
  MAP_WORLD->motion.targetPositionXQ12 = MAP_WORLD->motion.targetPositionXQ12 + xDelta;
  MAP_WORLD->motion.targetPositionYQ12 = MAP_WORLD->motion.targetPositionYQ12 + yComponent;
  MAP_WORLD->motion.positionXQ12 = MAP_WORLD->motion.positionXQ12 + xDelta;
  MAP_WORLD->motion.positionYQ12 = MAP_WORLD->motion.positionYQ12 + yComponent;
  WorldRuntime_ClearFieldGridDirtyFlag(MAP_WORLD);
#undef MAP_WORLD
}

/* Network games: writes the roster of faction factionIndex into g_InGamePlayerListTextScratchUtf16 (player
   names separated by ", ", each followed by "  P" while a pause is requested, "  x<n>" for a game speed n > 1
   and a coloured "  W" while the player renders slowly) and returns the number of players on that faction.
   The original writes without a capacity check, so five or more players with long names (sent by the peers)
   on one faction run past the INGAME_PLAYER_LIST_TEXT_BYTES heap buffer. Bounded here: a piece that does not
   fit (with room for the terminator) ends the text there; the players are still counted. When the text fits,
   the output is the original's. */
static int InGameHud_FormatFactionRoster(uint32_t factionIndex)

{
  static Bool8 s_rosterTruncationLogged = false;
  SelectionPlayerRuntimeBlock *selectionBlock;
  uint32_t stepTicks;
  uint32_t playerIndex;
  FrontendPlayerRuntimeRecord *playerBlock;
  uint16_t *rosterCursor;
  uint16_t *rosterEnd;
  uint32_t copiedByteCount;
  uint32_t nameCapacityBytes;
  Bool8 truncated;
  int rosterCount;

  rosterCount = 0;
  truncated = false;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  rosterCursor = g_InGamePlayerListTextScratchUtf16;
  rosterEnd = g_InGamePlayerListTextScratchUtf16 + INGAME_PLAYER_LIST_TEXT_BYTES / sizeof(uint16_t);
  /* The original is a do-while that runs once and then wraps on a player count of 0; skipped here. */
  for (playerIndex = 0; playerIndex < g_FrontendPlayerRuntimeBlockCount; playerIndex++, playerBlock++) {
    if (factionIndex != (playerBlock->factionAssignment).factionAssignmentIndex) {
      continue;
    }
    if (truncated) {
      rosterCount++;
      continue;
    }
    if (rosterCount != 0) {
      if (rosterEnd - rosterCursor < 2 + 1) {
        truncated = true;
        rosterCount++;
        continue;
      }
      rosterCursor[0] = L',';
      rosterCursor[1] = L' ';
      rosterCursor += 2;
    }
    rosterCount++;
    /* the name gets at most 40 bytes (with its terminator), less when the buffer end is closer */
    nameCapacityBytes = 40;
    if ((uint32_t)(rosterEnd - rosterCursor) * 2 < nameCapacityBytes) {
      nameCapacityBytes = (uint32_t)(rosterEnd - rosterCursor) * 2;
    }
    if (RichTextCommandStream_CopyExpanded
          (nameCapacityBytes,rosterCursor,(playerBlock->playerName).textUtf16,&copiedByteCount)) {
      rosterCursor = (uint16_t *)((uint8_t *)rosterCursor + copiedByteCount);
      selectionBlock = g_SelectionPlayerRuntimeBlockPointers[playerBlock->playerRuntimeId];
      stepTicks = selectionBlock->simulationStepTicks;
      if ((selectionBlock->sessionFlags & PLAYER_SESSION_FLAG_PAUSE_REQUESTED) != 0) {
        /* "  P" */
        if (rosterEnd - rosterCursor < 4) {
          truncated = true;
          continue;
        }
        rosterCursor[0] = L' ';
        rosterCursor[1] = L' ';
        rosterCursor[2] = L'P';
        rosterCursor[3] = 0;
        rosterCursor += 3;
      }
      if (1 < stepTicks) {
        /* "  x<n>": the original stores 'x' and '0' + stepTicks as one dword (the low 16 bits of the sum) */
        if (rosterEnd - rosterCursor < 4 + 1) {
          truncated = true;
          continue;
        }
        rosterCursor[0] = L' ';
        rosterCursor[1] = L' ';
        rosterCursor[2] = L'x';
        rosterCursor[3] = (uint16_t)(L'0' + stepTicks);
        rosterCursor += 4;
      }
      if ((selectionBlock->sessionFlags & PLAYER_SESSION_FLAG_SLOW_RENDERING) != 0) {
        /* "  W" in rich-text save colour / palette colour 3 ... restore colour */
        if (rosterEnd - rosterCursor < 6 + 1) {
          truncated = true;
          continue;
        }
        rosterCursor[0] = L' ';
        rosterCursor[1] = L' ';
        rosterCursor[2] = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_SAVE_COLOR;
        rosterCursor[3] = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_3;
        rosterCursor[4] = L'W';
        rosterCursor[5] = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_RESTORE_COLOR;
        rosterCursor += 6;
      }
    }
    else if (nameCapacityBytes < 40) {
      /* the name was cut at the buffer end (CopyExpanded terminated it there) */
      truncated = true;
    }
    /* Original quirk: a name longer than 40 bytes is left cut and terminated, the cursor is not advanced and
       its marks are skipped, so the next separator overwrites it. */
  }
  if (truncated && !s_rosterTruncationLogged) {
    Thandor_Log("hud: roster of faction %u cut at %u code units",factionIndex,
                (uint32_t)(rosterEnd - g_InGamePlayerListTextScratchUtf16));
    s_rosterTruncationLogged = true;
  }
  *rosterCursor = 0;
  return rosterCount;
}

/* Per-tick HUD text update. Every 20 ticks (one second) it formats the render statistics into the debug overlay
   and reports the local player as slow (fewer than 13 frames in that second) or no longer slow; every tick it
   formats the camera pose, the selection point, free memory and the elapsed game time, and builds the faction
   status lines (name, player roster with pause/speed/slow marks, a counter) for the active factions 1..7.
*/
void InGameHud_UpdateStatusCountersAndSessionPrompts()

{
  uint64_t elapsedMinutes;
  uint32_t value;
  uint32_t frameDivisor;
  uint32_t factionIndex;
  WorldRuntimeContext *world;
  GameFactionRuntimeRecord *factionRecord;
  int rosterCount;
  uint16_t *rosterText;
  uint16_t *destination;
  uint32_t copiedByteCount;
  uint16_t *factionName;
  uint16_t *statusTemplate;
  WorldCameraPosition cameraPosition;
  WorldCameraOrientation cameraOrientation;
  InGameRuntimeRoot *runtimeRoot;

  frameDivisor = g_RenderedFrameCountSinceDebugRefresh;
  g_DebugOverlayCounterRefreshCountdown--;
  if (g_DebugOverlayCounterRefreshCountdown == 0) {
    g_DebugOverlayCounterRefreshCountdown = 20;
    /* frames, then draw calls / texture binds / texture reloads per frame (2 decimals) */
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,g_RenderedFrameCountSinceDebugRefresh,
               g_FrontendDebugOverlayTextSlot00Utf16);
    if (frameDivisor == 0) {
      frameDivisor = 1;
    }
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,frameDivisor,
               g_PrimitiveDrawCallCount,g_FrontendDebugOverlayTextSlot01Utf16);
    /* texture binds and texture reloads: the original's hardware counters; the software renderer has
       neither, so both print 0 */
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,frameDivisor,
               0,g_FrontendDebugOverlayTextSlot02Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,frameDivisor,
               0,g_FrontendDebugOverlayTextSlot03Utf16);
    /* bit 0 of g_InGameReadyStateToggleFlags: the slow state is currently reported */
    if ((g_InGameReadyStateToggleFlags & 1) == 0) {
      if (g_RenderedFrameCountSinceDebugRefresh < 13) {
        InGameCommand_Issue<FrontendPlayerRuntime_SetSlowRenderingFlagById>(0,0,PLAYER_SESSION_FLAG_SLOW_RENDERING);
        g_InGameReadyStateToggleFlags = g_InGameReadyStateToggleFlags ^ 1;
      }
    }
    else if (12 < g_RenderedFrameCountSinceDebugRefresh) {
      InGameCommand_Issue<FrontendPlayerRuntime_SetSlowRenderingFlagById>(0,0,0);
      g_InGameReadyStateToggleFlags = g_InGameReadyStateToggleFlags ^ 1;
    }
    g_RenderedFrameCountSinceDebugRefresh = 0;
    g_PrimitiveDrawCallCount = 0;
  }
  runtimeRoot = g_InGameRuntimeRoot;
  world = &g_InGameRuntimeRoot->worldRuntime;
  cameraPosition = WorldRuntime_GetCameraPosition(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraPosition.xQ12,g_FrontendDebugOverlayTextSlot04Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraPosition.yQ12,g_FrontendDebugOverlayTextSlot05Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraPosition.zQ12,g_FrontendDebugOverlayTextSlot06Utf16);
  cameraOrientation = WorldRuntime_GetCameraOrientation(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraOrientation.magnitudeQ12,g_FrontendDebugOverlayTextSlot07Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraOrientation.headingAngle,g_FrontendDebugOverlayTextSlot08Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,cameraOrientation.pitchAngle,g_FrontendDebugOverlayTextSlot09Utf16);
  /* selection point, or "-" while pointerSurfaceHitDepth holds the 0x7FFFFFFF "none" marker */
  if ((runtimeRoot->worldRuntime).selection.pointerSurfaceHitDepth == INT32_MAX) {
    g_FrontendDebugOverlayTextSlot10Utf16[0] = L'-';
    g_FrontendDebugOverlayTextSlot10Utf16[1] = 0;
    g_FrontendDebugOverlayTextSlot11Utf16[0] = L'-';
    g_FrontendDebugOverlayTextSlot11Utf16[1] = 0;
  }
  else {
    WideNumber_FormatUtf16
              (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,
               10,1,(runtimeRoot->worldRuntime).selection.pointerSurfaceHitWorldX,
               g_FrontendDebugOverlayTextSlot10Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,
               10,1,(runtimeRoot->worldRuntime).selection.pointerSurfaceHitWorldY,
               g_FrontendDebugOverlayTextSlot11Utf16);
  }
  value = g_MemoryApi.queryFreeBytes();
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_HEXADECIMAL,0,10,1,value,
             g_FrontendDebugOverlayTextSlot12Utf16);
  /* 1200 simulation ticks = one minute at 20 ticks per second; rounded up, shown as hours and minutes */
  elapsedMinutes = (uint64_t)(g_GameFactionRuntimeImage.tail.simulationTick + 1199) / 1200;
  g_LocaleFormatTimeFieldsUtf16
            ((uint32_t)(elapsedMinutes / 60),(uint32_t)(elapsedMinutes % 60),g_FrontendDebugOverlayTextSlot13Utf16);
  /* faction 0 is skipped */
  factionRecord = (GameFactionRuntimeRecord *)THANDOR_ADDR(g_GameFactionRuntimeImage,sizeof(GameFactionRuntimeRecord));
  destination = g_InGameFactionStatusTextScratchUtf16;
  for (factionIndex = 1; factionIndex <= 7; factionIndex++, factionRecord++) {
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) &&
       (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] <
        FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED)) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 factionRecord->economyProgressScore + factionRecord->relationScore,(uint16_t *)THANDOR_ADDR(g_InGameHudNumberTextUtf16,0));
      rosterCount = 0;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        rosterCount = InGameHud_FormatFactionRoster(factionIndex);
      }
      if (rosterCount == 0) {
        /* Local session or no player on this faction: status template without a roster. */
        rosterText = TextResource_Resolve(TEXT_ID_FACTION_NO_ROSTER);
      }
      else {
        rosterText = TextResource_Resolve(TEXT_ID_FACTION_ROSTER_TEMPLATE);
        RichTextCommandStream_PatchPayloadBySelector(0,g_InGamePlayerListTextScratchUtf16,rosterText);
      }
      /* faction name, roster, and economyProgressScore + relationScore */
      factionName = TextResource_Resolve(factionRecord->colorIndex + TEXT_ID_FACTION_NAME_BASE);
      statusTemplate = TextResource_Resolve(TEXT_ID_FACTION_STATUS_TEMPLATE);
      RichTextCommandStream_PatchPayloadBySelector(0,factionName,statusTemplate);
      RichTextCommandStream_PatchPayloadBySelector(1,rosterText,statusTemplate);
      RichTextCommandStream_PatchPayloadBySelector(2,(void *)THANDOR_ADDR(g_InGameHudNumberTextUtf16,0),statusTemplate);
      if (RichTextCommandStream_CopyExpanded(1024,destination,statusTemplate,&copiedByteCount)) {
        destination = (uint16_t *)((uint8_t *)destination + copiedByteCount);
      }
    }
  }
}

/* Network games only: resizes the in-game player status box to one text line per player and formats each
   line into g_InGamePlayerStatusTextSlots (text 0xFF05 or 0xFF06 depending on the player's ready/wait state,
   with the player's name patched in). Runs under the in-game tick spin lock because the network code updates
   the player records.
*/
void InGamePanel_RebuildPlayerStatusRows(void *inGameRoot)

{
  FrontendPlayerRuntimeRecord *playerRecord;
  TextResourceId resourceId;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  int panelHalfHeight;
  InGamePlayerStatusTextSlot *destination;
  RichTextExtent textExtent;
  uint16_t *resolvedText;
  GraphicsTextureLogicalSize windowTextureSize;
  UiConditionalActionControl *statusBox;

  const SpinLockGuard tickLock((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    windowTextureSize = g_GraphicsTextureSourceGetLogicalSize(114,g_UiWindowTextureSource);
    textExtent = RichTextCommandStream_MeasureLine
                      (g_UiTextStyleNormal,(uint16_t *)g_GfxPanelPanel0GfxPathUtf16);
    panelHalfHeight = (textExtent.heightPixels * remainingPlayers >> 1) + windowTextureSize.logicalHeightPixels;
    destination = g_InGamePlayerStatusTextSlots;
    statusBox = UiConditionalActionTextBox_AsControl(&InGameUi_Image(inGameRoot)->playerStatusBox);
    statusBox->lineCount = remainingPlayers;
    (statusBox->base).bottomOffset = panelHalfHeight;
    (statusBox->base).topOffset = -panelHalfHeight;
    UiContainer_LayoutChildren((statusBox->base).parent);
    /* The original is a do-while that runs once and then wraps on a player count of 0; skipped here. */
    for (; remainingPlayers != 0; remainingPlayers--) {
      if ((playerRecord->factionAssignment).readyOrWaitState == 0) {
        resourceId = TEXT_ID_PLAYER_STATUS_STATE_ZERO;
      }
      else {
        resourceId = TEXT_ID_PLAYER_STATUS_STATE_SET;
      }
      resolvedText = TextResource_Resolve(resourceId);
      RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,resolvedText);
      RichTextCommandStream_CopyExpanded(128,destination->text,resolvedText,nullptr);
      destination++;
      playerRecord++;
    }
  }
}

/* Fills diplomacy row slotIndex for faction factionIndex (its record factionRecord): remembers the faction for
   the row button, shows the row page, sets the faction name, player number and relation texts, the name of
   the network player on that faction, and the relation icon (hidden again by the relationUiFlags rules). */
static void InGameDiplomacyPanel_FillRow(UiNodeBase *node,uint32_t slotIndex,uint32_t factionIndex,
          GameFactionRuntimeRecord *factionRecord)

{
  uint32_t relationState;
  int playerNameTextOffset;
  int iconButtonOffset;
  uint32_t iconSubresource;
  uint32_t *controlFlags;
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;

  g_UiAction1012TargetPlayerIndices[slotIndex] = factionIndex;
  UiPageStack_SetActiveIndex
            (0,(UiPageStackControl *)
               THANDOR_UI_AT(node,g_UiAction1012SlotPageOffsets[slotIndex]));
  /* the text fields hold text resource ids; the colour name of the faction's colorIndex */
  ((UiSingleLineTextControl *)((uint8_t *)node +g_UiAction1012PlayerLabelTextOffsets[slotIndex]))->text =
       (uint16_t *)(uintptr_t)(factionRecord->colorIndex + TEXT_ID_FACTION_NAME_BASE);
  ((UiSingleLineTextControl *)((uint8_t *)node +g_UiAction1012PlayerIndexTextOffsets[slotIndex]))->text =
       (uint16_t *)(uintptr_t)(factionIndex + TEXT_ID_PLAYER_NUMBER_BASE);
  relationState = g_GameFactionRuntimeImage.records
                  [reinterpret_cast<WorldRuntimeContext *>(&InGameUi_Image(node)->worldView)->activeFactionRuntimeIndex]
                  .packedRelationStates >> ((uint8_t)(factionIndex << 2) & SHIFT_COUNT_MASK) &
                  FACTION_RELATION_STATE_MASK;
  /* the original shifts the index left and back right around the nibble shift, which only clears its top
     two bits: the index itself is unchanged */
  factionIndex = factionIndex & 0x3fffffff;
  ((UiSingleLineTextControl *)((uint8_t *)node +g_UiAction1012StateTextOffsets[slotIndex]))->text =
       (uint16_t *)(uintptr_t)(relationState + TEXT_ID_DIPLOMATIC_RELATION_BASE);
  /* player name: empty, or in network games the name of the player assigned to this faction */
  playerNameTextOffset = g_UiAction1012IconImageOffsets[slotIndex];
  ((UiSingleLineTextControl *)((uint8_t *)node +playerNameTextOffset))->text = g_EmptyFrontendPlayerNameUtf16;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
    playerBlock = g_FrontendPlayerRuntimeBlocks;
    remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
    /* The original is a do-while that runs once and then wraps on a player count of 0; skipped here. */
    for (; remainingPlayerBlocks != 0; remainingPlayerBlocks--) {
      if ((playerBlock->factionAssignment).factionAssignmentIndex == factionIndex) {
        ((UiSingleLineTextControl *)((uint8_t *)node +playerNameTextOffset))->text = (uint16_t *)&playerBlock->playerName;
        break;
      }
      playerBlock++;
    }
  }
  /* the row's relation icon button: shown, with the sprite of the relation state; hidden again by the
     relationUiFlags rules */
  iconButtonOffset = g_UiAction1012ControlOffsets[slotIndex];
  iconSubresource = g_UiAction1012SubresourceByState[relationState];
  controlFlags = (uint32_t *)&THANDOR_UI_AT(node,iconButtonOffset)->nodeFlags;
  *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
  ((UiCommandSpriteButtonControl *)((uint8_t *)node +iconButtonOffset))->sprite.normalSubresourceStartOrDescriptor =
       iconSubresource;
  if (((g_GameFactionRuntimeImage.tail.relationUiFlags & 1) != 0) &&
     ((7 < relationState ||
      (((g_GameFactionRuntimeImage.tail.relationUiFlags & 2) != 0 &&
       ((3 < relationState || ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) != 0)))))))) {
    controlFlags = (uint32_t *)&THANDOR_UI_AT(node,iconButtonOffset)->nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  }
}

/* Rebuilds the diplomacy panel: one row (at most seven) per other active faction with its faction name,
   player number, relation state text and icon, and the name of the network player who controls it. The
   frame is sized for the row count (smaller offsets below 800 pixels width); the panel stays hidden with no
   other faction, while the world input is disabled, or when relationUiFlags bit 4 is set, and unused rows
   are switched to their empty page. g_UiAction1012TargetPlayerIndices keeps the faction of each row for the
   row buttons (action 0x1012).
*/
void InGameOtherPlayerCommand_RebuildTargetEntries(UiNodeBase *node)

{
  int countedFactionIndex;
  uint32_t candidateFactionIndex;
  GameFactionRuntimeRecord *candidateRecord;
  uint32_t remainingFactions;
  int frameExtraWidth;
  int frameExtraHeight;
  uint32_t slotIndex;
  UiGridDimensions gridDimensions;
  UiControlCount otherActiveCount;
  UiControlCount remainingRows;

  /* node becomes the in-game UI root (parent -1) */
  while (node->parent != UI_NODE_NONE) {
    node = node->parent;
  }
  InGameUiImage *image = InGameUi_Image(node);
  /* the world view node is the session's WorldRuntimeContext */
  WorldRuntimeContext *world = reinterpret_cast<WorldRuntimeContext *>(&image->worldView);
  countedFactionIndex = 1;
  otherActiveCount = 0;
  remainingFactions = g_GameFactionRuntimeImage.tail.activeFactionCount;
  /* Factions 1..activeFactionCount. The original is a do-while that runs once and then wraps on a count of 0,
     and a count of 8 or more (level data) reads past factionLifecycleStates[8]; bounded here to index 7. */
  for (; (remainingFactions != 0) && (countedFactionIndex < 8); remainingFactions--) {
    if (((g_GameFactionRuntimeImage.tail.factionLifecycleStates[countedFactionIndex] ==
          FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
         (countedFactionIndex != world->activeFactionRuntimeIndex)) &&
       ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0)) {
      otherActiveCount++;
    }
    countedFactionIndex++;
  }
  if (remainingFactions != 0) {
    static Bool8 s_factionCountLogged = false;
    if (!s_factionCountLogged) {
      Thandor_Log("diplomacy: activeFactionCount %u bounded to 7",g_GameFactionRuntimeImage.tail.activeFactionCount);
      s_factionCountLogged = true;
    }
  }
  gridDimensions = UiGrid_OneColumnDimensionsPacked(otherActiveCount);
  frameExtraWidth = (int)gridDimensions.columnCount * g_InGamePanelTextureSubresource32Width +
        g_InGamePanelTextureSubresource19Width + g_InGamePanelTextureSubresource20Width;
  frameExtraHeight = (int)gridDimensions.rowCount * g_InGamePanelTextureSubresource32Height +
        g_InGamePanelTextureSubresource18Height + g_InGamePanelTextureSubresource23Height;
  if ((int)g_FramebufferWidth < 800) {
    image->diplomacyFrame.base.leftOffset = -31;
    image->diplomacyFrame.base.rightOffset = -31;
    image->diplomacyFrame.base.topOffset = -100;
    image->diplomacyFrame.base.bottomOffset = -100;
  }
  else {
    image->diplomacyFrame.base.leftOffset = -39;
    image->diplomacyFrame.base.rightOffset = -39;
    image->diplomacyFrame.base.topOffset = -126;
    image->diplomacyFrame.base.bottomOffset = -126;
  }
  image->diplomacyFrame.base.leftOffset = image->diplomacyFrame.base.leftOffset - frameExtraWidth;
  image->diplomacyFrame.base.topOffset = image->diplomacyFrame.base.topOffset - frameExtraHeight;
  image->diplomacyPanel.selectable.base.nodeFlags =
       image->diplomacyPanel.selectable.base.nodeFlags | UI_NODE_SUPPRESSED;
  if ((otherActiveCount != 0) && ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) == 0)) {
    image->diplomacyPanel.selectable.base.nodeFlags =
         image->diplomacyPanel.selectable.base.nodeFlags & ~UI_NODE_SUPPRESSED;
  }
  (*image->diplomacyPanel.selectable.base.vtable->layout)(&image->diplomacyPanel.selectable.base);
  /* fill one row per other active faction, then switch the unused rows to their empty page */
  slotIndex = 0;
  candidateFactionIndex = 1;
  candidateRecord = (GameFactionRuntimeRecord *)THANDOR_ADDR(g_GameFactionRuntimeImage,sizeof(GameFactionRuntimeRecord)); /* records[1] */
  for (remainingRows = otherActiveCount; remainingRows != 0; candidateFactionIndex++, candidateRecord++) {
    if ((candidateFactionIndex != world->activeFactionRuntimeIndex) &&
       (g_GameFactionRuntimeImage.tail.factionLifecycleStates[candidateFactionIndex] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE)) {
      InGameDiplomacyPanel_FillRow(node,slotIndex,candidateFactionIndex,candidateRecord);
      slotIndex++;
      remainingRows--;
    }
  }
  for (; slotIndex < 7; slotIndex++) {
    UiPageStack_SetActiveIndex
              (1,(UiPageStackControl *)
                 THANDOR_UI_AT(node,g_UiAction1012SlotPageOffsets[slotIndex]));
  }
}

/* UI action 0x1012 (g_InGameUiActionHandlersPage10[18]): one of the seven relation buttons of the diplomacy
   rows. Finds the button's row through g_UiAction1012ControlOffsets and advances the relation of the local
   faction towards that row's faction (g_UiAction1012TargetPlayerIndices), or resets it when the activation
   carries UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK. Ignored while paused or with world input disabled.
*/
void InGameOtherPlayerCommand_DispatchSelectedTarget(UiCommandSpriteButtonControl *control)

{
  UiCommandSpriteButtonControl *rootControl;
  CommandPayload rowFactionIndex;
  FactionRuntimeIndex rootFactionValue;
  int slotIndex;

  /* the original reads rootControl[21].sprite.primaryTextureSource, which on the 32-bit layout is this field */
  static_assert(
                21 * sizeof(UiCommandSpriteButtonControl) +
                offsetof(UiCommandSpriteButtonControl,sprite.primaryTextureSource) ==
                offsetof(InGameUiImage,worldView) + offsetof(WorldRuntimeContext,activeFactionRuntimeIndex),
                "rootControl[21].sprite.primaryTextureSource overlays the world view's activeFactionRuntimeIndex");

  if ((g_UiCommandRuntimeFlags &
       (UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED | UI_COMMAND_RUNTIME_FLAG_PAUSED)) == 0) {
    rootControl = control;
    while ((rootControl->sprite).selectable.base.parent != UI_NODE_NONE) {
      rootControl = (UiCommandSpriteButtonControl *)(rootControl->sprite).selectable.base.parent;
    }
    slotIndex = 6;
    while ((int)((uintptr_t)control - (uintptr_t)rootControl) != g_UiAction1012ControlOffsets[slotIndex]) {
      slotIndex--;
      if (slotIndex < 0) {
        return;
      }
    }
    rowFactionIndex = g_UiAction1012TargetPlayerIndices[slotIndex];
    /* the local faction: the activeFactionRuntimeIndex of the in-game root's world runtime (the original
       reads it as rootControl[21].sprite.primaryTextureSource, see the static_assert above) */
    if ((control->activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK) == 0) {
      rootFactionValue =
           reinterpret_cast<WorldRuntimeContext *>(&InGameUi_Image(rootControl)->worldView)->activeFactionRuntimeIndex;
      InGameCommand_Issue<GameFactionRuntime_AdvancePairwiseRelationState>(0,rowFactionIndex,rootFactionValue);
    }
    else {
      rootFactionValue =
           reinterpret_cast<WorldRuntimeContext *>(&InGameUi_Image(rootControl)->worldView)->activeFactionRuntimeIndex;
      InGameCommand_Issue<GameFactionRuntime_ResetPairwiseRelationState>(0,rowFactionIndex,rootFactionValue);
    }
  }
}

/* Refreshes the HUD resource numbers of the active faction in the in-game root: Xenite and Tritium
   (current / storage limit), Energy demand / generation capacity, and baseline Energy supply plus the Tritium
   extraction rate. Q4 amounts are shown as whole units (>> 4); the Xenite amount is also formatted as text.
*/
void InGameHud_UpdateCurrentFactionMetricCache()

{
  XeniteAmountQ4 xeniteStorageLimit;
  TritiumAmountQ4 tritiumStorageLimit;
  int xeniteCurrentDisplay;
  FactionProgressAmountQ4 baselineEnergySupplyQ4;
  FactionProgressAmountQ4 energyGenerationCapacityQ4;
  FactionArmyContributionValue tritiumExtractionRate;
  InGameRuntimeRoot *runtimeRoot;
  int activeFactionIndex;
  
  runtimeRoot = g_InGameRuntimeRoot;
  activeFactionIndex = g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex;
  xeniteStorageLimit = g_GameFactionRuntimeImage.records[activeFactionIndex].xeniteStorageLimitQ4;
  xeniteCurrentDisplay = (int)g_GameFactionRuntimeImage.records[activeFactionIndex].xeniteCurrentQ4 >> 4;
  g_InGameRuntimeRoot->primaryResourceDisplayCurrent = xeniteCurrentDisplay;
  runtimeRoot->primaryResourceDisplayLimit = (int)xeniteStorageLimit >> 4;
  /* decimal, no fraction digits */
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,xeniteCurrentDisplay,
             g_FrontendCurrentFactionPrimaryResourceTextUtf16);
  tritiumStorageLimit = g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumStorageLimitQ4;
  baselineEnergySupplyQ4 = g_GameFactionRuntimeImage.records[activeFactionIndex].baselineEnergySupplyQ4;
  runtimeRoot->secondaryResourceDisplayCurrent =
       (int)g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumCurrentQ4 >> 4;
  runtimeRoot->secondaryResourceDisplayLimit = (int)tritiumStorageLimit >> 4;
  energyGenerationCapacityQ4 = g_GameFactionRuntimeImage.records[activeFactionIndex].energyGenerationCapacityQ4;
  tritiumExtractionRate =
       g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumExtractionRateQ4PerTick;
  runtimeRoot->energyDemandDisplay =
       (int)(g_GameFactionRuntimeImage.records[activeFactionIndex].suppliedEnergyDemandQ4 +
            g_GameFactionRuntimeImage.records[activeFactionIndex].unpoweredEnergyDemandQ4) >> 4;
  runtimeRoot->energyCapacityDisplay = (int)energyGenerationCapacityQ4 >> 4;
  /* the extraction rate is added unshifted, as in the original */
  runtimeRoot->baselineEnergySupplyDisplay =
       ((int)baselineEnergySupplyQ4 >> 4) + tritiumExtractionRate;
}
