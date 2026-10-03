/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/ui/ingame/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(4)) uint32_t g_RenderedFrameCountSinceDebugRefresh = 0;

/* one texture set per terrain material (26 used, TERRAIN_MATERIAL_COUNT); the original's object runs on to 0x00503A74 with 12 more NULL entries. Original quirk: UiCommandMatrix_SelectIndex fills twelve swatches from a page base that can reach 15, so it reads entry 26 (always NULL, an empty swatch). */
__declspec(align(4)) GraphicsTextureSet *g_TerrainMaterialTextureSets[38] = {0};

/* uint32_t render-state flag word copied into terrain packets (primitives.c); ui/ingame/commands.c sets/clears the masked G-colour variant bit */
__declspec(align(8)) uint32_t g_UiCommandModeGColorVariantFlags = 0x10000;

__declspec(align(4)) uint16_t g_ResourceRegistrationDirectoryUtf16[256] = {0};

__declspec(align(8)) uint16_t u_campagne_hex_0050e068[13] = L"campagne.hex";

__declspec(align(4)) uint16_t u_oldunit_hex_0050e094[12] = L"oldunit.hex";

__declspec(align(4)) uint8_t g_InGameResourceRegistrationBusyCount = 0;

__declspec(align(4)) uint32_t g_LocalPlayerRuntimeId = 0;

