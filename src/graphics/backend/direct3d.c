/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/direct3d.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/direct3d.h>

/* Implementation ownership: graphics/backend/direct3d. */

/* Address: 0x00578270.
   Ownership: graphics/backend/direct3d.
   Purpose: IDirect3D2::EnumDevices callback. Context points at the adapter record being expanded.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
sdword Direct3D_EnumDeviceCallback
                 (TH_LEGACY_GUID *deviceGuid,char *description,char *deviceName,
                 D3DDEVICEDESC_DX6 *hardwareDesc,D3DDEVICEDESC_DX6 *softwareDesc,
                 GraphicsAdapterRecord *adapterContext)

{
  GraphicsAdapterRecord *pGVar1;
  dword dVar2;
  dword dVar3;
  int iVar4;
  GraphicsAdapterRecord *pGVar5;
  GraphicsAdapterRecord *pGVar6;
  TH_LEGACY_GUID *pTVar7;
  D3DDEVICEDESC_DX6 *pDVar8;
  ArenaAllocEaxCf5 AVar9;
  
  dVar2 = g_GraphicsAdapterCount;
  if ((g_GraphicsAdapterCount < 0x10) &&
     ((g_GraphicsEnumerateAllDevicesFlag == 0 ||
      (((((((hardwareDesc->dwFlags & 1) != 0 && ((hardwareDesc->dwFlags & 0x100) != 0)) &&
          ((hardwareDesc->dwDeviceZBufferBitDepth & 0x400) != 0)) &&
         (((hardwareDesc->dwFlags & 2) != 0 && ((hardwareDesc->dwDevCaps & 0x40) != 0)))) &&
        ((((hardwareDesc->dwDevCaps & 0x200) != 0 &&
          (((hardwareDesc->dwFlags & 0x40) != 0 &&
           (((hardwareDesc->dpcTriCaps).dwZCmpCaps & 8) != 0)))) &&
         (((hardwareDesc->dpcTriCaps).dwTextureAddressCaps & 1) != 0)))) &&
       (((((((hardwareDesc->dpcTriCaps).dwTextureBlendCaps & 2) != 0 &&
           (((hardwareDesc->dpcTriCaps).dwSrcBlendCaps & 2) != 0)) &&
          (((hardwareDesc->dpcTriCaps).dwSrcBlendCaps & 0x10) != 0)) &&
         (((((hardwareDesc->dpcTriCaps).dwDestBlendCaps & 1) != 0 &&
           (((hardwareDesc->dpcTriCaps).dwDestBlendCaps & 2) != 0)) &&
          ((((hardwareDesc->dpcTriCaps).dwDestBlendCaps & 0x20) != 0 &&
           (((hardwareDesc->dpcTriCaps).dwShadeCaps & 8) != 0)))))) &&
        (((((hardwareDesc->dpcTriCaps).dwShadeCaps & 0x4000) != 0 ||
          (((hardwareDesc->dpcTriCaps).dwShadeCaps & 0x8000) != 0)) ||
         (((hardwareDesc->dpcTriCaps).dwRasterCaps & 0x200) != 0)))))))))) {
    AVar9 = (*g_MemoryApi.alloc)(0x198);
    dVar3 = g_GraphicsAdapterCount;
    if (!AVar9.carry) {
      pGVar5 = g_GraphicsAdapters + dVar2;
      iVar4 = 0x20;
      g_GraphicsAdapterCount = g_GraphicsAdapterCount + 1;
      pGVar6 = pGVar5;
      if ((g_GraphicsEnumerateAllDevicesFlag == 0) ||
         (pGVar1 = adapterContext, (adapterContext->deviceGuid).Data1 != 0)) {
        for (; pGVar1 = pGVar5, dVar3 = g_GraphicsAdapterCount, iVar4 != 0; iVar4 = iVar4 + -1) {
          (pGVar6->adapterGuid).Data1 = (adapterContext->adapterGuid).Data1;
          adapterContext = (GraphicsAdapterRecord *)&(adapterContext->adapterGuid).Data2;
          pGVar6 = (GraphicsAdapterRecord *)&(pGVar6->adapterGuid).Data2;
        }
      }
      g_GraphicsAdapterCount = dVar3;
      pTVar7 = &pGVar1->deviceGuid;
      for (iVar4 = 4; iVar4 != 0; iVar4 = iVar4 + -1) {
        pTVar7->Data1 = deviceGuid->Data1;
        deviceGuid = (TH_LEGACY_GUID *)&deviceGuid->Data2;
        pTVar7 = (TH_LEGACY_GUID *)&pTVar7->Data2;
      }
      Text_CopyNarrowToUtf16Cf(0x28,pGVar1->deviceNameUtf16,(byte *)deviceName);
      pGVar1->hardwareDesc = (D3DDEVICEDESC_DX6 *)AVar9.eax;
      pDVar8 = (D3DDEVICEDESC_DX6 *)AVar9.eax;
      for (iVar4 = 0x33; iVar4 != 0; iVar4 = iVar4 + -1) {
        pDVar8->dwSize = hardwareDesc->dwSize;
        hardwareDesc = (D3DDEVICEDESC_DX6 *)&hardwareDesc->dwFlags;
        pDVar8 = (D3DDEVICEDESC_DX6 *)&pDVar8->dwFlags;
      }
      pGVar1->softwareDesc = pDVar8;
      for (iVar4 = 0x33; iVar4 != 0; iVar4 = iVar4 + -1) {
        pDVar8->dwSize = softwareDesc->dwSize;
        softwareDesc = (D3DDEVICEDESC_DX6 *)&softwareDesc->dwFlags;
        pDVar8 = (D3DDEVICEDESC_DX6 *)&pDVar8->dwFlags;
      }
    }
  }
  return 1;
}


/* Address: 0x00578820.
   Ownership: graphics/backend/direct3d.
   Purpose: Semantic ABI remains deferred.
*/
sdword __thandor_eax_preserve_ecx_edx
GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback
          (DDSURFACEDESC_DX6 *surfaceDesc,TH_LEGACY_LPVOID context)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  DDPIXELFORMAT *pDVar8;
  TH_LEGACY_DWORD *pTVar9;
  
  uVar5 = (surfaceDesc->ddpfPixelFormat).dwRGBBitCount;
  uVar1 = (surfaceDesc->ddpfPixelFormat).dwFlags;
  if (uVar5 < 8) {
    return 1;
  }
  if (uVar5 == 8) {
    return 1;
  }
  if ((uVar5 != 0x10) && (uVar5 != 0x20)) {
    return 1;
  }
  if ((uVar1 & 0x40) == 0) {
    return 1;
  }
  if (g_Direct3DOpaqueTextureFormatBitsPerPixel != 0) {
    if (g_Direct3DOpaqueTextureFormatBitsPerPixel < uVar5) goto LAB_005788c0;
    if (g_Direct3DOpaqueTextureFormatBitsPerPixel == uVar5) {
      uVar7 = (surfaceDesc->ddpfPixelFormat).dwRBitMask | (surfaceDesc->ddpfPixelFormat).dwGBitMask
              | (surfaceDesc->ddpfPixelFormat).dwBBitMask;
      uVar5 = (_g_Direct3DOpaqueTextureFormatRedBitMask | _g_Direct3DOpaqueTextureFormatGreenBitMask
              | _g_Direct3DOpaqueTextureFormatBlueBitMask) ^ uVar7;
      if ((uVar5 == 0) || ((uVar5 & uVar7) == 0)) goto LAB_005788c0;
    }
  }
  pDVar8 = &surfaceDesc->ddpfPixelFormat;
  pTVar9 = (TH_LEGACY_DWORD *)0x577d90;
  for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
    *pTVar9 = pDVar8->dwSize;
    pDVar8 = (DDPIXELFORMAT *)&pDVar8->dwFlags;
    pTVar9 = pTVar9 + 1;
  }
LAB_005788c0:
  if (((uVar1 & 1) != 0) && (8 < (surfaceDesc->ddpfPixelFormat).dwRGBBitCount)) {
    iVar6 = 0x1f;
    if (_g_Direct3DAlphaTextureFormatAlphaBitMask != 0) {
      for (; _g_Direct3DAlphaTextureFormatAlphaBitMask >> iVar6 == 0; iVar6 = iVar6 + -1) {
      }
    }
    iVar2 = 0;
    if (_g_Direct3DAlphaTextureFormatAlphaBitMask != 0) {
      for (; (_g_Direct3DAlphaTextureFormatAlphaBitMask >> iVar2 & 1) == 0; iVar2 = iVar2 + 1) {
      }
    }
    pTVar9 = &(surfaceDesc->ddpfPixelFormat).dwRGBAlphaBitMask;
    iVar4 = 0x1f;
    if (*pTVar9 != 0) {
      for (; *pTVar9 >> iVar4 == 0; iVar4 = iVar4 + -1) {
      }
    }
    pTVar9 = &(surfaceDesc->ddpfPixelFormat).dwRGBAlphaBitMask;
    iVar3 = 0;
    if (*pTVar9 != 0) {
      for (; (*pTVar9 >> iVar3 & 1) == 0; iVar3 = iVar3 + 1) {
      }
    }
    if ((uint)(iVar2 - iVar6) < (uint)(iVar3 - iVar4)) {
      pDVar8 = &surfaceDesc->ddpfPixelFormat;
      pTVar9 = (TH_LEGACY_DWORD *)0x577db0;
      for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
        *pTVar9 = pDVar8->dwSize;
        pDVar8 = (DDPIXELFORMAT *)&pDVar8->dwFlags;
        pTVar9 = pTVar9 + 1;
      }
    }
  }
  return 1;
}


/* Address: 0x0057A450.
   Ownership: graphics/backend/direct3d.
   Purpose: Handles direct3 drenderer set antialias mode.
*/
Direct3DRenderStateApplyEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Direct3DRenderer_SetAntialiasMode(dword antialiasMode)

{
  sdword sVar1;
  Direct3DRenderStateApplyEaxCf5 DVar2;
  Direct3DRenderStateApplyEaxCf5 DVar3;
  
  sVar1 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                    (g_Direct3DDevice2,D3DRENDERSTATE_ANTIALIAS,antialiasMode);
  if (sVar1 == 0) {
    g_Direct3DAntialiasMode = antialiasMode;
    DVar2.carry = false;
    DVar2.appliedValueOrError = antialiasMode;
    return DVar2;
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,100,g_PackageLastErrorPath);
  DVar3.carry = true;
  DVar3.appliedValueOrError = 0x1d;
  return DVar3;
}


/* Address: 0x0057A4C0.
   Ownership: graphics/backend/direct3d.
   Purpose: Handles direct3 drenderer set texture filter mode.
*/
Direct3DRenderStateApplyEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Direct3DRenderer_SetTextureFilterMode(dword textureFilterMode)

