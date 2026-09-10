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
void GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy(int param_1,int param_2)

{
  GraphicsProjectedPointEdxEax8 GVar1;
  GraphicsProjectedPointEdxEax8 GVar2;
  GraphicsProjectedPointEdxEax8 GVar3;
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
  dword dVar18;
  sdword sVar19;
  int iVar20;
  Q12 QVar21;
  FixedMathScale32 FVar22;
  GraphicsProjectedPointEdxEax8 *pGVar23;
  uint uVar24;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int iVar25;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  AngleTurn32 azimuthAngle;
  int extraout_ECX_06;
  int extraout_ECX_07;
  int extraout_ECX_08;
  int extraout_ECX_09;
  int extraout_ECX_10;
  int extraout_ECX_11;
  AngleTurn32 azimuthAngle_00;
  int extraout_ECX_12;
  int extraout_ECX_13;
  int extraout_ECX_14;
  int extraout_ECX_15;
  int extraout_ECX_16;
  int extraout_ECX_17;
  AngleTurn32 azimuthAngle_01;
  int extraout_ECX_18;
  int extraout_ECX_19;
  int extraout_ECX_20;
  int extraout_ECX_21;
  int extraout_ECX_22;
  int extraout_ECX_23;
  AngleTurn32 azimuthAngle_02;
  int extraout_ECX_24;
  int extraout_ECX_25;
  int extraout_ECX_26;
  int extraout_ECX_27;
  int extraout_ECX_28;
  int extraout_ECX_29;
  AngleTurn32 azimuthAngle_03;
  int extraout_ECX_30;
  int extraout_ECX_31;
  int extraout_ECX_32;
  int extraout_ECX_33;
  int extraout_ECX_34;
  int extraout_ECX_35;
  AngleTurn32 azimuthAngle_04;
  int extraout_ECX_36;
  int extraout_ECX_37;
  int extraout_ECX_38;
  int extraout_ECX_39;
  int extraout_ECX_40;
  int extraout_ECX_41;
  AngleTurn32 azimuthAngle_05;
  int extraout_ECX_42;
  int extraout_ECX_43;
  int extraout_ECX_44;
  int extraout_ECX_45;
  int extraout_ECX_46;
  int extraout_ECX_47;
  AngleTurn32 azimuthAngle_06;
  int extraout_ECX_48;
  int extraout_ECX_49;
  int extraout_ECX_50;
  int extraout_ECX_51;
  int extraout_ECX_52;
  int extraout_ECX_53;
  AngleTurn32 azimuthAngle_07;
  int extraout_ECX_54;
  int extraout_ECX_55;
  int extraout_ECX_56;
  int extraout_ECX_57;
  int extraout_ECX_58;
  int extraout_ECX_59;
  AngleTurn32 azimuthAngle_08;
  int extraout_ECX_60;
  int extraout_ECX_61;
  int extraout_ECX_62;
  int extraout_ECX_63;
  int extraout_ECX_64;
  int extraout_ECX_65;
  AngleTurn32 azimuthAngle_09;
  int extraout_ECX_66;
  int extraout_ECX_67;
  int extraout_ECX_68;
  int extraout_ECX_69;
  int extraout_ECX_70;
  int extraout_ECX_71;
  AngleTurn32 azimuthAngle_10;
  int extraout_ECX_72;
  int extraout_ECX_73;
  int extraout_ECX_74;
  int iVar26;
  uint uVar27;
  int iVar28;
  uint uVar29;
  bool bVar30;
  undefined1 uVar31;
  undefined1 uVar32;
  undefined8 uVar33;
  undefined8 uVar34;
  undefined8 uVar35;
  undefined8 uVar36;
  FixedDirectionXZEdxEax8 FVar37;
  ulonglong uVar38;
  GraphicsProjectedPointEdxEax8 GVar39;
  
  if (DAT_004cce3c == 0) {
    DAT_004ccfe0 = *(int *)(param_1 + 0x94);
    DAT_004ccfe4 = *(int *)(param_1 + 0x98);
    DAT_004ccfe8 = *(uint *)(param_1 + 0x9c);
    iVar20 = *(int *)(param_1 + 0x54);
    g_ModelCullViewRelativeX = DAT_004ccfe0 - g_ViewOriginFixed.x;
    g_ModelCullViewRelativeY = DAT_004ccfe4 - g_ViewOriginFixed.y;
    bVar30 = DAT_004ccfe8 < (uint)g_ViewOriginFixed.z;
    g_ModelCullViewRelativeZ = DAT_004ccfe8 - g_ViewOriginFixed.z;
    GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(param_1);
    if (!bVar30) {
      uVar29 = (uint)(iVar20 * 9) >> 2;
      sVar19 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0,
                                (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
      if ((((sVar19 <= (int)uVar29) &&
           (sVar19 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 1,
                                      (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX),
           sVar19 <= (int)uVar29)) &&
          (sVar19 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 2,
                                     (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX),
          sVar19 <= (int)uVar29)) &&
         (sVar19 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 3,
                                    (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX),
         sVar19 <= (int)uVar29)) {
        FixedTransform_ApplyPoint
                  ((GraphicsFixedVec3 *)&g_ModelCullViewRelativeX,(GraphicsFixedVec3 *)&DAT_004ccfe0
                   ,&g_ViewProjectionMatrixFixed);
        if (((int)g_ProjectionScaleFixed < g_ModelCullViewRelativeZ) &&
           (*(int *)(*(int *)(param_1 + 0x40) + 0xd8) <
            (int)(g_ModelCullViewRelativeZ - g_ProjectionScaleFixed))) {
          DAT_004cce48 = 0x7fffffff;
          DAT_004cce4c = 0x7fffffff;
          DAT_004cce50 = -0x7fffffff;
          DAT_004cce54 = -0x7fffffff;
          GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(param_1);
          FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                             (0,*(int *)(param_2 + 0xb8) + 0x4000U & 0xffff,
                              DAT_004cce48 + DAT_004cce50 >> 1);
          DAT_004ccfe0 = DAT_004ccfe0 - (int)FVar37;
          DAT_004ccfe4 = DAT_004ccfe4 - extraout_ECX;
          DAT_004ccfe8 = DAT_004ccfe8 - (int)(FVar37 >> 0x20);
          FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                             (*(int *)(param_2 + 0xbc) + 0x4000,*(AngleTurn32 *)(param_2 + 0xb8),
                              DAT_004cce4c + DAT_004cce54 >> 1);
          DAT_004ccfe0 = DAT_004ccfe0 - (int)FVar37;
          DAT_004ccfe4 = DAT_004ccfe4 - extraout_ECX_00;
          DAT_004ccfe8 = DAT_004ccfe8 - (int)(FVar37 >> 0x20);
          DAT_004ccec0 = DAT_004ccfe0;
          DAT_004ccec4 = DAT_004ccfe4;
          DAT_004ccec8 = DAT_004ccfe8;
          DAT_004cced8 = DAT_004ccfe0;
          DAT_004ccedc = DAT_004ccfe4;
          DAT_004ccee0 = DAT_004ccfe8;
          DAT_004ccef0 = DAT_004ccfe0;
          DAT_004ccef4 = DAT_004ccfe4;
          DAT_004ccef8 = DAT_004ccfe8;
          DAT_004ccf08 = DAT_004ccfe0;
          DAT_004ccf0c = DAT_004ccfe4;
          DAT_004ccf10 = DAT_004ccfe8;
          DAT_004ccf20 = DAT_004ccfe0;
          DAT_004ccf24 = DAT_004ccfe4;
          DAT_004ccf28 = DAT_004ccfe8;
          DAT_004ccf38 = DAT_004ccfe0;
          DAT_004ccf3c = DAT_004ccfe4;
          DAT_004ccf40 = DAT_004ccfe8;
          DAT_004ccf50 = DAT_004ccfe0;
          DAT_004ccf54 = DAT_004ccfe4;
          DAT_004ccf58 = DAT_004ccfe8;
          DAT_004ccf68 = DAT_004ccfe0;
          DAT_004ccf6c = DAT_004ccfe4;
          DAT_004ccf70 = DAT_004ccfe8;
          DAT_004ccf80 = DAT_004ccfe0;
          DAT_004ccf84 = DAT_004ccfe4;
          DAT_004ccf88 = DAT_004ccfe8;
          DAT_004ccf98 = DAT_004ccfe0;
          DAT_004ccf9c = DAT_004ccfe4;
          DAT_004ccfa0 = DAT_004ccfe8;
          DAT_004ccfb0 = DAT_004ccfe0;
          DAT_004ccfb4 = DAT_004ccfe4;
          DAT_004ccfb8 = DAT_004ccfe8;
          DAT_004ccfc8 = DAT_004ccfe0;
          DAT_004ccfcc = DAT_004ccfe4;
          DAT_004ccfd0 = DAT_004ccfe8;
          iVar20 = (int)(CONCAT44(g_GraphicsShadingGridHalfSize - DAT_004cce58 >> 8,
                                  (g_GraphicsShadingGridHalfSize - DAT_004cce58) * 0x1000000) /
                        (ulonglong)(uint)(DAT_004cce50 - DAT_004cce48));
          DAT_004cce90 = (int)((ulonglong)
                               ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[0] *
                               (longlong)iVar20) >> 0x20) << 4 |
                         (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[0] *
                               (longlong)iVar20) >> 0x1c;
          DAT_004cce94 = (int)((ulonglong)
                               ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[1] *
                               (longlong)iVar20) >> 0x20) << 4 |
                         (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[1] *
                               (longlong)iVar20) >> 0x1c;
          DAT_004cce98 = (int)((ulonglong)
                               ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[2] *
                               (longlong)iVar20) >> 0x20) << 4 |
                         (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow0[2] *
                               (longlong)iVar20) >> 0x1c;
          DAT_004ccea8 = 0;
          DAT_004cceac = 0;
          DAT_004cceb0 = 0;
          iVar20 = (int)(CONCAT44(g_GraphicsShadingGridHalfSize - DAT_004cce58 >> 8,
                                  (g_GraphicsShadingGridHalfSize - DAT_004cce58) * 0x1000000) /
                        (ulonglong)(uint)(DAT_004cce54 - DAT_004cce4c));
          DAT_004cce9c = (int)((ulonglong)
                               ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[0] *
                               (longlong)iVar20) >> 0x20) << 4 |
                         (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[0] *
                               (longlong)iVar20) >> 0x1c;
          DAT_004ccea0 = (int)((ulonglong)
                               ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[1] *
                               (longlong)iVar20) >> 0x20) << 4 |
                         (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[1] *
                               (longlong)iVar20) >> 0x1c;
          DAT_004ccea4 = (int)((ulonglong)
                               ((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[2] *
                               (longlong)iVar20) >> 0x20) << 4 |
                         (uint)((longlong)g_AuxiliaryRotationMatrixFixed.basisRow1[2] *
                               (longlong)iVar20) >> 0x1c;
          DAT_004cceb4 = 0;
          DAT_004cceb8 = 0;
          DAT_004ccebc = 0;
          FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                             (0,*(int *)(param_2 + 0xb8) + 0x4000U & 0xffff,
                              DAT_004cce50 - DAT_004cce48 >> 1);
          iVar26 = (int)(FVar37 >> 0x20);
          iVar20 = (int)FVar37;
          DAT_004ccec0 = DAT_004ccec0 + iVar20;
          DAT_004ccec4 = DAT_004ccec4 + extraout_ECX_01;
          DAT_004ccec8 = DAT_004ccec8 + iVar26;
          DAT_004cced8 = DAT_004cced8 + iVar20;
          DAT_004ccedc = DAT_004ccedc + extraout_ECX_01;
          DAT_004ccee0 = DAT_004ccee0 + iVar26;
          DAT_004ccef0 = DAT_004ccef0 - iVar20;
          DAT_004ccef4 = DAT_004ccef4 - extraout_ECX_01;
          DAT_004ccef8 = DAT_004ccef8 - iVar26;
          DAT_004ccf08 = DAT_004ccf08 - iVar20;
          DAT_004ccf0c = DAT_004ccf0c - extraout_ECX_01;
          DAT_004ccf10 = DAT_004ccf10 - iVar26;
          DAT_004ccf20 = DAT_004ccf20 + iVar20;
          DAT_004ccf24 = DAT_004ccf24 + extraout_ECX_01;
          DAT_004ccf28 = DAT_004ccf28 + iVar26;
          DAT_004ccf38 = DAT_004ccf38 - iVar20;
          DAT_004ccf3c = DAT_004ccf3c - extraout_ECX_01;
          DAT_004ccf40 = DAT_004ccf40 - iVar26;
          iVar20 = iVar20 >> 1;
          iVar25 = extraout_ECX_01 >> 1;
          iVar26 = iVar26 >> 1;
          DAT_004ccf80 = DAT_004ccf80 + iVar20;
          DAT_004ccf84 = DAT_004ccf84 + iVar25;
          DAT_004ccf88 = DAT_004ccf88 + iVar26;
          DAT_004ccf98 = DAT_004ccf98 + iVar20;
          DAT_004ccf9c = DAT_004ccf9c + iVar25;
          DAT_004ccfa0 = DAT_004ccfa0 + iVar26;
          DAT_004ccfb0 = DAT_004ccfb0 - iVar20;
          DAT_004ccfb4 = DAT_004ccfb4 - iVar25;
          DAT_004ccfb8 = DAT_004ccfb8 - iVar26;
          DAT_004ccfc8 = DAT_004ccfc8 - iVar20;
          DAT_004ccfcc = DAT_004ccfcc - iVar25;
          DAT_004ccfd0 = DAT_004ccfd0 - iVar26;
          FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                             (*(int *)(param_2 + 0xbc) + 0x4000,*(AngleTurn32 *)(param_2 + 0xb8),
                              DAT_004cce54 - DAT_004cce4c >> 1);
          iVar26 = (int)(FVar37 >> 0x20);
          iVar20 = (int)FVar37;
          DAT_004ccec0 = DAT_004ccec0 + iVar20;
          DAT_004ccec4 = DAT_004ccec4 + extraout_ECX_02;
          DAT_004ccec8 = DAT_004ccec8 + iVar26;
          DAT_004cced8 = DAT_004cced8 - iVar20;
          DAT_004ccedc = DAT_004ccedc - extraout_ECX_02;
          DAT_004ccee0 = DAT_004ccee0 - iVar26;
          DAT_004ccef0 = DAT_004ccef0 + iVar20;
          DAT_004ccef4 = DAT_004ccef4 + extraout_ECX_02;
          DAT_004ccef8 = DAT_004ccef8 + iVar26;
          DAT_004ccf08 = DAT_004ccf08 - iVar20;
          DAT_004ccf0c = DAT_004ccf0c - extraout_ECX_02;
          DAT_004ccf10 = DAT_004ccf10 - iVar26;
          DAT_004ccf50 = DAT_004ccf50 + iVar20;
          DAT_004ccf54 = DAT_004ccf54 + extraout_ECX_02;
          DAT_004ccf58 = DAT_004ccf58 + iVar26;
          DAT_004ccf68 = DAT_004ccf68 - iVar20;
          DAT_004ccf6c = DAT_004ccf6c - extraout_ECX_02;
          DAT_004ccf70 = DAT_004ccf70 - iVar26;
          iVar20 = iVar20 >> 1;
          iVar25 = extraout_ECX_02 >> 1;
          iVar26 = iVar26 >> 1;
          DAT_004ccf80 = DAT_004ccf80 + iVar20;
          DAT_004ccf84 = DAT_004ccf84 + iVar25;
          DAT_004ccf88 = DAT_004ccf88 + iVar26;
          DAT_004ccf98 = DAT_004ccf98 - iVar20;
          DAT_004ccf9c = DAT_004ccf9c - iVar25;
          DAT_004ccfa0 = DAT_004ccfa0 - iVar26;
          DAT_004ccfb0 = DAT_004ccfb0 + iVar20;
          DAT_004ccfb4 = DAT_004ccfb4 + iVar25;
          DAT_004ccfb8 = DAT_004ccfb8 + iVar26;
          DAT_004ccfc8 = DAT_004ccfc8 - iVar20;
          DAT_004ccfcc = DAT_004ccfcc - iVar25;
          DAT_004ccfd0 = DAT_004ccfd0 - iVar26;
          DAT_004ccee4 = g_GraphicsShadingGridStepQ20Current - 0x1000;
          DAT_004ccf2c = (int)g_GraphicsShadingGridStepQ20Current >> 1;
          DAT_004ccecc = 0;
          DAT_004ccefc = 0;
          DAT_004ccf5c = 0;
          DAT_004ccf8c = DAT_004ccf2c - ((int)g_GraphicsShadingGridStepQ20Current >> 2);
          DAT_004ccfa4 = ((int)g_GraphicsShadingGridStepQ20Current >> 2) + DAT_004ccf2c;
          DAT_004ccf14 = DAT_004ccee4;
          DAT_004ccf44 = DAT_004ccf2c;
          DAT_004ccf74 = DAT_004ccee4;
          DAT_004ccfbc = DAT_004ccf8c;
          DAT_004ccfd4 = DAT_004ccfa4;
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccec4,DAT_004ccec0,*(FieldGridAsset **)(param_2 + 0x54));
          iVar20 = DAT_004ccecc;
          uVar29 = QVar21 - extraout_ECX_03;
          if (QVar21 < extraout_ECX_03) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccec8,DAT_004ccec4,DAT_004ccec0,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004cced4 = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccec0 = DAT_004ccec0 + (int)FVar37;
              DAT_004ccec4 = DAT_004ccec4 + extraout_ECX_04;
              DAT_004ccec8 = DAT_004ccec8 + (int)(FVar37 >> 0x20);
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccecc == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccecc) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004cced4 = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004ccec0 = DAT_004ccec0 + (int)FVar37;
              DAT_004ccec4 = DAT_004ccec4 + extraout_ECX_05;
              DAT_004ccec8 = DAT_004ccec8 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccec8,
                                  DAT_004ccec4,DAT_004ccec0,*(FieldGridAsset **)(param_2 + 0x54));
              FVar22 = (FixedMathScale32)uVar38;
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle,FVar22);
              DAT_004ccec0 = DAT_004ccec0 + (int)FVar37;
              DAT_004ccec4 = DAT_004ccec4 + extraout_ECX_06;
              DAT_004ccec8 = DAT_004ccec8 + (int)(FVar37 >> 0x20);
              DAT_004ccecc = DAT_004ccecc +
                             ((int)(((longlong)FVar22 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current < DAT_004ccecc) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004cced4 = 0;
            iVar25 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccecc = DAT_004ccecc - iVar25;
            if (iVar20 < iVar25) {
              DAT_004ccecc = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004ccec0 = DAT_004ccec0 - (int)FVar37;
              DAT_004ccec4 = DAT_004ccec4 - extraout_ECX_07;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccec8 = DAT_004ccec8 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccec8,DAT_004ccec4,DAT_004ccec0,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccec0 = DAT_004ccec0 + (int)FVar37;
              DAT_004ccec4 = DAT_004ccec4 + extraout_ECX_08;
              DAT_004ccec8 = DAT_004ccec8 + (int)(FVar37 >> 0x20);
            }
            else {
              DAT_004ccec8 = DAT_004ccec8 + uVar29;
            }
          }
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccef4,DAT_004ccef0,*(FieldGridAsset **)(param_2 + 0x54));
          iVar20 = DAT_004ccefc;
          uVar29 = QVar21 - extraout_ECX_09;
          if (QVar21 < extraout_ECX_09) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccef8,DAT_004ccef4,DAT_004ccef0,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004ccf04 = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccef0 = DAT_004ccef0 + (int)FVar37;
              DAT_004ccef4 = DAT_004ccef4 + extraout_ECX_10;
              DAT_004ccef8 = DAT_004ccef8 + (int)(FVar37 >> 0x20);
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccefc == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccefc) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004ccf04 = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004ccef0 = DAT_004ccef0 + (int)FVar37;
              DAT_004ccef4 = DAT_004ccef4 + extraout_ECX_11;
              DAT_004ccef8 = DAT_004ccef8 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccef8,
                                  DAT_004ccef4,DAT_004ccef0,*(FieldGridAsset **)(param_2 + 0x54));
              FVar22 = (FixedMathScale32)uVar38;
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle_00,FVar22);
              DAT_004ccef0 = DAT_004ccef0 + (int)FVar37;
              DAT_004ccef4 = DAT_004ccef4 + extraout_ECX_12;
              DAT_004ccef8 = DAT_004ccef8 + (int)(FVar37 >> 0x20);
              DAT_004ccefc = DAT_004ccefc +
                             ((int)(((longlong)FVar22 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current < DAT_004ccefc) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004ccf04 = 0;
            iVar25 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccefc = DAT_004ccefc - iVar25;
            if (iVar20 < iVar25) {
              DAT_004ccefc = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004ccef0 = DAT_004ccef0 - (int)FVar37;
              DAT_004ccef4 = DAT_004ccef4 - extraout_ECX_13;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccef8 = DAT_004ccef8 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccef8,DAT_004ccef4,DAT_004ccef0,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccef0 = DAT_004ccef0 + (int)FVar37;
              DAT_004ccef4 = DAT_004ccef4 + extraout_ECX_14;
              DAT_004ccef8 = DAT_004ccef8 + (int)(FVar37 >> 0x20);
            }
            else {
              DAT_004ccef8 = DAT_004ccef8 + uVar29;
            }
          }
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccedc,DAT_004cced8,*(FieldGridAsset **)(param_2 + 0x54));
          iVar20 = DAT_004ccee4;
          uVar29 = QVar21 - extraout_ECX_15;
          if (QVar21 < extraout_ECX_15) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccee0,DAT_004ccedc,DAT_004cced8,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004cceec = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004cced8 = DAT_004cced8 + (int)FVar37;
              DAT_004ccedc = DAT_004ccedc + extraout_ECX_16;
              DAT_004ccee0 = DAT_004ccee0 + (int)(FVar37 >> 0x20);
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccee4 == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccee4) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004cceec = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004cced8 = DAT_004cced8 + (int)FVar37;
              DAT_004ccedc = DAT_004ccedc + extraout_ECX_17;
              DAT_004ccee0 = DAT_004ccee0 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccee0,
                                  DAT_004ccedc,DAT_004cced8,*(FieldGridAsset **)(param_2 + 0x54));
              FVar22 = (FixedMathScale32)uVar38;
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle_01,FVar22);
              DAT_004cced8 = DAT_004cced8 + (int)FVar37;
              DAT_004ccedc = DAT_004ccedc + extraout_ECX_18;
              DAT_004ccee0 = DAT_004ccee0 + (int)(FVar37 >> 0x20);
              DAT_004ccee4 = DAT_004ccee4 +
                             ((int)(((longlong)FVar22 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current < DAT_004ccee4) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004cceec = 0;
            iVar25 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccee4 = DAT_004ccee4 - iVar25;
            if (iVar20 < iVar25) {
              DAT_004ccee4 = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004cced8 = DAT_004cced8 - (int)FVar37;
              DAT_004ccedc = DAT_004ccedc - extraout_ECX_19;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccee0 = DAT_004ccee0 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccee0,DAT_004ccedc,DAT_004cced8,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004cced8 = DAT_004cced8 + (int)FVar37;
              DAT_004ccedc = DAT_004ccedc + extraout_ECX_20;
              DAT_004ccee0 = DAT_004ccee0 + (int)(FVar37 >> 0x20);
            }
            else {
              DAT_004ccee0 = DAT_004ccee0 + uVar29;
            }
          }
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccf0c,DAT_004ccf08,*(FieldGridAsset **)(param_2 + 0x54));
          iVar20 = DAT_004ccf14;
          uVar29 = QVar21 - extraout_ECX_21;
          if (QVar21 < extraout_ECX_21) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccf10,DAT_004ccf0c,DAT_004ccf08,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004ccf1c = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf08 = DAT_004ccf08 + (int)FVar37;
              DAT_004ccf0c = DAT_004ccf0c + extraout_ECX_22;
              DAT_004ccf10 = DAT_004ccf10 + (int)(FVar37 >> 0x20);
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccf14 == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccf14) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004ccf1c = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004ccf08 = DAT_004ccf08 + (int)FVar37;
              DAT_004ccf0c = DAT_004ccf0c + extraout_ECX_23;
              DAT_004ccf10 = DAT_004ccf10 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccf10,
                                  DAT_004ccf0c,DAT_004ccf08,*(FieldGridAsset **)(param_2 + 0x54));
              FVar22 = (FixedMathScale32)uVar38;
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle_02,FVar22);
              DAT_004ccf08 = DAT_004ccf08 + (int)FVar37;
              DAT_004ccf0c = DAT_004ccf0c + extraout_ECX_24;
              DAT_004ccf10 = DAT_004ccf10 + (int)(FVar37 >> 0x20);
              DAT_004ccf14 = DAT_004ccf14 +
                             ((int)(((longlong)FVar22 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current < DAT_004ccf14) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004ccf1c = 0;
            iVar25 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccf14 = DAT_004ccf14 - iVar25;
            if (iVar20 < iVar25) {
              DAT_004ccf14 = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004ccf08 = DAT_004ccf08 - (int)FVar37;
              DAT_004ccf0c = DAT_004ccf0c - extraout_ECX_25;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccf10 = DAT_004ccf10 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccf10,DAT_004ccf0c,DAT_004ccf08,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf08 = DAT_004ccf08 + (int)FVar37;
              DAT_004ccf0c = DAT_004ccf0c + extraout_ECX_26;
              DAT_004ccf10 = DAT_004ccf10 + (int)(FVar37 >> 0x20);
            }
            else {
              DAT_004ccf10 = DAT_004ccf10 + uVar29;
            }
          }
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccf24,DAT_004ccf20,*(FieldGridAsset **)(param_2 + 0x54));
          iVar20 = DAT_004ccf2c;
          uVar29 = QVar21 - extraout_ECX_27;
          if (QVar21 < extraout_ECX_27) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccf28,DAT_004ccf24,DAT_004ccf20,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004ccf34 = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf20 = DAT_004ccf20 + (int)FVar37;
              DAT_004ccf24 = DAT_004ccf24 + extraout_ECX_28;
              DAT_004ccf28 = DAT_004ccf28 + (int)(FVar37 >> 0x20);
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccf2c == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccf2c) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004ccf34 = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004ccf20 = DAT_004ccf20 + (int)FVar37;
              DAT_004ccf24 = DAT_004ccf24 + extraout_ECX_29;
              DAT_004ccf28 = DAT_004ccf28 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccf28,
                                  DAT_004ccf24,DAT_004ccf20,*(FieldGridAsset **)(param_2 + 0x54));
              FVar22 = (FixedMathScale32)uVar38;
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle_03,FVar22);
              DAT_004ccf20 = DAT_004ccf20 + (int)FVar37;
              DAT_004ccf24 = DAT_004ccf24 + extraout_ECX_30;
              DAT_004ccf28 = DAT_004ccf28 + (int)(FVar37 >> 0x20);
              DAT_004ccf2c = DAT_004ccf2c +
                             ((int)(((longlong)FVar22 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current < DAT_004ccf2c) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004ccf34 = 0;
            iVar25 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccf2c = DAT_004ccf2c - iVar25;
            if (iVar20 < iVar25) {
              DAT_004ccf2c = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004ccf20 = DAT_004ccf20 - (int)FVar37;
              DAT_004ccf24 = DAT_004ccf24 - extraout_ECX_31;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccf28 = DAT_004ccf28 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccf28,DAT_004ccf24,DAT_004ccf20,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf20 = DAT_004ccf20 + (int)FVar37;
              DAT_004ccf24 = DAT_004ccf24 + extraout_ECX_32;
              DAT_004ccf28 = DAT_004ccf28 + (int)(FVar37 >> 0x20);
            }
            else {
              DAT_004ccf28 = DAT_004ccf28 + uVar29;
            }
          }
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccf3c,DAT_004ccf38,*(FieldGridAsset **)(param_2 + 0x54));
          iVar20 = DAT_004ccf44;
          uVar29 = QVar21 - extraout_ECX_33;
          if (QVar21 < extraout_ECX_33) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccf40,DAT_004ccf3c,DAT_004ccf38,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004ccf4c = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf38 = DAT_004ccf38 + (int)FVar37;
              DAT_004ccf3c = DAT_004ccf3c + extraout_ECX_34;
              DAT_004ccf40 = DAT_004ccf40 + (int)(FVar37 >> 0x20);
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccf44 == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccf44) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004ccf4c = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004ccf38 = DAT_004ccf38 + (int)FVar37;
              DAT_004ccf3c = DAT_004ccf3c + extraout_ECX_35;
              DAT_004ccf40 = DAT_004ccf40 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccf40,
                                  DAT_004ccf3c,DAT_004ccf38,*(FieldGridAsset **)(param_2 + 0x54));
              FVar22 = (FixedMathScale32)uVar38;
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle_04,FVar22);
              DAT_004ccf38 = DAT_004ccf38 + (int)FVar37;
              DAT_004ccf3c = DAT_004ccf3c + extraout_ECX_36;
              DAT_004ccf40 = DAT_004ccf40 + (int)(FVar37 >> 0x20);
              DAT_004ccf44 = DAT_004ccf44 +
                             ((int)(((longlong)FVar22 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current < DAT_004ccf44) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004ccf4c = 0;
            iVar25 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccf44 = DAT_004ccf44 - iVar25;
            if (iVar20 < iVar25) {
              DAT_004ccf44 = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004ccf38 = DAT_004ccf38 - (int)FVar37;
              DAT_004ccf3c = DAT_004ccf3c - extraout_ECX_37;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccf40 = DAT_004ccf40 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccf40,DAT_004ccf3c,DAT_004ccf38,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf38 = DAT_004ccf38 + (int)FVar37;
              DAT_004ccf3c = DAT_004ccf3c + extraout_ECX_38;
              DAT_004ccf40 = DAT_004ccf40 + (int)(FVar37 >> 0x20);
            }
            else {
              DAT_004ccf40 = DAT_004ccf40 + uVar29;
            }
          }
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccf54,DAT_004ccf50,*(FieldGridAsset **)(param_2 + 0x54));
          iVar20 = DAT_004ccf5c;
          uVar29 = QVar21 - extraout_ECX_39;
          if (QVar21 < extraout_ECX_39) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccf58,DAT_004ccf54,DAT_004ccf50,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004ccf64 = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf50 = DAT_004ccf50 + (int)FVar37;
              DAT_004ccf54 = DAT_004ccf54 + extraout_ECX_40;
              DAT_004ccf58 = DAT_004ccf58 + (int)(FVar37 >> 0x20);
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccf5c == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccf5c) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004ccf64 = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004ccf50 = DAT_004ccf50 + (int)FVar37;
              DAT_004ccf54 = DAT_004ccf54 + extraout_ECX_41;
              DAT_004ccf58 = DAT_004ccf58 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccf58,
                                  DAT_004ccf54,DAT_004ccf50,*(FieldGridAsset **)(param_2 + 0x54));
              FVar22 = (FixedMathScale32)uVar38;
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle_05,FVar22);
              DAT_004ccf50 = DAT_004ccf50 + (int)FVar37;
              DAT_004ccf54 = DAT_004ccf54 + extraout_ECX_42;
              DAT_004ccf58 = DAT_004ccf58 + (int)(FVar37 >> 0x20);
              DAT_004ccf5c = DAT_004ccf5c +
                             ((int)(((longlong)FVar22 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current < DAT_004ccf5c) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004ccf64 = 0;
            iVar25 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccf5c = DAT_004ccf5c - iVar25;
            if (iVar20 < iVar25) {
              DAT_004ccf5c = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004ccf50 = DAT_004ccf50 - (int)FVar37;
              DAT_004ccf54 = DAT_004ccf54 - extraout_ECX_43;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccf58 = DAT_004ccf58 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccf58,DAT_004ccf54,DAT_004ccf50,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf50 = DAT_004ccf50 + (int)FVar37;
              DAT_004ccf54 = DAT_004ccf54 + extraout_ECX_44;
              DAT_004ccf58 = DAT_004ccf58 + (int)(FVar37 >> 0x20);
            }
            else {
              DAT_004ccf58 = DAT_004ccf58 + uVar29;
            }
          }
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccf6c,DAT_004ccf68,*(FieldGridAsset **)(param_2 + 0x54));
          iVar20 = DAT_004ccf74;
          uVar29 = QVar21 - extraout_ECX_45;
          if (QVar21 < extraout_ECX_45) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccf70,DAT_004ccf6c,DAT_004ccf68,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004ccf7c = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf68 = DAT_004ccf68 + (int)FVar37;
              DAT_004ccf6c = DAT_004ccf6c + extraout_ECX_46;
              DAT_004ccf70 = DAT_004ccf70 + (int)(FVar37 >> 0x20);
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccf74 == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccf74) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004ccf7c = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004ccf68 = DAT_004ccf68 + (int)FVar37;
              DAT_004ccf6c = DAT_004ccf6c + extraout_ECX_47;
              DAT_004ccf70 = DAT_004ccf70 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccf70,
                                  DAT_004ccf6c,DAT_004ccf68,*(FieldGridAsset **)(param_2 + 0x54));
              FVar22 = (FixedMathScale32)uVar38;
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle_06,FVar22);
              DAT_004ccf68 = DAT_004ccf68 + (int)FVar37;
              DAT_004ccf6c = DAT_004ccf6c + extraout_ECX_48;
              DAT_004ccf70 = DAT_004ccf70 + (int)(FVar37 >> 0x20);
              DAT_004ccf74 = DAT_004ccf74 +
                             ((int)(((longlong)FVar22 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current < DAT_004ccf74) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004ccf7c = 0;
            iVar25 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccf74 = DAT_004ccf74 - iVar25;
            if (iVar20 < iVar25) {
              DAT_004ccf74 = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004ccf68 = DAT_004ccf68 - (int)FVar37;
              DAT_004ccf6c = DAT_004ccf6c - extraout_ECX_49;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccf70 = DAT_004ccf70 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccf70,DAT_004ccf6c,DAT_004ccf68,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf68 = DAT_004ccf68 + (int)FVar37;
              DAT_004ccf6c = DAT_004ccf6c + extraout_ECX_50;
              DAT_004ccf70 = DAT_004ccf70 + (int)(FVar37 >> 0x20);
            }
            else {
              DAT_004ccf70 = DAT_004ccf70 + uVar29;
            }
          }
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccf84,DAT_004ccf80,*(FieldGridAsset **)(param_2 + 0x54));
          iVar20 = DAT_004ccf8c;
          uVar29 = QVar21 - extraout_ECX_51;
          if (QVar21 < extraout_ECX_51) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccf88,DAT_004ccf84,DAT_004ccf80,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004ccf94 = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf80 = DAT_004ccf80 + (int)FVar37;
              DAT_004ccf84 = DAT_004ccf84 + extraout_ECX_52;
              DAT_004ccf88 = DAT_004ccf88 + (int)(FVar37 >> 0x20);
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccf8c == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccf8c) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004ccf94 = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004ccf80 = DAT_004ccf80 + (int)FVar37;
              DAT_004ccf84 = DAT_004ccf84 + extraout_ECX_53;
              DAT_004ccf88 = DAT_004ccf88 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccf88,
                                  DAT_004ccf84,DAT_004ccf80,*(FieldGridAsset **)(param_2 + 0x54));
              FVar22 = (FixedMathScale32)uVar38;
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle_07,FVar22);
              DAT_004ccf80 = DAT_004ccf80 + (int)FVar37;
              DAT_004ccf84 = DAT_004ccf84 + extraout_ECX_54;
              DAT_004ccf88 = DAT_004ccf88 + (int)(FVar37 >> 0x20);
              DAT_004ccf8c = DAT_004ccf8c +
                             ((int)(((longlong)FVar22 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current < DAT_004ccf8c) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004ccf94 = 0;
            iVar25 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccf8c = DAT_004ccf8c - iVar25;
            if (iVar20 < iVar25) {
              DAT_004ccf8c = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004ccf80 = DAT_004ccf80 - (int)FVar37;
              DAT_004ccf84 = DAT_004ccf84 - extraout_ECX_55;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccf88 = DAT_004ccf88 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccf88,DAT_004ccf84,DAT_004ccf80,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf80 = DAT_004ccf80 + (int)FVar37;
              DAT_004ccf84 = DAT_004ccf84 + extraout_ECX_56;
              DAT_004ccf88 = DAT_004ccf88 + (int)(FVar37 >> 0x20);
            }
            else {
              DAT_004ccf88 = DAT_004ccf88 + uVar29;
            }
          }
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccfb4,DAT_004ccfb0,*(FieldGridAsset **)(param_2 + 0x54));
          iVar20 = DAT_004ccfbc;
          uVar29 = QVar21 - extraout_ECX_57;
          if (QVar21 < extraout_ECX_57) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccfb8,DAT_004ccfb4,DAT_004ccfb0,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004ccfc4 = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccfb0 = DAT_004ccfb0 + (int)FVar37;
              DAT_004ccfb4 = DAT_004ccfb4 + extraout_ECX_58;
              DAT_004ccfb8 = DAT_004ccfb8 + (int)(FVar37 >> 0x20);
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccfbc == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccfbc) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004ccfc4 = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004ccfb0 = DAT_004ccfb0 + (int)FVar37;
              DAT_004ccfb4 = DAT_004ccfb4 + extraout_ECX_59;
              DAT_004ccfb8 = DAT_004ccfb8 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccfb8,
                                  DAT_004ccfb4,DAT_004ccfb0,*(FieldGridAsset **)(param_2 + 0x54));
              FVar22 = (FixedMathScale32)uVar38;
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle_08,FVar22);
              DAT_004ccfb0 = DAT_004ccfb0 + (int)FVar37;
              DAT_004ccfb4 = DAT_004ccfb4 + extraout_ECX_60;
              DAT_004ccfb8 = DAT_004ccfb8 + (int)(FVar37 >> 0x20);
              DAT_004ccfbc = DAT_004ccfbc +
                             ((int)(((longlong)FVar22 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current < DAT_004ccfbc) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004ccfc4 = 0;
            iVar25 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccfbc = DAT_004ccfbc - iVar25;
            if (iVar20 < iVar25) {
              DAT_004ccfbc = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004ccfb0 = DAT_004ccfb0 - (int)FVar37;
              DAT_004ccfb4 = DAT_004ccfb4 - extraout_ECX_61;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccfb8 = DAT_004ccfb8 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccfb8,DAT_004ccfb4,DAT_004ccfb0,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccfb0 = DAT_004ccfb0 + (int)FVar37;
              DAT_004ccfb4 = DAT_004ccfb4 + extraout_ECX_62;
              DAT_004ccfb8 = DAT_004ccfb8 + (int)(FVar37 >> 0x20);
            }
            else {
              DAT_004ccfb8 = DAT_004ccfb8 + uVar29;
            }
          }
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccf9c,DAT_004ccf98,*(FieldGridAsset **)(param_2 + 0x54));
          uVar24 = DAT_004ccfa4;
          uVar29 = QVar21 - extraout_ECX_63;
          if (QVar21 < extraout_ECX_63) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccfa0,DAT_004ccf9c,DAT_004ccf98,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004ccfac = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf98 = DAT_004ccf98 + (int)FVar37;
              DAT_004ccf9c = DAT_004ccf9c + extraout_ECX_64;
              DAT_004ccfa0 = DAT_004ccfa0 + (int)(FVar37 >> 0x20);
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccfa4 == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccfa4) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004ccfac = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004ccf98 = DAT_004ccf98 + (int)FVar37;
              DAT_004ccf9c = DAT_004ccf9c + extraout_ECX_65;
              DAT_004ccfa0 = DAT_004ccfa0 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccfa0,
                                  DAT_004ccf9c,DAT_004ccf98,*(FieldGridAsset **)(param_2 + 0x54));
              FVar22 = (FixedMathScale32)uVar38;
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle_09,FVar22);
              DAT_004ccf98 = DAT_004ccf98 + (int)FVar37;
              DAT_004ccf9c = DAT_004ccf9c + extraout_ECX_66;
              DAT_004ccfa0 = DAT_004ccfa0 + (int)(FVar37 >> 0x20);
              DAT_004ccfa4 = DAT_004ccfa4 +
                             ((int)(((longlong)FVar22 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current < (int)DAT_004ccfa4) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004ccfac = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccfa4 = DAT_004ccfa4 - iVar20;
            if ((int)uVar24 < iVar20) {
              DAT_004ccfa4 = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004ccf98 = DAT_004ccf98 - (int)FVar37;
              DAT_004ccf9c = DAT_004ccf9c - extraout_ECX_67;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccfa0 = DAT_004ccfa0 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccfa0,DAT_004ccf9c,DAT_004ccf98,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              DAT_004ccf98 = DAT_004ccf98 + (int)FVar37;
              DAT_004ccf9c = DAT_004ccf9c + extraout_ECX_68;
              DAT_004ccfa0 = DAT_004ccfa0 + (int)(FVar37 >> 0x20);
            }
            else {
              DAT_004ccfa0 = DAT_004ccfa0 + uVar29;
            }
          }
          QVar21 = FieldGrid_InterpolateTopSurfaceHeight
                             (DAT_004ccfcc,DAT_004ccfc8,*(FieldGridAsset **)(param_2 + 0x54));
          uVar24 = DAT_004ccfd4;
          uVar29 = QVar21 - extraout_ECX_69;
          if (QVar21 < extraout_ECX_69) {
            uVar29 = *(uint *)(param_1 + 0x54);
            bVar30 = CARRY4(uVar29,uVar29);
            uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                uVar29 * 2,DAT_004ccfd0,DAT_004ccfcc,DAT_004ccfc8,
                                *(int *)(param_2 + 0x54));
            if (bVar30) {
              DAT_004ccfdc = (FixedMathScale32)uVar38;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              uVar29 = (uint)(FVar37 >> 0x20);
              DAT_004ccfc8 = DAT_004ccfc8 + (int)FVar37;
              DAT_004ccfcc = DAT_004ccfcc + extraout_ECX_70;
              bVar30 = CARRY4(DAT_004ccfd0,uVar29);
              DAT_004ccfd0 = DAT_004ccfd0 + uVar29;
            }
            else {
              iVar20 = *(int *)(param_1 + 0x54) * 2;
              if (g_GraphicsShadingGridStepQ20Current - DAT_004ccfd4 == 0) {
                return;
              }
              QVar21 = (Q12)(((longlong)(int)(g_GraphicsShadingGridStepQ20Current - DAT_004ccfd4) *
                             (longlong)iVar20) / (longlong)(int)g_GraphicsShadingGridStepQ20Current)
              ;
              DAT_004ccfdc = iVar20;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  iVar20);
              DAT_004ccfc8 = DAT_004ccfc8 + (int)FVar37;
              DAT_004ccfcc = DAT_004ccfcc + extraout_ECX_71;
              DAT_004ccfd0 = DAT_004ccfd0 + (int)(FVar37 >> 0x20);
              bVar30 = 0xffffc000 < *(uint *)(param_2 + 0xbc);
              uVar38 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                                 (-*(uint *)(param_2 + 0xbc) - 0x4000,
                                  *(uint *)(param_2 + 0xb8) ^ 0x8000,QVar21,DAT_004ccfd0,
                                  DAT_004ccfcc,DAT_004ccfc8,*(FieldGridAsset **)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((AngleTurn32)(uVar38 >> 0x20),azimuthAngle_10,
                                  (FixedMathScale32)uVar38);
              DAT_004ccfc8 = DAT_004ccfc8 + (int)FVar37;
              DAT_004ccfcc = DAT_004ccfcc + extraout_ECX_72;
              DAT_004ccfd0 = DAT_004ccfd0 + (int)(FVar37 >> 0x20);
              DAT_004ccfd4 = DAT_004ccfd4 +
                             ((int)(((longlong)(int)uVar38 *
                                    (longlong)(int)g_GraphicsShadingGridStepQ20Current) /
                                   (longlong)*(int *)(param_1 + 0x54)) >> 1);
              bVar30 = DAT_004ccfd4 < g_GraphicsShadingGridStepQ20Current;
              if ((int)g_GraphicsShadingGridStepQ20Current < (int)DAT_004ccfd4) {
                return;
              }
            }
          }
          else {
            if (*(uint *)(param_1 + 0x54) <= uVar29) {
              uVar29 = *(uint *)(param_1 + 0x54);
            }
            DAT_004ccfdc = 0;
            iVar20 = (int)(((longlong)(int)g_GraphicsShadingGridStepQ20Current *
                           (longlong)
                           (int)((int)((ulonglong)
                                       ((longlong)(int)uVar29 *
                                       (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >> 0x20)
                                 << 3 | (uint)((longlong)(int)uVar29 *
                                              (longlong)g_FixedCosQ28[*(int *)(param_2 + 0xbc)]) >>
                                        0x1d)) / (longlong)*(int *)(param_1 + 0x54));
            DAT_004ccfd4 = DAT_004ccfd4 - iVar20;
            if ((int)uVar24 < iVar20) {
              DAT_004ccfd4 = 0;
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  *(int *)(param_1 + 0x54) * 2);
              DAT_004ccfc8 = DAT_004ccfc8 - (int)FVar37;
              DAT_004ccfcc = DAT_004ccfcc - extraout_ECX_73;
              uVar29 = *(uint *)(param_1 + 0x54);
              DAT_004ccfd0 = DAT_004ccfd0 - (int)(FVar37 >> 0x20);
              bVar30 = CARRY4(uVar29,uVar29);
              uVar38 = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  uVar29 * 2,DAT_004ccfd0,DAT_004ccfcc,DAT_004ccfc8,
                                  *(int *)(param_2 + 0x54));
              if (!bVar30) {
                return;
              }
              FVar37 = FixedMath_DirectionFromAnglesScaledRegs
                                 (*(AngleTurn32 *)(param_2 + 0xbc),*(AngleTurn32 *)(param_2 + 0xb8),
                                  (FixedMathScale32)uVar38);
              uVar29 = (uint)(FVar37 >> 0x20);
              DAT_004ccfc8 = DAT_004ccfc8 + (int)FVar37;
              DAT_004ccfcc = DAT_004ccfcc + extraout_ECX_74;
              bVar30 = CARRY4(DAT_004ccfd0,uVar29);
              DAT_004ccfd0 = DAT_004ccfd0 + uVar29;
            }
            else {
              bVar30 = CARRY4(DAT_004ccfd0,uVar29);
              DAT_004ccfd0 = DAT_004ccfd0 + uVar29;
            }
          }
          pGVar23 = (GraphicsProjectedPointEdxEax8 *)
                    GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks(param_2);
          if (!bVar30) {
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 0x65),(GraphicsFixedVec3 *)&DAT_004ccec0,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 0xb1),(GraphicsFixedVec3 *)&DAT_004cced8,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 0x71),(GraphicsFixedVec3 *)&DAT_004ccef0,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 0xa9),(GraphicsFixedVec3 *)&DAT_004ccf08,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 0x55),(GraphicsFixedVec3 *)&DAT_004ccf20,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 0x39),(GraphicsFixedVec3 *)&DAT_004ccf38,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 0x29),(GraphicsFixedVec3 *)&DAT_004ccf50,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 0x45),(GraphicsFixedVec3 *)&DAT_004ccf68,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 1),(GraphicsFixedVec3 *)&DAT_004ccf80,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 9),(GraphicsFixedVec3 *)&DAT_004ccf98,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 5),(GraphicsFixedVec3 *)&DAT_004ccfb0,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(pGVar23 + 0x11),(GraphicsFixedVec3 *)&DAT_004ccfc8,
                       &g_ViewProjectionMatrixFixed);
            if (((((int)pGVar23[0x66] < (int)g_ProjectionScaleFixed) ||
                 ((int)pGVar23[0xb2] < (int)g_ProjectionScaleFixed)) ||
                (((int)pGVar23[0x72] < (int)g_ProjectionScaleFixed ||
                 ((((int)pGVar23[0xaa] < (int)g_ProjectionScaleFixed ||
                   ((int)pGVar23[0x56] < (int)g_ProjectionScaleFixed)) ||
                  ((int)pGVar23[0x3a] < (int)g_ProjectionScaleFixed)))))) ||
               ((((int)pGVar23[0x2a] < (int)g_ProjectionScaleFixed ||
                 ((int)pGVar23[0x46] < (int)g_ProjectionScaleFixed)) ||
                (((int)pGVar23[2] < (int)g_ProjectionScaleFixed ||
                 ((((int)pGVar23[10] < (int)g_ProjectionScaleFixed ||
                   ((int)pGVar23[6] < (int)g_ProjectionScaleFixed)) ||
                  ((int)pGVar23[0x12] < (int)g_ProjectionScaleFixed)))))))) {
              GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks(param_2);
            }
            else {
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 0x65));
              pGVar23[100] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 0xb1));
              pGVar23[0xb0] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 0x71));
              pGVar23[0x70] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 0xa9));
              pGVar23[0xa8] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 0x55));
              pGVar23[0x54] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 0x39));
              pGVar23[0x38] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 0x29));
              pGVar23[0x28] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 0x45));
              pGVar23[0x44] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 1));
              *pGVar23 = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 9));
              pGVar23[8] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 5));
              pGVar23[4] = GVar39;
              GVar39 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(pGVar23 + 0x11));
              pGVar23[0x10] = GVar39;
              dVar18 = g_GraphicsShadingGridStepQ20;
              iVar26 = DAT_004cce1c;
              iVar25 = (g_GraphicsShadingGridStepQ20 - 0x1000) + DAT_004cce1c;
              iVar20 = ((int)g_GraphicsShadingGridStepQ20 >> 1) + DAT_004cce1c;
              *(int *)((int)pGVar23 + 0x334) = DAT_004cce1c;
              *(int *)((int)pGVar23 + 0x394) = iVar25;
              *(int *)((int)pGVar23 + 0x594) = iVar26;
              *(int *)((int)pGVar23 + 0x554) = iVar25;
              *(int *)((int)pGVar23 + 0x2b4) = iVar26;
              *(int *)((int)pGVar23 + 0x1d4) = iVar25;
              *(int *)((int)pGVar23 + 0x154) = iVar20;
              *(int *)((int)pGVar23 + 0x234) = iVar20;
              iVar25 = ((int)dVar18 >> 2) + iVar20;
              iVar20 = iVar20 - ((int)dVar18 >> 2);
              *(int *)((int)pGVar23 + 0x14) = iVar20;
              *(int *)((int)pGVar23 + 0x34) = iVar25;
              *(int *)((int)pGVar23 + 0x54) = iVar20;
              *(int *)((int)pGVar23 + 0x94) = iVar25;
              iVar20 = DAT_004cce20;
              iVar28 = DAT_004ccee4 + DAT_004cce20;
              iVar25 = DAT_004ccefc + DAT_004cce20;
              iVar26 = DAT_004ccf14 + DAT_004cce20;
              *(int *)(pGVar23 + 0x67) = DAT_004ccecc + DAT_004cce20;
              *(int *)(pGVar23 + 0xb3) = iVar28;
              *(int *)(pGVar23 + 0x73) = iVar25;
              *(int *)(pGVar23 + 0xab) = iVar26;
              iVar28 = DAT_004ccf44 + iVar20;
              iVar25 = DAT_004ccf5c + iVar20;
              iVar26 = DAT_004ccf74 + iVar20;
              *(int *)(pGVar23 + 0x57) = DAT_004ccf2c + iVar20;
              *(int *)(pGVar23 + 0x3b) = iVar28;
              *(int *)(pGVar23 + 0x2b) = iVar25;
              *(int *)(pGVar23 + 0x47) = iVar26;
              iVar28 = DAT_004ccfa4 + iVar20;
              iVar25 = DAT_004ccfbc + iVar20;
              iVar26 = DAT_004ccfd4 + iVar20;
              *(int *)(pGVar23 + 3) = DAT_004ccf8c + iVar20;
              *(int *)(pGVar23 + 0xb) = iVar28;
              *(int *)(pGVar23 + 7) = iVar25;
              *(int *)(pGVar23 + 0x13) = iVar26;
              *(int *)(pGVar23 + 0x66) = (int)pGVar23[0x66] + -0x1000;
              *(int *)(pGVar23 + 0xb2) = (int)pGVar23[0xb2] + -0x1000;
              *(int *)(pGVar23 + 0x72) = (int)pGVar23[0x72] + -0x1000;
              *(int *)(pGVar23 + 0xaa) = (int)pGVar23[0xaa] + -0x1000;
              *(int *)(pGVar23 + 0x56) = (int)pGVar23[0x56] + -0x1000;
              *(int *)(pGVar23 + 0x3a) = (int)pGVar23[0x3a] + -0x1000;
              *(int *)(pGVar23 + 0x2a) = (int)pGVar23[0x2a] + -0x1000;
              *(int *)(pGVar23 + 0x46) = (int)pGVar23[0x46] + -0x1000;
              *(int *)(pGVar23 + 2) = (int)pGVar23[2] + -0x1000;
              *(int *)(pGVar23 + 10) = (int)pGVar23[10] + -0x1000;
              *(int *)(pGVar23 + 6) = (int)pGVar23[6] + -0x1000;
              *(int *)(pGVar23 + 0x12) = (int)pGVar23[0x12] + -0x1000;
              uVar29 = *(uint *)(param_1 + 0x58) >> 1;
              uVar24 = uVar29 | 0xffffff;
              bVar16 = (byte)(*(uint *)(param_1 + 0x58) >> 0x18);
              uVar17 = CONCAT11(bVar16 >> 1,bVar16 >> 1);
              uVar32 = (undefined1)(uVar24 >> 0x10);
              uVar31 = (undefined1)(uVar24 >> 8);
              uVar36 = CONCAT26(uVar17 >> 4,
                                CONCAT24((ushort)(CONCAT35(CONCAT21(uVar17,uVar32),
                                                           CONCAT14(uVar32,uVar29)) >> 0x20) >> 4,
                                         CONCAT22(CONCAT11(uVar31,uVar31) >> 4,
                                                  CONCAT11((char)uVar24,(char)uVar24) >> 4)));
              uVar29 = 0x5000 - DAT_004cced4;
              if ((int)uVar29 < 0) {
                uVar29 = 0;
              }
              uVar24 = 0x5000 - DAT_004cceec;
              if ((int)uVar24 < 0) {
                uVar24 = 0;
              }
              uVar27 = 0x5000 - DAT_004ccf04;
              if ((int)uVar27 < 0) {
                uVar27 = 0;
              }
              uVar29 = uVar29 >> 6;
              uVar24 = uVar24 >> 6;
              uVar27 = uVar27 >> 6;
              if (0xff < uVar29) {
                uVar29 = 0xff;
              }
              if (0xff < uVar24) {
                uVar24 = 0xff;
              }
              if (0xff < uVar27) {
                uVar27 = 0xff;
              }
              uVar33 = pmulhw(*(undefined8 *)(uVar29 * 8 + 0x41ee80),uVar36);
              uVar34 = pmulhw(*(undefined8 *)(uVar24 * 8 + 0x41ee80),uVar36);
              uVar35 = pmulhw(*(undefined8 *)(uVar27 * 8 + 0x41ee80),uVar36);
              sVar4 = (short)uVar33;
              sVar7 = (short)((ulonglong)uVar33 >> 0x10);
              sVar10 = (short)((ulonglong)uVar33 >> 0x20);
              sVar13 = (short)((ulonglong)uVar33 >> 0x30);
              sVar5 = (short)uVar34;
              sVar8 = (short)((ulonglong)uVar34 >> 0x10);
              sVar11 = (short)((ulonglong)uVar34 >> 0x20);
              sVar14 = (short)((ulonglong)uVar34 >> 0x30);
              sVar6 = (short)uVar35;
              sVar9 = (short)((ulonglong)uVar35 >> 0x10);
              sVar12 = (short)((ulonglong)uVar35 >> 0x20);
              sVar15 = (short)((ulonglong)uVar35 >> 0x30);
              *(uint *)((int)pGVar23 + 0x33c) =
                   CONCAT13((0 < sVar13) * (sVar13 < 0x100) * (char)((ulonglong)uVar33 >> 0x30) -
                            (0xff < sVar13),
                            CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                                     (char)((ulonglong)uVar33 >> 0x20) - (0xff < sVar10),
                                     CONCAT11((0 < sVar7) * (sVar7 < 0x100) *
                                              (char)((ulonglong)uVar33 >> 0x10) - (0xff < sVar7),
                                              (0 < sVar4) * (sVar4 < 0x100) * (char)uVar33 -
                                              (0xff < sVar4))));
              *(uint *)((int)pGVar23 + 0x59c) =
                   CONCAT13((0 < sVar14) * (sVar14 < 0x100) * (char)((ulonglong)uVar34 >> 0x30) -
                            (0xff < sVar14),
                            CONCAT12((0 < sVar11) * (sVar11 < 0x100) *
                                     (char)((ulonglong)uVar34 >> 0x20) - (0xff < sVar11),
                                     CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                              (char)((ulonglong)uVar34 >> 0x10) - (0xff < sVar8),
                                              (0 < sVar5) * (sVar5 < 0x100) * (char)uVar34 -
                                              (0xff < sVar5))));
              *(uint *)((int)pGVar23 + 0x39c) =
                   CONCAT13((0 < sVar15) * (sVar15 < 0x100) * (char)((ulonglong)uVar35 >> 0x30) -
                            (0xff < sVar15),
                            CONCAT12((0 < sVar12) * (sVar12 < 0x100) *
                                     (char)((ulonglong)uVar35 >> 0x20) - (0xff < sVar12),
                                     CONCAT11((0 < sVar9) * (sVar9 < 0x100) *
                                              (char)((ulonglong)uVar35 >> 0x10) - (0xff < sVar9),
                                              (0 < sVar6) * (sVar6 < 0x100) * (char)uVar35 -
                                              (0xff < sVar6))));
              uVar29 = 0x5000 - DAT_004ccf1c;
              if ((int)uVar29 < 0) {
                uVar29 = 0;
              }
              uVar24 = 0x5000 - DAT_004ccf34;
              if ((int)uVar24 < 0) {
                uVar24 = 0;
              }
              uVar27 = 0x5000 - DAT_004ccf4c;
              if ((int)uVar27 < 0) {
                uVar27 = 0;
              }
              uVar29 = uVar29 >> 6;
              uVar24 = uVar24 >> 6;
              uVar27 = uVar27 >> 6;
              if (0xff < uVar29) {
                uVar29 = 0xff;
              }
              if (0xff < uVar24) {
                uVar24 = 0xff;
              }
              if (0xff < uVar27) {
                uVar27 = 0xff;
              }
              uVar33 = pmulhw(*(undefined8 *)(uVar29 * 8 + 0x41ee80),uVar36);
              uVar34 = pmulhw(*(undefined8 *)(uVar24 * 8 + 0x41ee80),uVar36);
              uVar35 = pmulhw(*(undefined8 *)(uVar27 * 8 + 0x41ee80),uVar36);
              sVar4 = (short)uVar33;
              sVar7 = (short)((ulonglong)uVar33 >> 0x10);
              sVar10 = (short)((ulonglong)uVar33 >> 0x20);
              sVar13 = (short)((ulonglong)uVar33 >> 0x30);
              sVar5 = (short)uVar34;
              sVar8 = (short)((ulonglong)uVar34 >> 0x10);
              sVar11 = (short)((ulonglong)uVar34 >> 0x20);
              sVar14 = (short)((ulonglong)uVar34 >> 0x30);
              sVar6 = (short)uVar35;
              sVar9 = (short)((ulonglong)uVar35 >> 0x10);
              sVar12 = (short)((ulonglong)uVar35 >> 0x20);
              sVar15 = (short)((ulonglong)uVar35 >> 0x30);
              *(uint *)((int)pGVar23 + 0x55c) =
                   CONCAT13((0 < sVar13) * (sVar13 < 0x100) * (char)((ulonglong)uVar33 >> 0x30) -
                            (0xff < sVar13),
                            CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                                     (char)((ulonglong)uVar33 >> 0x20) - (0xff < sVar10),
                                     CONCAT11((0 < sVar7) * (sVar7 < 0x100) *
                                              (char)((ulonglong)uVar33 >> 0x10) - (0xff < sVar7),
                                              (0 < sVar4) * (sVar4 < 0x100) * (char)uVar33 -
                                              (0xff < sVar4))));
              *(uint *)((int)pGVar23 + 700) =
                   CONCAT13((0 < sVar14) * (sVar14 < 0x100) * (char)((ulonglong)uVar34 >> 0x30) -
                            (0xff < sVar14),
                            CONCAT12((0 < sVar11) * (sVar11 < 0x100) *
                                     (char)((ulonglong)uVar34 >> 0x20) - (0xff < sVar11),
                                     CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                              (char)((ulonglong)uVar34 >> 0x10) - (0xff < sVar8),
                                              (0 < sVar5) * (sVar5 < 0x100) * (char)uVar34 -
                                              (0xff < sVar5))));
              *(uint *)((int)pGVar23 + 0x1dc) =
                   CONCAT13((0 < sVar15) * (sVar15 < 0x100) * (char)((ulonglong)uVar35 >> 0x30) -
                            (0xff < sVar15),
                            CONCAT12((0 < sVar12) * (sVar12 < 0x100) *
                                     (char)((ulonglong)uVar35 >> 0x20) - (0xff < sVar12),
                                     CONCAT11((0 < sVar9) * (sVar9 < 0x100) *
                                              (char)((ulonglong)uVar35 >> 0x10) - (0xff < sVar9),
                                              (0 < sVar6) * (sVar6 < 0x100) * (char)uVar35 -
                                              (0xff < sVar6))));
              uVar29 = 0x5000 - DAT_004ccf64;
              if ((int)uVar29 < 0) {
                uVar29 = 0;
              }
              uVar24 = 0x5000 - DAT_004ccf7c;
              if ((int)uVar24 < 0) {
                uVar24 = 0;
              }
              uVar27 = 0x5000 - DAT_004ccf94;
              if ((int)uVar27 < 0) {
                uVar27 = 0;
              }
              uVar29 = uVar29 >> 6;
              uVar24 = uVar24 >> 6;
              uVar27 = uVar27 >> 6;
              if (0xff < uVar29) {
                uVar29 = 0xff;
              }
              if (0xff < uVar24) {
                uVar24 = 0xff;
              }
              if (0xff < uVar27) {
                uVar27 = 0xff;
              }
              uVar33 = pmulhw(*(undefined8 *)(uVar29 * 8 + 0x41ee80),uVar36);
              uVar34 = pmulhw(*(undefined8 *)(uVar24 * 8 + 0x41ee80),uVar36);
              uVar35 = pmulhw(*(undefined8 *)(uVar27 * 8 + 0x41ee80),uVar36);
              sVar4 = (short)uVar33;
              sVar7 = (short)((ulonglong)uVar33 >> 0x10);
              sVar10 = (short)((ulonglong)uVar33 >> 0x20);
              sVar13 = (short)((ulonglong)uVar33 >> 0x30);
              sVar5 = (short)uVar34;
              sVar8 = (short)((ulonglong)uVar34 >> 0x10);
              sVar11 = (short)((ulonglong)uVar34 >> 0x20);
              sVar14 = (short)((ulonglong)uVar34 >> 0x30);
              sVar6 = (short)uVar35;
              sVar9 = (short)((ulonglong)uVar35 >> 0x10);
              sVar12 = (short)((ulonglong)uVar35 >> 0x20);
              sVar15 = (short)((ulonglong)uVar35 >> 0x30);
              *(uint *)((int)pGVar23 + 0x15c) =
                   CONCAT13((0 < sVar13) * (sVar13 < 0x100) * (char)((ulonglong)uVar33 >> 0x30) -
                            (0xff < sVar13),
                            CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                                     (char)((ulonglong)uVar33 >> 0x20) - (0xff < sVar10),
                                     CONCAT11((0 < sVar7) * (sVar7 < 0x100) *
                                              (char)((ulonglong)uVar33 >> 0x10) - (0xff < sVar7),
                                              (0 < sVar4) * (sVar4 < 0x100) * (char)uVar33 -
                                              (0xff < sVar4))));
              *(uint *)((int)pGVar23 + 0x23c) =
                   CONCAT13((0 < sVar14) * (sVar14 < 0x100) * (char)((ulonglong)uVar34 >> 0x30) -
                            (0xff < sVar14),
                            CONCAT12((0 < sVar11) * (sVar11 < 0x100) *
                                     (char)((ulonglong)uVar34 >> 0x20) - (0xff < sVar11),
                                     CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                              (char)((ulonglong)uVar34 >> 0x10) - (0xff < sVar8),
                                              (0 < sVar5) * (sVar5 < 0x100) * (char)uVar34 -
                                              (0xff < sVar5))));
              *(uint *)((int)pGVar23 + 0x1c) =
                   CONCAT13((0 < sVar15) * (sVar15 < 0x100) * (char)((ulonglong)uVar35 >> 0x30) -
                            (0xff < sVar15),
                            CONCAT12((0 < sVar12) * (sVar12 < 0x100) *
                                     (char)((ulonglong)uVar35 >> 0x20) - (0xff < sVar12),
                                     CONCAT11((0 < sVar9) * (sVar9 < 0x100) *
                                              (char)((ulonglong)uVar35 >> 0x10) - (0xff < sVar9),
                                              (0 < sVar6) * (sVar6 < 0x100) * (char)uVar35 -
                                              (0xff < sVar6))));
              uVar29 = 0x5000 - DAT_004ccfac;
              if ((int)uVar29 < 0) {
                uVar29 = 0;
              }
              uVar24 = 0x5000 - DAT_004ccfc4;
              if ((int)uVar24 < 0) {
                uVar24 = 0;
              }
              uVar27 = 0x5000 - DAT_004ccfdc;
              if ((int)uVar27 < 0) {
                uVar27 = 0;
              }
              uVar29 = uVar29 >> 6;
              uVar24 = uVar24 >> 6;
              uVar27 = uVar27 >> 6;
              if (0xff < uVar29) {
                uVar29 = 0xff;
              }
              if (0xff < uVar24) {
                uVar24 = 0xff;
              }
              if (0xff < uVar27) {
                uVar27 = 0xff;
              }
              uVar33 = pmulhw(*(undefined8 *)(uVar29 * 8 + 0x41ee80),uVar36);
              uVar34 = pmulhw(*(undefined8 *)(uVar24 * 8 + 0x41ee80),uVar36);
              uVar36 = pmulhw(*(undefined8 *)(uVar27 * 8 + 0x41ee80),uVar36);
              sVar4 = (short)uVar33;
              sVar7 = (short)((ulonglong)uVar33 >> 0x10);
              sVar10 = (short)((ulonglong)uVar33 >> 0x20);
              sVar13 = (short)((ulonglong)uVar33 >> 0x30);
              sVar5 = (short)uVar34;
              sVar8 = (short)((ulonglong)uVar34 >> 0x10);
              sVar11 = (short)((ulonglong)uVar34 >> 0x20);
              sVar14 = (short)((ulonglong)uVar34 >> 0x30);
              sVar6 = (short)uVar36;
              sVar9 = (short)((ulonglong)uVar36 >> 0x10);
              sVar12 = (short)((ulonglong)uVar36 >> 0x20);
              sVar15 = (short)((ulonglong)uVar36 >> 0x30);
              *(uint *)((int)pGVar23 + 0x5c) =
                   CONCAT13((0 < sVar13) * (sVar13 < 0x100) * (char)((ulonglong)uVar33 >> 0x30) -
                            (0xff < sVar13),
                            CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                                     (char)((ulonglong)uVar33 >> 0x20) - (0xff < sVar10),
                                     CONCAT11((0 < sVar7) * (sVar7 < 0x100) *
                                              (char)((ulonglong)uVar33 >> 0x10) - (0xff < sVar7),
                                              (0 < sVar4) * (sVar4 < 0x100) * (char)uVar33 -
                                              (0xff < sVar4))));
              *(uint *)((int)pGVar23 + 0x3c) =
                   CONCAT13((0 < sVar14) * (sVar14 < 0x100) * (char)((ulonglong)uVar34 >> 0x30) -
                            (0xff < sVar14),
                            CONCAT12((0 < sVar11) * (sVar11 < 0x100) *
                                     (char)((ulonglong)uVar34 >> 0x20) - (0xff < sVar11),
                                     CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                              (char)((ulonglong)uVar34 >> 0x10) - (0xff < sVar8),
                                              (0 < sVar5) * (sVar5 < 0x100) * (char)uVar34 -
                                              (0xff < sVar5))));
              *(uint *)((int)pGVar23 + 0x9c) =
                   CONCAT13((0 < sVar15) * (sVar15 < 0x100) * (char)((ulonglong)uVar36 >> 0x30) -
                            (0xff < sVar15),
                            CONCAT12((0 < sVar12) * (sVar12 < 0x100) *
                                     (char)((ulonglong)uVar36 >> 0x20) - (0xff < sVar12),
                                     CONCAT11((0 < sVar9) * (sVar9 < 0x100) *
                                              (char)((ulonglong)uVar36 >> 0x10) - (0xff < sVar9),
                                              (0 < sVar6) * (sVar6 < 0x100) * (char)uVar36 -
                                              (0xff < sVar6))));
              *(GraphicsTextureSetEntry **)((int)pGVar23 + 100) =
                   g_GraphicsShadingTextureSet->entries + DAT_004cce14;
              GVar39 = *pGVar23;
              GVar1 = pGVar23[1];
              GVar2 = pGVar23[2];
              GVar3 = pGVar23[3];
              pGVar23[0x20] = GVar39;
              pGVar23[0x21] = GVar1;
              pGVar23[0x22] = GVar2;
              pGVar23[0x23] = GVar3;
              pGVar23[0x50] = GVar39;
              pGVar23[0x51] = GVar1;
              pGVar23[0x52] = GVar2;
              pGVar23[0x53] = GVar3;
              pGVar23[0x60] = GVar39;
              pGVar23[0x61] = GVar1;
              pGVar23[0x62] = GVar2;
              pGVar23[99] = GVar3;
              pGVar23[0xd0] = GVar39;
              pGVar23[0xd1] = GVar1;
              pGVar23[0xd2] = GVar2;
              pGVar23[0xd3] = GVar3;
              GVar39 = pGVar23[4];
              GVar1 = pGVar23[5];
              GVar2 = pGVar23[6];
              GVar3 = pGVar23[7];
              pGVar23[0x14] = GVar39;
              pGVar23[0x15] = GVar1;
              pGVar23[0x16] = GVar2;
              pGVar23[0x17] = GVar3;
              pGVar23[0x24] = GVar39;
              pGVar23[0x25] = GVar1;
              pGVar23[0x26] = GVar2;
              pGVar23[0x27] = GVar3;
              pGVar23[0x34] = GVar39;
              pGVar23[0x35] = GVar1;
              pGVar23[0x36] = GVar2;
              pGVar23[0x37] = GVar3;
              pGVar23[0x74] = GVar39;
              pGVar23[0x75] = GVar1;
              pGVar23[0x76] = GVar2;
              pGVar23[0x77] = GVar3;
              pGVar23[0x84] = GVar39;
              pGVar23[0x85] = GVar1;
              pGVar23[0x86] = GVar2;
              pGVar23[0x87] = GVar3;
              GVar39 = pGVar23[8];
              GVar1 = pGVar23[9];
              GVar2 = pGVar23[10];
              GVar3 = pGVar23[0xb];
              pGVar23[0x18] = GVar39;
              pGVar23[0x19] = GVar1;
              pGVar23[0x1a] = GVar2;
              pGVar23[0x1b] = GVar3;
              pGVar23[0x48] = GVar39;
              pGVar23[0x49] = GVar1;
              pGVar23[0x4a] = GVar2;
              pGVar23[0x4b] = GVar3;
              pGVar23[0x58] = GVar39;
              pGVar23[0x59] = GVar1;
              pGVar23[0x5a] = GVar2;
              pGVar23[0x5b] = GVar3;
              pGVar23[0xb8] = GVar39;
              pGVar23[0xb9] = GVar1;
              pGVar23[0xba] = GVar2;
              pGVar23[0xbb] = GVar3;
              pGVar23[200] = GVar39;
              pGVar23[0xc9] = GVar1;
              pGVar23[0xca] = GVar2;
              pGVar23[0xcb] = GVar3;
              GVar39 = pGVar23[0x10];
              GVar1 = pGVar23[0x11];
              GVar2 = pGVar23[0x12];
              GVar3 = pGVar23[0x13];
              pGVar23[0x30] = GVar39;
              pGVar23[0x31] = GVar1;
              pGVar23[0x32] = GVar2;
              pGVar23[0x33] = GVar3;
              pGVar23[0x40] = GVar39;
              pGVar23[0x41] = GVar1;
              pGVar23[0x42] = GVar2;
              pGVar23[0x43] = GVar3;
              pGVar23[0x90] = GVar39;
              pGVar23[0x91] = GVar1;
              pGVar23[0x92] = GVar2;
              pGVar23[0x93] = GVar3;
              pGVar23[0xa0] = GVar39;
              pGVar23[0xa1] = GVar1;
              pGVar23[0xa2] = GVar2;
              pGVar23[0xa3] = GVar3;
              pGVar23[0x68] = pGVar23[0x28];
              pGVar23[0x69] = pGVar23[0x29];
              pGVar23[0x6a] = pGVar23[0x2a];
              pGVar23[0x6b] = pGVar23[0x2b];
              pGVar23[0x78] = pGVar23[0x28];
              pGVar23[0x79] = pGVar23[0x29];
              pGVar23[0x7a] = pGVar23[0x2a];
              pGVar23[0x7b] = pGVar23[0x2b];
              pGVar23[0x88] = pGVar23[0x38];
              pGVar23[0x89] = pGVar23[0x39];
              pGVar23[0x8a] = pGVar23[0x3a];
              pGVar23[0x8b] = pGVar23[0x3b];
              pGVar23[0x98] = pGVar23[0x38];
              pGVar23[0x99] = pGVar23[0x39];
              pGVar23[0x9a] = pGVar23[0x3a];
              pGVar23[0x9b] = pGVar23[0x3b];
              pGVar23[0xa4] = pGVar23[0x44];
              pGVar23[0xa5] = pGVar23[0x45];
              pGVar23[0xa6] = pGVar23[0x46];
              pGVar23[0xa7] = pGVar23[0x47];
              pGVar23[0xb4] = pGVar23[0x44];
              pGVar23[0xb5] = pGVar23[0x45];
              pGVar23[0xb6] = pGVar23[0x46];
              pGVar23[0xb7] = pGVar23[0x47];
              pGVar23[0xc4] = pGVar23[0x54];
              pGVar23[0xc5] = pGVar23[0x55];
              pGVar23[0xc6] = pGVar23[0x56];
              pGVar23[199] = pGVar23[0x57];
              pGVar23[0xd4] = pGVar23[0x54];
              pGVar23[0xd5] = pGVar23[0x55];
              pGVar23[0xd6] = pGVar23[0x56];
              pGVar23[0xd7] = pGVar23[0x57];
              pGVar23[0xd8] = pGVar23[100];
              pGVar23[0xd9] = pGVar23[0x65];
              pGVar23[0xda] = pGVar23[0x66];
              pGVar23[0xdb] = pGVar23[0x67];
              pGVar23[0x80] = pGVar23[0x70];
              pGVar23[0x81] = pGVar23[0x71];
              pGVar23[0x82] = pGVar23[0x72];
              pGVar23[0x83] = pGVar23[0x73];
              pGVar23[0x94] = pGVar23[0xa8];
              pGVar23[0x95] = pGVar23[0xa9];
              pGVar23[0x96] = pGVar23[0xaa];
              pGVar23[0x97] = pGVar23[0xab];
              pGVar23[0xc0] = pGVar23[0xb0];
              pGVar23[0xc1] = pGVar23[0xb1];
              pGVar23[0xc2] = pGVar23[0xb2];
              pGVar23[0xc3] = pGVar23[0xb3];
              GVar39 = pGVar23[0xc];
              GVar1 = pGVar23[0xd];
              GVar2 = pGVar23[0xe];
              GVar3 = pGVar23[0xf];
              pGVar23[0x1c] = GVar39;
              pGVar23[0x1d] = GVar1;
              pGVar23[0x1e] = GVar2;
              pGVar23[0x1f] = GVar3;
              pGVar23[0x2c] = GVar39;
              pGVar23[0x2d] = GVar1;
              pGVar23[0x2e] = GVar2;
              pGVar23[0x2f] = GVar3;
              pGVar23[0x3c] = GVar39;
              pGVar23[0x3d] = GVar1;
              pGVar23[0x3e] = GVar2;
              pGVar23[0x3f] = GVar3;
              pGVar23[0x4c] = GVar39;
              pGVar23[0x4d] = GVar1;
              pGVar23[0x4e] = GVar2;
              pGVar23[0x4f] = GVar3;
              pGVar23[0x5c] = GVar39;
              pGVar23[0x5d] = GVar1;
              pGVar23[0x5e] = GVar2;
              pGVar23[0x5f] = GVar3;
              pGVar23[0x6c] = GVar39;
              pGVar23[0x6d] = GVar1;
              pGVar23[0x6e] = GVar2;
              pGVar23[0x6f] = GVar3;
              pGVar23[0x7c] = GVar39;
              pGVar23[0x7d] = GVar1;
              pGVar23[0x7e] = GVar2;
              pGVar23[0x7f] = GVar3;
              pGVar23[0x8c] = GVar39;
              pGVar23[0x8d] = GVar1;
              pGVar23[0x8e] = GVar2;
              pGVar23[0x8f] = GVar3;
              pGVar23[0x9c] = GVar39;
              pGVar23[0x9d] = GVar1;
              pGVar23[0x9e] = GVar2;
              pGVar23[0x9f] = GVar3;
              pGVar23[0xac] = GVar39;
              pGVar23[0xad] = GVar1;
              pGVar23[0xae] = GVar2;
              pGVar23[0xaf] = GVar3;
              pGVar23[0xbc] = GVar39;
              pGVar23[0xbd] = GVar1;
              pGVar23[0xbe] = GVar2;
              pGVar23[0xbf] = GVar3;
              pGVar23[0xcc] = GVar39;
              pGVar23[0xcd] = GVar1;
              pGVar23[0xce] = GVar2;
              pGVar23[0xcf] = GVar3;
              pGVar23[0xdc] = GVar39;
              pGVar23[0xdd] = GVar1;
              pGVar23[0xde] = GVar2;
              pGVar23[0xdf] = GVar3;
              GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set(param_1);
              if (iVar28 != 0) {
                GraphicsShadingGeneratedTexture_FilterGridScratchMmx();
              }
              GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear(param_1);
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
void GraphicsIntensityClampTable_InitializeCf(void)

