/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/glide.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/glide.h>

/* Implementation ownership: graphics/backend/glide. */

/* Address: 0x005801B0.
   Ownership: graphics/backend/glide.
   Purpose: Populates a preallocated GraphicsTextureSet with Glide runtime texture resources. Nonstandard internal
   contract: EAX carries the preallocated set returned by GraphicsTextureSet_AllocateMetadata. The one stack
   argument is the original sourceAsset and is retained for the caller-visible ret 4 contract, but the
   implementation uses set->sourceAsset instead. ABI: CF clear means success. CF set means failure.
   Local calls: Glide3_TextureResource_Initialize, Glide3_TextureResource_Release.
   Cross-module calls: GraphicsTexture_SelectPixelFormat [graphics/resources/texture], GraphicsTexture_RegisterSlot
   [graphics/resources/texture].
*/
bool __thandor_cf_preserve_eax_ecx_edx
Glide3_TextureSet_CreateBackend
          (GraphicsTextureSet *textureSet,GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *sourceAsset_00;
  dword dVar1;
  DDPIXELFORMAT *pDVar2;
  GraphicsTextureResource *texture;
  bool bVar3;
  ArenaAllocEaxCf5 AVar4;
  GraphicsTextureSetEntry *pGStack_28;
  AssetSubresourceCount AStack_24;
  GraphicsSubresourceIndex GStack_20;
  
  sourceAsset_00 = textureSet->sourceAsset;
  AStack_24 = (sourceAsset_00->tableDescriptor).subresourceCount;
  pGStack_28 = textureSet->entries;
  GStack_20 = 0;
  do {
    pDVar2 = GraphicsTexture_SelectPixelFormat(GStack_20,sourceAsset_00);
    AVar4 = (*g_MemoryApi.alloc)(0x50);
    texture = (GraphicsTextureResource *)AVar4.eax;
    if (!AVar4.carry) {
      texture->stagingTexture2 = (IDirect3DTexture2 *)0x0;
      texture->stagingSurface3 = (IDirectDrawSurface3 *)0x0;
      texture->stagingSurfaceBase = (IDirectDrawSurface *)0x0;
      pGStack_28->texture = texture;
      texture->deviceTexture2 = (IDirect3DTexture2 *)0x0;
      texture->deviceSurface3 = (IDirectDrawSurface3 *)0x0;
      texture->deviceSurfaceBase = (IDirectDrawSurface *)0x0;
      texture->sourceAsset = sourceAsset_00;
      texture->subresourceIndex = GStack_20;
      texture->pixelFormat = pDVar2;
      dVar1 = g_TextureDownsampleShift;
      texture->lastUsedCounter = 0;
      texture->textureHandle = 0;
      texture->downsampleShift = dVar1;
      Glide3_TextureResource_Initialize(texture);
      bVar3 = GraphicsTexture_RegisterSlot(texture);
      if (bVar3) {
        Glide3_TextureResource_Release(texture);
        (*g_MemoryApi.free)(texture);
        pGStack_28->texture = (GraphicsTextureResource *)0x0;
      }
    }
    GStack_20 = GStack_20 + 1;
    pGStack_28 = pGStack_28 + 1;
    AStack_24 = AStack_24 - 1;
  } while (AStack_24 != 0);
  return false;
}


/* Address: 0x00580430.
   Ownership: graphics/backend/glide.
   Purpose: Walks all 4096 registered texture slots. Each non-null Glide texture resource is released and
   initialized again. Used when Glide texture-memory state must be rebuilt.
   Local calls: Glide3_TextureResource_Release, Glide3_TextureResource_Initialize.
*/
void __thandor_void_preserve_eax_ecx Glide3_TextureResource_ReinitializeAll(void)

{
  GraphicsTextureResource *texture;
  int textureSlotsRemaining;
  GraphicsTextureResource **textureSlotCursor;
  
  textureSlotsRemaining = 0x1000;
  textureSlotCursor = g_GraphicsTextureSlots;
  do {
    texture = *textureSlotCursor;
    if (texture != (GraphicsTextureResource *)0x0) {
      Glide3_TextureResource_Release(texture);
      Glide3_TextureResource_Initialize(texture);
    }
    textureSlotCursor = textureSlotCursor + 1;
    textureSlotsRemaining = textureSlotsRemaining + -1;
  } while (textureSlotsRemaining != 0);
  return;
}


/* Address: 0x0057F0C0.
   Ownership: graphics/backend/glide.
   Purpose: Glide backend shutdown wrapper that calls Glide3_Shutdown and returns. Existing register and flags
   results remain untouched by the prototype refinement.
   Local calls: Glide3_Shutdown.
*/
void __cdecl GlideBackend_ShutdownWrapper(void)

{
  Glide3_Shutdown();
  return;
}

/* Address: 0x0057F0F0.
   Ownership: graphics/backend/glide.
   Purpose: Implements the separate Glide 3 display-mode path, including dynamic DLL/API resolution and resource
   initialization. Carry carries success or failure; no normal scalar return is claimed. Typed parameters: p0
   adapterIndex→FrontendDisplayAdapterIndex_V302, p2 height→GraphicsPixelDimension_V302, p3
   width→GraphicsPixelDimension_V302. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: Glide3_TextureResource_Initialize.
   Cross-module calls: DynDLL_Load [platform/bootstrap/runtime], DynAPI_Resolve [platform/bootstrap/runtime],
   DynDLL_Unload [platform/bootstrap/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsGlide3_ApplyDisplayModeAndInitializeResourcesCf
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width)

{
  void **ppvVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  dword dVar5;
  void *output;
  GraphicsTextureMemoryAddress GVar6;
  uint uVar7;
  int iVar8;
  GraphicsTextureResidentTmuIndex tmuIndex;
  dword selectedRefreshRateCode;
  void **destination;
  void *pvVar9;
  GraphicsTextureResource **ppGVar10;
  DynDllLoadEaxCf5 DVar11;
  DynApiResolveEaxCf5 DVar12;
  ArenaAllocEaxCf5 AVar13;
  DisplayModeEaxCf5 DVar14;
  dword *output_00;
  dword resolutionQueryCode;
  
  resolutionQueryCode = 7;
  uVar4 = width | height << 0x10;
  if (((((uVar4 == 0x1e00280) || (resolutionQueryCode = 8, uVar4 == 0x2580320)) ||
       (resolutionQueryCode = 9, uVar4 == 0x2d003c0)) ||
      ((resolutionQueryCode = 0xc, uVar4 == 0x3000400 ||
       (resolutionQueryCode = 0xd, uVar4 == 0x4000500)))) ||
     (resolutionQueryCode = 0xe, uVar4 == 0x4b00640)) {
    DVar11 = DynDLL_Load(dynapi_5);
    if (!DVar11.carry) {
      g_GlideRuntimeActiveCount = g_GlideRuntimeActiveCount + 1;
      destination = (void **)&g_GrAADrawTriangle;
      do {
        DVar12 = DynAPI_Resolve(destination,DVar11.moduleOrError,destination[1]);
        if (DVar12.carry) goto Glide3_ReleaseRuntimeAfterInitializationFailure;
        ppvVar1 = destination + 3;
        destination = destination + 2;
      } while (*ppvVar1 != (void *)0x0);
      (*g_GrGlideInit)();
      dVar5._0_2_ = g_GraphicsAdapters[adapterIndex].adapterGuid.Data2;
      dVar5._2_2_ = g_GraphicsAdapters[adapterIndex].adapterGuid.Data3;
      (*g_GrSstSelect)(dVar5);
      g_GlideSelectedResolutionQuery = resolutionQueryCode;
      dVar5 = (*g_GrQueryResolutions)(&g_GlideSelectedResolutionQuery,(void *)0x0);
      output = (void *)0x19;
      if (0xf < (int)dVar5) {
        AVar13 = (*g_MemoryApi.alloc)(dVar5);
        output = (void *)AVar13.eax;
        if (!AVar13.carry) {
          uVar7 = dVar5 >> 4;
          (*g_GrQueryResolutions)(&g_GlideSelectedResolutionQuery,output);
          uVar4 = 0;
          pvVar9 = output;
          do {
            uVar2 = *(uint *)((int)pvVar9 + 4);
            if ((uVar2 < 9) && (uVar3 = *(uint *)(uVar2 * 4 + 0x57ecf0), uVar4 <= uVar3)) {
              uVar4 = uVar3;
              selectedRefreshRateCode = uVar2;
            }
            pvVar9 = (void *)((int)pvVar9 + 0x10);
            uVar7 = uVar7 - 1;
          } while (uVar7 != 0);
          (*g_MemoryApi.free)(output);
          g_GlideWindowContextHandle =
               (*g_GrSstWinOpen)((dword)g_MainWindow,resolutionQueryCode,selectedRefreshRateCode,0,0
                                 ,2,1);
          output = (void *)0x50;
          output_00 = &g_GraphicsAdapters[adapterIndex].reserved74;
          if (g_GlideWindowContextHandle != 0) {
            (*g_GrGet)(GLIDE_QUERY_SELECTOR_0x13,4,output_00);
            g_GlideTmuCount = *output_00;
            g_FramebufferWidth = width;
            g_FramebufferHeight = height;
            g_ActiveGraphicsAdapterIndex = adapterIndex;
            if ((int)g_GlideTmuCount < 1) {
              g_GlideTmuCount = 1;
            }
            else if (0x10 < (int)g_GlideTmuCount) {
              g_GlideTmuCount = 0x10;
            }
            g_DisplayFramebufferAccess.width = width;
            g_DisplayFramebufferAccess.height = height;
            g_DisplayFramebufferAccess.bytesPerPixel = SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT;
            g_DisplayFramebufferAccess.pixels = (byte *)0x0;
            g_FramebufferAccess = &g_DisplayFramebufferAccess;
            g_SoftwarePixelFormatConfig.redMask = 0xf800;
            g_SoftwarePixelFormatConfig.greenMask = 0x7e0;
            g_SoftwarePixelFormatConfig.blueMask = 0x1f;
            g_SoftwarePixelFormatConfig.redShift = 0xb;
            g_SoftwarePixelFormatConfig.greenShift = 5;
            g_SoftwarePixelFormatConfig.blueShift = 0;
            g_SoftwarePixelFormatConfig.redBitCount = 5;
            g_SoftwarePixelFormatConfig.greenBitCount = 6;
            g_SoftwarePixelFormatConfig.blueBitCount = 5;
            g_LastViewportRect.x1 = 0;
            g_LastViewportRect.y1 = 0;
            g_LastViewportRect.x2 = 0;
            g_LastViewportRect.y2 = 0;
            g_GraphicsFramebufferPresent = Glide3_Framebuffer_Present;
            g_GraphicsFramebufferCaptureRegion = Glide3_Framebuffer_CaptureRegion;
            g_GraphicsTextureSourceBlitSourceAlpha = Glide3_TextureSource_BlitSourceAlpha;
            g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha =
                 Glide3_TextureSource_BlitIntegerScaledSourceAlpha;
            g_GraphicsTextureSourceBlitSourceAlphaPaletteBank =
                 Glide3_TextureSource_BlitSourceAlphaPaletteBank;
            g_GraphicsTextureSourceBlitModulatedSourceAlpha =
                 Glide3_TextureSource_BlitModulatedSourceAlpha;
            g_GraphicsTextureSourceBlitSaturatedAddRgb = Glide3_TextureSource_BlitSaturatedAddRgb;
            g_GraphicsFramebufferFillRectArgb = Glide3_Framebuffer_FillRectArgb;
            g_GraphicsTextureSourceBlitHalfSourceRgb = Glide3_TextureSource_BlitHalfSourceRgb;
            g_GraphicsTextureSourceStretchDirectColorBilinear =
                 Glide3_TextureSource_StretchDirectColorBilinear;
            g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd =
                 Glide3_TextureSource_BlitHalfRgbSaturatedAdd;
            (*g_GuGammaCorrectionRGB)(0x3f800000,0x3f800000,0x3f800000);
            (*g_GrCoordinateSpace)(0);
            (*g_GrVertexLayout)(1,0,1);
            (*g_GrVertexLayout)(2,8,1);
            (*g_GrVertexLayout)(4,0xc,1);
            (*g_GrVertexLayout)(0x40,0x14,1);
            (*g_GrVertexLayout)(0x30,0x1c,1);
            (*g_GrCullMode)(0);
            (*g_GrDepthBufferMode)(1);
            (*g_GrDepthBufferFunction)(6);
            (*g_GrDepthMask)(1);
            g_GlideDepthWriteEnabledState = 1;
            tmuIndex = GRAPHICS_TEXTURE_RESIDENT_TMU0;
            dVar5 = g_GlideTmuCount;
            do {
              (*g_GrTexMipMapMode)(tmuIndex,0,0);
              (*g_GrTexClampMode)(tmuIndex,0,0);
              (*g_GrTexFilterMode)(tmuIndex,1,1);
              (*g_GrTexCombine)(tmuIndex,1,1,1,1,0,0);
              GVar6 = (*g_GrTexMinAddress)(tmuIndex);
              g_GlideTmuMinAddress[tmuIndex] = GVar6;
              GVar6 = (*g_GrTexMaxAddress)(tmuIndex);
              g_GlideTmuMaxAddress[tmuIndex] = GVar6;
              tmuIndex = tmuIndex + 1;
              dVar5 = dVar5 - 1;
            } while (dVar5 != 0);
            (*g_GrColorCombine)(3,1,0,1,0);
            (*g_GrAlphaCombine)(3,1,0,1,0);
            g_GlideTexturingDisabledState = 1;
            (*g_GrAlphaBlendFunction)(1,7,4,0);
            g_GlideBlendModeState = 0;
            g_GlideBoundTexture = (GraphicsTextureResource *)0x0;
            g_GlideResidentTextureHead = (GraphicsTextureResource *)0x0;
            g_GlideResidentTextureTail = (GraphicsTextureResource *)g_GlideTmuMinAddress[0];
            g_PrimarySurface3 = (IDirectDrawSurface3 *)0x0;
            DVar14 = (*g_GraphicsDisplayModeFinalizeCf)(adapterIndex,bitsPerPixel,height,width);
            output = (void *)DVar14.eax;
            if (!DVar14.carry) {
              iVar8 = 0x1000;
              ppGVar10 = g_GraphicsTextureSlots;
              do {
                if (*ppGVar10 != (GraphicsTextureResource *)0x0) {
                  Glide3_TextureResource_Initialize(*ppGVar10);
                }
                ppGVar10 = ppGVar10 + 1;
                iVar8 = iVar8 + -1;
              } while (iVar8 != 0);
              return false;
            }
            (*g_GrSstWinClose)(g_GlideWindowContextHandle);
          }
        }
      }
      (*g_GrGlideShutdown)(output);
Glide3_ReleaseRuntimeAfterInitializationFailure:
      DynDLL_Unload(dynapi_5);
      g_GlideRuntimeActiveCount = 0;
    }
  }
  return true;
}


/* Address: 0x0057F7B0.
   Ownership: graphics/backend/glide.
   Purpose: Traverses the typed primitive queue and submits packets through the Glide3 backend. It uses the same
   four-coordinate and queue ABI as Graphics_DrawPrimitiveQueue.
   Local calls: Glide3_TextureResource_EnsureResident.
   Cross-module calls: GraphicsPrimitiveQueue_Begin [graphics/render/primitives], GraphicsPrimitiveQueue_Next
   [graphics/render/primitives].
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_DrawPrimitiveQueue
          (sdword coordinate0,sdword coordinate1,sdword coordinate2,sdword coordinate3,
          GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitiveTextureCoordinateFixed *pGVar1;
  GraphicsTextureSetEntry *pGVar2;
  uint uVar3;
  GraphicsTextureResource *texture;
  GraphicsPrimitivePacket *pGVar4;
  sdword sVar5;
  byte bVar6;
  uint uVar7;
  uint uVar8;
  GraphicsPrimitivePacketEaxCf5 GVar9;
  
  sVar5 = g_GraphicsBackendAccessState;
  LOCK();
  g_GraphicsBackendAccessState = 1;
  UNLOCK();
  if (sVar5 == 0) {
    GVar9 = GraphicsPrimitiveQueue_Begin(queue);
    while (pGVar4 = GVar9.packet, !GVar9.carry) {
      if (pGVar4->vertices[0].screenX < 0x7f0001) {
        if (pGVar4->vertices[0].screenX < -0x7f0000) {
          pGVar4->vertices[0].screenX = -0x7f0000;
        }
      }
      else {
        pGVar4->vertices[0].screenX = 0x7f0000;
      }
      if (pGVar4->vertices[1].screenX < 0x7f0001) {
        if (pGVar4->vertices[1].screenX < -0x7f0000) {
          pGVar4->vertices[1].screenX = -0x7f0000;
        }
      }
      else {
        pGVar4->vertices[1].screenX = 0x7f0000;
      }
      if (pGVar4->vertices[2].screenX < 0x7f0001) {
        if (pGVar4->vertices[2].screenX < -0x7f0000) {
          pGVar4->vertices[2].screenX = -0x7f0000;
        }
      }
      else {
        pGVar4->vertices[2].screenX = 0x7f0000;
      }
      if (pGVar4->vertices[0].screenY < 0x7f0001) {
        if (pGVar4->vertices[0].screenY < -0x7f0000) {
          pGVar4->vertices[0].screenY = -0x7f0000;
        }
      }
      else {
        pGVar4->vertices[0].screenY = 0x7f0000;
      }
      if (pGVar4->vertices[1].screenY < 0x7f0001) {
        if (pGVar4->vertices[1].screenY < -0x7f0000) {
          pGVar4->vertices[1].screenY = -0x7f0000;
        }
      }
      else {
        pGVar4->vertices[1].screenY = 0x7f0000;
      }
      if (pGVar4->vertices[2].screenY < 0x7f0001) {
        if (pGVar4->vertices[2].screenY < -0x7f0000) {
          pGVar4->vertices[2].screenY = -0x7f0000;
        }
      }
      else {
        pGVar4->vertices[2].screenY = 0x7f0000;
      }
      g_GlideVertex0ScreenX = (dword)(float)pGVar4->vertices[0].screenX;
      g_GlideVertex0ScreenY = (dword)(float)pGVar4->vertices[0].screenY;
      if ((float)g_GlideVertex0ScreenX != 0.0) {
        g_GlideVertex0ScreenX = g_GlideVertex0ScreenX + 0xfa000000;
      }
      if ((float)g_GlideVertex0ScreenY != 0.0) {
        g_GlideVertex0ScreenY = g_GlideVertex0ScreenY + 0xfa000000;
      }
      g_GlideVertex1ScreenX = (dword)(float)pGVar4->vertices[1].screenX;
      g_GlideVertex1ScreenY = (dword)(float)pGVar4->vertices[1].screenY;
      if ((float)g_GlideVertex1ScreenX != 0.0) {
        g_GlideVertex1ScreenX = g_GlideVertex1ScreenX + 0xfa000000;
      }
      if ((float)g_GlideVertex1ScreenY != 0.0) {
        g_GlideVertex1ScreenY = g_GlideVertex1ScreenY + 0xfa000000;
      }
      g_GlideVertex2ScreenX = (dword)(float)pGVar4->vertices[2].screenX;
      g_GlideVertex2ScreenY = (dword)(float)pGVar4->vertices[2].screenY;
      if ((float)g_GlideVertex2ScreenX != 0.0) {
        g_GlideVertex2ScreenX = g_GlideVertex2ScreenX + 0xfa000000;
      }
      if ((float)g_GlideVertex2ScreenY != 0.0) {
        g_GlideVertex2ScreenY = g_GlideVertex2ScreenY + 0xfa000000;
      }
      g_GlideVertex0DiffuseColor = pGVar4->vertices[0].diffuseColor;
      g_GlideVertex1DiffuseColor = pGVar4->vertices[1].diffuseColor;
      g_GlideVertex2DiffuseColor = pGVar4->vertices[2].diffuseColor;
      pGVar2 = pGVar4->textureEntry;
      if (pGVar2 != (GraphicsTextureSetEntry *)0x0) {
        uVar8 = pGVar2->widthLog2;
        uVar3 = pGVar2->heightLog2;
        uVar7 = uVar8;
        if (uVar8 < uVar3) {
          uVar7 = uVar3;
        }
        bVar6 = (char)uVar7 - (char)uVar8;
        pGVar1 = &pGVar4->vertices[0].textureU;
        *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
        pGVar1 = &pGVar4->vertices[1].textureU;
        *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
        pGVar1 = &pGVar4->vertices[2].textureU;
        *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
        bVar6 = (char)uVar7 - (char)uVar3;
        pGVar1 = &pGVar4->vertices[0].textureV;
        *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
        pGVar1 = &pGVar4->vertices[1].textureV;
        *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
        pGVar1 = &pGVar4->vertices[2].textureV;
        *pGVar1 = *pGVar1 >> (bVar6 & 0x1f);
      }
      g_GlideVertex0ReciprocalDepth = (dword)(1.0 / (float)pGVar4->vertices[0].depth);
      g_GlideVertex1ReciprocalDepth = (dword)(1.0 / (float)pGVar4->vertices[1].depth);
      g_GlideVertex2ReciprocalDepth = (dword)(1.0 / (float)pGVar4->vertices[2].depth);
      g_GlideVertex0PerspectiveScale = (dword)(4096.0 / ((float)pGVar4->vertices[0].depth + 4096.0))
      ;
      g_GlideVertex1PerspectiveScale = (dword)(4096.0 / ((float)pGVar4->vertices[1].depth + 4096.0))
      ;
      g_GlideVertex2PerspectiveScale = (dword)(4096.0 / ((float)pGVar4->vertices[2].depth + 4096.0))
      ;
      if ((float)g_GlideVertex0ReciprocalDepth != 0.0) {
        g_GlideVertex0ReciprocalDepth = g_GlideVertex0ReciprocalDepth + 0xf000000;
      }
      if ((float)g_GlideVertex1ReciprocalDepth != 0.0) {
        g_GlideVertex1ReciprocalDepth = g_GlideVertex1ReciprocalDepth + 0xf000000;
      }
      if ((float)g_GlideVertex2ReciprocalDepth != 0.0) {
        g_GlideVertex2ReciprocalDepth = g_GlideVertex2ReciprocalDepth + 0xf000000;
      }
      g_GlideVertex0ProjectedTextureU =
           (dword)((float)pGVar4->vertices[0].textureU * (float)g_GlideVertex0PerspectiveScale);
      g_GlideVertex0ProjectedTextureV =
           (dword)((float)pGVar4->vertices[0].textureV * (float)g_GlideVertex0PerspectiveScale);
      if ((float)g_GlideVertex0ProjectedTextureU != 0.0) {
        g_GlideVertex0ProjectedTextureU = g_GlideVertex0ProjectedTextureU + 0xfa000000;
      }
      if ((float)g_GlideVertex0ProjectedTextureV != 0.0) {
        g_GlideVertex0ProjectedTextureV = g_GlideVertex0ProjectedTextureV + 0xfa000000;
      }
      g_GlideVertex1ProjectedTextureU =
           (dword)((float)pGVar4->vertices[1].textureU * (float)g_GlideVertex1PerspectiveScale);
      g_GlideVertex1ProjectedTextureV =
           (dword)((float)pGVar4->vertices[1].textureV * (float)g_GlideVertex1PerspectiveScale);
      if ((float)g_GlideVertex1ProjectedTextureU != 0.0) {
        g_GlideVertex1ProjectedTextureU = g_GlideVertex1ProjectedTextureU + 0xfa000000;
      }
      if ((float)g_GlideVertex1ProjectedTextureV != 0.0) {
        g_GlideVertex1ProjectedTextureV = g_GlideVertex1ProjectedTextureV + 0xfa000000;
      }
      g_GlideVertex2ProjectedTextureU =
           (dword)((float)pGVar4->vertices[2].textureU * (float)g_GlideVertex2PerspectiveScale);
      g_GlideVertex2ProjectedTextureV =
           (dword)((float)pGVar4->vertices[2].textureV * (float)g_GlideVertex2PerspectiveScale);
      if ((float)g_GlideVertex2ProjectedTextureU != 0.0) {
        g_GlideVertex2ProjectedTextureU = g_GlideVertex2ProjectedTextureU + 0xfa000000;
      }
      if ((float)g_GlideVertex2ProjectedTextureV != 0.0) {
        g_GlideVertex2ProjectedTextureV = g_GlideVertex2ProjectedTextureV + 0xfa000000;
      }
      uVar8 = pGVar4->renderFlags;
      if (((uVar8 & 0x10000) == 0) || (pGVar4->textureEntry == (GraphicsTextureSetEntry *)0x0)) {
        if (g_GlideTexturingDisabledState != 0)
        goto Glide3_DrawPrimitiveQueue_ConfigureUntexturedColorAndAlphaCombine;
      }
      else {
        texture = pGVar4->textureEntry->texture;
        if (((int)texture->residentTmuIndex < 0) &&
           (Glide3_TextureResource_EnsureResident(texture), (int)texture->residentTmuIndex < 0)) {
Glide3_DrawPrimitiveQueue_ConfigureUntexturedColorAndAlphaCombine:
          (*g_GrColorCombine)(1,0,0,2,0);
          (*g_GrAlphaCombine)(1,0,0,2,0);
        }
        else {
          if (g_GlideBoundTexture != texture) {
            g_GlideBoundTexture = texture;
            (*g_GrTexSource)(texture->residentTmuIndex,texture->residentAddress,3,
                             &texture->glideInfo);
          }
          if (g_GlideTexturingDisabledState == 0) {
            (*g_GrColorCombine)(3,1,0,1,0);
            (*g_GrAlphaCombine)(3,1,0,1,0);
          }
        }
      }
      if ((uVar8 & 0x20000) == 0) {
        uVar8 = uVar8 & 0x7000;
      }
      else {
        uVar8 = 0x1000;
      }
      if (uVar8 == 0x2000) {
        if (g_GlideBlendModeState != 1) {
          (*g_GrAlphaBlendFunction)(4,4,4,0);
          g_GlideBlendModeState = 1;
        }
      }
      else if (uVar8 == 0) {
        if (g_GlideBlendModeState != 2) {
          (*g_GrAlphaBlendFunction)(4,0,4,0);
          g_GlideBlendModeState = 2;
        }
      }
      else if (g_GlideBlendModeState != 0) {
        (*g_GrAlphaBlendFunction)(1,5,4,0);
        g_GlideBlendModeState = 0;
      }
      if ((uVar8 == 0x2000) || (uVar8 == 0x1000)) {
        if (g_GlideDepthWriteEnabledState != 0) {
          (*g_GrDepthMask)(0);
          g_GlideDepthWriteEnabledState = 0;
        }
      }
      else if (g_GlideDepthWriteEnabledState == 0) {
        (*g_GrDepthMask)(1);
        g_GlideDepthWriteEnabledState = 1;
      }
      (*g_GrDrawTriangle)(&g_GlideVertex2ScreenX,&g_GlideVertex1ScreenX,&g_GlideVertex0ScreenX);
      g_PrimitiveDrawCallCount = g_PrimitiveDrawCallCount + 1;
      GVar9 = GraphicsPrimitiveQueue_Next(queue);
    }
    g_GraphicsBackendAccessState = 0;
  }
  return;
}


/* Address: 0x005802F0.
   Ownership: graphics/backend/glide.
   Purpose: Unregisters and frees Glide texture resources, frees metadata, and returns the owned source asset.
   Nonstandard internal contract: the generic wrapper mirrors set in EBX as well as pushing it on the stack; this
   implementation consumes the EBX copy.
   Local calls: Glide3_TextureResource_Release.
   Cross-module calls: GraphicsTextureSet_FreeMetadata [graphics/resources/texture].
*/
GraphicsTextureSourceAsset * __thandor_eax_preserve_ecx_edx
Glide3_TextureSet_DestroyBackend(GraphicsTextureSet *setRegisterMirror,GraphicsTextureSet *set)

