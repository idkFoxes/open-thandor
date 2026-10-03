/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/glide.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/glide.h>
#include <thandor/thandor.h>

/* Implementation ownership: graphics/backend/glide. */

/* The Glide 3 entry points, called through the addresses that DynAPI_Resolve stored in g_GlideImportBindings
   (GraphicsGlide3_ApplyDisplayModeAndInitializeResources, Glide3_InitAndEnumerate). Each wrapper is the original
   indirect call through the binding's .procedure, same arguments in the same order. */
static __inline void *GlideImport_Procedure(enum GlideImportIndex index)
{
  return g_GlideImportBindings[index].procedure;
}

static __inline void GlideImport_grAlphaBlendFunction(uint32_t rgbSourceFactor,uint32_t rgbDestinationFactor,
          uint32_t alphaSourceFactor,uint32_t alphaDestinationFactor)
{
  ((GrAlphaBlendFunctionImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_ALPHA_BLEND_FUNCTION))(rgbSourceFactor,
            rgbDestinationFactor,alphaSourceFactor,alphaDestinationFactor);
}

static __inline void GlideImport_grAlphaCombine(uint32_t function,uint32_t factor,uint32_t local,uint32_t other,
          uint32_t invert)
{
  ((GrAlphaCombineImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_ALPHA_COMBINE))(function,factor,local,other,
            invert);
}

static __inline void GlideImport_grBufferClear(uint32_t color,uint32_t alpha,uint32_t depth)
{
  ((GrBufferClearImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_BUFFER_CLEAR))(color,alpha,depth);
}

static __inline void GlideImport_grBufferSwap(uint32_t swapInterval)
{
  ((GrBufferSwapImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_BUFFER_SWAP))(swapInterval);
}

static __inline void GlideImport_grClipWindow(uint32_t minX,uint32_t minY,uint32_t maxX,uint32_t maxY)
{
  ((GrClipWindowImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_CLIP_WINDOW))(minX,minY,maxX,maxY);
}

static __inline void GlideImport_grColorCombine(uint32_t function,uint32_t factor,uint32_t local,uint32_t other,
          uint32_t invert)
{
  ((GrColorCombineImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_COLOR_COMBINE))(function,factor,local,other,
            invert);
}

static __inline void GlideImport_grCoordinateSpace(uint32_t mode)
{
  ((GrCoordinateSpaceImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_COORDINATE_SPACE))(mode);
}

static __inline void GlideImport_grCullMode(uint32_t mode)
{
  ((GrCullModeImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_CULL_MODE))(mode);
}

static __inline void GlideImport_grDepthBufferFunction(uint32_t function)
{
  ((GrDepthBufferFunctionImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_DEPTH_BUFFER_FUNCTION))(function);
}

static __inline void GlideImport_grDepthBufferMode(uint32_t mode)
{
  ((GrDepthBufferModeImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_DEPTH_BUFFER_MODE))(mode);
}

static __inline void GlideImport_grDepthMask(uint32_t enabled)
{
  ((GrDepthMaskImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_DEPTH_MASK))(enabled);
}

static __inline void GlideImport_grDrawTriangle(uint32_t *vertexA,uint32_t *vertexB,uint32_t *vertexC)
{
  ((GrDrawTriangleImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_DRAW_TRIANGLE))(vertexA,vertexB,vertexC);
}

static __inline void GlideImport_grFinish(void)
{
  ((GrFinishImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_FINISH))();
}

static __inline uint32_t GlideImport_grGet(uint32_t selector,uint32_t sizeBytes,void *output)
{
  return ((GrGetImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_GET))(selector,sizeBytes,output);
}

static __inline char *GlideImport_grGetString(uint32_t selector)
{
  return ((GrGetStringImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_GET_STRING))(selector);
}

static __inline void GlideImport_grGlideInit(void)
{
  ((GrGlideInitImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_GLIDE_INIT))();
}

static __inline void GlideImport_grGlideShutdown(void)
{
  ((GrGlideShutdownImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_GLIDE_SHUTDOWN))();
}

static __inline uint32_t GlideImport_grLfbLock(uint32_t lockType,uint32_t buffer,uint32_t writeMode,uint32_t origin,
          uint32_t pixelPipeline,void *lfbInfo)
{
  return ((GrLfbLockImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_LFB_LOCK))(lockType,buffer,writeMode,origin,
            pixelPipeline,lfbInfo);
}

static __inline uint32_t GlideImport_grLfbReadRegion(uint32_t buffer,GraphicsScreenCoordinate sourceX,
          GraphicsScreenCoordinate sourceY,GraphicsPixelDimension width,GraphicsPixelDimension height,
          uint32_t destinationStrideBytes,uint16_t *destinationPixels)
{
  return ((GrLfbReadRegionImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_LFB_READ_REGION))(buffer,sourceX,
            sourceY,width,height,destinationStrideBytes,destinationPixels);
}

static __inline void GlideImport_grLfbUnlock(uint32_t lockType,uint32_t buffer)
{
  ((GrLfbUnlockImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_LFB_UNLOCK))(lockType,buffer);
}

static __inline uint32_t GlideImport_grQueryResolutions(void *query,void *output)
{
  return ((GrQueryResolutionsImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_QUERY_RESOLUTIONS))(query,output);
}

static __inline void GlideImport_grSstSelect(uint32_t boardIndex)
{
  ((GrSstSelectImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_SST_SELECT))(boardIndex);
}

static __inline void GlideImport_grSstWinClose(uint32_t context)
{
  ((GrSstWinCloseImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_SST_WIN_CLOSE))(context);
}

static __inline uint32_t GlideImport_grSstWinOpen(uint32_t windowHandle,uint32_t screenResolution,
          uint32_t refreshRate,uint32_t colorFormat,uint32_t origin,uint32_t colorBufferCount,
          uint32_t auxiliaryBufferCount)
{
  return ((GrSstWinOpenImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_SST_WIN_OPEN))(windowHandle,
            screenResolution,refreshRate,colorFormat,origin,colorBufferCount,auxiliaryBufferCount);
}

static __inline void GlideImport_grTexClampMode(uint32_t tmuIndex,uint32_t sClampMode,uint32_t tClampMode)
{
  ((GrTexClampModeImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_TEX_CLAMP_MODE))(tmuIndex,sClampMode,
            tClampMode);
}

static __inline void GlideImport_grTexCombine(uint32_t tmuIndex,uint32_t rgbFunction,uint32_t rgbFactor,
          uint32_t alphaFunction,uint32_t alphaFactor,uint32_t rgbInvert,uint32_t alphaInvert)
{
  ((GrTexCombineImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_TEX_COMBINE))(tmuIndex,rgbFunction,rgbFactor,
            alphaFunction,alphaFactor,rgbInvert,alphaInvert);
}

static __inline void GlideImport_grTexDownloadMipMap(GraphicsTextureResidentTmuIndex tmuIndex,
          GraphicsTextureMemoryAddress startAddress,uint32_t evenOddMask,GrTexInfo *textureInfo)
{
  ((GrTexDownloadMipMapImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_TEX_DOWNLOAD_MIP_MAP))(tmuIndex,
            startAddress,evenOddMask,textureInfo);
}

static __inline void GlideImport_grTexFilterMode(uint32_t tmuIndex,uint32_t minifyFilter,uint32_t magnifyFilter)
{
  ((GrTexFilterModeImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_TEX_FILTER_MODE))(tmuIndex,minifyFilter,
            magnifyFilter);
}

static __inline GraphicsTextureMemoryAddress GlideImport_grTexMaxAddress(GraphicsTextureResidentTmuIndex tmuIndex)
{
  return ((GrTexMaxAddressImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_TEX_MAX_ADDRESS))(tmuIndex);
}

static __inline GraphicsTextureMemoryAddress GlideImport_grTexMinAddress(GraphicsTextureResidentTmuIndex tmuIndex)
{
  return ((GrTexMinAddressImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_TEX_MIN_ADDRESS))(tmuIndex);
}

static __inline void GlideImport_grTexMipMapMode(uint32_t tmuIndex,uint32_t mode,uint32_t lodBlend)
{
  ((GrTexMipMapModeImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_TEX_MIP_MAP_MODE))(tmuIndex,mode,lodBlend);
}

static __inline void GlideImport_grTexSource(GraphicsTextureResidentTmuIndex tmuIndex,
          GraphicsTextureMemoryAddress residentAddress,uint32_t mode,GrTexInfo *textureInfo)
{
  ((GrTexSourceImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_TEX_SOURCE))(tmuIndex,residentAddress,mode,
            textureInfo);
}

static __inline void GlideImport_grVertexLayout(uint32_t parameter,uint32_t byteOffset,uint32_t mode)
{
  ((GrVertexLayoutImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_VERTEX_LAYOUT))(parameter,byteOffset,mode);
}

static __inline void GlideImport_grViewport(uint32_t x,uint32_t y,uint32_t width,uint32_t height)
{
  ((GrViewportImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GR_VIEWPORT))(x,y,width,height);
}

static __inline void GlideImport_guGammaCorrectionRGB(uint32_t redGamma,uint32_t greenGamma,uint32_t blueGamma)
{
  ((GuGammaCorrectionRGBImportProc *)GlideImport_Procedure(GLIDE_IMPORT_GU_GAMMA_CORRECTION_RGB))(redGamma,
            greenGamma,blueGamma);
}

/* MMX lane helpers for the converters and blitters below. Each one is exactly the Ghidra CONCAT form it
   replaces (same masking, same unsigned result). */

/* PUNPCKLBW of a byte with itself: 0xbb -> 0xbbbb (one unsigned 16-bit lane). */
#define GLIDE_DUP_BYTE(value) ((uint32_t)(((uint32_t)(uint8_t)(value) << 8) | (uint32_t)(uint8_t)(value)))

/* PACKUSWB of one word lane as Ghidra modelled it: values above 0xff saturate to 0xff. */
#define GLIDE_SATURATE_WORD_TO_BYTE(value) ((uint8_t)((value) > ARGB8888_CHANNEL_MAX ? ARGB8888_CHANNEL_MAX : (value)))

/* Average of one ARGB8888 channel (selected by bitShift) over four samples, done as in the MMX original:
   byte-duplicated word lanes, PSRLW 4, PADDW, PSRLW 6. */
#define GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(sample0,sample1,sample2,sample3,bitShift) \
  ((uint16_t)((GLIDE_DUP_BYTE((sample0) >> (bitShift)) >> 4) + (GLIDE_DUP_BYTE((sample1) >> (bitShift)) >> 4) + \
            (GLIDE_DUP_BYTE((sample2) >> (bitShift)) >> 4) + (GLIDE_DUP_BYTE((sample3) >> (bitShift)) >> 4)) >> 6)

/* Four 16-bit lanes into one qword, lane0 lowest (Ghidra: the CONCAT26/CONCAT24/CONCAT22 nest). */
static __inline uint64_t Glide_PackWordLanes(uint32_t lane3,uint32_t lane2,uint32_t lane1,uint32_t lane0)
{
  return ((uint64_t)(uint16_t)lane3 << 48) | ((uint64_t)(uint16_t)lane2 << 32) | ((uint64_t)(uint16_t)lane1 << 16) |
         (uint64_t)(uint16_t)lane0;
}

/* PADDW of two lane sets (each lane truncated to 16 bits), packed back into one qword. */
#define GLIDE_ADD_WORD_LANES(lanesA,lanesB) \
  Glide_PackWordLanes((short)((lanesA) >> 48) + (short)((lanesB) >> 48), \
                      (short)((lanesA) >> 32) + (short)((lanesB) >> 32), \
                      (short)((lanesA) >> 16) + (short)((lanesB) >> 16), \
                      (short)(lanesA) + (short)(lanesB))

/* PMULLW of a native pixel broadcast into all lanes (and masked with GLIDE_PACKED_PIXEL_MASKS) by the channel
   unpack scales, then PSRLW 2: the destination colour as word lanes (lane3 zero, lane0 blue). */
#define GLIDE_UNPACK_NATIVE_LANES(lanes) \
  Glide_PackWordLanes((uint16_t)((short)((lanes) >> 48) * g_SoftwarePixelMmxConstants.unpackScales.zero) >> 2, \
                      (uint16_t)((short)((lanes) >> 32) * g_SoftwarePixelMmxConstants.unpackScales.red) >> 2, \
                      (uint16_t)((short)((lanes) >> 16) * g_SoftwarePixelMmxConstants.unpackScales.green) >> 2, \
                      (uint16_t)((short)(lanes) * g_SoftwarePixelMmxConstants.unpackScales.blue) >> 2)

/* The MMX mask constants as qwords. */
#define GLIDE_LANES_AS_QWORD(lanes) \
  Glide_PackWordLanes((lanes).zero,(lanes).red,(lanes).green,(lanes).blue)
#define GLIDE_QUANTIZE_MASKS GLIDE_LANES_AS_QWORD(g_SoftwarePixelMmxConstants.quantizeMasksQ12)
#define GLIDE_PACKED_PIXEL_MASKS GLIDE_LANES_AS_QWORD(g_SoftwarePixelMmxConstants.packedPixelMasks)

/* PUNPCKLBW mm,mm + PSRLW shift of an ARGB8888 pixel: one byte-duplicated, shifted word lane per channel
   (lane3 = alpha, lane0 = blue). */
static __inline uint64_t Glide_UnpackArgbToWordLanes(uint32_t argb,int shift)
{
  return Glide_PackWordLanes(GLIDE_DUP_BYTE(argb >> 24) >> shift,GLIDE_DUP_BYTE(argb >> 16) >> shift,
                             GLIDE_DUP_BYTE(argb >> 8) >> shift,GLIDE_DUP_BYTE(argb) >> shift);
}

/* 'gfx' texture source access (layout: GFX_* in graphics/resources/texture.h). */
/* Asset-relative offset of the subresource record of subresourceIndex. */
#define GLIDE_RECORD_OFFSET(asset,subresourceIndex) \
  ((subresourceIndex) * GFX_SUBRESOURCE_RECORD_SIZE + ((asset)->tableDescriptor).subresourceTableOffset)
