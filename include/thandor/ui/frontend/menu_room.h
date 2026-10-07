/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/menu_room.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_MENU_ROOM_H
#define THANDOR_UI_FRONTEND_MENU_ROOM_H

#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

GraphicsCursorFrameIndex FrontendModelPointerContext_SelectBestModelHitTargetAndResolveAction (int pointerY,int pointerX,FrontendModelPointerHitContext *context);

void FrontendModelPointerContext_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext);

void FrontendModelPointerContext_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerHitContext *callbackContext);

void FrontendModelPointerContext_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext);

void FrontendModelPointerContext_Relocate
               (UiSerializedRelocationDelta relocationDelta,
               FrontendModelPointerContext *control);

void FrontendModelPointerContext_Layout(WorldRuntimeContext *callbackContext);

void FrontendModelPointerContext_RenderWorldViewQueuesClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FrontendModelPointerContext *control);

void FrontendModelPointerContext_RightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext);

void FrontendModelPointerContext_RightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext);

void FrontendModelPointerContext_DispatchWorldCameraPointerInput
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext);

void FrontendModelPointerContext_PointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext);

bool FrontendModelPointerContext_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          FrontendModelPointerHitContext *control);

void FrontendModelPointerContext_Tick(WorldRuntimeContext *callbackContext);

uint32_t FrontendRuntime_UpdatePointerContextAndSceneView
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,uint32_t hitMetric,
          void *pointedModelNode,FrontendPointerSceneRuntimeView *frontendRuntime);

void FrontendMenuRoom_PressNoOp
               (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,
               uint32_t hitMetric,uint32_t pointedModelNode,uint32_t pointerContext);

void FrontendMenuRoom_DragNoOp
               (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,
               uint32_t hitMetric,uint32_t pointedModelNode,uint32_t pointerContext);

void FrontendMenuRoom_ExecuteClickedRomAction
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,uint32_t hitMetric,
          FrontendCallbackArgument5 pointedModelNode,uint32_t pointerContext);

void FrontendMenuRoom_StopCameraFlight(uint32_t pointerContext);

uint64_t FrontendModelPointerContext_FindBestEligibleModelHitTarget (int pointerY,int pointerX,FrontendModelPointerHitContext *context);

extern UiNodeVtable g_FrontendModelPointerContextVtable;

extern GraphicsTextureSourceBlitProc *g_SelectionPanelBlitOpaque;
extern GraphicsTextureSourceTiledBlitProc *g_SelectionPanelBlitClipped;

#endif /* THANDOR_UI_FRONTEND_MENU_ROOM_H */