{
  GraphicsTextureResource *texture;
  GraphicsTextureResource **ppGVar1;
  GraphicsTextureSourceAsset *pGVar2;
  int iVar3;
  dword dVar4;
  GraphicsTextureSetEntry *pGVar5;
  GraphicsTextureResource **ppGVar6;
  
  pGVar2 = (GraphicsTextureSourceAsset *)0x0;
  if (setRegisterMirror != (GraphicsTextureSet *)0x0) {
    dVar4 = setRegisterMirror->subresourceCount;
    pGVar5 = setRegisterMirror->entries;
    do {
      texture = pGVar5->texture;
      if (texture != (GraphicsTextureResource *)0x0) {
        iVar3 = 0x1000;
        ppGVar1 = g_GraphicsTextureSlots;
        do {
          ppGVar6 = ppGVar1;
          if (texture == *ppGVar6) break;
          iVar3 = iVar3 + -1;
          ppGVar1 = ppGVar6 + 1;
        } while (iVar3 != 0);
        *ppGVar6 = (GraphicsTextureResource *)0x0;
        Glide3_TextureResource_Release(texture);
        (*g_MemoryApi.free)(texture);
      }
      pGVar5 = pGVar5 + 1;
      dVar4 = dVar4 - 1;
    } while (dVar4 != 0);
    pGVar2 = GraphicsTextureSet_FreeMetadata(set);
  }
  return pGVar2;
}


/* Address: 0x00580470.
   Ownership: graphics/backend/glide.
   Purpose: Presents the current Glide framebuffer, swaps the tracked backend state around the buffer-swap calls,
   and serializes access through g_GraphicsBackendAccessState.
   Local calls: Glide3_Cursor_ComposeBeforePresent.
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_Framebuffer_Present(SoftwareFramebufferAccess *framebuffer)

{
  SoftwareFramebufferAccess *pSVar1;
  sdword sVar2;
  sdword sVar3;
  sdword sVar4;
  
  sVar4 = g_CursorCurrentDrawY;
  sVar3 = g_CursorCurrentDrawX;
  sVar2 = g_CursorCurrentVisibilityToken;
  pSVar1 = g_CursorSavedBackground;
  LOCK();
  g_CursorSavedBackground = g_CursorAlternateSavedBackground;
  UNLOCK();
  LOCK();
  g_CursorCurrentVisibilityToken = g_CursorAlternateVisibilityToken;
  UNLOCK();
  LOCK();
  g_CursorCurrentDrawX = g_CursorAlternateDrawX;
  UNLOCK();
  LOCK();
  g_CursorCurrentDrawY = g_CursorAlternateDrawY;
  UNLOCK();
  g_CursorAlternateSavedBackground = pSVar1;
  g_CursorAlternateVisibilityToken = sVar2;
  g_CursorAlternateDrawX = sVar3;
  g_CursorAlternateDrawY = sVar4;
  Glide3_Cursor_ComposeBeforePresent((IDirectDrawSurface3 *)0x1);
  sVar2 = g_GraphicsBackendAccessState;
  LOCK();
  g_GraphicsBackendAccessState = 1;
  UNLOCK();
  if (sVar2 == 0) {
    (*g_GrFinish)();
    (*g_GrBufferSwap)(1);
    g_GraphicsBackendAccessState = g_GraphicsBackendAccessState + -1;
  }
  sVar4 = g_CursorCurrentDrawY;
  sVar3 = g_CursorCurrentDrawX;
  sVar2 = g_CursorCurrentVisibilityToken;
  pSVar1 = g_CursorSavedBackground;
  LOCK();
  g_CursorSavedBackground = g_CursorAlternateSavedBackground;
  UNLOCK();
  LOCK();
  g_CursorCurrentVisibilityToken = g_CursorAlternateVisibilityToken;
  UNLOCK();
  LOCK();
  g_CursorCurrentDrawX = g_CursorAlternateDrawX;
  UNLOCK();
  LOCK();
  g_CursorCurrentDrawY = g_CursorAlternateDrawY;
  UNLOCK();
  g_CursorAlternateSavedBackground = pSVar1;
  g_CursorAlternateVisibilityToken = sVar2;
  g_CursorAlternateDrawX = sVar3;
  g_CursorAlternateDrawY = sVar4;
  return;
}


/* Address: 0x0057EE90.
   Ownership: graphics/backend/glide.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code.
   Cross-module calls: DynDLL_Load [platform/bootstrap/runtime], DynAPI_Resolve [platform/bootstrap/runtime],
   DynDLL_Unload [platform/bootstrap/runtime], Text_CopyNarrowToUtf16Cf [core/text/string].
*/
bool __thandor_void_preserve_ecx_edx Glide3_InitAndEnumerate(void)

{
  void **ppvVar1;
  byte *source;
  byte *source_00;
  dword dVar2;
  int *output;
  FrontendDisplayDimensionPixels FVar3;
  uint uVar4;
  FrontendDisplayDimensionPixels FVar5;
  int *piVar6;
  void **destination;
  GraphicsAdapterRecord *pGVar7;
  GraphicsDisplayMode *pGVar8;
  DynDllLoadEaxCf5 DVar9;
  DynApiResolveEaxCf5 DVar10;
  ArenaAllocEaxCf5 AVar11;
  dword local_20;
  int local_1c;
  
  DVar9 = DynDLL_Load(dynapi_5);
  if (DVar9.carry) {
    return true;
  }
  destination = (void **)&g_GrAADrawTriangle;
  do {
    DVar10 = DynAPI_Resolve(destination,DVar9.moduleOrError,destination[1]);
    if (DVar10.carry) {
      DynDLL_Unload(dynapi_5);
      return true;
    }
    ppvVar1 = destination + 3;
    destination = destination + 2;
  } while (*ppvVar1 != (void *)0x0);
  (*g_GrGet)(0xf,4,&local_1c);
  if (local_1c != 0) {
    local_20 = 0;
    do {
      if (0xf < g_GraphicsAdapterCount) break;
      (*g_GrGlideInit)();
      (*g_GrSstSelect)(local_20);
      source = (byte *)(*g_GrGetString)(0xa2);
      source_00 = (byte *)(*g_GrGetString)(0xa1);
      pGVar7 = g_GraphicsAdapters + g_GraphicsAdapterCount;
      Text_CopyNarrowToUtf16Cf(0x28,pGVar7->driverDescriptionUtf16,source_00);
      Text_CopyNarrowToUtf16Cf(0x28,pGVar7->deviceNameUtf16,source);
      (pGVar7->adapterGuid).Data1 = 1;
      (pGVar7->adapterGuid).Data2 = (undefined2)local_20;
      (pGVar7->adapterGuid).Data3 = local_20._2_2_;
      (pGVar7->deviceGuid).Data1 = 1;
      dVar2 = (*g_GrQueryResolutions)(&g_GlideEnumerationResolutionQuery,(void *)0x0);
      if (dVar2 != 0) {
        AVar11 = (*g_MemoryApi.alloc)(dVar2);
        output = (int *)AVar11.eax;
        if (!AVar11.carry) {
          uVar4 = dVar2 >> 4;
          (*g_GrQueryResolutions)(&g_GlideEnumerationResolutionQuery,output);
          pGVar8 = g_GraphicsDisplayModes + g_GraphicsDisplayModeCount;
          piVar6 = output;
          do {
            if (0xff < g_GraphicsDisplayModeCount) break;
            FVar3 = 0x280;
            FVar5 = 0x1e0;
            if (*piVar6 == 7) {
Glide3_AppendEnumeratedDisplayMode:
              pGVar8->width = FVar3;
              pGVar8->height = FVar5;
              dVar2 = g_GraphicsAdapterCount;
              pGVar8->bitsPerPixel = 0x10;
              pGVar8->adapterIndex = dVar2;
              g_GraphicsDisplayModeCount = g_GraphicsDisplayModeCount + 1;
              pGVar8 = pGVar8 + 1;
            }
            else {
              FVar3 = 800;
              FVar5 = 600;
              if (*piVar6 == 8) goto Glide3_AppendEnumeratedDisplayMode;
              FVar3 = 0x3c0;
              FVar5 = 0x2d0;
              if (*piVar6 == 9) goto Glide3_AppendEnumeratedDisplayMode;
              FVar3 = 0x400;
              FVar5 = 0x300;
              if (*piVar6 == 0xc) goto Glide3_AppendEnumeratedDisplayMode;
              FVar3 = 0x500;
              FVar5 = 0x400;
              if (*piVar6 == 0xd) goto Glide3_AppendEnumeratedDisplayMode;
              FVar3 = 0x640;
              FVar5 = 0x4b0;
              if (*piVar6 == 0xe) goto Glide3_AppendEnumeratedDisplayMode;
            }
            piVar6 = piVar6 + 4;
            uVar4 = uVar4 - 1;
          } while (uVar4 != 0);
          (*g_MemoryApi.free)(output);
        }
      }
      (*g_GrGlideShutdown)();
      g_GraphicsAdapterCount = g_GraphicsAdapterCount + 1;
      local_20 = local_20 + 1;
      local_1c = local_1c + -1;
    } while (local_1c != 0);
  }
  DynDLL_Unload(dynapi_5);
  return false;
}


/* Address: 0x0057F0D0.
   Ownership: graphics/backend/glide.
   Purpose: Glide backend begin-scene wrapper. It saves and restores EAX, EBX, ECX, EDX, EDI, and ESI, performs no
   operation, and preserves incoming flags.
*/
void __thandor_void_preserve_eax_ecx_edx GlideBackend_BeginSceneNoOp(void)

{
  return;
}


/* Address: 0x0057F0E0.
   Ownership: graphics/backend/glide.
   Purpose: Glide backend end-scene wrapper recovered from a missed function slot. It saves and restores EAX, EBX,
   ECX, EDX, EDI, and ESI, performs no operation, and preserves incoming flags.
*/
void __thandor_void_preserve_eax_ecx_edx GlideBackend_EndSceneNoOp(void)

{
  return;
}


/* Address: 0x0057F740.
   Ownership: graphics/backend/glide.
   Purpose: Clears one Glide viewport rectangle. Typed parameters: p0 coordinate0→GraphicsScreenCoordinate_V307, p1
   coordinate1→GraphicsScreenCoordinate_V307, p2 coordinate2→GraphicsScreenCoordinate_V307, p3
   coordinate3→GraphicsScreenCoordinate_V307. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_ClearViewport
          (GraphicsScreenCoordinate coordinate0,GraphicsScreenCoordinate coordinate1,
          GraphicsScreenCoordinate coordinate2,GraphicsScreenCoordinate coordinate3)

{
  if (g_GlideDepthWriteEnabledState == 0) {
    (*g_GrDepthMask)(1);
    g_GlideDepthWriteEnabledState = 1;
  }
  (*g_GrViewport)(coordinate3,coordinate2,coordinate1 - coordinate3,coordinate0 - coordinate2);
  (*g_GrClipWindow)(coordinate3,coordinate2,coordinate1,coordinate0);
  (*g_GrBufferClear)(0,0,0);
  return;
}


/* Address: 0x00580370.
   Ownership: graphics/backend/glide.
   Purpose: Uploads one Glide color texture and refreshes its device state. Nonstandard internal contract:
   subresourceIndex is mirrored in ECX and on the stack. The implementation consumes ECX and reads set from the
   second stack argument.
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_TextureSet_RefreshColor(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  GraphicsTextureResource *arg0;
  GraphicsTextureResource *textureResource;
  
  arg0 = set->entries[subresourceIndex].texture;
  (*g_GlideTextureColorUpload[arg0->downsampleShift])(arg0);
  if (-1 < (int)arg0->residentTmuIndex) {
    (*g_GrTexDownloadMipMap)(arg0->residentTmuIndex,arg0->residentAddress,3,&arg0->glideInfo);
    g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
  }
  return;
}


/* Address: 0x005803D0.
   Ownership: graphics/backend/glide.
   Purpose: Uploads one Glide alpha texture and refreshes its device state. Nonstandard internal contract:
   subresourceIndex is mirrored in ECX and on the stack. The implementation consumes ECX and reads set from the
   second stack argument.
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_TextureSet_RefreshAlpha(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  GraphicsTextureResource *arg0;
  GraphicsTextureResource *textureResource;
  
  arg0 = set->entries[subresourceIndex].texture;
  (*g_GlideTextureAlphaUpload[arg0->downsampleShift])(arg0);
  if (-1 < (int)arg0->residentTmuIndex) {
    (*g_GrTexDownloadMipMap)(arg0->residentTmuIndex,arg0->residentAddress,3,&arg0->glideInfo);
    g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
  }
  return;
}


/* Address: 0x00580540.
   Ownership: graphics/backend/glide.
   Purpose: Captures Glide RGB565 data and converts it to opaque ARGB8888. Returns
   GraphicsCapturedTextureSourceAsset with a fixed 0x200-byte header, sourceEntry at 0x200, and argb8888Pixels at
   0x220. sourceEntry uses paletteIndex=-1, dataOffset=0x220, originX=originY=0, and logical/pixel dimensions equal
   to the capture dimensions. ABI: CF clear means success. CF set means failure.
*/
GraphicsFramebufferCaptureEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Glide3_Framebuffer_CaptureRegion
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX)