{
  sdword sVar1;
  Direct3DRenderStateApplyEaxCf5 DVar2;
  Direct3DRenderStateApplyEaxCf5 DVar3;
  sdword sStack_1c;
  
  sStack_1c = 0x6e;
  sVar1 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                    (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMAG,textureFilterMode);
  if (sVar1 == 0) {
    sStack_1c = 0x6f;
    sVar1 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMIN,textureFilterMode);
    if (sVar1 == 0) {
      g_Direct3DTextureFilterMode = textureFilterMode;
      DVar2.carry = false;
      DVar2.appliedValueOrError = textureFilterMode;
      return DVar2;
    }
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,sStack_1c,g_PackageLastErrorPath);
  DVar3.carry = true;
  DVar3.appliedValueOrError = 0x1d;
  return DVar3;
}


/* Address: 0x0057A550.
   Ownership: graphics/backend/direct3d.
   Purpose: Handles direct3 drenderer set texture perspective enabled.
*/
Direct3DRenderStateApplyEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Direct3DRenderer_SetTexturePerspectiveEnabled(dword texturePerspectiveEnabled)

{
  sdword sVar1;
  Direct3DRenderStateApplyEaxCf5 DVar2;
  Direct3DRenderStateApplyEaxCf5 DVar3;
  
  sVar1 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                    (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREPERSPECTIVE,texturePerspectiveEnabled);
  if (sVar1 == 0) {
    g_Direct3DTexturePerspectiveEnabled = texturePerspectiveEnabled;
    DVar2.carry = false;
    DVar2.appliedValueOrError = texturePerspectiveEnabled;
    return DVar2;
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x78,g_PackageLastErrorPath);
  DVar3.carry = true;
  DVar3.appliedValueOrError = 0x1d;
  return DVar3;
}


/* Address: 0x0057CCB0.
   Ownership: graphics/backend/direct3d.
   Purpose: Direct3D immediate primitive handler using render-state preset 0: ONE/ZERO, alpha blending disabled,
   Z-write enabled. Multiplies each vertex diffuse color by packet->modulationColor through MMX, converts
   screen/depth values into g_ImmediateTLVertices, zeroes texture coordinates, duplicates the second vertex into
   slot four when g_ImmediateVertexCount is four, and binds texture handle zero.
*/
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_UntexturedPreset0(GraphicsPrimitivePacket *packet)

