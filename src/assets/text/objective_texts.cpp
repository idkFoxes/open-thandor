/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/objective_texts.cpp
 * Project code (not in the original game)
 */

/* The hand-written short objectives of the stock campaign missions (docs/plans/objective_overlay.md 3a, reviewed
   table docs/plans/objective_overlay_texts.md): one line per level file and playable faction, German and English
   (English for every language but German, as the other project texts). Plain text, umlauts as Latin-1 code units.
   Text ids TEXT_ID_MISSION_OBJECTIVE_SHORT_BASE + table index (resources.h). The maps of level.dat and add-on
   levels have no entry; their objective is generated from the victory triggers (plan 3b). */

#include <thandor/assets/text/resources.h>

#include <cstddef>

namespace {

/* a UTF-16 stream from a literal (code units up to 0xFF are Latin-1) */
template <size_t N>
struct ObjectiveTextUtf16 {
    uint16_t units[N];
    constexpr ObjectiveTextUtf16(const char16_t (&text)[N]) : units{}
    {
        for (size_t i = 0; i < N; i++) {
            units[i] = static_cast<uint16_t>(text[i]);
        }
    }
};

/* the factions an entry applies to (bit f = faction f) */
constexpr uint32_t FACTION_1 = 1u << 1;
constexpr uint32_t FACTIONS_1_2 = (1u << 1) | (1u << 2); /* the two-player campaigns (nimm2, luke) */

/* X(name, level base name (lower case, no directory or extension), factions, German, English) in campaign order */
#define THANDOR_MISSION_OBJECTIVES(X) \
    X(T00, u"t00_tut", FACTION_1, u"Basis aufbauen, alle Gegner vernichten", u"Build a base, destroy all enemies") \
    X(T01, u"t01_tut", FACTION_1, u"Forschen, angreifen, alle Gegner vernichten", \
      u"Research, attack and destroy all enemies") \
    X(T02, u"t02_tut", FACTION_1, u"Tritium fördern und 2 Xenit-Minen bauen", \
      u"Extract tritium and build 2 xenite mines") \
    X(S01, u"s01_insel", FACTION_1, u"Basis ausbauen und alle Gegner vernichten", \
      u"Expand the base and destroy all enemies") \
    X(S02, u"s02_schlucht", FACTION_1, u"Canyon sichern: alle Gegner vernichten", \
      u"Secure the canyon: destroy all enemies") \
    X(S03, u"s03_erobern", FACTION_1, u"Bastion vor dem Raumhafen restlos vernichten", \
      u"Wipe out the bastion before the spaceport") \
    X(S04, u"s04_konflikt", FACTION_1, u"Einen der beiden Gegner vollständig vernichten", \
      u"Wipe out one of the two enemies") \
    X(S05, u"s05_trigelb", FACTION_1, u"Tritium-Förderung im Süden wiederherstellen", \
      u"Restore tritium production in the south") \
    X(S06, u"s06_trirot", FACTION_1, u"Tritium-Förderung im Westen wiederherstellen", \
      u"Restore tritium production in the west") \
    X(S07, u"s07_landunt", FACTION_1, u"Staudamm-Gebiet erobern, Gegner dort vernichten", \
      u"Take the dam area, wipe out its defenders") \
    X(S08, u"s08_instand", FACTION_1, u"Bauen: 2 Hauptquartiere, 4 Kraftwerke, 3 Xenitminen", \
      u"Build 2 HQs, 4 power plants and 3 xenite mines") \
    X(S09, u"s09_krieg", FACTION_1, u"Hochebene erobern: alle Gegner vernichten", \
      u"Take the high plain: destroy all enemies") \
    X(S10, u"s10_flug", FACTION_1, u"Absturzstelle sichern: alle Gegner vernichten", \
      u"Secure the crash site: destroy all enemies") \
    X(S11, u"s11_warn", FACTION_1, u"Alle leichten Geschütztürme des Gegners zerstören", \
      u"Destroy all enemy light gun turrets") \
    X(S12, u"s12_ueber", FACTION_1, u"Golrathen vernichten (Ilurer optional)", \
      u"Destroy the Golraths (Ilurens optional)") \
    X(S13, u"s13_kfeuer", FACTION_1, u"Tritium-Förderung von 240 t sichern", u"Secure tritium production of 240 t") \
    X(S14, u"s14_feind", FACTION_1, u"Tritium-Förderung von 300 t sichern", u"Secure tritium production of 300 t") \
    X(S15, u"s15_bombe", FACTION_1, u"Tritium-Pumpen der Golrathen in 30 Min. zerstören", \
      u"Destroy the Golrath tritium pumps within 30 min") \
    X(S16, u"s16_trommel", FACTION_1, u"Invasionsarmee der Golrathen zerschlagen", u"Smash the Golrath invasion army") \
    X(S17, u"s17_heftig", FACTION_1, u"Invasionsarmee der Golrathen zerschlagen", u"Smash the Golrath invasion army") \
    X(S18, u"s18_flucht", FACTION_1, u"Radaranlage 30 Min. halten oder Gegner vernichten", \
      u"Hold the radar 30 min or destroy the enemy") \
    X(S20, u"s20_posten", FACTION_1, u"Golrathischen Außenposten ausschalten", u"Eliminate the Golrath outpost") \
    X(S21, u"s21_marsch", FACTION_1, u"Marschroute säubern: alle Gegner vernichten", \
      u"Clear the route: destroy all enemies") \
    X(S22, u"s22_laser", FACTION_1, u"Gelände erkunden und alle Gegner vernichten", \
      u"Scout the area and destroy all enemies") \
    X(S23, u"s23_tritod", FACTION_1, u"Tritium-Förderung der Golrathen stoppen", \
      u"Shut down the Golrath tritium production") \
    X(S24, u"s24_indust", FACTION_1, u"Alle Gebäude der Golrathen zerstören, auch Mauern", \
      u"Destroy every Golrath building, walls included") \
    X(S25, u"s25_finale", FACTION_1, u"Letzte Bastion der Golrathen vernichten", u"Destroy the last Golrath bastion") \
    X(N1, u"ersterkontakt", FACTIONS_1_2, u"Radarstellung der Golrathen im Norden zerstören", \
      u"Destroy the Golrath radar post in the north") \
    X(N2, u"eisigehöhen", FACTIONS_1_2, u"Alle Xenit-Silos und Tritium-Tanks zerstören", \
      u"Destroy all xenite silos and tritium tanks") \
    X(N3, u"kometenhagel", FACTIONS_1_2, u"Alle Xenit-Minen und Tritium-Pumpen zerstören", \
      u"Destroy all xenite mines and tritium pumps") \
    X(N4, u"sumpfland", FACTIONS_1_2, u"Alle Einheiten der Golrathen vernichten", u"Destroy all Golrath units") \
    X(N5, u"paradis", FACTIONS_1_2, u"Golrathen auf der Insel vernichten", u"Wipe out the Golraths on the island") \
    X(L00, u"l00_inselkampf", FACTIONS_1_2, u"Handelswege freikämpfen: alle Piraten vernichten", \
      u"Free the trade routes: destroy all pirates") \
    X(L01, u"l01_dejavu", FACTIONS_1_2, u"Piraten stellen und alle vernichten", u"Hunt down and destroy all pirates") \
    X(L02, u"l02_gebirgsjäger", FACTIONS_1_2, u"Piratenlager in den Bergen vernichten", \
      u"Wipe out the pirate camp in the mountains") \
    X(L03, u"l03_dasnest", FACTIONS_1_2, u"Geheime Forschungsanlage (Laserspule) zerstören", \
      u"Destroy the secret research site (laser coil)")

#define THANDOR_MISSION_OBJECTIVE_DEFINE(name, level, factions, german, english) \
    ObjectiveTextUtf16 g_MissionObjectiveGerman_##name{german}; \
    ObjectiveTextUtf16 g_MissionObjectiveEnglish_##name{english};
THANDOR_MISSION_OBJECTIVES(THANDOR_MISSION_OBJECTIVE_DEFINE)
#undef THANDOR_MISSION_OBJECTIVE_DEFINE

struct MissionObjectiveEntry {
    const char16_t *levelBaseName;
    uint32_t factionMask;
    uint16_t *german;
    uint16_t *english;
};

#define THANDOR_MISSION_OBJECTIVE_ENTRY(name, level, factions, german, english) \
    {level, factions, g_MissionObjectiveGerman_##name.units, g_MissionObjectiveEnglish_##name.units},
const MissionObjectiveEntry g_MissionObjectives[] = {THANDOR_MISSION_OBJECTIVES(THANDOR_MISSION_OBJECTIVE_ENTRY)};
#undef THANDOR_MISSION_OBJECTIVE_ENTRY

constexpr uint32_t MISSION_OBJECTIVE_COUNT = sizeof(g_MissionObjectives) / sizeof(g_MissionObjectives[0]);
static_assert(MISSION_OBJECTIVE_COUNT <= TEXT_ID_MISSION_OBJECTIVE_SHORT_RESERVED,
              "one id of the reserved short-objective block per table entry");

/* ASCII and Latin-1 upper case letters to lower case (level file names are compared without case) */
uint32_t LowerLatin1(uint32_t unit)
{
  if (((unit >= 'A') && (unit <= 'Z')) || ((unit >= 0xC0) && (unit <= 0xDE) && (unit != 0xD7))) {
    return unit + 0x20;
  }
  return unit;
}

/* true when levelName (its part after the last '\' or '/' up to the first '.' or the terminator) equals baseName,
   case ignored */
bool LevelNameMatches(const uint16_t *levelName,const char16_t *baseName)
{
  const uint16_t *start = levelName;
  for (const uint16_t *cursor = levelName; *cursor != 0; cursor++) {
    if ((*cursor == '\\') || (*cursor == '/')) {
      start = cursor + 1;
    }
  }
  size_t index = 0;
  for (; (start[index] != 0) && (start[index] != '.'); index++) {
    if ((baseName[index] == 0) || (LowerLatin1(start[index]) != static_cast<uint32_t>(baseName[index]))) {
      return false;
    }
  }
  return baseName[index] == 0;
}

} // namespace

TextResourceId TextResource_MissionObjective(const uint16_t *levelName,uint32_t faction)
{
  if ((levelName == nullptr) || (faction >= 32)) {
    return TEXT_RESOURCE_ID_NONE;
  }
  for (uint32_t index = 0; index < MISSION_OBJECTIVE_COUNT; index++) {
    const MissionObjectiveEntry &entry = g_MissionObjectives[index];
    if (((entry.factionMask & (1u << faction)) != 0) && LevelNameMatches(levelName,entry.levelBaseName)) {
      return TEXT_ID_MISSION_OBJECTIVE_SHORT_BASE + index;
    }
  }
  return TEXT_RESOURCE_ID_NONE;
}

bool TextResource_TryResolveMissionObjective(TextResourceId resourceId,uint16_t **outText)
{
  if ((resourceId < TEXT_ID_MISSION_OBJECTIVE_SHORT_BASE) ||
      (resourceId >= TEXT_ID_MISSION_OBJECTIVE_SHORT_BASE + MISSION_OBJECTIVE_COUNT)) {
    return false;
  }
  const MissionObjectiveEntry &entry = g_MissionObjectives[resourceId - TEXT_ID_MISSION_OBJECTIVE_SHORT_BASE];
  *outText = TextResource_ProjectTextsAreGerman() ? entry.german : entry.english;
  return true;
}
