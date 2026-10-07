/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/pcx_preview.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CORE_PCX_PREVIEW_H
#define THANDOR_UI_CORE_PCX_PREVIEW_H

#include <thandor/core/types.h>
#include <thandor/ui/core/types.h>
#include <thandor/core/contracts.h>

/* Extension code for WidePath_SetExtensionCode (see WIDE_PATH_EXTENSION_* in core/text/path.h): ".pcx",
   the 64x64 player preview pictures of PcxPreview_Load64x64PaletteAndPixels. */
inline constexpr int32_t WIDE_PATH_EXTENSION_PCX = 0x786370;

bool PcxPreview_Load64x64PaletteAndPixels(PcxPreview64 *outputPreview,uint16_t *sourcePath);

#endif /* THANDOR_UI_CORE_PCX_PREVIEW_H */
