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

DirectSoundVoiceSet *g_UiButtonSoundVoiceSets7[7] = {};

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
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,xeniteGauge),36,6,94,13);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,tritiumGauge),36,17,94,24);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,energyGauge),36,28,94,35);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,xeniteAmountText),4,5,31,13);
  }
  else {
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,xeniteGauge),44,9,110,16);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,tritiumGauge),44,23,110,30);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,energyGauge),44,37,110,44);
    InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,xeniteAmountText),4,7,39,15);
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
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameLeftEdge),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameLeftEdge))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameRightEdge),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameRightEdge))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameTopCap),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameTopCap))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameMenuBar),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameMenuBar))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameInfoSection),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameInfoSection))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelFrameBottomCap),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,sidePanelFrameBottomCap))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,resourcePanel),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,resourcePanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,editorTabStripA))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,gamePanelsArea),0,0,0,0);
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,gamePanelsArea))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,editorTabStripB))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle8Popup))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle9Popup))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,diplomacyFrame))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,buildCatalogFrame))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,specialBuildCatalogFrame))->textureSource = panelTexture;
  ((UiNineSlicePanelControl *)INGAME_UI(inGameRoot,armyStockFrame))->textureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle8))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainHeight))->primaryTextureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle9))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainMaterial))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,resourcePanelIconButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainSmoothing))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,inGameMenuButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,missionObjectivesButton))->primaryTextureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,countdownDisplayPanel))->textureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,diplomacyPanel))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabRegion))->primaryTextureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,buildCatalogPanel))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabUnitPlacement))->primaryTextureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,specialBuildCatalogPanel))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabObjectPlacement))->primaryTextureSource = panelTexture;
  ((UiImageControl *)INGAME_UI(inGameRoot,armyStockPanel))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton3))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton4))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton5))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton6))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton7))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption3))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption3))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingRelaxGatedButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingRelaxLandButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption2))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption1))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,regionToolOption0))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,regionToolOption1))->primaryTextureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,minimapView),0,0,0,0);
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,modePreviewPageStack),0,0,0,0);
  ((UiImageActionControl *)INGAME_UI(inGameRoot,notificationTargetButton))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,heightToolPreview))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,smoothingToolPreview))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,regionToolPreview))->textureSource = panelTexture;
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,modeDetailPageStack),0,0,0,0);
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,singleSelectionUpgradeButton))->primaryTextureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,selectionDetailPanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,heightToolPanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,materialPalettePanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,smoothingToolPanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,unitPlacementPanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,objectPlacementPanel))->textureSource = panelTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,regionToolPanel))->textureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry00))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry01))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry02))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry03))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry04))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry05))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry06))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry07))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry08))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry09))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry10))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry11))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry12))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry13))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry14))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry15))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry16))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry17))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry18))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry19))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry20))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry21))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry22))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry23))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry24))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry25))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry26))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry27))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry28))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry29))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry30))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry31))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry32))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry33))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry34))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry35))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry36))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry37))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry38))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry39))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry40))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry41))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry42))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry43))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry44))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry45))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry46))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry47))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry00))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry01))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry02))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry03))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry04))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry05))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry06))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry07))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry08))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry09))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry10))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry11))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry12))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry13))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry14))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry15))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry16))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry17))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry18))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry19))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry20))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry21))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry22))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry23))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry24))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry25))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry26))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry27))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry28))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry29))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry30))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry31))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry32))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry33))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry34))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry35))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry36))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry37))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry38))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry39))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry40))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry41))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot00))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot01))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot02))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot03))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot04))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot05))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot06))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot07))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot08))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot09))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot10))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot11))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot12))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot13))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot14))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot15))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot16))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot17))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot18))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot19))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot20))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot21))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot22))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot23))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow1RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow1RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow2RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow2RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow3RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow3RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow4RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow4RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow5RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow5RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow6RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow6RelationButton))->primaryTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow7RelationButton))->alternateTextureSource = panelTexture;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow7RelationButton))->primaryTextureSource = panelTexture;
}

