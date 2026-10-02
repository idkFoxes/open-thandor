/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/direct3d.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/direct3d.h>
#include <thandor/thandor.h>

/* Implementation ownership: graphics/backend/direct3d. */

/* No address: C model of the inline MMX sequence the untextured primitive handlers use to modulate colors.
   MOVD mm,color; PUNPCKLBW mm,mm; PSRLW mm,4: each color byte b (B,G,R,A from low to high)
   becomes the 16-bit lane (b << 8 | b) >> 4. */
static __inline uint64_t Direct3D_MmxUnpackColorWords(PackedArgb32 color)
{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane++) {
    lanes.uw[lane] = (uint16_t)((((color >> (lane * 8)) & ARGB8888_CHANNEL_MASK) * COLOR_CHANNEL_TO_WORD_LANE) >> 4);
  }
  return lanes.q;
}

/* No address: C model of the inline MMX pack after PMULHW in the untextured primitive handlers.
   PACKUSWB mm,mm; MOVD color,mm: each signed 16-bit lane saturated to 0..0xff and packed back into
   the B,G,R,A bytes of a D3D color. */
static __inline PackedArgb32 Direct3D_MmxPackColorWords(uint64_t words)
{
  ThandorMmx lanes;
  PackedArgb32 color;
  int lane;
  short value;

  lanes.q = words;
  color = 0;
  for (lane = 0; lane < 4; lane++) {
    value = lanes.sw[lane];
    color = color | ((PackedArgb32)(value < 0 ? 0 : (value > ARGB8888_CHANNEL_MAX ? ARGB8888_CHANNEL_MAX : value)) << (lane * 8));
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
  uint32_t *recordCursor;
  uint32_t *guidCursor;
  uint32_t *descCursor;
  uint32_t descAllocationError;

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
    descAllocationError = g_MemoryApi.alloc(2 * sizeof(D3DDEVICEDESC_DX6),(void **)&descCursor);
    updatedAdapterCount = g_GraphicsAdapterCount;
    if (descAllocationError == 0) {
      newRecord = g_GraphicsAdapters + newAdapterIndex;
      remainingDwords = sizeof(GraphicsAdapterRecord) / sizeof(uint32_t);
      g_GraphicsAdapterCount++;
      recordCursor = (uint32_t *)newRecord;
      /* in the filtered mode the first device of an adapter reuses the adapter's record (count bumped back) */
      if ((g_GraphicsEnumerateAllDevicesFlag == 0) ||
         (filledRecord = adapterContext, (adapterContext->deviceGuid).Data1 != 0)) {
        for (; filledRecord = newRecord, updatedAdapterCount = g_GraphicsAdapterCount, remainingDwords != 0;
             remainingDwords--) {
          *recordCursor = *(uint32_t *)adapterContext;
          adapterContext = (GraphicsAdapterRecord *)((uint32_t *)adapterContext + 1);
          recordCursor++;
        }
      }
      g_GraphicsAdapterCount = updatedAdapterCount;
      guidCursor = (uint32_t *)&filledRecord->deviceGuid;
      for (remainingDwords = sizeof(TH_LEGACY_GUID) / sizeof(uint32_t); remainingDwords != 0; remainingDwords--) {
        *guidCursor = *(uint32_t *)deviceGuid;
        deviceGuid = (TH_LEGACY_GUID *)((uint32_t *)deviceGuid + 1);
        guidCursor++;
      }
      Text_CopyNarrowToUtf16(40,filledRecord->deviceNameUtf16,(uint8_t *)deviceName);
      filledRecord->hardwareDesc = (D3DDEVICEDESC_DX6 *)descCursor;
      for (remainingDwords = sizeof(D3DDEVICEDESC_DX6) / sizeof(uint32_t); remainingDwords != 0;
           remainingDwords--) {
        *descCursor = *(uint32_t *)hardwareDesc;
        hardwareDesc = (D3DDEVICEDESC_DX6 *)((uint32_t *)hardwareDesc + 1);
        descCursor++;
      }
      filledRecord->softwareDesc = (D3DDEVICEDESC_DX6 *)descCursor; /* the second half of the block */
      for (remainingDwords = sizeof(D3DDEVICEDESC_DX6) / sizeof(uint32_t); remainingDwords != 0;
           remainingDwords--) {
        *descCursor = *(uint32_t *)softwareDesc;
        softwareDesc = (D3DDEVICEDESC_DX6 *)((uint32_t *)softwareDesc + 1);
        descCursor++;
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
  TH_LEGACY_DWORD *pixelFormatCursor;
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
    pixelFormatCursor = (TH_LEGACY_DWORD *)&surfaceDesc->ddpfPixelFormat;
    formatDwordCursor = (TH_LEGACY_DWORD *)&g_Direct3DOpaqueTextureFormat;
    for (highBitOrCopyCount = sizeof(DDPIXELFORMAT) / sizeof(TH_LEGACY_DWORD); highBitOrCopyCount != 0;
         highBitOrCopyCount--) {
      *formatDwordCursor = *pixelFormatCursor;
      pixelFormatCursor++;
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
      pixelFormatCursor = (TH_LEGACY_DWORD *)&surfaceDesc->ddpfPixelFormat;
      formatDwordCursor = (TH_LEGACY_DWORD *)&g_Direct3DAlphaTextureFormat;
      for (highBitOrCopyCount = sizeof(DDPIXELFORMAT) / sizeof(TH_LEGACY_DWORD); highBitOrCopyCount != 0;
           highBitOrCopyCount--) {
        *formatDwordCursor = *pixelFormatCursor;
        pixelFormatCursor++;
        formatDwordCursor++;
      }
    }
  }
  return D3DENUMRET_OK;
}


/* Address: 0x0057A450.
   Sets D3DRENDERSTATE_ANTIALIAS on the live device and remembers the mode in g_Direct3DAntialiasMode, which the
   device setup (GraphicsDirectDraw_ApplyDisplayModeAndCreateResources) applies again after a mode change.
   Returns 0 on success, or FATAL_ERROR_DIRECT3D_SETUP with the stage number in g_PackageLastErrorPath (the
   original returned the mode itself on success, with CF clear). No caller or table slot in the executable or
   image data references it.
*/
uint32_t Direct3DRenderer_SetAntialiasMode(uint32_t antialiasMode)

{
  int32_t direct3DResult;

  direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                    (g_Direct3DDevice2,D3DRENDERSTATE_ANTIALIAS,antialiasMode);
  if (direct3DResult == 0) {
    g_Direct3DAntialiasMode = antialiasMode;
    return 0;
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,DIRECT3D_RENDER_STATE_STAGE_ANTIALIAS,
                          g_PackageLastErrorPath);
  return FATAL_ERROR_DIRECT3D_SETUP;
}


/* Address: 0x0057A4C0.
   Sets the same filter mode for D3DRENDERSTATE_TEXTUREMAG and D3DRENDERSTATE_TEXTUREMIN and remembers it in
   g_Direct3DTextureFilterMode for the device setup to reapply. Returns 0 on success (the original returned the
   mode), or FATAL_ERROR_DIRECT3D_SETUP with the stage of the failed call in g_PackageLastErrorPath. No caller or
   table slot in the executable or image data references it.
*/
uint32_t Direct3DRenderer_SetTextureFilterMode(uint32_t textureFilterMode)

{
  int32_t direct3DResult;
  int32_t failedStage;

  failedStage = DIRECT3D_RENDER_STATE_STAGE_TEXTURE_MAG;
  direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                    (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMAG,textureFilterMode);
  if (direct3DResult == 0) {
    failedStage = DIRECT3D_RENDER_STATE_STAGE_TEXTURE_MIN;
    direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMIN,textureFilterMode);
    if (direct3DResult == 0) {
      g_Direct3DTextureFilterMode = textureFilterMode;
      return 0;
    }
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,failedStage,g_PackageLastErrorPath);
  return FATAL_ERROR_DIRECT3D_SETUP;
}


/* Address: 0x0057A550.
   Sets D3DRENDERSTATE_TEXTUREPERSPECTIVE and remembers the value in g_Direct3DTexturePerspectiveEnabled for the
   device setup to reapply. Returns 0 on success (the original returned the value), or FATAL_ERROR_DIRECT3D_SETUP
   with the stage number in g_PackageLastErrorPath. No caller or table slot in the executable or image data
   references it.
*/
uint32_t Direct3DRenderer_SetTexturePerspectiveEnabled(uint32_t texturePerspectiveEnabled)

{
  int32_t direct3DResult;

  direct3DResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                    (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREPERSPECTIVE,texturePerspectiveEnabled);
  if (direct3DResult == 0) {
    g_Direct3DTexturePerspectiveEnabled = texturePerspectiveEnabled;
    return 0;
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,DIRECT3D_RENDER_STATE_STAGE_TEXTURE_PERSPECTIVE,
                          g_PackageLastErrorPath);
  return FATAL_ERROR_DIRECT3D_SETUP;
}


/* Address: 0x0057CCB0.
   Fills g_ImmediateTLVertices for one untextured opaque packet (render-state preset 0: Z-write on, alpha
   blending off; only those two states are applied, the blend factors are left as they are). Vertex colors are
   modulated by packet->modulationColor with MMX, texture coordinates are zeroed, the second vertex is copied
   into slot 3 for a four-vertex fan, and any bound texture is unbound. Graphics_DrawPrimitiveQueue calls it
   through g_GraphicsDispatchTable.primitive slots 0, 4, 8 and 12.
*/
void Direct3D_PrimitiveHandler_UntexturedPreset0(GraphicsPrimitivePacket *packet)

{
  PackedArgb32 packetModulationColor;
  PackedArgb32 vertex0Diffuse;
  PackedArgb32 vertex1Diffuse;
  PackedArgb32 vertex2Diffuse;
  uint64_t modulationWords;
  int32_t bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  uint32_t *sourceVertexCursor; /* dword copy of vertex 1 into slot 3 */
  uint32_t *destVertexCursor;
  
  if (g_PrimitiveRenderStatePresets[0].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[0].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[0].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[0].alphaBlendEnable;
    if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[0].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & D3DDD_COLORMODEL) == 0) || (deviceDesc->dcmColorModel == D3DCOLOR_RGB)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      /* without alpha-blended Gouraud shading fall back to stippled alpha */
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) == 0) {
        g_Direct3DDevice2->lpVtbl->SetRenderState
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = (uint32_t *)&g_ImmediateTLVertices[1];
    destVertexCursor = (uint32_t *)&g_ImmediateTLVertices[3];
    for (remainingDwords = sizeof(D3DTLVERTEX_DX6) / sizeof(uint32_t); remainingDwords != 0; remainingDwords--) {
      *destVertexCursor = *sourceVertexCursor;
      sourceVertexCursor++;
      destVertexCursor++;
    }
  }
  if (g_BoundTextureHandle != 0) {
    /* The original pushes the handle twice and on success pops it into g_BoundTextureHandle; the bind
       counter counts every attempt, failed or not. */
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult == 0) {
      g_BoundTextureHandle = 0;
    }
    g_TextureBindStateChangeCount++;
  }
  return;
}


