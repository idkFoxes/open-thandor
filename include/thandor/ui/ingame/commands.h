#ifndef THANDOR_UI_INGAME_COMMANDS_H
#define THANDOR_UI_INGAME_COMMANDS_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/commands. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056DB40 */
void InGameCommandModeG_Select0(UiSelectableControl *source);

/* 0x0056DB90 */
void InGameCommandModeG_Select1(UiSelectableControl *source);

/* 0x0056DBE0 */
void InGameCommandModeG_Select2(UiSelectableControl *source);

/* 0x0056DC30 */
void InGameCommandModeG_Select3(UiSelectableControl *source);

/* 0x0056DCA0 */
void InGameCommandModeG_Select4(UiSelectableControl *source);

/* 0x0056DCF0 */
void InGameCommandModeG_Select5(UiSelectableControl *source);

/* 0x0056AC50 */
undefined4 InGameCommandAction_SetFlag1000OrMarkReady(void *source);

/* 0x0056ACC0 */
undefined4 InGameCommandAction_ToggleRuntimeFlag0800(void *source);

/* 0x0056D6C0 */
void InGameCommandState_CloseSettingsAndDispatchOperation150(UiNodeBase *source);

/* 0x0056DFD0 */
void InGameCommandMatrix_SelectMappedControl(UiNodeBase *source);

/* 0x00516360 */
void UiCommandSpriteButtonControl_BeginPress (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiCommandSpriteButtonControl *control);

/* 0x005163A0 */
void UiCommandSpriteButtonControl_NonRightRelease (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiCommandSpriteButtonControl *control);

/* 0x00516410 */
void UiCommandSpriteButtonControl_RightRelease (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiCommandSpriteButtonControl *control);

/* 0x00516490 */
GraphicsCursorFrameIndex UiCommandSpriteVariantA_PointerMove (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX, UiCommandSpriteButtonControl *control);

