/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/credits.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/credits.h>
#include <thandor/thandor.h>
#include <thandor/graphics/resources/pcx.h>
#include <thandor/graphics/resources/texture.h>
#include <thandor/platform/bootstrap/image.h>
#include <string.h>

/* Module data. */

static uint16_t g_CreditsTexturePathUtf16[22] = {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 'c', 'r', 'e', 'd', 'i', 't', 's', '.', 'g', 'f', 'x', 0}; /* L"gfx\\panel\\credits.gfx" */

/* Highest subresource the credits screen shows: SoftwareMaskBuffer_AdvancePatternByPercentTick caps the outgoing
   and incoming indices at 13. */
static constexpr uint32_t CREDITS_LAST_SUBRESOURCE = 13;
/* SoftwareMaskBuffer_ApplyHorizontalBandBit fills 15-row bands; the pattern tick reaches band rows 0..23
   (shape steps 1..24, top-down or reversed), so the mask must be at least 24 * 15 rows high. */
static constexpr uint32_t CREDITS_MIN_HEIGHT = 24 * 15;
/* Keeps width * height (the work buffers, the mask's 32-bit pixel counts and its squared distances) far from
   overflow; the same limit as the GPU grey-scale image path. */
static constexpr uint32_t CREDITS_MAX_DIMENSION = 16384;

/* Plausibility check of the credits texture before its work buffers are allocated. The original trusts
   credits.gfx: the band reveal writes up to row 360 of a mask of subresource 0's logical size, and the
   cross-fade (SoftwareTexture_CrossFadeSubresources) walks the incoming subresource's pixel count through
   buffers of that size; bounded here because a smaller or uneven texture overruns both. So subresource 0 must
   be 1..16384 x 360..16384, and every shown subresource (0..13 as far as present) must have subresource 0's
   logical size as its pixel size. The stock credits.gfx (14 subresources, all 640 x 360) passes.
*/
static bool CreditsTexture_IsUsable(GraphicsTextureSourceAsset *creditsTexture)
{
  const GraphicsTextureLogicalSize size = g_GraphicsTextureSourceGetLogicalSize(0,creditsTexture);
  if (size.logicalWidthPixels == 0 || size.logicalWidthPixels > CREDITS_MAX_DIMENSION ||
      size.logicalHeightPixels < CREDITS_MIN_HEIGHT || size.logicalHeightPixels > CREDITS_MAX_DIMENSION) {
    Thandor_Log("credits: rejected credits.gfx, subresource 0 is %ux%u (needs 1..%u x %u..%u)",
                size.logicalWidthPixels,size.logicalHeightPixels,CREDITS_MAX_DIMENSION,CREDITS_MIN_HEIGHT,
                CREDITS_MAX_DIMENSION);
    return false;
  }
  /* a 'gfx' asset (GetLogicalSize returned a size), so its subresource table can be read */
  const GraphicsTextureSourceEntry *entries = GraphicsTextureSource_Entries(creditsTexture);
  const uint32_t subresourceCount = creditsTexture->tableDescriptor.subresourceCount;
  for (uint32_t index = 0; index < subresourceCount && index <= CREDITS_LAST_SUBRESOURCE; index++) {
    if (entries[index].pixelWidth != size.logicalWidthPixels ||
        entries[index].pixelHeight != size.logicalHeightPixels) {
      Thandor_Log("credits: rejected credits.gfx, subresource %u is %ux%u pixels, subresource 0 %ux%u",index,
                  entries[index].pixelWidth,entries[index].pixelHeight,size.logicalWidthPixels,
                  size.logicalHeightPixels);
      return false;
    }
  }
  return true;
}

/* Opens the credits screen (FRONTEND_PAGE_ACTION_CREDITS): loads gfx\panel\credits.gfx and two work buffers of
   its width * height bytes for the mask effect, then switches the frontend view to the credits page and hides
   the cursor. On any failure (including a texture rejected by CreditsTexture_IsUsable, before any allocation)
   the partial resources are released and the menu stays as it was.
*/
void CreditsScreen_Open(FrontendCreditsUiStateView *frontendCreditsView)

{
  uint32_t bufferBytes;
  GraphicsTextureSourceAsset *creditsTexture;
  void *blendedBufferPayload;
  void *maskBufferPayload;
  GraphicsTextureLogicalSize textureSizeResult;

  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  (frontendCreditsView->creditsMaskRuntime).textureSource = nullptr;
  (frontendCreditsView->creditsMaskRuntime).maskPixels = nullptr;
  (frontendCreditsView->creditsMaskRuntime).blendedSourcePixels = 0;
  (frontendCreditsView->creditsMaskRuntime).outgoingSubresource = 0;
  (frontendCreditsView->creditsMaskRuntime).incomingSubresource = 0;
  (frontendCreditsView->creditsMaskRuntime).tickCounter = 0;
  creditsTexture = g_GraphicsTextureSourceLoadPackageAsset(g_CreditsTexturePathUtf16,nullptr);
  if (creditsTexture != nullptr) {
    (frontendCreditsView->creditsMaskRuntime).textureSource = creditsTexture;
  }
  if (creditsTexture != nullptr && CreditsTexture_IsUsable(creditsTexture)) {
    textureSizeResult = g_GraphicsTextureSourceGetLogicalSize(0,creditsTexture);
    bufferBytes = textureSizeResult.logicalHeightPixels * textureSizeResult.logicalWidthPixels;
    /* The original lets the allocator store the pointer straight into maskPixels; that field is a 32-bit
       slot here, so the 64-bit pointer goes through a local. */
    if (g_MemoryApi.alloc(bufferBytes,&maskBufferPayload) == 0) {
      (frontendCreditsView->creditsMaskRuntime).maskPixels = static_cast<uint8_t *>(maskBufferPayload);
      if (g_MemoryApi.alloc(bufferBytes,&blendedBufferPayload) == 0) {
        /* blendedSourcePixels is the second work buffer */
        (frontendCreditsView->creditsMaskRuntime).blendedSourcePixels = reinterpret_cast<uintptr_t>(blendedBufferPayload);
        UiFrame_FlushInputAndResetPendingTicks();
        SoftwareMaskBuffer_Clear(&frontendCreditsView->creditsMaskRuntime);
        /* page 1 of the frontend view-mode stack: the full-screen view instead of the menu room */
        UiPageStack_SetActiveIndex(1,&frontendCreditsView->pageStack);
        g_CursorVisibilityToken--;
        return;
      }
    }
  }
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage
            ((frontendCreditsView->creditsMaskRuntime).textureSource);
  g_MemoryApi.free((frontendCreditsView->creditsMaskRuntime).maskPixels);
  g_MemoryApi.free(static_cast<void *>((frontendCreditsView->creditsMaskRuntime).blendedSourcePixels));
  (frontendCreditsView->creditsMaskRuntime).textureSource = nullptr;
  (frontendCreditsView->creditsMaskRuntime).maskPixels = nullptr;
  (frontendCreditsView->creditsMaskRuntime).blendedSourcePixels = 0;
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
}
