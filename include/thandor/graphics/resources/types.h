/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_TYPES_H
#define THANDOR_GRAPHICS_RESOURCES_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/ui/controls/types.h>

/* Types (split out by tools/dev/split_types.py). */

/* Graphics/display result records. */
typedef struct GraphicsTextureLogicalSize GraphicsTextureLogicalSize, *PGraphicsTextureLogicalSize;
typedef struct GraphicsPaletteAsset GraphicsPaletteAsset, *PGraphicsPaletteAsset;
typedef struct GraphicsTextureSet GraphicsTextureSet, *PGraphicsTextureSet;
typedef struct GraphicsPaletteAssetEntry GraphicsPaletteAssetEntry, *PGraphicsPaletteAssetEntry;
typedef struct GraphicsTextureSourceAsset GraphicsTextureSourceAsset, *PGraphicsTextureSourceAsset;
typedef struct GraphicsTextureSetEntry GraphicsTextureSetEntry, *PGraphicsTextureSetEntry;
typedef struct GeneratedAssetCommonPrefix GeneratedAssetCommonPrefix, *PGeneratedAssetCommonPrefix;
typedef struct GraphicsTextureSourceTableDescriptor GraphicsTextureSourceTableDescriptor, *PGraphicsTextureSourceTableDescriptor;
typedef struct GraphicsTextureResource GraphicsTextureResource, *PGraphicsTextureResource;
typedef struct GraphicsTextureSourceEntry GraphicsTextureSourceEntry, *PGraphicsTextureSourceEntry;
typedef struct GeneratedAssetBuildMetadata GeneratedAssetBuildMetadata, *PGeneratedAssetBuildMetadata;
typedef struct AssetBuildTimestampSet AssetBuildTimestampSet, *PAssetBuildTimestampSet;
typedef struct AssetProducerSourceNames AssetProducerSourceNames, *PAssetProducerSourceNames;
typedef struct GraphicsTexturePaletteEntry GraphicsTexturePaletteEntry, *PGraphicsTexturePaletteEntry;
typedef struct GraphicsPaletteTextureSourceAsset GraphicsPaletteTextureSourceAsset, *PGraphicsPaletteTextureSourceAsset;
typedef struct GraphicsTextureSourceLifecycleCallbackTable GraphicsTextureSourceLifecycleCallbackTable, *PGraphicsTextureSourceLifecycleCallbackTable;
typedef struct GraphicsPaletteAssetLifecycleCallbackTable GraphicsPaletteAssetLifecycleCallbackTable, *PGraphicsPaletteAssetLifecycleCallbackTable;
typedef struct GraphicsTextureSourceHeaderView GraphicsTextureSourceHeaderView, *PGraphicsTextureSourceHeaderView;
typedef struct GraphicsCapturedTextureSourceAsset GraphicsCapturedTextureSourceAsset;
typedef struct SoftwareFramebufferAccess SoftwareFramebufferAccess;

/* Logical size of one texture-source subresource (GraphicsTextureSource_GetLogicalSize); 0 x 0 for an invalid
   asset or index. */