{
  ushort uVar1;
  GraphicsCapturedTextureSourceAsset *pGVar2;
  dword dVar3;
  int iVar4;
  AssetAllocationSizeBytes extraout_EDX;
  ushort *puVar5;
  GraphicsCapturedTextureSourceAsset *pGVar6;
  dword *pdVar7;
  GraphicsFramebufferCaptureEaxCf5 GVar8;
  dword buffer;
  GraphicsPixelDimension width;
  GraphicsPixelDimension height;
  ushort *destinationPixels;
  
  iVar4 = captureWidth * captureHeight;
  GVar8 = (GraphicsFramebufferCaptureEaxCf5)(*g_MemoryApi.alloc)(iVar4 * 4 + 0x220);
  pGVar2 = GVar8.eax;
  if (!GVar8.carry) {
    destinationPixels = (ushort *)((int)pGVar2->argb8888Pixels + iVar4 * 2);
    dVar3 = captureWidth * 2;
    pGVar6 = pGVar2;
    for (iVar4 = iVar4 + 0x88; iVar4 != 0; iVar4 = iVar4 + -1) {
      (pGVar6->common).magic = 0;
      pGVar6 = (GraphicsCapturedTextureSourceAsset *)&(pGVar6->common).allocationSizeBytes;
    }
    buffer = 1;
    width = captureWidth;
    height = captureHeight;
    puVar5 = destinationPixels;
    (*g_GrFinish)();
    (*g_GrLfbReadRegion)(buffer,sourceX,sourceY,width,height,dVar3,destinationPixels);
    (pGVar2->common).magic = ASSET_MAGIC_GFX;
    (pGVar2->common).allocationSizeBytes = extraout_EDX;
    (pGVar2->common).formatVersion = 1;
    (pGVar2->common).converterVersion = 0;
    dVar3 = (*g_LocaleGetPackedCurrentTime)();
    (pGVar2->common).buildMetadata.timestamps.dateValue0 = dVar3;
    (pGVar2->common).buildMetadata.timestamps.dateValue1 = dVar3;
    (pGVar2->common).buildMetadata.timestamps.dateValue2 = dVar3;
    dVar3 = (*g_LocaleGetPackedCurrentDate)();
    (pGVar2->common).buildMetadata.timestamps.timeValue0 = dVar3;
    (pGVar2->common).buildMetadata.timestamps.timeValue1 = dVar3;
    (pGVar2->common).buildMetadata.timestamps.timeValue2 = dVar3;
    (*g_LocaleCopyDefaultComputerLabelUtf16)((pGVar2->common).buildMetadata.names.producerName);
    (*g_LocaleCopyDefaultComputerLabelUtf16)((pGVar2->common).buildMetadata.names.sourceName);
    pGVar2->opaqueTablePayloadBC_1FF[0x44] = 0;
    (pGVar2->tableDescriptor).subresourceCount = 1;
    (pGVar2->tableDescriptor).paletteBankCount = 0;
    (pGVar2->tableDescriptor).subresourceTableOffset = 0x200;
    (pGVar2->sourceEntry).logicalWidth = captureWidth;
    (pGVar2->sourceEntry).logicalHeight = captureHeight;
    (pGVar2->sourceEntry).pixelWidth = captureWidth;
    (pGVar2->sourceEntry).pixelHeight = captureHeight;
    (pGVar2->sourceEntry).originX = 0;
    (pGVar2->sourceEntry).originY = 0;
    (pGVar2->sourceEntry).paletteIndex = -1;
    (pGVar2->sourceEntry).dataOffset = 0x220;
    pdVar7 = pGVar2->argb8888Pixels;
    iVar4 = captureWidth * captureHeight;
    do {
      uVar1 = *puVar5;
      *pdVar7 = (((((uVar1 >> 0xb | 0x1fe0) << 3 | (uint)(uVar1 >> 0xd)) << 6 | (uVar1 & 0x7ff) >> 5
                  ) << 2 | (uVar1 & 0x7ff) >> 9) << 5 | uVar1 & 0x1f) << 3 | (uVar1 & 0x1f) >> 2;
      puVar5 = puVar5 + 1;
      pdVar7 = pdVar7 + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    GVar8 = (GraphicsFramebufferCaptureEaxCf5)((uint5)GVar8 & 0xffffffff);
  }
  return GVar8;
}


/* Address: 0x005806E0.
   Ownership: graphics/backend/glide.
   Purpose: Glide cursor restoration no-op. The Glide pre-present path draws directly into the locked framebuffer
   and does not use a saved DirectDraw surface background.
*/
void Glide3_Cursor_RestoreAfterPresentNoOp(IDirectDrawSurface3 *backSurfaceSentinel)

{
  return;
}

/* Address: 0x005807B0.
   Ownership: graphics/backend/glide.
   Purpose: Converts the selected gfx subresource into the two-byte Glide CPU upload buffer. Indexed entries read
   ARGB8888 colors from their 256-entry palette bank; direct-color entries read ARGB8888 pixels. glideInfo.format
   0x0A selects RGB565-style packing and 0x0C selects ARGB4444-style packing. This variant preserves the source
   dimensions.
*/
void __thandor_void_preserve_eax_ecx_edx Glide3_TextureUpload_1x(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *pGVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  byte *pbVar6;
  ushort *puVar7;
  int iStack_28;
  int iStack_24;
  
  pGVar1 = texture->sourceAsset;
  puVar7 = (texture->glideInfo).data;
  iVar5 = texture->subresourceIndex * 0x20 + (pGVar1->tableDescriptor).subresourceTableOffset;
  iVar2 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x10);
  iStack_24 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0xc);
  iVar3 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x20);
  pbVar6 = (pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
           *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar5 + -0x1c) +
           -0x28;
  iStack_28 = iVar2;
  if (iVar3 < 0) {
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          uVar4 = *(uint *)pbVar6;
          *puVar7 = (ushort)((ushort)(((uVar4 >> 3 & 0x1f) << 0x15) >> 0x10) |
                            (ushort)(((uVar4 >> 10) << 0x1a) >> 0x10)) >> 5 |
                    (ushort)(((uVar4 >> 0x13) << 0x1b) >> 0x10);
          pbVar6 = pbVar6 + 4;
          puVar7 = puVar7 + 1;
          iStack_28 = iStack_28 + -1;
        } while (iStack_28 != 0);
        iStack_24 = iStack_24 + -1;
        iStack_28 = iVar2;
      } while (iStack_24 != 0);
    }
    else {
      do {
        do {
          uVar4 = *(uint *)pbVar6;
          *puVar7 = (ushort)((ushort)((ushort)(((uVar4 >> 4 & 0xf) << 0x18) >> 0x10) |
                                     (ushort)(((uVar4 >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                            (ushort)(((uVar4 >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                    (ushort)(uVar4 >> 0x10) & 0xf000;
          pbVar6 = pbVar6 + 4;
          puVar7 = puVar7 + 1;
          iStack_28 = iStack_28 + -1;
        } while (iStack_28 != 0);
        iStack_24 = iStack_24 + -1;
        iStack_28 = iVar2;
      } while (iStack_24 != 0);
    }
  }
  else {
    pGVar1 = texture->sourceAsset;
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          uVar4 = *(uint *)(pGVar1[iVar3 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28
                           + (uint)*pbVar6 * 8 + -0x28);
          *puVar7 = (ushort)((ushort)(((uVar4 >> 3) << 0x1b) >> 0x16) |
                            (ushort)(((uVar4 >> 10) << 0x1a) >> 0x10)) >> 5 |
                    (ushort)(((uVar4 >> 0x13) << 0x1b) >> 0x10);
          pbVar6 = pbVar6 + 1;
          puVar7 = puVar7 + 1;
          iStack_28 = iStack_28 + -1;
        } while (iStack_28 != 0);
        iStack_24 = iStack_24 + -1;
        iStack_28 = iVar2;
      } while (iStack_24 != 0);
    }
    else {
      do {
        do {
          uVar4 = *(uint *)(pGVar1[iVar3 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28
                           + (uint)*pbVar6 * 8 + -0x28);
          *puVar7 = (ushort)((ushort)((ushort)(((uVar4 >> 4) << 0x1c) >> 0x14) |
                                     (ushort)(((uVar4 >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                            (ushort)(((uVar4 >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                    (ushort)(uVar4 >> 0x10) & 0xf000;
          pbVar6 = pbVar6 + 1;
          puVar7 = puVar7 + 1;
          iStack_28 = iStack_28 + -1;
        } while (iStack_28 != 0);
        iStack_24 = iStack_24 + -1;
        iStack_28 = iVar2;
      } while (iStack_24 != 0);
    }
  }
  return;
}


/* Address: 0x00580960.
   Ownership: graphics/backend/glide.
   Purpose: Converts the selected gfx subresource into the two-byte Glide CPU upload buffer. Indexed entries read
   ARGB8888 colors from their 256-entry palette bank; direct-color entries read ARGB8888 pixels. glideInfo.format
   0x0A selects RGB565-style packing and 0x0C selects ARGB4444-style packing. This variant reduces each dimension
   by 2 and averages each 2x2 source block before packing.
*/
void __thandor_void_preserve_eax_ecx_edx Glide3_TextureUpload_2x(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *pGVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  byte bVar7;
  ushort uVar8;
  uint uVar9;
  int iVar10;
  AssetProducerSourceNames *pAVar11;
  ushort *puVar12;
  undefined1 uVar17;
  ushort uVar13;
  ushort uVar14;
  undefined1 uVar18;
  uint3 uVar15;
  ushort uVar19;
  ushort uVar20;
  uint uVar16;
  ushort uVar21;
  ushort uVar22;
  undefined1 uVar23;
  undefined1 uVar24;
  undefined1 uVar25;
  undefined1 uVar26;
  undefined1 uVar27;
  undefined1 uVar28;
  uint uStack_28;
  uint uStack_24;
  
  pGVar1 = texture->sourceAsset;
  puVar12 = (texture->glideInfo).data;
  iVar10 = texture->subresourceIndex * 0x20 + (pGVar1->tableDescriptor).subresourceTableOffset;
  iVar2 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar10 + -0x20);
  pAVar11 = (AssetProducerSourceNames *)
            ((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
            *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar10 + -0x1c) +
            -0x28);
  uVar9 = *(uint *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar10 + -0x10) >>
          1;
  uStack_24 = *(uint *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar10 + -0xc)
              >> 1;
  uStack_28 = uVar9;
  if (iVar2 < 0) {
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          uVar3 = *(undefined4 *)pAVar11->producerName;
          uVar4 = *(undefined4 *)((int)pAVar11->producerName + 4);
          uVar5 = *(undefined4 *)((int)pAVar11->producerName + uVar9 * 8);
          uVar6 = *(undefined4 *)((int)pAVar11->producerName + uVar9 * 8 + 4);
          uVar18 = (undefined1)((uint)uVar3 >> 0x10);
          uVar17 = (undefined1)((uint)uVar3 >> 8);
          uVar24 = (undefined1)((uint)uVar4 >> 0x10);
          uVar23 = (undefined1)((uint)uVar4 >> 8);
          uVar26 = (undefined1)((uint)uVar5 >> 0x10);
          uVar25 = (undefined1)((uint)uVar5 >> 8);
          uVar28 = (undefined1)((uint)uVar6 >> 0x10);
          uVar27 = (undefined1)((uint)uVar6 >> 8);
          uVar13 = (ushort)((CONCAT11((char)uVar3,(char)uVar3) >> 4) +
                            (CONCAT11((char)uVar4,(char)uVar4) >> 4) +
                           (CONCAT11((char)uVar5,(char)uVar5) >> 4) +
                           (CONCAT11((char)uVar6,(char)uVar6) >> 4)) >> 6;
          uVar19 = (ushort)((CONCAT11(uVar17,uVar17) >> 4) + (CONCAT11(uVar23,uVar23) >> 4) +
                           (CONCAT11(uVar25,uVar25) >> 4) + (CONCAT11(uVar27,uVar27) >> 4)) >> 6;
          uVar21 = (ushort)((ushort)(CONCAT15(uVar18,CONCAT14(uVar18,uVar3)) >> 0x24) +
                            (ushort)(CONCAT15(uVar24,CONCAT14(uVar24,uVar4)) >> 0x24) +
                           (ushort)(CONCAT15(uVar26,CONCAT14(uVar26,uVar5)) >> 0x24) +
                           (ushort)(CONCAT15(uVar28,CONCAT14(uVar28,uVar6)) >> 0x24)) >> 6;
          bVar7 = (uVar21 != 0) * (uVar21 < 0x100) * (char)uVar21 - (0xff < uVar21);
          uVar15 = CONCAT12(bVar7,CONCAT11((uVar19 != 0) * (uVar19 < 0x100) * (char)uVar19 -
                                           (0xff < uVar19),
                                           (uVar13 != 0) * (uVar13 < 0x100) * (char)uVar13 -
                                           (0xff < uVar13)));
          *puVar12 = (ushort)((ushort)(((uVar15 >> 3 & 0x1f) << 0x15) >> 0x10) |
                             (ushort)(((uint)(uVar15 >> 10) << 0x1a) >> 0x10)) >> 5 |
                     (ushort)(((uint)(bVar7 >> 3) << 0x1b) >> 0x10);
          pAVar11 = (AssetProducerSourceNames *)((int)pAVar11->producerName + 8);
          puVar12 = puVar12 + 1;
          uStack_28 = uStack_28 - 1;
        } while (uStack_28 != 0);
        pAVar11 = (AssetProducerSourceNames *)(pAVar11->producerName + uVar9 * 4);
        uStack_24 = uStack_24 - 1;
        uStack_28 = uVar9;
      } while (uStack_24 != 0);
    }
    else {
      do {
        do {
          uVar3 = *(undefined4 *)pAVar11->producerName;
          uVar4 = *(undefined4 *)(pAVar11->producerName + 2);
          uVar5 = *(undefined4 *)(pAVar11->producerName + uVar9 * 4);
          uVar6 = *(undefined4 *)(pAVar11->producerName + uVar9 * 4 + 2);
          uVar17 = (undefined1)((uint)uVar3 >> 0x18);
          uVar13 = CONCAT11(uVar17,uVar17);
          uVar18 = (undefined1)((uint)uVar3 >> 0x10);
          uVar17 = (undefined1)((uint)uVar3 >> 8);
          uVar23 = (undefined1)((uint)uVar4 >> 0x18);
          uVar19 = CONCAT11(uVar23,uVar23);
          uVar24 = (undefined1)((uint)uVar4 >> 0x10);
          uVar23 = (undefined1)((uint)uVar4 >> 8);
          uVar25 = (undefined1)((uint)uVar5 >> 0x18);
          uVar21 = CONCAT11(uVar25,uVar25);
          uVar26 = (undefined1)((uint)uVar5 >> 0x10);
          uVar25 = (undefined1)((uint)uVar5 >> 8);
          uVar27 = (undefined1)((uint)uVar6 >> 0x18);
          uVar8 = CONCAT11(uVar27,uVar27);
          uVar28 = (undefined1)((uint)uVar6 >> 0x10);
          uVar27 = (undefined1)((uint)uVar6 >> 8);
          uVar14 = (ushort)((CONCAT11((char)uVar3,(char)uVar3) >> 4) +
                            (CONCAT11((char)uVar4,(char)uVar4) >> 4) +
                           (CONCAT11((char)uVar5,(char)uVar5) >> 4) +
                           (CONCAT11((char)uVar6,(char)uVar6) >> 4)) >> 6;
          uVar20 = (ushort)((CONCAT11(uVar17,uVar17) >> 4) + (CONCAT11(uVar23,uVar23) >> 4) +
                           (CONCAT11(uVar25,uVar25) >> 4) + (CONCAT11(uVar27,uVar27) >> 4)) >> 6;
          uVar22 = (ushort)(((ushort)(CONCAT35(CONCAT21(uVar13,uVar18),CONCAT14(uVar18,uVar3)) >>
                                     0x20) >> 4) +
                            ((ushort)(CONCAT35(CONCAT21(uVar19,uVar24),CONCAT14(uVar24,uVar4)) >>
                                     0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(uVar21,uVar26),CONCAT14(uVar26,uVar5)) >>
                                    0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(uVar8,uVar28),CONCAT14(uVar28,uVar6)) >> 0x20
                                    ) >> 4)) >> 6;
          uVar13 = (ushort)((uVar13 >> 4) + (uVar19 >> 4) + (uVar21 >> 4) + (uVar8 >> 4)) >> 6;
          uVar15 = CONCAT12((uVar22 != 0) * (uVar22 < 0x100) * (char)uVar22 - (0xff < uVar22),
                            CONCAT11((uVar20 != 0) * (uVar20 < 0x100) * (char)uVar20 -
                                     (0xff < uVar20),
                                     (uVar14 != 0) * (uVar14 < 0x100) * (char)uVar14 -
                                     (0xff < uVar14)));
          bVar7 = (uVar13 != 0) * (uVar13 < 0x100) * (char)uVar13 - (0xff < uVar13);
          uVar16 = CONCAT13(bVar7,uVar15);
          *puVar12 = (ushort)((ushort)((ushort)((((uVar15 & 0xf0) >> 4) << 0x18) >> 0x10) |
                                      (ushort)(((uVar16 >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                             (ushort)(((uVar16 >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                     (ushort)(((uint)(bVar7 >> 4) << 0x1c) >> 0x10);
          pAVar11 = (AssetProducerSourceNames *)(pAVar11->producerName + 4);
          puVar12 = puVar12 + 1;
          uStack_28 = uStack_28 - 1;
        } while (uStack_28 != 0);
        pAVar11 = (AssetProducerSourceNames *)(pAVar11->producerName + uVar9 * 4);
        uStack_24 = uStack_24 - 1;
        uStack_28 = uVar9;
      } while (uStack_24 != 0);
    }
  }
  else {
    pGVar1 = texture->sourceAsset;
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          uVar3 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)pAVar11->producerName[0] * 8 + -0x28);
          uVar4 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)*(byte *)((int)pAVar11->producerName + 1) * 8 + -0x28);
          uVar5 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)pAVar11->producerName[uVar9] * 8 + -0x28);
          uVar6 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)*(byte *)((int)pAVar11->producerName + uVar9 * 2 + 1) * 8 + -0x28);
          uVar18 = (undefined1)((uint)uVar3 >> 0x10);
          uVar17 = (undefined1)((uint)uVar3 >> 8);
          uVar24 = (undefined1)((uint)uVar4 >> 0x10);
          uVar23 = (undefined1)((uint)uVar4 >> 8);
          uVar26 = (undefined1)((uint)uVar5 >> 0x10);
          uVar25 = (undefined1)((uint)uVar5 >> 8);
          uVar28 = (undefined1)((uint)uVar6 >> 0x10);
          uVar27 = (undefined1)((uint)uVar6 >> 8);
          uVar13 = (ushort)((CONCAT11((char)uVar3,(char)uVar3) >> 4) +
                            (CONCAT11((char)uVar4,(char)uVar4) >> 4) +
                           (CONCAT11((char)uVar5,(char)uVar5) >> 4) +
                           (CONCAT11((char)uVar6,(char)uVar6) >> 4)) >> 6;
          uVar19 = (ushort)((CONCAT11(uVar17,uVar17) >> 4) + (CONCAT11(uVar23,uVar23) >> 4) +
                           (CONCAT11(uVar25,uVar25) >> 4) + (CONCAT11(uVar27,uVar27) >> 4)) >> 6;
          uVar21 = (ushort)((ushort)(CONCAT15(uVar18,CONCAT14(uVar18,uVar3)) >> 0x24) +
                            (ushort)(CONCAT15(uVar24,CONCAT14(uVar24,uVar4)) >> 0x24) +
                           (ushort)(CONCAT15(uVar26,CONCAT14(uVar26,uVar5)) >> 0x24) +
                           (ushort)(CONCAT15(uVar28,CONCAT14(uVar28,uVar6)) >> 0x24)) >> 6;
          bVar7 = (uVar21 != 0) * (uVar21 < 0x100) * (char)uVar21 - (0xff < uVar21);
          uVar15 = CONCAT12(bVar7,CONCAT11((uVar19 != 0) * (uVar19 < 0x100) * (char)uVar19 -
                                           (0xff < uVar19),
                                           (uVar13 != 0) * (uVar13 < 0x100) * (char)uVar13 -
                                           (0xff < uVar13)));
          *puVar12 = (ushort)((ushort)(((uint)(uVar15 >> 3) << 0x1b) >> 0x16) |
                             (ushort)(((uint)(uVar15 >> 10) << 0x1a) >> 0x10)) >> 5 |
                     (ushort)(((uint)(bVar7 >> 3) << 0x1b) >> 0x10);
          pAVar11 = (AssetProducerSourceNames *)(pAVar11->producerName + 1);
          puVar12 = puVar12 + 1;
          uStack_28 = uStack_28 - 1;
        } while (uStack_28 != 0);
        pAVar11 = (AssetProducerSourceNames *)(pAVar11->producerName + uVar9);
        uStack_24 = uStack_24 - 1;
        uStack_28 = uVar9;
      } while (uStack_24 != 0);
    }
    else {
      do {
        do {
          uVar3 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)pAVar11->producerName[0] * 8 + -0x28);
          uVar4 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)*(byte *)((int)pAVar11->producerName + 1) * 8 + -0x28);
          uVar5 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)pAVar11->producerName[uVar9] * 8 + -0x28);
          uVar6 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)*(byte *)((int)pAVar11->producerName + uVar9 * 2 + 1) * 8 + -0x28);
          uVar17 = (undefined1)((uint)uVar3 >> 0x18);
          uVar13 = CONCAT11(uVar17,uVar17);
          uVar18 = (undefined1)((uint)uVar3 >> 0x10);
          uVar17 = (undefined1)((uint)uVar3 >> 8);
          uVar23 = (undefined1)((uint)uVar4 >> 0x18);
          uVar19 = CONCAT11(uVar23,uVar23);
          uVar24 = (undefined1)((uint)uVar4 >> 0x10);
          uVar23 = (undefined1)((uint)uVar4 >> 8);
          uVar25 = (undefined1)((uint)uVar5 >> 0x18);
          uVar21 = CONCAT11(uVar25,uVar25);
          uVar26 = (undefined1)((uint)uVar5 >> 0x10);
          uVar25 = (undefined1)((uint)uVar5 >> 8);
          uVar27 = (undefined1)((uint)uVar6 >> 0x18);
          uVar8 = CONCAT11(uVar27,uVar27);
          uVar28 = (undefined1)((uint)uVar6 >> 0x10);
          uVar27 = (undefined1)((uint)uVar6 >> 8);
          uVar14 = (ushort)((CONCAT11((char)uVar3,(char)uVar3) >> 4) +
                            (CONCAT11((char)uVar4,(char)uVar4) >> 4) +
                           (CONCAT11((char)uVar5,(char)uVar5) >> 4) +
                           (CONCAT11((char)uVar6,(char)uVar6) >> 4)) >> 6;
          uVar20 = (ushort)((CONCAT11(uVar17,uVar17) >> 4) + (CONCAT11(uVar23,uVar23) >> 4) +
                           (CONCAT11(uVar25,uVar25) >> 4) + (CONCAT11(uVar27,uVar27) >> 4)) >> 6;
          uVar22 = (ushort)(((ushort)(CONCAT35(CONCAT21(uVar13,uVar18),CONCAT14(uVar18,uVar3)) >>
                                     0x20) >> 4) +
                            ((ushort)(CONCAT35(CONCAT21(uVar19,uVar24),CONCAT14(uVar24,uVar4)) >>
                                     0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(uVar21,uVar26),CONCAT14(uVar26,uVar5)) >>
                                    0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(uVar8,uVar28),CONCAT14(uVar28,uVar6)) >> 0x20
                                    ) >> 4)) >> 6;
          uVar13 = (ushort)((uVar13 >> 4) + (uVar19 >> 4) + (uVar21 >> 4) + (uVar8 >> 4)) >> 6;
          bVar7 = (uVar13 != 0) * (uVar13 < 0x100) * (char)uVar13 - (0xff < uVar13);
          uVar16 = CONCAT13(bVar7,CONCAT12((uVar22 != 0) * (uVar22 < 0x100) * (char)uVar22 -
                                           (0xff < uVar22),
                                           CONCAT11((uVar20 != 0) * (uVar20 < 0x100) * (char)uVar20
                                                    - (0xff < uVar20),
                                                    (uVar14 != 0) * (uVar14 < 0x100) * (char)uVar14
                                                    - (0xff < uVar14))));
          *puVar12 = (ushort)((ushort)((ushort)(((uVar16 >> 4) << 0x1c) >> 0x14) |
                                      (ushort)(((uVar16 >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                             (ushort)(((uVar16 >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                     (ushort)(((uint)(bVar7 >> 4) << 0x1c) >> 0x10);
          pAVar11 = (AssetProducerSourceNames *)(pAVar11->producerName + 1);
          puVar12 = puVar12 + 1;
          uStack_28 = uStack_28 - 1;
        } while (uStack_28 != 0);
        pAVar11 = (AssetProducerSourceNames *)(pAVar11->producerName + uVar9);
        uStack_24 = uStack_24 - 1;
        uStack_28 = uVar9;
      } while (uStack_24 != 0);
    }
  }
  return;
}


/* Address: 0x00580C60.
   Ownership: graphics/backend/glide.
   Purpose: Converts the selected gfx subresource into the two-byte Glide CPU upload buffer. Indexed entries read
   ARGB8888 colors from their 256-entry palette bank; direct-color entries read ARGB8888 pixels. glideInfo.format
   0x0A selects RGB565-style packing and 0x0C selects ARGB4444-style packing. This variant reduces each dimension
   by 4 and averages each 4x4 source block before packing.
*/
void __thandor_void_preserve_eax_ecx_edx Glide3_TextureUpload_4x(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *pGVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  byte bVar7;
  ushort uVar8;
  uint uVar9;
  int iVar10;
  word *pwVar11;
  ushort *puVar12;
  undefined1 uVar17;
  ushort uVar13;
  ushort uVar14;
  undefined1 uVar18;
  uint3 uVar15;
  ushort uVar19;
  ushort uVar20;
  uint uVar16;
  ushort uVar21;
  ushort uVar22;
  undefined1 uVar23;
  undefined1 uVar24;
  undefined1 uVar25;
  undefined1 uVar26;
  undefined1 uVar27;
  undefined1 uVar28;
  uint uStack_28;
  uint uStack_24;
  
  pGVar1 = texture->sourceAsset;
  puVar12 = (texture->glideInfo).data;
  iVar10 = texture->subresourceIndex * 0x20 + (pGVar1->tableDescriptor).subresourceTableOffset;
  iVar2 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar10 + -0x20);
  pwVar11 = (word *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                    *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar10 + -0x1c) + -0x28);
  uVar9 = *(uint *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar10 + -0x10) >>
          2;
  uStack_24 = *(uint *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar10 + -0xc)
              >> 2;
  uStack_28 = uVar9;
  if (iVar2 < 0) {
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          uVar3 = *(undefined4 *)pwVar11;
          uVar4 = *(undefined4 *)((AssetProducerSourceNames *)(pwVar11 + 4))->producerName;
          uVar5 = *(undefined4 *)(pwVar11 + uVar9 * 0x10);
          uVar6 = *(undefined4 *)(pwVar11 + uVar9 * 0x10 + 4);
          uVar18 = (undefined1)((uint)uVar3 >> 0x10);
          uVar17 = (undefined1)((uint)uVar3 >> 8);
          uVar24 = (undefined1)((uint)uVar4 >> 0x10);
          uVar23 = (undefined1)((uint)uVar4 >> 8);
          uVar26 = (undefined1)((uint)uVar5 >> 0x10);
          uVar25 = (undefined1)((uint)uVar5 >> 8);
          uVar28 = (undefined1)((uint)uVar6 >> 0x10);
          uVar27 = (undefined1)((uint)uVar6 >> 8);
          uVar13 = (ushort)((CONCAT11((char)uVar3,(char)uVar3) >> 4) +
                            (CONCAT11((char)uVar4,(char)uVar4) >> 4) +
                           (CONCAT11((char)uVar5,(char)uVar5) >> 4) +
                           (CONCAT11((char)uVar6,(char)uVar6) >> 4)) >> 6;
          uVar19 = (ushort)((CONCAT11(uVar17,uVar17) >> 4) + (CONCAT11(uVar23,uVar23) >> 4) +
                           (CONCAT11(uVar25,uVar25) >> 4) + (CONCAT11(uVar27,uVar27) >> 4)) >> 6;
          uVar21 = (ushort)((ushort)(CONCAT15(uVar18,CONCAT14(uVar18,uVar3)) >> 0x24) +
                            (ushort)(CONCAT15(uVar24,CONCAT14(uVar24,uVar4)) >> 0x24) +
                           (ushort)(CONCAT15(uVar26,CONCAT14(uVar26,uVar5)) >> 0x24) +
                           (ushort)(CONCAT15(uVar28,CONCAT14(uVar28,uVar6)) >> 0x24)) >> 6;
          bVar7 = (uVar21 != 0) * (uVar21 < 0x100) * (char)uVar21 - (0xff < uVar21);
          uVar15 = CONCAT12(bVar7,CONCAT11((uVar19 != 0) * (uVar19 < 0x100) * (char)uVar19 -
                                           (0xff < uVar19),
                                           (uVar13 != 0) * (uVar13 < 0x100) * (char)uVar13 -
                                           (0xff < uVar13)));
          *puVar12 = (ushort)((ushort)(((uVar15 >> 3 & 0x1f) << 0x15) >> 0x10) |
                             (ushort)(((uint)(uVar15 >> 10) << 0x1a) >> 0x10)) >> 5 |
                     (ushort)(((uint)(bVar7 >> 3) << 0x1b) >> 0x10);
          pwVar11 = pwVar11 + 8;
          puVar12 = puVar12 + 1;
          uStack_28 = uStack_28 - 1;
        } while (uStack_28 != 0);
        pwVar11 = pwVar11 + uVar9 * 0x18;
        uStack_24 = uStack_24 - 1;
        uStack_28 = uVar9;
      } while (uStack_24 != 0);
    }
    else {
      do {
        do {
          uVar3 = *(undefined4 *)pwVar11;
          uVar4 = *(undefined4 *)(pwVar11 + 4);
          uVar5 = *(undefined4 *)(pwVar11 + uVar9 * 0x10);
          uVar6 = *(undefined4 *)(pwVar11 + uVar9 * 0x10 + 4);
          uVar17 = (undefined1)((uint)uVar3 >> 0x18);
          uVar13 = CONCAT11(uVar17,uVar17);
          uVar18 = (undefined1)((uint)uVar3 >> 0x10);
          uVar17 = (undefined1)((uint)uVar3 >> 8);
          uVar23 = (undefined1)((uint)uVar4 >> 0x18);
          uVar19 = CONCAT11(uVar23,uVar23);
          uVar24 = (undefined1)((uint)uVar4 >> 0x10);
          uVar23 = (undefined1)((uint)uVar4 >> 8);
          uVar25 = (undefined1)((uint)uVar5 >> 0x18);
          uVar21 = CONCAT11(uVar25,uVar25);
          uVar26 = (undefined1)((uint)uVar5 >> 0x10);
          uVar25 = (undefined1)((uint)uVar5 >> 8);
          uVar27 = (undefined1)((uint)uVar6 >> 0x18);
          uVar8 = CONCAT11(uVar27,uVar27);
          uVar28 = (undefined1)((uint)uVar6 >> 0x10);
          uVar27 = (undefined1)((uint)uVar6 >> 8);
          uVar14 = (ushort)((CONCAT11((char)uVar3,(char)uVar3) >> 4) +
                            (CONCAT11((char)uVar4,(char)uVar4) >> 4) +
                           (CONCAT11((char)uVar5,(char)uVar5) >> 4) +
                           (CONCAT11((char)uVar6,(char)uVar6) >> 4)) >> 6;
          uVar20 = (ushort)((CONCAT11(uVar17,uVar17) >> 4) + (CONCAT11(uVar23,uVar23) >> 4) +
                           (CONCAT11(uVar25,uVar25) >> 4) + (CONCAT11(uVar27,uVar27) >> 4)) >> 6;
          uVar22 = (ushort)(((ushort)(CONCAT35(CONCAT21(uVar13,uVar18),CONCAT14(uVar18,uVar3)) >>
                                     0x20) >> 4) +
                            ((ushort)(CONCAT35(CONCAT21(uVar19,uVar24),CONCAT14(uVar24,uVar4)) >>
                                     0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(uVar21,uVar26),CONCAT14(uVar26,uVar5)) >>
                                    0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(uVar8,uVar28),CONCAT14(uVar28,uVar6)) >> 0x20
                                    ) >> 4)) >> 6;
          uVar13 = (ushort)((uVar13 >> 4) + (uVar19 >> 4) + (uVar21 >> 4) + (uVar8 >> 4)) >> 6;
          uVar15 = CONCAT12((uVar22 != 0) * (uVar22 < 0x100) * (char)uVar22 - (0xff < uVar22),
                            CONCAT11((uVar20 != 0) * (uVar20 < 0x100) * (char)uVar20 -
                                     (0xff < uVar20),
                                     (uVar14 != 0) * (uVar14 < 0x100) * (char)uVar14 -
                                     (0xff < uVar14)));
          bVar7 = (uVar13 != 0) * (uVar13 < 0x100) * (char)uVar13 - (0xff < uVar13);
          uVar16 = CONCAT13(bVar7,uVar15);
          *puVar12 = (ushort)((ushort)((ushort)((((uVar15 & 0xf0) >> 4) << 0x18) >> 0x10) |
                                      (ushort)(((uVar16 >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                             (ushort)(((uVar16 >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                     (ushort)(((uint)(bVar7 >> 4) << 0x1c) >> 0x10);
          pwVar11 = pwVar11 + 8;
          puVar12 = puVar12 + 1;
          uStack_28 = uStack_28 - 1;
        } while (uStack_28 != 0);
        pwVar11 = pwVar11 + uVar9 * 0x18;
        uStack_24 = uStack_24 - 1;
        uStack_28 = uVar9;
      } while (uStack_24 != 0);
    }
  }
  else {
    pGVar1 = texture->sourceAsset;
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          uVar3 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)*pwVar11 * 8 + -0x28);
          uVar4 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)pwVar11[1] * 8 + -0x28);
          uVar5 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)pwVar11[uVar9 * 4] * 8 + -0x28);
          uVar6 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)pwVar11[uVar9 * 4 + 1] * 8 + -0x28);
          uVar18 = (undefined1)((uint)uVar3 >> 0x10);
          uVar17 = (undefined1)((uint)uVar3 >> 8);
          uVar24 = (undefined1)((uint)uVar4 >> 0x10);
          uVar23 = (undefined1)((uint)uVar4 >> 8);
          uVar26 = (undefined1)((uint)uVar5 >> 0x10);
          uVar25 = (undefined1)((uint)uVar5 >> 8);
          uVar28 = (undefined1)((uint)uVar6 >> 0x10);
          uVar27 = (undefined1)((uint)uVar6 >> 8);
          uVar13 = (ushort)((CONCAT11((char)uVar3,(char)uVar3) >> 4) +
                            (CONCAT11((char)uVar4,(char)uVar4) >> 4) +
                           (CONCAT11((char)uVar5,(char)uVar5) >> 4) +
                           (CONCAT11((char)uVar6,(char)uVar6) >> 4)) >> 6;
          uVar19 = (ushort)((CONCAT11(uVar17,uVar17) >> 4) + (CONCAT11(uVar23,uVar23) >> 4) +
                           (CONCAT11(uVar25,uVar25) >> 4) + (CONCAT11(uVar27,uVar27) >> 4)) >> 6;
          uVar21 = (ushort)((ushort)(CONCAT15(uVar18,CONCAT14(uVar18,uVar3)) >> 0x24) +
                            (ushort)(CONCAT15(uVar24,CONCAT14(uVar24,uVar4)) >> 0x24) +
                           (ushort)(CONCAT15(uVar26,CONCAT14(uVar26,uVar5)) >> 0x24) +
                           (ushort)(CONCAT15(uVar28,CONCAT14(uVar28,uVar6)) >> 0x24)) >> 6;
          bVar7 = (uVar21 != 0) * (uVar21 < 0x100) * (char)uVar21 - (0xff < uVar21);
          uVar15 = CONCAT12(bVar7,CONCAT11((uVar19 != 0) * (uVar19 < 0x100) * (char)uVar19 -
                                           (0xff < uVar19),
                                           (uVar13 != 0) * (uVar13 < 0x100) * (char)uVar13 -
                                           (0xff < uVar13)));
          *puVar12 = (ushort)((ushort)(((uint)(uVar15 >> 3) << 0x1b) >> 0x16) |
                             (ushort)(((uint)(uVar15 >> 10) << 0x1a) >> 0x10)) >> 5 |
                     (ushort)(((uint)(bVar7 >> 3) << 0x1b) >> 0x10);
          pwVar11 = pwVar11 + 2;
          puVar12 = puVar12 + 1;
          uStack_28 = uStack_28 - 1;
        } while (uStack_28 != 0);
        pwVar11 = pwVar11 + uVar9 * 6;
        uStack_24 = uStack_24 - 1;
        uStack_28 = uVar9;
      } while (uStack_24 != 0);
    }
    else {
      do {
        do {
          uVar3 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)*pwVar11 * 8 + -0x28);
          uVar4 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)pwVar11[1] * 8 + -0x28);
          uVar5 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)pwVar11[uVar9 * 4] * 8 + -0x28);
          uVar6 = *(undefined4 *)
                   (pGVar1[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)pwVar11[uVar9 * 4 + 1] * 8 + -0x28);
          uVar17 = (undefined1)((uint)uVar3 >> 0x18);
          uVar13 = CONCAT11(uVar17,uVar17);
          uVar18 = (undefined1)((uint)uVar3 >> 0x10);
          uVar17 = (undefined1)((uint)uVar3 >> 8);
          uVar23 = (undefined1)((uint)uVar4 >> 0x18);
          uVar19 = CONCAT11(uVar23,uVar23);
          uVar24 = (undefined1)((uint)uVar4 >> 0x10);
          uVar23 = (undefined1)((uint)uVar4 >> 8);
          uVar25 = (undefined1)((uint)uVar5 >> 0x18);
          uVar21 = CONCAT11(uVar25,uVar25);
          uVar26 = (undefined1)((uint)uVar5 >> 0x10);
          uVar25 = (undefined1)((uint)uVar5 >> 8);
          uVar27 = (undefined1)((uint)uVar6 >> 0x18);
          uVar8 = CONCAT11(uVar27,uVar27);
          uVar28 = (undefined1)((uint)uVar6 >> 0x10);
          uVar27 = (undefined1)((uint)uVar6 >> 8);
          uVar14 = (ushort)((CONCAT11((char)uVar3,(char)uVar3) >> 4) +
                            (CONCAT11((char)uVar4,(char)uVar4) >> 4) +
                           (CONCAT11((char)uVar5,(char)uVar5) >> 4) +
                           (CONCAT11((char)uVar6,(char)uVar6) >> 4)) >> 6;
          uVar20 = (ushort)((CONCAT11(uVar17,uVar17) >> 4) + (CONCAT11(uVar23,uVar23) >> 4) +
                           (CONCAT11(uVar25,uVar25) >> 4) + (CONCAT11(uVar27,uVar27) >> 4)) >> 6;
          uVar22 = (ushort)(((ushort)(CONCAT35(CONCAT21(uVar13,uVar18),CONCAT14(uVar18,uVar3)) >>
                                     0x20) >> 4) +
                            ((ushort)(CONCAT35(CONCAT21(uVar19,uVar24),CONCAT14(uVar24,uVar4)) >>
                                     0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(uVar21,uVar26),CONCAT14(uVar26,uVar5)) >>
                                    0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(uVar8,uVar28),CONCAT14(uVar28,uVar6)) >> 0x20
                                    ) >> 4)) >> 6;
          uVar13 = (ushort)((uVar13 >> 4) + (uVar19 >> 4) + (uVar21 >> 4) + (uVar8 >> 4)) >> 6;
          bVar7 = (uVar13 != 0) * (uVar13 < 0x100) * (char)uVar13 - (0xff < uVar13);
          uVar16 = CONCAT13(bVar7,CONCAT12((uVar22 != 0) * (uVar22 < 0x100) * (char)uVar22 -
                                           (0xff < uVar22),
                                           CONCAT11((uVar20 != 0) * (uVar20 < 0x100) * (char)uVar20
                                                    - (0xff < uVar20),
                                                    (uVar14 != 0) * (uVar14 < 0x100) * (char)uVar14
                                                    - (0xff < uVar14))));
          *puVar12 = (ushort)((ushort)((ushort)(((uVar16 >> 4) << 0x1c) >> 0x14) |
                                      (ushort)(((uVar16 >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                             (ushort)(((uVar16 >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                     (ushort)(((uint)(bVar7 >> 4) << 0x1c) >> 0x10);
          pwVar11 = pwVar11 + 2;
          puVar12 = puVar12 + 1;
          uStack_28 = uStack_28 - 1;
        } while (uStack_28 != 0);
        pwVar11 = pwVar11 + uVar9 * 6;
        uStack_24 = uStack_24 - 1;
        uStack_28 = uVar9;
      } while (uStack_24 != 0);
    }
  }
  return;
}


/* Address: 0x00580F60.
   Ownership: graphics/backend/glide.
   Purpose: Fills Glide texture data with the recovered constant 0x0FFF pattern.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsGlide3_FillTextureDataConstant0FFF(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *pGVar1;
  int iVar2;
  int iVar3;
  ushort *glideDataCursor;
  ushort *nextGlideDataCursor;
  int iStack_28;
  int iStack_24;
  
  pGVar1 = texture->sourceAsset;
  glideDataCursor = (texture->glideInfo).data;
  iVar3 = texture->subresourceIndex * 0x20 + (pGVar1->tableDescriptor).subresourceTableOffset;
  iVar2 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar3 + -0x10);
  iStack_24 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + iVar3 + -0xc);
  iStack_28 = iVar2;
  do {
    do {
      nextGlideDataCursor = glideDataCursor + 1;
      *glideDataCursor = 0xfff;
      iStack_28 = iStack_28 + -1;
      glideDataCursor = nextGlideDataCursor;
    } while (iStack_28 != 0);
    iStack_24 = iStack_24 + -1;
    iStack_28 = iVar2;
  } while (iStack_24 != 0);
  return;
}

/* Address: 0x00580FF0.
   Ownership: graphics/backend/glide.
   Purpose: Downsamples alpha samples into white ARGB4444 texels in the dormant Glide path.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsGlide3_DownsampleAlpha8ToWhiteArgb4444(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *pGVar1;
  byte bVar2;
  uint downsampledWidth;
  int subresourceRecordOffset;
  ushort *sourcePairCursor;
  ushort *glideDataCursor;
  uint remainingColumns;
  uint remainingRows;
  
  pGVar1 = texture->sourceAsset;
  glideDataCursor = (texture->glideInfo).data;
  subresourceRecordOffset =
       texture->subresourceIndex * 0x20 + (pGVar1->tableDescriptor).subresourceTableOffset;
  sourcePairCursor =
       (ushort *)
       ((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
       *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
               subresourceRecordOffset + -0x1c) + -0x28);
  downsampledWidth =
       *(uint *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                subresourceRecordOffset + -0x10) >> 1;
  remainingRows =
       *(uint *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                subresourceRecordOffset + -0xc) >> 1;
  remainingColumns = downsampledWidth;
  do {
    do {
      bVar2 = (byte)*sourcePairCursor >> 2;
      *glideDataCursor =
           CONCAT11((byte)(*sourcePairCursor >> 10) + bVar2 +
                    (byte)(sourcePairCursor[downsampledWidth] >> 10) +
                    ((byte)sourcePairCursor[downsampledWidth] >> 2),bVar2) | 0xfff;
      sourcePairCursor = sourcePairCursor + 1;
      glideDataCursor = glideDataCursor + 1;
      remainingColumns = remainingColumns - 1;
    } while (remainingColumns != 0);
    sourcePairCursor = sourcePairCursor + downsampledWidth;
    remainingRows = remainingRows - 1;
    remainingColumns = downsampledWidth;
  } while (remainingRows != 0);
  return;
}

/* Address: 0x005810A0.
   Ownership: graphics/backend/glide.
   Purpose: Downsamples alternating alpha samples into white ARGB4444 texels in the dormant Glide path.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsGlide3_DownsampleAlternateAlphaSamplesToWhiteArgb4444(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *pGVar1;
  uint downsampledWidth;
  int subresourceRecordOffset;
  byte *sourceByteCursor;
  ushort *glideDataCursor;
  uint remainingColumns;
  uint remainingRows;
  
  pGVar1 = texture->sourceAsset;
  glideDataCursor = (texture->glideInfo).data;
  subresourceRecordOffset =
       texture->subresourceIndex * 0x20 + (pGVar1->tableDescriptor).subresourceTableOffset;
  sourceByteCursor =
       (pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
       *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
               subresourceRecordOffset + -0x1c) + -0x28;
  downsampledWidth =
       *(uint *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                subresourceRecordOffset + -0x10) >> 1;
  remainingRows =
       *(uint *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                subresourceRecordOffset + -0xc) >> 1;
  remainingColumns = downsampledWidth;
  do {
    do {
      *glideDataCursor =
           CONCAT11((*sourceByteCursor >> 2) + (sourceByteCursor[2] >> 2) +
                    (sourceByteCursor[downsampledWidth * 8] >> 2) +
                    (sourceByteCursor[downsampledWidth * 8 + 2] >> 2),sourceByteCursor[2] >> 2) |
           0xfff;
      sourceByteCursor = sourceByteCursor + 4;
      glideDataCursor = glideDataCursor + 1;
      remainingColumns = remainingColumns - 1;
    } while (remainingColumns != 0);
    sourceByteCursor = sourceByteCursor + downsampledWidth * 0xc;
    remainingRows = remainingRows - 1;
    remainingColumns = downsampledWidth;
  } while (remainingRows != 0);
  return;
}

/* Address: 0x00581150.
   Ownership: graphics/backend/glide.
   Purpose: Glide3-selected source-alpha blitter. It falls back to SoftwareTextureSource_BlitSourceAlpha16 for
   ordinary framebuffer objects and uses the Glide framebuffer-offset path for the special backend target. ABI: the
   implementation preserves the input drawX in EAX and drawY in EDX. That qword is register-preservation behavior,
   not a semantic API result. CF is cleared before normal return.
   Cross-module calls: SoftwareTextureSource_BlitSourceAlpha16 [graphics/backend/software].
*/
bool __thandor_cf_preserve_eax_ecx_edx
Glide3_TextureSource_BlitSourceAlpha
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  GraphicsPixelDimension GVar7;
  int iVar8;
  GraphicsPixelDimension GVar9;
  int iVar10;
  byte *pbVar11;
  uint *puVar12;
  byte *pbVar13;
  ulonglong uVar14;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  undefined8 mm0PackedValue2;
  undefined8 mm0PackedValue3;
  byte mm1PackedValue0ByteLane1;
  byte mm1PackedValue1ByteLane1;
  byte mm1PackedValue0ByteLane2;
  byte mm1PackedValue1ByteLane2;
  byte mm1PackedValue0ByteLane3;
  byte mm1PackedValue1ByteLane3;
  undefined8 mm1PackedValue0;
  undefined8 mm1PackedValue1;
  
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      iVar1 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      iVar6 = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar1 + -0x18);
      iVar8 = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar1 + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar1 + -0x20)
          == -1) {
        GVar9 = iVar6 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0x10);
        GVar7 = iVar8 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0xc);
        if (iVar6 < 0) {
          iVar6 = 0;
        }
        if (iVar8 < 0) {
          iVar8 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar9) {
          GVar9 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar7) {
          GVar7 = g_DisplayFramebufferAccess.height;
        }
        if (iVar6 < clipMinX) {
          iVar6 = clipMinX;
        }
        if (iVar8 < clipMinY) {
          iVar8 = clipMinY;
        }
        if (clipMaxX < (int)GVar9) {
          GVar9 = clipMaxX;
        }
        if (clipMaxY < (int)GVar7) {
          GVar7 = clipMaxY;
        }
        iVar10 = GVar9 - iVar6;
        if ((iVar10 != 0 && iVar6 <= (int)GVar9) &&
           (clipMinY = GVar7 - iVar8, clipMinY != 0 && iVar8 <= (int)GVar7)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar13 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar8 + iVar6) * 2;
          puVar12 = (uint *)((int)sourceAsset +
                            ((iVar8 - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar3 * 4 +
                            ((iVar6 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar10;
          iVar6 = iVar10;
          do {
            do {
              uVar5 = *puVar12;
              if (0xffffff < uVar5) {
                if (uVar5 < 0xff000000) {
                  uVar2 = *(undefined2 *)(pbVar13 + g_GlideSecondBufferOffset);
                  mm1PackedValue1ByteLane3 = (byte)(uVar5 >> 0x18);
                  mm1PackedValue1ByteLane2 = (byte)(uVar5 >> 0x10);
                  mm1PackedValue1ByteLane1 = (byte)(uVar5 >> 8);
                  pbVar13 = pbVar13 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                  uVar14 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                           (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                  mm1PackedValue1 =
                       pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                       >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                                  ,mm1PackedValue1ByteLane2),
                                                  CONCAT14(mm1PackedValue1ByteLane2,uVar5)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                                         mm1PackedValue1ByteLane1)
                                                                >> 2,CONCAT11((char)uVar5,
                                                                              (char)uVar5) >> 2))),
                              g_SoftwareBlendAlphaFactors[uVar5 >> 0x18]);
                  mm0PackedValue2 =
                       pmulhw(CONCAT26((ushort)((short)(uVar14 >> 0x30) *
                                               g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                       CONCAT24((ushort)((short)(uVar14 >> 0x20) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.red
                                                        ) >> 2,
                                                CONCAT22((ushort)((short)(uVar14 >> 0x10) *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.green) >> 2,
                                                         (ushort)((short)uVar14 *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.blue) >> 2))),
                              g_SoftwareBlendInverseAlphaFactors[uVar5 >> 0x18]);
                  mm0PackedValue3 =
                       pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue2 >> 0x30) +
                                        (short)((ulonglong)mm1PackedValue1 >> 0x30),
                                        CONCAT24((short)((ulonglong)mm0PackedValue2 >> 0x20) +
                                                 (short)((ulonglong)mm1PackedValue1 >> 0x20),
                                                 CONCAT22((short)((ulonglong)mm0PackedValue2 >> 0x10
                                                                 ) +
                                                          (short)((ulonglong)mm1PackedValue1 >> 0x10
                                                                 ),(short)mm0PackedValue2 +
                                                                   (short)mm1PackedValue1))) &
                               (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                               g_SoftwarePixelMmxConstants.packWeights);
                  *(short *)pbVar13 =
                       (short)((ulonglong)mm0PackedValue3 >> 8) +
                       (short)((ulonglong)mm0PackedValue3 >> 0x28);
                }
                else {
                  *(short *)pbVar13 =
                       (short)g_SoftwarePixelPackTables->blue[uVar5 & 0xff] +
                       (short)*(undefined4 *)
                               ((int)g_SoftwarePixelPackTables->green + ((uVar5 & 0xff00) >> 6)) +
                       (short)*(undefined4 *)
                               ((int)g_SoftwarePixelPackTables->red + ((uVar5 & 0xff0000) >> 0xe));
                }
              }
              puVar12 = puVar12 + 1;
              pbVar13 = pbVar13 + 2;
              iVar6 = iVar6 + -1;
            } while (iVar6 != 0);
            puVar12 = puVar12 + (iVar3 - iVar10);
            pbVar13 = pbVar13 + iVar1 * 2;
            clipMinY = clipMinY + -1;
            iVar6 = iVar10;
          } while (clipMinY != 0);
          return false;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar1 + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        GVar9 = iVar6 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0x10);
        GVar7 = iVar8 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0xc);
        if (iVar6 < 0) {
          iVar6 = 0;
        }
        if (iVar8 < 0) {
          iVar8 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar9) {
          GVar9 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar7) {
          GVar7 = g_DisplayFramebufferAccess.height;
        }
        if (iVar6 < clipMinX) {
          iVar6 = clipMinX;
        }
        if (iVar8 < clipMinY) {
          iVar8 = clipMinY;
        }
        if (clipMaxX < (int)GVar9) {
          GVar9 = clipMaxX;
        }
        if (clipMaxY < (int)GVar7) {
          GVar7 = clipMaxY;
        }
        iVar10 = GVar9 - iVar6;
        if ((iVar10 != 0 && iVar6 <= (int)GVar9) &&
           (clipMinY = GVar7 - iVar8, clipMinY != 0 && iVar8 <= (int)GVar7)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x20);
          iVar4 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar13 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar8 + iVar6) * 2;
          pbVar11 = (byte *)((int)sourceAsset +
                            ((iVar8 - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar4 +
                            ((iVar6 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar10;
          iVar6 = iVar10;
          do {
            do {
              uVar5 = *(uint *)(sourceAsset[iVar3 * 4 + 1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)*pbVar11 * 8 + -0x24);
              if (0xffffff < uVar5) {
                if (uVar5 < 0xff000000) {
                  uVar5 = *(uint *)(sourceAsset[iVar3 * 4 + 1].common.buildMetadata.
                                    assetRelativeAddressAnchor28 + (uint)*pbVar11 * 8 + -0x28);
                  uVar2 = *(undefined2 *)(pbVar13 + g_GlideSecondBufferOffset);
                  mm1PackedValue0ByteLane3 = (byte)(uVar5 >> 0x18);
                  mm1PackedValue0ByteLane2 = (byte)(uVar5 >> 0x10);
                  mm1PackedValue0ByteLane1 = (byte)(uVar5 >> 8);
                  pbVar13 = pbVar13 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                  uVar14 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                           (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                  mm1PackedValue0 =
                       pmulhw(CONCAT26(CONCAT11(mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                       >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                                  ,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,uVar5)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                         mm1PackedValue0ByteLane1)
                                                                >> 2,CONCAT11((char)uVar5,
                                                                              (char)uVar5) >> 2))),
                              g_SoftwareBlendAlphaFactors[uVar5 >> 0x18]);
                  mm0PackedValue0 =
                       pmulhw(CONCAT26((ushort)((short)(uVar14 >> 0x30) *
                                               g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                       CONCAT24((ushort)((short)(uVar14 >> 0x20) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.red
                                                        ) >> 2,
                                                CONCAT22((ushort)((short)(uVar14 >> 0x10) *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.green) >> 2,
                                                         (ushort)((short)uVar14 *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.blue) >> 2))),
                              g_SoftwareBlendInverseAlphaFactors[uVar5 >> 0x18]);
                  mm0PackedValue1 =
                       pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                                        (short)((ulonglong)mm1PackedValue0 >> 0x30),
                                        CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                                 (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                                 CONCAT22((short)((ulonglong)mm0PackedValue0 >> 0x10
                                                                 ) +
                                                          (short)((ulonglong)mm1PackedValue0 >> 0x10
                                                                 ),(short)mm0PackedValue0 +
                                                                   (short)mm1PackedValue0))) &
                               (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                               g_SoftwarePixelMmxConstants.packWeights);
                  *(short *)pbVar13 =
                       (short)((ulonglong)mm0PackedValue1 >> 8) +
                       (short)((ulonglong)mm0PackedValue1 >> 0x28);
                }
                else {
                  *(short *)pbVar13 = (short)uVar5;
                }
              }
              pbVar11 = pbVar11 + 1;
              pbVar13 = pbVar13 + 2;
              iVar6 = iVar6 + -1;
            } while (iVar6 != 0);
            pbVar11 = pbVar11 + (iVar4 - iVar10);
            pbVar13 = pbVar13 + iVar1 * 2;
            clipMinY = clipMinY + -1;
            iVar6 = iVar10;
          } while (clipMinY != 0);
        }
      }
    }
  }
  else {
    SoftwareTextureSource_BlitSourceAlpha16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,subresourceIndex,sourceAsset,
               framebuffer);
  }
  return false;
}