/* Sizes the side panel parts from the panel subresources: the frame and the pages inside it move left by the
   widths of subresources 1 and 2 (the left edge also by subresource 0), and down by the heights of the frame
   pieces above them (subresources 2, 36, 3, 37, 4); the bottom cap and the mode detail page end above
   subresources 0 and 5. */
static void InGameUiRuntime_SizeSidePanelFrame(UiRootNode *inGameRoot)

{
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

  leftEdge = INGAME_UI(inGameRoot,sidePanelFrameLeftEdge);
  rightEdge = INGAME_UI(inGameRoot,sidePanelFrameRightEdge);
  topCap = INGAME_UI(inGameRoot,sidePanelFrameTopCap);
  menuBar = INGAME_UI(inGameRoot,sidePanelFrameMenuBar);
  infoSection = INGAME_UI(inGameRoot,sidePanelFrameInfoSection);
  bottomCap = INGAME_UI(inGameRoot,sidePanelFrameBottomCap);
  minimap = INGAME_UI(inGameRoot,minimapView);
  modePreview = INGAME_UI(inGameRoot,modePreviewPageStack);
  modeDetail = INGAME_UI(inGameRoot,modeDetailPageStack);
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
  INGAME_UI(inGameRoot,resourcePanel)->leftOffset -= g_InGamePanelTextureSubresource06Width;
  INGAME_UI(inGameRoot,gamePanelsArea)->leftOffset -= g_InGamePanelTextureSubresource07Width;
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
  INGAME_UI(inGameRoot,resourcePanel)->bottomOffset += g_InGamePanelTextureSubresource06Height;
  INGAME_UI(inGameRoot,gamePanelsArea)->topOffset -= g_InGamePanelTextureSubresource07Height;
}

/* The menu buttons and the countdown share the menu bar's rows, the selection group buttons the info section's;
   all of them span the side panel frame horizontally. The selection group buttons then form a 4x2 grid (left
   +5/+36/+66/+97, top +17/+40). The world view ends where the side panel stack begins. */
static void InGameUiRuntime_PlaceMenuAndSelectionGroupButtons(UiRootNode *inGameRoot)

