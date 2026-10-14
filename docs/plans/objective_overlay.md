# Plan: Ziel-Overlay im Spiel

Branch `experimental/objective-overlay` (von `experimental-dev`). Status: vom Owner freigegeben (2026-10-14).

## 1. Ziel

Oben links im Spiel eine kompakte Anzeige des aktuellen Auftrags bzw. der Siegbedingung:

```
Ziel: Zerstöre die feindliche Basis
      Xenit 3200 / 5000
```

- Zeile 1: das Ziel, kurz gefasst. Zeile 2 (optional): Fortschritt live (Xenit, Countdown, Gebietsanteil in %).
- Klick auf das Overlay öffnet die volle Missionshilfe (F1, „Auftrag“).
- Ein-/ausschaltbar: Checkbox „Ziel anzeigen“ auf „Erweitert“; `thandor.ini` `[game] objective_overlay = on|off` (Standard on); eine Taste in der neuen änderbaren Tastenbelegung.
- Reine Anzeige: kein Einfluss auf Simulation, Determinismus, Multiplayer oder Spielstände.
- Gilt für Kampagnen, Einzelkarten und Multiplayer; jeder Spieler sieht die Ziele seiner Fraktion.

Nicht-Ziele: kein Questlog/Teilziel-Häkchen in v1; keine Änderung an Level-Daten, Briefings oder der Original-Missionshilfe; keine freie Positionierung.

## 2. Ist-Zustand (gemessen)

- F1 / „Auftrag“: `InGameMissionHelpPage_Toggle` (`src/ui/ingame/pages.cpp`), Seite `INGAME_WINDOW_PAGE_MISSION_HELP` (8).
- Missionshilfe-Text: Briefing aus `texte\inhalt.str` (Seite 0x23; `level.str` = Seite 0x22 hat nur die Titel) pro Mission und Fraktion, Text-ID
  `TEXT_ID_MISSION_HELP_BASE + titleTextResourceIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE + aktive Fraktion`
  (`include/thandor/ui/ingame/pages.h`, Basis 0x230017). Ein kurzes Missionsziel gibt es in den Originaldaten nicht.
- Siegbedingungen im Level-Skript (`InGameLevelConditionStorage`): 64 Bedingungen × 16 Bytes ab LEV +0x380, 16 Endtrigger × 8 Bytes ab +0x780 (Fraktion, Endauswahl, Variante 0 = Niederlage / 1 = Sieg, Bedingungsindex).
- Bedingungsarten (`include/thandor/gameplay/session/types.h`): 2 FACTION_HAS_NO_ARMY, 4 FACTION_HAS_NO_COMMAND_GROUP_A_ARMY, 6 FACTION_HAS_NO_ARMY_OF_ASSET, 8 FACTION_INACTIVE_OR_RELATION_AT_LEAST_8, 10 XENITE_AT_LEAST, 12 TRITIUM_AT_LEAST, 14 TRITIUM_EXTRACTION_RATE_AT_LEAST, 16 ARMY_OF_ASSET_COUNT_AT_LEAST, 18 FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST, 20 COUNTDOWN_ELAPSED, 22 XENITE_STORAGE_LIMIT_AT_MOST_0FA0, 24 NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A, 26 BOOLEAN_POSTFIX_EXPRESSION.
- `InGameUiImage` hat noch kein Projekt-Element. Das Overlay wird keins (O4): ein Knoten im Modulspeicher mit eigener vtable, zur Laufzeit in den In-Game-Root gehängt.

## 3. Textquelle

### 3a. Handgeschriebene Kurzziele (Hauptquelle)
Pro Kampagnen-Mission und Fraktion eine Zeile (Deutsch + Englisch; fr/it/es zeigen Englisch). ~36 Missionen (Tutorial 3, Die Invasion 24, Kyrrdon 5, Piraten 4). Schlüssel Level-Dateiname + Fraktion (z. B. `hansolo\s07_landunt` / 1). Ablage `src/assets/text/objective_texts.cpp`. Entwürfe aus den Briefings, Owner liest gegen. Max. ~50 Zeichen.

