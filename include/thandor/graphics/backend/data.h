/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/data.h
 */

#ifndef THANDOR_GRAPHICS_BACKEND_DATA_H
#define THANDOR_GRAPHICS_BACKEND_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint64_t g_SoftwareBilinearPackedByteClampMask;

extern SoftwarePixelMmxConstants g_SoftwarePixelMmxConstants;

extern SoftwareBgraWordLanes g_SoftwareBilinearForwardFactors[257];

extern SoftwareBgraWordLanes g_SoftwareBilinearInverseFactors[257];

extern SoftwareRgbWordLanes g_SoftwareBlendAlphaFactors[256];

extern SoftwareRgbWordLanes g_SoftwareBlendInverseAlphaFactors[256];

extern GraphicsEndSceneProc *g_GraphicsEndScene;

extern GraphicsDiagnosticCounter g_PrimitiveDrawCallCount;

extern GraphicsDiagnosticCounter g_TextureBindStateChangeCount;

extern GraphicsDiagnosticCounter g_TextureDeviceReloadCount;

extern SoftwarePixelPackTables *g_SoftwarePixelPackTables;

extern uint32_t g_ActiveGraphicsAdapterIndex; /* uint32_t index into g_GraphicsAdapters of the active graphics adapter; 0xFFFFFFFF (GRAPHICS_ADAPTER_INDEX_NONE) before a display mode is set. */

extern int32_t g_SoftwareColorScaleQ16;

extern int32_t g_SoftwareColorBiasQ16;

extern GraphicsDisplayMode *g_GraphicsDisplayModes;

extern GraphicsDisplayModeCount g_GraphicsDisplayModeCount;

extern GraphicsAdapterRecord *g_GraphicsAdapters;

extern uint32_t g_GraphicsAdapterCount;

extern SoftwarePixelFormatConfig g_SoftwarePixelFormatConfig;

extern SoftwareDisplayModeHookProc *g_GraphicsSetDisplayMode;

extern SoftwareFramebufferDestroyProc *g_SoftwareFramebufferDestroy; /* SoftwareFramebufferDestroyProc * hook slot, statically SoftwareFramebuffer_Destroy (graphics/backend/software.c). */

extern GraphicsFramebufferCaptureRegionProc *g_GraphicsFramebufferCaptureRegion;

extern SoftwareBuildPixelPackTablesProc *g_SoftwareBuildPixelPackTables;

extern GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitHalfSourceRgb;

extern GraphicsTextureSourceBlitIntegerScaledSourceAlphaProc *g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha;

extern GraphicsTextureSourceStretchDirectColorBilinearProc *g_GraphicsTextureSourceStretchDirectColorBilinear;

extern GraphicsTextureSourceBlitSourceAlphaPaletteBankProc *g_GraphicsTextureSourceBlitSourceAlphaPaletteBank;

extern GraphicsTextureSourceSaturatedAddRgbProc *g_GraphicsTextureSourceBlitSaturatedAddRgb;

extern GraphicsTextureSourceSaturatedAddRgbProc *g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd;

extern GraphicsFramebufferCopyRegionToOriginProc *g_GraphicsFramebufferCopyRegionToOrigin; /* GraphicsFramebufferCopyRegionToOriginProc * hook slot, statically SoftwareFramebuffer_CopyRegionToOrigin (software.c). */

extern GraphicsFramebufferCopyOriginToRegionProc *g_GraphicsFramebufferCopyOriginToRegion; /* GraphicsFramebufferCopyOriginToRegionProc * hook slot, statically SoftwareFramebuffer_CopyOriginToRegion (software.c). */

extern uint32_t g_SoftwareDepthRowStrideBytes;

extern void *g_SoftwareAuxiliaryTargetBase;

extern int32_t g_SoftwareDepthEpoch;

extern SoftwareDisplayModeHookProc *g_SoftwareChainedSetDisplayMode;

extern SoftwareDrawQueueProc *g_SoftwareDrawQueue;

extern SoftwareRasterHandler *g_SoftwareRasterHandlers16Bit[64];

extern SoftwareRasterHandler *g_SoftwareRasterHandlersNon16Bit[64];

extern SoftwareRasterHandler *g_SoftwareRasterHandlersAuxiliary[64];

extern int16_t g_SoftwareBilinearPackedInterpolationWeights256[256][4]; /* int16_t[256][4] MMX word lanes per 8-bit fraction f: lane0 = 0x4040 - 0x40*f, lane1 = 0x40*f (sum 0x4040), lanes 2/3 zero; PMADDWD horizontal weights of SoftwareTexture_SampleIntensity (graphics/backend/software.c) */

extern uint32_t g_SoftwarePixelIntensityToNativeColorLut256[256];

extern uint64_t g_SoftwareBlendUnityWordLanesQ14;

extern TH_LEGACY_GUID IID_IDirectDraw2_Local;

extern TH_LEGACY_GUID IID_IDirectDrawSurface3_Local;

extern IDirectDraw *g_DirectDraw;

extern IDirectDraw2 *g_DirectDraw2;

extern IDirectDrawSurface *g_PrimarySurfaceBase;

extern IDirectDrawSurface *g_BackSurfaceBase;

extern SoftwareDisplayModeHookProc *g_GraphicsDisplayModeFinalize;

extern int32_t g_CursorCurrentVisibilityToken;

extern int32_t g_GraphicsBackendAccessState;

extern GraphicsTextureResource **g_GraphicsTextureSlots;

extern SoftwareFramebufferAccess g_DisplayFramebufferAccess;

#endif
