/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/data.h
 */

#ifndef THANDOR_GRAPHICS_BACKEND_DATA_H
#define THANDOR_GRAPHICS_BACKEND_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint64_t g_SoftwareBilinearPackedByteClampMask; /* 0041F688 g_SoftwareBilinearPackedByteClampMask */

extern SoftwarePixelMmxConstants g_SoftwarePixelMmxConstants; /* 0041F6E0 g_SoftwarePixelMmxConstants */

extern SoftwareBgraWordLanes g_SoftwareBilinearForwardFactors[257]; /* 0041FF20 g_SoftwareBilinearForwardFactors */

extern SoftwareBgraWordLanes g_SoftwareBilinearInverseFactors[257]; /* 00420F20 g_SoftwareBilinearInverseFactors */

extern SoftwareRgbWordLanes g_SoftwareBlendAlphaFactors[256]; /* 00421720 g_SoftwareBlendAlphaFactors */

extern SoftwareRgbWordLanes g_SoftwareBlendInverseAlphaFactors[256]; /* 00421F20 g_SoftwareBlendInverseAlphaFactors */

extern GraphicsEndSceneProc *g_GraphicsEndScene; /* 00485824 g_GraphicsEndScene */

extern GraphicsDiagnosticCounter g_PrimitiveDrawCallCount; /* 00485850 g_PrimitiveDrawCallCount */

extern GraphicsDiagnosticCounter g_TextureBindStateChangeCount; /* 00485854 g_TextureBindStateChangeCount */

extern GraphicsDiagnosticCounter g_TextureDeviceReloadCount; /* 00485858 g_TextureDeviceReloadCount */

extern SoftwarePixelPackTables *g_SoftwarePixelPackTables; /* 004A8E80 g_SoftwarePixelPackTables */

extern uint32_t g_ActiveGraphicsAdapterIndex; /* 004A8E90 g_ActiveGraphicsAdapterIndex: uint32_t index into g_GraphicsAdapters of the active graphics adapter; 0xFFFFFFFF (GRAPHICS_ADAPTER_INDEX_NONE) before a display mode is set. */

extern int32_t g_SoftwareColorScaleQ16; /* 004A8E94 g_SoftwareColorScaleQ16 */

extern int32_t g_SoftwareColorBiasQ16; /* 004A8E98 g_SoftwareColorBiasQ16 */

extern GraphicsDisplayMode *g_GraphicsDisplayModes; /* 004A8E9C g_GraphicsDisplayModes */

extern GraphicsDisplayModeCount g_GraphicsDisplayModeCount; /* 004A8EA0 g_GraphicsDisplayModeCount */

extern GraphicsAdapterRecord *g_GraphicsAdapters; /* 004A8EA4 g_GraphicsAdapters */

extern uint32_t g_GraphicsAdapterCount; /* 004A8EA8 g_GraphicsAdapterCount */

extern SoftwarePixelFormatConfig g_SoftwarePixelFormatConfig; /* 004A8EAC g_SoftwarePixelFormatConfig */

extern SoftwareDisplayModeHookProc *g_GraphicsSetDisplayMode; /* 004A8ED0 g_GraphicsSetDisplayMode */

extern SoftwareFramebufferDestroyProc *g_SoftwareFramebufferDestroy; /* 004A8EDC g_SoftwareFramebufferDestroy: SoftwareFramebufferDestroyProc * hook slot, statically SoftwareFramebuffer_Destroy (graphics/backend/software.c). */

extern GraphicsFramebufferCaptureRegionProc *g_GraphicsFramebufferCaptureRegion; /* 004A8EE4 g_GraphicsFramebufferCaptureRegion */

extern SoftwareBuildPixelPackTablesProc *g_SoftwareBuildPixelPackTables; /* 004A8EE8 g_SoftwareBuildPixelPackTables */

extern GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitHalfSourceRgb; /* 004A8F04 g_GraphicsTextureSourceBlitHalfSourceRgb */

extern GraphicsTextureSourceBlitIntegerScaledSourceAlphaProc *g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha; /* 004A8F0C g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha */

