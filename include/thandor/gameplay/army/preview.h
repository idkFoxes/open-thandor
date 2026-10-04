/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/preview.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_PREVIEW_H
#define THANDOR_GAMEPLAY_ARMY_PREVIEW_H

#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* ArmyRuntime_RenderPreviewTexture: the temporary preview army is created far off the field at this world
   position (both axes, Q12), and the preview texture allocation is its pixels behind a 0x200-byte gfx header
   and one 0x20-byte subresource record */
#define ARMY_PREVIEW_WORLD_POSITION_Q12 0x6000000
#define ARMY_PREVIEW_TEXTURE_HEADER_BYTES 0x220
/* ArmyRuntime_RenderPreviewTexture view setup: auxiliary orientation angles (135 and -36 degrees), the two
   scene colours (opaque light and dark grey), view angle 0 (-180 degrees) */
#define ARMY_PREVIEW_AUXILIARY_ORIENTATION0_ANGLE16 (3 * FIXED_ANGLE16_EIGHTH_TURN)
#define ARMY_PREVIEW_AUXILIARY_ORIENTATION1_ANGLE16 (0U - FIXED_ANGLE16_FULL_TURN / 10)
#define ARMY_PREVIEW_PRIMARY_COLOR_ARGB 0xffc0c0c0
#define ARMY_PREVIEW_SECONDARY_COLOR_ARGB 0xff606060
#define ARMY_PREVIEW_VIEW_ANGLE0 (0U - FIXED_ANGLE16_HALF_TURN)
/* a byte * this repeats it in both halves of a 16-bit MMX lane (PUNPCKLBW mm,mm) */
#define ARMY_PREVIEW_BYTE_TO_WORD_REPEAT 0x101u

GraphicsTextureResource *ArmyRuntime_RenderPreviewTexture
          (GraphicsPixelDimension previewHeight,GraphicsPixelDimension previewWidth,
          FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime);

extern const uint32_t g_InGamePointerModePreviewArmyIds[8]; /* uint32_t[8]: preview army asset id per pointer mode (0 = none); ui/ingame/world_input.cpp */

/* Army ids from 400 up are rendered in their preview with faction 0 (ArmyAssetRegistry_ResolveOrCreatePreviewTexture). */
#define ARMY_ASSET_NEUTRAL_PREVIEW_FIRST_ID 400

void ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected(uint32_t uiRootAddress);

uintptr_t ArmyAssetRegistry_ResolveOrCreatePreviewTexture(uint32_t armyAssetRegistryId);

#endif /* THANDOR_GAMEPLAY_ARMY_PREVIEW_H */