/* Address: 0x005814D0.
   Ownership: graphics/backend/glide.
   Purpose: Glide3-selected half-source-RGB blitter with the same verified nine-argument ABI. ABI: the
   implementation preserves the input drawX in EAX and drawY in EDX. That qword is register-preservation behavior,
   not a semantic API result. CF is cleared before normal return. It selects an existing resource facet and does
   not imply sprite, model, or effect identity.
   Cross-module calls: SoftwareTextureSource_BlitHalfSourceRgb16 [graphics/backend/software].
*/
bool __thandor_cf_preserve_eax_ecx_edx
Glide3_TextureSource_BlitHalfSourceRgb
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  GraphicsPixelDimension GVar7;
  int iVar8;
  GraphicsPixelDimension GVar9;
  int iVar10;
  byte *pbVar11;
  uint *puVar12;
  byte *pbVar13;
  ulonglong uVar14;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  undefined8 mm0PackedValue2;
  undefined8 mm0PackedValue3;
  byte mm1PackedValue1ByteLane1;
  byte mm1PackedValue0ByteLane2;
  byte mm1PackedValue1ByteLane2;
  byte mm1PackedValue0ByteLane3;
  byte mm1PackedValue1ByteLane3;
  byte mm1PackedValue0ByteLane1;
  undefined8 mm1PackedValue0;
  undefined8 mm1PackedValue1;
  
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      iVar1 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      iVar6 = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar1 + -0x18);
      iVar8 = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar1 + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar1 + -0x20)
          == -1) {
        GVar9 = iVar6 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0x10);
        GVar7 = iVar8 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0xc);
        if (iVar6 < 0) {
          iVar6 = 0;
        }
        if (iVar8 < 0) {
          iVar8 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar9) {
          GVar9 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar7) {
          GVar7 = g_DisplayFramebufferAccess.height;
        }
        if (iVar6 < clipMinX) {
          iVar6 = clipMinX;
        }
        if (iVar8 < clipMinY) {
          iVar8 = clipMinY;
        }
        if (clipMaxX < (int)GVar9) {
          GVar9 = clipMaxX;
        }
        if (clipMaxY < (int)GVar7) {
          GVar7 = clipMaxY;
        }
        iVar10 = GVar9 - iVar6;
        if ((iVar10 != 0 && iVar6 <= (int)GVar9) &&
           (clipMinY = GVar7 - iVar8, clipMinY != 0 && iVar8 <= (int)GVar7)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar13 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar8 + iVar6) * 2;
          puVar12 = (uint *)((int)sourceAsset +
                            ((iVar8 - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar3 * 4 +
                            ((iVar6 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar10;
          iVar6 = iVar10;
          do {
            do {
              uVar5 = *puVar12;
              if (0xffffff < uVar5) {
                uVar2 = *(undefined2 *)(pbVar13 + g_GlideSecondBufferOffset);
                mm1PackedValue1ByteLane3 = (byte)(uVar5 >> 0x18);
                mm1PackedValue1ByteLane2 = (byte)(uVar5 >> 0x10);
                mm1PackedValue1ByteLane1 = (byte)(uVar5 >> 8);
                pbVar13 = pbVar13 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                uVar14 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                         (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                mm1PackedValue1 =
                     pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3) >>
                                     3,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                                  ,mm1PackedValue1ByteLane2),
                                                  CONCAT14(mm1PackedValue1ByteLane2,uVar5)) >> 0x20)
                                                >> 3,CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                                       mm1PackedValue1ByteLane1) >>
                                                              3,CONCAT11((char)uVar5,(char)uVar5) >>
                                                                3))),
                            g_SoftwareBlendAlphaFactors[uVar5 >> 0x18]);
                mm0PackedValue2 =
                     pmulhw(CONCAT26((ushort)((short)(uVar14 >> 0x30) *
                                             g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                     CONCAT24((ushort)((short)(uVar14 >> 0x20) *
                                                      g_SoftwarePixelMmxConstants.unpackScales.red)
                                              >> 2,CONCAT22((ushort)((short)(uVar14 >> 0x10) *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.green) >> 2,
                                                            (ushort)((short)uVar14 *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.blue) >> 2))),
                            g_SoftwareBlendInverseAlphaFactors[uVar5 >> 0x18]);
                mm0PackedValue3 =
                     pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue2 >> 0x30) +
                                      (short)((ulonglong)mm1PackedValue1 >> 0x30),
                                      CONCAT24((short)((ulonglong)mm0PackedValue2 >> 0x20) +
                                               (short)((ulonglong)mm1PackedValue1 >> 0x20),
                                               CONCAT22((short)((ulonglong)mm0PackedValue2 >> 0x10)
                                                        + (short)((ulonglong)mm1PackedValue1 >> 0x10
                                                                 ),
                                                        (short)mm0PackedValue2 +
                                                        (short)mm1PackedValue1))) &
                             (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)pbVar13 =
                     (short)((ulonglong)mm0PackedValue3 >> 8) +
                     (short)((ulonglong)mm0PackedValue3 >> 0x28);
              }
              puVar12 = puVar12 + 1;
              pbVar13 = pbVar13 + 2;
              iVar6 = iVar6 + -1;
            } while (iVar6 != 0);
            puVar12 = puVar12 + (iVar3 - iVar10);
            pbVar13 = pbVar13 + iVar1 * 2;
            clipMinY = clipMinY + -1;
            iVar6 = iVar10;
          } while (clipMinY != 0);
          return false;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar1 + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        GVar9 = iVar6 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0x10);
        GVar7 = iVar8 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0xc);
        if (iVar6 < 0) {
          iVar6 = 0;
        }
        if (iVar8 < 0) {
          iVar8 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar9) {
          GVar9 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar7) {
          GVar7 = g_DisplayFramebufferAccess.height;
        }
        if (iVar6 < clipMinX) {
          iVar6 = clipMinX;
        }
        if (iVar8 < clipMinY) {
          iVar8 = clipMinY;
        }
        if (clipMaxX < (int)GVar9) {
          GVar9 = clipMaxX;
        }
        if (clipMaxY < (int)GVar7) {
          GVar7 = clipMaxY;
        }
        iVar10 = GVar9 - iVar6;
        if ((iVar10 != 0 && iVar6 <= (int)GVar9) &&
           (clipMinY = GVar7 - iVar8, clipMinY != 0 && iVar8 <= (int)GVar7)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x20);
          iVar4 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar13 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar8 + iVar6) * 2;
          pbVar11 = (byte *)((int)sourceAsset +
                            ((iVar8 - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar4 +
                            ((iVar6 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar10;
          iVar6 = iVar10;
          do {
            do {
              uVar5 = *(uint *)(sourceAsset[iVar3 * 4 + 1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)*pbVar11 * 8 + -0x28);
              if (0xffffff < uVar5) {
                uVar2 = *(undefined2 *)(pbVar13 + g_GlideSecondBufferOffset);
                mm1PackedValue0ByteLane3 = (byte)(uVar5 >> 0x18);
                mm1PackedValue0ByteLane2 = (byte)(uVar5 >> 0x10);
                mm1PackedValue0ByteLane1 = (byte)(uVar5 >> 8);
                pbVar13 = pbVar13 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                uVar14 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                         (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                mm1PackedValue0 =
                     pmulhw(CONCAT26(CONCAT11(mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3) >>
                                     3,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                                  ,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,uVar5)) >> 0x20)
                                                >> 3,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                       mm1PackedValue0ByteLane1) >>
                                                              3,CONCAT11((char)uVar5,(char)uVar5) >>
                                                                3))),
                            g_SoftwareBlendAlphaFactors[uVar5 >> 0x18]);
                mm0PackedValue0 =
                     pmulhw(CONCAT26((ushort)((short)(uVar14 >> 0x30) *
                                             g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                     CONCAT24((ushort)((short)(uVar14 >> 0x20) *
                                                      g_SoftwarePixelMmxConstants.unpackScales.red)
                                              >> 2,CONCAT22((ushort)((short)(uVar14 >> 0x10) *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.green) >> 2,
                                                            (ushort)((short)uVar14 *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.blue) >> 2))),
                            g_SoftwareBlendInverseAlphaFactors[uVar5 >> 0x18]);
                mm0PackedValue1 =
                     pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                                      (short)((ulonglong)mm1PackedValue0 >> 0x30),
                                      CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                               (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                               CONCAT22((short)((ulonglong)mm0PackedValue0 >> 0x10)
                                                        + (short)((ulonglong)mm1PackedValue0 >> 0x10
                                                                 ),
                                                        (short)mm0PackedValue0 +
                                                        (short)mm1PackedValue0))) &
                             (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)pbVar13 =
                     (short)((ulonglong)mm0PackedValue1 >> 8) +
                     (short)((ulonglong)mm0PackedValue1 >> 0x28);
              }
              pbVar11 = pbVar11 + 1;
              pbVar13 = pbVar13 + 2;
              iVar6 = iVar6 + -1;
            } while (iVar6 != 0);
            pbVar11 = pbVar11 + (iVar4 - iVar10);
            pbVar13 = pbVar13 + iVar1 * 2;
            clipMinY = clipMinY + -1;
            iVar6 = iVar10;
          } while (clipMinY != 0);
        }
      }
    }
  }
  else {
    SoftwareTextureSource_BlitHalfSourceRgb16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,subresourceIndex,sourceAsset,
               framebuffer);
  }
  return false;
}


