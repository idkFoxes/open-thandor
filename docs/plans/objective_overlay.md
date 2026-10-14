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
- Missionshilfe-Text: Briefing aus `level.str` pro Mission und Fraktion, Text-ID
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