/* Address: 0x0057CF20.
   Fills g_ImmediateTLVertices for one untextured translucent packet (render-state preset 2: SRCALPHA/INVSRCALPHA,
   alpha blending on, Z-write off). Vertex colors are modulated by packet->modulationColor with MMX, texture
   coordinates are zeroed, the second vertex is copied into slot 3 for a four-vertex fan, and any bound texture
   is unbound. Graphics_DrawPrimitiveQueue calls it through g_GraphicsDispatchTable.primitive slots 1, 9, 32, 33,
   36, 38, 40, 41, 44 and 46.
*/
void Direct3D_PrimitiveHandler_UntexturedPreset2(GraphicsPrimitivePacket *packet)

{
  PackedArgb32 packetModulationColor;
  PackedArgb32 vertex0Diffuse;
  PackedArgb32 vertex1Diffuse;
  PackedArgb32 vertex2Diffuse;
  uint64_t modulationWords;
  int32_t bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  uint32_t *sourceVertexCursor; /* dword copy of vertex 1 into slot 3 */
  uint32_t *destVertexCursor;
  
  if (g_PrimitiveRenderStatePresets[2].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[2].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[2].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[2].alphaBlendEnable;
    if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[2].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & D3DDD_COLORMODEL) == 0) || (deviceDesc->dcmColorModel == D3DCOLOR_RGB)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      /* without alpha-blended Gouraud shading fall back to stippled alpha */
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) == 0) {
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
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[2].destinationBlend;
    g_Direct3DDevice2->lpVtbl->SetRenderState
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = (uint32_t *)&g_ImmediateTLVertices[1];
    destVertexCursor = (uint32_t *)&g_ImmediateTLVertices[3];
    for (remainingDwords = sizeof(D3DTLVERTEX_DX6) / sizeof(uint32_t); remainingDwords != 0; remainingDwords--) {
      *destVertexCursor = *sourceVertexCursor;
      sourceVertexCursor++;
      destVertexCursor++;
    }
  }
  if (g_BoundTextureHandle != 0) {
    /* The original pushes the handle twice and on success pops it into g_BoundTextureHandle; the bind
       counter counts every attempt, failed or not. */
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult == 0) {
      g_BoundTextureHandle = 0;
    }
    g_TextureBindStateChangeCount++;
  }
  return;
}


