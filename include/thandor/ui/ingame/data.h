/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/data.h
 */

#ifndef THANDOR_UI_INGAME_DATA_H
#define THANDOR_UI_INGAME_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint32_t g_RenderedFrameCountSinceDebugRefresh; /* 0048584C g_RenderedFrameCountSinceDebugRefresh */

extern GraphicsTextureSet *g_TerrainMaterialTextureSets[38]; /* 005039DC g_TerrainMaterialTextureSets: one texture set per terrain material (26 used, TERRAIN_MATERIAL_COUNT); the original's object runs on to 0x00503A74 with 12 more NULL entries. Original quirk: UiCommandMatrix_SelectIndex fills twelve swatches from a page base that can reach 15, so it reads entry 26 (always NULL, an empty swatch). */

extern uint32_t g_UiCommandModeGColorVariantFlags; /* 00503A88 g_UiCommandModeGColorVariantFlags: uint32_t render-state flag word copied into terrain packets (primitives.c); ui/ingame/commands.c sets/clears the masked G-colour variant bit */

extern uint16_t g_ResourceRegistrationDirectoryUtf16[256]; /* 0050DCC4 g_ResourceRegistrationDirectoryUtf16 */

extern uint16_t u_campagne_hex_0050e068[13]; /* 0050E068 u_campagne_hex_0050e068 */

extern uint16_t u_oldunit_hex_0050e094[12]; /* 0050E094 u_oldunit_hex_0050e094 */

extern uint8_t g_InGameResourceRegistrationBusyCount; /* 0050E0AC g_InGameResourceRegistrationBusyCount */

extern uint32_t g_LocalPlayerRuntimeId; /* 0050F0AC g_LocalPlayerRuntimeId */

extern UiNodeVtable g_UiNodeVtable_005162C0; /* 005162C0 g_UiNodeVtable_005162C0 */

extern UiNodeVtable g_UiNodeVtable_00516310; /* 00516310 g_UiNodeVtable_00516310 */

extern UiRootCallbacks g_UiRootCallbacks_0054FBC0; /* 0054FBC0 g_UiRootCallbacks_0054FBC0 */

extern uint16_t *g_InGamePlayerListTextScratchUtf16; /* 0054FBD8 g_InGamePlayerListTextScratchUtf16 */

extern InGamePlayerStatusTextSlot g_InGamePlayerStatusTextSlots[8]; /* 0054FBDC g_InGamePlayerStatusTextSlots */

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailNameTextUtf16; /* 0054FFDC g_InGameSelectionDetailNameTextUtf16 */

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailArmourTextUtf16; /* 0055005C g_InGameSelectionDetailArmourTextUtf16 */

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName0TextUtf16; /* 005500DC g_InGameSelectionDetailWeaponName0TextUtf16 */

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName1TextUtf16; /* 0055015C g_InGameSelectionDetailWeaponName1TextUtf16 */

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName2TextUtf16; /* 005501DC g_InGameSelectionDetailWeaponName2TextUtf16 */

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailTextSlot05Utf16; /* 0055025C g_InGameSelectionDetailTextSlot05Utf16 */

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailBuildXeniteCostTextUtf16; /* 005502DC g_InGameSelectionDetailBuildXeniteCostTextUtf16 */

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailBuildTimeTextUtf16; /* 0055035C g_InGameSelectionDetailBuildTimeTextUtf16 */

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailEnergyTextUtf16; /* 005503DC g_InGameSelectionDetailEnergyTextUtf16 */

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailTextSlot09Utf16; /* 0055045C g_InGameSelectionDetailTextSlot09Utf16 */

/* 005504FC g_InGameTechnologyCostRichText: one rich-text stream, patched in as payload 0 of the technology label
   (ui/ingame/technology.c): [0] command unit 0x8006 (RICHTEXT_OP_LITERAL_COLOR), [1..8] its eight colour digits
   (005504FE), [9..24] the xenite cost text (0055050E, RICHTEXT_RECORD_UNITS_LITERAL_COLOR units in); the
   interpreter reads on from the colour command into the text */
extern uint16_t g_InGameTechnologyCostRichText[25];

extern UiTechnologyValueTextBuffer16Utf16 g_InGameTechnologyEnergyCostTextUtf16; /* 0055052E g_InGameTechnologyEnergyCostTextUtf16 */

extern UiTechnologyValueTextBuffer16Utf16 g_InGameTechnologyResearchTimeTextUtf16; /* 0055054E g_InGameTechnologyResearchTimeTextUtf16 */

extern uint16_t g_InGameHudNumberTextUtf16[16]; /* 0055056E g_InGameHudNumberTextUtf16 */

