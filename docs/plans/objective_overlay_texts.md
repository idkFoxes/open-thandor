# Ziel-Overlay: Kurzziel-Texte der Kampagnen (Entwurf zur Durchsicht)

Paket O2 des Plans [objective_overlay.md](objective_overlay.md) (3a). Eine Zeile pro Kampagnen-Mission und spielbarer Fraktion, Deutsch und Englisch (Französisch, Italienisch und Spanisch zeigen Englisch). Grundlage: Sieg-/Niederlage-Bedingungen und Missionshilfe aus [objective_overlay_levels.md](objective_overlay_levels.md). Wo Briefing und Siegbedingung auseinandergehen, folgt der Text der Siegbedingung; die Abweichung steht unter Bemerkung.

Umgesetzt in `src/assets/text/objective_texts.cpp` (`TextResource_MissionObjective(levelName, faction)`). Schlüssel zur Laufzeit ist der Level-Dateiname ohne Verzeichnis und Endung, klein geschrieben (so steht er im Level-Kopf, `LevelAssetHeader.levelFileNameUtf16`, z. B. `s07_landunt.`); die Spalte „Level“ nennt zur Orientierung auch die Kampagne.

Umfang: 36 Missionen (Tutorial 3, Die Invasion 24, Kyrrdon 5, Piraten 4), 45 Paare Mission+Fraktion (Kyrrdon und Piraten: Fraktion 1 und 2 teilen sich einen Text), 36 Texte. Die 19 Karten aus `level.dat` (59 weitere Paare der O1-Zählung von 104) haben keinen Kurztext; dort greift der erzeugte Text (Plan 3b, „Alle Gegner ausschalten“). `hansolo\s00_tut` und `hansolo\s19_forschung` fehlen in LEVEL.PCK und haben keinen Eintrag.

Länge: höchstens ~50 Zeichen, Befehlsform. Fortschritt (Rate, Countdown, Anzahl) zeigt Zeile 2 aus O3, deshalb nennen die Texte nur Zahlen, die im Briefing und in der Bedingung übereinstimmen.

## Tutorial (tutorial.cgn)

| Level | Fr. | Titel | Ziel DE | Ziel EN | Bemerkung |
|---|---|---|---|---|---|
| `tutorial\t00_tut` | 1 | Tutorial I (Basisaufbau) | Basis aufbauen, alle Gegner vernichten | Build a base, destroy all enemies | Sieg = Fraktionen 2–5 ohne Einheiten. Der eigentliche Ablauf folgt den Tutorial-Anweisungen; **Owner**: lieber „Den Anweisungen des Tutorials folgen“? |
| `tutorial\t01_tut` | 1 | Tutorial II (Forschung) | Forschen, angreifen, alle Gegner vernichten | Research, attack and destroy all enemies | wie t00 |
| `tutorial\t02_tut` | 1 | Tutorial III (Rohstoffe) | Tritium fördern und 2 Xenit-Minen bauen | Extract tritium and build 2 xenite mines | Sieg = Tritium-Förderrate ≥ 6,875 (Q4 110 pro Tick) UND ≥ 2 Xenit-Minen; das Briefing nennt kein Ziel. Rate zeigt Zeile 2. |

## Die Invasion (hansolo.cgn)