/* Address: 0x0057D1D0.
   Fills g_ImmediateTLVertices for one untextured translucent packet that still writes depth (render-state
   preset 3: SRCALPHA/INVSRCALPHA, alpha blending on, Z-write on; same values as preset 1, which only the textured
   handlers use). Colors are modulated with MMX, texture coordinates zeroed, the second vertex copied into slot 3
   for a four-vertex fan, and any bound texture is unbound. Graphics_DrawPrimitiveQueue calls it through
   g_GraphicsDispatchTable.primitive slots 6 and 14.
*/
void Direct3D_PrimitiveHandler_UntexturedPreset3(GraphicsPrimitivePacket *packet)

{
  PackedArgb32 packetModulationColor;
  PackedArgb32 vertex0Diffuse;
  PackedArgb32 vertex1Diffuse;
  PackedArgb32 vertex2Diffuse;
  uint64_t modulationWords;
  int32_t bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  uint32_t *sourceVertexCursor; /* dword copy of vertex 1 into slot 3 */
  uint32_t *destVertexCursor;
  
  if (g_PrimitiveRenderStatePresets[3].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[3].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[3].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[3].alphaBlendEnable;
    if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[3].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & D3DDD_COLORMODEL) == 0) || (deviceDesc->dcmColorModel == D3DCOLOR_RGB)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      /* without alpha-blended Gouraud shading fall back to stippled alpha */
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) == 0) {
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
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[3].destinationBlend;
    g_Direct3DDevice2->lpVtbl->SetRenderState
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = (uint32_t *)&g_ImmediateTLVertices[1];
    destVertexCursor = (uint32_t *)&g_ImmediateTLVertices[3];
    for (remainingDwords = sizeof(D3DTLVERTEX_DX6) / sizeof(uint32_t); remainingDwords != 0; remainingDwords--) {
      *destVertexCursor = *sourceVertexCursor;
      sourceVertexCursor++;
      destVertexCursor++;
    }
  }
  if (g_BoundTextureHandle != 0) {
    /* The original pushes the handle twice and on success pops it into g_BoundTextureHandle; the bind
       counter counts every attempt, failed or not. */
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult == 0) {
      g_BoundTextureHandle = 0;
    }
    g_TextureBindStateChangeCount++;
  }
  return;
}


