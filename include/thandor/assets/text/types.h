/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/text/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_TEXT_TYPES_H
#define THANDOR_ASSETS_TEXT_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/army/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/platform/system/types.h>

typedef struct TextResourceAssetHeader TextResourceAssetHeader, *PTextResourceAssetHeader;
typedef struct TextResourceLocaleCountHeader TextResourceLocaleCountHeader, *PTextResourceLocaleCountHeader;
typedef struct TextResourcePageBinding TextResourcePageBinding, *PTextResourcePageBinding;
typedef struct TextResourceLocaleBlockPrefix TextResourceLocaleBlockPrefix, *PTextResourceLocaleBlockPrefix;
typedef struct TextResourceOverrideTable TextResourceOverrideTable, *PTextResourceOverrideTable;

inline constexpr int TEXT_RESOURCE_MISSING_SENTINEL_0x33 = 0x33;

using TextResourceStringCount = uint32_t;

using RichTextCommandSelector = int;

using TextResourceLocaleBlockByteSize = uint32_t;

using TextResourceId = uint32_t;

using TextResourcePageIndex = uint32_t;

struct TextResourceLocaleCountHeader {
    struct GeneratedAssetCommonPrefix common;
    AssetRecordCount localeBlockCount;
};

struct TextResourceAssetHeader {
    struct TextResourceLocaleCountHeader localeCountHeader;
    uint8_t reservedB4_1FF[332];
};

struct TextResourceLocaleBlockPrefix {
    TextResourceLocaleBlockByteSize blockSizeBytes; 
    TextResourceStringCount stringCount; 
    LocaleTelephoneCountryCode countryCode; 
    uint32_t reserved0C; 
};

struct TextResourcePageBinding {
    Ptr32<struct TextResourceLocaleBlockPrefix> selectedLocaleBlock; 
    Ptr32<struct TextResourceAssetHeader> asset; 
};

struct TextResourceOverrideTable {
    uint32_t resourceIds[4096]; 
    Ptr32<uint16_t> textPointers[4096]; 
};

#endif /* THANDOR_ASSETS_TEXT_TYPES_H */