| Level | Fr. | Titel | Ziel DE | Ziel EN | Bemerkung |
|---|---|---|---|---|---|
| `hansolo\s01_insel` | 1 | Inselspringen | Basis ausbauen und alle Gegner vernichten | Expand the base and destroy all enemies | Briefing: Brückenkopf ausbauen, Fahrzeuge bauen; Sieg = alle Gegner ohne Einheiten |
| `hansolo\s02_schlucht` | 1 | Nachschubwege | Canyon sichern: alle Gegner vernichten | Secure the canyon: destroy all enemies | Sieg = alle Gegner |
| `hansolo\s03_erobern` | 1 | Eroberung | Bastion vor dem Raumhafen restlos vernichten | Wipe out the bastion before the spaceport | Sieg = alle Gegner ohne Einheiten („restlos“), nicht nur die Basis |
| `hansolo\s04_konflikt` | 1 | Konflikt | Einen der beiden Gegner vollständig vernichten | Wipe out one of the two enemies | Zwei Sieg-Trigger: Fraktion 2 vernichtet → Endauswahl 1, Fraktion 3 vernichtet → Endauswahl 2 (Verzweigung s05/s06). Niederlage: eigenes Hauptquartier verloren. **Owner**: welche Fraktion Thoraner/Avares ist, ist nicht gemessen, deshalb ohne Namen. |
| `hansolo\s05_trigelb` | 1 | Tritium Gelb | Tritium-Förderung im Süden wiederherstellen | Restore tritium production in the south | Briefing „300 T/T“, Bedingung Q4 260 pro Tick (16,25) – Zahl weggelassen, Zeile 2 zeigt die Rate. Niederlage: Hauptquartier verloren. |
| `hansolo\s06_trirot` | 1 | Tritium Rot | Tritium-Förderung im Westen wiederherstellen | Restore tritium production in the west | wie s05 (Briefing 300, Bedingung 260) |
| `hansolo\s07_landunt` | 1 | Landunter | Staudamm-Gebiet erobern, Gegner dort vernichten | Take the dam area, wipe out its defenders | Sieg = nur Fraktion 2 ohne Einheiten (Fraktion 3 ist aktiv, muss aber nicht besiegt werden). Niederlage: Hauptquartier verloren. |
| `hansolo\s08_instand` | 1 | Instandsetzung | Bauen: 2 Hauptquartiere, 4 Kraftwerke, 3 Xenitminen | Build 2 HQs, 4 power plants and 3 xenite mines | Bedingung „mindestens“ je Gebäudeart (UND). DE 51 Zeichen; **Owner**: Kurzform „2 HQ“ auch im Deutschen? Briefing-Nebensatz (Angreifer ausschalten) ist keine Siegbedingung. |
| `hansolo\s09_krieg` | 1 | Krieg | Hochebene erobern: alle Gegner vernichten | Take the high plain: destroy all enemies | Sieg = Fraktionen 2, 3, 4 ohne Einheiten; Niederlage: Hauptquartier verloren |
| `hansolo\s10_flug` | 1 | Flugzeugbergung | Absturzstelle sichern: alle Gegner vernichten | Secure the crash site: destroy all enemies | Sieg = alle Gegner |
| `hansolo\s11_warn` | 1 | Warnschuß | Alle leichten Geschütztürme des Gegners zerstören | Destroy all enemy light gun turrets | Sieg = Fraktion 2 hat keinen leichten Geschützturm (ARM 370) mehr; das Briefing nennt nur „Lektion erteilen, Xenitproduktion sichern“. |
| `hansolo\s12_ueber` | 1 | Überraschung | Den neuen Gegner aufspüren und vernichten | Track down and destroy the new enemy | Zwei Sieg-Trigger: Endauswahl 1 = Fraktionen 2 und 3 vernichtet, Endauswahl 2 = nur Fraktion 3. Fraktion 3 ist vermutlich der „neue Gegner“ (Golrathen), Fraktion 2 die Ilurer (Waffenstillstand). **Owner**: Verzweigung beschreiben (z. B. „Golrathen vernichten (Ilurer optional)“)? |
| `hansolo\s13_kfeuer` | 1 | Kreuzfeuer | Tritium-Förderung von 240 t sichern | Secure tritium production of 240 t | Briefing 240 t = Bedingung Q4 240 pro Tick |
| `hansolo\s14_feind` | 1 | Der alte Feind | Tritium-Förderung von 300 t sichern | Secure tritium production of 300 t | Briefing 300 t = Bedingung Q4 300 pro Tick |
| `hansolo\s15_bombe` | 1 | Zeitbombe | Tritium-Pumpen der Golrathen in 30 Min. zerstören | Destroy the Golrath tritium pumps within 30 min | Sieg = Fraktion 2 hat keine Tritium-Pumpe (ARM 332) mehr; Niederlage nach 30:00 (Countdown in Zeile 2) oder ohne Einheiten |
| `hansolo\s16_trommel` | 1 | Trommelfeuer | Invasionsarmee der Golrathen zerschlagen | Smash the Golrath invasion army | Sieg = nur Fraktion 5 ohne Einheiten (Fraktionen 2–4 müssen nicht besiegt werden; Briefing hofft auf ihren Beistand) |
| `hansolo\s17_heftig` | 1 | Starkes Trommelfeuer | Invasionsarmee der Golrathen zerschlagen | Smash the Golrath invasion army | wie s16 |
| `hansolo\s18_flucht` | 1 | Flucht nach vorn | Radaranlage 30 Min. halten oder Gegner vernichten | Hold the radar 30 min or destroy the enemy | Sieg = Fraktion 2 ohne Einheiten ODER Countdown 30:00 abgelaufen. Niederlage: kein Geschützturm (ARM 340) mehr – die „Radaranlage“ des Briefings ist offenbar dieser Typ. **Owner**: Reihenfolge (Halten zuerst) ok? |
| `hansolo\s20_posten` | 1 | Der Außenposten | Golrathischen Außenposten ausschalten | Eliminate the Golrath outpost | Sieg = alle Gegner ohne Einheiten (= der Außenposten mit allen Einheiten) |
| `hansolo\s21_Marsch` | 1 | Auf dem Vormarsch | Marschroute säubern: alle Gegner vernichten | Clear the route: destroy all enemies | Sieg = alle Gegner. Dateiname im .cgn mit großem M, im Level-Kopf `s21_marsch`; Vergleich ohne Groß/klein. |
| `hansolo\s22_laser` | 1 | Der Gegenangriff | Gelände erkunden und alle Gegner vernichten | Scout the area and destroy all enemies | Sieg = alle Gegner; das Briefing nennt nur Erkunden |
| `hansolo\s23_tritod` | 1 | Tritiumtod | Tritium-Förderung der Golrathen stoppen | Shut down the Golrath tritium production | Sieg = NICHT (Förderrate Fraktion 2 ≥ Q4 1), also Rate 0 – Pumpen zerstören oder erobern |
| `hansolo\s24_indust` | 1 | Industriekomplex | Hauptstreitmacht im Industriekomplex vernichten | Destroy the main force at the industrial complex | Sieg = Fraktion 2 hat keine Einheiten der Kommandogruppe A mehr. **Owner**: Welche Einheiten Gruppe A sind, ist nicht gemessen; „Hauptstreitmacht“ ist die Deutung aus Plan 3b. |
| `hansolo\s25_finale` | 1 | Der Todesstoß | Letzte Bastion der Golrathen vernichten | Destroy the last Golrath bastion | Briefing: Hauptquartier „einnehmen“; Sieg = alle Gegner ohne Einheiten |