extern uint16_t g_EmptyFrontendPlayerNameUtf16[1]; /* 0055058E g_EmptyFrontendPlayerNameUtf16 */

extern int32_t g_InGameSelectionInsertTripletDwordCount; /* 0055F124 g_InGameSelectionInsertTripletDwordCount */

extern int32_t g_InGameSelectionRemoveTripletDwordCount; /* 0055F128 g_InGameSelectionRemoveTripletDwordCount: followed by 4 bytes 0x90 fill (dropped) */

extern InGameUiActionHandlerPage10Prefix40 g_InGameUiActionHandlersPage10; /* 005624A0 g_InGameUiActionHandlersPage10 */

extern InGameUiCommandModeActionHandlerPage11 g_InGameUiActionHandlersPage11; /* 00562540 g_InGameUiActionHandlersPage11 */

extern InGameUiActionHandlerPage12Prefix28 g_InGameUiActionHandlersPage12; /* 005625B8 g_InGameUiActionHandlersPage12 */

extern int32_t g_UiAction100AControlOffsets[8]; /* 00562628 g_UiAction100AControlOffsets */

extern uint32_t g_UiCommandSpriteVariantAColumnCount; /* 00562650 g_UiCommandSpriteVariantAColumnCount */

extern int32_t *g_UiCommandSpriteVariantAOffsetTables[5]; /* 00562694 g_UiCommandSpriteVariantAOffsetTables */

extern int32_t g_UiCommandSpriteVariantAOffsets[24]; /* 00562C60 g_UiCommandSpriteVariantAOffsets */

extern int32_t g_UiAction1012PlayerIndexTextOffsets[7]; /* 00562CC0 g_UiAction1012PlayerIndexTextOffsets */

extern int32_t g_UiAction1012PlayerLabelTextOffsets[7]; /* 00562CDC g_UiAction1012PlayerLabelTextOffsets */

extern int32_t g_UiAction1012IconImageOffsets[7]; /* 00562CF8 g_UiAction1012IconImageOffsets */

extern int32_t g_UiAction1012StateTextOffsets[7]; /* 00562D14 g_UiAction1012StateTextOffsets */

extern int32_t g_UiAction1012ControlOffsets[7]; /* 00562D30 g_UiAction1012ControlOffsets */

extern int32_t g_UiAction1012SlotPageOffsets[7]; /* 00562D4C g_UiAction1012SlotPageOffsets */

extern int g_TechnologyPanelRowFlagOffsets[7]; /* 00562D68 g_TechnologyPanelRowFlagOffsets */

extern int g_TechnologyPanelRowValueOffsets[7]; /* 00562D84 g_TechnologyPanelRowValueOffsets */

extern int g_InGameSelectionDetailGridCellOffsets[12]; /* 00562DA0 g_InGameSelectionDetailGridCellOffsets */

extern int32_t g_UiSevenSlotSelectionControlOffsets[7]; /* 00562DD0 g_UiSevenSlotSelectionControlOffsets */

extern uint32_t g_UiAction1012SubresourceByState[11]; /* 00562E1C g_UiAction1012SubresourceByState: uint32_t[11]: sprite subresource index (0xA9..0xAB) of the diplomacy row's relation icon per relation state; ui/ingame/runtime.c */

extern uint16_t u_gfx_panel_panel0_gfx_005630d0[21]; /* 005630D0 u_gfx_panel_panel0_gfx_005630d0 */

extern uint16_t u_gfx_panel_tech_gfx_005630fa[19]; /* 005630FA u_gfx_panel_tech_gfx_005630fa */

extern uint16_t u_gfx_panel_diagram0_gfx_00563120[23]; /* 00563120 u_gfx_panel_diagram0_gfx_00563120 */

extern uint16_t u_gfx_panel_window_gfx_0056318e[21]; /* 0056318E u_gfx_panel_window_gfx_0056318e */

extern uint16_t g_DeveloperChatPhraseUtf16[32]; /* 005631DE g_DeveloperChatPhraseUtf16 */

extern uint16_t u_Hmmm__na_gut________0056321e[20]; /* 0056321E u_Hmmm__na_gut________0056321e */

extern uint32_t g_UiCommandRuntimeFlags; /* 00563260 g_UiCommandRuntimeFlags */

extern int32_t g_InGamePanelTextureSubresource00Width; /* 00563290 g_InGamePanelTextureSubresource00Width */

extern int32_t g_InGamePanelTextureSubresource01Width; /* 00563294 g_InGamePanelTextureSubresource01Width */

extern int32_t g_InGamePanelTextureSubresource02Width; /* 00563298 g_InGamePanelTextureSubresource02Width */