/* Field GFX_SUBRESOURCE_<field> of the subresource record at asset-relative recordOffset. */
#define GLIDE_RECORD_INT(asset,recordOffset,field) \
  (*(int *)((uint8_t *)(asset) + (recordOffset) + GFX_SUBRESOURCE_##field))
#define GLIDE_RECORD_UINT(asset,recordOffset,field) \
  (*(uint32_t *)((uint8_t *)(asset) + (recordOffset) + GFX_SUBRESOURCE_##field))
/* Address of the byte at an asset-relative offset (a record's GFX_SUBRESOURCE_PIXEL_OFFSET). */
#define GLIDE_ASSET_BYTES(asset,offset) \
  ((uint8_t *)(asset) + (offset))
/* Palette entry `index` of palette bank `bank`: 8 bytes at asset + GFX_ASSET_HEADER_SIZE +
   bank * GFX_PALETTE_BANK_SIZE + index * 8 (asset[bank * 4 + 1], the asset struct being 0x200 bytes), holding
   the ARGB8888 colour and then the native framebuffer pixel. */
#define GLIDE_PALETTE_ARGB(asset,bank,index) \
  (*(uint32_t *)((uint8_t *)&(asset)[(bank) * 4 + 1] + (index) * 8))
#define GLIDE_PALETTE_NATIVE(asset,bank,index) \
  (*(uint32_t *)((uint8_t *)&(asset)[(bank) * 4 + 1] + (index) * 8 + 4))

/* Address: 0x005801B0.
   Glide backend of texture-set creation: gives every subresource of the set's source asset a fresh
   GraphicsTextureResource (pixel format, current downsample shift), prepares its Glide upload data and
   registers it in the texture slot table. A resource that cannot be allocated or registered leaves its entry
   empty; the function itself always succeeds (CF clear). EAX carries the set from
   GraphicsTextureSet_AllocateMetadata; the stack argument (RET 4) is unused, set->sourceAsset is read instead.
*/
bool Glide3_TextureSet_CreateBackend(GraphicsTextureSet *textureSet,GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *setSourceAsset;
  uint32_t currentDownsampleShift;
  DDPIXELFORMAT *selectedPixelFormat;
  GraphicsTextureResource *texture;
  bool registerFailed;
  uint32_t textureAllocError;
  GraphicsTextureSetEntry *entryCursor;
  AssetSubresourceCount remainingSubresources;
  GraphicsSubresourceIndex currentSubresource;
  
  setSourceAsset = textureSet->sourceAsset;
  remainingSubresources = (setSourceAsset->tableDescriptor).subresourceCount;
  entryCursor = textureSet->entries;
  currentSubresource = 0;
  do {
    selectedPixelFormat = GraphicsTexture_SelectPixelFormat(currentSubresource,setSourceAsset);
    textureAllocError = g_MemoryApi.alloc(sizeof(GraphicsTextureResource),(void **)&texture);
    if (textureAllocError != 0) {
      texture = (GraphicsTextureResource *)textureAllocError;
    }
    else {
      texture->stagingTexture2 = NULL;
      texture->stagingSurface3 = NULL;
      texture->stagingSurfaceBase = NULL;
      entryCursor->texture = texture;
      texture->deviceTexture2 = NULL;
      texture->deviceSurface3 = NULL;
      texture->deviceSurfaceBase = NULL;
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
        g_MemoryApi.free(texture);
        entryCursor->texture = NULL;
      }
    }
    currentSubresource++;
    entryCursor++;
    remainingSubresources = remainingSubresources - 1;
  } while (remainingSubresources != 0);
  return false;
}


/* Address: 0x00580430.
   Releases and re-prepares the Glide upload data of every registered texture (all 4096 slots), for when the
   Glide texture state has to be rebuilt.
*/
void Glide3_TextureResource_ReinitializeAll(void)

{
  GraphicsTextureResource *texture;
  int textureSlotsRemaining;
  GraphicsTextureResource **textureSlotCursor;

  textureSlotsRemaining = GRAPHICS_TEXTURE_SLOT_CAPACITY;
  textureSlotCursor = g_GraphicsTextureSlots;
  do {
    texture = *textureSlotCursor;
    if (texture != NULL) {
      Glide3_TextureResource_Release(texture);
      Glide3_TextureResource_Initialize(texture);
    }
    textureSlotCursor++;
    textureSlotsRemaining--;
  } while (textureSlotsRemaining != 0);
  return;
}


/* Address: 0x0057F0C0.
   Glide entry of the backend shutdown slot: just calls Glide3_Shutdown.
*/
void __cdecl GlideBackend_ShutdownWrapper(void)

{
  Glide3_Shutdown();
  return;
}

/* Not an original function: the Glide resolution code of a width x height display mode (the original compares
   height:width packed into one dword). False for any size Glide 3 does not offer. */
static bool Glide3_ResolutionCodeForSize(GraphicsPixelDimension height,GraphicsPixelDimension width,
          uint32_t *resolutionCode)
{
  switch (width | height << 16) {
  case (480 << 16) | 640:
    *resolutionCode = GR_RESOLUTION_640x480;
    return true;
  case (600 << 16) | 800:
    *resolutionCode = GR_RESOLUTION_800x600;
    return true;
  case (720 << 16) | 960:
    *resolutionCode = GR_RESOLUTION_960x720;
    return true;
  case (768 << 16) | 1024:
    *resolutionCode = GR_RESOLUTION_1024x768;
    return true;
  case (1024 << 16) | 1280:
    *resolutionCode = GR_RESOLUTION_1280x1024;
    return true;
  case (1200 << 16) | 1600:
    *resolutionCode = GR_RESOLUTION_1600x1200;
    return true;
  default:
    return false;
  }
}

/* Not an original function: the part of GraphicsGlide3_ApplyDisplayModeAndInitializeResources after the Glide
   window is open: reads the TMU count, installs the Glide framebuffer description (RGB565) and the Glide
   framebuffer/blit handlers, and sets the fixed Glide render state for every TMU. */
static void Glide3_InstallFramebufferAndRenderState(FrontendDisplayAdapterIndex adapterIndex,
          GraphicsPixelDimension height,GraphicsPixelDimension width)
{
  uint32_t *tmuCountOutput;
  uint32_t tmuCount;
  GraphicsTextureResidentTmuIndex tmuIndex;
  GraphicsTextureMemoryAddress tmuAddress;

  tmuCountOutput = &g_GraphicsAdapters[adapterIndex].glideTmuCount;
  GlideImport_grGet(GR_NUM_TMU,sizeof *tmuCountOutput,tmuCountOutput);
  g_GlideTmuCount = *tmuCountOutput;
  g_FramebufferWidth = width;
  g_FramebufferHeight = height;
  g_ActiveGraphicsAdapterIndex = adapterIndex;
  if ((int)g_GlideTmuCount < 1) {
    g_GlideTmuCount = 1;
  }
  else if (16 < (int)g_GlideTmuCount) {
    g_GlideTmuCount = 16;
  }
  g_DisplayFramebufferAccess.width = width;
  g_DisplayFramebufferAccess.height = height;
  g_DisplayFramebufferAccess.bytesPerPixel = SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT;
  g_DisplayFramebufferAccess.pixels = NULL;
  g_FramebufferAccess = &g_DisplayFramebufferAccess;
  /* the Glide framebuffer is locked as RGB565 (GR_LFBWRITEMODE_565) */
  g_SoftwarePixelFormatConfig.redMask = RGB565_RED_MASK;
  g_SoftwarePixelFormatConfig.greenMask = RGB565_GREEN_MASK;
  g_SoftwarePixelFormatConfig.blueMask = RGB565_BLUE_MASK;
  g_SoftwarePixelFormatConfig.redShift = 11;
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
  g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha = Glide3_TextureSource_BlitIntegerScaledSourceAlpha;
  g_GraphicsTextureSourceBlitSourceAlphaPaletteBank = Glide3_TextureSource_BlitSourceAlphaPaletteBank;
  g_GraphicsTextureSourceBlitModulatedSourceAlpha = Glide3_TextureSource_BlitModulatedSourceAlpha;
  g_GraphicsTextureSourceBlitSaturatedAddRgb = Glide3_TextureSource_BlitSaturatedAddRgb;
  g_GraphicsFramebufferFillRectArgb = Glide3_Framebuffer_FillRectArgb;
  g_GraphicsTextureSourceBlitHalfSourceRgb = Glide3_TextureSource_BlitHalfSourceRgb;
  g_GraphicsTextureSourceStretchDirectColorBilinear = Glide3_TextureSource_StretchDirectColorBilinear;
  g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd = Glide3_TextureSource_BlitHalfRgbSaturatedAdd;
  /* 1.0f for red, green and blue */
  GlideImport_guGammaCorrectionRGB(GLIDE_FLOAT_BITS_ONE,GLIDE_FLOAT_BITS_ONE,GLIDE_FLOAT_BITS_ONE);
  GlideImport_grCoordinateSpace(GR_WINDOW_COORDS);
  /* the layout of the g_GlideVertices records handed to grDrawTriangle (byte offsets of GLIDE_VERTEX_*) */
  GlideImport_grVertexLayout(GR_PARAM_XY,0,GR_PARAM_ENABLE);
  GlideImport_grVertexLayout(GR_PARAM_Z,8,GR_PARAM_ENABLE);
  GlideImport_grVertexLayout(GR_PARAM_Q,12,GR_PARAM_ENABLE);
  GlideImport_grVertexLayout(GR_PARAM_ST0,20,GR_PARAM_ENABLE);
  GlideImport_grVertexLayout(GR_PARAM_PARGB,28,GR_PARAM_ENABLE);
  GlideImport_grCullMode(GR_CULL_DISABLE);
  GlideImport_grDepthBufferMode(GR_DEPTHBUFFER_ZBUFFER);
  GlideImport_grDepthBufferFunction(GR_CMP_GEQUAL); /* the vertices carry a scaled 1/depth as Z */
  GlideImport_grDepthMask(FXTRUE);
  g_GlideDepthWriteEnabledState = 1;
  /* g_GlideTmuCount is 1..16 here */
  tmuCount = g_GlideTmuCount;
  for (tmuIndex = GRAPHICS_TEXTURE_RESIDENT_TMU0; (uint32_t)tmuIndex < tmuCount; tmuIndex++) {
    GlideImport_grTexMipMapMode(tmuIndex,GR_MIPMAP_DISABLE,FXFALSE);
    GlideImport_grTexClampMode(tmuIndex,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);
    GlideImport_grTexFilterMode(tmuIndex,GR_TEXTUREFILTER_BILINEAR,GR_TEXTUREFILTER_BILINEAR);
    GlideImport_grTexCombine(tmuIndex,GR_COMBINE_FUNCTION_LOCAL,GR_COMBINE_FACTOR_LOCAL,GR_COMBINE_FUNCTION_LOCAL,
                   GR_COMBINE_FACTOR_LOCAL,FXFALSE,FXFALSE);
    tmuAddress = GlideImport_grTexMinAddress(tmuIndex);
    g_GlideTmuMinAddress[tmuIndex] = tmuAddress;
    tmuAddress = GlideImport_grTexMaxAddress(tmuIndex);
    g_GlideTmuMaxAddress[tmuIndex] = tmuAddress;
  }
  /* textured: texture colour/alpha times the iterated vertex colour/alpha */
  GlideImport_grColorCombine(GR_COMBINE_FUNCTION_SCALE_OTHER,GR_COMBINE_FACTOR_LOCAL,GR_COMBINE_LOCAL_ITERATED,
                   GR_COMBINE_OTHER_TEXTURE,FXFALSE);
  GlideImport_grAlphaCombine(GR_COMBINE_FUNCTION_SCALE_OTHER,GR_COMBINE_FACTOR_LOCAL,GR_COMBINE_LOCAL_ITERATED,
                   GR_COMBINE_OTHER_TEXTURE,FXFALSE);
  g_GlideTexturingDisabledState = 1;
  /* State 0 stands for SRC_ALPHA/ONE_MINUS_SRC_ALPHA in Glide3_DrawPrimitiveQueue, but the original
     starts with ONE_MINUS_DST_ALPHA here, which stays until another blend mode is drawn. */
  GlideImport_grAlphaBlendFunction(GR_BLEND_SRC_ALPHA,GR_BLEND_ONE_MINUS_DST_ALPHA,GR_BLEND_ONE,GR_BLEND_ZERO);
  g_GlideBlendModeState = 0;
  g_GlideBoundTexture = NULL;
  g_GlideResidentTextureHead = NULL;
  g_GlideResidentTextureTail = (GraphicsTextureResource *)g_GlideTmuMinAddress[0];
  g_PrimarySurface3 = NULL;
}

/* Address: 0x0057F0F0.
   Glide 3 display-mode switch: accepts only the Glide resolutions 640x480 .. 1600x1200, loads glide3x.dll,
   selects the adapter's board, opens the window with the highest refresh rate the board offers for that
   resolution, installs the Glide framebuffer and blit handlers (RGB565 layout), sets the fixed Glide render
   state for every TMU and re-prepares all registered textures. CF set on failure; the DLL is unloaded again.
*/
bool GraphicsGlide3_ApplyDisplayModeAndInitializeResources
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width)

{
  uint32_t resolutionQueryCode;
  HINSTANCE glideDll;
  GlideImportBinding *binding;
  uint32_t sstIndex;
  uint32_t resolutionListSize;
  uint32_t resolutionCount;
  uint32_t resolutionNumber;
  GrResolution *resolutionList;
  uint32_t refreshRateCode;
  uint32_t refreshRateHz;
  uint32_t bestRefreshRateHz;
  uint32_t selectedRefreshRateCode;
  uint32_t finalizeError; /* discarded: only success or failure leaves this function */
  int slotIndex;

  /* The original computes an error code in EAX on each failure path (FATAL_ERROR_DIRECTDRAW_SET_DISPLAY_MODE
     (0x1A) for an unsupported resolution at 0x0057F156, 0x19 when the board lists no resolution, 0x50 when
     grSstWinOpen fails), but the common exit restores the caller's EAX (POP EAX at 0x0057F164 and 0x0057F5BB),
     so only CF leaves the function; the bool return is complete. */
  if (!Glide3_ResolutionCodeForSize(height,width,&resolutionQueryCode)) {
    return true;
  }
  glideDll = DynDLL_Load(sz_GLIDE3X);
  if (glideDll == NULL) {
    return true;
  }
  g_GlideRuntimeActiveCount++;
  binding = g_GlideImportBindings;
  do {
    if (DynAPI_Resolve(&binding->procedure,glideDll,binding->importName) != 0) {
      DynDLL_Unload(sz_GLIDE3X);
      g_GlideRuntimeActiveCount = 0;
      return true;
    }
    binding++;
  } while (binding->importName != NULL);
  GlideImport_grGlideInit();
  /* Glide3_InitAndEnumerate stored the board index in Data2/Data3 of the adapter GUID */
  sstIndex = (uint32_t)g_GraphicsAdapters[adapterIndex].adapterGuid.Data2 |
             ((uint32_t)g_GraphicsAdapters[adapterIndex].adapterGuid.Data3 << 16);
  GlideImport_grSstSelect(sstIndex);
  g_GlideSelectedResolutionQuery.resolution = resolutionQueryCode;
  resolutionListSize = GlideImport_grQueryResolutions(&g_GlideSelectedResolutionQuery,NULL);
  /* at least one GrResolution record, and the list buffer allocated */
  if ((15 < (int)resolutionListSize) &&
      (g_MemoryApi.alloc(resolutionListSize,(void **)&resolutionList) == 0)) {
    resolutionCount = resolutionListSize / sizeof(GrResolution);
    GlideImport_grQueryResolutions(&g_GlideSelectedResolutionQuery,resolutionList);
    bestRefreshRateHz = 0;
    for (resolutionNumber = 0; resolutionNumber < resolutionCount; resolutionNumber++) {
      /* codes beyond GR_REFRESH_120Hz (8) are ignored */
      refreshRateCode = resolutionList[resolutionNumber].refresh;
      if (refreshRateCode < 9) {
        refreshRateHz = g_GlideRefreshRatesHz[refreshRateCode];
        if (bestRefreshRateHz <= refreshRateHz) {
          bestRefreshRateHz = refreshRateHz;
          selectedRefreshRateCode = refreshRateCode;
        }
      }
    }
    g_MemoryApi.free(resolutionList);
    /* two colour buffers, one aux (depth) buffer */
    g_GlideWindowContextHandle =
         GlideImport_grSstWinOpen((uint32_t)g_MainWindow,resolutionQueryCode,selectedRefreshRateCode,
                        GR_COLORFORMAT_ARGB,GR_ORIGIN_UPPER_LEFT,2,1);
    if (g_GlideWindowContextHandle != 0) {
      Glide3_InstallFramebufferAndRenderState(adapterIndex,height,width);
      if (g_GraphicsDisplayModeFinalize(adapterIndex,bitsPerPixel,height,width,&finalizeError)) {
        for (slotIndex = 0; slotIndex < GRAPHICS_TEXTURE_SLOT_CAPACITY; slotIndex++) {
          if (g_GraphicsTextureSlots[slotIndex] != NULL) {
            Glide3_TextureResource_Initialize(g_GraphicsTextureSlots[slotIndex]);
          }
        }
        return false;
      }
      GlideImport_grSstWinClose(g_GlideWindowContextHandle);
    }
  }
  GlideImport_grGlideShutdown(); /* grGlideShutdown(void) */
  DynDLL_Unload(sz_GLIDE3X);
  g_GlideRuntimeActiveCount = 0;
  return true;
}


/* The g_GlideVertices records are floats stored in uint32_t globals: FSTP float ptr writes the bit pattern,
   CMP/ADD/SUB dword ptr then work on that pattern, FMUL float ptr reads it back as a float. */
/* Not an original function: the bit pattern of a float (the FSTP float ptr store). */
static __inline uint32_t Glide_FloatBits(float value)
{
  uint32_t bits;
  memcpy(&bits,&value,sizeof bits);
  return bits;
}

/* Not an original function: a stored bit pattern read back as a float (the FMUL/FLD float ptr load). */
static __inline float Glide_BitsToFloat(uint32_t bits)
{
  float value;
  memcpy(&value,&bits,sizeof value);
  return value;
}

/* Address: 0x0057F7B0.
   Glide backend of Graphics_DrawPrimitiveQueue: converts each queued triangle into the three g_GlideVertices
   records (clamped screen position, 1/depth, perspective-corrected texture coordinates scaled to the texture's
   larger side, packed colour), switches colour combine, blend function and depth writes only when they change,
   and draws it with grDrawTriangle. Does nothing when the backend is already being accessed. The clip
   rectangle arguments are unused.
   The vertex records hold float bit patterns (FILD/FSTP float ptr, 0x0057F89F), and every nonzero pattern is
   rescaled by integer arithmetic on its exponent (SUB 0x6000000 at 0x0057F8C0, ADD 0xF000000 at 0x0057FA42).
   The x87 intermediates (FILD of an int, FIADD, FDIVRP, FMUL) are done in double below (FILD is exact, the
   CRT's default x87 precision is 53 bits) and rounded to float only at the FSTP.
*/
void Glide3_DrawPrimitiveQueue(int32_t clipMaxY,int32_t clipMaxX,int32_t clipMinY,int32_t clipMinX,
          GraphicsPrimitiveQueue *queue)

{
  GraphicsTextureSetEntry *packetTextureEntry;
  uint32_t textureHeightLog2;
  GraphicsTextureResource *texture;
  GraphicsPrimitivePacket *currentPacket;
  int32_t previousAccessState;
  uint8_t coordinateShift;
  uint32_t maxDimensionLog2;
  uint32_t textureWidthLog2;
  uint32_t renderFlags;
  uint32_t blendMode;

  previousAccessState = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_GraphicsBackendAccessState,1);
  if (previousAccessState == 0) {
    currentPacket = GraphicsPrimitiveQueue_Begin(queue);
    while (currentPacket != NULL) {
      if (currentPacket->vertices[0].screenX < GLIDE_SCREEN_COORDINATE_LIMIT + 1) {
        if (currentPacket->vertices[0].screenX < -GLIDE_SCREEN_COORDINATE_LIMIT) {
          currentPacket->vertices[0].screenX = -GLIDE_SCREEN_COORDINATE_LIMIT;
        }
      }
      else {
        currentPacket->vertices[0].screenX = GLIDE_SCREEN_COORDINATE_LIMIT;
      }
      if (currentPacket->vertices[1].screenX < GLIDE_SCREEN_COORDINATE_LIMIT + 1) {
        if (currentPacket->vertices[1].screenX < -GLIDE_SCREEN_COORDINATE_LIMIT) {
          currentPacket->vertices[1].screenX = -GLIDE_SCREEN_COORDINATE_LIMIT;
        }
      }
      else {
        currentPacket->vertices[1].screenX = GLIDE_SCREEN_COORDINATE_LIMIT;
      }
      if (currentPacket->vertices[2].screenX < GLIDE_SCREEN_COORDINATE_LIMIT + 1) {
        if (currentPacket->vertices[2].screenX < -GLIDE_SCREEN_COORDINATE_LIMIT) {
          currentPacket->vertices[2].screenX = -GLIDE_SCREEN_COORDINATE_LIMIT;
        }
      }
      else {
        currentPacket->vertices[2].screenX = GLIDE_SCREEN_COORDINATE_LIMIT;
      }
      if (currentPacket->vertices[0].screenY < GLIDE_SCREEN_COORDINATE_LIMIT + 1) {
        if (currentPacket->vertices[0].screenY < -GLIDE_SCREEN_COORDINATE_LIMIT) {
          currentPacket->vertices[0].screenY = -GLIDE_SCREEN_COORDINATE_LIMIT;
        }
      }
      else {
        currentPacket->vertices[0].screenY = GLIDE_SCREEN_COORDINATE_LIMIT;
      }
      if (currentPacket->vertices[1].screenY < GLIDE_SCREEN_COORDINATE_LIMIT + 1) {
        if (currentPacket->vertices[1].screenY < -GLIDE_SCREEN_COORDINATE_LIMIT) {
          currentPacket->vertices[1].screenY = -GLIDE_SCREEN_COORDINATE_LIMIT;
        }
      }
      else {
        currentPacket->vertices[1].screenY = GLIDE_SCREEN_COORDINATE_LIMIT;
      }
      if (currentPacket->vertices[2].screenY < GLIDE_SCREEN_COORDINATE_LIMIT + 1) {
        if (currentPacket->vertices[2].screenY < -GLIDE_SCREEN_COORDINATE_LIMIT) {
          currentPacket->vertices[2].screenY = -GLIDE_SCREEN_COORDINATE_LIMIT;
        }
      }
      else {
        currentPacket->vertices[2].screenY = GLIDE_SCREEN_COORDINATE_LIMIT;
      }
      g_GlideVertices[0][GLIDE_VERTEX_SCREEN_X] = Glide_FloatBits((float)(double)currentPacket->vertices[0].screenX);
      g_GlideVertices[0][GLIDE_VERTEX_SCREEN_Y] = Glide_FloatBits((float)(double)currentPacket->vertices[0].screenY);
      if (g_GlideVertices[0][GLIDE_VERTEX_SCREEN_X] != 0) {
        g_GlideVertices[0][GLIDE_VERTEX_SCREEN_X] = g_GlideVertices[0][GLIDE_VERTEX_SCREEN_X] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      if (g_GlideVertices[0][GLIDE_VERTEX_SCREEN_Y] != 0) {
        g_GlideVertices[0][GLIDE_VERTEX_SCREEN_Y] = g_GlideVertices[0][GLIDE_VERTEX_SCREEN_Y] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      g_GlideVertices[1][GLIDE_VERTEX_SCREEN_X] = Glide_FloatBits((float)(double)currentPacket->vertices[1].screenX);
      g_GlideVertices[1][GLIDE_VERTEX_SCREEN_Y] = Glide_FloatBits((float)(double)currentPacket->vertices[1].screenY);
      if (g_GlideVertices[1][GLIDE_VERTEX_SCREEN_X] != 0) {
        g_GlideVertices[1][GLIDE_VERTEX_SCREEN_X] = g_GlideVertices[1][GLIDE_VERTEX_SCREEN_X] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      if (g_GlideVertices[1][GLIDE_VERTEX_SCREEN_Y] != 0) {
        g_GlideVertices[1][GLIDE_VERTEX_SCREEN_Y] = g_GlideVertices[1][GLIDE_VERTEX_SCREEN_Y] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      g_GlideVertices[2][GLIDE_VERTEX_SCREEN_X] = Glide_FloatBits((float)(double)currentPacket->vertices[2].screenX);
      g_GlideVertices[2][GLIDE_VERTEX_SCREEN_Y] = Glide_FloatBits((float)(double)currentPacket->vertices[2].screenY);
      if (g_GlideVertices[2][GLIDE_VERTEX_SCREEN_X] != 0) {
        g_GlideVertices[2][GLIDE_VERTEX_SCREEN_X] = g_GlideVertices[2][GLIDE_VERTEX_SCREEN_X] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      if (g_GlideVertices[2][GLIDE_VERTEX_SCREEN_Y] != 0) {
        g_GlideVertices[2][GLIDE_VERTEX_SCREEN_Y] = g_GlideVertices[2][GLIDE_VERTEX_SCREEN_Y] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      g_GlideVertices[0][GLIDE_VERTEX_DIFFUSE_COLOR] = currentPacket->vertices[0].diffuseColor;
      g_GlideVertices[1][GLIDE_VERTEX_DIFFUSE_COLOR] = currentPacket->vertices[1].diffuseColor;
      g_GlideVertices[2][GLIDE_VERTEX_DIFFUSE_COLOR] = currentPacket->vertices[2].diffuseColor;
      packetTextureEntry = currentPacket->textureEntry;
      if (packetTextureEntry != NULL) {
        textureWidthLog2 = packetTextureEntry->widthLog2;
        textureHeightLog2 = packetTextureEntry->heightLog2;
        maxDimensionLog2 = textureWidthLog2;
        if (textureWidthLog2 < textureHeightLog2) {
          maxDimensionLog2 = textureHeightLog2;
        }
        coordinateShift = (char)maxDimensionLog2 - (char)textureWidthLog2;
        currentPacket->vertices[0].textureU = currentPacket->vertices[0].textureU >> (coordinateShift & SHIFT_COUNT_MASK);
        currentPacket->vertices[1].textureU = currentPacket->vertices[1].textureU >> (coordinateShift & SHIFT_COUNT_MASK);
        currentPacket->vertices[2].textureU = currentPacket->vertices[2].textureU >> (coordinateShift & SHIFT_COUNT_MASK);
        coordinateShift = (char)maxDimensionLog2 - (char)textureHeightLog2;
        currentPacket->vertices[0].textureV = currentPacket->vertices[0].textureV >> (coordinateShift & SHIFT_COUNT_MASK);
        currentPacket->vertices[1].textureV = currentPacket->vertices[1].textureV >> (coordinateShift & SHIFT_COUNT_MASK);
        currentPacket->vertices[2].textureV = currentPacket->vertices[2].textureV >> (coordinateShift & SHIFT_COUNT_MASK);
      }
      g_GlideVertices[0][GLIDE_VERTEX_RECIPROCAL_DEPTH] =
           Glide_FloatBits((float)(1.0 / (double)currentPacket->vertices[0].depth));
      g_GlideVertices[1][GLIDE_VERTEX_RECIPROCAL_DEPTH] =
           Glide_FloatBits((float)(1.0 / (double)currentPacket->vertices[1].depth));
      g_GlideVertices[2][GLIDE_VERTEX_RECIPROCAL_DEPTH] =
           Glide_FloatBits((float)(1.0 / (double)currentPacket->vertices[2].depth));
      g_GlideVertices[0][GLIDE_VERTEX_PERSPECTIVE_SCALE] =
           Glide_FloatBits((float)(4096.0 / ((double)currentPacket->vertices[0].depth + 4096.0)));
      g_GlideVertices[1][GLIDE_VERTEX_PERSPECTIVE_SCALE] =
           Glide_FloatBits((float)(4096.0 / ((double)currentPacket->vertices[1].depth + 4096.0)));
      g_GlideVertices[2][GLIDE_VERTEX_PERSPECTIVE_SCALE] =
           Glide_FloatBits((float)(4096.0 / ((double)currentPacket->vertices[2].depth + 4096.0)));
      if (g_GlideVertices[0][GLIDE_VERTEX_RECIPROCAL_DEPTH] != 0) {
        g_GlideVertices[0][GLIDE_VERTEX_RECIPROCAL_DEPTH] =
             g_GlideVertices[0][GLIDE_VERTEX_RECIPROCAL_DEPTH] + GLIDE_FLOAT_BITS_MULTIPLY_BY_2POW30;
      }
      if (g_GlideVertices[1][GLIDE_VERTEX_RECIPROCAL_DEPTH] != 0) {
        g_GlideVertices[1][GLIDE_VERTEX_RECIPROCAL_DEPTH] =
             g_GlideVertices[1][GLIDE_VERTEX_RECIPROCAL_DEPTH] + GLIDE_FLOAT_BITS_MULTIPLY_BY_2POW30;
      }
      if (g_GlideVertices[2][GLIDE_VERTEX_RECIPROCAL_DEPTH] != 0) {
        g_GlideVertices[2][GLIDE_VERTEX_RECIPROCAL_DEPTH] =
             g_GlideVertices[2][GLIDE_VERTEX_RECIPROCAL_DEPTH] + GLIDE_FLOAT_BITS_MULTIPLY_BY_2POW30;
      }
      g_GlideVertices[0][GLIDE_VERTEX_PROJECTED_TEXTURE_U] =
           Glide_FloatBits((float)((double)currentPacket->vertices[0].textureU *
                                   (double)Glide_BitsToFloat(g_GlideVertices[0][GLIDE_VERTEX_PERSPECTIVE_SCALE])));
      g_GlideVertices[0][GLIDE_VERTEX_PROJECTED_TEXTURE_V] =
           Glide_FloatBits((float)((double)currentPacket->vertices[0].textureV *
                                   (double)Glide_BitsToFloat(g_GlideVertices[0][GLIDE_VERTEX_PERSPECTIVE_SCALE])));
      if (g_GlideVertices[0][GLIDE_VERTEX_PROJECTED_TEXTURE_U] != 0) {
        g_GlideVertices[0][GLIDE_VERTEX_PROJECTED_TEXTURE_U] =
             g_GlideVertices[0][GLIDE_VERTEX_PROJECTED_TEXTURE_U] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      if (g_GlideVertices[0][GLIDE_VERTEX_PROJECTED_TEXTURE_V] != 0) {
        g_GlideVertices[0][GLIDE_VERTEX_PROJECTED_TEXTURE_V] =
             g_GlideVertices[0][GLIDE_VERTEX_PROJECTED_TEXTURE_V] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      g_GlideVertices[1][GLIDE_VERTEX_PROJECTED_TEXTURE_U] =
           Glide_FloatBits((float)((double)currentPacket->vertices[1].textureU *
                                   (double)Glide_BitsToFloat(g_GlideVertices[1][GLIDE_VERTEX_PERSPECTIVE_SCALE])));
      g_GlideVertices[1][GLIDE_VERTEX_PROJECTED_TEXTURE_V] =
           Glide_FloatBits((float)((double)currentPacket->vertices[1].textureV *
                                   (double)Glide_BitsToFloat(g_GlideVertices[1][GLIDE_VERTEX_PERSPECTIVE_SCALE])));
      if (g_GlideVertices[1][GLIDE_VERTEX_PROJECTED_TEXTURE_U] != 0) {
        g_GlideVertices[1][GLIDE_VERTEX_PROJECTED_TEXTURE_U] =
             g_GlideVertices[1][GLIDE_VERTEX_PROJECTED_TEXTURE_U] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      if (g_GlideVertices[1][GLIDE_VERTEX_PROJECTED_TEXTURE_V] != 0) {
        g_GlideVertices[1][GLIDE_VERTEX_PROJECTED_TEXTURE_V] =
             g_GlideVertices[1][GLIDE_VERTEX_PROJECTED_TEXTURE_V] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      g_GlideVertices[2][GLIDE_VERTEX_PROJECTED_TEXTURE_U] =
           Glide_FloatBits((float)((double)currentPacket->vertices[2].textureU *
                                   (double)Glide_BitsToFloat(g_GlideVertices[2][GLIDE_VERTEX_PERSPECTIVE_SCALE])));
      g_GlideVertices[2][GLIDE_VERTEX_PROJECTED_TEXTURE_V] =
           Glide_FloatBits((float)((double)currentPacket->vertices[2].textureV *
                                   (double)Glide_BitsToFloat(g_GlideVertices[2][GLIDE_VERTEX_PERSPECTIVE_SCALE])));
      if (g_GlideVertices[2][GLIDE_VERTEX_PROJECTED_TEXTURE_U] != 0) {
        g_GlideVertices[2][GLIDE_VERTEX_PROJECTED_TEXTURE_U] =
             g_GlideVertices[2][GLIDE_VERTEX_PROJECTED_TEXTURE_U] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      if (g_GlideVertices[2][GLIDE_VERTEX_PROJECTED_TEXTURE_V] != 0) {
        g_GlideVertices[2][GLIDE_VERTEX_PROJECTED_TEXTURE_V] =
             g_GlideVertices[2][GLIDE_VERTEX_PROJECTED_TEXTURE_V] + GLIDE_FLOAT_BITS_DIVIDE_BY_4096;
      }
      renderFlags = currentPacket->renderFlags;
      if (((renderFlags & GRAPHICS_PRIMITIVE_FLAG_TEXTURED) == 0) || (currentPacket->textureEntry == NULL)) {
        if (g_GlideTexturingDisabledState != 0) {
          /* Untextured color and alpha combine. */
          GlideImport_grColorCombine(GR_COMBINE_FUNCTION_LOCAL,GR_COMBINE_FACTOR_ZERO,GR_COMBINE_LOCAL_ITERATED,
                           GR_COMBINE_OTHER_CONSTANT,FXFALSE);
          GlideImport_grAlphaCombine(GR_COMBINE_FUNCTION_LOCAL,GR_COMBINE_FACTOR_ZERO,GR_COMBINE_LOCAL_ITERATED,
                           GR_COMBINE_OTHER_CONSTANT,FXFALSE);
        }
      }
      else {
        texture = currentPacket->textureEntry->texture;
        if ((int)texture->residentTmuIndex < 0) {
          Glide3_TextureResource_EnsureResident(texture);
        }
        if ((int)texture->residentTmuIndex < 0) {
          /* Texture could not be made resident: untextured color and alpha combine. */
          GlideImport_grColorCombine(GR_COMBINE_FUNCTION_LOCAL,GR_COMBINE_FACTOR_ZERO,GR_COMBINE_LOCAL_ITERATED,
                           GR_COMBINE_OTHER_CONSTANT,FXFALSE);
          GlideImport_grAlphaCombine(GR_COMBINE_FUNCTION_LOCAL,GR_COMBINE_FACTOR_ZERO,GR_COMBINE_LOCAL_ITERATED,
                           GR_COMBINE_OTHER_CONSTANT,FXFALSE);
        }
        else {
          if (g_GlideBoundTexture != texture) {
            g_GlideBoundTexture = texture;
            GlideImport_grTexSource(texture->residentTmuIndex,texture->residentAddress,GR_MIPMAPLEVELMASK_BOTH,
                             &texture->glideInfo);
          }
          if (g_GlideTexturingDisabledState == 0) {
            GlideImport_grColorCombine(GR_COMBINE_FUNCTION_SCALE_OTHER,GR_COMBINE_FACTOR_LOCAL,GR_COMBINE_LOCAL_ITERATED,
                             GR_COMBINE_OTHER_TEXTURE,FXFALSE);
            GlideImport_grAlphaCombine(GR_COMBINE_FUNCTION_SCALE_OTHER,GR_COMBINE_FACTOR_LOCAL,GR_COMBINE_LOCAL_ITERATED,
                             GR_COMBINE_OTHER_TEXTURE,FXFALSE);
          }
        }
      }
      if ((renderFlags & GRAPHICS_PRIMITIVE_FLAG_FORCE_TRANSLUCENT) == 0) {
        blendMode = renderFlags & GRAPHICS_PRIMITIVE_BLEND_MASK;
      }
      else {
        blendMode = GRAPHICS_PRIMITIVE_BLEND_TRANSLUCENT;
      }
      if (blendMode == GRAPHICS_PRIMITIVE_BLEND_ADDITIVE) {
        if (g_GlideBlendModeState != 1) {
          GlideImport_grAlphaBlendFunction(GR_BLEND_ONE,GR_BLEND_ONE,GR_BLEND_ONE,GR_BLEND_ZERO);
          g_GlideBlendModeState = 1;
        }
      }
      else if (blendMode == GRAPHICS_PRIMITIVE_BLEND_OPAQUE) {
        if (g_GlideBlendModeState != 2) {
          GlideImport_grAlphaBlendFunction(GR_BLEND_ONE,GR_BLEND_ZERO,GR_BLEND_ONE,GR_BLEND_ZERO);
          g_GlideBlendModeState = 2;
        }
      }
      else if (g_GlideBlendModeState != 0) {
        GlideImport_grAlphaBlendFunction(GR_BLEND_SRC_ALPHA,GR_BLEND_ONE_MINUS_SRC_ALPHA,GR_BLEND_ONE,GR_BLEND_ZERO);
        g_GlideBlendModeState = 0;
      }
      if ((blendMode == GRAPHICS_PRIMITIVE_BLEND_ADDITIVE) ||
          (blendMode == GRAPHICS_PRIMITIVE_BLEND_TRANSLUCENT)) {
        if (g_GlideDepthWriteEnabledState != 0) {
          GlideImport_grDepthMask(FXFALSE);
          g_GlideDepthWriteEnabledState = 0;
        }
      }
      else if (g_GlideDepthWriteEnabledState == 0) {
        GlideImport_grDepthMask(FXTRUE);
        g_GlideDepthWriteEnabledState = 1;
      }
      GlideImport_grDrawTriangle(g_GlideVertices[2],g_GlideVertices[1],g_GlideVertices[0]);
      g_PrimitiveDrawCallCount++;
      currentPacket = GraphicsPrimitiveQueue_Next(queue);
    }
    g_GraphicsBackendAccessState = 0;
  }
  return;
}


/* Address: 0x005802F0.
   Glide backend of texture-set destruction: removes every resource of the set from the texture slot table,
   releases its Glide data and frees it, then frees the set metadata and returns the source asset it owned
   (NULL for a NULL set). The generic wrapper passes the set in EBX and on the stack; the EBX copy is used.
*/
GraphicsTextureSourceAsset * Glide3_TextureSet_DestroyBackend(GraphicsTextureSet *setRegisterMirror,
          GraphicsTextureSet *set)

{
  GraphicsTextureResource *texture;
  GraphicsTextureResource **slotCursor;
  GraphicsTextureSourceAsset *returnedSourceAsset;
  int slotsRemaining;
  uint32_t remainingEntries;
  GraphicsTextureSetEntry *entryCursor;
  GraphicsTextureResource **matchedSlot;

  returnedSourceAsset = NULL;
  if (setRegisterMirror != NULL) {
    remainingEntries = setRegisterMirror->subresourceCount;
    entryCursor = setRegisterMirror->entries;
    do {
      texture = entryCursor->texture;
      if (texture != NULL) {
        slotsRemaining = GRAPHICS_TEXTURE_SLOT_CAPACITY;
        slotCursor = g_GraphicsTextureSlots;
        do {
          matchedSlot = slotCursor;
          if (texture == *matchedSlot) break;
          slotsRemaining--;
          slotCursor = matchedSlot + 1;
        } while (slotsRemaining != 0);
        /* when the texture is not registered, the last slot is cleared (as in the original) */
        *matchedSlot = NULL;
        Glide3_TextureResource_Release(texture);
        g_MemoryApi.free(texture);
      }
      entryCursor++;
      remainingEntries = remainingEntries - 1;
    } while (remainingEntries != 0);
    returnedSourceAsset = GraphicsTextureSet_FreeMetadata(set);
  }
  return returnedSourceAsset;
}


/* Swaps the current cursor record (saved background, visibility token, draw position) with the alternate one. */
static void Glide3_Cursor_SwapWithAlternate(void)

{
  SoftwareFramebufferAccess *savedBackground;
  int32_t visibilityToken;
  int32_t drawX;
  int32_t drawY;

  savedBackground = (SoftwareFramebufferAccess *)(uintptr_t)
       THANDOR_ATOMIC_EXCHANGE(&g_CursorSavedBackground,g_CursorAlternateSavedBackground);
  visibilityToken =
       (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_CursorCurrentVisibilityToken,g_CursorAlternateVisibilityToken);
  drawX = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_CursorCurrentDrawX,g_CursorAlternateDrawX);
  drawY = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_CursorCurrentDrawY,g_CursorAlternateDrawY);
  g_CursorAlternateSavedBackground = savedBackground;
  g_CursorAlternateVisibilityToken = visibilityToken;
  g_CursorAlternateDrawX = drawX;
  g_CursorAlternateDrawY = drawY;
}


/* Address: 0x00580470.
   Glide backend of the frame present (framebuffer is unused): swaps the current and alternate cursor records,
   draws the cursor into the back buffer and, unless the framebuffer is locked (g_GraphicsBackendAccessState),
   waits for Glide and swaps buffers on the next vertical retrace. The cursor records are swapped back afterwards,
   so the cursor drawn for this frame is tracked in the alternate record.
*/
void Glide3_Framebuffer_Present(SoftwareFramebufferAccess *framebuffer)

{
  int32_t previousAccessState;

  Glide3_Cursor_SwapWithAlternate();
  Glide3_Cursor_ComposeBeforePresent(GLIDE_CURSOR_PRESENT_SENTINEL);
  previousAccessState = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_GraphicsBackendAccessState,1);
  if (previousAccessState == 0) {
    GlideImport_grFinish();
    GlideImport_grBufferSwap(1); /* swap interval: one vertical retrace */
    g_GraphicsBackendAccessState--;
  }
  Glide3_Cursor_SwapWithAlternate();
  return;
}


/* Adds a 16-bit display mode for the adapter being enumerated (index g_GraphicsAdapterCount) for every queried
   resolution of 640x480 and up, until the display mode table is full. */
static void Glide3_AddDisplayModesForAdapter(const GrResolution *resolutionList,uint32_t resolutionCount)

{
  const GrResolution *resolutionCursor;
  GraphicsDisplayMode *displayMode;
  FrontendDisplayDimensionPixels modeWidth;
  FrontendDisplayDimensionPixels modeHeight;
  uint32_t remainingResolutions;

  displayMode = g_GraphicsDisplayModes + g_GraphicsDisplayModeCount;
  resolutionCursor = resolutionList;
  for (remainingResolutions = resolutionCount;
       (remainingResolutions != 0) && (g_GraphicsDisplayModeCount <= GRAPHICS_DISPLAY_MODE_CAPACITY - 1);
       remainingResolutions--) {
    switch (resolutionCursor->resolution) {
    case GR_RESOLUTION_640x480:
      modeWidth = 640;
      modeHeight = 480;
      break;
    case GR_RESOLUTION_800x600:
      modeWidth = 800;
      modeHeight = 600;
      break;
    case GR_RESOLUTION_960x720:
      modeWidth = 960;
      modeHeight = 720;
      break;
    case GR_RESOLUTION_1024x768:
      modeWidth = 1024;
      modeHeight = 768;
      break;
    case GR_RESOLUTION_1280x1024:
      modeWidth = 1280;
      modeHeight = 1024;
      break;
    case GR_RESOLUTION_1600x1200:
      modeWidth = 1600;
      modeHeight = 1200;
      break;
    default:
      /* Resolution not offered: skip it. */
      modeWidth = 0;
      modeHeight = 0;
      break;
    }
    if (modeWidth != 0) {
      displayMode->width = modeWidth;
      displayMode->height = modeHeight;
      displayMode->bitsPerPixel = 16;
      displayMode->adapterIndex = g_GraphicsAdapterCount;
      g_GraphicsDisplayModeCount++;
      displayMode = displayMode + 1;
    }
    resolutionCursor++;
  }
}


/* Address: 0x0057EE90.
   Loads glide3x.dll, binds its entry points and adds every 3dfx board as an adapter (GUID Data1 =
   GRAPHICS_ADAPTER_GUID_GLIDE, board index in Data2/Data3) with all its 16-bit resolutions of 640x480 and up
   as display modes, then unloads the DLL again. Returns 0 on success, or the DLL error code (never 0) when the
   DLL or one of its procedures is missing.
*/
uint32_t Glide3_InitAndEnumerate(void)

{
  uint8_t *boardName;
  uint8_t *driverDescription;
  uint32_t resolutionQuerySize;
  GrResolution *resolutionList;
  GlideImportBinding *binding;
  GraphicsAdapterRecord *adapter;
  HINSTANCE glideDll;
  uint32_t resolveError;
  uint32_t resolutionAllocError;
  uint32_t sstIndex;
  int remainingBoards;

  glideDll = DynDLL_Load(sz_GLIDE3X);
  if (glideDll == NULL) {
    return FATAL_ERROR_DLL_LOAD_FAILED;
  }
  binding = g_GlideImportBindings;
  do {
    resolveError = DynAPI_Resolve(&binding->procedure,glideDll,binding->importName);
    if (resolveError != 0) {
      DynDLL_Unload(sz_GLIDE3X);
      return resolveError;
    }
    binding = binding + 1;
  } while (binding->importName != NULL);
  GlideImport_grGet(GR_NUM_BOARDS,sizeof remainingBoards,&remainingBoards);
  sstIndex = 0;
  while ((remainingBoards != 0) && (g_GraphicsAdapterCount <= GRAPHICS_ADAPTER_CAPACITY - 1)) {
    GlideImport_grGlideInit();
    GlideImport_grSstSelect(sstIndex);
    boardName = (uint8_t *)GlideImport_grGetString(GR_RENDERER);
    driverDescription = (uint8_t *)GlideImport_grGetString(GR_HARDWARE);
    adapter = g_GraphicsAdapters + g_GraphicsAdapterCount;
    Text_CopyNarrowToUtf16(40,adapter->driverDescriptionUtf16,driverDescription);
    Text_CopyNarrowToUtf16(40,adapter->deviceNameUtf16,boardName);
    (adapter->adapterGuid).Data1 = GRAPHICS_ADAPTER_GUID_GLIDE;
    (adapter->adapterGuid).Data2 = (uint16_t)sstIndex;
    (adapter->adapterGuid).Data3 = (uint16_t)(sstIndex >> 16);
    (adapter->deviceGuid).Data1 = 1; /* nonzero: the adapter renders in 3D */
    resolutionQuerySize = GlideImport_grQueryResolutions(&g_GlideEnumerationResolutionQuery,NULL);
    if (resolutionQuerySize != 0) {
      resolutionAllocError = g_MemoryApi.alloc(resolutionQuerySize,(void **)&resolutionList);
      if (resolutionAllocError == 0) {
        GlideImport_grQueryResolutions(&g_GlideEnumerationResolutionQuery,resolutionList);
        Glide3_AddDisplayModesForAdapter(resolutionList,resolutionQuerySize / sizeof(GrResolution));
        g_MemoryApi.free(resolutionList);
      }
    }
    GlideImport_grGlideShutdown();
    g_GraphicsAdapterCount++;
    sstIndex++;
    remainingBoards--;
  }
  DynDLL_Unload(sz_GLIDE3X);
  return 0;
}


/* Address: 0x0057F0D0.
   Glide entry of the begin-scene slot: Glide needs no scene bracket, so it does nothing (all registers and
   flags preserved).
*/
void GlideBackend_BeginSceneNoOp(void)

{
  return;
}


/* Address: 0x0057F0E0.
   Glide entry of the end-scene slot: does nothing, like GlideBackend_BeginSceneNoOp (all registers and flags
   preserved).
*/
void GlideBackend_EndSceneNoOp(void)

{
  return;
}


/* Address: 0x0057F740.
   Glide backend of Graphics_SetViewportAndClearDepth: sets viewport and clip window to the rectangle and
   clears colour, alpha and depth there to 0 (depth writes are switched on first, grBufferClear needs them).
*/
void Glide3_ClearViewport(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX)

{
  if (g_GlideDepthWriteEnabledState == 0) {
    GlideImport_grDepthMask(FXTRUE);
    g_GlideDepthWriteEnabledState = 1;
  }
  GlideImport_grViewport(clipMinX,clipMinY,clipMaxX - clipMinX,clipMaxY - clipMinY);
  GlideImport_grClipWindow(clipMinX,clipMinY,clipMaxX,clipMaxY);
  GlideImport_grBufferClear(0,0,0);
  return;
}


/* Address: 0x00580370.
   Glide backend of the colour refresh of one texture-set entry: rebuilds the Glide upload data with the
   converter for the texture's downsample shift and, when the texture is resident in a TMU, downloads it again.
   The subresource index arrives in ECX (mirrored on the stack), the set as the second stack argument.
*/
void Glide3_TextureSet_RefreshColor(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  GraphicsTextureResource *entryTexture;

  entryTexture = set->entries[subresourceIndex].texture;
  g_GlideTextureColorUpload[entryTexture->downsampleShift](entryTexture);
  if (-1 < (int)entryTexture->residentTmuIndex) {
    GlideImport_grTexDownloadMipMap(entryTexture->residentTmuIndex,entryTexture->residentAddress,GR_MIPMAPLEVELMASK_BOTH,
                          &entryTexture->glideInfo);
    g_TextureDeviceReloadCount++;
  }
  return;
}


/* Address: 0x005803D0.
   Alpha counterpart of Glide3_TextureSet_RefreshColor: rebuilds the upload data with the alpha converter for
   the downsample shift and re-downloads a resident texture. Same register contract.
*/
void Glide3_TextureSet_RefreshAlpha(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  GraphicsTextureResource *entryTexture;

  entryTexture = set->entries[subresourceIndex].texture;
  g_GlideTextureAlphaUpload[entryTexture->downsampleShift](entryTexture);
  if (-1 < (int)entryTexture->residentTmuIndex) {
    GlideImport_grTexDownloadMipMap(entryTexture->residentTmuIndex,entryTexture->residentAddress,GR_MIPMAPLEVELMASK_BOTH,
                          &entryTexture->glideInfo);
    g_TextureDeviceReloadCount++;
  }
  return;
}


/* Address: 0x00580540.
   Glide backend of GraphicsFramebuffer_CaptureRegion (screenshots, captured textures): reads the region of the
   RGB565 back buffer with grLfbReadRegion and returns it as a one-image 'gfx' asset of opaque ARGB8888 pixels
   (same layout as the DirectDraw capture: source entry at GRAPHICS_CAPTURE_SOURCE_ENTRY_OFFSET, pixels at
   GRAPHICS_CAPTURE_PIXELS_OFFSET). Returns the asset, or NULL when the allocation fails.
*/
GraphicsCapturedTextureSourceAsset *Glide3_Framebuffer_CaptureRegion
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX)

{
  uint16_t rgb565Pixel;
  GraphicsCapturedTextureSourceAsset *capturedAsset;
  uint32_t rowStrideBytes;
  uint32_t packedTime;
  uint32_t packedDate;
  int pixelCount;
  int remainingClearDwords;
  int remainingPixels;
  uint16_t *sourcePixelCursor;
  uint32_t *clearCursor;
  uint32_t *argbCursor;
  uint32_t allocError;
  uint16_t *rgb565Staging;

  pixelCount = captureWidth * captureHeight;
  allocError = g_MemoryApi.alloc(pixelCount * 4 + GRAPHICS_CAPTURE_PIXELS_OFFSET,(void **)&capturedAsset);
  if (allocError != 0) {
    return NULL;
  }
  /* the RGB565 read-back goes into the upper half of the pixel area and is widened in place, front to back */
  rgb565Staging = (uint16_t *)((uint8_t *)capturedAsset->argb8888Pixels + pixelCount * 2);
  rowStrideBytes = captureWidth * 2;
  /* dword clear of the header (0x88 dwords = GRAPHICS_CAPTURE_PIXELS_OFFSET) and of the pixel dwords */
  clearCursor = (uint32_t *)capturedAsset;
  for (remainingClearDwords = pixelCount + GRAPHICS_CAPTURE_PIXELS_OFFSET / 4; remainingClearDwords != 0;
       remainingClearDwords--) {
    *clearCursor = 0;
    clearCursor++;
  }
  GlideImport_grFinish();
  GlideImport_grLfbReadRegion(GR_BUFFER_BACKBUFFER,sourceX,sourceY,captureWidth,captureHeight,rowStrideBytes,
                              rgb565Staging);
  (capturedAsset->common).magic = ASSET_MAGIC_GFX;
  /* The original stores EDX after the Glide calls, i.e. whatever glide3x left there; this is the
     allocation size, as the DirectDraw capture stores (the screenshot writer uses it as file size). */
  (capturedAsset->common).allocationSizeBytes = captureWidth * captureHeight * 4 + GRAPHICS_CAPTURE_PIXELS_OFFSET;
  (capturedAsset->common).formatVersion = 1;
  (capturedAsset->common).converterVersion = 0;
  packedTime = g_LocaleGetPackedCurrentTime();
  (capturedAsset->common).buildMetadata.timestamps.timeValue0 = packedTime;
  (capturedAsset->common).buildMetadata.timestamps.timeValue1 = packedTime;
  (capturedAsset->common).buildMetadata.timestamps.timeValue2 = packedTime;
  packedDate = g_LocaleGetPackedCurrentDate();
  (capturedAsset->common).buildMetadata.timestamps.dateValue0 = packedDate;
  (capturedAsset->common).buildMetadata.timestamps.dateValue1 = packedDate;
  (capturedAsset->common).buildMetadata.timestamps.dateValue2 = packedDate;
  g_LocaleCopyDefaultComputerLabelUtf16((capturedAsset->common).buildMetadata.names.producerName);
  g_LocaleCopyDefaultComputerLabelUtf16((capturedAsset->common).buildMetadata.names.sourceName);
  capturedAsset->unusedText[0] = 0;
  (capturedAsset->tableDescriptor).subresourceCount = 1;
  (capturedAsset->tableDescriptor).paletteBankCount = 0;
  (capturedAsset->tableDescriptor).subresourceTableOffset = GRAPHICS_CAPTURE_SOURCE_ENTRY_OFFSET;
  (capturedAsset->sourceEntry).logicalWidth = captureWidth;
  (capturedAsset->sourceEntry).logicalHeight = captureHeight;
  (capturedAsset->sourceEntry).pixelWidth = captureWidth;
  (capturedAsset->sourceEntry).pixelHeight = captureHeight;
  (capturedAsset->sourceEntry).originX = 0;
  (capturedAsset->sourceEntry).originY = 0;
  (capturedAsset->sourceEntry).paletteIndex = -1;
  (capturedAsset->sourceEntry).dataOffset = GRAPHICS_CAPTURE_PIXELS_OFFSET;
  sourcePixelCursor = rgb565Staging;
  argbCursor = capturedAsset->argb8888Pixels;
  remainingPixels = captureWidth * captureHeight;
  do {
    /* RGB565 -> ARGB8888 with alpha 0xff; each channel's top bits are repeated into its low bits (the
       original's SHLD chain) */
    rgb565Pixel = *sourcePixelCursor;
    *argbCursor = (((((rgb565Pixel >> 11 | GLIDE_CAPTURE_ALPHA_ABOVE_RED5) << 3 | (uint32_t)(rgb565Pixel >> 13)) << 6 | (rgb565Pixel & (RGB565_GREEN_MASK | RGB565_BLUE_MASK)) >> 5
                ) << 2 | (rgb565Pixel & (RGB565_GREEN_MASK | RGB565_BLUE_MASK)) >> 9) << 5 | rgb565Pixel & RGB565_BLUE_MASK) << 3 | (rgb565Pixel & RGB565_BLUE_MASK) >> 2;
    sourcePixelCursor++;
    argbCursor++;
    remainingPixels--;
  } while (remainingPixels != 0);
  return capturedAsset;
}


/* Address: 0x005806E0.
   Glide version of GraphicsCursor_RestoreAfterPresent: does nothing, because Glide3_Cursor_ComposeBeforePresent
   draws into the back buffer, which the next frame redraws anyway, and saves no background.
*/
void Glide3_Cursor_RestoreAfterPresentNoOp(IDirectDrawSurface3 *backSurfaceSentinel)

{
  return;
}

/* Address: 0x005807B0.
   Converts the texture's source subresource at full size into its two-byte Glide upload buffer
   (glideInfo.data), as GR_TEXFMT_RGB_565 or GR_TEXFMT_ARGB_4444 (glideInfo.format). Paletted subresources take
   each texel's ARGB8888 colour from their palette bank, direct ones read ARGB8888 pixels. Entry 0 (downsample
   shift 0) of both g_GlideTextureColorUpload and g_GlideTextureAlphaUpload, called through them by
   Glide3_TextureResource_Initialize, Glide3_TextureSet_RefreshColor and Glide3_TextureSet_RefreshAlpha.
*/
void Glide3_TextureUpload_1x(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *asset;
  int sourceWidth;
  int paletteBank;
  uint32_t sourceArgb;
  int subresourceRecordOffset;
  uint8_t *sourceCursor;
  uint16_t *destinationCursor;
  int remainingColumns;
  int remainingRows;
  
  asset = texture->sourceAsset;
  destinationCursor = (texture->glideInfo).data;
  /* the subresource's source record: palette bank (negative: direct ARGB8888 pixels), pixel data offset,
     stored width/height */
  subresourceRecordOffset = GLIDE_RECORD_OFFSET(asset,texture->subresourceIndex);
  sourceWidth = GLIDE_RECORD_INT(asset,subresourceRecordOffset,PIXEL_WIDTH);
  remainingRows = GLIDE_RECORD_INT(asset,subresourceRecordOffset,PIXEL_HEIGHT);
  paletteBank = GLIDE_RECORD_INT(asset,subresourceRecordOffset,PALETTE_INDEX);
  sourceCursor = GLIDE_ASSET_BYTES(asset,GLIDE_RECORD_INT(asset,subresourceRecordOffset,PIXEL_OFFSET));
  remainingColumns = sourceWidth;
  if (paletteBank < 0) {
    if ((texture->glideInfo).format == GR_TEXFMT_RGB_565) {
      do {
        do {
          sourceArgb = *(uint32_t *)sourceCursor;
          /* ARGB8888 -> RGB565: red 15..11, green 10..5, blue 4..0 (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)(((sourceArgb >> 3 & RGB565_BLUE_MASK) << 21) >> 16) |
                            (uint16_t)(((sourceArgb >> 10) << 26) >> 16)) >> 5 |
                    (uint16_t)(((sourceArgb >> 19) << 27) >> 16);
          sourceCursor = sourceCursor + 4;
          destinationCursor++;
          remainingColumns--;
        } while (remainingColumns != 0);
        remainingRows--;
        remainingColumns = sourceWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          sourceArgb = *(uint32_t *)sourceCursor;
          /* ARGB8888 -> ARGB4444: the top four bits of each channel (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)((uint16_t)(((sourceArgb >> 4 & 0xf) << 24) >> 16) |
                                     (uint16_t)(((sourceArgb >> 12) << 28) >> 16)) >> 4 |
                            (uint16_t)(((sourceArgb >> 20) << 28) >> 16)) >> 4 |
                    (uint16_t)(sourceArgb >> 16) & ARGB4444_ALPHA_MASK;
          sourceCursor = sourceCursor + 4;
          destinationCursor++;
          remainingColumns--;
        } while (remainingColumns != 0);
        remainingRows--;
        remainingColumns = sourceWidth;
      } while (remainingRows != 0);
    }
  }
  else {
    /* palette entry n of bank b: an 8-byte record (ARGB8888, then the native pixel) at asset + 0x200 + b * 0x800
       + n * 8 */
    asset = texture->sourceAsset;
    if ((texture->glideInfo).format == GR_TEXFMT_RGB_565) {
      do {
        do {
          sourceArgb = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)*sourceCursor);
          /* ARGB8888 -> RGB565: red 15..11, green 10..5, blue 4..0 (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)(((sourceArgb >> 3) << 27) >> 22) |
                            (uint16_t)(((sourceArgb >> 10) << 26) >> 16)) >> 5 |
                    (uint16_t)(((sourceArgb >> 19) << 27) >> 16);
          sourceCursor++;
          destinationCursor++;
          remainingColumns--;
        } while (remainingColumns != 0);
        remainingRows--;
        remainingColumns = sourceWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          sourceArgb = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)*sourceCursor);
          /* ARGB8888 -> ARGB4444: the top four bits of each channel (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)((uint16_t)(((sourceArgb >> 4) << 28) >> 20) |
                                     (uint16_t)(((sourceArgb >> 12) << 28) >> 16)) >> 4 |
                            (uint16_t)(((sourceArgb >> 20) << 28) >> 16)) >> 4 |
                    (uint16_t)(sourceArgb >> 16) & ARGB4444_ALPHA_MASK;
          sourceCursor++;
          destinationCursor++;
          remainingColumns--;
        } while (remainingColumns != 0);
        remainingRows--;
        remainingColumns = sourceWidth;
      } while (remainingRows != 0);
    }
  }
  return;
}


/* Address: 0x00580960.
   Glide3_TextureUpload_1x at half size: each destination texel is the MMX average of a 2x2 block of source
   colours, packed as GR_TEXFMT_RGB_565 or GR_TEXFMT_ARGB_4444. Entry 1 (downsample shift 1) of both
   g_GlideTextureColorUpload and g_GlideTextureAlphaUpload.
*/
void Glide3_TextureUpload_2x(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *asset;
  int paletteBank;
  uint32_t upperLeftSample;
  uint32_t upperRightSample;
  uint32_t lowerLeftSample;
  uint32_t lowerRightSample;
  uint8_t clampedRed;
  uint8_t clampedAlpha;
  uint32_t destinationWidth;
  int subresourceRecordOffset;
  uint8_t *sourceBytes;
  uint16_t *destinationCursor;
  uint16_t blueAverage;
  uint16_t greenAverage;
  uint16_t redAverage;
  uint16_t alphaAverage;
  uint32_t averagedRgb;
  uint32_t averagedArgb;
  uint32_t remainingColumns;
  uint32_t remainingRows;

  asset = texture->sourceAsset;
  destinationCursor = (texture->glideInfo).data;
  /* the subresource's source record: palette bank (negative: direct ARGB8888 pixels), pixel data offset,
     stored width/height */
  subresourceRecordOffset = GLIDE_RECORD_OFFSET(asset,texture->subresourceIndex);
  paletteBank = GLIDE_RECORD_INT(asset,subresourceRecordOffset,PALETTE_INDEX);
  sourceBytes = (uint8_t *)GLIDE_ASSET_BYTES(asset,GLIDE_RECORD_INT(asset,subresourceRecordOffset,PIXEL_OFFSET));
  destinationWidth = GLIDE_RECORD_UINT(asset,subresourceRecordOffset,PIXEL_WIDTH) >>
          1;
  remainingRows = GLIDE_RECORD_UINT(asset,subresourceRecordOffset,PIXEL_HEIGHT)
              >> 1;
  remainingColumns = destinationWidth;
  if (paletteBank < 0) {
    if ((texture->glideInfo).format == GR_TEXFMT_RGB_565) {
      do {
        do {
          upperLeftSample = *(uint32_t *)sourceBytes;
          upperRightSample = *(uint32_t *)(sourceBytes + 4);
          lowerLeftSample = *(uint32_t *)(sourceBytes + destinationWidth * 8);
          lowerRightSample = *(uint32_t *)(sourceBytes + destinationWidth * 8 + 4);
          blueAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                           lowerRightSample,0);
          greenAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,8);
          redAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                          lowerRightSample,16);
          clampedRed = GLIDE_SATURATE_WORD_TO_BYTE(redAverage);
          averagedRgb = (uint32_t)clampedRed << 16 | (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(greenAverage) << 8 |
                        (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(blueAverage);
          /* ARGB8888 -> RGB565: red 15..11, green 10..5, blue 4..0 (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)(((averagedRgb >> 3 & RGB565_BLUE_MASK) << 21) >> 16) |
                             (uint16_t)(((uint32_t)(averagedRgb >> 10) << 26) >> 16)) >> 5 |
                     (uint16_t)(((uint32_t)(clampedRed >> 3) << 27) >> 16);
          sourceBytes = sourceBytes + 8;
          destinationCursor++;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        /* skip the second source row of the block */
        sourceBytes = sourceBytes + destinationWidth * 8;
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          upperLeftSample = *(uint32_t *)sourceBytes;
          upperRightSample = *(uint32_t *)(sourceBytes + 4);
          lowerLeftSample = *(uint32_t *)(sourceBytes + destinationWidth * 8);
          lowerRightSample = *(uint32_t *)(sourceBytes + destinationWidth * 8 + 4);
          blueAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                           lowerRightSample,0);
          greenAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,8);
          redAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                          lowerRightSample,16);
          alphaAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,24);
          averagedRgb = (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(redAverage) << 16 |
                        (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(greenAverage) << 8 |
                        (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(blueAverage);
          clampedAlpha = GLIDE_SATURATE_WORD_TO_BYTE(alphaAverage);
          averagedArgb = (uint32_t)clampedAlpha << 24 | averagedRgb;
          /* ARGB8888 -> ARGB4444: the top four bits of each channel (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)((uint16_t)((((averagedRgb & 0xf0) >> 4) << 24) >> 16) |
                                      (uint16_t)(((averagedArgb >> 12) << 28) >> 16)) >> 4 |
                             (uint16_t)(((averagedArgb >> 20) << 28) >> 16)) >> 4 |
                     (uint16_t)(((uint32_t)(clampedAlpha >> 4) << 28) >> 16);
          sourceBytes = sourceBytes + 8;
          destinationCursor++;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        /* skip the second source row of the block */
        sourceBytes = sourceBytes + destinationWidth * 8;
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
  }
  else {
    asset = texture->sourceAsset;
    if ((texture->glideInfo).format == GR_TEXFMT_RGB_565) {
      do {
        do {
          upperLeftSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)sourceBytes[0]);
          upperRightSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)sourceBytes[1]);
          lowerLeftSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)sourceBytes[destinationWidth * 2]);
          lowerRightSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)sourceBytes[destinationWidth * 2 + 1]);
          blueAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                           lowerRightSample,0);
          greenAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,8);
          redAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                          lowerRightSample,16);
          clampedRed = GLIDE_SATURATE_WORD_TO_BYTE(redAverage);
          averagedRgb = (uint32_t)clampedRed << 16 | (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(greenAverage) << 8 |
                        (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(blueAverage);
          /* ARGB8888 -> RGB565: red 15..11, green 10..5, blue 4..0 (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)(((uint32_t)(averagedRgb >> 3) << 27) >> 22) |
                             (uint16_t)(((uint32_t)(averagedRgb >> 10) << 26) >> 16)) >> 5 |
                     (uint16_t)(((uint32_t)(clampedRed >> 3) << 27) >> 16);
          sourceBytes = sourceBytes + 2;
          destinationCursor++;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        /* skip the second source row of the block */
        sourceBytes = sourceBytes + destinationWidth * 2;
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          upperLeftSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)sourceBytes[0]);
          upperRightSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)sourceBytes[1]);
          lowerLeftSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)sourceBytes[destinationWidth * 2]);
          lowerRightSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)sourceBytes[destinationWidth * 2 + 1]);
          blueAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                           lowerRightSample,0);
          greenAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,8);
          redAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                          lowerRightSample,16);
          alphaAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,24);
          clampedAlpha = GLIDE_SATURATE_WORD_TO_BYTE(alphaAverage);
          averagedArgb = (uint32_t)clampedAlpha << 24 | (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(redAverage) << 16 |
                         (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(greenAverage) << 8 |
                         (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(blueAverage);
          /* ARGB8888 -> ARGB4444: the top four bits of each channel (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)((uint16_t)(((averagedArgb >> 4) << 28) >> 20) |
                                      (uint16_t)(((averagedArgb >> 12) << 28) >> 16)) >> 4 |
                             (uint16_t)(((averagedArgb >> 20) << 28) >> 16)) >> 4 |
                     (uint16_t)(((uint32_t)(clampedAlpha >> 4) << 28) >> 16);
          sourceBytes = sourceBytes + 2;
          destinationCursor++;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        /* skip the second source row of the block */
        sourceBytes = sourceBytes + destinationWidth * 2;
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
  }
  return;
}


/* Address: 0x00580C60.
   Glide3_TextureUpload_1x at quarter size: each destination texel is the MMX average of four texels of its 4x4
   source block (columns and rows 0 and 2), packed as GR_TEXFMT_RGB_565 or GR_TEXFMT_ARGB_4444. Entry 2
   (downsample shift 2) of both g_GlideTextureColorUpload and g_GlideTextureAlphaUpload.
*/
void Glide3_TextureUpload_4x(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *asset;
  int paletteBank;
  uint32_t upperLeftSample;
  uint32_t upperRightSample;
  uint32_t lowerLeftSample;
  uint32_t lowerRightSample;
  uint8_t clampedRed;
  uint8_t clampedAlpha;
  uint32_t destinationWidth;
  int subresourceRecordOffset;
  uint16_t *sourceCursor; /* steps in 16-bit units; paletted texels are the low byte of each word read */
  uint16_t *destinationCursor;
  uint16_t blueAverage;
  uint16_t greenAverage;
  uint16_t redAverage;
  uint16_t alphaAverage;
  uint32_t averagedRgb;
  uint32_t averagedArgb;
  uint32_t remainingColumns;
  uint32_t remainingRows;
  
  asset = texture->sourceAsset;
  destinationCursor = (texture->glideInfo).data;
  /* the subresource's source record: palette bank (negative: direct ARGB8888 pixels), pixel data offset,
     stored width/height */
  subresourceRecordOffset = GLIDE_RECORD_OFFSET(asset,texture->subresourceIndex);
  paletteBank = GLIDE_RECORD_INT(asset,subresourceRecordOffset,PALETTE_INDEX);
  sourceCursor = (uint16_t *)(GLIDE_ASSET_BYTES(asset,GLIDE_RECORD_INT(asset,subresourceRecordOffset,PIXEL_OFFSET)));
  destinationWidth = GLIDE_RECORD_UINT(asset,subresourceRecordOffset,PIXEL_WIDTH) >>
          2;
  remainingRows = GLIDE_RECORD_UINT(asset,subresourceRecordOffset,PIXEL_HEIGHT)
              >> 2;
  remainingColumns = destinationWidth;
  if (paletteBank < 0) {
    if ((texture->glideInfo).format == GR_TEXFMT_RGB_565) {
      do {
        do {
          upperLeftSample = *(uint32_t *)sourceCursor;
          upperRightSample = *(uint32_t *)(sourceCursor + 4);
          lowerLeftSample = *(uint32_t *)(sourceCursor + destinationWidth * 16);
          lowerRightSample = *(uint32_t *)(sourceCursor + destinationWidth * 16 + 4);
          blueAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                           lowerRightSample,0);
          greenAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,8);
          redAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                          lowerRightSample,16);
          clampedRed = GLIDE_SATURATE_WORD_TO_BYTE(redAverage);
          averagedRgb = (uint32_t)clampedRed << 16 | (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(greenAverage) << 8 |
                        (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(blueAverage);
          /* ARGB8888 -> RGB565: red 15..11, green 10..5, blue 4..0 (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)(((averagedRgb >> 3 & RGB565_BLUE_MASK) << 21) >> 16) |
                             (uint16_t)(((uint32_t)(averagedRgb >> 10) << 26) >> 16)) >> 5 |
                     (uint16_t)(((uint32_t)(clampedRed >> 3) << 27) >> 16);
          sourceCursor = sourceCursor + 8;
          destinationCursor++;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = sourceCursor + destinationWidth * 24; /* skip the other three source rows of the block */
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          upperLeftSample = *(uint32_t *)sourceCursor;
          upperRightSample = *(uint32_t *)(sourceCursor + 4);
          lowerLeftSample = *(uint32_t *)(sourceCursor + destinationWidth * 16);
          lowerRightSample = *(uint32_t *)(sourceCursor + destinationWidth * 16 + 4);
          blueAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                           lowerRightSample,0);
          greenAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,8);
          redAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                          lowerRightSample,16);
          alphaAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,24);
          averagedRgb = (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(redAverage) << 16 |
                        (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(greenAverage) << 8 |
                        (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(blueAverage);
          clampedAlpha = GLIDE_SATURATE_WORD_TO_BYTE(alphaAverage);
          averagedArgb = (uint32_t)clampedAlpha << 24 | averagedRgb;
          /* ARGB8888 -> ARGB4444: the top four bits of each channel (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)((uint16_t)((((averagedRgb & 0xf0) >> 4) << 24) >> 16) |
                                      (uint16_t)(((averagedArgb >> 12) << 28) >> 16)) >> 4 |
                             (uint16_t)(((averagedArgb >> 20) << 28) >> 16)) >> 4 |
                     (uint16_t)(((uint32_t)(clampedAlpha >> 4) << 28) >> 16);
          sourceCursor = sourceCursor + 8;
          destinationCursor++;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = sourceCursor + destinationWidth * 24; /* skip the other three source rows of the block */
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
  }
  else {
    asset = texture->sourceAsset;
    if ((texture->glideInfo).format == GR_TEXFMT_RGB_565) {
      do {
        do {
          upperLeftSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)(uint8_t)*sourceCursor);
          upperRightSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)(uint8_t)sourceCursor[1]);
          lowerLeftSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)(uint8_t)sourceCursor[destinationWidth * 4]);
          lowerRightSample = GLIDE_PALETTE_ARGB(asset,paletteBank,
                                                (uint32_t)(uint8_t)sourceCursor[destinationWidth * 4 + 1]);
          blueAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                           lowerRightSample,0);
          greenAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,8);
          redAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                          lowerRightSample,16);
          clampedRed = GLIDE_SATURATE_WORD_TO_BYTE(redAverage);
          averagedRgb = (uint32_t)clampedRed << 16 | (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(greenAverage) << 8 |
                        (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(blueAverage);
          /* ARGB8888 -> RGB565: red 15..11, green 10..5, blue 4..0 (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)(((uint32_t)(averagedRgb >> 3) << 27) >> 22) |
                             (uint16_t)(((uint32_t)(averagedRgb >> 10) << 26) >> 16)) >> 5 |
                     (uint16_t)(((uint32_t)(clampedRed >> 3) << 27) >> 16);
          sourceCursor = sourceCursor + 2;
          destinationCursor++;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = sourceCursor + destinationWidth * 6; /* skip the other three source rows of the block */
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
    else {
      do {
        do {
          upperLeftSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)(uint8_t)*sourceCursor);
          upperRightSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)(uint8_t)sourceCursor[1]);
          lowerLeftSample = GLIDE_PALETTE_ARGB(asset,paletteBank,(uint32_t)(uint8_t)sourceCursor[destinationWidth * 4]);
          lowerRightSample = GLIDE_PALETTE_ARGB(asset,paletteBank,
                                                (uint32_t)(uint8_t)sourceCursor[destinationWidth * 4 + 1]);
          blueAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                           lowerRightSample,0);
          greenAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,8);
          redAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                          lowerRightSample,16);
          alphaAverage = GLIDE_AVERAGE_FOUR_SAMPLES_CHANNEL(upperLeftSample,upperRightSample,lowerLeftSample,
                                                            lowerRightSample,24);
          clampedAlpha = GLIDE_SATURATE_WORD_TO_BYTE(alphaAverage);
          averagedArgb = (uint32_t)clampedAlpha << 24 | (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(redAverage) << 16 |
                         (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(greenAverage) << 8 |
                         (uint32_t)GLIDE_SATURATE_WORD_TO_BYTE(blueAverage);
          /* ARGB8888 -> ARGB4444: the top four bits of each channel (the original's SHR/SHRD chain) */
          *destinationCursor = (uint16_t)((uint16_t)((uint16_t)(((averagedArgb >> 4) << 28) >> 20) |
                                      (uint16_t)(((averagedArgb >> 12) << 28) >> 16)) >> 4 |
                             (uint16_t)(((averagedArgb >> 20) << 28) >> 16)) >> 4 |
                     (uint16_t)(((uint32_t)(clampedAlpha >> 4) << 28) >> 16);
          sourceCursor = sourceCursor + 2;
          destinationCursor++;
          remainingColumns = remainingColumns - 1;
        } while (remainingColumns != 0);
        sourceCursor = sourceCursor + destinationWidth * 6; /* skip the other three source rows of the block */
        remainingRows = remainingRows - 1;
        remainingColumns = destinationWidth;
      } while (remainingRows != 0);
    }
  }
  return;
}