extern GraphicsTextureSourceStretchDirectColorBilinearProc *g_GraphicsTextureSourceStretchDirectColorBilinear; /* 004A8F10 g_GraphicsTextureSourceStretchDirectColorBilinear */

extern GraphicsTextureSourceBlitSourceAlphaPaletteBankProc *g_GraphicsTextureSourceBlitSourceAlphaPaletteBank; /* 004A8F14 g_GraphicsTextureSourceBlitSourceAlphaPaletteBank */

extern GraphicsTextureSourceSaturatedAddRgbProc *g_GraphicsTextureSourceBlitSaturatedAddRgb; /* 004A8F1C g_GraphicsTextureSourceBlitSaturatedAddRgb */

extern GraphicsTextureSourceSaturatedAddRgbProc *g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd; /* 004A8F24 g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd */

extern GraphicsFramebufferCopyRegionToOriginProc *g_GraphicsFramebufferCopyRegionToOrigin; /* 004A8F34 g_GraphicsFramebufferCopyRegionToOrigin: GraphicsFramebufferCopyRegionToOriginProc * hook slot, statically SoftwareFramebuffer_CopyRegionToOrigin (software.c). */

extern GraphicsFramebufferCopyOriginToRegionProc *g_GraphicsFramebufferCopyOriginToRegion; /* 004A8F38 g_GraphicsFramebufferCopyOriginToRegion: GraphicsFramebufferCopyOriginToRegionProc * hook slot, statically SoftwareFramebuffer_CopyOriginToRegion (software.c). */

extern uint32_t g_SoftwareDepthRowStrideBytes; /* 004D1234 g_SoftwareDepthRowStrideBytes */

extern void *g_SoftwareAuxiliaryTargetBase; /* 004D123C g_SoftwareAuxiliaryTargetBase */

extern int32_t g_SoftwareDepthEpoch; /* 004D1240 g_SoftwareDepthEpoch */

extern SoftwareDisplayModeHookProc *g_SoftwareChainedSetDisplayMode; /* 004D1244 g_SoftwareChainedSetDisplayMode */

extern SoftwareDrawQueueProc *g_SoftwareDrawQueue; /* 004D1248 g_SoftwareDrawQueue */

extern SoftwareRasterHandler *g_SoftwareRasterHandlers16Bit[64]; /* 004D1260 g_SoftwareRasterHandlers16Bit */

extern SoftwareRasterHandler *g_SoftwareRasterHandlersNon16Bit[64]; /* 004D1360 g_SoftwareRasterHandlersNon16Bit */

extern SoftwareRasterHandler *g_SoftwareRasterHandlersAuxiliary[64]; /* 004D1460 g_SoftwareRasterHandlersAuxiliary */

extern int16_t g_SoftwareBilinearPackedInterpolationWeights256[256][4]; /* 00518080 g_SoftwareBilinearPackedInterpolationWeights256: int16_t[256][4] MMX word lanes per 8-bit fraction f: lane0 = 0x4040 - 0x40*f, lane1 = 0x40*f (sum 0x4040), lanes 2/3 zero; PMADDWD horizontal weights of SoftwareTexture_SampleIntensity (graphics/backend/software.c) */

extern uint32_t g_SoftwarePixelIntensityToNativeColorLut256[256]; /* 00518880 g_SoftwarePixelIntensityToNativeColorLut256 */

extern uint64_t g_SoftwareBlendUnityWordLanesQ14; /* 00518C80 g_SoftwareBlendUnityWordLanesQ14 */

extern GlideImportBinding g_GlideImportBindings[89]; /* 00573FD8 g_GlideImportBindings */

extern char sz_GLIDE3X[8]; /* 005744CA sz_GLIDE3X */

extern char g_GlideImportName_grAADrawTriangle[21]; /* 005745FA dynapi_24 */

extern char g_GlideImportName_grAlphaBlendFunction[25]; /* 00574610 dynapi_25 */

extern char g_GlideImportName_grAlphaCombine[19]; /* 0057462A dynapi_26 */

extern char g_GlideImportName_grAlphaControlsITRGBLighting[32]; /* 0057463E dynapi_27 */

