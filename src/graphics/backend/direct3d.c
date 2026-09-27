/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/direct3d.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/direct3d.h>
#include <thandor/thandor.h>

/* Implementation ownership: graphics/backend/direct3d. */

/* MOVD mm,color; PUNPCKLBW mm,mm; PSRLW mm,4: each color byte b (B,G,R,A from low to high)
   becomes the 16-bit lane (b << 8 | b) >> 4. */
static __inline ulonglong Direct3D_MmxUnpackColorWords(PackedArgb32 color)
{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane = lane + 1) {
    lanes.uw[lane] = (word)((((color >> (lane * 8)) & 0xff) * 0x101) >> 4);
  }
  return lanes.q;
}

/* PACKUSWB mm,mm; MOVD color,mm: each signed 16-bit lane saturated to 0..0xff and packed back into
   the B,G,R,A bytes of a D3D color. */
static __inline PackedArgb32 Direct3D_MmxPackColorWords(ulonglong words)
{
  ThandorMmx lanes;
  PackedArgb32 color;
  int lane;
  short value;

  lanes.q = words;
  color = 0;
  for (lane = 0; lane < 4; lane = lane + 1) {
    value = lanes.sw[lane];
    color = color | ((PackedArgb32)(value < 0 ? 0 : (value > 0xff ? 0xff : value)) << (lane * 8));
  }
  return color;
}

/* Address: 0x00578270.
   Ownership: graphics/backend/direct3d.
   Purpose: IDirect3D2::EnumDevices callback. Context points at the adapter record being expanded.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
sdword __stdcall Direct3D_EnumDeviceCallback
                 (TH_LEGACY_GUID *deviceGuid,char *description,char *deviceName,
                 D3DDEVICEDESC_DX6 *hardwareDesc,D3DDEVICEDESC_DX6 *softwareDesc,
                 GraphicsAdapterRecord *adapterContext)

{
  GraphicsAdapterRecord *filledRecord;
  dword newAdapterIndex;
  dword updatedAdapterCount;
  int remainingDwords;
  GraphicsAdapterRecord *newRecord;
  GraphicsAdapterRecord *recordCursor;
  TH_LEGACY_GUID *guidCursor;
  D3DDEVICEDESC_DX6 *descCursor;
  ArenaAllocEaxCf5 descAllocation;
  
  newAdapterIndex = g_GraphicsAdapterCount;
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
    descAllocation = (*g_MemoryApi.alloc)(0x198);
    updatedAdapterCount = g_GraphicsAdapterCount;
    if (!descAllocation.carry) {
      newRecord = g_GraphicsAdapters + newAdapterIndex;
      remainingDwords = 0x20;
      g_GraphicsAdapterCount = g_GraphicsAdapterCount + 1;
      recordCursor = newRecord;
      if ((g_GraphicsEnumerateAllDevicesFlag == 0) ||
         (filledRecord = adapterContext, (adapterContext->deviceGuid).Data1 != 0)) {
        for (; filledRecord = newRecord, updatedAdapterCount = g_GraphicsAdapterCount, remainingDwords != 0; remainingDwords = remainingDwords + -1) {
          (recordCursor->adapterGuid).Data1 = (adapterContext->adapterGuid).Data1;
          adapterContext = (GraphicsAdapterRecord *)&(adapterContext->adapterGuid).Data2;
          recordCursor = (GraphicsAdapterRecord *)&(recordCursor->adapterGuid).Data2;
        }
      }
      g_GraphicsAdapterCount = updatedAdapterCount;
      guidCursor = &filledRecord->deviceGuid;
      for (remainingDwords = 4; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
        guidCursor->Data1 = deviceGuid->Data1;
        deviceGuid = (TH_LEGACY_GUID *)&deviceGuid->Data2;
        guidCursor = (TH_LEGACY_GUID *)&guidCursor->Data2;
      }
      Text_CopyNarrowToUtf16Cf(0x28,filledRecord->deviceNameUtf16,(byte *)deviceName);
      filledRecord->hardwareDesc = (D3DDEVICEDESC_DX6 *)descAllocation.eax;
      descCursor = (D3DDEVICEDESC_DX6 *)descAllocation.eax;
      for (remainingDwords = 0x33; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
        descCursor->dwSize = hardwareDesc->dwSize;
        hardwareDesc = (D3DDEVICEDESC_DX6 *)&hardwareDesc->dwFlags;
        descCursor = (D3DDEVICEDESC_DX6 *)&descCursor->dwFlags;
      }
      filledRecord->softwareDesc = descCursor;
      for (remainingDwords = 0x33; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
        descCursor->dwSize = softwareDesc->dwSize;
        softwareDesc = (D3DDEVICEDESC_DX6 *)&softwareDesc->dwFlags;
        descCursor = (D3DDEVICEDESC_DX6 *)&descCursor->dwFlags;
      }
    }
  }
  return 1;
}


/* Address: 0x00578820.
   Ownership: graphics/backend/direct3d.
   Purpose: Semantic ABI remains deferred.
*/
/* Called by IDirect3DDevice2::EnumTextureFormats: __stdcall (the original returns with RET 8). */
sdword __stdcall
GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback
          (DDSURFACEDESC_DX6 *surfaceDesc,TH_LEGACY_LPVOID context)

