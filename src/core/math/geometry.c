/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/geometry.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/math/geometry.h>

/* Implementation ownership: core/math/geometry. */

/* Address: 0x004869B0.
   Ownership: core/math/geometry.
   Purpose: Handles triangle2 d compute barycentric weights q12 packed.
*/
TriangleBarycentricWeightsQ12
Triangle2D_ComputeBarycentricWeightsQ12Packed
          (GraphicsProjectedCoordinate vertexAY,GraphicsProjectedCoordinate vertexAX,
          GraphicsProjectedCoordinate vertexBY,GraphicsProjectedCoordinate vertexBX,
          GraphicsProjectedCoordinate vertexCY,GraphicsProjectedCoordinate vertexCX,
          GraphicsProjectedCoordinate pointY,GraphicsProjectedCoordinate pointX)

{
  ulonglong uVar1;
  longlong lVar2;
  longlong lVar3;
  TriangleBarycentricWeightsQ12 TVar4;
  TriangleBarycentricWeightsQ12 TVar5;
  TriangleBarycentricWeightsQ12 TVar6;
  TriangleBarycentricWeightsQ12 TVar7;
  TriangleBarycentricWeightsQ12 TVar8;
  longlong lVar9;
  int iVar10;
  int iVar11;
  uint uVar12;
  uint uVar13;
  Q12 QVar14;
  int iVar15;
  TriangleBarycentricWeightsQ12 TVar16;
  
  TVar5.weightVertexA_Q12 = vertexAX;
  TVar5.weightVertexB_Q12 = pointX;
  TVar4.weightVertexA_Q12 = vertexAX;
  TVar4.weightVertexB_Q12 = pointX;
  if ((((pointX <= vertexCX) || (pointX <= vertexBX)) || (pointX <= vertexAX)) &&
     (((vertexCX <= pointX || (vertexBX <= pointX)) || (TVar4 = TVar5, vertexAX <= pointX)))) {
    TVar6.weightVertexA_Q12 = vertexAY;
    TVar6.weightVertexB_Q12 = pointY;
    TVar4.weightVertexA_Q12 = vertexAY;
    TVar4.weightVertexB_Q12 = pointY;
    if ((((pointY <= vertexCY) || (pointY <= vertexBY)) || (pointY <= vertexAY)) &&
       (((vertexCY <= pointY || (vertexBY <= pointY)) || (TVar4 = TVar6, vertexAY <= pointY)))) {
      lVar2 = (longlong)vertexAX * (longlong)(vertexBY - vertexCY);
      lVar3 = (longlong)vertexBX * (longlong)(vertexCY - vertexAY) +
              (longlong)vertexCX * (longlong)(vertexAY - vertexBY) + lVar2;
      TVar4.weightVertexA_Q12 = (Q12)((ulonglong)lVar2 >> 0x20);
      TVar4.weightVertexB_Q12 = (uint)lVar3;
      iVar11 = (int)((ulonglong)lVar3 >> 0x20);
      uVar12 = iVar11 << 0x10 | (uint)lVar3 >> 0x10;
      if (uVar12 != 0) {
        TVar4 = (TriangleBarycentricWeightsQ12)
                ((longlong)(vertexCY - pointY) * (longlong)vertexBX +
                 (longlong)(pointY - vertexBY) * (longlong)vertexCX +
                (longlong)(vertexBY - vertexCY) * (longlong)pointX);
        uVar13 = TVar4.weightVertexA_Q12;
        if (lVar3 < 0) {
          if ((int)uVar13 < iVar11) {
            return TVar4;
          }
        }
        else if (iVar11 < (int)uVar13) {
          return TVar4;
        }
        uVar1 = (ulonglong)uVar13 << 0x20 | (ulonglong)TVar4 & 0xffffffff;
        iVar10 = (int)((longlong)uVar1 / (longlong)(int)uVar12);
        QVar14 = (Q12)((longlong)uVar1 % (longlong)(int)uVar12);
        TVar7.weightVertexA_Q12 = QVar14;
        TVar7.weightVertexB_Q12 = iVar10;
        TVar4.weightVertexA_Q12 = QVar14;
        TVar4.weightVertexB_Q12 = iVar10;
        if ((-1 < iVar10) && (TVar4 = TVar7, iVar10 < 0x10001)) {
          lVar9 = (longlong)(vertexCY - vertexAY) * (longlong)pointX +
                  (longlong)(vertexAY - pointY) * (longlong)vertexCX;
          lVar2 = (longlong)(pointY - vertexCY) * (longlong)vertexAX;
          iVar15 = (int)((ulonglong)(lVar9 + lVar2) >> 0x20);
          if (lVar3 < 0) {
            if (iVar15 < iVar11) {
              return (TriangleBarycentricWeightsQ12)(lVar9 + lVar2);
            }
          }
          else if (iVar11 < iVar15) {
            return (TriangleBarycentricWeightsQ12)(lVar2 + lVar9);
          }
          iVar11 = (int)((lVar9 + lVar2) / (longlong)(int)uVar12);
          TVar8.weightVertexA_Q12 = iVar10;
          TVar8.weightVertexB_Q12 = iVar11;
          TVar4.weightVertexA_Q12 = iVar10;
          TVar4.weightVertexB_Q12 = iVar11;
          if ((-1 < iVar11) && (TVar4 = TVar8, iVar11 + iVar10 < 0x10001)) {
            TVar16.weightVertexB_Q12 = iVar11 >> 4;
            TVar16.weightVertexA_Q12 = iVar10 >> 4;
            return TVar16;
          }
        }
      }
    }
  }
  return TVar4;
}