### 3b. Automatisch aus den Siegbedingungen (Rückfall)
Für Einzelkarten, Multiplayer, Add-ons und fehlende Einträge. Endtrigger mit Variante 1 und Fraktion = lokale Fraktion (wie bei der Kampagnenkarte).

| Bedingung | Zeile 1 | Zeile 2 |
|---|---|---|
| FACTION_HAS_NO_ARMY (Gegner) | Alle Gegner vernichten | Gegnerische Einheiten: 37 |
| FACTION_HAS_NO_COMMAND_GROUP_A_ARMY | Feindliche Hauptstreitmacht vernichten | – |
| FACTION_HAS_NO_ARMY_OF_ASSET | <Einheitentyp> zerstören | Verbleibend: 2 |
| FACTION_INACTIVE_OR_RELATION_AT_LEAST_8 | <Fraktion> ausschalten | – |
| XENITE_AT_LEAST | Xenit sammeln | Xenit 3200 / 5000 |
| TRITIUM_AT_LEAST | Tritium sammeln | Tritium 800 / 1500 |
| TRITIUM_EXTRACTION_RATE_AT_LEAST | Tritium-Förderung steigern | Rate 12 / 20 |
| ARMY_OF_ASSET_COUNT_AT_LEAST | <Typ> bauen | 2 / 5 |
| TERRAIN_OCCUPANCY …PERCENT | Gebiet kontrollieren | 60 % / 75 % |
| COUNTDOWN_ELAPSED | Durchhalten | Noch 4:30 |
| XENITE_STORAGE_LIMIT_AT_MOST | Xenit-Lager klein halten | – |
| NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A | Einheiten zusammenhalten | – |
| BOOLEAN_POSTFIX_EXPRESSION | Teilziele mit „und“/„oder“, sonst „Mehrere Ziele (F1)“ | erstes messbares Teilziel |

Einheitentypen/Fraktionsnamen aus vorhandenen Texten. Mehrere Sieg-Trigger: erster + „(+n weitere)“.

### 3c. Kein Ziel ermittelbar
Overlay bleibt ausgeblendet.

Fortschrittswerte kommen aus denselben Zählern wie die Bedingungsprüfung; nur lesend, keine Zufallszüge, nichts in die Simulation.

## 4. Darstellung

- Oben links unter der Menüleiste, Abstand passt sich der UI-Skalierung an.
- Spielschrift; „Ziel:“ in Hervorhebungsfarbe, Rest normal; halbtransparenter dunkler Hintergrund mit kleinem Rahmen im Original-Stil (Rahmenzeichnen wie bei der Kampagnenkarte); max. zwei Zeilen, Abschneiden mit „…“.
- Aktualisierung ~2× pro Sekunde; Zeile nur neu bauen, wenn sich der Wert ändert.
- Ausblenden: Einstellung aus, kein Ziel, Spielmenü oder Missionshilfe offen, Endfilm.
- Klick öffnet die Missionshilfe; die Welt bekommt den Klick in diesem Bereich nicht.
- Sprache folgt der Sprachauswahl (Deutsch / sonst Englisch).

## 5. Arbeitspakete

