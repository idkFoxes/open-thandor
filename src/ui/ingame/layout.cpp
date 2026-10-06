/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/layout.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/layout.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

THANDOR_ALIGN(4) GraphicsTextureSourceAsset *g_InGameDiagramTextureSource = nullptr;

THANDOR_ALIGN(8) GraphicsTextureSourceAsset *g_InGameTechnologyTextureSource = nullptr;

THANDOR_ALIGN(4) GraphicsTextureSourceAsset *g_InGameWindowTextureSource = nullptr;

THANDOR_ALIGN(16) GraphicsTextureSourceAsset *g_InGamePanelTextureSource = nullptr;

uint16_t g_GfxPanelPanel0GfxPathUtf16[21] = {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 'p', 'a', 'n', 'e', 'l', '0', '.', 'g', 'f', 'x', 0}; /* L"gfx\\panel\\panel0.gfx" */

static uint16_t g_GfxPanelTechGfxPathUtf16[19] = {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 't', 'e', 'c', 'h', '.', 'g', 'f', 'x', 0}; /* L"gfx\\panel\\tech.gfx" */

static uint16_t g_GfxPanelDiagram0GfxPathUtf16[23] = {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 'd', 'i', 'a', 'g', 'r', 'a', 'm', '0', '.', 'g', 'f', 'x', 0}; /* L"gfx\\panel\\diagram0.gfx" */

static uint16_t g_GfxPanelWindowGfxPathUtf16[21] = {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 'w', 'i', 'n', 'd', 'o', 'w', '.', 'g', 'f', 'x', 0}; /* L"gfx\\panel\\window.gfx" */

static int32_t g_InGamePanelTextureSubresource00Width = 0;

static int32_t g_InGamePanelTextureSubresource01Width = 0;

static int32_t g_InGamePanelTextureSubresource06Width = 0;

static int32_t g_InGamePanelTextureSubresource07Width = 0;

int32_t g_InGamePanelTextureSubresource19Width = 0;

int32_t g_InGamePanelTextureSubresource20Width = 0;

int32_t g_InGamePanelTextureSubresource32Width = 0;

static int32_t g_InGamePanelTextureSubresource33Width = 0;

static int32_t g_InGamePanelTextureSubresource02Height = 0;

static int32_t g_InGamePanelTextureSubresource03Height = 0;

static int32_t g_InGamePanelTextureSubresource04Height = 0;

static int32_t g_InGamePanelTextureSubresource05Height = 0;

static int32_t g_InGamePanelTextureSubresource36Height = 0;

static int32_t g_InGamePanelTextureSubresource37Height = 0;

static int32_t g_InGamePanelTextureSubresource06Height = 0;

static int32_t g_InGamePanelTextureSubresource00Height = 0;

static int32_t g_InGamePanelTextureSubresource07Height = 0;

int32_t g_InGamePanelTextureSubresource18Height = 0;

int32_t g_InGamePanelTextureSubresource23Height = 0;

int32_t g_InGamePanelTextureSubresource32Height = 0;

int32_t g_InGamePanelTextureSubresource02Width = 0;

int32_t g_InGamePanelTextureSubresource27Width = 0;

int32_t g_InGamePanelTextureSubresource28Width = 0;

int32_t g_InGamePanelTextureSubresource34Width = 0;

int32_t g_InGamePanelTextureSubresource26Height = 0;

int32_t g_InGamePanelTextureSubresource31Height = 0;

int32_t g_InGamePanelTextureSubresource34Height = 0;

SoundVoiceSet *g_UiButtonSoundVoiceSets7[7] = {};

/* Gives node the given edge offsets. */
static void InGameUiRuntime_SetEdgeOffsets
          (UiNodeBase *node,int32_t leftOffset,int32_t topOffset,int32_t rightOffset,int32_t bottomOffset)

{
  node->leftOffset = leftOffset;
  node->topOffset = topOffset;
  node->rightOffset = rightOffset;
  node->bottomOffset = bottomOffset;
}

/* The graphics variant digit in "gfx\panel\panel0.gfx" / "gfx\panel\diagram0.gfx" (0 below 800x600, 1 below
   1024x768, 2 otherwise) and the resource gauge geometry follow the display size. */
static void InGameUiRuntime_SelectDisplayModeLayout(UiRootNode *inGameRoot)

{
  InGameUiImage *const ui = InGameUi_Image(inGameRoot);
  uint16_t variantDigit;

  if ((g_FramebufferWidth < 800) || (g_FramebufferHeight < 600)) {
    variantDigit = L'0';
  }
  else if ((g_FramebufferWidth < 1024) || (g_FramebufferHeight < 768)) {
    variantDigit = L'1';
  }
  else {
    variantDigit = L'2';
  }
  g_GfxPanelPanel0GfxPathUtf16[INGAME_PANEL_GFX_PATH_VARIANT_DIGIT] = variantDigit;
  g_GfxPanelDiagram0GfxPathUtf16[INGAME_DIAGRAM_GFX_PATH_VARIANT_DIGIT] = variantDigit;
  if (variantDigit == L'0') {
    InGameUiRuntime_SetEdgeOffsets(&ui->xeniteGauge.base,36,6,94,13);
    InGameUiRuntime_SetEdgeOffsets(&ui->tritiumGauge.base,36,17,94,24);
    InGameUiRuntime_SetEdgeOffsets(&ui->energyGauge.base,36,28,94,35);
    InGameUiRuntime_SetEdgeOffsets(&ui->xeniteAmountText.base,4,5,31,13);
  }
  else {
    InGameUiRuntime_SetEdgeOffsets(&ui->xeniteGauge.base,44,9,110,16);
    InGameUiRuntime_SetEdgeOffsets(&ui->tritiumGauge.base,44,23,110,30);
    InGameUiRuntime_SetEdgeOffsets(&ui->energyGauge.base,44,37,110,44);
    InGameUiRuntime_SetEdgeOffsets(&ui->xeniteAmountText.base,4,7,39,15);
  }
}

/* Caches the subresource sizes of panel0.gfx that the side panel layout needs. */
static void InGameUiRuntime_CachePanelSubresourceSizes(GraphicsTextureSourceAsset *panelTexture)

{
  GraphicsTextureLogicalSize logicalSize;

  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,panelTexture);
  g_InGamePanelTextureSubresource00Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(1,panelTexture);
  g_InGamePanelTextureSubresource01Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(2,panelTexture);
  g_InGamePanelTextureSubresource02Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(6,panelTexture);
  g_InGamePanelTextureSubresource06Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(7,panelTexture);
  g_InGamePanelTextureSubresource07Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(27,panelTexture);
  g_InGamePanelTextureSubresource27Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(28,panelTexture);
  g_InGamePanelTextureSubresource28Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(19,panelTexture);
  g_InGamePanelTextureSubresource19Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(20,panelTexture);
  g_InGamePanelTextureSubresource20Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(34,panelTexture);
  g_InGamePanelTextureSubresource34Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(32,panelTexture);
  g_InGamePanelTextureSubresource32Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(33,panelTexture);
  g_InGamePanelTextureSubresource33Width = logicalSize.logicalWidthPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(2,panelTexture);
  g_InGamePanelTextureSubresource02Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(3,panelTexture);
  g_InGamePanelTextureSubresource03Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(4,panelTexture);
  g_InGamePanelTextureSubresource04Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(5,panelTexture);
  g_InGamePanelTextureSubresource05Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(36,panelTexture);
  g_InGamePanelTextureSubresource36Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(37,panelTexture);
  g_InGamePanelTextureSubresource37Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(6,panelTexture);
  g_InGamePanelTextureSubresource06Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,panelTexture);
  g_InGamePanelTextureSubresource00Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(7,panelTexture);
  g_InGamePanelTextureSubresource07Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(26,panelTexture);
  g_InGamePanelTextureSubresource26Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(31,panelTexture);
  g_InGamePanelTextureSubresource31Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(18,panelTexture);
  g_InGamePanelTextureSubresource18Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(23,panelTexture);
  g_InGamePanelTextureSubresource23Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(34,panelTexture);
  g_InGamePanelTextureSubresource34Height = logicalSize.logicalHeightPixels;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(32,panelTexture);
  g_InGamePanelTextureSubresource32Height = logicalSize.logicalHeightPixels;
}

/* Zeroes the edge offsets of the side panel frame parts and binds panel0.gfx to every panel control (its
   textureSource or primaryTextureSource field, depending on the control class). */
static void InGameUiRuntime_BindPanelTexture(UiRootNode *inGameRoot,GraphicsTextureSourceAsset *panelTexture)

