#include <thandor/graphics/render/projection.h>

/* Implementation ownership: graphics/render/projection. */

/* Address: 0x00486B00.
   Ownership: graphics/render/projection.
   Purpose: Handles graphics offscreen render model list to texture source carry-flag result.
   Cross-module calls: Graphics_SetProjectionClipRect [graphics/core/runtime], Graphics_SetViewProjectionParameters
   [graphics/core/runtime], Graphics_SetProjectionViewport [graphics/core/runtime],
   Graphics_SetAuxiliaryOrientation [graphics/core/runtime], Graphics_SetSceneBounds [graphics/core/runtime],
   Graphics_RebuildFrustumPlanes [graphics/core/runtime].
*/
undefined8 __fastcall
GraphicsOffscreen_RenderModelListToTextureSourceCf
          (undefined4 param_1,undefined4 param_2,GraphicsOffscreenSceneExtents *sceneExtents,
          undefined4 param_4,GraphicsOffscreenViewParameters *viewParameters,
          GraphicsPixelDimension outputWidth,GraphicsPixelDimension outputHeight,
          ModelRuntimeCount modelCount,ModelRuntimeNode **modelNodes)

{
  sdword *psVar1;
  sdword sVar2;
  sdword *memory;
  dword dVar3;
  sdword *psVar4;
  GraphicsPrimitiveQueue *queue;
  GraphicsPrimitiveQueue *queue_00;
  int iVar5;
  uint extraout_ECX;
  uint uVar6;
  AngleTurn32 *extraout_ECX_00;
  ModelRuntimeCount extraout_ECX_01;
  uint extraout_EDX;
  sdword *psVar7;
  bool bVar8;
  undefined1 uVar9;
  
  iVar5 = (int)((longlong)(int)outputHeight * (longlong)(int)outputWidth);
  bVar8 = (longlong)iVar5 != (longlong)(int)outputHeight * (longlong)(int)outputWidth;
  memory = (*g_MemoryApi.alloc)(iVar5 * 4 + 0x220);
  if (!bVar8) {
    psVar4 = memory;
    for (uVar6 = extraout_ECX >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
      *psVar4 = 0;
      psVar4 = psVar4 + 1;
    }
    *memory = 0x786667;
    memory[1] = extraout_ECX;
    memory[2] = 1;
    memory[3] = 0;
    dVar3 = (*g_LocaleGetPackedCurrentTime)();
    memory[4] = dVar3;
    memory[6] = dVar3;
    memory[8] = dVar3;
    dVar3 = (*g_LocaleGetPackedCurrentDate)();
    memory[5] = dVar3;
    memory[7] = dVar3;
    memory[9] = dVar3;
    (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(memory + 0xc));
    (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(memory + 0x1c));
    *(undefined1 *)(memory + 0x40) = 0;
    memory[0x2c] = 1;
    memory[0x2d] = 0;
    memory[0x2e] = 0x200;
    memory[0x80] = outputHeight;
    memory[0x81] = outputWidth;
    memory[0x86] = outputHeight;
    memory[0x87] = outputWidth;
    memory[0x84] = 0;
    memory[0x85] = 0;
    memory[0x82] = -1;
    memory[0x83] = 0x220;
    bVar8 = (int)(outputWidth * outputHeight * 2) < 0;
    psVar4 = (*g_MemoryApi.alloc)(outputWidth * outputHeight * 4);
    if (!bVar8) {
      uVar9 = 0;
      psVar7 = psVar4;
      for (uVar6 = extraout_EDX >> 2; sVar2 = g_SoftwareDepthEpoch, psVar1 = g_SoftwareDepthBuffer,
          uVar6 != 0; uVar6 = uVar6 - 1) {
        *psVar7 = -1;
        psVar7 = psVar7 + 1;
      }
      LOCK();
      g_SoftwareDepthEpoch = -1;
      UNLOCK();
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = psVar4;
      Graphics_SetProjectionClipRect(outputWidth,outputHeight,0,0);
      Graphics_SetViewProjectionParameters
                (viewParameters->projectionShift,viewParameters->viewAngle1,
                 viewParameters->viewAngle0,viewParameters->projectionScale,viewParameters->originZ,
                 viewParameters->originY,viewParameters->originX);
      Graphics_SetProjectionViewport(outputWidth,outputHeight,0,0);
      Graphics_SetAuxiliaryOrientation(extraout_ECX_00[1],*extraout_ECX_00);
      Graphics_SetSceneBounds
                (sceneExtents->verticalExtent,sceneExtents->horizontalExtent,
                 sceneExtents->verticalExtent,sceneExtents->horizontalExtent,0,0,0,0);
      Graphics_RebuildFrustumPlanes();
      g_GraphicsShadingCompactRecordCount = 0;
      queue = GraphicsPrimitiveQueue_ResetGlobal();
      if (!(bool)uVar9) {
        Graphics_SetActivePrimitiveQueue(queue);
        for (; modelCount != 0; modelCount = modelCount - 1) {
          if (*modelNodes != (ModelRuntimeNode *)0x0) {
            ModelRuntime_CullAndRenderHierarchyRecursive(*modelNodes);
            modelCount = extraout_ECX_01;
          }
          modelNodes = modelNodes + 1;
        }
        GraphicsPrimitiveQueue_RadixSortForRendering(GRAPHICS_STATE_DISABLED,queue_00);
        SoftwareRenderer_DrawQueueAuxiliary(outputWidth,outputHeight,memory + 0x88,queue_00);
      }
      psVar4 = g_SoftwareDepthBuffer;
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = psVar1;
      g_SoftwareDepthEpoch = sVar2;
      (*g_MemoryApi.free)(psVar4);
      return CONCAT44(param_2,memory);
    }
    (*g_MemoryApi.free)(memory);
    memory = psVar4;
  }
  return CONCAT44(param_2,memory);
}