/* Address: 0x00580F60.
   Fills the texture's full-size Glide upload buffer with 0x0FFF, i.e. GR_TEXFMT_ARGB_4444 white with alpha 0.
   Neither called nor referenced by any table in the original (like the two downsamplers after it).
*/
void GraphicsGlide3_FillTextureDataConstant0FFF(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *asset;
  int sourceWidth;
  int subresourceRecordOffset;
  uint16_t *glideDataCursor;
  uint16_t *nextGlideDataCursor;
  int remainingColumns;
  int remainingRows;
  
  asset = texture->sourceAsset;
  glideDataCursor = (texture->glideInfo).data;
  subresourceRecordOffset = GLIDE_RECORD_OFFSET(asset,texture->subresourceIndex);
  /* width and height of the source entry (see Glide3_TextureUpload_1x) */
  sourceWidth = GLIDE_RECORD_INT(asset,subresourceRecordOffset,PIXEL_WIDTH);
  remainingRows = GLIDE_RECORD_INT(asset,subresourceRecordOffset,PIXEL_HEIGHT);
  remainingColumns = sourceWidth;
  do {
    do {
      nextGlideDataCursor = glideDataCursor + 1;
      *glideDataCursor = ARGB4444_RGB_MASK; /* ARGB4444: alpha 0, white */
      remainingColumns--;
      glideDataCursor = nextGlideDataCursor;
    } while (remainingColumns != 0);
    remainingRows--;
    remainingColumns = sourceWidth;
  } while (remainingRows != 0);
  return;
}