{
  PackedArgb32 PVar1;
  PackedArgb32 PVar2;
  PackedArgb32 PVar3;
  PackedArgb32 PVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  sdword sVar13;
  int iVar14;
  D3DDEVICEDESC_DX6 *pDVar15;
  D3DTLVERTEX_DX6 *pDVar16;
  D3DTLVERTEX_DX6 *pDVar17;
  undefined1 mm0PackedValue0ByteLane1;
  undefined1 mm0PackedValue0ByteLane2;
  sdword sVar19;
  undefined8 mm0PackedValue0;
  undefined8 mm1PackedValue0;
  undefined1 mm1PackedValue0ByteLane1;
  undefined1 mm1PackedValue0ByteLane2;
  unkbyte10 in_ST1;
  unkbyte10 extraout_ST1;
  unkbyte10 extraout_ST1_00;
  unkbyte10 extraout_ST1_01;
  unkbyte10 extraout_ST1_02;
  undefined8 mm2PackedValue0;
  undefined1 mm2PackedValue0ByteLane1;
  undefined1 mm2PackedValue0ByteLane2;
  unkbyte10 in_ST2;
  undefined8 mm3PackedValue0;
  undefined1 mm3PackedValue0ByteLane1;
  undefined1 mm3PackedValue0ByteLane2;
  unkbyte10 in_ST3;
  IDirect3DDevice2 *pIVar18;
  
  if (g_PrimitiveRenderStatePresets[0].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[0].zWriteEnable;
    _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[0].zWriteEnable);
    in_ST1 = extraout_ST1;
  }
  if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[0].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
      in_ST1 = extraout_ST1_00;
    }
    else {
      _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[0].alphaBlendEnable);
      pDVar15 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((pDVar15->dwFlags & 1) == 0) || (pDVar15->dcmColorModel == 2)) {
        pDVar15 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      in_ST1 = extraout_ST1_01;
      if (((pDVar15->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                            (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
        in_ST1 = extraout_ST1_02;
      }
    }
  }
  PVar1 = packet->modulationColor;
  PVar2 = packet->vertices[0].diffuseColor;
  PVar3 = packet->vertices[1].diffuseColor;
  PVar4 = packet->vertices[2].diffuseColor;
  mm0PackedValue0ByteLane1 = (undefined1)(PVar2 >> 0x18);
  uVar9 = CONCAT31(CONCAT21((short)((unkuint10)_sVar19 >> 0x40),mm0PackedValue0ByteLane1),
                   mm0PackedValue0ByteLane1);
  mm0PackedValue0ByteLane2 = (undefined1)(PVar2 >> 0x10);
  mm0PackedValue0ByteLane1 = (undefined1)(PVar2 >> 8);
  mm1PackedValue0ByteLane1 = (undefined1)(PVar3 >> 0x18);
  uVar10 = CONCAT31(CONCAT21((short)((unkuint10)in_ST1 >> 0x40),mm1PackedValue0ByteLane1),
                    mm1PackedValue0ByteLane1);
  mm1PackedValue0ByteLane2 = (undefined1)(PVar3 >> 0x10);
  mm1PackedValue0ByteLane1 = (undefined1)(PVar3 >> 8);
  mm2PackedValue0ByteLane1 = (undefined1)(PVar4 >> 0x18);
  uVar11 = CONCAT31(CONCAT21((short)((unkuint10)in_ST2 >> 0x40),mm2PackedValue0ByteLane1),
                    mm2PackedValue0ByteLane1);
  mm2PackedValue0ByteLane2 = (undefined1)(PVar4 >> 0x10);
  mm2PackedValue0ByteLane1 = (undefined1)(PVar4 >> 8);
  mm3PackedValue0ByteLane1 = (undefined1)(PVar1 >> 0x18);
  uVar12 = CONCAT31(CONCAT21((short)((unkuint10)in_ST3 >> 0x40),mm3PackedValue0ByteLane1),
                    mm3PackedValue0ByteLane1);
  mm3PackedValue0ByteLane2 = (undefined1)(PVar1 >> 0x10);
  mm3PackedValue0ByteLane1 = (undefined1)(PVar1 >> 8);
  mm3PackedValue0._0_2_ = CONCAT11((char)PVar1,(char)PVar1) >> 4;
  mm3PackedValue0._2_2_ = CONCAT11(mm3PackedValue0ByteLane1,mm3PackedValue0ByteLane1) >> 4;
  mm3PackedValue0._0_4_ = CONCAT22(mm3PackedValue0._2_2_,(ushort)mm3PackedValue0);
  mm3PackedValue0._4_2_ =
       (ushort)(CONCAT55(CONCAT41(uVar12,mm3PackedValue0ByteLane2),
                         CONCAT14(mm3PackedValue0ByteLane2,PVar1)) >> 0x20);
  mm3PackedValue0._4_2_ = mm3PackedValue0._4_2_ >> 4;
  mm3PackedValue0._0_6_ = CONCAT24(mm3PackedValue0._4_2_,(undefined4)mm3PackedValue0);
  mm3PackedValue0._6_2_ = (ushort)uVar12;
  mm3PackedValue0._6_2_ = mm3PackedValue0._6_2_ >> 4;
  mm3PackedValue0 = CONCAT26(mm3PackedValue0._6_2_,(undefined6)mm3PackedValue0);
  mm0PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar9 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar9,mm0PackedValue0ByteLane2),
                                                  CONCAT14(mm0PackedValue0ByteLane2,PVar2)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                       mm0PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar2,(char)PVar2) >> 4))),
              mm3PackedValue0);
  mm1PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar10 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar10,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,PVar3)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                       mm1PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar3,(char)PVar3) >> 4))),
              mm3PackedValue0);
  mm2PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar11 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar11,mm2PackedValue0ByteLane2),
                                                  CONCAT14(mm2PackedValue0ByteLane2,PVar4)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm2PackedValue0ByteLane1,
                                                       mm2PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar4,(char)PVar4) >> 4))),
              mm3PackedValue0);
  sVar5 = (short)mm0PackedValue0;
  sVar6 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
  g_ImmediateTLVertices[0].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm0PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm0PackedValue0 -
                                  (0xff < sVar5))));
  sVar5 = (short)mm1PackedValue0;
  sVar6 = (short)((ulonglong)mm1PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm1PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm1PackedValue0 >> 0x30);
  g_ImmediateTLVertices[1].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm1PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm1PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm1PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm1PackedValue0 -
                                  (0xff < sVar5))));
  sVar5 = (short)mm2PackedValue0;
  sVar6 = (short)((ulonglong)mm2PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm2PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm2PackedValue0 >> 0x30);
  g_ImmediateTLVertices[2].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm2PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm2PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm2PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm2PackedValue0 -
                                  (0xff < sVar5))));
  g_ImmediateTLVertices[0].specular = 0;
  g_ImmediateTLVertices[1].specular = 0;
  g_ImmediateTLVertices[2].specular = 0;
  g_ImmediateTLVertices[0].sx = (float)packet->vertices[0].screenX;
  g_ImmediateTLVertices[0].sy = (float)packet->vertices[0].screenY;
  g_ImmediateTLVertices[1].sx = (float)packet->vertices[1].screenX;
  g_ImmediateTLVertices[1].sy = (float)packet->vertices[1].screenY;
  g_ImmediateTLVertices[2].sx = (float)packet->vertices[2].screenX;
  g_ImmediateTLVertices[2].sy = (float)packet->vertices[2].screenY;
  if (g_ImmediateTLVertices[0].sx != 0.0) {
    g_ImmediateTLVertices[0].sx = (float)((int)g_ImmediateTLVertices[0].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    g_ImmediateTLVertices[0].sy = (float)((int)g_ImmediateTLVertices[0].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    g_ImmediateTLVertices[1].sx = (float)((int)g_ImmediateTLVertices[1].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    g_ImmediateTLVertices[1].sy = (float)((int)g_ImmediateTLVertices[1].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    g_ImmediateTLVertices[2].sx = (float)((int)g_ImmediateTLVertices[2].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    g_ImmediateTLVertices[2].sy = (float)((int)g_ImmediateTLVertices[2].sy + -0x6000000);
  }
  g_ImmediateTLVertices[0].tu = 0.0;
  g_ImmediateTLVertices[0].tv = 0.0;
  g_ImmediateTLVertices[1].tu = 0.0;
  g_ImmediateTLVertices[1].tv = 0.0;
  g_ImmediateTLVertices[2].tu = 0.0;
  g_ImmediateTLVertices[2].tv = 0.0;
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    g_ImmediateTLVertices[0].sz = (float)((int)g_ImmediateTLVertices[0].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    g_ImmediateTLVertices[0].rhw = (float)((int)g_ImmediateTLVertices[0].rhw + 0xa000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    g_ImmediateTLVertices[1].sz = (float)((int)g_ImmediateTLVertices[1].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    g_ImmediateTLVertices[1].rhw = (float)((int)g_ImmediateTLVertices[1].rhw + 0xa000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    g_ImmediateTLVertices[2].sz = (float)((int)g_ImmediateTLVertices[2].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    g_ImmediateTLVertices[2].rhw = (float)((int)g_ImmediateTLVertices[2].rhw + 0xa000000);
  }
  if (3 < g_ImmediateVertexCount) {
    pDVar16 = g_ImmediateTLVertices + 1;
    pDVar17 = g_ImmediateTLVertices + 3;
    for (iVar14 = 8; iVar14 != 0; iVar14 = iVar14 + -1) {
      pDVar17->sx = pDVar16->sx;
      pDVar16 = (D3DTLVERTEX_DX6 *)&pDVar16->sy;
      pDVar17 = (D3DTLVERTEX_DX6 *)&pDVar17->sy;
    }
  }
  if (g_BoundTextureHandle != 0) {
    pIVar18 = g_Direct3DDevice2;
    sVar13 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (sVar13 != 0) {
      pIVar18 = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)pIVar18;
    g_TextureBindStateChangeCount = g_TextureBindStateChangeCount + 1;
  }
  return;
}


/* Address: 0x0057CF20.
   Ownership: graphics/backend/direct3d.
   Purpose: Direct3D immediate primitive handler using render-state preset 2: SRCALPHA/INVSRCALPHA, alpha blending
   enabled, Z-write disabled. Multiplies each vertex diffuse color by packet->modulationColor through MMX, converts
   screen/depth values into g_ImmediateTLVertices, zeroes texture coordinates, duplicates the second vertex into
   slot four when g_ImmediateVertexCount is four, and binds texture handle zero.
*/
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_UntexturedPreset2(GraphicsPrimitivePacket *packet)

{
  PackedArgb32 PVar1;
  PackedArgb32 PVar2;
  PackedArgb32 PVar3;
  PackedArgb32 PVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  sdword sVar13;
  int iVar14;
  D3DDEVICEDESC_DX6 *pDVar15;
  D3DTLVERTEX_DX6 *pDVar16;
  D3DTLVERTEX_DX6 *pDVar17;
  undefined1 mm0PackedValue0ByteLane1;
  undefined1 mm0PackedValue0ByteLane2;
  sdword sVar19;
  undefined8 mm0PackedValue0;
  undefined8 mm1PackedValue0;
  undefined1 mm1PackedValue0ByteLane1;
  undefined1 mm1PackedValue0ByteLane2;
  unkbyte10 in_ST1;
  unkbyte10 extraout_ST1;
  unkbyte10 extraout_ST1_00;
  unkbyte10 extraout_ST1_01;
  unkbyte10 extraout_ST1_02;
  unkbyte10 extraout_ST1_03;
  unkbyte10 extraout_ST1_04;
  undefined8 mm2PackedValue0;
  undefined1 mm2PackedValue0ByteLane1;
  undefined1 mm2PackedValue0ByteLane2;
  unkbyte10 in_ST2;
  undefined8 mm3PackedValue0;
  undefined1 mm3PackedValue0ByteLane1;
  undefined1 mm3PackedValue0ByteLane2;
  unkbyte10 in_ST3;
  IDirect3DDevice2 *pIVar18;
  
  if (g_PrimitiveRenderStatePresets[2].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[2].zWriteEnable;
    _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[2].zWriteEnable);
    in_ST1 = extraout_ST1;
  }
  if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[2].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
      in_ST1 = extraout_ST1_00;
    }
    else {
      _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[2].alphaBlendEnable);
      pDVar15 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((pDVar15->dwFlags & 1) == 0) || (pDVar15->dcmColorModel == 2)) {
        pDVar15 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      in_ST1 = extraout_ST1_01;
      if (((pDVar15->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                            (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
        in_ST1 = extraout_ST1_02;
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[2].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[2].sourceBlend;
    _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
                         g_PrimitiveRenderStatePresets[2].sourceBlend);
    in_ST1 = extraout_ST1_03;
  }
  if (g_PrimitiveRenderStatePresets[2].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[2].destinationBlend
    ;
    _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
                         g_PrimitiveRenderStatePresets[2].destinationBlend);
    in_ST1 = extraout_ST1_04;
  }
  PVar1 = packet->modulationColor;
  PVar2 = packet->vertices[0].diffuseColor;
  PVar3 = packet->vertices[1].diffuseColor;
  PVar4 = packet->vertices[2].diffuseColor;
  mm0PackedValue0ByteLane1 = (undefined1)(PVar2 >> 0x18);
  uVar9 = CONCAT31(CONCAT21((short)((unkuint10)_sVar19 >> 0x40),mm0PackedValue0ByteLane1),
                   mm0PackedValue0ByteLane1);
  mm0PackedValue0ByteLane2 = (undefined1)(PVar2 >> 0x10);
  mm0PackedValue0ByteLane1 = (undefined1)(PVar2 >> 8);
  mm1PackedValue0ByteLane1 = (undefined1)(PVar3 >> 0x18);
  uVar10 = CONCAT31(CONCAT21((short)((unkuint10)in_ST1 >> 0x40),mm1PackedValue0ByteLane1),
                    mm1PackedValue0ByteLane1);
  mm1PackedValue0ByteLane2 = (undefined1)(PVar3 >> 0x10);
  mm1PackedValue0ByteLane1 = (undefined1)(PVar3 >> 8);
  mm2PackedValue0ByteLane1 = (undefined1)(PVar4 >> 0x18);
  uVar11 = CONCAT31(CONCAT21((short)((unkuint10)in_ST2 >> 0x40),mm2PackedValue0ByteLane1),
                    mm2PackedValue0ByteLane1);
  mm2PackedValue0ByteLane2 = (undefined1)(PVar4 >> 0x10);
  mm2PackedValue0ByteLane1 = (undefined1)(PVar4 >> 8);
  mm3PackedValue0ByteLane1 = (undefined1)(PVar1 >> 0x18);
  uVar12 = CONCAT31(CONCAT21((short)((unkuint10)in_ST3 >> 0x40),mm3PackedValue0ByteLane1),
                    mm3PackedValue0ByteLane1);
  mm3PackedValue0ByteLane2 = (undefined1)(PVar1 >> 0x10);
  mm3PackedValue0ByteLane1 = (undefined1)(PVar1 >> 8);
  mm3PackedValue0._0_2_ = CONCAT11((char)PVar1,(char)PVar1) >> 4;
  mm3PackedValue0._2_2_ = CONCAT11(mm3PackedValue0ByteLane1,mm3PackedValue0ByteLane1) >> 4;
  mm3PackedValue0._0_4_ = CONCAT22(mm3PackedValue0._2_2_,(ushort)mm3PackedValue0);
  mm3PackedValue0._4_2_ =
       (ushort)(CONCAT55(CONCAT41(uVar12,mm3PackedValue0ByteLane2),
                         CONCAT14(mm3PackedValue0ByteLane2,PVar1)) >> 0x20);
  mm3PackedValue0._4_2_ = mm3PackedValue0._4_2_ >> 4;
  mm3PackedValue0._0_6_ = CONCAT24(mm3PackedValue0._4_2_,(undefined4)mm3PackedValue0);
  mm3PackedValue0._6_2_ = (ushort)uVar12;
  mm3PackedValue0._6_2_ = mm3PackedValue0._6_2_ >> 4;
  mm3PackedValue0 = CONCAT26(mm3PackedValue0._6_2_,(undefined6)mm3PackedValue0);
  mm0PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar9 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar9,mm0PackedValue0ByteLane2),
                                                  CONCAT14(mm0PackedValue0ByteLane2,PVar2)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                       mm0PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar2,(char)PVar2) >> 4))),
              mm3PackedValue0);
  mm1PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar10 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar10,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,PVar3)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                       mm1PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar3,(char)PVar3) >> 4))),
              mm3PackedValue0);
  mm2PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar11 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar11,mm2PackedValue0ByteLane2),
                                                  CONCAT14(mm2PackedValue0ByteLane2,PVar4)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm2PackedValue0ByteLane1,
                                                       mm2PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar4,(char)PVar4) >> 4))),
              mm3PackedValue0);
  sVar5 = (short)mm0PackedValue0;
  sVar6 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
  g_ImmediateTLVertices[0].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm0PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm0PackedValue0 -
                                  (0xff < sVar5))));
  sVar5 = (short)mm1PackedValue0;
  sVar6 = (short)((ulonglong)mm1PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm1PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm1PackedValue0 >> 0x30);
  g_ImmediateTLVertices[1].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm1PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm1PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm1PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm1PackedValue0 -
                                  (0xff < sVar5))));
  sVar5 = (short)mm2PackedValue0;
  sVar6 = (short)((ulonglong)mm2PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm2PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm2PackedValue0 >> 0x30);
  g_ImmediateTLVertices[2].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm2PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm2PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm2PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm2PackedValue0 -
                                  (0xff < sVar5))));
  g_ImmediateTLVertices[0].specular = 0;
  g_ImmediateTLVertices[1].specular = 0;
  g_ImmediateTLVertices[2].specular = 0;
  g_ImmediateTLVertices[0].sx = (float)packet->vertices[0].screenX;
  g_ImmediateTLVertices[0].sy = (float)packet->vertices[0].screenY;
  g_ImmediateTLVertices[1].sx = (float)packet->vertices[1].screenX;
  g_ImmediateTLVertices[1].sy = (float)packet->vertices[1].screenY;
  g_ImmediateTLVertices[2].sx = (float)packet->vertices[2].screenX;
  g_ImmediateTLVertices[2].sy = (float)packet->vertices[2].screenY;
  if (g_ImmediateTLVertices[0].sx != 0.0) {
    g_ImmediateTLVertices[0].sx = (float)((int)g_ImmediateTLVertices[0].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    g_ImmediateTLVertices[0].sy = (float)((int)g_ImmediateTLVertices[0].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    g_ImmediateTLVertices[1].sx = (float)((int)g_ImmediateTLVertices[1].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    g_ImmediateTLVertices[1].sy = (float)((int)g_ImmediateTLVertices[1].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    g_ImmediateTLVertices[2].sx = (float)((int)g_ImmediateTLVertices[2].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    g_ImmediateTLVertices[2].sy = (float)((int)g_ImmediateTLVertices[2].sy + -0x6000000);
  }
  g_ImmediateTLVertices[0].tu = 0.0;
  g_ImmediateTLVertices[0].tv = 0.0;
  g_ImmediateTLVertices[1].tu = 0.0;
  g_ImmediateTLVertices[1].tv = 0.0;
  g_ImmediateTLVertices[2].tu = 0.0;
  g_ImmediateTLVertices[2].tv = 0.0;
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    g_ImmediateTLVertices[0].sz = (float)((int)g_ImmediateTLVertices[0].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    g_ImmediateTLVertices[0].rhw = (float)((int)g_ImmediateTLVertices[0].rhw + 0xa000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    g_ImmediateTLVertices[1].sz = (float)((int)g_ImmediateTLVertices[1].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    g_ImmediateTLVertices[1].rhw = (float)((int)g_ImmediateTLVertices[1].rhw + 0xa000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    g_ImmediateTLVertices[2].sz = (float)((int)g_ImmediateTLVertices[2].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    g_ImmediateTLVertices[2].rhw = (float)((int)g_ImmediateTLVertices[2].rhw + 0xa000000);
  }
  if (3 < g_ImmediateVertexCount) {
    pDVar16 = g_ImmediateTLVertices + 1;
    pDVar17 = g_ImmediateTLVertices + 3;
    for (iVar14 = 8; iVar14 != 0; iVar14 = iVar14 + -1) {
      pDVar17->sx = pDVar16->sx;
      pDVar16 = (D3DTLVERTEX_DX6 *)&pDVar16->sy;
      pDVar17 = (D3DTLVERTEX_DX6 *)&pDVar17->sy;
    }
  }
  if (g_BoundTextureHandle != 0) {
    pIVar18 = g_Direct3DDevice2;
    sVar13 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (sVar13 != 0) {
      pIVar18 = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)pIVar18;
    g_TextureBindStateChangeCount = g_TextureBindStateChangeCount + 1;
  }
  return;
}


/* Address: 0x0057D1D0.
   Ownership: graphics/backend/direct3d.
   Purpose: Direct3D immediate primitive handler using render-state preset 3: SRCALPHA/INVSRCALPHA, alpha blending
   enabled, Z-write enabled; kept distinct from preset 1 because the executable uses separate table families.
   Multiplies each vertex diffuse color by packet->modulationColor through MMX, converts screen/depth values into
   g_ImmediateTLVertices, zeroes texture coordinates, duplicates the second vertex into slot four when
   g_ImmediateVertexCount is four, and binds texture handle zero.
*/
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_UntexturedPreset3(GraphicsPrimitivePacket *packet)

{
  PackedArgb32 PVar1;
  PackedArgb32 PVar2;
  PackedArgb32 PVar3;
  PackedArgb32 PVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  sdword sVar13;
  int iVar14;
  D3DDEVICEDESC_DX6 *pDVar15;
  D3DTLVERTEX_DX6 *pDVar16;
  D3DTLVERTEX_DX6 *pDVar17;
  undefined1 mm0PackedValue0ByteLane1;
  undefined1 mm0PackedValue0ByteLane2;
  sdword sVar19;
  undefined8 mm0PackedValue0;
  undefined8 mm1PackedValue0;
  undefined1 mm1PackedValue0ByteLane1;
  undefined1 mm1PackedValue0ByteLane2;
  unkbyte10 in_ST1;
  unkbyte10 extraout_ST1;
  unkbyte10 extraout_ST1_00;
  unkbyte10 extraout_ST1_01;
  unkbyte10 extraout_ST1_02;
  unkbyte10 extraout_ST1_03;
  unkbyte10 extraout_ST1_04;
  undefined8 mm2PackedValue0;
  undefined1 mm2PackedValue0ByteLane1;
  undefined1 mm2PackedValue0ByteLane2;
  unkbyte10 in_ST2;
  undefined8 mm3PackedValue0;
  undefined1 mm3PackedValue0ByteLane1;
  undefined1 mm3PackedValue0ByteLane2;
  unkbyte10 in_ST3;
  IDirect3DDevice2 *pIVar18;
  
  if (g_PrimitiveRenderStatePresets[3].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[3].zWriteEnable;
    _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[3].zWriteEnable);
    in_ST1 = extraout_ST1;
  }
  if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[3].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
      in_ST1 = extraout_ST1_00;
    }
    else {
      _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[3].alphaBlendEnable);
      pDVar15 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((pDVar15->dwFlags & 1) == 0) || (pDVar15->dcmColorModel == 2)) {
        pDVar15 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      in_ST1 = extraout_ST1_01;
      if (((pDVar15->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                            (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
        in_ST1 = extraout_ST1_02;
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[3].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[3].sourceBlend;
    _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
                         g_PrimitiveRenderStatePresets[3].sourceBlend);
    in_ST1 = extraout_ST1_03;
  }
  if (g_PrimitiveRenderStatePresets[3].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[3].destinationBlend
    ;
    _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
                         g_PrimitiveRenderStatePresets[3].destinationBlend);
    in_ST1 = extraout_ST1_04;
  }
  PVar1 = packet->modulationColor;
  PVar2 = packet->vertices[0].diffuseColor;
  PVar3 = packet->vertices[1].diffuseColor;
  PVar4 = packet->vertices[2].diffuseColor;
  mm0PackedValue0ByteLane1 = (undefined1)(PVar2 >> 0x18);
  uVar9 = CONCAT31(CONCAT21((short)((unkuint10)_sVar19 >> 0x40),mm0PackedValue0ByteLane1),
                   mm0PackedValue0ByteLane1);
  mm0PackedValue0ByteLane2 = (undefined1)(PVar2 >> 0x10);
  mm0PackedValue0ByteLane1 = (undefined1)(PVar2 >> 8);
  mm1PackedValue0ByteLane1 = (undefined1)(PVar3 >> 0x18);
  uVar10 = CONCAT31(CONCAT21((short)((unkuint10)in_ST1 >> 0x40),mm1PackedValue0ByteLane1),
                    mm1PackedValue0ByteLane1);
  mm1PackedValue0ByteLane2 = (undefined1)(PVar3 >> 0x10);
  mm1PackedValue0ByteLane1 = (undefined1)(PVar3 >> 8);
  mm2PackedValue0ByteLane1 = (undefined1)(PVar4 >> 0x18);
  uVar11 = CONCAT31(CONCAT21((short)((unkuint10)in_ST2 >> 0x40),mm2PackedValue0ByteLane1),
                    mm2PackedValue0ByteLane1);
  mm2PackedValue0ByteLane2 = (undefined1)(PVar4 >> 0x10);
  mm2PackedValue0ByteLane1 = (undefined1)(PVar4 >> 8);
  mm3PackedValue0ByteLane1 = (undefined1)(PVar1 >> 0x18);
  uVar12 = CONCAT31(CONCAT21((short)((unkuint10)in_ST3 >> 0x40),mm3PackedValue0ByteLane1),
                    mm3PackedValue0ByteLane1);
  mm3PackedValue0ByteLane2 = (undefined1)(PVar1 >> 0x10);
  mm3PackedValue0ByteLane1 = (undefined1)(PVar1 >> 8);
  mm3PackedValue0._0_2_ = CONCAT11((char)PVar1,(char)PVar1) >> 4;
  mm3PackedValue0._2_2_ = CONCAT11(mm3PackedValue0ByteLane1,mm3PackedValue0ByteLane1) >> 4;
  mm3PackedValue0._0_4_ = CONCAT22(mm3PackedValue0._2_2_,(ushort)mm3PackedValue0);
  mm3PackedValue0._4_2_ =
       (ushort)(CONCAT55(CONCAT41(uVar12,mm3PackedValue0ByteLane2),
                         CONCAT14(mm3PackedValue0ByteLane2,PVar1)) >> 0x20);
  mm3PackedValue0._4_2_ = mm3PackedValue0._4_2_ >> 4;
  mm3PackedValue0._0_6_ = CONCAT24(mm3PackedValue0._4_2_,(undefined4)mm3PackedValue0);
  mm3PackedValue0._6_2_ = (ushort)uVar12;
  mm3PackedValue0._6_2_ = mm3PackedValue0._6_2_ >> 4;
  mm3PackedValue0 = CONCAT26(mm3PackedValue0._6_2_,(undefined6)mm3PackedValue0);
  mm0PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar9 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar9,mm0PackedValue0ByteLane2),
                                                  CONCAT14(mm0PackedValue0ByteLane2,PVar2)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                       mm0PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar2,(char)PVar2) >> 4))),
              mm3PackedValue0);
  mm1PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar10 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar10,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,PVar3)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                       mm1PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar3,(char)PVar3) >> 4))),
              mm3PackedValue0);
  mm2PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar11 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar11,mm2PackedValue0ByteLane2),
                                                  CONCAT14(mm2PackedValue0ByteLane2,PVar4)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm2PackedValue0ByteLane1,
                                                       mm2PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar4,(char)PVar4) >> 4))),
              mm3PackedValue0);
  sVar5 = (short)mm0PackedValue0;
  sVar6 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
  g_ImmediateTLVertices[0].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm0PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm0PackedValue0 -
                                  (0xff < sVar5))));
  sVar5 = (short)mm1PackedValue0;
  sVar6 = (short)((ulonglong)mm1PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm1PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm1PackedValue0 >> 0x30);
  g_ImmediateTLVertices[1].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm1PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm1PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm1PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm1PackedValue0 -
                                  (0xff < sVar5))));
  sVar5 = (short)mm2PackedValue0;
  sVar6 = (short)((ulonglong)mm2PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm2PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm2PackedValue0 >> 0x30);
  g_ImmediateTLVertices[2].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm2PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm2PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm2PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm2PackedValue0 -
                                  (0xff < sVar5))));
  g_ImmediateTLVertices[0].specular = 0;
  g_ImmediateTLVertices[1].specular = 0;
  g_ImmediateTLVertices[2].specular = 0;
  g_ImmediateTLVertices[0].sx = (float)packet->vertices[0].screenX;
  g_ImmediateTLVertices[0].sy = (float)packet->vertices[0].screenY;
  g_ImmediateTLVertices[1].sx = (float)packet->vertices[1].screenX;
  g_ImmediateTLVertices[1].sy = (float)packet->vertices[1].screenY;
  g_ImmediateTLVertices[2].sx = (float)packet->vertices[2].screenX;
  g_ImmediateTLVertices[2].sy = (float)packet->vertices[2].screenY;
  if (g_ImmediateTLVertices[0].sx != 0.0) {
    g_ImmediateTLVertices[0].sx = (float)((int)g_ImmediateTLVertices[0].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    g_ImmediateTLVertices[0].sy = (float)((int)g_ImmediateTLVertices[0].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    g_ImmediateTLVertices[1].sx = (float)((int)g_ImmediateTLVertices[1].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    g_ImmediateTLVertices[1].sy = (float)((int)g_ImmediateTLVertices[1].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    g_ImmediateTLVertices[2].sx = (float)((int)g_ImmediateTLVertices[2].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    g_ImmediateTLVertices[2].sy = (float)((int)g_ImmediateTLVertices[2].sy + -0x6000000);
  }
  g_ImmediateTLVertices[0].tu = 0.0;
  g_ImmediateTLVertices[0].tv = 0.0;
  g_ImmediateTLVertices[1].tu = 0.0;
  g_ImmediateTLVertices[1].tv = 0.0;
  g_ImmediateTLVertices[2].tu = 0.0;
  g_ImmediateTLVertices[2].tv = 0.0;
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    g_ImmediateTLVertices[0].sz = (float)((int)g_ImmediateTLVertices[0].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    g_ImmediateTLVertices[0].rhw = (float)((int)g_ImmediateTLVertices[0].rhw + 0xa000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    g_ImmediateTLVertices[1].sz = (float)((int)g_ImmediateTLVertices[1].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    g_ImmediateTLVertices[1].rhw = (float)((int)g_ImmediateTLVertices[1].rhw + 0xa000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    g_ImmediateTLVertices[2].sz = (float)((int)g_ImmediateTLVertices[2].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    g_ImmediateTLVertices[2].rhw = (float)((int)g_ImmediateTLVertices[2].rhw + 0xa000000);
  }
  if (3 < g_ImmediateVertexCount) {
    pDVar16 = g_ImmediateTLVertices + 1;
    pDVar17 = g_ImmediateTLVertices + 3;
    for (iVar14 = 8; iVar14 != 0; iVar14 = iVar14 + -1) {
      pDVar17->sx = pDVar16->sx;
      pDVar16 = (D3DTLVERTEX_DX6 *)&pDVar16->sy;
      pDVar17 = (D3DTLVERTEX_DX6 *)&pDVar17->sy;
    }
  }
  if (g_BoundTextureHandle != 0) {
    pIVar18 = g_Direct3DDevice2;
    sVar13 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (sVar13 != 0) {
      pIVar18 = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)pIVar18;
    g_TextureBindStateChangeCount = g_TextureBindStateChangeCount + 1;
  }
  return;
}


/* Address: 0x0057D480.
   Ownership: graphics/backend/direct3d.
   Purpose: Direct3D immediate primitive handler using render-state preset 4: ONE/ONE additive blending, alpha
   blending enabled, Z-write disabled. Multiplies each vertex diffuse color by packet->modulationColor through MMX,
   converts screen/depth values into g_ImmediateTLVertices, zeroes texture coordinates, duplicates the second
   vertex into slot four when g_ImmediateVertexCount is four, and binds texture handle zero.
*/
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_UntexturedPreset4(GraphicsPrimitivePacket *packet)

{
  PackedArgb32 PVar1;
  PackedArgb32 PVar2;
  PackedArgb32 PVar3;
  PackedArgb32 PVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  sdword sVar13;
  int iVar14;
  D3DDEVICEDESC_DX6 *pDVar15;
  D3DTLVERTEX_DX6 *pDVar16;
  D3DTLVERTEX_DX6 *pDVar17;
  undefined1 mm0PackedValue0ByteLane1;
  undefined1 mm0PackedValue0ByteLane2;
  sdword sVar19;
  undefined8 mm0PackedValue0;
  undefined8 mm1PackedValue0;
  undefined1 mm1PackedValue0ByteLane1;
  undefined1 mm1PackedValue0ByteLane2;
  unkbyte10 in_ST1;
  unkbyte10 extraout_ST1;
  unkbyte10 extraout_ST1_00;
  unkbyte10 extraout_ST1_01;
  unkbyte10 extraout_ST1_02;
  unkbyte10 extraout_ST1_03;
  unkbyte10 extraout_ST1_04;
  undefined8 mm2PackedValue0;
  undefined1 mm2PackedValue0ByteLane1;
  undefined1 mm2PackedValue0ByteLane2;
  unkbyte10 in_ST2;
  undefined8 mm3PackedValue0;
  undefined1 mm3PackedValue0ByteLane1;
  undefined1 mm3PackedValue0ByteLane2;
  unkbyte10 in_ST3;
  IDirect3DDevice2 *pIVar18;
  
  if (g_PrimitiveRenderStatePresets[4].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[4].zWriteEnable;
    _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[4].zWriteEnable);
    in_ST1 = extraout_ST1;
  }
  if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[4].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
      in_ST1 = extraout_ST1_00;
    }
    else {
      _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[4].alphaBlendEnable);
      pDVar15 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((pDVar15->dwFlags & 1) == 0) || (pDVar15->dcmColorModel == 2)) {
        pDVar15 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      in_ST1 = extraout_ST1_01;
      if (((pDVar15->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                            (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
        in_ST1 = extraout_ST1_02;
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[4].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[4].sourceBlend;
    _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
                         g_PrimitiveRenderStatePresets[4].sourceBlend);
    in_ST1 = extraout_ST1_03;
  }
  if (g_PrimitiveRenderStatePresets[4].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[4].destinationBlend
    ;
    _sVar19 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
                         g_PrimitiveRenderStatePresets[4].destinationBlend);
    in_ST1 = extraout_ST1_04;
  }
  PVar1 = packet->modulationColor;
  PVar2 = packet->vertices[0].diffuseColor;
  PVar3 = packet->vertices[1].diffuseColor;
  PVar4 = packet->vertices[2].diffuseColor;
  mm0PackedValue0ByteLane1 = (undefined1)(PVar2 >> 0x18);
  uVar9 = CONCAT31(CONCAT21((short)((unkuint10)_sVar19 >> 0x40),mm0PackedValue0ByteLane1),
                   mm0PackedValue0ByteLane1);
  mm0PackedValue0ByteLane2 = (undefined1)(PVar2 >> 0x10);
  mm0PackedValue0ByteLane1 = (undefined1)(PVar2 >> 8);
  mm1PackedValue0ByteLane1 = (undefined1)(PVar3 >> 0x18);
  uVar10 = CONCAT31(CONCAT21((short)((unkuint10)in_ST1 >> 0x40),mm1PackedValue0ByteLane1),
                    mm1PackedValue0ByteLane1);
  mm1PackedValue0ByteLane2 = (undefined1)(PVar3 >> 0x10);
  mm1PackedValue0ByteLane1 = (undefined1)(PVar3 >> 8);
  mm2PackedValue0ByteLane1 = (undefined1)(PVar4 >> 0x18);
  uVar11 = CONCAT31(CONCAT21((short)((unkuint10)in_ST2 >> 0x40),mm2PackedValue0ByteLane1),
                    mm2PackedValue0ByteLane1);
  mm2PackedValue0ByteLane2 = (undefined1)(PVar4 >> 0x10);
  mm2PackedValue0ByteLane1 = (undefined1)(PVar4 >> 8);
  mm3PackedValue0ByteLane1 = (undefined1)(PVar1 >> 0x18);
  uVar12 = CONCAT31(CONCAT21((short)((unkuint10)in_ST3 >> 0x40),mm3PackedValue0ByteLane1),
                    mm3PackedValue0ByteLane1);
  mm3PackedValue0ByteLane2 = (undefined1)(PVar1 >> 0x10);
  mm3PackedValue0ByteLane1 = (undefined1)(PVar1 >> 8);
  mm3PackedValue0._0_2_ = CONCAT11((char)PVar1,(char)PVar1) >> 4;
  mm3PackedValue0._2_2_ = CONCAT11(mm3PackedValue0ByteLane1,mm3PackedValue0ByteLane1) >> 4;
  mm3PackedValue0._0_4_ = CONCAT22(mm3PackedValue0._2_2_,(ushort)mm3PackedValue0);
  mm3PackedValue0._4_2_ =
       (ushort)(CONCAT55(CONCAT41(uVar12,mm3PackedValue0ByteLane2),
                         CONCAT14(mm3PackedValue0ByteLane2,PVar1)) >> 0x20);
  mm3PackedValue0._4_2_ = mm3PackedValue0._4_2_ >> 4;
  mm3PackedValue0._0_6_ = CONCAT24(mm3PackedValue0._4_2_,(undefined4)mm3PackedValue0);
  mm3PackedValue0._6_2_ = (ushort)uVar12;
  mm3PackedValue0._6_2_ = mm3PackedValue0._6_2_ >> 4;
  mm3PackedValue0 = CONCAT26(mm3PackedValue0._6_2_,(undefined6)mm3PackedValue0);
  mm0PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar9 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar9,mm0PackedValue0ByteLane2),
                                                  CONCAT14(mm0PackedValue0ByteLane2,PVar2)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                       mm0PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar2,(char)PVar2) >> 4))),
              mm3PackedValue0);
  mm1PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar10 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar10,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,PVar3)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                       mm1PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar3,(char)PVar3) >> 4))),
              mm3PackedValue0);
  mm2PackedValue0 =
       pmulhw(CONCAT26((ushort)uVar11 >> 4,
                       CONCAT24((ushort)(CONCAT55(CONCAT41(uVar11,mm2PackedValue0ByteLane2),
                                                  CONCAT14(mm2PackedValue0ByteLane2,PVar4)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm2PackedValue0ByteLane1,
                                                       mm2PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar4,(char)PVar4) >> 4))),
              mm3PackedValue0);
  sVar5 = (short)mm0PackedValue0;
  sVar6 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
  g_ImmediateTLVertices[0].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm0PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm0PackedValue0 -
                                  (0xff < sVar5))));
  sVar5 = (short)mm1PackedValue0;
  sVar6 = (short)((ulonglong)mm1PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm1PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm1PackedValue0 >> 0x30);
  g_ImmediateTLVertices[1].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm1PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm1PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm1PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm1PackedValue0 -
                                  (0xff < sVar5))));
  sVar5 = (short)mm2PackedValue0;
  sVar6 = (short)((ulonglong)mm2PackedValue0 >> 0x10);
  sVar7 = (short)((ulonglong)mm2PackedValue0 >> 0x20);
  sVar8 = (short)((ulonglong)mm2PackedValue0 >> 0x30);
  g_ImmediateTLVertices[2].color =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm2PackedValue0 >> 0x30) -
                (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)mm2PackedValue0 >> 0x20)
                         - (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                  (char)((ulonglong)mm2PackedValue0 >> 0x10) - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)mm2PackedValue0 -
                                  (0xff < sVar5))));
  g_ImmediateTLVertices[0].specular = 0;
  g_ImmediateTLVertices[1].specular = 0;
  g_ImmediateTLVertices[2].specular = 0;
  g_ImmediateTLVertices[0].sx = (float)packet->vertices[0].screenX;
  g_ImmediateTLVertices[0].sy = (float)packet->vertices[0].screenY;
  g_ImmediateTLVertices[1].sx = (float)packet->vertices[1].screenX;
  g_ImmediateTLVertices[1].sy = (float)packet->vertices[1].screenY;
  g_ImmediateTLVertices[2].sx = (float)packet->vertices[2].screenX;
  g_ImmediateTLVertices[2].sy = (float)packet->vertices[2].screenY;
  if (g_ImmediateTLVertices[0].sx != 0.0) {
    g_ImmediateTLVertices[0].sx = (float)((int)g_ImmediateTLVertices[0].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    g_ImmediateTLVertices[0].sy = (float)((int)g_ImmediateTLVertices[0].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    g_ImmediateTLVertices[1].sx = (float)((int)g_ImmediateTLVertices[1].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    g_ImmediateTLVertices[1].sy = (float)((int)g_ImmediateTLVertices[1].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    g_ImmediateTLVertices[2].sx = (float)((int)g_ImmediateTLVertices[2].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    g_ImmediateTLVertices[2].sy = (float)((int)g_ImmediateTLVertices[2].sy + -0x6000000);
  }
  g_ImmediateTLVertices[0].tu = 0.0;
  g_ImmediateTLVertices[0].tv = 0.0;
  g_ImmediateTLVertices[1].tu = 0.0;
  g_ImmediateTLVertices[1].tv = 0.0;
  g_ImmediateTLVertices[2].tu = 0.0;
  g_ImmediateTLVertices[2].tv = 0.0;
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    g_ImmediateTLVertices[0].sz = (float)((int)g_ImmediateTLVertices[0].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    g_ImmediateTLVertices[0].rhw = (float)((int)g_ImmediateTLVertices[0].rhw + 0xa000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    g_ImmediateTLVertices[1].sz = (float)((int)g_ImmediateTLVertices[1].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    g_ImmediateTLVertices[1].rhw = (float)((int)g_ImmediateTLVertices[1].rhw + 0xa000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    g_ImmediateTLVertices[2].sz = (float)((int)g_ImmediateTLVertices[2].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    g_ImmediateTLVertices[2].rhw = (float)((int)g_ImmediateTLVertices[2].rhw + 0xa000000);
  }
  if (3 < g_ImmediateVertexCount) {
    pDVar16 = g_ImmediateTLVertices + 1;
    pDVar17 = g_ImmediateTLVertices + 3;
    for (iVar14 = 8; iVar14 != 0; iVar14 = iVar14 + -1) {
      pDVar17->sx = pDVar16->sx;
      pDVar16 = (D3DTLVERTEX_DX6 *)&pDVar16->sy;
      pDVar17 = (D3DTLVERTEX_DX6 *)&pDVar17->sy;
    }
  }
  if (g_BoundTextureHandle != 0) {
    pIVar18 = g_Direct3DDevice2;
    sVar13 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (sVar13 != 0) {
      pIVar18 = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)pIVar18;
    g_TextureBindStateChangeCount = g_TextureBindStateChangeCount + 1;
  }
  return;
}


/* Address: 0x0057D730.
   Ownership: graphics/backend/direct3d.
   Purpose: Direct3D immediate textured primitive handler using render-state preset 0: ONE/ZERO, alpha blending
   disabled, Z-write enabled. Copies vertex diffuse colors, normalizes U/V to the larger texture log2 dimension,
   converts screen/depth values into g_ImmediateTLVertices, duplicates the second vertex into slot four when
   requested, updates texture->lastUsedCounter from g_TextureUseSerial, lazily creates a missing device texture,
   and binds its D3DTEXTUREHANDLE.
   Cross-module calls: GraphicsTexture_CreateDeviceTexture [graphics/resources/texture].
*/
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_TexturedPreset0(GraphicsPrimitivePacket *packet)

{
  GraphicsPrimitiveTextureCoordinateFixed *pGVar1;
  uint uVar2;
  uint uVar3;
  GraphicsTextureSetEntry *pGVar4;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle arg2;
  sdword sVar5;
  byte bVar6;
  uint uVar7;
  int iVar8;
  D3DDEVICEDESC_DX6 *pDVar9;
  D3DTLVERTEX_DX6 *pDVar10;
  D3DTLVERTEX_DX6 *pDVar11;
  IDirect3DDevice2 *pIVar12;
  
  if (g_PrimitiveRenderStatePresets[0].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[0].zWriteEnable;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[0].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[0].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[0].alphaBlendEnable);
      pDVar9 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((pDVar9->dwFlags & 1) == 0) || (pDVar9->dcmColorModel == 2)) {
        pDVar9 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((pDVar9->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                  (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[0].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[0].sourceBlend;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
               g_PrimitiveRenderStatePresets[0].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[0].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[0].destinationBlend
    ;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
               g_PrimitiveRenderStatePresets[0].destinationBlend);
  }
  g_ImmediateTLVertices[0].color = packet->vertices[0].diffuseColor;
  g_ImmediateTLVertices[1].color = packet->vertices[1].diffuseColor;
  g_ImmediateTLVertices[2].color = packet->vertices[2].diffuseColor;
  g_ImmediateTLVertices[0].specular = 0;
  g_ImmediateTLVertices[1].specular = 0;
  g_ImmediateTLVertices[2].specular = 0;
  g_ImmediateTLVertices[0].sx = (float)packet->vertices[0].screenX;
  g_ImmediateTLVertices[0].sy = (float)packet->vertices[0].screenY;
  g_ImmediateTLVertices[1].sx = (float)packet->vertices[1].screenX;
  g_ImmediateTLVertices[1].sy = (float)packet->vertices[1].screenY;
  g_ImmediateTLVertices[2].sx = (float)packet->vertices[2].screenX;
  g_ImmediateTLVertices[2].sy = (float)packet->vertices[2].screenY;
  if (g_ImmediateTLVertices[0].sx != 0.0) {
    g_ImmediateTLVertices[0].sx = (float)((int)g_ImmediateTLVertices[0].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    g_ImmediateTLVertices[0].sy = (float)((int)g_ImmediateTLVertices[0].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    g_ImmediateTLVertices[1].sx = (float)((int)g_ImmediateTLVertices[1].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    g_ImmediateTLVertices[1].sy = (float)((int)g_ImmediateTLVertices[1].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    g_ImmediateTLVertices[2].sx = (float)((int)g_ImmediateTLVertices[2].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    g_ImmediateTLVertices[2].sy = (float)((int)g_ImmediateTLVertices[2].sy + -0x6000000);
  }
  uVar2 = packet->textureEntry->widthLog2;
  uVar3 = packet->textureEntry->heightLog2;
  uVar7 = uVar2;
  if (uVar2 < uVar3) {
    uVar7 = uVar3;
  }
  bVar6 = (char)uVar7 - (char)uVar2;
  pGVar1 = &packet->vertices[0].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[1].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[2].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  bVar6 = (char)uVar7 - (char)uVar3;
  pGVar1 = &packet->vertices[0].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[1].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[2].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    g_ImmediateTLVertices[0].tu = (float)((int)g_ImmediateTLVertices[0].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    g_ImmediateTLVertices[0].tv = (float)((int)g_ImmediateTLVertices[0].tv + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    g_ImmediateTLVertices[1].tu = (float)((int)g_ImmediateTLVertices[1].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    g_ImmediateTLVertices[1].tv = (float)((int)g_ImmediateTLVertices[1].tv + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    g_ImmediateTLVertices[2].tu = (float)((int)g_ImmediateTLVertices[2].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    g_ImmediateTLVertices[2].tv = (float)((int)g_ImmediateTLVertices[2].tv + -0xa000000);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    g_ImmediateTLVertices[0].sz = (float)((int)g_ImmediateTLVertices[0].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    g_ImmediateTLVertices[0].rhw = (float)((int)g_ImmediateTLVertices[0].rhw + 0x9000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    g_ImmediateTLVertices[1].sz = (float)((int)g_ImmediateTLVertices[1].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    g_ImmediateTLVertices[1].rhw = (float)((int)g_ImmediateTLVertices[1].rhw + 0x9000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    g_ImmediateTLVertices[2].sz = (float)((int)g_ImmediateTLVertices[2].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    g_ImmediateTLVertices[2].rhw = (float)((int)g_ImmediateTLVertices[2].rhw + 0x9000000);
  }
  pGVar4 = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    pDVar10 = g_ImmediateTLVertices + 1;
    pDVar11 = g_ImmediateTLVertices + 3;
    for (iVar8 = 8; iVar8 != 0; iVar8 = iVar8 + -1) {
      pDVar11->sx = pDVar10->sx;
      pDVar10 = (D3DTLVERTEX_DX6 *)&pDVar10->sy;
      pDVar11 = (D3DTLVERTEX_DX6 *)&pDVar11->sy;
    }
  }
  texture = pGVar4->texture;
  arg2 = 0;
  if (texture != (GraphicsTextureResource *)0x0) {
    arg2 = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial = g_TextureUseSerial + 1;
    if (arg2 == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
      arg2 = texture->textureHandle;
    }
  }
  if (arg2 != g_BoundTextureHandle) {
    pIVar12 = g_Direct3DDevice2;
    sVar5 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,arg2);
    if (sVar5 != 0) {
      pIVar12 = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)pIVar12;
    g_TextureBindStateChangeCount = g_TextureBindStateChangeCount + 1;
  }
  return;
}


/* Address: 0x0057DA50.
   Ownership: graphics/backend/direct3d.
   Purpose: Direct3D immediate textured primitive handler using render-state preset 1: SRCALPHA/INVSRCALPHA, alpha
   blending enabled, Z-write enabled. Copies vertex diffuse colors, normalizes U/V to the larger texture log2
   dimension, converts screen/depth values into g_ImmediateTLVertices, duplicates the second vertex into slot four
   when requested, updates texture->lastUsedCounter from g_TextureUseSerial, lazily creates a missing device
   texture, and binds its D3DTEXTUREHANDLE.
   Cross-module calls: GraphicsTexture_CreateDeviceTexture [graphics/resources/texture].
*/
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_TexturedPreset1(GraphicsPrimitivePacket *packet)

{
  GraphicsPrimitiveTextureCoordinateFixed *pGVar1;
  uint uVar2;
  uint uVar3;
  GraphicsTextureSetEntry *pGVar4;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle arg2;
  sdword sVar5;
  byte bVar6;
  uint uVar7;
  int iVar8;
  D3DDEVICEDESC_DX6 *pDVar9;
  D3DTLVERTEX_DX6 *pDVar10;
  D3DTLVERTEX_DX6 *pDVar11;
  IDirect3DDevice2 *pIVar12;
  
  if (g_PrimitiveRenderStatePresets[1].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[1].zWriteEnable;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[1].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[1].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[1].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[1].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[1].alphaBlendEnable);
      pDVar9 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((pDVar9->dwFlags & 1) == 0) || (pDVar9->dcmColorModel == 2)) {
        pDVar9 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((pDVar9->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                  (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[1].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[1].sourceBlend;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
               g_PrimitiveRenderStatePresets[1].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[1].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[1].destinationBlend
    ;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
               g_PrimitiveRenderStatePresets[1].destinationBlend);
  }
  g_ImmediateTLVertices[0].color = packet->vertices[0].diffuseColor;
  g_ImmediateTLVertices[1].color = packet->vertices[1].diffuseColor;
  g_ImmediateTLVertices[2].color = packet->vertices[2].diffuseColor;
  g_ImmediateTLVertices[0].specular = 0;
  g_ImmediateTLVertices[1].specular = 0;
  g_ImmediateTLVertices[2].specular = 0;
  g_ImmediateTLVertices[0].sx = (float)packet->vertices[0].screenX;
  g_ImmediateTLVertices[0].sy = (float)packet->vertices[0].screenY;
  g_ImmediateTLVertices[1].sx = (float)packet->vertices[1].screenX;
  g_ImmediateTLVertices[1].sy = (float)packet->vertices[1].screenY;
  g_ImmediateTLVertices[2].sx = (float)packet->vertices[2].screenX;
  g_ImmediateTLVertices[2].sy = (float)packet->vertices[2].screenY;
  if (g_ImmediateTLVertices[0].sx != 0.0) {
    g_ImmediateTLVertices[0].sx = (float)((int)g_ImmediateTLVertices[0].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    g_ImmediateTLVertices[0].sy = (float)((int)g_ImmediateTLVertices[0].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    g_ImmediateTLVertices[1].sx = (float)((int)g_ImmediateTLVertices[1].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    g_ImmediateTLVertices[1].sy = (float)((int)g_ImmediateTLVertices[1].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    g_ImmediateTLVertices[2].sx = (float)((int)g_ImmediateTLVertices[2].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    g_ImmediateTLVertices[2].sy = (float)((int)g_ImmediateTLVertices[2].sy + -0x6000000);
  }
  uVar2 = packet->textureEntry->widthLog2;
  uVar3 = packet->textureEntry->heightLog2;
  uVar7 = uVar2;
  if (uVar2 < uVar3) {
    uVar7 = uVar3;
  }
  bVar6 = (char)uVar7 - (char)uVar2;
  pGVar1 = &packet->vertices[0].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[1].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[2].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  bVar6 = (char)uVar7 - (char)uVar3;
  pGVar1 = &packet->vertices[0].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[1].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[2].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    g_ImmediateTLVertices[0].tu = (float)((int)g_ImmediateTLVertices[0].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    g_ImmediateTLVertices[0].tv = (float)((int)g_ImmediateTLVertices[0].tv + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    g_ImmediateTLVertices[1].tu = (float)((int)g_ImmediateTLVertices[1].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    g_ImmediateTLVertices[1].tv = (float)((int)g_ImmediateTLVertices[1].tv + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    g_ImmediateTLVertices[2].tu = (float)((int)g_ImmediateTLVertices[2].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    g_ImmediateTLVertices[2].tv = (float)((int)g_ImmediateTLVertices[2].tv + -0xa000000);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    g_ImmediateTLVertices[0].sz = (float)((int)g_ImmediateTLVertices[0].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    g_ImmediateTLVertices[0].rhw = (float)((int)g_ImmediateTLVertices[0].rhw + 0x9000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    g_ImmediateTLVertices[1].sz = (float)((int)g_ImmediateTLVertices[1].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    g_ImmediateTLVertices[1].rhw = (float)((int)g_ImmediateTLVertices[1].rhw + 0x9000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    g_ImmediateTLVertices[2].sz = (float)((int)g_ImmediateTLVertices[2].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    g_ImmediateTLVertices[2].rhw = (float)((int)g_ImmediateTLVertices[2].rhw + 0x9000000);
  }
  pGVar4 = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    pDVar10 = g_ImmediateTLVertices + 1;
    pDVar11 = g_ImmediateTLVertices + 3;
    for (iVar8 = 8; iVar8 != 0; iVar8 = iVar8 + -1) {
      pDVar11->sx = pDVar10->sx;
      pDVar10 = (D3DTLVERTEX_DX6 *)&pDVar10->sy;
      pDVar11 = (D3DTLVERTEX_DX6 *)&pDVar11->sy;
    }
  }
  texture = pGVar4->texture;
  arg2 = 0;
  if (texture != (GraphicsTextureResource *)0x0) {
    arg2 = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial = g_TextureUseSerial + 1;
    if (arg2 == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
      arg2 = texture->textureHandle;
    }
  }
  if (arg2 != g_BoundTextureHandle) {
    pIVar12 = g_Direct3DDevice2;
    sVar5 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,arg2);
    if (sVar5 != 0) {
      pIVar12 = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)pIVar12;
    g_TextureBindStateChangeCount = g_TextureBindStateChangeCount + 1;
  }
  return;
}


/* Address: 0x0057DD70.
   Ownership: graphics/backend/direct3d.
   Purpose: Direct3D immediate textured primitive handler using render-state preset 2: SRCALPHA/INVSRCALPHA, alpha
   blending enabled, Z-write disabled. Copies vertex diffuse colors, normalizes U/V to the larger texture log2
   dimension, converts screen/depth values into g_ImmediateTLVertices, duplicates the second vertex into slot four
   when requested, updates texture->lastUsedCounter from g_TextureUseSerial, lazily creates a missing device
   texture, and binds its D3DTEXTUREHANDLE.
   Cross-module calls: GraphicsTexture_CreateDeviceTexture [graphics/resources/texture].
*/
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_TexturedPreset2(GraphicsPrimitivePacket *packet)

{
  GraphicsPrimitiveTextureCoordinateFixed *pGVar1;
  uint uVar2;
  uint uVar3;
  GraphicsTextureSetEntry *pGVar4;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle arg2;
  sdword sVar5;
  byte bVar6;
  uint uVar7;
  int iVar8;
  D3DDEVICEDESC_DX6 *pDVar9;
  D3DTLVERTEX_DX6 *pDVar10;
  D3DTLVERTEX_DX6 *pDVar11;
  IDirect3DDevice2 *pIVar12;
  
  if (g_PrimitiveRenderStatePresets[2].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[2].zWriteEnable;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[2].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[2].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[2].alphaBlendEnable);
      pDVar9 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((pDVar9->dwFlags & 1) == 0) || (pDVar9->dcmColorModel == 2)) {
        pDVar9 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((pDVar9->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                  (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[2].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[2].sourceBlend;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
               g_PrimitiveRenderStatePresets[2].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[2].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[2].destinationBlend
    ;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
               g_PrimitiveRenderStatePresets[2].destinationBlend);
  }
  g_ImmediateTLVertices[0].color = packet->vertices[0].diffuseColor;
  g_ImmediateTLVertices[1].color = packet->vertices[1].diffuseColor;
  g_ImmediateTLVertices[2].color = packet->vertices[2].diffuseColor;
  g_ImmediateTLVertices[0].specular = 0;
  g_ImmediateTLVertices[1].specular = 0;
  g_ImmediateTLVertices[2].specular = 0;
  g_ImmediateTLVertices[0].sx = (float)packet->vertices[0].screenX;
  g_ImmediateTLVertices[0].sy = (float)packet->vertices[0].screenY;
  g_ImmediateTLVertices[1].sx = (float)packet->vertices[1].screenX;
  g_ImmediateTLVertices[1].sy = (float)packet->vertices[1].screenY;
  g_ImmediateTLVertices[2].sx = (float)packet->vertices[2].screenX;
  g_ImmediateTLVertices[2].sy = (float)packet->vertices[2].screenY;
  if (g_ImmediateTLVertices[0].sx != 0.0) {
    g_ImmediateTLVertices[0].sx = (float)((int)g_ImmediateTLVertices[0].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    g_ImmediateTLVertices[0].sy = (float)((int)g_ImmediateTLVertices[0].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    g_ImmediateTLVertices[1].sx = (float)((int)g_ImmediateTLVertices[1].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    g_ImmediateTLVertices[1].sy = (float)((int)g_ImmediateTLVertices[1].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    g_ImmediateTLVertices[2].sx = (float)((int)g_ImmediateTLVertices[2].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    g_ImmediateTLVertices[2].sy = (float)((int)g_ImmediateTLVertices[2].sy + -0x6000000);
  }
  uVar2 = packet->textureEntry->widthLog2;
  uVar3 = packet->textureEntry->heightLog2;
  uVar7 = uVar2;
  if (uVar2 < uVar3) {
    uVar7 = uVar3;
  }
  bVar6 = (char)uVar7 - (char)uVar2;
  pGVar1 = &packet->vertices[0].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[1].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[2].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  bVar6 = (char)uVar7 - (char)uVar3;
  pGVar1 = &packet->vertices[0].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[1].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[2].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    g_ImmediateTLVertices[0].tu = (float)((int)g_ImmediateTLVertices[0].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    g_ImmediateTLVertices[0].tv = (float)((int)g_ImmediateTLVertices[0].tv + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    g_ImmediateTLVertices[1].tu = (float)((int)g_ImmediateTLVertices[1].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    g_ImmediateTLVertices[1].tv = (float)((int)g_ImmediateTLVertices[1].tv + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    g_ImmediateTLVertices[2].tu = (float)((int)g_ImmediateTLVertices[2].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    g_ImmediateTLVertices[2].tv = (float)((int)g_ImmediateTLVertices[2].tv + -0xa000000);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    g_ImmediateTLVertices[0].sz = (float)((int)g_ImmediateTLVertices[0].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    g_ImmediateTLVertices[0].rhw = (float)((int)g_ImmediateTLVertices[0].rhw + 0x9000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    g_ImmediateTLVertices[1].sz = (float)((int)g_ImmediateTLVertices[1].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    g_ImmediateTLVertices[1].rhw = (float)((int)g_ImmediateTLVertices[1].rhw + 0x9000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    g_ImmediateTLVertices[2].sz = (float)((int)g_ImmediateTLVertices[2].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    g_ImmediateTLVertices[2].rhw = (float)((int)g_ImmediateTLVertices[2].rhw + 0x9000000);
  }
  pGVar4 = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    pDVar10 = g_ImmediateTLVertices + 1;
    pDVar11 = g_ImmediateTLVertices + 3;
    for (iVar8 = 8; iVar8 != 0; iVar8 = iVar8 + -1) {
      pDVar11->sx = pDVar10->sx;
      pDVar10 = (D3DTLVERTEX_DX6 *)&pDVar10->sy;
      pDVar11 = (D3DTLVERTEX_DX6 *)&pDVar11->sy;
    }
  }
  texture = pGVar4->texture;
  arg2 = 0;
  if (texture != (GraphicsTextureResource *)0x0) {
    arg2 = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial = g_TextureUseSerial + 1;
    if (arg2 == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
      arg2 = texture->textureHandle;
    }
  }
  if (arg2 != g_BoundTextureHandle) {
    pIVar12 = g_Direct3DDevice2;
    sVar5 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,arg2);
    if (sVar5 != 0) {
      pIVar12 = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)pIVar12;
    g_TextureBindStateChangeCount = g_TextureBindStateChangeCount + 1;
  }
  return;
}


/* Address: 0x0057E090.
   Ownership: graphics/backend/direct3d.
   Purpose: Direct3D immediate textured primitive handler using render-state preset 3: SRCALPHA/INVSRCALPHA, alpha
   blending enabled, Z-write enabled; kept distinct from preset 1 because the executable uses separate table
   families. Copies vertex diffuse colors, normalizes U/V to the larger texture log2 dimension, converts
   screen/depth values into g_ImmediateTLVertices, duplicates the second vertex into slot four when requested,
   updates texture->lastUsedCounter from g_TextureUseSerial, lazily creates a missing device texture, and binds its
   D3DTEXTUREHANDLE.
   Cross-module calls: GraphicsTexture_CreateDeviceTexture [graphics/resources/texture].
*/
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_TexturedPreset3(GraphicsPrimitivePacket *packet)

{
  GraphicsPrimitiveTextureCoordinateFixed *pGVar1;
  uint uVar2;
  uint uVar3;
  GraphicsTextureSetEntry *pGVar4;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle arg2;
  sdword sVar5;
  byte bVar6;
  uint uVar7;
  int iVar8;
  D3DDEVICEDESC_DX6 *pDVar9;
  D3DTLVERTEX_DX6 *pDVar10;
  D3DTLVERTEX_DX6 *pDVar11;
  IDirect3DDevice2 *pIVar12;
  
  if (g_PrimitiveRenderStatePresets[3].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[3].zWriteEnable;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[3].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[3].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[3].alphaBlendEnable);
      pDVar9 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((pDVar9->dwFlags & 1) == 0) || (pDVar9->dcmColorModel == 2)) {
        pDVar9 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((pDVar9->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                  (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[3].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[3].sourceBlend;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
               g_PrimitiveRenderStatePresets[3].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[3].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[3].destinationBlend
    ;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
               g_PrimitiveRenderStatePresets[3].destinationBlend);
  }
  g_ImmediateTLVertices[0].color = packet->vertices[0].diffuseColor;
  g_ImmediateTLVertices[1].color = packet->vertices[1].diffuseColor;
  g_ImmediateTLVertices[2].color = packet->vertices[2].diffuseColor;
  g_ImmediateTLVertices[0].specular = 0;
  g_ImmediateTLVertices[1].specular = 0;
  g_ImmediateTLVertices[2].specular = 0;
  g_ImmediateTLVertices[0].sx = (float)packet->vertices[0].screenX;
  g_ImmediateTLVertices[0].sy = (float)packet->vertices[0].screenY;
  g_ImmediateTLVertices[1].sx = (float)packet->vertices[1].screenX;
  g_ImmediateTLVertices[1].sy = (float)packet->vertices[1].screenY;
  g_ImmediateTLVertices[2].sx = (float)packet->vertices[2].screenX;
  g_ImmediateTLVertices[2].sy = (float)packet->vertices[2].screenY;
  if (g_ImmediateTLVertices[0].sx != 0.0) {
    g_ImmediateTLVertices[0].sx = (float)((int)g_ImmediateTLVertices[0].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    g_ImmediateTLVertices[0].sy = (float)((int)g_ImmediateTLVertices[0].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    g_ImmediateTLVertices[1].sx = (float)((int)g_ImmediateTLVertices[1].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    g_ImmediateTLVertices[1].sy = (float)((int)g_ImmediateTLVertices[1].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    g_ImmediateTLVertices[2].sx = (float)((int)g_ImmediateTLVertices[2].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    g_ImmediateTLVertices[2].sy = (float)((int)g_ImmediateTLVertices[2].sy + -0x6000000);
  }
  uVar2 = packet->textureEntry->widthLog2;
  uVar3 = packet->textureEntry->heightLog2;
  uVar7 = uVar2;
  if (uVar2 < uVar3) {
    uVar7 = uVar3;
  }
  bVar6 = (char)uVar7 - (char)uVar2;
  pGVar1 = &packet->vertices[0].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[1].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[2].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  bVar6 = (char)uVar7 - (char)uVar3;
  pGVar1 = &packet->vertices[0].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[1].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[2].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    g_ImmediateTLVertices[0].tu = (float)((int)g_ImmediateTLVertices[0].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    g_ImmediateTLVertices[0].tv = (float)((int)g_ImmediateTLVertices[0].tv + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    g_ImmediateTLVertices[1].tu = (float)((int)g_ImmediateTLVertices[1].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    g_ImmediateTLVertices[1].tv = (float)((int)g_ImmediateTLVertices[1].tv + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    g_ImmediateTLVertices[2].tu = (float)((int)g_ImmediateTLVertices[2].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    g_ImmediateTLVertices[2].tv = (float)((int)g_ImmediateTLVertices[2].tv + -0xa000000);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    g_ImmediateTLVertices[0].sz = (float)((int)g_ImmediateTLVertices[0].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    g_ImmediateTLVertices[0].rhw = (float)((int)g_ImmediateTLVertices[0].rhw + 0x9000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    g_ImmediateTLVertices[1].sz = (float)((int)g_ImmediateTLVertices[1].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    g_ImmediateTLVertices[1].rhw = (float)((int)g_ImmediateTLVertices[1].rhw + 0x9000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    g_ImmediateTLVertices[2].sz = (float)((int)g_ImmediateTLVertices[2].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    g_ImmediateTLVertices[2].rhw = (float)((int)g_ImmediateTLVertices[2].rhw + 0x9000000);
  }
  pGVar4 = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    pDVar10 = g_ImmediateTLVertices + 1;
    pDVar11 = g_ImmediateTLVertices + 3;
    for (iVar8 = 8; iVar8 != 0; iVar8 = iVar8 + -1) {
      pDVar11->sx = pDVar10->sx;
      pDVar10 = (D3DTLVERTEX_DX6 *)&pDVar10->sy;
      pDVar11 = (D3DTLVERTEX_DX6 *)&pDVar11->sy;
    }
  }
  texture = pGVar4->texture;
  arg2 = 0;
  if (texture != (GraphicsTextureResource *)0x0) {
    arg2 = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial = g_TextureUseSerial + 1;
    if (arg2 == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
      arg2 = texture->textureHandle;
    }
  }
  if (arg2 != g_BoundTextureHandle) {
    pIVar12 = g_Direct3DDevice2;
    sVar5 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,arg2);
    if (sVar5 != 0) {
      pIVar12 = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)pIVar12;
    g_TextureBindStateChangeCount = g_TextureBindStateChangeCount + 1;
  }
  return;
}


/* Address: 0x0057E3B0.
   Ownership: graphics/backend/direct3d.
   Purpose: Direct3D immediate textured primitive handler using render-state preset 4: ONE/ONE additive blending,
   alpha blending enabled, Z-write disabled. Copies vertex diffuse colors, normalizes U/V to the larger texture
   log2 dimension, converts screen/depth values into g_ImmediateTLVertices, duplicates the second vertex into slot
   four when requested, updates texture->lastUsedCounter from g_TextureUseSerial, lazily creates a missing device
   texture, and binds its D3DTEXTUREHANDLE.
   Cross-module calls: GraphicsTexture_CreateDeviceTexture [graphics/resources/texture].
*/
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_TexturedPreset4(GraphicsPrimitivePacket *packet)

{
  GraphicsPrimitiveTextureCoordinateFixed *pGVar1;
  uint uVar2;
  uint uVar3;
  GraphicsTextureSetEntry *pGVar4;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle arg2;
  sdword sVar5;
  byte bVar6;
  uint uVar7;
  int iVar8;
  D3DDEVICEDESC_DX6 *pDVar9;
  D3DTLVERTEX_DX6 *pDVar10;
  D3DTLVERTEX_DX6 *pDVar11;
  IDirect3DDevice2 *pIVar12;
  
  if (g_PrimitiveRenderStatePresets[4].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[4].zWriteEnable;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[4].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[4].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[4].alphaBlendEnable);
      pDVar9 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((pDVar9->dwFlags & 1) == 0) || (pDVar9->dcmColorModel == 2)) {
        pDVar9 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((pDVar9->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                  (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[4].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[4].sourceBlend;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
               g_PrimitiveRenderStatePresets[4].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[4].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[4].destinationBlend
    ;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
              (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
               g_PrimitiveRenderStatePresets[4].destinationBlend);
  }
  g_ImmediateTLVertices[0].color = packet->vertices[0].diffuseColor;
  g_ImmediateTLVertices[1].color = packet->vertices[1].diffuseColor;
  g_ImmediateTLVertices[2].color = packet->vertices[2].diffuseColor;
  g_ImmediateTLVertices[0].specular = 0;
  g_ImmediateTLVertices[1].specular = 0;
  g_ImmediateTLVertices[2].specular = 0;
  g_ImmediateTLVertices[0].sx = (float)packet->vertices[0].screenX;
  g_ImmediateTLVertices[0].sy = (float)packet->vertices[0].screenY;
  g_ImmediateTLVertices[1].sx = (float)packet->vertices[1].screenX;
  g_ImmediateTLVertices[1].sy = (float)packet->vertices[1].screenY;
  g_ImmediateTLVertices[2].sx = (float)packet->vertices[2].screenX;
  g_ImmediateTLVertices[2].sy = (float)packet->vertices[2].screenY;
  if (g_ImmediateTLVertices[0].sx != 0.0) {
    g_ImmediateTLVertices[0].sx = (float)((int)g_ImmediateTLVertices[0].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    g_ImmediateTLVertices[0].sy = (float)((int)g_ImmediateTLVertices[0].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    g_ImmediateTLVertices[1].sx = (float)((int)g_ImmediateTLVertices[1].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    g_ImmediateTLVertices[1].sy = (float)((int)g_ImmediateTLVertices[1].sy + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    g_ImmediateTLVertices[2].sx = (float)((int)g_ImmediateTLVertices[2].sx + -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    g_ImmediateTLVertices[2].sy = (float)((int)g_ImmediateTLVertices[2].sy + -0x6000000);
  }
  uVar2 = packet->textureEntry->widthLog2;
  uVar3 = packet->textureEntry->heightLog2;
  uVar7 = uVar2;
  if (uVar2 < uVar3) {
    uVar7 = uVar3;
  }
  bVar6 = (char)uVar7 - (char)uVar2;
  pGVar1 = &packet->vertices[0].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[1].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[2].textureU;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  bVar6 = (char)uVar7 - (char)uVar3;
  pGVar1 = &packet->vertices[0].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[1].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  pGVar1 = &packet->vertices[2].textureV;
  *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    g_ImmediateTLVertices[0].tu = (float)((int)g_ImmediateTLVertices[0].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    g_ImmediateTLVertices[0].tv = (float)((int)g_ImmediateTLVertices[0].tv + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    g_ImmediateTLVertices[1].tu = (float)((int)g_ImmediateTLVertices[1].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    g_ImmediateTLVertices[1].tv = (float)((int)g_ImmediateTLVertices[1].tv + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    g_ImmediateTLVertices[2].tu = (float)((int)g_ImmediateTLVertices[2].tu + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    g_ImmediateTLVertices[2].tv = (float)((int)g_ImmediateTLVertices[2].tv + -0xa000000);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    g_ImmediateTLVertices[0].sz = (float)((int)g_ImmediateTLVertices[0].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    g_ImmediateTLVertices[0].rhw = (float)((int)g_ImmediateTLVertices[0].rhw + 0x9000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    g_ImmediateTLVertices[1].sz = (float)((int)g_ImmediateTLVertices[1].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    g_ImmediateTLVertices[1].rhw = (float)((int)g_ImmediateTLVertices[1].rhw + 0x9000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    g_ImmediateTLVertices[2].sz = (float)((int)g_ImmediateTLVertices[2].sz + -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    g_ImmediateTLVertices[2].rhw = (float)((int)g_ImmediateTLVertices[2].rhw + 0x9000000);
  }
  pGVar4 = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    pDVar10 = g_ImmediateTLVertices + 1;
    pDVar11 = g_ImmediateTLVertices + 3;
    for (iVar8 = 8; iVar8 != 0; iVar8 = iVar8 + -1) {
      pDVar11->sx = pDVar10->sx;
      pDVar10 = (D3DTLVERTEX_DX6 *)&pDVar10->sy;
      pDVar11 = (D3DTLVERTEX_DX6 *)&pDVar11->sy;
    }
  }
  texture = pGVar4->texture;
  arg2 = 0;
  if (texture != (GraphicsTextureResource *)0x0) {
    arg2 = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial = g_TextureUseSerial + 1;
    if (arg2 == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
      arg2 = texture->textureHandle;
    }
  }
  if (arg2 != g_BoundTextureHandle) {
    pIVar12 = g_Direct3DDevice2;
    sVar5 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,arg2);
    if (sVar5 != 0) {
      pIVar12 = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)pIVar12;
    g_TextureBindStateChangeCount = g_TextureBindStateChangeCount + 1;
  }
  return;
}