/* Address: 0x005817F0.
   Ownership: graphics/backend/glide.
   Purpose: Glide3-selected direct-color bilinear stretch. Ordinary framebuffer objects fall back to
   SoftwareTextureSource_StretchDirectColorBilinear16. The source entry must be direct color: paletteIndex == -1.
   The function computes 8-bit fractional source steps from (pixelWidth-1)/(destinationWidth-1) and
   (pixelHeight-1)/(destinationHeight-1). Four neighboring ARGB8888 pixels are blended horizontally and vertically
   through g_SoftwareBilinearForwardFactors and g_SoftwareBilinearInverseFactors.
   Cross-module calls: SoftwareTextureSource_StretchDirectColorBilinear16 [graphics/backend/software].
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_TextureSource_StretchDirectColorBilinear
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  char cVar11;
  char cVar15;
  GraphicsPixelDimension GVar19;
  uint uVar20;
  int iVar21;
  uint uVar22;
  uint uVar23;
  uint uVar24;
  uint uVar25;
  uint uVar26;
  uint uVar27;
  byte *pbVar28;
  byte *pbVar29;
  byte *pbVar30;
  ushort uVar31;
  undefined1 mm0PackedValue0ByteLane1;
  undefined1 mm0PackedValue0ByteLane2;
  ushort uVar33;
  ushort uVar34;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  ushort uVar35;
  ulonglong uVar32;
  undefined8 mm0PackedValue2;
  undefined1 mm1PackedValue0ByteLane1;
  undefined1 mm1PackedValue0ByteLane2;
  undefined8 mm1PackedValue0;
  undefined1 mm2PackedValue0ByteLane1;
  undefined1 mm2PackedValue0ByteLane2;
  undefined8 mm2PackedValue0;
  undefined8 mm2PackedValue1;
  undefined1 mm3PackedValue0ByteLane1;
  undefined1 mm3PackedValue0ByteLane2;
  undefined8 mm3PackedValue0;
  ushort uVar36;
  undefined1 mm4PackedValue0ByteLane1;
  undefined1 mm4PackedValue0ByteLane2;
  ushort uVar38;
  ushort uVar39;
  undefined8 mm4PackedValue0;
  undefined8 mm4PackedValue1;
  ushort uVar40;
  ulonglong uVar37;
  undefined8 mm4PackedValue2;
  undefined1 mm5PackedValue0ByteLane1;
  undefined1 mm5PackedValue0ByteLane2;
  undefined8 mm5PackedValue0;
  undefined1 mm6PackedValue0ByteLane1;
  undefined1 mm6PackedValue0ByteLane2;
  undefined8 mm6PackedValue0;
  undefined8 mm6PackedValue1;
  undefined1 mm7PackedValue0ByteLane1;
  undefined1 mm7PackedValue0ByteLane2;
  undefined8 mm7PackedValue0;
  byte *pbVar41;
  char cVar12;
  char cVar13;
  char cVar14;
  char cVar16;
  char cVar17;
  char cVar18;
  
  GVar19 = g_DisplayFramebufferAccess.width;
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (((((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
         (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) &&
        (iVar2 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset,
        g_DisplayFramebufferAccess.bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT)) &&
       (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar2 + -0x20)
        == -1)) {
      pbVar30 = g_DisplayFramebufferAccess.pixels +
                (destinationY * g_DisplayFramebufferAccess.width + destinationX) * 2;
      uVar20 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                       iVar2 + -0x10) - 1;
      iVar21 = (int)(((ulonglong)(uVar20 >> 0x18) << 0x20 | (ulonglong)uVar20 * 0x100 & 0xffffffff)
                    / (ulonglong)(destinationWidth - 1));
      uVar22 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                       iVar2 + -0xc) - 1;
      uVar23 = destinationHeight - 1;
      pbVar28 = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar2 + -0x1c) + -0x28;
      iVar2 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                      iVar2 + -0x10) * 4;
      uVar24 = 0;
      uVar26 = 0;
      pbVar29 = pbVar28;
      uVar20 = destinationWidth >> 1;
      pbVar41 = pbVar30;
      do {
        do {
          uVar25 = uVar24 >> 8;
          iVar1 = iVar2 + uVar25 * 4;
          uVar3 = *(undefined4 *)(pbVar29 + uVar25 * 4);
          uVar4 = *(undefined4 *)(pbVar29 + uVar25 * 4 + 4);
          uVar5 = *(undefined4 *)(pbVar29 + iVar1);
          uVar6 = *(undefined4 *)(pbVar29 + iVar1 + 4);
          uVar25 = uVar24 + iVar21 >> 8;
          iVar1 = iVar2 + uVar25 * 4;
          uVar7 = *(undefined4 *)(pbVar29 + uVar25 * 4);
          uVar8 = *(undefined4 *)(pbVar29 + uVar25 * 4 + 4);
          uVar9 = *(undefined4 *)(pbVar29 + iVar1);
          uVar10 = *(undefined4 *)(pbVar29 + iVar1 + 4);
          mm0PackedValue0ByteLane1 = (undefined1)((uint)uVar3 >> 0x18);
          uVar31 = CONCAT11(mm0PackedValue0ByteLane1,mm0PackedValue0ByteLane1);
          mm0PackedValue0ByteLane2 = (undefined1)((uint)uVar3 >> 0x10);
          mm0PackedValue0ByteLane1 = (undefined1)((uint)uVar3 >> 8);
          mm1PackedValue0ByteLane1 = (undefined1)((uint)uVar4 >> 0x18);
          uVar33 = CONCAT11(mm1PackedValue0ByteLane1,mm1PackedValue0ByteLane1);
          mm1PackedValue0ByteLane2 = (undefined1)((uint)uVar4 >> 0x10);
          mm1PackedValue0ByteLane1 = (undefined1)((uint)uVar4 >> 8);
          mm2PackedValue0ByteLane1 = (undefined1)((uint)uVar5 >> 0x18);
          uVar34 = CONCAT11(mm2PackedValue0ByteLane1,mm2PackedValue0ByteLane1);
          mm2PackedValue0ByteLane2 = (undefined1)((uint)uVar5 >> 0x10);
          mm2PackedValue0ByteLane1 = (undefined1)((uint)uVar5 >> 8);
          mm3PackedValue0ByteLane1 = (undefined1)((uint)uVar6 >> 0x18);
          uVar35 = CONCAT11(mm3PackedValue0ByteLane1,mm3PackedValue0ByteLane1);
          mm3PackedValue0ByteLane2 = (undefined1)((uint)uVar6 >> 0x10);
          mm3PackedValue0ByteLane1 = (undefined1)((uint)uVar6 >> 8);
          mm4PackedValue0ByteLane1 = (undefined1)((uint)uVar7 >> 0x18);
          uVar36 = CONCAT11(mm4PackedValue0ByteLane1,mm4PackedValue0ByteLane1);
          mm4PackedValue0ByteLane2 = (undefined1)((uint)uVar7 >> 0x10);
          mm4PackedValue0ByteLane1 = (undefined1)((uint)uVar7 >> 8);
          mm5PackedValue0ByteLane1 = (undefined1)((uint)uVar8 >> 0x18);
          uVar38 = CONCAT11(mm5PackedValue0ByteLane1,mm5PackedValue0ByteLane1);
          mm5PackedValue0ByteLane2 = (undefined1)((uint)uVar8 >> 0x10);
          mm5PackedValue0ByteLane1 = (undefined1)((uint)uVar8 >> 8);
          mm6PackedValue0ByteLane1 = (undefined1)((uint)uVar9 >> 0x18);
          uVar39 = CONCAT11(mm6PackedValue0ByteLane1,mm6PackedValue0ByteLane1);
          mm6PackedValue0ByteLane2 = (undefined1)((uint)uVar9 >> 0x10);
          mm6PackedValue0ByteLane1 = (undefined1)((uint)uVar9 >> 8);
          mm7PackedValue0ByteLane1 = (undefined1)((uint)uVar10 >> 0x18);
          uVar40 = CONCAT11(mm7PackedValue0ByteLane1,mm7PackedValue0ByteLane1);
          mm7PackedValue0ByteLane2 = (undefined1)((uint)uVar10 >> 0x10);
          mm7PackedValue0ByteLane1 = (undefined1)((uint)uVar10 >> 8);
          uVar25 = uVar24 & 0xff;
          uVar27 = uVar26 & 0xff;
          mm0PackedValue0 =
               pmulhw(CONCAT26(uVar31 >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar31,mm0PackedValue0ByteLane2),
                                                          CONCAT14(mm0PackedValue0ByteLane2,uVar3))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                          mm0PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)uVar3,(char)uVar3) >> 2))),
                      g_SoftwareBilinearInverseFactors[uVar25]);
          mm1PackedValue0 =
               pmulhw(CONCAT26(uVar33 >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar33,mm1PackedValue0ByteLane2),
                                                          CONCAT14(mm1PackedValue0ByteLane2,uVar4))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                          mm1PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)uVar4,(char)uVar4) >> 2))),
                      g_SoftwareBilinearForwardFactors[uVar25]);
          mm2PackedValue0 =
               pmulhw(CONCAT26(uVar34 >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar34,mm2PackedValue0ByteLane2),
                                                          CONCAT14(mm2PackedValue0ByteLane2,uVar5))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm2PackedValue0ByteLane1,
                                                          mm2PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)uVar5,(char)uVar5) >> 2))),
                      g_SoftwareBilinearInverseFactors[uVar25]);
          mm3PackedValue0 =
               pmulhw(CONCAT26(uVar35 >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar35,mm3PackedValue0ByteLane2),
                                                          CONCAT14(mm3PackedValue0ByteLane2,uVar6))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm3PackedValue0ByteLane1,
                                                          mm3PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)uVar6,(char)uVar6) >> 2))),
                      g_SoftwareBilinearForwardFactors[uVar25]);
          mm0PackedValue1 =
               pmulhw(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                               (short)((ulonglong)mm1PackedValue0 >> 0x30),
                               CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                        (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                        CONCAT22((short)((ulonglong)mm0PackedValue0 >> 0x10) +
                                                 (short)((ulonglong)mm1PackedValue0 >> 0x10),
                                                 (short)mm0PackedValue0 + (short)mm1PackedValue0))),
                      g_SoftwareBilinearInverseFactors[uVar27]);
          mm2PackedValue1 =
               pmulhw(CONCAT26((short)((ulonglong)mm2PackedValue0 >> 0x30) +
                               (short)((ulonglong)mm3PackedValue0 >> 0x30),
                               CONCAT24((short)((ulonglong)mm2PackedValue0 >> 0x20) +
                                        (short)((ulonglong)mm3PackedValue0 >> 0x20),
                                        CONCAT22((short)((ulonglong)mm2PackedValue0 >> 0x10) +
                                                 (short)((ulonglong)mm3PackedValue0 >> 0x10),
                                                 (short)mm2PackedValue0 + (short)mm3PackedValue0))),
                      g_SoftwareBilinearForwardFactors[uVar27]);
          uVar25 = uVar24 + iVar21 & 0xff;
          mm4PackedValue0 =
               pmulhw(CONCAT26(uVar36 >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar36,mm4PackedValue0ByteLane2),
                                                          CONCAT14(mm4PackedValue0ByteLane2,uVar7))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm4PackedValue0ByteLane1,
                                                          mm4PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)uVar7,(char)uVar7) >> 2))),
                      g_SoftwareBilinearInverseFactors[uVar25]);
          mm5PackedValue0 =
               pmulhw(CONCAT26(uVar38 >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar38,mm5PackedValue0ByteLane2),
                                                          CONCAT14(mm5PackedValue0ByteLane2,uVar8))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm5PackedValue0ByteLane1,
                                                          mm5PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)uVar8,(char)uVar8) >> 2))),
                      g_SoftwareBilinearForwardFactors[uVar25]);
          mm6PackedValue0 =
               pmulhw(CONCAT26(uVar39 >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar39,mm6PackedValue0ByteLane2),
                                                          CONCAT14(mm6PackedValue0ByteLane2,uVar9))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm6PackedValue0ByteLane1,
                                                          mm6PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)uVar9,(char)uVar9) >> 2))),
                      g_SoftwareBilinearInverseFactors[uVar25]);
          mm7PackedValue0 =
               pmulhw(CONCAT26(uVar40 >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar40,mm7PackedValue0ByteLane2),
                                                          CONCAT14(mm7PackedValue0ByteLane2,uVar10))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm7PackedValue0ByteLane1,
                                                          mm7PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)uVar10,(char)uVar10) >> 2))),
                      g_SoftwareBilinearForwardFactors[uVar25]);
          mm4PackedValue1 =
               pmulhw(CONCAT26((short)((ulonglong)mm4PackedValue0 >> 0x30) +
                               (short)((ulonglong)mm5PackedValue0 >> 0x30),
                               CONCAT24((short)((ulonglong)mm4PackedValue0 >> 0x20) +
                                        (short)((ulonglong)mm5PackedValue0 >> 0x20),
                                        CONCAT22((short)((ulonglong)mm4PackedValue0 >> 0x10) +
                                                 (short)((ulonglong)mm5PackedValue0 >> 0x10),
                                                 (short)mm4PackedValue0 + (short)mm5PackedValue0))),
                      g_SoftwareBilinearInverseFactors[uVar27]);
          mm6PackedValue1 =
               pmulhw(CONCAT26((short)((ulonglong)mm6PackedValue0 >> 0x30) +
                               (short)((ulonglong)mm7PackedValue0 >> 0x30),
                               CONCAT24((short)((ulonglong)mm6PackedValue0 >> 0x20) +
                                        (short)((ulonglong)mm7PackedValue0 >> 0x20),
                                        CONCAT22((short)((ulonglong)mm6PackedValue0 >> 0x10) +
                                                 (short)((ulonglong)mm7PackedValue0 >> 0x10),
                                                 (short)mm6PackedValue0 + (short)mm7PackedValue0))),
                      g_SoftwareBilinearForwardFactors[uVar27]);
          uVar31 = (ushort)((short)mm0PackedValue1 + (short)mm2PackedValue1) >> 2;
          uVar33 = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x10) +
                           (short)((ulonglong)mm2PackedValue1 >> 0x10)) >> 2;
          uVar34 = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x20) +
                           (short)((ulonglong)mm2PackedValue1 >> 0x20)) >> 2;
          uVar35 = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x30) +
                           (short)((ulonglong)mm2PackedValue1 >> 0x30)) >> 2;
          uVar36 = (ushort)((short)mm4PackedValue1 + (short)mm6PackedValue1) >> 2;
          uVar38 = (ushort)((short)((ulonglong)mm4PackedValue1 >> 0x10) +
                           (short)((ulonglong)mm6PackedValue1 >> 0x10)) >> 2;
          uVar39 = (ushort)((short)((ulonglong)mm4PackedValue1 >> 0x20) +
                           (short)((ulonglong)mm6PackedValue1 >> 0x20)) >> 2;
          uVar40 = (ushort)((short)((ulonglong)mm4PackedValue1 >> 0x30) +
                           (short)((ulonglong)mm6PackedValue1 >> 0x30)) >> 2;
          cVar11 = (uVar31 != 0) * (uVar31 < 0x100) * (char)uVar31 - (0xff < uVar31);
          cVar12 = (uVar33 != 0) * (uVar33 < 0x100) * (char)uVar33 - (0xff < uVar33);
          cVar13 = (uVar34 != 0) * (uVar34 < 0x100) * (char)uVar34 - (0xff < uVar34);
          cVar14 = (uVar35 != 0) * (uVar35 < 0x100) * (char)uVar35 - (0xff < uVar35);
          cVar15 = (uVar36 != 0) * (uVar36 < 0x100) * (char)uVar36 - (0xff < uVar36);
          cVar16 = (uVar38 != 0) * (uVar38 < 0x100) * (char)uVar38 - (0xff < uVar38);
          cVar17 = (uVar39 != 0) * (uVar39 < 0x100) * (char)uVar39 - (0xff < uVar39);
          cVar18 = (uVar40 != 0) * (uVar40 < 0x100) * (char)uVar40 - (0xff < uVar40);
          uVar32 = psllw(CONCAT62(CONCAT51(CONCAT41(CONCAT31(CONCAT21(CONCAT11(cVar14,cVar14),cVar13
                                                                     ),cVar13),cVar12),cVar12),
                                  CONCAT11(cVar11,cVar11)),4);
          uVar37 = psllw(CONCAT62(CONCAT51(CONCAT41(CONCAT31(CONCAT21(CONCAT11(cVar18,cVar18),cVar17
                                                                     ),cVar17),cVar16),cVar16),
                                  CONCAT11(cVar15,cVar15)),4);
          mm0PackedValue2 =
               pmaddwd(uVar32 & (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                       g_SoftwarePixelMmxConstants.packWeights);
          mm4PackedValue2 =
               pmaddwd(uVar37 & (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                       g_SoftwarePixelMmxConstants.packWeights);
          *(uint *)pbVar30 =
               CONCAT22((short)((ulonglong)mm4PackedValue2 >> 8) +
                        (short)((ulonglong)mm4PackedValue2 >> 0x28),
                        (short)((ulonglong)mm0PackedValue2 >> 8) +
                        (short)((ulonglong)mm0PackedValue2 >> 0x28));
          uVar24 = uVar24 + iVar21 * 2;
          pbVar30 = pbVar30 + 4;
          uVar20 = uVar20 - 1;
        } while (uVar20 != 0);
        uVar26 = uVar26 + (int)(((ulonglong)(uVar22 >> 0x18) << 0x20 |
                                (ulonglong)uVar22 * 0x100 & 0xffffffff) / (ulonglong)uVar23);
        pbVar30 = pbVar41 + GVar19 * 2;
        pbVar29 = pbVar28 + (uVar26 >> 8) * iVar2;
        uVar24 = 0;
        destinationHeight = destinationHeight - 1;
        uVar20 = destinationWidth >> 1;
        pbVar41 = pbVar30;
      } while (destinationHeight != 0);
    }
  }
  else {
    SoftwareTextureSource_StretchDirectColorBilinear16
              (destinationHeight,destinationWidth,destinationY,destinationX,subresourceIndex,
               sourceAsset,framebuffer);
  }
  return;
}


/* Address: 0x00581A90.
   Ownership: graphics/backend/glide.
   Purpose: Glide3 display implementation of the integer-scaled source-alpha compositor. The display path reads
   destination pixels through g_GlideSecondBufferOffset for partial-alpha blending. The source entry originX and
   originY are multiplied by integerScale before being added to drawX and drawY. Both indexed palette entries and
   direct ARGB8888 entries are supported. For indexed entries, the active palette bank comes from
   sourceEntry.paletteIndex and framebufferPixel is read at palette-entry offset +4.
   Cross-module calls: SoftwareTextureSource_BlitHalfSourceRgb16 [graphics/backend/software].
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_TextureSource_BlitIntegerScaledSourceAlpha
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsIntegerScale integerScale,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  undefined2 uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  dword dVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  int iVar10;
  int iVar11;
  byte *pbVar12;
  byte *pbVar13;
  byte *pbVar14;
  byte *pbVar15;
  ulonglong uVar16;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  byte mm1PackedValue0ByteLane1;
  byte mm1PackedValue1ByteLane1;
  byte mm1PackedValue1ByteLane3;
  undefined8 mm0PackedValue2;
  undefined8 mm0PackedValue3;
  byte mm1PackedValue0ByteLane2;
  byte mm1PackedValue0ByteLane3;
  undefined8 mm1PackedValue0;
  byte mm1PackedValue1ByteLane2;
  undefined8 mm1PackedValue1;
  int iStack_2c;
  GraphicsIntegerScale GStack_24;
  GraphicsIntegerScale GStack_20;
  
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      iVar10 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      iVar1 = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar10 + -0x18) * integerScale;
      drawY = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar10 + -0x14) * integerScale;
      if (clipMinX < 0) {
        clipMinX = 0;
      }
      if (clipMinY < 0) {
        clipMinY = 0;
      }
      if ((int)g_DisplayFramebufferAccess.width < clipMaxX) {
        clipMaxX = g_DisplayFramebufferAccess.width;
      }
      uVar3 = *(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                       iVar10 + -0x20);
      if ((int)g_DisplayFramebufferAccess.height < clipMaxY) {
        clipMaxY = g_DisplayFramebufferAccess.height;
      }
      if ((int)uVar3 < 0) {
        iVar4 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar10 + -0x10);
        iStack_2c = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar10 + -0xc);
        pbVar15 = g_DisplayFramebufferAccess.pixels +
                  (g_DisplayFramebufferAccess.width * drawY + iVar1) * 2;
        iVar5 = g_DisplayFramebufferAccess.width * 2;
        pbVar13 = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                  *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar10 + -0x1c) + -0x28;
        GStack_24 = integerScale;
        do {
          do {
            if ((clipMinY <= drawY) &&
               (iVar10 = iVar4, iVar11 = iVar1, pbVar12 = pbVar13, pbVar14 = pbVar15,
               drawY < clipMaxY)) {
              do {
                GStack_20 = integerScale;
                uVar3 = *(uint *)pbVar12;
                if (uVar3 < 0x1000000) {
                  iVar11 = iVar11 + integerScale;
                  pbVar14 = pbVar14 + integerScale * 2;
                }
                else if (uVar3 < 0xff000000) {
                  do {
                    if ((clipMinX <= iVar11) && (iVar11 < clipMaxX)) {
                      uVar2 = *(undefined2 *)(pbVar14 + g_GlideSecondBufferOffset);
                      mm1PackedValue1ByteLane3 = (byte)(uVar3 >> 0x18);
                      mm1PackedValue1ByteLane2 = (byte)(uVar3 >> 0x10);
                      mm1PackedValue1ByteLane1 = (byte)(uVar3 >> 8);
                      pbVar14 = pbVar14 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                      uVar16 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                               (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                      mm1PackedValue1 =
                           pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,
                                                    mm1PackedValue1ByteLane3) >> 2,
                                           CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                                  ,mm1PackedValue1ByteLane2),
                                                  CONCAT14(mm1PackedValue1ByteLane2,uVar3)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                                         mm1PackedValue1ByteLane1)
                                                                >> 2,CONCAT11((char)uVar3,
                                                                              (char)uVar3) >> 2))),
                                  g_SoftwareBlendAlphaFactors[uVar3 >> 0x18]);
                      mm0PackedValue2 =
                           pmulhw(CONCAT26((ushort)((short)(uVar16 >> 0x30) *
                                                   g_SoftwarePixelMmxConstants.unpackScales.zero) >>
                                           2,CONCAT24((ushort)((short)(uVar16 >> 0x20) *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.red) >> 2,
                                                      CONCAT22((ushort)((short)(uVar16 >> 0x10) *
                                                                       g_SoftwarePixelMmxConstants.
                                                                       unpackScales.green) >> 2,
                                                               (ushort)((short)uVar16 *
                                                                       g_SoftwarePixelMmxConstants.
                                                                       unpackScales.blue) >> 2))),
                                  g_SoftwareBlendInverseAlphaFactors[uVar3 >> 0x18]);
                      mm0PackedValue3 =
                           pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue2 >> 0x30) +
                                            (short)((ulonglong)mm1PackedValue1 >> 0x30),
                                            CONCAT24((short)((ulonglong)mm0PackedValue2 >> 0x20) +
                                                     (short)((ulonglong)mm1PackedValue1 >> 0x20),
                                                     CONCAT22((short)((ulonglong)mm0PackedValue2 >>
                                                                     0x10) +
                                                              (short)((ulonglong)mm1PackedValue1 >>
                                                                     0x10),
                                                              (short)mm0PackedValue2 +
                                                              (short)mm1PackedValue1))) &
                                   (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                                   g_SoftwarePixelMmxConstants.packWeights);
                      *(short *)pbVar14 =
                           (short)((ulonglong)mm0PackedValue3 >> 8) +
                           (short)((ulonglong)mm0PackedValue3 >> 0x28);
                    }
                    iVar11 = iVar11 + 1;
                    pbVar14 = pbVar14 + 2;
                    GStack_20 = GStack_20 - 1;
                  } while (GStack_20 != 0);
                }
                else {
                  dVar7 = g_SoftwarePixelPackTables->blue[uVar3 & 0xff];
                  uVar8 = *(undefined4 *)
                           ((int)g_SoftwarePixelPackTables->green + ((uVar3 & 0xff00) >> 6));
                  uVar9 = *(undefined4 *)
                           ((int)g_SoftwarePixelPackTables->red + ((uVar3 & 0xff0000) >> 0xe));
                  do {
                    if ((clipMinX <= iVar11) && (iVar11 < clipMaxX)) {
                      *(short *)pbVar14 = (short)dVar7 + (short)uVar8 + (short)uVar9;
                    }
                    iVar11 = iVar11 + 1;
                    pbVar14 = pbVar14 + 2;
                    GStack_20 = GStack_20 - 1;
                  } while (GStack_20 != 0);
                }
                iVar10 = iVar10 + -1;
                pbVar12 = pbVar12 + 4;
              } while (iVar10 != 0);
            }
            drawY = drawY + 1;
            pbVar15 = pbVar15 + iVar5;
            GStack_24 = GStack_24 - 1;
          } while (GStack_24 != 0);
          pbVar13 = pbVar13 + iVar4 * 4;
          GStack_24 = integerScale;
          iStack_2c = iStack_2c + -1;
        } while (iStack_2c != 0);
        return;
      }
      if (uVar3 < (sourceAsset->tableDescriptor).paletteBankCount) {
        iVar4 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar10 + -0x10);
        iStack_2c = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar10 + -0xc);
        pbVar15 = g_DisplayFramebufferAccess.pixels +
                  (g_DisplayFramebufferAccess.width * drawY + iVar1) * 2;
        iVar5 = g_DisplayFramebufferAccess.width * 2;
        pbVar13 = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                  *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar10 + -0x1c) + -0x28;
        GStack_24 = integerScale;
        do {
          do {
            if ((clipMinY <= drawY) &&
               (iVar10 = iVar4, iVar11 = iVar1, pbVar12 = pbVar13, pbVar14 = pbVar15,
               drawY < clipMaxY)) {
              do {
                GStack_20 = integerScale;
                uVar6 = *(uint *)(sourceAsset[uVar3 * 4 + 1].common.buildMetadata.
                                  assetRelativeAddressAnchor28 + (uint)*pbVar12 * 8 + -0x24);
                if (uVar6 < 0x1000000) {
                  iVar11 = iVar11 + integerScale;
                  pbVar14 = pbVar14 + integerScale * 2;
                }
                else if (uVar6 < 0xff000000) {
                  uVar6 = *(uint *)(sourceAsset[uVar3 * 4 + 1].common.buildMetadata.
                                    assetRelativeAddressAnchor28 + (uint)*pbVar12 * 8 + -0x28);
                  do {
                    if ((clipMinX <= iVar11) && (iVar11 < clipMaxX)) {
                      uVar2 = *(undefined2 *)(pbVar14 + g_GlideSecondBufferOffset);
                      mm1PackedValue0ByteLane3 = (byte)(uVar6 >> 0x18);
                      mm1PackedValue0ByteLane2 = (byte)(uVar6 >> 0x10);
                      mm1PackedValue0ByteLane1 = (byte)(uVar6 >> 8);
                      pbVar14 = pbVar14 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                      uVar16 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                               (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                      mm1PackedValue0 =
                           pmulhw(CONCAT26(CONCAT11(mm1PackedValue0ByteLane3,
                                                    mm1PackedValue0ByteLane3) >> 2,
                                           CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                                  ,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,uVar6)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                         mm1PackedValue0ByteLane1)
                                                                >> 2,CONCAT11((char)uVar6,
                                                                              (char)uVar6) >> 2))),
                                  g_SoftwareBlendAlphaFactors[uVar6 >> 0x18]);
                      mm0PackedValue0 =
                           pmulhw(CONCAT26((ushort)((short)(uVar16 >> 0x30) *
                                                   g_SoftwarePixelMmxConstants.unpackScales.zero) >>
                                           2,CONCAT24((ushort)((short)(uVar16 >> 0x20) *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.red) >> 2,
                                                      CONCAT22((ushort)((short)(uVar16 >> 0x10) *
                                                                       g_SoftwarePixelMmxConstants.
                                                                       unpackScales.green) >> 2,
                                                               (ushort)((short)uVar16 *
                                                                       g_SoftwarePixelMmxConstants.
                                                                       unpackScales.blue) >> 2))),
                                  g_SoftwareBlendInverseAlphaFactors[uVar6 >> 0x18]);
                      mm0PackedValue1 =
                           pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                                            (short)((ulonglong)mm1PackedValue0 >> 0x30),
                                            CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                                     (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                                     CONCAT22((short)((ulonglong)mm0PackedValue0 >>
                                                                     0x10) +
                                                              (short)((ulonglong)mm1PackedValue0 >>
                                                                     0x10),
                                                              (short)mm0PackedValue0 +
                                                              (short)mm1PackedValue0))) &
                                   (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                                   g_SoftwarePixelMmxConstants.packWeights);
                      *(short *)pbVar14 =
                           (short)((ulonglong)mm0PackedValue1 >> 8) +
                           (short)((ulonglong)mm0PackedValue1 >> 0x28);
                    }
                    iVar11 = iVar11 + 1;
                    pbVar14 = pbVar14 + 2;
                    GStack_20 = GStack_20 - 1;
                  } while (GStack_20 != 0);
                }
                else {
                  do {
                    if ((clipMinX <= iVar11) && (iVar11 < clipMaxX)) {
                      *(short *)pbVar14 = (short)uVar6;
                    }
                    iVar11 = iVar11 + 1;
                    pbVar14 = pbVar14 + 2;
                    GStack_20 = GStack_20 - 1;
                  } while (GStack_20 != 0);
                }
                iVar10 = iVar10 + -1;
                pbVar12 = pbVar12 + 1;
              } while (iVar10 != 0);
            }
            drawY = drawY + 1;
            pbVar15 = pbVar15 + iVar5;
            GStack_24 = GStack_24 - 1;
          } while (GStack_24 != 0);
          pbVar13 = pbVar13 + iVar4;
          GStack_24 = integerScale;
          iStack_2c = iStack_2c + -1;
        } while (iStack_2c != 0);
      }
    }
  }
  else {
    SoftwareTextureSource_BlitHalfSourceRgb16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,integerScale,
               (GraphicsTextureSourceAsset *)subresourceIndex,
               (SoftwareFramebufferAccess *)sourceAsset);
  }
  return;
}


/* Address: 0x00581EF0.
   Ownership: graphics/backend/glide.
   Purpose: Glide3-selected source-alpha blitter with explicit palette-bank selection. Ordinary framebuffer objects
   fall back to the 16-bit software implementation. Direct-color entries behave like the ordinary source-alpha
   blitter and ignore paletteBankIndex. Indexed entries read palette pixels from asset + 0x200 +
   paletteBankIndex*0x800 instead of using sourceEntry.paletteIndex. Transparent pixels are skipped, opaque pixels
   are copied, and partial alpha uses the verified source-alpha blend tables.
   Cross-module calls: SoftwareTextureSource_BlitSourceAlphaPaletteBank16 [graphics/backend/software].
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_TextureSource_BlitSourceAlphaPaletteBank
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PaletteBankIndex paletteBankIndex,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  GraphicsPixelDimension GVar6;
  int iVar7;
  GraphicsPixelDimension GVar8;
  int iVar9;
  byte *pbVar10;
  uint *puVar11;
  byte *pbVar12;
  ulonglong uVar13;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  byte mm1PackedValue0ByteLane1;
  byte mm1PackedValue1ByteLane1;
  byte mm1PackedValue0ByteLane3;
  byte mm1PackedValue1ByteLane3;
  undefined8 mm0PackedValue2;
  undefined8 mm0PackedValue3;
  byte mm1PackedValue0ByteLane2;
  undefined8 mm1PackedValue0;
  byte mm1PackedValue1ByteLane2;
  undefined8 mm1PackedValue1;
  
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      iVar1 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      iVar5 = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar1 + -0x18);
      iVar7 = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar1 + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar1 + -0x20)
          == -1) {
        GVar8 = iVar5 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0x10);
        GVar6 = iVar7 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0xc);
        if (iVar5 < 0) {
          iVar5 = 0;
        }
        if (iVar7 < 0) {
          iVar7 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar8) {
          GVar8 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar6) {
          GVar6 = g_DisplayFramebufferAccess.height;
        }
        if (iVar5 < clipMinX) {
          iVar5 = clipMinX;
        }
        if (iVar7 < clipMinY) {
          iVar7 = clipMinY;
        }
        if (clipMaxX < (int)GVar8) {
          GVar8 = clipMaxX;
        }
        if (clipMaxY < (int)GVar6) {
          GVar6 = clipMaxY;
        }
        iVar9 = GVar8 - iVar5;
        if ((iVar9 != 0 && iVar5 <= (int)GVar8) &&
           (clipMinY = GVar6 - iVar7, clipMinY != 0 && iVar7 <= (int)GVar6)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar12 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar7 + iVar5) * 2;
          puVar11 = (uint *)((int)sourceAsset +
                            ((iVar7 - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar3 * 4 +
                            ((iVar5 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar9;
          iVar5 = iVar9;
          do {
            do {
              uVar4 = *puVar11;
              if (0xffffff < uVar4) {
                if (uVar4 < 0xff000000) {
                  uVar2 = *(undefined2 *)(pbVar12 + g_GlideSecondBufferOffset);
                  mm1PackedValue1ByteLane3 = (byte)(uVar4 >> 0x18);
                  mm1PackedValue1ByteLane2 = (byte)(uVar4 >> 0x10);
                  mm1PackedValue1ByteLane1 = (byte)(uVar4 >> 8);
                  pbVar12 = pbVar12 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                  uVar13 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                           (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                  mm1PackedValue1 =
                       pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                       >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                                  ,mm1PackedValue1ByteLane2),
                                                  CONCAT14(mm1PackedValue1ByteLane2,uVar4)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                                         mm1PackedValue1ByteLane1)
                                                                >> 2,CONCAT11((char)uVar4,
                                                                              (char)uVar4) >> 2))),
                              g_SoftwareBlendAlphaFactors[uVar4 >> 0x18]);
                  mm0PackedValue2 =
                       pmulhw(CONCAT26((ushort)((short)(uVar13 >> 0x30) *
                                               g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                       CONCAT24((ushort)((short)(uVar13 >> 0x20) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.red
                                                        ) >> 2,
                                                CONCAT22((ushort)((short)(uVar13 >> 0x10) *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.green) >> 2,
                                                         (ushort)((short)uVar13 *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.blue) >> 2))),
                              g_SoftwareBlendInverseAlphaFactors[uVar4 >> 0x18]);
                  mm0PackedValue3 =
                       pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue2 >> 0x30) +
                                        (short)((ulonglong)mm1PackedValue1 >> 0x30),
                                        CONCAT24((short)((ulonglong)mm0PackedValue2 >> 0x20) +
                                                 (short)((ulonglong)mm1PackedValue1 >> 0x20),
                                                 CONCAT22((short)((ulonglong)mm0PackedValue2 >> 0x10
                                                                 ) +
                                                          (short)((ulonglong)mm1PackedValue1 >> 0x10
                                                                 ),(short)mm0PackedValue2 +
                                                                   (short)mm1PackedValue1))) &
                               (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                               g_SoftwarePixelMmxConstants.packWeights);
                  *(short *)pbVar12 =
                       (short)((ulonglong)mm0PackedValue3 >> 8) +
                       (short)((ulonglong)mm0PackedValue3 >> 0x28);
                }
                else {
                  *(short *)pbVar12 =
                       (short)g_SoftwarePixelPackTables->blue[uVar4 & 0xff] +
                       (short)*(undefined4 *)
                               ((int)g_SoftwarePixelPackTables->green + ((uVar4 & 0xff00) >> 6)) +
                       (short)*(undefined4 *)
                               ((int)g_SoftwarePixelPackTables->red + ((uVar4 & 0xff0000) >> 0xe));
                }
              }
              puVar11 = puVar11 + 1;
              pbVar12 = pbVar12 + 2;
              iVar5 = iVar5 + -1;
            } while (iVar5 != 0);
            puVar11 = puVar11 + (iVar3 - iVar9);
            pbVar12 = pbVar12 + iVar1 * 2;
            clipMinY = clipMinY + -1;
            iVar5 = iVar9;
          } while (clipMinY != 0);
          return;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar1 + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        GVar8 = iVar5 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0x10);
        GVar6 = iVar7 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0xc);
        if (iVar5 < 0) {
          iVar5 = 0;
        }
        if (iVar7 < 0) {
          iVar7 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar8) {
          GVar8 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar6) {
          GVar6 = g_DisplayFramebufferAccess.height;
        }
        if (iVar5 < clipMinX) {
          iVar5 = clipMinX;
        }
        if (iVar7 < clipMinY) {
          iVar7 = clipMinY;
        }
        if (clipMaxX < (int)GVar8) {
          GVar8 = clipMaxX;
        }
        if (clipMaxY < (int)GVar6) {
          GVar6 = clipMaxY;
        }
        iVar9 = GVar8 - iVar5;
        if ((iVar9 != 0 && iVar5 <= (int)GVar8) &&
           (clipMinY = GVar6 - iVar7, clipMinY != 0 && iVar7 <= (int)GVar6)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar12 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar7 + iVar5) * 2;
          pbVar10 = (byte *)((int)sourceAsset +
                            ((iVar7 - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar3 +
                            ((iVar5 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar9;
          iVar5 = iVar9;
          if (paletteBankIndex < (sourceAsset->tableDescriptor).paletteBankCount) {
            do {
              do {
                uVar4 = *(uint *)(sourceAsset[paletteBankIndex * 4 + 1].common.buildMetadata.
                                  assetRelativeAddressAnchor28 + (uint)*pbVar10 * 8 + -0x24);
                if (0xffffff < uVar4) {
                  if (uVar4 < 0xff000000) {
                    uVar4 = *(uint *)(sourceAsset[paletteBankIndex * 4 + 1].common.buildMetadata.
                                      assetRelativeAddressAnchor28 + (uint)*pbVar10 * 8 + -0x28);
                    uVar2 = *(undefined2 *)(pbVar12 + g_GlideSecondBufferOffset);
                    mm1PackedValue0ByteLane3 = (byte)(uVar4 >> 0x18);
                    mm1PackedValue0ByteLane2 = (byte)(uVar4 >> 0x10);
                    mm1PackedValue0ByteLane1 = (byte)(uVar4 >> 8);
                    pbVar12 = pbVar12 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                    uVar13 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                             (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                    mm1PackedValue0 =
                         pmulhw(CONCAT26(CONCAT11(mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                         >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                                  ,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,uVar4)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                         mm1PackedValue0ByteLane1)
                                                                >> 2,CONCAT11((char)uVar4,
                                                                              (char)uVar4) >> 2))),
                                g_SoftwareBlendAlphaFactors[uVar4 >> 0x18]);
                    mm0PackedValue0 =
                         pmulhw(CONCAT26((ushort)((short)(uVar13 >> 0x30) *
                                                 g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2
                                         ,CONCAT24((ushort)((short)(uVar13 >> 0x20) *
                                                           g_SoftwarePixelMmxConstants.unpackScales.
                                                           red) >> 2,
                                                   CONCAT22((ushort)((short)(uVar13 >> 0x10) *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.green) >> 2,
                                                            (ushort)((short)uVar13 *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.blue) >> 2))),
                                g_SoftwareBlendInverseAlphaFactors[uVar4 >> 0x18]);
                    mm0PackedValue1 =
                         pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                                          (short)((ulonglong)mm1PackedValue0 >> 0x30),
                                          CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                                   (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                                   CONCAT22((short)((ulonglong)mm0PackedValue0 >>
                                                                   0x10) +
                                                            (short)((ulonglong)mm1PackedValue0 >>
                                                                   0x10),
                                                            (short)mm0PackedValue0 +
                                                            (short)mm1PackedValue0))) &
                                 (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                                 g_SoftwarePixelMmxConstants.packWeights);
                    *(short *)pbVar12 =
                         (short)((ulonglong)mm0PackedValue1 >> 8) +
                         (short)((ulonglong)mm0PackedValue1 >> 0x28);
                  }
                  else {
                    *(short *)pbVar12 = (short)uVar4;
                  }
                }
                pbVar10 = pbVar10 + 1;
                pbVar12 = pbVar12 + 2;
                iVar5 = iVar5 + -1;
              } while (iVar5 != 0);
              pbVar10 = pbVar10 + (iVar3 - iVar9);
              pbVar12 = pbVar12 + iVar1 * 2;
              clipMinY = clipMinY + -1;
              iVar5 = iVar9;
            } while (clipMinY != 0);
          }
        }
      }
    }
  }
  else {
    SoftwareTextureSource_BlitSourceAlphaPaletteBank16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,paletteBankIndex,subresourceIndex,
               sourceAsset,framebuffer);
  }
  return;
}


/* Address: 0x00582290.
   Ownership: graphics/backend/glide.
   Purpose: Glide3-selected saturated-additive RGB compositor. Ordinary framebuffer objects fall back to
   SoftwareTextureSource_BlitSaturatedAddRgb16; the shared display path reads destination pixels through the
   optional second Glide buffer. Pixels whose source RGB value is exactly 0x000000 are skipped, regardless of
   source alpha. For each drawn channel: destination = min(255, destination + sourceContribution). The source alpha
   byte does not participate in the operation.
   Cross-module calls: SoftwareTextureSource_BlitSaturatedAddRgb16 [graphics/backend/software].
*/
bool __thandor_cf_preserve_eax_ecx_edx
Glide3_TextureSource_BlitSaturatedAddRgb
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  GraphicsPixelDimension GVar7;
  int iVar8;
  GraphicsPixelDimension GVar9;
  int iVar10;
  byte *pbVar11;
  uint *puVar12;
  byte *pbVar13;
  ulonglong uVar14;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  undefined8 mm0PackedValue2;
  undefined8 mm0PackedValue3;
  undefined1 uVar15;
  undefined1 uVar16;
  undefined1 uVar17;
  
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      iVar1 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      iVar6 = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar1 + -0x18);
      iVar8 = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar1 + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar1 + -0x20)
          == -1) {
        GVar9 = iVar6 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0x10);
        GVar7 = iVar8 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0xc);
        if (iVar6 < 0) {
          iVar6 = 0;
        }
        if (iVar8 < 0) {
          iVar8 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar9) {
          GVar9 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar7) {
          GVar7 = g_DisplayFramebufferAccess.height;
        }
        if (iVar6 < clipMinX) {
          iVar6 = clipMinX;
        }
        if (iVar8 < clipMinY) {
          iVar8 = clipMinY;
        }
        if (clipMaxX < (int)GVar9) {
          GVar9 = clipMaxX;
        }
        if (clipMaxY < (int)GVar7) {
          GVar7 = clipMaxY;
        }
        iVar10 = GVar9 - iVar6;
        if ((iVar10 != 0 && iVar6 <= (int)GVar9) &&
           (clipMinY = GVar7 - iVar8, clipMinY != 0 && iVar8 <= (int)GVar7)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar13 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar8 + iVar6) * 2;
          puVar12 = (uint *)((int)sourceAsset +
                            ((iVar8 - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar3 * 4 +
                            ((iVar6 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar10;
          iVar6 = iVar10;
          do {
            do {
              uVar5 = *puVar12;
              if ((uVar5 & 0xffffff) != 0) {
                uVar2 = *(undefined2 *)(pbVar13 + g_GlideSecondBufferOffset);
                uVar17 = (undefined1)(uVar5 >> 0x18);
                uVar16 = (undefined1)(uVar5 >> 0x10);
                uVar15 = (undefined1)(uVar5 >> 8);
                pbVar13 = pbVar13 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                uVar14 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                         (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                mm0PackedValue2 =
                     paddusw(CONCAT26((short)(uVar14 >> 0x30) *
                                      g_SoftwarePixelMmxConstants.unpackScales.zero,
                                      CONCAT24((short)(uVar14 >> 0x20) *
                                               g_SoftwarePixelMmxConstants.unpackScales.red,
                                               CONCAT22((short)(uVar14 >> 0x10) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.
                                                        green,(short)uVar14 *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.blue))),
                             CONCAT62(CONCAT51(CONCAT41((int)(CONCAT35(CONCAT21(CONCAT11(uVar17,
                                                  uVar17),uVar16),CONCAT14(uVar16,uVar5)) >> 0x20),
                                                  uVar15),uVar15),CONCAT11((char)uVar5,(char)uVar5))
                            );
                mm0PackedValue3 =
                     pmaddwd(CONCAT26((ushort)((ulonglong)mm0PackedValue2 >> 0x34),
                                      CONCAT24((ushort)((ulonglong)mm0PackedValue2 >> 0x20) >> 4,
                                               CONCAT22((ushort)((ulonglong)mm0PackedValue2 >> 0x10)
                                                        >> 4,(ushort)mm0PackedValue2 >> 4))) &
                             (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)pbVar13 =
                     (short)((ulonglong)mm0PackedValue3 >> 8) +
                     (short)((ulonglong)mm0PackedValue3 >> 0x28);
              }
              puVar12 = puVar12 + 1;
              pbVar13 = pbVar13 + 2;
              iVar6 = iVar6 + -1;
            } while (iVar6 != 0);
            puVar12 = puVar12 + (iVar3 - iVar10);
            pbVar13 = pbVar13 + iVar1 * 2;
            clipMinY = clipMinY + -1;
            iVar6 = iVar10;
          } while (clipMinY != 0);
          return false;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar1 + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        GVar9 = iVar6 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0x10);
        GVar7 = iVar8 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0xc);
        if (iVar6 < 0) {
          iVar6 = 0;
        }
        if (iVar8 < 0) {
          iVar8 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar9) {
          GVar9 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar7) {
          GVar7 = g_DisplayFramebufferAccess.height;
        }
        if (iVar6 < clipMinX) {
          iVar6 = clipMinX;
        }
        if (iVar8 < clipMinY) {
          iVar8 = clipMinY;
        }
        if (clipMaxX < (int)GVar9) {
          GVar9 = clipMaxX;
        }
        if (clipMaxY < (int)GVar7) {
          GVar7 = clipMaxY;
        }
        iVar10 = GVar9 - iVar6;
        if ((iVar10 != 0 && iVar6 <= (int)GVar9) &&
           (clipMinY = GVar7 - iVar8, clipMinY != 0 && iVar8 <= (int)GVar7)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x20);
          iVar4 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar13 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar8 + iVar6) * 2;
          pbVar11 = (byte *)((int)sourceAsset +
                            ((iVar8 - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar4 +
                            ((iVar6 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar10;
          iVar6 = iVar10;
          do {
            do {
              uVar5 = *(uint *)(sourceAsset[iVar3 * 4 + 1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)*pbVar11 * 8 + -0x28);
              if ((uVar5 & 0xffffff) != 0) {
                uVar2 = *(undefined2 *)(pbVar13 + g_GlideSecondBufferOffset);
                uVar17 = (undefined1)(uVar5 >> 0x18);
                uVar16 = (undefined1)(uVar5 >> 0x10);
                uVar15 = (undefined1)(uVar5 >> 8);
                pbVar13 = pbVar13 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                uVar14 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                         (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                mm0PackedValue0 =
                     paddusw(CONCAT26((short)(uVar14 >> 0x30) *
                                      g_SoftwarePixelMmxConstants.unpackScales.zero,
                                      CONCAT24((short)(uVar14 >> 0x20) *
                                               g_SoftwarePixelMmxConstants.unpackScales.red,
                                               CONCAT22((short)(uVar14 >> 0x10) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.
                                                        green,(short)uVar14 *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.blue))),
                             CONCAT62(CONCAT51(CONCAT41((int)(CONCAT35(CONCAT21(CONCAT11(uVar17,
                                                  uVar17),uVar16),CONCAT14(uVar16,uVar5)) >> 0x20),
                                                  uVar15),uVar15),CONCAT11((char)uVar5,(char)uVar5))
                            );
                mm0PackedValue1 =
                     pmaddwd(CONCAT26((ushort)((ulonglong)mm0PackedValue0 >> 0x34),
                                      CONCAT24((ushort)((ulonglong)mm0PackedValue0 >> 0x20) >> 4,
                                               CONCAT22((ushort)((ulonglong)mm0PackedValue0 >> 0x10)
                                                        >> 4,(ushort)mm0PackedValue0 >> 4))) &
                             (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)pbVar13 =
                     (short)((ulonglong)mm0PackedValue1 >> 8) +
                     (short)((ulonglong)mm0PackedValue1 >> 0x28);
              }
              pbVar11 = pbVar11 + 1;
              pbVar13 = pbVar13 + 2;
              iVar6 = iVar6 + -1;
            } while (iVar6 != 0);
            pbVar11 = pbVar11 + (iVar4 - iVar10);
            pbVar13 = pbVar13 + iVar1 * 2;
            clipMinY = clipMinY + -1;
            iVar6 = iVar10;
          } while (clipMinY != 0);
        }
      }
    }
  }
  else {
    SoftwareTextureSource_BlitSaturatedAddRgb16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,subresourceIndex,sourceAsset,
               framebuffer);
  }
  return false;
}


/* Address: 0x00582580.
   Ownership: graphics/backend/glide.
   Purpose: Glide3-selected half-source-RGB saturated-add compositor. Ordinary framebuffer objects fall back to
   SoftwareTextureSource_BlitHalfRgbSaturatedAdd16. Pixels whose source RGB value is exactly 0x000000 are skipped,
   regardless of source alpha. For each drawn channel: destination = min(255, destination + sourceContribution).
   The source alpha byte does not participate in the operation.
   Cross-module calls: SoftwareTextureSource_BlitHalfRgbSaturatedAdd16 [graphics/backend/software].
*/
bool __thandor_cf_preserve_eax_ecx_edx
Glide3_TextureSource_BlitHalfRgbSaturatedAdd
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  ushort uVar6;
  int iVar7;
  GraphicsPixelDimension GVar8;
  int iVar9;
  GraphicsPixelDimension GVar10;
  int iVar11;
  byte *pbVar12;
  uint *puVar13;
  byte *pbVar14;
  ulonglong uVar15;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  undefined8 mm0PackedValue2;
  undefined8 mm0PackedValue3;
  undefined1 uVar16;
  undefined1 uVar17;
  
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      iVar1 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      iVar7 = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar1 + -0x18);
      iVar9 = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar1 + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar1 + -0x20)
          == -1) {
        GVar10 = iVar7 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 iVar1 + -0x10);
        GVar8 = iVar9 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0xc);
        if (iVar7 < 0) {
          iVar7 = 0;
        }
        if (iVar9 < 0) {
          iVar9 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar10) {
          GVar10 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar8) {
          GVar8 = g_DisplayFramebufferAccess.height;
        }
        if (iVar7 < clipMinX) {
          iVar7 = clipMinX;
        }
        if (iVar9 < clipMinY) {
          iVar9 = clipMinY;
        }
        if (clipMaxX < (int)GVar10) {
          GVar10 = clipMaxX;
        }
        if (clipMaxY < (int)GVar8) {
          GVar8 = clipMaxY;
        }
        iVar11 = GVar10 - iVar7;
        if ((iVar11 != 0 && iVar7 <= (int)GVar10) &&
           (clipMinY = GVar8 - iVar9, clipMinY != 0 && iVar9 <= (int)GVar8)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar14 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar9 + iVar7) * 2;
          puVar13 = (uint *)((int)sourceAsset +
                            ((iVar9 - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar3 * 4 +
                            ((iVar7 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar11;
          iVar7 = iVar11;
          do {
            do {
              uVar5 = *puVar13;
              if ((uVar5 & 0xffffff) != 0) {
                uVar2 = *(undefined2 *)(pbVar14 + g_GlideSecondBufferOffset);
                uVar16 = (undefined1)(uVar5 >> 0x18);
                uVar6 = CONCAT11(uVar16,uVar16);
                uVar17 = (undefined1)(uVar5 >> 0x10);
                uVar16 = (undefined1)(uVar5 >> 8);
                pbVar14 = pbVar14 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                uVar15 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                         (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                mm0PackedValue2 =
                     paddusw(CONCAT26((short)(uVar15 >> 0x30) *
                                      g_SoftwarePixelMmxConstants.unpackScales.zero,
                                      CONCAT24((short)(uVar15 >> 0x20) *
                                               g_SoftwarePixelMmxConstants.unpackScales.red,
                                               CONCAT22((short)(uVar15 >> 0x10) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.
                                                        green,(short)uVar15 *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.blue))),
                             CONCAT26(uVar6 >> 1,
                                      CONCAT24((ushort)(CONCAT35(CONCAT21(uVar6,uVar17),
                                                                 CONCAT14(uVar17,uVar5)) >> 0x20) >>
                                               1,CONCAT22(CONCAT11(uVar16,uVar16) >> 1,
                                                          CONCAT11((char)uVar5,(char)uVar5) >> 1))))
                ;
                mm0PackedValue3 =
                     pmaddwd(CONCAT26((ushort)((ulonglong)mm0PackedValue2 >> 0x34),
                                      CONCAT24((ushort)((ulonglong)mm0PackedValue2 >> 0x20) >> 4,
                                               CONCAT22((ushort)((ulonglong)mm0PackedValue2 >> 0x10)
                                                        >> 4,(ushort)mm0PackedValue2 >> 4))) &
                             (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)pbVar14 =
                     (short)((ulonglong)mm0PackedValue3 >> 8) +
                     (short)((ulonglong)mm0PackedValue3 >> 0x28);
              }
              puVar13 = puVar13 + 1;
              pbVar14 = pbVar14 + 2;
              iVar7 = iVar7 + -1;
            } while (iVar7 != 0);
            puVar13 = puVar13 + (iVar3 - iVar11);
            pbVar14 = pbVar14 + iVar1 * 2;
            clipMinY = clipMinY + -1;
            iVar7 = iVar11;
          } while (clipMinY != 0);
          return false;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar1 + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        GVar10 = iVar7 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 iVar1 + -0x10);
        GVar8 = iVar9 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar1 + -0xc);
        if (iVar7 < 0) {
          iVar7 = 0;
        }
        if (iVar9 < 0) {
          iVar9 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar10) {
          GVar10 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar8) {
          GVar8 = g_DisplayFramebufferAccess.height;
        }
        if (iVar7 < clipMinX) {
          iVar7 = clipMinX;
        }
        if (iVar9 < clipMinY) {
          iVar9 = clipMinY;
        }
        if (clipMaxX < (int)GVar10) {
          GVar10 = clipMaxX;
        }
        if (clipMaxY < (int)GVar8) {
          GVar8 = clipMaxY;
        }
        iVar11 = GVar10 - iVar7;
        if ((iVar11 != 0 && iVar7 <= (int)GVar10) &&
           (clipMinY = GVar8 - iVar9, clipMinY != 0 && iVar9 <= (int)GVar8)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x20);
          iVar4 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar14 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar9 + iVar7) * 2;
          pbVar12 = (byte *)((int)sourceAsset +
                            ((iVar9 - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar4 +
                            ((iVar7 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar11;
          iVar7 = iVar11;
          do {
            do {
              uVar5 = *(uint *)(sourceAsset[iVar3 * 4 + 1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)*pbVar12 * 8 + -0x28);
              if ((uVar5 & 0xffffff) != 0) {
                uVar2 = *(undefined2 *)(pbVar14 + g_GlideSecondBufferOffset);
                uVar16 = (undefined1)(uVar5 >> 0x18);
                uVar6 = CONCAT11(uVar16,uVar16);
                uVar17 = (undefined1)(uVar5 >> 0x10);
                uVar16 = (undefined1)(uVar5 >> 8);
                pbVar14 = pbVar14 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                uVar15 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                         (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                mm0PackedValue0 =
                     paddusw(CONCAT26((short)(uVar15 >> 0x30) *
                                      g_SoftwarePixelMmxConstants.unpackScales.zero,
                                      CONCAT24((short)(uVar15 >> 0x20) *
                                               g_SoftwarePixelMmxConstants.unpackScales.red,
                                               CONCAT22((short)(uVar15 >> 0x10) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.
                                                        green,(short)uVar15 *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.blue))),
                             CONCAT26(uVar6 >> 1,
                                      CONCAT24((ushort)(CONCAT35(CONCAT21(uVar6,uVar17),
                                                                 CONCAT14(uVar17,uVar5)) >> 0x20) >>
                                               1,CONCAT22(CONCAT11(uVar16,uVar16) >> 1,
                                                          CONCAT11((char)uVar5,(char)uVar5) >> 1))))
                ;
                mm0PackedValue1 =
                     pmaddwd(CONCAT26((ushort)((ulonglong)mm0PackedValue0 >> 0x34),
                                      CONCAT24((ushort)((ulonglong)mm0PackedValue0 >> 0x20) >> 4,
                                               CONCAT22((ushort)((ulonglong)mm0PackedValue0 >> 0x10)
                                                        >> 4,(ushort)mm0PackedValue0 >> 4))) &
                             (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)pbVar14 =
                     (short)((ulonglong)mm0PackedValue1 >> 8) +
                     (short)((ulonglong)mm0PackedValue1 >> 0x28);
              }
              pbVar12 = pbVar12 + 1;
              pbVar14 = pbVar14 + 2;
              iVar7 = iVar7 + -1;
            } while (iVar7 != 0);
            pbVar12 = pbVar12 + (iVar4 - iVar11);
            pbVar14 = pbVar14 + iVar1 * 2;
            clipMinY = clipMinY + -1;
            iVar7 = iVar11;
          } while (clipMinY != 0);
        }
      }
    }
  }
  else {
    SoftwareTextureSource_BlitHalfRgbSaturatedAdd16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,subresourceIndex,sourceAsset,
               framebuffer);
  }
  return false;
}


/* Address: 0x00582870.
   Ownership: graphics/backend/glide.
   Purpose: Uses the color-modulated Glide display path when framebuffer is the shared display framebuffer. For
   other framebuffer objects the executable deliberately forwards the nine non-modulation arguments to the existing
   software source blitter at 004ABA70. For each source channel: outputChannel = (sourceChannel *
   modulationChannel) >> 8. The modulated alpha then selects transparent, opaque, or partial source-alpha blending.
   Both indexed palette pixels and direct ARGB8888 pixels are supported.
   Cross-module calls: SoftwareTextureSource_BlitHalfRgbSaturatedAdd16 [graphics/backend/software].
*/
bool __thandor_cf_preserve_eax_ecx_edx
Glide3_TextureSource_BlitModulatedSourceAlpha
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PackedArgb32 modulationArgb8888,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  GraphicsPixelDimension GVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  GraphicsPixelDimension GVar16;
  int iVar17;
  uint uVar18;
  byte *pbVar19;
  uint *puVar20;
  byte *pbVar21;
  ulonglong uVar22;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  undefined8 mm0PackedValue2;
  undefined8 mm0PackedValue3;
  byte mm1PackedValue0ByteLane0;
  byte mm1PackedValue1ByteLane0;
  byte mm1PackedValue0ByteLane3;
  byte mm1PackedValue1ByteLane3;
  byte mm1PackedValue1ByteLane1;
  byte mm1PackedValue0ByteLane1;
  byte mm1PackedValue0ByteLane2;
  undefined8 mm1PackedValue0;
  byte mm1PackedValue1ByteLane2;
  undefined8 mm1PackedValue1;
  
  if (framebuffer == &g_DisplayFramebufferAccess) {
    uVar15 = (modulationArgb8888 & 0xff00) >> 8;
    uVar9 = (modulationArgb8888 & 0xff0000) >> 0x10;
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      iVar1 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      iVar5 = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar1 + -0x18);
      iVar12 = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                               iVar1 + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar1 + -0x20)
          == -1) {
        GVar16 = iVar5 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 iVar1 + -0x10);
        GVar10 = iVar12 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28
                                  + iVar1 + -0xc);
        if (iVar5 < 0) {
          iVar5 = 0;
        }
        if (iVar12 < 0) {
          iVar12 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar16) {
          GVar16 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar10) {
          GVar10 = g_DisplayFramebufferAccess.height;
        }
        if (iVar5 < clipMinX) {
          iVar5 = clipMinX;
        }
        if (iVar12 < clipMinY) {
          iVar12 = clipMinY;
        }
        if (clipMaxX < (int)GVar16) {
          GVar16 = clipMaxX;
        }
        if (clipMaxY < (int)GVar10) {
          GVar10 = clipMaxY;
        }
        iVar17 = GVar16 - iVar5;
        if ((iVar17 != 0 && iVar5 <= (int)GVar16) &&
           (clipMinY = GVar10 - iVar12, clipMinY != 0 && iVar12 <= (int)GVar10)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar21 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar12 + iVar5) * 2;
          puVar20 = (uint *)((int)sourceAsset +
                            ((iVar12 - *(int *)((sourceAsset->common).buildMetadata.
                                                assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar3 * 4 +
                            ((iVar5 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar17;
          iVar5 = iVar17;
          do {
            do {
              uVar7 = *puVar20;
              uVar6 = (uVar7 & 0xff) * (modulationArgb8888 & 0xff);
              uVar13 = (uVar7 >> 0x18) * (modulationArgb8888 >> 0x18);
              uVar18 = ((uVar7 & 0xff00) >> 8) * uVar15 & 0xff00;
              uVar11 = ((uVar7 & 0xff0000) >> 0x10) * uVar9 & 0xff00;
              uVar14 = uVar13 & 0xff00;
              uVar7 = uVar6 >> 8;
              uVar8 = uVar7 | uVar18 | uVar11 << 8 | uVar14 << 0x10;
              if (0xffffff < uVar8) {
                if (uVar8 < 0xff000000) {
                  uVar2 = *(undefined2 *)(pbVar21 + g_GlideSecondBufferOffset);
                  mm1PackedValue1ByteLane3 = (byte)(uVar14 >> 8);
                  mm1PackedValue1ByteLane2 = (byte)(uVar11 >> 8);
                  mm1PackedValue1ByteLane1 = (byte)(uVar18 >> 8);
                  mm1PackedValue1ByteLane0 = (byte)(uVar6 >> 8);
                  pbVar21 = pbVar21 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                  uVar22 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                           (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                  uVar13 = uVar13 >> 8;
                  mm1PackedValue1 =
                       pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                       >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                                  ,mm1PackedValue1ByteLane2),
                                                  CONCAT14(mm1PackedValue1ByteLane2,uVar8)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                                         mm1PackedValue1ByteLane1)
                                                                >> 2,CONCAT11(
                                                  mm1PackedValue1ByteLane0,mm1PackedValue1ByteLane0)
                                                  >> 2))),g_SoftwareBlendAlphaFactors[uVar13]);
                  mm0PackedValue2 =
                       pmulhw(CONCAT26((ushort)((short)(uVar22 >> 0x30) *
                                               g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                       CONCAT24((ushort)((short)(uVar22 >> 0x20) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.red
                                                        ) >> 2,
                                                CONCAT22((ushort)((short)(uVar22 >> 0x10) *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.green) >> 2,
                                                         (ushort)((short)uVar22 *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.blue) >> 2))),
                              g_SoftwareBlendInverseAlphaFactors[uVar13]);
                  mm0PackedValue3 =
                       pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue2 >> 0x30) +
                                        (short)((ulonglong)mm1PackedValue1 >> 0x30),
                                        CONCAT24((short)((ulonglong)mm0PackedValue2 >> 0x20) +
                                                 (short)((ulonglong)mm1PackedValue1 >> 0x20),
                                                 CONCAT22((short)((ulonglong)mm0PackedValue2 >> 0x10
                                                                 ) +
                                                          (short)((ulonglong)mm1PackedValue1 >> 0x10
                                                                 ),(short)mm0PackedValue2 +
                                                                   (short)mm1PackedValue1))) &
                               (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                               g_SoftwarePixelMmxConstants.packWeights);
                  *(short *)pbVar21 =
                       (short)((ulonglong)mm0PackedValue3 >> 8) +
                       (short)((ulonglong)mm0PackedValue3 >> 0x28);
                }
                else {
                  *(short *)pbVar21 =
                       (short)g_SoftwarePixelPackTables->blue[uVar7] +
                       (short)*(undefined4 *)((int)g_SoftwarePixelPackTables->green + (uVar18 >> 6))
                       + (short)*(undefined4 *)((int)g_SoftwarePixelPackTables->red + (uVar11 >> 6))
                  ;
                }
              }
              puVar20 = puVar20 + 1;
              pbVar21 = pbVar21 + 2;
              iVar5 = iVar5 + -1;
            } while (iVar5 != 0);
            puVar20 = puVar20 + (iVar3 - iVar17);
            pbVar21 = pbVar21 + iVar1 * 2;
            clipMinY = clipMinY + -1;
            iVar5 = iVar17;
          } while (clipMinY != 0);
          return false;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar1 + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        GVar16 = iVar5 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 iVar1 + -0x10);
        GVar10 = iVar12 + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28
                                  + iVar1 + -0xc);
        if (iVar5 < 0) {
          iVar5 = 0;
        }
        if (iVar12 < 0) {
          iVar12 = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)GVar16) {
          GVar16 = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)GVar10) {
          GVar10 = g_DisplayFramebufferAccess.height;
        }
        if (iVar5 < clipMinX) {
          iVar5 = clipMinX;
        }
        if (iVar12 < clipMinY) {
          iVar12 = clipMinY;
        }
        if (clipMaxX < (int)GVar16) {
          GVar16 = clipMaxX;
        }
        if (clipMaxY < (int)GVar10) {
          GVar10 = clipMaxY;
        }
        iVar17 = GVar16 - iVar5;
        if ((iVar17 != 0 && iVar5 <= (int)GVar16) &&
           (clipMinY = GVar10 - iVar12, clipMinY != 0 && iVar12 <= (int)GVar10)) {
          iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x20);
          iVar4 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar1 + -0x10);
          pbVar21 = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * iVar12 + iVar5) * 2;
          pbVar19 = (byte *)((int)sourceAsset +
                            ((iVar12 - *(int *)((sourceAsset->common).buildMetadata.
                                                assetRelativeAddressAnchor28 + iVar1 + -0x14)) -
                            drawY) * iVar4 +
                            ((iVar5 - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + iVar1 + -0x1c));
          iVar1 = g_DisplayFramebufferAccess.width - iVar17;
          iVar5 = iVar17;
          do {
            do {
              uVar7 = *(uint *)(sourceAsset[iVar3 * 4 + 1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)*pbVar19 * 8 + -0x28);
              uVar6 = (uVar7 & 0xff) * (modulationArgb8888 & 0xff);
              uVar13 = (uVar7 >> 0x18) * (modulationArgb8888 >> 0x18);
              uVar18 = ((uVar7 & 0xff00) >> 8) * uVar15 & 0xff00;
              uVar11 = ((uVar7 & 0xff0000) >> 0x10) * uVar9 & 0xff00;
              uVar14 = uVar13 & 0xff00;
              uVar7 = uVar6 >> 8;
              uVar8 = uVar7 | uVar18 | uVar11 << 8 | uVar14 << 0x10;
              if (0xffffff < uVar8) {
                if (uVar8 < 0xff000000) {
                  uVar2 = *(undefined2 *)(pbVar21 + g_GlideSecondBufferOffset);
                  mm1PackedValue0ByteLane3 = (byte)(uVar14 >> 8);
                  mm1PackedValue0ByteLane2 = (byte)(uVar11 >> 8);
                  mm1PackedValue0ByteLane1 = (byte)(uVar18 >> 8);
                  mm1PackedValue0ByteLane0 = (byte)(uVar6 >> 8);
                  pbVar21 = pbVar21 + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                  uVar22 = CONCAT44(CONCAT22(uVar2,uVar2),CONCAT22(uVar2,uVar2)) &
                           (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
                  uVar13 = uVar13 >> 8;
                  mm1PackedValue0 =
                       pmulhw(CONCAT26(CONCAT11(mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                       >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                                  ,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,uVar8)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                         mm1PackedValue0ByteLane1)
                                                                >> 2,CONCAT11(
                                                  mm1PackedValue0ByteLane0,mm1PackedValue0ByteLane0)
                                                  >> 2))),g_SoftwareBlendAlphaFactors[uVar13]);
                  mm0PackedValue0 =
                       pmulhw(CONCAT26((ushort)((short)(uVar22 >> 0x30) *
                                               g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                       CONCAT24((ushort)((short)(uVar22 >> 0x20) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.red
                                                        ) >> 2,
                                                CONCAT22((ushort)((short)(uVar22 >> 0x10) *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.green) >> 2,
                                                         (ushort)((short)uVar22 *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.blue) >> 2))),
                              g_SoftwareBlendInverseAlphaFactors[uVar13]);
                  mm0PackedValue1 =
                       pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                                        (short)((ulonglong)mm1PackedValue0 >> 0x30),
                                        CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                                 (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                                 CONCAT22((short)((ulonglong)mm0PackedValue0 >> 0x10
                                                                 ) +
                                                          (short)((ulonglong)mm1PackedValue0 >> 0x10
                                                                 ),(short)mm0PackedValue0 +
                                                                   (short)mm1PackedValue0))) &
                               (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                               g_SoftwarePixelMmxConstants.packWeights);
                  *(short *)pbVar21 =
                       (short)((ulonglong)mm0PackedValue1 >> 8) +
                       (short)((ulonglong)mm0PackedValue1 >> 0x28);
                }
                else {
                  *(short *)pbVar21 =
                       (short)g_SoftwarePixelPackTables->blue[uVar7] +
                       (short)*(undefined4 *)((int)g_SoftwarePixelPackTables->green + (uVar18 >> 6))
                       + (short)*(undefined4 *)((int)g_SoftwarePixelPackTables->red + (uVar11 >> 6))
                  ;
                }
              }
              pbVar19 = pbVar19 + 1;
              pbVar21 = pbVar21 + 2;
              iVar5 = iVar5 + -1;
            } while (iVar5 != 0);
            pbVar19 = pbVar19 + (iVar4 - iVar17);
            pbVar21 = pbVar21 + iVar1 * 2;
            clipMinY = clipMinY + -1;
            iVar5 = iVar17;
          } while (clipMinY != 0);
        }
      }
    }
  }
  else {
    SoftwareTextureSource_BlitHalfRgbSaturatedAdd16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,subresourceIndex,sourceAsset,
               framebuffer);
  }
  return false;
}


