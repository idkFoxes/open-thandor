/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/glide.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/glide.h>
#include <thandor/thandor.h>

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
  GraphicsTextureSourceAsset *setSourceAsset;
  dword currentDownsampleShift;
  DDPIXELFORMAT *selectedPixelFormat;
  GraphicsTextureResource *texture;
  bool registerFailed;
  ArenaAllocEaxCf5 textureAlloc;
  GraphicsTextureSetEntry *entryCursor;
  AssetSubresourceCount remainingSubresources;
  GraphicsSubresourceIndex currentSubresource;
  
  setSourceAsset = textureSet->sourceAsset;
  remainingSubresources = (setSourceAsset->tableDescriptor).subresourceCount;
  entryCursor = textureSet->entries;
  currentSubresource = 0;
  do {
    selectedPixelFormat = GraphicsTexture_SelectPixelFormat(currentSubresource,setSourceAsset);
    textureAlloc = (*g_MemoryApi.alloc)(0x50);
    texture = (GraphicsTextureResource *)textureAlloc.eax;
    if (!textureAlloc.carry) {
      texture->stagingTexture2 = (IDirect3DTexture2 *)0x0;
      texture->stagingSurface3 = (IDirectDrawSurface3 *)0x0;
      texture->stagingSurfaceBase = (IDirectDrawSurface *)0x0;
      entryCursor->texture = texture;
      texture->deviceTexture2 = (IDirect3DTexture2 *)0x0;
      texture->deviceSurface3 = (IDirectDrawSurface3 *)0x0;
      texture->deviceSurfaceBase = (IDirectDrawSurface *)0x0;
      texture->sourceAsset = setSourceAsset;
      texture->subresourceIndex = currentSubresource;
      texture->pixelFormat = selectedPixelFormat;
      currentDownsampleShift = g_TextureDownsampleShift;
      texture->lastUsedCounter = 0;
      texture->textureHandle = 0;
      texture->downsampleShift = currentDownsampleShift;
      Glide3_TextureResource_Initialize(texture);
      registerFailed = GraphicsTexture_RegisterSlot(texture);
      if (registerFailed) {
        Glide3_TextureResource_Release(texture);
        (*g_MemoryApi.free)(texture);
        entryCursor->texture = (GraphicsTextureResource *)0x0;
      }
    }
    currentSubresource = currentSubresource + 1;
    entryCursor = entryCursor + 1;
    remainingSubresources = remainingSubresources - 1;
  } while (remainingSubresources != 0);
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
  uint refreshRateCode;
  uint refreshRateHz;
  uint resolutionKeyOrBestHz;
  dword sstIndexOrSizeOrCount;
  void *output;
  GraphicsTextureMemoryAddress tmuAddress;
  uint remainingResolutions;
  int slotsRemaining;
  GraphicsTextureResidentTmuIndex tmuIndex;
  dword selectedRefreshRateCode;
  GlideImportBinding *binding;
  void *resolutionCursor;
  GraphicsTextureResource **textureSlotCursor;
  DynDllLoadEaxCf5 glideDll;
  DynApiResolveEaxCf5 resolveResult;
  ArenaAllocEaxCf5 resolutionAlloc;
  DisplayModeEaxCf5 displayModeResult;
  dword *tmuCountOutput;
  dword resolutionQueryCode;
  
  resolutionQueryCode = 7;
  resolutionKeyOrBestHz = width | height << 0x10;
  if (((((resolutionKeyOrBestHz == 0x1e00280) || (resolutionQueryCode = 8, resolutionKeyOrBestHz == 0x2580320)) ||
       (resolutionQueryCode = 9, resolutionKeyOrBestHz == 0x2d003c0)) ||
      ((resolutionQueryCode = 0xc, resolutionKeyOrBestHz == 0x3000400 ||
       (resolutionQueryCode = 0xd, resolutionKeyOrBestHz == 0x4000500)))) ||
     (resolutionQueryCode = 0xe, resolutionKeyOrBestHz == 0x4b00640)) {
    glideDll = DynDLL_Load(dynapi_5);
    if (!glideDll.carry) {
      g_GlideRuntimeActiveCount = g_GlideRuntimeActiveCount + 1;
      binding = g_GlideImportBindings;
      do {
        resolveResult = DynAPI_Resolve(&binding->procedure,glideDll.moduleOrError,binding->importName);
        if (resolveResult.carry) goto Glide3_ReleaseRuntimeAfterInitializationFailure;
        binding = binding + 1;
      } while (binding->importName != (char *)0x0);
      (*g_GrGlideInit)();
      THANDOR_PART(word, sstIndexOrSizeOrCount, 0) = g_GraphicsAdapters[adapterIndex].adapterGuid.Data2;
      THANDOR_PART(word, sstIndexOrSizeOrCount, 2) = g_GraphicsAdapters[adapterIndex].adapterGuid.Data3;
      (*g_GrSstSelect)(sstIndexOrSizeOrCount);
      g_GlideSelectedResolutionQuery = resolutionQueryCode;
      sstIndexOrSizeOrCount = (*g_GrQueryResolutions)(&g_GlideSelectedResolutionQuery,(void *)0x0);
      output = (void *)0x19;
      if (0xf < (int)sstIndexOrSizeOrCount) {
        resolutionAlloc = (*g_MemoryApi.alloc)(sstIndexOrSizeOrCount);
        output = (void *)resolutionAlloc.eax;
        if (!resolutionAlloc.carry) {
          remainingResolutions = sstIndexOrSizeOrCount >> 4;
          (*g_GrQueryResolutions)(&g_GlideSelectedResolutionQuery,output);
          resolutionKeyOrBestHz = 0;
          resolutionCursor = output;
          do {
            refreshRateCode = *(uint *)((int)resolutionCursor + 4);
            if ((refreshRateCode < 9) && (refreshRateHz = *(uint *)(refreshRateCode * 4 + THANDOR_ADDR(g_GlideRefreshRatesHz,0)), resolutionKeyOrBestHz <= refreshRateHz)) {
              resolutionKeyOrBestHz = refreshRateHz;
              selectedRefreshRateCode = refreshRateCode;
            }
            resolutionCursor = (void *)((int)resolutionCursor + 0x10);
            remainingResolutions = remainingResolutions - 1;
          } while (remainingResolutions != 0);
          (*g_MemoryApi.free)(output);
          g_GlideWindowContextHandle =
               (*g_GrSstWinOpen)((dword)g_MainWindow,resolutionQueryCode,selectedRefreshRateCode,0,0
                                 ,2,1);
          output = (void *)0x50;
          tmuCountOutput = &g_GraphicsAdapters[adapterIndex].reserved74;
          if (g_GlideWindowContextHandle != 0) {
            (*g_GrGet)(GLIDE_QUERY_SELECTOR_0x13,4,tmuCountOutput);
            g_GlideTmuCount = *tmuCountOutput;
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
            sstIndexOrSizeOrCount = g_GlideTmuCount;
            do {
              (*g_GrTexMipMapMode)(tmuIndex,0,0);
              (*g_GrTexClampMode)(tmuIndex,0,0);
              (*g_GrTexFilterMode)(tmuIndex,1,1);
              (*g_GrTexCombine)(tmuIndex,1,1,1,1,0,0);
              tmuAddress = (*g_GrTexMinAddress)(tmuIndex);
              g_GlideTmuMinAddress[tmuIndex] = tmuAddress;
              tmuAddress = (*g_GrTexMaxAddress)(tmuIndex);
              g_GlideTmuMaxAddress[tmuIndex] = tmuAddress;
              tmuIndex = tmuIndex + 1;
              sstIndexOrSizeOrCount = sstIndexOrSizeOrCount - 1;
            } while (sstIndexOrSizeOrCount != 0);
            (*g_GrColorCombine)(3,1,0,1,0);
            (*g_GrAlphaCombine)(3,1,0,1,0);
            g_GlideTexturingDisabledState = 1;
            (*g_GrAlphaBlendFunction)(1,7,4,0);
            g_GlideBlendModeState = 0;
            g_GlideBoundTexture = (GraphicsTextureResource *)0x0;
            g_GlideResidentTextureHead = (GraphicsTextureResource *)0x0;
            g_GlideResidentTextureTail = (GraphicsTextureResource *)g_GlideTmuMinAddress[0];
            g_PrimarySurface3 = (IDirectDrawSurface3 *)0x0;
            displayModeResult = (*g_GraphicsDisplayModeFinalizeCf)(adapterIndex,bitsPerPixel,height,width);
            output = (void *)displayModeResult.eax;
            if (!displayModeResult.carry) {
              slotsRemaining = 0x1000;
              textureSlotCursor = g_GraphicsTextureSlots;
              do {
                if (*textureSlotCursor != (GraphicsTextureResource *)0x0) {
                  Glide3_TextureResource_Initialize(*textureSlotCursor);
                }
                textureSlotCursor = textureSlotCursor + 1;
                slotsRemaining = slotsRemaining + -1;
              } while (slotsRemaining != 0);
              return false;
            }
            (*g_GrSstWinClose)(g_GlideWindowContextHandle);
          }
        }
      }
      (*g_GrGlideShutdown)(); /* grGlideShutdown(void); Ghidra passed a stale register */
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
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinate;
  GraphicsTextureSetEntry *packetTextureEntry;
  uint textureHeightLog2;
  GraphicsTextureResource *texture;
  GraphicsPrimitivePacket *currentPacket;
  sdword previousAccessState;
  byte coordinateShift;
  uint maxDimensionLog2;
  uint widthLog2OrFlags;
  GraphicsPrimitivePacketEaxCf5 packetResult;
  
  previousAccessState = g_GraphicsBackendAccessState;
  LOCK();
  g_GraphicsBackendAccessState = 1;
  UNLOCK();
  if (previousAccessState == 0) {
    packetResult = GraphicsPrimitiveQueue_Begin(queue);
    while (currentPacket = packetResult.packet, !packetResult.carry) {
      if (currentPacket->vertices[0].screenX < 0x7f0001) {
        if (currentPacket->vertices[0].screenX < -0x7f0000) {
          currentPacket->vertices[0].screenX = -0x7f0000;
        }
      }
      else {
        currentPacket->vertices[0].screenX = 0x7f0000;
      }
      if (currentPacket->vertices[1].screenX < 0x7f0001) {
        if (currentPacket->vertices[1].screenX < -0x7f0000) {
          currentPacket->vertices[1].screenX = -0x7f0000;
        }
      }
      else {
        currentPacket->vertices[1].screenX = 0x7f0000;
      }
      if (currentPacket->vertices[2].screenX < 0x7f0001) {
        if (currentPacket->vertices[2].screenX < -0x7f0000) {
          currentPacket->vertices[2].screenX = -0x7f0000;
        }
      }
      else {
        currentPacket->vertices[2].screenX = 0x7f0000;
      }
      if (currentPacket->vertices[0].screenY < 0x7f0001) {
        if (currentPacket->vertices[0].screenY < -0x7f0000) {
          currentPacket->vertices[0].screenY = -0x7f0000;
        }
      }
      else {
        currentPacket->vertices[0].screenY = 0x7f0000;
      }
      if (currentPacket->vertices[1].screenY < 0x7f0001) {
        if (currentPacket->vertices[1].screenY < -0x7f0000) {
          currentPacket->vertices[1].screenY = -0x7f0000;
        }
      }
      else {
        currentPacket->vertices[1].screenY = 0x7f0000;
      }
      if (currentPacket->vertices[2].screenY < 0x7f0001) {
        if (currentPacket->vertices[2].screenY < -0x7f0000) {
          currentPacket->vertices[2].screenY = -0x7f0000;
        }
      }
      else {
        currentPacket->vertices[2].screenY = 0x7f0000;
      }
      g_GlideVertex0ScreenX = (dword)(float)currentPacket->vertices[0].screenX;
      g_GlideVertex0ScreenY = (dword)(float)currentPacket->vertices[0].screenY;
      if ((float)g_GlideVertex0ScreenX != 0.0) {
        g_GlideVertex0ScreenX = g_GlideVertex0ScreenX + 0xfa000000;
      }
      if ((float)g_GlideVertex0ScreenY != 0.0) {
        g_GlideVertex0ScreenY = g_GlideVertex0ScreenY + 0xfa000000;
      }
      g_GlideVertex1ScreenX = (dword)(float)currentPacket->vertices[1].screenX;
      g_GlideVertex1ScreenY = (dword)(float)currentPacket->vertices[1].screenY;
      if ((float)g_GlideVertex1ScreenX != 0.0) {
        g_GlideVertex1ScreenX = g_GlideVertex1ScreenX + 0xfa000000;
      }
      if ((float)g_GlideVertex1ScreenY != 0.0) {
        g_GlideVertex1ScreenY = g_GlideVertex1ScreenY + 0xfa000000;
      }
      g_GlideVertex2ScreenX = (dword)(float)currentPacket->vertices[2].screenX;
      g_GlideVertex2ScreenY = (dword)(float)currentPacket->vertices[2].screenY;
      if ((float)g_GlideVertex2ScreenX != 0.0) {
        g_GlideVertex2ScreenX = g_GlideVertex2ScreenX + 0xfa000000;
      }
      if ((float)g_GlideVertex2ScreenY != 0.0) {
        g_GlideVertex2ScreenY = g_GlideVertex2ScreenY + 0xfa000000;
      }
      g_GlideVertex0DiffuseColor = currentPacket->vertices[0].diffuseColor;
      g_GlideVertex1DiffuseColor = currentPacket->vertices[1].diffuseColor;
      g_GlideVertex2DiffuseColor = currentPacket->vertices[2].diffuseColor;
      packetTextureEntry = currentPacket->textureEntry;
      if (packetTextureEntry != (GraphicsTextureSetEntry *)0x0) {
        widthLog2OrFlags = packetTextureEntry->widthLog2;
        textureHeightLog2 = packetTextureEntry->heightLog2;
        maxDimensionLog2 = widthLog2OrFlags;
        if (widthLog2OrFlags < textureHeightLog2) {
          maxDimensionLog2 = textureHeightLog2;
        }
        coordinateShift = (char)maxDimensionLog2 - (char)widthLog2OrFlags;
        textureCoordinate = &currentPacket->vertices[0].textureU;
        *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
        textureCoordinate = &currentPacket->vertices[1].textureU;
        *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
        textureCoordinate = &currentPacket->vertices[2].textureU;
        *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
        coordinateShift = (char)maxDimensionLog2 - (char)textureHeightLog2;
        textureCoordinate = &currentPacket->vertices[0].textureV;
        *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
        textureCoordinate = &currentPacket->vertices[1].textureV;
        *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
        textureCoordinate = &currentPacket->vertices[2].textureV;
        *textureCoordinate = *textureCoordinate >> (coordinateShift & 0x1f);
      }
      g_GlideVertex0ReciprocalDepth = (dword)(1.0 / (float)currentPacket->vertices[0].depth);
      g_GlideVertex1ReciprocalDepth = (dword)(1.0 / (float)currentPacket->vertices[1].depth);
      g_GlideVertex2ReciprocalDepth = (dword)(1.0 / (float)currentPacket->vertices[2].depth);
      g_GlideVertex0PerspectiveScale = (dword)(4096.0 / ((float)currentPacket->vertices[0].depth + 4096.0))
      ;
      g_GlideVertex1PerspectiveScale = (dword)(4096.0 / ((float)currentPacket->vertices[1].depth + 4096.0))
      ;
      g_GlideVertex2PerspectiveScale = (dword)(4096.0 / ((float)currentPacket->vertices[2].depth + 4096.0))
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
           (dword)((float)currentPacket->vertices[0].textureU * (float)g_GlideVertex0PerspectiveScale);
      g_GlideVertex0ProjectedTextureV =
           (dword)((float)currentPacket->vertices[0].textureV * (float)g_GlideVertex0PerspectiveScale);
      if ((float)g_GlideVertex0ProjectedTextureU != 0.0) {
        g_GlideVertex0ProjectedTextureU = g_GlideVertex0ProjectedTextureU + 0xfa000000;
      }
      if ((float)g_GlideVertex0ProjectedTextureV != 0.0) {
        g_GlideVertex0ProjectedTextureV = g_GlideVertex0ProjectedTextureV + 0xfa000000;
      }
      g_GlideVertex1ProjectedTextureU =
           (dword)((float)currentPacket->vertices[1].textureU * (float)g_GlideVertex1PerspectiveScale);
      g_GlideVertex1ProjectedTextureV =
           (dword)((float)currentPacket->vertices[1].textureV * (float)g_GlideVertex1PerspectiveScale);
      if ((float)g_GlideVertex1ProjectedTextureU != 0.0) {
        g_GlideVertex1ProjectedTextureU = g_GlideVertex1ProjectedTextureU + 0xfa000000;
      }
      if ((float)g_GlideVertex1ProjectedTextureV != 0.0) {
        g_GlideVertex1ProjectedTextureV = g_GlideVertex1ProjectedTextureV + 0xfa000000;
      }
      g_GlideVertex2ProjectedTextureU =
           (dword)((float)currentPacket->vertices[2].textureU * (float)g_GlideVertex2PerspectiveScale);
      g_GlideVertex2ProjectedTextureV =
           (dword)((float)currentPacket->vertices[2].textureV * (float)g_GlideVertex2PerspectiveScale);
      if ((float)g_GlideVertex2ProjectedTextureU != 0.0) {
        g_GlideVertex2ProjectedTextureU = g_GlideVertex2ProjectedTextureU + 0xfa000000;
      }
      if ((float)g_GlideVertex2ProjectedTextureV != 0.0) {
        g_GlideVertex2ProjectedTextureV = g_GlideVertex2ProjectedTextureV + 0xfa000000;
      }
      widthLog2OrFlags = currentPacket->renderFlags;
      if (((widthLog2OrFlags & 0x10000) == 0) || (currentPacket->textureEntry == (GraphicsTextureSetEntry *)0x0)) {
        if (g_GlideTexturingDisabledState != 0)
        goto Glide3_DrawPrimitiveQueue_ConfigureUntexturedColorAndAlphaCombine;
      }
      else {
        texture = currentPacket->textureEntry->texture;
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
      if ((widthLog2OrFlags & 0x20000) == 0) {
        widthLog2OrFlags = widthLog2OrFlags & 0x7000;
      }
      else {
        widthLog2OrFlags = 0x1000;
      }
      if (widthLog2OrFlags == 0x2000) {
        if (g_GlideBlendModeState != 1) {
          (*g_GrAlphaBlendFunction)(4,4,4,0);
          g_GlideBlendModeState = 1;
        }
      }
      else if (widthLog2OrFlags == 0) {
        if (g_GlideBlendModeState != 2) {
          (*g_GrAlphaBlendFunction)(4,0,4,0);
          g_GlideBlendModeState = 2;
        }
      }
      else if (g_GlideBlendModeState != 0) {
        (*g_GrAlphaBlendFunction)(1,5,4,0);
        g_GlideBlendModeState = 0;
      }
      if ((widthLog2OrFlags == 0x2000) || (widthLog2OrFlags == 0x1000)) {
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
      packetResult = GraphicsPrimitiveQueue_Next(queue);
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
  GraphicsTextureResource **slotCursor;
  GraphicsTextureSourceAsset *returnedSourceAsset;
  int slotsRemaining;
  dword remainingEntries;
  GraphicsTextureSetEntry *entryCursor;
  GraphicsTextureResource **matchedSlot;
  
  returnedSourceAsset = (GraphicsTextureSourceAsset *)0x0;
  if (setRegisterMirror != (GraphicsTextureSet *)0x0) {
    remainingEntries = setRegisterMirror->subresourceCount;
    entryCursor = setRegisterMirror->entries;
    do {
      texture = entryCursor->texture;
      if (texture != (GraphicsTextureResource *)0x0) {
        slotsRemaining = 0x1000;
        slotCursor = g_GraphicsTextureSlots;
        do {
          matchedSlot = slotCursor;
          if (texture == *matchedSlot) break;
          slotsRemaining = slotsRemaining + -1;
          slotCursor = matchedSlot + 1;
        } while (slotsRemaining != 0);
        *matchedSlot = (GraphicsTextureResource *)0x0;
        Glide3_TextureResource_Release(texture);
        (*g_MemoryApi.free)(texture);
      }
      entryCursor = entryCursor + 1;
      remainingEntries = remainingEntries - 1;
    } while (remainingEntries != 0);
    returnedSourceAsset = GraphicsTextureSet_FreeMetadata(set);
  }
  return returnedSourceAsset;
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
  SoftwareFramebufferAccess *savedBackground;
  sdword visibilityTokenOrAccessState;
  sdword drawX;
  sdword drawY;
  
  drawY = g_CursorCurrentDrawY;
  drawX = g_CursorCurrentDrawX;
  visibilityTokenOrAccessState = g_CursorCurrentVisibilityToken;
  savedBackground = g_CursorSavedBackground;
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
  g_CursorAlternateSavedBackground = savedBackground;
  g_CursorAlternateVisibilityToken = visibilityTokenOrAccessState;
  g_CursorAlternateDrawX = drawX;
  g_CursorAlternateDrawY = drawY;
  Glide3_Cursor_ComposeBeforePresent((IDirectDrawSurface3 *)0x1);
  visibilityTokenOrAccessState = g_GraphicsBackendAccessState;
  LOCK();
  g_GraphicsBackendAccessState = 1;
  UNLOCK();
  if (visibilityTokenOrAccessState == 0) {
    (*g_GrFinish)();
    (*g_GrBufferSwap)(1);
    g_GraphicsBackendAccessState = g_GraphicsBackendAccessState + -1;
  }
  drawY = g_CursorCurrentDrawY;
  drawX = g_CursorCurrentDrawX;
  visibilityTokenOrAccessState = g_CursorCurrentVisibilityToken;
  savedBackground = g_CursorSavedBackground;
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
  g_CursorAlternateSavedBackground = savedBackground;
  g_CursorAlternateVisibilityToken = visibilityTokenOrAccessState;
  g_CursorAlternateDrawX = drawX;
  g_CursorAlternateDrawY = drawY;
  return;
}


/* Address: 0x0057EE90.
   Ownership: graphics/backend/glide.
   Purpose: Loads Glide 3 and appends its boards and resolutions to the adapter and display mode
   lists. CF=1 when the DLL or one of its entry points is missing; EAX is then that error code.
   Cross-module calls: DynDLL_Load [platform/bootstrap/runtime], DynAPI_Resolve [platform/bootstrap/runtime],
   DynDLL_Unload [platform/bootstrap/runtime], Text_CopyNarrowToUtf16Cf [core/text/string].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Glide3_InitAndEnumerate(void)

{
  byte *source;
  byte *driverDescription;
  dword querySizeOrAdapterIndex;
  int *output;
  FrontendDisplayDimensionPixels modeWidth;
  uint remainingResolutions;
  FrontendDisplayDimensionPixels modeHeight;
  int *resolutionCursor;
  GlideImportBinding *binding;
  GraphicsAdapterRecord *adapter;
  GraphicsDisplayMode *displayMode;
  DynDllLoadEaxCf5 glideDll;
  DynApiResolveEaxCf5 resolveResult;
  ArenaAllocEaxCf5 resolutionAlloc;
  dword sstIndex;
  int remainingBoards;
  StatusValueEaxCf5 result;
  
  glideDll = DynDLL_Load(dynapi_5);
  if (glideDll.carry) {
    result.valueOrError = (dword)glideDll.moduleOrError;
    result.carry = true;
    return result;
  }
  binding = g_GlideImportBindings;
  do {
    resolveResult = DynAPI_Resolve(&binding->procedure,glideDll.moduleOrError,binding->importName);
    if (resolveResult.carry) {
      DynDLL_Unload(dynapi_5);
      result.valueOrError = (dword)resolveResult.procedureOrError;
      result.carry = true;
      return result;
    }
    binding = binding + 1;
  } while (binding->importName != (char *)0x0);
  (*g_GrGet)(0xf,4,&remainingBoards);
  if (remainingBoards != 0) {
    sstIndex = 0;
    do {
      if (0xf < g_GraphicsAdapterCount) break;
      (*g_GrGlideInit)();
      (*g_GrSstSelect)(sstIndex);
      source = (byte *)(*g_GrGetString)(0xa2);
      driverDescription = (byte *)(*g_GrGetString)(0xa1);
      adapter = g_GraphicsAdapters + g_GraphicsAdapterCount;
      Text_CopyNarrowToUtf16Cf(0x28,adapter->driverDescriptionUtf16,driverDescription);
      Text_CopyNarrowToUtf16Cf(0x28,adapter->deviceNameUtf16,source);
      (adapter->adapterGuid).Data1 = 1;
      (adapter->adapterGuid).Data2 = (undefined2)sstIndex;
      (adapter->adapterGuid).Data3 = THANDOR_PART(word, sstIndex, 2);
      (adapter->deviceGuid).Data1 = 1;
      querySizeOrAdapterIndex = (*g_GrQueryResolutions)(&g_GlideEnumerationResolutionQuery,(void *)0x0);
      if (querySizeOrAdapterIndex != 0) {
        resolutionAlloc = (*g_MemoryApi.alloc)(querySizeOrAdapterIndex);
        output = (int *)resolutionAlloc.eax;
        if (!resolutionAlloc.carry) {
          remainingResolutions = querySizeOrAdapterIndex >> 4;
          (*g_GrQueryResolutions)(&g_GlideEnumerationResolutionQuery,output);
          displayMode = g_GraphicsDisplayModes + g_GraphicsDisplayModeCount;
          resolutionCursor = output;
          do {
            if (0xff < g_GraphicsDisplayModeCount) break;
            modeWidth = 0x280;
            modeHeight = 0x1e0;
            if (*resolutionCursor == 7) {
Glide3_AppendEnumeratedDisplayMode:
              displayMode->width = modeWidth;
              displayMode->height = modeHeight;
              querySizeOrAdapterIndex = g_GraphicsAdapterCount;
              displayMode->bitsPerPixel = 0x10;
              displayMode->adapterIndex = querySizeOrAdapterIndex;
              g_GraphicsDisplayModeCount = g_GraphicsDisplayModeCount + 1;
              displayMode = displayMode + 1;
            }
            else {
              modeWidth = 800;
              modeHeight = 600;
              if (*resolutionCursor == 8) goto Glide3_AppendEnumeratedDisplayMode;
              modeWidth = 0x3c0;
              modeHeight = 0x2d0;
              if (*resolutionCursor == 9) goto Glide3_AppendEnumeratedDisplayMode;
              modeWidth = 0x400;
              modeHeight = 0x300;
              if (*resolutionCursor == 0xc) goto Glide3_AppendEnumeratedDisplayMode;
              modeWidth = 0x500;
              modeHeight = 0x400;
              if (*resolutionCursor == 0xd) goto Glide3_AppendEnumeratedDisplayMode;
              modeWidth = 0x640;
              modeHeight = 0x4b0;
              if (*resolutionCursor == 0xe) goto Glide3_AppendEnumeratedDisplayMode;
            }
            resolutionCursor = resolutionCursor + 4;
            remainingResolutions = remainingResolutions - 1;
          } while (remainingResolutions != 0);
          (*g_MemoryApi.free)(output);
        }
      }
      (*g_GrGlideShutdown)();
      g_GraphicsAdapterCount = g_GraphicsAdapterCount + 1;
      sstIndex = sstIndex + 1;
      remainingBoards = remainingBoards + -1;
    } while (remainingBoards != 0);
  }
  DynDLL_Unload(dynapi_5);
  result.valueOrError = 0;
  result.carry = false;
  return result;
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
  GraphicsTextureResource *entryTexture;
  GraphicsTextureResource *textureResource;
  
  entryTexture = set->entries[subresourceIndex].texture;
  (*g_GlideTextureColorUpload[entryTexture->downsampleShift])(entryTexture);
  if (-1 < (int)entryTexture->residentTmuIndex) {
    (*g_GrTexDownloadMipMap)(entryTexture->residentTmuIndex,entryTexture->residentAddress,3,&entryTexture->glideInfo);
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
  GraphicsTextureResource *entryTexture;
  GraphicsTextureResource *textureResource;
  
  entryTexture = set->entries[subresourceIndex].texture;
  (*g_GlideTextureAlphaUpload[entryTexture->downsampleShift])(entryTexture);
  if (-1 < (int)entryTexture->residentTmuIndex) {
    (*g_GrTexDownloadMipMap)(entryTexture->residentTmuIndex,entryTexture->residentAddress,3,&entryTexture->glideInfo);
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
  ushort rgb565Pixel;
  GraphicsCapturedTextureSourceAsset *capturedAsset;
  dword strideOrTimestamp;
  int pixelCountOrRemaining;
  ushort *sourcePixelCursor;
  GraphicsCapturedTextureSourceAsset *clearCursor;
  dword *argbCursor;
  GraphicsFramebufferCaptureEaxCf5 captureResult;
  dword buffer;
  GraphicsPixelDimension width;
  GraphicsPixelDimension height;
  ushort *destinationPixels;
  
  pixelCountOrRemaining = captureWidth * captureHeight;
  captureResult = THANDOR_BITCAST(ArenaAllocEaxCf5, GraphicsFramebufferCaptureEaxCf5, (*g_MemoryApi.alloc)(pixelCountOrRemaining * 4 + 0x220));
  capturedAsset = captureResult.eax;
  if (!captureResult.carry) {
    destinationPixels = (ushort *)((int)capturedAsset->argb8888Pixels + pixelCountOrRemaining * 2);
    strideOrTimestamp = captureWidth * 2;
    clearCursor = capturedAsset;
    for (pixelCountOrRemaining = pixelCountOrRemaining + 0x88; pixelCountOrRemaining != 0; pixelCountOrRemaining = pixelCountOrRemaining + -1) {
      (clearCursor->common).magic = 0;
      clearCursor = (GraphicsCapturedTextureSourceAsset *)&(clearCursor->common).allocationSizeBytes;
    }
    buffer = 1;
    width = captureWidth;
    height = captureHeight;
    sourcePixelCursor = destinationPixels;
    (*g_GrFinish)();
    (*g_GrLfbReadRegion)(buffer,sourceX,sourceY,width,height,strideOrTimestamp,destinationPixels);
    (capturedAsset->common).magic = ASSET_MAGIC_GFX;
    /* The original stores EDX after the Glide calls, i.e. whatever glide3x left there; this is the
       allocation size, as the DirectDraw capture stores (the screenshot writer uses it as file size). */
    (capturedAsset->common).allocationSizeBytes = captureWidth * captureHeight * 4 + 0x220;
    (capturedAsset->common).formatVersion = 1;
    (capturedAsset->common).converterVersion = 0;
    strideOrTimestamp = (*g_LocaleGetPackedCurrentTime)();
    (capturedAsset->common).buildMetadata.timestamps.dateValue0 = strideOrTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.dateValue1 = strideOrTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.dateValue2 = strideOrTimestamp;
    strideOrTimestamp = (*g_LocaleGetPackedCurrentDate)();
    (capturedAsset->common).buildMetadata.timestamps.timeValue0 = strideOrTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.timeValue1 = strideOrTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.timeValue2 = strideOrTimestamp;
    (*g_LocaleCopyDefaultComputerLabelUtf16)((capturedAsset->common).buildMetadata.names.producerName);
    (*g_LocaleCopyDefaultComputerLabelUtf16)((capturedAsset->common).buildMetadata.names.sourceName);
    capturedAsset->opaqueTablePayloadBC_1FF[0x44] = 0;
    (capturedAsset->tableDescriptor).subresourceCount = 1;
    (capturedAsset->tableDescriptor).paletteBankCount = 0;
    (capturedAsset->tableDescriptor).subresourceTableOffset = 0x200;
    (capturedAsset->sourceEntry).logicalWidth = captureWidth;
    (capturedAsset->sourceEntry).logicalHeight = captureHeight;
    (capturedAsset->sourceEntry).pixelWidth = captureWidth;
    (capturedAsset->sourceEntry).pixelHeight = captureHeight;
    (capturedAsset->sourceEntry).originX = 0;
    (capturedAsset->sourceEntry).originY = 0;
    (capturedAsset->sourceEntry).paletteIndex = -1;
    (capturedAsset->sourceEntry).dataOffset = 0x220;
    argbCursor = capturedAsset->argb8888Pixels;
    pixelCountOrRemaining = captureWidth * captureHeight;
    do {
      rgb565Pixel = *sourcePixelCursor;
      *argbCursor = (((((rgb565Pixel >> 0xb | 0x1fe0) << 3 | (uint)(rgb565Pixel >> 0xd)) << 6 | (rgb565Pixel & 0x7ff) >> 5
                  ) << 2 | (rgb565Pixel & 0x7ff) >> 9) << 5 | rgb565Pixel & 0x1f) << 3 | (rgb565Pixel & 0x1f) >> 2;
      sourcePixelCursor = sourcePixelCursor + 1;
      argbCursor = argbCursor + 1;
      pixelCountOrRemaining = pixelCountOrRemaining + -1;
    } while (pixelCountOrRemaining != 0);
    captureResult = THANDOR_BITCAST(qword, GraphicsFramebufferCaptureEaxCf5, ((THANDOR_BITCAST(GraphicsFramebufferCaptureEaxCf5, qword, captureResult) & 0xFFFFFFFFFFull) & 0xffffffff));
  }
  return captureResult;
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
  GraphicsTextureSourceAsset *asset;
  int sourceWidth;
  int entryPaletteIndex;
  uint sourceArgb;
  int subresourceRecordOffset;
  byte *sourceCursor;
  ushort *destinationCursor;
  int remainingColumns;
  int remainingRows;
  
  asset = texture->sourceAsset;
  destinationCursor = (texture->glideInfo).data;
  subresourceRecordOffset = texture->subresourceIndex * 0x20 + (asset->tableDescriptor).subresourceTableOffset;
  sourceWidth = *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0x10);
  remainingRows = *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0xc);
  entryPaletteIndex = *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0x20);
  sourceCursor = (asset->common).buildMetadata.assetRelativeAddressAnchor28 +
           *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0x1c) +
           -0x28;
  remainingColumns = sourceWidth;
  if (entryPaletteIndex < 0) {
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          sourceArgb = *(uint *)sourceCursor;
          *destinationCursor = (ushort)((ushort)(((sourceArgb >> 3 & 0x1f) << 0x15) >> 0x10) |
                            (ushort)(((sourceArgb >> 10) << 0x1a) >> 0x10)) >> 5 |
                    (ushort)(((sourceArgb >> 0x13) << 0x1b) >> 0x10);
          sourceCursor = sourceCursor + 4;
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns + -1;
        } while (remainingColumns != 0);
        remainingRows = remainingRows + -1;
        remainingColumns = sourceWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          sourceArgb = *(uint *)sourceCursor;
          *destinationCursor = (ushort)((ushort)((ushort)(((sourceArgb >> 4 & 0xf) << 0x18) >> 0x10) |
                                     (ushort)(((sourceArgb >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                            (ushort)(((sourceArgb >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                    (ushort)(sourceArgb >> 0x10) & 0xf000;
          sourceCursor = sourceCursor + 4;
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns + -1;
        } while (remainingColumns != 0);
        remainingRows = remainingRows + -1;
        remainingColumns = sourceWidth;
      } while (remainingRows != 0);
    }
  }
  else {
    asset = texture->sourceAsset;
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          sourceArgb = *(uint *)(asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28
                           + (uint)*sourceCursor * 8 + -0x28);
          *destinationCursor = (ushort)((ushort)(((sourceArgb >> 3) << 0x1b) >> 0x16) |
                            (ushort)(((sourceArgb >> 10) << 0x1a) >> 0x10)) >> 5 |
                    (ushort)(((sourceArgb >> 0x13) << 0x1b) >> 0x10);
          sourceCursor = sourceCursor + 1;
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns + -1;
        } while (remainingColumns != 0);
        remainingRows = remainingRows + -1;
        remainingColumns = sourceWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          sourceArgb = *(uint *)(asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28
                           + (uint)*sourceCursor * 8 + -0x28);
          *destinationCursor = (ushort)((ushort)((ushort)(((sourceArgb >> 4) << 0x1c) >> 0x14) |
                                     (ushort)(((sourceArgb >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                            (ushort)(((sourceArgb >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                    (ushort)(sourceArgb >> 0x10) & 0xf000;
          sourceCursor = sourceCursor + 1;
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns + -1;
        } while (remainingColumns != 0);
        remainingRows = remainingRows + -1;
        remainingColumns = sourceWidth;
      } while (remainingRows != 0);
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
  GraphicsTextureSourceAsset *asset;
  int entryPaletteIndex;
  undefined4 upperLeftSample;
  undefined4 upperRightSample;
  undefined4 lowerLeftSample;
  undefined4 lowerRightSample;
  byte clampedRedOrAlpha;
  ushort lowerRightAlpha;
  uint destinationWidth;
  int subresourceRecordOffset;
  AssetProducerSourceNames *sourceCursor;
  ushort *destinationCursor;
  undefined1 upperLeftGreenOrAlpha;
  ushort blueOrAlphaAverage;
  ushort blueAverage;
  undefined1 upperLeftRed;
  uint3 averagedRgb;
  ushort greenAverageOrUpperRightAlpha;
  ushort greenAverage;
  uint averagedArgb;
  ushort redAverageOrLowerLeftAlpha;
  ushort redAverage;
  undefined1 upperRightGreenOrAlpha;
  undefined1 upperRightRed;
  undefined1 lowerLeftGreenOrAlpha;
  undefined1 lowerLeftRed;
  undefined1 lowerRightGreenOrAlpha;
  undefined1 lowerRightRed;
  uint remainingColumns;
  uint remainingRows;
  
  asset = texture->sourceAsset;
  destinationCursor = (texture->glideInfo).data;
  subresourceRecordOffset = texture->subresourceIndex * 0x20 + (asset->tableDescriptor).subresourceTableOffset;
  entryPaletteIndex = *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0x20);
  sourceCursor = (AssetProducerSourceNames *)
            ((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
            *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0x1c) +
            -0x28);
  destinationWidth = *(uint *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0x10) >>
          1;
  remainingRows = *(uint *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0xc)
              >> 1;
  remainingColumns = destinationWidth;
  if (entryPaletteIndex < 0) {
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          upperLeftSample = *(undefined4 *)sourceCursor->producerName;
          upperRightSample = *(undefined4 *)((int)sourceCursor->producerName + 4);
          lowerLeftSample = *(undefined4 *)((int)sourceCursor->producerName + destinationWidth * 8);
          lowerRightSample = *(undefined4 *)((int)sourceCursor->producerName + destinationWidth * 8 + 4);
          upperLeftRed = (undefined1)((uint)upperLeftSample >> 0x10);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 8);
          upperRightRed = (undefined1)((uint)upperRightSample >> 0x10);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 8);
          lowerLeftRed = (undefined1)((uint)lowerLeftSample >> 0x10);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 8);
          lowerRightRed = (undefined1)((uint)lowerRightSample >> 0x10);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 8);
          blueOrAlphaAverage = (ushort)((CONCAT11((char)upperLeftSample,(char)upperLeftSample) >> 4) +
                            (CONCAT11((char)upperRightSample,(char)upperRightSample) >> 4) +
                           (CONCAT11((char)lowerLeftSample,(char)lowerLeftSample) >> 4) +
                           (CONCAT11((char)lowerRightSample,(char)lowerRightSample) >> 4)) >> 6;
          greenAverageOrUpperRightAlpha = (ushort)((CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha) >> 4) + (CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha) >> 4) +
                           (CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha) >> 4) + (CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha) >> 4)) >> 6;
          redAverageOrLowerLeftAlpha = (ushort)((ushort)(CONCAT15(upperLeftRed,CONCAT14(upperLeftRed,upperLeftSample)) >> 0x24) +
                            (ushort)(CONCAT15(upperRightRed,CONCAT14(upperRightRed,upperRightSample)) >> 0x24) +
                           (ushort)(CONCAT15(lowerLeftRed,CONCAT14(lowerLeftRed,lowerLeftSample)) >> 0x24) +
                           (ushort)(CONCAT15(lowerRightRed,CONCAT14(lowerRightRed,lowerRightSample)) >> 0x24)) >> 6;
          clampedRedOrAlpha = (redAverageOrLowerLeftAlpha != 0) * (redAverageOrLowerLeftAlpha < 0x100) * (char)redAverageOrLowerLeftAlpha - (0xff < redAverageOrLowerLeftAlpha);
          averagedRgb = CONCAT12(clampedRedOrAlpha,CONCAT11((greenAverageOrUpperRightAlpha != 0) * (greenAverageOrUpperRightAlpha < 0x100) * (char)greenAverageOrUpperRightAlpha -
                                           (0xff < greenAverageOrUpperRightAlpha),
                                           (blueOrAlphaAverage != 0) * (blueOrAlphaAverage < 0x100) * (char)blueOrAlphaAverage -
                                           (0xff < blueOrAlphaAverage)));
          *destinationCursor = (ushort)((ushort)(((averagedRgb >> 3 & 0x1f) << 0x15) >> 0x10) |
                             (ushort)(((uint)(averagedRgb >> 10) << 0x1a) >> 0x10)) >> 5 |
                     (ushort)(((uint)(clampedRedOrAlpha >> 3) << 0x1b) >> 0x10);
          sourceCursor = (AssetProducerSourceNames *)((int)sourceCursor->producerName + 8);
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = (AssetProducerSourceNames *)(sourceCursor->producerName + destinationWidth * 4);
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          upperLeftSample = *(undefined4 *)sourceCursor->producerName;
          upperRightSample = *(undefined4 *)(sourceCursor->producerName + 2);
          lowerLeftSample = *(undefined4 *)(sourceCursor->producerName + destinationWidth * 4);
          lowerRightSample = *(undefined4 *)(sourceCursor->producerName + destinationWidth * 4 + 2);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 0x18);
          blueOrAlphaAverage = CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha);
          upperLeftRed = (undefined1)((uint)upperLeftSample >> 0x10);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 8);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 0x18);
          greenAverageOrUpperRightAlpha = CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha);
          upperRightRed = (undefined1)((uint)upperRightSample >> 0x10);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 8);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 0x18);
          redAverageOrLowerLeftAlpha = CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha);
          lowerLeftRed = (undefined1)((uint)lowerLeftSample >> 0x10);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 8);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 0x18);
          lowerRightAlpha = CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha);
          lowerRightRed = (undefined1)((uint)lowerRightSample >> 0x10);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 8);
          blueAverage = (ushort)((CONCAT11((char)upperLeftSample,(char)upperLeftSample) >> 4) +
                            (CONCAT11((char)upperRightSample,(char)upperRightSample) >> 4) +
                           (CONCAT11((char)lowerLeftSample,(char)lowerLeftSample) >> 4) +
                           (CONCAT11((char)lowerRightSample,(char)lowerRightSample) >> 4)) >> 6;
          greenAverage = (ushort)((CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha) >> 4) + (CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha) >> 4) +
                           (CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha) >> 4) + (CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha) >> 4)) >> 6;
          redAverage = (ushort)(((ushort)(CONCAT35(CONCAT21(blueOrAlphaAverage,upperLeftRed),CONCAT14(upperLeftRed,upperLeftSample)) >>
                                     0x20) >> 4) +
                            ((ushort)(CONCAT35(CONCAT21(greenAverageOrUpperRightAlpha,upperRightRed),CONCAT14(upperRightRed,upperRightSample)) >>
                                     0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(redAverageOrLowerLeftAlpha,lowerLeftRed),CONCAT14(lowerLeftRed,lowerLeftSample)) >>
                                    0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(lowerRightAlpha,lowerRightRed),CONCAT14(lowerRightRed,lowerRightSample)) >> 0x20
                                    ) >> 4)) >> 6;
          blueOrAlphaAverage = (ushort)((blueOrAlphaAverage >> 4) + (greenAverageOrUpperRightAlpha >> 4) + (redAverageOrLowerLeftAlpha >> 4) + (lowerRightAlpha >> 4)) >> 6;
          averagedRgb = CONCAT12((redAverage != 0) * (redAverage < 0x100) * (char)redAverage - (0xff < redAverage),
                            CONCAT11((greenAverage != 0) * (greenAverage < 0x100) * (char)greenAverage -
                                     (0xff < greenAverage),
                                     (blueAverage != 0) * (blueAverage < 0x100) * (char)blueAverage -
                                     (0xff < blueAverage)));
          clampedRedOrAlpha = (blueOrAlphaAverage != 0) * (blueOrAlphaAverage < 0x100) * (char)blueOrAlphaAverage - (0xff < blueOrAlphaAverage);
          averagedArgb = CONCAT13(clampedRedOrAlpha,averagedRgb);
          *destinationCursor = (ushort)((ushort)((ushort)((((averagedRgb & 0xf0) >> 4) << 0x18) >> 0x10) |
                                      (ushort)(((averagedArgb >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                             (ushort)(((averagedArgb >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                     (ushort)(((uint)(clampedRedOrAlpha >> 4) << 0x1c) >> 0x10);
          sourceCursor = (AssetProducerSourceNames *)(sourceCursor->producerName + 4);
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = (AssetProducerSourceNames *)(sourceCursor->producerName + destinationWidth * 4);
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
  }
  else {
    asset = texture->sourceAsset;
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          upperLeftSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)sourceCursor->producerName[0] * 8 + -0x28);
          upperRightSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)*(byte *)((int)sourceCursor->producerName + 1) * 8 + -0x28);
          lowerLeftSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)sourceCursor->producerName[destinationWidth] * 8 + -0x28);
          lowerRightSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)*(byte *)((int)sourceCursor->producerName + destinationWidth * 2 + 1) * 8 + -0x28);
          upperLeftRed = (undefined1)((uint)upperLeftSample >> 0x10);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 8);
          upperRightRed = (undefined1)((uint)upperRightSample >> 0x10);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 8);
          lowerLeftRed = (undefined1)((uint)lowerLeftSample >> 0x10);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 8);
          lowerRightRed = (undefined1)((uint)lowerRightSample >> 0x10);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 8);
          blueOrAlphaAverage = (ushort)((CONCAT11((char)upperLeftSample,(char)upperLeftSample) >> 4) +
                            (CONCAT11((char)upperRightSample,(char)upperRightSample) >> 4) +
                           (CONCAT11((char)lowerLeftSample,(char)lowerLeftSample) >> 4) +
                           (CONCAT11((char)lowerRightSample,(char)lowerRightSample) >> 4)) >> 6;
          greenAverageOrUpperRightAlpha = (ushort)((CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha) >> 4) + (CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha) >> 4) +
                           (CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha) >> 4) + (CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha) >> 4)) >> 6;
          redAverageOrLowerLeftAlpha = (ushort)((ushort)(CONCAT15(upperLeftRed,CONCAT14(upperLeftRed,upperLeftSample)) >> 0x24) +
                            (ushort)(CONCAT15(upperRightRed,CONCAT14(upperRightRed,upperRightSample)) >> 0x24) +
                           (ushort)(CONCAT15(lowerLeftRed,CONCAT14(lowerLeftRed,lowerLeftSample)) >> 0x24) +
                           (ushort)(CONCAT15(lowerRightRed,CONCAT14(lowerRightRed,lowerRightSample)) >> 0x24)) >> 6;
          clampedRedOrAlpha = (redAverageOrLowerLeftAlpha != 0) * (redAverageOrLowerLeftAlpha < 0x100) * (char)redAverageOrLowerLeftAlpha - (0xff < redAverageOrLowerLeftAlpha);
          averagedRgb = CONCAT12(clampedRedOrAlpha,CONCAT11((greenAverageOrUpperRightAlpha != 0) * (greenAverageOrUpperRightAlpha < 0x100) * (char)greenAverageOrUpperRightAlpha -
                                           (0xff < greenAverageOrUpperRightAlpha),
                                           (blueOrAlphaAverage != 0) * (blueOrAlphaAverage < 0x100) * (char)blueOrAlphaAverage -
                                           (0xff < blueOrAlphaAverage)));
          *destinationCursor = (ushort)((ushort)(((uint)(averagedRgb >> 3) << 0x1b) >> 0x16) |
                             (ushort)(((uint)(averagedRgb >> 10) << 0x1a) >> 0x10)) >> 5 |
                     (ushort)(((uint)(clampedRedOrAlpha >> 3) << 0x1b) >> 0x10);
          sourceCursor = (AssetProducerSourceNames *)(sourceCursor->producerName + 1);
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = (AssetProducerSourceNames *)(sourceCursor->producerName + destinationWidth);
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          upperLeftSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)sourceCursor->producerName[0] * 8 + -0x28);
          upperRightSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)*(byte *)((int)sourceCursor->producerName + 1) * 8 + -0x28);
          lowerLeftSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)sourceCursor->producerName[destinationWidth] * 8 + -0x28);
          lowerRightSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)*(byte *)((int)sourceCursor->producerName + destinationWidth * 2 + 1) * 8 + -0x28);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 0x18);
          blueOrAlphaAverage = CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha);
          upperLeftRed = (undefined1)((uint)upperLeftSample >> 0x10);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 8);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 0x18);
          greenAverageOrUpperRightAlpha = CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha);
          upperRightRed = (undefined1)((uint)upperRightSample >> 0x10);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 8);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 0x18);
          redAverageOrLowerLeftAlpha = CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha);
          lowerLeftRed = (undefined1)((uint)lowerLeftSample >> 0x10);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 8);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 0x18);
          lowerRightAlpha = CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha);
          lowerRightRed = (undefined1)((uint)lowerRightSample >> 0x10);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 8);
          blueAverage = (ushort)((CONCAT11((char)upperLeftSample,(char)upperLeftSample) >> 4) +
                            (CONCAT11((char)upperRightSample,(char)upperRightSample) >> 4) +
                           (CONCAT11((char)lowerLeftSample,(char)lowerLeftSample) >> 4) +
                           (CONCAT11((char)lowerRightSample,(char)lowerRightSample) >> 4)) >> 6;
          greenAverage = (ushort)((CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha) >> 4) + (CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha) >> 4) +
                           (CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha) >> 4) + (CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha) >> 4)) >> 6;
          redAverage = (ushort)(((ushort)(CONCAT35(CONCAT21(blueOrAlphaAverage,upperLeftRed),CONCAT14(upperLeftRed,upperLeftSample)) >>
                                     0x20) >> 4) +
                            ((ushort)(CONCAT35(CONCAT21(greenAverageOrUpperRightAlpha,upperRightRed),CONCAT14(upperRightRed,upperRightSample)) >>
                                     0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(redAverageOrLowerLeftAlpha,lowerLeftRed),CONCAT14(lowerLeftRed,lowerLeftSample)) >>
                                    0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(lowerRightAlpha,lowerRightRed),CONCAT14(lowerRightRed,lowerRightSample)) >> 0x20
                                    ) >> 4)) >> 6;
          blueOrAlphaAverage = (ushort)((blueOrAlphaAverage >> 4) + (greenAverageOrUpperRightAlpha >> 4) + (redAverageOrLowerLeftAlpha >> 4) + (lowerRightAlpha >> 4)) >> 6;
          clampedRedOrAlpha = (blueOrAlphaAverage != 0) * (blueOrAlphaAverage < 0x100) * (char)blueOrAlphaAverage - (0xff < blueOrAlphaAverage);
          averagedArgb = CONCAT13(clampedRedOrAlpha,CONCAT12((redAverage != 0) * (redAverage < 0x100) * (char)redAverage -
                                           (0xff < redAverage),
                                           CONCAT11((greenAverage != 0) * (greenAverage < 0x100) * (char)greenAverage
                                                    - (0xff < greenAverage),
                                                    (blueAverage != 0) * (blueAverage < 0x100) * (char)blueAverage
                                                    - (0xff < blueAverage))));
          *destinationCursor = (ushort)((ushort)((ushort)(((averagedArgb >> 4) << 0x1c) >> 0x14) |
                                      (ushort)(((averagedArgb >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                             (ushort)(((averagedArgb >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                     (ushort)(((uint)(clampedRedOrAlpha >> 4) << 0x1c) >> 0x10);
          sourceCursor = (AssetProducerSourceNames *)(sourceCursor->producerName + 1);
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = (AssetProducerSourceNames *)(sourceCursor->producerName + destinationWidth);
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
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
  GraphicsTextureSourceAsset *asset;
  int entryPaletteIndex;
  undefined4 upperLeftSample;
  undefined4 upperRightSample;
  undefined4 lowerLeftSample;
  undefined4 lowerRightSample;
  byte clampedRedOrAlpha;
  ushort lowerRightAlpha;
  uint destinationWidth;
  int subresourceRecordOffset;
  word *sourceCursor;
  ushort *destinationCursor;
  undefined1 upperLeftGreenOrAlpha;
  ushort blueOrAlphaAverage;
  ushort blueAverage;
  undefined1 upperLeftRed;
  uint3 averagedRgb;
  ushort greenAverageOrUpperRightAlpha;
  ushort greenAverage;
  uint averagedArgb;
  ushort redAverageOrLowerLeftAlpha;
  ushort redAverage;
  undefined1 upperRightGreenOrAlpha;
  undefined1 upperRightRed;
  undefined1 lowerLeftGreenOrAlpha;
  undefined1 lowerLeftRed;
  undefined1 lowerRightGreenOrAlpha;
  undefined1 lowerRightRed;
  uint remainingColumns;
  uint remainingRows;
  
  asset = texture->sourceAsset;
  destinationCursor = (texture->glideInfo).data;
  subresourceRecordOffset = texture->subresourceIndex * 0x20 + (asset->tableDescriptor).subresourceTableOffset;
  entryPaletteIndex = *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0x20);
  sourceCursor = (word *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                    *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            subresourceRecordOffset + -0x1c) + -0x28);
  destinationWidth = *(uint *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0x10) >>
          2;
  remainingRows = *(uint *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0xc)
              >> 2;
  remainingColumns = destinationWidth;
  if (entryPaletteIndex < 0) {
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          upperLeftSample = *(undefined4 *)sourceCursor;
          upperRightSample = *(undefined4 *)((AssetProducerSourceNames *)(sourceCursor + 4))->producerName;
          lowerLeftSample = *(undefined4 *)(sourceCursor + destinationWidth * 0x10);
          lowerRightSample = *(undefined4 *)(sourceCursor + destinationWidth * 0x10 + 4);
          upperLeftRed = (undefined1)((uint)upperLeftSample >> 0x10);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 8);
          upperRightRed = (undefined1)((uint)upperRightSample >> 0x10);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 8);
          lowerLeftRed = (undefined1)((uint)lowerLeftSample >> 0x10);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 8);
          lowerRightRed = (undefined1)((uint)lowerRightSample >> 0x10);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 8);
          blueOrAlphaAverage = (ushort)((CONCAT11((char)upperLeftSample,(char)upperLeftSample) >> 4) +
                            (CONCAT11((char)upperRightSample,(char)upperRightSample) >> 4) +
                           (CONCAT11((char)lowerLeftSample,(char)lowerLeftSample) >> 4) +
                           (CONCAT11((char)lowerRightSample,(char)lowerRightSample) >> 4)) >> 6;
          greenAverageOrUpperRightAlpha = (ushort)((CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha) >> 4) + (CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha) >> 4) +
                           (CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha) >> 4) + (CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha) >> 4)) >> 6;
          redAverageOrLowerLeftAlpha = (ushort)((ushort)(CONCAT15(upperLeftRed,CONCAT14(upperLeftRed,upperLeftSample)) >> 0x24) +
                            (ushort)(CONCAT15(upperRightRed,CONCAT14(upperRightRed,upperRightSample)) >> 0x24) +
                           (ushort)(CONCAT15(lowerLeftRed,CONCAT14(lowerLeftRed,lowerLeftSample)) >> 0x24) +
                           (ushort)(CONCAT15(lowerRightRed,CONCAT14(lowerRightRed,lowerRightSample)) >> 0x24)) >> 6;
          clampedRedOrAlpha = (redAverageOrLowerLeftAlpha != 0) * (redAverageOrLowerLeftAlpha < 0x100) * (char)redAverageOrLowerLeftAlpha - (0xff < redAverageOrLowerLeftAlpha);
          averagedRgb = CONCAT12(clampedRedOrAlpha,CONCAT11((greenAverageOrUpperRightAlpha != 0) * (greenAverageOrUpperRightAlpha < 0x100) * (char)greenAverageOrUpperRightAlpha -
                                           (0xff < greenAverageOrUpperRightAlpha),
                                           (blueOrAlphaAverage != 0) * (blueOrAlphaAverage < 0x100) * (char)blueOrAlphaAverage -
                                           (0xff < blueOrAlphaAverage)));
          *destinationCursor = (ushort)((ushort)(((averagedRgb >> 3 & 0x1f) << 0x15) >> 0x10) |
                             (ushort)(((uint)(averagedRgb >> 10) << 0x1a) >> 0x10)) >> 5 |
                     (ushort)(((uint)(clampedRedOrAlpha >> 3) << 0x1b) >> 0x10);
          sourceCursor = sourceCursor + 8;
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = sourceCursor + destinationWidth * 0x18;
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          upperLeftSample = *(undefined4 *)sourceCursor;
          upperRightSample = *(undefined4 *)(sourceCursor + 4);
          lowerLeftSample = *(undefined4 *)(sourceCursor + destinationWidth * 0x10);
          lowerRightSample = *(undefined4 *)(sourceCursor + destinationWidth * 0x10 + 4);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 0x18);
          blueOrAlphaAverage = CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha);
          upperLeftRed = (undefined1)((uint)upperLeftSample >> 0x10);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 8);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 0x18);
          greenAverageOrUpperRightAlpha = CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha);
          upperRightRed = (undefined1)((uint)upperRightSample >> 0x10);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 8);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 0x18);
          redAverageOrLowerLeftAlpha = CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha);
          lowerLeftRed = (undefined1)((uint)lowerLeftSample >> 0x10);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 8);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 0x18);
          lowerRightAlpha = CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha);
          lowerRightRed = (undefined1)((uint)lowerRightSample >> 0x10);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 8);
          blueAverage = (ushort)((CONCAT11((char)upperLeftSample,(char)upperLeftSample) >> 4) +
                            (CONCAT11((char)upperRightSample,(char)upperRightSample) >> 4) +
                           (CONCAT11((char)lowerLeftSample,(char)lowerLeftSample) >> 4) +
                           (CONCAT11((char)lowerRightSample,(char)lowerRightSample) >> 4)) >> 6;
          greenAverage = (ushort)((CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha) >> 4) + (CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha) >> 4) +
                           (CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha) >> 4) + (CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha) >> 4)) >> 6;
          redAverage = (ushort)(((ushort)(CONCAT35(CONCAT21(blueOrAlphaAverage,upperLeftRed),CONCAT14(upperLeftRed,upperLeftSample)) >>
                                     0x20) >> 4) +
                            ((ushort)(CONCAT35(CONCAT21(greenAverageOrUpperRightAlpha,upperRightRed),CONCAT14(upperRightRed,upperRightSample)) >>
                                     0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(redAverageOrLowerLeftAlpha,lowerLeftRed),CONCAT14(lowerLeftRed,lowerLeftSample)) >>
                                    0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(lowerRightAlpha,lowerRightRed),CONCAT14(lowerRightRed,lowerRightSample)) >> 0x20
                                    ) >> 4)) >> 6;
          blueOrAlphaAverage = (ushort)((blueOrAlphaAverage >> 4) + (greenAverageOrUpperRightAlpha >> 4) + (redAverageOrLowerLeftAlpha >> 4) + (lowerRightAlpha >> 4)) >> 6;
          averagedRgb = CONCAT12((redAverage != 0) * (redAverage < 0x100) * (char)redAverage - (0xff < redAverage),
                            CONCAT11((greenAverage != 0) * (greenAverage < 0x100) * (char)greenAverage -
                                     (0xff < greenAverage),
                                     (blueAverage != 0) * (blueAverage < 0x100) * (char)blueAverage -
                                     (0xff < blueAverage)));
          clampedRedOrAlpha = (blueOrAlphaAverage != 0) * (blueOrAlphaAverage < 0x100) * (char)blueOrAlphaAverage - (0xff < blueOrAlphaAverage);
          averagedArgb = CONCAT13(clampedRedOrAlpha,averagedRgb);
          *destinationCursor = (ushort)((ushort)((ushort)((((averagedRgb & 0xf0) >> 4) << 0x18) >> 0x10) |
                                      (ushort)(((averagedArgb >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                             (ushort)(((averagedArgb >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                     (ushort)(((uint)(clampedRedOrAlpha >> 4) << 0x1c) >> 0x10);
          sourceCursor = sourceCursor + 8;
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = sourceCursor + destinationWidth * 0x18;
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
  }
  else {
    asset = texture->sourceAsset;
    if ((texture->glideInfo).format == 10) {
      do {
        do {
          upperLeftSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)*sourceCursor * 8 + -0x28);
          upperRightSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)sourceCursor[1] * 8 + -0x28);
          lowerLeftSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)sourceCursor[destinationWidth * 4] * 8 + -0x28);
          lowerRightSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)sourceCursor[destinationWidth * 4 + 1] * 8 + -0x28);
          upperLeftRed = (undefined1)((uint)upperLeftSample >> 0x10);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 8);
          upperRightRed = (undefined1)((uint)upperRightSample >> 0x10);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 8);
          lowerLeftRed = (undefined1)((uint)lowerLeftSample >> 0x10);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 8);
          lowerRightRed = (undefined1)((uint)lowerRightSample >> 0x10);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 8);
          blueOrAlphaAverage = (ushort)((CONCAT11((char)upperLeftSample,(char)upperLeftSample) >> 4) +
                            (CONCAT11((char)upperRightSample,(char)upperRightSample) >> 4) +
                           (CONCAT11((char)lowerLeftSample,(char)lowerLeftSample) >> 4) +
                           (CONCAT11((char)lowerRightSample,(char)lowerRightSample) >> 4)) >> 6;
          greenAverageOrUpperRightAlpha = (ushort)((CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha) >> 4) + (CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha) >> 4) +
                           (CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha) >> 4) + (CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha) >> 4)) >> 6;
          redAverageOrLowerLeftAlpha = (ushort)((ushort)(CONCAT15(upperLeftRed,CONCAT14(upperLeftRed,upperLeftSample)) >> 0x24) +
                            (ushort)(CONCAT15(upperRightRed,CONCAT14(upperRightRed,upperRightSample)) >> 0x24) +
                           (ushort)(CONCAT15(lowerLeftRed,CONCAT14(lowerLeftRed,lowerLeftSample)) >> 0x24) +
                           (ushort)(CONCAT15(lowerRightRed,CONCAT14(lowerRightRed,lowerRightSample)) >> 0x24)) >> 6;
          clampedRedOrAlpha = (redAverageOrLowerLeftAlpha != 0) * (redAverageOrLowerLeftAlpha < 0x100) * (char)redAverageOrLowerLeftAlpha - (0xff < redAverageOrLowerLeftAlpha);
          averagedRgb = CONCAT12(clampedRedOrAlpha,CONCAT11((greenAverageOrUpperRightAlpha != 0) * (greenAverageOrUpperRightAlpha < 0x100) * (char)greenAverageOrUpperRightAlpha -
                                           (0xff < greenAverageOrUpperRightAlpha),
                                           (blueOrAlphaAverage != 0) * (blueOrAlphaAverage < 0x100) * (char)blueOrAlphaAverage -
                                           (0xff < blueOrAlphaAverage)));
          *destinationCursor = (ushort)((ushort)(((uint)(averagedRgb >> 3) << 0x1b) >> 0x16) |
                             (ushort)(((uint)(averagedRgb >> 10) << 0x1a) >> 0x10)) >> 5 |
                     (ushort)(((uint)(clampedRedOrAlpha >> 3) << 0x1b) >> 0x10);
          sourceCursor = sourceCursor + 2;
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = sourceCursor + destinationWidth * 6;
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          upperLeftSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)*sourceCursor * 8 + -0x28);
          upperRightSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)sourceCursor[1] * 8 + -0x28);
          lowerLeftSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)sourceCursor[destinationWidth * 4] * 8 + -0x28);
          lowerRightSample = *(undefined4 *)
                   (asset[entryPaletteIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                   (uint)(byte)sourceCursor[destinationWidth * 4 + 1] * 8 + -0x28);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 0x18);
          blueOrAlphaAverage = CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha);
          upperLeftRed = (undefined1)((uint)upperLeftSample >> 0x10);
          upperLeftGreenOrAlpha = (undefined1)((uint)upperLeftSample >> 8);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 0x18);
          greenAverageOrUpperRightAlpha = CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha);
          upperRightRed = (undefined1)((uint)upperRightSample >> 0x10);
          upperRightGreenOrAlpha = (undefined1)((uint)upperRightSample >> 8);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 0x18);
          redAverageOrLowerLeftAlpha = CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha);
          lowerLeftRed = (undefined1)((uint)lowerLeftSample >> 0x10);
          lowerLeftGreenOrAlpha = (undefined1)((uint)lowerLeftSample >> 8);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 0x18);
          lowerRightAlpha = CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha);
          lowerRightRed = (undefined1)((uint)lowerRightSample >> 0x10);
          lowerRightGreenOrAlpha = (undefined1)((uint)lowerRightSample >> 8);
          blueAverage = (ushort)((CONCAT11((char)upperLeftSample,(char)upperLeftSample) >> 4) +
                            (CONCAT11((char)upperRightSample,(char)upperRightSample) >> 4) +
                           (CONCAT11((char)lowerLeftSample,(char)lowerLeftSample) >> 4) +
                           (CONCAT11((char)lowerRightSample,(char)lowerRightSample) >> 4)) >> 6;
          greenAverage = (ushort)((CONCAT11(upperLeftGreenOrAlpha,upperLeftGreenOrAlpha) >> 4) + (CONCAT11(upperRightGreenOrAlpha,upperRightGreenOrAlpha) >> 4) +
                           (CONCAT11(lowerLeftGreenOrAlpha,lowerLeftGreenOrAlpha) >> 4) + (CONCAT11(lowerRightGreenOrAlpha,lowerRightGreenOrAlpha) >> 4)) >> 6;
          redAverage = (ushort)(((ushort)(CONCAT35(CONCAT21(blueOrAlphaAverage,upperLeftRed),CONCAT14(upperLeftRed,upperLeftSample)) >>
                                     0x20) >> 4) +
                            ((ushort)(CONCAT35(CONCAT21(greenAverageOrUpperRightAlpha,upperRightRed),CONCAT14(upperRightRed,upperRightSample)) >>
                                     0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(redAverageOrLowerLeftAlpha,lowerLeftRed),CONCAT14(lowerLeftRed,lowerLeftSample)) >>
                                    0x20) >> 4) +
                           ((ushort)(CONCAT35(CONCAT21(lowerRightAlpha,lowerRightRed),CONCAT14(lowerRightRed,lowerRightSample)) >> 0x20
                                    ) >> 4)) >> 6;
          blueOrAlphaAverage = (ushort)((blueOrAlphaAverage >> 4) + (greenAverageOrUpperRightAlpha >> 4) + (redAverageOrLowerLeftAlpha >> 4) + (lowerRightAlpha >> 4)) >> 6;
          clampedRedOrAlpha = (blueOrAlphaAverage != 0) * (blueOrAlphaAverage < 0x100) * (char)blueOrAlphaAverage - (0xff < blueOrAlphaAverage);
          averagedArgb = CONCAT13(clampedRedOrAlpha,CONCAT12((redAverage != 0) * (redAverage < 0x100) * (char)redAverage -
                                           (0xff < redAverage),
                                           CONCAT11((greenAverage != 0) * (greenAverage < 0x100) * (char)greenAverage
                                                    - (0xff < greenAverage),
                                                    (blueAverage != 0) * (blueAverage < 0x100) * (char)blueAverage
                                                    - (0xff < blueAverage))));
          *destinationCursor = (ushort)((ushort)((ushort)(((averagedArgb >> 4) << 0x1c) >> 0x14) |
                                      (ushort)(((averagedArgb >> 0xc) << 0x1c) >> 0x10)) >> 4 |
                             (ushort)(((averagedArgb >> 0x14) << 0x1c) >> 0x10)) >> 4 |
                     (ushort)(((uint)(clampedRedOrAlpha >> 4) << 0x1c) >> 0x10);
          sourceCursor = sourceCursor + 2;
          destinationCursor = destinationCursor + 1;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = sourceCursor + destinationWidth * 6;
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
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
  GraphicsTextureSourceAsset *asset;
  int sourceWidth;
  int subresourceRecordOffset;
  ushort *glideDataCursor;
  ushort *nextGlideDataCursor;
  int remainingColumns;
  int remainingRows;
  
  asset = texture->sourceAsset;
  glideDataCursor = (texture->glideInfo).data;
  subresourceRecordOffset = texture->subresourceIndex * 0x20 + (asset->tableDescriptor).subresourceTableOffset;
  sourceWidth = *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0x10);
  remainingRows = *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 + subresourceRecordOffset + -0xc);
  remainingColumns = sourceWidth;
  do {
    do {
      nextGlideDataCursor = glideDataCursor + 1;
      *glideDataCursor = 0xfff;
      remainingColumns = remainingColumns + -1;
      glideDataCursor = nextGlideDataCursor;
    } while (remainingColumns != 0);
    remainingRows = remainingRows + -1;
    remainingColumns = sourceWidth;
  } while (remainingRows != 0);
  return;
}