extern char g_GlideImportName_grAlphaTestFunction[23]; /* 0057465E dynapi_28 */

extern char g_GlideImportName_grAlphaTestReferenceValue[29]; /* 00574676 dynapi_29 */

extern char g_GlideImportName_grBufferClear[18]; /* 00574694 dynapi_30 */

extern char g_GlideImportName_grBufferSwap[16]; /* 005746A6 dynapi_31 */

extern char g_GlideImportName_grChromakeyMode[19]; /* 005746B6 dynapi_32 */

extern char g_GlideImportName_grChromakeyValue[20]; /* 005746CA dynapi_33 */

extern char g_GlideImportName_grClipWindow[17]; /* 00574706 dynapi_36 */

extern char g_GlideImportName_grColorCombine[19]; /* 00574718 dynapi_37 */

extern char g_GlideImportName_grColorMask[15]; /* 0057472C dynapi_38 */

extern char g_GlideImportName_grConstantColorValue[24]; /* 0057473C dynapi_39 */

extern char g_GlideImportName_grCoordinateSpace[21]; /* 00574754 dynapi_40 */

extern char g_GlideImportName_grCullMode[14]; /* 0057476A dynapi_41 */

extern char g_GlideImportName_grDepthBiasLevel[20]; /* 00574778 dynapi_42 */

extern char g_GlideImportName_grDepthBufferFunction[25]; /* 0057478C dynapi_43 */

extern char g_GlideImportName_grDepthBufferMode[21]; /* 005747A6 dynapi_44 */

extern char g_GlideImportName_grDepthMask[15]; /* 005747BC dynapi_45 */

extern char g_GlideImportName_grDepthRange[16]; /* 005747CC dynapi_46 */

extern char g_GlideImportName_grDisable[13]; /* 005747DC dynapi_47 */

extern char g_GlideImportName_grDisableAllEffects[23]; /* 005747EA dynapi_48 */

extern char g_GlideImportName_grDitherMode[16]; /* 00574802 dynapi_49 */

extern char g_GlideImportName_grDrawLine[14]; /* 00574812 dynapi_50 */

extern char g_GlideImportName_grDrawPoint[15]; /* 00574820 dynapi_51 */

extern char g_GlideImportName_grDrawTriangle[19]; /* 00574830 dynapi_52 */

extern char g_GlideImportName_grDrawVertexArray[22]; /* 00574844 dynapi_53 */

extern char g_GlideImportName_grDrawVertexArrayContiguous[32]; /* 0057485A dynapi_54 */

extern char g_GlideImportName_grEnable[12]; /* 0057487A dynapi_55 */

extern char g_GlideImportName_grErrorSetCallback[22]; /* 00574886 dynapi_56 */

extern char g_GlideImportName_grFinish[12]; /* 0057489C dynapi_57 */

extern char g_GlideImportName_grFlush[11]; /* 005748A8 dynapi_58 */

extern char g_GlideImportName_grFogColorValue[19]; /* 005748B4 dynapi_59 */

extern char g_GlideImportName_grFogMode[13]; /* 005748C8 dynapi_60 */

extern char g_GlideImportName_grFogTable[14]; /* 005748D6 dynapi_61 */

extern char g_GlideImportName_grGet[10]; /* 005748E4 dynapi_62 */

extern char g_GlideImportName_grGetProcAddress[20]; /* 005748EE dynapi_63 */

extern char g_GlideImportName_grGetString[15]; /* 00574902 dynapi_64 */

extern char g_GlideImportName_grGlideGetState[19]; /* 00574912 dynapi_65 */

extern char g_GlideImportName_grGlideGetVertexLayout[26]; /* 00574926 dynapi_66 */

extern char g_GlideImportName_grGlideInit[15]; /* 00574940 dynapi_67 */

extern char g_GlideImportName_grGlideSetState[19]; /* 00574950 dynapi_68 */

extern char g_GlideImportName_grGlideSetVertexLayout[26]; /* 00574964 dynapi_69 */

extern char g_GlideImportName_grGlideShutdown[19]; /* 0057497E dynapi_70 */

extern char g_GlideImportName_grLfbConstantAlpha[22]; /* 00574992 dynapi_71 */