/* 0x00517F60 */
void UiCommandVisibilityWrappedText_DrawWhenAllowed (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x00518010 */
void UiCommandVisibilitySingleLineText_DrawWhenAllowed (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x0055F4A0 */
undefined8 __fastcall InGameCommandMode_TogglePlayerFlagBit0AndReconcileGlobal (undefined4 param_1,undefined4 param_2,int param_3);

/* 0x005604D0 */
void InGameCommand_ExecuteLocalPlacementFromSelection (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04, CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

/* 0x0056A2A0 */
void UiCommandSpriteVariantA_RebuildGrid(UiNodeBase *node);

/* 0x0056AFD0 */
undefined8 InGameCommandAction_ClearSelectedArmyTokenAndClosePage(UiNodeBase *control);

/* 0x0056C660 */
undefined4 InGameCommandPanel_OpenPage4AndRefreshAvailability(InGameCommandPanelSourceAddress32 source);

/* 0x0056CFA0 */
void InGameCommandCatalog_SubmitGroup48Entry(UiCatalogEntryControl *source);

/* 0x0056D090 */
void InGameCommandCatalog_SubmitGroup42Entry(UiCatalogEntryControl *source);

/* 0x0056D180 */
void InGameCommandSprite_DispatchVariantAControl24(UiCommandSpriteButtonControl *control);

/* 0x0056D540 */
void InGameCommandSprite_DispatchFixedControl8(UiCommandSpriteButtonControl *control);

/* 0x0056D620 */
void InGameCommandState_SetRuntimeFlag1000(UiNodeBase *source);

/* 0x0056D640 */
void InGameCommandState_SelectAndPropagateBinaryMode(UiSelectableControl *source);

/* 0x0056D920 */
void UiCommandModeG_ClearNodeFlag00800000(UiNodeBase *node);

/* 0x0056DD50 */
void InGameCommandModeC_Select0(UiSpriteButtonControl *source);

/* 0x0056DDA0 */
void InGameCommandModeC_Select1(UiSpriteButtonControl *source);

/* 0x0056DDF0 */
void InGameCommandModeC_Select2(UiSpriteButtonControl *source);

/* 0x0056DE40 */
void InGameCommandModeC_Select3(UiSpriteButtonControl *source);

/* 0x0056DE90 */
void InGameCommandModeD_Select0(UiSpriteButtonControl *source);

/* 0x0056DEE0 */
void InGameCommandModeD_Select1(UiSpriteButtonControl *source);

/* 0x0056DF30 */
void InGameCommandModeD_Select2(UiSpriteButtonControl *source);

/* 0x0056DF80 */
void InGameCommandModeD_Select3(UiSpriteButtonControl *source);

/* 0x0056E050 */
void InGameCommandModeA_Select0(UiSpriteButtonControl *source);

/* 0x0056E090 */
void InGameCommandModeA_Select1(UiSpriteButtonControl *source);

/* 0x0056E0D0 */
void InGameCommandModeA_Select2(UiSpriteButtonControl *source);

/* 0x0056E110 */
void InGameCommandModeB_Select0(UiSpriteButtonControl *source);

/* 0x0056E150 */
void InGameCommandModeB_Select1(UiSpriteButtonControl *source);

/* 0x0056E190 */
void InGameCommandModeB_Select2(UiSpriteButtonControl *source);

/* 0x0056E1D0 */
void InGameCommandModeE_Select0(UiSpriteButtonControl *source);

/* 0x0056E220 */
void InGameCommandModeE_Select1(UiSpriteButtonControl *source);

/* 0x0056E260 */
void InGameCommandModeE_Select2(UiSpriteButtonControl *source);

/* 0x0056E2A0 */
void InGameCommandRange_DispatchState0(UiNodeBase *source);

/* 0x0056E2E0 */
void InGameCommandRange_DispatchState1(UiNodeBase *source);

/* 0x0056E320 */
void InGameCommandModeF_Select0(UiSpriteButtonControl *source);

/* 0x0056E370 */
void InGameCommandModeF_Select1(UiSpriteButtonControl *source);

/* 0x00570F20 */
void UiCommandRuntime_CallbackNoOp(void);

/* 0x0055F280 */
void InGameCommand150_HandlePlayerDepartureAndOwnership (PlayerOrFactionRuntimeId32 playerOrFactionId,dword value1,dword value2, GameEntityCommandFlags flags);

/* 0x0056D980 */
void UiCommandModeG_ApplyMaskedColorVariant(void *visualState);

/* 0x0056DA50 */
void UiCommandModeG_SetNodeFlag02000000(UiNodeBase *node);

/* 0x00571440 */
void UiCommandMatrix_SelectIndex(UiCommandModeIndex absoluteIndex,UiNodeBase *root);

/* 0x0055F440 */
void UiCommandRuntimeFlags_ApplyClearSetToggleMasks (undefined4 param_1,UiCommandRuntimeFlagMask toggleMask, UiCommandRuntimeFlagMask setMask,UiCommandRuntimeFlagMask clearMask);

/* 0x0056D860 */
void UiCommandModeG_ClearNodeFlag00100000(UiNodeBase *node);

/* 0x0056D880 */
void UiCommandModeG_SetNodeFlag00200000(UiNodeBase *node);

/* 0x0056D940 */
void UiCommandModeG_SetNodeFlag01000000(UiNodeBase *node);

/* 0x0056D8C0 */
void UiCommandModeG_SetNodeFlag00000400(UiNodeBase *node);

/* 0x0056D8E0 */
void UiCommandModeG_ClearNodeFlags00000480(UiNodeBase *node);

/* 0x0056D840 */
void UiCommandModeG_SetNodeFlag00100000(UiNodeBase *node);

/* 0x0056D8A0 */
void UiCommandModeG_ClearNodeFlag00200000(UiNodeBase *node);

/* 0x0056D960 */
void UiCommandModeG_ClearNodeFlag01000000(UiNodeBase *node);

/* 0x0056D9F0 */
void UiCommandModeG_ApplyRawColorVariant(void *visualState);

/* 0x0056DA70 */
void UiCommandModeG_ClearNodeFlag02000000(UiNodeBase *node);

/* 0x0056D900 */
void UiCommandModeG_SetNodeFlag00800000(UiNodeBase *node);

/* 0x0056DA90 */
void UiCommandModeG_SelectAndSyncPages(UiCommandModeIndex modeIndex,UiSelectableControl *source);

#endif /* THANDOR_UI_INGAME_COMMANDS_H */
