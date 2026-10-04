/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/core/draw2d.cpp
 */

/* The 2D draw-list front end (see graphics/core/draw2d.h): the backend switch, the slot installation and the
   GPU_RECORD functions that turn blits and fills into Draw2DItems. */

#include <thandor/graphics/core/draw2d.h>
#include <thandor/thandor.h>

#include <vector>

/* Module data. */

GraphicsMinimapDrawProc *g_GraphicsMinimapDraw = SoftwareTexture_DrawMinimapBilinear32;
GraphicsFillColumnSegmentsProc *g_GraphicsFillColumnSegments = SoftwareFramebuffer_FillColumnSegments32;
GraphicsGreyScaleImageProc *g_GraphicsGreyScaleImage = SoftwareTexture_BilinearBlendScaleSubresources;
Draw2DSpriteRecordedProc *g_Draw2DSpriteRecorded = nullptr;

namespace {

Draw2DBackend s_backend = DRAW2D_BACKEND_SOFTWARE;
std::vector<Draw2DItem> s_items;
/* CPU images of the IMAGE_REGION items. The outer vector may move the inner ones when it grows, which keeps their
   buffers (and so the items' pixel pointers) in place. Recycled by Draw2D_BeginFrame. */
std::vector<std::vector<uint32_t>> s_scratch;
std::size_t s_scratchUsed = 0;

/* The last results graph column: its FILL items (one per drawn segment) are the last `count` items from
   `firstItem`. The next column at nextX widens them by one pixel when its segments match (same rows and colours)
   instead of appending new ones. */
struct ColumnRun {
    bool open;
    std::size_t firstItem;
    std::size_t count;
    int32_t nextX;
};
ColumnRun s_columnRun = {};

bool IsDisplay(const SoftwareFramebufferAccess *framebuffer)
{
    return framebuffer == &g_DisplayFramebufferAccess;
}

uint32_t *AcquireScratch(std::size_t pixelCount)
{
    if (s_scratchUsed == s_scratch.size()) {
        s_scratch.emplace_back();
    }
    std::vector<uint32_t> &buffer = s_scratch[s_scratchUsed++];
    buffer.assign(pixelCount, 0);
    return buffer.data();
}

/* A SoftwareFramebufferAccess over a scratch image, so the software functions can draw into it. */
SoftwareFramebufferAccess ScratchFramebuffer(uint32_t *pixels, int32_t pitchPixels, int32_t height)
{
    SoftwareFramebufferAccess access = {};
    access.width = (GraphicsPixelDimension)pitchPixels;
    access.height = (GraphicsPixelDimension)height;
    access.bytesPerPixel = SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT;
    access.pixels = (uint8_t *)pixels;
    return access;
}

void SetRect(int32_t *rect, int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    rect[0] = x0;
    rect[1] = y0;
    rect[2] = x1;
    rect[3] = y1;
}

/* The clipping of the software blits (Blit_ClipRect): the rectangle limited to the display framebuffer and the
   clip rectangle. False when nothing is left. */
bool ClipToDisplay(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t *left,
                   int32_t *top, int32_t *right, int32_t *bottom)
{
    if (*left < 0) {
        *left = 0;
    }
    if (*top < 0) {
        *top = 0;
    }
    if (*right > (int32_t)g_DisplayFramebufferAccess.width) {
        *right = (int32_t)g_DisplayFramebufferAccess.width;
    }
    if (*bottom > (int32_t)g_DisplayFramebufferAccess.height) {
        *bottom = (int32_t)g_DisplayFramebufferAccess.height;
    }
    if (*left < clipMinX) {
        *left = clipMinX;
    }
    if (*top < clipMinY) {
        *top = clipMinY;
    }
    if (*right > clipMaxX) {
        *right = clipMaxX;
    }
    if (*bottom > clipMaxY) {
        *bottom = clipMaxY;
    }
    return *right > *left && *bottom > *top;
}

Draw2DItem *AppendItem(uint8_t op, uint8_t blend)
{
    Draw2DItem item = {};
    item.op = op;
    item.blend = blend;
    item.tintArgb = ARGB8888_OPAQUE_WHITE;
    s_items.push_back(item);
    return &s_items.back();
}

/* The subresource entry of a drawable texture source (as Blit_SetupSubresource checks it), or nullptr. */
const GraphicsTextureSourceEntry *SubresourceEntry(const GraphicsTextureSourceAsset *asset, uint32_t subresource)
{
    if (asset == nullptr || asset->common.magic != ASSET_MAGIC_GFX ||
        subresource >= asset->tableDescriptor.subresourceCount) {
        return nullptr;
    }
    const GraphicsTextureSourceEntry *entry =
        (const GraphicsTextureSourceEntry *)((const uint8_t *)asset + asset->tableDescriptor.subresourceTableOffset) +
        subresource;
    if (entry->paletteIndex != -1 && (uint32_t)entry->paletteIndex >= asset->tableDescriptor.paletteBankCount) {
        return nullptr;
    }
    return entry;
}

void RecordSprite(uint8_t blend, uint32_t tintArgb, int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY,
                  int32_t clipMinX, int32_t drawY, int32_t drawX, uint32_t subresource,
                  const GraphicsTextureSourceAsset *asset)
{
    const GraphicsTextureSourceEntry *entry = SubresourceEntry(asset, subresource);
    if (entry == nullptr) {
        return;
    }
    const int32_t x0 = drawX + entry->originX;
    const int32_t y0 = drawY + entry->originY;
    const int32_t x1 = x0 + (int32_t)entry->pixelWidth;
    const int32_t y1 = y0 + (int32_t)entry->pixelHeight;
    int32_t left = x0;
    int32_t top = y0;
    int32_t right = x1;
    int32_t bottom = y1;
    if (!ClipToDisplay(clipMaxY, clipMaxX, clipMinY, clipMinX, &left, &top, &right, &bottom)) {
        return;
    }
    Draw2DItem *item = AppendItem(DRAW2D_OP_SPRITE, blend);
    item->asset = asset;
    item->subresource = subresource;
    item->paletteBank = entry->paletteIndex == -1 ? DRAW2D_PALETTE_BANK_DIRECT : (uint32_t)entry->paletteIndex;
    item->tintArgb = tintArgb;
    SetRect(item->dst, x0, y0, x1, y1);
    SetRect(item->src, 0, 0, (int32_t)entry->pixelWidth, (int32_t)entry->pixelHeight);
    SetRect(item->clip, left, top, right, bottom);
    if (g_Draw2DSpriteRecorded != nullptr) {
        g_Draw2DSpriteRecorded((uint32_t)(s_items.size() - 1), item);
    }
}

/* An IMAGE_REGION item for width x height pixels at (x, y), clipped to the display framebuffer. */
Draw2DItem *RecordImageRegion(int32_t x, int32_t y, int32_t width, int32_t height, const uint32_t *pixels,
                              int32_t pitchBytes)
{
    int32_t left = x;
    int32_t top = y;
    int32_t right = x + width;
    int32_t bottom = y + height;
    if (!ClipToDisplay(INT32_MAX, INT32_MAX, INT32_MIN, INT32_MIN, &left, &top, &right, &bottom)) {
        return nullptr;
    }
    Draw2DItem *item = AppendItem(DRAW2D_OP_IMAGE_REGION, DRAW2D_BLEND_OPAQUE);
    SetRect(item->dst, x, y, x + width, y + height);
    SetRect(item->src, 0, 0, width, height);
    SetRect(item->clip, left, top, right, bottom);
    item->pixels = pixels;
    item->pitchBytes = pitchBytes;
    return item;
}

/* ---- GPU_RECORD slot functions ---- */

Bool8 RecordBlitSourceAlpha(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t drawY,
                            int32_t drawX, uint32_t subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,
                            SoftwareFramebufferAccess *framebuffer)
{
    if (!IsDisplay(framebuffer)) {
        return SoftwareTextureSource_BlitSourceAlpha32(clipMaxY, clipMaxX, clipMinY, clipMinX, drawY, drawX,
                                                       subresourceIndex, sourceAsset, framebuffer);
    }
    RecordSprite(DRAW2D_BLEND_SRC_ALPHA_SKIP0, ARGB8888_OPAQUE_WHITE, clipMaxY, clipMaxX, clipMinY, clipMinX, drawY,
                 drawX, subresourceIndex, sourceAsset);
    return false;
}

Bool8 RecordBlitHalfSourceRgb(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t drawY,
                              int32_t drawX, uint32_t subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,
                              SoftwareFramebufferAccess *framebuffer)
{
    if (!IsDisplay(framebuffer)) {
        return SoftwareTextureSource_BlitHalfSourceRgb32(clipMaxY, clipMaxX, clipMinY, clipMinX, drawY, drawX,
                                                         subresourceIndex, sourceAsset, framebuffer);
    }
    RecordSprite(DRAW2D_BLEND_HALF_RGB, ARGB8888_OPAQUE_WHITE, clipMaxY, clipMaxX, clipMinY, clipMinX, drawY, drawX,
                 subresourceIndex, sourceAsset);
    return false;
}

Bool8 RecordBlitModulatedSourceAlpha(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX,
                                     int32_t drawY, int32_t drawX, uint32_t modulationArgb8888,
                                     uint32_t subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,
                                     SoftwareFramebufferAccess *framebuffer)
{
    if (!IsDisplay(framebuffer)) {
        return SoftwareTextureSource_BlitModulatedSourceAlpha32(clipMaxY, clipMaxX, clipMinY, clipMinX, drawY, drawX,
                                                                modulationArgb8888, subresourceIndex, sourceAsset,
                                                                framebuffer);
    }
    RecordSprite(DRAW2D_BLEND_MODULATED, modulationArgb8888, clipMaxY, clipMaxX, clipMinY, clipMinX, drawY, drawX,
                 subresourceIndex, sourceAsset);
    return false;
}

void RecordFillRectArgb(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t rectMaxY,
                        int32_t rectMaxX, int32_t rectMinY, int32_t rectMinX, uint32_t argb8888,
                        SoftwareFramebufferAccess *framebuffer)
{
    if (!IsDisplay(framebuffer)) {
        SoftwareFramebuffer_FillRectArgb32(clipMaxY, clipMaxX, clipMinY, clipMinX, rectMaxY, rectMaxX, rectMinY,
                                           rectMinX, argb8888, framebuffer);
        return;
    }
    if ((argb8888 & ARGB8888_ALPHA_MASK) == 0 ||
        !ClipToDisplay(clipMaxY, clipMaxX, clipMinY, clipMinX, &rectMinX, &rectMinY, &rectMaxX, &rectMaxY)) {
        return;
    }
    Draw2DItem *item = AppendItem(DRAW2D_OP_FILL, (argb8888 & ARGB8888_ALPHA_MASK) == ARGB8888_ALPHA_MASK
                                                      ? DRAW2D_BLEND_OPAQUE
                                                      : DRAW2D_BLEND_SRC_ALPHA_SKIP0);
    item->tintArgb = argb8888;
    SetRect(item->dst, rectMinX, rectMinY, rectMaxX, rectMaxY);
    SetRect(item->clip, rectMinX, rectMinY, rectMaxX, rectMaxY);
}

/* An IMAGE_BILINEAR item: width x height pixels (pitchBytes apart) stretched over the destination
   (destinationLeft, destinationTop, destinationWidth x destinationHeight), of which the left drawnWidth columns
   are written; clipped to the display. Hands the item to the GPU backend at once (it uploads the pixels). */
void RecordImageBilinear(int32_t destinationLeft, int32_t destinationTop, int32_t destinationWidth,
                         int32_t destinationHeight, int32_t drawnWidth, const uint32_t *pixels, int32_t width,
                         int32_t height, int32_t pitchBytes)
{
    int32_t left = destinationLeft;
    int32_t top = destinationTop;
    int32_t right = destinationLeft + drawnWidth;
    int32_t bottom = destinationTop + destinationHeight;
    if (!ClipToDisplay(INT32_MAX, INT32_MAX, INT32_MIN, INT32_MIN, &left, &top, &right, &bottom)) {
        return;
    }
    Draw2DItem *item = AppendItem(DRAW2D_OP_IMAGE_BILINEAR, DRAW2D_BLEND_OPAQUE);
    SetRect(item->dst, destinationLeft, destinationTop, destinationLeft + destinationWidth,
            destinationTop + destinationHeight);
    SetRect(item->src, 0, 0, width, height);
    SetRect(item->clip, left, top, right, bottom);
    item->pixels = pixels;
    item->pitchBytes = pitchBytes;
    if (g_Draw2DSpriteRecorded != nullptr) {
        g_Draw2DSpriteRecorded((uint32_t)(s_items.size() - 1), item);
    }
}

/* The bilinear stretch (movie frames, briefing images): an IMAGE_BILINEAR item over the subresource's ARGB8888
   texels, when the software function would draw at all. The software stretch writes pixel pairs, so an odd last
   column keeps what was there before; the item's clip leaves it out as well (a column drawn there would differ
   from the software frame, e.g. next to the end movie's letterbox bars). */
void RecordStretchDirectColorBilinear(uint32_t destinationHeight, uint32_t destinationWidth, int32_t destinationY,
                                      int32_t destinationX, uint32_t subresourceIndex,
                                      GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer)
{
    if (!IsDisplay(framebuffer)) {
        SoftwareTextureSource_StretchDirectColorBilinear32(destinationHeight, destinationWidth, destinationY,
                                                           destinationX, subresourceIndex, sourceAsset, framebuffer);
        return;
    }
    const GraphicsTextureSourceEntry *entry = SubresourceEntry(sourceAsset, subresourceIndex);
    if (entry == nullptr || entry->paletteIndex != -1 || destinationWidth < 2 || destinationHeight == 0 ||
        entry->pixelWidth == 0 || entry->pixelHeight == 0) {
        return;
    }
    /* the software function's bounds check against the display framebuffer */
    const int64_t pitch = g_DisplayFramebufferAccess.width;
    const int64_t first = (int64_t)destinationY * pitch + destinationX;
    const int64_t end = first + (int64_t)(destinationHeight - 1) * pitch + (destinationWidth & ~1u);
    if (first < 0 || end > pitch * g_DisplayFramebufferAccess.height) {
        return;
    }
    if (destinationWidth > (uint32_t)INT32_MAX || destinationHeight > (uint32_t)INT32_MAX ||
        entry->pixelWidth > (uint32_t)INT32_MAX / 4 || entry->pixelHeight > (uint32_t)INT32_MAX) {
        return;
    }
    const uint32_t *texels = (const uint32_t *)((const uint8_t *)sourceAsset + entry->dataOffset);
    RecordImageBilinear(destinationX, destinationY, (int32_t)destinationWidth, (int32_t)destinationHeight,
                        (int32_t)(destinationWidth & ~1u), texels, (int32_t)entry->pixelWidth,
                        (int32_t)entry->pixelHeight, (int32_t)entry->pixelWidth * 4);
}

void RecordMinimapDraw(int32_t destY, int32_t destX, int32_t height, int32_t width, uint32_t startU, uint32_t startV,
                       uint32_t pixelStepU, uint32_t pixelStepV, uint32_t rowStepU, uint32_t rowStepV,
                       GraphicsTextureSourceAsset *texture, SoftwareFramebufferAccess *framebuffer)
{
    if (!IsDisplay(framebuffer)) {
        SoftwareTexture_DrawMinimapBilinear32(destY, destX, height, width, startU, startV, pixelStepU, pixelStepV,
                                              rowStepU, rowStepV, texture, framebuffer);
        return;
    }
    if (width <= 0 || height <= 0) {
        return;
    }
    uint32_t *pixels = AcquireScratch((std::size_t)width * (std::size_t)height);
    SoftwareFramebufferAccess scratch = ScratchFramebuffer(pixels, width, height);
    SoftwareTexture_DrawMinimapBilinear32(0, 0, height, width, startU, startV, pixelStepU, pixelStepV, rowStepU,
                                          rowStepV, texture, &scratch);
    RecordImageRegion(destX, destY, width, height, pixels, width * 4);
}

/* A results graph column: one opaque FILL per drawn segment, in the packed colour (a framebuffer pixel, already
   through the pack tables, so it is written as it is). Neighbouring columns with the same segments widen the
   previous column's fills instead of adding new ones. */
void RecordFillColumnSegments(int32_t topY, int32_t drawX, uint32_t segmentCount, const int32_t *segmentHeights,
                              const uint32_t *packedColors, SoftwareFramebufferAccess *framebuffer)
{
    if (!IsDisplay(framebuffer)) {
        SoftwareFramebuffer_FillColumnSegments32(topY, drawX, segmentCount, segmentHeights, packedColors,
                                                 framebuffer);
        return;
    }
    int64_t total = 0;
    std::size_t drawn = 0;
    for (uint32_t segment = 0; segment < segmentCount; segment++) {
        if (segmentHeights[segment] < 0) {
            return; /* the software loop would run away; never happens with a valid span */
        }
        total += segmentHeights[segment];
        drawn += segmentHeights[segment] != 0 ? 1 : 0;
    }
    if (total <= 0 || (int64_t)topY + total > INT32_MAX) {
        return;
    }
    const int32_t displayWidth = (int32_t)g_DisplayFramebufferAccess.width;
    const int32_t displayHeight = (int32_t)g_DisplayFramebufferAccess.height;
    /* widen the previous column's fills when every drawn segment matches one of them (rows and colour) */
    ColumnRun &run = s_columnRun;
    if (run.open && run.nextX == drawX && run.count == drawn && run.firstItem + run.count == s_items.size() &&
        drawX >= 0 && drawX < displayWidth) {
        bool matches = true;
        std::size_t index = run.firstItem;
        int32_t y = topY;
        for (uint32_t segment = 0; segment < segmentCount && matches; segment++) {
            const int32_t height = segmentHeights[segment];
            if (height == 0) {
                continue;
            }
            const Draw2DItem &item = s_items[index++];
            const int32_t top = y < 0 ? 0 : y;
            const int32_t bottom = y + height > displayHeight ? displayHeight : y + height;
            matches = item.dst[1] == top && item.dst[3] == bottom && item.dst[2] == drawX &&
                      item.tintArgb == (packedColors[segment] | ARGB8888_ALPHA_MASK);
            y += height;
        }
        if (matches) {
            for (std::size_t item = run.firstItem; item < run.firstItem + run.count; item++) {
                s_items[item].dst[2] = drawX + 1;
                s_items[item].clip[2] = drawX + 1;
            }
            run.nextX = drawX + 1;
            return;
        }
    }
    run.open = false;
    const std::size_t firstItem = s_items.size();
    int32_t y = topY;
    for (uint32_t segment = 0; segment < segmentCount; segment++) {
        const int32_t height = segmentHeights[segment];
        if (height == 0) {
            continue;
        }
        int32_t left = drawX;
        int32_t top = y;
        int32_t right = drawX + 1;
        int32_t bottom = y + height;
        y += height;
        if (!ClipToDisplay(INT32_MAX, INT32_MAX, INT32_MIN, INT32_MIN, &left, &top, &right, &bottom)) {
            return; /* off the display (never for a valid graph): no run to widen */
        }
        Draw2DItem *item = AppendItem(DRAW2D_OP_FILL, DRAW2D_BLEND_OPAQUE);
        item->tintArgb = packedColors[segment] | ARGB8888_ALPHA_MASK;
        SetRect(item->dst, left, top, right, bottom);
        SetRect(item->clip, left, top, right, bottom);
    }
    run.open = true;
    run.firstItem = firstItem;
    run.count = s_items.size() - firstItem;
    run.nextX = drawX + 1;
}

/* The software function's early outs (nothing is drawn): the image region is recorded only when it draws. */
bool GreyScaleImageDraws(uint32_t subresourceA, uint32_t subresourceB, const GraphicsTextureSourceAsset *asset)
{
    if (asset == nullptr || asset->common.magic != ASSET_MAGIC_GFX ||
        subresourceB >= asset->tableDescriptor.subresourceCount ||
        subresourceA >= asset->tableDescriptor.subresourceCount) {
        return false;
    }
    const GraphicsTextureSourceEntry *entries =
        (const GraphicsTextureSourceEntry *)((const uint8_t *)asset + asset->tableDescriptor.subresourceTableOffset);
    return entries[subresourceA].paletteIndex >= 0 && entries[subresourceB].paletteIndex >= 0;
}

void RecordGreyScaleImage(GraphicsPixelDimension destinationHeight, GraphicsPixelDimension destinationWidth,
                          GraphicsScreenCoordinate destinationTop, GraphicsScreenCoordinate destinationLeft,
                          uint64_t *blendedSourcePixels, uint64_t *blendFactorPixels,
                          GraphicsSubresourceIndex sourceSubresourceIndexA,
                          GraphicsSubresourceIndex sourceSubresourceIndexB, int *graphicsTextureAsset,
                          int *framebufferAccess)
{
    if (!IsDisplay((const SoftwareFramebufferAccess *)framebufferAccess) ||
        !GreyScaleImageDraws(sourceSubresourceIndexA, sourceSubresourceIndexB,
                             (const GraphicsTextureSourceAsset *)graphicsTextureAsset) ||
        destinationWidth < 2 || destinationHeight < 2 || destinationWidth > 16384 || destinationHeight > 16384) {
        /* other destinations, and the cases the software function handles by itself (sizes 0 and 1 loop or divide
           by zero there) */
        SoftwareTexture_BilinearBlendScaleSubresources(destinationHeight, destinationWidth, destinationTop,
                                                       destinationLeft, blendedSourcePixels, blendFactorPixels,
                                                       sourceSubresourceIndexA, sourceSubresourceIndexB,
                                                       graphicsTextureAsset, framebufferAccess);
        return;
    }
    /* The cross-fade into blendedSourcePixels stays on the CPU (the control keeps that state between ticks; the
       factor is a per-pixel mask, credits_mask.cpp); its grey levels are uploaded once and scaled on the GPU. The
       software scale shows intensity 255 - sample (g_SoftwarePixelIntensityToNativeColorLut256 runs from white to
       black), so the texels are inverted here: the GPU's bilinear sample of 255 - b is 255 - (sample of b), up to
       the weights' rounding. */
    const GraphicsTextureSourceAsset *asset = (const GraphicsTextureSourceAsset *)graphicsTextureAsset;
    SoftwareTexture_CrossFadeSubresources(blendedSourcePixels, blendFactorPixels, sourceSubresourceIndexA,
                                          sourceSubresourceIndexB, asset);
    const GraphicsTextureSourceEntry *entryB =
        (const GraphicsTextureSourceEntry *)((const uint8_t *)asset + asset->tableDescriptor.subresourceTableOffset) +
        sourceSubresourceIndexB;
    const int32_t width = (int32_t)entryB->pixelWidth;
    const int32_t height = (int32_t)entryB->pixelHeight;
    uint32_t *pixels = AcquireScratch((std::size_t)width * (std::size_t)height);
    const uint8_t *blended = (const uint8_t *)blendedSourcePixels;
    for (std::size_t index = 0; index < (std::size_t)width * (std::size_t)height; index++) {
        pixels[index] = ARGB8888_ALPHA_MASK | (uint32_t)(255 - blended[index]) * 0x010101u;
    }
    RecordImageBilinear(destinationLeft, destinationTop, (int32_t)destinationWidth, (int32_t)destinationHeight,
                        (int32_t)destinationWidth, pixels, width, height, width * 4);
}

} // namespace