struct GraphicsTextureLogicalSize {
    uint32_t logicalWidthPixels;
    uint32_t logicalHeightPixels;
};
using GraphicsFramebufferPresentProc = void (SoftwareFramebufferAccess * framebuffer);
using GraphicsFramebufferCaptureRegionProc = GraphicsCapturedTextureSourceAsset *(uint32_t captureHeight, uint32_t captureWidth, int32_t sourceY, int32_t sourceX);
using GraphicsFramebufferBeginAccessProc = Bool8 ();
using GraphicsFramebufferEndAccessProc = void ();
using GraphicsTextureSourceGetLogicalSizeProc = GraphicsTextureLogicalSize (uint32_t subresourceIndex, GraphicsTextureSourceAsset * sourceAsset);
using GraphicsFramebufferFillRectArgbProc = void (int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t rectMaxY, int32_t rectMaxX, int32_t rectMinY, int32_t rectMinX, uint32_t argb8888, SoftwareFramebufferAccess * framebuffer);
using GraphicsTextureSourceResolveAllocationBaseProc = GraphicsTextureSourceAsset * (GraphicsTextureSourceAsset * sourceAsset);
using GraphicsPaletteAssetResolveAllocationBaseProc = GraphicsPaletteAsset * (GraphicsPaletteAsset * paletteAsset);
using GraphicsTextureSourceBlitProc = Bool8 (int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t drawY, int32_t drawX, uint32_t subresourceIndex, GraphicsTextureSourceAsset * sourceAsset, SoftwareFramebufferAccess * framebuffer);
using GraphicsTextureSourceBlitModulatedSourceAlphaProc = Bool8 (int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t drawY, int32_t drawX, uint32_t modulationArgb8888, uint32_t subresourceIndex, GraphicsTextureSourceAsset * sourceAsset, SoftwareFramebufferAccess * framebuffer);
using GraphicsTextureSourceSaturatedAddRgbProc = Bool8 (int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t drawY, int32_t drawX, uint32_t subresourceIndex, GraphicsTextureSourceAsset * sourceAsset, SoftwareFramebufferAccess * framebuffer);
using GraphicsTextureSourceTestOpaquePixelProc = Bool8 (int32_t queryY, int32_t queryX, int32_t drawY, int32_t drawX, uint32_t subresourceIndex, GraphicsTextureSourceAsset * sourceAsset);
using GraphicsTextureSourceTiledBlitProc = void (int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t repeatEndY, int32_t repeatEndX, int32_t tileOriginY, int32_t tileOriginX, uint32_t subresourceIndex, GraphicsTextureSourceAsset * sourceAsset, SoftwareFramebufferAccess * framebuffer);
using GraphicsTextureSourceTiledSaturatedAddRgbProc = void (int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t repeatEndY, int32_t repeatEndX, int32_t tileOriginY, int32_t tileOriginX, uint32_t subresourceIndex, GraphicsTextureSourceAsset * sourceAsset, SoftwareFramebufferAccess * framebuffer);
using GraphicsTextureSourceStretchDirectColorBilinearProc = void (uint32_t destinationHeight, uint32_t destinationWidth, int32_t destinationY, int32_t destinationX, uint32_t subresourceIndex, GraphicsTextureSourceAsset * sourceAsset, SoftwareFramebufferAccess * framebuffer);
using GraphicsTextureSourceDecomposeSubresourceProc = Bool8 (uint32_t entryIndex, GraphicsTextureSourceAsset * sourceAsset, GraphicsTextureSourceAsset * * outAsset, uint32_t * outError);

using GraphicsTextureSourceLoadPackageAssetProc = GraphicsTextureSourceAsset * (uint16_t * pathUtf16, uint32_t * outError);

using AssetAllocationSizeBytes = uint32_t;

using AssetPaletteBankCount = uint32_t;

using GraphicsTextureDimensionLog2 = uint32_t;

enum {
    PCK_CONVERTER_ROM_00010005=65541,
    PCK_CONVERTER_TEC_00020000=131072,
    PCK_CONVERTER_SPR_00020007=131079,
    PCK_CONVERTER_ARM_00020008=131080,
    PCK_CONVERTER_EFF_00040007=262151,
    PCK_CONVERTER_FLD_SHT_00060006=393222,
    PCK_CONVERTER_LEV_00070001=458753,
    PCK_CONVERTER_MDL_0008000A=524298
};
using PckConverterVersion = int;

using AssetSubresourceCount = uint32_t;

using AssetDimension = uint32_t;

using GraphicsPaletteIndex = int;

using GraphicsPixelOrigin = int;

using AssetPackedDate = uint32_t;

using AssetPackedTime = uint32_t;

struct AssetProducerSourceNames {
    uint16_t producerName[32];
    uint16_t sourceName[32];
};