extern int32_t g_InGamePanelTextureSubresource06Width; /* 0056329C g_InGamePanelTextureSubresource06Width */

extern int32_t g_InGamePanelTextureSubresource07Width; /* 005632A0 g_InGamePanelTextureSubresource07Width */

extern int32_t g_InGamePanelTextureSubresource27Width; /* 005632A4 g_InGamePanelTextureSubresource27Width */

extern int32_t g_InGamePanelTextureSubresource28Width; /* 005632A8 g_InGamePanelTextureSubresource28Width */

extern int32_t g_InGamePanelTextureSubresource19Width; /* 005632AC g_InGamePanelTextureSubresource19Width */

extern int32_t g_InGamePanelTextureSubresource20Width; /* 005632B0 g_InGamePanelTextureSubresource20Width */

extern int32_t g_InGamePanelTextureSubresource34Width; /* 005632B4 g_InGamePanelTextureSubresource34Width */

extern int32_t g_InGamePanelTextureSubresource32Width; /* 005632B8 g_InGamePanelTextureSubresource32Width */

extern int32_t g_InGamePanelTextureSubresource33Width; /* 005632BC g_InGamePanelTextureSubresource33Width */

extern int32_t g_InGamePanelTextureSubresource02Height; /* 005632C0 g_InGamePanelTextureSubresource02Height */

extern int32_t g_InGamePanelTextureSubresource03Height; /* 005632C4 g_InGamePanelTextureSubresource03Height */

extern int32_t g_InGamePanelTextureSubresource04Height; /* 005632C8 g_InGamePanelTextureSubresource04Height */

extern int32_t g_InGamePanelTextureSubresource05Height; /* 005632CC g_InGamePanelTextureSubresource05Height */

extern int32_t g_InGamePanelTextureSubresource36Height; /* 005632D0 g_InGamePanelTextureSubresource36Height */

extern int32_t g_InGamePanelTextureSubresource37Height; /* 005632D4 g_InGamePanelTextureSubresource37Height */

extern int32_t g_InGamePanelTextureSubresource06Height; /* 005632D8 g_InGamePanelTextureSubresource06Height */

extern int32_t g_InGamePanelTextureSubresource00Height; /* 005632DC g_InGamePanelTextureSubresource00Height */

extern int32_t g_InGamePanelTextureSubresource07Height; /* 005632E0 g_InGamePanelTextureSubresource07Height */

extern int32_t g_InGamePanelTextureSubresource26Height; /* 005632E4 g_InGamePanelTextureSubresource26Height */

extern int32_t g_InGamePanelTextureSubresource31Height; /* 005632E8 g_InGamePanelTextureSubresource31Height */

extern int32_t g_InGamePanelTextureSubresource18Height; /* 005632EC g_InGamePanelTextureSubresource18Height */

extern int32_t g_InGamePanelTextureSubresource23Height; /* 005632F0 g_InGamePanelTextureSubresource23Height */

extern int32_t g_InGamePanelTextureSubresource34Height; /* 005632F4 g_InGamePanelTextureSubresource34Height */

extern int32_t g_InGamePanelTextureSubresource32Height; /* 005632F8 g_InGamePanelTextureSubresource32Height */

extern UiCommandRuntimeRecordPrefix *g_UiHoverSelectionRecord; /* 005632FC g_UiHoverSelectionRecord */

extern PckTechnologyIdCatalog g_InGameSelectedTechnologyId; /* 00563300 g_InGameSelectedTechnologyId: followed by 28 bytes 0x90 fill (dropped) */

extern uint16_t g_FrontendDebugOverlayTextSlot10Utf16[16]; /* 00563474 g_FrontendDebugOverlayTextSlot10Utf16 */

extern uint16_t g_FrontendDebugOverlayTextSlot11Utf16[16]; /* 00563494 g_FrontendDebugOverlayTextSlot11Utf16 */

extern uint32_t g_InGameReadyStateToggleFlags; /* 00563514 g_InGameReadyStateToggleFlags */

extern UiCommandRuntimeRecordPrefix *g_UiCatalogGroup48Records[48]; /* 00563518 g_UiCatalogGroup48Records */

extern UiCommandRuntimeRecordPrefix *g_UiCatalogGroup42Records[42]; /* 005635D8 g_UiCatalogGroup42Records */

extern UiCommandRuntimeRecordPrefix *g_UiCommandSpriteVariantARecords[24]; /* 00563680 g_UiCommandSpriteVariantARecords */

extern uint32_t g_UiAction1012TargetPlayerIndices[7]; /* 005636E0 g_UiAction1012TargetPlayerIndices */

