/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/campaign_level_info.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* open-thandor (campaign map, docs/plans/campaign_map.md 5.4): reads the title index and the win selections of
   every level of a campaign for the mission map. Everything here is native memory and native file calls: the
   arena (g_MemoryApi) must not see the map, or the arena order of a later session would shift (plan section 3).
   The package path is the one CM2 measured as arena-free: Package_FindEntryAcrossMounts (no allocation) plus
   Package_DecodeEntryInto into a std::vector for compression methods 0 and 1 (only method 2, the field grid
   decoder, allocates). */

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <thandor/ui/frontend/campaign_level_info.h>
#include <thandor/thandor.h>
#include <thandor/assets/package/runtime.h>
#include <thandor/core/text/path.h>
#include <thandor/gameplay/session/level_script.h>
#include <thandor/gameplay/session/scenario_load.h>
#include <thandor/gameplay/session/types.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/filesystem/win32.h>

/* The LEV offsets of 5.4 ("Measured (CM2)"): the level loader copies the LEV prefix unchanged into
   InGameLevelConditionStorage, so file offsets equal struct offsets. */
static_assert(offsetof(LevelAssetHeader,titleTextResourceIndex) == 0x170);
static_assert(offsetof(InGameLevelConditionStorage,schedule) == 0x380);
static_assert(offsetof(InGameLevelConditionStorage,schedule) + offsetof(InGameConditionSchedule,triggers) == 0x780);
static_assert(sizeof(InGameLevelConditionStorage) == 0x800);

/* end selections a campaign record has successors for (CampaignLevelRecord.successorLevelIds) */
static constexpr uint32_t CAMPAIGN_LEVEL_SELECTION_COUNT =
    sizeof(CampaignLevelRecord::successorLevelIds) / sizeof(CampaignLevelRecord::successorLevelIds[0]);
/* code units of CampaignLevelRecord.levelFileName */
static constexpr size_t CAMPAIGN_LEVEL_FILE_NAME_UNITS =
    sizeof(CampaignLevelRecord::levelFileName) / sizeof(CampaignLevelRecord::levelFileName[0]);
/* InGameEndConditionTriggerRecord8.movieVariantSelector of a win (the success movie, plan 4.3) */
static constexpr uint8_t CAMPAIGN_LEVEL_WIN_MOVIE_VARIANT = 1;

/* One cached level file. triggers keeps (faction, selection, variant) of the active end triggers for the
   dev-tools dump. */
struct CampaignLevelCacheEntry {
    CampaignLevelFileInfo info;
    std::vector<InGameEndConditionTriggerRecord8ReferenceView> activeTriggers;
};

/* Module data: the cache, keyed by the lowercased level path (level\<name>.lev). */
static std::map<std::u16string,CampaignLevelCacheEntry> s_CampaignLevelCache;

/* The decoded LEV image of a package entry, or nullopt; *outFound tells whether a mounted package has the entry. */
static std::optional<std::vector<uint8_t>> CampaignLevelInfo_ReadPackageEntry(const uint16_t *levelPath,
                                                                              bool *outFound)
{
  uint16_t lookupPath[THANDOR_PATH_CAPACITY];
  EngineFileHandle fileHandle = 0;
  uint32_t decodedByteCount = 0;
  uint32_t decodeErrorCode = 0;

  /* Package_FindEntryAcrossMounts lowercases the path it is given in place (original quirk): pass a copy */
  size_t unitIndex = 0;
  for (; unitIndex + 1 < THANDOR_PATH_CAPACITY && levelPath[unitIndex] != 0; unitIndex++) {
    lookupPath[unitIndex] = levelPath[unitIndex];
  }
  lookupPath[unitIndex] = 0;
  *outFound = false;
  PckEntryHeader *entry = Package_FindEntryAcrossMounts(lookupPath,&fileHandle);
  if (entry == nullptr) {
    return std::nullopt;
  }
  *outFound = true;
  /* only the arena-free decoders (method 2 allocates from the arena); the guards of Package_LoadEntryIntoBuffer:
     the packed bytes are staged in g_PackageScratchBuffer and the destination is our own vector */
  if (entry->compressionMethod != PCK_COMPRESSION_HUFFMAN_RLE && entry->compressionMethod != PCK_COMPRESSION_STORED) {
    Thandor_Log("campaign level info: \"%ls\" has compression method %d, not read",
                reinterpret_cast<const wchar_t *>(levelPath),(int)entry->compressionMethod);
    return std::nullopt;
  }
  if (entry->packedSize > (uint32_t)PACKAGE_SCRATCH_BUFFER_BYTES ||
      entry->unpackedSize > (uint32_t)PACKAGE_SCRATCH_BUFFER_BYTES) {
    Thandor_Log("campaign level info: \"%ls\" is too large (packed %u, unpacked %u bytes), not read",
                reinterpret_cast<const wchar_t *>(levelPath),(unsigned)entry->packedSize,
                (unsigned)entry->unpackedSize);
    return std::nullopt;
  }
  /* the stored decoder writes whole dwords, the Huffman decoder exactly unpackedSize bytes */
  std::vector<uint8_t> bytes((entry->unpackedSize + 3) & PACKAGE_DWORD_ALIGN_MASK);
  if (bytes.empty() ||
      !Package_DecodeEntryInto(bytes.data(),entry,fileHandle,&decodedByteCount,&decodeErrorCode)) {
    Thandor_Log("campaign level info: decoding \"%ls\" failed (error 0x%08X)",
                reinterpret_cast<const wchar_t *>(levelPath),decodeErrorCode);
    return std::nullopt;
  }
  bytes.resize(entry->unpackedSize); /* the image size, as Package_LoadEntryWithSize reports it */
  return bytes;
}