/* Address: 0x0057D480.
   Fills g_ImmediateTLVertices for one untextured additive packet (render-state preset 4: ONE/ONE, alpha blending
   on, Z-write off). Colors are modulated by packet->modulationColor with MMX, texture coordinates zeroed, the
   second vertex copied into slot 3 for a four-vertex fan, and any bound texture is unbound.
   Graphics_DrawPrimitiveQueue calls it through g_GraphicsDispatchTable.primitive slots 2, 10, 34 and 42.
*/
void Direct3D_PrimitiveHandler_UntexturedPreset4(GraphicsPrimitivePacket *packet)

{
  PackedArgb32 packetModulationColor;
  PackedArgb32 vertex0Diffuse;
  PackedArgb32 vertex1Diffuse;
  PackedArgb32 vertex2Diffuse;
  uint64_t modulationWords;
  int32_t bindResult;
  int remainingDwords;
  D3DDEVICEDESC_DX6 *deviceDesc;
  uint32_t *sourceVertexCursor; /* dword copy of vertex 1 into slot 3 */
  uint32_t *destVertexCursor;
  
  if (g_PrimitiveRenderStatePresets[4].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[4].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[4].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[4].alphaBlendEnable;
    if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[4].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & D3DDD_COLORMODEL) == 0) || (deviceDesc->dcmColorModel == D3DCOLOR_RGB)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      /* without alpha-blended Gouraud shading fall back to stippled alpha */
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) == 0) {
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
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[4].destinationBlend;
    g_Direct3DDevice2->lpVtbl->SetRenderState
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = (uint32_t *)&g_ImmediateTLVertices[1];
    destVertexCursor = (uint32_t *)&g_ImmediateTLVertices[3];
    for (remainingDwords = sizeof(D3DTLVERTEX_DX6) / sizeof(uint32_t); remainingDwords != 0; remainingDwords--) {
      *destVertexCursor = *sourceVertexCursor;
      sourceVertexCursor++;
      destVertexCursor++;
    }
  }
  if (g_BoundTextureHandle != 0) {
    /* The original pushes the handle twice and on success pops it into g_BoundTextureHandle; the bind
       counter counts every attempt, failed or not. */
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                       (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
    if (bindResult == 0) {
      g_BoundTextureHandle = 0;
    }
    g_TextureBindStateChangeCount++;
  }
  return;
}