/* Address: 0x00582D30.
   Ownership: graphics/backend/glide.
   Purpose: Uses the Glide display framebuffer path when framebuffer == &g_DisplayFramebufferAccess. Other
   framebuffer objects fall back to SoftwareFramebuffer_FillRectArgb16. Partial alpha also updates the optional
   second Glide buffer through g_GlideSecondBufferOffset. The effective rectangle is the intersection of the draw
   rectangle, clipping rectangle, and framebuffer bounds. argb8888 alpha below 1 is ignored; alpha 255 takes the
   direct-fill path; values 1-254 take the source-alpha blend path.
   Cross-module calls: SoftwareFramebuffer_FillRectArgb16 [graphics/backend/software].
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_Framebuffer_FillRectArgb
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate rectMaxY,GraphicsScreenCoordinate rectMaxX,
          GraphicsScreenCoordinate rectMinY,GraphicsScreenCoordinate rectMinX,PackedArgb32 argb8888,
          SoftwareFramebufferAccess *framebuffer)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  ushort uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  byte *pbVar8;
  ulonglong uVar9;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  undefined1 mm1PackedValue0ByteLane1;
  undefined1 mm1PackedValue0ByteLane2;
  undefined8 mm1PackedValue0;
  
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (rectMinX < 0) {
      rectMinX = 0;
    }
    if (rectMinY < 0) {
      rectMinY = 0;
    }
    if ((int)g_DisplayFramebufferAccess.width < rectMaxX) {
      rectMaxX = g_DisplayFramebufferAccess.width;
    }
    if ((int)g_DisplayFramebufferAccess.height < rectMaxY) {
      rectMaxY = g_DisplayFramebufferAccess.height;
    }
    if (rectMinX < clipMinX) {
      rectMinX = clipMinX;
    }
    if (rectMinY < clipMinY) {
      rectMinY = clipMinY;
    }
    if (clipMaxX < rectMaxX) {
      rectMaxX = clipMaxX;
    }
    if (clipMaxY < rectMaxY) {
      rectMaxY = clipMaxY;
    }
    iVar7 = rectMaxX - rectMinX;
    if ((iVar7 != 0 && rectMinX <= rectMaxX) &&
       (clipMinY = rectMaxY - rectMinY, clipMinY != 0 && rectMinY <= rectMaxY)) {
      iVar2 = (g_DisplayFramebufferAccess.width - iVar7) * 2;
      pbVar8 = g_DisplayFramebufferAccess.pixels +
               (rectMinY * g_DisplayFramebufferAccess.width + rectMinX) * 2;
      uVar5 = g_SoftwarePixelPackTables->blue[argb8888 & 0xff] + (argb8888 & 0xff000000) +
              *(int *)((int)g_SoftwarePixelPackTables->green + ((argb8888 & 0xff00) >> 6)) +
              *(int *)((int)g_SoftwarePixelPackTables->red + ((argb8888 & 0xff0000) >> 0xe));
      if (0xffffff < uVar5) {
        iVar6 = iVar7;
        if (uVar5 < 0xff000000) {
          do {
            do {
              pbVar8 = pbVar8 + g_GlideSecondBufferOffset;
              uVar1 = *(undefined2 *)pbVar8;
              mm1PackedValue0ByteLane1 = (undefined1)(uVar5 >> 0x18);
              uVar4 = CONCAT11(mm1PackedValue0ByteLane1,mm1PackedValue0ByteLane1);
              mm1PackedValue0ByteLane2 = (undefined1)(uVar5 >> 0x10);
              mm1PackedValue0ByteLane1 = (undefined1)(uVar5 >> 8);
              iVar3 = -g_GlideSecondBufferOffset;
              uVar9 = CONCAT44(CONCAT22(uVar1,uVar1),CONCAT22(uVar1,uVar1)) &
                      (ulonglong)g_SoftwarePixelMmxConstants.packedPixelMasks;
              mm1PackedValue0 =
                   pmulhw(CONCAT26(uVar4 >> 2,
                                   CONCAT24((ushort)(CONCAT35(CONCAT21(uVar4,
                                                  mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,uVar5)) >> 0x20)
                                            >> 2,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                   mm1PackedValue0ByteLane1) >> 2,
                                                          CONCAT11((char)uVar5,(char)uVar5) >> 2))),
                          g_SoftwareBlendAlphaFactors[uVar5 >> 0x18]);
              mm0PackedValue0 =
                   pmulhw(CONCAT26((ushort)((short)(uVar9 >> 0x30) *
                                           g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                   CONCAT24((ushort)((short)(uVar9 >> 0x20) *
                                                    g_SoftwarePixelMmxConstants.unpackScales.red) >>
                                            2,CONCAT22((ushort)((short)(uVar9 >> 0x10) *
                                                               g_SoftwarePixelMmxConstants.
                                                               unpackScales.green) >> 2,
                                                       (ushort)((short)uVar9 *
                                                               g_SoftwarePixelMmxConstants.
                                                               unpackScales.blue) >> 2))),
                          g_SoftwareBlendInverseAlphaFactors[uVar5 >> 0x18]);
              mm0PackedValue1 =
                   pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                                    (short)((ulonglong)mm1PackedValue0 >> 0x30),
                                    CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                             (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                             CONCAT22((short)((ulonglong)mm0PackedValue0 >> 0x10) +
                                                      (short)((ulonglong)mm1PackedValue0 >> 0x10),
                                                      (short)mm0PackedValue0 +
                                                      (short)mm1PackedValue0))) &
                           (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                           g_SoftwarePixelMmxConstants.packWeights);
              *(short *)(pbVar8 + iVar3) =
                   (short)((ulonglong)mm0PackedValue1 >> 8) +
                   (short)((ulonglong)mm0PackedValue1 >> 0x28);
              pbVar8 = pbVar8 + iVar3 + 2;
              iVar6 = iVar6 + -1;
            } while (iVar6 != 0);
            pbVar8 = pbVar8 + iVar2;
            clipMinY = clipMinY + -1;
            iVar6 = iVar7;
          } while (clipMinY != 0);
          return;
        }
        do {
          for (; iVar6 != 0; iVar6 = iVar6 + -1) {
            *(short *)pbVar8 = (short)uVar5;
            pbVar8 = pbVar8 + 2;
          }
          pbVar8 = pbVar8 + iVar2;
          clipMinY = clipMinY + -1;
          iVar6 = iVar7;
        } while (clipMinY != 0);
      }
    }
  }
  else {
    SoftwareFramebuffer_FillRectArgb16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,rectMaxY,rectMaxX,rectMinY,rectMinX,argb8888,
               framebuffer);
  }
  return;
}


/* Address: 0x005806F0.
   Ownership: graphics/backend/glide.
   Purpose: Draws the selected cursor gfx subresource directly into the shared Glide framebuffer. The argument is
   the backend's back-surface sentinel and must equal pointer value 1. The function acquires framebuffer access,
   calls the source-alpha blitter, and releases access.
   Local calls: Glide3_Framebuffer_BeginAccess, Glide3_Framebuffer_EndAccess.
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_Cursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurfaceSentinel)

{
  UiPixelOffset UVar1;
  UiPixelCoordinate UVar2;
  GraphicsSubresourceIndex arg6;
  GraphicsCursorFrameRecord *pGVar3;
  UiPixelCoordinate cursorY;
  bool bVar4;
  UiPixelOffset cursorHotspotY;
  
  g_CursorCurrentVisibilityToken = g_CursorVisibilityToken;
  if ((-1 < g_CursorVisibilityToken) && (backSurfaceSentinel == (IDirectDrawSurface3 *)0x1)) {
    UVar2 = g_MouseX;
    cursorY = g_MouseY;
    if (g_CursorUseOverridePosition != 0) {
      UVar2 = g_CursorOverrideX;
      cursorY = g_CursorOverrideY;
    }
    pGVar3 = g_CursorFrameRecords + g_CursorFrameIndex;
    UVar1 = pGVar3->hotspotX;
    cursorHotspotY = pGVar3->hotspotY;
    arg6 = pGVar3->activeSubresourceIndex;
    if ((g_CursorButtonState & 7) == 0) {
      arg6 = pGVar3->idleSubresourceIndex;
    }
    bVar4 = Glide3_Framebuffer_BeginAccess();
    if (!bVar4) {
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (g_FramebufferHeight,g_FramebufferWidth,0,0,cursorY - cursorHotspotY,UVar2 - UVar1,
                 arg6,g_CursorSourceAsset,g_FramebufferAccess);
      Glide3_Framebuffer_EndAccess();
    }
  }
  return;
}


/* Address: 0x0057F5C0.
   Ownership: graphics/backend/glide.
   Purpose: Handles glide3 shutdown.
   Cross-module calls: GraphicsTexture_ReleaseObjects [graphics/resources/texture], DynDLL_Unload
   [platform/bootstrap/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx Glide3_Shutdown(void)

{
  int textureSlotsRemaining;
  GraphicsTextureResource **textureSlotCursor;
  
  g_CursorCurrentVisibilityToken = -1;
  g_CursorAlternateVisibilityToken = -1;
  if (g_GlideRuntimeActiveCount != 0) {
    textureSlotsRemaining = 0x1000;
    textureSlotCursor = g_GraphicsTextureSlots;
    do {
      if (*textureSlotCursor != (GraphicsTextureResource *)0x0) {
        GraphicsTexture_ReleaseObjects(*textureSlotCursor);
      }
      textureSlotCursor = textureSlotCursor + 1;
      textureSlotsRemaining = textureSlotsRemaining + -1;
    } while (textureSlotsRemaining != 0);
    (*g_GrSstWinClose)(g_GlideWindowContextHandle);
    (*g_GrGlideShutdown)();
    DynDLL_Unload(dynapi_5);
    g_GlideRuntimeActiveCount = 0;
  }
  return;
}


/* Address: 0x0057F630.
   Ownership: graphics/backend/glide.
   Purpose: Acquires the shared backend access state, locks the Glide front/read buffer, updates
   g_GlideFramebufferAccess.width and pixels, and optionally locks a second buffer with the same row stride. ABI:
   CF clear means success. CF set means failure.
*/
bool __thandor_cf_preserve_eax_ecx_edx Glide3_Framebuffer_BeginAccess(void)