/* The loose level file, tried next to the executable first and then as given, like
   FileSystem_LoadWholeFileNearExecutable (the fallback of Package_LoadEntryWithSize). */
static std::optional<std::vector<uint8_t>> CampaignLevelInfo_ReadLooseFile(uint16_t *levelPath)
{
  uint16_t nearExecutablePath[THANDOR_PATH_CAPACITY];

  WidePath_CombineDirectoryAndLeaf(nearExecutablePath,levelPath,g_ExecutableDirectoryUtf16);
  std::optional<std::vector<uint8_t>> bytes = FileSystem_ReadWholeFileNative(nearExecutablePath);
  if (!bytes) {
    bytes = FileSystem_ReadWholeFileNative(levelPath);
  }
  return bytes;
}

/* Fills info (and the trigger list) from a decoded LEV image, with the checks of the level loaders
   (level.cpp LevelPackage_ValidateAndMount, NewLevel_ValidateImage, FrontendLevelAsset_LoadedImageFits). */
static void CampaignLevelInfo_ParseImage(const std::vector<uint8_t> &image,const uint16_t *levelPath,
                                         CampaignLevelCacheEntry *entry)
{
  if (image.size() < sizeof(InGameLevelConditionStorage)) {
    Thandor_Log("campaign level info: \"%ls\" has %u bytes, too short for a level",
                reinterpret_cast<const wchar_t *>(levelPath),(unsigned)image.size());
    return;
  }
  /* copy the prefix out of the byte vector (no alignment or aliasing assumptions) */
  InGameLevelConditionStorage storage;
  std::memcpy(&storage,image.data(),sizeof storage);
  const LevelAssetHeader &header = storage.levelImage.header;
  const uint32_t prefixByteSize = header.resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset;
  const uint32_t imageByteSize = header.common.allocationSizeBytes;
  if (header.common.magic != ASSET_MAGIC_LEV || header.common.converterVersion != PCK_CONVERTER_LEV_00070001 ||
      prefixByteSize < sizeof(InGameLevelConditionStorage) || prefixByteSize > imageByteSize ||
      imageByteSize > image.size()) {
    Thandor_Log("campaign level info: \"%ls\": magic 0x%08X, converter 0x%08X, prefix 0x%X, image 0x%X of "
                "0x%X bytes; not a valid level",reinterpret_cast<const wchar_t *>(levelPath),
                (unsigned)header.common.magic,(unsigned)header.common.converterVersion,prefixByteSize,
                imageByteSize,(unsigned)image.size());
    return;
  }
  entry->info.headerValid = true;
  entry->info.titleTextIndex = (int32_t)header.titleTextResourceIndex;
  /* reject what the game would reject when it loads the level (InGameLevelScript_Validate logs the reason) */
  if (!InGameLevelScript_Validate(&storage.schedule)) {
    Thandor_Log("campaign level info: \"%ls\": level script rejected, win selections unknown",
                reinterpret_cast<const wchar_t *>(levelPath));
    return;
  }
  entry->info.triggersValid = true;
  for (const InGameEndConditionTriggerRecord8ReferenceView &trigger : storage.schedule.triggers) {
    if (trigger.stateFlags != INGAME_END_CONDITION_TRIGGER_ACTIVE) {
      continue;
    }
    entry->activeTriggers.push_back(trigger);
    if (trigger.movieVariantSelector == CAMPAIGN_LEVEL_WIN_MOVIE_VARIANT &&
        trigger.endMovieSelectionIndex < CAMPAIGN_LEVEL_SELECTION_COUNT) {
      entry->info.winSelectionMask |= 1u << trigger.endMovieSelectionIndex;
    }
  }
}

/* Reads one level file (package entry, else loose file) into a new cache entry. */
static CampaignLevelCacheEntry CampaignLevelInfo_ReadLevel(uint16_t *levelPath)
{
  CampaignLevelCacheEntry entry;
  bool foundInPackage = false;

  std::optional<std::vector<uint8_t>> image = CampaignLevelInfo_ReadPackageEntry(levelPath,&foundInPackage);
  if (foundInPackage) {
    entry.info.fileExists = true;
  }
  else {
    image = CampaignLevelInfo_ReadLooseFile(levelPath);
    entry.info.fileExists = image.has_value();
  }
  if (image) {
    CampaignLevelInfo_ParseImage(*image,levelPath,&entry);
  }
  return entry;
}