{
  uint pixelFormatFlags;
  int currentAlphaLowBit;
  int candidateAlphaLowBit;
  int candidateAlphaHighBit;
  uint bitCountOrMaskDelta;
  int highBitOrCopyCount;
  uint candidateColorMask;
  int replaceOpaqueFormat;
  DDPIXELFORMAT *pixelFormatCursor;
  TH_LEGACY_DWORD *formatDwordCursor;
  
  bitCountOrMaskDelta = (surfaceDesc->ddpfPixelFormat).dwRGBBitCount;
  pixelFormatFlags = (surfaceDesc->ddpfPixelFormat).dwFlags;
  if (bitCountOrMaskDelta < 8) {
    return 1;
  }
  if (bitCountOrMaskDelta == 8) {
    return 1;
  }
  if ((bitCountOrMaskDelta != 0x10) && (bitCountOrMaskDelta != 0x20)) {
    return 1;
  }
  if ((pixelFormatFlags & 0x40) == 0) {
    return 1;
  }
  /* Take the candidate as the opaque format when none is chosen yet, when it has fewer bits per pixel, or
     when it has the same depth and color bits the current format lacks. */
  replaceOpaqueFormat = 1;
  if (g_Direct3DOpaqueTextureFormatBitsPerPixel != 0) {
    if (g_Direct3DOpaqueTextureFormatBitsPerPixel < bitCountOrMaskDelta) {
      replaceOpaqueFormat = 0;
    }
    else if (g_Direct3DOpaqueTextureFormatBitsPerPixel == bitCountOrMaskDelta) {
      candidateColorMask = (surfaceDesc->ddpfPixelFormat).dwRBitMask | (surfaceDesc->ddpfPixelFormat).dwGBitMask
              | (surfaceDesc->ddpfPixelFormat).dwBBitMask;
      bitCountOrMaskDelta = (_g_Direct3DOpaqueTextureFormatRedBitMask | _g_Direct3DOpaqueTextureFormatGreenBitMask
              | _g_Direct3DOpaqueTextureFormatBlueBitMask) ^ candidateColorMask;
      if ((bitCountOrMaskDelta == 0) || ((bitCountOrMaskDelta & candidateColorMask) == 0)) {
        replaceOpaqueFormat = 0;
      }
    }
  }
  if (replaceOpaqueFormat) {
    pixelFormatCursor = &surfaceDesc->ddpfPixelFormat;
    formatDwordCursor = (TH_LEGACY_DWORD *)THANDOR_ADDR(g_Direct3DOpaqueTextureFormat,0);
    for (highBitOrCopyCount = 8; highBitOrCopyCount != 0; highBitOrCopyCount = highBitOrCopyCount + -1) {
      *formatDwordCursor = pixelFormatCursor->dwSize;
      pixelFormatCursor = (DDPIXELFORMAT *)&pixelFormatCursor->dwFlags;
      formatDwordCursor = formatDwordCursor + 1;
    }
  }
  if (((pixelFormatFlags & 1) != 0) && (8 < (surfaceDesc->ddpfPixelFormat).dwRGBBitCount)) {
    highBitOrCopyCount = 0x1f;
    if (_g_Direct3DAlphaTextureFormatAlphaBitMask != 0) {
      for (; _g_Direct3DAlphaTextureFormatAlphaBitMask >> highBitOrCopyCount == 0; highBitOrCopyCount = highBitOrCopyCount + -1) {
      }
    }
    currentAlphaLowBit = 0;
    if (_g_Direct3DAlphaTextureFormatAlphaBitMask != 0) {
      for (; (_g_Direct3DAlphaTextureFormatAlphaBitMask >> currentAlphaLowBit & 1) == 0; currentAlphaLowBit = currentAlphaLowBit + 1) {
      }
    }
    formatDwordCursor = &(surfaceDesc->ddpfPixelFormat).dwRGBAlphaBitMask;
    candidateAlphaHighBit = 0x1f;
    if (*formatDwordCursor != 0) {
      for (; *formatDwordCursor >> candidateAlphaHighBit == 0; candidateAlphaHighBit = candidateAlphaHighBit + -1) {
      }
    }
    formatDwordCursor = &(surfaceDesc->ddpfPixelFormat).dwRGBAlphaBitMask;
    candidateAlphaLowBit = 0;
    if (*formatDwordCursor != 0) {
      for (; (*formatDwordCursor >> candidateAlphaLowBit & 1) == 0; candidateAlphaLowBit = candidateAlphaLowBit + 1) {
      }
    }
    if ((uint)(currentAlphaLowBit - highBitOrCopyCount) < (uint)(candidateAlphaLowBit - candidateAlphaHighBit)) {
      pixelFormatCursor = &surfaceDesc->ddpfPixelFormat;
      formatDwordCursor = (TH_LEGACY_DWORD *)THANDOR_ADDR(g_Direct3DAlphaTextureFormat,0);
      for (highBitOrCopyCount = 8; highBitOrCopyCount != 0; highBitOrCopyCount = highBitOrCopyCount + -1) {
        *formatDwordCursor = pixelFormatCursor->dwSize;
        pixelFormatCursor = (DDPIXELFORMAT *)&pixelFormatCursor->dwFlags;
        formatDwordCursor = formatDwordCursor + 1;
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
  sdword direct3DResult;
  Direct3DRenderStateApplyEaxCf5 successResult;
  Direct3DRenderStateApplyEaxCf5 failureResult;
  
  direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                    (g_Direct3DDevice2,D3DRENDERSTATE_ANTIALIAS,antialiasMode);
  if (direct3DResult == 0) {
    g_Direct3DAntialiasMode = antialiasMode;
    successResult.carry = false;
    successResult.appliedValueOrError = antialiasMode;
    return successResult;
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,100,g_PackageLastErrorPath);
  failureResult.carry = true;
  failureResult.appliedValueOrError = 0x1d;
  return failureResult;
}


/* Address: 0x0057A4C0.
   Ownership: graphics/backend/direct3d.
   Purpose: Handles direct3 drenderer set texture filter mode.
*/
Direct3DRenderStateApplyEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Direct3DRenderer_SetTextureFilterMode(dword textureFilterMode)

{
  sdword direct3DResult;
  Direct3DRenderStateApplyEaxCf5 successResult;
  Direct3DRenderStateApplyEaxCf5 failureResult;
  sdword errorCode;
  
  errorCode = 0x6e;
  direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                    (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMAG,textureFilterMode);
  if (direct3DResult == 0) {
    errorCode = 0x6f;
    direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMIN,textureFilterMode);
    if (direct3DResult == 0) {
      g_Direct3DTextureFilterMode = textureFilterMode;
      successResult.carry = false;
      successResult.appliedValueOrError = textureFilterMode;
      return successResult;
    }
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,errorCode,g_PackageLastErrorPath);
  failureResult.carry = true;
  failureResult.appliedValueOrError = 0x1d;
  return failureResult;
}


/* Address: 0x0057A550.
   Ownership: graphics/backend/direct3d.
   Purpose: Handles direct3 drenderer set texture perspective enabled.
*/
Direct3DRenderStateApplyEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Direct3DRenderer_SetTexturePerspectiveEnabled(dword texturePerspectiveEnabled)

{
  sdword direct3DResult;
  Direct3DRenderStateApplyEaxCf5 successResult;
  Direct3DRenderStateApplyEaxCf5 failureResult;
  
  direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                    (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREPERSPECTIVE,texturePerspectiveEnabled);
  if (direct3DResult == 0) {
    g_Direct3DTexturePerspectiveEnabled = texturePerspectiveEnabled;
    successResult.carry = false;
    successResult.appliedValueOrError = texturePerspectiveEnabled;
    return successResult;
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x78,g_PackageLastErrorPath);
  failureResult.carry = true;
  failureResult.appliedValueOrError = 0x1d;
  return failureResult;
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
  long direct3DResult; /* Ghidra: _sVar19, HRESULT folded into the x87/MMX register image */
  PackedArgb32 packetModulationColor;
  PackedArgb32 vertex0Diffuse;
  PackedArgb32 vertex1Diffuse;
  PackedArgb32 vertex2Diffuse;
  ulonglong modulationWords;
  sdword bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  sdword unusedResult;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[0].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[0].zWriteEnable;
    direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[0].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[0].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[0].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                            (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  packetModulationColor = packet->modulationColor;
  vertex0Diffuse = packet->vertices[0].diffuseColor;
  vertex1Diffuse = packet->vertices[1].diffuseColor;
  vertex2Diffuse = packet->vertices[2].diffuseColor;
  /* MM3 = modulation color, MM0..MM2 = vertex colors; PMULHW each vertex by MM3, PACKUSWB, MOVD. */
  modulationWords = Direct3D_MmxUnpackColorWords(packetModulationColor);
  g_ImmediateTLVertices[0].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex0Diffuse),modulationWords));
  g_ImmediateTLVertices[1].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex1Diffuse),modulationWords));
  g_ImmediateTLVertices[2].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex2Diffuse),modulationWords));
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -0x6000000);
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 0xa000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 0xa000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 0xa000000);
  }
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = g_ImmediateTLVertices + 1;
    destVertexCursor = g_ImmediateTLVertices + 3;
    for (remainingDwords = 8; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destVertexCursor->sx = sourceVertexCursor->sx;
      sourceVertexCursor = (D3DTLVERTEX_DX6 *)&sourceVertexCursor->sy;
      destVertexCursor = (D3DTLVERTEX_DX6 *)&destVertexCursor->sy;
    }
  }
  if (g_BoundTextureHandle != 0) {
    newBoundTextureHandle = g_Direct3DDevice2;
    bindResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)newBoundTextureHandle;
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
  long direct3DResult; /* Ghidra: _sVar19, HRESULT folded into the x87/MMX register image */
  PackedArgb32 packetModulationColor;
  PackedArgb32 vertex0Diffuse;
  PackedArgb32 vertex1Diffuse;
  PackedArgb32 vertex2Diffuse;
  ulonglong modulationWords;
  sdword bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  sdword unusedResult;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[2].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[2].zWriteEnable;
    direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[2].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[2].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[2].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                            (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[2].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[2].sourceBlend;
    direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
                         g_PrimitiveRenderStatePresets[2].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[2].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[2].destinationBlend
    ;
    direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
                         g_PrimitiveRenderStatePresets[2].destinationBlend);
  }
  packetModulationColor = packet->modulationColor;
  vertex0Diffuse = packet->vertices[0].diffuseColor;
  vertex1Diffuse = packet->vertices[1].diffuseColor;
  vertex2Diffuse = packet->vertices[2].diffuseColor;
  /* MM3 = modulation color, MM0..MM2 = vertex colors; PMULHW each vertex by MM3, PACKUSWB, MOVD. */
  modulationWords = Direct3D_MmxUnpackColorWords(packetModulationColor);
  g_ImmediateTLVertices[0].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex0Diffuse),modulationWords));
  g_ImmediateTLVertices[1].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex1Diffuse),modulationWords));
  g_ImmediateTLVertices[2].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex2Diffuse),modulationWords));
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -0x6000000);
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 0xa000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 0xa000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 0xa000000);
  }
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = g_ImmediateTLVertices + 1;
    destVertexCursor = g_ImmediateTLVertices + 3;
    for (remainingDwords = 8; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destVertexCursor->sx = sourceVertexCursor->sx;
      sourceVertexCursor = (D3DTLVERTEX_DX6 *)&sourceVertexCursor->sy;
      destVertexCursor = (D3DTLVERTEX_DX6 *)&destVertexCursor->sy;
    }
  }
  if (g_BoundTextureHandle != 0) {
    newBoundTextureHandle = g_Direct3DDevice2;
    bindResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)newBoundTextureHandle;
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
  long direct3DResult; /* Ghidra: _sVar19, HRESULT folded into the x87/MMX register image */
  PackedArgb32 packetModulationColor;
  PackedArgb32 vertex0Diffuse;
  PackedArgb32 vertex1Diffuse;
  PackedArgb32 vertex2Diffuse;
  ulonglong modulationWords;
  sdword bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  sdword unusedResult;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[3].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[3].zWriteEnable;
    direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[3].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[3].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[3].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                            (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[3].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[3].sourceBlend;
    direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
                         g_PrimitiveRenderStatePresets[3].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[3].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[3].destinationBlend
    ;
    direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
                         g_PrimitiveRenderStatePresets[3].destinationBlend);
  }
  packetModulationColor = packet->modulationColor;
  vertex0Diffuse = packet->vertices[0].diffuseColor;
  vertex1Diffuse = packet->vertices[1].diffuseColor;
  vertex2Diffuse = packet->vertices[2].diffuseColor;
  /* MM3 = modulation color, MM0..MM2 = vertex colors; PMULHW each vertex by MM3, PACKUSWB, MOVD. */
  modulationWords = Direct3D_MmxUnpackColorWords(packetModulationColor);
  g_ImmediateTLVertices[0].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex0Diffuse),modulationWords));
  g_ImmediateTLVertices[1].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex1Diffuse),modulationWords));
  g_ImmediateTLVertices[2].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex2Diffuse),modulationWords));
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -0x6000000);
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 0xa000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 0xa000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 0xa000000);
  }
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = g_ImmediateTLVertices + 1;
    destVertexCursor = g_ImmediateTLVertices + 3;
    for (remainingDwords = 8; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destVertexCursor->sx = sourceVertexCursor->sx;
      sourceVertexCursor = (D3DTLVERTEX_DX6 *)&sourceVertexCursor->sy;
      destVertexCursor = (D3DTLVERTEX_DX6 *)&destVertexCursor->sy;
    }
  }
  if (g_BoundTextureHandle != 0) {
    newBoundTextureHandle = g_Direct3DDevice2;
    bindResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)newBoundTextureHandle;
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
  long direct3DResult; /* Ghidra: _sVar19, HRESULT folded into the x87/MMX register image */
  PackedArgb32 packetModulationColor;
  PackedArgb32 vertex0Diffuse;
  PackedArgb32 vertex1Diffuse;
  PackedArgb32 vertex2Diffuse;
  ulonglong modulationWords;
  sdword bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  sdword unusedResult;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[4].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[4].zWriteEnable;
    direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[4].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[4].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[4].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                            (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[4].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[4].sourceBlend;
    direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
                         g_PrimitiveRenderStatePresets[4].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[4].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[4].destinationBlend
    ;
    direct3DResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                        (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
                         g_PrimitiveRenderStatePresets[4].destinationBlend);
  }
  packetModulationColor = packet->modulationColor;
  vertex0Diffuse = packet->vertices[0].diffuseColor;
  vertex1Diffuse = packet->vertices[1].diffuseColor;
  vertex2Diffuse = packet->vertices[2].diffuseColor;
  /* MM3 = modulation color, MM0..MM2 = vertex colors; PMULHW each vertex by MM3, PACKUSWB, MOVD. */
  modulationWords = Direct3D_MmxUnpackColorWords(packetModulationColor);
  g_ImmediateTLVertices[0].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex0Diffuse),modulationWords));
  g_ImmediateTLVertices[1].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex1Diffuse),modulationWords));
  g_ImmediateTLVertices[2].color =
       Direct3D_MmxPackColorWords(pmulhw(Direct3D_MmxUnpackColorWords(vertex2Diffuse),modulationWords));
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -0x6000000);
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 0xa000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 0xa000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 0xa000000);
  }
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = g_ImmediateTLVertices + 1;
    destVertexCursor = g_ImmediateTLVertices + 3;
    for (remainingDwords = 8; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destVertexCursor->sx = sourceVertexCursor->sx;
      sourceVertexCursor = (D3DTLVERTEX_DX6 *)&sourceVertexCursor->sy;
      destVertexCursor = (D3DTLVERTEX_DX6 *)&destVertexCursor->sy;
    }
  }
  if (g_BoundTextureHandle != 0) {
    newBoundTextureHandle = g_Direct3DDevice2;
    bindResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)newBoundTextureHandle;
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
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinate;
  uint textureWidthLog2;
  uint textureHeightLog2;
  GraphicsTextureSetEntry *packetTextureEntry;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle deviceTextureHandle;
  sdword bindResult;
  byte coordinateShift;
  uint largerDimensionLog2;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  IDirect3DDevice2 *newBoundTextureHandle;
  
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
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -0x6000000);
  }
  textureWidthLog2 = packet->textureEntry->widthLog2;
  textureHeightLog2 = packet->textureEntry->heightLog2;
  largerDimensionLog2 = textureWidthLog2;
  if (textureWidthLog2 < textureHeightLog2) {
    largerDimensionLog2 = textureHeightLog2;
  }
  coordinateShift = (char)largerDimensionLog2 - (char)textureWidthLog2;
  textureCoordinate = &packet->vertices[0].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[1].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[2].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  coordinateShift = (char)largerDimensionLog2 - (char)textureHeightLog2;
  textureCoordinate = &packet->vertices[0].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[1].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[2].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tv, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tv, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tv, -0xa000000);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 0x9000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 0x9000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 0x9000000);
  }
  packetTextureEntry = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = g_ImmediateTLVertices + 1;
    destVertexCursor = g_ImmediateTLVertices + 3;
    for (remainingDwords = 8; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destVertexCursor->sx = sourceVertexCursor->sx;
      sourceVertexCursor = (D3DTLVERTEX_DX6 *)&sourceVertexCursor->sy;
      destVertexCursor = (D3DTLVERTEX_DX6 *)&destVertexCursor->sy;
    }
  }
  texture = packetTextureEntry->texture;
  deviceTextureHandle = 0;
  if (texture != (GraphicsTextureResource *)0x0) {
    deviceTextureHandle = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial = g_TextureUseSerial + 1;
    if (deviceTextureHandle == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
      deviceTextureHandle = texture->textureHandle;
    }
  }
  if (deviceTextureHandle != g_BoundTextureHandle) {
    newBoundTextureHandle = g_Direct3DDevice2;
    bindResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)newBoundTextureHandle;
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
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinate;
  uint textureWidthLog2;
  uint textureHeightLog2;
  GraphicsTextureSetEntry *packetTextureEntry;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle deviceTextureHandle;
  sdword bindResult;
  byte coordinateShift;
  uint largerDimensionLog2;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  IDirect3DDevice2 *newBoundTextureHandle;
  
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
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -0x6000000);
  }
  textureWidthLog2 = packet->textureEntry->widthLog2;
  textureHeightLog2 = packet->textureEntry->heightLog2;
  largerDimensionLog2 = textureWidthLog2;
  if (textureWidthLog2 < textureHeightLog2) {
    largerDimensionLog2 = textureHeightLog2;
  }
  coordinateShift = (char)largerDimensionLog2 - (char)textureWidthLog2;
  textureCoordinate = &packet->vertices[0].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[1].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[2].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  coordinateShift = (char)largerDimensionLog2 - (char)textureHeightLog2;
  textureCoordinate = &packet->vertices[0].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[1].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[2].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tv, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tv, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tv, -0xa000000);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 0x9000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 0x9000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 0x9000000);
  }
  packetTextureEntry = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = g_ImmediateTLVertices + 1;
    destVertexCursor = g_ImmediateTLVertices + 3;
    for (remainingDwords = 8; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destVertexCursor->sx = sourceVertexCursor->sx;
      sourceVertexCursor = (D3DTLVERTEX_DX6 *)&sourceVertexCursor->sy;
      destVertexCursor = (D3DTLVERTEX_DX6 *)&destVertexCursor->sy;
    }
  }
  texture = packetTextureEntry->texture;
  deviceTextureHandle = 0;
  if (texture != (GraphicsTextureResource *)0x0) {
    deviceTextureHandle = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial = g_TextureUseSerial + 1;
    if (deviceTextureHandle == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
      deviceTextureHandle = texture->textureHandle;
    }
  }
  if (deviceTextureHandle != g_BoundTextureHandle) {
    newBoundTextureHandle = g_Direct3DDevice2;
    bindResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)newBoundTextureHandle;
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
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinate;
  uint textureWidthLog2;
  uint textureHeightLog2;
  GraphicsTextureSetEntry *packetTextureEntry;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle deviceTextureHandle;
  sdword bindResult;
  byte coordinateShift;
  uint largerDimensionLog2;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  IDirect3DDevice2 *newBoundTextureHandle;
  
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
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -0x6000000);
  }
  textureWidthLog2 = packet->textureEntry->widthLog2;
  textureHeightLog2 = packet->textureEntry->heightLog2;
  largerDimensionLog2 = textureWidthLog2;
  if (textureWidthLog2 < textureHeightLog2) {
    largerDimensionLog2 = textureHeightLog2;
  }
  coordinateShift = (char)largerDimensionLog2 - (char)textureWidthLog2;
  textureCoordinate = &packet->vertices[0].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[1].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[2].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  coordinateShift = (char)largerDimensionLog2 - (char)textureHeightLog2;
  textureCoordinate = &packet->vertices[0].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[1].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[2].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tv, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tv, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tv, -0xa000000);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 0x9000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 0x9000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 0x9000000);
  }
  packetTextureEntry = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = g_ImmediateTLVertices + 1;
    destVertexCursor = g_ImmediateTLVertices + 3;
    for (remainingDwords = 8; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destVertexCursor->sx = sourceVertexCursor->sx;
      sourceVertexCursor = (D3DTLVERTEX_DX6 *)&sourceVertexCursor->sy;
      destVertexCursor = (D3DTLVERTEX_DX6 *)&destVertexCursor->sy;
    }
  }
  texture = packetTextureEntry->texture;
  deviceTextureHandle = 0;
  if (texture != (GraphicsTextureResource *)0x0) {
    deviceTextureHandle = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial = g_TextureUseSerial + 1;
    if (deviceTextureHandle == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
      deviceTextureHandle = texture->textureHandle;
    }
  }
  if (deviceTextureHandle != g_BoundTextureHandle) {
    newBoundTextureHandle = g_Direct3DDevice2;
    bindResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)newBoundTextureHandle;
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
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinate;
  uint textureWidthLog2;
  uint textureHeightLog2;
  GraphicsTextureSetEntry *packetTextureEntry;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle deviceTextureHandle;
  sdword bindResult;
  byte coordinateShift;
  uint largerDimensionLog2;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  IDirect3DDevice2 *newBoundTextureHandle;
  
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
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -0x6000000);
  }
  textureWidthLog2 = packet->textureEntry->widthLog2;
  textureHeightLog2 = packet->textureEntry->heightLog2;
  largerDimensionLog2 = textureWidthLog2;
  if (textureWidthLog2 < textureHeightLog2) {
    largerDimensionLog2 = textureHeightLog2;
  }
  coordinateShift = (char)largerDimensionLog2 - (char)textureWidthLog2;
  textureCoordinate = &packet->vertices[0].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[1].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[2].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  coordinateShift = (char)largerDimensionLog2 - (char)textureHeightLog2;
  textureCoordinate = &packet->vertices[0].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[1].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[2].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tv, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tv, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tv, -0xa000000);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 0x9000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 0x9000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 0x9000000);
  }
  packetTextureEntry = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = g_ImmediateTLVertices + 1;
    destVertexCursor = g_ImmediateTLVertices + 3;
    for (remainingDwords = 8; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destVertexCursor->sx = sourceVertexCursor->sx;
      sourceVertexCursor = (D3DTLVERTEX_DX6 *)&sourceVertexCursor->sy;
      destVertexCursor = (D3DTLVERTEX_DX6 *)&destVertexCursor->sy;
    }
  }
  texture = packetTextureEntry->texture;
  deviceTextureHandle = 0;
  if (texture != (GraphicsTextureResource *)0x0) {
    deviceTextureHandle = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial = g_TextureUseSerial + 1;
    if (deviceTextureHandle == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
      deviceTextureHandle = texture->textureHandle;
    }
  }
  if (deviceTextureHandle != g_BoundTextureHandle) {
    newBoundTextureHandle = g_Direct3DDevice2;
    bindResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)newBoundTextureHandle;
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
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinate;
  uint textureWidthLog2;
  uint textureHeightLog2;
  GraphicsTextureSetEntry *packetTextureEntry;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle deviceTextureHandle;
  sdword bindResult;
  byte coordinateShift;
  uint largerDimensionLog2;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  IDirect3DDevice2 *newBoundTextureHandle;
  
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
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -0x6000000);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -0x6000000);
  }
  textureWidthLog2 = packet->textureEntry->widthLog2;
  textureHeightLog2 = packet->textureEntry->heightLog2;
  largerDimensionLog2 = textureWidthLog2;
  if (textureWidthLog2 < textureHeightLog2) {
    largerDimensionLog2 = textureHeightLog2;
  }
  coordinateShift = (char)largerDimensionLog2 - (char)textureWidthLog2;
  textureCoordinate = &packet->vertices[0].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[1].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[2].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  coordinateShift = (char)largerDimensionLog2 - (char)textureHeightLog2;
  textureCoordinate = &packet->vertices[0].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[1].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  textureCoordinate = &packet->vertices[2].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tv, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tv, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tu, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tv, -0xa000000);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 0x9000000);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 0x9000000);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -0xa000000);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 0x9000000);
  }
  packetTextureEntry = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = g_ImmediateTLVertices + 1;
    destVertexCursor = g_ImmediateTLVertices + 3;
    for (remainingDwords = 8; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destVertexCursor->sx = sourceVertexCursor->sx;
      sourceVertexCursor = (D3DTLVERTEX_DX6 *)&sourceVertexCursor->sy;
      destVertexCursor = (D3DTLVERTEX_DX6 *)&destVertexCursor->sy;
    }
  }
  texture = packetTextureEntry->texture;
  deviceTextureHandle = 0;
  if (texture != (GraphicsTextureResource *)0x0) {
    deviceTextureHandle = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial = g_TextureUseSerial + 1;
    if (deviceTextureHandle == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
      deviceTextureHandle = texture->textureHandle;
    }
  }
  if (deviceTextureHandle != g_BoundTextureHandle) {
    newBoundTextureHandle = g_Direct3DDevice2;
    bindResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (dword)newBoundTextureHandle;
    g_TextureBindStateChangeCount = g_TextureBindStateChangeCount + 1;
  }
  return;
}