/* Address: 0x00580FF0.
   Half-size conversion of an 8-bit alpha image into GR_TEXFMT_ARGB_4444 white texels: the alpha nibble is the
   average of the 2x2 source block (the sum of the four samples divided by 4 each). Neither called nor referenced
   by any table in the original.
*/
void GraphicsGlide3_DownsampleAlpha8ToWhiteArgb4444(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *asset;
  uint8_t firstAlphaQuarter;
  uint32_t downsampledWidth;
  int subresourceRecordOffset;
  uint16_t *sourcePairCursor;
  uint16_t *glideDataCursor;
  uint32_t remainingColumns;
  uint32_t remainingRows;
  
  asset = texture->sourceAsset;
  glideDataCursor = (texture->glideInfo).data;
  subresourceRecordOffset =
       GLIDE_RECORD_OFFSET(asset,texture->subresourceIndex);
  sourcePairCursor =
       (uint16_t *)
       (GLIDE_ASSET_BYTES(asset,GLIDE_RECORD_INT(asset,subresourceRecordOffset,PIXEL_OFFSET)));
  downsampledWidth =
       GLIDE_RECORD_UINT(asset,subresourceRecordOffset,PIXEL_WIDTH) >> 1;
  remainingRows =
       GLIDE_RECORD_UINT(asset,subresourceRecordOffset,PIXEL_HEIGHT) >> 1;
  remainingColumns = downsampledWidth;
  do {
    do {
      /* high byte: the block average; the OR with 0xFFF keeps only its top nibble as alpha and makes the colour
         white, so the first sample's quarter in the low byte has no effect */
      firstAlphaQuarter = (uint8_t)*sourcePairCursor >> 2;
      *glideDataCursor =
           (uint16_t)((uint32_t)(uint8_t)((uint8_t)(*sourcePairCursor >> 10) + firstAlphaQuarter +
                                 (uint8_t)(sourcePairCursor[downsampledWidth] >> 10) +
                                 ((uint8_t)sourcePairCursor[downsampledWidth] >> 2)) << 8 |
                    (uint32_t)firstAlphaQuarter | ARGB4444_RGB_MASK);
      sourcePairCursor = sourcePairCursor + 1;
      glideDataCursor = glideDataCursor + 1;
      remainingColumns = remainingColumns - 1;
    } while (remainingColumns != 0);
    sourcePairCursor = sourcePairCursor + downsampledWidth; /* skip the second source row */
    remainingRows = remainingRows - 1;
    remainingColumns = downsampledWidth;
  } while (remainingRows != 0);
  return;
}