#ifdef THANDOR_DEV_TOOLS
/* Dev-tools dump (OPEN_THANDOR_CAMPAIGN_LEVEL_INFO_DUMP=1): one line per level, laid out like
   tools/data/cgn.py dump --triggers so the two can be compared. */
static void CampaignLevelInfo_Dump(const CampaignAsset *campaign,const CampaignLevelCacheEntry &entry,
                                   const uint16_t *levelFileName)
{
  char triggerText[16 * 12 + 1];
  size_t used = 0;

  triggerText[0] = 0;
  /* sorted, without duplicates, as cgn.py prints them */
  std::vector<uint32_t> keys;
  for (const InGameEndConditionTriggerRecord8ReferenceView &trigger : entry.activeTriggers) {
    keys.push_back((uint32_t)trigger.factionRuntimeIndex << 16 | (uint32_t)trigger.endMovieSelectionIndex << 8 |
                   trigger.movieVariantSelector);
  }
  std::sort(keys.begin(),keys.end());
  keys.erase(std::unique(keys.begin(),keys.end()),keys.end());
  for (uint32_t key : keys) {
    int written = snprintf(triggerText + used,sizeof triggerText - used,"%s(%u,%u,%u)",used != 0 ? " " : "",
                           key >> 16,key >> 8 & 0xff,key & 0xff);
    if (written < 0 || (size_t)written >= sizeof triggerText - used) {
      break;
    }
    used += (size_t)written;
  }
  Thandor_Log("campaign level info: first %d, id %2d %-28ls exists %d header %d script %d title %d win 0x%02X "
              "triggers (faction, selection, variant): %s",campaign->firstLevelId,entry.info.levelId,
              reinterpret_cast<const wchar_t *>(levelFileName),entry.info.fileExists,entry.info.headerValid,
              entry.info.triggersValid,entry.info.titleTextIndex,entry.info.winSelectionMask,triggerText);
}
#endif

std::vector<CampaignLevelFileInfo> CampaignLevelInfo_Read(const CampaignAsset *campaign)
{
  std::vector<CampaignLevelFileInfo> result;

  if (campaign == nullptr || campaign->levelRecordCount < 1) {
    return result;
  }
#ifdef THANDOR_DEV_TOOLS
  const bool dump = getenv("OPEN_THANDOR_CAMPAIGN_LEVEL_INFO_DUMP") != nullptr;
#endif
  result.reserve((size_t)campaign->levelRecordCount);
  for (int32_t recordIndex = 0; recordIndex < campaign->levelRecordCount; recordIndex++) {
    const CampaignLevelRecord &record = campaign->levels[recordIndex];
    /* level\<levelFileName>.lev as FrontendScenarioSession_LoadOrRequestCampaignBundle builds it; the record's
       name is copied into a terminated buffer first because the file field need not hold a terminator */
    uint16_t levelFileName[CAMPAIGN_LEVEL_FILE_NAME_UNITS + 1] = {};
    std::memcpy(levelFileName,record.levelFileName,sizeof record.levelFileName);
    uint16_t levelPath[THANDOR_PATH_CAPACITY] = {};
    WidePath_CombineDirectoryAndLeaf(levelPath,levelFileName,g_ScenarioLevelDirectoryUtf16);
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_LEV,levelPath,THANDOR_PATH_CAPACITY);

    std::u16string key;
    for (const uint16_t *unit = levelPath; *unit != 0; unit++) {
      key.push_back((char16_t)((*unit >= 'A' && *unit <= 'Z') ? *unit + ('a' - 'A') : *unit));
    }
    auto cached = s_CampaignLevelCache.find(key);
    if (cached == s_CampaignLevelCache.end()) {
      cached = s_CampaignLevelCache.emplace(key,CampaignLevelInfo_ReadLevel(levelPath)).first;
    }
    CampaignLevelFileInfo info = cached->second.info;
    info.levelId = record.levelId; /* the same file may serve records of several campaigns */
    result.push_back(info);
#ifdef THANDOR_DEV_TOOLS
    if (dump) {
      CampaignLevelCacheEntry shown = cached->second;
      shown.info.levelId = record.levelId;
      CampaignLevelInfo_Dump(campaign,shown,levelFileName);
    }
#endif
  }
  return result;
}

void CampaignLevelInfo_ClearCache()
{
  s_CampaignLevelCache.clear();
}

std::optional<std::vector<uint8_t>> CampaignLevelInfo_ReadEntryNative(uint16_t *path)
{
  bool foundInPackage = false;

  std::optional<std::vector<uint8_t>> bytes = CampaignLevelInfo_ReadPackageEntry(path,&foundInPackage);
  if (foundInPackage) {
    return bytes;
  }
  return CampaignLevelInfo_ReadLooseFile(path);
}
