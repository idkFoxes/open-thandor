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
static __inline uint64_t Direct3D_MmxUnpackColorWords(PackedArgb32 color)
{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane = lane + 1) {
    lanes.uw[lane] = (uint16_t)((((color >> (lane * 8)) & 0xff) * 0x101) >> 4);
  }
  return lanes.q;
}

/* PACKUSWB mm,mm; MOVD color,mm: each signed 16-bit lane saturated to 0..0xff and packed back into
   the B,G,R,A bytes of a D3D color. */
static __inline PackedArgb32 Direct3D_MmxPackColorWords(uint64_t words)
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
   IDirect3D2::EnumDevices callback for the DirectDraw adapter in adapterContext: adds one adapter record per
   Direct3D device (copy of the adapter record plus device GUID, name and both device descriptions). Without
   -D3DALL (g_GraphicsEnumerateAllDevicesFlag nonzero) only hardware devices with the caps the renderer needs
   are taken, and the first one fills the adapter's own record instead of a new one. Always continues (returns 1).
*/
int32_t __stdcall Direct3D_EnumDeviceCallback
                 (TH_LEGACY_GUID *deviceGuid,char *description,char *deviceName,
                 D3DDEVICEDESC_DX6 *hardwareDesc,D3DDEVICEDESC_DX6 *softwareDesc,
                 GraphicsAdapterRecord *adapterContext)