{
  void *pvVar1;
  int iVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  char *pcVar6;
  undefined1 in_CF;
  
  pvVar1 = (*g_MemoryApi.alloc)(0x20000);
  if (!(bool)in_CF) {
    uVar4 = 0;
    pcVar6 = (char *)((int)pvVar1 + 0xffffU & 0xffff0000);
    iVar2 = 0x100;
    iVar5 = 0;
    g_GraphicsIntensityClampTableBase = pcVar6;
    do {
      do {
        cVar3 = (char)uVar4;
        if (iVar5 < (int)uVar4) {
          if (iVar5 + 0x15 < (int)uVar4) {
            *pcVar6 = (char)(iVar5 + 0x15);
          }
          else {
            *pcVar6 = cVar3;
          }
        }
        else if ((int)uVar4 < iVar5 + -0x15) {
          *pcVar6 = (char)(iVar5 + -0x15);
        }
        else {
          *pcVar6 = cVar3;
        }
        pcVar6 = pcVar6 + 1;
        uVar4 = (uint)(byte)(cVar3 + 1U);
      } while ((byte)(cVar3 + 1U) != 0);
      iVar5 = iVar5 + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    return;
  }
  return;
}

/* Address: 0x004CCA90.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading runtime accumulate compact lighting at point mmx register result.
*/
void __fastcall
GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs(undefined4 param_1,int *param_2)

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
  undefined8 in_MM0;
  undefined1 uVar11;
  undefined1 uVar12;
  undefined8 uVar10;
  
  shadingRecord1 = g_GraphicsShadingCompactRecords;
  for (GVar8 = g_GraphicsShadingCompactRecordCount; GVar8 != 0; GVar8 = GVar8 - 1) {
    if (shadingRecord1->targetRadiusQ12 != 0) {
      uVar7 = (uint)shadingRecord1->squaredRadiusQ24;
      iVar4 = *param_2 - shadingRecord1->worldXQ12;
      lVar2 = (longlong)iVar4 * (longlong)iVar4;
      uVar5 = (uint)lVar2;
      uVar9 = uVar7 - uVar5;
      iVar4 = (*(int *)((int)&shadingRecord1->squaredRadiusQ24 + 4) -
              (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar7 < uVar5);
      if (-1 < iVar4) {
        iVar6 = param_2[1] - shadingRecord1->worldYQ12;
        lVar2 = (longlong)iVar6 * (longlong)iVar6;
        uVar7 = (uint)lVar2;
        uVar5 = uVar9 - uVar7;
        iVar4 = (iVar4 - (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar9 < uVar7);
        if (-1 < iVar4) {
          iVar6 = param_2[2] - shadingRecord1->worldZQ12;
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
              in_MM0 = paddusw(in_MM0,uVar10);
            }
          }
        }
      }
    }
    shadingRecord1 = shadingRecord1 + 1;
  }
  return;
}

