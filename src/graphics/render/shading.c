/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/shading.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/shading.h>

/* Implementation ownership: graphics/render/shading. */

/* Address: 0x004CDD40.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture process renderable hierarchy.
   Local calls: GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry,
   GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds,
   GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks,
   GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks,
   GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set, GraphicsShadingGeneratedTexture_FilterGridScratchMmx,
   GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear, GraphicsShadingGeneratedTexture_AdvanceTileCursor.
   Cross-module calls: FixedVec3_DotQ28 [core/math/fixed], FixedTransform_ApplyPoint [core/math/fixed],
   FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FieldGrid_InterpolateTopSurfaceHeight
   [world/terrain/grid], FieldGrid_RaycastTerrainTrianglesAlongDirection [world/terrain/grid],
   FieldGrid_RaycastTerrainSurfaceDistanceCf [world/terrain/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
          (ModelRuntimeNode *modelNode,GeneratedTextureRenderContextView *renderContext)

{
  GraphicsProjectedPointPair GVar1;
  GraphicsProjectedPointPair GVar2;
  GraphicsProjectedPointPair GVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  short sVar9;
  short sVar10;
  short sVar11;
  short sVar12;
  short sVar13;
  short sVar14;
  short sVar15;
  byte bVar16;
  ushort uVar17;
  GraphicsWorldCoordinateQ12 GVar18;
  sdword sVar19;
  int iVar20;
  dword dVar21;
  FixedMathScale32 FVar22;
  Q12 QVar23;
  GraphicsProjectedPointPair *pGVar24;
  uint uVar25;
  dword dVar26;
  int iVar27;
  dword dVar28;
  int iVar29;
  uint uVar30;
  uint uVar31;
  bool bVar32;
  undefined1 uVar33;
  undefined1 uVar34;
  undefined8 uVar35;
  undefined8 uVar36;
  undefined8 uVar37;
  undefined8 uVar38;
  GraphicsProjectedPointPair GVar39;
  FieldGridHeightEaxCf5 FVar40;
  GraphicsProjectedBlockEaxCf5 GVar41;
  FieldGridRaycastEaxEdxCf9 FVar42;
  FixedDirectionXyzRegs12 FVar43;
  
  if (g_GraphicsShadingGeneratedTextureCompletedTraversalCount == 0) {
    g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x =
         (modelNode->worldTransform).translation.x;
    g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y =
         (modelNode->worldTransform).translation.y;
    g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z =
         (modelNode->worldTransform).translation.z;
    iVar20 = modelNode->subtreeBoundingRadiusQ12;
    g_ModelCullViewRelativeX =
         g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x - g_ViewOriginFixed.x;
    g_ModelCullViewRelativeY =
         g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y - g_ViewOriginFixed.y;
    g_ModelCullViewRelativeZ =
         g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z - g_ViewOriginFixed.z;
    bVar32 = GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(modelNode);
    if (!bVar32) {
      uVar31 = (uint)(iVar20 * 9) >> 2;
      sVar19 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0,
                                (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
      if ((((sVar19 <= (int)uVar31) &&
           (sVar19 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 1,
                                      (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX),
           sVar19 <= (int)uVar31)) &&
          (sVar19 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 2,
                                     (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX),
          sVar19 <= (int)uVar31)) &&
         (sVar19 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 3,
                                    (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX),
         sVar19 <= (int)uVar31)) {
        FixedTransform_ApplyPoint
                  ((GraphicsFixedVec3 *)&g_ModelCullViewRelativeX,
                   &g_GeneratedTextureScratchRuntime.currentModelOriginQ12,
                   &g_ViewProjectionMatrixFixed);
        if (((int)g_ProjectionScaleFixed < g_ModelCullViewRelativeZ) &&
           (((modelNode->modelPayload).modelResource)->boundingRadiusQ12 <
            (int)(g_ModelCullViewRelativeZ - g_ProjectionScaleFixed))) {
          g_GeneratedTextureScratchRuntime.projectedMinX = 0x7fffffff;
          g_GeneratedTextureScratchRuntime.projectedMinY = 0x7fffffff;
          g_GeneratedTextureScratchRuntime.projectedMaxX = -0x7fffffff;
          g_GeneratedTextureScratchRuntime.projectedMaxY = -0x7fffffff;
          GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(modelNode);
          FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                             (0,renderContext->viewAngleB8 + 0x4000 & 0xffff,
                              g_GeneratedTextureScratchRuntime.projectedMinX +
                              g_GeneratedTextureScratchRuntime.projectedMaxX >> 1);
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x - FVar43.eax;
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y - FVar43.ecx;
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z - FVar43.edx;
          FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                             (renderContext->viewAngleBC + 0x4000,renderContext->viewAngleB8,
                              g_GeneratedTextureScratchRuntime.projectedMinY +
                              g_GeneratedTextureScratchRuntime.projectedMaxY >> 1);
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x - FVar43.eax;
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y - FVar43.ecx;
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z - FVar43.edx;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          iVar20 = (int)(CONCAT44((int)(g_GraphicsShadingGridHalfSize -
                                       g_GeneratedTextureScratchRuntime.downsampleBorderOffset) >> 8
                                  ,(g_GraphicsShadingGridHalfSize -
                                   g_GeneratedTextureScratchRuntime.downsampleBorderOffset) *
                                   0x1000000) /
                        (ulonglong)
                        (uint)(g_GeneratedTextureScratchRuntime.projectedMaxX -
                              g_GeneratedTextureScratchRuntime.projectedMinX));
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow0[0] =
               (int)((ulonglong)
                     ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[0] * (longlong)iVar20) >>
                    0x20) << 4 |
               (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[0] * (longlong)iVar20) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow0[1] =
               (int)((ulonglong)
                     ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[1] * (longlong)iVar20) >>
                    0x20) << 4 |
               (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[1] * (longlong)iVar20) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow0[2] =
               (int)((ulonglong)
                     ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[2] * (longlong)iVar20) >>
                    0x20) << 4 |
               (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[2] * (longlong)iVar20) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow2[0] = 0;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow2[1] = 0;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow2[2] = 0;
          iVar20 = (int)(CONCAT44((int)(g_GraphicsShadingGridHalfSize -
                                       g_GeneratedTextureScratchRuntime.downsampleBorderOffset) >> 8
                                  ,(g_GraphicsShadingGridHalfSize -
                                   g_GeneratedTextureScratchRuntime.downsampleBorderOffset) *
                                   0x1000000) /
                        (ulonglong)
                        (uint)(g_GeneratedTextureScratchRuntime.projectedMaxY -
                              g_GeneratedTextureScratchRuntime.projectedMinY));
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow1[0] =
               (int)((ulonglong)
                     ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[0] * (longlong)iVar20) >>
                    0x20) << 4 |
               (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[0] * (longlong)iVar20) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow1[1] =
               (int)((ulonglong)
                     ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[1] * (longlong)iVar20) >>
                    0x20) << 4 |
               (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[1] * (longlong)iVar20) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow1[2] =
               (int)((ulonglong)
                     ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[2] * (longlong)iVar20) >>
                    0x20) << 4 |
               (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[2] * (longlong)iVar20) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.translation.x = 0;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.translation.y = 0;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.translation.z = 0;
          FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                             (0,renderContext->viewAngleB8 + 0x4000 & 0xffff,
                              g_GeneratedTextureScratchRuntime.projectedMaxX -
                              g_GeneratedTextureScratchRuntime.projectedMinX >> 1);
          dVar28 = FVar43.edx;
          dVar26 = FVar43.ecx;
          dVar21 = FVar43.eax;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + dVar21;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + dVar26;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + dVar28;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x + dVar21;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y + dVar26;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + dVar28;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x - dVar21;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y - dVar26;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z - dVar28;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x - dVar21;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y - dVar26;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z - dVar28;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x + dVar21;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y + dVar26;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + dVar28;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x - dVar21;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y - dVar26;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z - dVar28;
          iVar20 = (int)dVar21 >> 1;
          iVar27 = (int)dVar26 >> 1;
          iVar29 = (int)dVar28 >> 1;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + iVar20;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + iVar27;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + iVar29;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x + iVar20;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y + iVar27;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + iVar29;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x - iVar20;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y - iVar27;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z - iVar29;
          g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x - iVar20;
          g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y - iVar27;
          g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z - iVar29;
          FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                             (renderContext->viewAngleBC + 0x4000,renderContext->viewAngleB8,
                              g_GeneratedTextureScratchRuntime.projectedMaxY -
                              g_GeneratedTextureScratchRuntime.projectedMinY >> 1);
          dVar28 = FVar43.edx;
          dVar26 = FVar43.ecx;
          dVar21 = FVar43.eax;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + dVar21;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + dVar26;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + dVar28;
          GVar18 = g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x - dVar21;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y - dVar26;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z - dVar28;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x + dVar21;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y + dVar26;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + dVar28;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x - dVar21;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y - dVar26;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z - dVar28;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x + dVar21;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y + dVar26;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + dVar28;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x - dVar21;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y - dVar26;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z - dVar28;
          iVar20 = (int)dVar21 >> 1;
          iVar27 = (int)dVar26 >> 1;
          iVar29 = (int)dVar28 >> 1;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + iVar20;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + iVar27;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + iVar29;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x - iVar20;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y - iVar27;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z - iVar29;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x + iVar20;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y + iVar27;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + iVar29;
          g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x - iVar20;
          g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y - iVar27;
          g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z - iVar29;
          g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 =
               g_GraphicsShadingGridStepQ20Current - 0x1000;
          g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 =
               (int)g_GraphicsShadingGridStepQ20Current >> 1;
          g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 = 0;
          g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 = 0;
          g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 = 0;
          g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 -
               ((int)g_GraphicsShadingGridStepQ20Current >> 2);
          g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 =
               ((int)g_GraphicsShadingGridStepQ20Current >> 2) +
               g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20;
          g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20;
          g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20;
          g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20;
          g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20;
          g_GeneratedTextureScratchRuntime.samples[0xb].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[0].terrainRayDistanceQ12 = FVar42.distanceQ12
              ;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[0].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[0].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[0].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + uVar31;
            }
          }
          GVar18 = g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[2].terrainRayDistanceQ12 = FVar42.distanceQ12
              ;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[2].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[2].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[2].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + uVar31;
            }
          }
          GVar18 = g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[1].terrainRayDistanceQ12 = FVar42.distanceQ12
              ;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[1].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[1].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[1].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + uVar31;
            }
          }
          GVar18 = g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[3].terrainRayDistanceQ12 = FVar42.distanceQ12
              ;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[3].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[3].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[3].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z + uVar31;
            }
          }
          GVar18 = g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[4].terrainRayDistanceQ12 = FVar42.distanceQ12
              ;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[4].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[4].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[4].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + uVar31;
            }
          }
          GVar18 = g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[5].terrainRayDistanceQ12 = FVar42.distanceQ12
              ;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[5].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[5].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[5].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z + uVar31;
            }
          }
          GVar18 = g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[6].terrainRayDistanceQ12 = FVar42.distanceQ12
              ;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[6].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[6].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[6].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + uVar31;
            }
          }
          GVar18 = g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[7].terrainRayDistanceQ12 = FVar42.distanceQ12
              ;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[7].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[7].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[7].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z + uVar31;
            }
          }
          GVar18 = g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[8].terrainRayDistanceQ12 = FVar42.distanceQ12
              ;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[8].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[8].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[8].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + uVar31;
            }
          }
          GVar18 = g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[10].terrainRayDistanceQ12 =
                   FVar42.distanceQ12;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[10].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[10].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[10].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + uVar31;
            }
          }
          GVar18 = g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[9].terrainRayDistanceQ12 = FVar42.distanceQ12
              ;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[9].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[9].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[9].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + uVar31;
            }
          }
          GVar18 = g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z;
          FVar40 = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x,
                              renderContext->fieldGrid);
          sVar19 = g_GeneratedTextureScratchRuntime.samples[0xb].textureCoordinateOffsetQ20;
          uVar31 = FVar40.heightQ12 - GVar18;
          if (FVar40.heightQ12 < GVar18) {
            FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x,
                                renderContext->fieldGrid);
            if (FVar42.carry) {
              g_GeneratedTextureScratchRuntime.samples[0xb].terrainRayDistanceQ12 =
                   FVar42.distanceQ12;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z + FVar43.edx;
            }
            else {
              iVar20 = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[0xb].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              QVar23 = (Q12)(((longlong)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[0xb].
                                   textureCoordinateOffsetQ20) * (longlong)iVar20) /
                            (longlong)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[0xb].terrainRayDistanceQ12 = iVar20;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,iVar20);
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z + FVar43.edx;
              uVar31 = renderContext->viewAngleB8 ^ 0x8000;
              FVar42 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-renderContext->viewAngleBC - 0x4000,uVar31,QVar23,
                                  g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x,
                                  renderContext->fieldGrid);
              FVar22 = FVar42.distanceQ12;
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (FVar42.materialOrCellIndex,uVar31,FVar22);
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z + FVar43.edx;
              g_GeneratedTextureScratchRuntime.samples[0xb].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[0xb].textureCoordinateOffsetQ20 +
                   ((int)(((longlong)FVar22 * (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                         (longlong)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[0xb].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint)modelNode->subtreeBoundingRadiusQ12 <= uVar31) {
              uVar31 = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[0xb].terrainRayDistanceQ12 = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar31 *
                                       (longlong)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar31 *
                                              (longlong)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (longlong)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[0xb].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[0xb].textureCoordinateOffsetQ20 - iVar20;
            if (sVar19 < iVar20) {
              g_GeneratedTextureScratchRuntime.samples[0xb].textureCoordinateOffsetQ20 = 0;
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x - FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y - FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z - FVar43.edx;
              FVar42 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!FVar42.carry) {
                return;
              }
              FVar43 = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  FVar42.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.x + FVar43.eax;
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.y + FVar43.ecx;
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z + FVar43.edx;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint.z + uVar31;
            }
          }
          GVar41 = GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks
                             (renderContext);
          pGVar24 = GVar41.firstBlock;
          if (!GVar41.carry) {
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 0x65),
                       &g_GeneratedTextureScratchRuntime.samples[0].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 0xb1),
                       &g_GeneratedTextureScratchRuntime.samples[1].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 0x71),
                       &g_GeneratedTextureScratchRuntime.samples[2].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 0xa9),
                       &g_GeneratedTextureScratchRuntime.samples[3].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 0x55),
                       &g_GeneratedTextureScratchRuntime.samples[4].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 0x39),
                       &g_GeneratedTextureScratchRuntime.samples[5].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 0x29),
                       &g_GeneratedTextureScratchRuntime.samples[6].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 0x45),
                       &g_GeneratedTextureScratchRuntime.samples[7].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 1),
                       &g_GeneratedTextureScratchRuntime.samples[8].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 9),
                       &g_GeneratedTextureScratchRuntime.samples[9].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 5),
                       &g_GeneratedTextureScratchRuntime.samples[10].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar24 + 0x11),
                       &g_GeneratedTextureScratchRuntime.samples[0xb].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            if ((((pGVar24[0x66].projectedX < (int)g_ProjectionScaleFixed) ||
                 (pGVar24[0xb2].projectedX < (int)g_ProjectionScaleFixed)) ||
                ((pGVar24[0x72].projectedX < (int)g_ProjectionScaleFixed ||
                 (((pGVar24[0xaa].projectedX < (int)g_ProjectionScaleFixed ||
                   (pGVar24[0x56].projectedX < (int)g_ProjectionScaleFixed)) ||
                  (pGVar24[0x3a].projectedX < (int)g_ProjectionScaleFixed)))))) ||
               (((pGVar24[0x2a].projectedX < (int)g_ProjectionScaleFixed ||
                 (pGVar24[0x46].projectedX < (int)g_ProjectionScaleFixed)) ||
                ((pGVar24[2].projectedX < (int)g_ProjectionScaleFixed ||
                 (((pGVar24[10].projectedX < (int)g_ProjectionScaleFixed ||
                   (pGVar24[6].projectedX < (int)g_ProjectionScaleFixed)) ||
                  (pGVar24[0x12].projectedX < (int)g_ProjectionScaleFixed)))))))) {
              GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks(renderContext);
            }
            else {
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 0x65));
              pGVar24[100] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 0xb1));
              pGVar24[0xb0] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 0x71));
              pGVar24[0x70] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 0xa9));
              pGVar24[0xa8] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 0x55));
              pGVar24[0x54] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 0x39));
              pGVar24[0x38] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 0x29));
              pGVar24[0x28] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 0x45));
              pGVar24[0x44] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 1));
              *pGVar24 = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 9));
              pGVar24[8] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 5));
              pGVar24[4] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar24 + 0x11));
              pGVar24[0x10] = GVar39;
              dVar26 = g_GraphicsShadingGridStepQ20;
              dVar21 = g_GraphicsShadingGeneratedTextureTileXQ20;
              iVar27 = (g_GraphicsShadingGridStepQ20 - 0x1000) +
                       g_GraphicsShadingGeneratedTextureTileXQ20;
              iVar20 = ((int)g_GraphicsShadingGridStepQ20 >> 1) +
                       g_GraphicsShadingGeneratedTextureTileXQ20;
              pGVar24[0x66].projectedY = g_GraphicsShadingGeneratedTextureTileXQ20;
              pGVar24[0x72].projectedY = iVar27;
              pGVar24[0xb2].projectedY = dVar21;
              pGVar24[0xaa].projectedY = iVar27;
              pGVar24[0x56].projectedY = dVar21;
              pGVar24[0x3a].projectedY = iVar27;
              pGVar24[0x2a].projectedY = iVar20;
              pGVar24[0x46].projectedY = iVar20;
              iVar27 = ((int)dVar26 >> 2) + iVar20;
              iVar20 = iVar20 - ((int)dVar26 >> 2);
              pGVar24[2].projectedY = iVar20;
              pGVar24[6].projectedY = iVar27;
              pGVar24[10].projectedY = iVar20;
              pGVar24[0x12].projectedY = iVar27;
              dVar21 = g_GraphicsShadingGeneratedTextureTileYQ20;
              iVar29 = g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 +
                       g_GraphicsShadingGeneratedTextureTileYQ20;
              iVar20 = g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 +
                       g_GraphicsShadingGeneratedTextureTileYQ20;
              iVar27 = g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 +
                       g_GraphicsShadingGeneratedTextureTileYQ20;
              pGVar24[0x67].projectedX =
                   g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 +
                   g_GraphicsShadingGeneratedTextureTileYQ20;
              pGVar24[0xb3].projectedX = iVar29;
              pGVar24[0x73].projectedX = iVar20;
              pGVar24[0xab].projectedX = iVar27;
              iVar29 = g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 +
                       dVar21;
              iVar20 = g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 +
                       dVar21;
              iVar27 = g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 +
                       dVar21;
              pGVar24[0x57].projectedX =
                   g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 + dVar21;
              pGVar24[0x3b].projectedX = iVar29;
              pGVar24[0x2b].projectedX = iVar20;
              pGVar24[0x47].projectedX = iVar27;
              iVar29 = g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 +
                       dVar21;
              iVar20 = g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 +
                       dVar21;
              iVar27 = g_GeneratedTextureScratchRuntime.samples[0xb].textureCoordinateOffsetQ20 +
                       dVar21;
              pGVar24[3].projectedX =
                   g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 + dVar21;
              pGVar24[0xb].projectedX = iVar29;
              pGVar24[7].projectedX = iVar20;
              pGVar24[0x13].projectedX = iVar27;
              pGVar24[0x66].projectedX = pGVar24[0x66].projectedX + -0x1000;
              pGVar24[0xb2].projectedX = pGVar24[0xb2].projectedX + -0x1000;
              pGVar24[0x72].projectedX = pGVar24[0x72].projectedX + -0x1000;
              pGVar24[0xaa].projectedX = pGVar24[0xaa].projectedX + -0x1000;
              pGVar24[0x56].projectedX = pGVar24[0x56].projectedX + -0x1000;
              pGVar24[0x3a].projectedX = pGVar24[0x3a].projectedX + -0x1000;
              pGVar24[0x2a].projectedX = pGVar24[0x2a].projectedX + -0x1000;
              pGVar24[0x46].projectedX = pGVar24[0x46].projectedX + -0x1000;
              pGVar24[2].projectedX = pGVar24[2].projectedX + -0x1000;
              pGVar24[10].projectedX = pGVar24[10].projectedX + -0x1000;
              pGVar24[6].projectedX = pGVar24[6].projectedX + -0x1000;
              pGVar24[0x12].projectedX = pGVar24[0x12].projectedX + -0x1000;
              uVar31 = modelNode->tintArgb >> 1;
              uVar25 = uVar31 | 0xffffff;
              bVar16 = (byte)(modelNode->tintArgb >> 0x18);
              uVar17 = CONCAT11(bVar16 >> 1,bVar16 >> 1);
              uVar34 = (undefined1)(uVar25 >> 0x10);
              uVar33 = (undefined1)(uVar25 >> 8);
              uVar38 = CONCAT26(uVar17 >> 4,
                                CONCAT24((ushort)(CONCAT35(CONCAT21(uVar17,uVar34),
                                                           CONCAT14(uVar34,uVar31)) >> 0x20) >> 4,
                                         CONCAT22(CONCAT11(uVar33,uVar33) >> 4,
                                                  CONCAT11((char)uVar25,(char)uVar25) >> 4)));
              uVar31 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[0].terrainRayDistanceQ12;
              if ((int)uVar31 < 0) {
                uVar31 = 0;
              }
              uVar25 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[1].terrainRayDistanceQ12;
              if ((int)uVar25 < 0) {
                uVar25 = 0;
              }
              uVar30 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[2].terrainRayDistanceQ12;
              if ((int)uVar30 < 0) {
                uVar30 = 0;
              }
              uVar31 = uVar31 >> 6;
              uVar25 = uVar25 >> 6;
              uVar30 = uVar30 >> 6;
              if (0xff < uVar31) {
                uVar31 = 0xff;
              }
              if (0xff < uVar25) {
                uVar25 = 0xff;
              }
              if (0xff < uVar30) {
                uVar30 = 0xff;
              }
              uVar35 = pmulhw(*(undefined8 *)(uVar31 * 8 + 0x41ee80),uVar38);
              uVar36 = pmulhw(*(undefined8 *)(uVar25 * 8 + 0x41ee80),uVar38);
              uVar37 = pmulhw(*(undefined8 *)(uVar30 * 8 + 0x41ee80),uVar38);
              sVar4 = (short)uVar35;
              sVar7 = (short)((ulonglong)uVar35 >> 0x10);
              sVar10 = (short)((ulonglong)uVar35 >> 0x20);
              sVar13 = (short)((ulonglong)uVar35 >> 0x30);
              sVar5 = (short)uVar36;
              sVar8 = (short)((ulonglong)uVar36 >> 0x10);
              sVar11 = (short)((ulonglong)uVar36 >> 0x20);
              sVar14 = (short)((ulonglong)uVar36 >> 0x30);
              sVar6 = (short)uVar37;
              sVar9 = (short)((ulonglong)uVar37 >> 0x10);
              sVar12 = (short)((ulonglong)uVar37 >> 0x20);
              sVar15 = (short)((ulonglong)uVar37 >> 0x30);
              pGVar24[0x67].projectedY =
                   CONCAT13((0 < sVar13) * (sVar13 < 0x100) * (char)((ulonglong)uVar35 >> 0x30) -
                            (0xff < sVar13),
                            CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                                     (char)((ulonglong)uVar35 >> 0x20) - (0xff < sVar10),
                                     CONCAT11((0 < sVar7) * (sVar7 < 0x100) *
                                              (char)((ulonglong)uVar35 >> 0x10) - (0xff < sVar7),
                                              (0 < sVar4) * (sVar4 < 0x100) * (char)uVar35 -
                                              (0xff < sVar4))));
              pGVar24[0xb3].projectedY =
                   CONCAT13((0 < sVar14) * (sVar14 < 0x100) * (char)((ulonglong)uVar36 >> 0x30) -
                            (0xff < sVar14),
                            CONCAT12((0 < sVar11) * (sVar11 < 0x100) *
                                     (char)((ulonglong)uVar36 >> 0x20) - (0xff < sVar11),
                                     CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                              (char)((ulonglong)uVar36 >> 0x10) - (0xff < sVar8),
                                              (0 < sVar5) * (sVar5 < 0x100) * (char)uVar36 -
                                              (0xff < sVar5))));
              pGVar24[0x73].projectedY =
                   CONCAT13((0 < sVar15) * (sVar15 < 0x100) * (char)((ulonglong)uVar37 >> 0x30) -
                            (0xff < sVar15),
                            CONCAT12((0 < sVar12) * (sVar12 < 0x100) *
                                     (char)((ulonglong)uVar37 >> 0x20) - (0xff < sVar12),
                                     CONCAT11((0 < sVar9) * (sVar9 < 0x100) *
                                              (char)((ulonglong)uVar37 >> 0x10) - (0xff < sVar9),
                                              (0 < sVar6) * (sVar6 < 0x100) * (char)uVar37 -
                                              (0xff < sVar6))));
              uVar31 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[3].terrainRayDistanceQ12;
              if ((int)uVar31 < 0) {
                uVar31 = 0;
              }
              uVar25 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[4].terrainRayDistanceQ12;
              if ((int)uVar25 < 0) {
                uVar25 = 0;
              }
              uVar30 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[5].terrainRayDistanceQ12;
              if ((int)uVar30 < 0) {
                uVar30 = 0;
              }
              uVar31 = uVar31 >> 6;
              uVar25 = uVar25 >> 6;
              uVar30 = uVar30 >> 6;
              if (0xff < uVar31) {
                uVar31 = 0xff;
              }
              if (0xff < uVar25) {
                uVar25 = 0xff;
              }
              if (0xff < uVar30) {
                uVar30 = 0xff;
              }
              uVar35 = pmulhw(*(undefined8 *)(uVar31 * 8 + 0x41ee80),uVar38);
              uVar36 = pmulhw(*(undefined8 *)(uVar25 * 8 + 0x41ee80),uVar38);
              uVar37 = pmulhw(*(undefined8 *)(uVar30 * 8 + 0x41ee80),uVar38);
              sVar4 = (short)uVar35;
              sVar7 = (short)((ulonglong)uVar35 >> 0x10);
              sVar10 = (short)((ulonglong)uVar35 >> 0x20);
              sVar13 = (short)((ulonglong)uVar35 >> 0x30);
              sVar5 = (short)uVar36;
              sVar8 = (short)((ulonglong)uVar36 >> 0x10);
              sVar11 = (short)((ulonglong)uVar36 >> 0x20);
              sVar14 = (short)((ulonglong)uVar36 >> 0x30);
              sVar6 = (short)uVar37;
              sVar9 = (short)((ulonglong)uVar37 >> 0x10);
              sVar12 = (short)((ulonglong)uVar37 >> 0x20);
              sVar15 = (short)((ulonglong)uVar37 >> 0x30);
              pGVar24[0xab].projectedY =
                   CONCAT13((0 < sVar13) * (sVar13 < 0x100) * (char)((ulonglong)uVar35 >> 0x30) -
                            (0xff < sVar13),
                            CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                                     (char)((ulonglong)uVar35 >> 0x20) - (0xff < sVar10),
                                     CONCAT11((0 < sVar7) * (sVar7 < 0x100) *
                                              (char)((ulonglong)uVar35 >> 0x10) - (0xff < sVar7),
                                              (0 < sVar4) * (sVar4 < 0x100) * (char)uVar35 -
                                              (0xff < sVar4))));
              pGVar24[0x57].projectedY =
                   CONCAT13((0 < sVar14) * (sVar14 < 0x100) * (char)((ulonglong)uVar36 >> 0x30) -
                            (0xff < sVar14),
                            CONCAT12((0 < sVar11) * (sVar11 < 0x100) *
                                     (char)((ulonglong)uVar36 >> 0x20) - (0xff < sVar11),
                                     CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                              (char)((ulonglong)uVar36 >> 0x10) - (0xff < sVar8),
                                              (0 < sVar5) * (sVar5 < 0x100) * (char)uVar36 -
                                              (0xff < sVar5))));
              pGVar24[0x3b].projectedY =
                   CONCAT13((0 < sVar15) * (sVar15 < 0x100) * (char)((ulonglong)uVar37 >> 0x30) -
                            (0xff < sVar15),
                            CONCAT12((0 < sVar12) * (sVar12 < 0x100) *
                                     (char)((ulonglong)uVar37 >> 0x20) - (0xff < sVar12),
                                     CONCAT11((0 < sVar9) * (sVar9 < 0x100) *
                                              (char)((ulonglong)uVar37 >> 0x10) - (0xff < sVar9),
                                              (0 < sVar6) * (sVar6 < 0x100) * (char)uVar37 -
                                              (0xff < sVar6))));
              uVar31 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[6].terrainRayDistanceQ12;
              if ((int)uVar31 < 0) {
                uVar31 = 0;
              }
              uVar25 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[7].terrainRayDistanceQ12;
              if ((int)uVar25 < 0) {
                uVar25 = 0;
              }
              uVar30 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[8].terrainRayDistanceQ12;
              if ((int)uVar30 < 0) {
                uVar30 = 0;
              }
              uVar31 = uVar31 >> 6;
              uVar25 = uVar25 >> 6;
              uVar30 = uVar30 >> 6;
              if (0xff < uVar31) {
                uVar31 = 0xff;
              }
              if (0xff < uVar25) {
                uVar25 = 0xff;
              }
              if (0xff < uVar30) {
                uVar30 = 0xff;
              }
              uVar35 = pmulhw(*(undefined8 *)(uVar31 * 8 + 0x41ee80),uVar38);
              uVar36 = pmulhw(*(undefined8 *)(uVar25 * 8 + 0x41ee80),uVar38);
              uVar37 = pmulhw(*(undefined8 *)(uVar30 * 8 + 0x41ee80),uVar38);
              sVar4 = (short)uVar35;
              sVar7 = (short)((ulonglong)uVar35 >> 0x10);
              sVar10 = (short)((ulonglong)uVar35 >> 0x20);
              sVar13 = (short)((ulonglong)uVar35 >> 0x30);
              sVar5 = (short)uVar36;
              sVar8 = (short)((ulonglong)uVar36 >> 0x10);
              sVar11 = (short)((ulonglong)uVar36 >> 0x20);
              sVar14 = (short)((ulonglong)uVar36 >> 0x30);
              sVar6 = (short)uVar37;
              sVar9 = (short)((ulonglong)uVar37 >> 0x10);
              sVar12 = (short)((ulonglong)uVar37 >> 0x20);
              sVar15 = (short)((ulonglong)uVar37 >> 0x30);
              pGVar24[0x2b].projectedY =
                   CONCAT13((0 < sVar13) * (sVar13 < 0x100) * (char)((ulonglong)uVar35 >> 0x30) -
                            (0xff < sVar13),
                            CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                                     (char)((ulonglong)uVar35 >> 0x20) - (0xff < sVar10),
                                     CONCAT11((0 < sVar7) * (sVar7 < 0x100) *
                                              (char)((ulonglong)uVar35 >> 0x10) - (0xff < sVar7),
                                              (0 < sVar4) * (sVar4 < 0x100) * (char)uVar35 -
                                              (0xff < sVar4))));
              pGVar24[0x47].projectedY =
                   CONCAT13((0 < sVar14) * (sVar14 < 0x100) * (char)((ulonglong)uVar36 >> 0x30) -
                            (0xff < sVar14),
                            CONCAT12((0 < sVar11) * (sVar11 < 0x100) *
                                     (char)((ulonglong)uVar36 >> 0x20) - (0xff < sVar11),
                                     CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                              (char)((ulonglong)uVar36 >> 0x10) - (0xff < sVar8),
                                              (0 < sVar5) * (sVar5 < 0x100) * (char)uVar36 -
                                              (0xff < sVar5))));
              pGVar24[3].projectedY =
                   CONCAT13((0 < sVar15) * (sVar15 < 0x100) * (char)((ulonglong)uVar37 >> 0x30) -
                            (0xff < sVar15),
                            CONCAT12((0 < sVar12) * (sVar12 < 0x100) *
                                     (char)((ulonglong)uVar37 >> 0x20) - (0xff < sVar12),
                                     CONCAT11((0 < sVar9) * (sVar9 < 0x100) *
                                              (char)((ulonglong)uVar37 >> 0x10) - (0xff < sVar9),
                                              (0 < sVar6) * (sVar6 < 0x100) * (char)uVar37 -
                                              (0xff < sVar6))));
              uVar31 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[9].terrainRayDistanceQ12;
              if ((int)uVar31 < 0) {
                uVar31 = 0;
              }
              uVar25 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[10].terrainRayDistanceQ12;
              if ((int)uVar25 < 0) {
                uVar25 = 0;
              }
              uVar30 = 0x5000 - g_GeneratedTextureScratchRuntime.samples[0xb].terrainRayDistanceQ12;
              if ((int)uVar30 < 0) {
                uVar30 = 0;
              }
              uVar31 = uVar31 >> 6;
              uVar25 = uVar25 >> 6;
              uVar30 = uVar30 >> 6;
              if (0xff < uVar31) {
                uVar31 = 0xff;
              }
              if (0xff < uVar25) {
                uVar25 = 0xff;
              }
              if (0xff < uVar30) {
                uVar30 = 0xff;
              }
              uVar35 = pmulhw(*(undefined8 *)(uVar31 * 8 + 0x41ee80),uVar38);
              uVar36 = pmulhw(*(undefined8 *)(uVar25 * 8 + 0x41ee80),uVar38);
              uVar38 = pmulhw(*(undefined8 *)(uVar30 * 8 + 0x41ee80),uVar38);
              sVar4 = (short)uVar35;
              sVar7 = (short)((ulonglong)uVar35 >> 0x10);
              sVar10 = (short)((ulonglong)uVar35 >> 0x20);
              sVar13 = (short)((ulonglong)uVar35 >> 0x30);
              sVar5 = (short)uVar36;
              sVar8 = (short)((ulonglong)uVar36 >> 0x10);
              sVar11 = (short)((ulonglong)uVar36 >> 0x20);
              sVar14 = (short)((ulonglong)uVar36 >> 0x30);
              sVar6 = (short)uVar38;
              sVar9 = (short)((ulonglong)uVar38 >> 0x10);
              sVar12 = (short)((ulonglong)uVar38 >> 0x20);
              sVar15 = (short)((ulonglong)uVar38 >> 0x30);
              pGVar24[0xb].projectedY =
                   CONCAT13((0 < sVar13) * (sVar13 < 0x100) * (char)((ulonglong)uVar35 >> 0x30) -
                            (0xff < sVar13),
                            CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                                     (char)((ulonglong)uVar35 >> 0x20) - (0xff < sVar10),
                                     CONCAT11((0 < sVar7) * (sVar7 < 0x100) *
                                              (char)((ulonglong)uVar35 >> 0x10) - (0xff < sVar7),
                                              (0 < sVar4) * (sVar4 < 0x100) * (char)uVar35 -
                                              (0xff < sVar4))));
              pGVar24[7].projectedY =
                   CONCAT13((0 < sVar14) * (sVar14 < 0x100) * (char)((ulonglong)uVar36 >> 0x30) -
                            (0xff < sVar14),
                            CONCAT12((0 < sVar11) * (sVar11 < 0x100) *
                                     (char)((ulonglong)uVar36 >> 0x20) - (0xff < sVar11),
                                     CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                              (char)((ulonglong)uVar36 >> 0x10) - (0xff < sVar8),
                                              (0 < sVar5) * (sVar5 < 0x100) * (char)uVar36 -
                                              (0xff < sVar5))));
              pGVar24[0x13].projectedY =
                   CONCAT13((0 < sVar15) * (sVar15 < 0x100) * (char)((ulonglong)uVar38 >> 0x30) -
                            (0xff < sVar15),
                            CONCAT12((0 < sVar12) * (sVar12 < 0x100) *
                                     (char)((ulonglong)uVar38 >> 0x20) - (0xff < sVar12),
                                     CONCAT11((0 < sVar9) * (sVar9 < 0x100) *
                                              (char)((ulonglong)uVar38 >> 0x10) - (0xff < sVar9),
                                              (0 < sVar6) * (sVar6 < 0x100) * (char)uVar38 -
                                              (0xff < sVar6))));
              pGVar24[0xc].projectedY =
                   (GraphicsPrimitiveBackendCoordinate)
                   (g_GraphicsShadingTextureSet->entries +
                   g_GraphicsShadingGeneratedTextureSubresourceIndex);
              GVar39 = *pGVar24;
              GVar1 = pGVar24[1];
              GVar2 = pGVar24[2];
              GVar3 = pGVar24[3];
              pGVar24[0x20] = GVar39;
              pGVar24[0x21] = GVar1;
              pGVar24[0x22] = GVar2;
              pGVar24[0x23] = GVar3;
              pGVar24[0x50] = GVar39;
              pGVar24[0x51] = GVar1;
              pGVar24[0x52] = GVar2;
              pGVar24[0x53] = GVar3;
              pGVar24[0x60] = GVar39;
              pGVar24[0x61] = GVar1;
              pGVar24[0x62] = GVar2;
              pGVar24[99] = GVar3;
              pGVar24[0xd0] = GVar39;
              pGVar24[0xd1] = GVar1;
              pGVar24[0xd2] = GVar2;
              pGVar24[0xd3] = GVar3;
              GVar39 = pGVar24[4];
              GVar1 = pGVar24[5];
              GVar2 = pGVar24[6];
              GVar3 = pGVar24[7];
              pGVar24[0x14] = GVar39;
              pGVar24[0x15] = GVar1;
              pGVar24[0x16] = GVar2;
              pGVar24[0x17] = GVar3;
              pGVar24[0x24] = GVar39;
              pGVar24[0x25] = GVar1;
              pGVar24[0x26] = GVar2;
              pGVar24[0x27] = GVar3;
              pGVar24[0x34] = GVar39;
              pGVar24[0x35] = GVar1;
              pGVar24[0x36] = GVar2;
              pGVar24[0x37] = GVar3;
              pGVar24[0x74] = GVar39;
              pGVar24[0x75] = GVar1;
              pGVar24[0x76] = GVar2;
              pGVar24[0x77] = GVar3;
              pGVar24[0x84] = GVar39;
              pGVar24[0x85] = GVar1;
              pGVar24[0x86] = GVar2;
              pGVar24[0x87] = GVar3;
              GVar39 = pGVar24[8];
              GVar1 = pGVar24[9];
              GVar2 = pGVar24[10];
              GVar3 = pGVar24[0xb];
              pGVar24[0x18] = GVar39;
              pGVar24[0x19] = GVar1;
              pGVar24[0x1a] = GVar2;
              pGVar24[0x1b] = GVar3;
              pGVar24[0x48] = GVar39;
              pGVar24[0x49] = GVar1;
              pGVar24[0x4a] = GVar2;
              pGVar24[0x4b] = GVar3;
              pGVar24[0x58] = GVar39;
              pGVar24[0x59] = GVar1;
              pGVar24[0x5a] = GVar2;
              pGVar24[0x5b] = GVar3;
              pGVar24[0xb8] = GVar39;
              pGVar24[0xb9] = GVar1;
              pGVar24[0xba] = GVar2;
              pGVar24[0xbb] = GVar3;
              pGVar24[200] = GVar39;
              pGVar24[0xc9] = GVar1;
              pGVar24[0xca] = GVar2;
              pGVar24[0xcb] = GVar3;
              GVar39 = pGVar24[0x10];
              GVar1 = pGVar24[0x11];
              GVar2 = pGVar24[0x12];
              GVar3 = pGVar24[0x13];
              pGVar24[0x30] = GVar39;
              pGVar24[0x31] = GVar1;
              pGVar24[0x32] = GVar2;
              pGVar24[0x33] = GVar3;
              pGVar24[0x40] = GVar39;
              pGVar24[0x41] = GVar1;
              pGVar24[0x42] = GVar2;
              pGVar24[0x43] = GVar3;
              pGVar24[0x90] = GVar39;
              pGVar24[0x91] = GVar1;
              pGVar24[0x92] = GVar2;
              pGVar24[0x93] = GVar3;
              pGVar24[0xa0] = GVar39;
              pGVar24[0xa1] = GVar1;
              pGVar24[0xa2] = GVar2;
              pGVar24[0xa3] = GVar3;
              pGVar24[0x68] = pGVar24[0x28];
              pGVar24[0x69] = pGVar24[0x29];
              pGVar24[0x6a] = pGVar24[0x2a];
              pGVar24[0x6b] = pGVar24[0x2b];
              pGVar24[0x78] = pGVar24[0x28];
              pGVar24[0x79] = pGVar24[0x29];
              pGVar24[0x7a] = pGVar24[0x2a];
              pGVar24[0x7b] = pGVar24[0x2b];
              pGVar24[0x88] = pGVar24[0x38];
              pGVar24[0x89] = pGVar24[0x39];
              pGVar24[0x8a] = pGVar24[0x3a];
              pGVar24[0x8b] = pGVar24[0x3b];
              pGVar24[0x98] = pGVar24[0x38];
              pGVar24[0x99] = pGVar24[0x39];
              pGVar24[0x9a] = pGVar24[0x3a];
              pGVar24[0x9b] = pGVar24[0x3b];
              pGVar24[0xa4] = pGVar24[0x44];
              pGVar24[0xa5] = pGVar24[0x45];
              pGVar24[0xa6] = pGVar24[0x46];
              pGVar24[0xa7] = pGVar24[0x47];
              pGVar24[0xb4] = pGVar24[0x44];
              pGVar24[0xb5] = pGVar24[0x45];
              pGVar24[0xb6] = pGVar24[0x46];
              pGVar24[0xb7] = pGVar24[0x47];
              pGVar24[0xc4] = pGVar24[0x54];
              pGVar24[0xc5] = pGVar24[0x55];
              pGVar24[0xc6] = pGVar24[0x56];
              pGVar24[199] = pGVar24[0x57];
              pGVar24[0xd4] = pGVar24[0x54];
              pGVar24[0xd5] = pGVar24[0x55];
              pGVar24[0xd6] = pGVar24[0x56];
              pGVar24[0xd7] = pGVar24[0x57];
              pGVar24[0xd8] = pGVar24[100];
              pGVar24[0xd9] = pGVar24[0x65];
              pGVar24[0xda] = pGVar24[0x66];
              pGVar24[0xdb] = pGVar24[0x67];
              pGVar24[0x80] = pGVar24[0x70];
              pGVar24[0x81] = pGVar24[0x71];
              pGVar24[0x82] = pGVar24[0x72];
              pGVar24[0x83] = pGVar24[0x73];
              pGVar24[0x94] = pGVar24[0xa8];
              pGVar24[0x95] = pGVar24[0xa9];
              pGVar24[0x96] = pGVar24[0xaa];
              pGVar24[0x97] = pGVar24[0xab];
              pGVar24[0xc0] = pGVar24[0xb0];
              pGVar24[0xc1] = pGVar24[0xb1];
              pGVar24[0xc2] = pGVar24[0xb2];
              pGVar24[0xc3] = pGVar24[0xb3];
              GVar39 = pGVar24[0xc];
              GVar1 = pGVar24[0xd];
              GVar2 = pGVar24[0xe];
              GVar3 = pGVar24[0xf];
              pGVar24[0x1c] = GVar39;
              pGVar24[0x1d] = GVar1;
              pGVar24[0x1e] = GVar2;
              pGVar24[0x1f] = GVar3;
              pGVar24[0x2c] = GVar39;
              pGVar24[0x2d] = GVar1;
              pGVar24[0x2e] = GVar2;
              pGVar24[0x2f] = GVar3;
              pGVar24[0x3c] = GVar39;
              pGVar24[0x3d] = GVar1;
              pGVar24[0x3e] = GVar2;
              pGVar24[0x3f] = GVar3;
              pGVar24[0x4c] = GVar39;
              pGVar24[0x4d] = GVar1;
              pGVar24[0x4e] = GVar2;
              pGVar24[0x4f] = GVar3;
              pGVar24[0x5c] = GVar39;
              pGVar24[0x5d] = GVar1;
              pGVar24[0x5e] = GVar2;
              pGVar24[0x5f] = GVar3;
              pGVar24[0x6c] = GVar39;
              pGVar24[0x6d] = GVar1;
              pGVar24[0x6e] = GVar2;
              pGVar24[0x6f] = GVar3;
              pGVar24[0x7c] = GVar39;
              pGVar24[0x7d] = GVar1;
              pGVar24[0x7e] = GVar2;
              pGVar24[0x7f] = GVar3;
              pGVar24[0x8c] = GVar39;
              pGVar24[0x8d] = GVar1;
              pGVar24[0x8e] = GVar2;
              pGVar24[0x8f] = GVar3;
              pGVar24[0x9c] = GVar39;
              pGVar24[0x9d] = GVar1;
              pGVar24[0x9e] = GVar2;
              pGVar24[0x9f] = GVar3;
              pGVar24[0xac] = GVar39;
              pGVar24[0xad] = GVar1;
              pGVar24[0xae] = GVar2;
              pGVar24[0xaf] = GVar3;
              pGVar24[0xbc] = GVar39;
              pGVar24[0xbd] = GVar1;
              pGVar24[0xbe] = GVar2;
              pGVar24[0xbf] = GVar3;
              pGVar24[0xcc] = GVar39;
              pGVar24[0xcd] = GVar1;
              pGVar24[0xce] = GVar2;
              pGVar24[0xcf] = GVar3;
              pGVar24[0xdc] = GVar39;
              pGVar24[0xdd] = GVar1;
              pGVar24[0xde] = GVar2;
              pGVar24[0xdf] = GVar3;
              GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set(modelNode);
              if (iVar29 != 0) {
                GraphicsShadingGeneratedTexture_FilterGridScratchMmx();
              }
              GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear(modelNode);
              GraphicsShadingGeneratedTexture_AdvanceTileCursor();
            }
          }
        }
      }
    }
  }
  return;
}


/* Address: 0x004BCF70.
   Ownership: graphics/render/shading.
   Purpose: Allocates and aligns a 256 by 256 byte lookup table and fills each entry with the first input clamped
   to within plus or minus 0x15 of the second input. Carry reports allocation failure.
*/
bool __thandor_void_preserve_ecx_edx GraphicsIntensityClampTable_InitializeCf(void)

