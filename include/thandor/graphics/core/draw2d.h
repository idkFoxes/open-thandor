/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/core/draw2d.h
 */

#ifndef THANDOR_GRAPHICS_CORE_DRAW2D_H
#define THANDOR_GRAPHICS_CORE_DRAW2D_H

/* The 2D draw-list front end (step 9, docs/plans/step9_gpu_ui.md sections 5 and 6.4).

Every 2D draw of the UI goes through the blit/fill slots (g_GraphicsTextureSourceBlit*,
g_GraphicsTextureSourceStretchDirectColorBilinear, g_GraphicsFramebufferFillRectArgb) and the three slots below
that replace the direct pixel writes A1-A3 (minimap, results graph columns, credits grey-scale image). Which
functions those slots hold is chosen by the backend:

- DRAW2D_BACKEND_SOFTWARE (the default and the reference): the slots hold the software functions themselves, so
  every draw writes the CPU framebuffer immediately and the output is bit-identical to the software renderer.
- DRAW2D_BACKEND_GPU_RECORD: a draw whose destination is the display framebuffer (&g_DisplayFramebufferAccess)
  appends a Draw2DItem to the frame's list instead of writing pixels; any other destination (the cursor
  composite buffer, offscreen buffers, the raster self-test) still goes to the software function. The
  special cases (minimap, results columns, grey-scale image and the bilinear stretch) are drawn by their
  software function into a CPU scratch image that the item points to (DRAW2D_OP_IMAGE_REGION, the MVP's
  streaming fallback).
- DRAW2D_BACKEND_COMPARE (the developer tools' OPEN_THANDOR_GPU=compare): every draw does both - the software
  function writes the CPU framebuffer (the reference picture, as with DRAW2D_BACKEND_SOFTWARE) and, for the display
  framebuffer, the item is recorded as with DRAW2D_BACKEND_GPU_RECORD, so the GPU can draw the same frame and the
  two pictures can be compared.

The list holds items in call order (no reordering). Draw2D_BeginFrame empties it and releases the scratch
images of the previous frame; the pixel pointers of IMAGE_REGION items stay valid until the next
Draw2D_BeginFrame. Items keep the asset pointer of SPRITE draws only: the GPU backend must read (hash or upload)
the asset while the frame is recorded or right at its flush, before the simulation can release it (see 6.4,
"Simulation runs between render passes"). All of this runs on the main thread. */

#include <stdint.h>
#include <thandor/core/types.h>
#include <thandor/graphics/backend/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

enum Draw2DBackend : uint8_t {
    DRAW2D_BACKEND_SOFTWARE = 0,
    DRAW2D_BACKEND_GPU_RECORD = 1,
    DRAW2D_BACKEND_COMPARE = 2,
};

enum Draw2DOp : uint8_t {
    DRAW2D_OP_SPRITE = 0,       /* one subresource of asset: dst <- src texels, through clip */
    DRAW2D_OP_FILL = 1,         /* dst filled with tintArgb, through clip */
    DRAW2D_OP_IMAGE_REGION = 2, /* CPU image (pixels, pitchBytes) of src size drawn at dst, through clip */
    DRAW2D_OP_EXTERNAL_3D = 3,  /* the 3D scene ends here: it covers clip (= dst) */
};

enum Draw2DBlend : uint8_t {
    /* source alpha: alpha 0 is skipped, alpha 0xFF is written as is (the software path converts it through the
       brightness/contrast tables), anything else is blended. BlitSourceAlpha, translucent fills. */
    DRAW2D_BLEND_SRC_ALPHA_SKIP0 = 0,
    /* BlitHalfSourceRgb: alpha 0 skipped, everything else blended; a paletted source is drawn at half strength */
    DRAW2D_BLEND_HALF_RGB = 1,
    /* BlitModulatedSourceAlpha: every channel (alpha too) multiplied by tintArgb, (c * m) >> 8, then blended */
    DRAW2D_BLEND_MODULATED = 2,
    /* written as is: opaque fills (tintArgb, alpha 0xFF) and IMAGE_REGION items (whose alpha byte is not
       meaningful: the pixels are framebuffer pixels) */
    DRAW2D_BLEND_OPAQUE = 3,
};

/* paletteBank of a SPRITE item whose subresource stores ARGB8888 texels (paletteIndex -1) */
#define DRAW2D_PALETTE_BANK_DIRECT 0xffffffffu

/* One recorded draw. Rectangles are {x0, y0, x1, y1} with x1/y1 exclusive; dst and clip are in logical
   framebuffer pixels, src in texels of the subresource (SPRITE) or pixels of the image (IMAGE_REGION).
   - SPRITE: dst is the whole stored image (draw position + subresource origin, pixel size, unclipped),
     src = {0, 0, pixelWidth, pixelHeight}; clip is the clip rectangle intersected with the framebuffer and the
     drawn image (never empty). tintArgb is the modulation for DRAW2D_BLEND_MODULATED, else 0xFFFFFFFF.
   - FILL: dst = clip = the filled rectangle after clipping; asset nullptr; blend OPAQUE (alpha 0xFF) or
     SRC_ALPHA_SKIP0; tintArgb the ARGB8888 colour.
   - IMAGE_REGION: pixels/pitchBytes describe src[2] x src[3] pixels (src[0] = src[1] = 0) in framebuffer
     format (XRGB8888 after the brightness/contrast tables), shown 1:1 at dst; clip = dst; blend OPAQUE.
   - EXTERNAL_3D: dst = clip = the scene's clip rectangle; nothing else is set. */
