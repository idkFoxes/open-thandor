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

extern TH_LEGACY_GUID IID_IDirectDraw2_Local; /* 00577C00 IID_IDirectDraw2_Local */

extern TH_LEGACY_GUID IID_IDirectDrawSurface3_Local; /* 00577C10 IID_IDirectDrawSurface3_Local */

extern IDirectDraw *g_DirectDraw; /* 00577C40 g_DirectDraw */

extern IDirectDraw2 *g_DirectDraw2; /* 00577C44 g_DirectDraw2 */

extern IDirectDrawSurface *g_PrimarySurfaceBase; /* 00577C48 g_PrimarySurfaceBase */

extern IDirectDrawSurface *g_BackSurfaceBase; /* 00577C50 g_BackSurfaceBase */

extern SoftwareDisplayModeHookProc *g_GraphicsDisplayModeFinalize; /* 00577E30 g_GraphicsDisplayModeFinalize */

extern int32_t g_CursorCurrentVisibilityToken; /* 00577E34 g_CursorCurrentVisibilityToken */

extern int32_t g_GraphicsBackendAccessState; /* 00577E4C g_GraphicsBackendAccessState */

extern GraphicsTextureResource **g_GraphicsTextureSlots; /* 0057805C g_GraphicsTextureSlots */

extern SoftwareFramebufferAccess g_DisplayFramebufferAccess; /* 00578060 g_DisplayFramebufferAccess */

#endif