/* Address: 0x00580FF0.
   Ownership: graphics/backend/glide.
   Purpose: Downsamples alpha samples into white ARGB4444 texels in the dormant Glide path.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsGlide3_DownsampleAlpha8ToWhiteArgb4444(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *asset;
  byte firstAlphaQuarter;
  uint downsampledWidth;
  int subresourceRecordOffset;
  ushort *sourcePairCursor;
  ushort *glideDataCursor;
  uint remainingColumns;
  uint remainingRows;
  
  asset = texture->sourceAsset;
  glideDataCursor = (texture->glideInfo).data;
  subresourceRecordOffset =
       texture->subresourceIndex * 0x20 + (asset->tableDescriptor).subresourceTableOffset;
  sourcePairCursor =
       (ushort *)
       ((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
       *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
               subresourceRecordOffset + -0x1c) + -0x28);
  downsampledWidth =
       *(uint *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                subresourceRecordOffset + -0x10) >> 1;
  remainingRows =
       *(uint *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                subresourceRecordOffset + -0xc) >> 1;
  remainingColumns = downsampledWidth;
  do {
    do {
      firstAlphaQuarter = (byte)*sourcePairCursor >> 2;
      *glideDataCursor =
           CONCAT11((byte)(*sourcePairCursor >> 10) + firstAlphaQuarter +
                    (byte)(sourcePairCursor[downsampledWidth] >> 10) +
                    ((byte)sourcePairCursor[downsampledWidth] >> 2),firstAlphaQuarter) | 0xfff;
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
  GraphicsTextureSourceAsset *asset;
  uint downsampledWidth;
  int subresourceRecordOffset;
  byte *sourceByteCursor;
  ushort *glideDataCursor;
  uint remainingColumns;
  uint remainingRows;
  
  asset = texture->sourceAsset;
  glideDataCursor = (texture->glideInfo).data;
  subresourceRecordOffset =
       texture->subresourceIndex * 0x20 + (asset->tableDescriptor).subresourceTableOffset;
  sourceByteCursor =
       (asset->common).buildMetadata.assetRelativeAddressAnchor28 +
       *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
               subresourceRecordOffset + -0x1c) + -0x28;
  downsampledWidth =
       *(uint *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                subresourceRecordOffset + -0x10) >> 1;
  remainingRows =
       *(uint *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
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
  int recordOffsetOrRowSkip;
  undefined2 framebufferPixel;
  int widthOrPaletteIndex;
  int indexedSourceWidth;
  uint sourceArgb;
  int leftOrRemainingColumns;
  GraphicsPixelDimension clippedBottom;
  int clippedTop;
  GraphicsPixelDimension clippedRight;
  int clippedWidth;
  byte *sourceIndexCursor;
  uint *sourceArgbCursor;
  byte *destinationCursor;
  ulonglong destinationLanes;
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
      recordOffsetOrRowSkip = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      leftOrRemainingColumns = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRowSkip + -0x18);
      clippedTop = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRowSkip + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x20)
          == -1) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          widthOrPaletteIndex = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceArgbCursor = (uint *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * widthOrPaletteIndex * 4 +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          do {
            do {
              sourceArgb = *sourceArgbCursor;
              if (0xffffff < sourceArgb) {
                if (sourceArgb < 0xff000000) {
                  framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                  mm1PackedValue1ByteLane3 = (byte)(sourceArgb >> 0x18);
                  mm1PackedValue1ByteLane2 = (byte)(sourceArgb >> 0x10);
                  mm1PackedValue1ByteLane1 = (byte)(sourceArgb >> 8);
                  destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                  destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                           THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                  mm1PackedValue1 =
                       pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                       >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                                  ,mm1PackedValue1ByteLane2),
                                                  CONCAT14(mm1PackedValue1ByteLane2,sourceArgb)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                                         mm1PackedValue1ByteLane1)
                                                                >> 2,CONCAT11((char)sourceArgb,
                                                                              (char)sourceArgb) >> 2))),
                              g_SoftwareBlendAlphaFactors[sourceArgb >> 0x18]);
                  mm0PackedValue2 =
                       pmulhw(CONCAT26((ushort)((short)(destinationLanes >> 0x30) *
                                               g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                       CONCAT24((ushort)((short)(destinationLanes >> 0x20) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.red
                                                        ) >> 2,
                                                CONCAT22((ushort)((short)(destinationLanes >> 0x10) *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.green) >> 2,
                                                         (ushort)((short)destinationLanes *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.blue) >> 2))),
                              g_SoftwareBlendInverseAlphaFactors[sourceArgb >> 0x18]);
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
                               THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                               g_SoftwarePixelMmxConstants.packWeights);
                  *(short *)destinationCursor =
                       (short)((ulonglong)mm0PackedValue3 >> 8) +
                       (short)((ulonglong)mm0PackedValue3 >> 0x28);
                }
                else {
                  *(short *)destinationCursor =
                       (short)g_SoftwarePixelPackTables->blue[sourceArgb & 0xff] +
                       (short)*(undefined4 *)
                               ((int)g_SoftwarePixelPackTables->green + ((sourceArgb & 0xff00) >> 6)) +
                       (short)*(undefined4 *)
                               ((int)g_SoftwarePixelPackTables->red + ((sourceArgb & 0xff0000) >> 0xe));
                }
              }
              sourceArgbCursor = sourceArgbCursor + 1;
              destinationCursor = destinationCursor + 2;
              leftOrRemainingColumns = leftOrRemainingColumns + -1;
            } while (leftOrRemainingColumns != 0);
            sourceArgbCursor = sourceArgbCursor + (widthOrPaletteIndex - clippedWidth);
            destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
            clipMinY = clipMinY + -1;
            leftOrRemainingColumns = clippedWidth;
          } while (clipMinY != 0);
          return false;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        recordOffsetOrRowSkip + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          widthOrPaletteIndex = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x20);
          indexedSourceWidth = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceIndexCursor = (byte *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * indexedSourceWidth +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          do {
            do {
              sourceArgb = *(uint *)(sourceAsset[widthOrPaletteIndex * 4 + 1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)*sourceIndexCursor * 8 + -0x24);
              if (0xffffff < sourceArgb) {
                if (sourceArgb < 0xff000000) {
                  sourceArgb = *(uint *)(sourceAsset[widthOrPaletteIndex * 4 + 1].common.buildMetadata.
                                    assetRelativeAddressAnchor28 + (uint)*sourceIndexCursor * 8 + -0x28);
                  framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                  mm1PackedValue0ByteLane3 = (byte)(sourceArgb >> 0x18);
                  mm1PackedValue0ByteLane2 = (byte)(sourceArgb >> 0x10);
                  mm1PackedValue0ByteLane1 = (byte)(sourceArgb >> 8);
                  destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                  destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                           THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                  mm1PackedValue0 =
                       pmulhw(CONCAT26(CONCAT11(mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                       >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                                  ,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,sourceArgb)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                         mm1PackedValue0ByteLane1)
                                                                >> 2,CONCAT11((char)sourceArgb,
                                                                              (char)sourceArgb) >> 2))),
                              g_SoftwareBlendAlphaFactors[sourceArgb >> 0x18]);
                  mm0PackedValue0 =
                       pmulhw(CONCAT26((ushort)((short)(destinationLanes >> 0x30) *
                                               g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                       CONCAT24((ushort)((short)(destinationLanes >> 0x20) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.red
                                                        ) >> 2,
                                                CONCAT22((ushort)((short)(destinationLanes >> 0x10) *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.green) >> 2,
                                                         (ushort)((short)destinationLanes *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.blue) >> 2))),
                              g_SoftwareBlendInverseAlphaFactors[sourceArgb >> 0x18]);
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
                               THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                               g_SoftwarePixelMmxConstants.packWeights);
                  *(short *)destinationCursor =
                       (short)((ulonglong)mm0PackedValue1 >> 8) +
                       (short)((ulonglong)mm0PackedValue1 >> 0x28);
                }
                else {
                  *(short *)destinationCursor = (short)sourceArgb;
                }
              }
              sourceIndexCursor = sourceIndexCursor + 1;
              destinationCursor = destinationCursor + 2;
              leftOrRemainingColumns = leftOrRemainingColumns + -1;
            } while (leftOrRemainingColumns != 0);
            sourceIndexCursor = sourceIndexCursor + (indexedSourceWidth - clippedWidth);
            destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
            clipMinY = clipMinY + -1;
            leftOrRemainingColumns = clippedWidth;
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
  int recordOffsetOrRowSkip;
  undefined2 framebufferPixel;
  int widthOrPaletteIndex;
  int indexedSourceWidth;
  uint sourceArgb;
  int leftOrRemainingColumns;
  GraphicsPixelDimension clippedBottom;
  int clippedTop;
  GraphicsPixelDimension clippedRight;
  int clippedWidth;
  byte *sourceIndexCursor;
  uint *sourceArgbCursor;
  byte *destinationCursor;
  ulonglong destinationLanes;
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
      recordOffsetOrRowSkip = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      leftOrRemainingColumns = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRowSkip + -0x18);
      clippedTop = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRowSkip + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x20)
          == -1) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          widthOrPaletteIndex = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceArgbCursor = (uint *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * widthOrPaletteIndex * 4 +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          do {
            do {
              sourceArgb = *sourceArgbCursor;
              if (0xffffff < sourceArgb) {
                framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                mm1PackedValue1ByteLane3 = (byte)(sourceArgb >> 0x18);
                mm1PackedValue1ByteLane2 = (byte)(sourceArgb >> 0x10);
                mm1PackedValue1ByteLane1 = (byte)(sourceArgb >> 8);
                destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                         THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                mm1PackedValue1 =
                     pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3) >>
                                     3,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                                  ,mm1PackedValue1ByteLane2),
                                                  CONCAT14(mm1PackedValue1ByteLane2,sourceArgb)) >> 0x20)
                                                >> 3,CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                                       mm1PackedValue1ByteLane1) >>
                                                              3,CONCAT11((char)sourceArgb,(char)sourceArgb) >>
                                                                3))),
                            g_SoftwareBlendAlphaFactors[sourceArgb >> 0x18]);
                mm0PackedValue2 =
                     pmulhw(CONCAT26((ushort)((short)(destinationLanes >> 0x30) *
                                             g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                     CONCAT24((ushort)((short)(destinationLanes >> 0x20) *
                                                      g_SoftwarePixelMmxConstants.unpackScales.red)
                                              >> 2,CONCAT22((ushort)((short)(destinationLanes >> 0x10) *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.green) >> 2,
                                                            (ushort)((short)destinationLanes *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.blue) >> 2))),
                            g_SoftwareBlendInverseAlphaFactors[sourceArgb >> 0x18]);
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
                             THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)destinationCursor =
                     (short)((ulonglong)mm0PackedValue3 >> 8) +
                     (short)((ulonglong)mm0PackedValue3 >> 0x28);
              }
              sourceArgbCursor = sourceArgbCursor + 1;
              destinationCursor = destinationCursor + 2;
              leftOrRemainingColumns = leftOrRemainingColumns + -1;
            } while (leftOrRemainingColumns != 0);
            sourceArgbCursor = sourceArgbCursor + (widthOrPaletteIndex - clippedWidth);
            destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
            clipMinY = clipMinY + -1;
            leftOrRemainingColumns = clippedWidth;
          } while (clipMinY != 0);
          return false;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        recordOffsetOrRowSkip + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          widthOrPaletteIndex = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x20);
          indexedSourceWidth = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceIndexCursor = (byte *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * indexedSourceWidth +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          do {
            do {
              sourceArgb = *(uint *)(sourceAsset[widthOrPaletteIndex * 4 + 1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)*sourceIndexCursor * 8 + -0x28);
              if (0xffffff < sourceArgb) {
                framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                mm1PackedValue0ByteLane3 = (byte)(sourceArgb >> 0x18);
                mm1PackedValue0ByteLane2 = (byte)(sourceArgb >> 0x10);
                mm1PackedValue0ByteLane1 = (byte)(sourceArgb >> 8);
                destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                         THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                mm1PackedValue0 =
                     pmulhw(CONCAT26(CONCAT11(mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3) >>
                                     3,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                                  ,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,sourceArgb)) >> 0x20)
                                                >> 3,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                       mm1PackedValue0ByteLane1) >>
                                                              3,CONCAT11((char)sourceArgb,(char)sourceArgb) >>
                                                                3))),
                            g_SoftwareBlendAlphaFactors[sourceArgb >> 0x18]);
                mm0PackedValue0 =
                     pmulhw(CONCAT26((ushort)((short)(destinationLanes >> 0x30) *
                                             g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                     CONCAT24((ushort)((short)(destinationLanes >> 0x20) *
                                                      g_SoftwarePixelMmxConstants.unpackScales.red)
                                              >> 2,CONCAT22((ushort)((short)(destinationLanes >> 0x10) *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.green) >> 2,
                                                            (ushort)((short)destinationLanes *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.blue) >> 2))),
                            g_SoftwareBlendInverseAlphaFactors[sourceArgb >> 0x18]);
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
                             THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)destinationCursor =
                     (short)((ulonglong)mm0PackedValue1 >> 8) +
                     (short)((ulonglong)mm0PackedValue1 >> 0x28);
              }
              sourceIndexCursor = sourceIndexCursor + 1;
              destinationCursor = destinationCursor + 2;
              leftOrRemainingColumns = leftOrRemainingColumns + -1;
            } while (leftOrRemainingColumns != 0);
            sourceIndexCursor = sourceIndexCursor + (indexedSourceWidth - clippedWidth);
            destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
            clipMinY = clipMinY + -1;
            leftOrRemainingColumns = clippedWidth;
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
  int lowerRowOffset;
  int recordOffsetOrSourceStride;
  undefined4 firstUpperLeft;
  undefined4 firstUpperRight;
  undefined4 firstLowerLeft;
  undefined4 firstLowerRight;
  undefined4 secondUpperLeft;
  undefined4 secondUpperRight;
  undefined4 secondLowerLeft;
  undefined4 secondLowerRight;
  char firstClampedLane0;
  char secondClampedLane0;
  GraphicsPixelDimension framebufferWidth;
  uint widthMinusOneOrRemainingPairs;
  int sourceStepX;
  uint sourceHeightMinusOne;
  uint destinationHeightMinusOne;
  uint sourceXFixed;
  uint columnOrFractionX;
  uint sourceYFixed;
  uint fractionY;
  byte *sourcePixels;
  byte *sourceRowCursor;
  byte *destinationCursor;
  ushort firstLane0OrAlpha;
  undefined1 mm0PackedValue0ByteLane1;
  undefined1 mm0PackedValue0ByteLane2;
  ushort firstLane1OrAlpha;
  ushort firstLane2OrAlpha;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  ushort firstLane3OrAlpha;
  ulonglong firstShiftedLanes;
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
  ushort secondLane0OrAlpha;
  undefined1 mm4PackedValue0ByteLane1;
  undefined1 mm4PackedValue0ByteLane2;
  ushort secondLane1OrAlpha;
  ushort secondLane2OrAlpha;
  undefined8 mm4PackedValue0;
  undefined8 mm4PackedValue1;
  ushort secondLane3OrAlpha;
  ulonglong secondShiftedLanes;
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
  byte *destinationRowStart;
  char firstClampedLane1;
  char firstClampedLane2;
  char firstClampedLane3;
  char secondClampedLane1;
  char secondClampedLane2;
  char secondClampedLane3;
  
  framebufferWidth = g_DisplayFramebufferAccess.width;
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (((((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
         (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) &&
        (recordOffsetOrSourceStride = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset,
        g_DisplayFramebufferAccess.bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT)) &&
       (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + recordOffsetOrSourceStride + -0x20)
        == -1)) {
      destinationCursor = g_DisplayFramebufferAccess.pixels +
                (destinationY * g_DisplayFramebufferAccess.width + destinationX) * 2;
      widthMinusOneOrRemainingPairs = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                       recordOffsetOrSourceStride + -0x10) - 1;
      sourceStepX = (int)(((ulonglong)(widthMinusOneOrRemainingPairs >> 0x18) << 0x20 | (ulonglong)widthMinusOneOrRemainingPairs * 0x100 & 0xffffffff)
                    / (ulonglong)(destinationWidth - 1));
      sourceHeightMinusOne = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                       recordOffsetOrSourceStride + -0xc) - 1;
      destinationHeightMinusOne = destinationHeight - 1;
      sourcePixels = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        recordOffsetOrSourceStride + -0x1c) + -0x28;
      recordOffsetOrSourceStride = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                      recordOffsetOrSourceStride + -0x10) * 4;
      sourceXFixed = 0;
      sourceYFixed = 0;
      sourceRowCursor = sourcePixels;
      widthMinusOneOrRemainingPairs = destinationWidth >> 1;
      destinationRowStart = destinationCursor;
      do {
        do {
          columnOrFractionX = sourceXFixed >> 8;
          lowerRowOffset = recordOffsetOrSourceStride + columnOrFractionX * 4;
          firstUpperLeft = *(undefined4 *)(sourceRowCursor + columnOrFractionX * 4);
          firstUpperRight = *(undefined4 *)(sourceRowCursor + columnOrFractionX * 4 + 4);
          firstLowerLeft = *(undefined4 *)(sourceRowCursor + lowerRowOffset);
          firstLowerRight = *(undefined4 *)(sourceRowCursor + lowerRowOffset + 4);
          columnOrFractionX = sourceXFixed + sourceStepX >> 8;
          lowerRowOffset = recordOffsetOrSourceStride + columnOrFractionX * 4;
          secondUpperLeft = *(undefined4 *)(sourceRowCursor + columnOrFractionX * 4);
          secondUpperRight = *(undefined4 *)(sourceRowCursor + columnOrFractionX * 4 + 4);
          secondLowerLeft = *(undefined4 *)(sourceRowCursor + lowerRowOffset);
          secondLowerRight = *(undefined4 *)(sourceRowCursor + lowerRowOffset + 4);
          mm0PackedValue0ByteLane1 = (undefined1)((uint)firstUpperLeft >> 0x18);
          firstLane0OrAlpha = CONCAT11(mm0PackedValue0ByteLane1,mm0PackedValue0ByteLane1);
          mm0PackedValue0ByteLane2 = (undefined1)((uint)firstUpperLeft >> 0x10);
          mm0PackedValue0ByteLane1 = (undefined1)((uint)firstUpperLeft >> 8);
          mm1PackedValue0ByteLane1 = (undefined1)((uint)firstUpperRight >> 0x18);
          firstLane1OrAlpha = CONCAT11(mm1PackedValue0ByteLane1,mm1PackedValue0ByteLane1);
          mm1PackedValue0ByteLane2 = (undefined1)((uint)firstUpperRight >> 0x10);
          mm1PackedValue0ByteLane1 = (undefined1)((uint)firstUpperRight >> 8);
          mm2PackedValue0ByteLane1 = (undefined1)((uint)firstLowerLeft >> 0x18);
          firstLane2OrAlpha = CONCAT11(mm2PackedValue0ByteLane1,mm2PackedValue0ByteLane1);
          mm2PackedValue0ByteLane2 = (undefined1)((uint)firstLowerLeft >> 0x10);
          mm2PackedValue0ByteLane1 = (undefined1)((uint)firstLowerLeft >> 8);
          mm3PackedValue0ByteLane1 = (undefined1)((uint)firstLowerRight >> 0x18);
          firstLane3OrAlpha = CONCAT11(mm3PackedValue0ByteLane1,mm3PackedValue0ByteLane1);
          mm3PackedValue0ByteLane2 = (undefined1)((uint)firstLowerRight >> 0x10);
          mm3PackedValue0ByteLane1 = (undefined1)((uint)firstLowerRight >> 8);
          mm4PackedValue0ByteLane1 = (undefined1)((uint)secondUpperLeft >> 0x18);
          secondLane0OrAlpha = CONCAT11(mm4PackedValue0ByteLane1,mm4PackedValue0ByteLane1);
          mm4PackedValue0ByteLane2 = (undefined1)((uint)secondUpperLeft >> 0x10);
          mm4PackedValue0ByteLane1 = (undefined1)((uint)secondUpperLeft >> 8);
          mm5PackedValue0ByteLane1 = (undefined1)((uint)secondUpperRight >> 0x18);
          secondLane1OrAlpha = CONCAT11(mm5PackedValue0ByteLane1,mm5PackedValue0ByteLane1);
          mm5PackedValue0ByteLane2 = (undefined1)((uint)secondUpperRight >> 0x10);
          mm5PackedValue0ByteLane1 = (undefined1)((uint)secondUpperRight >> 8);
          mm6PackedValue0ByteLane1 = (undefined1)((uint)secondLowerLeft >> 0x18);
          secondLane2OrAlpha = CONCAT11(mm6PackedValue0ByteLane1,mm6PackedValue0ByteLane1);
          mm6PackedValue0ByteLane2 = (undefined1)((uint)secondLowerLeft >> 0x10);
          mm6PackedValue0ByteLane1 = (undefined1)((uint)secondLowerLeft >> 8);
          mm7PackedValue0ByteLane1 = (undefined1)((uint)secondLowerRight >> 0x18);
          secondLane3OrAlpha = CONCAT11(mm7PackedValue0ByteLane1,mm7PackedValue0ByteLane1);
          mm7PackedValue0ByteLane2 = (undefined1)((uint)secondLowerRight >> 0x10);
          mm7PackedValue0ByteLane1 = (undefined1)((uint)secondLowerRight >> 8);
          columnOrFractionX = sourceXFixed & 0xff;
          fractionY = sourceYFixed & 0xff;
          mm0PackedValue0 =
               pmulhw(CONCAT26(firstLane0OrAlpha >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(firstLane0OrAlpha,mm0PackedValue0ByteLane2),
                                                          CONCAT14(mm0PackedValue0ByteLane2,firstUpperLeft))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                          mm0PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)firstUpperLeft,(char)firstUpperLeft) >> 2))),
                      g_SoftwareBilinearInverseFactors[columnOrFractionX]);
          mm1PackedValue0 =
               pmulhw(CONCAT26(firstLane1OrAlpha >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(firstLane1OrAlpha,mm1PackedValue0ByteLane2),
                                                          CONCAT14(mm1PackedValue0ByteLane2,firstUpperRight))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                          mm1PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)firstUpperRight,(char)firstUpperRight) >> 2))),
                      g_SoftwareBilinearForwardFactors[columnOrFractionX]);
          mm2PackedValue0 =
               pmulhw(CONCAT26(firstLane2OrAlpha >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(firstLane2OrAlpha,mm2PackedValue0ByteLane2),
                                                          CONCAT14(mm2PackedValue0ByteLane2,firstLowerLeft))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm2PackedValue0ByteLane1,
                                                          mm2PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)firstLowerLeft,(char)firstLowerLeft) >> 2))),
                      g_SoftwareBilinearInverseFactors[columnOrFractionX]);
          mm3PackedValue0 =
               pmulhw(CONCAT26(firstLane3OrAlpha >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(firstLane3OrAlpha,mm3PackedValue0ByteLane2),
                                                          CONCAT14(mm3PackedValue0ByteLane2,firstLowerRight))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm3PackedValue0ByteLane1,
                                                          mm3PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)firstLowerRight,(char)firstLowerRight) >> 2))),
                      g_SoftwareBilinearForwardFactors[columnOrFractionX]);
          mm0PackedValue1 =
               pmulhw(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                               (short)((ulonglong)mm1PackedValue0 >> 0x30),
                               CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                        (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                        CONCAT22((short)((ulonglong)mm0PackedValue0 >> 0x10) +
                                                 (short)((ulonglong)mm1PackedValue0 >> 0x10),
                                                 (short)mm0PackedValue0 + (short)mm1PackedValue0))),
                      g_SoftwareBilinearInverseFactors[fractionY]);
          mm2PackedValue1 =
               pmulhw(CONCAT26((short)((ulonglong)mm2PackedValue0 >> 0x30) +
                               (short)((ulonglong)mm3PackedValue0 >> 0x30),
                               CONCAT24((short)((ulonglong)mm2PackedValue0 >> 0x20) +
                                        (short)((ulonglong)mm3PackedValue0 >> 0x20),
                                        CONCAT22((short)((ulonglong)mm2PackedValue0 >> 0x10) +
                                                 (short)((ulonglong)mm3PackedValue0 >> 0x10),
                                                 (short)mm2PackedValue0 + (short)mm3PackedValue0))),
                      g_SoftwareBilinearForwardFactors[fractionY]);
          columnOrFractionX = sourceXFixed + sourceStepX & 0xff;
          mm4PackedValue0 =
               pmulhw(CONCAT26(secondLane0OrAlpha >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(secondLane0OrAlpha,mm4PackedValue0ByteLane2),
                                                          CONCAT14(mm4PackedValue0ByteLane2,secondUpperLeft))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm4PackedValue0ByteLane1,
                                                          mm4PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)secondUpperLeft,(char)secondUpperLeft) >> 2))),
                      g_SoftwareBilinearInverseFactors[columnOrFractionX]);
          mm5PackedValue0 =
               pmulhw(CONCAT26(secondLane1OrAlpha >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(secondLane1OrAlpha,mm5PackedValue0ByteLane2),
                                                          CONCAT14(mm5PackedValue0ByteLane2,secondUpperRight))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm5PackedValue0ByteLane1,
                                                          mm5PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)secondUpperRight,(char)secondUpperRight) >> 2))),
                      g_SoftwareBilinearForwardFactors[columnOrFractionX]);
          mm6PackedValue0 =
               pmulhw(CONCAT26(secondLane2OrAlpha >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(secondLane2OrAlpha,mm6PackedValue0ByteLane2),
                                                          CONCAT14(mm6PackedValue0ByteLane2,secondLowerLeft))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm6PackedValue0ByteLane1,
                                                          mm6PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)secondLowerLeft,(char)secondLowerLeft) >> 2))),
                      g_SoftwareBilinearInverseFactors[columnOrFractionX]);
          mm7PackedValue0 =
               pmulhw(CONCAT26(secondLane3OrAlpha >> 2,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(secondLane3OrAlpha,mm7PackedValue0ByteLane2),
                                                          CONCAT14(mm7PackedValue0ByteLane2,secondLowerRight))
                                                >> 0x20) >> 2,
                                        CONCAT22(CONCAT11(mm7PackedValue0ByteLane1,
                                                          mm7PackedValue0ByteLane1) >> 2,
                                                 CONCAT11((char)secondLowerRight,(char)secondLowerRight) >> 2))),
                      g_SoftwareBilinearForwardFactors[columnOrFractionX]);
          mm4PackedValue1 =
               pmulhw(CONCAT26((short)((ulonglong)mm4PackedValue0 >> 0x30) +
                               (short)((ulonglong)mm5PackedValue0 >> 0x30),
                               CONCAT24((short)((ulonglong)mm4PackedValue0 >> 0x20) +
                                        (short)((ulonglong)mm5PackedValue0 >> 0x20),
                                        CONCAT22((short)((ulonglong)mm4PackedValue0 >> 0x10) +
                                                 (short)((ulonglong)mm5PackedValue0 >> 0x10),
                                                 (short)mm4PackedValue0 + (short)mm5PackedValue0))),
                      g_SoftwareBilinearInverseFactors[fractionY]);
          mm6PackedValue1 =
               pmulhw(CONCAT26((short)((ulonglong)mm6PackedValue0 >> 0x30) +
                               (short)((ulonglong)mm7PackedValue0 >> 0x30),
                               CONCAT24((short)((ulonglong)mm6PackedValue0 >> 0x20) +
                                        (short)((ulonglong)mm7PackedValue0 >> 0x20),
                                        CONCAT22((short)((ulonglong)mm6PackedValue0 >> 0x10) +
                                                 (short)((ulonglong)mm7PackedValue0 >> 0x10),
                                                 (short)mm6PackedValue0 + (short)mm7PackedValue0))),
                      g_SoftwareBilinearForwardFactors[fractionY]);
          firstLane0OrAlpha = (ushort)((short)mm0PackedValue1 + (short)mm2PackedValue1) >> 2;
          firstLane1OrAlpha = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x10) +
                           (short)((ulonglong)mm2PackedValue1 >> 0x10)) >> 2;
          firstLane2OrAlpha = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x20) +
                           (short)((ulonglong)mm2PackedValue1 >> 0x20)) >> 2;
          firstLane3OrAlpha = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x30) +
                           (short)((ulonglong)mm2PackedValue1 >> 0x30)) >> 2;
          secondLane0OrAlpha = (ushort)((short)mm4PackedValue1 + (short)mm6PackedValue1) >> 2;
          secondLane1OrAlpha = (ushort)((short)((ulonglong)mm4PackedValue1 >> 0x10) +
                           (short)((ulonglong)mm6PackedValue1 >> 0x10)) >> 2;
          secondLane2OrAlpha = (ushort)((short)((ulonglong)mm4PackedValue1 >> 0x20) +
                           (short)((ulonglong)mm6PackedValue1 >> 0x20)) >> 2;
          secondLane3OrAlpha = (ushort)((short)((ulonglong)mm4PackedValue1 >> 0x30) +
                           (short)((ulonglong)mm6PackedValue1 >> 0x30)) >> 2;
          firstClampedLane0 = (firstLane0OrAlpha != 0) * (firstLane0OrAlpha < 0x100) * (char)firstLane0OrAlpha - (0xff < firstLane0OrAlpha);
          firstClampedLane1 = (firstLane1OrAlpha != 0) * (firstLane1OrAlpha < 0x100) * (char)firstLane1OrAlpha - (0xff < firstLane1OrAlpha);
          firstClampedLane2 = (firstLane2OrAlpha != 0) * (firstLane2OrAlpha < 0x100) * (char)firstLane2OrAlpha - (0xff < firstLane2OrAlpha);
          firstClampedLane3 = (firstLane3OrAlpha != 0) * (firstLane3OrAlpha < 0x100) * (char)firstLane3OrAlpha - (0xff < firstLane3OrAlpha);
          secondClampedLane0 = (secondLane0OrAlpha != 0) * (secondLane0OrAlpha < 0x100) * (char)secondLane0OrAlpha - (0xff < secondLane0OrAlpha);
          secondClampedLane1 = (secondLane1OrAlpha != 0) * (secondLane1OrAlpha < 0x100) * (char)secondLane1OrAlpha - (0xff < secondLane1OrAlpha);
          secondClampedLane2 = (secondLane2OrAlpha != 0) * (secondLane2OrAlpha < 0x100) * (char)secondLane2OrAlpha - (0xff < secondLane2OrAlpha);
          secondClampedLane3 = (secondLane3OrAlpha != 0) * (secondLane3OrAlpha < 0x100) * (char)secondLane3OrAlpha - (0xff < secondLane3OrAlpha);
          firstShiftedLanes = psllw(CONCAT62(CONCAT51(CONCAT41(CONCAT31(CONCAT21(CONCAT11(firstClampedLane3,firstClampedLane3),firstClampedLane2
                                                                     ),firstClampedLane2),firstClampedLane1),firstClampedLane1),
                                  CONCAT11(firstClampedLane0,firstClampedLane0)),4);
          secondShiftedLanes = psllw(CONCAT62(CONCAT51(CONCAT41(CONCAT31(CONCAT21(CONCAT11(secondClampedLane3,secondClampedLane3),secondClampedLane2
                                                                     ),secondClampedLane2),secondClampedLane1),secondClampedLane1),
                                  CONCAT11(secondClampedLane0,secondClampedLane0)),4);
          mm0PackedValue2 =
               pmaddwd(firstShiftedLanes & THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                       g_SoftwarePixelMmxConstants.packWeights);
          mm4PackedValue2 =
               pmaddwd(secondShiftedLanes & THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                       g_SoftwarePixelMmxConstants.packWeights);
          *(uint *)destinationCursor =
               CONCAT22((short)((ulonglong)mm4PackedValue2 >> 8) +
                        (short)((ulonglong)mm4PackedValue2 >> 0x28),
                        (short)((ulonglong)mm0PackedValue2 >> 8) +
                        (short)((ulonglong)mm0PackedValue2 >> 0x28));
          sourceXFixed = sourceXFixed + sourceStepX * 2;
          destinationCursor = destinationCursor + 4;
          widthMinusOneOrRemainingPairs = widthMinusOneOrRemainingPairs - 1;
        } while (widthMinusOneOrRemainingPairs != 0);
        sourceYFixed = sourceYFixed + (int)(((ulonglong)(sourceHeightMinusOne >> 0x18) << 0x20 |
                                (ulonglong)sourceHeightMinusOne * 0x100 & 0xffffffff) / (ulonglong)destinationHeightMinusOne);
        destinationCursor = destinationRowStart + framebufferWidth * 2;
        sourceRowCursor = sourcePixels + (sourceYFixed >> 8) * recordOffsetOrSourceStride;
        sourceXFixed = 0;
        destinationHeight = destinationHeight - 1;
        widthMinusOneOrRemainingPairs = destinationWidth >> 1;
        destinationRowStart = destinationCursor;
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
  int startX;
  undefined2 framebufferPixel;
  uint paletteIndexOrSourceArgb;
  int sourceWidth;
  int framebufferPitch;
  uint paletteArgb;
  dword packedBlue;
  undefined4 packedGreen;
  undefined4 packedRed;
  int recordOffsetOrRemainingColumns;
  int destinationX;
  byte *sourceCursor;
  byte *sourceRow;
  byte *destinationCursor;
  byte *destinationRow;
  ulonglong destinationLanes;
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
  int remainingSourceRows;
  GraphicsIntegerScale remainingRowRepeats;
  GraphicsIntegerScale remainingColumnRepeats;
  
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      recordOffsetOrRemainingColumns = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      startX = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRemainingColumns + -0x18) * integerScale;
      drawY = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRemainingColumns + -0x14) * integerScale;
      if (clipMinX < 0) {
        clipMinX = 0;
      }
      if (clipMinY < 0) {
        clipMinY = 0;
      }
      if ((int)g_DisplayFramebufferAccess.width < clipMaxX) {
        clipMaxX = g_DisplayFramebufferAccess.width;
      }
      paletteIndexOrSourceArgb = *(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                       recordOffsetOrRemainingColumns + -0x20);
      if ((int)g_DisplayFramebufferAccess.height < clipMaxY) {
        clipMaxY = g_DisplayFramebufferAccess.height;
      }
      if ((int)paletteIndexOrSourceArgb < 0) {
        sourceWidth = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        recordOffsetOrRemainingColumns + -0x10);
        remainingSourceRows = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            recordOffsetOrRemainingColumns + -0xc);
        destinationRow = g_DisplayFramebufferAccess.pixels +
                  (g_DisplayFramebufferAccess.width * drawY + startX) * 2;
        framebufferPitch = g_DisplayFramebufferAccess.width * 2;
        sourceRow = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                  *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRemainingColumns + -0x1c) + -0x28;
        remainingRowRepeats = integerScale;
        do {
          do {
            if ((clipMinY <= drawY) &&
               (recordOffsetOrRemainingColumns = sourceWidth, destinationX = startX, sourceCursor = sourceRow, destinationCursor = destinationRow,
               drawY < clipMaxY)) {
              do {
                remainingColumnRepeats = integerScale;
                paletteIndexOrSourceArgb = *(uint *)sourceCursor;
                if (paletteIndexOrSourceArgb < 0x1000000) {
                  destinationX = destinationX + integerScale;
                  destinationCursor = destinationCursor + integerScale * 2;
                }
                else if (paletteIndexOrSourceArgb < 0xff000000) {
                  do {
                    if ((clipMinX <= destinationX) && (destinationX < clipMaxX)) {
                      framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                      mm1PackedValue1ByteLane3 = (byte)(paletteIndexOrSourceArgb >> 0x18);
                      mm1PackedValue1ByteLane2 = (byte)(paletteIndexOrSourceArgb >> 0x10);
                      mm1PackedValue1ByteLane1 = (byte)(paletteIndexOrSourceArgb >> 8);
                      destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                      destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                               THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                      mm1PackedValue1 =
                           pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,
                                                    mm1PackedValue1ByteLane3) >> 2,
                                           CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                                  ,mm1PackedValue1ByteLane2),
                                                  CONCAT14(mm1PackedValue1ByteLane2,paletteIndexOrSourceArgb)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                                         mm1PackedValue1ByteLane1)
                                                                >> 2,CONCAT11((char)paletteIndexOrSourceArgb,
                                                                              (char)paletteIndexOrSourceArgb) >> 2))),
                                  g_SoftwareBlendAlphaFactors[paletteIndexOrSourceArgb >> 0x18]);
                      mm0PackedValue2 =
                           pmulhw(CONCAT26((ushort)((short)(destinationLanes >> 0x30) *
                                                   g_SoftwarePixelMmxConstants.unpackScales.zero) >>
                                           2,CONCAT24((ushort)((short)(destinationLanes >> 0x20) *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.red) >> 2,
                                                      CONCAT22((ushort)((short)(destinationLanes >> 0x10) *
                                                                       g_SoftwarePixelMmxConstants.
                                                                       unpackScales.green) >> 2,
                                                               (ushort)((short)destinationLanes *
                                                                       g_SoftwarePixelMmxConstants.
                                                                       unpackScales.blue) >> 2))),
                                  g_SoftwareBlendInverseAlphaFactors[paletteIndexOrSourceArgb >> 0x18]);
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
                                   THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                                   g_SoftwarePixelMmxConstants.packWeights);
                      *(short *)destinationCursor =
                           (short)((ulonglong)mm0PackedValue3 >> 8) +
                           (short)((ulonglong)mm0PackedValue3 >> 0x28);
                    }
                    destinationX = destinationX + 1;
                    destinationCursor = destinationCursor + 2;
                    remainingColumnRepeats = remainingColumnRepeats - 1;
                  } while (remainingColumnRepeats != 0);
                }
                else {
                  packedBlue = g_SoftwarePixelPackTables->blue[paletteIndexOrSourceArgb & 0xff];
                  packedGreen = *(undefined4 *)
                           ((int)g_SoftwarePixelPackTables->green + ((paletteIndexOrSourceArgb & 0xff00) >> 6));
                  packedRed = *(undefined4 *)
                           ((int)g_SoftwarePixelPackTables->red + ((paletteIndexOrSourceArgb & 0xff0000) >> 0xe));
                  do {
                    if ((clipMinX <= destinationX) && (destinationX < clipMaxX)) {
                      *(short *)destinationCursor = (short)packedBlue + (short)packedGreen + (short)packedRed;
                    }
                    destinationX = destinationX + 1;
                    destinationCursor = destinationCursor + 2;
                    remainingColumnRepeats = remainingColumnRepeats - 1;
                  } while (remainingColumnRepeats != 0);
                }
                recordOffsetOrRemainingColumns = recordOffsetOrRemainingColumns + -1;
                sourceCursor = sourceCursor + 4;
              } while (recordOffsetOrRemainingColumns != 0);
            }
            drawY = drawY + 1;
            destinationRow = destinationRow + framebufferPitch;
            remainingRowRepeats = remainingRowRepeats - 1;
          } while (remainingRowRepeats != 0);
          sourceRow = sourceRow + sourceWidth * 4;
          remainingRowRepeats = integerScale;
          remainingSourceRows = remainingSourceRows + -1;
        } while (remainingSourceRows != 0);
        return;
      }
      if (paletteIndexOrSourceArgb < (sourceAsset->tableDescriptor).paletteBankCount) {
        sourceWidth = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        recordOffsetOrRemainingColumns + -0x10);
        remainingSourceRows = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            recordOffsetOrRemainingColumns + -0xc);
        destinationRow = g_DisplayFramebufferAccess.pixels +
                  (g_DisplayFramebufferAccess.width * drawY + startX) * 2;
        framebufferPitch = g_DisplayFramebufferAccess.width * 2;
        sourceRow = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                  *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRemainingColumns + -0x1c) + -0x28;
        remainingRowRepeats = integerScale;
        do {
          do {
            if ((clipMinY <= drawY) &&
               (recordOffsetOrRemainingColumns = sourceWidth, destinationX = startX, sourceCursor = sourceRow, destinationCursor = destinationRow,
               drawY < clipMaxY)) {
              do {
                remainingColumnRepeats = integerScale;
                paletteArgb = *(uint *)(sourceAsset[paletteIndexOrSourceArgb * 4 + 1].common.buildMetadata.
                                  assetRelativeAddressAnchor28 + (uint)*sourceCursor * 8 + -0x24);
                if (paletteArgb < 0x1000000) {
                  destinationX = destinationX + integerScale;
                  destinationCursor = destinationCursor + integerScale * 2;
                }
                else if (paletteArgb < 0xff000000) {
                  paletteArgb = *(uint *)(sourceAsset[paletteIndexOrSourceArgb * 4 + 1].common.buildMetadata.
                                    assetRelativeAddressAnchor28 + (uint)*sourceCursor * 8 + -0x28);
                  do {
                    if ((clipMinX <= destinationX) && (destinationX < clipMaxX)) {
                      framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                      mm1PackedValue0ByteLane3 = (byte)(paletteArgb >> 0x18);
                      mm1PackedValue0ByteLane2 = (byte)(paletteArgb >> 0x10);
                      mm1PackedValue0ByteLane1 = (byte)(paletteArgb >> 8);
                      destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                      destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                               THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                      mm1PackedValue0 =
                           pmulhw(CONCAT26(CONCAT11(mm1PackedValue0ByteLane3,
                                                    mm1PackedValue0ByteLane3) >> 2,
                                           CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                                  ,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,paletteArgb)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                         mm1PackedValue0ByteLane1)
                                                                >> 2,CONCAT11((char)paletteArgb,
                                                                              (char)paletteArgb) >> 2))),
                                  g_SoftwareBlendAlphaFactors[paletteArgb >> 0x18]);
                      mm0PackedValue0 =
                           pmulhw(CONCAT26((ushort)((short)(destinationLanes >> 0x30) *
                                                   g_SoftwarePixelMmxConstants.unpackScales.zero) >>
                                           2,CONCAT24((ushort)((short)(destinationLanes >> 0x20) *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.red) >> 2,
                                                      CONCAT22((ushort)((short)(destinationLanes >> 0x10) *
                                                                       g_SoftwarePixelMmxConstants.
                                                                       unpackScales.green) >> 2,
                                                               (ushort)((short)destinationLanes *
                                                                       g_SoftwarePixelMmxConstants.
                                                                       unpackScales.blue) >> 2))),
                                  g_SoftwareBlendInverseAlphaFactors[paletteArgb >> 0x18]);
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
                                   THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                                   g_SoftwarePixelMmxConstants.packWeights);
                      *(short *)destinationCursor =
                           (short)((ulonglong)mm0PackedValue1 >> 8) +
                           (short)((ulonglong)mm0PackedValue1 >> 0x28);
                    }
                    destinationX = destinationX + 1;
                    destinationCursor = destinationCursor + 2;
                    remainingColumnRepeats = remainingColumnRepeats - 1;
                  } while (remainingColumnRepeats != 0);
                }
                else {
                  do {
                    if ((clipMinX <= destinationX) && (destinationX < clipMaxX)) {
                      *(short *)destinationCursor = (short)paletteArgb;
                    }
                    destinationX = destinationX + 1;
                    destinationCursor = destinationCursor + 2;
                    remainingColumnRepeats = remainingColumnRepeats - 1;
                  } while (remainingColumnRepeats != 0);
                }
                recordOffsetOrRemainingColumns = recordOffsetOrRemainingColumns + -1;
                sourceCursor = sourceCursor + 1;
              } while (recordOffsetOrRemainingColumns != 0);
            }
            drawY = drawY + 1;
            destinationRow = destinationRow + framebufferPitch;
            remainingRowRepeats = remainingRowRepeats - 1;
          } while (remainingRowRepeats != 0);
          sourceRow = sourceRow + sourceWidth;
          remainingRowRepeats = integerScale;
          remainingSourceRows = remainingSourceRows + -1;
        } while (remainingSourceRows != 0);
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
  int recordOffsetOrRowSkip;
  undefined2 framebufferPixel;
  int sourceWidth;
  uint sourceArgb;
  int leftOrRemainingColumns;
  GraphicsPixelDimension clippedBottom;
  int clippedTop;
  GraphicsPixelDimension clippedRight;
  int clippedWidth;
  byte *sourceIndexCursor;
  uint *sourceArgbCursor;
  byte *destinationCursor;
  ulonglong destinationLanes;
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
      recordOffsetOrRowSkip = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      leftOrRemainingColumns = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRowSkip + -0x18);
      clippedTop = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRowSkip + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x20)
          == -1) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          sourceWidth = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceArgbCursor = (uint *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * sourceWidth * 4 +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          do {
            do {
              sourceArgb = *sourceArgbCursor;
              if (0xffffff < sourceArgb) {
                if (sourceArgb < 0xff000000) {
                  framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                  mm1PackedValue1ByteLane3 = (byte)(sourceArgb >> 0x18);
                  mm1PackedValue1ByteLane2 = (byte)(sourceArgb >> 0x10);
                  mm1PackedValue1ByteLane1 = (byte)(sourceArgb >> 8);
                  destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                  destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                           THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                  mm1PackedValue1 =
                       pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                       >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                                  ,mm1PackedValue1ByteLane2),
                                                  CONCAT14(mm1PackedValue1ByteLane2,sourceArgb)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                                         mm1PackedValue1ByteLane1)
                                                                >> 2,CONCAT11((char)sourceArgb,
                                                                              (char)sourceArgb) >> 2))),
                              g_SoftwareBlendAlphaFactors[sourceArgb >> 0x18]);
                  mm0PackedValue2 =
                       pmulhw(CONCAT26((ushort)((short)(destinationLanes >> 0x30) *
                                               g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                       CONCAT24((ushort)((short)(destinationLanes >> 0x20) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.red
                                                        ) >> 2,
                                                CONCAT22((ushort)((short)(destinationLanes >> 0x10) *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.green) >> 2,
                                                         (ushort)((short)destinationLanes *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.blue) >> 2))),
                              g_SoftwareBlendInverseAlphaFactors[sourceArgb >> 0x18]);
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
                               THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                               g_SoftwarePixelMmxConstants.packWeights);
                  *(short *)destinationCursor =
                       (short)((ulonglong)mm0PackedValue3 >> 8) +
                       (short)((ulonglong)mm0PackedValue3 >> 0x28);
                }
                else {
                  *(short *)destinationCursor =
                       (short)g_SoftwarePixelPackTables->blue[sourceArgb & 0xff] +
                       (short)*(undefined4 *)
                               ((int)g_SoftwarePixelPackTables->green + ((sourceArgb & 0xff00) >> 6)) +
                       (short)*(undefined4 *)
                               ((int)g_SoftwarePixelPackTables->red + ((sourceArgb & 0xff0000) >> 0xe));
                }
              }
              sourceArgbCursor = sourceArgbCursor + 1;
              destinationCursor = destinationCursor + 2;
              leftOrRemainingColumns = leftOrRemainingColumns + -1;
            } while (leftOrRemainingColumns != 0);
            sourceArgbCursor = sourceArgbCursor + (sourceWidth - clippedWidth);
            destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
            clipMinY = clipMinY + -1;
            leftOrRemainingColumns = clippedWidth;
          } while (clipMinY != 0);
          return;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        recordOffsetOrRowSkip + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          sourceWidth = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceIndexCursor = (byte *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * sourceWidth +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          if (paletteBankIndex < (sourceAsset->tableDescriptor).paletteBankCount) {
            do {
              do {
                sourceArgb = *(uint *)(sourceAsset[paletteBankIndex * 4 + 1].common.buildMetadata.
                                  assetRelativeAddressAnchor28 + (uint)*sourceIndexCursor * 8 + -0x24);
                if (0xffffff < sourceArgb) {
                  if (sourceArgb < 0xff000000) {
                    sourceArgb = *(uint *)(sourceAsset[paletteBankIndex * 4 + 1].common.buildMetadata.
                                      assetRelativeAddressAnchor28 + (uint)*sourceIndexCursor * 8 + -0x28);
                    framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                    mm1PackedValue0ByteLane3 = (byte)(sourceArgb >> 0x18);
                    mm1PackedValue0ByteLane2 = (byte)(sourceArgb >> 0x10);
                    mm1PackedValue0ByteLane1 = (byte)(sourceArgb >> 8);
                    destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                    destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                             THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                    mm1PackedValue0 =
                         pmulhw(CONCAT26(CONCAT11(mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                         >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                                  ,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,sourceArgb)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                         mm1PackedValue0ByteLane1)
                                                                >> 2,CONCAT11((char)sourceArgb,
                                                                              (char)sourceArgb) >> 2))),
                                g_SoftwareBlendAlphaFactors[sourceArgb >> 0x18]);
                    mm0PackedValue0 =
                         pmulhw(CONCAT26((ushort)((short)(destinationLanes >> 0x30) *
                                                 g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2
                                         ,CONCAT24((ushort)((short)(destinationLanes >> 0x20) *
                                                           g_SoftwarePixelMmxConstants.unpackScales.
                                                           red) >> 2,
                                                   CONCAT22((ushort)((short)(destinationLanes >> 0x10) *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.green) >> 2,
                                                            (ushort)((short)destinationLanes *
                                                                    g_SoftwarePixelMmxConstants.
                                                                    unpackScales.blue) >> 2))),
                                g_SoftwareBlendInverseAlphaFactors[sourceArgb >> 0x18]);
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
                                 THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                                 g_SoftwarePixelMmxConstants.packWeights);
                    *(short *)destinationCursor =
                         (short)((ulonglong)mm0PackedValue1 >> 8) +
                         (short)((ulonglong)mm0PackedValue1 >> 0x28);
                  }
                  else {
                    *(short *)destinationCursor = (short)sourceArgb;
                  }
                }
                sourceIndexCursor = sourceIndexCursor + 1;
                destinationCursor = destinationCursor + 2;
                leftOrRemainingColumns = leftOrRemainingColumns + -1;
              } while (leftOrRemainingColumns != 0);
              sourceIndexCursor = sourceIndexCursor + (sourceWidth - clippedWidth);
              destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
              clipMinY = clipMinY + -1;
              leftOrRemainingColumns = clippedWidth;
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
  int recordOffsetOrRowSkip;
  undefined2 framebufferPixel;
  int widthOrPaletteIndex;
  int indexedSourceWidth;
  uint sourceArgb;
  int leftOrRemainingColumns;
  GraphicsPixelDimension clippedBottom;
  int clippedTop;
  GraphicsPixelDimension clippedRight;
  int clippedWidth;
  byte *sourceIndexCursor;
  uint *sourceArgbCursor;
  byte *destinationCursor;
  ulonglong destinationLanes;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  undefined8 mm0PackedValue2;
  undefined8 mm0PackedValue3;
  undefined1 sourceGreen;
  undefined1 sourceRed;
  undefined1 sourceAlpha;
  
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      recordOffsetOrRowSkip = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      leftOrRemainingColumns = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRowSkip + -0x18);
      clippedTop = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRowSkip + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x20)
          == -1) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          widthOrPaletteIndex = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceArgbCursor = (uint *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * widthOrPaletteIndex * 4 +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          do {
            do {
              sourceArgb = *sourceArgbCursor;
              if ((sourceArgb & 0xffffff) != 0) {
                framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                sourceAlpha = (undefined1)(sourceArgb >> 0x18);
                sourceRed = (undefined1)(sourceArgb >> 0x10);
                sourceGreen = (undefined1)(sourceArgb >> 8);
                destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                         THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                mm0PackedValue2 =
                     paddusw(CONCAT26((short)(destinationLanes >> 0x30) *
                                      g_SoftwarePixelMmxConstants.unpackScales.zero,
                                      CONCAT24((short)(destinationLanes >> 0x20) *
                                               g_SoftwarePixelMmxConstants.unpackScales.red,
                                               CONCAT22((short)(destinationLanes >> 0x10) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.
                                                        green,(short)destinationLanes *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.blue))),
                             CONCAT62(CONCAT51(CONCAT41((int)(CONCAT35(CONCAT21(CONCAT11(sourceAlpha,
                                                  sourceAlpha),sourceRed),CONCAT14(sourceRed,sourceArgb)) >> 0x20),
                                                  sourceGreen),sourceGreen),CONCAT11((char)sourceArgb,(char)sourceArgb))
                            );
                mm0PackedValue3 =
                     pmaddwd(CONCAT26((ushort)((ulonglong)mm0PackedValue2 >> 0x34),
                                      CONCAT24((ushort)((ulonglong)mm0PackedValue2 >> 0x20) >> 4,
                                               CONCAT22((ushort)((ulonglong)mm0PackedValue2 >> 0x10)
                                                        >> 4,(ushort)mm0PackedValue2 >> 4))) &
                             THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)destinationCursor =
                     (short)((ulonglong)mm0PackedValue3 >> 8) +
                     (short)((ulonglong)mm0PackedValue3 >> 0x28);
              }
              sourceArgbCursor = sourceArgbCursor + 1;
              destinationCursor = destinationCursor + 2;
              leftOrRemainingColumns = leftOrRemainingColumns + -1;
            } while (leftOrRemainingColumns != 0);
            sourceArgbCursor = sourceArgbCursor + (widthOrPaletteIndex - clippedWidth);
            destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
            clipMinY = clipMinY + -1;
            leftOrRemainingColumns = clippedWidth;
          } while (clipMinY != 0);
          return false;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        recordOffsetOrRowSkip + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          widthOrPaletteIndex = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x20);
          indexedSourceWidth = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceIndexCursor = (byte *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * indexedSourceWidth +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          do {
            do {
              sourceArgb = *(uint *)(sourceAsset[widthOrPaletteIndex * 4 + 1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)*sourceIndexCursor * 8 + -0x28);
              if ((sourceArgb & 0xffffff) != 0) {
                framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                sourceAlpha = (undefined1)(sourceArgb >> 0x18);
                sourceRed = (undefined1)(sourceArgb >> 0x10);
                sourceGreen = (undefined1)(sourceArgb >> 8);
                destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                         THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                mm0PackedValue0 =
                     paddusw(CONCAT26((short)(destinationLanes >> 0x30) *
                                      g_SoftwarePixelMmxConstants.unpackScales.zero,
                                      CONCAT24((short)(destinationLanes >> 0x20) *
                                               g_SoftwarePixelMmxConstants.unpackScales.red,
                                               CONCAT22((short)(destinationLanes >> 0x10) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.
                                                        green,(short)destinationLanes *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.blue))),
                             CONCAT62(CONCAT51(CONCAT41((int)(CONCAT35(CONCAT21(CONCAT11(sourceAlpha,
                                                  sourceAlpha),sourceRed),CONCAT14(sourceRed,sourceArgb)) >> 0x20),
                                                  sourceGreen),sourceGreen),CONCAT11((char)sourceArgb,(char)sourceArgb))
                            );
                mm0PackedValue1 =
                     pmaddwd(CONCAT26((ushort)((ulonglong)mm0PackedValue0 >> 0x34),
                                      CONCAT24((ushort)((ulonglong)mm0PackedValue0 >> 0x20) >> 4,
                                               CONCAT22((ushort)((ulonglong)mm0PackedValue0 >> 0x10)
                                                        >> 4,(ushort)mm0PackedValue0 >> 4))) &
                             THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)destinationCursor =
                     (short)((ulonglong)mm0PackedValue1 >> 8) +
                     (short)((ulonglong)mm0PackedValue1 >> 0x28);
              }
              sourceIndexCursor = sourceIndexCursor + 1;
              destinationCursor = destinationCursor + 2;
              leftOrRemainingColumns = leftOrRemainingColumns + -1;
            } while (leftOrRemainingColumns != 0);
            sourceIndexCursor = sourceIndexCursor + (indexedSourceWidth - clippedWidth);
            destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
            clipMinY = clipMinY + -1;
            leftOrRemainingColumns = clippedWidth;
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
  int recordOffsetOrRowSkip;
  undefined2 framebufferPixel;
  int widthOrPaletteIndex;
  int indexedSourceWidth;
  uint sourceArgb;
  ushort sourceAlphaWord;
  int leftOrRemainingColumns;
  GraphicsPixelDimension clippedBottom;
  int clippedTop;
  GraphicsPixelDimension clippedRight;
  int clippedWidth;
  byte *sourceIndexCursor;
  uint *sourceArgbCursor;
  byte *destinationCursor;
  ulonglong destinationLanes;
  undefined8 mm0PackedValue0;
  undefined8 mm0PackedValue1;
  undefined8 mm0PackedValue2;
  undefined8 mm0PackedValue3;
  undefined1 sourceGreenOrAlpha;
  undefined1 sourceRed;
  
  if (framebuffer == &g_DisplayFramebufferAccess) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      recordOffsetOrRowSkip = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      leftOrRemainingColumns = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRowSkip + -0x18);
      clippedTop = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRowSkip + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x20)
          == -1) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          widthOrPaletteIndex = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceArgbCursor = (uint *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * widthOrPaletteIndex * 4 +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          do {
            do {
              sourceArgb = *sourceArgbCursor;
              if ((sourceArgb & 0xffffff) != 0) {
                framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                sourceGreenOrAlpha = (undefined1)(sourceArgb >> 0x18);
                sourceAlphaWord = CONCAT11(sourceGreenOrAlpha,sourceGreenOrAlpha);
                sourceRed = (undefined1)(sourceArgb >> 0x10);
                sourceGreenOrAlpha = (undefined1)(sourceArgb >> 8);
                destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                         THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                mm0PackedValue2 =
                     paddusw(CONCAT26((short)(destinationLanes >> 0x30) *
                                      g_SoftwarePixelMmxConstants.unpackScales.zero,
                                      CONCAT24((short)(destinationLanes >> 0x20) *
                                               g_SoftwarePixelMmxConstants.unpackScales.red,
                                               CONCAT22((short)(destinationLanes >> 0x10) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.
                                                        green,(short)destinationLanes *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.blue))),
                             CONCAT26(sourceAlphaWord >> 1,
                                      CONCAT24((ushort)(CONCAT35(CONCAT21(sourceAlphaWord,sourceRed),
                                                                 CONCAT14(sourceRed,sourceArgb)) >> 0x20) >>
                                               1,CONCAT22(CONCAT11(sourceGreenOrAlpha,sourceGreenOrAlpha) >> 1,
                                                          CONCAT11((char)sourceArgb,(char)sourceArgb) >> 1))))
                ;
                mm0PackedValue3 =
                     pmaddwd(CONCAT26((ushort)((ulonglong)mm0PackedValue2 >> 0x34),
                                      CONCAT24((ushort)((ulonglong)mm0PackedValue2 >> 0x20) >> 4,
                                               CONCAT22((ushort)((ulonglong)mm0PackedValue2 >> 0x10)
                                                        >> 4,(ushort)mm0PackedValue2 >> 4))) &
                             THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)destinationCursor =
                     (short)((ulonglong)mm0PackedValue3 >> 8) +
                     (short)((ulonglong)mm0PackedValue3 >> 0x28);
              }
              sourceArgbCursor = sourceArgbCursor + 1;
              destinationCursor = destinationCursor + 2;
              leftOrRemainingColumns = leftOrRemainingColumns + -1;
            } while (leftOrRemainingColumns != 0);
            sourceArgbCursor = sourceArgbCursor + (widthOrPaletteIndex - clippedWidth);
            destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
            clipMinY = clipMinY + -1;
            leftOrRemainingColumns = clippedWidth;
          } while (clipMinY != 0);
          return false;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        recordOffsetOrRowSkip + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          widthOrPaletteIndex = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x20);
          indexedSourceWidth = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceIndexCursor = (byte *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                               assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * indexedSourceWidth +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          do {
            do {
              sourceArgb = *(uint *)(sourceAsset[widthOrPaletteIndex * 4 + 1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)*sourceIndexCursor * 8 + -0x28);
              if ((sourceArgb & 0xffffff) != 0) {
                framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                sourceGreenOrAlpha = (undefined1)(sourceArgb >> 0x18);
                sourceAlphaWord = CONCAT11(sourceGreenOrAlpha,sourceGreenOrAlpha);
                sourceRed = (undefined1)(sourceArgb >> 0x10);
                sourceGreenOrAlpha = (undefined1)(sourceArgb >> 8);
                destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                         THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                mm0PackedValue0 =
                     paddusw(CONCAT26((short)(destinationLanes >> 0x30) *
                                      g_SoftwarePixelMmxConstants.unpackScales.zero,
                                      CONCAT24((short)(destinationLanes >> 0x20) *
                                               g_SoftwarePixelMmxConstants.unpackScales.red,
                                               CONCAT22((short)(destinationLanes >> 0x10) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.
                                                        green,(short)destinationLanes *
                                                              g_SoftwarePixelMmxConstants.
                                                              unpackScales.blue))),
                             CONCAT26(sourceAlphaWord >> 1,
                                      CONCAT24((ushort)(CONCAT35(CONCAT21(sourceAlphaWord,sourceRed),
                                                                 CONCAT14(sourceRed,sourceArgb)) >> 0x20) >>
                                               1,CONCAT22(CONCAT11(sourceGreenOrAlpha,sourceGreenOrAlpha) >> 1,
                                                          CONCAT11((char)sourceArgb,(char)sourceArgb) >> 1))))
                ;
                mm0PackedValue1 =
                     pmaddwd(CONCAT26((ushort)((ulonglong)mm0PackedValue0 >> 0x34),
                                      CONCAT24((ushort)((ulonglong)mm0PackedValue0 >> 0x20) >> 4,
                                               CONCAT22((ushort)((ulonglong)mm0PackedValue0 >> 0x10)
                                                        >> 4,(ushort)mm0PackedValue0 >> 4))) &
                             THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                             g_SoftwarePixelMmxConstants.packWeights);
                *(short *)destinationCursor =
                     (short)((ulonglong)mm0PackedValue1 >> 8) +
                     (short)((ulonglong)mm0PackedValue1 >> 0x28);
              }
              sourceIndexCursor = sourceIndexCursor + 1;
              destinationCursor = destinationCursor + 2;
              leftOrRemainingColumns = leftOrRemainingColumns + -1;
            } while (leftOrRemainingColumns != 0);
            sourceIndexCursor = sourceIndexCursor + (indexedSourceWidth - clippedWidth);
            destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
            clipMinY = clipMinY + -1;
            leftOrRemainingColumns = clippedWidth;
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
  int recordOffsetOrRowSkip;
  undefined2 framebufferPixel;
  int widthOrPaletteIndex;
  int indexedSourceWidth;
  int leftOrRemainingColumns;
  uint blueProduct;
  uint sourceArgbOrBlue;
  uint modulatedArgb;
  uint modulationRed;
  GraphicsPixelDimension clippedBottom;
  uint redProduct;
  int clippedTop;
  uint alphaProductOrAlpha;
  uint alphaProductHigh;
  uint modulationGreen;
  GraphicsPixelDimension clippedRight;
  int clippedWidth;
  uint greenProduct;
  byte *sourceIndexCursor;
  uint *sourceArgbCursor;
  byte *destinationCursor;
  ulonglong destinationLanes;
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
    modulationGreen = (modulationArgb8888 & 0xff00) >> 8;
    modulationRed = (modulationArgb8888 & 0xff0000) >> 0x10;
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      recordOffsetOrRowSkip = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
      leftOrRemainingColumns = drawX + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                              recordOffsetOrRowSkip + -0x18);
      clippedTop = drawY + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                               recordOffsetOrRowSkip + -0x14);
      if (*(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x20)
          == -1) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28
                                  + recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          widthOrPaletteIndex = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceArgbCursor = (uint *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                                assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * widthOrPaletteIndex * 4 +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) * 4 +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          do {
            do {
              sourceArgbOrBlue = *sourceArgbCursor;
              blueProduct = (sourceArgbOrBlue & 0xff) * (modulationArgb8888 & 0xff);
              alphaProductOrAlpha = (sourceArgbOrBlue >> 0x18) * (modulationArgb8888 >> 0x18);
              greenProduct = ((sourceArgbOrBlue & 0xff00) >> 8) * modulationGreen & 0xff00;
              redProduct = ((sourceArgbOrBlue & 0xff0000) >> 0x10) * modulationRed & 0xff00;
              alphaProductHigh = alphaProductOrAlpha & 0xff00;
              sourceArgbOrBlue = blueProduct >> 8;
              modulatedArgb = sourceArgbOrBlue | greenProduct | redProduct << 8 | alphaProductHigh << 0x10;
              if (0xffffff < modulatedArgb) {
                if (modulatedArgb < 0xff000000) {
                  framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                  mm1PackedValue1ByteLane3 = (byte)(alphaProductHigh >> 8);
                  mm1PackedValue1ByteLane2 = (byte)(redProduct >> 8);
                  mm1PackedValue1ByteLane1 = (byte)(greenProduct >> 8);
                  mm1PackedValue1ByteLane0 = (byte)(blueProduct >> 8);
                  destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                  destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                           THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                  alphaProductOrAlpha = alphaProductOrAlpha >> 8;
                  mm1PackedValue1 =
                       pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                       >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3)
                                                  ,mm1PackedValue1ByteLane2),
                                                  CONCAT14(mm1PackedValue1ByteLane2,modulatedArgb)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                                         mm1PackedValue1ByteLane1)
                                                                >> 2,CONCAT11(
                                                  mm1PackedValue1ByteLane0,mm1PackedValue1ByteLane0)
                                                  >> 2))),g_SoftwareBlendAlphaFactors[alphaProductOrAlpha]);
                  mm0PackedValue2 =
                       pmulhw(CONCAT26((ushort)((short)(destinationLanes >> 0x30) *
                                               g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                       CONCAT24((ushort)((short)(destinationLanes >> 0x20) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.red
                                                        ) >> 2,
                                                CONCAT22((ushort)((short)(destinationLanes >> 0x10) *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.green) >> 2,
                                                         (ushort)((short)destinationLanes *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.blue) >> 2))),
                              g_SoftwareBlendInverseAlphaFactors[alphaProductOrAlpha]);
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
                               THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                               g_SoftwarePixelMmxConstants.packWeights);
                  *(short *)destinationCursor =
                       (short)((ulonglong)mm0PackedValue3 >> 8) +
                       (short)((ulonglong)mm0PackedValue3 >> 0x28);
                }
                else {
                  *(short *)destinationCursor =
                       (short)g_SoftwarePixelPackTables->blue[sourceArgbOrBlue] +
                       (short)*(undefined4 *)((int)g_SoftwarePixelPackTables->green + (greenProduct >> 6))
                       + (short)*(undefined4 *)((int)g_SoftwarePixelPackTables->red + (redProduct >> 6))
                  ;
                }
              }
              sourceArgbCursor = sourceArgbCursor + 1;
              destinationCursor = destinationCursor + 2;
              leftOrRemainingColumns = leftOrRemainingColumns + -1;
            } while (leftOrRemainingColumns != 0);
            sourceArgbCursor = sourceArgbCursor + (widthOrPaletteIndex - clippedWidth);
            destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
            clipMinY = clipMinY + -1;
            leftOrRemainingColumns = clippedWidth;
          } while (clipMinY != 0);
          return false;
        }
      }
      else if (*(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        recordOffsetOrRowSkip + -0x20) < (sourceAsset->tableDescriptor).paletteBankCount) {
        clippedRight = leftOrRemainingColumns + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 recordOffsetOrRowSkip + -0x10);
        clippedBottom = clippedTop + *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28
                                  + recordOffsetOrRowSkip + -0xc);
        if (leftOrRemainingColumns < 0) {
          leftOrRemainingColumns = 0;
        }
        if (clippedTop < 0) {
          clippedTop = 0;
        }
        if ((int)g_DisplayFramebufferAccess.width < (int)clippedRight) {
          clippedRight = g_DisplayFramebufferAccess.width;
        }
        if ((int)g_DisplayFramebufferAccess.height < (int)clippedBottom) {
          clippedBottom = g_DisplayFramebufferAccess.height;
        }
        if (leftOrRemainingColumns < clipMinX) {
          leftOrRemainingColumns = clipMinX;
        }
        if (clippedTop < clipMinY) {
          clippedTop = clipMinY;
        }
        if (clipMaxX < (int)clippedRight) {
          clippedRight = clipMaxX;
        }
        if (clipMaxY < (int)clippedBottom) {
          clippedBottom = clipMaxY;
        }
        clippedWidth = clippedRight - leftOrRemainingColumns;
        if ((clippedWidth != 0 && leftOrRemainingColumns <= (int)clippedRight) &&
           (clipMinY = clippedBottom - clippedTop, clipMinY != 0 && clippedTop <= (int)clippedBottom)) {
          widthOrPaletteIndex = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x20);
          indexedSourceWidth = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          recordOffsetOrRowSkip + -0x10);
          destinationCursor = g_DisplayFramebufferAccess.pixels +
                    (g_DisplayFramebufferAccess.width * clippedTop + leftOrRemainingColumns) * 2;
          sourceIndexCursor = (byte *)((int)sourceAsset +
                            ((clippedTop - *(int *)((sourceAsset->common).buildMetadata.
                                                assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x14)) -
                            drawY) * indexedSourceWidth +
                            ((leftOrRemainingColumns - drawX) -
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x18)) +
                            *(int *)((sourceAsset->common).buildMetadata.
                                     assetRelativeAddressAnchor28 + recordOffsetOrRowSkip + -0x1c));
          recordOffsetOrRowSkip = g_DisplayFramebufferAccess.width - clippedWidth;
          leftOrRemainingColumns = clippedWidth;
          do {
            do {
              sourceArgbOrBlue = *(uint *)(sourceAsset[widthOrPaletteIndex * 4 + 1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)*sourceIndexCursor * 8 + -0x28);
              blueProduct = (sourceArgbOrBlue & 0xff) * (modulationArgb8888 & 0xff);
              alphaProductOrAlpha = (sourceArgbOrBlue >> 0x18) * (modulationArgb8888 >> 0x18);
              greenProduct = ((sourceArgbOrBlue & 0xff00) >> 8) * modulationGreen & 0xff00;
              redProduct = ((sourceArgbOrBlue & 0xff0000) >> 0x10) * modulationRed & 0xff00;
              alphaProductHigh = alphaProductOrAlpha & 0xff00;
              sourceArgbOrBlue = blueProduct >> 8;
              modulatedArgb = sourceArgbOrBlue | greenProduct | redProduct << 8 | alphaProductHigh << 0x10;
              if (0xffffff < modulatedArgb) {
                if (modulatedArgb < 0xff000000) {
                  framebufferPixel = *(undefined2 *)(destinationCursor + g_GlideSecondBufferOffset);
                  mm1PackedValue0ByteLane3 = (byte)(alphaProductHigh >> 8);
                  mm1PackedValue0ByteLane2 = (byte)(redProduct >> 8);
                  mm1PackedValue0ByteLane1 = (byte)(greenProduct >> 8);
                  mm1PackedValue0ByteLane0 = (byte)(blueProduct >> 8);
                  destinationCursor = destinationCursor + g_GlideSecondBufferOffset + -g_GlideSecondBufferOffset;
                  destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                           THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
                  alphaProductOrAlpha = alphaProductOrAlpha >> 8;
                  mm1PackedValue0 =
                       pmulhw(CONCAT26(CONCAT11(mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                       >> 2,CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(
                                                  mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3)
                                                  ,mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,modulatedArgb)) >> 0x20)
                                                  >> 2,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                         mm1PackedValue0ByteLane1)
                                                                >> 2,CONCAT11(
                                                  mm1PackedValue0ByteLane0,mm1PackedValue0ByteLane0)
                                                  >> 2))),g_SoftwareBlendAlphaFactors[alphaProductOrAlpha]);
                  mm0PackedValue0 =
                       pmulhw(CONCAT26((ushort)((short)(destinationLanes >> 0x30) *
                                               g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                       CONCAT24((ushort)((short)(destinationLanes >> 0x20) *
                                                        g_SoftwarePixelMmxConstants.unpackScales.red
                                                        ) >> 2,
                                                CONCAT22((ushort)((short)(destinationLanes >> 0x10) *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.green) >> 2,
                                                         (ushort)((short)destinationLanes *
                                                                 g_SoftwarePixelMmxConstants.
                                                                 unpackScales.blue) >> 2))),
                              g_SoftwareBlendInverseAlphaFactors[alphaProductOrAlpha]);
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
                               THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                               g_SoftwarePixelMmxConstants.packWeights);
                  *(short *)destinationCursor =
                       (short)((ulonglong)mm0PackedValue1 >> 8) +
                       (short)((ulonglong)mm0PackedValue1 >> 0x28);
                }
                else {
                  *(short *)destinationCursor =
                       (short)g_SoftwarePixelPackTables->blue[sourceArgbOrBlue] +
                       (short)*(undefined4 *)((int)g_SoftwarePixelPackTables->green + (greenProduct >> 6))
                       + (short)*(undefined4 *)((int)g_SoftwarePixelPackTables->red + (redProduct >> 6))
                  ;
                }
              }
              sourceIndexCursor = sourceIndexCursor + 1;
              destinationCursor = destinationCursor + 2;
              leftOrRemainingColumns = leftOrRemainingColumns + -1;
            } while (leftOrRemainingColumns != 0);
            sourceIndexCursor = sourceIndexCursor + (indexedSourceWidth - clippedWidth);
            destinationCursor = destinationCursor + recordOffsetOrRowSkip * 2;
            clipMinY = clipMinY + -1;
            leftOrRemainingColumns = clippedWidth;
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
  undefined2 framebufferPixel;
  int rowSkipBytes;
  int secondBufferBackOffset;
  ushort fillAlphaWord;
  uint fillColor;
  int remainingColumns;
  int clippedWidth;
  byte *destinationCursor;
  ulonglong destinationLanes;
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
    clippedWidth = rectMaxX - rectMinX;
    if ((clippedWidth != 0 && rectMinX <= rectMaxX) &&
       (clipMinY = rectMaxY - rectMinY, clipMinY != 0 && rectMinY <= rectMaxY)) {
      rowSkipBytes = (g_DisplayFramebufferAccess.width - clippedWidth) * 2;
      destinationCursor = g_DisplayFramebufferAccess.pixels +
               (rectMinY * g_DisplayFramebufferAccess.width + rectMinX) * 2;
      fillColor = g_SoftwarePixelPackTables->blue[argb8888 & 0xff] + (argb8888 & 0xff000000) +
              *(int *)((int)g_SoftwarePixelPackTables->green + ((argb8888 & 0xff00) >> 6)) +
              *(int *)((int)g_SoftwarePixelPackTables->red + ((argb8888 & 0xff0000) >> 0xe));
      if (0xffffff < fillColor) {
        remainingColumns = clippedWidth;
        if (fillColor < 0xff000000) {
          do {
            do {
              destinationCursor = destinationCursor + g_GlideSecondBufferOffset;
              framebufferPixel = *(undefined2 *)destinationCursor;
              mm1PackedValue0ByteLane1 = (undefined1)(fillColor >> 0x18);
              fillAlphaWord = CONCAT11(mm1PackedValue0ByteLane1,mm1PackedValue0ByteLane1);
              mm1PackedValue0ByteLane2 = (undefined1)(fillColor >> 0x10);
              mm1PackedValue0ByteLane1 = (undefined1)(fillColor >> 8);
              secondBufferBackOffset = -g_GlideSecondBufferOffset;
              destinationLanes = CONCAT44(CONCAT22(framebufferPixel,framebufferPixel),CONCAT22(framebufferPixel,framebufferPixel)) &
                      THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.packedPixelMasks);
              mm1PackedValue0 =
                   pmulhw(CONCAT26(fillAlphaWord >> 2,
                                   CONCAT24((ushort)(CONCAT35(CONCAT21(fillAlphaWord,
                                                  mm1PackedValue0ByteLane2),
                                                  CONCAT14(mm1PackedValue0ByteLane2,fillColor)) >> 0x20)
                                            >> 2,CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                                   mm1PackedValue0ByteLane1) >> 2,
                                                          CONCAT11((char)fillColor,(char)fillColor) >> 2))),
                          g_SoftwareBlendAlphaFactors[fillColor >> 0x18]);
              mm0PackedValue0 =
                   pmulhw(CONCAT26((ushort)((short)(destinationLanes >> 0x30) *
                                           g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2,
                                   CONCAT24((ushort)((short)(destinationLanes >> 0x20) *
                                                    g_SoftwarePixelMmxConstants.unpackScales.red) >>
                                            2,CONCAT22((ushort)((short)(destinationLanes >> 0x10) *
                                                               g_SoftwarePixelMmxConstants.
                                                               unpackScales.green) >> 2,
                                                       (ushort)((short)destinationLanes *
                                                               g_SoftwarePixelMmxConstants.
                                                               unpackScales.blue) >> 2))),
                          g_SoftwareBlendInverseAlphaFactors[fillColor >> 0x18]);
              mm0PackedValue1 =
                   pmaddwd(CONCAT26((short)((ulonglong)mm0PackedValue0 >> 0x30) +
                                    (short)((ulonglong)mm1PackedValue0 >> 0x30),
                                    CONCAT24((short)((ulonglong)mm0PackedValue0 >> 0x20) +
                                             (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                             CONCAT22((short)((ulonglong)mm0PackedValue0 >> 0x10) +
                                                      (short)((ulonglong)mm1PackedValue0 >> 0x10),
                                                      (short)mm0PackedValue0 +
                                                      (short)mm1PackedValue0))) &
                           THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                           g_SoftwarePixelMmxConstants.packWeights);
              *(short *)(destinationCursor + secondBufferBackOffset) =
                   (short)((ulonglong)mm0PackedValue1 >> 8) +
                   (short)((ulonglong)mm0PackedValue1 >> 0x28);
              destinationCursor = destinationCursor + secondBufferBackOffset + 2;
              remainingColumns = remainingColumns + -1;
            } while (remainingColumns != 0);
            destinationCursor = destinationCursor + rowSkipBytes;
            clipMinY = clipMinY + -1;
            remainingColumns = clippedWidth;
          } while (clipMinY != 0);
          return;
        }
        do {
          for (; remainingColumns != 0; remainingColumns = remainingColumns + -1) {
            *(short *)destinationCursor = (short)fillColor;
            destinationCursor = destinationCursor + 2;
          }
          destinationCursor = destinationCursor + rowSkipBytes;
          clipMinY = clipMinY + -1;
          remainingColumns = clippedWidth;
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
  UiPixelOffset cursorHotspotX;
  UiPixelCoordinate cursorX;
  GraphicsSubresourceIndex cursorSubresource;
  GraphicsCursorFrameRecord *frameRecord;
  UiPixelCoordinate cursorY;
  bool accessFailed;
  UiPixelOffset cursorHotspotY;
  
  g_CursorCurrentVisibilityToken = g_CursorVisibilityToken;
  if ((-1 < g_CursorVisibilityToken) && (backSurfaceSentinel == (IDirectDrawSurface3 *)0x1)) {
    cursorX = g_MouseX;
    cursorY = g_MouseY;
    if (g_CursorUseOverridePosition != 0) {
      cursorX = g_CursorOverrideX;
      cursorY = g_CursorOverrideY;
    }
    frameRecord = g_CursorFrameRecords + g_CursorFrameIndex;
    cursorHotspotX = frameRecord->hotspotX;
    cursorHotspotY = frameRecord->hotspotY;
    cursorSubresource = frameRecord->activeSubresourceIndex;
    if ((g_CursorButtonState & 7) == 0) {
      cursorSubresource = frameRecord->idleSubresourceIndex;
    }
    accessFailed = Glide3_Framebuffer_BeginAccess();
    if (!accessFailed) {
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (g_FramebufferHeight,g_FramebufferWidth,0,0,cursorY - cursorHotspotY,cursorX - cursorHotspotX,
                 cursorSubresource,g_CursorSourceAsset,g_FramebufferAccess);
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
      g_FramebufferRowStrideBytes = g_GlidePrimaryLfbInfo.strideBytes;
      g_DisplayFramebufferAccess.width = g_GlidePrimaryLfbInfo.strideBytes >> 1;
      g_DisplayFramebufferAccess.pixels = g_GlidePrimaryLfbInfo.pixels;
      secondaryLfbLockSucceeded = (*g_GrLfbLock)(0x10,1,0,0,0,&g_GlideSecondaryLfbInfo);
      if ((secondaryLfbLockSucceeded != 0) &&
         (g_FramebufferRowStrideBytes == g_GlideSecondaryLfbInfo.strideBytes)) {
        /* The original reads the primary buffer's pointer here, not g_GlideSecondaryLfbInfo.pixels, so
           the second-buffer offset is always 0 and blending reads back the buffer it draws to. */
        g_GlideSecondBufferBase = g_GlidePrimaryLfbInfo.pixels;
        g_GlideSecondBufferOffset =
             (int)g_GlidePrimaryLfbInfo.pixels - (int)g_DisplayFramebufferAccess.pixels;
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
  GraphicsTextureSourceAsset *asset;
  dword placementAddress;
  GraphicsTextureResource *nextResident;
  GraphicsTextureMemoryAddress reclaimedAddress;
  GraphicsTextureResource *followingResident;
  int subresourceRecordOffset;
  uint tailFreeBytes;
  GraphicsTextureResidentTmuIndex tmuIndex;
  GraphicsTextureResource *residentCursor;
  
  placementAddress = g_GlideTmuMinAddress[0];
  if (((int)texture->residentTmuIndex < 0) &&
     (residentCursor = g_GlideResidentTextureTail, (texture->glideInfo).data != (void *)0x0)) {
    do {
      if (residentCursor == (GraphicsTextureResource *)0x0) {
        texture->residentTmuIndex = GRAPHICS_TEXTURE_RESIDENT_TMU0;
        texture->residentNext = (GraphicsTextureResource *)0x0;
        texture->residentAddress = placementAddress;
        g_GlideResidentTextureTail = texture;
        g_GlideResidentTextureHead = texture;
        goto Glide3_TextureResource_EnsureResident_CommitResidentPlacementAndDownloadMipMap;
      }
      if (residentCursor->residentNext == (GraphicsTextureResource *)0x0) {
        asset = residentCursor->sourceAsset;
        subresourceRecordOffset = residentCursor->subresourceIndex * 0x20 + (asset->tableDescriptor).subresourceTableOffset;
        tmuIndex = residentCursor->residentTmuIndex;
        tailFreeBytes = g_GlideTmuMaxAddress[tmuIndex] -
                (((uint)(*(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 subresourceRecordOffset + -0x10) *
                         *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                 subresourceRecordOffset + -0xc) * 2) >>
                 ((char)residentCursor->downsampleShift * '\x02' & 0x1fU)) + residentCursor->residentAddress);
        asset = texture->sourceAsset;
        subresourceRecordOffset = texture->subresourceIndex * 0x20 + (asset->tableDescriptor).subresourceTableOffset;
        if ((uint)(*(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                           subresourceRecordOffset + -0x10) *
                   *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                           subresourceRecordOffset + -0xc) * 2) >> ((char)texture->downsampleShift * '\x02' & 0x1fU) <
            tailFreeBytes) {
          residentCursor->residentNext = texture;
          placementAddress = g_GlideTmuMaxAddress[tmuIndex];
          texture->residentTmuIndex = tmuIndex;
          texture->residentAddress = placementAddress - tailFreeBytes;
          texture->residentNext = (GraphicsTextureResource *)0x0;
          g_GlideResidentTextureTail = texture;
Glide3_TextureResource_EnsureResident_CommitResidentPlacementAndDownloadMipMap:
          g_GlideBoundTexture = (GraphicsTextureResource *)0x0;
          (*g_GrTexDownloadMipMap)
                    (texture->residentTmuIndex,texture->residentAddress,3,&texture->glideInfo);
          return;
        }
        tmuIndex = tmuIndex + 1;
        nextResident = g_GlideResidentTextureHead;
        if (tmuIndex < g_GlideTmuCount) {
          placementAddress = g_GlideTmuMinAddress[tmuIndex];
          residentCursor->residentNext = texture;
          texture->residentTmuIndex = tmuIndex;
          texture->residentAddress = placementAddress;
          texture->residentNext = (GraphicsTextureResource *)0x0;
          g_GlideResidentTextureTail = texture;
          goto Glide3_TextureResource_EnsureResident_CommitResidentPlacementAndDownloadMipMap;
        }
      }
      else {
        asset = texture->sourceAsset;
        subresourceRecordOffset = texture->subresourceIndex * 0x20 + (asset->tableDescriptor).subresourceTableOffset;
        nextResident = residentCursor->residentNext;
        tmuIndex = residentCursor->residentTmuIndex;
        if ((tmuIndex == nextResident->residentTmuIndex) &&
           ((uint)(*(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                           subresourceRecordOffset + -0x10) *
                   *(int *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                           subresourceRecordOffset + -0xc) * 2) >> ((char)texture->downsampleShift * '\x02' & 0x1fU)
            <= nextResident->residentAddress - residentCursor->residentAddress)) {
          reclaimedAddress = nextResident->residentAddress;
          followingResident = nextResident->residentNext;
          residentCursor->residentNext = texture;
          texture->residentAddress = reclaimedAddress;
          texture->residentNext = followingResident;
          texture->residentTmuIndex = tmuIndex;
          g_GlideResidentTextureTail = texture;
          nextResident->residentNext = (GraphicsTextureResource *)0x0;
          nextResident->residentTmuIndex = GRAPHICS_TEXTURE_NOT_RESIDENT;
          nextResident->residentAddress = 0;
          goto Glide3_TextureResource_EnsureResident_CommitResidentPlacementAndDownloadMipMap;
        }
      }
      residentCursor = nextResident;
    } while (residentCursor != g_GlideResidentTextureTail);
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
  GrAspectRatio_t *aspectRatioLog2Field;
  GrLOD_t *largeLodLog2Field;
  DDPIXELFORMAT *texturePixelFormat;
  GraphicsTextureDownsampleShift shift;
  dword globalDownsampleShift;
  dword widthOrLodSize;
  dword heightValue;
  ArenaAllocEaxCf5 uploadAlloc;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  
  globalDownsampleShift = g_TextureDownsampleShift;
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(texture->subresourceIndex,texture->sourceAsset);
  heightValue = logicalSize.logicalHeightPixels;
  widthOrLodSize = logicalSize.logicalWidthPixels;
  texture->downsampleShift = globalDownsampleShift;
  (texture->glideInfo).aspectRatioLog2 = 0;
  while (widthOrLodSize != heightValue) {
    if ((int)widthOrLodSize < (int)heightValue) {
      aspectRatioLog2Field = &(texture->glideInfo).aspectRatioLog2;
      *aspectRatioLog2Field = *aspectRatioLog2Field + -1;
      widthOrLodSize = widthOrLodSize * 2;
    }
    else {
      aspectRatioLog2Field = &(texture->glideInfo).aspectRatioLog2;
      *aspectRatioLog2Field = *aspectRatioLog2Field + 1;
      heightValue = heightValue * 2;
    }
  }
  (texture->glideInfo).smallLodLog2 = 0;
  (texture->glideInfo).largeLodLog2 = 0;
  texturePixelFormat = texture->pixelFormat;
  for (widthOrLodSize = 1; widthOrLodSize != heightValue; widthOrLodSize = widthOrLodSize * 2) {
    (texture->glideInfo).smallLodLog2 = (texture->glideInfo).smallLodLog2 + 1;
    largeLodLog2Field = &(texture->glideInfo).largeLodLog2;
    *largeLodLog2Field = *largeLodLog2Field + 1;
  }
  (texture->glideInfo).format = 0xc;
  shift = texture->downsampleShift;
  if ((texturePixelFormat == (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DOpaqueTextureFormat,0)) || (texturePixelFormat == (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DSelectedOpaqueTextureFormat,0))) {
    (texture->glideInfo).format = 10;
  }
  (texture->glideInfo).smallLodLog2 = (texture->glideInfo).smallLodLog2 - shift;
  largeLodLog2Field = &(texture->glideInfo).largeLodLog2;
  *largeLodLog2Field = *largeLodLog2Field - shift;
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(texture->subresourceIndex,texture->sourceAsset);
  (texture->glideInfo).data = (void *)0x0;
  texture->residentNext = (GraphicsTextureResource *)0x0;
  texture->residentTmuIndex = GRAPHICS_TEXTURE_NOT_RESIDENT;
  texture->residentAddress = 0;
  uploadAlloc = (*g_MemoryApi.alloc)
                    ((logicalSize.logicalHeightPixels >> ((byte)shift & 0x1f)) *
                     (logicalSize.logicalWidthPixels >> ((byte)shift & 0x1f)) * 2);
  if (!uploadAlloc.carry) {
    (texture->glideInfo).data = (void *)uploadAlloc.eax;
    (*g_GlideTextureColorUpload[shift])(texture);
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
  GraphicsTextureResource *residentCursorOrNewTail;
  GraphicsTextureResource *replacementListHead;
  
  if (-1 < (int)texture->residentTmuIndex) {
    previousResidentTexture = (GraphicsTextureResource *)0x0;
    for (residentCursorOrNewTail = g_GlideResidentTextureHead; residentCursorOrNewTail != (GraphicsTextureResource *)0x0;
        residentCursorOrNewTail = residentCursorOrNewTail->residentNext) {
      if (residentCursorOrNewTail == texture) {
        residentCursorOrNewTail = texture->residentNext;
        replacementListHead = residentCursorOrNewTail;
        if (previousResidentTexture != (GraphicsTextureResource *)0x0) {
          previousResidentTexture->residentNext = residentCursorOrNewTail;
          residentCursorOrNewTail = previousResidentTexture;
          replacementListHead = g_GlideResidentTextureHead;
        }
        g_GlideResidentTextureHead = replacementListHead;
        if (texture == g_GlideResidentTextureTail) {
          g_GlideResidentTextureTail = residentCursorOrNewTail;
        }
        if (texture == g_GlideBoundTexture) {
          g_GlideBoundTexture = (GraphicsTextureResource *)0x0;
        }
        break;
      }
      previousResidentTexture = residentCursorOrNewTail;
    }
  }
  texture->residentTmuIndex = GRAPHICS_TEXTURE_NOT_RESIDENT;
  texture->residentNext = (GraphicsTextureResource *)0x0;
  texture->residentAddress = 0;
  (*g_MemoryApi.free)((texture->glideInfo).data);
  (texture->glideInfo).data = (void *)0x0;
  return;
}