| Paket | Inhalt | Dateien | Größe |
|---|---|---|---|
| O1 Messung | `tools/data/objectives.py`: alle Stock-Level mit lev.py/cgn.py auswerten (Sieg-Trigger, Bedingungsarten, Parameter pro Mission+Fraktion), Abdeckung des Rückfalls, Briefing-Texte ausgeben; Tabelle in diesen Plan | `tools/data/objectives.py`, dieser Plan | S |
| O2 Kurzziel-Texte | Entwürfe DE/EN aus den Briefings, Owner-Review, Tabelle + `TextResource_MissionObjective(levelName, faction)` | `src/assets/text/objective_texts.cpp`, `resources.h` | M |
| O3 Ziel-Modell | `MissionObjective_Build(levelImage, localFaction)` (Kurztext, sonst 3b), `MissionObjective_Progress()` nur lesend; Selbsttest `objectives` | `src/gameplay/session/mission_objective.{h,cpp}` | M |
| O4 Overlay-Element | Projekt-Knoten im Modulspeicher mit eigener vtable, nach `UiRootStack_Push` als Geschwister direkt hinter die Weltansicht gehängt (über der Welt, unter Fenstern/Nachrichten/Seitenleiste; Template, `InGameRuntimeRoot`, Layout-Asserts und uitemplate-Golden unverändert), Zeichnen, Klick → Missionshilfe, UI-Skalierung | `ui/ingame/objective_overlay.{h,cpp}`, `gameplay/session/startup.cpp` | M |
| O5 Aktualisierung + Einstellungen | 2×/s, Ausblendregeln, Checkbox „Ziel anzeigen“, ini `[game] objective_overlay` (reservierter Platz 0x54), Taste „Ziel ein/aus“ (über die änderbare Tastenbelegung, Branch `experimental/keybinding-edit`; sonst nachreichen) | `display_settings.cpp`, `persistent.cpp`, `key_bindings.cpp` | S–M |
| O6 Tests + Doku | `tools/test/objective_overlay.txt` mit Screenshots; Determinismus/KI-Hash an+aus identisch; Patch Notes, BUILDING.md, liesmich | `tools/test/…`, Docs | S |

Reihenfolge: O1 → O2, O3, O4 parallel → O5 → O6 (voller run_checks).

## 6. Tests

| Test | Was | Erwartung |
|---|---|---|
| T1 | Selbsttest `objectives` | jede Stock-Mission ergibt pro spielbarer Fraktion einen Text; erzeugte Texte stimmen |
| T2 | Kampagnen-Mission mit Kurztext | Kurztext oben links, DE und EN |
| T3 | Einzelkarte ohne Kurztext | erzeugter Text, z. B. „Alle Gegner vernichten“ |
| T4 | Ressourcen-/Countdown-Ziel | Fortschritt zählt mit |
| T5 | Overlay aus (Checkbox, ini, Taste) | nichts sichtbar, bleibt nach Neustart |
| T6 | Klick aufs Overlay | Missionshilfe öffnet sich |
| T7 | Determinismus, KI-Hash, Multiplayer | identisch / PASS, Overlay an und aus |
| T8 | UI-Skalierung 1/2/3, Software + Vulkan | Lage und Lesbarkeit korrekt |

## 7. Risiken

| Risiko | Folge | Gegenmaßnahme |
|---|---|---|
| Projekt-Knoten im In-Game-Baum | verschobene Offsets, Relokation | kein Template-Knoten: Laufzeit-Verknüpfung nach der Relokation, vor dem Freigeben gelöst; uitemplate/Pixel unverändert |
| Overlay verdeckt Welt/Menüleiste | störend | max. zwei Zeilen, halbtransparent, abschaltbar |
| verknüpfte Siegbedingungen | unverständlicher Text | Kurztexte haben Vorrang, „Mehrere Ziele (F1)“ |
| mehrere Sieg-Trigger | unvollständig | erster + „(+n weitere)“ |
| falsche Zähler | irreführend | Selbsttest gegen bekannte Level, nur lesend |
| Netzwerkspiel | Abweichung | rein lokale Anzeige |

## 8. Entscheidungen

| # | Frage | Entscheidung |
|---|---|---|
| E1 | Textquelle | handgeschriebene Kurzziele + automatischer Rückfall |
| E2 | Fortschrittsanzeige | ja |
| E3 | Position | fest oben links unter der Menüleiste |
| E4 | Standard | an |
| E5 | Geltungsbereich | Kampagnen, Einzelkarten, Multiplayer (dort nur erzeugter Text) |

## 9. O1 Messung