{
  InGameUiImage *const ui = InGameUi_Image(inGameRoot);
  InGameUiRuntime_SetEdgeOffsets(&ui->sidePanelFrameLeftEdge.base,0,0,0,0);
  ui->sidePanelFrameLeftEdge.textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(&ui->sidePanelFrameRightEdge.base,0,0,0,0);
  ui->sidePanelFrameRightEdge.textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(&ui->sidePanelFrameTopCap.base,0,0,0,0);
  ui->sidePanelFrameTopCap.textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(&ui->sidePanelFrameMenuBar.base,0,0,0,0);
  ui->sidePanelFrameMenuBar.textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(&ui->sidePanelFrameInfoSection.base,0,0,0,0);
  ui->sidePanelFrameInfoSection.textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(&ui->sidePanelFrameBottomCap.base,0,0,0,0);
  ui->sidePanelFrameBottomCap.textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(&ui->resourcePanel.base,0,0,0,0);
  ui->resourcePanel.textureSource = panelTexture;
  ui->editorTabStripA.textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(&ui->gamePanelsArea.base,0,0,0,0);
  ui->gamePanelsArea.textureSource = panelTexture;
  ui->editorTabStripB.textureSource = panelTexture;
  ui->resourcePanelImageToggle8Popup.textureSource = panelTexture;
  ui->resourcePanelImageToggle9Popup.textureSource = panelTexture;
  ui->diplomacyFrame.textureSource = panelTexture;
  ui->buildCatalogFrame.textureSource = panelTexture;
  ui->specialBuildCatalogFrame.textureSource = panelTexture;
  ui->armyStockFrame.textureSource = panelTexture;
  ui->resourcePanelImageToggle8.textureSource = panelTexture;
  ui->editorModeTabTerrainHeight.primaryTextureSource = panelTexture;
  ui->resourcePanelImageToggle9.textureSource = panelTexture;
  ui->editorModeTabTerrainMaterial.primaryTextureSource = panelTexture;
  ui->resourcePanelIconButton.primaryTextureSource = panelTexture;
  ui->editorModeTabTerrainSmoothing.primaryTextureSource = panelTexture;
  ui->inGameMenuButton.primaryTextureSource = panelTexture;
  ui->missionObjectivesButton.primaryTextureSource = panelTexture;
  ui->countdownDisplayPanel.textureSource = panelTexture;
  ui->diplomacyPanel.textureSource = panelTexture;
  ui->editorModeTabRegion.primaryTextureSource = panelTexture;
  ui->buildCatalogPanel.textureSource = panelTexture;
  ui->editorModeTabUnitPlacement.primaryTextureSource = panelTexture;
  ui->specialBuildCatalogPanel.textureSource = panelTexture;
  /* Truncated template nodes (the image ends them before the control's last field, so InGameUiImage keeps them
     untyped): viewed as their control for the fields inside the node. */
  reinterpret_cast<UiSpriteButtonControl *>(&ui->editorModeTabObjectPlacement)->primaryTextureSource = panelTexture;
  ui->armyStockPanel.textureSource = panelTexture;
  ui->selectionGroupButton0.sprite.primaryTextureSource = panelTexture;
  ui->selectionGroupButton1.sprite.primaryTextureSource = panelTexture;
  ui->selectionGroupButton2.sprite.primaryTextureSource = panelTexture;
  ui->selectionGroupButton3.sprite.primaryTextureSource = panelTexture;
  ui->selectionGroupButton4.sprite.primaryTextureSource = panelTexture;
  ui->selectionGroupButton5.sprite.primaryTextureSource = panelTexture;
  ui->selectionGroupButton6.sprite.primaryTextureSource = panelTexture;
  ui->selectionGroupButton7.sprite.primaryTextureSource = panelTexture;
  ui->heightToolOption0.primaryTextureSource = panelTexture;
  ui->heightToolOption1.primaryTextureSource = panelTexture;
  ui->heightToolOption2.primaryTextureSource = panelTexture;
  ui->heightToolOption3.primaryTextureSource = panelTexture;
  ui->materialToolOption0.primaryTextureSource = panelTexture;
  ui->materialToolOption1.primaryTextureSource = panelTexture;
  ui->materialToolOption2.primaryTextureSource = panelTexture;
  ui->materialToolOption3.primaryTextureSource = panelTexture;
  ui->smoothingToolOption0.primaryTextureSource = panelTexture;
  ui->smoothingToolOption1.primaryTextureSource = panelTexture;
  ui->smoothingToolOption2.primaryTextureSource = panelTexture;
  ui->smoothingRelaxGatedButton.primaryTextureSource = panelTexture;
  ui->smoothingRelaxLandButton.primaryTextureSource = panelTexture;
  ui->unitPlacementOption0.primaryTextureSource = panelTexture;
  ui->unitPlacementOption2.primaryTextureSource = panelTexture;
  ui->unitPlacementOption1.primaryTextureSource = panelTexture;
  ui->objectPlacementOption0.primaryTextureSource = panelTexture;
  ui->objectPlacementOption2.primaryTextureSource = panelTexture;
  ui->objectPlacementOption1.primaryTextureSource = panelTexture;
  ui->regionToolOption0.primaryTextureSource = panelTexture;
  ui->regionToolOption1.primaryTextureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(&ui->minimapView.base,0,0,0,0);
  InGameUiRuntime_SetEdgeOffsets(&ui->modePreviewPageStack.base,0,0,0,0);
  reinterpret_cast<UiImageActionControl *>(&ui->notificationTargetButton)->textureSource = panelTexture;
  ui->heightToolPreview.textureSource = panelTexture;
  ui->smoothingToolPreview.textureSource = panelTexture;
  ui->regionToolPreview.textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(&ui->modeDetailPageStack.base,0,0,0,0);
  reinterpret_cast<UiSpriteButtonControl *>(&ui->singleSelectionUpgradeButton)->primaryTextureSource = panelTexture;
  ui->selectionDetailPanel.textureSource = panelTexture;
  ui->heightToolPanel.textureSource = panelTexture;
  ui->materialPalettePanel.textureSource = panelTexture;
  ui->smoothingToolPanel.textureSource = panelTexture;
  ui->unitPlacementPanel.textureSource = panelTexture;
  ui->objectPlacementPanel.textureSource = panelTexture;
  ui->regionToolPanel.textureSource = panelTexture;
  ui->buildCatalogEntry00.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry01.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry02.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry03.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry04.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry05.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry06.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry07.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry08.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry09.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry10.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry11.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry12.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry13.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry14.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry15.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry16.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry17.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry18.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry19.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry20.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry21.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry22.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry23.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry24.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry25.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry26.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry27.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry28.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry29.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry30.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry31.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry32.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry33.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry34.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry35.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry36.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry37.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry38.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry39.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry40.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry41.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry42.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry43.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry44.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry45.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry46.command.sprite.alternateTextureSource = panelTexture;
  ui->buildCatalogEntry47.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry00.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry01.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry02.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry03.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry04.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry05.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry06.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry07.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry08.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry09.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry10.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry11.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry12.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry13.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry14.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry15.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry16.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry17.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry18.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry19.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry20.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry21.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry22.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry23.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry24.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry25.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry26.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry27.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry28.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry29.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry30.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry31.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry32.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry33.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry34.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry35.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry36.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry37.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry38.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry39.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry40.command.sprite.alternateTextureSource = panelTexture;
  ui->specialBuildCatalogEntry41.command.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot00.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot01.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot02.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot03.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot04.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot05.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot06.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot07.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot08.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot09.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot10.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot11.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot12.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot13.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot14.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot15.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot16.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot17.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot18.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot19.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot20.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot21.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot22.sprite.alternateTextureSource = panelTexture;
  ui->armyStockSlot23.command.sprite.alternateTextureSource = panelTexture;
  ui->diplomacyRow1RelationButton.sprite.alternateTextureSource = panelTexture;
  ui->diplomacyRow1RelationButton.sprite.primaryTextureSource = panelTexture;
  ui->diplomacyRow2RelationButton.sprite.alternateTextureSource = panelTexture;
  ui->diplomacyRow2RelationButton.sprite.primaryTextureSource = panelTexture;
  ui->diplomacyRow3RelationButton.sprite.alternateTextureSource = panelTexture;
  ui->diplomacyRow3RelationButton.sprite.primaryTextureSource = panelTexture;
  ui->diplomacyRow4RelationButton.sprite.alternateTextureSource = panelTexture;
  ui->diplomacyRow4RelationButton.sprite.primaryTextureSource = panelTexture;
  ui->diplomacyRow5RelationButton.sprite.alternateTextureSource = panelTexture;
  ui->diplomacyRow5RelationButton.sprite.primaryTextureSource = panelTexture;
  ui->diplomacyRow6RelationButton.sprite.alternateTextureSource = panelTexture;
  ui->diplomacyRow6RelationButton.sprite.primaryTextureSource = panelTexture;
  ui->diplomacyRow7RelationButton.sprite.alternateTextureSource = panelTexture;
  ui->diplomacyRow7RelationButton.sprite.primaryTextureSource = panelTexture;
}

