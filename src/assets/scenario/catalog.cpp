/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/scenario/catalog.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/scenario/catalog.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

/* UTF-16 L"level\\*.lev" after the save pattern; no code reference found */
THANDOR_ALIGN(4) uint16_t g_UnreferencedLevelPatternUtf16[12] = {'l', 'e', 'v', 'e', 'l', '\\', '*', '.', 'l', 'e', 'v', 0};

/* UTF-16 L"level\\*.cgn"; no code reference found */
THANDOR_ALIGN(4) uint16_t g_UnreferencedCampaignPatternUtf16[12] = {'l', 'e', 'v', 'e', 'l', '\\', '*', '.', 'c', 'g', 'n', 0};

static uint16_t g_LevelLevelDatPathUtf16[16] = {'l', 'e', 'v', 'e', 'l', '\\', 'l', 'e', 'v', 'e', 'l', '.', 'd', 'a', 't', 0}; /* L"level\\level.dat" */

static ScenarioLevelDataPathTemplate24 g_ScenarioLevelDataPathTemplateUtf16 = {
    .prefixCodeUnits = {0x6C, 0x65, 0x76, 0x65, 0x6C, 0x5C, 0x6C, 0x65, 0x76, 0x65, 0x6C},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = {'.', 'd', 'a', 't', 0}}; /* L".dat" */

static uint16_t g_LevelCampagneDatPathUtf16[19] = {'l', 'e', 'v', 'e', 'l', '\\', 'c', 'a', 'm', 'p', 'a', 'g', 'n', 'e', '.', 'd', 'a', 't', 0}; /* L"level\\campagne.dat" */

static ScenarioCampaignDataPathTemplate2A g_ScenarioCampaignDataPathTemplateUtf16 = {
    .prefixCodeUnits = {0x6C, 0x65, 0x76, 0x65, 0x6C, 0x5C, 0x63, 0x61, 0x6D, 0x70, 0x61, 0x67, 0x6E, 0x65},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = {'.', 'd', 'a', 't', 0}}; /* L".dat" */

ScenarioCatalogHeader *g_ScenarioCatalog = 0;

uint32_t g_ScenarioCatalogUsedBytes = 0;

uint16_t g_SaveSvePatternUtf16[11] = {'s', 'a', 'v', 'e', '\\', '*', '.', 's', 'v', 'e', 0}; /* L"save\\*.sve" */

/* Implementation ownership: assets/scenario/catalog. */

/* Copies a whole loaded catalog file (byteCount / 4 dwords) into a catalog section. */
static void ScenarioCatalog_CopyFileIntoSection
          (ScenarioCatalogRecord *sectionRecords,const void *fileBytes,uint32_t byteCount)
{
  uint32_t *destinationDword = (uint32_t *)sectionRecords;
  const uint32_t *sourceDword = (const uint32_t *)fileBytes;
  uint32_t dwordsRemaining;

  for (dwordsRemaining = byteCount >> 2; dwordsRemaining != 0; dwordsRemaining--) {
    *destinationDword = *sourceDword;
    sourceDword++;
    destinationDword++;
  }
}

/* Merges the add-on files <prefix>00.dat .. <prefix>99.dat of a path template into a catalog section and
   returns the new record count. The two digit code units are packed as one dword (UTF16_DIGIT_PAIR): the units
   digit counts '0'..'9', then subtracting UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP resets it to '0' and increments
   the tens digit; the loop ends when the tens digit passes '9'. */
static ScenarioCatalogRecordCount ScenarioCatalog_MergeAddOnFiles
          (uint16_t *pathTemplate,union Utf16DecimalDigitPair4 *decimalDigits,
           ScenarioCatalogRecordCount recordCount,ScenarioCatalogRecord *sectionRecords)
{
  void *loadedBuffer;
  uint32_t loadedByteCount;

  decimalDigits->packedDigits = UTF16_DIGIT_PAIR('0','0');
  do {
    if (Resource_Load(pathTemplate,&loadedBuffer,&loadedByteCount,NULL)) {
      recordCount = ScenarioCatalog_MergeRecordsByName
                        (loadedByteCount,(ScenarioCatalogRecord *)loadedBuffer,recordCount,sectionRecords);
      Resource_Release((ScenarioCatalogRecord *)loadedBuffer);
    }
    decimalDigits->codeUnits[1]++;
    if (decimalDigits->codeUnits[1] >= '9' + 1) {
      /* units digit wrapped: back to '0', tens digit + 1 (the tens digit is checked only then; before, it
         is at most '9') */
      decimalDigits->packedDigits = decimalDigits->packedDigits - UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP;
    }
  } while (decimalDigits->codeUnits[0] < '9' + 1);
  return recordCount;
}

