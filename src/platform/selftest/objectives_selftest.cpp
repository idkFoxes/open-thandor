/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/objectives_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=objectives: the mission objective model (gameplay/session/mission_objective.h,
   docs/plans/objective_overlay.md test T1).
     - unit cases: synthetic level scripts for every condition kind, postfix AND / OR / NOT, several victory
       triggers, no victory trigger; line 1 compared with the expected German (and some English) text, line 2 for
       the kinds that need no running world (faction resources, countdown). No game data needed: unit and faction
       names fall back to "Einheiten" / the faction number without loaded texts and registries.
     - stock levels: every level\*.lev entry of PATCH01.PCK, PATCH00.PCK and LEVEL.PCK in the current directory
       (first package with a path wins), decoded natively (g_MemoryApi pointed at new/delete while decoding, as the
       uiatlas test does); for each faction 1..7 with an active victory trigger MissionObjective_Build must give a
       generated or short line 1 that is not empty. One log line per level and faction (line 1 in German).
   One line per failed check and a summary line; without the packages the stock part logs "skipped". */

#include <stdio.h>
#include <string.h>

#include <memory>
#include <new>
#include <string>
#include <unordered_set>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/assets/text/resources.h>
#include <thandor/gameplay/session/level_script.h>
#include <thandor/gameplay/session/mission_objective.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>

namespace {

unsigned s_ObjectivesChecks = 0;
unsigned s_ObjectivesFailures = 0;

std::string Objectives_Utf8(const uint16_t *text)
{
  std::string result;
  for (; (text != nullptr) && (*text != 0); text++) {
    const uint16_t unit = *text;
    if (unit < 0x80) {
      result += (char)unit;
    }
    else if (unit < 0x800) {
      result += (char)(0xC0 | (unit >> 6));
      result += (char)(0x80 | (unit & 0x3F));
    }
    else {
      result += (char)(0xE0 | (unit >> 12));
      result += (char)(0x80 | ((unit >> 6) & 0x3F));
      result += (char)(0x80 | (unit & 0x3F));
    }
  }
  return result;
}

bool Objectives_Equal(const uint16_t *text, const char16_t *expected)
{
  for (;; text++, expected++) {
    if (*text != (uint16_t)*expected) {
      return false;
    }
    if (*text == 0) {
      return true;
    }
  }
}

std::string Objectives_Utf8(const char16_t *text)
{
  std::vector<uint16_t> units;
  for (; *text != 0; text++) {
    units.push_back((uint16_t)*text);
  }
  units.push_back(0);
  return Objectives_Utf8(units.data());
}

void Objectives_Check(const char *name, const uint16_t *actual, const char16_t *expected)
{
  s_ObjectivesChecks++;
  if (!Objectives_Equal(actual, expected)) {
    s_ObjectivesFailures++;
    Thandor_Log("objectives: FAIL %s: \"%s\", expected \"%s\"", name, Objectives_Utf8(actual).c_str(),
                Objectives_Utf8(expected).c_str());
  }
}

void Objectives_CheckTrue(const char *name, bool condition)
{
  s_ObjectivesChecks++;
  if (!condition) {
    s_ObjectivesFailures++;
    Thandor_Log("objectives: FAIL %s", name);
  }
}

/* A synthetic level script: conditions and victory/defeat triggers. */
struct ObjectivesScript {
  InGameLevelConditionStorage storage{};
  int triggerCount = 0;