Werkzeug `tools/data/objectives.py` (liest die PCKs der Soak-Kopie nur lesend, PATCH00 vor ENGINE); volle Liste pro Mission und Fraktion mit Sieg-/Niederlage-Bedingungen in Worten, erzeugtem 3b-Text und Missionshilfe DE/EN: [objective_overlay_levels.md](objective_overlay_levels.md). Neu erzeugen: `python tools/data/objectives.py --out docs/plans/objective_overlay_levels.md --summary`.

Umfang: 57 Level eingetragen (4 Kampagnen aus `campagne.dat` + 19 Karten aus `level.dat`), 55 vorhanden; `hansolo\s00_tut` und `hansolo\s19_forschung` stehen in `hansolo.cgn`, fehlen aber in LEVEL.PCK. 36 Kampagnen-Missionen (Tutorial 3, Die Invasion 24, Kyrrdon 5, Piraten 4). 104 Paare Mission+spielbare Fraktion (Fraktionen 1..zuweisbar mit Endtrigger oder echter Missionshilfe; die Platzhalter „Langtext für die n. Partei.“ zählen nicht).

Textquellen geprüft: Titel = `level.str` String 0x30 + Titelindex; Missionshilfe = `TEXT_ID_MISSION_HELP_BASE` 0x230017 + Titelindex × 0x10 + Fraktion, also `inhalt.str` (Seite 0x23) String 0x17 + 0x10·i + f, Locale-Blöcke 49 (DE) und 44 (EN). Alle 104 Paare haben DE- und EN-Text.

Bedingungsarten (Anzahl Trigger; „Teilbedingung“ = kommt im aufgelösten Postfix-Baum vor):

| Bedingungsart | Sieg-Trigger (oberste Art) | Sieg (als Teilbedingung) | Niederlage-Trigger (oberste Art) | Niederlage (als Teilbedingung) |
|---|---|---|---|---|
| 2 FACTION_HAS_NO_ARMY | 16 | 29 | 78 | 96 |
| 4 FACTION_HAS_NO_COMMAND_GROUP_A_ARMY | 1 | 1 | 0 | 0 |
| 6 FACTION_HAS_NO_ARMY_OF_ASSET | 6 | 10 | 6 | 7 |
| 8 FACTION_INACTIVE_OR_RELATION_AT_LEAST_8 | 0 | 58 | 0 | 0 |
| 14 TRITIUM_EXTRACTION_RATE_AT_LEAST | 4 | 6 | 0 | 0 |
| 16 ARMY_OF_ASSET_COUNT_AT_LEAST | 0 | 2 | 0 | 0 |
| 20 COUNTDOWN_ELAPSED | 0 | 1 | 0 | 1 |
| 26 BOOLEAN_POSTFIX_EXPRESSION | 78 | 0 | 19 | 0 |

Nie benutzt: 10 XENITE_AT_LEAST, 12 TRITIUM_AT_LEAST, 18 TERRAIN_OCCUPANCY, 22 XENITE_STORAGE_LIMIT, 24 NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A. Postfix-Bäume sind flach (nur UND, ODER, NICHT NICHT x = x, einmal NICHT x); keine Verschachtelung von UND und ODER.

Abdeckung des Rückfalls 3b (gut = eine Art bzw. „alle Gegner“ / Fraktionsliste; teilweise = und/oder-Teilziele, NICHT, mehrere Sieg-Trigger; nur „Mehrere Ziele (F1)“ = verschachtelt):

| Gruppe | Paare | gut | teilweise | nur „Mehrere Ziele“ | kein Sieg |
|---|---|---|---|---|---|
| Tutorial | 3 | 2 | 1 | 0 | 0 |
| Die Invasion (hansolo) | 24 | 19 | 5 | 0 | 0 |
| Kyrrdon (nimm2) | 10 | 6 | 4 | 0 | 0 |
| Piraten (luke) | 8 | 8 | 0 | 0 | 0 |
| Karten (level.dat) | 59 | 58 | 0 | 0 | 1 |
| gesamt | 104 | 93 | 10 | 0 | 1 |

Nicht „gut“:

| Level | Fraktion | Klasse | erzeugter Text (3b) |
|---|---|---|---|
| `tutorial\t02_tut` | 1 | teilweise | Tritium-Förderung steigern (6.875) und Xenit-Mine bauen (2) |
| `hansolo\s04_konflikt` | 1 | teilweise | Fraktion 2 vernichten (+1 weitere) – zwei Sieg-Trigger, Endauswahl 1 / 2 (Verzweigung) |
| `hansolo\s08_instand` | 1 | teilweise | Hauptquartier bauen (2) und Xenit-Mine bauen (3) und Kraftwerk bauen (4) |
| `hansolo\s12_ueber` | 1 | teilweise | Alle Gegner vernichten (+1 weitere) – Endauswahl 2 = nur Fraktion 3 |
| `hansolo\s18_flucht` | 1 | teilweise | Fraktion 2 vernichten oder Durchhalten (30:00) |
| `hansolo\s23_tritod` | 1 | teilweise | NICHT Tritium-Förderrate Fraktion 2 >= 1 (Q4) → „Tritium-Förderung von Fraktion 2 stoppen“ |
| `nimm2\eisigehöhen` | 1, 2 | teilweise | Xenit-Silo zerstören und Tritium-Tank zerstören |
| `nimm2\kometenhagel` | 1, 2 | teilweise | Xenit-Mine zerstören und Tritium-Pumpe zerstören |
| `lucas03` | 5 | kein Sieg | – (5 zuweisbare Fraktionen, Skript nur für 1–4) |

Befunde:

- Alle 19 Karten aus `level.dat` haben dasselbe Skript: Sieg = UND der drei Paare „Fraktion f und g: eine inaktiv oder verbündet“ über g ∈ 1..4 ohne f, Niederlage = keine eigenen Einheiten. 3b: „Alle Gegner ausschalten“ (Verbünden zählt mit). Ausnahme `lucas03` (5 Fraktionen): das Skript deckt nur 1–4 ab; Fraktion 5 hat keine Endtrigger und muss für die anderen nicht ausgeschaltet werden („Fraktionen 2, 3 und 4 ausschalten“).
- Kampagnen-Standard (Tutorial I/II, s01–s03, s10, s20–s22, s25): Sieg = Fraktionen 2–5 ohne Einheiten (die Fraktionsliste nennt Fraktion 3 doppelt), Niederlage = NICHT NICHT „eigene Fraktion ohne Einheiten“. „Gut“ heißt hier nur, dass 3b korrekt „Alle Gegner vernichten“ erzeugt; in den Tutorials ist das eigentliche Ziel das Lernen, die Kurzziele (O2) sind dort trotzdem nötig.
- `s16_trommel`, `s17_heftig`: Sieg = nur Fraktion 5 vernichten (5 aktive Fraktionen); 3b sagt „Fraktion 5 vernichten“ – ohne Fraktionsnamen wenig hilfreich.
- Niederlage häufig „Hauptquartier verloren“ (s04–s09, Art 6) bzw. „Geschützturm verloren“ (s18). Countdown nur in `s15_bombe` (Niederlage nach 30:00, Briefing: „Sie haben 30 Minuten Zeit“) und `s18_flucht` (Sieg nach 30:00); der Operand zählt Sekunden (die Prüfung läuft alle 20 Ticks).
- Kyrrdon und Piraten sind Zwei-Spieler-Kampagnen: Fraktion 1 und 2 haben je einen Sieg-Trigger auf dieselbe Bedingung (Fraktion 3 bzw. ihre Gebäude) und dieselbe Missionshilfe.
- Tritium-Förderrate: Operand in Q4 pro Tick (260 = 16,25). Welche Einheit die Anzeige (Zeile 2) zeigen soll, ist in O3 zu klären.
- Gebäudenamen aus den Enum-Kommentaren (`ARM_0345` hat keinen Modellnamen, dort „Laserspule“); für O3 die Namen aus `help.str` (Modellname) nehmen.