extern char g_GlideImportName_grLfbConstantDepth[22]; /* 005749A8 dynapi_72 */

extern char g_GlideImportName_grLfbLock[14]; /* 005749BE dynapi_73 */

extern char g_GlideImportName_grLfbReadRegion[20]; /* 005749CC dynapi_74 */

extern char g_GlideImportName_grLfbUnlock[15]; /* 005749E0 dynapi_75 */

extern char g_GlideImportName_grLfbWriteRegion[21]; /* 005749F0 dynapi_76 */

extern char g_GlideImportName_grLoadGammaTable[21]; /* 00574A06 dynapi_77 */

extern char g_GlideImportName_grQueryResolutions[22]; /* 00574A1C dynapi_78 */

extern char g_GlideImportName_grRenderBuffer[18]; /* 00574A32 dynapi_79 */

extern char g_GlideImportName_grReset[11]; /* 00574A44 dynapi_80 */

extern char g_GlideImportName_grSelectContext[19]; /* 00574A50 dynapi_81 */

extern char g_GlideImportName_grSstOrigin[15]; /* 00574A64 dynapi_82 */

extern char g_GlideImportName_grSstSelect[15]; /* 00574A74 dynapi_83 */

extern char g_GlideImportName_grSstWinClose[17]; /* 00574A84 dynapi_84 */

extern char g_GlideImportName_grSstWinOpen[17]; /* 00574A96 dynapi_85 */

extern char g_GlideImportName_grTexCalcMemRequired[25]; /* 00574AA8 dynapi_86 */

extern char g_GlideImportName_grTexClampMode[19]; /* 00574AF0 dynapi_89 */

extern char g_GlideImportName_grTexCombine[17]; /* 00574B04 dynapi_90 */

extern char g_GlideImportName_grTexDetailControl[23]; /* 00574B16 dynapi_91 */

extern char g_GlideImportName_grTexDownloadMipMap[24]; /* 00574B2E dynapi_92 */

extern char g_GlideImportName_grTexDownloadMipMapLevel[29]; /* 00574B46 dynapi_93 */

extern char g_GlideImportName_grTexDownloadMipMapLevelPartial[36]; /* 00574B64 dynapi_94 */

extern char g_GlideImportName_grTexDownloadTable[22]; /* 00574B88 dynapi_95 */

extern char g_GlideImportName_grTexDownloadTablePartial[30]; /* 00574B9E dynapi_96 */

extern char g_GlideImportName_grTexFilterMode[20]; /* 00574BBC dynapi_97 */

extern char g_GlideImportName_grTexLodBiasValue[21]; /* 00574BD0 dynapi_98 */

extern char g_GlideImportName_grTexMaxAddress[19]; /* 00574BE6 dynapi_99 */

extern char g_GlideImportName_grTexMinAddress[19]; /* 00574BFA dynapi_100 */

extern char g_GlideImportName_grTexMipMapMode[20]; /* 00574C0E dynapi_101 */

extern char g_GlideImportName_grTexMultibase[18]; /* 00574C22 dynapi_102 */

extern char g_GlideImportName_grTexMultibaseAddress[26]; /* 00574C34 dynapi_103 */

extern char g_GlideImportName_grTexNCCTable[17]; /* 00574C4E dynapi_104 */

extern char g_GlideImportName_grTexSource[16]; /* 00574C60 dynapi_105 */

extern char g_GlideImportName_grTexTextureMemRequired[27]; /* 00574C70 dynapi_106 */

extern char g_GlideImportName_grVertexLayout[19]; /* 00574C8C dynapi_107 */

extern char g_GlideImportName_grViewport[15]; /* 00574CA0 dynapi_108 */

extern char g_GlideImportName_gu3dfGetInfo[16]; /* 00574CB0 dynapi_109 */

extern char g_GlideImportName_gu3dfLoad[13]; /* 00574CC0 dynapi_110 */

extern char g_GlideImportName_guFogGenerateExp[20]; /* 00574CCE dynapi_111 */

extern char g_GlideImportName_guFogGenerateExp2[21]; /* 00574CE2 dynapi_112 */

