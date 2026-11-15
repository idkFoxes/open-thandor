/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/text/hd_font.cpp
 * Project code (not in the original game)
 */

/* The high-resolution game font (ui/text/hd_font.h): unpacks the generated data (src/assets/text/hd_font_data.cpp,
   format in tools/data/font_hd/make_hd_font.py), checks every glyph against the loaded font and builds one 'gfx'
   texture source per font and scale that the GPU UI texture cache takes like any other asset. Everything lives in
   std::vector memory, never in the game's memory arena (g_MemoryApi), so the arena addresses the determinism
   references depend on stay the same with the option on or off. */

#include <thandor/ui/text/hd_font.h>
#include <thandor/ui/text/font.h>
#include <thandor/thandor.h>
#include <thandor/core/settings/original_mode.h>
#include <thandor/core/settings/persistent.h>

#include <miniz.h>

#include <cstring>
#include <vector>

extern const uint8_t g_HdFontPacked[];
extern const size_t g_HdFontPackedSize;
extern const size_t g_HdFontUnpackedSize;

namespace {

constexpr uint32_t kFormatVersion = 1;
constexpr int kMinScale = 2;
constexpr int kMaxScale = 3;
constexpr uint32_t kWhite = 0x00FFFFFFu; /* the glyph texels' colour; the coverage goes into alpha */

/* One unpacked glyph set (a font at one scale) of the data. */
struct DataSet {
  uint32_t glyphCount = 0;
  const uint8_t *records = nullptr; /* glyphCount x (u32 hash, u16 width, u16 height) */
  const uint8_t *alpha = nullptr;   /* the glyphs' alpha bytes in order */
};

/* A built texture source: the asset block (header, record table, ARGB texels) of one font at one scale. */
struct BuiltSource {
  const GraphicsTextureSourceAsset *font = nullptr; /* the loaded font it was built for */
  std::vector<uint64_t> block;                      /* 8-byte aligned asset block */
  bool failed = false;
};

struct State {
  bool unpacked = false;
  bool unpackFailed = false;
  std::vector<uint8_t> data;
  DataSet sets[FONT_TEXTURE_SOURCE_COUNT][kMaxScale + 1];
  BuiltSource built[FONT_TEXTURE_SOURCE_COUNT][kMaxScale + 1];
};

State s_hd;

uint32_t ReadU32(const uint8_t *bytes)
{
  return static_cast<uint32_t>(bytes[0]) | (static_cast<uint32_t>(bytes[1]) << 8) |
         (static_cast<uint32_t>(bytes[2]) << 16) | (static_cast<uint32_t>(bytes[3]) << 24);
}

uint16_t ReadU16(const uint8_t *bytes)
{
  return static_cast<uint16_t>(bytes[0] | (bytes[1] << 8));
}

uint32_t Fnv1a(const uint8_t *bytes, size_t count, uint32_t hash)
{
  for (size_t index = 0; index < count; index++) {
    hash = (hash ^ bytes[index]) * 0x01000193u;
  }
  return hash;
}

/* Unpacks the data once and finds the sets; false (logged once) when it is broken. */
bool Unpack()
{
  if (s_hd.unpacked || s_hd.unpackFailed) {
    return s_hd.unpacked;
  }
  s_hd.unpackFailed = true;
  s_hd.data.resize(g_HdFontUnpackedSize);
  mz_ulong size = static_cast<mz_ulong>(g_HdFontUnpackedSize);
  if ((mz_uncompress(s_hd.data.data(), &size, g_HdFontPacked, static_cast<mz_ulong>(g_HdFontPackedSize)) != MZ_OK) ||
      (size != g_HdFontUnpackedSize) || (size < 12) || (std::memcmp(s_hd.data.data(), "OTHF", 4) != 0) ||
      (ReadU32(s_hd.data.data() + 4) != kFormatVersion)) {
    Thandor_Log("hd font: the glyph data cannot be unpacked; the plain font is used");
    s_hd.data.clear();
    return false;
  }
  const uint8_t *bytes = s_hd.data.data();
  const size_t end = s_hd.data.size();
  const uint32_t setCount = ReadU32(bytes + 8);
  size_t headOffset = 12;
  /* first pass: the set headers and the size of their alpha bytes */
  struct Pending {
    uint32_t font, scale, count;
    size_t records;
  };
  std::vector<Pending> pending;
  for (uint32_t set = 0; set < setCount; set++) {
    if (headOffset + 4 > end) {
      Thandor_Log("hd font: the glyph data is truncated; the plain font is used");
      return false;
    }
    Pending entry{bytes[headOffset], bytes[headOffset + 1], ReadU16(bytes + headOffset + 2), headOffset + 4};
    headOffset = entry.records + static_cast<size_t>(entry.count) * 8;
    pending.push_back(entry);
  }
  size_t alphaOffset = headOffset;
  for (const Pending &entry : pending) {
    if (alphaOffset > end) {
      break;
    }
    size_t alphaBytes = 0;
    for (uint32_t glyph = 0; glyph < entry.count; glyph++) {
      const uint8_t *record = bytes + entry.records + static_cast<size_t>(glyph) * 8;
      alphaBytes += static_cast<size_t>(ReadU16(record + 4)) * ReadU16(record + 6) * entry.scale * entry.scale;
    }
    if ((entry.font < FONT_TEXTURE_SOURCE_COUNT) && (entry.scale >= kMinScale) && (entry.scale <= kMaxScale) &&
        (alphaOffset + alphaBytes <= end)) {
      s_hd.sets[entry.font][entry.scale] = DataSet{entry.count, bytes + entry.records, bytes + alphaOffset};
    }
    alphaOffset += alphaBytes;
  }
  if (alphaOffset != end) {
    Thandor_Log("hd font: the glyph data has the wrong size; the plain font is used");
    for (auto &font : s_hd.sets) {
      for (DataSet &set : font) {
        set = DataSet{};
      }
    }
    return false;
  }
  s_hd.unpackFailed = false;
  s_hd.unpacked = true;
  return true;
}

/* The source hash of one glyph of the loaded font, as make_hd_font.py takes it: FNV-1a over the u16 width, u16
   height and the 1x alpha bytes (the palette entries' +0 ARGB alpha, or the direct-colour texels' alpha). */
uint32_t SourceGlyphHash(const GraphicsTextureSourceAsset *font, const GraphicsTextureSourceEntry &entry)
{
  const uint8_t *bytes = GraphicsTextureSource_Bytes(font);
  const uint32_t width = entry.pixelWidth;
  const uint32_t height = entry.pixelHeight;
  const uint8_t size[4] = {static_cast<uint8_t>(width), static_cast<uint8_t>(width >> 8),
                           static_cast<uint8_t>(height), static_cast<uint8_t>(height >> 8)};
  uint32_t hash = Fnv1a(size, 4, 0x811C9DC5u);
  const uint8_t *texels = bytes + entry.dataOffset;
  const size_t count = static_cast<size_t>(width) * height;
  if (entry.paletteIndex == -1) {
    for (size_t index = 0; index < count; index++) {
      hash = Fnv1a(texels + index * 4 + 3, 1, hash);
    }
    return hash;
  }
  const uint8_t *palette = bytes + GFX_ASSET_HEADER_SIZE + static_cast<size_t>(entry.paletteIndex) * GFX_PALETTE_BANK_SIZE;
  for (size_t index = 0; index < count; index++) {
    hash = Fnv1a(palette + static_cast<size_t>(texels[index]) * 8 + 3, 1, hash);
  }
  return hash;
}

/* Builds the texture source of font fontIndex at scale from the data and the loaded font. */
void Build(BuiltSource &built, uint32_t fontIndex, int scale, const GraphicsTextureSourceAsset *font)
{
  built = BuiltSource{};
  built.font = font;
  built.failed = true;
  const DataSet &set = s_hd.sets[fontIndex][scale];
  if ((set.records == nullptr) || (font->common.magic != ASSET_MAGIC_GFX)) {
    return;
  }
  const uint32_t fontGlyphs = font->tableDescriptor.subresourceCount;
  const uint32_t count = fontGlyphs < set.glyphCount ? fontGlyphs : set.glyphCount;
  const GraphicsTextureSourceEntry *fontEntries = GraphicsTextureSource_Entries(font);
  /* which glyphs match, and the texel bytes they need */
  std::vector<size_t> alphaStart(count);
  std::vector<bool> matches(count, false);
  size_t alphaOffset = 0;
  size_t texelBytes = 0;
  uint32_t matched = 0;
  for (uint32_t glyph = 0; glyph < count; glyph++) {
    const uint8_t *record = set.records + static_cast<size_t>(glyph) * 8;
    const uint32_t width = ReadU16(record + 4);
    const uint32_t height = ReadU16(record + 6);
    alphaStart[glyph] = alphaOffset;
    alphaOffset += static_cast<size_t>(width) * height * scale * scale;
    const GraphicsTextureSourceEntry &entry = fontEntries[glyph];
    if ((width == 0) || (height == 0) || (entry.pixelWidth != width) || (entry.pixelHeight != height) ||
        (entry.originX != 0) || (entry.originY != 0) ||
        ((entry.paletteIndex != -1) && (static_cast<uint32_t>(entry.paletteIndex) >= font->tableDescriptor.paletteBankCount)) ||
        (SourceGlyphHash(font, entry) != ReadU32(record))) {
      continue;
    }
    matches[glyph] = true;
    matched++;
    texelBytes += static_cast<size_t>(width) * height * scale * scale * 4;
  }
  const size_t tableOffset = GFX_ASSET_HEADER_SIZE;
  const size_t texelOffset = tableOffset + static_cast<size_t>(count) * GFX_SUBRESOURCE_RECORD_SIZE;
  const size_t blockBytes = texelOffset + texelBytes;
  if (blockBytes > 0x7FFFFFFFu) {
    return;
  }
  built.block.assign((blockBytes + 7) / 8, 0);
  auto *bytes = reinterpret_cast<uint8_t *>(built.block.data());
  auto *asset = reinterpret_cast<GraphicsTextureSourceAsset *>(bytes);
  asset->common.magic = ASSET_MAGIC_GFX;
  asset->tableDescriptor.subresourceCount = count;
  asset->tableDescriptor.paletteBankCount = 0;
  asset->tableDescriptor.subresourceTableOffset = static_cast<uint32_t>(tableOffset);
  auto *entries = reinterpret_cast<GraphicsTextureSourceEntry *>(bytes + tableOffset);
  size_t texelAt = texelOffset;
  for (uint32_t glyph = 0; glyph < count; glyph++) {
    GraphicsTextureSourceEntry &entry = entries[glyph];
    entry.paletteIndex = -1;
    entry.dataOffset = static_cast<uint32_t>(texelOffset);
    if (!matches[glyph]) {
      continue; /* 0 x 0: the texture cache refuses it, the caller draws the 1x glyph */
    }
    const uint32_t width = fontEntries[glyph].pixelWidth * static_cast<uint32_t>(scale);
    const uint32_t height = fontEntries[glyph].pixelHeight * static_cast<uint32_t>(scale);
    entry.logicalWidth = width;
    entry.logicalHeight = height;
    entry.pixelWidth = width;
    entry.pixelHeight = height;
    entry.dataOffset = static_cast<uint32_t>(texelAt);
    const uint8_t *alpha = set.alpha + alphaStart[glyph];
    for (size_t index = 0; index < static_cast<size_t>(width) * height; index++) {
      Thandor_StoreU32(bytes + texelAt + index * 4, kWhite | (static_cast<uint32_t>(alpha[index]) << 24));
    }
    texelAt += static_cast<size_t>(width) * height * 4;
  }
  built.failed = false;
  Thandor_Log("hd font: font %u at %dx: %u of %u glyphs in high resolution", fontIndex, scale, matched, fontGlyphs);
}

} // namespace