/* Address: 0x005810A0.
   Like GraphicsGlide3_DownsampleAlpha8ToWhiteArgb4444 for a 32-bit source: the alpha nibble averages bytes 0
   and 2 of a texel and of the texel one source row (8 * downsampledWidth bytes) below. It advances only one texel
   per output texel and two source rows per output row, so it reads just the left half of each row pair.
   Neither called nor referenced by any table in the original.
*/
void GraphicsGlide3_DownsampleAlternateAlphaSamplesToWhiteArgb4444(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *asset;
  uint32_t downsampledWidth;
  int subresourceRecordOffset;
  uint8_t *sourceByteCursor;
  uint16_t *glideDataCursor;
  uint32_t remainingColumns;
  uint32_t remainingRows;
  
  asset = texture->sourceAsset;
  glideDataCursor = (texture->glideInfo).data;
  subresourceRecordOffset =
       GLIDE_RECORD_OFFSET(asset,texture->subresourceIndex);
  sourceByteCursor =
       GLIDE_ASSET_BYTES(asset,GLIDE_RECORD_INT(asset,subresourceRecordOffset,PIXEL_OFFSET));
  downsampledWidth =
       GLIDE_RECORD_UINT(asset,subresourceRecordOffset,PIXEL_WIDTH) >> 1;
  remainingRows =
       GLIDE_RECORD_UINT(asset,subresourceRecordOffset,PIXEL_HEIGHT) >> 1;
  remainingColumns = downsampledWidth;
  do {
    do {
      /* as in the alpha8 variant, only the high byte survives the OR with 0xFFF */
      *glideDataCursor =
           (uint16_t)((uint32_t)(uint8_t)((*sourceByteCursor >> 2) + (sourceByteCursor[2] >> 2) +
                                 (sourceByteCursor[downsampledWidth * 8] >> 2) +
                                 (sourceByteCursor[downsampledWidth * 8 + 2] >> 2)) << 8 |
                    (uint32_t)(uint8_t)(sourceByteCursor[2] >> 2) | ARGB4444_RGB_MASK);
      sourceByteCursor = sourceByteCursor + 4;
      glideDataCursor = glideDataCursor + 1;
      remainingColumns = remainingColumns - 1;
    } while (remainingColumns != 0);
    sourceByteCursor = sourceByteCursor + downsampledWidth * 12;
    remainingRows = remainingRows - 1;
    remainingColumns = downsampledWidth;
  } while (remainingRows != 0);
  return;
}

/* Glide texture-source blits (0x00581150..0x00582D30): each one is the Glide version of the matching
   SoftwareTextureSource_*16 blit (docs/software_raster.md, "Asset layout" and "Common structure"), which it calls
   for any framebuffer other than the locked Glide back buffer g_DisplayFramebufferAccess. Blends read the
   destination pixel at destination + g_GlideSecondBufferOffset (the LFB read lock) and write it at the
   destination. The source record fields and palette entries are read with the GLIDE_RECORD_* and
   GLIDE_PALETTE_* macros (top of this file): GLIDE_PALETTE_ARGB is the entry's ARGB8888 colour,
   GLIDE_PALETTE_NATIVE its +4 dword (native pixel with alpha). */

/* Not an original function: the destination rectangle of a Glide texture-source blit. left/top are the
   unclipped subresource origin on the way in and the clipped origin on the way out. */
typedef struct GlideBlitRect {
  int left;
  int top;
  int width; /* clipped width in pixels */
  int rows;  /* clipped height in rows */
} GlideBlitRect;

/* Not an original function: the clipping every Glide texture-source blit below repeats inline. Clamps the
   pixelWidth x pixelHeight subresource at (rect->left, rect->top) to the Glide back buffer and then to the clip
   rectangle; false when nothing is left to draw (width or height 0, or an inverted rectangle). */
static bool Glide_ClipBlitRect(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,int pixelWidth,int pixelHeight,
          GlideBlitRect *rect)
{
  int left;
  int top;
  GraphicsPixelDimension right;
  GraphicsPixelDimension bottom;

  left = rect->left;
  top = rect->top;
  right = left + pixelWidth;
  bottom = top + pixelHeight;
  if (left < 0) {
    left = 0;
  }
  if (top < 0) {
    top = 0;
  }
  if ((int)g_DisplayFramebufferAccess.width < (int)right) {
    right = g_DisplayFramebufferAccess.width;
  }
  if ((int)g_DisplayFramebufferAccess.height < (int)bottom) {
    bottom = g_DisplayFramebufferAccess.height;
  }
  if (left < clipMinX) {
    left = clipMinX;
  }
  if (top < clipMinY) {
    top = clipMinY;
  }
  if (clipMaxX < (int)right) {
    right = clipMaxX;
  }
  if (clipMaxY < (int)bottom) {
    bottom = clipMaxY;
  }
  rect->left = left;
  rect->top = top;
  rect->width = right - left;
  rect->rows = bottom - top;
  return rect->width != 0 && left <= (int)right && rect->rows != 0 && top <= (int)bottom;
}

/* Address: 0x00581150.
   Source-alpha blit of one subresource at (drawX, drawY) into the Glide back buffer: alpha 0 is skipped, alpha
   255 writes the colour converted to the native pixel, anything between is blended. Paletted texels test and
   write the palette entry's +4 dword and blend +0 (see docs/software_raster.md). Returns with CF clear.
*/
bool Glide3_TextureSource_BlitSourceAlpha(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  int recordOffset;
  int rowSkip;
  int sourceWidth;
  int paletteIndex;
  int rowsRemaining;
  int columnsRemaining;
  GlideBlitRect rect;
  uint16_t framebufferPixel;
  uint32_t sourceArgb;
  uint8_t *sourceIndexCursor;
  uint32_t *sourceArgbCursor;
  uint8_t *destinationCursor;
  uint64_t destinationLanes;
  uint64_t weightedSource;
  uint64_t weightedDestination;
  uint64_t packedPair;

  if (framebuffer != &g_DisplayFramebufferAccess) {
    SoftwareTextureSource_BlitSourceAlpha16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,subresourceIndex,sourceAsset,
               framebuffer);
    return false;
  }
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= (sourceAsset->tableDescriptor).subresourceCount)) {
    return false;
  }
  recordOffset = GLIDE_RECORD_OFFSET(sourceAsset,subresourceIndex);
  rect.left = drawX + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X);
  rect.top = drawY + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y);
  if (GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX) == -1) {
    if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
      return false;
    }
    sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
    destinationCursor = g_DisplayFramebufferAccess.pixels +
              (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
    sourceArgbCursor = (uint32_t *)((uint8_t *)sourceAsset +
                      ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth * 4 +
                      ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) * 4 +
                      GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET));
    rowSkip = g_DisplayFramebufferAccess.width - rect.width;
    for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
      for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
        sourceArgb = *sourceArgbCursor;
        if (ARGB8888_RGB_MASK < sourceArgb) {
          if (sourceArgb < ARGB8888_ALPHA_MASK) {
            framebufferPixel = *(uint16_t *)(destinationCursor + g_GlideSecondBufferOffset);
            destinationLanes = Glide_PackWordLanes(framebufferPixel,framebufferPixel,framebufferPixel,
                                                   framebufferPixel) &
                     GLIDE_PACKED_PIXEL_MASKS;
            weightedSource =
                 pmulhw(Glide_UnpackArgbToWordLanes(sourceArgb,2),
                        g_SoftwareBlendAlphaFactors[sourceArgb >> 24]);
            weightedDestination =
                 pmulhw(GLIDE_UNPACK_NATIVE_LANES(destinationLanes),
                        g_SoftwareBlendInverseAlphaFactors[sourceArgb >> 24]);
            packedPair =
                 pmaddwd(GLIDE_ADD_WORD_LANES(weightedDestination,weightedSource) & GLIDE_QUANTIZE_MASKS,
                         g_SoftwarePixelMmxConstants.packWeights);
            *(short *)destinationCursor = (short)(packedPair >> 8) + (short)(packedPair >> 40);
          }
          else {
            *(short *)destinationCursor =
                 (short)g_SoftwarePixelPackTables->blue[sourceArgb & ARGB8888_BLUE_MASK] +
                 (short)*(uint32_t *)
                         ((uint8_t *)g_SoftwarePixelPackTables->green + ((sourceArgb & ARGB8888_GREEN_MASK) >> 6)) +
                 (short)*(uint32_t *)
                         ((uint8_t *)g_SoftwarePixelPackTables->red + ((sourceArgb & ARGB8888_RED_MASK) >> 14));
          }
        }
        sourceArgbCursor++;
        destinationCursor = destinationCursor + 2;
      }
      sourceArgbCursor = sourceArgbCursor + (sourceWidth - rect.width);
      destinationCursor = destinationCursor + rowSkip * 2;
    }
  }
  else if (GLIDE_RECORD_UINT(sourceAsset,recordOffset,PALETTE_INDEX) <
           (sourceAsset->tableDescriptor).paletteBankCount) {
    if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
      return false;
    }
    paletteIndex = GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX);
    sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
    destinationCursor = g_DisplayFramebufferAccess.pixels +
              (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
    sourceIndexCursor = (uint8_t *)sourceAsset +
                      ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth +
                      ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) +
                      GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET);
    rowSkip = g_DisplayFramebufferAccess.width - rect.width;
    for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
      for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
        sourceArgb = GLIDE_PALETTE_NATIVE(sourceAsset,paletteIndex,(uint32_t)*sourceIndexCursor);
        if (ARGB8888_RGB_MASK < sourceArgb) {
          if (sourceArgb < ARGB8888_ALPHA_MASK) {
            sourceArgb = GLIDE_PALETTE_ARGB(sourceAsset,paletteIndex,(uint32_t)*sourceIndexCursor);
            framebufferPixel = *(uint16_t *)(destinationCursor + g_GlideSecondBufferOffset);
            destinationLanes = Glide_PackWordLanes(framebufferPixel,framebufferPixel,framebufferPixel,
                                                   framebufferPixel) &
                     GLIDE_PACKED_PIXEL_MASKS;
            weightedSource =
                 pmulhw(Glide_UnpackArgbToWordLanes(sourceArgb,2),
                        g_SoftwareBlendAlphaFactors[sourceArgb >> 24]);
            weightedDestination =
                 pmulhw(GLIDE_UNPACK_NATIVE_LANES(destinationLanes),
                        g_SoftwareBlendInverseAlphaFactors[sourceArgb >> 24]);
            packedPair =
                 pmaddwd(GLIDE_ADD_WORD_LANES(weightedDestination,weightedSource) & GLIDE_QUANTIZE_MASKS,
                         g_SoftwarePixelMmxConstants.packWeights);
            *(short *)destinationCursor = (short)(packedPair >> 8) + (short)(packedPair >> 40);
          }
          else {
            *(short *)destinationCursor = (short)sourceArgb;
          }
        }
        sourceIndexCursor++;
        destinationCursor = destinationCursor + 2;
      }
      sourceIndexCursor = sourceIndexCursor + (sourceWidth - rect.width);
      destinationCursor = destinationCursor + rowSkip * 2;
    }
  }
  return false;
}


/* Address: 0x005814D0.
   Like Glide3_TextureSource_BlitSourceAlpha, but the source colour enters the blend at half strength (lanes
   >> 3 instead of >> 2) and there is no opaque shortcut: every texel with alpha != 0 is blended, alpha 255
   included. Returns with CF clear.
*/
bool Glide3_TextureSource_BlitHalfSourceRgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  int recordOffset;
  int rowSkip;
  int sourceWidth;
  int paletteIndex;
  int rowsRemaining;
  int columnsRemaining;
  GlideBlitRect rect;
  uint16_t framebufferPixel;
  uint32_t sourceArgb;
  uint8_t *sourceIndexCursor;
  uint32_t *sourceArgbCursor;
  uint8_t *destinationCursor;
  uint64_t destinationLanes;
  uint64_t weightedSource;
  uint64_t weightedDestination;
  uint64_t packedPair;

  if (framebuffer != &g_DisplayFramebufferAccess) {
    SoftwareTextureSource_BlitHalfSourceRgb16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,subresourceIndex,sourceAsset,
               framebuffer);
    return false;
  }
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= (sourceAsset->tableDescriptor).subresourceCount)) {
    return false;
  }
  recordOffset = GLIDE_RECORD_OFFSET(sourceAsset,subresourceIndex);
  rect.left = drawX + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X);
  rect.top = drawY + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y);
  if (GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX) == -1) {
    if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
      return false;
    }
    sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
    destinationCursor = g_DisplayFramebufferAccess.pixels +
              (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
    sourceArgbCursor = (uint32_t *)((uint8_t *)sourceAsset +
                      ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth * 4 +
                      ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) * 4 +
                      GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET));
    rowSkip = g_DisplayFramebufferAccess.width - rect.width;
    for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
      for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
        sourceArgb = *sourceArgbCursor;
        if (ARGB8888_RGB_MASK < sourceArgb) {
          framebufferPixel = *(uint16_t *)(destinationCursor + g_GlideSecondBufferOffset);
          destinationLanes = Glide_PackWordLanes(framebufferPixel,framebufferPixel,framebufferPixel,
                                                 framebufferPixel) &
                   GLIDE_PACKED_PIXEL_MASKS;
          weightedSource =
               pmulhw(Glide_UnpackArgbToWordLanes(sourceArgb,3),
                      g_SoftwareBlendAlphaFactors[sourceArgb >> 24]);
          weightedDestination =
               pmulhw(GLIDE_UNPACK_NATIVE_LANES(destinationLanes),
                      g_SoftwareBlendInverseAlphaFactors[sourceArgb >> 24]);
          packedPair =
               pmaddwd(GLIDE_ADD_WORD_LANES(weightedDestination,weightedSource) & GLIDE_QUANTIZE_MASKS,
                       g_SoftwarePixelMmxConstants.packWeights);
          *(short *)destinationCursor = (short)(packedPair >> 8) + (short)(packedPair >> 40);
        }
        sourceArgbCursor++;
        destinationCursor = destinationCursor + 2;
      }
      sourceArgbCursor = sourceArgbCursor + (sourceWidth - rect.width);
      destinationCursor = destinationCursor + rowSkip * 2;
    }
  }
  else if (GLIDE_RECORD_UINT(sourceAsset,recordOffset,PALETTE_INDEX) <
           (sourceAsset->tableDescriptor).paletteBankCount) {
    if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
      return false;
    }
    paletteIndex = GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX);
    sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
    destinationCursor = g_DisplayFramebufferAccess.pixels +
              (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
    sourceIndexCursor = (uint8_t *)sourceAsset +
                      ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth +
                      ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) +
                      GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET);
    rowSkip = g_DisplayFramebufferAccess.width - rect.width;
    for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
      for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
        sourceArgb = GLIDE_PALETTE_ARGB(sourceAsset,paletteIndex,(uint32_t)*sourceIndexCursor);
        if (ARGB8888_RGB_MASK < sourceArgb) {
          framebufferPixel = *(uint16_t *)(destinationCursor + g_GlideSecondBufferOffset);
          destinationLanes = Glide_PackWordLanes(framebufferPixel,framebufferPixel,framebufferPixel,
                                                 framebufferPixel) &
                   GLIDE_PACKED_PIXEL_MASKS;
          weightedSource =
               pmulhw(Glide_UnpackArgbToWordLanes(sourceArgb,3),
                      g_SoftwareBlendAlphaFactors[sourceArgb >> 24]);
          weightedDestination =
               pmulhw(GLIDE_UNPACK_NATIVE_LANES(destinationLanes),
                      g_SoftwareBlendInverseAlphaFactors[sourceArgb >> 24]);
          packedPair =
               pmaddwd(GLIDE_ADD_WORD_LANES(weightedDestination,weightedSource) & GLIDE_QUANTIZE_MASKS,
                       g_SoftwarePixelMmxConstants.packWeights);
          *(short *)destinationCursor = (short)(packedPair >> 8) + (short)(packedPair >> 40);
        }
        sourceIndexCursor++;
        destinationCursor = destinationCursor + 2;
      }
      sourceIndexCursor = sourceIndexCursor + (sourceWidth - rect.width);
      destinationCursor = destinationCursor + rowSkip * 2;
    }
  }
  return false;
}


