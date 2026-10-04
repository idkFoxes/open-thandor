/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/panels.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_PANELS_H
#define THANDOR_UI_CONTROLS_PANELS_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/panels. */

/* panelFlags of UiImagePanelControl (also the base of UiArmyMetricsPanel). */
#define UI_IMAGE_PANEL_CENTER_X 0x01
#define UI_IMAGE_PANEL_ALIGN_RIGHT 0x02
#define UI_IMAGE_PANEL_CENTER_Y 0x04
#define UI_IMAGE_PANEL_ALIGN_BOTTOM 0x08
#define UI_IMAGE_PANEL_DROP_SHADOW 0x10
#define UI_IMAGE_PANEL_NEVER_HIT 0x20
#define UI_IMAGE_PANEL_HIT_WHOLE_BOX 0x40 /* skip the opaque-pixel test */
#define UI_IMAGE_PANEL_STRETCH 0x80 /* stretch the texture over the layout box */

/* fillFlags of UiFillPanelControl. */
#define UI_FILL_PANEL_TILE_X 0x01
#define UI_FILL_PANEL_TILE_Y 0x02
#define UI_FILL_PANEL_DROP_SHADOW 0x10

/* gaugeFlags of UiFormattedContainer. */
#define UI_GAUGE_TWO_SIDED_SCALE 0x01
#define UI_GAUGE_HAS_MARKER 0x02 /* the node is a UiFormattedContainerWithMarker */
/* UiFormattedContainer_DrawClipped: frame offsets from firstFrameSubresource. Each bar look is three frames
   (left cap, tiled middle, right cap); the empty bar is look 0, the six fill colours start at 3, 6, ... 18. */
#define UI_GAUGE_FRAME_TRACK 1
#define UI_GAUGE_FRAME_END_CAP 2
#define UI_GAUGE_FRAME_MARKER 21

/* Functions are grouped by semantic ownership. */

void UiImagePanelControl_DrawAlignedTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiImagePanelControl *control);

UiNodeBase * UiImagePanelControl_HitTestAlignedTextureAndChildren(int pointerY,int pointerX,UiImagePanelControl *control);

void UiFillPanelControl_DrawColorOrTiledTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiFillPanelControl *control);

void UiNineSlicePanelControl_DrawTextureFrameAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNineSlicePanelControl *control);

void UiFormattedContainer_RelocateWithPatchedTextPayloads
          (UiSerializedRelocationDelta relocationDelta,UiFormattedContainer *control);

void UiFormattedContainer_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiFormattedContainer *control);

void UiArmyMetricsPanel_DrawTextureMetricsAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiArmyMetricsPanel *control);

void UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiSoftwareTexturePreviewControl *control);

void UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control);

void UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control);

Bool8 UiSoftwareTexturePreviewControl_HandleKeyboardActivation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSoftwareTexturePreviewControl *control);

extern UiNodeVtable g_UiImagePanelControlVtable;

extern UiNodeVtable g_UiNineSlicePanelControlVtable;

extern UiNodeVtable g_UiFormattedContainerVtable;
extern UiNodeVtable g_UiArmyMetricsPanelVtable;
extern UiNodeVtable g_UiSoftwareTexturePreviewControlVtable;

void UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPanelControl *control);

UiNodeBase * UiFillPanelControl_HitTestChildrenOnly
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

extern UiNodeVtable g_UiFillPanelControlVtable;
extern UiNodeVtable g_UiPanelControlVtable;

#endif /* THANDOR_UI_CONTROLS_PANELS_H */