struct Draw2DItem {
    uint8_t op;    /* Draw2DOp */
    uint8_t blend; /* Draw2DBlend */
    const GraphicsTextureSourceAsset *asset;
    uint32_t subresource;
    uint32_t paletteBank; /* palette bank of the subresource or DRAW2D_PALETTE_BANK_DIRECT */
    int32_t dst[4];
    int32_t src[4];
    int32_t clip[4];
    uint32_t tintArgb;
    const uint32_t *pixels; /* IMAGE_REGION only */
    int32_t pitchBytes;     /* IMAGE_REGION only */
};

/* ---- New slots for the former direct pixel writes (software implementations in graphics/backend) ---- */

/* A1, the minimap (UiSelectionGeometryControl_DrawClipped): fills width x height pixels at (destX, destY) with
   opaque bilinear samples of subresource 0 (ARGB8888 texels; texels outside it count as 0) of texture. The
   Q12 source position starts at (startU, startV) and advances by (pixelStepU, pixelStepV) per pixel and
   (rowStepU, rowStepV) per row (wrapping 32-bit arithmetic). No clipping: the rectangle must lie in the
   framebuffer. */
using GraphicsMinimapDrawProc = void (int32_t destY, int32_t destX, int32_t height, int32_t width,
                                      uint32_t startU, uint32_t startV, uint32_t pixelStepU, uint32_t pixelStepV,
                                      uint32_t rowStepU, uint32_t rowStepV,
                                      GraphicsTextureSourceAsset *texture, SoftwareFramebufferAccess *framebuffer);

/* A2, one results graph column (FrontendResultsGraph_*Column): from (drawX, topY) downwards, segmentCount
   segments of segmentHeights[i] pixels, each written with the framebuffer pixel packedColors[i] (already
   converted through the pack tables). A segment of height 0 is skipped. No clipping. */
using GraphicsFillColumnSegmentsProc = void (int32_t topY, int32_t drawX, uint32_t segmentCount,
                                             const int32_t *segmentHeights, const uint32_t *packedColors,
                                             SoftwareFramebufferAccess *framebuffer);

/* A3, the credits grey-scale image: the signature of SoftwareTexture_BilinearBlendScaleSubresources (cross-fades
   two 8-bit subresources into blendedSourcePixels and draws the result scaled as grey levels). */
using GraphicsGreyScaleImageProc = void (GraphicsPixelDimension destinationHeight,
                                         GraphicsPixelDimension destinationWidth,
                                         GraphicsScreenCoordinate destinationTop,
                                         GraphicsScreenCoordinate destinationLeft, uint64_t *blendedSourcePixels,
                                         uint64_t *blendFactorPixels, GraphicsSubresourceIndex sourceSubresourceIndexA,
                                         GraphicsSubresourceIndex sourceSubresourceIndexB, int *graphicsTextureAsset,
                                         int *framebufferAccess);

extern GraphicsMinimapDrawProc *g_GraphicsMinimapDraw;
extern GraphicsFillColumnSegmentsProc *g_GraphicsFillColumnSegments;
extern GraphicsGreyScaleImageProc *g_GraphicsGreyScaleImage;

/* ---- Front end ---- */

/* Installs the slot functions of the current backend: the blit/fill slots, the tiled slots' per-tile blits
   follow automatically (they call g_GraphicsTextureSourceBlitSourceAlpha / HalfSourceRgb), and the three slots
   above. Called by GraphicsDisplay_PublishFramebuffer on every display mode change and by Draw2D_SetBackend. */
void Draw2D_InstallSlots();

/* Switches the backend and reinstalls the slots. Switching starts a new, empty frame list. */
void Draw2D_SetBackend(Draw2DBackend backend);
Draw2DBackend Draw2D_GetBackend();

/* Starts a frame: empties the item list and recycles the scratch images of the previous frame. */
void Draw2D_BeginFrame();

/* The items recorded since Draw2D_BeginFrame, in call order (count in *outCount; nullptr when empty). */
const Draw2DItem *Draw2D_FrameItems(uint32_t *outCount);

/* Ends the frame's recording (the list stays readable until the next Draw2D_BeginFrame). */
void Draw2D_EndFrame();

/* GPU backend hook (nullptr when unused): called right after a SPRITE item is appended, with its index in the
   frame list. The GPU backend reads the asset here (texture cache lookup, which converts the texels into its
   staging buffer at once), because the simulation may release or rewrite the asset before the flush (6.4). */
using Draw2DSpriteRecordedProc = void (uint32_t itemIndex, const Draw2DItem *item);
extern Draw2DSpriteRecordedProc *g_Draw2DSpriteRecorded;

/* Marks where a 3D scene ends (after g_GraphicsEndScene): records a DRAW2D_OP_EXTERNAL_3D item covering the
   scene's clip rectangle in GPU_RECORD and COMPARE mode; does nothing in software mode. */
void Draw2D_MarkExternal3D(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX);

#endif /* THANDOR_GRAPHICS_CORE_DRAW2D_H */
