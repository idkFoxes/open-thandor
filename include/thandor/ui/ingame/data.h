/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/data.h
 */

#ifndef THANDOR_UI_INGAME_DATA_H
#define THANDOR_UI_INGAME_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint32_t g_RenderedFrameCountSinceDebugRefresh;

extern GraphicsTextureSet *g_TerrainMaterialTextureSets[38]; /* one texture set per terrain material (26 used, TERRAIN_MATERIAL_COUNT); the original's object runs on to 0x00503A74 with 12 more NULL entries. Original quirk: UiCommandMatrix_SelectIndex fills twelve swatches from a page base that can reach 15, so it reads entry 26 (always NULL, an empty swatch). */

extern uint32_t g_UiCommandModeGColorVariantFlags; /* uint32_t render-state flag word copied into terrain packets (primitives.c); ui/ingame/commands.c sets/clears the masked G-colour variant bit */

extern uint16_t g_ResourceRegistrationDirectoryUtf16[256];

extern uint16_t u_campagne_hex_0050e068[13];

extern uint16_t u_oldunit_hex_0050e094[12];

extern uint8_t g_InGameResourceRegistrationBusyCount;

extern uint32_t g_LocalPlayerRuntimeId;

extern UiNodeVtable g_UiNodeVtable_005162C0;

extern UiNodeVtable g_UiNodeVtable_00516310;

extern UiRootCallbacks g_UiRootCallbacks_0054FBC0;

extern uint16_t *g_InGamePlayerListTextScratchUtf16;

extern InGamePlayerStatusTextSlot g_InGamePlayerStatusTextSlots[8];

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailNameTextUtf16;

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailArmourTextUtf16;

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName0TextUtf16;

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName1TextUtf16;

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName2TextUtf16;

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailTextSlot05Utf16;

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailBuildXeniteCostTextUtf16;

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailBuildTimeTextUtf16;

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailEnergyTextUtf16;

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailTextSlot09Utf16;

/* one rich-text stream, patched in as payload 0 of the technology label
   (ui/ingame/technology.c): [0] command unit 0x8006 (RICHTEXT_OP_LITERAL_COLOR), [1..8] its eight colour digits
   (005504FE), [9..24] the xenite cost text (0055050E, RICHTEXT_RECORD_UNITS_LITERAL_COLOR units in); the
   interpreter reads on from the colour command into the text */
extern uint16_t g_InGameTechnologyCostRichText[25];

extern UiTechnologyValueTextBuffer16Utf16 g_InGameTechnologyEnergyCostTextUtf16;

extern UiTechnologyValueTextBuffer16Utf16 g_InGameTechnologyResearchTimeTextUtf16;

extern uint16_t g_InGameHudNumberTextUtf16[16];

extern uint16_t g_EmptyFrontendPlayerNameUtf16[1];

extern int32_t g_InGameSelectionInsertTripletDwordCount;

extern int32_t g_InGameSelectionRemoveTripletDwordCount; /* followed by 4 bytes 0x90 fill (dropped) */

extern InGameUiActionHandlerPage10Prefix40 g_InGameUiActionHandlersPage10;

extern InGameUiCommandModeActionHandlerPage11 g_InGameUiActionHandlersPage11;

extern InGameUiActionHandlerPage12Prefix28 g_InGameUiActionHandlersPage12;

extern int32_t g_UiAction100AControlOffsets[8];

extern uint32_t g_UiCommandSpriteVariantAColumnCount;

extern int32_t *g_UiCommandSpriteVariantAOffsetTables[5];

extern int32_t g_UiCommandSpriteVariantAOffsets[24];

extern int32_t g_UiAction1012PlayerIndexTextOffsets[7];

extern int32_t g_UiAction1012PlayerLabelTextOffsets[7];

extern int32_t g_UiAction1012IconImageOffsets[7];

extern int32_t g_UiAction1012StateTextOffsets[7];

extern int32_t g_UiAction1012ControlOffsets[7];

extern int32_t g_UiAction1012SlotPageOffsets[7];

extern int g_TechnologyPanelRowFlagOffsets[7];

extern int g_TechnologyPanelRowValueOffsets[7];

extern int g_InGameSelectionDetailGridCellOffsets[12];

extern int32_t g_UiSevenSlotSelectionControlOffsets[7];

extern uint32_t g_UiAction1012SubresourceByState[11]; /* uint32_t[11]: sprite subresource index (0xA9..0xAB) of the diplomacy row's relation icon per relation state; ui/ingame/runtime.c */

extern uint16_t u_gfx_panel_panel0_gfx_005630d0[21];

extern uint16_t u_gfx_panel_tech_gfx_005630fa[19];

extern uint16_t u_gfx_panel_diagram0_gfx_00563120[23];

extern uint16_t u_gfx_panel_window_gfx_0056318e[21];

extern uint16_t g_DeveloperChatPhraseUtf16[32];

extern uint16_t u_Hmmm__na_gut________0056321e[20];

extern uint32_t g_UiCommandRuntimeFlags;

extern int32_t g_InGamePanelTextureSubresource00Width;