bool HdFont_Enabled()
{
  return (PersistentSettings_Read(PERSISTENT_HD_FONT_OFF, PERSISTENT_SETTING_HD_FONT) == PERSISTENT_HD_FONT_ON) &&
         OtFeature_Active(OT_FEATURE_HD_FONT);
}

const GraphicsTextureSourceAsset *HdFont_GlyphSource(const GraphicsTextureSourceAsset *fontSource, int uiScale)
{
  if ((fontSource == nullptr) || (uiScale < kMinScale) || (uiScale > kMaxScale)) {
    return nullptr;
  }
  uint32_t fontIndex = 0;
  while ((fontIndex < FONT_TEXTURE_SOURCE_COUNT) && (g_FontTextureSources[fontIndex] != fontSource)) {
    fontIndex++;
  }
  if ((fontIndex == FONT_TEXTURE_SOURCE_COUNT) || !HdFont_Enabled() || !Unpack()) {
    return nullptr;
  }
  BuiltSource &built = s_hd.built[fontIndex][uiScale];
  if (built.font != fontSource) {
    Build(built, fontIndex, uiScale, fontSource);
  }
  if (built.failed) {
    return nullptr;
  }
  return reinterpret_cast<const GraphicsTextureSourceAsset *>(built.block.data());
}