{
  static const int32_t groupButtonColumnShifts[4] = { 5, 36, 66, 97 };
  static const int32_t groupButtonRowShifts[2] = { 17, 40 };
  UiNodeBase *groupButtons[8];
  UiNodeBase *menuBar;
  UiNodeBase *infoSection;
  int32_t frameLeft;
  int32_t frameRight;
  int buttonIndex;

  menuBar = INGAME_UI(inGameRoot,sidePanelFrameMenuBar);
  infoSection = INGAME_UI(inGameRoot,sidePanelFrameInfoSection);
  frameLeft = INGAME_UI(inGameRoot,sidePanelFrameLeftEdge)->leftOffset;
  frameRight = INGAME_UI(inGameRoot,sidePanelFrameRightEdge)->rightOffset;
  InGameUiRuntime_SetEdgeOffsets
            (INGAME_UI(inGameRoot,inGameMenuButton),frameLeft,menuBar->topOffset,frameRight,menuBar->bottomOffset);
  InGameUiRuntime_SetEdgeOffsets
            (INGAME_UI(inGameRoot,missionObjectivesButton),frameLeft,menuBar->topOffset,frameRight,
             menuBar->bottomOffset);
  InGameUiRuntime_SetEdgeOffsets
            (INGAME_UI(inGameRoot,countdownDisplayPanel),frameLeft,menuBar->topOffset,frameRight,
             menuBar->bottomOffset);
  groupButtons[0] = INGAME_UI(inGameRoot,selectionGroupButton0);
  groupButtons[1] = INGAME_UI(inGameRoot,selectionGroupButton1);
  groupButtons[2] = INGAME_UI(inGameRoot,selectionGroupButton2);
  groupButtons[3] = INGAME_UI(inGameRoot,selectionGroupButton3);
  groupButtons[4] = INGAME_UI(inGameRoot,selectionGroupButton4);
  groupButtons[5] = INGAME_UI(inGameRoot,selectionGroupButton5);
  groupButtons[6] = INGAME_UI(inGameRoot,selectionGroupButton6);
  groupButtons[7] = INGAME_UI(inGameRoot,selectionGroupButton7);
  for (buttonIndex = 0; buttonIndex < 8; buttonIndex++) {
    InGameUiRuntime_SetEdgeOffsets
              (groupButtons[buttonIndex],frameLeft + groupButtonColumnShifts[buttonIndex % 4],
               infoSection->topOffset + groupButtonRowShifts[buttonIndex / 4],frameRight,infoSection->bottomOffset);
  }
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,worldViewArea),0,0,frameLeft,0);
  InGameUiRuntime_SetEdgeOffsets(INGAME_UI(inGameRoot,sidePanelStack),frameLeft,0,0,0);
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
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry00),columnEdges,rowEdges,0,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry01),columnEdges,rowEdges,1,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry02),columnEdges,rowEdges,2,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry03),columnEdges,rowEdges,3,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry04),columnEdges,rowEdges,0,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry05),columnEdges,rowEdges,1,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry06),columnEdges,rowEdges,2,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry07),columnEdges,rowEdges,3,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry08),columnEdges,rowEdges,0,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry09),columnEdges,rowEdges,1,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry10),columnEdges,rowEdges,2,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry11),columnEdges,rowEdges,3,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry12),columnEdges,rowEdges,0,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry13),columnEdges,rowEdges,1,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry14),columnEdges,rowEdges,2,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry15),columnEdges,rowEdges,3,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry16),columnEdges,rowEdges,0,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry17),columnEdges,rowEdges,1,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry18),columnEdges,rowEdges,2,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry19),columnEdges,rowEdges,3,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry20),columnEdges,rowEdges,0,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry21),columnEdges,rowEdges,1,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry22),columnEdges,rowEdges,2,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry23),columnEdges,rowEdges,3,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry24),columnEdges,rowEdges,4,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry25),columnEdges,rowEdges,4,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry26),columnEdges,rowEdges,4,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry27),columnEdges,rowEdges,4,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry28),columnEdges,rowEdges,4,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry29),columnEdges,rowEdges,4,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry30),columnEdges,rowEdges,5,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry31),columnEdges,rowEdges,5,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry32),columnEdges,rowEdges,5,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry33),columnEdges,rowEdges,5,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry34),columnEdges,rowEdges,5,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry35),columnEdges,rowEdges,5,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry36),columnEdges,rowEdges,6,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry37),columnEdges,rowEdges,6,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry38),columnEdges,rowEdges,6,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry39),columnEdges,rowEdges,6,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry40),columnEdges,rowEdges,6,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry41),columnEdges,rowEdges,6,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry42),columnEdges,rowEdges,7,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry43),columnEdges,rowEdges,7,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry44),columnEdges,rowEdges,7,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry45),columnEdges,rowEdges,7,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry46),columnEdges,rowEdges,7,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,buildCatalogEntry47),columnEdges,rowEdges,7,5);
  /* special build catalog: 4x7 cells, then 2 columns of 7 */
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry00),columnEdges,rowEdges,0,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry01),columnEdges,rowEdges,1,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry02),columnEdges,rowEdges,2,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry03),columnEdges,rowEdges,3,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry04),columnEdges,rowEdges,0,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry05),columnEdges,rowEdges,1,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry06),columnEdges,rowEdges,2,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry07),columnEdges,rowEdges,3,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry08),columnEdges,rowEdges,0,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry09),columnEdges,rowEdges,1,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry10),columnEdges,rowEdges,2,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry11),columnEdges,rowEdges,3,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry12),columnEdges,rowEdges,0,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry13),columnEdges,rowEdges,1,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry14),columnEdges,rowEdges,2,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry15),columnEdges,rowEdges,3,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry16),columnEdges,rowEdges,0,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry17),columnEdges,rowEdges,1,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry18),columnEdges,rowEdges,2,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry19),columnEdges,rowEdges,3,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry20),columnEdges,rowEdges,0,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry21),columnEdges,rowEdges,1,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry22),columnEdges,rowEdges,2,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry23),columnEdges,rowEdges,3,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry24),columnEdges,rowEdges,0,6);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry25),columnEdges,rowEdges,1,6);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry26),columnEdges,rowEdges,2,6);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry27),columnEdges,rowEdges,3,6);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry28),columnEdges,rowEdges,4,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry29),columnEdges,rowEdges,4,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry30),columnEdges,rowEdges,4,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry31),columnEdges,rowEdges,4,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry32),columnEdges,rowEdges,4,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry33),columnEdges,rowEdges,4,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry34),columnEdges,rowEdges,4,6);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry35),columnEdges,rowEdges,5,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry36),columnEdges,rowEdges,5,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry37),columnEdges,rowEdges,5,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry38),columnEdges,rowEdges,5,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry39),columnEdges,rowEdges,5,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry40),columnEdges,rowEdges,5,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,specialBuildCatalogEntry41),columnEdges,rowEdges,5,6);
  /* army stock: 4x6 cells */
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot00),columnEdges,rowEdges,0,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot01),columnEdges,rowEdges,1,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot02),columnEdges,rowEdges,2,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot03),columnEdges,rowEdges,3,0);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot04),columnEdges,rowEdges,0,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot05),columnEdges,rowEdges,1,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot06),columnEdges,rowEdges,2,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot07),columnEdges,rowEdges,3,1);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot08),columnEdges,rowEdges,0,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot09),columnEdges,rowEdges,1,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot10),columnEdges,rowEdges,2,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot11),columnEdges,rowEdges,3,2);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot12),columnEdges,rowEdges,0,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot13),columnEdges,rowEdges,1,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot14),columnEdges,rowEdges,2,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot15),columnEdges,rowEdges,3,3);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot16),columnEdges,rowEdges,0,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot17),columnEdges,rowEdges,1,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot18),columnEdges,rowEdges,2,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot19),columnEdges,rowEdges,3,4);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot20),columnEdges,rowEdges,0,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot21),columnEdges,rowEdges,1,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot22),columnEdges,rowEdges,2,5);
  InGameUiRuntime_PlaceGridCell(INGAME_UI(inGameRoot,armyStockSlot23),columnEdges,rowEdges,3,5);
  /* the technology description scroll's left edge and frame's right edge move one catalog cell width */
  INGAME_UI(inGameRoot,technologyDescriptionScroll)->leftOffset += g_InGamePanelTextureSubresource34Width;
  INGAME_UI(inGameRoot,technologyDescriptionFrame)->rightOffset += g_InGamePanelTextureSubresource34Width;
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
  UiNodeBase *rows[7];
  int32_t rowLeftOffset;
  int32_t rowRightOffset;
  int32_t rowBottomShift;
  int32_t rowTopShift;
  int32_t labelShift;
  int rowIndex;

  rows[0] = INGAME_UI(inGameRoot,diplomacyRow1);
  rows[1] = INGAME_UI(inGameRoot,diplomacyRow2);
  rows[2] = INGAME_UI(inGameRoot,diplomacyRow3);
  rows[3] = INGAME_UI(inGameRoot,diplomacyRow4);
  rows[4] = INGAME_UI(inGameRoot,diplomacyRow5);
  rows[5] = INGAME_UI(inGameRoot,diplomacyRow6);
  rows[6] = INGAME_UI(inGameRoot,diplomacyRow7);
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
            (INGAME_UI(inGameRoot,diplomacyRow1RelationButton),INGAME_UI(inGameRoot,diplomacyRow1PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow1RelationLabel),INGAME_UI(inGameRoot,diplomacyRow1PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow1FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow2RelationButton),INGAME_UI(inGameRoot,diplomacyRow2PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow2RelationLabel),INGAME_UI(inGameRoot,diplomacyRow2PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow2FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow3RelationButton),INGAME_UI(inGameRoot,diplomacyRow3PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow3RelationLabel),INGAME_UI(inGameRoot,diplomacyRow3PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow3FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow4RelationButton),INGAME_UI(inGameRoot,diplomacyRow4PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow4RelationLabel),INGAME_UI(inGameRoot,diplomacyRow4PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow4FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow5RelationButton),INGAME_UI(inGameRoot,diplomacyRow5PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow5RelationLabel),INGAME_UI(inGameRoot,diplomacyRow5PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow5FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow6RelationButton),INGAME_UI(inGameRoot,diplomacyRow6PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow6RelationLabel),INGAME_UI(inGameRoot,diplomacyRow6PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow6FactionLabel),labelShift);
  InGameUiRuntime_ShiftDiplomacyRowLabels
            (INGAME_UI(inGameRoot,diplomacyRow7RelationButton),INGAME_UI(inGameRoot,diplomacyRow7PlayerNumberLabel),
             INGAME_UI(inGameRoot,diplomacyRow7RelationLabel),INGAME_UI(inGameRoot,diplomacyRow7PlayerNameLabel),
             INGAME_UI(inGameRoot,diplomacyRow7FactionLabel),labelShift);
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
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,editorTabStripA),INGAME_UI(inGameRoot,resourcePanel));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,editorTabStripB),INGAME_UI(inGameRoot,gamePanelsArea));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,heightToolOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,materialToolOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,smoothingToolOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,unitPlacementOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,objectPlacementOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,regionToolOption0),INGAME_UI(inGameRoot,selectionGroupButton0));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,heightToolOption1),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,materialToolOption1),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,smoothingToolOption1),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,unitPlacementOption2),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,objectPlacementOption2),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,regionToolOption1),INGAME_UI(inGameRoot,selectionGroupButton1));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,heightToolOption2),INGAME_UI(inGameRoot,selectionGroupButton2));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,materialToolOption2),INGAME_UI(inGameRoot,selectionGroupButton2));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,smoothingToolOption2),INGAME_UI(inGameRoot,selectionGroupButton2));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,unitPlacementOption1),INGAME_UI(inGameRoot,selectionGroupButton2));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,objectPlacementOption1),INGAME_UI(inGameRoot,selectionGroupButton2));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,smoothingRelaxGatedButton),INGAME_UI(inGameRoot,selectionGroupButton6));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,heightToolOption3),INGAME_UI(inGameRoot,selectionGroupButton7));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,materialToolOption3),INGAME_UI(inGameRoot,selectionGroupButton7));
  InGameUiRuntime_CopyEdgeOffsets(INGAME_UI(inGameRoot,smoothingRelaxLandButton),INGAME_UI(inGameRoot,selectionGroupButton7));
}