/* Sizes the side panel parts from the panel subresources: the frame and the pages inside it move left by the
   widths of subresources 1 and 2 (the left edge also by subresource 0), and down by the heights of the frame
   pieces above them (subresources 2, 36, 3, 37, 4); the bottom cap and the mode detail page end above
   subresources 0 and 5. */
static void InGameUiRuntime_SizeSidePanelFrame(UiRootNode *inGameRoot)

{
  InGameUiImage *const ui = InGameUi_Image(inGameRoot);
  UiNodeBase *leftEdge;
  UiNodeBase *rightEdge;
  UiNodeBase *topCap;
  UiNodeBase *menuBar;
  UiNodeBase *infoSection;
  UiNodeBase *bottomCap;
  UiNodeBase *minimap;
  UiNodeBase *modePreview;
  UiNodeBase *modeDetail;
  int32_t size;

  leftEdge = &ui->sidePanelFrameLeftEdge.base;
  rightEdge = &ui->sidePanelFrameRightEdge.base;
  topCap = &ui->sidePanelFrameTopCap.base;
  menuBar = &ui->sidePanelFrameMenuBar.base;
  infoSection = &ui->sidePanelFrameInfoSection.base;
  bottomCap = &ui->sidePanelFrameBottomCap.base;
  minimap = &ui->minimapView.base;
  modePreview = &ui->modePreviewPageStack.base;
  modeDetail = &ui->modeDetailPageStack.base;
  size = g_InGamePanelTextureSubresource01Width;
  leftEdge->leftOffset -= size;
  leftEdge->rightOffset -= size;
  rightEdge->leftOffset -= size;
  topCap->leftOffset -= size;
  topCap->rightOffset -= size;
  menuBar->leftOffset -= size;
  menuBar->rightOffset -= size;
  infoSection->leftOffset -= size;
  infoSection->rightOffset -= size;
  bottomCap->leftOffset -= size;
  bottomCap->rightOffset -= size;
  minimap->leftOffset -= size;
  minimap->rightOffset -= size;
  modePreview->leftOffset -= size;
  modePreview->rightOffset -= size;
  modeDetail->leftOffset -= size;
  modeDetail->rightOffset -= size;
  size = g_InGamePanelTextureSubresource02Width;
  leftEdge->leftOffset -= size;
  leftEdge->rightOffset -= size;
  topCap->leftOffset -= size;
  menuBar->leftOffset -= size;
  infoSection->leftOffset -= size;
  bottomCap->leftOffset -= size;
  minimap->leftOffset -= size;
  modePreview->leftOffset -= size;
  modeDetail->leftOffset -= size;
  leftEdge->leftOffset -= g_InGamePanelTextureSubresource00Width;
  ui->resourcePanel.base.leftOffset -= g_InGamePanelTextureSubresource06Width;
  ui->gamePanelsArea.base.leftOffset -= g_InGamePanelTextureSubresource07Width;
  size = g_InGamePanelTextureSubresource02Height;
  topCap->bottomOffset += size;
  menuBar->topOffset += size;
  menuBar->bottomOffset += size;
  infoSection->topOffset += size;
  infoSection->bottomOffset += size;
  minimap->topOffset += size;
  minimap->bottomOffset += size;
  modePreview->topOffset += size;
  modePreview->bottomOffset += size;
  modeDetail->topOffset += size;
  size = g_InGamePanelTextureSubresource36Height;
  menuBar->topOffset += size;
  menuBar->bottomOffset += size;
  infoSection->topOffset += size;
  infoSection->bottomOffset += size;
  minimap->bottomOffset += size;
  modePreview->topOffset += size;
  modePreview->bottomOffset += size;
  modeDetail->topOffset += size;
  size = g_InGamePanelTextureSubresource03Height;
  menuBar->bottomOffset += size;
  infoSection->topOffset += size;
  infoSection->bottomOffset += size;
  modePreview->topOffset += size;
  modePreview->bottomOffset += size;
  modeDetail->topOffset += size;
  size = g_InGamePanelTextureSubresource37Height;
  infoSection->topOffset += size;
  infoSection->bottomOffset += size;
  modePreview->bottomOffset += size;
  modeDetail->topOffset += size;
  size = g_InGamePanelTextureSubresource04Height;
  infoSection->bottomOffset += size;
  modeDetail->topOffset += size;
  size = g_InGamePanelTextureSubresource00Height;
  bottomCap->topOffset += size;
  bottomCap->bottomOffset += size;
  modeDetail->bottomOffset += size;
  size = g_InGamePanelTextureSubresource05Height;
  bottomCap->topOffset -= size;
  modeDetail->bottomOffset -= size;
  ui->resourcePanel.base.bottomOffset += g_InGamePanelTextureSubresource06Height;
  ui->gamePanelsArea.base.topOffset -= g_InGamePanelTextureSubresource07Height;
}

/* The menu buttons and the countdown share the menu bar's rows, the selection group buttons the info section's;
   all of them span the side panel frame horizontally. The selection group buttons then form a 4x2 grid (left
   +5/+36/+66/+97, top +17/+40). The world view ends where the side panel stack begins. */
static void InGameUiRuntime_PlaceMenuAndSelectionGroupButtons(UiRootNode *inGameRoot)

{
  InGameUiImage *const ui = InGameUi_Image(inGameRoot);
  static const int32_t groupButtonColumnShifts[4] = { 5, 36, 66, 97 };
  static const int32_t groupButtonRowShifts[2] = { 17, 40 };
  UiNodeBase *groupButtons[8];
  UiNodeBase *menuBar;
  UiNodeBase *infoSection;
  int32_t frameLeft;
  int32_t frameRight;
  int buttonIndex;

  menuBar = &ui->sidePanelFrameMenuBar.base;
  infoSection = &ui->sidePanelFrameInfoSection.base;
  frameLeft = ui->sidePanelFrameLeftEdge.base.leftOffset;
  frameRight = ui->sidePanelFrameRightEdge.base.rightOffset;
  InGameUiRuntime_SetEdgeOffsets
            (&ui->inGameMenuButton.selectable.base,frameLeft,menuBar->topOffset,frameRight,menuBar->bottomOffset);
  InGameUiRuntime_SetEdgeOffsets
            (&ui->missionObjectivesButton.selectable.base,frameLeft,menuBar->topOffset,frameRight,
             menuBar->bottomOffset);
  InGameUiRuntime_SetEdgeOffsets
            (&ui->countdownDisplayPanel.base,frameLeft,menuBar->topOffset,frameRight,
             menuBar->bottomOffset);
  groupButtons[0] = &ui->selectionGroupButton0.sprite.selectable.base;
  groupButtons[1] = &ui->selectionGroupButton1.sprite.selectable.base;
  groupButtons[2] = &ui->selectionGroupButton2.sprite.selectable.base;
  groupButtons[3] = &ui->selectionGroupButton3.sprite.selectable.base;
  groupButtons[4] = &ui->selectionGroupButton4.sprite.selectable.base;
  groupButtons[5] = &ui->selectionGroupButton5.sprite.selectable.base;
  groupButtons[6] = &ui->selectionGroupButton6.sprite.selectable.base;
  groupButtons[7] = &ui->selectionGroupButton7.sprite.selectable.base;
  for (buttonIndex = 0; buttonIndex < 8; buttonIndex++) {
    InGameUiRuntime_SetEdgeOffsets
              (groupButtons[buttonIndex],frameLeft + groupButtonColumnShifts[buttonIndex % 4],
               infoSection->topOffset + groupButtonRowShifts[buttonIndex / 4],frameRight,infoSection->bottomOffset);
  }
  InGameUiRuntime_SetEdgeOffsets(&ui->worldViewArea.base,0,0,frameLeft,0);
  InGameUiRuntime_SetEdgeOffsets(&ui->sidePanelStack.base,frameLeft,0,0,0);
}