/* Rebuilds g_ScenarioCatalog, the list behind the "Choose game" tabs: the single missions of level\level.dat
   and the campaigns of level\campagne.dat, each updated by the add-on files level00..99.dat /
   campagne00..99.dat (records merged by name), followed by the header record of every save\*.sve.
   The catalog is also what a network host sends to its clients.
*/
void ScenarioCatalog_Rebuild(void)

{
  ScenarioCatalogHeader *catalog;
  uint32_t recordCount;
  void *handle;
  uint32_t saveFilesRemaining;
  uint8_t *saveFileEntry; /* FILESYSTEM_ENUMERATION_RECORD_BYTES per entry, starting with the file name */
  ScenarioCatalogRecord *recordsBase;
  ScenarioCatalogSaveRecord *saveRecord;
  uint32_t allocationError;
  void *allocationPayload;
  uintptr_t checkedValue;
  uint32_t openError;
  Bool8 loaded;
  void *loadedBuffer;
  uint32_t loadedByteCount;
  void *handleToClose;

  g_MemoryApi.free(g_ScenarioCatalog);
  allocationError = g_MemoryApi.alloc(SCENARIO_CATALOG_CAPACITY,&allocationPayload);
  checkedValue = FatalError_ExitIfFailed(allocationError != 0 ? allocationError : (uintptr_t)allocationPayload,allocationError != 0);
  catalog = (ScenarioCatalogHeader *)checkedValue;
  g_ScenarioCatalogUsedBytes = SCENARIO_CATALOG_HEADER_SIZE;
  g_ScenarioCatalog = catalog;
  catalog->levelRecordsOffset = SCENARIO_CATALOG_HEADER_SIZE;
  catalog->campaignRecordsOffset = SCENARIO_CATALOG_HEADER_SIZE;
  catalog->saveRecordsOffset = SCENARIO_CATALOG_HEADER_SIZE;
  catalog->levelRecordCount = 0;
  catalog->campaignRecordCount = 0;
  catalog->saveRecordCount = 0;
  loaded = Resource_Load((uint16_t *)g_LevelLevelDatPathUtf16,&loadedBuffer,&loadedByteCount,NULL);
  catalog = g_ScenarioCatalog;
  if (loaded) {
    recordCount = loadedByteCount / SCENARIO_CATALOG_RECORD_SIZE;
    recordsBase = (ScenarioCatalogRecord *)
             ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->levelRecordsOffset);
    ScenarioCatalog_CopyFileIntoSection(recordsBase,loadedBuffer,loadedByteCount);
    Resource_Release((uint32_t *)loadedBuffer);
    /* level00.dat .. level99.dat */
    recordCount = ScenarioCatalog_MergeAddOnFiles
                      (g_ScenarioLevelDataPathTemplateUtf16.prefixCodeUnits,
                       &g_ScenarioLevelDataPathTemplateUtf16.decimalDigits,recordCount,recordsBase);
    /* count the records (at least one, as in the original) */
    do {
      catalog->campaignRecordsOffset = catalog->campaignRecordsOffset + SCENARIO_CATALOG_RECORD_STRIDE;
      catalog->saveRecordsOffset = catalog->saveRecordsOffset + SCENARIO_CATALOG_RECORD_STRIDE;
      catalog->levelRecordCount++;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + SCENARIO_CATALOG_RECORD_STRIDE;
      recordCount--;
    } while (recordCount != 0);
  }
  loaded = Resource_Load((uint16_t *)g_LevelCampagneDatPathUtf16,&loadedBuffer,&loadedByteCount,NULL);
  catalog = g_ScenarioCatalog;
  if (loaded) {
    recordCount = loadedByteCount / SCENARIO_CATALOG_RECORD_SIZE;
    recordsBase = (ScenarioCatalogRecord *)
             ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->campaignRecordsOffset);
    ScenarioCatalog_CopyFileIntoSection(recordsBase,loadedBuffer,loadedByteCount);
    Resource_Release((uint32_t *)loadedBuffer);
    /* campagne00.dat .. campagne99.dat */
    recordCount = ScenarioCatalog_MergeAddOnFiles
                      (g_ScenarioCampaignDataPathTemplateUtf16.prefixCodeUnits,
                       &g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits,recordCount,recordsBase);
    /* count the records (at least one, as in the original) */
    do {
      catalog->saveRecordsOffset = catalog->saveRecordsOffset + SCENARIO_CATALOG_RECORD_STRIDE;
      catalog->campaignRecordCount++;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + SCENARIO_CATALOG_RECORD_STRIDE;
      recordCount--;
    } while (recordCount != 0);
  }
  WidePath_CombineDirectoryAndLeaf
            (g_ScenarioCatalogPathScratchUtf16,(uint16_t *)g_SaveSvePatternUtf16,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  saveFilesRemaining = g_FileSystemEnumerateDirectoryOrVolumeEntries
                     (FILESYSTEM_ENUMERATE_FILES,UINT32_MAX,PACKAGE_SCRATCH_BUFFER_BYTES,g_PackageScratchBuffer,
                      (uint8_t *)g_ScenarioCatalogPathScratchUtf16);
  catalog = g_ScenarioCatalog;
  if (saveFilesRemaining != 0) {
    saveRecord = (ScenarioCatalogSaveRecord *)
                 ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->saveRecordsOffset);
    saveFileEntry = g_PackageScratchBuffer;
    do {
      WidePath_CombineDirectoryAndLeaf
                (g_ScenarioCatalogPathScratchUtf16,(uint16_t *)g_SaveDirectoryUtf16,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      WidePath_CombineDirectoryAndLeaf
                (g_ScenarioCatalogPathScratchUtf16,(uint16_t *)saveFileEntry,
                 g_ScenarioCatalogPathScratchUtf16);
      openError = g_FileSystemOpen
                         (FILESYSTEM_OPEN_EXCLUSIVE_SHARE,g_ScenarioCatalogPathScratchUtf16,
                          &handle);
      FatalError_ExitIfFailed(openError,openError != 0); /* does not return on failure */
      handleToClose = handle;
      /* The save's catalog record is the second 0x100-byte block of the file. */
      g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,SCENARIO_CATALOG_RECORD_SIZE,handle);
      g_FileSystemReadExact(SCENARIO_CATALOG_RECORD_SIZE,saveRecord,handle);
      g_FileSystemClose(handleToClose);
      /* Turn the stored level title index and the optional campaign title index (negative = none) into
         text resource ids. */
      saveRecord->levelTitleTextId = saveRecord->levelTitleTextId + TEXT_ID_LEVEL_TITLE_BASE;
      if (-1 < saveRecord->campaignTitleTextId) {
        saveRecord->campaignTitleTextId =
             saveRecord->campaignTitleTextId + TEXT_ID_CAMPAIGN_TITLE_BASE;
      }
      saveRecord++;
      catalog->saveRecordCount++;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + SCENARIO_CATALOG_RECORD_STRIDE;
      saveFileEntry = saveFileEntry + FILESYSTEM_ENUMERATION_RECORD_BYTES;
      saveFilesRemaining--;
    } while (saveFilesRemaining != 0);
  }
  return;
}

