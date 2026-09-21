/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/projection.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/projection.h>

/* Implementation ownership: graphics/render/projection. */

/* Address: 0x00486940.
   Ownership: graphics/render/projection.
   Purpose: Tests whether a projected point lies outside the recovered triangle bounds contract.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsProjectedTriangle_PointOutsideBoundsCf
          (GraphicsProjectedCoordinate vertexAY,GraphicsProjectedCoordinate vertexAX,
          GraphicsProjectedCoordinate vertexBY,GraphicsProjectedCoordinate vertexBX,
          GraphicsProjectedCoordinate vertexCY,GraphicsProjectedCoordinate vertexCX,
          GraphicsProjectedCoordinate pointY,GraphicsProjectedCoordinate pointX)

{
  if ((((pointX <= vertexCX) || (pointX <= vertexBX)) || (pointX <= vertexAX)) &&
     (((vertexCX <= pointX || (vertexBX <= pointX)) || (vertexAX <= pointX)))) {
    if ((((pointY <= vertexCY) || (pointY <= vertexBY)) || (pointY <= vertexAY)) &&
       (((vertexCY <= pointY || (vertexBY <= pointY)) || (vertexAY <= pointY)))) {
      return false;
    }
  }
  return true;
}

/* Address: 0x00486B00.
   Ownership: graphics/render/projection.
   Purpose: Handles graphics offscreen render model list to texture source carry-flag result.
   Cross-module calls: Graphics_SetProjectionClipRect [graphics/core/runtime], Graphics_SetViewProjectionParameters
   [graphics/core/runtime], Graphics_SetProjectionViewport [graphics/core/runtime],
   Graphics_SetAuxiliaryOrientation [graphics/core/runtime], Graphics_SetSceneBounds [graphics/core/runtime],
   Graphics_RebuildFrustumPlanes [graphics/core/runtime].
*/
GraphicsOffscreenAllocationEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsOffscreen_RenderModelListToTextureSourceCf
          (GraphicsOffscreenSceneExtents *sceneExtents,AngleTurn32 *auxiliaryOrientationAngles,
          GraphicsOffscreenViewParameters *viewParameters,GraphicsPixelDimension outputWidth,
          GraphicsPixelDimension outputHeight,ModelRuntimeCount modelCount,
          ModelRuntimeNode **modelNodes)

{
  sdword *psVar1;
  sdword sVar2;
  sdword *psVar3;
  dword dVar4;
  sdword *psVar5;
  GraphicsPrimitiveQueue *queue;
  uint uVar6;
  uint uVar7;
  sdword *psVar8;
  ArenaAllocEaxCf5 AVar9;
  ArenaAllocEaxCf5 AVar10;
  GraphicsPrimitiveQueueEaxCf5 GVar11;
  GraphicsOffscreenAllocationEaxCf5 GVar12;
  
  uVar7 = outputHeight * outputWidth * 4 + 0x220;
  AVar9 = (*g_MemoryApi.alloc)(uVar7);
  psVar3 = (sdword *)AVar9.eax;
  if (!AVar9.carry) {
    psVar5 = psVar3;
    for (uVar6 = uVar7 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
      *psVar5 = 0;
      psVar5 = psVar5 + 1;
    }
    *psVar3 = 0x786667;
    psVar3[1] = uVar7;
    psVar3[2] = 1;
    psVar3[3] = 0;
    dVar4 = (*g_LocaleGetPackedCurrentTime)();
    psVar3[4] = dVar4;
    psVar3[6] = dVar4;
    psVar3[8] = dVar4;
    dVar4 = (*g_LocaleGetPackedCurrentDate)();
    psVar3[5] = dVar4;
    psVar3[7] = dVar4;
    psVar3[9] = dVar4;
    (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(psVar3 + 0xc));
    (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(psVar3 + 0x1c));
    *(undefined1 *)(psVar3 + 0x40) = 0;
    psVar3[0x2c] = 1;
    psVar3[0x2d] = 0;
    psVar3[0x2e] = 0x200;
    psVar3[0x80] = outputHeight;
    psVar3[0x81] = outputWidth;
    psVar3[0x86] = outputHeight;
    psVar3[0x87] = outputWidth;
    psVar3[0x84] = 0;
    psVar3[0x85] = 0;
    psVar3[0x82] = -1;
    psVar3[0x83] = 0x220;
    AVar10 = (*g_MemoryApi.alloc)(outputWidth * outputHeight * 4);
    psVar5 = (sdword *)AVar10.eax;
    if (!AVar10.carry) {
      psVar8 = psVar5;
      for (uVar7 = outputWidth * outputHeight & 0x3fffffff; sVar2 = g_SoftwareDepthEpoch,
          psVar1 = g_SoftwareDepthBuffer, uVar7 != 0; uVar7 = uVar7 - 1) {
        *psVar8 = -1;
        psVar8 = psVar8 + 1;
      }
      LOCK();
      g_SoftwareDepthEpoch = -1;
      UNLOCK();
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = psVar5;
      Graphics_SetProjectionClipRect(outputWidth,outputHeight,0,0);
      Graphics_SetViewProjectionParameters
                (viewParameters->projectionShift,viewParameters->viewAngle1,
                 viewParameters->viewAngle0,viewParameters->projectionScale,viewParameters->originZ,
                 viewParameters->originY,viewParameters->originX);
      Graphics_SetProjectionViewport(outputWidth,outputHeight,0,0);
      Graphics_SetAuxiliaryOrientation(auxiliaryOrientationAngles[1],*auxiliaryOrientationAngles);
      Graphics_SetSceneBounds
                (sceneExtents->verticalExtent,sceneExtents->horizontalExtent,
                 sceneExtents->verticalExtent,sceneExtents->horizontalExtent,0,0,0,0);
      Graphics_RebuildFrustumPlanes();
      g_GraphicsShadingCompactRecordCount = 0;
      GVar11 = GraphicsPrimitiveQueue_ResetGlobal();
      queue = GVar11.queue;
      if (!GVar11.carry) {
        Graphics_SetActivePrimitiveQueue(queue);
        for (; modelCount != 0; modelCount = modelCount - 1) {
          if (*modelNodes != (ModelRuntimeNode *)0x0) {
            ModelRuntime_CullAndRenderHierarchyRecursive(*modelNodes);
          }
          modelNodes = modelNodes + 1;
        }
        GraphicsPrimitiveQueue_RadixSortForRendering(GRAPHICS_STATE_DISABLED,queue);
        SoftwareRenderer_DrawQueueAuxiliary(outputWidth,outputHeight,psVar3 + 0x88,queue);
      }
      psVar3 = g_SoftwareDepthBuffer;
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = psVar1;
      g_SoftwareDepthEpoch = sVar2;
      (*g_MemoryApi.free)(psVar3);
      return (GraphicsOffscreenAllocationEaxCf5)((uint5)AVar9 & 0xffffffff);
    }
    (*g_MemoryApi.free)(psVar3);
    psVar3 = psVar5;
  }
  GVar12.carry = true;
  GVar12.allocation = psVar3;
  return GVar12;
}


/* Address: 0x0050A6A0.
   Ownership: graphics/render/projection.
   Purpose: CF set means inside; scalar registers are preserved.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsProjectedPoint_IsInsideTriangleCf
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
        return true;
      }
    }
  }
  return false;
}