void Draw2D_InstallSlots()
{
    if (s_backend == DRAW2D_BACKEND_GPU_RECORD) {
        g_GraphicsTextureSourceBlitSourceAlpha = RecordBlitSourceAlpha;
        g_GraphicsTextureSourceBlitHalfSourceRgb = RecordBlitHalfSourceRgb;
        g_GraphicsTextureSourceStretchDirectColorBilinear = RecordStretchDirectColorBilinear;
        g_GraphicsTextureSourceBlitModulatedSourceAlpha = RecordBlitModulatedSourceAlpha;
        g_GraphicsFramebufferFillRectArgb = RecordFillRectArgb;
        g_GraphicsMinimapDraw = RecordMinimapDraw;
        g_GraphicsFillColumnSegments = RecordFillColumnSegments;
        g_GraphicsGreyScaleImage = RecordGreyScaleImage;
        return;
    }
    g_GraphicsTextureSourceBlitSourceAlpha = SoftwareTextureSource_BlitSourceAlpha32;
    g_GraphicsTextureSourceBlitHalfSourceRgb = SoftwareTextureSource_BlitHalfSourceRgb32;
    g_GraphicsTextureSourceStretchDirectColorBilinear = SoftwareTextureSource_StretchDirectColorBilinear32;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha = SoftwareTextureSource_BlitModulatedSourceAlpha32;
    g_GraphicsFramebufferFillRectArgb = SoftwareFramebuffer_FillRectArgb32;
    g_GraphicsMinimapDraw = SoftwareTexture_DrawMinimapBilinear32;
    g_GraphicsFillColumnSegments = SoftwareFramebuffer_FillColumnSegments32;
    g_GraphicsGreyScaleImage = SoftwareTexture_BilinearBlendScaleSubresources;
}