/* Three time/date pairs, time first: every writer stores Locale_GetPackedCurrentTime at +0x10/+0x18/+0x20 and
   Locale_GetPackedCurrentDate at +0x14/+0x1C/+0x24 (the stock PCK files hold e.g. 0x000E3904 = 14:39:04 and
   0x07D10214 = 2001-02-20 in that order). */
struct AssetBuildTimestampSet {
    AssetPackedTime timeValue0;
    AssetPackedDate dateValue0;
    AssetPackedTime timeValue1;
    AssetPackedDate dateValue1;
    AssetPackedTime timeValue2;
    AssetPackedDate dateValue2;
};

struct GeneratedAssetBuildMetadata {
    struct AssetBuildTimestampSet timestamps;
    uint8_t assetAnchor28[8]; // Asset +0x28, zero in every asset. Only its address is used, as an anchor for asset-relative offsets (assetAnchor28 + offset - 0x28 = asset + offset) where the plain (uint8_t *)asset + offset form compiles differently (GFX_ANCHORED_ASSET_BYTES).
    struct AssetProducerSourceNames names;
};

struct GeneratedAssetCommonPrefix {
    AssetMagic magic;
    AssetAllocationSizeBytes allocationSizeBytes;
    AssetFormatVersion formatVersion;
    PckConverterVersion converterVersion;
    struct GeneratedAssetBuildMetadata buildMetadata;
};

struct GraphicsPaletteAssetEntry {
    PackedArgb32 argb8888;
    PackedArgb32 alternateModulationColorArgb; // Packed ARGB modulation color consumed by the alternate model-render and terrain primitive-packet paths.
};

struct GraphicsPaletteAsset {
    AssetMagic magic;
    AssetAllocationSizeBytes allocationSizeBytes;
    uint8_t reserved08_AF[168];
    AssetPaletteBankCount paletteBankCount;
    uint8_t reservedB4_1FF[332];
    struct GraphicsPaletteAssetEntry paletteEntries[1];
};

/* One per image of a texture set (GraphicsTextureSet_Create). It held the surfaces and texture state of the
   original's hardware renderers and has no content left; it is still allocated, at the original 0x50 bytes,
   because the arena layout decides the texture-set addresses GraphicsPrimitiveQueue_RadixSortForRendering
   sorts opaque packets by. */
struct GraphicsTextureResource {
    uint8_t reserved00[0x50];
};

struct GraphicsTextureSetEntry {
    Ptr32<struct GraphicsTextureResource> texture; 
    GraphicsTextureDimensionLog2 widthLog2; 
    GraphicsTextureDimensionLog2 heightLog2; 
    Ptr32<struct GraphicsTextureSourceAsset> sourceAsset; 
    Ptr32<struct GraphicsTextureSourceEntry> sourceEntry; 
    GraphicsSubresourceIndex subresourceIndex; 
    uint32_t reserved18; 
    uint32_t reserved1C; 
};

struct GraphicsTextureSourceEntry {
    AssetDimension logicalWidth;
    AssetDimension logicalHeight;
    GraphicsPaletteIndex paletteIndex; 
    AssetRelativeOffset dataOffset;
    GraphicsPixelOrigin originX; 
    GraphicsPixelOrigin originY; 
    AssetDimension pixelWidth;
    AssetDimension pixelHeight;
};

struct GraphicsTextureSourceTableDescriptor {
    AssetSubresourceCount subresourceCount;
    AssetPaletteBankCount paletteBankCount;
    AssetRelativeOffset subresourceTableOffset;
};

struct GraphicsTextureSet {
    Ptr32<struct GraphicsTextureSourceAsset> sourceAsset; 
    uint32_t subresourceCount; 
    struct GraphicsTextureSetEntry entries[1]; 
};