{
  int lfbLockSucceeded;
  int secondaryLfbLockSucceeded;
  sdword previousAccessState;
  
  previousAccessState = g_GraphicsBackendAccessState;
  LOCK();
  g_GraphicsBackendAccessState = 1;
  UNLOCK();
  if (previousAccessState == 0) {
    (*g_GrFinish)();
    lfbLockSucceeded = (*g_GrLfbLock)(0x11,1,0,0,0,&g_GlidePrimaryLfbInfo);
    if (lfbLockSucceeded != 0) {
      g_FramebufferRowStrideBytes = g_GlidePrimaryLfbStrideBytes;
      g_DisplayFramebufferAccess.width = g_GlidePrimaryLfbStrideBytes >> 1;
      g_DisplayFramebufferAccess.pixels = g_GlidePrimaryLfbPixels;
      secondaryLfbLockSucceeded = (*g_GrLfbLock)(0x10,1,0,0,0,&g_GlideSecondaryLfbInfo);
      if ((secondaryLfbLockSucceeded != 0) &&
         (g_FramebufferRowStrideBytes == g_GlideSecondaryLfbStrideBytes)) {
        g_GlideSecondBufferBase = g_GlidePrimaryLfbPixels;
        g_GlideSecondBufferOffset =
             (int)g_GlidePrimaryLfbPixels - (int)g_DisplayFramebufferAccess.pixels;
      }
      return false;
    }
  }
  return true;
}