/* Address: 0x0057D730.
   Fills g_ImmediateTLVertices for one textured opaque packet (render-state preset 0: ONE/ZERO, alpha blending
   off, Z-write on). U/V are shifted down so both are relative to the texture's larger log2 dimension, the second
   vertex is copied into slot 3 for a four-vertex fan, and the texture is marked used, recreated on the device if
   it was evicted, and bound. Graphics_DrawPrimitiveQueue calls it through g_GraphicsDispatchTable.primitive
   slots 16 and 24.
*/
void Direct3D_PrimitiveHandler_TexturedPreset0(GraphicsPrimitivePacket *packet)

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
  uint32_t *sourceVertexCursor; /* dword copy of vertex 1 into slot 3 */
  uint32_t *destVertexCursor;
  
  if (g_PrimitiveRenderStatePresets[0].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[0].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[0].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[0].alphaBlendEnable;
    if (g_PrimitiveRenderStatePresets[0].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[0].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & D3DDD_COLORMODEL) == 0) || (deviceDesc->dcmColorModel == D3DCOLOR_RGB)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      /* without alpha-blended Gouraud shading fall back to stippled alpha */
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) == 0) {
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
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[0].destinationBlend;
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  textureWidthLog2 = packet->textureEntry->widthLog2;
  textureHeightLog2 = packet->textureEntry->heightLog2;
  largerDimensionLog2 = textureWidthLog2;
  if (textureWidthLog2 < textureHeightLog2) {
    largerDimensionLog2 = textureHeightLog2;
  }
  coordinateShift = (char)largerDimensionLog2 - (char)textureWidthLog2;
  textureCoordinate = &packet->vertices[0].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[1].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[2].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  coordinateShift = (char)largerDimensionLog2 - (char)textureHeightLog2;
  textureCoordinate = &packet->vertices[0].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[1].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[2].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  packetTextureEntry = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = (uint32_t *)&g_ImmediateTLVertices[1];
    destVertexCursor = (uint32_t *)&g_ImmediateTLVertices[3];
    for (remainingDwords = sizeof(D3DTLVERTEX_DX6) / sizeof(uint32_t); remainingDwords != 0; remainingDwords--) {
      *destVertexCursor = *sourceVertexCursor;
      sourceVertexCursor++;
      destVertexCursor++;
    }
  }
  texture = packetTextureEntry->texture;
  deviceTextureHandle = 0;
  if (texture != NULL) {
    deviceTextureHandle = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial++;
    if (deviceTextureHandle == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount++;
      deviceTextureHandle = texture->textureHandle;
    }
  }
  if (deviceTextureHandle != g_BoundTextureHandle) {
    /* The original pushes the handle twice and on success pops it into g_BoundTextureHandle; the bind
       counter counts every attempt, failed or not. */
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult == 0) {
      g_BoundTextureHandle = deviceTextureHandle;
    }
    g_TextureBindStateChangeCount++;
  }
  return;
}


/* Address: 0x0057DA50.
   Fills g_ImmediateTLVertices for one textured translucent packet that still writes depth (render-state preset 1:
   SRCALPHA/INVSRCALPHA, alpha blending on, Z-write on). U/V are made relative to the texture's larger log2
   dimension, the second vertex is copied into slot 3 for a four-vertex fan, and the texture is marked used,
   recreated on the device if it was evicted, and bound. Graphics_DrawPrimitiveQueue calls it through
   g_GraphicsDispatchTable.primitive slots 20 and 28.
*/
void Direct3D_PrimitiveHandler_TexturedPreset1(GraphicsPrimitivePacket *packet)

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
  uint32_t *sourceVertexCursor; /* dword copy of vertex 1 into slot 3 */
  uint32_t *destVertexCursor;
  
  if (g_PrimitiveRenderStatePresets[1].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[1].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[1].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[1].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[1].alphaBlendEnable;
    if (g_PrimitiveRenderStatePresets[1].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[1].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & D3DDD_COLORMODEL) == 0) || (deviceDesc->dcmColorModel == D3DCOLOR_RGB)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      /* without alpha-blended Gouraud shading fall back to stippled alpha */
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) == 0) {
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
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[1].destinationBlend;
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  textureWidthLog2 = packet->textureEntry->widthLog2;
  textureHeightLog2 = packet->textureEntry->heightLog2;
  largerDimensionLog2 = textureWidthLog2;
  if (textureWidthLog2 < textureHeightLog2) {
    largerDimensionLog2 = textureHeightLog2;
  }
  coordinateShift = (char)largerDimensionLog2 - (char)textureWidthLog2;
  textureCoordinate = &packet->vertices[0].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[1].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[2].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  coordinateShift = (char)largerDimensionLog2 - (char)textureHeightLog2;
  textureCoordinate = &packet->vertices[0].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[1].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[2].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  packetTextureEntry = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = (uint32_t *)&g_ImmediateTLVertices[1];
    destVertexCursor = (uint32_t *)&g_ImmediateTLVertices[3];
    for (remainingDwords = sizeof(D3DTLVERTEX_DX6) / sizeof(uint32_t); remainingDwords != 0; remainingDwords--) {
      *destVertexCursor = *sourceVertexCursor;
      sourceVertexCursor++;
      destVertexCursor++;
    }
  }
  texture = packetTextureEntry->texture;
  deviceTextureHandle = 0;
  if (texture != NULL) {
    deviceTextureHandle = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial++;
    if (deviceTextureHandle == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount++;
      deviceTextureHandle = texture->textureHandle;
    }
  }
  if (deviceTextureHandle != g_BoundTextureHandle) {
    /* The original pushes the handle twice and on success pops it into g_BoundTextureHandle; the bind
       counter counts every attempt, failed or not. */
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult == 0) {
      g_BoundTextureHandle = deviceTextureHandle;
    }
    g_TextureBindStateChangeCount++;
  }
  return;
}