  void Condition(int index, uint32_t kind, uint32_t op0 = 0, uint32_t op1 = 0, uint32_t op2 = 0)
  {
    storage.schedule.conditions[index].statusAndKind.raw = kind;
    storage.schedule.conditions[index].payload.operands[0] = op0;
    storage.schedule.conditions[index].payload.operands[1] = op1;
    storage.schedule.conditions[index].payload.operands[2] = op2;
  }
  /* a postfix expression; tokens end with INGAME_CONDITION_TOKEN_END (added here) */
  void Expression(int index, std::initializer_list<int> tokens)
  {
    InGameScheduledConditionRecord10 &record = storage.schedule.conditions[index];
    memset(&record, 0, sizeof record);
    uint8_t *bytes = reinterpret_cast<uint8_t *>(&record);
    bytes[0] = INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION;
    int position = 1;
    for (int token : tokens) {
      bytes[position++] = (uint8_t)token;
    }
    bytes[position] = INGAME_CONDITION_TOKEN_END;
  }
  void Trigger(uint8_t faction, uint8_t variant, uint8_t conditionIndex)
  {
    InGameEndConditionTriggerRecord8ReferenceView &trigger = storage.schedule.triggers[triggerCount++];
    trigger.stateFlags = INGAME_END_CONDITION_TRIGGER_ACTIVE;
    trigger.movieVariantSelector = variant;
    trigger.factionRuntimeIndex = faction;
    trigger.conditionIndex = conditionIndex;
  }
};

constexpr uint32_t LOCAL = 1;
constexpr int AND = INGAME_CONDITION_TOKEN_AND;
constexpr int OR = INGAME_CONDITION_TOKEN_OR;
constexpr int NOT = INGAME_CONDITION_TOKEN_NOT;

/* Builds the objective of the local faction for a script with one victory trigger on condition 0 (or the given
   script) and checks line 1 (and line 2 when expectedProgress is given). */
void Objectives_Case(const char *name, ObjectivesScript &script, const char16_t *expectedLine1,
                     const char16_t *expectedProgress = nullptr)
{
  MissionObjective objective = MissionObjective_Build(&script.storage, LOCAL);
  Objectives_Check(name, objective.line1, expectedLine1);
  if (expectedProgress != nullptr) {
    uint16_t progress[MISSION_OBJECTIVE_LINE_UNITS];
    MissionObjective_FormatProgress(&objective, progress, MISSION_OBJECTIVE_LINE_UNITS);
    std::string progressName = std::string(name) + " (progress)";
    Objectives_Check(progressName.c_str(), progress, expectedProgress);
  }
}

void Objectives_SingleCase(const char *name, uint32_t kind, uint32_t op0, uint32_t op1, uint32_t op2,
                           const char16_t *expectedLine1, const char16_t *expectedProgress = nullptr)
{
  ObjectivesScript script;
  script.Condition(0, kind, op0, op1, op2);
  script.Trigger(LOCAL, 1, 0);
  Objectives_Case(name, script, expectedLine1, expectedProgress);
}

void Objectives_UnitCases()
{
  GameFactionRuntimeRecord savedRecord = g_GameFactionRuntimeImage.records[LOCAL];
  g_GameFactionRuntimeImage.records[LOCAL].xeniteCurrentQ4 = 3200 << Q4_SHIFT;
  g_GameFactionRuntimeImage.records[LOCAL].tritiumCurrentQ4 = 800 << Q4_SHIFT;
  g_GameFactionRuntimeImage.records[LOCAL].tritiumExtractionRateQ4PerTick = 0;

  TextResource_SelectLanguage((uint32_t)LocaleTelephoneCountryCode::LOCALE_COUNTRY_GERMANY);
  Objectives_CheckTrue("German project texts", TextResource_ProjectTextsAreGerman());

  Objectives_SingleCase("no army", INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY, 2, 0, 0, u"Gegner 2 vernichten");
  Objectives_SingleCase("no group A army", INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_COMMAND_GROUP_A_ARMY, 2, 0, 0,
                        u"Alle feindlichen Gebäude zerstören");
  Objectives_SingleCase("no army of asset", INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY_OF_ASSET, 2, 0, 50,
                        u"Einheiten zerstören");
  Objectives_SingleCase("inactive or allied", INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8, 3,
                        LOCAL, 0, u"Gegner 3 ausschalten");
  Objectives_SingleCase("xenite", INGAME_SCHEDULED_CONDITION_XENITE_AT_LEAST, LOCAL, 5000 << Q4_SHIFT, 0,
                        u"Xenit sammeln", u"Xenit 3200 / 5000");
  Objectives_SingleCase("tritium", INGAME_SCHEDULED_CONDITION_TRITIUM_AT_LEAST, LOCAL, 1500 << Q4_SHIFT, 0,
                        u"Tritium sammeln", u"Tritium 800 / 1500");
  Objectives_SingleCase("tritium rate", INGAME_SCHEDULED_CONDITION_TRITIUM_EXTRACTION_RATE_AT_LEAST, LOCAL, 16, 0,
                        u"Tritium-Förderung steigern", u"Tritium 0 / 150 pro Minute");
  Objectives_SingleCase("army count", INGAME_SCHEDULED_CONDITION_ARMY_OF_ASSET_COUNT_AT_LEAST, LOCAL, 5, 50,
                        u"Einheiten bauen");
  Objectives_SingleCase("terrain", INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST,
                        LOCAL, 75, 0, u"Gebiet kontrollieren");
  Objectives_SingleCase("countdown", INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED, 0, 270, 0, u"Durchhalten",
                        u"Noch 4:30");
  Objectives_SingleCase("countdown 30 min", INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED, 0, 1800, 0, u"Durchhalten",
                        u"Noch 30:00");
  Objectives_SingleCase("xenite storage", INGAME_SCHEDULED_CONDITION_XENITE_STORAGE_LIMIT_AT_MOST_0FA0, LOCAL, 0, 0,
                        u"Xenit-Lager klein halten");
  Objectives_SingleCase("class outside group A", INGAME_SCHEDULED_CONDITION_NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A,
                        7, 0, 0, u"Einheiten zusammenhalten");
  Objectives_SingleCase("unused kind", INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED, 0, 0, 0,
                        u"Ziel siehe Auftrag (F1)");
  Objectives_SingleCase("own army gone", INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY, LOCAL, 0, 0,
                        u"Ziel siehe Auftrag (F1)");

  {
    /* the standard skirmish script of faction 1: AND over the three pairs containing it */
    ObjectivesScript script;
    script.Condition(1, INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8, 2, 1);
    script.Condition(2, INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8, 3, 1);
    script.Condition(3, INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8, 4, 1);
    script.Expression(21, {1, 2, AND, 3, AND});
    script.Condition(31, INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY, 1);
    script.Trigger(1, 0, 31);
    script.Trigger(1, 1, 21);
    Objectives_Case("skirmish", script, u"Alle Gegner ausschalten");
    TextResource_SelectLanguage((uint32_t)LocaleTelephoneCountryCode::LOCALE_COUNTRY_GREAT_BRITAIN);
    Objectives_Case("skirmish (English)", script, u"Eliminate all enemies");
    TextResource_SelectLanguage((uint32_t)LocaleTelephoneCountryCode::LOCALE_COUNTRY_GERMANY);
  }
  {
    ObjectivesScript script;
    script.Condition(1, INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY, 2);
    script.Condition(2, INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY, 3);
    script.Expression(0, {1, 2, AND});
    script.Trigger(LOCAL, 1, 0);
    Objectives_Case("no army of two enemies", script, u"Alle Gegner vernichten");
    MissionObjective objective = MissionObjective_Build(&script.storage, LOCAL);
    Objectives_CheckTrue("no army of two enemies: one progress over both",
                         objective.progress.kind == MissionObjectiveProgressKind::ENEMY_UNITS &&
                             objective.progress.factionMask == ((1u << 2) | (1u << 3)));
    if (g_InGameRuntimeRoot == nullptr) {
      uint16_t progress[MISSION_OBJECTIVE_LINE_UNITS];
      Objectives_CheckTrue("enemy units without a world: no line 2",
                           !MissionObjective_FormatProgress(&objective, progress, MISSION_OBJECTIVE_LINE_UNITS) &&
                               progress[0] == 0);
    }
  }
  {
    ObjectivesScript script;
    script.Condition(1, INGAME_SCHEDULED_CONDITION_XENITE_AT_LEAST, LOCAL, 5000 << Q4_SHIFT);
    script.Condition(2, INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED, 0, 90);
    script.Expression(0, {2, 1, AND});
    script.Trigger(LOCAL, 1, 0);
    Objectives_Case("countdown and xenite", script, u"Durchhalten und Xenit sammeln", u"Noch 1:30");
  }
  {
    ObjectivesScript script;
    script.Condition(1, INGAME_SCHEDULED_CONDITION_XENITE_AT_LEAST, LOCAL, 5000 << Q4_SHIFT);
    script.Condition(2, INGAME_SCHEDULED_CONDITION_TRITIUM_AT_LEAST, LOCAL, 1500 << Q4_SHIFT);
    script.Expression(0, {1, 2, OR});
    script.Trigger(LOCAL, 1, 0);
    Objectives_Case("xenite or tritium", script, u"Xenit sammeln oder Tritium sammeln", u"Xenit 3200 / 5000");
    script.Condition(3, INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED, 0, 90);
    script.Expression(0, {1, 2, OR, 3, OR});
    Objectives_Case("three parts", script, u"Mehrere Ziele (F1)", u"Xenit 3200 / 5000");
    script.Expression(0, {1, 2, OR, 3, AND});
    Objectives_Case("mixed operators", script, u"Mehrere Ziele (F1)");
    script.Expression(0, {1, 2, OR, NOT});
    Objectives_Case("NOT over a sub-expression", script, u"Mehrere Ziele (F1)");
    script.Expression(0, {1, NOT, NOT});
    Objectives_Case("NOT NOT x", script, u"Xenit sammeln");
    script.Expression(0, {1, NOT});
    Objectives_Case("NOT xenite", script, u"Ziel siehe Auftrag (F1)");
    /* nested expression */
    script.Expression(4, {1, 2, AND});
    script.Expression(0, {4});
    Objectives_Case("nested expression", script, u"Xenit sammeln und Tritium sammeln");
  }
  {
    ObjectivesScript script;
    script.Condition(1, INGAME_SCHEDULED_CONDITION_TRITIUM_EXTRACTION_RATE_AT_LEAST, 3, 1);
    script.Condition(2, INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY, 3);
    script.Expression(0, {1, NOT});
    script.Trigger(LOCAL, 1, 0);
    Objectives_Case("stop tritium", script, u"Tritium-Förderung von 3 stoppen");
    script.Expression(0, {1, NOT, 2, OR});
    Objectives_Case("stop tritium or destroy", script, u"Tritium-Förderung von 3 stoppen oder Gegner 3 vernichten");
  }
  {
    ObjectivesScript script;
    script.Condition(0, INGAME_SCHEDULED_CONDITION_XENITE_AT_LEAST, LOCAL, 5000 << Q4_SHIFT);
    script.Condition(1, INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY, 2);
    script.Trigger(LOCAL, 1, 0);
    script.Trigger(LOCAL, 1, 0); /* the same condition again (another end selection): not counted */
    script.Trigger(LOCAL, 1, 1);
    Objectives_Case("two victory triggers", script, u"Xenit sammeln (+1 weitere)");
    MissionObjective objective = MissionObjective_Build(&script.storage, LOCAL);
    Objectives_CheckTrue("two victory triggers: count", objective.moreTriggers == 1 &&
                                                            objective.source == MissionObjectiveSource::GENERATED &&
                                                            objective.shortTextId == TEXT_RESOURCE_ID_NONE);
  }
  {
    ObjectivesScript script;
    script.Condition(0, INGAME_SCHEDULED_CONDITION_XENITE_AT_LEAST, 2, 5000 << Q4_SHIFT);
    script.Trigger(2, 1, 0);    /* another faction's victory */
    script.Trigger(LOCAL, 0, 0); /* the local faction's defeat */
    MissionObjective objective = MissionObjective_Build(&script.storage, LOCAL);
    Objectives_CheckTrue("no victory trigger: hidden", objective.source == MissionObjectiveSource::NONE &&
                                                           objective.line1[0] == 0);
    Objectives_CheckTrue("null storage: hidden",
                         MissionObjective_Build(nullptr, LOCAL).source == MissionObjectiveSource::NONE);
  }
  TextResource_SelectLanguage((uint32_t)LocaleTelephoneCountryCode::LOCALE_COUNTRY_GREAT_BRITAIN);
  Objectives_SingleCase("countdown (English)", INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED, 0, 270, 0, u"Hold out",
                        u"4:30 left");
  Objectives_SingleCase("army of asset (English)", INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY_OF_ASSET, 2, 0, 50,
                        u"Destroy Units");
  TextResource_SelectLanguage((uint32_t)LocaleTelephoneCountryCode::LOCALE_COUNTRY_GERMANY);

  uint16_t noName[] = {'s', '0', '1', 0};
  Objectives_CheckTrue("no short objectives yet", TextResource_MissionObjective(noName, LOCAL) == TEXT_RESOURCE_ID_NONE);

  g_GameFactionRuntimeImage.records[LOCAL] = savedRecord;
}

/* ---- stock levels ---------------------------------------------------------------------------------------------- */

uint32_t ObjectivesTest_Alloc(uint32_t bytes, void **outPayload)
{
  void *payload = ::operator new(bytes, std::nothrow);
  if (payload == nullptr) {
    return FATAL_ERROR_ARENA_EXHAUSTED;
  }
  *outPayload = payload;
  return 0;
}

uint32_t ObjectivesTest_Free(void *memory)
{
  ::operator delete(memory);
  return 0;
}

struct ObjectivesLevel {
  std::string package;
  std::string path;
  std::vector<uint8_t> image;
};

/* Decodes every level\*.lev entry of one package whose path is new. */
void Objectives_LoadPackageLevels(const char *packageName, uint8_t *packed, std::unordered_set<std::string> &seenPaths,
                                  std::vector<ObjectivesLevel> &out)
{
  FILE *pck = fopen(packageName, "rb");
  if (pck == nullptr) {
    return;
  }
  long position = PCK_ENTRY_HEADER_BYTES;
  for (;;) {
    PckEntryHeader header;
    if ((fseek(pck, position, SEEK_SET) != 0) || (fread(&header, sizeof header, 1, pck) != 1)) {
      break;
    }
    if ((header.packedSize == 0) || (header.packedSize > PACKAGE_SCRATCH_BUFFER_BYTES) ||
        ((uint32_t)header.compressionMethod >= sizeof g_PckDecoderTable / sizeof g_PckDecoderTable[0])) {
      break;
    }
    position += PCK_ENTRY_HEADER_BYTES + (long)header.packedSize;
    std::string path;
    for (int k = 0; k < PCK_ENTRY_PATH_UNITS && header.path[k] != 0; k++) {
      const uint16_t unit = header.path[k];
      path += (char)((unit >= 'A' && unit <= 'Z') ? unit + ('a' - 'A') : (unit < 0x80 ? unit : '?'));
    }
    if ((path.rfind("level\\", 0) != 0) || (path.size() < 4) || (path.compare(path.size() - 4, 4, ".lev") != 0) ||
        !seenPaths.insert(path).second) {
      continue;
    }
    if ((g_PckDecoderTable[header.compressionMethod] == nullptr) ||
        (header.unpackedSize < sizeof(InGameLevelConditionStorage)) ||
        (header.unpackedSize > PACKAGE_SCRATCH_BUFFER_BYTES)) {
      s_ObjectivesChecks++;
      s_ObjectivesFailures++;
      Thandor_Log("objectives: FAIL %s %s: method %d, %u bytes, not decodable", packageName, path.c_str(),
                  (int)header.compressionMethod, (unsigned)header.unpackedSize);
      continue;
    }
    if (fread(packed, 1, header.packedSize, pck) != header.packedSize) {
      break;
    }
    std::vector<uint8_t> image(((size_t)header.unpackedSize + 4 + 3) & ~(size_t)3);
    if (!g_PckDecoderTable[header.compressionMethod](header.unpackedSize, image.data(), header.packedSize, packed,
                                                       nullptr, nullptr)) {
      s_ObjectivesChecks++;
      s_ObjectivesFailures++;
      Thandor_Log("objectives: FAIL %s %s: decoding failed", packageName, path.c_str());
      continue;
    }
    out.push_back(ObjectivesLevel{packageName, path, std::move(image)});
  }
  fclose(pck);
}

void Objectives_StockLevels()
{
  static const char *packages[] = {"PATCH01.PCK", "PATCH00.PCK", "LEVEL.PCK"};
  uint32_t (*savedAlloc)(uint32_t, void **) = g_MemoryApi.alloc;
  uint32_t (*savedFree)(void *) = g_MemoryApi.free;
  g_MemoryApi.alloc = ObjectivesTest_Alloc;
  g_MemoryApi.free = ObjectivesTest_Free;
  auto packedBuffer = std::make_unique_for_overwrite<uint8_t[]>(PACKAGE_SCRATCH_BUFFER_BYTES);
  std::unordered_set<std::string> seenPaths;
  std::vector<ObjectivesLevel> levels;
  for (const char *package : packages) {
    Objectives_LoadPackageLevels(package, packedBuffer.get(), seenPaths, levels);
  }
  g_MemoryApi.alloc = savedAlloc;
  g_MemoryApi.free = savedFree;
  if (levels.empty()) {
    Thandor_Log("objectives: stock levels skipped (no level packages in the current directory)");
    return;
  }

  unsigned objectiveCount = 0;
  unsigned generatedCount = 0;
  unsigned multipleCount = 0;
  unsigned progressCount = 0;
  for (const ObjectivesLevel &level : levels) {
    InGameLevelConditionStorage storage;
    memcpy(&storage, level.image.data(), sizeof storage);
    if (!InGameLevelScript_Validate(&storage.schedule)) {
      s_ObjectivesChecks++;
      s_ObjectivesFailures++;
      Thandor_Log("objectives: FAIL %s: level script rejected", level.path.c_str());
      continue;
    }
    uint16_t levelName[sizeof(LevelAssetHeader::levelFileNameUtf16) / sizeof(uint16_t) + 1] = {};
    memcpy(levelName, storage.levelImage.header.levelFileNameUtf16, sizeof storage.levelImage.header.levelFileNameUtf16);
    for (uint32_t faction = 1; faction < 8; faction++) {
      bool hasVictory = false;
      for (const InGameEndConditionTriggerRecord8ReferenceView &trigger : storage.schedule.triggers) {
        hasVictory = hasVictory || ((trigger.stateFlags == INGAME_END_CONDITION_TRIGGER_ACTIVE) &&
                                    (trigger.movieVariantSelector == 1) && (trigger.factionRuntimeIndex == faction));
      }
      if (!hasVictory) {
        continue;
      }
      objectiveCount++;
      MissionObjective objective = MissionObjective_Build(&storage, faction);
      uint16_t progress[MISSION_OBJECTIVE_LINE_UNITS];
      const bool hasProgress = MissionObjective_FormatProgress(&objective, progress, MISSION_OBJECTIVE_LINE_UNITS);
      generatedCount += (objective.source == MissionObjectiveSource::GENERATED) ? 1 : 0;
      progressCount += (objective.progress.kind != MissionObjectiveProgressKind::NONE) ? 1 : 0;
      uint16_t *multipleText = nullptr;
      TextResource_TryResolve(TEXT_ID_OBJECTIVE_MULTIPLE, &multipleText);
      multipleCount += (memcmp(objective.line1, multipleText, sizeof(uint16_t) * 4) == 0) ? 1 : 0;
      s_ObjectivesChecks++;
      if ((objective.source == MissionObjectiveSource::NONE) || (objective.line1[0] == 0)) {
        s_ObjectivesFailures++;
        Thandor_Log("objectives: FAIL %s faction %u: no line 1", level.path.c_str(), faction);
        continue;
      }
      Thandor_Log("objectives: %-28s (%s) faction %u: \"%s\"%s%s%s", level.path.c_str(),
                  Objectives_Utf8(levelName).c_str(), faction, Objectives_Utf8(objective.line1).c_str(),
                  hasProgress ? " / \"" : "", hasProgress ? Objectives_Utf8(progress).c_str() : "",
                  hasProgress ? "\"" : "");
    }
  }
  Thandor_Log("objectives: %u stock levels, %u faction objectives (%u generated, %u \"Mehrere Ziele\", %u with a "
              "progress kind)", (unsigned)levels.size(), objectiveCount, generatedCount, multipleCount, progressCount);
}

} // namespace

void Thandor_SelfTestObjectives()
{
  const uint32_t savedLanguage = TextResource_SelectedLanguage();
  Objectives_UnitCases();
  TextResource_SelectLanguage((uint32_t)LocaleTelephoneCountryCode::LOCALE_COUNTRY_GERMANY);
  Objectives_StockLevels();
  TextResource_SelectLanguage(savedLanguage);
  Thandor_Log("objectives: %s (%u checks, %u failed)", s_ObjectivesFailures == 0 ? "PASS" : "FAIL", s_ObjectivesChecks,
              s_ObjectivesFailures);
}