struct GraphicsTextureSourceAsset {
    struct GeneratedAssetCommonPrefix common;
    struct GraphicsTextureSourceTableDescriptor tableDescriptor;
    uint32_t unusedHeaderDwordBC; // Never read; TerrainCompositeTexture_Create clears it.
    uint8_t reservedC0_FF[64];
    char unusedText[256]; // Zero-terminated text that is empty in every stock asset and never read; asset writers clear its first byte.
};

enum {
    GRAPHICS_PALETTE_TEXTURE_MAGIC_GFX=7890535
};
using GraphicsPaletteTextureAssetMagic = int;

enum {
    GRAPHICS_PALETTE_TEXTURE_FORMAT_VERSION_1=1
};
using GraphicsPaletteTextureFormatVersion = int;

using GraphicsAssetRelativeByteOffset = uint32_t;

using PackedFramebufferPixel = uint32_t;

using GraphicsAssetAllocationByteSize = uint32_t;

using GraphicsPaletteBankCount = uint32_t;

struct GraphicsTexturePaletteEntry {
    PackedArgb32 argb8888; 
    PackedFramebufferPixel framebufferPixel; 
};

struct GraphicsPaletteTextureSourceAsset {
    GraphicsPaletteTextureAssetMagic magic; 
    GraphicsAssetAllocationByteSize allocationSizeBytes; 
    GraphicsPaletteTextureFormatVersion formatVersion; 
    uint32_t reserved0C; 
    uint8_t reserved10_AF[160]; 
    GraphicsAssetSubresourceCount subresourceCount; 
    GraphicsPaletteBankCount paletteBankCount; 
    GraphicsAssetRelativeByteOffset subresourceTableOffset; 
    uint8_t reservedBC_1FF[324]; 
    struct GraphicsTexturePaletteEntry paletteEntries[1]; 
};

struct GraphicsTextureSourceLifecycleCallbackTable {
    Ptr32<void (struct GraphicsTextureSourceAsset *)> releasePackage; 
    Ptr32<GraphicsTextureSourceAsset * (struct GraphicsTextureSourceAsset *)> clone; 
    Ptr32<void (struct GraphicsTextureSourceAsset *)> releaseClone; 
};

struct GraphicsPaletteAssetLifecycleCallbackTable {
    Ptr32<void (struct GraphicsPaletteAsset *)> releasePackage; 
    Ptr32<GraphicsPaletteAsset * (struct GraphicsPaletteAsset *)> clone; 
    Ptr32<void (struct GraphicsPaletteAsset *)> releaseClone; 
};

struct GraphicsTextureSourceHeaderView {
    struct GeneratedAssetCommonPrefix common;
    struct GraphicsTextureSourceTableDescriptor tableDescriptor;
};
using GraphicsPaletteAssetLoadPackageProc = GraphicsPaletteAsset * (uint16_t * pathUtf16, uint32_t * outErrorCode);
using GraphicsPaletteAssetValidateProc = GraphicsPaletteAsset * (GraphicsPaletteAsset * paletteAsset, uint32_t * outErrorCode);
using GraphicsTextureRebuildAllProc = void __cdecl ();
using GraphicsTextureSetCreateProc = GraphicsTextureSet * (GraphicsTextureSourceAsset * sourceAsset, uint32_t * outErrorCode);
using GraphicsTextureSetDestroyProc = GraphicsTextureSourceAsset * (GraphicsTextureSet * set);
using GraphicsTextureSetLoadPackageProc = GraphicsTextureSet * (uint16_t * pathUtf16, uint32_t * outErrorCode);
using GraphicsTextureSetRefreshProc = void (uint32_t subresourceIndex, GraphicsTextureSet * set);
using GraphicsTextureSetReleasePackageProc = void (GraphicsTextureSet * set);
using GraphicsTextureSourceConvertPaletteEntriesProc = uint32_t (GraphicsPaletteTextureSourceAsset * sourceAsset);

#endif /* THANDOR_GRAPHICS_RESOURCES_TYPES_H */