/* Selection detail page: icon/metrics box of one catalog cell plus a 2 pixel border with the text below it, the
   placeholders of the selection detail text templates, and the 12 metric cells of the multi-selection page. */
static void InGameUiRuntime_LayoutSelectionDetailPage(UiRootNode *inGameRoot)

{
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
  INGAME_UI(inGameRoot,singleSelectionMetrics)->rightOffset = paddedIconWidth;
  INGAME_UI(inGameRoot,singleSelectionMetrics)->bottomOffset = paddedIconHeight;
  INGAME_UI(inGameRoot,singleSelectionMetrics)->leftOffset = 2;
  INGAME_UI(inGameRoot,singleSelectionMetrics)->topOffset = 2;
  INGAME_UI(inGameRoot,hoverItemIcon)->rightOffset = paddedIconWidth;
  INGAME_UI(inGameRoot,hoverItemIcon)->bottomOffset = paddedIconHeight;
  INGAME_UI(inGameRoot,hoverItemIcon)->leftOffset = 2;
  INGAME_UI(inGameRoot,hoverItemIcon)->topOffset = 2;
  textWrapWidth = g_InGamePanelTextureSubresource02Width - 4;
  INGAME_UI(inGameRoot,singleSelectionStatsText)->leftOffset = 2;
  INGAME_UI(inGameRoot,singleSelectionStatsText)->topOffset = iconHeight + 4;
  INGAME_UI(inGameRoot,singleSelectionStatsText)->rightOffset = -2;
  ((UiWrappedTextControl *)INGAME_UI(inGameRoot,singleSelectionStatsText))->wrapWidth = (UiPixelExtent)textWrapWidth;
  INGAME_UI(inGameRoot,hoverItemStatsText)->leftOffset = 2;
  INGAME_UI(inGameRoot,hoverItemStatsText)->topOffset = iconHeight + 4;
  INGAME_UI(inGameRoot,hoverItemStatsText)->rightOffset = -2;
  ((UiWrappedTextControl *)INGAME_UI(inGameRoot,hoverItemStatsText))->wrapWidth = (UiPixelExtent)textWrapWidth;
  INGAME_UI(inGameRoot,unitPlacementStatsText)->leftOffset = 2;
  INGAME_UI(inGameRoot,unitPlacementStatsText)->topOffset = 2;
  INGAME_UI(inGameRoot,unitPlacementStatsText)->rightOffset = -2;
  ((UiWrappedTextControl *)INGAME_UI(inGameRoot,unitPlacementStatsText))->wrapWidth = (UiPixelExtent)textWrapWidth;
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
    cell = THANDOR_UI_AT(inGameRoot,g_InGameSelectionDetailGridCellOffsets[cellIndex]);
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
  GraphicsTextureLogicalSize logicalSize;
  uint32_t techTextureHeight;
  uint32_t halfWidthGrowth;

  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,techTexture);
  techTextureHeight = logicalSize.logicalHeightPixels;
  INGAME_UI(inGameRoot,technologyAreaTab1)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab2)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab3)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab4)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab5)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab6)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyAreaTab7)->topOffset -= techTextureHeight;
  INGAME_UI(inGameRoot,technologyDescriptionScroll)->bottomOffset -= techTextureHeight;
  halfWidthGrowth = logicalSize.logicalWidthPixels * 7 >> 1;
  INGAME_UI(inGameRoot,technologyWindow)->leftOffset -= halfWidthGrowth;
  INGAME_UI(inGameRoot,technologyWindow)->rightOffset += halfWidthGrowth;
  INGAME_UI(inGameRoot,technologyWindow)->topOffset -= techTextureHeight >> 1;
  INGAME_UI(inGameRoot,technologyWindow)->bottomOffset += techTextureHeight >> 1;
  ((UiWrappedTextControl *)INGAME_UI(inGameRoot,technologyDescriptionText))->wrapWidth =
       (INGAME_UI(inGameRoot,technologyWindow)->rightOffset - INGAME_UI(inGameRoot,technologyWindow)->leftOffset) + -24 +
       (INGAME_UI(inGameRoot,technologyDescriptionScroll)->rightOffset - INGAME_UI(inGameRoot,technologyDescriptionScroll)->leftOffset);
}