/* Sets the edges of one grid cell: edges[n] is the right/bottom edge of column/row n (counted from the bottom right
   cell) and edges[n + 1] its left/top edge. */
static void InGameUiRuntime_PlaceGridCell
          (UiNodeBase *cell,const int32_t *columnEdges,const int32_t *rowEdges,int column,int row)

{
  cell->leftOffset = columnEdges[column + 1];
  cell->topOffset = rowEdges[row + 1];
  cell->rightOffset = columnEdges[column];
  cell->bottomOffset = rowEdges[row];
}

/* Cells of the build catalog, special build catalog and army stock grids, laid out from the bottom right: each
   further column/row moves one subresource-34 cell to the left/up. */
static void InGameUiRuntime_PlaceCatalogGridCells(UiRootNode *inGameRoot)

{
  InGameUiImage *const ui = InGameUi_Image(inGameRoot);
  int32_t columnEdges[9];
  int32_t rowEdges[8];
  int edgeIndex;

  columnEdges[0] = -g_InGamePanelTextureSubresource28Width;
  for (edgeIndex = 1; edgeIndex < 9; edgeIndex++) {
    columnEdges[edgeIndex] = columnEdges[edgeIndex - 1] - g_InGamePanelTextureSubresource34Width;
  }
  rowEdges[0] = -g_InGamePanelTextureSubresource31Height;
  for (edgeIndex = 1; edgeIndex < 8; edgeIndex++) {
    rowEdges[edgeIndex] = rowEdges[edgeIndex - 1] - g_InGamePanelTextureSubresource34Height;
  }
  /* build catalog: 4x6 cells, then 4 columns of 6 */
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry00.command.sprite.selectable.base,columnEdges,rowEdges,0,0);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry01.command.sprite.selectable.base,columnEdges,rowEdges,1,0);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry02.command.sprite.selectable.base,columnEdges,rowEdges,2,0);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry03.command.sprite.selectable.base,columnEdges,rowEdges,3,0);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry04.command.sprite.selectable.base,columnEdges,rowEdges,0,1);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry05.command.sprite.selectable.base,columnEdges,rowEdges,1,1);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry06.command.sprite.selectable.base,columnEdges,rowEdges,2,1);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry07.command.sprite.selectable.base,columnEdges,rowEdges,3,1);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry08.command.sprite.selectable.base,columnEdges,rowEdges,0,2);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry09.command.sprite.selectable.base,columnEdges,rowEdges,1,2);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry10.command.sprite.selectable.base,columnEdges,rowEdges,2,2);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry11.command.sprite.selectable.base,columnEdges,rowEdges,3,2);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry12.command.sprite.selectable.base,columnEdges,rowEdges,0,3);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry13.command.sprite.selectable.base,columnEdges,rowEdges,1,3);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry14.command.sprite.selectable.base,columnEdges,rowEdges,2,3);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry15.command.sprite.selectable.base,columnEdges,rowEdges,3,3);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry16.command.sprite.selectable.base,columnEdges,rowEdges,0,4);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry17.command.sprite.selectable.base,columnEdges,rowEdges,1,4);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry18.command.sprite.selectable.base,columnEdges,rowEdges,2,4);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry19.command.sprite.selectable.base,columnEdges,rowEdges,3,4);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry20.command.sprite.selectable.base,columnEdges,rowEdges,0,5);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry21.command.sprite.selectable.base,columnEdges,rowEdges,1,5);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry22.command.sprite.selectable.base,columnEdges,rowEdges,2,5);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry23.command.sprite.selectable.base,columnEdges,rowEdges,3,5);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry24.command.sprite.selectable.base,columnEdges,rowEdges,4,0);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry25.command.sprite.selectable.base,columnEdges,rowEdges,4,1);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry26.command.sprite.selectable.base,columnEdges,rowEdges,4,2);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry27.command.sprite.selectable.base,columnEdges,rowEdges,4,3);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry28.command.sprite.selectable.base,columnEdges,rowEdges,4,4);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry29.command.sprite.selectable.base,columnEdges,rowEdges,4,5);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry30.command.sprite.selectable.base,columnEdges,rowEdges,5,0);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry31.command.sprite.selectable.base,columnEdges,rowEdges,5,1);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry32.command.sprite.selectable.base,columnEdges,rowEdges,5,2);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry33.command.sprite.selectable.base,columnEdges,rowEdges,5,3);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry34.command.sprite.selectable.base,columnEdges,rowEdges,5,4);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry35.command.sprite.selectable.base,columnEdges,rowEdges,5,5);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry36.command.sprite.selectable.base,columnEdges,rowEdges,6,0);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry37.command.sprite.selectable.base,columnEdges,rowEdges,6,1);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry38.command.sprite.selectable.base,columnEdges,rowEdges,6,2);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry39.command.sprite.selectable.base,columnEdges,rowEdges,6,3);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry40.command.sprite.selectable.base,columnEdges,rowEdges,6,4);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry41.command.sprite.selectable.base,columnEdges,rowEdges,6,5);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry42.command.sprite.selectable.base,columnEdges,rowEdges,7,0);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry43.command.sprite.selectable.base,columnEdges,rowEdges,7,1);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry44.command.sprite.selectable.base,columnEdges,rowEdges,7,2);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry45.command.sprite.selectable.base,columnEdges,rowEdges,7,3);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry46.command.sprite.selectable.base,columnEdges,rowEdges,7,4);
  InGameUiRuntime_PlaceGridCell(&ui->buildCatalogEntry47.command.sprite.selectable.base,columnEdges,rowEdges,7,5);
  /* special build catalog: 4x7 cells, then 2 columns of 7 */
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry00.command.sprite.selectable.base,columnEdges,rowEdges,0,0);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry01.command.sprite.selectable.base,columnEdges,rowEdges,1,0);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry02.command.sprite.selectable.base,columnEdges,rowEdges,2,0);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry03.command.sprite.selectable.base,columnEdges,rowEdges,3,0);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry04.command.sprite.selectable.base,columnEdges,rowEdges,0,1);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry05.command.sprite.selectable.base,columnEdges,rowEdges,1,1);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry06.command.sprite.selectable.base,columnEdges,rowEdges,2,1);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry07.command.sprite.selectable.base,columnEdges,rowEdges,3,1);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry08.command.sprite.selectable.base,columnEdges,rowEdges,0,2);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry09.command.sprite.selectable.base,columnEdges,rowEdges,1,2);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry10.command.sprite.selectable.base,columnEdges,rowEdges,2,2);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry11.command.sprite.selectable.base,columnEdges,rowEdges,3,2);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry12.command.sprite.selectable.base,columnEdges,rowEdges,0,3);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry13.command.sprite.selectable.base,columnEdges,rowEdges,1,3);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry14.command.sprite.selectable.base,columnEdges,rowEdges,2,3);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry15.command.sprite.selectable.base,columnEdges,rowEdges,3,3);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry16.command.sprite.selectable.base,columnEdges,rowEdges,0,4);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry17.command.sprite.selectable.base,columnEdges,rowEdges,1,4);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry18.command.sprite.selectable.base,columnEdges,rowEdges,2,4);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry19.command.sprite.selectable.base,columnEdges,rowEdges,3,4);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry20.command.sprite.selectable.base,columnEdges,rowEdges,0,5);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry21.command.sprite.selectable.base,columnEdges,rowEdges,1,5);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry22.command.sprite.selectable.base,columnEdges,rowEdges,2,5);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry23.command.sprite.selectable.base,columnEdges,rowEdges,3,5);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry24.command.sprite.selectable.base,columnEdges,rowEdges,0,6);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry25.command.sprite.selectable.base,columnEdges,rowEdges,1,6);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry26.command.sprite.selectable.base,columnEdges,rowEdges,2,6);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry27.command.sprite.selectable.base,columnEdges,rowEdges,3,6);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry28.command.sprite.selectable.base,columnEdges,rowEdges,4,0);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry29.command.sprite.selectable.base,columnEdges,rowEdges,4,1);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry30.command.sprite.selectable.base,columnEdges,rowEdges,4,2);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry31.command.sprite.selectable.base,columnEdges,rowEdges,4,3);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry32.command.sprite.selectable.base,columnEdges,rowEdges,4,4);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry33.command.sprite.selectable.base,columnEdges,rowEdges,4,5);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry34.command.sprite.selectable.base,columnEdges,rowEdges,4,6);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry35.command.sprite.selectable.base,columnEdges,rowEdges,5,0);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry36.command.sprite.selectable.base,columnEdges,rowEdges,5,1);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry37.command.sprite.selectable.base,columnEdges,rowEdges,5,2);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry38.command.sprite.selectable.base,columnEdges,rowEdges,5,3);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry39.command.sprite.selectable.base,columnEdges,rowEdges,5,4);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry40.command.sprite.selectable.base,columnEdges,rowEdges,5,5);
  InGameUiRuntime_PlaceGridCell(&ui->specialBuildCatalogEntry41.command.sprite.selectable.base,columnEdges,rowEdges,5,6);
  /* army stock: 4x6 cells */
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot00.sprite.selectable.base,columnEdges,rowEdges,0,0);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot01.sprite.selectable.base,columnEdges,rowEdges,1,0);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot02.sprite.selectable.base,columnEdges,rowEdges,2,0);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot03.sprite.selectable.base,columnEdges,rowEdges,3,0);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot04.sprite.selectable.base,columnEdges,rowEdges,0,1);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot05.sprite.selectable.base,columnEdges,rowEdges,1,1);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot06.sprite.selectable.base,columnEdges,rowEdges,2,1);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot07.sprite.selectable.base,columnEdges,rowEdges,3,1);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot08.sprite.selectable.base,columnEdges,rowEdges,0,2);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot09.sprite.selectable.base,columnEdges,rowEdges,1,2);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot10.sprite.selectable.base,columnEdges,rowEdges,2,2);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot11.sprite.selectable.base,columnEdges,rowEdges,3,2);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot12.sprite.selectable.base,columnEdges,rowEdges,0,3);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot13.sprite.selectable.base,columnEdges,rowEdges,1,3);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot14.sprite.selectable.base,columnEdges,rowEdges,2,3);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot15.sprite.selectable.base,columnEdges,rowEdges,3,3);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot16.sprite.selectable.base,columnEdges,rowEdges,0,4);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot17.sprite.selectable.base,columnEdges,rowEdges,1,4);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot18.sprite.selectable.base,columnEdges,rowEdges,2,4);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot19.sprite.selectable.base,columnEdges,rowEdges,3,4);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot20.sprite.selectable.base,columnEdges,rowEdges,0,5);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot21.sprite.selectable.base,columnEdges,rowEdges,1,5);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot22.sprite.selectable.base,columnEdges,rowEdges,2,5);
  InGameUiRuntime_PlaceGridCell(&ui->armyStockSlot23.command.sprite.selectable.base,columnEdges,rowEdges,3,5);
  /* the technology description scroll's left edge and frame's right edge move one catalog cell width */
  ui->technologyDescriptionScroll.base.leftOffset += g_InGamePanelTextureSubresource34Width;
  ui->technologyDescriptionFrame.base.rightOffset += g_InGamePanelTextureSubresource34Width;
}

