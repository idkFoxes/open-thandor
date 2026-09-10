#ifndef THANDOR_UI_CONTROLS_MISC_H
#define THANDOR_UI_CONTROLS_MISC_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/misc. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00422910 */
void UiDisplaySettingsRoot_RefreshModeSelection(UiRootNode *root);

/* 0x00423C40 */
void UiDisplayModeAction_UpdateAdapterSelection(UiNodeBase *sourceNode);

/* 0x00423C80 */
void UiDisplayModeAction_UpdateResolutionSelection(UiNodeBase *sourceNode);

/* 0x00423CB0 */
void UiDisplayModeAction_UpdateColorDepthSelection(UiNodeBase *sourceNode);

/* 0x00424590 */
void UiDisplayModeAction_ApplyFourValueDialogAndReopenSettings(UiNodeBase *sourceNode);

/* 0x004BC8B0 */
void UiImageControl_NonRightDrag (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiImageControl *control);

/* 0x004BCB50 */
void UiImageControl_TickHover(UiImageControl *control);

/* 0x00423B30 */
void UiDisplayModeAction_ApplyPendingMode(UiNodeBase *sourceNode);

/* 0x00423C00 */
void UiDisplayModeAction_CancelAndRebuildPixelPacking(UiNodeBase *sourceNode);

/* 0x004242D0 */
void UiFourValueDialog_TickCountdownAndRequestClose(UiRootNode *root);

/* 0x004B3F40 */
void UiRangeSliderControl_DrawTrackAndThumb (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B41C0 */
void UiRangeSliderControl_BeginThumbDrag (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B4280 */
void UiRangeSliderControl_EndThumbDrag(void);

/* 0x004B45F0 */
void UiRangeSliderControl_SuppressIfActionId(UiActionId actionId,UiNodeBase *control);

/* 0x004B4620 */
void UiRangeSliderControl_UnsuppressIfActionId(UiActionId actionId,UiNodeBase *control);

/* 0x004BC5C0 */
void UiImageControl_DrawClipped (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiImageControl *control);

/* 0x004BC6E0 */
void UiImageControl_NonRightPress (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiImageControl *control);

/* 0x004BC7E0 */
void UiImageControl_NonRightRelease (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiImageControl *control);

/* 0x004BD2A0 */
void UiModelControl_RefreshStateTint(ModelRuntimeNode *control);

/* 0x00517E30 */
void UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control);

/* 0x00423600 */
void __cdecl UiDisplaySettings_OpenAndPopulateModeSelectionCf(void);

/* 0x004BC9B0 */
UiNodeBase * UiImageControl_HitTestOpaque (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control);

/* 0x00423D70 */
void UiDisplayModeSelection_RefreshEnumeratedOptions (FrontendDisplayAdapterIndex adapterIndex,UiNodeBase *selectedModeValue, FrontendDisplayDimensionPixels modeHeight,FrontendDisplayDimensionPixels modeWidth, UiNodeBase *displaySettingsRoot);

#endif /* THANDOR_UI_CONTROLS_MISC_H */