/* Address: 0x0057DD70.
   Fills g_ImmediateTLVertices for one textured translucent packet (render-state preset 2: SRCALPHA/INVSRCALPHA,
   alpha blending on, Z-write off). U/V are made relative to the texture's larger log2 dimension, the second
   vertex is copied into slot 3 for a four-vertex fan, and the texture is marked used, recreated on the device if
   it was evicted, and bound. Graphics_DrawPrimitiveQueue calls it through g_GraphicsDispatchTable.primitive
   slots 17, 25, 48, 49, 52, 54, 56, 57, 60 and 62.
*/
void Direct3D_PrimitiveHandler_TexturedPreset2(GraphicsPrimitivePacket *packet)

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
  uint32_t *sourceVertexCursor; /* dword copy of vertex 1 into slot 3 */
  uint32_t *destVertexCursor;
  
  if (g_PrimitiveRenderStatePresets[2].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[2].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[2].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[2].alphaBlendEnable;
    if (g_PrimitiveRenderStatePresets[2].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[2].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & D3DDD_COLORMODEL) == 0) || (deviceDesc->dcmColorModel == D3DCOLOR_RGB)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      /* without alpha-blended Gouraud shading fall back to stippled alpha */
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) == 0) {
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
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[2].destinationBlend;
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  textureWidthLog2 = packet->textureEntry->widthLog2;
  textureHeightLog2 = packet->textureEntry->heightLog2;
  largerDimensionLog2 = textureWidthLog2;
  if (textureWidthLog2 < textureHeightLog2) {
    largerDimensionLog2 = textureHeightLog2;
  }
  coordinateShift = (char)largerDimensionLog2 - (char)textureWidthLog2;
  textureCoordinate = &packet->vertices[0].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[1].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[2].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  coordinateShift = (char)largerDimensionLog2 - (char)textureHeightLog2;
  textureCoordinate = &packet->vertices[0].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[1].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[2].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  packetTextureEntry = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = (uint32_t *)&g_ImmediateTLVertices[1];
    destVertexCursor = (uint32_t *)&g_ImmediateTLVertices[3];
    for (remainingDwords = sizeof(D3DTLVERTEX_DX6) / sizeof(uint32_t); remainingDwords != 0; remainingDwords--) {
      *destVertexCursor = *sourceVertexCursor;
      sourceVertexCursor++;
      destVertexCursor++;
    }
  }
  texture = packetTextureEntry->texture;
  deviceTextureHandle = 0;
  if (texture != NULL) {
    deviceTextureHandle = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial++;
    if (deviceTextureHandle == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount++;
      deviceTextureHandle = texture->textureHandle;
    }
  }
  if (deviceTextureHandle != g_BoundTextureHandle) {
    /* The original pushes the handle twice and on success pops it into g_BoundTextureHandle; the bind
       counter counts every attempt, failed or not. */
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult == 0) {
      g_BoundTextureHandle = deviceTextureHandle;
    }
    g_TextureBindStateChangeCount++;
  }
  return;
}