void Draw2D_SetBackend(Draw2DBackend backend)
{
    s_backend = backend;
    Draw2D_BeginFrame();
    Draw2D_InstallSlots();
}

Draw2DBackend Draw2D_GetBackend()
{
    return s_backend;
}

void Draw2D_BeginFrame()
{
    s_items.clear();
    s_scratchUsed = 0;
    s_columnRun.open = false;
}

const Draw2DItem *Draw2D_FrameItems(uint32_t *outCount)
{
    *outCount = (uint32_t)s_items.size();
    return s_items.empty() ? nullptr : s_items.data();
}

void Draw2D_EndFrame()
{
    s_columnRun.open = false;
}

void Draw2D_MarkExternal3D(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX)
{
    if (s_backend != DRAW2D_BACKEND_GPU_RECORD) {
        return;
    }
    int32_t left = clipMinX;
    int32_t top = clipMinY;
    int32_t right = clipMaxX;
    int32_t bottom = clipMaxY;
    if (!ClipToDisplay(INT32_MAX, INT32_MAX, INT32_MIN, INT32_MIN, &left, &top, &right, &bottom)) {
        return;
    }
    Draw2DItem *item = AppendItem(DRAW2D_OP_EXTERNAL_3D, DRAW2D_BLEND_OPAQUE);
    item->tintArgb = 0;
    SetRect(item->dst, left, top, right, bottom);
    SetRect(item->clip, left, top, right, bottom);
}