/* Address: 0x0050A6A0.
   Ownership: graphics/render/projection.
   Purpose: CF set means inside; scalar registers are preserved.
*/
void GraphicsProjectedPoint_IsInsideTriangleCf
               (int pointerY,int pointerX,GraphicsProjectedPoint2i *vertex0,
               GraphicsProjectedPoint2i *vertex1,GraphicsProjectedPoint2i *vertex2)

{
  longlong lVar1;
  longlong lVar2;
  GraphicsProjectedPoint2i *pGVar3;
  
  lVar2 = (longlong)(vertex0->y - vertex1->y) * (longlong)vertex2->x +
          (longlong)(vertex2->y - vertex0->y) * (longlong)vertex1->x;
  lVar1 = (longlong)(vertex1->y - vertex2->y) * (longlong)vertex0->x;
  pGVar3 = vertex2;
  if (-1 < (int)((int)((ulonglong)lVar1 >> 0x20) + (int)((ulonglong)lVar2 >> 0x20) +
                (uint)CARRY4((uint)lVar1,(uint)lVar2))) {
    pGVar3 = vertex1;
    vertex1 = vertex2;
  }
  lVar2 = (longlong)(pointerY - vertex1->y) * (longlong)pGVar3->x +
          (longlong)(pGVar3->y - pointerY) * (longlong)vertex1->x;
  lVar1 = (longlong)(vertex1->y - pGVar3->y) * (longlong)pointerX;
  if ((int)((int)((ulonglong)lVar1 >> 0x20) + (int)((ulonglong)lVar2 >> 0x20) +
           (uint)CARRY4((uint)lVar1,(uint)lVar2)) < 0) {
    lVar2 = (longlong)(vertex0->y - pointerY) * (longlong)pGVar3->x +
            (longlong)(pGVar3->y - vertex0->y) * (longlong)pointerX;
    lVar1 = (longlong)(pointerY - pGVar3->y) * (longlong)vertex0->x;
    if ((int)((int)((ulonglong)lVar1 >> 0x20) + (int)((ulonglong)lVar2 >> 0x20) +
             (uint)CARRY4((uint)lVar1,(uint)lVar2)) < 0) {
      lVar2 = (longlong)(vertex0->y - vertex1->y) * (longlong)pointerX +
              (longlong)(pointerY - vertex0->y) * (longlong)vertex1->x;
      lVar1 = (longlong)(vertex1->y - pointerY) * (longlong)vertex0->x;
      if ((int)((int)((ulonglong)lVar1 >> 0x20) + (int)((ulonglong)lVar2 >> 0x20) +
               (uint)CARRY4((uint)lVar1,(uint)lVar2)) < 0) {
        return;
      }
    }
  }
  return;
}