/* Address: 0x005817F0.
   Glide version of SoftwareTextureSource_StretchDirectColorBilinear16 (the fallback for any other framebuffer):
   draws a direct-colour subresource (paletteIndex -1) scaled bilinearly to destinationWidth x destinationHeight
   at (destinationX, destinationY) of the locked 16-bit Glide back buffer, two pixels per step. Source steps are
   8.8 fixed point, (size - 1) * 256 / (destinationSize - 1); nothing is clipped. Original quirks kept: an odd
   width drops the last column, a width below 2 or a height of 0 loops 2^32 times, and a destination size of 1
   divides by zero.
*/
void Glide3_TextureSource_StretchDirectColorBilinear
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  int lowerRowOffset;
  int recordOffset;
  int sourceStride;
  uint32_t firstUpperLeft;
  uint32_t firstUpperRight;
  uint32_t firstLowerLeft;
  uint32_t firstLowerRight;
  uint32_t secondUpperLeft;
  uint32_t secondUpperRight;
  uint32_t secondLowerLeft;
  uint32_t secondLowerRight;
  uint8_t firstClampedLane0;
  uint8_t secondClampedLane0;
  GraphicsPixelDimension framebufferWidth;
  uint32_t sourceWidthMinusOne;
  uint32_t pairsRemaining;
  int sourceStepX;
  uint32_t sourceHeightMinusOne;
  uint32_t destinationHeightMinusOne;
  uint32_t sourceXFixed;
  uint32_t sourceColumn;
  uint32_t fractionX;
  uint32_t sourceYFixed;
  uint32_t fractionY;
  uint8_t *sourcePixels;
  uint8_t *sourceRowCursor;
  uint8_t *destinationCursor;
  uint16_t firstLane0;
  uint16_t firstLane1;
  uint16_t firstLane2;
  uint16_t firstLane3;
  uint64_t firstUpperLeftWeighted;
  uint64_t firstUpperRowWeighted;
  uint64_t firstShiftedLanes;
  uint64_t firstPackedPair;
  uint64_t firstUpperRightWeighted;
  uint64_t firstLowerLeftWeighted;
  uint64_t firstLowerRowWeighted;
  uint64_t firstLowerRightWeighted;
  uint16_t secondLane0;
  uint16_t secondLane1;
  uint16_t secondLane2;
  uint16_t secondLane3;
  uint64_t secondUpperLeftWeighted;
  uint64_t secondUpperRowWeighted;
  uint64_t secondShiftedLanes;
  uint64_t secondPackedPair;
  uint64_t secondUpperRightWeighted;
  uint64_t secondLowerLeftWeighted;
  uint64_t secondLowerRowWeighted;
  uint64_t secondLowerRightWeighted;
  uint8_t *destinationRowStart;
  uint8_t firstClampedLane1;
  uint8_t firstClampedLane2;
  uint8_t firstClampedLane3;
  uint8_t secondClampedLane1;
  uint8_t secondClampedLane2;
  uint8_t secondClampedLane3;

  framebufferWidth = g_DisplayFramebufferAccess.width;
  if (framebuffer != &g_DisplayFramebufferAccess) {
    SoftwareTextureSource_StretchDirectColorBilinear16
              (destinationHeight,destinationWidth,destinationY,destinationX,subresourceIndex,
               sourceAsset,framebuffer);
    return;
  }
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= (sourceAsset->tableDescriptor).subresourceCount)) {
    return;
  }
  recordOffset = GLIDE_RECORD_OFFSET(sourceAsset,subresourceIndex);
  if ((g_DisplayFramebufferAccess.bytesPerPixel != SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) ||
      (GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX) != -1)) {
    return;
  }
  destinationCursor = g_DisplayFramebufferAccess.pixels +
            (destinationY * g_DisplayFramebufferAccess.width + destinationX) * 2;
  sourceWidthMinusOne = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH) - 1;
  /* MUL (unsigned 64-bit product) then DIV. */
  sourceStepX = (int)((uint64_t)sourceWidthMinusOne * 256 / (uint64_t)(destinationWidth - 1));
  sourceHeightMinusOne = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT) - 1;
  destinationHeightMinusOne = destinationHeight - 1;
  /* asset + dataOffset */
  sourcePixels = GLIDE_ASSET_BYTES(sourceAsset,GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET));
  sourceStride = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH) * 4;
  sourceXFixed = 0;
  sourceYFixed = 0;
  sourceRowCursor = sourcePixels;
  destinationRowStart = destinationCursor;
  /* Both loops test only after a pass (the quirks above: a height of 0 or fewer than two pairs wrap around). */
  do {
    pairsRemaining = destinationWidth >> 1;
    do {
      sourceColumn = sourceXFixed >> 8;
      lowerRowOffset = sourceStride + sourceColumn * 4;
      firstUpperLeft = *(uint32_t *)(sourceRowCursor + sourceColumn * 4);
      firstUpperRight = *(uint32_t *)(sourceRowCursor + sourceColumn * 4 + 4);
      firstLowerLeft = *(uint32_t *)(sourceRowCursor + lowerRowOffset);
      firstLowerRight = *(uint32_t *)(sourceRowCursor + lowerRowOffset + 4);
      sourceColumn = (sourceXFixed + sourceStepX) >> 8;
      lowerRowOffset = sourceStride + sourceColumn * 4;
      secondUpperLeft = *(uint32_t *)(sourceRowCursor + sourceColumn * 4);
      secondUpperRight = *(uint32_t *)(sourceRowCursor + sourceColumn * 4 + 4);
      secondLowerLeft = *(uint32_t *)(sourceRowCursor + lowerRowOffset);
      secondLowerRight = *(uint32_t *)(sourceRowCursor + lowerRowOffset + 4);
      fractionX = sourceXFixed & 0xff;
      fractionY = sourceYFixed & 0xff;
      firstUpperLeftWeighted =
           pmulhw(Glide_UnpackArgbToWordLanes(firstUpperLeft,2),g_SoftwareBilinearInverseFactors[fractionX]);
      firstUpperRightWeighted =
           pmulhw(Glide_UnpackArgbToWordLanes(firstUpperRight,2),g_SoftwareBilinearForwardFactors[fractionX]);
      firstLowerLeftWeighted =
           pmulhw(Glide_UnpackArgbToWordLanes(firstLowerLeft,2),g_SoftwareBilinearInverseFactors[fractionX]);
      firstLowerRightWeighted =
           pmulhw(Glide_UnpackArgbToWordLanes(firstLowerRight,2),g_SoftwareBilinearForwardFactors[fractionX]);
      firstUpperRowWeighted =
           pmulhw(GLIDE_ADD_WORD_LANES(firstUpperLeftWeighted,firstUpperRightWeighted),
                  g_SoftwareBilinearInverseFactors[fractionY]);
      firstLowerRowWeighted =
           pmulhw(GLIDE_ADD_WORD_LANES(firstLowerLeftWeighted,firstLowerRightWeighted),
                  g_SoftwareBilinearForwardFactors[fractionY]);
      fractionX = (sourceXFixed + sourceStepX) & 0xff;
      secondUpperLeftWeighted =
           pmulhw(Glide_UnpackArgbToWordLanes(secondUpperLeft,2),g_SoftwareBilinearInverseFactors[fractionX]);
      secondUpperRightWeighted =
           pmulhw(Glide_UnpackArgbToWordLanes(secondUpperRight,2),g_SoftwareBilinearForwardFactors[fractionX]);
      secondLowerLeftWeighted =
           pmulhw(Glide_UnpackArgbToWordLanes(secondLowerLeft,2),g_SoftwareBilinearInverseFactors[fractionX]);
      secondLowerRightWeighted =
           pmulhw(Glide_UnpackArgbToWordLanes(secondLowerRight,2),g_SoftwareBilinearForwardFactors[fractionX]);
      secondUpperRowWeighted =
           pmulhw(GLIDE_ADD_WORD_LANES(secondUpperLeftWeighted,secondUpperRightWeighted),
                  g_SoftwareBilinearInverseFactors[fractionY]);
      secondLowerRowWeighted =
           pmulhw(GLIDE_ADD_WORD_LANES(secondLowerLeftWeighted,secondLowerRightWeighted),
                  g_SoftwareBilinearForwardFactors[fractionY]);
      firstLane0 = (uint16_t)((short)firstUpperRowWeighted + (short)firstLowerRowWeighted) >> 2;
      firstLane1 = (uint16_t)((short)(firstUpperRowWeighted >> 16) + (short)(firstLowerRowWeighted >> 16)) >> 2;
      firstLane2 = (uint16_t)((short)(firstUpperRowWeighted >> 32) + (short)(firstLowerRowWeighted >> 32)) >> 2;
      firstLane3 = (uint16_t)((short)(firstUpperRowWeighted >> 48) + (short)(firstLowerRowWeighted >> 48)) >> 2;
      secondLane0 = (uint16_t)((short)secondUpperRowWeighted + (short)secondLowerRowWeighted) >> 2;
      secondLane1 = (uint16_t)((short)(secondUpperRowWeighted >> 16) + (short)(secondLowerRowWeighted >> 16)) >> 2;
      secondLane2 = (uint16_t)((short)(secondUpperRowWeighted >> 32) + (short)(secondLowerRowWeighted >> 32)) >> 2;
      secondLane3 = (uint16_t)((short)(secondUpperRowWeighted >> 48) + (short)(secondLowerRowWeighted >> 48)) >> 2;
      firstClampedLane0 = GLIDE_SATURATE_WORD_TO_BYTE(firstLane0);
      firstClampedLane1 = GLIDE_SATURATE_WORD_TO_BYTE(firstLane1);
      firstClampedLane2 = GLIDE_SATURATE_WORD_TO_BYTE(firstLane2);
      firstClampedLane3 = GLIDE_SATURATE_WORD_TO_BYTE(firstLane3);
      secondClampedLane0 = GLIDE_SATURATE_WORD_TO_BYTE(secondLane0);
      secondClampedLane1 = GLIDE_SATURATE_WORD_TO_BYTE(secondLane1);
      secondClampedLane2 = GLIDE_SATURATE_WORD_TO_BYTE(secondLane2);
      secondClampedLane3 = GLIDE_SATURATE_WORD_TO_BYTE(secondLane3);
      firstShiftedLanes = psllw(Glide_PackWordLanes(GLIDE_DUP_BYTE(firstClampedLane3),
                                                    GLIDE_DUP_BYTE(firstClampedLane2),
                                                    GLIDE_DUP_BYTE(firstClampedLane1),
                                                    GLIDE_DUP_BYTE(firstClampedLane0)),4);
      secondShiftedLanes = psllw(Glide_PackWordLanes(GLIDE_DUP_BYTE(secondClampedLane3),
                                                     GLIDE_DUP_BYTE(secondClampedLane2),
                                                     GLIDE_DUP_BYTE(secondClampedLane1),
                                                     GLIDE_DUP_BYTE(secondClampedLane0)),4);
      firstPackedPair =
           pmaddwd(firstShiftedLanes & GLIDE_QUANTIZE_MASKS,g_SoftwarePixelMmxConstants.packWeights);
      secondPackedPair =
           pmaddwd(secondShiftedLanes & GLIDE_QUANTIZE_MASKS,g_SoftwarePixelMmxConstants.packWeights);
      /* Two RGB565 pixels at once: the second one in the high word. */
      *(uint32_t *)destinationCursor =
           (uint32_t)(uint16_t)((short)(secondPackedPair >> 8) + (short)(secondPackedPair >> 40)) << 16 |
           (uint32_t)(uint16_t)((short)(firstPackedPair >> 8) + (short)(firstPackedPair >> 40));
      sourceXFixed = sourceXFixed + sourceStepX * 2;
      destinationCursor = destinationCursor + 4;
      pairsRemaining--;
    } while (pairsRemaining != 0);
    sourceYFixed = sourceYFixed + (int)((uint64_t)sourceHeightMinusOne * 256 /
                                        (uint64_t)destinationHeightMinusOne);
    destinationCursor = destinationRowStart + framebufferWidth * 2;
    sourceRowCursor = sourcePixels + (sourceYFixed >> 8) * sourceStride;
    sourceXFixed = 0;
    destinationHeight--;
    destinationRowStart = destinationCursor;
  } while (destinationHeight != 0);
}


/* Not an original function: the source-alpha blend the Glide blits below repeat inline. Blends the ARGB8888
   colour sourceArgb with the pixel g_GlideSecondBufferOffset bytes after destinationCursor and stores the result
   as the native pixel at destinationCursor. */
static void Glide_BlendSourceAlphaPixel(uint8_t *destinationCursor,uint32_t sourceArgb)

{
  uint16_t framebufferPixel;
  uint64_t destinationLanes;
  uint64_t weightedSource;
  uint64_t weightedDestination;
  uint64_t packedPair;

  framebufferPixel = *(uint16_t *)(destinationCursor + g_GlideSecondBufferOffset);
  destinationLanes = Glide_PackWordLanes(framebufferPixel,framebufferPixel,framebufferPixel,framebufferPixel) &
           GLIDE_PACKED_PIXEL_MASKS;
  weightedSource = pmulhw(Glide_UnpackArgbToWordLanes(sourceArgb,2),g_SoftwareBlendAlphaFactors[sourceArgb >> 24]);
  weightedDestination = pmulhw(GLIDE_UNPACK_NATIVE_LANES(destinationLanes),
                               g_SoftwareBlendInverseAlphaFactors[sourceArgb >> 24]);
  packedPair = pmaddwd(GLIDE_ADD_WORD_LANES(weightedDestination,weightedSource) & GLIDE_QUANTIZE_MASKS,
                       g_SoftwarePixelMmxConstants.packWeights);
  *(short *)destinationCursor = (short)(packedPair >> 8) + (short)(packedPair >> 40);
}

/* Not an original function: one destination row of Glide3_TextureSource_BlitIntegerScaledSourceAlpha for a
   direct ARGB8888 subresource. Every texel covers integerScale pixels starting at destinationX; alpha 0 is
   skipped, alpha 255 writes the packed colour, anything between is blended; pixels outside
   [clipMinX, clipMaxX) are left alone. */
static void Glide_BlitIntegerScaledDirectRow(uint8_t *sourceCursor,int sourceWidth,uint8_t *destinationCursor,
          int destinationX,GraphicsIntegerScale integerScale,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate clipMaxX)

{
  int remainingColumns;
  uint32_t sourceArgb;
  uint32_t packedBlue;
  uint32_t packedGreen;
  uint32_t packedRed;
  GraphicsIntegerScale remainingColumnRepeats;

  remainingColumns = sourceWidth;
  do {
    remainingColumnRepeats = integerScale;
    sourceArgb = *(uint32_t *)sourceCursor;
    if (sourceArgb < ARGB8888_ALPHA_ONE) {
      destinationX = destinationX + integerScale;
      destinationCursor = destinationCursor + integerScale * 2;
    }
    else if (sourceArgb < ARGB8888_ALPHA_MASK) {
      do {
        if ((clipMinX <= destinationX) && (destinationX < clipMaxX)) {
          Glide_BlendSourceAlphaPixel(destinationCursor,sourceArgb);
        }
        destinationX++;
        destinationCursor = destinationCursor + 2;
        remainingColumnRepeats = remainingColumnRepeats - 1;
      } while (remainingColumnRepeats != 0);
    }
    else {
      packedBlue = g_SoftwarePixelPackTables->blue[sourceArgb & ARGB8888_BLUE_MASK];
      packedGreen = *(uint32_t *)
               ((uint8_t *)g_SoftwarePixelPackTables->green + ((sourceArgb & ARGB8888_GREEN_MASK) >> 6));
      packedRed = *(uint32_t *)
               ((uint8_t *)g_SoftwarePixelPackTables->red + ((sourceArgb & ARGB8888_RED_MASK) >> 14));
      do {
        if ((clipMinX <= destinationX) && (destinationX < clipMaxX)) {
          *(short *)destinationCursor = (short)packedBlue + (short)packedGreen + (short)packedRed;
        }
        destinationX++;
        destinationCursor = destinationCursor + 2;
        remainingColumnRepeats = remainingColumnRepeats - 1;
      } while (remainingColumnRepeats != 0);
    }
    remainingColumns--;
    sourceCursor = sourceCursor + 4;
  } while (remainingColumns != 0);
}

/* Not an original function: Glide_BlitIntegerScaledDirectRow for an 8-bit paletted subresource drawn with
   palette bank paletteIndex. The alpha tests and the opaque write use the palette entry's native dword (+4),
   the blend uses its ARGB8888 dword (+0). */
static void Glide_BlitIntegerScaledPalettedRow(GraphicsTextureSourceAsset *sourceAsset,uint32_t paletteIndex,
          uint8_t *sourceCursor,int sourceWidth,uint8_t *destinationCursor,int destinationX,
          GraphicsIntegerScale integerScale,GraphicsScreenCoordinate clipMinX,GraphicsScreenCoordinate clipMaxX)

{
  int remainingColumns;
  uint32_t paletteNative;
  uint32_t paletteArgb;
  GraphicsIntegerScale remainingColumnRepeats;

  remainingColumns = sourceWidth;
  do {
    remainingColumnRepeats = integerScale;
    paletteNative = GLIDE_PALETTE_NATIVE(sourceAsset,paletteIndex,(uint32_t)*sourceCursor);
    if (paletteNative < ARGB8888_ALPHA_ONE) {
      destinationX = destinationX + integerScale;
      destinationCursor = destinationCursor + integerScale * 2;
    }
    else if (paletteNative < ARGB8888_ALPHA_MASK) {
      paletteArgb = GLIDE_PALETTE_ARGB(sourceAsset,paletteIndex,(uint32_t)*sourceCursor);
      do {
        if ((clipMinX <= destinationX) && (destinationX < clipMaxX)) {
          Glide_BlendSourceAlphaPixel(destinationCursor,paletteArgb);
        }
        destinationX++;
        destinationCursor = destinationCursor + 2;
        remainingColumnRepeats = remainingColumnRepeats - 1;
      } while (remainingColumnRepeats != 0);
    }
    else {
      do {
        if ((clipMinX <= destinationX) && (destinationX < clipMaxX)) {
          *(short *)destinationCursor = (short)paletteNative;
        }
        destinationX++;
        destinationCursor = destinationCursor + 2;
        remainingColumnRepeats = remainingColumnRepeats - 1;
      } while (remainingColumnRepeats != 0);
    }
    remainingColumns--;
    sourceCursor++;
  } while (remainingColumns != 0);
}

/* Address: 0x00581A90.
   Source-alpha blit that draws every texel as an integerScale x integerScale block (origin scaled too). The
   image is not clipped as a whole: the clip rectangle is clamped to the back buffer and every written pixel is
   tested against it. Original bug: for any other framebuffer it calls SoftwareTextureSource_BlitHalfSourceRgb16
   (not the integer-scaled software blit) with the argument list shifted by integerScale.
*/
void Glide3_TextureSource_BlitIntegerScaledSourceAlpha
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsIntegerScale integerScale,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  int recordOffset;
  int startX;
  uint32_t paletteIndex;
  bool directColour;
  int sourceWidth;
  int framebufferPitch;
  uint8_t *sourceRow;
  uint8_t *destinationRow;
  int remainingSourceRows;
  GraphicsIntegerScale remainingRowRepeats;

  if (framebuffer != &g_DisplayFramebufferAccess) {
    /* original bug (CALL 0x004A9B20 with ten pushes at 0x00581AAD): wrong target, arguments shifted */
    SoftwareTextureSource_BlitHalfSourceRgb16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,integerScale,
               (GraphicsTextureSourceAsset *)subresourceIndex,
               (SoftwareFramebufferAccess *)sourceAsset);
    return;
  }
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= (sourceAsset->tableDescriptor).subresourceCount)) {
    return;
  }
  recordOffset = GLIDE_RECORD_OFFSET(sourceAsset,subresourceIndex);
  startX = drawX + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X) * integerScale;
  drawY = drawY + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y) * integerScale;
  if (clipMinX < 0) {
    clipMinX = 0;
  }
  if (clipMinY < 0) {
    clipMinY = 0;
  }
  if ((int)g_DisplayFramebufferAccess.width < clipMaxX) {
    clipMaxX = g_DisplayFramebufferAccess.width;
  }
  paletteIndex = GLIDE_RECORD_UINT(sourceAsset,recordOffset,PALETTE_INDEX);
  if ((int)g_DisplayFramebufferAccess.height < clipMaxY) {
    clipMaxY = g_DisplayFramebufferAccess.height;
  }
  directColour = (int)paletteIndex < 0;
  if (!directColour && (paletteIndex >= (sourceAsset->tableDescriptor).paletteBankCount)) {
    return;
  }
  sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
  remainingSourceRows = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT);
  destinationRow = g_DisplayFramebufferAccess.pixels +
            (g_DisplayFramebufferAccess.width * drawY + startX) * 2;
  framebufferPitch = g_DisplayFramebufferAccess.width * 2;
  sourceRow = GLIDE_ASSET_BYTES(sourceAsset,GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET));
  do {
    /* every source row is drawn integerScale times; rows outside [clipMinY, clipMaxY) are skipped */
    remainingRowRepeats = integerScale;
    do {
      if ((clipMinY <= drawY) && (drawY < clipMaxY)) {
        if (directColour) {
          Glide_BlitIntegerScaledDirectRow(sourceRow,sourceWidth,destinationRow,startX,integerScale,clipMinX,
                                           clipMaxX);
        }
        else {
          Glide_BlitIntegerScaledPalettedRow(sourceAsset,paletteIndex,sourceRow,sourceWidth,destinationRow,startX,
                                             integerScale,clipMinX,clipMaxX);
        }
      }
      drawY++;
      destinationRow = destinationRow + framebufferPitch;
      remainingRowRepeats = remainingRowRepeats - 1;
    } while (remainingRowRepeats != 0);
    if (directColour) {
      sourceRow = sourceRow + sourceWidth * 4;
    }
    else {
      sourceRow = sourceRow + sourceWidth;
    }
    remainingSourceRows--;
  } while (remainingSourceRows != 0);
  return;
}


