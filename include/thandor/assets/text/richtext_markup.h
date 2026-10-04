/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/text/richtext_markup.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_TEXT_RICHTEXT_MARKUP_H
#define THANDOR_ASSETS_TEXT_RICHTEXT_MARKUP_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/text/richtext_markup. */

/* TXT2STR markup (RichTextMarkup_ParseAndBuildStringAsset): '#@'..'#~' select code page (c - '@'), which adds
   (c - '@') * RICHTEXT_MARKUP_CODE_PAGE_UNITS to the following bytes; '#!' makes '@'..'_' command code units. */
#define RICHTEXT_MARKUP_CODE_PAGE_UNITS 0x80
#define RICHTEXT_MARKUP_COMMAND_BIAS (RICHTEXT_COMMAND_FLAG - '@') /* '@' + bias = RICHTEXT_COMMAND_FLAG | 0 */
/* Code unit of the "TXT2STR: unknown character" message where the byte offset is written (+0x4C). */
#define RICHTEXT_MARKUP_ERROR_OFFSET_UNIT 38

/* Tags RichTextMarkup_ParseAndBuildStringAsset can collect (the original keeps them on the machine stack). */
#define RICHTEXT_MARKUP_TAG_LIMIT 4096

/* Functions are grouped by semantic ownership. */

Bool8 RichTextMarkup_ParseAndBuildStringAsset(uint8_t *markupBytes,void **outAsset,uint32_t *outError);

#endif /* THANDOR_ASSETS_TEXT_RICHTEXT_MARKUP_H */