__declspec(align(16)) UiNodeVtable g_UiNodeVtable_005162C0 = {
        .relocate = (void *)UiSpriteButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiSpriteButtonControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiCommandSpriteButtonControl_BeginPress,
        .nonRightRelease = (void *)UiCommandSpriteButtonControl_NonRightRelease,
        .rightPress = (void *)UiCommandSpriteButtonControl_BeginPress,
        .rightRelease = (void *)UiCommandSpriteButtonControl_RightRelease,
        .nonRightDrag = (void *)UiSpriteButtonControl_NonRightDrag,
        .rightDrag = (void *)UiSpriteButtonControl_NonRightDrag,
        .pointerMove = (void *)InGameArmyStock_PointerMoveShowSlotDetails,
        .hitTest = (void *)UiSpriteButtonControl_HitTestOpaque,
        .keyboardEvent = (void *)UiSelectableControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

__declspec(align(16)) UiNodeVtable g_UiNodeVtable_00516310 = {
        .relocate = (void *)UiSpriteButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiSpriteButtonControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiCommandSpriteButtonControl_BeginPress,
        .nonRightRelease = (void *)UiCommandSpriteButtonControl_NonRightRelease,
        .rightPress = (void *)UiCommandSpriteButtonControl_BeginPress,
        .rightRelease = (void *)UiCommandSpriteButtonControl_RightRelease,
        .nonRightDrag = (void *)UiSpriteButtonControl_NonRightDrag,
        .rightDrag = (void *)UiSpriteButtonControl_NonRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiSpriteButtonControl_HitTestOpaque,
        .keyboardEvent = (void *)UiSelectableControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

__declspec(align(16)) UiRootCallbacks g_UiRootCallbacks_0054FBC0 = {
    .frameUpdate = (void *)InGameUiRoot_UpdateFrame,
    .keyboardFallback = (void *)InGameHotkeys_DispatchCommandByFlags};

__declspec(align(8)) uint16_t *g_InGamePlayerListTextScratchUtf16 = 0;

__declspec(align(4)) InGamePlayerStatusTextSlot g_InGamePlayerStatusTextSlots[8] = {0};

__declspec(align(4)) UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailNameTextUtf16 = {0};

__declspec(align(4)) UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailArmourTextUtf16 = {0};

__declspec(align(4)) UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName0TextUtf16 = {0};

__declspec(align(4)) UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName1TextUtf16 = {0};

__declspec(align(4)) UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName2TextUtf16 = {0};

__declspec(align(4)) UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailTextSlot05Utf16 = {0};

__declspec(align(4)) UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailBuildXeniteCostTextUtf16 = {0};

__declspec(align(4)) UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailBuildTimeTextUtf16 = {0};

__declspec(align(4)) UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailEnergyTextUtf16 = {0};

__declspec(align(4)) UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailTextSlot09Utf16 = {0};

/* one rich-text stream, patched in as payload 0 of the technology label
   (ui/ingame/technology.c): [0] command unit 0x8006 (RICHTEXT_OP_LITERAL_COLOR), [1..8] its eight colour digits
   (005504FE), [9..24] the xenite cost text (0055050E); the interpreter reads on from the colour command into the
   text */
__declspec(align(4)) uint16_t g_InGameTechnologyCostRichText[25] = {32774};

__declspec(align(4)) UiTechnologyValueTextBuffer16Utf16 g_InGameTechnologyEnergyCostTextUtf16 = {0};

__declspec(align(4)) UiTechnologyValueTextBuffer16Utf16 g_InGameTechnologyResearchTimeTextUtf16 = {0};

__declspec(align(4)) uint16_t g_InGameHudNumberTextUtf16[16] = {0};

__declspec(align(4)) uint16_t g_EmptyFrontendPlayerNameUtf16[1] = {0};

__declspec(align(4)) int32_t g_InGameSelectionInsertTripletDwordCount = 0;

__declspec(align(8)) int32_t g_InGameSelectionRemoveTripletDwordCount = 0;

__declspec(align(16)) InGameUiActionHandlerPage10Prefix40 g_InGameUiActionHandlersPage10 = {
        .handlers = {
            /*  0 */ (void *)InGameMapAction_RecenterViewFromGridCoordinates,
            /*  1 */ (void *)InGameArmyStock_TakeOrSellSlotArmy,
            /*  2 */ (void *)InGameSevenSlotCommand_ClosePage,
            /*  3 */ (void *)InGameSettingsPage_ToggleAndSynchronizeControls,
            /*  4 */ (void *)InGameSevenSlotCommand_SubmitTextAndSelectionMask,
            /*  5 */ (void *)InGameSevenSlotCommand_SubmitAndClosePage,
            /*  6 */ (void *)InGameSelectionPage_RebuildActivePlayerEntries,
            /*  7 */ (void *)InGameSelectionPage_RebuildRuntimeRecordEntries,
            /*  8 */ (void *)InGameSelectionPage_ShowSubpage1,
            /*  9 */ (void *)InGameEndMovie_Skip,
            /* 10 */ (void *)InGameSelectionGroupButton_RecallOrStoreGroup,
            /* 11 */ (void *)InGameBuildCatalog_QueueOrCancelEntry,
            /* 12 */ (void *)InGameSpecialBuildCatalog_QueueOrCancelEntry,
            /* 13 */ (void *)InGameTargetingContext_AdvanceOrResolveTarget,
            /* 14 */ (void *)InGameTargetingContext_CancelAndRestoreState,
            /* 15 */ (void *)InGameRecentText_TrimHistoryToThree,
            /* 16 */ (void *)InGameTechnologyPanel_ToggleForSelection,
            /* 17 */ (void *)InGameCommandAction_ClearSelectedArmyTokenAndClosePage,
            /* 18 */ (void *)InGameOtherPlayerCommand_DispatchSelectedTarget,
            /* 19 */ (void *)InGameTechnologyResearch_StartSelected,
            /* 20 */ (void *)InGameTechnologyAreaTab_SelectAndRebuild,
            /* 21 */ (void *)InGameTechnologyAreaTab_SelectAndRebuild,
            /* 22 */ (void *)InGameTechnologyAreaTab_SelectAndRebuild,
            /* 23 */ (void *)InGameTechnologyAreaTab_SelectAndRebuild,
            /* 24 */ (void *)InGameTechnologyAreaTab_SelectAndRebuild,
            /* 25 */ (void *)InGameTechnologyAreaTab_SelectAndRebuild,
            /* 26 */ (void *)InGameTechnologyAreaTab_SelectAndRebuild,
            /* 27 */ (void *)InGameResultsScreen_ContinueOrMarkReady,
            /* 28 */ (void *)InGameResultsScreen_SelectChartTab,
            /* 29 */ (void *)InGameQuitMenu_AbortMission,
            /* 30 */ (void *)InGameQuitMenu_Surrender,
            /* 31 */ (void *)InGameMissionHelpPage_Toggle,
            /* 32 */ (void *)InGameSettingsAction_CloseAlternatePanel,
            /* 33 */ (void *)InGameMissionHelpPage_SelectBriefingTab,
            /* 34 */ (void *)InGameMissionHelpPage_SelectKeyboardTab,
            /* 35 */ (void *)InGameMissionHelpPage_SelectMouseTab,
            /* 36 */ (void *)InGameChatInput_SendLineOrCheckCheatPhrase,
            /* 37 */ (void *)InGameResultsScreen_CloseLocally,
            /* 38 */ (void *)InGameCommandState_SelectAndPropagateBinaryMode,
            /* 39 */ (void *)InGameQuitMenu_RestartMission
        }};

__declspec(align(16)) InGameUiCommandModeActionHandlerPage11 g_InGameUiActionHandlersPage11 = {
        .handlers = {
            /*  0 */ (void *)InGameCommandModeG_Select0,
            /*  1 */ (void *)InGameCommandModeG_Select1,
            /*  2 */ (void *)InGameCommandModeG_Select2,
            /*  3 */ 0,
            /*  4 */ (void *)InGameCommandModeG_Select5,
            /*  5 */ (void *)InGameCommandModeG_Select3,
            /*  6 */ (void *)InGameCommandModeG_Select4,
            /*  7 */ 0,
            /*  8 */ (void *)InGameCommandModeC_Select0,
            /*  9 */ (void *)InGameCommandModeC_Select1,
            /* 10 */ (void *)InGameCommandModeC_Select2,
            /* 11 */ (void *)InGameCommandModeC_Select3,
            /* 12 */ (void *)InGameCommandModeD_Select0,
            /* 13 */ (void *)InGameCommandModeD_Select1,
            /* 14 */ (void *)InGameCommandModeD_Select2,
            /* 15 */ (void *)InGameCommandModeD_Select3,
            /* 16 */ (void *)InGameCommandMatrix_SelectMappedControl,
            /* 17 */ (void *)InGameCommandModeA_Select0,
            /* 18 */ (void *)InGameCommandModeA_Select1,
            /* 19 */ (void *)InGameCommandModeA_Select2,
            /* 20 */ (void *)InGameCommandModeB_Select0,
            /* 21 */ (void *)InGameCommandModeB_Select1,
            /* 22 */ (void *)InGameCommandModeB_Select2,
            /* 23 */ (void *)InGameCommandModeE_Select0,
            /* 24 */ (void *)InGameCommandModeE_Select1,
            /* 25 */ (void *)InGameCommandModeE_Select2,
            /* 26 */ (void *)InGameCommandRange_DispatchState0,
            /* 27 */ (void *)InGameCommandRange_DispatchState1,
            /* 28 */ (void *)InGameCommandModeF_Select0,
            /* 29 */ (void *)InGameCommandModeF_Select1
        }};

__declspec(align(8)) InGameUiActionHandlerPage12Prefix28 g_InGameUiActionHandlersPage12 = {
        .handlers = {
            /*  0 */ (void *)InGameQuitMenu_OpenAndRefreshButtons,
            /*  1 */ (void *)InGameSettingsPage_CloseViaSharedToggle,
            /*  2 */ (void *)InGameGraphicsSettings_OpenAndSynchronize,
            /*  3 */ (void *)InGameAudioSettings_OpenAndSynchronize,
            /*  4 */ (void *)InGameShadingSettings_SetEnabled,
            /*  5 */ (void *)InGameShadingSettings_ApplyLevel,
            /*  6 */ (void *)InGameModelSettings_SetLodDepthThresholdQ8,
            /*  7 */ (void *)InGameTextureSettings_SetQuality,
            /*  8 */ (void *)InGameAudioSettings_SetEffectsEnabled,
            /*  9 */ (void *)InGameAudioSettings_SetMusicEnabled,
            /* 10 */ (void *)InGameAudioSettings_SetReverseStereo,
            /* 11 */ (void *)InGameAudioSettings_SetEffectsGain,
            /* 12 */ (void *)InGameAudioSettings_SetMovieDefaultGain,
            /* 13 */ (void *)InGameAudioSettings_SetMusicGain,
            /* 14 */ (void *)InGameSaveGamePage_RebuildCatalog,
            /* 15 */ (void *)InGameSaveGameList_SelectAndRefreshDetail,
            /* 16 */ (void *)InGameSaveGame_SaveSelectedOrTypedName,
            /* 17 */ (void *)InGameSaveName_UpdateSaveActionValidity,
            /* 18 */ (void *)InGameGameplaySettings_SetAutomaticZoomOff,
            /* 19 */ (void *)InGameGameplaySettings_SetAutomaticRotationOff,
            /* 20 */ (void *)InGameGameplaySettings_SetLinkRotationZoom,
            /* 21 */ (void *)InGameGameplaySettings_SetLinkRotationTilt,
            /* 22 */ (void *)InGameGameplaySettings_SetRightButtonDoesNotScroll,
            /* 23 */ (void *)InGameGameplaySettings_SetCameraScrollStep,
            /* 24 */ (void *)InGameSettingsPage_OpenViaSharedToggle,
            /* 25 */ (void *)InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog,
            /* 26 */ (void *)InGameAudioSettings_SetMovieAlternateGain,
            /* 27 */ (void *)InGameGameplaySettings_SetHidePanel
        }};

__declspec(align(8)) int32_t g_UiAction100AControlOffsets[8] = {45584, 45712, 45840, 45968, 46096, 46224, 46352, 46480};

__declspec(align(16)) uint32_t g_UiCommandSpriteVariantAColumnCount = 0;

__declspec(align(4)) int32_t *g_UiCommandSpriteVariantAOffsetTables[5] = {
    /* 0 */ (void *)&g_UiCommandSpriteVariantAOffsets,
    /* 1 */ (void *)&g_UiCommandSpriteVariantAOffsets,
    /* 2 */ (void *)&g_UiCommandSpriteVariantAOffsets,
    /* 3 */ (void *)&g_UiCommandSpriteVariantAOffsets,
    /* 4 */ (void *)&g_UiCommandSpriteVariantAOffsets};

__declspec(align(16)) int32_t g_UiCommandSpriteVariantAOffsets[24] = {
    /*  0 */ 36116, 36240, 36364, 36488, 36612, 36736, 36860, 36984,
    /*  8 */ 37108, 37232, 37356, 37480, 37604, 37728, 37852, 37976,
    /* 16 */ 38100, 38224, 38348, 38472, 38596, 38720, 38844, 38968};

__declspec(align(16)) int32_t g_UiAction1012PlayerIndexTextOffsets[7] = {20540, 20632, 20724, 20816, 20908, 21000, 21092};

__declspec(align(4)) int32_t g_UiAction1012PlayerLabelTextOffsets[7] = {21184, 21276, 21368, 21460, 21552, 21644, 21736};

__declspec(align(8)) int32_t g_UiAction1012IconImageOffsets[7] = {22472, 22564, 22656, 22748, 22840, 22932, 23024};

__declspec(align(4)) int32_t g_UiAction1012StateTextOffsets[7] = {0x5544, 0x55A0, 0x55FC, 0x5658, 0x56B4, 0x5710, 0x576C};

__declspec(align(16)) int32_t g_UiAction1012ControlOffsets[7] = {23116, 23240, 23364, 23488, 23612, 23736, 23860};

__declspec(align(4)) int32_t g_UiAction1012SlotPageOffsets[7] = {19924, 20012, 20100, 20188, 20276, 20364, 20452};

__declspec(align(8)) int g_TechnologyPanelRowFlagOffsets[7] = {5444, 5548, 5652, 5756, 5860, 5964, 6068};

__declspec(align(4)) int g_TechnologyPanelRowValueOffsets[7] = {6164, 6256, 6348, 6440, 6532, 6624, 6716};

__declspec(align(16)) int g_InGameSelectionDetailGridCellOffsets[12] = {41464, 41560, 41656, 41752, 41848, 41944, 42040, 42136, 42232, 42328, 42424, 42520};

__declspec(align(16)) int32_t g_UiSevenSlotSelectionControlOffsets[7] = {8420, 8516, 8612, 8708, 8804, 8900, 8996};

/* uint32_t[11]: sprite subresource index (0xA9..0xAB) of the diplomacy row's relation icon per relation state; ui/ingame/runtime.c */
__declspec(align(4)) uint32_t g_UiAction1012SubresourceByState[11] = {0xA9, 0xA9, 0xA9, 0xA9, 0xAA, 0xAA, 0xAA, 0xA9, 0xAB, 0xAB, 0xAB};

__declspec(align(16)) uint16_t u_gfx_panel_panel0_gfx_005630d0[21] = L"gfx\\panel\\panel0.gfx";

__declspec(align(4)) uint16_t u_gfx_panel_tech_gfx_005630fa[19] = L"gfx\\panel\\tech.gfx";

__declspec(align(16)) uint16_t u_gfx_panel_diagram0_gfx_00563120[23] = L"gfx\\panel\\diagram0.gfx";

__declspec(align(4)) uint16_t u_gfx_panel_window_gfx_0056318e[21] = L"gfx\\panel\\window.gfx";

__declspec(align(4)) uint16_t g_DeveloperChatPhraseUtf16[32] = L"Oh grosser Thomas, erl\366se mich!";

__declspec(align(4)) uint16_t u_Hmmm__na_gut________0056321e[20] = L"Hmmm, na gut... ;-)";

__declspec(align(16)) uint32_t g_UiCommandRuntimeFlags = 0;

__declspec(align(16)) int32_t g_InGamePanelTextureSubresource00Width = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource01Width = 0;

__declspec(align(8)) int32_t g_InGamePanelTextureSubresource02Width = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource06Width = 0;

__declspec(align(16)) int32_t g_InGamePanelTextureSubresource07Width = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource27Width = 0;

__declspec(align(8)) int32_t g_InGamePanelTextureSubresource28Width = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource19Width = 0;

__declspec(align(16)) int32_t g_InGamePanelTextureSubresource20Width = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource34Width = 0;

__declspec(align(8)) int32_t g_InGamePanelTextureSubresource32Width = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource33Width = 0;

__declspec(align(16)) int32_t g_InGamePanelTextureSubresource02Height = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource03Height = 0;

__declspec(align(8)) int32_t g_InGamePanelTextureSubresource04Height = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource05Height = 0;

__declspec(align(16)) int32_t g_InGamePanelTextureSubresource36Height = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource37Height = 0;

__declspec(align(8)) int32_t g_InGamePanelTextureSubresource06Height = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource00Height = 0;

__declspec(align(16)) int32_t g_InGamePanelTextureSubresource07Height = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource26Height = 0;

__declspec(align(8)) int32_t g_InGamePanelTextureSubresource31Height = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource18Height = 0;

__declspec(align(16)) int32_t g_InGamePanelTextureSubresource23Height = 0;

__declspec(align(4)) int32_t g_InGamePanelTextureSubresource34Height = 0;

__declspec(align(8)) int32_t g_InGamePanelTextureSubresource32Height = 0;

__declspec(align(4)) UiCommandRuntimeRecordPrefix *g_UiHoverSelectionRecord = 0;

__declspec(align(16)) PckTechnologyIdCatalog g_InGameSelectedTechnologyId = 0;

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot10Utf16[16] = {0};

__declspec(align(4)) uint16_t g_FrontendDebugOverlayTextSlot11Utf16[16] = {0};

__declspec(align(4)) uint32_t g_InGameReadyStateToggleFlags = 0;

__declspec(align(8)) UiCommandRuntimeRecordPrefix *g_UiCatalogGroup48Records[48] = {0};

__declspec(align(8)) UiCommandRuntimeRecordPrefix *g_UiCatalogGroup42Records[42] = {0};

__declspec(align(16)) UiCommandRuntimeRecordPrefix *g_UiCommandSpriteVariantARecords[24] = {0};

__declspec(align(16)) uint32_t g_UiAction1012TargetPlayerIndices[7] = {0};

/* 63 key command records and the terminator record (commandCode 0) at
   00567CA4 that ends the dispatcher's scan */
__declspec(align(16)) UiCommandDispatchRecord g_InGameCommandDispatchRecords[64] = {
    /*  0 */ {.commandCode = 0x30073, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567F60},
    /*  1 */ {.commandCode = 0x30073, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567FC0},
    /*  2 */ {.commandCode = 0x30073, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x568020},
    /*  3 */ {.commandCode = 0x30073, .continuationEntryAddress = 0x567F60},
    /*  4 */ {.commandCode = 0x30062, .continuationEntryAddress = 0x567ED0},
    /*  5 */ {.commandCode = 0x30061, .continuationEntryAddress = 0x5680F0},
    /*  6 */ {.commandCode = 0x30031, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /*  7 */ {.commandCode = 0x30032, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /*  8 */ {.commandCode = 0x30033, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /*  9 */ {.commandCode = 0x30034, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /* 10 */ {.commandCode = 0x30035, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /* 11 */ {.commandCode = 0x30036, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /* 12 */ {.commandCode = 0x30037, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /* 13 */ {.commandCode = 0x30038, .modifierClassFlags = 0x33, .continuationEntryAddress = 0x567DB0},
    /* 14 */ {.commandCode = 0x30031, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 15 */ {.commandCode = 0x30032, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 16 */ {.commandCode = 0x30033, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 17 */ {.commandCode = 0x30034, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 18 */ {.commandCode = 0x30035, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 19 */ {.commandCode = 0x30036, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 20 */ {.commandCode = 0x30037, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 21 */ {.commandCode = 0x30038, .modifierClassFlags = 0xF, .continuationEntryAddress = 0x567DB0},
    /* 22 */ {.commandCode = 0x30031, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 23 */ {.commandCode = 0x30032, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 24 */ {.commandCode = 0x30033, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 25 */ {.commandCode = 0x30034, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 26 */ {.commandCode = 0x30035, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 27 */ {.commandCode = 0x30036, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 28 */ {.commandCode = 0x30037, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 29 */ {.commandCode = 0x30038, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567D60},
    /* 30 */ {.commandCode = 0x30031, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 31 */ {.commandCode = 0x30032, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 32 */ {.commandCode = 0x30033, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 33 */ {.commandCode = 0x30034, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 34 */ {.commandCode = 0x30035, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 35 */ {.commandCode = 0x30036, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 36 */ {.commandCode = 0x30037, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 37 */ {.commandCode = 0x30038, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x567D60},
    /* 38 */ {.commandCode = 0x30031, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 39 */ {.commandCode = 0x30032, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 40 */ {.commandCode = 0x30033, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 41 */ {.commandCode = 0x30034, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 42 */ {.commandCode = 0x30035, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 43 */ {.commandCode = 0x30036, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 44 */ {.commandCode = 0x30037, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 45 */ {.commandCode = 0x30038, .modifierClassFlags = 0x3, .continuationEntryAddress = 0x567D10},
    /* 46 */ {.commandCode = 0x30031, .continuationEntryAddress = 0x567CC0},
    /* 47 */ {.commandCode = 0x30032, .continuationEntryAddress = 0x567CC0},
    /* 48 */ {.commandCode = 0x30033, .continuationEntryAddress = 0x567CC0},
    /* 49 */ {.commandCode = 0x30034, .continuationEntryAddress = 0x567CC0},
    /* 50 */ {.commandCode = 0x30035, .continuationEntryAddress = 0x567CC0},
    /* 51 */ {.commandCode = 0x30036, .continuationEntryAddress = 0x567CC0},
    /* 52 */ {.commandCode = 0x30037, .continuationEntryAddress = 0x567CC0},
    /* 53 */ {.commandCode = 0x30038, .continuationEntryAddress = 0x567CC0},
    /* 54 */ {.commandCode = 0x20, .continuationEntryAddress = 0x567E00},
    /* 55 */ {.commandCode = 0x20, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567E40},
    /* 56 */ {.commandCode = 0x10003, .continuationEntryAddress = 0x567E20},
    /* 57 */ {.commandCode = 0x30066, .continuationEntryAddress = 0x568080},
    /* 58 */ {.commandCode = 0x3006F, .continuationEntryAddress = 0x5681A0},
    /* 59 */ {.commandCode = 0x30076, .modifierClassFlags = 0x3C, .continuationEntryAddress = 0x5681B0},
    /* 60 */ {.commandCode = 0x30063, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x568190},
    /* 61 */ {.commandCode = 0x10015, .continuationEntryAddress = 0x567EA0},
    /* 62 */ {.commandCode = 0x30064, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x568130},
    /* 63 */ {.commandCode = 0x0, .modifierClassFlags = 0x90909090, .continuationEntryAddress = 0x90909090}}; /* commandCode 0, the rest is the original's NOP fill */

__declspec(align(16)) uint32_t g_UiCommandModeG = 0;

__declspec(align(4)) uint32_t g_UiCommandModeC = 0;

__declspec(align(8)) uint32_t g_UiCommandModeD = 0;

__declspec(align(4)) uint32_t g_UiCommandModeE = 0;

__declspec(align(16)) uint32_t g_UiCommandModeA = 0;

__declspec(align(4)) uint32_t g_UiCommandModeB = 0;

__declspec(align(4)) uint32_t g_UiCommandModeF = 0;

__declspec(align(16)) uint32_t g_UiCommandAbsoluteSelectionIndex = 0;

__declspec(align(4)) uint32_t g_UiCommandSelectionPageBaseIndex = 0;

__declspec(align(8)) uint32_t g_UiCommandTerrainMaskToggleValue = 0;

__declspec(align(4)) FactionRuntimeIndex g_UiCommandModeGOwnerFactionIndex = 1;

__declspec(align(16)) uint32_t g_UiCommandModeGArmyAssetId = 0;

__declspec(align(4)) int32_t g_UiCommandDragReferenceX = 0;

__declspec(align(8)) int32_t g_UiCommandDragReferenceY = 0;

__declspec(align(4)) PckArmyAssetIdCatalog g_UiCommandMode4ArmyAssetId = ARM_0500_LBAUM_MDL0500;

__declspec(align(16)) uint32_t g_UiCommandCallerMaskHighBit = 0;

/* uint32_t[6]: active page of the mode preview page stack per command mode G; ui/ingame commands/runtime */
__declspec(align(4)) uint32_t g_UiCommandModeGPrimaryPageIndices[6] = {1, 2, 3, 4, 5, 7};

/* uint32_t[6]: active page of the mode detail page stack per command mode G; ui/ingame commands/runtime */
__declspec(align(4)) uint32_t g_UiCommandModeGSecondaryPageIndices[6] = {1, 2, 3, 4, 5, 7};

/* uint32_t[6]: active page of the mode command page stack per command mode G; ui/ingame commands/runtime */
__declspec(align(4)) uint32_t g_UiCommandModeGTertiaryPageIndices[6] = {1, 2, 3, 4, 5, 7};

__declspec(align(4)) int32_t g_UiCommandModeGControlOffsets[6] = {19364, 19484, 19604, 39216, 39336, 39096};

__declspec(align(4)) void *g_UiCommandModeGHandlers[6] = {
    /* 0 */ (void *)InGameCommandModeG_Select0,
    /* 1 */ (void *)InGameCommandModeG_Select1,
    /* 2 */ (void *)InGameCommandModeG_Select2,
    /* 3 */ (void *)InGameCommandModeG_Select3,
    /* 4 */ (void *)InGameCommandModeG_Select4,
    /* 5 */ (void *)InGameCommandModeG_Select5};

__declspec(align(4)) int32_t g_UiMappedCommandControlOffsets[12] = {42892, 43076, 43260, 43444, 43628, 43812, 43996, 44180, 44364, 44548, 44732, 44916};

__declspec(align(4)) int32_t g_UiCommandSelectionAnchorWorldXQ12 = 0;

__declspec(align(16)) int32_t g_UiCommandSelectionAnchorWorldYQ12 = 0;

__declspec(align(4)) int32_t g_UiCommandSelectionCurrentWorldXQ12 = 0;

__declspec(align(8)) int32_t g_UiCommandSelectionCurrentWorldYQ12 = 0;

__declspec(align(4)) uint32_t g_UiCommandDragAnchorWorldXQ12 = 0;

__declspec(align(16)) uint32_t g_UiCommandDragAnchorWorldYQ12 = 0;

__declspec(align(4)) uint32_t g_UiCommandDragStartScreenX = 0;

__declspec(align(8)) uint32_t g_UiCommandDragStartScreenY = 0;

__declspec(align(8)) DirectSoundVoiceSet *g_UiButtonSoundVoiceSets7[7] = {0};