extern int32_t g_InGamePanelTextureSubresource01Width;

extern int32_t g_InGamePanelTextureSubresource02Width;

extern int32_t g_InGamePanelTextureSubresource06Width;

extern int32_t g_InGamePanelTextureSubresource07Width;

extern int32_t g_InGamePanelTextureSubresource27Width;

extern int32_t g_InGamePanelTextureSubresource28Width;

extern int32_t g_InGamePanelTextureSubresource19Width;

extern int32_t g_InGamePanelTextureSubresource20Width;

extern int32_t g_InGamePanelTextureSubresource34Width;

extern int32_t g_InGamePanelTextureSubresource32Width;

extern int32_t g_InGamePanelTextureSubresource33Width;

extern int32_t g_InGamePanelTextureSubresource02Height;

extern int32_t g_InGamePanelTextureSubresource03Height;

extern int32_t g_InGamePanelTextureSubresource04Height;

extern int32_t g_InGamePanelTextureSubresource05Height;

extern int32_t g_InGamePanelTextureSubresource36Height;

extern int32_t g_InGamePanelTextureSubresource37Height;

extern int32_t g_InGamePanelTextureSubresource06Height;

extern int32_t g_InGamePanelTextureSubresource00Height;

extern int32_t g_InGamePanelTextureSubresource07Height;

extern int32_t g_InGamePanelTextureSubresource26Height;

extern int32_t g_InGamePanelTextureSubresource31Height;

extern int32_t g_InGamePanelTextureSubresource18Height;

extern int32_t g_InGamePanelTextureSubresource23Height;

extern int32_t g_InGamePanelTextureSubresource34Height;

extern int32_t g_InGamePanelTextureSubresource32Height;

extern UiCommandRuntimeRecordPrefix *g_UiHoverSelectionRecord;

extern PckTechnologyIdCatalog g_InGameSelectedTechnologyId; /* followed by 28 bytes 0x90 fill (dropped) */

extern uint16_t g_FrontendDebugOverlayTextSlot10Utf16[16];

extern uint16_t g_FrontendDebugOverlayTextSlot11Utf16[16];

extern uint32_t g_InGameReadyStateToggleFlags;

extern UiCommandRuntimeRecordPrefix *g_UiCatalogGroup48Records[48];

extern UiCommandRuntimeRecordPrefix *g_UiCatalogGroup42Records[42];

extern UiCommandRuntimeRecordPrefix *g_UiCommandSpriteVariantARecords[24];

extern uint32_t g_UiAction1012TargetPlayerIndices[7];

extern UiCommandDispatchRecord g_InGameCommandDispatchRecords[64]; /* 63 records + terminator at 00567CA4 */

extern uint32_t g_UiCommandModeG;

extern uint32_t g_UiCommandModeC;

extern uint32_t g_UiCommandModeD;

extern uint32_t g_UiCommandModeE;

extern uint32_t g_UiCommandModeA;

extern uint32_t g_UiCommandModeB; /* followed by an all-zero dword (0x0056D738) no code reaches (dropped) */

extern uint32_t g_UiCommandModeF;

extern uint32_t g_UiCommandAbsoluteSelectionIndex;

extern uint32_t g_UiCommandSelectionPageBaseIndex;

extern uint32_t g_UiCommandTerrainMaskToggleValue;

extern FactionRuntimeIndex g_UiCommandModeGOwnerFactionIndex;

extern uint32_t g_UiCommandModeGArmyAssetId;

extern int32_t g_UiCommandDragReferenceX;

extern int32_t g_UiCommandDragReferenceY;

extern PckArmyAssetIdCatalog g_UiCommandMode4ArmyAssetId;

extern uint32_t g_UiCommandCallerMaskHighBit;

extern uint32_t g_UiCommandModeGPrimaryPageIndices[6]; /* uint32_t[6]: active page of the mode preview page stack per command mode G; ui/ingame commands/runtime */

extern uint32_t g_UiCommandModeGSecondaryPageIndices[6]; /* uint32_t[6]: active page of the mode detail page stack per command mode G; ui/ingame commands/runtime */

extern uint32_t g_UiCommandModeGTertiaryPageIndices[6]; /* uint32_t[6]: active page of the mode command page stack per command mode G; ui/ingame commands/runtime */

extern int32_t g_UiCommandModeGControlOffsets[6];

extern void *g_UiCommandModeGHandlers[6];

extern int32_t g_UiMappedCommandControlOffsets[12];

extern int32_t g_UiCommandSelectionAnchorWorldXQ12;

extern int32_t g_UiCommandSelectionAnchorWorldYQ12;

extern int32_t g_UiCommandSelectionCurrentWorldXQ12;

extern int32_t g_UiCommandSelectionCurrentWorldYQ12;

extern uint32_t g_UiCommandDragAnchorWorldXQ12;

extern uint32_t g_UiCommandDragAnchorWorldYQ12;

extern uint32_t g_UiCommandDragStartScreenX;

extern uint32_t g_UiCommandDragStartScreenY;

extern DirectSoundVoiceSet *g_UiButtonSoundVoiceSets7[7];

#endif