/* Address: 0x0057E090.
   Fills g_ImmediateTLVertices for one textured translucent packet that writes depth, using render-state preset 3
   (same values as preset 1: SRCALPHA/INVSRCALPHA, alpha blending on, Z-write on). U/V are made relative to the
   texture's larger log2 dimension, the second vertex is copied into slot 3 for a four-vertex fan, and the texture
   is marked used, recreated on the device if it was evicted, and bound. Graphics_DrawPrimitiveQueue calls it
   through g_GraphicsDispatchTable.primitive slots 22 and 30.
*/
void Direct3D_PrimitiveHandler_TexturedPreset3(GraphicsPrimitivePacket *packet)

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
  uint32_t *sourceVertexCursor; /* dword copy of vertex 1 into slot 3 */
  uint32_t *destVertexCursor;
  
  if (g_PrimitiveRenderStatePresets[3].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[3].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[3].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[3].alphaBlendEnable;
    if (g_PrimitiveRenderStatePresets[3].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[3].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & D3DDD_COLORMODEL) == 0) || (deviceDesc->dcmColorModel == D3DCOLOR_RGB)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      /* without alpha-blended Gouraud shading fall back to stippled alpha */
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) == 0) {
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
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[3].destinationBlend;
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  textureWidthLog2 = packet->textureEntry->widthLog2;
  textureHeightLog2 = packet->textureEntry->heightLog2;
  largerDimensionLog2 = textureWidthLog2;
  if (textureWidthLog2 < textureHeightLog2) {
    largerDimensionLog2 = textureHeightLog2;
  }
  coordinateShift = (char)largerDimensionLog2 - (char)textureWidthLog2;
  textureCoordinate = &packet->vertices[0].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[1].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[2].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  coordinateShift = (char)largerDimensionLog2 - (char)textureHeightLog2;
  textureCoordinate = &packet->vertices[0].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[1].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[2].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  packetTextureEntry = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = (uint32_t *)&g_ImmediateTLVertices[1];
    destVertexCursor = (uint32_t *)&g_ImmediateTLVertices[3];
    for (remainingDwords = sizeof(D3DTLVERTEX_DX6) / sizeof(uint32_t); remainingDwords != 0; remainingDwords--) {
      *destVertexCursor = *sourceVertexCursor;
      sourceVertexCursor++;
      destVertexCursor++;
    }
  }
  texture = packetTextureEntry->texture;
  deviceTextureHandle = 0;
  if (texture != NULL) {
    deviceTextureHandle = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial++;
    if (deviceTextureHandle == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount++;
      deviceTextureHandle = texture->textureHandle;
    }
  }
  if (deviceTextureHandle != g_BoundTextureHandle) {
    /* The original pushes the handle twice and on success pops it into g_BoundTextureHandle; the bind
       counter counts every attempt, failed or not. */
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult == 0) {
      g_BoundTextureHandle = deviceTextureHandle;
    }
    g_TextureBindStateChangeCount++;
  }
  return;
}