/* Click sounds: voice sets 0..6 of g_UiButtonSoundVoiceSets7, stored at the control class's sound field
   (activationSound or pointerActivationSound). */
static void InGameUiRuntime_AssignClickSounds(UiRootNode *inGameRoot)

{
  DirectSoundVoiceSet *buttonVoiceSet;

  buttonVoiceSet = g_UiButtonSoundVoiceSets7[0];
  ((UiImageControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle8))->pointerActivationSound = g_UiButtonSoundVoiceSets7[0];
  ((UiImageControl *)INGAME_UI(inGameRoot,resourcePanelImageToggle9))->pointerActivationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,resourcePanelIconButton))->activationSound = buttonVoiceSet;
  ((UiImageControl *)INGAME_UI(inGameRoot,diplomacyPanel))->pointerActivationSound = buttonVoiceSet;
  ((UiImageControl *)INGAME_UI(inGameRoot,buildCatalogPanel))->pointerActivationSound = buttonVoiceSet;
  ((UiImageControl *)INGAME_UI(inGameRoot,specialBuildCatalogPanel))->pointerActivationSound = buttonVoiceSet;
  ((UiImageControl *)INGAME_UI(inGameRoot,armyStockPanel))->pointerActivationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainHeight))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainMaterial))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabTerrainSmoothing))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabRegion))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabUnitPlacement))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,editorModeTabObjectPlacement))->activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[1];
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,inGameMenuButton))->activationSound = g_UiButtonSoundVoiceSets7[1];
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,missionObjectivesButton))->activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[2];
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton0))->activationSound = g_UiButtonSoundVoiceSets7[2];
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton3))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton4))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton5))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton6))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,selectionGroupButton7))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,heightToolOption3))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,materialToolOption3))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingToolOption2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,smoothingRelaxLandButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,unitPlacementOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption2))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,objectPlacementOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,regionToolOption0))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,regionToolOption1))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,singleSelectionUpgradeButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow1RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow2RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow3RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow4RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow5RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow6RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,diplomacyRow7RelationButton))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry00))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry01))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry02))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry03))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry04))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry05))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry06))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry07))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry08))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry09))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry10))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry11))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry12))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry13))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry14))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry15))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry16))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry17))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry18))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry19))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry20))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry21))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry22))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry23))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry24))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry25))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry26))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry27))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry28))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry29))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry30))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry31))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry32))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry33))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry34))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry35))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry36))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry37))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry38))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry39))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry40))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry41))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry42))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry43))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry44))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry45))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry46))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,buildCatalogEntry47))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry00))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry01))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry02))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry03))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry04))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry05))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry06))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry07))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry08))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry09))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry10))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry11))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry12))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry13))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry14))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry15))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry16))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry17))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry18))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry19))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry20))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry21))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry22))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry23))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry24))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry25))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry26))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry27))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry28))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry29))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry30))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry31))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry32))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry33))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry34))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry35))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry36))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry37))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry38))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry39))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry40))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,specialBuildCatalogEntry41))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot00))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot01))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot02))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot03))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot04))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot05))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot06))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot07))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot08))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot09))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot10))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot11))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot12))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot13))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot14))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot15))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot16))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot17))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot18))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot19))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot20))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot21))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot22))->activationSound = buttonVoiceSet;
  ((UiSpriteButtonControl *)INGAME_UI(inGameRoot,armyStockSlot23))->activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[3];
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsTabMilitary))->activationSound = g_UiButtonSoundVoiceSets7[3];
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsTabEconomy))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsTabThird))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsContinueButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsSecondaryExitButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsChartModeButtonA))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,resultsChartModeButtonB))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,gameMenuSaveButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,gameMenuGraphicsButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,gameMenuQuitButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,gameMenuAudioButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,gameMenuCloseButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,saveGameBackButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,saveGameSaveButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,saveGameDeleteButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,quitMenuBackButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,quitMenuAbortMissionButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,quitMenuSurrenderButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,quitMenuRestartMissionButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,graphicsOptionsBackButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,soundOptionsBackButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,messageSendButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,messageSendAndCloseButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,messageCancelButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyResearchButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyCloseButton))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,missionHelpCloseButton))->activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[4];
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,autoZoomOffCheckbox))->activationSound = g_UiButtonSoundVoiceSets7[4];
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,autoRotationOffCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,linkRotationZoomCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,linkRotationTiltCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,hidePanelCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,shadingEnabledCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,textureQualityLowButton))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,textureQualityMediumButton))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,textureQualityHighButton))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,musicEnabledCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,effectsEnabledCheckbox))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,reverseStereoCheckbox))->activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel32x32Button))->base.activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel32x64Button))->base.activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel32x128Button))->base.activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel64x64Button))->base.activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel64x128Button))->base.activationSound = buttonVoiceSet;
  ((UiNumericPairTextButton *)INGAME_UI(inGameRoot,shadingLevel128x128Button))->base.activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientPlayersTab))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientAllTab))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientGroupsTab))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox1))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox2))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox3))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox4))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox5))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox6))->activationSound = buttonVoiceSet;
  ((UiTextButtonControl *)INGAME_UI(inGameRoot,messageRecipientCheckbox7))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab1))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab2))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab3))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab4))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab5))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab6))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,technologyAreaTab7))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,missionHelpBriefingTab))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,missionHelpKeyboardTab))->activationSound = buttonVoiceSet;
  ((UiFramedTextButtonControl *)INGAME_UI(inGameRoot,missionHelpMouseTab))->activationSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[5];
  ((UiRangeSliderControl *)INGAME_UI(inGameRoot,modelDetailSlider))->clickSound = g_UiButtonSoundVoiceSets7[5];
  ((UiRangeSliderControl *)INGAME_UI(inGameRoot,effectsVolumeSlider))->clickSound = buttonVoiceSet;
  ((UiRangeSliderControl *)INGAME_UI(inGameRoot,movieVolumeSlider))->clickSound = buttonVoiceSet;
  ((UiRangeSliderControl *)INGAME_UI(inGameRoot,musicVolumeSlider))->clickSound = buttonVoiceSet;
  ((UiRangeSliderControl *)INGAME_UI(inGameRoot,messageMovieVolumeSlider))->clickSound = buttonVoiceSet;
  buttonVoiceSet = g_UiButtonSoundVoiceSets7[6];
  ((UiRequiredTextEditControl *)INGAME_UI(inGameRoot,saveNameEdit))->activationSound = g_UiButtonSoundVoiceSets7[6];
  ((UiListControl *)INGAME_UI(inGameRoot,saveGameList))->activationSound = buttonVoiceSet;
  ((UiRequiredTextEditControl *)INGAME_UI(inGameRoot,messageTextEdit))->activationSound = buttonVoiceSet;
  ((UiRequiredTextEditControl *)INGAME_UI(inGameRoot,chatInputTextEdit))->activationSound = buttonVoiceSet;
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
  ((UiFormattedContainer *)INGAME_UI(inGameRoot,xeniteGauge))->textureSource = diagramTexture;
  ((UiFormattedContainer *)INGAME_UI(inGameRoot,tritiumGauge))->textureSource = diagramTexture;
  ((UiFormattedContainer *)INGAME_UI(inGameRoot,energyGauge))->textureSource = diagramTexture;

  windowTexture = InGameUiRuntime_ReplaceTexturePackage
                    ((uint16_t *)g_GfxPanelWindowGfxPathUtf16,
                     (GraphicsTextureSourceAsset **)&g_InGameWindowTextureSource,&textureLoadError);
  if (windowTexture == nullptr) {
    *outError = textureLoadError;
    return false;
  }
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,messageWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,gameMenuWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,quitGameWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,saveGameWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,graphicsSettingsWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,audioSettingsWindow))->textureSource = windowTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,missionHelpWindow))->textureSource = windowTexture;

  techTexture = InGameUiRuntime_ReplaceTexturePackage
                  ((uint16_t *)g_GfxPanelTechGfxPathUtf16,
                   (GraphicsTextureSourceAsset **)&g_InGameTechnologyTextureSource,&textureLoadError);
  if (techTexture == nullptr) {
    *outError = textureLoadError;
    return false;
  }
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab1Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab2Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab3Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab4Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab5Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab6Icon))->textureSource = techTexture;
  ((UiImagePanelControl *)INGAME_UI(inGameRoot,technologyAreaTab7Icon))->textureSource = techTexture;
  InGameUiRuntime_SizeTechnologyWindow(inGameRoot,techTexture);
  InGameUiRuntime_AssignClickSounds(inGameRoot);
  return true;
}