extern char g_GlideImportName_guFogGenerateLinear[24]; /* 00574CF8 dynapi_113 */

extern char g_GlideImportName_guFogTableIndexToW[22]; /* 00574D10 dynapi_114 */

extern char g_GlideImportName_guGammaCorrectionRGB[25]; /* 00574D26 dynapi_115 */

extern TH_LEGACY_GUID IID_IDirectDraw2_Local; /* 00577C00 IID_IDirectDraw2_Local */

extern TH_LEGACY_GUID IID_IDirectDrawSurface3_Local; /* 00577C10 IID_IDirectDrawSurface3_Local */

extern TH_LEGACY_GUID IID_IDirect3D2_Local; /* 00577C20 IID_IDirect3D2_Local */

extern IDirectDraw *g_DirectDraw; /* 00577C40 g_DirectDraw */

extern IDirectDraw2 *g_DirectDraw2; /* 00577C44 g_DirectDraw2 */

extern IDirectDrawSurface *g_PrimarySurfaceBase; /* 00577C48 g_PrimarySurfaceBase */

extern IDirectDrawSurface *g_BackSurfaceBase; /* 00577C50 g_BackSurfaceBase */

extern IDirect3D2 *g_Direct3D2; /* 00577C58 g_Direct3D2 */

extern IDirect3DDevice2 *g_Direct3DDevice2; /* 00577C5C g_Direct3DDevice2 */

extern IDirectDrawSurface *g_ZSurfaceBase; /* 00577C60 g_ZSurfaceBase */

extern IDirectDrawSurface3 *g_ZSurface3; /* 00577C64 g_ZSurface3 */

extern DDPIXELFORMAT g_Direct3DOpaqueTextureFormat; /* 00577D90 g_Direct3DOpaqueTextureFormat */

extern DDPIXELFORMAT g_Direct3DAlphaTextureFormat; /* 00577DB0 g_Direct3DAlphaTextureFormat */

extern DDPIXELFORMAT g_Direct3DSelectedOpaqueTextureFormat; /* 00577DD0 g_Direct3DSelectedOpaqueTextureFormat */

extern DDPIXELFORMAT g_Direct3DSelectedAlphaTextureFormat; /* 00577DF0 g_Direct3DSelectedAlphaTextureFormat */

extern uint32_t g_Direct3DTextureFilterMode; /* 00577E10 g_Direct3DTextureFilterMode: uint32_t: D3DRENDERSTATE_TEXTUREMAG/MIN filter (2 = D3DFILTER_LINEAR) reapplied by the device setup; graphics/backend direct3d/directdraw */

extern uint32_t g_Direct3DTexturePerspectiveEnabled; /* 00577E14 g_Direct3DTexturePerspectiveEnabled: uint32_t: D3DRENDERSTATE_TEXTUREPERSPECTIVE value (1) reapplied by the device setup; graphics/backend direct3d/directdraw */

extern uint32_t g_Direct3DAntialiasMode; /* 00577E18 g_Direct3DAntialiasMode */

extern GraphicsPrimitiveRenderStateCache g_PrimitiveRenderStateCache; /* 00577E1C g_PrimitiveRenderStateCache */

extern uint32_t g_BoundTextureHandle; /* 00577E2C g_BoundTextureHandle */

extern SoftwareDisplayModeHookProc *g_GraphicsDisplayModeFinalize; /* 00577E30 g_GraphicsDisplayModeFinalize */

extern int32_t g_CursorCurrentVisibilityToken; /* 00577E34 g_CursorCurrentVisibilityToken */

extern int32_t g_CursorAlternateVisibilityToken; /* 00577E38 g_CursorAlternateVisibilityToken */

extern int32_t g_GraphicsBackendAccessState; /* 00577E4C g_GraphicsBackendAccessState */

extern GraphicsPrimitiveRenderStatePreset g_PrimitiveRenderStatePresets[5]; /* 00577E50 g_PrimitiveRenderStatePresets */

extern GraphicsDispatchTable g_GraphicsDispatchTable; /* 00577EA0 g_GraphicsDispatchTable */