/* Address: 0x0057E3B0.
   Fills g_ImmediateTLVertices for one textured additive packet (render-state preset 4: ONE/ONE, alpha blending
   on, Z-write off). U/V are made relative to the texture's larger log2 dimension, the second vertex is copied
   into slot 3 for a four-vertex fan, and the texture is marked used, recreated on the device if it was evicted,
   and bound. Graphics_DrawPrimitiveQueue calls it through g_GraphicsDispatchTable.primitive slots 18, 26, 50
   and 58.
*/
void Direct3D_PrimitiveHandler_TexturedPreset4(GraphicsPrimitivePacket *packet)

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
  uint32_t *sourceVertexCursor; /* dword copy of vertex 1 into slot 3 */
  uint32_t *destVertexCursor;
  
  if (g_PrimitiveRenderStatePresets[4].zWriteEnable != g_PrimitiveRenderStateCache.zWriteEnable) {
    g_PrimitiveRenderStateCache.zWriteEnable = g_PrimitiveRenderStatePresets[4].zWriteEnable;
    g_Direct3DDevice2->lpVtbl->SetRenderState
              (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
               g_PrimitiveRenderStatePresets[4].zWriteEnable);
  }
  if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable !=
      g_PrimitiveRenderStateCache.alphaBlendEnable) {
    g_PrimitiveRenderStateCache.alphaBlendEnable = g_PrimitiveRenderStatePresets[4].alphaBlendEnable;
    if (g_PrimitiveRenderStatePresets[4].alphaBlendEnable == GRAPHICS_STATE_DISABLED) {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,0);
    }
    else {
      g_Direct3DDevice2->lpVtbl->SetRenderState
                (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                 g_PrimitiveRenderStatePresets[4].alphaBlendEnable);
      deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
      if (((deviceDesc->dwFlags & D3DDD_COLORMODEL) == 0) || (deviceDesc->dcmColorModel == D3DCOLOR_RGB)) {
        deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].softwareDesc;
      }
      /* without alpha-blended Gouraud shading fall back to stippled alpha */
      if (((deviceDesc->dpcTriCaps).dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) == 0) {
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
    g_PrimitiveRenderStateCache.destinationBlend = g_PrimitiveRenderStatePresets[4].destinationBlend;
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
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sx != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sx, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sy != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sy, -12 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  textureWidthLog2 = packet->textureEntry->widthLog2;
  textureHeightLog2 = packet->textureEntry->heightLog2;
  largerDimensionLog2 = textureWidthLog2;
  if (textureWidthLog2 < textureHeightLog2) {
    largerDimensionLog2 = textureHeightLog2;
  }
  coordinateShift = (char)largerDimensionLog2 - (char)textureWidthLog2;
  textureCoordinate = &packet->vertices[0].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[1].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[2].textureU;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  coordinateShift = (char)largerDimensionLog2 - (char)textureHeightLog2;
  textureCoordinate = &packet->vertices[0].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[1].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  textureCoordinate = &packet->vertices[2].textureV;
  *textureCoordinate = *textureCoordinate >> (coordinateShift & SHIFT_COUNT_MASK);
  g_ImmediateTLVertices[0].tu = (float)packet->vertices[0].textureU;
  g_ImmediateTLVertices[0].tv = (float)packet->vertices[0].textureV;
  g_ImmediateTLVertices[1].tu = (float)packet->vertices[1].textureU;
  g_ImmediateTLVertices[1].tv = (float)packet->vertices[1].textureV;
  g_ImmediateTLVertices[2].tu = (float)packet->vertices[2].textureU;
  g_ImmediateTLVertices[2].tv = (float)packet->vertices[2].textureV;
  if (g_ImmediateTLVertices[0].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].tu != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tu, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].tv != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].tv, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  g_ImmediateTLVertices[0].sz = (float)packet->vertices[0].depth;
  g_ImmediateTLVertices[0].rhw = 1.0 / g_ImmediateTLVertices[0].sz;
  g_ImmediateTLVertices[1].sz = (float)packet->vertices[1].depth;
  g_ImmediateTLVertices[1].rhw = 1.0 / g_ImmediateTLVertices[1].sz;
  g_ImmediateTLVertices[2].sz = (float)packet->vertices[2].depth;
  g_ImmediateTLVertices[2].rhw = 1.0 / g_ImmediateTLVertices[2].sz;
  if (g_ImmediateTLVertices[0].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[0].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[0].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[1].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[1].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].sz != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].sz, -20 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  if (g_ImmediateTLVertices[2].rhw != 0.0) {
    THANDOR_FLOAT_ADD_EXPONENT_BITS(g_ImmediateTLVertices[2].rhw, 18 * DIRECT3D_FLOAT_EXPONENT_STEP);
  }
  packetTextureEntry = packet->textureEntry;
  if (3 < g_ImmediateVertexCount) {
    sourceVertexCursor = (uint32_t *)&g_ImmediateTLVertices[1];
    destVertexCursor = (uint32_t *)&g_ImmediateTLVertices[3];
    for (remainingDwords = sizeof(D3DTLVERTEX_DX6) / sizeof(uint32_t); remainingDwords != 0; remainingDwords--) {
      *destVertexCursor = *sourceVertexCursor;
      sourceVertexCursor++;
      destVertexCursor++;
    }
  }
  texture = packetTextureEntry->texture;
  deviceTextureHandle = 0;
  if (texture != NULL) {
    deviceTextureHandle = texture->textureHandle;
    texture->lastUsedCounter = g_TextureUseSerial;
    g_TextureUseSerial++;
    if (deviceTextureHandle == 0) {
      GraphicsTexture_CreateDeviceTexture(texture);
      g_TextureDeviceReloadCount++;
      deviceTextureHandle = texture->textureHandle;
    }
  }
  if (deviceTextureHandle != g_BoundTextureHandle) {
    /* The original pushes the handle twice and on success pops it into g_BoundTextureHandle; the bind
       counter counts every attempt, failed or not. */
    bindResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,deviceTextureHandle);
    if (bindResult == 0) {
      g_BoundTextureHandle = deviceTextureHandle;
    }
    g_TextureBindStateChangeCount++;
  }
  return;
}

