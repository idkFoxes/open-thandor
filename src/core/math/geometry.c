#include <thandor/core/math/geometry.h>

/* Implementation ownership: core/math/geometry. */

/* Address: 0x004869B0.
   Ownership: core/math/geometry.
   Purpose: Handles triangle2 d compute barycentric weights q12 packed.
*/
ulonglong Triangle2D_ComputeBarycentricWeightsQ12Packed
                    (int param_1,int param_2,int param_3,int param_4,int param_5,int param_6,
                    int param_7,int param_8)

{
  ulonglong uVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  undefined4 uVar9;
  int iVar10;
  
  if ((((param_8 <= param_6) || (param_8 <= param_4)) ||
      (uVar1 = CONCAT44(param_2,param_8), param_8 <= param_2)) &&
     (((param_6 <= param_8 || (param_4 <= param_8)) ||
      (uVar1 = CONCAT44(param_2,param_8), param_2 <= param_8)))) {
    if ((((param_7 <= param_5) || (param_7 <= param_3)) ||
        (uVar1 = CONCAT44(param_1,param_7), param_7 <= param_1)) &&
       (((param_5 <= param_7 || (param_3 <= param_7)) ||
        (uVar1 = CONCAT44(param_1,param_7), param_1 <= param_7)))) {
      lVar2 = (longlong)param_2 * (longlong)(param_3 - param_5);
      lVar3 = (longlong)param_4 * (longlong)(param_5 - param_1) +
              (longlong)param_6 * (longlong)(param_1 - param_3) + lVar2;
      iVar6 = (int)((ulonglong)lVar3 >> 0x20);
      uVar7 = iVar6 << 0x10 | (uint)lVar3 >> 0x10;
      uVar1 = CONCAT44((int)((ulonglong)lVar2 >> 0x20),(uint)lVar3);
      if (uVar7 != 0) {
        uVar1 = (longlong)(param_5 - param_7) * (longlong)param_4 +
                (longlong)(param_7 - param_3) * (longlong)param_6 +
                (longlong)(param_3 - param_5) * (longlong)param_8;
        uVar8 = (uint)(uVar1 >> 0x20);
        if (lVar3 < 0) {
          if ((int)uVar8 < iVar6) {
            return uVar1;
          }
        }
        else if (iVar6 < (int)uVar8) {
          return uVar1;
        }
        uVar1 = (ulonglong)uVar8 << 0x20 | uVar1 & 0xffffffff;
        iVar5 = (int)((longlong)uVar1 / (longlong)(int)uVar7);
        uVar9 = (undefined4)((longlong)uVar1 % (longlong)(int)uVar7);
        uVar1 = CONCAT44(uVar9,iVar5);
        if ((-1 < iVar5) && (uVar1 = CONCAT44(uVar9,iVar5), iVar5 < 0x10001)) {
          lVar4 = (longlong)(param_5 - param_1) * (longlong)param_8 +
                  (longlong)(param_1 - param_7) * (longlong)param_6;
          lVar2 = (longlong)(param_7 - param_5) * (longlong)param_2;
          iVar10 = (int)((ulonglong)(lVar4 + lVar2) >> 0x20);
          if (lVar3 < 0) {
            if (iVar10 < iVar6) {
              return lVar4 + lVar2;
            }
          }
          else if (iVar6 < iVar10) {
            return lVar2 + lVar4;
          }
          iVar6 = (int)((lVar4 + lVar2) / (longlong)(int)uVar7);
          uVar1 = CONCAT44(iVar5,iVar6);
          if ((-1 < iVar6) && (uVar1 = CONCAT44(iVar5,iVar6), iVar6 + iVar5 < 0x10001)) {
            return CONCAT44(iVar5 >> 4,iVar6 >> 4);
          }
        }
      }
    }
  }
  return uVar1;
}