extern D3DTLVERTEX_DX6 g_ImmediateTLVertices[4]; /* 00577FC0 g_ImmediateTLVertices */

extern uint32_t g_ImmediateVertexCount; /* 00578050 g_ImmediateVertexCount */

extern uint32_t g_TextureUseSerial; /* 00578054 g_TextureUseSerial */

extern GraphicsTextureResource **g_GraphicsTextureSlots; /* 0057805C g_GraphicsTextureSlots */

extern SoftwareFramebufferAccess g_DisplayFramebufferAccess; /* 00578060 g_DisplayFramebufferAccess */

extern uint32_t g_GraphicsEnumerateAllDevicesFlag; /* 00578074 g_GraphicsEnumerateAllDevicesFlag */

extern GrResolution g_GlideEnumerationResolutionQuery; /* 0057ECD0 g_GlideEnumerationResolutionQuery: GrResolution: grQueryResolutions template {GR_QUERY_ANY, GR_QUERY_ANY, 2 colour buffers, 1 aux buffer} for enumerating modes; Glide3_InitAndEnumerate */

extern GrResolution g_GlideSelectedResolutionQuery; /* 0057ECE0 g_GlideSelectedResolutionQuery: GrResolution: grQueryResolutions template {resolution set at run time, GR_QUERY_ANY refresh, 2, 1} to pick the best refresh rate; GraphicsGlide3_ApplyDisplayModeAndInitializeResources */

extern uint32_t g_GlideRefreshRatesHz[9]; /* 0057ECF0 g_GlideRefreshRatesHz: uint32_t[9]: Hz per GR_REFRESH_* code (60,70,72,75,80,90,100,85,120); graphics/backend/glide.c */

extern uint32_t g_GlideWindowContextHandle; /* 0057ED14 g_GlideWindowContextHandle */

extern uint32_t g_GlideRuntimeActiveCount; /* 0057ED18 g_GlideRuntimeActiveCount */

extern GlideLfbInfo g_GlidePrimaryLfbInfo; /* 0057ED20 g_GlidePrimaryLfbInfo */

extern GlideLfbInfo g_GlideSecondaryLfbInfo; /* 0057ED34 g_GlideSecondaryLfbInfo */

extern uint32_t g_GlideVertices[3][8]; /* 0057ED54 g_GlideVertices: uint32_t[3][8]: the three GrVertex records (float bit patterns) Glide3_DrawPrimitiveQueue fills and hands to grDrawTriangle; fields GLIDE_VERTEX_* (graphics/backend/glide.h), dword 4 of each record is unused */

extern uint32_t g_GlideTmuCount; /* 0057EDC8 g_GlideTmuCount */

extern GlideTextureUploadProc *g_GlideTextureColorUpload[3]; /* 0057EDCC g_GlideTextureColorUpload */

extern GlideTextureUploadProc *g_GlideTextureAlphaUpload[3]; /* 0057EDD8 g_GlideTextureAlphaUpload */

extern uint32_t g_GlideTexturingDisabledState; /* 0057EDE4 g_GlideTexturingDisabledState */

extern uint32_t g_GlideBlendModeState; /* 0057EDE8 g_GlideBlendModeState */

extern uint32_t g_GlideDepthWriteEnabledState; /* 0057EDEC g_GlideDepthWriteEnabledState */

extern GraphicsTextureResource *g_GlideBoundTexture; /* 0057EDF0 g_GlideBoundTexture */

extern uint32_t g_GlideTmuMinAddress[16]; /* 0057EDF4 g_GlideTmuMinAddress */

extern uint32_t g_GlideTmuMaxAddress[16]; /* 0057EE34 g_GlideTmuMaxAddress */

extern GraphicsTextureResource *g_GlideResidentTextureTail; /* 0057EE74 g_GlideResidentTextureTail */

extern GraphicsTextureResource *g_GlideResidentTextureHead; /* 0057EE78 g_GlideResidentTextureHead */

extern int32_t g_GlideSecondBufferOffset; /* 0057EE7C g_GlideSecondBufferOffset */

extern uint8_t *g_GlideSecondBufferBase; /* 0057EE80 g_GlideSecondBufferBase */

#endif
