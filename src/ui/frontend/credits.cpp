/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/credits.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/credits.h>
#include <thandor/thandor.h>
#include <thandor/graphics/resources/pcx.h>
#include <string.h>

/* Module data. */

static uint16_t g_CreditsTexturePathUtf16[22] = {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 'c', 'r', 'e', 'd', 'i', 't', 's', '.', 'g', 'f', 'x', 0}; /* L"gfx\\panel\\credits.gfx" */

/* Implementation ownership: ui/frontend/credits. */

/* Opens the credits screen (FRONTEND_PAGE_ACTION_CREDITS): loads gfx\panel\credits.gfx and two work buffers of
   its width * height bytes for the mask effect, then switches the frontend view to the credits page and hides
   the cursor. On any failure the partial resources are released and the menu stays as it was.
*/
void CreditsScreen_Open(FrontendCreditsUiStateView *frontendCreditsView)

{
  uint32_t bufferBytes;
  GraphicsTextureSourceAsset *creditsTexture;
  void *blendedBufferPayload;
  void *maskBufferPayload;
  GraphicsTextureLogicalSize textureSizeResult;

  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  (frontendCreditsView->creditsMaskRuntime).textureSource = NULL;
  (frontendCreditsView->creditsMaskRuntime).maskPixels = NULL;
  (frontendCreditsView->creditsMaskRuntime).blendedSourcePixels = 0;
  (frontendCreditsView->creditsMaskRuntime).outgoingSubresource = 0;
  (frontendCreditsView->creditsMaskRuntime).incomingSubresource = 0;
  (frontendCreditsView->creditsMaskRuntime).tickCounter = 0;
  creditsTexture = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)g_CreditsTexturePathUtf16,NULL);
  if (creditsTexture != NULL) {
    (frontendCreditsView->creditsMaskRuntime).textureSource = creditsTexture;
    textureSizeResult = g_GraphicsTextureSourceGetLogicalSize(0,creditsTexture);
    bufferBytes = textureSizeResult.logicalHeightPixels * textureSizeResult.logicalWidthPixels;
    /* The original lets the allocator store the pointer straight into maskPixels; that field is a 32-bit
       slot here, so the 64-bit pointer goes through a local. */
    if (g_MemoryApi.alloc(bufferBytes,&maskBufferPayload) == 0) {
      (frontendCreditsView->creditsMaskRuntime).maskPixels = (uint8_t *)maskBufferPayload;
      if (g_MemoryApi.alloc(bufferBytes,&blendedBufferPayload) == 0) {
        /* blendedSourcePixels is the second work buffer */
        (frontendCreditsView->creditsMaskRuntime).blendedSourcePixels = (uintptr_t)blendedBufferPayload;
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
  g_MemoryApi.free((void *)(frontendCreditsView->creditsMaskRuntime).blendedSourcePixels);
  (frontendCreditsView->creditsMaskRuntime).textureSource = NULL;
  (frontendCreditsView->creditsMaskRuntime).maskPixels = NULL;
  (frontendCreditsView->creditsMaskRuntime).blendedSourcePixels = 0;
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
}