/* Handler of FRONTEND_COMMAND_STOP_ROM_TRANSITION (frontend command signature: player id and three arguments,
   all ignored): skips the running menu-room camera flight via FrontendRomTransition_RequestStop.
*/
void ScenarioCatalog_RequestRomTransitionStopCallback(uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,
                                                 uint32_t unusedArg3)

{
  FrontendRomTransition_RequestStop();
  return;
}

/* Compares the 0x40-byte identifiers of two catalog records dword by dword. */
static Bool8 ScenarioCatalog_RecordIdentifiersEqual
          (const ScenarioCatalogRecord *firstRecord,const ScenarioCatalogRecord *secondRecord)
{
  const uint32_t *firstDwords = (const uint32_t *)firstRecord->identifier;
  const uint32_t *secondDwords = (const uint32_t *)secondRecord->identifier;
  uint32_t dwordIndex;

  for (dwordIndex = 0; dwordIndex < sizeof(firstRecord->identifier) / 4; dwordIndex++) {
    if (firstDwords[dwordIndex] != secondDwords[dwordIndex]) {
      return false;
    }
  }
  return true;
}

/* Merges sourceByteCount / 0x100 catalog records into destinationRecords, matching them by their 0x40-byte
   UTF-16 identifier: a match is overwritten, a new identifier is appended. Returns the new destination
   record count. Like the original, the loops assume at least one source and one existing destination record.
*/
ScenarioCatalogRecordCount ScenarioCatalog_MergeRecordsByName
          (ScenarioCatalogSourceByteCount sourceByteCount,ScenarioCatalogRecord *sourceRecords,
          ScenarioCatalogRecordCount existingRecordCount,ScenarioCatalogRecord *destinationRecords)

{
  uint32_t sourceRecordsRemaining;
  ScenarioCatalogRecordCount destinationRecordsRemaining;
  ScenarioCatalogRecord *destinationRecordCursor;

  sourceRecordsRemaining = sourceByteCount / SCENARIO_CATALOG_RECORD_SIZE;
  do {
    /* find the destination record with the same identifier */
    destinationRecordsRemaining = existingRecordCount;
    destinationRecordCursor = destinationRecords;
    while (!ScenarioCatalog_RecordIdentifiersEqual(sourceRecords,destinationRecordCursor)) {
      destinationRecordCursor++;
      destinationRecordsRemaining--;
      if (destinationRecordsRemaining == 0) {
        /* No match: append after the existing records (the cursor is already there). */
        existingRecordCount++;
        break;
      }
    }
    /* copy the whole 0x100-byte record */
    *destinationRecordCursor = *sourceRecords;
    sourceRecords++;
    sourceRecordsRemaining--;
  } while (sourceRecordsRemaining != 0);
  return existingRecordCount;
}