/* Address: 0x00581EF0.
   Glide3_TextureSource_BlitSourceAlpha with a caller-chosen palette: a paletted subresource is drawn with bank
   paletteBankIndex (checked against paletteBankCount after clipping) instead of its own bank, which must still
   be valid. Direct-colour subresources ignore paletteBankIndex.
*/
void Glide3_TextureSource_BlitSourceAlphaPaletteBank
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PaletteBankIndex paletteBankIndex,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  int recordOffset;
  int rowSkip;
  int sourceWidth;
  int rowsRemaining;
  int columnsRemaining;
  GlideBlitRect rect;
  uint32_t sourceArgb;
  uint32_t paletteNative;
  uint8_t *sourceIndexCursor;
  uint32_t *sourceArgbCursor;
  uint8_t *destinationCursor;

  if (framebuffer != &g_DisplayFramebufferAccess) {
    SoftwareTextureSource_BlitSourceAlphaPaletteBank16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,paletteBankIndex,subresourceIndex,
               sourceAsset,framebuffer);
    return;
  }
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= (sourceAsset->tableDescriptor).subresourceCount)) {
    return;
  }
  recordOffset = GLIDE_RECORD_OFFSET(sourceAsset,subresourceIndex);
  rect.left = drawX + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X);
  rect.top = drawY + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y);
  if (GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX) == -1) {
    if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
      return;
    }
    sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
    destinationCursor = g_DisplayFramebufferAccess.pixels +
              (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
    sourceArgbCursor = (uint32_t *)((uint8_t *)sourceAsset +
                      ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth * 4 +
                      ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) * 4 +
                      GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET));
    rowSkip = g_DisplayFramebufferAccess.width - rect.width;
    for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
      for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
        sourceArgb = *sourceArgbCursor;
        if (ARGB8888_RGB_MASK < sourceArgb) {
          if (sourceArgb < ARGB8888_ALPHA_MASK) {
            Glide_BlendSourceAlphaPixel(destinationCursor,sourceArgb);
          }
          else {
            *(short *)destinationCursor =
                 (short)g_SoftwarePixelPackTables->blue[sourceArgb & ARGB8888_BLUE_MASK] +
                 (short)*(uint32_t *)
                         ((uint8_t *)g_SoftwarePixelPackTables->green + ((sourceArgb & ARGB8888_GREEN_MASK) >> 6)) +
                 (short)*(uint32_t *)
                         ((uint8_t *)g_SoftwarePixelPackTables->red + ((sourceArgb & ARGB8888_RED_MASK) >> 14));
          }
        }
        sourceArgbCursor++;
        destinationCursor = destinationCursor + 2;
      }
      sourceArgbCursor = sourceArgbCursor + (sourceWidth - rect.width);
      destinationCursor = destinationCursor + rowSkip * 2;
    }
    return;
  }
  if (GLIDE_RECORD_UINT(sourceAsset,recordOffset,PALETTE_INDEX) >= (sourceAsset->tableDescriptor).paletteBankCount) {
    return;
  }
  if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                          GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                          GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
    return;
  }
  sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
  destinationCursor = g_DisplayFramebufferAccess.pixels +
            (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
  sourceIndexCursor = (uint8_t *)sourceAsset +
                    ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth +
                    ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) +
                    GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET);
  rowSkip = g_DisplayFramebufferAccess.width - rect.width;
  /* the subresource's own bank only has to be valid; the texels are looked up in paletteBankIndex */
  if (paletteBankIndex >= (sourceAsset->tableDescriptor).paletteBankCount) {
    return;
  }
  for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
    for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
      paletteNative = GLIDE_PALETTE_NATIVE(sourceAsset,paletteBankIndex,(uint32_t)*sourceIndexCursor);
      if (ARGB8888_RGB_MASK < paletteNative) {
        if (paletteNative < ARGB8888_ALPHA_MASK) {
          Glide_BlendSourceAlphaPixel(destinationCursor,
                                      GLIDE_PALETTE_ARGB(sourceAsset,paletteBankIndex,(uint32_t)*sourceIndexCursor));
        }
        else {
          *(short *)destinationCursor = (short)paletteNative;
        }
      }
      sourceIndexCursor++;
      destinationCursor = destinationCursor + 2;
    }
    sourceIndexCursor = sourceIndexCursor + (sourceWidth - rect.width);
    destinationCursor = destinationCursor + rowSkip * 2;
  }
  return;
}


/* Address: 0x00582290.
   Additive blit: adds the texel's RGB to the pixel with saturation (PADDUSW). Texels
   with RGB 0 are skipped whatever their alpha; alpha is otherwise ignored. Returns with CF clear.
*/
bool Glide3_TextureSource_BlitSaturatedAddRgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  int recordOffset;
  int rowSkip;
  int sourceWidth;
  int paletteIndex;
  int rowsRemaining;
  int columnsRemaining;
  GlideBlitRect rect;
  uint16_t framebufferPixel;
  uint32_t sourceArgb;
  uint8_t *sourceIndexCursor;
  uint32_t *sourceArgbCursor;
  uint8_t *destinationCursor;
  uint64_t destinationLanes;
  uint64_t saturatedSum;
  uint64_t packedPair;

  if (framebuffer != &g_DisplayFramebufferAccess) {
    SoftwareTextureSource_BlitSaturatedAddRgb16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,subresourceIndex,sourceAsset,
               framebuffer);
    return false;
  }
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= (sourceAsset->tableDescriptor).subresourceCount)) {
    return false;
  }
  recordOffset = GLIDE_RECORD_OFFSET(sourceAsset,subresourceIndex);
  rect.left = drawX + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X);
  rect.top = drawY + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y);
  if (GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX) == -1) {
    if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
      return false;
    }
    sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
    destinationCursor = g_DisplayFramebufferAccess.pixels +
              (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
    sourceArgbCursor = (uint32_t *)((uint8_t *)sourceAsset +
                      ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth * 4 +
                      ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) * 4 +
                      GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET));
    rowSkip = g_DisplayFramebufferAccess.width - rect.width;
    for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
      for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
        sourceArgb = *sourceArgbCursor;
        if ((sourceArgb & ARGB8888_RGB_MASK) != 0) {
          framebufferPixel = *(uint16_t *)(destinationCursor + g_GlideSecondBufferOffset);
          destinationLanes = Glide_PackWordLanes(framebufferPixel,framebufferPixel,framebufferPixel,
                                                 framebufferPixel) &
                   GLIDE_PACKED_PIXEL_MASKS;
          saturatedSum =
               paddusw(Glide_PackWordLanes((short)(destinationLanes >> 48) *
                                           g_SoftwarePixelMmxConstants.unpackScales.zero,
                                           (short)(destinationLanes >> 32) *
                                           g_SoftwarePixelMmxConstants.unpackScales.red,
                                           (short)(destinationLanes >> 16) *
                                           g_SoftwarePixelMmxConstants.unpackScales.green,
                                           (short)destinationLanes *
                                           g_SoftwarePixelMmxConstants.unpackScales.blue),
                       Glide_UnpackArgbToWordLanes(sourceArgb,0));
          packedPair =
               pmaddwd(Glide_PackWordLanes((uint16_t)(saturatedSum >> 52),
                                           (uint16_t)(saturatedSum >> 32) >> 4,
                                           (uint16_t)(saturatedSum >> 16) >> 4,
                                           (uint16_t)saturatedSum >> 4) &
                       GLIDE_QUANTIZE_MASKS,
                       g_SoftwarePixelMmxConstants.packWeights);
          *(short *)destinationCursor = (short)(packedPair >> 8) + (short)(packedPair >> 40);
        }
        sourceArgbCursor++;
        destinationCursor = destinationCursor + 2;
      }
      sourceArgbCursor = sourceArgbCursor + (sourceWidth - rect.width);
      destinationCursor = destinationCursor + rowSkip * 2;
    }
  }
  else if (GLIDE_RECORD_UINT(sourceAsset,recordOffset,PALETTE_INDEX) <
           (sourceAsset->tableDescriptor).paletteBankCount) {
    if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
      return false;
    }
    paletteIndex = GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX);
    sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
    destinationCursor = g_DisplayFramebufferAccess.pixels +
              (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
    sourceIndexCursor = (uint8_t *)sourceAsset +
                      ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth +
                      ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) +
                      GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET);
    rowSkip = g_DisplayFramebufferAccess.width - rect.width;
    for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
      for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
        sourceArgb = GLIDE_PALETTE_ARGB(sourceAsset,paletteIndex,(uint32_t)*sourceIndexCursor);
        if ((sourceArgb & ARGB8888_RGB_MASK) != 0) {
          framebufferPixel = *(uint16_t *)(destinationCursor + g_GlideSecondBufferOffset);
          destinationLanes = Glide_PackWordLanes(framebufferPixel,framebufferPixel,framebufferPixel,
                                                 framebufferPixel) &
                   GLIDE_PACKED_PIXEL_MASKS;
          saturatedSum =
               paddusw(Glide_PackWordLanes((short)(destinationLanes >> 48) *
                                           g_SoftwarePixelMmxConstants.unpackScales.zero,
                                           (short)(destinationLanes >> 32) *
                                           g_SoftwarePixelMmxConstants.unpackScales.red,
                                           (short)(destinationLanes >> 16) *
                                           g_SoftwarePixelMmxConstants.unpackScales.green,
                                           (short)destinationLanes *
                                           g_SoftwarePixelMmxConstants.unpackScales.blue),
                       Glide_UnpackArgbToWordLanes(sourceArgb,0));
          packedPair =
               pmaddwd(Glide_PackWordLanes((uint16_t)(saturatedSum >> 52),
                                           (uint16_t)(saturatedSum >> 32) >> 4,
                                           (uint16_t)(saturatedSum >> 16) >> 4,
                                           (uint16_t)saturatedSum >> 4) &
                       GLIDE_QUANTIZE_MASKS,
                       g_SoftwarePixelMmxConstants.packWeights);
          *(short *)destinationCursor = (short)(packedPair >> 8) + (short)(packedPair >> 40);
        }
        sourceIndexCursor++;
        destinationCursor = destinationCursor + 2;
      }
      sourceIndexCursor = sourceIndexCursor + (sourceWidth - rect.width);
      destinationCursor = destinationCursor + rowSkip * 2;
    }
  }
  return false;
}


/* Address: 0x00582580.
   Glide3_TextureSource_BlitSaturatedAddRgb with the texel's RGB at half strength. Returns with CF clear.
*/
bool Glide3_TextureSource_BlitHalfRgbSaturatedAdd(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  int recordOffset;
  int rowSkip;
  int sourceWidth;
  int paletteIndex;
  int rowsRemaining;
  int columnsRemaining;
  GlideBlitRect rect;
  uint16_t framebufferPixel;
  uint32_t sourceArgb;
  uint8_t *sourceIndexCursor;
  uint32_t *sourceArgbCursor;
  uint8_t *destinationCursor;
  uint64_t destinationLanes;
  uint64_t saturatedSum;
  uint64_t packedPair;

  if (framebuffer != &g_DisplayFramebufferAccess) {
    SoftwareTextureSource_BlitHalfRgbSaturatedAdd16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,subresourceIndex,sourceAsset,
               framebuffer);
    return false;
  }
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= (sourceAsset->tableDescriptor).subresourceCount)) {
    return false;
  }
  recordOffset = GLIDE_RECORD_OFFSET(sourceAsset,subresourceIndex);
  rect.left = drawX + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X);
  rect.top = drawY + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y);
  if (GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX) == -1) {
    if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
      return false;
    }
    sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
    destinationCursor = g_DisplayFramebufferAccess.pixels +
              (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
    sourceArgbCursor = (uint32_t *)((uint8_t *)sourceAsset +
                      ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth * 4 +
                      ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) * 4 +
                      GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET));
    rowSkip = g_DisplayFramebufferAccess.width - rect.width;
    for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
      for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
        sourceArgb = *sourceArgbCursor;
        if ((sourceArgb & ARGB8888_RGB_MASK) != 0) {
          framebufferPixel = *(uint16_t *)(destinationCursor + g_GlideSecondBufferOffset);
          destinationLanes = Glide_PackWordLanes(framebufferPixel,framebufferPixel,framebufferPixel,
                                                 framebufferPixel) &
                   GLIDE_PACKED_PIXEL_MASKS;
          saturatedSum =
               paddusw(Glide_PackWordLanes((short)(destinationLanes >> 48) *
                                           g_SoftwarePixelMmxConstants.unpackScales.zero,
                                           (short)(destinationLanes >> 32) *
                                           g_SoftwarePixelMmxConstants.unpackScales.red,
                                           (short)(destinationLanes >> 16) *
                                           g_SoftwarePixelMmxConstants.unpackScales.green,
                                           (short)destinationLanes *
                                           g_SoftwarePixelMmxConstants.unpackScales.blue),
                       Glide_UnpackArgbToWordLanes(sourceArgb,1));
          packedPair =
               pmaddwd(Glide_PackWordLanes((uint16_t)(saturatedSum >> 52),
                                           (uint16_t)(saturatedSum >> 32) >> 4,
                                           (uint16_t)(saturatedSum >> 16) >> 4,
                                           (uint16_t)saturatedSum >> 4) &
                       GLIDE_QUANTIZE_MASKS,
                       g_SoftwarePixelMmxConstants.packWeights);
          *(short *)destinationCursor = (short)(packedPair >> 8) + (short)(packedPair >> 40);
        }
        sourceArgbCursor++;
        destinationCursor = destinationCursor + 2;
      }
      sourceArgbCursor = sourceArgbCursor + (sourceWidth - rect.width);
      destinationCursor = destinationCursor + rowSkip * 2;
    }
  }
  else if (GLIDE_RECORD_UINT(sourceAsset,recordOffset,PALETTE_INDEX) <
           (sourceAsset->tableDescriptor).paletteBankCount) {
    if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
      return false;
    }
    paletteIndex = GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX);
    sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
    destinationCursor = g_DisplayFramebufferAccess.pixels +
              (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
    sourceIndexCursor = (uint8_t *)sourceAsset +
                      ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth +
                      ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) +
                      GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET);
    rowSkip = g_DisplayFramebufferAccess.width - rect.width;
    for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
      for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
        sourceArgb = GLIDE_PALETTE_ARGB(sourceAsset,paletteIndex,(uint32_t)*sourceIndexCursor);
        if ((sourceArgb & ARGB8888_RGB_MASK) != 0) {
          framebufferPixel = *(uint16_t *)(destinationCursor + g_GlideSecondBufferOffset);
          destinationLanes = Glide_PackWordLanes(framebufferPixel,framebufferPixel,framebufferPixel,
                                                 framebufferPixel) &
                   GLIDE_PACKED_PIXEL_MASKS;
          saturatedSum =
               paddusw(Glide_PackWordLanes((short)(destinationLanes >> 48) *
                                           g_SoftwarePixelMmxConstants.unpackScales.zero,
                                           (short)(destinationLanes >> 32) *
                                           g_SoftwarePixelMmxConstants.unpackScales.red,
                                           (short)(destinationLanes >> 16) *
                                           g_SoftwarePixelMmxConstants.unpackScales.green,
                                           (short)destinationLanes *
                                           g_SoftwarePixelMmxConstants.unpackScales.blue),
                       Glide_UnpackArgbToWordLanes(sourceArgb,1));
          packedPair =
               pmaddwd(Glide_PackWordLanes((uint16_t)(saturatedSum >> 52),
                                           (uint16_t)(saturatedSum >> 32) >> 4,
                                           (uint16_t)(saturatedSum >> 16) >> 4,
                                           (uint16_t)saturatedSum >> 4) &
                       GLIDE_QUANTIZE_MASKS,
                       g_SoftwarePixelMmxConstants.packWeights);
          *(short *)destinationCursor = (short)(packedPair >> 8) + (short)(packedPair >> 40);
        }
        sourceIndexCursor++;
        destinationCursor = destinationCursor + 2;
      }
      sourceIndexCursor = sourceIndexCursor + (sourceWidth - rect.width);
      destinationCursor = destinationCursor + rowSkip * 2;
    }
  }
  return false;
}


/* Address: 0x00582870.
   Glide3_TextureSource_BlitSourceAlpha with a tint: every source channel, alpha included, is first multiplied
   by the matching channel of modulationArgb8888 (/ 256), then the modulated alpha selects skip, write or blend.
   Original bug: for any other framebuffer it calls SoftwareTextureSource_BlitHalfRgbSaturatedAdd16 (CALL
   0x004ABA70 at 0x005828A1) without the modulation colour instead of the modulated software blit. Returns with
   CF clear.
*/
bool Glide3_TextureSource_BlitModulatedSourceAlpha(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PackedArgb32 modulationArgb8888,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  int recordOffset;
  int rowSkip;
  int sourceWidth;
  int paletteIndex;
  int rowsRemaining;
  int columnsRemaining;
  GlideBlitRect rect;
  uint16_t framebufferPixel;
  uint32_t sourceArgb;
  uint32_t modulationGreen;
  uint32_t modulationRed;
  uint32_t blueProduct;
  uint32_t greenProduct;
  uint32_t redProduct;
  uint32_t alphaProduct;
  uint32_t alphaProductHigh;
  uint32_t modulatedBlue;
  uint32_t modulatedAlpha;
  uint32_t modulatedArgb;
  uint8_t *sourceIndexCursor;
  uint32_t *sourceArgbCursor;
  uint8_t *destinationCursor;
  uint64_t destinationLanes;
  uint64_t weightedSource;
  uint64_t weightedDestination;
  uint64_t packedPair;

  if (framebuffer != &g_DisplayFramebufferAccess) {
    /* original bug: wrong software blit, the modulation colour is dropped */
    SoftwareTextureSource_BlitHalfRgbSaturatedAdd16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,drawY,drawX,subresourceIndex,sourceAsset,
               framebuffer);
    return false;
  }
  modulationGreen = (modulationArgb8888 & ARGB8888_GREEN_MASK) >> 8;
  modulationRed = (modulationArgb8888 & ARGB8888_RED_MASK) >> 16;
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= (sourceAsset->tableDescriptor).subresourceCount)) {
    return false;
  }
  recordOffset = GLIDE_RECORD_OFFSET(sourceAsset,subresourceIndex);
  rect.left = drawX + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X);
  rect.top = drawY + GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y);
  if (GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX) == -1) {
    if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
      return false;
    }
    sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
    destinationCursor = g_DisplayFramebufferAccess.pixels +
              (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
    sourceArgbCursor = (uint32_t *)((uint8_t *)sourceAsset +
                      ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth * 4 +
                      ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) * 4 +
                      GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET));
    rowSkip = g_DisplayFramebufferAccess.width - rect.width;
    for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
      for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
        sourceArgb = *sourceArgbCursor;
        blueProduct = (sourceArgb & ARGB8888_BLUE_MASK) * (modulationArgb8888 & ARGB8888_BLUE_MASK);
        alphaProduct = (sourceArgb >> 24) * (modulationArgb8888 >> 24);
        greenProduct = ((sourceArgb & ARGB8888_GREEN_MASK) >> 8) * modulationGreen & 0xff00;
        redProduct = ((sourceArgb & ARGB8888_RED_MASK) >> 16) * modulationRed & 0xff00;
        alphaProductHigh = alphaProduct & 0xff00;
        modulatedBlue = blueProduct >> 8;
        modulatedArgb = modulatedBlue | greenProduct | redProduct << 8 | alphaProductHigh << 16;
        if (ARGB8888_RGB_MASK < modulatedArgb) {
          if (modulatedArgb < ARGB8888_ALPHA_MASK) {
            framebufferPixel = *(uint16_t *)(destinationCursor + g_GlideSecondBufferOffset);
            destinationLanes = Glide_PackWordLanes(framebufferPixel,framebufferPixel,framebufferPixel,
                                                   framebufferPixel) &
                     GLIDE_PACKED_PIXEL_MASKS;
            modulatedAlpha = alphaProduct >> 8;
            /* Lane bytes are the modulated channels alphaProductHigh >> 8, redProduct >> 8,
               greenProduct >> 8 and blueProduct >> 8, i.e. bytes 3..0 of modulatedArgb. */
            weightedSource =
                 pmulhw(Glide_UnpackArgbToWordLanes(modulatedArgb,2),
                        g_SoftwareBlendAlphaFactors[modulatedAlpha]);
            weightedDestination =
                 pmulhw(GLIDE_UNPACK_NATIVE_LANES(destinationLanes),
                        g_SoftwareBlendInverseAlphaFactors[modulatedAlpha]);
            packedPair =
                 pmaddwd(GLIDE_ADD_WORD_LANES(weightedDestination,weightedSource) & GLIDE_QUANTIZE_MASKS,
                         g_SoftwarePixelMmxConstants.packWeights);
            *(short *)destinationCursor = (short)(packedPair >> 8) + (short)(packedPair >> 40);
          }
          else {
            *(short *)destinationCursor =
                 (short)g_SoftwarePixelPackTables->blue[modulatedBlue] +
                 (short)*(uint32_t *)((uint8_t *)g_SoftwarePixelPackTables->green + (greenProduct >> 6)) +
                 (short)*(uint32_t *)((uint8_t *)g_SoftwarePixelPackTables->red + (redProduct >> 6));
          }
        }
        sourceArgbCursor++;
        destinationCursor = destinationCursor + 2;
      }
      sourceArgbCursor = sourceArgbCursor + (sourceWidth - rect.width);
      destinationCursor = destinationCursor + rowSkip * 2;
    }
  }
  else if (GLIDE_RECORD_UINT(sourceAsset,recordOffset,PALETTE_INDEX) <
           (sourceAsset->tableDescriptor).paletteBankCount) {
    if (!Glide_ClipBlitRect(clipMaxY,clipMaxX,clipMinY,clipMinX,
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH),
                            GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_HEIGHT),&rect)) {
      return false;
    }
    paletteIndex = GLIDE_RECORD_INT(sourceAsset,recordOffset,PALETTE_INDEX);
    sourceWidth = GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_WIDTH);
    destinationCursor = g_DisplayFramebufferAccess.pixels +
              (g_DisplayFramebufferAccess.width * rect.top + rect.left) * 2;
    sourceIndexCursor = (uint8_t *)sourceAsset +
                      ((rect.top - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_Y)) - drawY) * sourceWidth +
                      ((rect.left - drawX) - GLIDE_RECORD_INT(sourceAsset,recordOffset,ORIGIN_X)) +
                      GLIDE_RECORD_INT(sourceAsset,recordOffset,PIXEL_OFFSET);
    rowSkip = g_DisplayFramebufferAccess.width - rect.width;
    for (rowsRemaining = rect.rows; rowsRemaining != 0; rowsRemaining--) {
      for (columnsRemaining = rect.width; columnsRemaining != 0; columnsRemaining--) {
        sourceArgb = GLIDE_PALETTE_ARGB(sourceAsset,paletteIndex,(uint32_t)*sourceIndexCursor);
        blueProduct = (sourceArgb & ARGB8888_BLUE_MASK) * (modulationArgb8888 & ARGB8888_BLUE_MASK);
        alphaProduct = (sourceArgb >> 24) * (modulationArgb8888 >> 24);
        greenProduct = ((sourceArgb & ARGB8888_GREEN_MASK) >> 8) * modulationGreen & 0xff00;
        redProduct = ((sourceArgb & ARGB8888_RED_MASK) >> 16) * modulationRed & 0xff00;
        alphaProductHigh = alphaProduct & 0xff00;
        modulatedBlue = blueProduct >> 8;
        modulatedArgb = modulatedBlue | greenProduct | redProduct << 8 | alphaProductHigh << 16;
        if (ARGB8888_RGB_MASK < modulatedArgb) {
          if (modulatedArgb < ARGB8888_ALPHA_MASK) {
            framebufferPixel = *(uint16_t *)(destinationCursor + g_GlideSecondBufferOffset);
            destinationLanes = Glide_PackWordLanes(framebufferPixel,framebufferPixel,framebufferPixel,
                                                   framebufferPixel) &
                     GLIDE_PACKED_PIXEL_MASKS;
            modulatedAlpha = alphaProduct >> 8;
            weightedSource =
                 pmulhw(Glide_UnpackArgbToWordLanes(modulatedArgb,2),
                        g_SoftwareBlendAlphaFactors[modulatedAlpha]);
            weightedDestination =
                 pmulhw(GLIDE_UNPACK_NATIVE_LANES(destinationLanes),
                        g_SoftwareBlendInverseAlphaFactors[modulatedAlpha]);
            packedPair =
                 pmaddwd(GLIDE_ADD_WORD_LANES(weightedDestination,weightedSource) & GLIDE_QUANTIZE_MASKS,
                         g_SoftwarePixelMmxConstants.packWeights);
            *(short *)destinationCursor = (short)(packedPair >> 8) + (short)(packedPair >> 40);
          }
          else {
            *(short *)destinationCursor =
                 (short)g_SoftwarePixelPackTables->blue[modulatedBlue] +
                 (short)*(uint32_t *)((uint8_t *)g_SoftwarePixelPackTables->green + (greenProduct >> 6)) +
                 (short)*(uint32_t *)((uint8_t *)g_SoftwarePixelPackTables->red + (redProduct >> 6));
          }
        }
        sourceIndexCursor++;
        destinationCursor = destinationCursor + 2;
      }
      sourceIndexCursor = sourceIndexCursor + (sourceWidth - rect.width);
      destinationCursor = destinationCursor + rowSkip * 2;
    }
  }
  return false;
}


