/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/misc.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_MISC_H
#define THANDOR_UI_CONTROLS_MISC_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/misc. */

/* Action ids of the display settings dialog (g_UiDisplaySettingsRootTemplate); action 0x200 + n runs
   g_UiDisplayModeSelectionActionHandlers20[n]. UiDisplayModeSelection_RefreshEnumeratedOptions suppresses
   the option buttons whose mode was not enumerated. */
#define UI_DISPLAY_MODE_ACTION_APPLY 0x200
#define UI_DISPLAY_MODE_ACTION_FIRST_COLOR_DEPTH 0x201 /* 4 colour-depth buttons */
#define UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION 0x205 /* 8 resolution buttons */
#define UI_DISPLAY_MODE_ACTION_REVERT 0x20D /* "keep the new mode?" dialog: back to the previous mode */
#define UI_DISPLAY_MODE_ACTION_CANCEL 0x20E
#define UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER 0x20F /* 5 adapter buttons */
/* Extra fields in the tail of the display settings dialog's applyButton
   (DISPLAY_SETTINGS_UI_FIELD(root,applyButton,offset,type)): the selected mode tuple and colour bias/scale,
   then the same six values as they were when the dialog opened. */
#define DISPLAY_SETTINGS_SELECTED_WIDTH 0x5C
#define DISPLAY_SETTINGS_SELECTED_HEIGHT 0x60
#define DISPLAY_SETTINGS_SELECTED_BITS_PER_PIXEL 0x64
#define DISPLAY_SETTINGS_SELECTED_ADAPTER 0x68
#define DISPLAY_SETTINGS_SELECTED_COLOR_BIAS 0x6C /* Q16 */
#define DISPLAY_SETTINGS_SELECTED_COLOR_SCALE 0x70 /* Q16 */
#define DISPLAY_SETTINGS_ORIGINAL_WIDTH 0x74
#define DISPLAY_SETTINGS_ORIGINAL_HEIGHT 0x78
#define DISPLAY_SETTINGS_ORIGINAL_BITS_PER_PIXEL 0x7C
#define DISPLAY_SETTINGS_ORIGINAL_ADAPTER 0x80
#define DISPLAY_SETTINGS_ORIGINAL_COLOR_BIAS 0x84
#define DISPLAY_SETTINGS_ORIGINAL_COLOR_SCALE 0x88
/* Extra fields in the tail of the "keep the new display mode?" dialog's countdownMessageText
   (FOUR_VALUE_DIALOG_UI_FIELD(root,countdownMessageText,offset,type)), see UiRuntime_OpenFourValueDialog. */
#define FOUR_VALUE_DIALOG_COUNTDOWN 0x5C /* the number shown */
#define FOUR_VALUE_DIALOG_STEP_TICKS 0x60 /* frame updates left until the number counts down */
#define FOUR_VALUE_DIALOG_PREVIOUS_WIDTH 0x64
#define FOUR_VALUE_DIALOG_PREVIOUS_HEIGHT 0x68
#define FOUR_VALUE_DIALOG_PREVIOUS_BITS_PER_PIXEL 0x6C
#define FOUR_VALUE_DIALOG_PREVIOUS_ADAPTER 0x70
#define FOUR_VALUE_DIALOG_NUMBER_BUFFER 0x74 /* UTF-16 digits of the countdown */
/* Frame updates per step of the countdown in the "keep the new display mode?" dialog
   (UiFourValueDialog_TickCountdownAndRequestClose). */
#define UI_DISPLAY_MODE_COUNTDOWN_STEP_TICKS 20
/* Pointer coordinate far outside every control, used by UiImageControl_NonRightDrag for the synthetic events
   it sends when the pointer moves from one child to another (the new child's press, the old child's drag and
   release), so that no pixel test of theirs hits. */
#define UI_POINTER_FAR_OUTSIDE 0x70000000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00422910 */
void __thandor_void_preserve_eax_ecx_edx
UiDisplaySettingsRoot_RefreshModeSelection(UiRootNode *root);

/* 0x00423C40 */
void __thandor_preserve_eax UiDisplayModeAction_UpdateAdapterSelection(UiNodeBase *sourceNode);

/* 0x00423C80 */
void __thandor_preserve_eax UiDisplayModeAction_UpdateResolutionSelection(UiNodeBase *sourceNode);

/* 0x00423CB0 */
void __thandor_preserve_eax UiDisplayModeAction_UpdateColorDepthSelection(UiNodeBase *sourceNode);

/* 0x00424590 */
void __thandor_void_preserve_eax_ecx_edx
UiDisplayModeAction_ApplyFourValueDialogAndReopenSettings(UiNodeBase *sourceNode);

/* 0x004BC8B0 */
void __thandor_preserve_eax_edx
UiImageControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control);

/* 0x004BCB50 */
void __thandor_void_preserve_eax_ecx UiImageControl_TickHover(UiImageControl *control);

/* 0x00423B30 */
void __thandor_void_preserve_eax_ecx_edx
UiDisplayModeAction_ApplyPendingMode(UiNodeBase *sourceNode);

/* 0x00423C00 */
void __thandor_void_preserve_eax_ecx_edx
UiDisplayModeAction_CancelAndRebuildPixelPacking(UiNodeBase *sourceNode);

/* 0x004242D0 */
void __thandor_void_preserve_eax_ecx_edx
UiFourValueDialog_TickCountdownAndRequestClose(UiRootNode *root);

/* 0x004B3F40 */
void __thandor_void_preserve_eax_ecx_edx
UiRangeSliderControl_DrawTrackAndThumb
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiRangeSliderControl *control);

/* 0x004B41C0 */
void __thandor_void_preserve_eax_ecx_edx
UiRangeSliderControl_BeginThumbDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control);

/* 0x004B4280 */
void UiRangeSliderControl_EndThumbDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiRangeSliderControl *control);

/* 0x004B45F0 */
void __thandor_void_preserve_eax_ecx_edx
UiRangeSliderControl_SuppressIfActionId(UiActionId actionId,UiRangeSliderControl *control);

/* 0x004B4620 */
void __thandor_void_preserve_eax_ecx_edx
UiRangeSliderControl_UnsuppressIfActionId(UiActionId actionId,UiRangeSliderControl *control);

/* 0x004BC5C0 */
void __thandor_void_preserve_eax_ecx_edx
UiImageControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiImageControl *control);

/* 0x004BC6E0 */
void __thandor_void_preserve_eax_ecx_edx
UiImageControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control);

/* 0x004BC7E0 */
void __thandor_void_preserve_eax_ecx_edx
UiImageControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control);

/* 0x004BD2A0 */
void __thandor_preserve_eax ModelNodeRuntime_RefreshStateTint(ModelRuntimeNode *modelNode);

/* 0x00517E30 */
void __thandor_void_preserve_eax_ecx_edx
UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiHorizontalGaugeControl *control);

/* 0x00423600 */
void __thandor_void_preserve_eax_ecx_edx UiDisplaySettings_OpenAndPopulateModeSelection(void);

/* 0x004BC9B0 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiImageControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control);

/* 0x00423D70 */
void __thandor_void_preserve_eax_ecx_edx
UiDisplayModeSelection_RefreshEnumeratedOptions
          (FrontendDisplayAdapterIndex adapterIndex,UiNodeBase *selectedModeValue,
          FrontendDisplayDimensionPixels modeHeight,FrontendDisplayDimensionPixels modeWidth,
          UiNodeBase *displaySettingsRoot);

#endif /* THANDOR_UI_CONTROLS_MISC_H */