{
  GraphicsAdapterRecord *filledRecord;
  uint32_t newAdapterIndex;
  uint32_t updatedAdapterCount;
  int remainingDwords;
  GraphicsAdapterRecord *newRecord;
  GraphicsAdapterRecord *recordCursor;
  TH_LEGACY_GUID *guidCursor;
  D3DDEVICEDESC_DX6 *descCursor;
  ArenaAllocResult descAllocation;
  
  newAdapterIndex = g_GraphicsAdapterCount;
  /* The original also tests D3DDEVCAPS_DRAWPRIMTLVERTEX (0x400) but never branches on the result. */
  if ((g_GraphicsAdapterCount < GRAPHICS_ADAPTER_CAPACITY) &&
     ((g_GraphicsEnumerateAllDevicesFlag == 0 ||
      (((((((hardwareDesc->dwFlags & D3DDD_COLORMODEL) != 0 &&
            ((hardwareDesc->dwFlags & D3DDD_DEVICEZBUFFERBITDEPTH) != 0)) &&
          ((hardwareDesc->dwDeviceZBufferBitDepth & DDBD_16) != 0)) &&
         (((hardwareDesc->dwFlags & D3DDD_DEVCAPS) != 0 &&
           ((hardwareDesc->dwDevCaps & D3DDEVCAPS_TLVERTEXSYSTEMMEMORY) != 0)))) &&
        ((((hardwareDesc->dwDevCaps & D3DDEVCAPS_TEXTUREVIDEOMEMORY) != 0 &&
          (((hardwareDesc->dwFlags & D3DDD_TRICAPS) != 0 &&
           (((hardwareDesc->dpcTriCaps).dwZCmpCaps & D3DPCMPCAPS_LESSEQUAL) != 0)))) &&
         (((hardwareDesc->dpcTriCaps).dwTextureAddressCaps & D3DPTADDRESSCAPS_WRAP) != 0)))) &&
       (((((((hardwareDesc->dpcTriCaps).dwTextureBlendCaps & D3DPTBLENDCAPS_MODULATE) != 0 &&
           (((hardwareDesc->dpcTriCaps).dwSrcBlendCaps & D3DPBLENDCAPS_ONE) != 0)) &&
          (((hardwareDesc->dpcTriCaps).dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA) != 0)) &&
         (((((hardwareDesc->dpcTriCaps).dwDestBlendCaps & D3DPBLENDCAPS_ZERO) != 0 &&
           (((hardwareDesc->dpcTriCaps).dwDestBlendCaps & D3DPBLENDCAPS_ONE) != 0)) &&
          ((((hardwareDesc->dpcTriCaps).dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA) != 0 &&
           (((hardwareDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB) != 0)))))) &&
        (((((hardwareDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) != 0 ||
          (((hardwareDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED) != 0)) ||
         (((hardwareDesc->dpcTriCaps).dwRasterCaps & D3DPRASTERCAPS_STIPPLE) != 0)))))))))) {
    /* one block for the copies of the hardware and the software device description */
    descAllocation = g_MemoryApi.alloc(2 * sizeof(D3DDEVICEDESC_DX6));
    updatedAdapterCount = g_GraphicsAdapterCount;
    if (!descAllocation.failed) {
      newRecord = g_GraphicsAdapters + newAdapterIndex;
      remainingDwords = sizeof(GraphicsAdapterRecord) / sizeof(uint32_t);
      g_GraphicsAdapterCount++;
      recordCursor = newRecord;
      /* in the filtered mode the first device of an adapter reuses the adapter's record (count bumped back) */
      if ((g_GraphicsEnumerateAllDevicesFlag == 0) ||
         (filledRecord = adapterContext, (adapterContext->deviceGuid).Data1 != 0)) {
        for (; filledRecord = newRecord, updatedAdapterCount = g_GraphicsAdapterCount, remainingDwords != 0;
             remainingDwords--) {
          (recordCursor->adapterGuid).Data1 = (adapterContext->adapterGuid).Data1;
          adapterContext = (GraphicsAdapterRecord *)&(adapterContext->adapterGuid).Data2;
          recordCursor = (GraphicsAdapterRecord *)&(recordCursor->adapterGuid).Data2;
        }
      }
      g_GraphicsAdapterCount = updatedAdapterCount;
      guidCursor = &filledRecord->deviceGuid;
      for (remainingDwords = sizeof(TH_LEGACY_GUID) / sizeof(uint32_t); remainingDwords != 0; remainingDwords--) {
        guidCursor->Data1 = deviceGuid->Data1;
        deviceGuid = (TH_LEGACY_GUID *)&deviceGuid->Data2;
        guidCursor = (TH_LEGACY_GUID *)&guidCursor->Data2;
      }
      Text_CopyNarrowToUtf16(40,filledRecord->deviceNameUtf16,(uint8_t *)deviceName);
      filledRecord->hardwareDesc = (D3DDEVICEDESC_DX6 *)descAllocation.payloadOrError;
      descCursor = (D3DDEVICEDESC_DX6 *)descAllocation.payloadOrError;
      for (remainingDwords = sizeof(D3DDEVICEDESC_DX6) / sizeof(uint32_t); remainingDwords != 0;
           remainingDwords--) {
        descCursor->dwSize = hardwareDesc->dwSize;
        hardwareDesc = (D3DDEVICEDESC_DX6 *)&hardwareDesc->dwFlags;
        descCursor = (D3DDEVICEDESC_DX6 *)&descCursor->dwFlags;
      }
      filledRecord->softwareDesc = descCursor; /* the second half of the block */
      for (remainingDwords = sizeof(D3DDEVICEDESC_DX6) / sizeof(uint32_t); remainingDwords != 0;
           remainingDwords--) {
        descCursor->dwSize = softwareDesc->dwSize;
        softwareDesc = (D3DDEVICEDESC_DX6 *)&softwareDesc->dwFlags;
        descCursor = (D3DDEVICEDESC_DX6 *)&descCursor->dwFlags;
      }
    }
  }
  return 1;
}


/* Address: 0x00578820.
   IDirect3DDevice2::EnumTextureFormats callback that picks the two texture formats the renderer uploads
   to: the opaque format (the shallowest 16/32-bit RGB format, preferring one with more colour bits) and
   the alpha format (the RGB format with the narrowest alpha mask of at least two bits). Each chosen
   DDPIXELFORMAT is copied into g_Direct3DOpaqueTextureFormat / g_Direct3DAlphaTextureFormat.
   __stdcall because DirectDraw calls it (the original returns with RET 8).
*/
int32_t __stdcall
GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback
          (DDSURFACEDESC_DX6 *surfaceDesc,TH_LEGACY_LPVOID context)

{
  uint32_t pixelFormatFlags;
  int currentAlphaLowBit;
  int candidateAlphaLowBit;
  int candidateAlphaHighBit;
  uint32_t bitCountOrMaskDelta; /* first the candidate's bits per pixel, then the colour mask difference */
  int highBitOrCopyCount; /* first the current alpha mask's highest bit, then the dword copy counter */
  uint32_t candidateColorMask;
  int replaceOpaqueFormat;
  DDPIXELFORMAT *pixelFormatCursor;
  TH_LEGACY_DWORD *formatDwordCursor;

  bitCountOrMaskDelta = (surfaceDesc->ddpfPixelFormat).dwRGBBitCount;
  pixelFormatFlags = (surfaceDesc->ddpfPixelFormat).dwFlags;
  if (bitCountOrMaskDelta < 8) {
    return D3DENUMRET_OK;
  }
  if (bitCountOrMaskDelta == 8) {
    return D3DENUMRET_OK;
  }
  if ((bitCountOrMaskDelta != 16) && (bitCountOrMaskDelta != 32)) {
    return D3DENUMRET_OK;
  }
  if ((pixelFormatFlags & DDPF_RGB) == 0) {
    return D3DENUMRET_OK;
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
    for (highBitOrCopyCount = sizeof(DDPIXELFORMAT) / sizeof(TH_LEGACY_DWORD); highBitOrCopyCount != 0;
         highBitOrCopyCount--) {
      *formatDwordCursor = pixelFormatCursor->dwSize;
      pixelFormatCursor = (DDPIXELFORMAT *)&pixelFormatCursor->dwFlags;
      formatDwordCursor++;
    }
  }
  if (((pixelFormatFlags & DDPF_ALPHAPIXELS) != 0) && (8 < (surfaceDesc->ddpfPixelFormat).dwRGBBitCount)) {
    /* BSR/BSF of both alpha masks, compared as unsigned (low - high): a one-bit mask gives 0 and never wins,
       otherwise the narrower mask gives the larger value (e.g. 4444 beats 8888). */
    highBitOrCopyCount = 31;
    if (_g_Direct3DAlphaTextureFormatAlphaBitMask != 0) {
      for (; _g_Direct3DAlphaTextureFormatAlphaBitMask >> highBitOrCopyCount == 0; highBitOrCopyCount--) {
      }
    }
    currentAlphaLowBit = 0;
    if (_g_Direct3DAlphaTextureFormatAlphaBitMask != 0) {
      for (; (_g_Direct3DAlphaTextureFormatAlphaBitMask >> currentAlphaLowBit & 1) == 0; currentAlphaLowBit++) {
      }
    }
    formatDwordCursor = &(surfaceDesc->ddpfPixelFormat).dwRGBAlphaBitMask;
    candidateAlphaHighBit = 31;
    if (*formatDwordCursor != 0) {
      for (; *formatDwordCursor >> candidateAlphaHighBit == 0; candidateAlphaHighBit--) {
      }
    }
    formatDwordCursor = &(surfaceDesc->ddpfPixelFormat).dwRGBAlphaBitMask;
    candidateAlphaLowBit = 0;
    if (*formatDwordCursor != 0) {
      for (; (*formatDwordCursor >> candidateAlphaLowBit & 1) == 0; candidateAlphaLowBit++) {
      }
    }
    if ((uint32_t)(currentAlphaLowBit - highBitOrCopyCount) < (uint32_t)(candidateAlphaLowBit - candidateAlphaHighBit)) {
      pixelFormatCursor = &surfaceDesc->ddpfPixelFormat;
      formatDwordCursor = (TH_LEGACY_DWORD *)THANDOR_ADDR(g_Direct3DAlphaTextureFormat,0);
      for (highBitOrCopyCount = sizeof(DDPIXELFORMAT) / sizeof(TH_LEGACY_DWORD); highBitOrCopyCount != 0;
           highBitOrCopyCount--) {
        *formatDwordCursor = pixelFormatCursor->dwSize;
        pixelFormatCursor = (DDPIXELFORMAT *)&pixelFormatCursor->dwFlags;
        formatDwordCursor++;
      }
    }
  }
  return D3DENUMRET_OK;
}


/* Address: 0x0057A450.
   Ownership: graphics/backend/direct3d.
   Purpose: Handles direct3 drenderer set antialias mode.
*/
RenderStateApplyResult __thandor_eax_cf_preserve_ecx_edx
Direct3DRenderer_SetAntialiasMode(uint32_t antialiasMode)

{
  int32_t direct3DResult;
  RenderStateApplyResult successResult;
  RenderStateApplyResult failureResult;
  
  direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                    (g_Direct3DDevice2,D3DRENDERSTATE_ANTIALIAS,antialiasMode);
  if (direct3DResult == 0) {
    g_Direct3DAntialiasMode = antialiasMode;
    successResult.failed = false;
    successResult.appliedValueOrError = antialiasMode;
    return successResult;
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,100,g_PackageLastErrorPath);
  failureResult.failed = true;
  failureResult.appliedValueOrError = 0x1d;
  return failureResult;
}


/* Address: 0x0057A4C0.
   Ownership: graphics/backend/direct3d.
   Purpose: Handles direct3 drenderer set texture filter mode.
*/
RenderStateApplyResult __thandor_eax_cf_preserve_ecx_edx
Direct3DRenderer_SetTextureFilterMode(uint32_t textureFilterMode)

{
  int32_t direct3DResult;
  RenderStateApplyResult successResult;
  RenderStateApplyResult failureResult;
  int32_t errorCode;
  
  errorCode = 0x6e;
  direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                    (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMAG,textureFilterMode);
  if (direct3DResult == 0) {
    errorCode = 0x6f;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMIN,textureFilterMode);
    if (direct3DResult == 0) {
      g_Direct3DTextureFilterMode = textureFilterMode;
      successResult.failed = false;
      successResult.appliedValueOrError = textureFilterMode;
      return successResult;
    }
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,errorCode,g_PackageLastErrorPath);
  failureResult.failed = true;
  failureResult.appliedValueOrError = 0x1d;
  return failureResult;
}


/* Address: 0x0057A550.
   Ownership: graphics/backend/direct3d.
   Purpose: Handles direct3 drenderer set texture perspective enabled.
*/
RenderStateApplyResult __thandor_eax_cf_preserve_ecx_edx
Direct3DRenderer_SetTexturePerspectiveEnabled(uint32_t texturePerspectiveEnabled)

{
  int32_t direct3DResult;
  RenderStateApplyResult successResult;
  RenderStateApplyResult failureResult;
  
  direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                    (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREPERSPECTIVE,texturePerspectiveEnabled);
  if (direct3DResult == 0) {
    g_Direct3DTexturePerspectiveEnabled = texturePerspectiveEnabled;
    successResult.failed = false;
    successResult.appliedValueOrError = texturePerspectiveEnabled;
    return successResult;
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x78,g_PackageLastErrorPath);
  failureResult.failed = true;
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
  uint64_t modulationWords;
  int32_t bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  int32_t unusedResult;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[0].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[0].zWriteEnable;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[0].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[0].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[0].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
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
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (uint32_t)newBoundTextureHandle;
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
  uint64_t modulationWords;
  int32_t bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  int32_t unusedResult;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[2].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[2].zWriteEnable;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[2].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[2].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[2].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                            (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[2].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[2].sourceBlend;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                        (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
                         g_PrimitiveRenderStatePresets[2].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[2].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[2].destinationBlend
    ;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
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
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (uint32_t)newBoundTextureHandle;
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
  uint64_t modulationWords;
  int32_t bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  int32_t unusedResult;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[3].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[3].zWriteEnable;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[3].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[3].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[3].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                            (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[3].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[3].sourceBlend;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                        (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
                         g_PrimitiveRenderStatePresets[3].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[3].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[3].destinationBlend
    ;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
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
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (uint32_t)newBoundTextureHandle;
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
  uint64_t modulationWords;
  int32_t bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  int32_t unusedResult;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[4].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[4].zWriteEnable;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                        (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                         g_PrimitiveRenderStatePresets[4].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[4].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                          (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                           g_PrimitiveRenderStatePresets[4].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                            (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[4].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[4].sourceBlend;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                        (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
                         g_PrimitiveRenderStatePresets[4].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[4].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[4].destinationBlend
    ;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
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
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (uint32_t)newBoundTextureHandle;
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
  uint32_t textureWidthLog2;
  uint32_t textureHeightLog2;
  GraphicsTextureSetEntry *packetTextureEntry;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle deviceTextureHandle;
  int32_t bindResult;
  uint8_t coordinateShift;
  uint32_t largerDimensionLog2;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[0].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[0].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[0].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[0].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[0].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        g_Direct3DDevice2->lpVtbl->SetRenderState
                  (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[0].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[0].sourceBlend;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
               g_PrimitiveRenderStatePresets[0].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[0].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[0].destinationBlend
    ;
    g_Direct3DDevice2->lpVtbl->SetRenderState
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
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (uint32_t)newBoundTextureHandle;
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
  uint32_t textureWidthLog2;
  uint32_t textureHeightLog2;
  GraphicsTextureSetEntry *packetTextureEntry;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle deviceTextureHandle;
  int32_t bindResult;
  uint8_t coordinateShift;
  uint32_t largerDimensionLog2;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[1].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[1].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[1].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[1].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[1].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[1].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[1].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        g_Direct3DDevice2->lpVtbl->SetRenderState
                  (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[1].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[1].sourceBlend;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
               g_PrimitiveRenderStatePresets[1].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[1].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[1].destinationBlend
    ;
    g_Direct3DDevice2->lpVtbl->SetRenderState
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
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (uint32_t)newBoundTextureHandle;
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
  uint32_t textureWidthLog2;
  uint32_t textureHeightLog2;
  GraphicsTextureSetEntry *packetTextureEntry;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle deviceTextureHandle;
  int32_t bindResult;
  uint8_t coordinateShift;
  uint32_t largerDimensionLog2;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[2].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[2].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[2].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[2].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[2].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        g_Direct3DDevice2->lpVtbl->SetRenderState
                  (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[2].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[2].sourceBlend;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
               g_PrimitiveRenderStatePresets[2].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[2].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[2].destinationBlend
    ;
    g_Direct3DDevice2->lpVtbl->SetRenderState
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
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (uint32_t)newBoundTextureHandle;
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
  uint32_t textureWidthLog2;
  uint32_t textureHeightLog2;
  GraphicsTextureSetEntry *packetTextureEntry;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle deviceTextureHandle;
  int32_t bindResult;
  uint8_t coordinateShift;
  uint32_t largerDimensionLog2;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[3].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[3].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[3].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[3].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[3].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        g_Direct3DDevice2->lpVtbl->SetRenderState
                  (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[3].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[3].sourceBlend;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
               g_PrimitiveRenderStatePresets[3].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[3].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[3].destinationBlend
    ;
    g_Direct3DDevice2->lpVtbl->SetRenderState
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
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (uint32_t)newBoundTextureHandle;
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
  uint32_t textureWidthLog2;
  uint32_t textureHeightLog2;
  GraphicsTextureSetEntry *packetTextureEntry;
  GraphicsTextureResource *texture;
  GraphicsTextureHandle deviceTextureHandle;
  int32_t bindResult;
  uint8_t coordinateShift;
  uint32_t largerDimensionLog2;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  D3DTLVERTEX_DX6 *sourceVertexCursor;
  D3DTLVERTEX_DX6 *destVertexCursor;
  IDirect3DDevice2 *newBoundTextureHandle;
  
  if (g_PrimitiveRenderStatePresets[4].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[4].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[4].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[4].alphaBlendEnable
    ;
    if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[4].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & 1) == 0) || (deviceDesc->dcmColorModel == 2)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & 0x4000) == 0) {
        g_Direct3DDevice2->lpVtbl->SetRenderState
                  (g_Direct3DDevice2,D3DRENDERSTATE_STIPPLEDALPHA,1);
      }
    }
  }
  if (g_PrimitiveRenderStatePresets[4].sourceBlend != g_PrimitiveRenderStateCache.sourceBlend) {
    g_PrimitiveRenderStateCache.sourceBlend = g_PrimitiveRenderStatePresets[4].sourceBlend;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
               g_PrimitiveRenderStatePresets[4].sourceBlend);
  }
  if (g_PrimitiveRenderStatePresets[4].destinationBlend !=
      g_PrimitiveRenderStateCache.destinationBlend) {
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[4].destinationBlend
    ;
    g_Direct3DDevice2->lpVtbl->SetRenderState
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
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult != 0) {
      newBoundTextureHandle = (IDirect3DDevice2 *)g_BoundTextureHandle;
    }
    g_BoundTextureHandle = (uint32_t)newBoundTextureHandle;
    g_TextureBindStateChangeCount = g_TextureBindStateChangeCount + 1;
  }
  return;
}