/* Address: 0x00582D30.
   Glide backend of the framebuffer rectangle fill: on the locked Glide back buffer (g_DisplayFramebufferAccess)
   it fills the intersection of the rectangle, the clip rectangle and the buffer with argb8888, converted to the
   native 16-bit pixel. Alpha 0 draws nothing, alpha 255 writes the pixel, anything between blends it over the
   pixel read back through the LFB read lock (g_GlideSecondBufferOffset). Any other framebuffer goes to
   SoftwareFramebuffer_FillRectArgb16.
*/
void Glide3_Framebuffer_FillRectArgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate rectMaxY,GraphicsScreenCoordinate rectMaxX,
          GraphicsScreenCoordinate rectMinY,GraphicsScreenCoordinate rectMinX,PackedArgb32 argb8888,
          SoftwareFramebufferAccess *framebuffer)

{
  int rowSkipBytes;
  uint32_t fillColor;
  int remainingColumns;
  int remainingRows;
  int clippedWidth;
  int clippedHeight;
  uint8_t *destinationCursor;

  if (framebuffer != &g_DisplayFramebufferAccess) {
    SoftwareFramebuffer_FillRectArgb16
              (clipMaxY,clipMaxX,clipMinY,clipMinX,rectMaxY,rectMaxX,rectMinY,rectMinX,argb8888,
               framebuffer);
    return;
  }
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
  clippedHeight = rectMaxY - rectMinY;
  if ((clippedWidth == 0) || (rectMinX > rectMaxX) || (clippedHeight == 0) || (rectMinY > rectMaxY)) {
    return;
  }
  rowSkipBytes = (g_DisplayFramebufferAccess.width - clippedWidth) * 2;
  destinationCursor = g_DisplayFramebufferAccess.pixels +
           (rectMinY * g_DisplayFramebufferAccess.width + rectMinX) * 2;
  /* native 16-bit pixel in the low word, source alpha in the top byte; the green/red lookups are
     byte offsets (channel * 4) into the pack tables */
  fillColor = g_SoftwarePixelPackTables->blue[argb8888 & ARGB8888_BLUE_MASK] + (argb8888 & ARGB8888_ALPHA_MASK) +
          *(int *)((uint8_t *)g_SoftwarePixelPackTables->green + ((argb8888 & ARGB8888_GREEN_MASK) >> 6)) +
          *(int *)((uint8_t *)g_SoftwarePixelPackTables->red + ((argb8888 & ARGB8888_RED_MASK) >> 14));
  if (fillColor <= ARGB8888_RGB_MASK) { /* alpha 0: nothing to draw */
    return;
  }
  if (fillColor < ARGB8888_ALPHA_MASK) { /* alpha 1..254: blend */
    for (remainingRows = clippedHeight; remainingRows != 0; remainingRows--) {
      for (remainingColumns = clippedWidth; remainingColumns != 0; remainingColumns--) {
        /* original quirk (MOVD MM1,EAX at 0x00582E57): the source lanes are the bytes of the packed
           native pixel in fillColor, not the argb8888 channels */
        Glide_BlendSourceAlphaPixel(destinationCursor,fillColor);
        destinationCursor = destinationCursor + 2;
      }
      destinationCursor = destinationCursor + rowSkipBytes;
    }
    return;
  }
  /* alpha 255: plain fill */
  for (remainingRows = clippedHeight; remainingRows != 0; remainingRows--) {
    for (remainingColumns = clippedWidth; remainingColumns != 0; remainingColumns--) {
      *(short *)destinationCursor = (short)fillColor;
      destinationCursor = destinationCursor + 2;
    }
    destinationCursor = destinationCursor + rowSkipBytes;
  }
  return;
}


/* Address: 0x005806F0.
   Glide version of GraphicsCursor_ComposeBeforePresent: draws the current cursor frame (the pressed image while
   a mouse button is down) straight into the locked Glide back buffer, so there is no background to save or
   restore. Only Glide3_Framebuffer_Present's call (GLIDE_CURSOR_PRESENT_SENTINEL) draws, just before the
   buffer swap; the visibility token is latched on every call.
*/
void Glide3_Cursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurfaceSentinel)

{
  UiPixelOffset cursorHotspotX;
  UiPixelCoordinate cursorX;
  GraphicsSubresourceIndex cursorSubresource;
  GraphicsCursorFrameRecord *frameRecord;
  UiPixelCoordinate cursorY;
  bool accessFailed;
  UiPixelOffset cursorHotspotY;

  g_CursorCurrentVisibilityToken = g_CursorVisibilityToken;
  /* a negative token hides the cursor */
  if ((-1 < g_CursorVisibilityToken) && (backSurfaceSentinel == GLIDE_CURSOR_PRESENT_SENTINEL)) {
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
    if ((g_CursorButtonState & LEFT_MIDDLE_RIGHT) == 0) { /* none of the three mouse buttons is down */
      cursorSubresource = frameRecord->idleSubresourceIndex;
    }
    accessFailed = Glide3_Framebuffer_BeginAccess();
    if (!accessFailed) {
      g_GraphicsTextureSourceBlitSourceAlpha
                (g_FramebufferHeight,g_FramebufferWidth,0,0,cursorY - cursorHotspotY,cursorX - cursorHotspotX,
                 cursorSubresource,g_CursorSourceAsset,g_FramebufferAccess);
      Glide3_Framebuffer_EndAccess();
    }
  }
  return;
}


/* Address: 0x0057F5C0.
   Glide backend shutdown: invalidates both saved cursor backgrounds and, when Glide is active, releases the
   device objects of every registered texture, closes the Glide window, shuts Glide down and unloads the DLL.
*/
void Glide3_Shutdown(void)

{
  int textureSlotsRemaining;
  GraphicsTextureResource **textureSlotCursor;

  g_CursorCurrentVisibilityToken = -1;
  g_CursorAlternateVisibilityToken = -1;
  if (g_GlideRuntimeActiveCount != 0) {
    textureSlotsRemaining = GRAPHICS_TEXTURE_SLOT_CAPACITY;
    textureSlotCursor = g_GraphicsTextureSlots;
    do {
      if (*textureSlotCursor != NULL) {
        GraphicsTexture_ReleaseObjects(*textureSlotCursor);
      }
      textureSlotCursor++;
      textureSlotsRemaining--;
    } while (textureSlotsRemaining != 0);
    GlideImport_grSstWinClose(g_GlideWindowContextHandle);
    GlideImport_grGlideShutdown();
    DynDLL_Unload(sz_GLIDE3X);
    g_GlideRuntimeActiveCount = 0;
  }
  return;
}


/* Address: 0x0057F630.
   Gives the software blitters direct access to the Glide back buffer: takes the backend access flag, locks
   the back buffer for writing as RGB565 and points g_DisplayFramebufferAccess at it, then tries a second,
   read-only lock of the same buffer for blending. CF set (and the access flag left taken) when the backend is
   busy or the write lock fails.
*/
bool Glide3_Framebuffer_BeginAccess(void)

{
  int lfbLockSucceeded;
  int secondaryLfbLockSucceeded;
  int32_t previousAccessState;
  
  previousAccessState = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_GraphicsBackendAccessState,1);
  if (previousAccessState == 0) {
    GlideImport_grFinish();
    lfbLockSucceeded = GlideImport_grLfbLock(GR_LFB_WRITE_ONLY | GR_LFB_NOIDLE,GR_BUFFER_BACKBUFFER,GR_LFBWRITEMODE_565,
                                   GR_ORIGIN_UPPER_LEFT,FXFALSE,&g_GlidePrimaryLfbInfo);
    if (lfbLockSucceeded != 0) {
      g_FramebufferRowStrideBytes = g_GlidePrimaryLfbInfo.strideBytes;
      g_DisplayFramebufferAccess.width = g_GlidePrimaryLfbInfo.strideBytes >> 1; /* 2 bytes per pixel */
      g_DisplayFramebufferAccess.pixels = g_GlidePrimaryLfbInfo.pixels;
      secondaryLfbLockSucceeded = GlideImport_grLfbLock(GR_LFB_READ_ONLY | GR_LFB_NOIDLE,GR_BUFFER_BACKBUFFER,
                                              GR_LFBWRITEMODE_565,GR_ORIGIN_UPPER_LEFT,FXFALSE,
                                              &g_GlideSecondaryLfbInfo);
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
   Ends Glide3_Framebuffer_BeginAccess: drops the read-only lock if it was taken, unlocks the write lock,
   clears g_DisplayFramebufferAccess.pixels and releases the backend access flag.
*/
void Glide3_Framebuffer_EndAccess(void)

{
  if (g_GlideSecondBufferBase != NULL) {
    GlideImport_grLfbUnlock(GR_LFB_READ_ONLY,GR_BUFFER_BACKBUFFER);
    g_GlideSecondBufferOffset = 0;
    g_GlideSecondBufferBase = NULL;
  }
  GlideImport_grLfbUnlock(GR_LFB_WRITE_ONLY,GR_BUFFER_BACKBUFFER);
  g_DisplayFramebufferAccess.pixels = NULL;
  g_GraphicsBackendAccessState = 0;
  return;
}


/* TMU bytes of a texture: pixelWidth * pixelHeight * 2 bytes, divided by 4 per downsample step. */
#define GLIDE_TMU_BYTES(asset,recordOffset,downsampleShift) \
  ((uint32_t)(GLIDE_RECORD_INT(asset,recordOffset,PIXEL_WIDTH) * GLIDE_RECORD_INT(asset,recordOffset,PIXEL_HEIGHT) * \
              2) >> ((char)(downsampleShift) * 2 & (uint32_t)SHIFT_COUNT_MASK))

/* Not an original function: the common tail of every placement in Glide3_TextureResource_EnsureResident. Drops
   the bound texture and downloads the upload buffer to the texture's TMU and address. */
static void Glide3_TextureResource_Download(GraphicsTextureResource *texture)

{
  g_GlideBoundTexture = NULL;
  GlideImport_grTexDownloadMipMap(texture->residentTmuIndex,texture->residentAddress,GR_MIPMAPLEVELMASK_BOTH,
                        &texture->glideInfo);
}

/* Address: 0x0057FF50.
   Makes a texture resident in TMU memory before it is drawn. The resident textures form a list ordered by TMU
   and address that is searched like a ring, starting at the most recently placed texture
   (g_GlideResidentTextureTail, not necessarily the list end). At the list end the texture is appended behind the
   last texture when the rest of that TMU fits it, else at the start of the next TMU; elsewhere it replaces
   (evicts) the following texture on the same TMU when it fits into the distance between the two addresses.
   The upload buffer is then downloaded to the chosen address. Textures without an upload buffer, or with no
   room after a full round, stay not resident.
*/
void Glide3_TextureResource_EnsureResident(GraphicsTextureResource *texture)

{
  GraphicsTextureSourceAsset *asset;
  GraphicsTextureResource *nextResident;
  GraphicsTextureMemoryAddress reclaimedAddress;
  GraphicsTextureResource *followingResident;
  int subresourceRecordOffset;
  uint32_t tailFreeBytes;
  GraphicsTextureResidentTmuIndex tmuIndex;
  GraphicsTextureResource *residentCursor;

  if ((int)texture->residentTmuIndex >= 0) {
    return;
  }
  if ((texture->glideInfo).data == NULL) {
    return;
  }
  residentCursor = g_GlideResidentTextureTail;
  do {
    if (residentCursor == NULL) {
      /* empty list: the texture starts it at the bottom of TMU 0 */
      texture->residentTmuIndex = GRAPHICS_TEXTURE_RESIDENT_TMU0;
      texture->residentNext = NULL;
      texture->residentAddress = g_GlideTmuMinAddress[0];
      g_GlideResidentTextureTail = texture;
      g_GlideResidentTextureHead = texture;
      Glide3_TextureResource_Download(texture);
      return;
    }
    if (residentCursor->residentNext == NULL) {
      /* list end: append behind the last texture, or at the start of the next TMU */
      asset = residentCursor->sourceAsset;
      subresourceRecordOffset = GLIDE_RECORD_OFFSET(asset,residentCursor->subresourceIndex);
      tmuIndex = residentCursor->residentTmuIndex;
      tailFreeBytes = g_GlideTmuMaxAddress[tmuIndex] -
              (GLIDE_TMU_BYTES(asset,subresourceRecordOffset,residentCursor->downsampleShift) +
               residentCursor->residentAddress);
      asset = texture->sourceAsset;
      subresourceRecordOffset = GLIDE_RECORD_OFFSET(asset,texture->subresourceIndex);
      if (GLIDE_TMU_BYTES(asset,subresourceRecordOffset,texture->downsampleShift) < tailFreeBytes) {
        residentCursor->residentNext = texture;
        texture->residentTmuIndex = tmuIndex;
        texture->residentAddress = g_GlideTmuMaxAddress[tmuIndex] - tailFreeBytes;
        texture->residentNext = NULL;
        g_GlideResidentTextureTail = texture;
        Glide3_TextureResource_Download(texture);
        return;
      }
      tmuIndex++;
      if (tmuIndex < g_GlideTmuCount) {
        residentCursor->residentNext = texture;
        texture->residentTmuIndex = tmuIndex;
        texture->residentAddress = g_GlideTmuMinAddress[tmuIndex];
        texture->residentNext = NULL;
        g_GlideResidentTextureTail = texture;
        Glide3_TextureResource_Download(texture);
        return;
      }
      nextResident = g_GlideResidentTextureHead; /* no next TMU: continue the round at the list head */
    }
    else {
      /* inside the list: evict the following texture when this one fits */
      asset = texture->sourceAsset;
      subresourceRecordOffset = GLIDE_RECORD_OFFSET(asset,texture->subresourceIndex);
      nextResident = residentCursor->residentNext;
      tmuIndex = residentCursor->residentTmuIndex;
      /* original quirk: the fit is checked against the cursor's own slot (next address - cursor address),
         but the texture is placed at the next texture's address */
      if ((tmuIndex == nextResident->residentTmuIndex) &&
         (GLIDE_TMU_BYTES(asset,subresourceRecordOffset,texture->downsampleShift) <=
          nextResident->residentAddress - residentCursor->residentAddress)) {
        reclaimedAddress = nextResident->residentAddress;
        followingResident = nextResident->residentNext;
        residentCursor->residentNext = texture;
        texture->residentAddress = reclaimedAddress;
        texture->residentNext = followingResident;
        texture->residentTmuIndex = tmuIndex;
        g_GlideResidentTextureTail = texture;
        nextResident->residentNext = NULL;
        nextResident->residentTmuIndex = GRAPHICS_TEXTURE_NOT_RESIDENT;
        nextResident->residentAddress = 0;
        Glide3_TextureResource_Download(texture);
        return;
      }
    }
    residentCursor = nextResident;
  } while (residentCursor != g_GlideResidentTextureTail);
}


/* Address: 0x0057FD40.
   Prepares a texture resource for Glide at the current global downsample shift: fills its GrTexInfo (aspect
   ratio, one LOD level for the larger side, RGB565 for the opaque texture formats and ARGB4444 otherwise),
   allocates the 16-bit CPU upload buffer, marks it not resident and converts the source image into the buffer
   with the converter for that shift. A failed allocation leaves glideInfo.data NULL. EAX is preserved, which
   callers use as the texture pointer result.
*/
void Glide3_TextureResource_Initialize(GraphicsTextureResource *texture)

{
  DDPIXELFORMAT *texturePixelFormat;
  GraphicsTextureDownsampleShift shift;
  uint32_t globalDownsampleShift;
  uint32_t width;
  uint32_t height;
  uint32_t lodSize;
  GraphicsTextureLogicalSize logicalSize;

  globalDownsampleShift = g_TextureDownsampleShift;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(texture->subresourceIndex,texture->sourceAsset);
  height = logicalSize.logicalHeightPixels;
  width = logicalSize.logicalWidthPixels;
  texture->downsampleShift = globalDownsampleShift;
  /* aspect ratio = log2(width / height): double the smaller side until both are equal */
  (texture->glideInfo).aspectRatioLog2 = 0;
  while (width != height) {
    if ((int)width < (int)height) {
      (texture->glideInfo).aspectRatioLog2 = (texture->glideInfo).aspectRatioLog2 - 1;
      width = width * 2;
    }
    else {
      (texture->glideInfo).aspectRatioLog2 = (texture->glideInfo).aspectRatioLog2 + 1;
      height = height * 2;
    }
  }
  /* small and large LOD = log2 of the larger side (height now holds it): a single mipmap level */
  (texture->glideInfo).smallLodLog2 = 0;
  (texture->glideInfo).largeLodLog2 = 0;
  texturePixelFormat = texture->pixelFormat;
  for (lodSize = 1; lodSize != height; lodSize = lodSize * 2) {
    (texture->glideInfo).smallLodLog2 = (texture->glideInfo).smallLodLog2 + 1;
    (texture->glideInfo).largeLodLog2 = (texture->glideInfo).largeLodLog2 + 1;
  }
  (texture->glideInfo).format = GR_TEXFMT_ARGB_4444;
  shift = texture->downsampleShift;
  if ((texturePixelFormat == &g_Direct3DOpaqueTextureFormat) ||
      (texturePixelFormat == &g_Direct3DSelectedOpaqueTextureFormat)) {
    (texture->glideInfo).format = GR_TEXFMT_RGB_565;
  }
  (texture->glideInfo).smallLodLog2 = (texture->glideInfo).smallLodLog2 - shift;
  (texture->glideInfo).largeLodLog2 = (texture->glideInfo).largeLodLog2 - shift;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(texture->subresourceIndex,texture->sourceAsset);
  (texture->glideInfo).data = NULL;
  texture->residentNext = NULL;
  texture->residentTmuIndex = GRAPHICS_TEXTURE_NOT_RESIDENT;
  texture->residentAddress = 0;
  /* two bytes per texel of the downsampled image */
  if (g_MemoryApi.alloc
        ((logicalSize.logicalHeightPixels >> ((uint8_t)shift & SHIFT_COUNT_MASK)) *
         (logicalSize.logicalWidthPixels >> ((uint8_t)shift & SHIFT_COUNT_MASK)) * 2,
         &(texture->glideInfo).data) == 0) {
    g_GlideTextureColorUpload[shift](texture);
  }
  return;
}


/* Address: 0x0057FE80.
   Frees the Glide side of a texture resource: unlinks it from the resident list (moving the placement cursor
   g_GlideResidentTextureTail to its predecessor, or successor for the head, and dropping it as the bound
   texture), marks it not resident and frees its upload buffer. The TMU memory simply becomes free for
   Glide3_TextureResource_EnsureResident. EAX is preserved (the texture pointer).
*/
void Glide3_TextureResource_Release(GraphicsTextureResource *texture)

{
  GraphicsTextureResource *previousResident;
  GraphicsTextureResource *residentCursor;
  GraphicsTextureResource *successor;
  GraphicsTextureResource *newTail;

  if ((int)texture->residentTmuIndex >= 0) {
    previousResident = NULL;
    residentCursor = g_GlideResidentTextureHead;
    while ((residentCursor != NULL) && (residentCursor != texture)) {
      previousResident = residentCursor;
      residentCursor = residentCursor->residentNext;
    }
    if (residentCursor != NULL) {
      successor = texture->residentNext;
      if (previousResident == NULL) {
        g_GlideResidentTextureHead = successor;
        newTail = successor;
      }
      else {
        previousResident->residentNext = successor;
        newTail = previousResident;
      }
      if (texture == g_GlideResidentTextureTail) {
        g_GlideResidentTextureTail = newTail;
      }
      if (texture == g_GlideBoundTexture) {
        g_GlideBoundTexture = NULL;
      }
    }
  }
  texture->residentTmuIndex = GRAPHICS_TEXTURE_NOT_RESIDENT;
  texture->residentNext = NULL;
  texture->residentAddress = 0;
  g_MemoryApi.free((texture->glideInfo).data);
  (texture->glideInfo).data = NULL;
  return;
}