/* Address: 0x004CCB40.
   Ownership: graphics/render/shading.
   Purpose: Allocates one free GraphicsShadingRuntimeRecord. transitionDurationTicks controls radius interpolation;
   EAX and CF remain the nonstandard pointer/status result channels.
*/
longlong GraphicsShadingRuntime_AllocateRecordRegs
                   (GraphicsTransitionTickCount transitionDurationTicks,GraphicsRadiusQ12 radiusQ12,
                   PackedRgb24 packedColorRgb,GraphicsWorldCoordinateQ12 worldZQ12,
                   GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12)

{
  int recordsRemaining;
  uint in_EDX;
  GraphicsShadingRuntimeRecord *recordCursor;
  
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
        return CONCAT44(in_EDX,recordCursor);
      }
      recordCursor = recordCursor + 1;
      recordsRemaining = recordsRemaining + -1;
    } while (recordsRemaining != 0);
  }
  return (ulonglong)in_EDX << 0x20;
}

/* Address: 0x004CCC60.
   Ownership: graphics/render/shading.
   Purpose: Clears all 256 GraphicsShadingRuntimeRecord entries, exactly 0x4000 bytes. Preserved EAX is incidental,
   not a normal return value.
*/
void __cdecl GraphicsShadingRuntime_ClearRecordTable(void)

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
void __cdecl GraphicsShadingRuntime_RebuildCompactLightingRecords(void)