{
  int iVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  char *pcVar5;
  ArenaAllocEaxCf5 AVar6;
  
  AVar6 = (*g_MemoryApi.alloc)(0x20000);
  if (!AVar6.carry) {
    uVar3 = 0;
    pcVar5 = (char *)(AVar6.eax + 0xffff & 0xffff0000);
    iVar1 = 0x100;
    iVar4 = 0;
    g_GraphicsIntensityClampTableBase = pcVar5;
    do {
      do {
        cVar2 = (char)uVar3;
        if (iVar4 < (int)uVar3) {
          if (iVar4 + 0x15 < (int)uVar3) {
            *pcVar5 = (char)(iVar4 + 0x15);
          }
          else {
            *pcVar5 = cVar2;
          }
        }
        else if ((int)uVar3 < iVar4 + -0x15) {
          *pcVar5 = (char)(iVar4 + -0x15);
        }
        else {
          *pcVar5 = cVar2;
        }
        pcVar5 = pcVar5 + 1;
        uVar3 = (uint)(byte)(cVar2 + 1U);
      } while ((byte)(cVar2 + 1U) != 0);
      iVar4 = iVar4 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
    return false;
  }
  return true;
}


/* Address: 0x004CCA90.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading runtime accumulate compact lighting at point mmx register result.
*/
MmxPackedValue64 __thandor_void_preserve_ecx_edx_mm1
GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
          (GraphicsFixedVec3 *worldPointQ12,MmxPackedValue64 packedLightAccumulatorMmx)

{
  PackedRgb24 PVar1;
  longlong lVar2;
  ushort uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  GraphicsShadingRecordCount GVar8;
  uint uVar9;
  GraphicsShadingRuntimeRecord *shadingRecord1;
  undefined1 uVar11;
  undefined1 uVar12;
  undefined8 uVar10;
  
  shadingRecord1 = g_GraphicsShadingCompactRecords;
  for (GVar8 = g_GraphicsShadingCompactRecordCount; GVar8 != 0; GVar8 = GVar8 - 1) {
    if (shadingRecord1->targetRadiusQ12 != 0) {
      uVar7 = (uint)shadingRecord1->squaredRadiusQ24;
      iVar4 = worldPointQ12->x - shadingRecord1->worldXQ12;
      lVar2 = (longlong)iVar4 * (longlong)iVar4;
      uVar5 = (uint)lVar2;
      uVar9 = uVar7 - uVar5;
      iVar4 = (*(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) -
              (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar7 < uVar5);
      if (-1 < iVar4) {
        iVar6 = worldPointQ12->y - shadingRecord1->worldYQ12;
        lVar2 = (longlong)iVar6 * (longlong)iVar6;
        uVar7 = (uint)lVar2;
        uVar5 = uVar9 - uVar7;
        iVar4 = (iVar4 - (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar9 < uVar7);
        if (-1 < iVar4) {
          iVar6 = worldPointQ12->z - shadingRecord1->worldZQ12;
          lVar2 = (longlong)iVar6 * (longlong)iVar6;
          uVar7 = (uint)lVar2;
          iVar4 = (iVar4 - (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar5 < uVar7);
          if (-1 < iVar4) {
            PVar1 = shadingRecord1->packedColorRgbActive;
            uVar9 = *(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) << 0x14 |
                    (uint)shadingRecord1->squaredRadiusQ24 >> 0xc;
            if (uVar9 != 0) {
              uVar11 = (undefined1)(PVar1 >> 0x18);
              uVar3 = CONCAT11(uVar11,uVar11);
              uVar12 = (undefined1)(PVar1 >> 0x10);
              uVar11 = (undefined1)(PVar1 >> 8);
              uVar10 = pmulhw(CONCAT26(uVar3 >> 2,
                                       CONCAT24((ushort)(CONCAT35(CONCAT21(uVar3,uVar12),
                                                                  CONCAT14(uVar12,PVar1)) >> 0x20)
                                                >> 2,CONCAT22(CONCAT11(uVar11,uVar11) >> 2,
                                                              CONCAT11((char)PVar1,(char)PVar1) >> 2
                                                             ))),
                              *(undefined8 *)
                               (&g_PackedLightingLookupTable +
                               ((iVar4 * 0x8000000 | uVar5 - uVar7 >> 5) / uVar9) * 8));
              packedLightAccumulatorMmx = paddusw(packedLightAccumulatorMmx,uVar10);
            }
          }
        }
      }
    }
    shadingRecord1 = shadingRecord1 + 1;
  }
  return packedLightAccumulatorMmx;
}


/* Address: 0x004CCB40.
   Ownership: graphics/render/shading.
   Purpose: Allocates one free GraphicsShadingRuntimeRecord. transitionDurationTicks controls radius interpolation;
   EAX and CF remain the nonstandard pointer/status result channels.
*/
GraphicsShadingRuntimeRecordEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsShadingRuntime_AllocateRecordRegs
          (GraphicsTransitionTickCount transitionDurationTicks,GraphicsRadiusQ12 radiusQ12,
          PackedRgb24 packedColorRgb,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12)

{
  int recordsRemaining;
  GraphicsShadingRuntimeRecord *recordCursor;
  GraphicsShadingRuntimeRecordEaxCf5 GVar1;
  GraphicsShadingRuntimeRecordEaxCf5 GVar2;
  
  if (packedColorRgb != 0) {
    recordCursor = g_GraphicsShadingRuntimeRecords;
    recordsRemaining = 0x100;
    do {
      if (recordCursor->packedColorRgbActive == 0) {
        recordCursor->targetRadiusQ12 = radiusQ12;
        recordCursor->packedColorRgbActive = packedColorRgb & 0xffffff;
        if (transitionDurationTicks == 0) {
          recordCursor->squaredRadiusQ24 = (longlong)radiusQ12 * (longlong)radiusQ12;
          recordCursor->radiusTransitionDurationTicks = 0;
          recordCursor->radiusTransitionElapsedTicks = 0;
        }
        else {
          *(undefined4 *)((int)&recordCursor->squaredRadiusQ24 + 4) = 0;
          *(undefined4 *)&recordCursor->squaredRadiusQ24 = 0;
          recordCursor->radiusTransitionDurationTicks = transitionDurationTicks;
          recordCursor->radiusTransitionElapsedTicks = 0;
        }
        recordCursor->worldXQ12 = worldXQ12;
        recordCursor->worldYQ12 = worldYQ12;
        recordCursor->worldZQ12 = worldZQ12;
        GVar2.carry = false;
        GVar2.record = recordCursor;
        return GVar2;
      }
      recordCursor = recordCursor + 1;
      recordsRemaining = recordsRemaining + -1;
    } while (recordsRemaining != 0);
  }
  GVar1.record = (GraphicsShadingRuntimeRecord *)0x0;
  GVar1.carry = true;
  return GVar1;
}


/* Address: 0x004CCC60.
   Ownership: graphics/render/shading.
   Purpose: Clears all 256 GraphicsShadingRuntimeRecord entries, exactly 0x4000 bytes. Preserved EAX is incidental,
   not a normal return value.
*/
void __thandor_void_preserve_eax_ecx GraphicsShadingRuntime_ClearRecordTable(void)

{
  int recordDwordsRemaining;
  GraphicsShadingRuntimeRecord *recordDwordCursor;
  
  recordDwordCursor = g_GraphicsShadingRuntimeRecords;
  for (recordDwordsRemaining = 0x1000; recordDwordsRemaining != 0;
      recordDwordsRemaining = recordDwordsRemaining + -1) {
    recordDwordCursor->worldXQ12 = 0;
    recordDwordCursor = (GraphicsShadingRuntimeRecord *)&recordDwordCursor->worldYQ12;
  }
  return;
}


/* Address: 0x004CCD00.
   Ownership: graphics/render/shading.
   Purpose: Scans all 256 runtime shading records, copies the active lighting fields into the fixed compact table,
   and publishes the compact record count. Exact body 004CCD00-004CCD6A.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx GraphicsShadingRuntime_RebuildCompactLightingRecords(void)

{
  GraphicsRadiusQ12 GVar1;
  undefined4 uVar2;
  GraphicsShadingRecordCount GVar3;
  int iVar4;
  GraphicsShadingRuntimeRecord *point;
  GraphicsShadingRuntimeRecord *output;
  
  point = g_GraphicsShadingRuntimeRecords;
  output = g_GraphicsShadingCompactRecords;
  iVar4 = 0x100;
  GVar3 = 0;
  do {
    if (point->packedColorRgbActive != 0) {
      FixedTransform_ApplyPoint
                ((GraphicsFixedVec3 *)output,(GraphicsFixedVec3 *)point,&g_ViewProjectionMatrixFixed
                );
      GVar1 = point->targetRadiusQ12;
      output->packedColorRgbActive = point->packedColorRgbActive;
      output->targetRadiusQ12 = GVar1;
      uVar2 = *(undefined4 *)((int)&point->squaredRadiusQ24 + 4);
      GVar3 = GVar3 + 1;
      *(int *)&output->squaredRadiusQ24 = (int)point->squaredRadiusQ24;
      *(undefined4 *)((int)&output->squaredRadiusQ24 + 4) = uVar2;
      output = output + 1;
    }
    point = point + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  g_GraphicsShadingCompactRecordCount = GVar3;
  return;
}


/* Address: 0x004CCD70.
   Ownership: graphics/render/shading.
   Purpose: Filters compact lighting records by sphere overlap, copies matching 0x40-byte records into
   g_GraphicsShadingNearbyRecords, and publishes g_GraphicsShadingNearbyRecordCount. Preserved EDX:EAX is
   incidental.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingRuntime_CollectNearbyRecords
          (GraphicsRadiusQ12 queryRadiusQ12,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12)

{
  int iVar1;
  int deltaYQ12;
  int deltaZQ12;
  dword sourceRecordsRemaining;
  GraphicsShadingRuntimeRecord *sourceRecordCursor;
  GraphicsShadingRuntimeRecord *destinationRecordCursor;
  longlong combinedRadiusSquaredQ24;
  longlong distanceSquaredQ24;
  
  sourceRecordCursor = g_GraphicsShadingCompactRecords;
  destinationRecordCursor = g_GraphicsShadingNearbyRecords;
  g_GraphicsShadingNearbyRecordCount = 0;
  for (sourceRecordsRemaining = g_GraphicsShadingCompactRecordCount; sourceRecordsRemaining != 0;
      sourceRecordsRemaining = sourceRecordsRemaining - 1) {
    iVar1 = worldXQ12 - sourceRecordCursor->worldXQ12;
    deltaYQ12 = worldYQ12 - sourceRecordCursor->worldYQ12;
    deltaZQ12 = worldZQ12 - sourceRecordCursor->worldZQ12;
    distanceSquaredQ24 =
         (longlong)deltaYQ12 * (longlong)deltaYQ12 + (longlong)iVar1 * (longlong)iVar1 +
         (longlong)deltaZQ12 * (longlong)deltaZQ12;
    iVar1 = queryRadiusQ12 + sourceRecordCursor->targetRadiusQ12;
    combinedRadiusSquaredQ24 = (longlong)iVar1 * (longlong)iVar1;
    if ((int)(((int)((ulonglong)combinedRadiusSquaredQ24 >> 0x20) -
              (int)((ulonglong)distanceSquaredQ24 >> 0x20)) -
             (uint)((uint)combinedRadiusSquaredQ24 < (uint)distanceSquaredQ24)) < 0) {
      sourceRecordCursor = sourceRecordCursor + 1;
    }
    else {
      g_GraphicsShadingNearbyRecordCount = g_GraphicsShadingNearbyRecordCount + 1;
      for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
        destinationRecordCursor->worldXQ12 = sourceRecordCursor->worldXQ12;
        sourceRecordCursor = (GraphicsShadingRuntimeRecord *)&sourceRecordCursor->worldYQ12;
        destinationRecordCursor =
             (GraphicsShadingRuntimeRecord *)&destinationRecordCursor->worldYQ12;
      }
    }
  }
  return;
}


/* Address: 0x004CCFF0.
   Ownership: graphics/render/shading.
   Purpose: Allocates a square byte grid of (gridHalfSize*2)^2, creates a generated gfx image with one 256-entry
   palette and the source-entry table at fixed offset 0xA00, appends subresourceCount direct 0x20-byte source
   entries with textureDimension squared pixels, derives fixed-point sampling state, creates the graphics texture
   set, and reports failure through CF. Typed parameters: p0 subresourceCount→GraphicsAssetSubresourceCount_V308.
   Nearby but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p1
   gridHalfSize→GraphicsPixelDimension_V302, p2 textureDimension→GraphicsPixelDimension_V302.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsShadingRuntime_InitializeGeneratedTextureCf
          (GraphicsAssetSubresourceCount subresourceCount,GraphicsPixelDimension gridHalfSize,
          GraphicsPixelDimension textureDimension)

{
  GraphicsGeneratedTextureAssetOrEntryView200 *pGVar2;
  AssetMagic AVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  AssetRelativeOffset PVar7;
  GraphicsGeneratedTextureAssetOrEntryView200 *pGVar5;
  ArenaAllocEaxCf5 AVar6;
  GraphicsTextureSetEaxCf5 GVar7;
  StatusValueEaxCf5 SVar8;
  
  uVar2 = gridHalfSize * 2 * gridHalfSize * 2;
  AVar6 = (*g_MemoryApi.alloc)(uVar2);
  pGVar2 = (GraphicsGeneratedTextureAssetOrEntryView200 *)AVar6.eax;
  if (!AVar6.carry) {
    uVar2 = uVar2 >> 2;
    g_GraphicsShadingGridScratchInterior = (undefined *)((int)pGVar2 + uVar2 + (gridHalfSize >> 1));
    g_GraphicsShadingGridScratch = pGVar2;
    for (; uVar2 != 0; uVar2 = uVar2 - 1) {
      (pGVar2->asset).common.magic = 0;
      pGVar2 = (GraphicsGeneratedTextureAssetOrEntryView200 *)
               &(pGVar2->asset).common.allocationSizeBytes;
    }
    uVar2 = (textureDimension * textureDimension + 0x20) * subresourceCount + 0xa00;
    AVar6 = (*g_MemoryApi.alloc)(uVar2);
    pGVar2 = (GraphicsGeneratedTextureAssetOrEntryView200 *)AVar6.eax;
    if (!AVar6.carry) {
      pGVar5 = pGVar2;
      g_GraphicsShadingGeneratedAsset = (GraphicsTextureSourceAsset *)pGVar2;
      for (uVar3 = uVar2 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
        (pGVar5->asset).common.magic = 0;
        pGVar5 = (GraphicsGeneratedTextureAssetOrEntryView200 *)
                 &(pGVar5->asset).common.allocationSizeBytes;
      }
      (pGVar2->asset).common.magic = ASSET_MAGIC_GFX;
      (pGVar2->asset).common.allocationSizeBytes = uVar2;
      (pGVar2->asset).tableDescriptor.subresourceCount = subresourceCount;
      (pGVar2->asset).tableDescriptor.paletteBankCount = 1;
      (pGVar2->asset).tableDescriptor.subresourceTableOffset = 0xa00;
      AVar1 = 0xffffff;
      pGVar5 = pGVar2 + 1;
      iVar4 = 0x100;
      do {
        (pGVar5->asset).common.magic = AVar1;
        AVar1 = AVar1 + 0x1000000;
        pGVar5 = (GraphicsGeneratedTextureAssetOrEntryView200 *)
                 &(pGVar5->asset).common.formatVersion;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      g_GraphicsShadingSubresourceCount = subresourceCount;
      pGVar5 = pGVar2 + 5;
      PVar7 = subresourceCount * 0x20 + 0xa00;
      do {
        (pGVar5->asset).common.magic = textureDimension;
        (pGVar5->asset).common.allocationSizeBytes = textureDimension;
        (pGVar5->asset).common.formatVersion = 0;
        (pGVar5->sourceEntry).dataOffset = PVar7;
        (pGVar5->asset).common.buildMetadata.timestamps.dateValue0 = 0;
        (pGVar5->asset).common.buildMetadata.timestamps.timeValue0 = 0;
        (pGVar5->asset).common.buildMetadata.timestamps.dateValue1 = textureDimension;
        (pGVar5->asset).common.buildMetadata.timestamps.timeValue1 = textureDimension;
        PVar7 = PVar7 + textureDimension * textureDimension;
        pGVar5 = (GraphicsGeneratedTextureAssetOrEntryView200 *)
                 &(pGVar5->asset).common.buildMetadata.timestamps.dateValue2;
        subresourceCount = subresourceCount - 1;
      } while (subresourceCount != 0);
      g_GraphicsShadingTextureDimension = textureDimension;
      g_GraphicsShadingGridHalfSize = gridHalfSize;
      g_GraphicsShadingGridStepQ20 =
           (dword)(((ulonglong)gridHalfSize * 0x100000) / (ulonglong)textureDimension);
      iVar4 = ((int)gridHalfSize >> 1) + -1;
      g_GraphicsShadingPositiveGridOriginQ12 = iVar4 * 0x1000;
      g_GraphicsShadingNegativeGridOriginQ12 = iVar4 * -0x1000;
      g_GraphicsShadingGridStepQ20Current = g_GraphicsShadingGridStepQ20;
      GVar7 = (*g_GraphicsCreateTextureSet)(g_GraphicsShadingGeneratedAsset);
      pGVar2 = (GraphicsGeneratedTextureAssetOrEntryView200 *)GVar7.textureSet;
      if (!GVar7.carry) {
        g_GraphicsShadingTextureSet = (GraphicsTextureSet *)&pGVar2->asset;
        return (StatusValueEaxCf5)((uint5)GVar7 & 0xffffffff);
      }
      (*g_MemoryApi.free)(g_GraphicsShadingGeneratedAsset);
    }
  }
  SVar8.carry = true;
  SVar8.valueOrError = (dword)pGVar2;
  return SVar8;
}


/* Address: 0x004CD1B0.
   Ownership: graphics/render/shading.
   Purpose: Destroys the generated shading texture set, frees the generated gfx allocation and square grid
   allocation, and clears all three published pointers. EAX is preserved.
*/
void __thandor_void_preserve_eax GraphicsShadingRuntime_Shutdown(void)

{
  (*g_GraphicsDestroyTextureSet)(g_GraphicsShadingTextureSet);
  g_GraphicsShadingTextureSet = (GraphicsTextureSet *)0x0;
  (*g_MemoryApi.free)(g_GraphicsShadingGeneratedAsset);
  g_GraphicsShadingGeneratedAsset = (GraphicsTextureSourceAsset *)0x0;
  (*g_MemoryApi.free)(g_GraphicsShadingGridScratch);
  g_GraphicsShadingGridScratch = (void *)0x0;
  return;
}


/* Address: 0x004CD200.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture reset pass scratch and clear alpha planes.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes(void)

{
  AssetRelativeOffset AVar1;
  uint uVar2;
  byte *pbVar3;
  
  g_GeneratedTextureScratchRuntime.downsampleBorderOffset = g_TextureDownsampleShift << 2;
  g_GraphicsShadingGeneratedTexturePixelCursor =
       (g_GraphicsShadingGeneratedAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
       ((g_GraphicsShadingTextureDimension + 1) * g_GraphicsShadingGridHalfSize >> 1) +
       *(int *)((g_GraphicsShadingGeneratedAsset->common).buildMetadata.assetRelativeAddressAnchor28
               + ((g_GraphicsShadingGeneratedAsset->tableDescriptor).subresourceTableOffset - 0x1c))
       + -0x28;
  g_GraphicsShadingGeneratedTextureTileX = 0;
  g_GraphicsShadingGeneratedTextureTileY = 0;
  g_GraphicsShadingGeneratedTextureSubresourceIndex = 0;
  g_GraphicsShadingGeneratedTextureTileXQ20 = 0;
  g_GraphicsShadingGeneratedTextureTileYQ20 = 0;
  g_GraphicsShadingGeneratedTextureCompletedTraversalCount = 0;
  AVar1 = (g_GraphicsShadingGeneratedAsset->tableDescriptor).subresourceTableOffset;
  pbVar3 = (g_GraphicsShadingGeneratedAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
           g_GraphicsShadingSubresourceCount * 0x20 + AVar1 + -0x28;
  for (uVar2 = g_GraphicsShadingSubresourceCount *
               *(int *)((g_GraphicsShadingGeneratedAsset->common).buildMetadata.
                        assetRelativeAddressAnchor28 + (AVar1 - 0x10)) *
               *(int *)((g_GraphicsShadingGeneratedAsset->common).buildMetadata.
                        assetRelativeAddressAnchor28 + (AVar1 - 0xc)) >> 2; uVar2 != 0;
      uVar2 = uVar2 - 1) {
    pbVar3[0] = 0;
    pbVar3[1] = 0;
    pbVar3[2] = 0;
    pbVar3[3] = 0;
    pbVar3 = pbVar3 + 4;
  }
  return;
}


/* Address: 0x004CD360.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture refresh touched alpha subresources.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources(void)

{
  dword arg0;
  GraphicsSubresourceIndex GVar1;
  
  arg0 = 0;
  for (GVar1 = g_GraphicsShadingGeneratedTextureSubresourceIndex; GVar1 != 0; GVar1 = GVar1 - 1) {
    (*g_GraphicsRefreshTextureAlpha)(arg0,g_GraphicsShadingTextureSet);
    arg0 = arg0 + 1;
  }
  if ((g_GraphicsShadingGeneratedTextureCompletedTraversalCount == 0) &&
     ((g_GraphicsShadingGeneratedTextureTileX != 0 || (g_GraphicsShadingGeneratedTextureTileY != 0))
     )) {
    (*g_GraphicsRefreshTextureAlpha)(arg0,g_GraphicsShadingTextureSet);
  }
  return;
}


/* Address: 0x004D1170.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture reserve one projected point block carry-flag result.
*/
void GraphicsShadingGeneratedTexture_ReserveOneProjectedPointBlockCf
               (GeneratedTextureRenderContextView *renderContext)

{
  uint *puVar1;
  uint uVar2;
  
  puVar1 = renderContext->projectedPointBlockPool;
  uVar2 = puVar1[1];
  if (uVar2 + 1 < *puVar1) {
    puVar1[1] = uVar2 + 1;
    puVar1[uVar2 * 4 + 9] = uVar2 * 0x80 + puVar1[2];
    return;
  }
  return;
}


/* Address: 0x004CD880.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture rasterize record batch flag0 clear.
   Local calls: GraphicsShadingGeneratedTexture_TransformPointXYQuantized,
   GraphicsShadingGeneratedTexture_RasterizeTriangleMask.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear(ModelMeshGroupAddress32 meshGroup)

{
  int iVar1;
  int iVar2;
  GraphicsFixedVec3 *point;
  
  iVar1 = *(int *)(meshGroup + 8);
  iVar2 = *(int *)(meshGroup + 0xc);
  if (((*(uint *)(meshGroup + 0x10) & 1) == 0) &&
     (point = (GraphicsFixedVec3 *)(meshGroup + 0x20), iVar1 != 0)) {
    do {
      GraphicsShadingGeneratedTexture_TransformPointXYQuantized
                ((GraphicsFixedVec2 *)&point[2].z,point,
                 &g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform);
      point = (GraphicsFixedVec3 *)&point[5].y;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
    for (; iVar2 != 0; iVar2 = iVar2 + -1) {
      GraphicsShadingGeneratedTexture_RasterizeTriangleMask
                ((GraphicsFixedVec2 *)(*(int *)((int)point + 0x18) + 0x20),
                 (GraphicsFixedVec2 *)(*(int *)((int)point + 0xc) + 0x20),
                 (GraphicsFixedVec2 *)(point->x + 0x20));
      point = (GraphicsFixedVec3 *)((int)point + 0x40);
    }
  }
  return;
}


/* Address: 0x004CD930.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture traverse hierarchy flag0 clear.
   Local calls: GraphicsShadingGeneratedTexture_ComposeTransform,
   GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear(ModelRuntimeNode *modelNode)

{
  GraphicsFixedVec3 *pGVar1;
  GraphicsWorldCoordinateQ12 *pGVar2;
  ModelResourceHitTestAndRenderView210 *pMVar3;
  GraphicsWorldCoordinateQ12 GVar4;
  GraphicsWorldCoordinateQ12 GVar5;
  GraphicsWorldCoordinateQ12 GVar6;
  int iVar7;
  dword dVar8;
  byte *meshGroup;
  
  GVar6 = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
  GVar5 = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
  GVar4 = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  pGVar1 = &(modelNode->worldTransform).translation;
  pGVar1->x = pGVar1->x - g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  pGVar2 = &(modelNode->worldTransform).translation.y;
  *pGVar2 = *pGVar2 - GVar5;
  pGVar2 = &(modelNode->worldTransform).translation.z;
  *pGVar2 = *pGVar2 - GVar6;
  GraphicsShadingGeneratedTexture_ComposeTransform
            (&g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform,
             &modelNode->worldTransform,
             &g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform);
  pGVar1 = &(modelNode->worldTransform).translation;
  pGVar1->x = pGVar1->x + GVar4;
  pGVar2 = &(modelNode->worldTransform).translation.y;
  *pGVar2 = *pGVar2 + GVar5;
  pGVar2 = &(modelNode->worldTransform).translation.z;
  *pGVar2 = *pGVar2 + GVar6;
  pMVar3 = (modelNode->modelPayload).modelResource;
  iVar7 = *(int *)pMVar3->reservedEC_1FF;
  if (iVar7 != 0) {
    meshGroup = pMVar3->reserved00_AF + iVar7 + 0x20;
    for (iVar7 = *(int *)(pMVar3->reserved00_AF + iVar7 + 4); iVar7 != 0; iVar7 = iVar7 + -1) {
      GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear
                ((ModelMeshGroupAddress32)meshGroup);
      meshGroup = meshGroup + *(int *)meshGroup;
    }
  }
  iVar7 = 0;
  for (dVar8 = modelNode->childCount; dVar8 != 0; dVar8 = dVar8 - 1) {
    if (modelNode->childNodes[iVar7] != (ModelRuntimeNode *)0x0) {
      GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear(modelNode->childNodes[iVar7]);
    }
    iVar7 = iVar7 + 1;
  }
  return;
}


/* Address: 0x004CD9F0.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture rasterize record batch flag0 set.
   Local calls: GraphicsShadingGeneratedTexture_TransformPointXYQuantized,
   GraphicsShadingGeneratedTexture_RasterizeTriangleMask.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set(ModelMeshGroupAddress32 meshGroup)

{
  int iVar1;
  int iVar2;
  GraphicsFixedVec3 *point;
  
  iVar1 = *(int *)(meshGroup + 8);
  iVar2 = *(int *)(meshGroup + 0xc);
  if (((*(uint *)(meshGroup + 0x10) & 1) != 0) &&
     (point = (GraphicsFixedVec3 *)(meshGroup + 0x20), iVar1 != 0)) {
    do {
      GraphicsShadingGeneratedTexture_TransformPointXYQuantized
                ((GraphicsFixedVec2 *)&point[2].z,point,
                 &g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform);
      point = (GraphicsFixedVec3 *)&point[5].y;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
    for (; iVar2 != 0; iVar2 = iVar2 + -1) {
      GraphicsShadingGeneratedTexture_RasterizeTriangleMask
                ((GraphicsFixedVec2 *)(*(int *)((int)point + 0x18) + 0x20),
                 (GraphicsFixedVec2 *)(*(int *)((int)point + 0xc) + 0x20),
                 (GraphicsFixedVec2 *)(point->x + 0x20));
      point = (GraphicsFixedVec3 *)((int)point + 0x40);
    }
  }
  return;
}


/* Address: 0x004CDAB0.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture traverse hierarchy flag0 set.
   Local calls: GraphicsShadingGeneratedTexture_ComposeTransform,
   GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set(ModelRuntimeNode *modelNode)

{
  GraphicsFixedVec3 *pGVar1;
  GraphicsWorldCoordinateQ12 *pGVar2;
  ModelResourceHitTestAndRenderView210 *pMVar3;
  GraphicsWorldCoordinateQ12 GVar4;
  GraphicsWorldCoordinateQ12 GVar5;
  GraphicsWorldCoordinateQ12 GVar6;
  int iVar7;
  dword dVar8;
  byte *meshGroup;
  
  GVar6 = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
  GVar5 = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
  GVar4 = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  pGVar1 = &(modelNode->worldTransform).translation;
  pGVar1->x = pGVar1->x - g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  pGVar2 = &(modelNode->worldTransform).translation.y;
  *pGVar2 = *pGVar2 - GVar5;
  pGVar2 = &(modelNode->worldTransform).translation.z;
  *pGVar2 = *pGVar2 - GVar6;
  GraphicsShadingGeneratedTexture_ComposeTransform
            (&g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform,
             &modelNode->worldTransform,
             &g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform);
  pGVar1 = &(modelNode->worldTransform).translation;
  pGVar1->x = pGVar1->x + GVar4;
  pGVar2 = &(modelNode->worldTransform).translation.y;
  *pGVar2 = *pGVar2 + GVar5;
  pGVar2 = &(modelNode->worldTransform).translation.z;
  *pGVar2 = *pGVar2 + GVar6;
  pMVar3 = (modelNode->modelPayload).modelResource;
  iVar7 = *(int *)pMVar3->reservedEC_1FF;
  if (iVar7 != 0) {
    meshGroup = pMVar3->reserved00_AF + iVar7 + 0x20;
    for (iVar7 = *(int *)(pMVar3->reserved00_AF + iVar7 + 4); iVar7 != 0; iVar7 = iVar7 + -1) {
      GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set
                ((ModelMeshGroupAddress32)meshGroup);
      meshGroup = meshGroup + *(int *)meshGroup;
    }
  }
  iVar7 = 0;
  for (dVar8 = modelNode->childCount; dVar8 != 0; dVar8 = dVar8 - 1) {
    if (modelNode->childNodes[iVar7] != (ModelRuntimeNode *)0x0) {
      GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set(modelNode->childNodes[iVar7]);
    }
    iVar7 = iVar7 + 1;
  }
  return;
}


/* Address: 0x004CDB80.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture accumulate projected bounds from records.
   Local calls: GraphicsShadingGeneratedTexture_TransformPointXY.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords
          (ModelMeshGroupAddress32 meshGroup)

{
  int iVar1;
  int iVar2;
  int iVar3;
  GraphicsFixedVec3 *point;
  
  point = (GraphicsFixedVec3 *)(meshGroup + 0x20);
  for (iVar3 = *(int *)(meshGroup + 8); iVar3 != 0; iVar3 = iVar3 + -1) {
    GraphicsShadingGeneratedTexture_TransformPointXY
              ((GraphicsFixedVec2 *)&point[2].z,point,
               &g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform);
    iVar1 = point[2].z;
    iVar2 = point[3].x;
    point = (GraphicsFixedVec3 *)&point[5].y;
    if (iVar1 < g_GeneratedTextureScratchRuntime.projectedMinX) {
      g_GeneratedTextureScratchRuntime.projectedMinX = iVar1;
    }
    if (iVar2 < g_GeneratedTextureScratchRuntime.projectedMinY) {
      g_GeneratedTextureScratchRuntime.projectedMinY = iVar2;
    }
    if (g_GeneratedTextureScratchRuntime.projectedMaxX < iVar1) {
      g_GeneratedTextureScratchRuntime.projectedMaxX = iVar1;
    }
    if (g_GeneratedTextureScratchRuntime.projectedMaxY < iVar2) {
      g_GeneratedTextureScratchRuntime.projectedMaxY = iVar2;
    }
  }
  return;
}


/* Address: 0x004CDC20.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture traverse hierarchy and accumulate projected bounds.
   Local calls: GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords.
   Cross-module calls: FixedTransform_Compose [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds
          (ModelRuntimeNode *modelNode)

{
  GraphicsFixedVec3 *pGVar1;
  GraphicsWorldCoordinateQ12 *pGVar2;
  ModelResourceHitTestAndRenderView210 *pMVar3;
  GraphicsWorldCoordinateQ12 GVar4;
  GraphicsWorldCoordinateQ12 GVar5;
  GraphicsWorldCoordinateQ12 GVar6;
  int iVar7;
  dword dVar8;
  byte *meshGroup;
  
  GVar6 = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
  GVar5 = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
  GVar4 = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  pGVar1 = &(modelNode->worldTransform).translation;
  pGVar1->x = pGVar1->x - g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  pGVar2 = &(modelNode->worldTransform).translation.y;
  *pGVar2 = *pGVar2 - GVar5;
  pGVar2 = &(modelNode->worldTransform).translation.z;
  *pGVar2 = *pGVar2 - GVar6;
  FixedTransform_Compose
            (&g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform,
             &modelNode->worldTransform,&g_AuxiliaryRotationMatrixFixed);
  pGVar1 = &(modelNode->worldTransform).translation;
  pGVar1->x = pGVar1->x + GVar4;
  pGVar2 = &(modelNode->worldTransform).translation.y;
  *pGVar2 = *pGVar2 + GVar5;
  pGVar2 = &(modelNode->worldTransform).translation.z;
  *pGVar2 = *pGVar2 + GVar6;
  pMVar3 = (modelNode->modelPayload).modelResource;
  iVar7 = *(int *)pMVar3->reservedEC_1FF;
  if (iVar7 != 0) {
    meshGroup = pMVar3->reserved00_AF + iVar7 + 0x20;
    for (iVar7 = *(int *)(pMVar3->reserved00_AF + iVar7 + 4); iVar7 != 0; iVar7 = iVar7 + -1) {
      GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords
                ((ModelMeshGroupAddress32)meshGroup);
      meshGroup = meshGroup + *(int *)meshGroup;
    }
  }
  iVar7 = 0;
  for (dVar8 = modelNode->childCount; dVar8 != 0; dVar8 = dVar8 - 1) {
    if (modelNode->childNodes[iVar7] != (ModelRuntimeNode *)0x0) {
      GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds
                (modelNode->childNodes[iVar7]);
    }
    iVar7 = iVar7 + 1;
  }
  return;
}


/* Address: 0x00485020.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture transform point xy.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_TransformPointXY
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform)

{
  int iVar1;
  longlong lVar2;
  
  lVar2 = (longlong)transform->basisRow0[1] * (longlong)point->y +
          (longlong)transform->basisRow0[0] * (longlong)point->x +
          (longlong)transform->basisRow0[2] * (longlong)point->z;
  iVar1 = transform->basisRow1[0];
  outputXY->component0 =
       ((int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c) + (transform->translation).x;
  lVar2 = (longlong)transform->basisRow1[1] * (longlong)point->y +
          (longlong)iVar1 * (longlong)point->x +
          (longlong)transform->basisRow1[2] * (longlong)point->z;
  outputXY->component1 =
       ((int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c) + (transform->translation).y;
  return;
}


/* Address: 0x004CD2B0.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture advance tile cursor.
*/
void __thandor_void_preserve_eax_ecx GraphicsShadingGeneratedTexture_AdvanceTileCursor(void)

{
  g_GraphicsShadingGeneratedTextureTileX =
       g_GraphicsShadingGeneratedTextureTileX + g_GraphicsShadingGridHalfSize;
  g_GraphicsShadingGeneratedTexturePixelCursor =
       g_GraphicsShadingGeneratedTexturePixelCursor + g_GraphicsShadingGridHalfSize;
  g_GraphicsShadingGeneratedTextureTileXQ20 =
       g_GraphicsShadingGeneratedTextureTileXQ20 + g_GraphicsShadingGridStepQ20;
  if (g_GraphicsShadingTextureDimension <= g_GraphicsShadingGeneratedTextureTileX) {
    g_GraphicsShadingGeneratedTextureTileX = 0;
    g_GraphicsShadingGeneratedTextureTileXQ20 = 0;
    g_GraphicsShadingGeneratedTextureTileY =
         g_GraphicsShadingGeneratedTextureTileY + g_GraphicsShadingGridHalfSize;
    g_GraphicsShadingGeneratedTextureTileYQ20 =
         g_GraphicsShadingGeneratedTextureTileYQ20 + g_GraphicsShadingGridStepQ20;
    g_GraphicsShadingGeneratedTexturePixelCursor =
         g_GraphicsShadingGeneratedTexturePixelCursor +
         (g_GraphicsShadingGridHalfSize * g_GraphicsShadingTextureDimension -
         g_GraphicsShadingTextureDimension);
    if (g_GraphicsShadingTextureDimension <= g_GraphicsShadingGeneratedTextureTileY) {
      g_GraphicsShadingGeneratedTextureSubresourceIndex =
           g_GraphicsShadingGeneratedTextureSubresourceIndex + 1;
      g_GraphicsShadingGeneratedTextureTileY = 0;
      g_GraphicsShadingGeneratedTextureTileYQ20 = 0;
      if (g_GraphicsShadingSubresourceCount <= g_GraphicsShadingGeneratedTextureSubresourceIndex) {
        g_GraphicsShadingGeneratedTextureCompletedTraversalCount =
             g_GraphicsShadingGeneratedTextureCompletedTraversalCount + 1;
      }
    }
  }
  return;
}


/* Address: 0x004CD3D0.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture filter grid scratch mmx.
*/
void __thandor_void_preserve_eax_ecx_edx GraphicsShadingGeneratedTexture_FilterGridScratchMmx(void)

{
  int iVar1;
  qword qVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  ulonglong *puVar6;
  undefined8 *puVar7;
  longlong *plVar8;
  ulonglong *puVar9;
  ulonglong *puVar10;
  uint uVar11;
  bool bVar12;
  undefined8 uVar13;
  ulonglong uVar14;
  undefined8 uVar15;
  ulonglong uVar16;
  undefined8 uVar17;
  ulonglong uVar18;
  undefined8 uVar19;
  ulonglong uVar20;
  undefined8 uVar21;
  undefined8 uVar22;
  undefined8 uVar23;
  undefined8 uVar24;
  
  qVar2 = g_GraphicsShadingMmxPacked3BitPerByteMask;
  uVar11 = g_GraphicsShadingGridHalfSize >> 4;
  puVar9 = (ulonglong *)
           (g_GraphicsShadingGeneratedTexturePixelCursor +
           (-(g_GraphicsShadingGridHalfSize >> 1) -
           g_GraphicsShadingTextureDimension * (g_GraphicsShadingGridHalfSize >> 1)));
  uVar4 = g_GraphicsShadingGridHalfSize;
  uVar5 = g_GraphicsShadingGridHalfSize;
  puVar6 = (ulonglong *)g_GraphicsShadingGridScratchInterior;
  puVar10 = puVar9;
  do {
    do {
      uVar14 = puVar10[1];
      uVar16 = puVar10[2];
      uVar18 = puVar10[3];
      *puVar6 = *puVar10 >> 5 & qVar2;
      puVar6[1] = uVar14 >> 5 & qVar2;
      puVar6[2] = uVar16 >> 5 & qVar2;
      puVar6[3] = uVar18 >> 5 & qVar2;
      puVar10 = puVar10 + 4;
      puVar6 = puVar6 + 4;
      bVar12 = 0x1f < uVar4;
      uVar4 = uVar4 - 0x20;
    } while (bVar12 && uVar4 != 0);
    puVar6 = (ulonglong *)((int)puVar6 + g_GraphicsShadingGridHalfSize);
    puVar10 = (ulonglong *)
              ((int)puVar10 + (g_GraphicsShadingTextureDimension - g_GraphicsShadingGridHalfSize));
    uVar5 = uVar5 - 1;
    uVar4 = g_GraphicsShadingGridHalfSize;
  } while (uVar5 != 0);
  iVar3 = g_GraphicsShadingGridHalfSize * 2 * uVar11;
  uVar5 = g_GraphicsShadingGridHalfSize;
  plVar8 = (longlong *)g_GraphicsShadingGridScratchInterior;
  do {
    do {
      uVar13 = paddusb(*plVar8 << 2,*(undefined8 *)((int)plVar8 + uVar11 * 2));
      uVar15 = paddusb(plVar8[1] << 2,*(undefined8 *)((int)plVar8 + uVar11 * 2 + 8));
      uVar17 = paddusb(plVar8[2] << 2,*(undefined8 *)((int)plVar8 + uVar11 * 2 + 0x10));
      uVar19 = paddusb(plVar8[3] << 2,*(undefined8 *)((int)plVar8 + uVar11 * 2 + 0x18));
      puVar7 = (undefined8 *)((int)plVar8 - uVar11);
      uVar21 = paddusb(*(undefined8 *)((int)plVar8 + uVar11),*puVar7);
      uVar22 = paddusb(*(undefined8 *)((int)plVar8 + uVar11 + 8),puVar7[1]);
      uVar23 = paddusb(*(undefined8 *)((int)plVar8 + uVar11 + 0x10),puVar7[2]);
      uVar24 = paddusb(*(undefined8 *)((int)plVar8 + uVar11 + 0x18),puVar7[3]);
      puVar7 = (undefined8 *)((int)puVar7 - uVar11);
      uVar13 = paddusb(uVar13,*puVar7);
      uVar15 = paddusb(uVar15,puVar7[1]);
      uVar17 = paddusb(uVar17,puVar7[2]);
      uVar19 = paddusb(uVar19,puVar7[3]);
      puVar7 = (undefined8 *)((int)puVar7 + (uVar11 * 2 - iVar3));
      uVar21 = paddusb(uVar21,*puVar7);
      uVar22 = paddusb(uVar22,puVar7[1]);
      uVar23 = paddusb(uVar23,puVar7[2]);
      uVar24 = paddusb(uVar24,puVar7[3]);
      uVar13 = paddusb(uVar13,*(undefined8 *)((int)puVar7 + uVar11));
      uVar15 = paddusb(uVar15,*(undefined8 *)((int)puVar7 + uVar11 + 8));
      uVar17 = paddusb(uVar17,*(undefined8 *)((int)puVar7 + uVar11 + 0x10));
      uVar19 = paddusb(uVar19,*(undefined8 *)((int)puVar7 + uVar11 + 0x18));
      uVar13 = paddusb(uVar13,*(undefined8 *)((int)puVar7 + uVar11 * 2));
      uVar15 = paddusb(uVar15,*(undefined8 *)((int)puVar7 + uVar11 * 2 + 8));
      uVar17 = paddusb(uVar17,*(undefined8 *)((int)puVar7 + uVar11 * 2 + 0x10));
      uVar19 = paddusb(uVar19,*(undefined8 *)((int)puVar7 + uVar11 * 2 + 0x18));
      puVar7 = (undefined8 *)((int)puVar7 - uVar11);
      uVar13 = paddusb(uVar13,*puVar7);
      uVar15 = paddusb(uVar15,puVar7[1]);
      uVar17 = paddusb(uVar17,puVar7[2]);
      uVar19 = paddusb(uVar19,puVar7[3]);
      puVar7 = (undefined8 *)((int)puVar7 - uVar11);
      uVar13 = paddusb(uVar13,*puVar7);
      uVar15 = paddusb(uVar15,puVar7[1]);
      uVar17 = paddusb(uVar17,puVar7[2]);
      uVar19 = paddusb(uVar19,puVar7[3]);
      puVar7 = (undefined8 *)((int)puVar7 + iVar3 * 2 + uVar11 * 2);
      uVar21 = paddusb(uVar21,*puVar7);
      uVar22 = paddusb(uVar22,puVar7[1]);
      uVar23 = paddusb(uVar23,puVar7[2]);
      uVar24 = paddusb(uVar24,puVar7[3]);
      uVar13 = paddusb(uVar13,*(undefined8 *)((int)puVar7 + uVar11));
      uVar15 = paddusb(uVar15,*(undefined8 *)((int)puVar7 + uVar11 + 8));
      uVar17 = paddusb(uVar17,*(undefined8 *)((int)puVar7 + uVar11 + 0x10));
      uVar19 = paddusb(uVar19,*(undefined8 *)((int)puVar7 + uVar11 + 0x18));
      uVar13 = paddusb(uVar13,*(undefined8 *)((int)puVar7 + uVar11 * 2));
      uVar15 = paddusb(uVar15,*(undefined8 *)((int)puVar7 + uVar11 * 2 + 8));
      uVar17 = paddusb(uVar17,*(undefined8 *)((int)puVar7 + uVar11 * 2 + 0x10));
      uVar19 = paddusb(uVar19,*(undefined8 *)((int)puVar7 + uVar11 * 2 + 0x18));
      iVar1 = -iVar3 - uVar11;
      uVar13 = paddusb(uVar13,*(undefined8 *)((int)puVar7 + iVar3 + iVar1));
      uVar15 = paddusb(uVar15,*(undefined8 *)((int)puVar7 + iVar3 + 8 + iVar1));
      uVar17 = paddusb(uVar17,*(undefined8 *)((int)puVar7 + iVar3 + 0x10 + iVar1));
      uVar19 = paddusb(uVar19,*(undefined8 *)((int)puVar7 + iVar3 + 0x18 + iVar1));
      iVar1 = (iVar1 - uVar11) - iVar3;
      uVar13 = paddusb(uVar13,*(undefined8 *)((int)puVar7 + iVar3 * 2 + iVar1));
      uVar15 = paddusb(uVar15,*(undefined8 *)((int)puVar7 + iVar3 * 2 + iVar1 + 8));
      uVar17 = paddusb(uVar17,*(undefined8 *)((int)puVar7 + iVar3 * 2 + iVar1 + 0x10));
      uVar19 = paddusb(uVar19,*(undefined8 *)((int)puVar7 + iVar3 * 2 + iVar1 + 0x18));
      uVar13 = paddusb(uVar13,uVar21);
      uVar15 = paddusb(uVar15,uVar22);
      uVar17 = paddusb(uVar17,uVar23);
      uVar19 = paddusb(uVar19,uVar24);
      uVar13 = paddusb(uVar13,uVar21);
      uVar15 = paddusb(uVar15,uVar22);
      uVar17 = paddusb(uVar17,uVar23);
      uVar19 = paddusb(uVar19,uVar24);
      puVar7 = (undefined8 *)((int)puVar7 + uVar11 * 2 + (iVar1 - iVar3));
      uVar13 = paddusb(uVar13,uVar21);
      uVar15 = paddusb(uVar15,uVar22);
      uVar17 = paddusb(uVar17,uVar23);
      uVar19 = paddusb(uVar19,uVar24);
      uVar13 = paddusb(uVar13,*puVar7);
      uVar15 = paddusb(uVar15,puVar7[1]);
      uVar17 = paddusb(uVar17,puVar7[2]);
      uVar19 = paddusb(uVar19,puVar7[3]);
      uVar13 = paddusb(uVar13,*(undefined8 *)((int)puVar7 + uVar11));
      uVar15 = paddusb(uVar15,*(undefined8 *)((int)puVar7 + uVar11 + 8));
      uVar17 = paddusb(uVar17,*(undefined8 *)((int)puVar7 + uVar11 + 0x10));
      uVar19 = paddusb(uVar19,*(undefined8 *)((int)puVar7 + uVar11 + 0x18));
      uVar13 = paddusb(uVar13,*(undefined8 *)((int)puVar7 + iVar3 * 4));
      uVar15 = paddusb(uVar15,*(undefined8 *)((int)puVar7 + iVar3 * 4 + 8));
      uVar17 = paddusb(uVar17,*(undefined8 *)((int)puVar7 + iVar3 * 4 + 0x10));
      uVar19 = paddusb(uVar19,*(undefined8 *)((int)puVar7 + iVar3 * 4 + 0x18));
      puVar7 = (undefined8 *)((int)puVar7 - uVar11);
      uVar13 = paddusb(uVar13,*puVar7);
      uVar15 = paddusb(uVar15,puVar7[1]);
      uVar17 = paddusb(uVar17,puVar7[2]);
      uVar19 = paddusb(uVar19,puVar7[3]);
      uVar13 = paddusb(uVar13,*(undefined8 *)((int)puVar7 + iVar3 * 4));
      uVar15 = paddusb(uVar15,*(undefined8 *)((int)puVar7 + iVar3 * 4 + 8));
      uVar17 = paddusb(uVar17,*(undefined8 *)((int)puVar7 + iVar3 * 4 + 0x10));
      uVar19 = paddusb(uVar19,*(undefined8 *)((int)puVar7 + iVar3 * 4 + 0x18));
      uVar14 = paddusb(uVar13,*(undefined8 *)((int)puVar7 + iVar3 * 4 + uVar11 * 2));
      uVar16 = paddusb(uVar15,*(undefined8 *)((int)puVar7 + iVar3 * 4 + uVar11 * 2 + 8));
      uVar18 = paddusb(uVar17,*(undefined8 *)((int)puVar7 + iVar3 * 4 + uVar11 * 2 + 0x10));
      uVar20 = paddusb(uVar19,*(undefined8 *)((int)puVar7 + iVar3 * 4 + uVar11 * 2 + 0x18));
      *puVar9 = uVar14;
      puVar9[1] = uVar16;
      puVar9[2] = uVar18;
      puVar9[3] = uVar20;
      plVar8 = (longlong *)((int)puVar7 + iVar3 * 2 + uVar11 + 0x20);
      puVar9 = puVar9 + 4;
      bVar12 = 0x1f < uVar4;
      uVar4 = uVar4 - 0x20;
    } while (bVar12 && uVar4 != 0);
    plVar8 = (longlong *)((int)plVar8 + g_GraphicsShadingGridHalfSize);
    puVar9 = (ulonglong *)
             ((int)puVar9 + (g_GraphicsShadingTextureDimension - g_GraphicsShadingGridHalfSize));
    uVar5 = uVar5 - 1;
    uVar4 = g_GraphicsShadingGridHalfSize;
  } while (uVar5 != 0);
  return;
}


/* Address: 0x004CDCE0.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture probe hierarchy for geometry.
*/
bool __thandor_void_preserve_eax_ecx
GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(ModelRuntimeNode *modelNode)

{
  dword dVar1;
  dword dVar2;
  int iVar3;
  bool bVar4;
  
  if (*(int *)((modelNode->modelPayload).modelResource)->reservedEC_1FF == 0) {
    dVar2 = modelNode->childCount;
    do {
      dVar1 = dVar2 - 1;
      if ((int)dVar1 < 0) {
        return true;
      }
      iVar3 = dVar2 - 1;
      dVar2 = dVar1;
    } while ((modelNode->childNodes[iVar3] == (ModelRuntimeNode *)0x0) ||
            (bVar4 = GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry
                               (modelNode->childNodes[iVar3]), bVar4));
  }
  return false;
}


/* Address: 0x004D1060.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture reserve fourteen projected point blocks.
*/
GraphicsProjectedBlockEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks
          (GeneratedTextureRenderContextView *renderContext)

{
  uint *puVar1;
  uint uVar2;
  GraphicsProjectedBlockEaxCf5 GVar3;
  GraphicsProjectedBlockEaxCf5 GVar4;
  
  puVar1 = renderContext->projectedPointBlockPool;
  uVar2 = puVar1[1];
  GVar4.firstBlock = (void *)(uVar2 + 0xe);
  if (GVar4.firstBlock < (void *)*puVar1) {
    puVar1[1] = (uint)GVar4.firstBlock;
    GVar3.firstBlock = uVar2 * 0x80 + puVar1[2];
    puVar1[uVar2 * 4 + 9] = GVar3.firstBlock;
    puVar1[uVar2 * 4 + 0xd] = GVar3.firstBlock + 0x80;
    puVar1[uVar2 * 4 + 0x11] = GVar3.firstBlock + 0x100;
    puVar1[uVar2 * 4 + 0x15] = GVar3.firstBlock + 0x180;
    puVar1[uVar2 * 4 + 0x19] = GVar3.firstBlock + 0x200;
    puVar1[uVar2 * 4 + 0x1d] = GVar3.firstBlock + 0x280;
    puVar1[uVar2 * 4 + 0x21] = GVar3.firstBlock + 0x300;
    puVar1[uVar2 * 4 + 0x25] = GVar3.firstBlock + 0x380;
    puVar1[uVar2 * 4 + 0x29] = GVar3.firstBlock + 0x400;
    puVar1[uVar2 * 4 + 0x2d] = GVar3.firstBlock + 0x480;
    puVar1[uVar2 * 4 + 0x31] = GVar3.firstBlock + 0x500;
    puVar1[uVar2 * 4 + 0x35] = GVar3.firstBlock + 0x580;
    puVar1[uVar2 * 4 + 0x39] = GVar3.firstBlock + 0x600;
    puVar1[uVar2 * 4 + 0x3d] = GVar3.firstBlock + 0x680;
    *(undefined4 *)(GVar3.firstBlock + 0x60) = 0;
    *(undefined4 *)(GVar3.firstBlock + 0x68) = 0x11000;
    GVar3.carry = false;
    return GVar3;
  }
  GVar4.carry = true;
  return GVar4;
}


/* Address: 0x004D1150.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture rollback fourteen projected point blocks.
*/
void GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks
               (GeneratedTextureRenderContextView *renderContext)

{
  renderContext->projectedPointBlockPool[1] = renderContext->projectedPointBlockPool[1] - 0xe;
  return;
}


/* Address: 0x00484FA0.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture transform point xyquantized.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_TransformPointXYQuantized
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform)

{
  int iVar1;
  longlong lVar2;
  
  lVar2 = (longlong)transform->basisRow0[1] * (longlong)point->y +
          (longlong)transform->basisRow0[0] * (longlong)point->x +
          (longlong)transform->basisRow0[2] * (longlong)point->z;
  iVar1 = transform->basisRow1[0];
  outputXY->component0 =
       ((int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc) + (transform->translation).x &
       0xfffff000;
  lVar2 = (longlong)transform->basisRow1[1] * (longlong)point->y +
          (longlong)iVar1 * (longlong)point->x +
          (longlong)transform->basisRow1[2] * (longlong)point->z;
  outputXY->component1 =
       ((int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc) + (transform->translation).y &
       0xfffff000;
  return;
}


/* Address: 0x00485320.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture compose transform.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_ComposeTransform
          (GraphicsFixedMatrix3x4 *outTransform,GraphicsFixedMatrix3x4 *rhsTransform,
          GraphicsFixedMatrix3x4 *lhsTransform)

{
  int iVar1;
  longlong lVar2;
  
  lVar2 = (longlong)lhsTransform->basisRow0[1] * (longlong)rhsTransform->basisRow1[0] +
          (longlong)lhsTransform->basisRow0[0] * (longlong)rhsTransform->basisRow0[0] +
          (longlong)lhsTransform->basisRow0[2] * (longlong)rhsTransform->basisRow2[0];
  iVar1 = lhsTransform->basisRow0[0];
  outTransform->basisRow0[0] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)lhsTransform->basisRow0[1] * (longlong)rhsTransform->basisRow1[1] +
          (longlong)iVar1 * (longlong)rhsTransform->basisRow0[1] +
          (longlong)lhsTransform->basisRow0[2] * (longlong)rhsTransform->basisRow2[1];
  iVar1 = lhsTransform->basisRow0[0];
  outTransform->basisRow0[1] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)lhsTransform->basisRow0[1] * (longlong)rhsTransform->basisRow1[2] +
          (longlong)iVar1 * (longlong)rhsTransform->basisRow0[2] +
          (longlong)lhsTransform->basisRow0[2] * (longlong)rhsTransform->basisRow2[2];
  iVar1 = lhsTransform->basisRow0[0];
  outTransform->basisRow0[2] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)lhsTransform->basisRow0[1] * (longlong)(rhsTransform->translation).y +
          (longlong)iVar1 * (longlong)(rhsTransform->translation).x +
          (longlong)lhsTransform->basisRow0[2] * (longlong)(rhsTransform->translation).z;
  iVar1 = lhsTransform->basisRow1[0];
  (outTransform->translation).x =
       ((int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc) +
       (lhsTransform->translation).x;
  lVar2 = (longlong)lhsTransform->basisRow1[1] * (longlong)rhsTransform->basisRow1[0] +
          (longlong)iVar1 * (longlong)rhsTransform->basisRow0[0] +
          (longlong)lhsTransform->basisRow1[2] * (longlong)rhsTransform->basisRow2[0];
  iVar1 = lhsTransform->basisRow1[0];
  outTransform->basisRow1[0] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)lhsTransform->basisRow1[1] * (longlong)rhsTransform->basisRow1[1] +
          (longlong)iVar1 * (longlong)rhsTransform->basisRow0[1] +
          (longlong)lhsTransform->basisRow1[2] * (longlong)rhsTransform->basisRow2[1];
  iVar1 = lhsTransform->basisRow1[0];
  outTransform->basisRow1[1] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)lhsTransform->basisRow1[1] * (longlong)rhsTransform->basisRow1[2] +
          (longlong)iVar1 * (longlong)rhsTransform->basisRow0[2] +
          (longlong)lhsTransform->basisRow1[2] * (longlong)rhsTransform->basisRow2[2];
  iVar1 = lhsTransform->basisRow1[0];
  outTransform->basisRow1[2] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)lhsTransform->basisRow1[1] * (longlong)(rhsTransform->translation).y +
          (longlong)iVar1 * (longlong)(rhsTransform->translation).x +
          (longlong)lhsTransform->basisRow1[2] * (longlong)(rhsTransform->translation).z;
  iVar1 = lhsTransform->basisRow2[0];
  (outTransform->translation).y =
       ((int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc) +
       (lhsTransform->translation).y;
  lVar2 = (longlong)lhsTransform->basisRow2[1] * (longlong)rhsTransform->basisRow1[0] +
          (longlong)iVar1 * (longlong)rhsTransform->basisRow0[0] +
          (longlong)lhsTransform->basisRow2[2] * (longlong)rhsTransform->basisRow2[0];
  iVar1 = lhsTransform->basisRow2[0];
  outTransform->basisRow2[0] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)lhsTransform->basisRow2[1] * (longlong)rhsTransform->basisRow1[1] +
          (longlong)iVar1 * (longlong)rhsTransform->basisRow0[1] +
          (longlong)lhsTransform->basisRow2[2] * (longlong)rhsTransform->basisRow2[1];
  iVar1 = lhsTransform->basisRow2[0];
  outTransform->basisRow2[1] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)lhsTransform->basisRow2[1] * (longlong)rhsTransform->basisRow1[2] +
          (longlong)iVar1 * (longlong)rhsTransform->basisRow0[2] +
          (longlong)lhsTransform->basisRow2[2] * (longlong)rhsTransform->basisRow2[2];
  iVar1 = lhsTransform->basisRow2[0];
  outTransform->basisRow2[2] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)lhsTransform->basisRow2[1] * (longlong)(rhsTransform->translation).y +
          (longlong)iVar1 * (longlong)(rhsTransform->translation).x +
          (longlong)lhsTransform->basisRow2[2] * (longlong)(rhsTransform->translation).z;
  (outTransform->translation).z =
       ((int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc) +
       (lhsTransform->translation).z;
  return;
}


/* Address: 0x004CD690.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture rasterize triangle mask.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_RasterizeTriangleMask
          (GraphicsFixedVec2 *vertexA,GraphicsFixedVec2 *vertexB,GraphicsFixedVec2 *vertexC)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  sdword sVar8;
  int iVar9;
  GraphicsFixedVec2 *pGVar10;
  GraphicsFixedVec2 *pGVar11;
  GraphicsFixedVec2 *pGVar12;
  byte *pbVar13;
  byte *pbVar14;
  sdword sVar15;
  int iVar16;
  sdword sVar17;
  
  iVar16 = vertexC->component1;
  iVar7 = vertexB->component1;
  iVar6 = vertexA->component1;
  iVar3 = iVar16;
  pGVar12 = vertexB;
  if (iVar7 < iVar16) {
    iVar3 = iVar7;
    iVar7 = iVar16;
    pGVar12 = vertexC;
    vertexC = vertexB;
  }
  iVar16 = iVar3;
  pGVar10 = vertexA;
  if (iVar6 < iVar3) {
    iVar16 = iVar6;
    iVar6 = iVar3;
    pGVar10 = vertexC;
    vertexC = vertexA;
  }
  iVar3 = iVar7;
  pGVar11 = pGVar10;
  if (iVar6 < iVar7) {
    iVar3 = iVar6;
    iVar6 = iVar7;
    pGVar11 = pGVar12;
    pGVar12 = pGVar10;
  }
  if (iVar16 < g_GraphicsShadingNegativeGridOriginQ12) {
    if (iVar3 < g_GraphicsShadingNegativeGridOriginQ12) {
      iVar3 = g_GraphicsShadingNegativeGridOriginQ12;
    }
    iVar16 = g_GraphicsShadingNegativeGridOriginQ12;
    if (iVar6 < g_GraphicsShadingNegativeGridOriginQ12) {
      return;
    }
  }
  if (g_GraphicsShadingPositiveGridOriginQ12 < iVar6) {
    if (g_GraphicsShadingPositiveGridOriginQ12 < iVar3) {
      iVar3 = g_GraphicsShadingPositiveGridOriginQ12;
    }
    iVar6 = g_GraphicsShadingPositiveGridOriginQ12;
    if (g_GraphicsShadingPositiveGridOriginQ12 < iVar16) {
      return;
    }
  }
  iVar7 = iVar6 - iVar16 >> 0xc;
  if (iVar7 != 0) {
    iVar2 = iVar3 - iVar16 >> 0xc;
    iVar6 = vertexC->component0;
    iVar3 = pGVar12->component0;
    iVar9 = pGVar11->component0;
    sVar15 = g_GraphicsShadingNegativeGridOriginQ12;
    if ((g_GraphicsShadingNegativeGridOriginQ12 <= iVar6) &&
       (sVar15 = iVar6, g_GraphicsShadingPositiveGridOriginQ12 < iVar6)) {
      sVar15 = g_GraphicsShadingPositiveGridOriginQ12;
    }
    sVar17 = g_GraphicsShadingNegativeGridOriginQ12;
    if ((g_GraphicsShadingNegativeGridOriginQ12 <= iVar3) &&
       (sVar17 = iVar3, g_GraphicsShadingPositiveGridOriginQ12 < iVar3)) {
      sVar17 = g_GraphicsShadingPositiveGridOriginQ12;
    }
    sVar8 = g_GraphicsShadingNegativeGridOriginQ12;
    if ((g_GraphicsShadingNegativeGridOriginQ12 <= iVar9) &&
       (sVar8 = iVar9, g_GraphicsShadingPositiveGridOriginQ12 < iVar9)) {
      sVar8 = g_GraphicsShadingPositiveGridOriginQ12;
    }
    iVar3 = sVar17 - sVar15;
    iVar9 = sVar8 - sVar15;
    pbVar13 = g_GraphicsShadingGeneratedTexturePixelCursor +
              (iVar16 >> 0xc) * g_GraphicsShadingTextureDimension;
    iVar6 = iVar9 / iVar7;
    iVar16 = sVar15;
    if (iVar2 != 0) {
      iVar1 = iVar3 / iVar2;
      do {
        pbVar14 = pbVar13 + (sVar15 >> 0xc);
        iVar4 = (iVar16 >> 0xc) - (sVar15 >> 0xc);
        if (iVar4 != 0) {
          iVar5 = iVar4;
          if (iVar4 < 0) {
            iVar5 = -iVar4;
            pbVar14 = pbVar14 + iVar4;
          }
          for (; iVar5 != 0; iVar5 = iVar5 + -1) {
            *pbVar14 = 0xff;
            pbVar14 = pbVar14 + 1;
          }
        }
        pbVar13 = pbVar13 + g_GraphicsShadingTextureDimension;
        sVar15 = sVar15 + iVar1;
        iVar16 = iVar16 + iVar6;
        iVar7 = iVar7 - (int)g_GraphicsShadingRasterizeMmxPackedDwordOneZero;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
    }
    if (iVar7 != 0) {
      iVar3 = (iVar9 - iVar3) / iVar7;
      do {
        pbVar14 = pbVar13 + (sVar17 >> 0xc);
        iVar9 = (iVar16 >> 0xc) - (sVar17 >> 0xc);
        if (iVar9 != 0) {
          iVar2 = iVar9;
          if (iVar9 < 0) {
            iVar2 = -iVar9;
            pbVar14 = pbVar14 + iVar9;
          }
          for (; iVar2 != 0; iVar2 = iVar2 + -1) {
            *pbVar14 = 0xff;
            pbVar14 = pbVar14 + 1;
          }
        }
        pbVar13 = pbVar13 + g_GraphicsShadingTextureDimension;
        sVar17 = sVar17 + iVar3;
        iVar16 = iVar16 + iVar6;
        iVar7 = iVar7 + -1;
      } while (iVar7 != 0);
    }
  }
  return;
}