extern UiCommandDispatchRecord g_InGameCommandDispatchRecords[64]; /* 005679B0 g_InGameCommandDispatchRecords: 63 records + terminator at 00567CA4 */

extern uint32_t g_UiCommandModeG; /* 0056D720 g_UiCommandModeG */

extern uint32_t g_UiCommandModeC; /* 0056D724 g_UiCommandModeC */

extern uint32_t g_UiCommandModeD; /* 0056D728 g_UiCommandModeD */

extern uint32_t g_UiCommandModeE; /* 0056D72C g_UiCommandModeE */

extern uint32_t g_UiCommandModeA; /* 0056D730 g_UiCommandModeA */

extern uint32_t g_UiCommandModeB; /* 0056D734 g_UiCommandModeB: followed by an all-zero dword (0x0056D738) no code reaches (dropped) */

extern uint32_t g_UiCommandModeF; /* 0056D73C g_UiCommandModeF */

extern uint32_t g_UiCommandAbsoluteSelectionIndex; /* 0056D740 g_UiCommandAbsoluteSelectionIndex */

extern uint32_t g_UiCommandSelectionPageBaseIndex; /* 0056D744 g_UiCommandSelectionPageBaseIndex */

extern uint32_t g_UiCommandTerrainMaskToggleValue; /* 0056D748 g_UiCommandTerrainMaskToggleValue */

extern FactionRuntimeIndex g_UiCommandModeGOwnerFactionIndex; /* 0056D74C g_UiCommandModeGOwnerFactionIndex */

extern uint32_t g_UiCommandModeGArmyAssetId; /* 0056D750 g_UiCommandModeGArmyAssetId */

extern int32_t g_UiCommandDragReferenceX; /* 0056D754 g_UiCommandDragReferenceX */

extern int32_t g_UiCommandDragReferenceY; /* 0056D758 g_UiCommandDragReferenceY */

extern PckArmyAssetIdCatalog g_UiCommandMode4ArmyAssetId; /* 0056D75C g_UiCommandMode4ArmyAssetId */

extern uint32_t g_UiCommandCallerMaskHighBit; /* 0056D760 g_UiCommandCallerMaskHighBit */

extern uint32_t g_UiCommandModeGPrimaryPageIndices[6]; /* 0056D764 g_UiCommandModeGPrimaryPageIndices: uint32_t[6]: active page of the mode preview page stack per command mode G; ui/ingame commands/runtime */

extern uint32_t g_UiCommandModeGSecondaryPageIndices[6]; /* 0056D77C g_UiCommandModeGSecondaryPageIndices: uint32_t[6]: active page of the mode detail page stack per command mode G; ui/ingame commands/runtime */

extern uint32_t g_UiCommandModeGTertiaryPageIndices[6]; /* 0056D794 g_UiCommandModeGTertiaryPageIndices: uint32_t[6]: active page of the mode command page stack per command mode G; ui/ingame commands/runtime */

extern int32_t g_UiCommandModeGControlOffsets[6]; /* 0056D7AC g_UiCommandModeGControlOffsets */

extern void *g_UiCommandModeGHandlers[6]; /* 0056D7C4 g_UiCommandModeGHandlers */

extern int32_t g_UiMappedCommandControlOffsets[12]; /* 0056D7DC g_UiMappedCommandControlOffsets */

extern int32_t g_UiCommandSelectionAnchorWorldXQ12; /* 0056D80C g_UiCommandSelectionAnchorWorldXQ12 */

extern int32_t g_UiCommandSelectionAnchorWorldYQ12; /* 0056D810 g_UiCommandSelectionAnchorWorldYQ12 */

extern int32_t g_UiCommandSelectionCurrentWorldXQ12; /* 0056D814 g_UiCommandSelectionCurrentWorldXQ12 */

extern int32_t g_UiCommandSelectionCurrentWorldYQ12; /* 0056D818 g_UiCommandSelectionCurrentWorldYQ12 */

extern uint32_t g_UiCommandDragAnchorWorldXQ12; /* 0056D81C g_UiCommandDragAnchorWorldXQ12 */

extern uint32_t g_UiCommandDragAnchorWorldYQ12; /* 0056D820 g_UiCommandDragAnchorWorldYQ12 */

extern uint32_t g_UiCommandDragStartScreenX; /* 0056D824 g_UiCommandDragStartScreenX */

extern uint32_t g_UiCommandDragStartScreenY; /* 0056D828 g_UiCommandDragStartScreenY */

extern DirectSoundVoiceSet *g_UiButtonSoundVoiceSets7[7]; /* 00572AD8 g_UiButtonSoundVoiceSets7 */

#endif