/* Address: 0x0057F6E0.
   Ownership: graphics/backend/glide.
   Purpose: Unlocks the optional second Glide buffer, unlocks the primary buffer, clears
   g_GlideFramebufferAccess.pixels, and releases the shared backend access state.
*/
void __thandor_void_preserve_eax_ecx_edx Glide3_Framebuffer_EndAccess(void)

{
  if (g_GlideSecondBufferBase != (byte *)0x0) {
    (*g_GrLfbUnlock)(0,1);
    g_GlideSecondBufferOffset = 0;
    g_GlideSecondBufferBase = (byte *)0x0;
  }
  (*g_GrLfbUnlock)(1,1);
  g_DisplayFramebufferAccess.pixels = (byte *)0x0;
  g_GraphicsBackendAccessState = 0;
  return;
}


/* Address: 0x0057FF50.
   Ownership: graphics/backend/glide.
   Purpose: Places an unresident texture into available TMU address space, maintaining the address-ordered resident
   list. It may invalidate an older resident record when no direct gap is available. After placement it downloads
   the embedded GlideTextureInfo with the selected TMU index and residentAddress. Resources without a CPU upload
   buffer remain unresident.
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_TextureResource_EnsureResident(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *pGVar1;
  dword dVar2;
  GraphicsTextureResource *pGVar3;
  GraphicsTextureMemoryAddress GVar4;
  GraphicsTextureResource *pGVar5;
  int iVar6;
  uint uVar7;
  GraphicsTextureResidentTmuIndex GVar8;
  GraphicsTextureResource *pGVar9;
  
  dVar2 = g_GlideTmuMinAddress[0];
  if (((int)texture->residentTmuIndex < 0) &&
     (pGVar9 = g_GlideResidentTextureTail, (texture->glideInfo).data != (void *)0x0)) {
    do {
      if (pGVar9 == (GraphicsTextureResource *)0x0) {
        texture->residentTmuIndex = GRAPHICS_TEXTURE_RESIDENT_TMU0;
        texture->residentNext = (GraphicsTextureResource *)0x0;
        texture->residentAddress = dVar2;
        g_GlideResidentTextureTail = texture;
        g_GlideResidentTextureHead = texture;
        goto Glide3_TextureResource_EnsureResident_CommitResidentPlacementAndDownloadMipMap;
      }
      if (pGVar9->residentNext == (GraphicsTextureResource *)0x0) {
        pGVar1 = pGVar9->sourceAsset;
        iVar6 = pGVar9->subresourceIndex * 0x20 + (pGVar1->tableDescriptor).subresourceTableOffset;
        GVar8 = pGVar9->residentTmuIndex;
        uVar7 = g_GlideTmuMaxAddress[GVar8] -
                (((uint)(*(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 iVar6 + -0x10) *
                         *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 iVar6 + -0xc) * 2) >>
                 ((char)pGVar9->downsampleShift * '\x02' & 0x1fU)) + pGVar9->residentAddress);
        pGVar1 = texture->sourceAsset;
        iVar6 = texture->subresourceIndex * 0x20 + (pGVar1->tableDescriptor).subresourceTableOffset;
        if ((uint)(*(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                           iVar6 + -0x10) *
                   *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                           iVar6 + -0xc) * 2) >> ((char)texture->downsampleShift * '\x02' & 0x1fU) <
            uVar7) {
          pGVar9->residentNext = texture;
          dVar2 = g_GlideTmuMaxAddress[GVar8];
          texture->residentTmuIndex = GVar8;
          texture->residentAddress = dVar2 - uVar7;
          texture->residentNext = (GraphicsTextureResource *)0x0;
          g_GlideResidentTextureTail = texture;
Glide3_TextureResource_EnsureResident_CommitResidentPlacementAndDownloadMipMap:
          g_GlideBoundTexture = (GraphicsTextureResource *)0x0;
          (*g_GrTexDownloadMipMap)
                    (texture->residentTmuIndex,texture->residentAddress,3,&texture->glideInfo);
          return;
        }
        GVar8 = GVar8 + 1;
        pGVar3 = g_GlideResidentTextureHead;
        if (GVar8 < g_GlideTmuCount) {
          dVar2 = g_GlideTmuMinAddress[GVar8];
          pGVar9->residentNext = texture;
          texture->residentTmuIndex = GVar8;
          texture->residentAddress = dVar2;
          texture->residentNext = (GraphicsTextureResource *)0x0;
          g_GlideResidentTextureTail = texture;
          goto Glide3_TextureResource_EnsureResident_CommitResidentPlacementAndDownloadMipMap;
        }
      }
      else {
        pGVar1 = texture->sourceAsset;
        iVar6 = texture->subresourceIndex * 0x20 + (pGVar1->tableDescriptor).subresourceTableOffset;
        pGVar3 = pGVar9->residentNext;
        GVar8 = pGVar9->residentTmuIndex;
        if ((GVar8 == pGVar3->residentTmuIndex) &&
           ((uint)(*(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                           iVar6 + -0x10) *
                   *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                           iVar6 + -0xc) * 2) >> ((char)texture->downsampleShift * '\x02' & 0x1fU)
            <= pGVar3->residentAddress - pGVar9->residentAddress)) {
          GVar4 = pGVar3->residentAddress;
          pGVar5 = pGVar3->residentNext;
          pGVar9->residentNext = texture;
          texture->residentAddress = GVar4;
          texture->residentNext = pGVar5;
          texture->residentTmuIndex = GVar8;
          g_GlideResidentTextureTail = texture;
          pGVar3->residentNext = (GraphicsTextureResource *)0x0;
          pGVar3->residentTmuIndex = GRAPHICS_TEXTURE_NOT_RESIDENT;
          pGVar3->residentAddress = 0;
          goto Glide3_TextureResource_EnsureResident_CommitResidentPlacementAndDownloadMipMap;
        }
      }
      pGVar9 = pGVar3;
    } while (pGVar9 != g_GlideResidentTextureTail);
  }
  return;
}


/* Address: 0x0057FD40.
   Ownership: graphics/backend/glide.
   Purpose: Builds the embedded GlideTextureInfo, selects RGB565-style or ARGB4444-style output, allocates a two-
   byte-per-texel CPU upload buffer, marks the resource unresident, and invokes the converter selected by
   texture->downsampleShift. The function preserves EAX. Existing callers that pass the same pointer in EAX use
   that preservation as a pointer result. Allocation failure is reported through CF.
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_TextureResource_Initialize(GraphicsTextureResource *texture)

{
  GrAspectRatio_t *pGVar1;
  GrLOD_t *pGVar2;
  DDPIXELFORMAT *pDVar3;
  GraphicsTextureDownsampleShift GVar4;
  dword dVar5;
  dword dVar6;
  dword dVar7;
  ArenaAllocEaxCf5 AVar8;
  GraphicsTextureSizeEaxEdxCf9 GVar9;
  
  dVar5 = g_TextureDownsampleShift;
  GVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(texture->subresourceIndex,texture->sourceAsset);
  dVar7 = GVar9.logicalHeightPixels;
  dVar6 = GVar9.logicalWidthPixels;
  texture->downsampleShift = dVar5;
  (texture->glideInfo).aspectRatioLog2 = 0;
  while (dVar6 != dVar7) {
    if ((int)dVar6 < (int)dVar7) {
      pGVar1 = &(texture->glideInfo).aspectRatioLog2;
      *pGVar1 = *pGVar1 + -1;
      dVar6 = dVar6 * 2;
    }
    else {
      pGVar1 = &(texture->glideInfo).aspectRatioLog2;
      *pGVar1 = *pGVar1 + 1;
      dVar7 = dVar7 * 2;
    }
  }
  (texture->glideInfo).smallLodLog2 = 0;
  (texture->glideInfo).largeLodLog2 = 0;
  pDVar3 = texture->pixelFormat;
  for (dVar6 = 1; dVar6 != dVar7; dVar6 = dVar6 * 2) {
    (texture->glideInfo).smallLodLog2 = (texture->glideInfo).smallLodLog2 + 1;
    pGVar2 = &(texture->glideInfo).largeLodLog2;
    *pGVar2 = *pGVar2 + 1;
  }
  (texture->glideInfo).format = 0xc;
  GVar4 = texture->downsampleShift;
  if ((pDVar3 == (DDPIXELFORMAT *)0x577d90) || (pDVar3 == (DDPIXELFORMAT *)0x577dd0)) {
    (texture->glideInfo).format = 10;
  }
  (texture->glideInfo).smallLodLog2 = (texture->glideInfo).smallLodLog2 - GVar4;
  pGVar2 = &(texture->glideInfo).largeLodLog2;
  *pGVar2 = *pGVar2 - GVar4;
  GVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(texture->subresourceIndex,texture->sourceAsset);
  (texture->glideInfo).data = (void *)0x0;
  texture->residentNext = (GraphicsTextureResource *)0x0;
  texture->residentTmuIndex = GRAPHICS_TEXTURE_NOT_RESIDENT;
  texture->residentAddress = 0;
  AVar8 = (*g_MemoryApi.alloc)
                    ((GVar9.logicalHeightPixels >> ((byte)GVar4 & 0x1f)) *
                     (GVar9.logicalWidthPixels >> ((byte)GVar4 & 0x1f)) * 2);
  if (!AVar8.carry) {
    (texture->glideInfo).data = (void *)AVar8.eax;
    (*g_GlideTextureColorUpload[GVar4])(texture);
  }
  return;
}


/* Address: 0x0057FE80.
   Ownership: graphics/backend/glide.
   Purpose: Removes a resident texture from the address-ordered linked list, updates the list head/tail and bound-
   texture cache, resets TMU state, frees glideInfo.data, and returns the preserved input pointer in EAX.
*/
void __thandor_void_preserve_eax_ecx_edx
Glide3_TextureResource_Release(GraphicsTextureResource *texture)

{
  GraphicsTextureResource *previousResidentTexture;
  GraphicsTextureResource *pGVar1;
  GraphicsTextureResource *replacementListHead;
  
  if (-1 < (int)texture->residentTmuIndex) {
    previousResidentTexture = (GraphicsTextureResource *)0x0;
    for (pGVar1 = g_GlideResidentTextureHead; pGVar1 != (GraphicsTextureResource *)0x0;
        pGVar1 = pGVar1->residentNext) {
      if (pGVar1 == texture) {
        pGVar1 = texture->residentNext;
        replacementListHead = pGVar1;
        if (previousResidentTexture != (GraphicsTextureResource *)0x0) {
          previousResidentTexture->residentNext = pGVar1;
          pGVar1 = previousResidentTexture;
          replacementListHead = g_GlideResidentTextureHead;
        }
        g_GlideResidentTextureHead = replacementListHead;
        if (texture == g_GlideResidentTextureTail) {
          g_GlideResidentTextureTail = pGVar1;
        }
        if (texture == g_GlideBoundTexture) {
          g_GlideBoundTexture = (GraphicsTextureResource *)0x0;
        }
        break;
      }
      previousResidentTexture = pGVar1;
    }
  }
  texture->residentTmuIndex = GRAPHICS_TEXTURE_NOT_RESIDENT;
  texture->residentNext = (GraphicsTextureResource *)0x0;
  texture->residentAddress = 0;
  (*g_MemoryApi.free)((texture->glideInfo).data);
  (texture->glideInfo).data = (void *)0x0;
  return;
}