/* Shifts the right edge of one diplomacy row's label columns and places its relation button. */
static void InGameUiRuntime_ShiftDiplomacyRowLabels
          (UiNodeBase *relationButton,UiNodeBase *playerNumberLabel,UiNodeBase *relationLabel,
           UiNodeBase *playerNameLabel,UiNodeBase *factionLabel,int32_t labelShift)

{
  relationButton->leftOffset = labelShift;
  playerNumberLabel->rightOffset += labelShift;
  relationLabel->rightOffset += labelShift;
  playerNameLabel->rightOffset += labelShift;
  factionLabel->rightOffset += labelShift;
}

/* The seven diplomacy rows, one subresource-32 height apart from the bottom, then their label columns. */
static void InGameUiRuntime_PlaceDiplomacyRows(UiRootNode *inGameRoot)

{
  InGameUiImage *const ui = InGameUi_Image(inGameRoot);
  UiNodeBase *rows[7];
  int32_t rowLeftOffset;
  int32_t rowRightOffset;
  int32_t rowBottomShift;
  int32_t rowTopShift;
  int32_t labelShift;
  int rowIndex;

  rows[0] = &ui->diplomacyRow1.base;
  rows[1] = &ui->diplomacyRow2.base;
  rows[2] = &ui->diplomacyRow3.base;
  rows[3] = &ui->diplomacyRow4.base;
  rows[4] = &ui->diplomacyRow5.base;
  rows[5] = &ui->diplomacyRow6.base;
  rows[6] = &ui->diplomacyRow7.base;
  rowLeftOffset = g_InGamePanelTextureSubresource19Width;
  rowRightOffset = -g_InGamePanelTextureSubresource20Width;
  rowBottomShift = g_InGamePanelTextureSubresource23Height;
  for (rowIndex = 0; rowIndex < 7; rowIndex++) {
    rowTopShift = rowBottomShift + g_InGamePanelTextureSubresource32Height;
    rows[rowIndex]->bottomOffset -= rowBottomShift;
    rows[rowIndex]->leftOffset = rowLeftOffset;
    rows[rowIndex]->rightOffset = rowRightOffset;
    rows[rowIndex]->topOffset -= rowTopShift;
    rowBottomShift = rowTopShift;
  }
  labelShift = -g_InGamePanelTextureSubresource33Width;
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (&ui->diplomacyRow1RelationButton.sprite.selectable.base,&ui->diplomacyRow1PlayerNumberLabel.base,
             &ui->diplomacyRow1RelationLabel.base,&ui->diplomacyRow1PlayerNameLabel.base,
             &ui->diplomacyRow1FactionLabel.base,labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (&ui->diplomacyRow2RelationButton.sprite.selectable.base,&ui->diplomacyRow2PlayerNumberLabel.base,
             &ui->diplomacyRow2RelationLabel.base,&ui->diplomacyRow2PlayerNameLabel.base,
             &ui->diplomacyRow2FactionLabel.base,labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (&ui->diplomacyRow3RelationButton.sprite.selectable.base,&ui->diplomacyRow3PlayerNumberLabel.base,
             &ui->diplomacyRow3RelationLabel.base,&ui->diplomacyRow3PlayerNameLabel.base,
             &ui->diplomacyRow3FactionLabel.base,labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (&ui->diplomacyRow4RelationButton.sprite.selectable.base,&ui->diplomacyRow4PlayerNumberLabel.base,
             &ui->diplomacyRow4RelationLabel.base,&ui->diplomacyRow4PlayerNameLabel.base,
             &ui->diplomacyRow4FactionLabel.base,labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (&ui->diplomacyRow5RelationButton.sprite.selectable.base,&ui->diplomacyRow5PlayerNumberLabel.base,
             &ui->diplomacyRow5RelationLabel.base,&ui->diplomacyRow5PlayerNameLabel.base,
             &ui->diplomacyRow5FactionLabel.base,labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (&ui->diplomacyRow6RelationButton.sprite.selectable.base,&ui->diplomacyRow6PlayerNumberLabel.base,
             &ui->diplomacyRow6RelationLabel.base,&ui->diplomacyRow6PlayerNameLabel.base,
             &ui->diplomacyRow6FactionLabel.base,labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (&ui->diplomacyRow7RelationButton.sprite.selectable.base,&ui->diplomacyRow7PlayerNumberLabel.base,
             &ui->diplomacyRow7RelationLabel.base,&ui->diplomacyRow7PlayerNameLabel.base,
             &ui->diplomacyRow7FactionLabel.base,labelShift);
}

/* Gives target the edge offsets (leftOffset..bottomOffset) of source. */
static void InGameUiRuntime_CopyEdgeOffsets(UiNodeBase *target,const UiNodeBase *source)

{
  target->leftOffset = source->leftOffset;
  target->topOffset = source->topOffset;
  target->rightOffset = source->rightOffset;
  target->bottomOffset = source->bottomOffset;
}

/* The editor tab strips cover the resource panel and the game panel area; the editor tool option buttons reuse
   the selection group button positions. */
static void InGameUiRuntime_PlaceEditorToolOptions(UiRootNode *inGameRoot)

{
  InGameUiImage *const ui = InGameUi_Image(inGameRoot);
  InGameUiRuntime_CopyEdgeOffsets(&ui->editorTabStripA.base,&ui->resourcePanel.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->editorTabStripB.base,&ui->gamePanelsArea.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->heightToolOption0.selectable.base,&ui->selectionGroupButton0.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->materialToolOption0.selectable.base,&ui->selectionGroupButton0.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->smoothingToolOption0.selectable.base,&ui->selectionGroupButton0.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->unitPlacementOption0.selectable.base,&ui->selectionGroupButton0.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->objectPlacementOption0.selectable.base,&ui->selectionGroupButton0.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->regionToolOption0.selectable.base,&ui->selectionGroupButton0.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->heightToolOption1.selectable.base,&ui->selectionGroupButton1.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->materialToolOption1.selectable.base,&ui->selectionGroupButton1.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->smoothingToolOption1.selectable.base,&ui->selectionGroupButton1.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->unitPlacementOption2.selectable.base,&ui->selectionGroupButton1.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->objectPlacementOption2.selectable.base,&ui->selectionGroupButton1.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->regionToolOption1.selectable.base,&ui->selectionGroupButton1.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->heightToolOption2.selectable.base,&ui->selectionGroupButton2.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->materialToolOption2.selectable.base,&ui->selectionGroupButton2.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->smoothingToolOption2.selectable.base,&ui->selectionGroupButton2.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->unitPlacementOption1.selectable.base,&ui->selectionGroupButton2.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->objectPlacementOption1.selectable.base,&ui->selectionGroupButton2.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->smoothingRelaxGatedButton.selectable.base,&ui->selectionGroupButton6.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->heightToolOption3.selectable.base,&ui->selectionGroupButton7.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->materialToolOption3.selectable.base,&ui->selectionGroupButton7.sprite.selectable.base);
  InGameUiRuntime_CopyEdgeOffsets(&ui->smoothingRelaxLandButton.selectable.base,&ui->selectionGroupButton7.sprite.selectable.base);
}

/* Selection detail page: icon/metrics box of one catalog cell plus a 2 pixel border with the text below it, the
   placeholders of the selection detail text templates, and the 12 metric cells of the multi-selection page. */
static void InGameUiRuntime_LayoutSelectionDetailPage(UiRootNode *inGameRoot)

{
  InGameUiImage *const ui = InGameUi_Image(inGameRoot);
  int32_t iconHeight;
  int32_t paddedIconWidth;
  int32_t paddedIconHeight;
  int32_t textWrapWidth;
  TextResourceId resourceId;
  uint16_t *templateText;
  int columnsRemaining;
  int cellSize;
  int cellLeft;
  int cellTop;
  uint32_t cellIndex;
  UiNodeBase *cell;

  iconHeight = g_InGamePanelTextureSubresource34Height;
  paddedIconWidth = g_InGamePanelTextureSubresource34Width + 2;
  paddedIconHeight = g_InGamePanelTextureSubresource34Height + 2;
  ui->singleSelectionMetrics.base.base.rightOffset = paddedIconWidth;
  ui->singleSelectionMetrics.base.base.bottomOffset = paddedIconHeight;
  ui->singleSelectionMetrics.base.base.leftOffset = 2;
  ui->singleSelectionMetrics.base.base.topOffset = 2;
  ui->hoverItemIcon.base.rightOffset = paddedIconWidth;
  ui->hoverItemIcon.base.bottomOffset = paddedIconHeight;
  ui->hoverItemIcon.base.leftOffset = 2;
  ui->hoverItemIcon.base.topOffset = 2;
  textWrapWidth = g_InGamePanelTextureSubresource02Width - 4;
  ui->singleSelectionStatsText.base.leftOffset = 2;
  ui->singleSelectionStatsText.base.topOffset = iconHeight + 4;
  ui->singleSelectionStatsText.base.rightOffset = -2;
  ui->singleSelectionStatsText.wrapWidth = (UiPixelExtent)textWrapWidth;
  ui->hoverItemStatsText.base.leftOffset = 2;
  ui->hoverItemStatsText.base.topOffset = iconHeight + 4;
  ui->hoverItemStatsText.base.rightOffset = -2;
  ui->hoverItemStatsText.wrapWidth = (UiPixelExtent)textWrapWidth;
  ui->unitPlacementStatsText.base.leftOffset = 2;
  ui->unitPlacementStatsText.base.topOffset = 2;
  ui->unitPlacementStatsText.base.rightOffset = -2;
  ui->unitPlacementStatsText.wrapWidth = (UiPixelExtent)textWrapWidth;
  /* point the placeholders 0..9 of the selection detail text templates at the shared value buffers */
  for (resourceId = TEXT_ID_SELECTION_DETAIL_TEMPLATE_BASE; resourceId < TEXT_ID_MODEL_NAME_BASE; resourceId++) {
    templateText = TextResource_Resolve(resourceId);
    RichTextCommandStream_PatchPayloadBySelector(0,g_InGameSelectionDetailNameTextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(1,g_InGameSelectionDetailArmourTextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(2,g_InGameSelectionDetailWeaponName0TextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(3,g_InGameSelectionDetailWeaponName1TextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(4,g_InGameSelectionDetailWeaponName2TextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(5,g_InGameSelectionDetailTextSlot05Utf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(6,g_InGameSelectionDetailBuildXeniteCostTextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(7,g_InGameSelectionDetailBuildTimeTextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(8,g_InGameSelectionDetailEnergyTextUtf16,templateText);
    RichTextCommandStream_PatchPayloadBySelector(9,g_InGameSelectionDetailTextSlot09Utf16,templateText);
  }
  /* multi-selection page: 12 metric cells in rows of three, each a third of the panel width square */
  columnsRemaining = 3;
  cellSize = (int)((uint64_t)(int64_t)g_InGamePanelTextureSubresource02Width / 3);
  cellLeft = 0;
  cellTop = 0;
  for (cellIndex = 0; cellIndex < 12; cellIndex++) {
    cell = &(ui->*g_InGameSelectionDetailGridCells[cellIndex]).base.base;
    cell->leftOffset = cellLeft;
    cell->topOffset = cellTop;
    cell->rightOffset = cellLeft + cellSize;
    cell->bottomOffset = cellTop + cellSize;
    columnsRemaining--;
    if (columnsRemaining == 0) {
      columnsRemaining = 3;
      cellLeft = 0;
      cellTop = cellTop + cellSize;
    }
    else {
      cellLeft = cellLeft + cellSize;
    }
  }
}

/* Sizes the technology window around tech.gfx: the seven area tabs and the description scroll move up by one tab
   icon height, the window grows by seven icon widths and one icon height around its centre. */
static void InGameUiRuntime_SizeTechnologyWindow(UiRootNode *inGameRoot,GraphicsTextureSourceAsset *techTexture)

{
  InGameUiImage *const ui = InGameUi_Image(inGameRoot);
  GraphicsTextureLogicalSize logicalSize;
  uint32_t techTextureHeight;
  uint32_t halfWidthGrowth;

  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,techTexture);
  techTextureHeight = logicalSize.logicalHeightPixels;
  ui->technologyAreaTab1.selectable.base.topOffset -= techTextureHeight;
  ui->technologyAreaTab2.selectable.base.topOffset -= techTextureHeight;
  ui->technologyAreaTab3.selectable.base.topOffset -= techTextureHeight;
  ui->technologyAreaTab4.selectable.base.topOffset -= techTextureHeight;
  ui->technologyAreaTab5.selectable.base.topOffset -= techTextureHeight;
  ui->technologyAreaTab6.selectable.base.topOffset -= techTextureHeight;
  ui->technologyAreaTab7.selectable.base.topOffset -= techTextureHeight;
  ui->technologyDescriptionScroll.base.bottomOffset -= techTextureHeight;
  halfWidthGrowth = logicalSize.logicalWidthPixels * 7 >> 1;
  ui->technologyWindow.base.leftOffset -= halfWidthGrowth;
  ui->technologyWindow.base.rightOffset += halfWidthGrowth;
  ui->technologyWindow.base.topOffset -= techTextureHeight >> 1;
  ui->technologyWindow.base.bottomOffset += techTextureHeight >> 1;
  ui->technologyDescriptionText.wrapWidth =
       (ui->technologyWindow.base.rightOffset - ui->technologyWindow.base.leftOffset) + -24 +
       (ui->technologyDescriptionScroll.base.rightOffset - ui->technologyDescriptionScroll.base.leftOffset);
}

/* Click sounds: voice sets 0..6 of g_UiButtonSoundVoiceSets7, stored at the control class's sound field
   (activationSound or pointerActivationSound). */
static void InGameUiRuntime_AssignClickSounds(UiRootNode *inGameRoot)

{
  InGameUiImage *const ui = InGameUi_Image(inGameRoot);
  SoundVoiceSet *buttonVoiceSet;

  buttonVoiceSet = g_UiButtonSoundVoiceSets7[0];
  ui->resourcePanelImageToggle8.pointerActivationSound = g_UiButtonSoundVoiceSets7[0];
  ui->resourcePanelImageToggle9.pointerActivationSound = buttonVoiceSet;
  ui->resourcePanelIconButton.activationSound = buttonVoiceSet;
  ui->diplomacyPanel.pointerActivationSound = buttonVoiceSet;
  ui->buildCatalogPanel.pointerActivationSound = buttonVoiceSet;
  ui->specialBuildCatalogPanel.pointerActivationSound = buttonVoiceSet;
  ui->armyStockPanel.pointerActivationSound = buttonVoiceSet;
  ui->editorModeTabTerrainHeight.activationSound = buttonVoiceSet;
  ui->editorModeTabTerrainMaterial.activationSound = buttonVoiceSet;
  ui->editorModeTabTerrainSmoothing.activationSound = buttonVoiceSet;
  ui->editorModeTabRegion.activationSound = buttonVoiceSet;
  ui->editorModeTabUnitPlacement.activationSound = buttonVoiceSet;
  /* Truncated template nodes, untyped in InGameUiImage (see InGameUiRuntime_BindPanelTexture). */
  reinterpret_cast<UiSpriteButtonControl *>(&ui->editorModeTabObjectPlacement)->activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[1];
  ui->inGameMenuButton.activationSound = g_UiButtonSoundVoiceSets7[1];
  ui->missionObjectivesButton.activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[2];
  ui->selectionGroupButton0.sprite.activationSound = g_UiButtonSoundVoiceSets7[2];
  ui->selectionGroupButton1.sprite.activationSound = buttonVoiceSet;
  ui->selectionGroupButton2.sprite.activationSound = buttonVoiceSet;
  ui->selectionGroupButton3.sprite.activationSound = buttonVoiceSet;
  ui->selectionGroupButton4.sprite.activationSound = buttonVoiceSet;
  ui->selectionGroupButton5.sprite.activationSound = buttonVoiceSet;
  ui->selectionGroupButton6.sprite.activationSound = buttonVoiceSet;
  ui->selectionGroupButton7.sprite.activationSound = buttonVoiceSet;
  ui->heightToolOption0.activationSound = buttonVoiceSet;
  ui->heightToolOption1.activationSound = buttonVoiceSet;
  ui->heightToolOption2.activationSound = buttonVoiceSet;
  ui->heightToolOption3.activationSound = buttonVoiceSet;
  ui->materialToolOption0.activationSound = buttonVoiceSet;
  ui->materialToolOption1.activationSound = buttonVoiceSet;
  ui->materialToolOption2.activationSound = buttonVoiceSet;
  ui->materialToolOption3.activationSound = buttonVoiceSet;
  ui->smoothingToolOption0.activationSound = buttonVoiceSet;
  ui->smoothingToolOption1.activationSound = buttonVoiceSet;
  ui->smoothingToolOption2.activationSound = buttonVoiceSet;
  ui->smoothingRelaxLandButton.activationSound = buttonVoiceSet;
  ui->unitPlacementOption0.activationSound = buttonVoiceSet;
  ui->unitPlacementOption2.activationSound = buttonVoiceSet;
  ui->unitPlacementOption1.activationSound = buttonVoiceSet;
  ui->objectPlacementOption0.activationSound = buttonVoiceSet;
  ui->objectPlacementOption2.activationSound = buttonVoiceSet;
  ui->objectPlacementOption1.activationSound = buttonVoiceSet;
  ui->regionToolOption0.activationSound = buttonVoiceSet;
  ui->regionToolOption1.activationSound = buttonVoiceSet;
  reinterpret_cast<UiSpriteButtonControl *>(&ui->singleSelectionUpgradeButton)->activationSound = buttonVoiceSet;
  ui->diplomacyRow1RelationButton.sprite.activationSound = buttonVoiceSet;
  ui->diplomacyRow2RelationButton.sprite.activationSound = buttonVoiceSet;
  ui->diplomacyRow3RelationButton.sprite.activationSound = buttonVoiceSet;
  ui->diplomacyRow4RelationButton.sprite.activationSound = buttonVoiceSet;
  ui->diplomacyRow5RelationButton.sprite.activationSound = buttonVoiceSet;
  ui->diplomacyRow6RelationButton.sprite.activationSound = buttonVoiceSet;
  ui->diplomacyRow7RelationButton.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry00.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry01.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry02.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry03.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry04.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry05.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry06.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry07.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry08.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry09.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry10.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry11.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry12.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry13.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry14.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry15.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry16.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry17.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry18.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry19.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry20.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry21.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry22.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry23.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry24.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry25.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry26.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry27.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry28.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry29.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry30.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry31.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry32.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry33.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry34.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry35.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry36.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry37.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry38.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry39.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry40.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry41.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry42.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry43.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry44.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry45.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry46.command.sprite.activationSound = buttonVoiceSet;
  ui->buildCatalogEntry47.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry00.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry01.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry02.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry03.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry04.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry05.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry06.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry07.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry08.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry09.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry10.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry11.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry12.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry13.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry14.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry15.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry16.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry17.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry18.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry19.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry20.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry21.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry22.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry23.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry24.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry25.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry26.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry27.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry28.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry29.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry30.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry31.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry32.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry33.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry34.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry35.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry36.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry37.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry38.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry39.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry40.command.sprite.activationSound = buttonVoiceSet;
  ui->specialBuildCatalogEntry41.command.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot00.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot01.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot02.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot03.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot04.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot05.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot06.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot07.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot08.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot09.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot10.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot11.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot12.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot13.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot14.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot15.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot16.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot17.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot18.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot19.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot20.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot21.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot22.sprite.activationSound = buttonVoiceSet;
  ui->armyStockSlot23.command.sprite.activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[3];
  ui->resultsTabMilitary.activationSound = g_UiButtonSoundVoiceSets7[3];
  ui->resultsTabEconomy.activationSound = buttonVoiceSet;
  ui->resultsTabThird.activationSound = buttonVoiceSet;
  ui->resultsContinueButton.activationSound = buttonVoiceSet;
  ui->resultsSecondaryExitButton.activationSound = buttonVoiceSet;
  ui->resultsChartModeButtonA.activationSound = buttonVoiceSet;
  ui->resultsChartModeButtonB.activationSound = buttonVoiceSet;
  ui->gameMenuSaveButton.activationSound = buttonVoiceSet;
  ui->gameMenuGraphicsButton.activationSound = buttonVoiceSet;
  ui->gameMenuQuitButton.activationSound = buttonVoiceSet;
  ui->gameMenuAudioButton.activationSound = buttonVoiceSet;
  ui->gameMenuCloseButton.activationSound = buttonVoiceSet;
  ui->saveGameBackButton.activationSound = buttonVoiceSet;
  ui->saveGameSaveButton.activationSound = buttonVoiceSet;
  ui->saveGameDeleteButton.activationSound = buttonVoiceSet;
  ui->quitMenuBackButton.activationSound = buttonVoiceSet;
  ui->quitMenuAbortMissionButton.activationSound = buttonVoiceSet;
  ui->quitMenuSurrenderButton.activationSound = buttonVoiceSet;
  ui->quitMenuRestartMissionButton.activationSound = buttonVoiceSet;
  ui->graphicsOptionsBackButton.activationSound = buttonVoiceSet;
  ui->soundOptionsBackButton.activationSound = buttonVoiceSet;
  ui->messageSendButton.activationSound = buttonVoiceSet;
  ui->messageSendAndCloseButton.activationSound = buttonVoiceSet;
  ui->messageCancelButton.activationSound = buttonVoiceSet;
  ui->technologyResearchButton.activationSound = buttonVoiceSet;
  ui->technologyCloseButton.activationSound = buttonVoiceSet;
  ui->missionHelpCloseButton.activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[4];
  ui->autoZoomOffCheckbox.activationSound = g_UiButtonSoundVoiceSets7[4];
  ui->autoRotationOffCheckbox.activationSound = buttonVoiceSet;
  ui->linkRotationZoomCheckbox.activationSound = buttonVoiceSet;
  ui->linkRotationTiltCheckbox.activationSound = buttonVoiceSet;
  ui->hidePanelCheckbox.activationSound = buttonVoiceSet;
  ui->shadingEnabledCheckbox.activationSound = buttonVoiceSet;
  ui->textureQualityLowButton.activationSound = buttonVoiceSet;
  ui->textureQualityMediumButton.activationSound = buttonVoiceSet;
  ui->textureQualityHighButton.activationSound = buttonVoiceSet;
  ui->musicEnabledCheckbox.activationSound = buttonVoiceSet;
  ui->effectsEnabledCheckbox.activationSound = buttonVoiceSet;
  ui->reverseStereoCheckbox.activationSound = buttonVoiceSet;
  ui->shadingLevel32x32Button.base.activationSound = buttonVoiceSet;
  ui->shadingLevel32x64Button.base.activationSound = buttonVoiceSet;
  ui->shadingLevel32x128Button.base.activationSound = buttonVoiceSet;
  ui->shadingLevel64x64Button.base.activationSound = buttonVoiceSet;
  ui->shadingLevel64x128Button.base.activationSound = buttonVoiceSet;
  ui->shadingLevel128x128Button.base.activationSound = buttonVoiceSet;
  ui->messageRecipientPlayersTab.activationSound = buttonVoiceSet;
  ui->messageRecipientAllTab.activationSound = buttonVoiceSet;
  ui->messageRecipientGroupsTab.activationSound = buttonVoiceSet;
  ui->messageRecipientCheckbox1.activationSound = buttonVoiceSet;
  ui->messageRecipientCheckbox2.activationSound = buttonVoiceSet;
  ui->messageRecipientCheckbox3.activationSound = buttonVoiceSet;
  ui->messageRecipientCheckbox4.activationSound = buttonVoiceSet;
  ui->messageRecipientCheckbox5.activationSound = buttonVoiceSet;
  ui->messageRecipientCheckbox6.activationSound = buttonVoiceSet;
  ui->messageRecipientCheckbox7.activationSound = buttonVoiceSet;
  ui->technologyAreaTab1.activationSound = buttonVoiceSet;
  ui->technologyAreaTab2.activationSound = buttonVoiceSet;
  ui->technologyAreaTab3.activationSound = buttonVoiceSet;
  ui->technologyAreaTab4.activationSound = buttonVoiceSet;
  ui->technologyAreaTab5.activationSound = buttonVoiceSet;
  ui->technologyAreaTab6.activationSound = buttonVoiceSet;
  ui->technologyAreaTab7.activationSound = buttonVoiceSet;
  ui->missionHelpBriefingTab.activationSound = buttonVoiceSet;
  ui->missionHelpKeyboardTab.activationSound = buttonVoiceSet;
  ui->missionHelpMouseTab.activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[5];
  ui->modelDetailSlider.clickSound = g_UiButtonSoundVoiceSets7[5];
  ui->effectsVolumeSlider.clickSound = buttonVoiceSet;
  ui->movieVolumeSlider.clickSound = buttonVoiceSet;
  ui->musicVolumeSlider.clickSound = buttonVoiceSet;
  ui->messageMovieVolumeSlider.clickSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[6];
  ui->saveNameEdit.activationSound = g_UiButtonSoundVoiceSets7[6];
  ui->saveGameList.activationSound = buttonVoiceSet;
  ui->messageTextEdit.activationSound = buttonVoiceSet;
  ui->chatInputTextEdit.activationSound = buttonVoiceSet;
}

/* Loads a graphics package; on success it replaces the package in *slot (an atomic exchange in the original) and
   the previous package is released. Returns the loaded package, or NULL with the loader's error in *loadError. */
static GraphicsTextureSourceAsset *InGameUiRuntime_ReplaceTexturePackage
          (uint16_t *packagePath,GraphicsTextureSourceAsset **slot,uint32_t *loadError)

{
  GraphicsTextureSourceAsset *loadedPackage;
  GraphicsTextureSourceAsset *previousPackage;

  loadedPackage = g_GraphicsTextureSourceLoadPackageAsset(packagePath,loadError);
  previousPackage = *slot;
  if (loadedPackage != nullptr) {
    *slot = loadedPackage;
    g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(previousPackage);
  }
  return loadedPackage;
}

/* Lays out the freshly copied in-game UI template for the current display mode (called by both session
   initialisers): picks the panel/diagram graphics variant (gfx\panel\panel0/diagram0 with the digit 0, 1 or 2
   for below 800x600, below 1024x768, or larger) and the resource gauge geometry, loads panel0.gfx and caches the
   subresource sizes the layout needs, binds the texture to the side panel controls and positions them (frame
   edges, menu bar, selection group buttons, the build catalog / special catalog / army stock grids, diplomacy
   rows, editor tool options, selection detail page), patches the selection detail text templates
   0x18002C..0x18004E, then loads diagram0.gfx, window.gfx and tech.gfx (sizing the technology window) and assigns
   the UI click sounds. Returns true on success; false with the loader's error in *outError when a graphics
   package cannot be loaded.
   The diagram, window and technology texture slots are typed uint32_t in the image data, hence the slot casts.
*/
Bool8 InGameUiRuntime_InitializeControlTreeResources(UiRootNode *inGameRoot,uint32_t *outError)

{
  InGameUiImage *const ui = InGameUi_Image(inGameRoot);
  uint32_t textureLoadError;
  GraphicsTextureSourceAsset *panelTexture;
  GraphicsTextureSourceAsset *diagramTexture;
  GraphicsTextureSourceAsset *windowTexture;
  GraphicsTextureSourceAsset *techTexture;

  InGameUiRuntime_SelectDisplayModeLayout(inGameRoot);
  panelTexture = InGameUiRuntime_ReplaceTexturePackage
                   ((uint16_t *)g_GfxPanelPanel0GfxPathUtf16,&g_InGamePanelTextureSource,&textureLoadError);
  if (panelTexture == nullptr) {
    *outError = textureLoadError;
    return false;
  }
  InGameUiRuntime_CachePanelSubresourceSizes(panelTexture);
  InGameUiRuntime_BindPanelTexture(inGameRoot,panelTexture);
  InGameUiRuntime_SizeSidePanelFrame(inGameRoot);
  InGameUiRuntime_PlaceMenuAndSelectionGroupButtons(inGameRoot);
  InGameUiRuntime_PlaceCatalogGridCells(inGameRoot);
  InGameUiRuntime_PlaceDiplomacyRows(inGameRoot);
  InGameUiRuntime_PlaceEditorToolOptions(inGameRoot);
  InGameUiRuntime_LayoutSelectionDetailPage(inGameRoot);

  diagramTexture = InGameUiRuntime_ReplaceTexturePackage
                     ((uint16_t *)g_GfxPanelDiagram0GfxPathUtf16,
                      (GraphicsTextureSourceAsset **)&g_InGameDiagramTextureSource,&textureLoadError);
  if (diagramTexture == nullptr) {
    *outError = textureLoadError;
    return false;
  }
  ui->xeniteGauge.textureSource = diagramTexture;
  ui->tritiumGauge.textureSource = diagramTexture;
  ui->energyGauge.textureSource = diagramTexture;

  windowTexture = InGameUiRuntime_ReplaceTexturePackage
                    ((uint16_t *)g_GfxPanelWindowGfxPathUtf16,
                     (GraphicsTextureSourceAsset **)&g_InGameWindowTextureSource,&textureLoadError);
  if (windowTexture == nullptr) {
    *outError = textureLoadError;
    return false;
  }
  ui->technologyWindow.textureSource = windowTexture;
  ui->messageWindow.textureSource = windowTexture;
  ui->gameMenuWindow.textureSource = windowTexture;
  ui->quitGameWindow.textureSource = windowTexture;
  ui->saveGameWindow.textureSource = windowTexture;
  ui->graphicsSettingsWindow.textureSource = windowTexture;
  ui->audioSettingsWindow.textureSource = windowTexture;
  ui->missionHelpWindow.textureSource = windowTexture;

  techTexture = InGameUiRuntime_ReplaceTexturePackage
                  ((uint16_t *)g_GfxPanelTechGfxPathUtf16,
                   (GraphicsTextureSourceAsset **)&g_InGameTechnologyTextureSource,&textureLoadError);
  if (techTexture == nullptr) {
    *outError = textureLoadError;
    return false;
  }
  ui->technologyAreaTab1Icon.textureSource = techTexture;
  ui->technologyAreaTab2Icon.textureSource = techTexture;
  ui->technologyAreaTab3Icon.textureSource = techTexture;
  ui->technologyAreaTab4Icon.textureSource = techTexture;
  ui->technologyAreaTab5Icon.textureSource = techTexture;
  ui->technologyAreaTab6Icon.textureSource = techTexture;
  ui->technologyAreaTab7Icon.textureSource = techTexture;
  InGameUiRuntime_SizeTechnologyWindow(inGameRoot,techTexture);
  InGameUiRuntime_AssignClickSounds(inGameRoot);
  return true;
}