## Kyrrdon (nimm2.cgn, zwei Spieler)

| Level | Fr. | Titel | Ziel DE | Ziel EN | Bemerkung |
|---|---|---|---|---|---|
| `nimm2\ersterkontakt` | 1, 2 | Multi: Erster Kontakt | Radarstellung der Golrathen im Norden zerstören | Destroy the Golrath radar post in the north | Sieg = Fraktion 3 hat keinen Geschützturm (ARM 340) mehr; der „sekundäre“ Vorposten ist nicht nötig. Das Briefing schreibt „Golraten“; hier einheitlich „Golrathen“. |
| `nimm2\eisigehöhen` | 1, 2 | Multi: Eisige Höhen | Alle Xenit-Silos und Tritium-Tanks zerstören | Destroy all xenite silos and tritium tanks | Sieg = Fraktion 3 ohne Xenit-Silo (ARM 331) UND ohne Tritium-Tank (ARM 333); Briefing „Tanklager“ |
| `nimm2\kometenhagel` | 1, 2 | Multi: Kometenhagel | Alle Xenit-Minen und Tritium-Pumpen zerstören | Destroy all xenite mines and tritium pumps | Sieg = Fraktion 3 ohne Xenit-Mine (ARM 330) UND ohne Tritium-Pumpe (ARM 332); Briefing „Rohstoffminen“ |
| `nimm2\sumpfland` | 1, 2 | Multi: Sumpfland | Alle Einheiten der Golrathen vernichten | Destroy all Golrath units | Sieg = Fraktion 3 ohne Einheiten (die Prototypen eingeschlossen) |
| `nimm2\paradis` | 1, 2 | Multi: Paradis | Golrathen auf der Insel vernichten | Wipe out the Golraths on the island | Sieg = Fraktion 3 ohne Einheiten |

## Piraten (luke.cgn, zwei Spieler)

| Level | Fr. | Titel | Ziel DE | Ziel EN | Bemerkung |
|---|---|---|---|---|---|
| `luke\l00_inselkampf` | 1, 2 | Multi: Inselkampf | Handelswege freikämpfen: alle Piraten vernichten | Free the trade routes: destroy all pirates | Sieg = Fraktion 3 ohne Einheiten |
| `luke\l01_dejavu` | 1, 2 | Multi: Deja Vu | Piraten stellen und alle vernichten | Hunt down and destroy all pirates | Sieg = Fraktion 3 ohne Einheiten |
| `luke\l02_gebirgsjäger` | 1, 2 | Multi: Gebirgsjäger | Piratenlager in den Bergen vernichten | Wipe out the pirate camp in the mountains | Sieg = Fraktion 3 ohne Einheiten („keiner darf entkommen“) |
| `luke\l03_dasnest` | 1, 2 | Multi: Das Nest | Geheime Forschungsanlage (Laserspule) zerstören | Destroy the secret research site (laser coil) | Sieg = Fraktion 3 hat kein ARM 345 mehr. ARM 345 hat keinen Modellnamen, „Laserspule“ stammt aus dem Enum-Kommentar. **Owner**: Klammer streichen oder den Namen aus `help.str` nehmen? |

## Offene Fragen an den Owner

1. Tutorials (t00, t01): Siegbedingung „alle Gegner vernichten“ nennen oder „Den Anweisungen des Tutorials folgen“?
2. s04 Konflikt: Gegner ohne Namen lassen („einen der beiden“) oder Thoraner/Avares nennen (Zuordnung Fraktion 2/3 nicht gemessen)?
3. s12 Überraschung: die Verzweigung (nur Fraktion 3 vs. 2 und 3) erwähnen?
4. s05/s06: Briefing 300 T/T, Bedingung 260 – Zahl weglassen (Entwurf) oder „260“ nennen?
5. s08: „Hauptquartiere“ ausschreiben (51 Zeichen) oder „HQ“?
6. s18: „halten oder vernichten“ – passt die Reihenfolge?
7. s24: Kommandogruppe A als „Hauptstreitmacht“?
8. l03: „Laserspule“ als Name der Anlage behalten?
9. Schreibweise „Golrathen“ auch in Kyrrdon (dort im Original „Golraten“)?