{
  GraphicsRadiusQ12 GVar1;
  undefined4 uVar2;
  GraphicsShadingRecordCount GVar3;
  int extraout_EDX;
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
      GVar3 = extraout_EDX + 1;
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
void GraphicsShadingRuntime_CollectNearbyRecords
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
undefined8
GraphicsShadingRuntime_InitializeGeneratedTextureCf
          (GraphicsAssetSubresourceCount subresourceCount,GraphicsPixelDimension gridHalfSize,
          GraphicsPixelDimension textureDimension)

{
  longlong lVar1;
  GraphicsGeneratedTextureAssetOrEntryView200 *pGVar2;
  AssetMagic AVar2;
  GraphicsTextureSet *pGVar3;
  dword bytes;
  uint extraout_ECX;
  uint uVar4;
  uint extraout_ECX_00;
  undefined4 in_EDX;
  int iVar5;
  AssetRelativeOffset PVar7;
  GraphicsGeneratedTextureAssetOrEntryView200 *pGVar6;
  bool bVar7;
  
  lVar1 = (longlong)(int)(gridHalfSize * 2) * (longlong)(int)(gridHalfSize * 2);
  bytes = (dword)lVar1;
  bVar7 = (int)bytes != lVar1;
  pGVar2 = (*g_MemoryApi.alloc)(bytes);
  if (!bVar7) {
    uVar4 = extraout_ECX >> 2;
    g_GraphicsShadingGridScratchInterior = (undefined *)((int)pGVar2 + uVar4 + (gridHalfSize >> 1));
    g_GraphicsShadingGridScratch = pGVar2;
    for (; uVar4 != 0; uVar4 = uVar4 - 1) {
      (pGVar2->asset).common.magic = 0;
      pGVar2 = (GraphicsGeneratedTextureAssetOrEntryView200 *)
               &(pGVar2->asset).common.allocationSizeBytes;
    }
    uVar4 = (textureDimension * textureDimension + 0x20) * subresourceCount;
    bVar7 = 0xfffff5ff < uVar4;
    pGVar2 = (*g_MemoryApi.alloc)(uVar4 + 0xa00);
    if (!bVar7) {
      pGVar6 = pGVar2;
      g_GraphicsShadingGeneratedAsset = (GraphicsTextureSourceAsset *)pGVar2;
      for (uVar4 = extraout_ECX_00 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
        (pGVar6->asset).common.magic = 0;
        pGVar6 = (GraphicsGeneratedTextureAssetOrEntryView200 *)
                 &(pGVar6->asset).common.allocationSizeBytes;
      }
      (pGVar2->asset).common.magic = ASSET_MAGIC_GFX;
      (pGVar2->asset).common.allocationSizeBytes = extraout_ECX_00;
      (pGVar2->asset).tableDescriptor.subresourceCount = subresourceCount;
      (pGVar2->asset).tableDescriptor.paletteBankCount = 1;
      (pGVar2->asset).tableDescriptor.subresourceTableOffset = 0xa00;
      AVar2 = 0xffffff;
      pGVar6 = pGVar2 + 1;
      iVar5 = 0x100;
      do {
        (pGVar6->asset).common.magic = AVar2;
        AVar2 = AVar2 + 0x1000000;
        pGVar6 = (GraphicsGeneratedTextureAssetOrEntryView200 *)
                 &(pGVar6->asset).common.formatVersion;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      g_GraphicsShadingSubresourceCount = subresourceCount;
      pGVar6 = pGVar2 + 5;
      PVar7 = subresourceCount * 0x20 + 0xa00;
      do {
        (pGVar6->asset).common.magic = textureDimension;
        (pGVar6->asset).common.allocationSizeBytes = textureDimension;
        (pGVar6->asset).common.formatVersion = 0;
        (pGVar6->sourceEntry).dataOffset = PVar7;
        (pGVar6->asset).common.buildMetadata.timestamps.dateValue0 = 0;
        (pGVar6->asset).common.buildMetadata.timestamps.timeValue0 = 0;
        (pGVar6->asset).common.buildMetadata.timestamps.dateValue1 = textureDimension;
        (pGVar6->asset).common.buildMetadata.timestamps.timeValue1 = textureDimension;
        PVar7 = PVar7 + textureDimension * textureDimension;
        pGVar6 = (GraphicsGeneratedTextureAssetOrEntryView200 *)
                 &(pGVar6->asset).common.buildMetadata.timestamps.dateValue2;
        subresourceCount = subresourceCount - 1;
      } while (subresourceCount != 0);
      g_GraphicsShadingTextureDimension = textureDimension;
      g_GraphicsShadingGridHalfSize = gridHalfSize;
      g_GraphicsShadingGridStepQ20 =
           (dword)(((ulonglong)gridHalfSize * 0x100000) / (ulonglong)textureDimension);
      iVar5 = ((int)gridHalfSize >> 1) + -1;
      g_GraphicsShadingPositiveGridOriginQ12 = iVar5 * 0x1000;
      g_GraphicsShadingNegativeGridOriginQ12 = iVar5 * -0x1000;
      bVar7 = iVar5 * -0x800 < 0;
      g_GraphicsShadingGridStepQ20Current = g_GraphicsShadingGridStepQ20;
      pGVar3 = (*g_GraphicsCreateTextureSet)(g_GraphicsShadingGeneratedAsset);
      if (!bVar7) {
        g_GraphicsShadingTextureSet = pGVar3;
        return CONCAT44(in_EDX,pGVar3);
      }
      pGVar2 = (GraphicsGeneratedTextureAssetOrEntryView200 *)g_GraphicsShadingGeneratedAsset;
      (*g_MemoryApi.free)(g_GraphicsShadingGeneratedAsset);
    }
  }
  return CONCAT44(in_EDX,pGVar2);
}

/* Address: 0x004CD1B0.
   Ownership: graphics/render/shading.
   Purpose: Destroys the generated shading texture set, frees the generated gfx allocation and square grid
   allocation, and clears all three published pointers. EAX is preserved.
*/
undefined4 __cdecl GraphicsShadingRuntime_Shutdown(void)

{
  undefined4 in_EAX;
  
  (*g_GraphicsDestroyTextureSet)(g_GraphicsShadingTextureSet);
  g_GraphicsShadingTextureSet = (GraphicsTextureSet *)0x0;
  (*g_MemoryApi.free)(g_GraphicsShadingGeneratedAsset);
  g_GraphicsShadingGeneratedAsset = (GraphicsTextureSourceAsset *)0x0;
  (*g_MemoryApi.free)(g_GraphicsShadingGridScratch);
  g_GraphicsShadingGridScratch = (void *)0x0;
  return in_EAX;
}

/* Address: 0x004CD200.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture reset pass scratch and clear alpha planes.
*/
void __cdecl GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes(void)

{
  AssetRelativeOffset AVar1;
  uint uVar2;
  byte *pbVar3;
  
  DAT_004cce58 = g_TextureDownsampleShift << 2;
  DAT_004cce08 = (g_GraphicsShadingGeneratedAsset->common).buildMetadata.
                 assetRelativeAddressAnchor28 +
                 ((g_GraphicsShadingTextureDimension + 1) * g_GraphicsShadingGridHalfSize >> 1) +
                 *(int *)((g_GraphicsShadingGeneratedAsset->common).buildMetadata.
                          assetRelativeAddressAnchor28 +
                         ((g_GraphicsShadingGeneratedAsset->tableDescriptor).subresourceTableOffset
                         - 0x1c)) + -0x28;
  DAT_004cce0c = 0;
  DAT_004cce10 = 0;
  DAT_004cce14 = 0;
  DAT_004cce1c = 0;
  DAT_004cce20 = 0;
  DAT_004cce3c = 0;
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
void GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources(void)

{
  dword arg0;
  int iVar1;
  int extraout_ECX;
  
  arg0 = 0;
  if (DAT_004cce14 != 0) {
    do {
      iVar1 = (*g_GraphicsRefreshTextureAlpha)(arg0,g_GraphicsShadingTextureSet);
      arg0 = iVar1 + 1;
    } while (extraout_ECX != 1);
  }
  if ((DAT_004cce3c == 0) && ((DAT_004cce0c != 0 || (DAT_004cce10 != 0)))) {
    (*g_GraphicsRefreshTextureAlpha)(arg0,g_GraphicsShadingTextureSet);
  }
  return;
}

/* Address: 0x004D1170.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture reserve one projected point block carry-flag result.
*/
void GraphicsShadingGeneratedTexture_ReserveOneProjectedPointBlockCf(int param_1)

{
  uint *puVar1;
  uint uVar2;
  
  puVar1 = *(uint **)(param_1 + 200);
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
void GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear(int param_1)

{
  int extraout_ECX;
  int extraout_EDX;
  int extraout_EDX_00;
  int *piVar1;
  
  if (((*(uint *)(param_1 + 0x10) & 1) == 0) &&
     (piVar1 = (int *)(param_1 + 0x20), *(int *)(param_1 + 8) != 0)) {
    do {
      GraphicsShadingGeneratedTexture_TransformPointXYQuantized
                ((uint *)(piVar1 + 8),piVar1,(int *)0x4cce60);
      piVar1 = piVar1 + 0x10;
    } while (extraout_ECX != 1);
    if (extraout_EDX != 0) {
      do {
        GraphicsShadingGeneratedTexture_RasterizeTriangleMask
                  ((int *)(piVar1[6] + 0x20),(int *)(piVar1[3] + 0x20),(int *)(*piVar1 + 0x20));
        piVar1 = piVar1 + 0x10;
      } while (extraout_EDX_00 != 1);
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
void GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear(int param_1)

{
  int iVar1;
  int extraout_EAX;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_EDX;
  int iVar2;
  int extraout_EDX_00;
  int iVar3;
  int *piVar4;
  
  iVar2 = DAT_004ccfe8;
  iVar3 = DAT_004ccfe4;
  *(int *)(param_1 + 0x94) = *(int *)(param_1 + 0x94) - DAT_004ccfe0;
  *(int *)(param_1 + 0x98) = *(int *)(param_1 + 0x98) - iVar3;
  *(int *)(param_1 + 0x9c) = *(int *)(param_1 + 0x9c) - iVar2;
  GraphicsShadingGeneratedTexture_ComposeTransform
            ((uint *)0x4cce60,(int *)(param_1 + 0x70),&DAT_004cce90);
  *(int *)(param_1 + 0x94) = *(int *)(param_1 + 0x94) + extraout_EAX;
  *(int *)(param_1 + 0x98) = *(int *)(param_1 + 0x98) + extraout_ECX;
  *(int *)(param_1 + 0x9c) = *(int *)(param_1 + 0x9c) + extraout_EDX;
  iVar3 = *(int *)(*(int *)(param_1 + 0x40) + 0xec);
  if ((iVar3 != 0) &&
     (iVar3 = *(int *)(param_1 + 0x40) + iVar3, piVar4 = (int *)(iVar3 + 0x20),
     *(int *)(iVar3 + 4) != 0)) {
    do {
      GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear((int)piVar4);
      piVar4 = (int *)((int)piVar4 + *piVar4);
    } while (extraout_ECX_00 != 1);
  }
  iVar2 = 0;
  for (iVar3 = *(int *)(param_1 + 200); iVar3 != 0; iVar3 = iVar3 + -1) {
    iVar1 = *(int *)(param_1 + 0xcc + iVar2 * 4);
    if (iVar1 != 0) {
      GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear(iVar1);
      iVar3 = extraout_ECX_01;
      iVar2 = extraout_EDX_00;
    }
    iVar2 = iVar2 + 1;
  }
  return;
}

/* Address: 0x004CD9F0.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture rasterize record batch flag0 set.
   Local calls: GraphicsShadingGeneratedTexture_TransformPointXYQuantized,
   GraphicsShadingGeneratedTexture_RasterizeTriangleMask.
*/
void GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set(int param_1)

{
  int extraout_ECX;
  int extraout_EDX;
  int extraout_EDX_00;
  int *piVar1;
  
  if (((*(uint *)(param_1 + 0x10) & 1) != 0) &&
     (piVar1 = (int *)(param_1 + 0x20), *(int *)(param_1 + 8) != 0)) {
    do {
      GraphicsShadingGeneratedTexture_TransformPointXYQuantized
                ((uint *)(piVar1 + 8),piVar1,(int *)0x4cce60);
      piVar1 = piVar1 + 0x10;
    } while (extraout_ECX != 1);
    if (extraout_EDX != 0) {
      do {
        GraphicsShadingGeneratedTexture_RasterizeTriangleMask
                  ((int *)(piVar1[6] + 0x20),(int *)(piVar1[3] + 0x20),(int *)(*piVar1 + 0x20));
        piVar1 = piVar1 + 0x10;
      } while (extraout_EDX_00 != 1);
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
void GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set(int param_1)

{
  int iVar1;
  int extraout_EAX;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_EDX;
  int iVar2;
  int extraout_EDX_00;
  int iVar3;
  int *piVar4;
  
  iVar2 = DAT_004ccfe8;
  iVar3 = DAT_004ccfe4;
  *(int *)(param_1 + 0x94) = *(int *)(param_1 + 0x94) - DAT_004ccfe0;
  *(int *)(param_1 + 0x98) = *(int *)(param_1 + 0x98) - iVar3;
  *(int *)(param_1 + 0x9c) = *(int *)(param_1 + 0x9c) - iVar2;
  GraphicsShadingGeneratedTexture_ComposeTransform
            ((uint *)0x4cce60,(int *)(param_1 + 0x70),&DAT_004cce90);
  *(int *)(param_1 + 0x94) = *(int *)(param_1 + 0x94) + extraout_EAX;
  *(int *)(param_1 + 0x98) = *(int *)(param_1 + 0x98) + extraout_ECX;
  *(int *)(param_1 + 0x9c) = *(int *)(param_1 + 0x9c) + extraout_EDX;
  iVar3 = *(int *)(*(int *)(param_1 + 0x40) + 0xec);
  if ((iVar3 != 0) &&
     (iVar3 = *(int *)(param_1 + 0x40) + iVar3, piVar4 = (int *)(iVar3 + 0x20),
     *(int *)(iVar3 + 4) != 0)) {
    do {
      GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set((int)piVar4);
      piVar4 = (int *)((int)piVar4 + *piVar4);
    } while (extraout_ECX_00 != 1);
  }
  iVar2 = 0;
  for (iVar3 = *(int *)(param_1 + 200); iVar3 != 0; iVar3 = iVar3 + -1) {
    iVar1 = *(int *)(param_1 + 0xcc + iVar2 * 4);
    if (iVar1 != 0) {
      GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set(iVar1);
      iVar3 = extraout_ECX_01;
      iVar2 = extraout_EDX_00;
    }
    iVar2 = iVar2 + 1;
  }
  return;
}

/* Address: 0x004CDB80.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture accumulate projected bounds from records.
   Local calls: GraphicsShadingGeneratedTexture_TransformPointXY.
*/
void GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords(int param_1)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int *piVar3;
  
  piVar3 = (int *)(param_1 + 0x20);
  if (*(int *)(param_1 + 8) != 0) {
    do {
      GraphicsShadingGeneratedTexture_TransformPointXY(piVar3 + 8,piVar3,(int *)0x4cce60);
      iVar1 = piVar3[8];
      iVar2 = piVar3[9];
      piVar3 = piVar3 + 0x10;
      if (iVar1 < DAT_004cce48) {
        DAT_004cce48 = iVar1;
      }
      if (iVar2 < DAT_004cce4c) {
        DAT_004cce4c = iVar2;
      }
      if (DAT_004cce50 < iVar1) {
        DAT_004cce50 = iVar1;
      }
      if (DAT_004cce54 < iVar2) {
        DAT_004cce54 = iVar2;
      }
    } while (extraout_ECX != 1);
  }
  return;
}

/* Address: 0x004CDC20.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture traverse hierarchy and accumulate projected bounds.
   Local calls: GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords.
   Cross-module calls: FixedTransform_Compose [core/math/fixed].
*/
void GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(int param_1)

{
  int iVar1;
  int extraout_EAX;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_EDX;
  int iVar2;
  int extraout_EDX_00;
  int iVar3;
  int *piVar4;
  
  iVar2 = DAT_004ccfe8;
  iVar3 = DAT_004ccfe4;
  *(int *)(param_1 + 0x94) = *(int *)(param_1 + 0x94) - DAT_004ccfe0;
  *(int *)(param_1 + 0x98) = *(int *)(param_1 + 0x98) - iVar3;
  *(int *)(param_1 + 0x9c) = *(int *)(param_1 + 0x9c) - iVar2;
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)0x4cce60,(GraphicsFixedMatrix3x4 *)(param_1 + 0x70),
             &g_AuxiliaryRotationMatrixFixed);
  *(int *)(param_1 + 0x94) = *(int *)(param_1 + 0x94) + extraout_EAX;
  *(int *)(param_1 + 0x98) = *(int *)(param_1 + 0x98) + extraout_ECX;
  *(int *)(param_1 + 0x9c) = *(int *)(param_1 + 0x9c) + extraout_EDX;
  iVar3 = *(int *)(*(int *)(param_1 + 0x40) + 0xec);
  if ((iVar3 != 0) &&
     (iVar3 = *(int *)(param_1 + 0x40) + iVar3, piVar4 = (int *)(iVar3 + 0x20),
     *(int *)(iVar3 + 4) != 0)) {
    do {
      GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords((int)piVar4);
      piVar4 = (int *)((int)piVar4 + *piVar4);
    } while (extraout_ECX_00 != 1);
  }
  iVar2 = 0;
  for (iVar3 = *(int *)(param_1 + 200); iVar3 != 0; iVar3 = iVar3 + -1) {
    iVar1 = *(int *)(param_1 + 0xcc + iVar2 * 4);
    if (iVar1 != 0) {
      GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(iVar1);
      iVar3 = extraout_ECX_01;
      iVar2 = extraout_EDX_00;
    }
    iVar2 = iVar2 + 1;
  }
  return;
}

/* Address: 0x00485020.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture transform point xy.
*/
void GraphicsShadingGeneratedTexture_TransformPointXY(int *param_1,int *param_2,int *param_3)

{
  int iVar1;
  longlong lVar2;
  
  lVar2 = (longlong)param_3[1] * (longlong)param_2[1] + (longlong)*param_3 * (longlong)*param_2 +
          (longlong)param_3[2] * (longlong)param_2[2];
  iVar1 = param_3[3];
  *param_1 = ((int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c) + param_3[9];
  lVar2 = (longlong)param_3[4] * (longlong)param_2[1] + (longlong)iVar1 * (longlong)*param_2 +
          (longlong)param_3[5] * (longlong)param_2[2];
  param_1[1] = ((int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c) + param_3[10];
  return;
}

/* Address: 0x004CD2B0.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture advance tile cursor.
*/
void GraphicsShadingGeneratedTexture_AdvanceTileCursor(void)

{
  DAT_004cce0c = DAT_004cce0c + g_GraphicsShadingGridHalfSize;
  DAT_004cce08 = DAT_004cce08 + g_GraphicsShadingGridHalfSize;
  DAT_004cce1c = DAT_004cce1c + g_GraphicsShadingGridStepQ20;
  if (g_GraphicsShadingTextureDimension <= DAT_004cce0c) {
    DAT_004cce08 = DAT_004cce08 - g_GraphicsShadingTextureDimension;
    DAT_004cce0c = 0;
    DAT_004cce1c = 0;
    DAT_004cce10 = DAT_004cce10 + g_GraphicsShadingGridHalfSize;
    DAT_004cce20 = DAT_004cce20 + g_GraphicsShadingGridStepQ20;
    DAT_004cce08 = DAT_004cce08 + g_GraphicsShadingGridHalfSize * g_GraphicsShadingTextureDimension;
    if (g_GraphicsShadingTextureDimension <= DAT_004cce10) {
      DAT_004cce14 = DAT_004cce14 + 1;
      DAT_004cce10 = 0;
      DAT_004cce20 = 0;
      if (g_GraphicsShadingSubresourceCount <= DAT_004cce14) {
        DAT_004cce3c = DAT_004cce3c + 1;
      }
    }
  }
  return;
}

/* Address: 0x004CD3D0.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture filter grid scratch mmx.
*/
void __cdecl GraphicsShadingGeneratedTexture_FilterGridScratchMmx(void)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  ulonglong *puVar5;
  undefined8 *puVar6;
  longlong *plVar7;
  ulonglong *puVar8;
  ulonglong *puVar9;
  uint uVar10;
  bool bVar11;
  undefined8 uVar12;
  ulonglong uVar13;
  undefined8 uVar14;
  ulonglong uVar15;
  undefined8 uVar16;
  ulonglong uVar17;
  undefined8 uVar18;
  ulonglong uVar19;
  undefined8 uVar20;
  undefined8 uVar21;
  undefined8 uVar22;
  undefined8 uVar23;
  
  uVar13 = _DAT_0041f6c8;
  uVar10 = g_GraphicsShadingGridHalfSize >> 4;
  puVar8 = (ulonglong *)
           ((DAT_004cce08 - g_GraphicsShadingTextureDimension * (g_GraphicsShadingGridHalfSize >> 1)
            ) - (g_GraphicsShadingGridHalfSize >> 1));
  uVar3 = g_GraphicsShadingGridHalfSize;
  uVar4 = g_GraphicsShadingGridHalfSize;
  puVar5 = (ulonglong *)g_GraphicsShadingGridScratchInterior;
  puVar9 = puVar8;
  do {
    do {
      uVar15 = puVar9[1];
      uVar17 = puVar9[2];
      uVar19 = puVar9[3];
      *puVar5 = *puVar9 >> 5 & uVar13;
      puVar5[1] = uVar15 >> 5 & uVar13;
      puVar5[2] = uVar17 >> 5 & uVar13;
      puVar5[3] = uVar19 >> 5 & uVar13;
      puVar9 = puVar9 + 4;
      puVar5 = puVar5 + 4;
      bVar11 = 0x1f < uVar3;
      uVar3 = uVar3 - 0x20;
    } while (bVar11 && uVar3 != 0);
    puVar5 = (ulonglong *)((int)puVar5 + g_GraphicsShadingGridHalfSize);
    puVar9 = (ulonglong *)
             ((int)puVar9 + (g_GraphicsShadingTextureDimension - g_GraphicsShadingGridHalfSize));
    uVar4 = uVar4 - 1;
    uVar3 = g_GraphicsShadingGridHalfSize;
  } while (uVar4 != 0);
  iVar2 = g_GraphicsShadingGridHalfSize * 2 * uVar10;
  uVar4 = g_GraphicsShadingGridHalfSize;
  plVar7 = (longlong *)g_GraphicsShadingGridScratchInterior;
  do {
    do {
      uVar12 = paddusb(*plVar7 << 2,*(undefined8 *)((int)plVar7 + uVar10 * 2));
      uVar14 = paddusb(plVar7[1] << 2,*(undefined8 *)((int)plVar7 + uVar10 * 2 + 8));
      uVar16 = paddusb(plVar7[2] << 2,*(undefined8 *)((int)plVar7 + uVar10 * 2 + 0x10));
      uVar18 = paddusb(plVar7[3] << 2,*(undefined8 *)((int)plVar7 + uVar10 * 2 + 0x18));
      puVar6 = (undefined8 *)((int)plVar7 - uVar10);
      uVar20 = paddusb(*(undefined8 *)((int)plVar7 + uVar10),*puVar6);
      uVar21 = paddusb(*(undefined8 *)((int)plVar7 + uVar10 + 8),puVar6[1]);
      uVar22 = paddusb(*(undefined8 *)((int)plVar7 + uVar10 + 0x10),puVar6[2]);
      uVar23 = paddusb(*(undefined8 *)((int)plVar7 + uVar10 + 0x18),puVar6[3]);
      puVar6 = (undefined8 *)((int)puVar6 - uVar10);
      uVar12 = paddusb(uVar12,*puVar6);
      uVar14 = paddusb(uVar14,puVar6[1]);
      uVar16 = paddusb(uVar16,puVar6[2]);
      uVar18 = paddusb(uVar18,puVar6[3]);
      puVar6 = (undefined8 *)((int)puVar6 + (uVar10 * 2 - iVar2));
      uVar20 = paddusb(uVar20,*puVar6);
      uVar21 = paddusb(uVar21,puVar6[1]);
      uVar22 = paddusb(uVar22,puVar6[2]);
      uVar23 = paddusb(uVar23,puVar6[3]);
      uVar12 = paddusb(uVar12,*(undefined8 *)((int)puVar6 + uVar10));
      uVar14 = paddusb(uVar14,*(undefined8 *)((int)puVar6 + uVar10 + 8));
      uVar16 = paddusb(uVar16,*(undefined8 *)((int)puVar6 + uVar10 + 0x10));
      uVar18 = paddusb(uVar18,*(undefined8 *)((int)puVar6 + uVar10 + 0x18));
      uVar12 = paddusb(uVar12,*(undefined8 *)((int)puVar6 + uVar10 * 2));
      uVar14 = paddusb(uVar14,*(undefined8 *)((int)puVar6 + uVar10 * 2 + 8));
      uVar16 = paddusb(uVar16,*(undefined8 *)((int)puVar6 + uVar10 * 2 + 0x10));
      uVar18 = paddusb(uVar18,*(undefined8 *)((int)puVar6 + uVar10 * 2 + 0x18));
      puVar6 = (undefined8 *)((int)puVar6 - uVar10);
      uVar12 = paddusb(uVar12,*puVar6);
      uVar14 = paddusb(uVar14,puVar6[1]);
      uVar16 = paddusb(uVar16,puVar6[2]);
      uVar18 = paddusb(uVar18,puVar6[3]);
      puVar6 = (undefined8 *)((int)puVar6 - uVar10);
      uVar12 = paddusb(uVar12,*puVar6);
      uVar14 = paddusb(uVar14,puVar6[1]);
      uVar16 = paddusb(uVar16,puVar6[2]);
      uVar18 = paddusb(uVar18,puVar6[3]);
      puVar6 = (undefined8 *)((int)puVar6 + iVar2 * 2 + uVar10 * 2);
      uVar20 = paddusb(uVar20,*puVar6);
      uVar21 = paddusb(uVar21,puVar6[1]);
      uVar22 = paddusb(uVar22,puVar6[2]);
      uVar23 = paddusb(uVar23,puVar6[3]);
      uVar12 = paddusb(uVar12,*(undefined8 *)((int)puVar6 + uVar10));
      uVar14 = paddusb(uVar14,*(undefined8 *)((int)puVar6 + uVar10 + 8));
      uVar16 = paddusb(uVar16,*(undefined8 *)((int)puVar6 + uVar10 + 0x10));
      uVar18 = paddusb(uVar18,*(undefined8 *)((int)puVar6 + uVar10 + 0x18));
      uVar12 = paddusb(uVar12,*(undefined8 *)((int)puVar6 + uVar10 * 2));
      uVar14 = paddusb(uVar14,*(undefined8 *)((int)puVar6 + uVar10 * 2 + 8));
      uVar16 = paddusb(uVar16,*(undefined8 *)((int)puVar6 + uVar10 * 2 + 0x10));
      uVar18 = paddusb(uVar18,*(undefined8 *)((int)puVar6 + uVar10 * 2 + 0x18));
      iVar1 = -iVar2 - uVar10;
      uVar12 = paddusb(uVar12,*(undefined8 *)((int)puVar6 + iVar2 + iVar1));
      uVar14 = paddusb(uVar14,*(undefined8 *)((int)puVar6 + iVar2 + 8 + iVar1));
      uVar16 = paddusb(uVar16,*(undefined8 *)((int)puVar6 + iVar2 + 0x10 + iVar1));
      uVar18 = paddusb(uVar18,*(undefined8 *)((int)puVar6 + iVar2 + 0x18 + iVar1));
      iVar1 = (iVar1 - uVar10) - iVar2;
      uVar12 = paddusb(uVar12,*(undefined8 *)((int)puVar6 + iVar2 * 2 + iVar1));
      uVar14 = paddusb(uVar14,*(undefined8 *)((int)puVar6 + iVar2 * 2 + iVar1 + 8));
      uVar16 = paddusb(uVar16,*(undefined8 *)((int)puVar6 + iVar2 * 2 + iVar1 + 0x10));
      uVar18 = paddusb(uVar18,*(undefined8 *)((int)puVar6 + iVar2 * 2 + iVar1 + 0x18));
      uVar12 = paddusb(uVar12,uVar20);
      uVar14 = paddusb(uVar14,uVar21);
      uVar16 = paddusb(uVar16,uVar22);
      uVar18 = paddusb(uVar18,uVar23);
      uVar12 = paddusb(uVar12,uVar20);
      uVar14 = paddusb(uVar14,uVar21);
      uVar16 = paddusb(uVar16,uVar22);
      uVar18 = paddusb(uVar18,uVar23);
      puVar6 = (undefined8 *)((int)puVar6 + uVar10 * 2 + (iVar1 - iVar2));
      uVar12 = paddusb(uVar12,uVar20);
      uVar14 = paddusb(uVar14,uVar21);
      uVar16 = paddusb(uVar16,uVar22);
      uVar18 = paddusb(uVar18,uVar23);
      uVar12 = paddusb(uVar12,*puVar6);
      uVar14 = paddusb(uVar14,puVar6[1]);
      uVar16 = paddusb(uVar16,puVar6[2]);
      uVar18 = paddusb(uVar18,puVar6[3]);
      uVar12 = paddusb(uVar12,*(undefined8 *)((int)puVar6 + uVar10));
      uVar14 = paddusb(uVar14,*(undefined8 *)((int)puVar6 + uVar10 + 8));
      uVar16 = paddusb(uVar16,*(undefined8 *)((int)puVar6 + uVar10 + 0x10));
      uVar18 = paddusb(uVar18,*(undefined8 *)((int)puVar6 + uVar10 + 0x18));
      uVar12 = paddusb(uVar12,*(undefined8 *)((int)puVar6 + iVar2 * 4));
      uVar14 = paddusb(uVar14,*(undefined8 *)((int)puVar6 + iVar2 * 4 + 8));
      uVar16 = paddusb(uVar16,*(undefined8 *)((int)puVar6 + iVar2 * 4 + 0x10));
      uVar18 = paddusb(uVar18,*(undefined8 *)((int)puVar6 + iVar2 * 4 + 0x18));
      puVar6 = (undefined8 *)((int)puVar6 - uVar10);
      uVar12 = paddusb(uVar12,*puVar6);
      uVar14 = paddusb(uVar14,puVar6[1]);
      uVar16 = paddusb(uVar16,puVar6[2]);
      uVar18 = paddusb(uVar18,puVar6[3]);
      uVar12 = paddusb(uVar12,*(undefined8 *)((int)puVar6 + iVar2 * 4));
      uVar14 = paddusb(uVar14,*(undefined8 *)((int)puVar6 + iVar2 * 4 + 8));
      uVar16 = paddusb(uVar16,*(undefined8 *)((int)puVar6 + iVar2 * 4 + 0x10));
      uVar18 = paddusb(uVar18,*(undefined8 *)((int)puVar6 + iVar2 * 4 + 0x18));
      uVar13 = paddusb(uVar12,*(undefined8 *)((int)puVar6 + iVar2 * 4 + uVar10 * 2));
      uVar15 = paddusb(uVar14,*(undefined8 *)((int)puVar6 + iVar2 * 4 + uVar10 * 2 + 8));
      uVar17 = paddusb(uVar16,*(undefined8 *)((int)puVar6 + iVar2 * 4 + uVar10 * 2 + 0x10));
      uVar19 = paddusb(uVar18,*(undefined8 *)((int)puVar6 + iVar2 * 4 + uVar10 * 2 + 0x18));
      *puVar8 = uVar13;
      puVar8[1] = uVar15;
      puVar8[2] = uVar17;
      puVar8[3] = uVar19;
      plVar7 = (longlong *)((int)puVar6 + iVar2 * 2 + uVar10 + 0x20);
      puVar8 = puVar8 + 4;
      bVar11 = 0x1f < uVar3;
      uVar3 = uVar3 - 0x20;
    } while (bVar11 && uVar3 != 0);
    plVar7 = (longlong *)((int)plVar7 + g_GraphicsShadingGridHalfSize);
    puVar8 = (ulonglong *)
             ((int)puVar8 + (g_GraphicsShadingTextureDimension - g_GraphicsShadingGridHalfSize));
    uVar4 = uVar4 - 1;
    uVar3 = g_GraphicsShadingGridHalfSize;
  } while (uVar4 != 0);
  return;
}

/* Address: 0x004CDCE0.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture probe hierarchy for geometry.
*/
undefined4 GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 in_EAX;
  int extraout_ECX;
  bool bVar3;
  
  if (*(int *)(*(int *)(param_1 + 0x40) + 0xec) == 0) {
    iVar1 = *(int *)(param_1 + 200);
    do {
      iVar1 = iVar1 + -1;
      if (iVar1 < 0) {
        return in_EAX;
      }
      iVar2 = *(int *)(param_1 + 0xcc + iVar1 * 4);
      bVar3 = false;
    } while ((iVar2 == 0) ||
            (GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(iVar2), iVar1 = extraout_ECX,
            bVar3));
  }
  return in_EAX;
}

/* Address: 0x004D1060.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture reserve fourteen projected point blocks.
*/
void GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks(int param_1)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  
  puVar1 = *(uint **)(param_1 + 200);
  uVar2 = puVar1[1];
  if (uVar2 + 0xe < *puVar1) {
    puVar1[1] = uVar2 + 0xe;
    uVar3 = uVar2 * 0x80 + puVar1[2];
    puVar1[uVar2 * 4 + 9] = uVar3;
    puVar1[uVar2 * 4 + 0xd] = uVar3 + 0x80;
    puVar1[uVar2 * 4 + 0x11] = uVar3 + 0x100;
    puVar1[uVar2 * 4 + 0x15] = uVar3 + 0x180;
    puVar1[uVar2 * 4 + 0x19] = uVar3 + 0x200;
    puVar1[uVar2 * 4 + 0x1d] = uVar3 + 0x280;
    puVar1[uVar2 * 4 + 0x21] = uVar3 + 0x300;
    puVar1[uVar2 * 4 + 0x25] = uVar3 + 0x380;
    puVar1[uVar2 * 4 + 0x29] = uVar3 + 0x400;
    puVar1[uVar2 * 4 + 0x2d] = uVar3 + 0x480;
    puVar1[uVar2 * 4 + 0x31] = uVar3 + 0x500;
    puVar1[uVar2 * 4 + 0x35] = uVar3 + 0x580;
    puVar1[uVar2 * 4 + 0x39] = uVar3 + 0x600;
    puVar1[uVar2 * 4 + 0x3d] = uVar3 + 0x680;
    *(undefined4 *)(uVar3 + 0x60) = 0;
    *(undefined4 *)(uVar3 + 0x68) = 0x11000;
    return;
  }
  return;
}

/* Address: 0x004D1150.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture rollback fourteen projected point blocks.
*/
void GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks(int param_1)

{
  int *piVar1;
  
  piVar1 = (int *)(*(int *)(param_1 + 200) + 4);
  *piVar1 = *piVar1 + -0xe;
  return;
}

/* Address: 0x00484FA0.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture transform point xyquantized.
*/
void GraphicsShadingGeneratedTexture_TransformPointXYQuantized
               (uint *param_1,int *param_2,int *param_3)

{
  int iVar1;
  longlong lVar2;
  
  lVar2 = (longlong)param_3[1] * (longlong)param_2[1] + (longlong)*param_3 * (longlong)*param_2 +
          (longlong)param_3[2] * (longlong)param_2[2];
  iVar1 = param_3[3];
  *param_1 = ((int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc) + param_3[9] &
             0xfffff000;
  lVar2 = (longlong)param_3[4] * (longlong)param_2[1] + (longlong)iVar1 * (longlong)*param_2 +
          (longlong)param_3[5] * (longlong)param_2[2];
  param_1[1] = ((int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc) + param_3[10] &
               0xfffff000;
  return;
}

/* Address: 0x00485320.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture compose transform.
*/
void GraphicsShadingGeneratedTexture_ComposeTransform(uint *param_1,int *param_2,int *param_3)

{
  int iVar1;
  longlong lVar2;
  
  lVar2 = (longlong)param_3[1] * (longlong)param_2[3] + (longlong)*param_3 * (longlong)*param_2 +
          (longlong)param_3[2] * (longlong)param_2[6];
  iVar1 = *param_3;
  *param_1 = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)param_3[1] * (longlong)param_2[4] + (longlong)iVar1 * (longlong)param_2[1] +
          (longlong)param_3[2] * (longlong)param_2[7];
  iVar1 = *param_3;
  param_1[1] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)param_3[1] * (longlong)param_2[5] + (longlong)iVar1 * (longlong)param_2[2] +
          (longlong)param_3[2] * (longlong)param_2[8];
  iVar1 = *param_3;
  param_1[2] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)param_3[1] * (longlong)param_2[10] + (longlong)iVar1 * (longlong)param_2[9] +
          (longlong)param_3[2] * (longlong)param_2[0xb];
  iVar1 = param_3[3];
  param_1[9] = ((int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc) + param_3[9];
  lVar2 = (longlong)param_3[4] * (longlong)param_2[3] + (longlong)iVar1 * (longlong)*param_2 +
          (longlong)param_3[5] * (longlong)param_2[6];
  iVar1 = param_3[3];
  param_1[3] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)param_3[4] * (longlong)param_2[4] + (longlong)iVar1 * (longlong)param_2[1] +
          (longlong)param_3[5] * (longlong)param_2[7];
  iVar1 = param_3[3];
  param_1[4] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)param_3[4] * (longlong)param_2[5] + (longlong)iVar1 * (longlong)param_2[2] +
          (longlong)param_3[5] * (longlong)param_2[8];
  iVar1 = param_3[3];
  param_1[5] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)param_3[4] * (longlong)param_2[10] + (longlong)iVar1 * (longlong)param_2[9] +
          (longlong)param_3[5] * (longlong)param_2[0xb];
  iVar1 = param_3[6];
  param_1[10] = ((int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc) + param_3[10];
  lVar2 = (longlong)param_3[7] * (longlong)param_2[3] + (longlong)iVar1 * (longlong)*param_2 +
          (longlong)param_3[8] * (longlong)param_2[6];
  iVar1 = param_3[6];
  param_1[6] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)param_3[7] * (longlong)param_2[4] + (longlong)iVar1 * (longlong)param_2[1] +
          (longlong)param_3[8] * (longlong)param_2[7];
  iVar1 = param_3[6];
  param_1[7] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)param_3[7] * (longlong)param_2[5] + (longlong)iVar1 * (longlong)param_2[2] +
          (longlong)param_3[8] * (longlong)param_2[8];
  iVar1 = param_3[6];
  param_1[8] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)param_3[7] * (longlong)param_2[10] + (longlong)iVar1 * (longlong)param_2[9] +
          (longlong)param_3[8] * (longlong)param_2[0xb];
  param_1[0xb] = ((int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc) + param_3[0xb];
  return;
}

/* Address: 0x004CD690.
   Ownership: graphics/render/shading.
   Purpose: Handles graphics shading generated texture rasterize triangle mask.
*/
void GraphicsShadingGeneratedTexture_RasterizeTriangleMask(int *param_1,int *param_2,int *param_3)

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
  int *piVar10;
  int *piVar11;
  int *piVar12;
  int iVar13;
  undefined1 *puVar14;
  sdword sVar15;
  int iVar16;
  sdword sVar17;
  
  iVar16 = param_3[1];
  iVar7 = param_2[1];
  iVar6 = param_1[1];
  iVar3 = iVar16;
  piVar12 = param_2;
  if (iVar7 < iVar16) {
    iVar3 = iVar7;
    iVar7 = iVar16;
    piVar12 = param_3;
    param_3 = param_2;
  }
  iVar16 = iVar3;
  piVar10 = param_1;
  if (iVar6 < iVar3) {
    iVar16 = iVar6;
    iVar6 = iVar3;
    piVar10 = param_3;
    param_3 = param_1;
  }
  iVar3 = iVar7;
  piVar11 = piVar10;
  if (iVar6 < iVar7) {
    iVar3 = iVar6;
    iVar6 = iVar7;
    piVar11 = piVar12;
    piVar12 = piVar10;
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
    iVar6 = *param_3;
    iVar3 = *piVar12;
    iVar9 = *piVar11;
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
    iVar13 = (iVar16 >> 0xc) * g_GraphicsShadingTextureDimension + DAT_004cce08;
    iVar6 = iVar9 / iVar7;
    iVar16 = sVar15;
    if (iVar2 != 0) {
      iVar1 = iVar3 / iVar2;
      do {
        puVar14 = (undefined1 *)((sVar15 >> 0xc) + iVar13);
        iVar4 = (iVar16 >> 0xc) - (sVar15 >> 0xc);
        if (iVar4 != 0) {
          iVar5 = iVar4;
          if (iVar4 < 0) {
            iVar5 = -iVar4;
            puVar14 = puVar14 + iVar4;
          }
          for (; iVar5 != 0; iVar5 = iVar5 + -1) {
            *puVar14 = 0xff;
            puVar14 = puVar14 + 1;
          }
        }
        iVar13 = iVar13 + g_GraphicsShadingTextureDimension;
        sVar15 = sVar15 + iVar1;
        iVar16 = iVar16 + iVar6;
        iVar7 = iVar7 - (int)_DAT_0041f680;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
    }
    if (iVar7 != 0) {
      iVar3 = (iVar9 - iVar3) / iVar7;
      do {
        puVar14 = (undefined1 *)((sVar17 >> 0xc) + iVar13);
        iVar9 = (iVar16 >> 0xc) - (sVar17 >> 0xc);
        if (iVar9 != 0) {
          iVar2 = iVar9;
          if (iVar9 < 0) {
            iVar2 = -iVar9;
            puVar14 = puVar14 + iVar9;
          }
          for (; iVar2 != 0; iVar2 = iVar2 + -1) {
            *puVar14 = 0xff;
            puVar14 = puVar14 + 1;
          }
        }
        iVar13 = iVar13 + g_GraphicsShadingTextureDimension;
        sVar17 = sVar17 + iVar3;
        iVar16 = iVar16 + iVar6;
        iVar7 = iVar7 + -1;
      } while (iVar7 != 0);
    }
  }
  return;
}
