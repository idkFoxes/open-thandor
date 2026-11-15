# Ziel-Overlay: Siegbedingungen und Briefings der Stock-Level

Erzeugt von `python tools/data/objectives.py --out docs/plans/objective_overlay_levels.md` aus den Spieldaten (LEVEL.PCK, texte\level.str / texte\inhalt.str, PATCH00 vor ENGINE). Eingabe für die Kurzziel-Texte (Plan O2). Pro Mission und spielbarer Fraktion: Sieg-Trigger (Variante 1) und Niederlage-Trigger (Variante 0) in Worten, die Abdeckung durch den automatischen Rückfall (Plan 3b) mit dem erzeugten Text und die Missionshilfe (F1) auf Deutsch und Englisch.

Abdeckung: **gut** = eine Bedingungsart bzw. „alle Gegner“, **teilweise** = und/oder-Teilziele, NICHT oder mehrere Sieg-Trigger,**nur Mehrere Ziele** = verschachtelt, **kein Sieg** = kein Sieg-Trigger.

## Tutorial (tutorial.cgn)

### `tutorial\t00_tut` – Tutorial I (Basisaufbau) / Tutorial I (Building a base)

id 1, Titelindex 66, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 4 hat keine Einheiten mehr UND Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner vernichten“

Missionshilfe DE (0x230438):

> Willkommen beim Tutorial von Thandor. In den nächsten Minuten werden Sie die Grundlagen erlernen, die für ein erfolgreiches Spielen von Thandor notwendig sind. Drücken Sie den Beginnen-Schalter, um die Welt von Thandor kennen zu lernen.

Missionshilfe EN:

> Welcome to the tutorial for Thandor. In the next few minutes, you will learn the basics required to play Thandor successfully. Press the Begin key and enter the world of Thandor.

### `tutorial\t01_tut` – Tutorial II (Forschung) / Tutorial II (Research)

id 2, Titelindex 67, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 4 hat keine Einheiten mehr UND Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner vernichten“

Missionshilfe DE (0x230448):

> Sehr gut! Sie sind jetzt mit den Grundgebäuden, aus denen eine Basis besteht, vertraut. Um das Überleben in der kriegerischen Welt von Thandor zu sichern, müssen wir jedoch noch weitere Wissenslücken schließen. Um die Fähigkeit der Forschung und des Angriffskrieges zu erlernen, setzen Sie bitte das Tutorial mit dem Beginnen-Schalter fort.

Missionshilfe EN:

> Very good! Now you are acquainted with the basic structures which make up a base. If you are to survive in the war-torn world of Thandor, however, we will have to fill in a few more gaps in your knowledge. To learn about research and conducting an offensive war, continue the tutorial by pressing the Begin button.

### `tutorial\t02_tut` – Tutorial III (Rohstoffe) / Tutorial III (Raw materials)

id 3, Titelindex 68, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 9): Tritium-Förderrate von Fraktion 1 (eigene) mindestens 6.875 (Q4 110 pro Tick) UND Fraktion 1 (eigene) hat mindestens 2 x Xenit-Mine (ARM 330)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **teilweise** – erzeugt: „Tritium-Förderung steigern (6.875) und Xenit-Mine bauen (2)“

Missionshilfe DE (0x230458):

> Sie machen große Fortschritte und lernen schnell. Eigenschaften, die Sie noch brauchen werden! Im nächsten Abschnitt geht es weniger kampfbetont weiter. Denn nach einer erfolgreichen Schlacht gilt es, sein Herrschaftsgebiet auszudehnen und eroberte Rohstoffvorkommen zu erschließen. Um fortzufahren, drücken Sie bitte den Beginnen-Schalter.

Missionshilfe EN:

> You are making great progress and learn quickly. These are traits that you will need very soon! The next section has less to do with fighting. After you have won a battle, you have to expand the territory under your control and recover the captured raw material deposits. To continue, please press the Begin button.

## Die Invasion (hansolo.cgn)

### `hansolo\s00_tut` (id 1)

Level-Datei fehlt in LEVEL.PCK (im .cgn eingetragen).

### `hansolo\s01_insel` – Inselspringen / Island hopping

id 2, Titelindex 50, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 4 hat keine Einheiten mehr UND Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner vernichten“

Missionshilfe DE (0x230338):

> Planet:      Thorgon
> Klima:       gemäßigt
>
> Neue Optionen:
> -Fahrzeugfabrik
>
> Das erste Ziel der Allianz von Avares (ARES) ist der kleinen Planet Thorgon. Er bietet uns mit seinem Raumhafen, eine optimale Ausgangsbasis für alle weiteren Aktionen. Im Zuge der Befreiung von Thorgon, hat die alliierte Flotte mehrere Landungskorps zusammengestellt, um die Hoheit über die verstreut liegenden Inseln in der Nähe des Raumhafens zurückzugewinnen. Übernehmen Sie einen der Brückenköpfe und bauen Sie die Basis weiter aus. Da Feindkontakt wahrscheinlich ist, erweitern Sie Ihre Armee um einige Fahrzeuge.

Missionshilfe EN:

> Planet:      Thorgon
> Climate:       moderate
>
> New options:
> - Vehicle factory
>
> The first target of the Alliance of Avares (ARES) is the small planet Thorgon. Its spaceport would provide us an optimal starting point for all of our following campaigns. In the course of the liberation of Thorgon, the allied fleet has assembled several landing corps to win back control of the scattered islands near the spaceport. Take command of one of the bridgeheads and expand the base. As enemy contact is probable, expand your army by several vehicles.

### `hansolo\s02_schlucht` – Nachschubwege / Supply Lines

id 3, Titelindex 21, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 4 hat keine Einheiten mehr UND Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner vernichten“

Missionshilfe DE (0x230168):

> Planet:      Thorgon
> Klima:       gemäßigt
>
> Neue Optionen:
> -Panzerfabrik
>
> Nachdem die Inseln erobert sind, kann mit der Invasion des Raumhafens von Thorgon begonnen werden. Es ist im Vorfeld unabdingbar, die Nachschubwege abzusichern, um die Versorgung jederzeit aufrecht zu erhalten. Der Canyon, in der Nähe Ihres Lagers, stellt die optimale Route für den Nachschub da. Sichern Sie ihn ab gegen feindliche Übergriffe.

Missionshilfe EN:

> Planet:      Thorgon
> Climate:       moderate
>
> New option:
> - Tank factory
>
> Once the islands have been captured, the invasion of the Thorgon spaceport can begin. It is essential to secure the supply lines beforehand so that the constant flow of provisions is assured. The canyon near your camp is the optimal route for supplies. Secure it against enemy attack.

### `hansolo\s03_erobern` – Eroberung / Conquest

id 4, Titelindex 51, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 4 hat keine Einheiten mehr UND Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner vernichten“

Missionshilfe DE (0x230348):

> Planet:      Thorgon
> Klima:       gemäßigt
>
> Neue Optionen:
> -Forschunglabor für Flak und Kanonen
> -Panzer Panther
>
> Die Hoheit über Thorgon zurückzuerlangen, ist in greifbare Nähe gerückt. Die letzte Militärbasis der Thoraner befindet sich auf einer schwer verteidigten Anhöhe. Etablieren Sie eine Basis und stellen Sie eine schlagkräftige Armee zusammen, um die letzte Bastion vor dem Raumhafen auszuschalten.

Missionshilfe EN:

> Planet:      Thorgon
> Climate:       moderate
>
> New options:
> - Research laboratory for flak and cannon 
> - Panther tank
>
> Regaining control over Thorgon is now within our grasp. The last Thoranian military base is on a hill and is heavily fortified. Establish a base and assemble a powerful army so that you can knock out the last bastion guarding the spaceport.

### `hansolo\s04_konflikt` – Konflikt / Conflict

id 5, Titelindex 70, Fraktionen zuweisbar 1 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 2): Fraktion 2 hat keine Einheiten mehr
- Sieg, Trigger 2 (Endauswahl 2, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat kein(e) Hauptquartier (ARM 300) mehr
- Abdeckung 3b: **teilweise** – erzeugt: „Fraktion 2 vernichten (+1 weitere)“

Missionshilfe DE (0x230478):

> Planet:      Gotol
> Klima:       Wüste
>
> Neue Optionen:
> -Forschungslabor Raketen und Haubitzen
>
> Thorgon befindet sich wieder fest in der Hand von ARES. Unser nächstes Ziel ist der strategisch wichtige Wüstenplanet Gotol. Unsere Truppen sind bereits gelandet und haben ein Basislager errichtet. In der Nähe unseres Landestützpunktes haben sich auch die Thoraner und die Avares eingenistet. Damit unsere Präsenz auf Gotol nicht gefährdet ist, muss eines der beiden Völker in diesem Sektor handlungsunfähig gemacht werden - Die Mittel stehen Ihnen frei.

Missionshilfe EN:

> Planet:      Gotol
> Climate:       Desert
>
> New options:
> - Research laboratory: rockets and howitzers 
>
> Thorgon is once again firmly in the hands of ARES. Our next goal is the strategically important desert planet, Gotol. Our troops have already landed there and built a base camp. The Thoranians and the Avares have also dug themselves in near our land base. If our presence on Gotol is not to be endangered, one of these two peoples in the sector must be put out of action - the means for doing so are up to you.

### `hansolo\s05_trigelb` – Tritium Gelb / Tritium yellow

id 6, Titelindex 71, Fraktionen zuweisbar 1 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 1): Tritium-Förderrate von Fraktion 1 (eigene) mindestens 16.25 (Q4 260 pro Tick)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 2): Fraktion 1 (eigene) hat kein(e) Hauptquartier (ARM 300) mehr
- Abdeckung 3b: **gut** – erzeugt: „Tritium-Förderung steigern (16.25)“

Missionshilfe DE (0x230488):

> Planet:      Gotol
> Klima:       Wüste
>
> Neue Optionen:
> -Haubitzen
>
> Die Landung auf Gotol war ein voller Erfolg, doch nachdem wir ihr Lager im Landungs-Sektor zerstört haben, schlugen die Thoraner sofort zurück. Sie haben unsere Tritiumpumpen im Süden erobert und uns geht das Tritium langsam aus. Stellen Sie die Fördermenge von 300 T/T im Südsektor wieder her.

Missionshilfe EN:

> Planet:      Gotol
> Climate:       Desert
>
> New options:
>
> - Howitzers
>
> The landing on Gotol was a complete success, but after we destroyed their camp in the landing sector, the Thoranians struck back immediately. They have captured our tritium pump in the south and we are slowly running out of tritium. Restore production volume of 300 T/T in the south sector.

### `hansolo\s06_trirot` – Tritium Rot / Tritium red

id 7, Titelindex 72, Fraktionen zuweisbar 1 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 1): Tritium-Förderrate von Fraktion 1 (eigene) mindestens 16.25 (Q4 260 pro Tick)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 2): Fraktion 1 (eigene) hat kein(e) Hauptquartier (ARM 300) mehr
- Abdeckung 3b: **gut** – erzeugt: „Tritium-Förderung steigern (16.25)“

Missionshilfe DE (0x230498):

> Planet:      Gotol
> Klima:       Wüste
>
> Neue Optionen:
> -Haubitzen
>
> Die Landung auf Gotol war ein voller Erfolg, doch nachdem wir ihr Lager im Landungs-Sektor zerstört haben, schlugen die Avares sofort zurück. Sie haben unsere Tritiumpumpen im Westen erobert und uns geht das Tritium langsam aus. Stellen Sie die Fördermenge von 300 T/T im Westsektor wieder her.

Missionshilfe EN:

> Planet:      Gotol
> Climate:       Desert
>
> New options:
> - howitzers
>
> The landing on Gotol was a complete success, but after we destroyed their camp in the landing sector, the Avares struck back immediately. They have captured our tritium pump in the west and we are slowly running out of tritium. Restore production volume of 300 T/T in the west sector.

### `hansolo\s07_landunt` – Landunter / Land under water

id 8, Titelindex 73, Fraktionen zuweisbar 1 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 2): Fraktion 2 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat kein(e) Hauptquartier (ARM 300) mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 2 vernichten“

Missionshilfe DE (0x2304A8):

> Planet:      Gotol
> Klima:       Wüste
>
> Auf Gotol wurde ein geheimes Forschungsprojekt betrieben, das während des Verfalls in Vergessenheit geraten ist. In den Tiefen eines neu angelegten Stausees liegt eine unserer Basen aus den alten Tagen der Allianz. Erobern Sie das Gebiet um den Staudamm, so dass wir ihn sprengen und so das alte Forschungszentrum freilegen können.

Missionshilfe EN:

> Planet:      Gotol
> Climate:       Desert
>
> At one time, a secret research project was conducted on Gotol, but it was forgotten during the decay of the Alliance. One of our bases from the old days of the Alliance is at the bottom of a new reservoir. Capture the area around the dam so that we can blow it up and regain access to the old research centre.

### `hansolo\s08_instand` – Instandsetzung / Maintenance

id 9, Titelindex 74, Fraktionen zuweisbar 1 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 5): Fraktion 1 (eigene) hat mindestens 2 x Hauptquartier (ARM 300) UND Fraktion 1 (eigene) hat mindestens 3 x Xenit-Mine (ARM 330) UND Fraktion 1 (eigene) hat mindestens 4 x Kraftwerk (ARM 310)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat kein(e) Hauptquartier (ARM 300) mehr
- Abdeckung 3b: **teilweise** – erzeugt: „Hauptquartier bauen (2) und Xenit-Mine bauen (3) und Kraftwerk bauen (4)“

Missionshilfe DE (0x2304B8):

> Planet:      Gotol
> Klima:       Wüste
>
> Neue Optionen:
> -Forschungslabor Laser und Ionen
>
> Dank Ihres Einsatzes konnten wir den Staudamm sprengen und die alte Basis freilegen. Auch die alte Forschungsstation ist wieder in Betrieb genommen worden. Bauen Sie unsere Basis auf 2 Hauptquartiere, 4 Kraftwerke und 3 Xenitminen aus. Falls ein anders Volk unsere Machtposition gefährdet oder an unsere neue Forschung kommen will, schalten Sie seine Streitkräfte aus.

Missionshilfe EN:

> Planet:      Gotol
> Climate:       Desert
>
> New options:
> - Research laboratory for lasers and ions 
>
> Thanks to your operation, we were able to blow up the dam and regain access to the old base. The old research centre has also been put back into operation. Expand our base to 2 headquarters, 4 power plants and 3 xenite mines. If another race of people endangers our power base or tries to steal our research results, eliminate their armies.

### `hansolo\s09_krieg` – Krieg / War

id 10, Titelindex 75, Fraktionen zuweisbar 1 / aktiv 4.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 35): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 4 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat kein(e) Hauptquartier (ARM 300) mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner vernichten“

Missionshilfe DE (0x2304C8):

> Planet:      Gotol
> Klima:       Wüste
>
> Neue Optionen:
> -Walkerfabrik
>
> Wir stehen vor der entscheidenden Schlacht - wenn wir die einzigen fruchtbare Hochebene erobert haben, halten wir Gotol in unseren Händen! Mit weiterem Widerstand wäre nicht mehr zu rechnen. Schicken Sie sich an, Gotol auf diese Weise in die Allianz zurück zu bringen.

Missionshilfe EN:

> Planet:      Gotol
> Climate:       Desert
>
> New options:
> - Walker factory
>
> We are coming up to the decisive battle - if we can capture the only fertile high plain, we will have complete control of Gotol! No further resistance would be expected. Commence operations to bring Gotol back into the Alliance with this method.

### `hansolo\s10_flug` – Flugzeugbergung / Aircraft recovery

id 11, Titelindex 52, Fraktionen zuweisbar 1 / aktiv 5.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 4 hat keine Einheiten mehr UND Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner vernichten“

Missionshilfe DE (0x230358):

> Planet:      Urdon
> Klima:       Eiswüste
>
> Neue Optionen:
> -Lenkraketen
> -Thermoraketen
>
> Bei den Vorbereitungen für die Invasion auf dem Planeten Urdon ist ein Spionageflugzeug der ARES aus ungeklärter Ursache abgestürzt. Unsere Satelliten im Orbit haben neben dem Peilsender feindliche Radarsignale von der Absturzstelle aufgefangen. Die Daten, die sich an Bord befinden, dürfen auf gar keinen Fall in falsche Hände geraten. Sie werden - mit genügend Rohstoffen ausgerüstet - circa eine Meile von der Absturzstelle entfernt auf dem Packeis abgesetzt. Organisieren Sie einen bewaffneten Bergungstrupp, und sorgen Sie für die uneingeschränkte Gebietshoheit über das gesamte Areal. Wir vermuten starke Feindpräsenz, da in der Gegend verstärkt nach Rohstoffen gesucht wird.

Missionshilfe EN:

> Planet:      Urdon
>
> Climate:       Ice desert
>
> New options:
> - Guided missiles
> - Heat-seeking missiles
>
> During the preparations for the invasion of the planet Urdon, an ARES spy plane crashed for unknown reasons. Our satellites in orbit have picked up enemy radar signals as well as those of the directional transmitter from the crash site. The data which is on board must under no circumstances fall into the wrong hands. You will be given adequate raw materials and dropped off on the pack ice about a mile from the crash site. Organise an armed recovery unit and secure total control over the entire area. We presume there will be considerable enemy presence as the search for raw materials has been intensified in the area.

### `hansolo\s11_warn` – Warnschuß / Warning shot

id 12, Titelindex 53, Fraktionen zuweisbar 1 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 2): Fraktion 2 hat kein(e) leichter Geschuetzturm (ARM 370) mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „leichter Geschuetzturm zerstören“

Missionshilfe DE (0x230368):

> Planet:      Urdon
> Klima:       Eiswüste
>
> Neue Optionen:
> -Ionenwerfer
>
> Wer auch immer das Radar an der Absturzstelle installiert hat, benutzte die Kennung unseres alten Feindes, der Golrathen. Halten Sie in jedem Falle die Augen offen.
> Wie dem auch sei, Ziel des geborgenen Flugzeugs war unsere erste Xenitförderanlage auf Urdon. Die Ilurer haben einen massiven Angriff auf unsere dortige Stellung begonnen. Vermutlich wird die Förderstation bei Ihrem Eintreffen bereits völlig zerstört sein. Erteilen Sie diesen dreisten Angreifern eine Lektion und sichern Sie die Xenitproduktion dauerhaft.

Missionshilfe EN:

> Planet:      Urdon
> Climate:       Ice desert
>
> New options:
> - Ion thrower
>
> Whoever has installed the radar at the crash site is using the identification signal of our old enemies, the Golraths. Keep your eyes open in any case.
> Be that as it may, the destination of the recovered aircraft was our first xenite mining facility on Urdon. The Ilurens have launched a massive attack on our position there. Presumably the mining facility will have been completely destroyed by the time you arrive. Teach these impertinent attackers a lesson and secure the xenite production permanently.

### `hansolo\s12_ueber` – Überraschung / Surprise

id 13, Titelindex 54, Fraktionen zuweisbar 3 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr
- Sieg, Trigger 2 (Endauswahl 2, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **teilweise** – erzeugt: „Alle Gegner vernichten (+1 weitere)“

Missionshilfe DE (0x230378):

> Planet:      Urdon
> Klima:       Eiswüste
>
> Die seismischen Scanner haben auf ganz Urdon zwei gewaltige Detonationen verzeichnet. Sie müssten sich in unmittelbarer Nähe befinden. Gehen Sie der Ursache auf den Grund. Die Ilurer haben einem Waffenstillstand zugestimmt. Diese Sache hat absoluten Vorrang. Wir fürchten, die Golrathen selbst sind zurückgekehrt.

Missionshilfe EN:

> Planet:      Urdon
> Climate:       Ice desert
>
> The seismic scanners have recorded two enormous detonations on Urdon. You must be in the immediate vicinity. Find out exactly what has happened. The Ilurens have agreed to a cease-fire. The matter has top priority. We fear that the Golraths themselves have returned.

### `hansolo\s13_kfeuer` – Kreuzfeuer / Cross-fire

id 14, Titelindex 55, Fraktionen zuweisbar 3 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 12): Tritium-Förderrate von Fraktion 1 (eigene) mindestens 15 (Q4 240 pro Tick)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Tritium-Förderung steigern (15)“

Missionshilfe DE (0x230388):

> Planet:      Avares
> Klima:       gemäßigt
>
> Neue Optionen:
> -Spezialforschunglabor
>
> Der alte Feind der Allianz ist mächtiger als je zuvor zurückgekehrt. Seine Absichten liegen auf der Hand. Die Allianz ist zerschlagen, die Völker liegen im Krieg. Es ist eine günstige Gelegenheit, die Herrschaft über das gesamte System an sich zu reißen. Niemand allein wäre stark genug, sich den Golrathen entgegenzustellen. Zu allem Überfluss haben sie eine Invasion auf Avares gestartet, mit dem Ziel die Tritiumförderung der Allianz zu zerschlagen. Für den Fortbestand von Ares wäre dieser Schlag eine große Bedrohung. Sichern Sie auf Avares eine Tritiumförderung von 240t

Missionshilfe EN:

> Planet:      Avares
> Climate:       moderate
>
> New options:
> - Special research laboratory
>
> The Alliance's old enemy has returned, more powerful than ever. His intentions are obvious. The Alliance has collapsed, its peoples at war with one another. It is a favourable opportunity to take control of the entire system. No-one alone would be strong enough to stand up to the Golraths. As if this were not enough, they have begun an invasion of Avares, where they hope to destroy the Alliance's tritium production. Such a blow would be a serious threat to the existence of ARES. Secure the production of 240 t of tritium on Avares.

### `hansolo\s14_feind` – Der alte Feind / The old enemy

id 15, Titelindex 56, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 12): Tritium-Förderrate von Fraktion 1 (eigene) mindestens 18.75 (Q4 300 pro Tick)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Tritium-Förderung steigern (18.75)“

Missionshilfe DE (0x230398):

> Planet:      Avares
> Klima:       gemäßigt
>
> Neue Optionen:
> -Spezialforschungslabor
>
> Der alte Feind der Allianz ist mächtiger als je zuvor zurückgekehrt. Seine Absichten liegen auf der Hand. Die Allianz ist zerschlagen, die Völker liegen im Krieg. Es ist eine günstige Gelegenheit, die Herrschaft über das gesamte System an sich zu reißen. Die Ilurer sind die Allianz zurückgekehrt doch allein ist die Allianz nicht stark genug sich den Golrathen entgegenzustellen. Zu allem Überfluss haben sie eine Invasion auf Avares gestartet, mit dem Ziel die Tritiumförderung der Allianz zu zerschlagen. Für den Fortbestand von Ares wäre dieser Schlag eine große Bedrohung. Sichern Sie eine Tritiumförderung von 300t

Missionshilfe EN:

> Planet:      Avares
> Climate:       moderate
>
> New options:
> - Special research laboratory
> The Alliance's old enemy has returned, more powerful than ever. His intentions are obvious. The Alliance has collapsed, its peoples at war with one another. It is a favourable opportunity to take control of the entire system. The Ilurens have returned to the fold of the Alliance, but the Alliance is not strong enough to stand up to the Golraths. As if this were not enough, they have begun an invasion of Avares, where they hope to destroy the Alliance's tritium production. Such a blow would be a serious threat to the existence of ARES. Secure the production of 300 t of tritium on Avares.

### `hansolo\s15_bombe` – Zeitbombe / Time bomb

id 16, Titelindex 57, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat kein(e) Tritium-Pumpe (ARM 332) mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 13): Fraktion 1 (eigene) hat keine Einheiten mehr ODER Countdown abgelaufen (1800 s = 30:00)
- Abdeckung 3b: **gut** – erzeugt: „Tritium-Pumpe zerstören“

Missionshilfe DE (0x2303A8):

> Planet:      Avares
> Klima:       gemäßigt
>
> Die Ilurer haben uns eine wichtige Information über eine geplante Invasion der Golrathen auf Gotol zukommen lassen. Von Avares aus soll eine Invasionsflotte schon bald in Richtung Gotol aufbrechen. Verhindern können wir dies nicht, jedoch hat unsere Aufklärung die wichtigste Tritiumquelle der Golrathen auf Avares ausfindig gemacht. Wir werden Sie mit einem kleinen Landungskorps durch die feindlichen Linien dorthin bringen. Vielleicht gelingt es, die Invasion von Gotol zumindest abzuschwächen. Sie haben 30 Minuten Zeit.

Missionshilfe EN:

> Planet:      Avares
> Climate:       moderate
>
> The Ilurens have sent us important information that the Golraths are planning an invasion of Gotol. An invasion fleet will soon be taking off from Avares, heading towards Gotol. We cannot stop the invasion, but our reconnaissance has discovered the Golraths' most important source of tritium on Avares. We will take you there with a small landing corps and drop you behind enemy lines. Perhaps we can then at least weaken the invasion of Gotol. You have 30 minutes.

### `hansolo\s16_trommel` – Trommelfeuer / Drumfire

id 17, Titelindex 58, Fraktionen zuweisbar 5 / aktiv 5.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 5): Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 5 vernichten“

Missionshilfe DE (0x2303B8):

> Planet:      Gotol
> Klima:       gemäßigt
>
> Neue Optionen:
> -Reparaturmodul
>
> Nachdem Sie die golrathische Tritiumproduktion auf Avares zerschlagen haben, übernehmen Sie die Aufgabe, die Invasion abzuwehren. Die Chancen, Gotol gegen die Golrathen zu verteidigen, sind gering. Dennoch könnte es uns gelingen, wenigstens einen Sektor zu halten. Zerschlagen Sie die Invasionsarmee der Golrathen, bevor sie sich festgefressen haben. Wir hoffen, dass die Völker, welche ebenfalls dort ansässig sind, die Situation richtig einschätzen und der ARES beistehen.

Missionshilfe EN:

> Planet:      Gotol
> Climate:       moderate
>
> New options:
> -Repair module
>
> Now that you have destroyed the Golrathian tritium production on Avares, you have been assigned the task of defeating the invasion. The chances of defending Gotol against the Golraths are slight. Still, we might succeed in holding at least one sector. Smash the Golrath invasion army before they can dig in. We hope that the peoples who live there will evaluate the situation correctly and support the ARES.

### `hansolo\s17_heftig` – Starkes Trommelfeuer / Intense drumfire

id 18, Titelindex 59, Fraktionen zuweisbar 5 / aktiv 5.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 5): Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 5 vernichten“

Missionshilfe DE (0x2303C8):

> Planet:      Gotol
> Klima:       gemäßigt
>
> Neue Optionen:
> -Reparaturmodul
>
> Nachdem Sie die golrathische Tritiumproduktion auf Avares nicht zerschlagen konnten, sind unsere Chancen, Gotol gegen die Golrathen zu verteidigen, sehr gering. Dennoch könnte es uns gelingen, wenigstens einen Sektor zu halten. Zerschlagen Sie die Invasionsarmee der Golrathen, bevor sie sich festgefressen haben. Wir hoffen, dass die Völker, welche ebenfalls dort ansässig sind, die Situation richtig einschätzen und der ARES beistehen.

Missionshilfe EN:

> Planet:      Gotol
> Climate:       moderate
>
> New options:
> -Repair module
>
> As you were unable to destroy the Golrathian tritium production on Avares, our chances of defending Gotol against the Golraths are minimal. Still, we might succeed in holding at least one sector. Smash the Golrath invasion army before they can dig in. We hope that the peoples who live there will evaluate the situation correctly and support the ARES.

### `hansolo\s18_flucht` – Flucht nach vorn / Forward flight

id 19, Titelindex 60, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 12): Fraktion 2 hat keine Einheiten mehr ODER Countdown abgelaufen (1800 s = 30:00)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat kein(e) Geschuetzturm (ARM 340) mehr
- Abdeckung 3b: **teilweise** – erzeugt: „Fraktion 2 vernichten oder Durchhalten (30:00)“

Missionshilfe DE (0x2303D8):

> Planet:      Gotol
> Klima:       gemäßigt
>
> Die Übermacht der Golrathen auf Gotol wird immer drückender. Auch der letzte alliierte Sektor kann nicht mehr länger gegen die Golrathen verteidigt werden. Es sind bereits Transportschiffe im Orbit von Gotol eingetroffen, um die restlichen Truppen zu evakuieren. Um die letzte Phase der Operation durchzuführen, ist es notwendig, die einzige verbleibende Radaranlage der Allianz abzusichern, bis die Evakuierung abgeschlossen ist. Wir rechnen mit massiven Angriffen der Golrathen. Wenn es möglich ist, starten Sie einen Gegenangriff.

Missionshilfe EN:

> Planet:      Gotol
> Climate:       moderate
>
> The Golraths' superiority on Gotol is gaining the upper hand. The last Allied sector cannot be defended much longer against the Golraths. Transport ships have already gone into orbit around Gotol to evacuate the remaining troops. If the final phase of the operation is to be carried out, the Alliance's only remaining radar station must be secured until evacuation has been completed. We expect massive Golrath attacks. If possible, launch a counter-attack.

### `hansolo\s19_forschung` (id 20)

Level-Datei fehlt in LEVEL.PCK (im .cgn eingetragen).

### `hansolo\s20_posten` – Der Außenposten / The outpost

id 21, Titelindex 62, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 4 hat keine Einheiten mehr UND Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner vernichten“

Missionshilfe DE (0x2303F8):

> Planet:      Golkarath
> Klima:       vulkanisch
>
> Neue Optionen:
> -Walker Gorilla
>
> ARES hat mit der Landung auf Golkarath begonnen. Sie übernehmen die Führung über das erste Landungskorps. Ihre Aufgabe besteht darin, das Zielgebiet für die Invasion vorzubereiten. Unsere Aufklärung hat einen golrathischen Außenposten geortet. Schalten Sie ihn aus und machen Sie den Weg für unsere Truppen frei.
> Unsere Scanner haben Einschläge von Meteoriten im Zielgebiet geortet. Achten Sie auf eventuelle Schäden.

Missionshilfe EN:

> Planet:      Golkarath
> Climate:       volcanic
>
> New options:
> - Walker Gorilla
>
> ARES has begun with the landing on Golkarath. You take command of the first landing corps. Your task is to prepare the target area for the invasion. Our reconnaissance has located a Golrathian outpost there. Eliminate the outpost and free the way for our troops.
> Our scanners have discovered meteorite strikes in the target area. Watch out for possible damage.

### `hansolo\s21_Marsch` – Auf dem Vormarsch / On the advance

id 22, Titelindex 63, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 4 hat keine Einheiten mehr UND Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner vernichten“

Missionshilfe DE (0x230408):

> Planet:      Golkarath
> Klima:       vulkanisch
>
> Neue Optionen:
> -Panzer Goliath
> -Artillerie
>
> Aus dem gesamten Sonnensystem treffen beunruhigende Nachrichten ein. Die Golrathen erobern Planet um Planet.
> Das unterstreicht die Wichtigkeit Ihrer Mission.
> Die Landung auf Golkarath ist geglückt und die alliierten Truppen sind auf dem Vormarsch. Ihre Pioniere haben die Aufgabe, die geplante Marschroute von feindlichen Kräften zu säubern. Wir rechnen mit extrem starker Feindpräsenz.

Missionshilfe EN:

> Planet:      Golkarath
> Climate:       volcanic
>
> New options:
> - Tank Goliath
> - Artillery
>
> Disturbing news is coming in from all over the solar system. The Golraths are conquering one planet after another.
> This underlines the importance of your mission.
> We have successfully landed on Golkarath and the Allied troops are on the advance. Your Pioneers have the task of clearing enemy forces from the planned route of advance. We expect extremely strong enemy presence.

### `hansolo\s22_laser` – Der Gegenangriff / The counter-attack

id 23, Titelindex 64, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 4 hat keine Einheiten mehr UND Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner vernichten“

Missionshilfe DE (0x230418):

> Planet:      Golkarath
> Klima:       vulkanisch
> Rohstoffe:  häufig
>
> Neue Optionen:
> -
>
> Unserer Schiff im Orbit meldet, dass die Golrathen unweit Ihrer Position mehrere Hubbrücken aktiviert haben. Außerdem ist der Magmapegel stellenweise gestiegen. Erkunden Sie die neuen Gegebenheiten und passen Sie auf, dass Sie nicht ins Kreuzfeuer geraten.

Missionshilfe EN:

> Planet:      Golkarath
> Climate:       volcanic
> Raw materials:  common
>
> New options:
> -
>
> Our ship in orbit reports that the Golraths have activated a number of lift bridges not far from your position. Furthermore, the magma level has risen in a number of places. Reconnoitre the new situation and watch out that you don't get caught in cross-fire.

### `hansolo\s23_tritod` – Tritiumtod / Tritium death

id 24, Titelindex 76, Fraktionen zuweisbar 1 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 3): NICHT (Tritium-Förderrate von Fraktion 2 mindestens 0.0625 (Q4 1 pro Tick))
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 2): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **teilweise** – erzeugt: „Tritium-Förderung von Fraktion 2 stoppen“

Missionshilfe DE (0x2304D8):

> Planet:      Golkarath
> Klima:       vulkanisch
>
> Unsere Heimatwelten sind in arger Bedrängnis! Thorgon ist gefallen und von den Golrathen verwüstet worden. Ein Grund mehr mit voller Härte vorzugehen. Wir haben es bis vor die Tore ihrer Tritiumversorgung geschafft. Das energetische Herz Golkaraths liegt nun in Ihren Händen - bringen Sie es zum Stillstand!

Missionshilfe EN:

> Planet:      Golkarath
> Climate:       volcanic
>
> Our home worlds are under intense pressure! Thorgon has fallen and been wasted by the Golraths. One more reason to proceed without compunction. We have reached the gates of their tritium supply. The heart of Golkarath's energy supply is now in your hands - shut it down!

### `hansolo\s24_indust` – Industriekomplex / Industrial complex

id 25, Titelindex 77, Fraktionen zuweisbar 1 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 1): Fraktion 2 hat keine Einheiten der Kommandogruppe A mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 2): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Feindliche Hauptstreitmacht vernichten“

Missionshilfe DE (0x2304E8):

> Planet:      Golkarath
> Klima:       vulkanisch
>
> Nach der Vernichtung von Golkaraths Tritiumindustrie ist der Vormarsch der Golrathen stark verlangsamt worden. Ganz aufhalten können Sie ihn nun vollständig, da wir den Pass, der zu ihrem Hauptindustriekomplex führt, freigekämpft haben. Ihnen wird ein kleiner Spezialtrupp zugewiesen - schlagen Sie schnell und gründlich zu und ein Ende des Schreckens rückt in greifbare Nähe.

Missionshilfe EN:

> Planet:      Golkarath
> Climate:       volcanic
>
> Following the destruction of Golkarath's tritium industry, the Golraths' advance has been slowed considerably. You now have the opportunity to bring it to a complete standstill, for we have freed the pass leading to their main industrial complex. A small troop of specialists will be assigned to your command - strike quickly and thoroughly, and an end to the horror will be within our grasp.

### `hansolo\s25_finale` – Der Todesstoß / The fatal blow

id 26, Titelindex 65, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 11): Fraktion 2 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 3 hat keine Einheiten mehr UND Fraktion 4 hat keine Einheiten mehr UND Fraktion 5 hat keine Einheiten mehr
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 10): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner vernichten“

Missionshilfe DE (0x230428):

> Planet:      Golkarath
> Klima:       vulkanisch
> Rohstoffe:  häufig
>
> Neue Optionen:
> -
>
> Golkaraths Schwerindustrie ist empfindlich getroffen worden. Wenn es uns jetzt geling, die letzte und am schwersten verteidigte Bastion - das militärische Hauptquartier der Golrathen einzunehmen - sind wir endlich am Ziel.
>
> Viel Glück !

Missionshilfe EN:

> Planet:      Golkarath
> Climate:       volcanic
> Raw materials:  common
>
> New options:
> -
>
> Golkarath's heavy industry has suffered crippling blows. If we can now manage to capture the last and most heavily fortified bastion - Golrathian military headquarters - we will at last have reached our goal.
>
> Good luck!

## Kyrrdon (nimm2.cgn)

### `nimm2\ersterkontakt` – Multi: Erster Kontakt / Multi: First Contact

id 1, Titelindex 14, Fraktionen zuweisbar 2 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 0 (Endauswahl 1, Bedingung 3): Fraktion 3 hat kein(e) Geschuetzturm (ARM 340) mehr
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Geschuetzturm zerstören“

Missionshilfe DE (0x2300F8):

> Sie wurden auserwählt die Vormachtstellung der Golraten in unserem System zu brechen. Aus diesem Grund werden Sie auf dem Planeten Kyrrdon landen, dessen enorme Rohstoffreserven eine Wende im Kampf gegen die Golraten bringen könnten.
> Leider ist es diesen gelungen den Planeten schon vor uns zu okkupieren.
> Für eine erfolgreiche Durchführung der Operation ist es daher notwendig eng mit den Truppen unserer Verbündeten zusammenzuarbeiten. Sprechen Sie ein gemeinsames Vorgehen ab, um möglichst rasch die Vormacht auf Kyrrdon zu erringen.
>
> Sie werden auf Kyrrdon in der Nähe der Küste abgesetzt, um den feindlichen Radar zu unterwandern. Suchen Sie sich an der Küste einen geeigneten Landungsplatz und beginnen sofort mit der Errichtung eines Lagers.
> Ihr primäres Ziel soll es sein die Radarstellung der Golraten im Norden, auf einer Anhöhe gelegen, zu zerstören, um ein weiteres Vorrücken der Truppen ins Hinterland zu ermöglichen.
> Sekundär sollen Sie den Vorposten des Gegners am Fuße des Berges zerstören. Unterschätzen Sie seine Schlagkraft nicht.

Missionshilfe EN:

> You have been chosen to break the dominance of the Golraths in our system. In carrying out your mission, you will land on the planet Kyrrdon, whose enormous raw material reserves could give us a decisive advantage in our struggle against the Golraths.
> Unfortunately, the Golraths have succeeded in occupying the planet before us.
> If the operation is to have any chance for success, you will have to cooperate closely with our allies' troops. You must agree on a common strategy to take over Kyrrdon as soon as possible.
>
> You will land on Kyrrdon in the area of the coast so that you will not be picked up by enemy radar. Find a suitable landing place on the coast and begin immediately with the construction of a camp.
> Your primary goal is the destruction of the Golrath radar station in the north, located on top of a mountain, so that troops can move forward into the hinterland.
> Your secondary goal is the destruction of an enemy outpost at the foot of the mountain. Do not underestimate his striking power.

#### Fraktion 2

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 3): Fraktion 3 hat kein(e) Geschuetzturm (ARM 340) mehr
- Niederlage, Trigger 3 (Endauswahl 0, Bedingung 2): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Geschuetzturm zerstören“

Missionshilfe DE (0x2300F9):

> Sie wurden auserwählt die Vormachtstellung der Golraten in unserem System zu brechen. Aus diesem Grund werden Sie auf dem Planeten Kyrrdon landen, dessen enorme Rohstoffreserven eine Wende im Kampf gegen die Golraten bringen könnten.
> Leider ist es diesen gelungen, den Planeten schon vor uns zu okkupieren.
> Für eine erfolgreiche Durchführung der Operation ist es daher notwendig, eng mit den Truppen unserer Verbündeten zusammenzuarbeiten. Sprechen Sie ein gemeinsames Vorgehen ab, um möglichst rasch die Vormacht auf Kyrrdon zu erringen.
>
> Sie werden auf Kyrrdon in der Nähe der Küste abgesetzt, um den feindlichen Radar zu unterwandern. Suchen Sie sich an der Küste einen geeigneten Landungsplatz und beginnen sofort mit der Errichtung eines Lagers.
> Ihr primäres Ziel soll es sein, die Radarstellung der Golraten im Norden, auf einer Anhöhe gelegen, zu zerstören, um ein weiteres Vorrücken der Truppen ins Hinterland zu ermöglichen.
> Sekundär sollen Sie den Vorposten des Gegners am Fuße des Berges zerstören. Unterschätzen Sie seine Schlagkraft nicht.

Missionshilfe EN:

> You have been chosen to break the dominance of the Golraths in our system. In carrying out your mission, you will land on the planet Kyrrdon, whose enormous raw material reserves could give us a decisive advantage in our struggle against the Golraths.
> Unfortunately, the Golraths have succeeded in occupying the planet before us.
> If the operation is to have any chance for success, you will have to cooperate closely with our allies' troops. You must agree on a common strategy to take over Kyrrdon as soon as possible.
>
> You will land on Kyrrdon in the area of the coast so that you will not be picked up by enemy radar. Find a suitable landing place on the coast and begin immediately with the construction of a camp.
> Your primary goal is the destruction of the Golrath radar station in the north, located on top of a mountain, so that troops can move forward into the hinterland.
> Your secondary goal is the destruction of an enemy outpost at the foot of the mountain. Do not underestimate his striking power.

### `nimm2\eisigehöhen` – Multi: Eisige Höhen / Multi: Icy Heights

id 2, Titelindex 15, Fraktionen zuweisbar 2 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 0 (Endauswahl 1, Bedingung 3): Fraktion 3 hat kein(e) Xenit-Silo (ARM 331) mehr UND Fraktion 3 hat kein(e) Tritium-Tank (ARM 333) mehr
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **teilweise** – erzeugt: „Xenit-Silo zerstören und Tritium-Tank zerstören“

Missionshilfe DE (0x230108):

> Durch Ihren erfolgreichen Einsatz ist es uns nun möglich, weiter ins Landesinnere vorzudringen. Es wird für Sie ein harter Weg durch die unwirtliche Hochgebirgswelt sein.
> Das Ziel dieser Mission liegt darin, die Versorgungswege des Feindes nachhaltig zu stören. Deshalb ist es von höchster Priorität ihr Tanklager zu erreichen und zu vernichten. Rücken Sie mit einer gut ausgerüsteten Mannschaft vor, da uns von massiven Verteidigungsanlagen berichtet wurde.

Missionshilfe EN:

> Thanks to the success of your operation, we can now penetrate deeper into the interior. But your path through the forbidding mountains will be arduous.
> The goal of this mission is to destroy permanently the enemy supply lines. That is why the highest priority is to reach their container depot and destroy it. Equip your team well, for we have reports of massive defence systems.

#### Fraktion 2

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 3): Fraktion 3 hat kein(e) Xenit-Silo (ARM 331) mehr UND Fraktion 3 hat kein(e) Tritium-Tank (ARM 333) mehr
- Niederlage, Trigger 3 (Endauswahl 0, Bedingung 2): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **teilweise** – erzeugt: „Xenit-Silo zerstören und Tritium-Tank zerstören“

Missionshilfe DE (0x230109):

> Durch Ihren erfolgreichen Einsatz ist es uns nun möglich, weiter ins Landesinnere vorzudringen. Es wird für Sie ein harter Weg durch die unwirtliche Hochgebirgswelt sein.
> Das Ziel dieser Mission liegt darin, die Versorgungswege des Feindes nachhaltig zu stören. Deshalb ist es von höchster Priorität ihr Tanklager zu erreichen und zu vernichten. Rücken Sie mit einer gut ausgerüsteten Mannschaft vor, da uns von massiven Verteidigungsanlagen berichtet wurde.

Missionshilfe EN:

> Thanks to the success of your operation, we can now penetrate deeper into the interior. But your path through the forbidding mountains will be arduous.
> The goal of this mission is to destroy permanently the enemy supply lines. That is why the highest priority is to reach their container depot and destroy it. Equip your team well, for we have reports of massive defence systems.

### `nimm2\kometenhagel` – Multi: Kometenhagel / Multi: Comet Hail

id 3, Titelindex 16, Fraktionen zuweisbar 2 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 0 (Endauswahl 1, Bedingung 6): Fraktion 3 hat kein(e) Xenit-Mine (ARM 330) mehr UND Fraktion 3 hat kein(e) Tritium-Pumpe (ARM 332) mehr
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **teilweise** – erzeugt: „Xenit-Mine zerstören und Tritium-Pumpe zerstören“

Missionshilfe DE (0x230118):

> Im Landesinneren sind Sie nun in ein aktives Vulkangebiet vorgedrungen. Hier gibt es riesige Rohstoffvorkommen, die es den Golraten ermöglicht haben, auf einem Vulkankegel ein gut ausgebautes Lager zu errichten. Ihre Auftrag ist simpel: Zerstören Sie die Rohstoffminen des Gegners. 
> Bereiten Sie sich aber gut auf diesen Schlag vor, da es den Golraten gelungen ist ihre Forschung zum Erfolg zu führen und sie mit neuen Waffentechnologien aufwarten können.

Missionshilfe EN:

> You have now reached an active volcanic area in the interior. There are gigantic raw material deposits here which have enabled the Golraths to erect a strong camp on a volcanic cone. Your objective is simple: destroy the enemy raw material mines. 
> But make very careful preparations for this attack, for the Golraths have managed to bring their research to a successful conclusion and they can face you with new weapon technologies at their disposal.

#### Fraktion 2

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 6): Fraktion 3 hat kein(e) Xenit-Mine (ARM 330) mehr UND Fraktion 3 hat kein(e) Tritium-Pumpe (ARM 332) mehr
- Niederlage, Trigger 3 (Endauswahl 0, Bedingung 2): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **teilweise** – erzeugt: „Xenit-Mine zerstören und Tritium-Pumpe zerstören“

Missionshilfe DE (0x230119):

> Im Landesinneren sind Sie nun in ein aktives Vulkangebiet vorgedrungen. Hier gibt es riesige Rohstoffvorkommen, die es den Golraten ermöglicht haben, auf einem Vulkankegel ein gut ausgebautes Lager zu errichten. Ihre Auftrag ist simpel: Zerstören Sie die Rohstoffminen des Gegners. 
> Bereiten Sie sich aber gut auf diesen Schlag vor, da es den Golraten gelungen ist ihre Forschung zum Erfolg zu führen und sie mit neuen Waffentechnologien aufwarten können.

Missionshilfe EN:

> You have now reached an active volcanic area in the interior. There are gigantic raw material deposits here which have enabled the Golraths to erect a strong camp on a volcanic cone. Your objective is simple: destroy the enemy raw material mines. 
> But make very careful preparations for this attack, for the Golraths have managed to bring their research to a successful conclusion and they can face you with new weapon technologies at their disposal.

### `nimm2\sumpfland` – Multi: Sumpfland / Multi: Swampland

id 4, Titelindex 17, Fraktionen zuweisbar 2 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 0 (Endauswahl 1, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 3 vernichten“

Missionshilfe DE (0x230128):

> Sie haben den Golraten bei Ihrem letzten Einsatz einen entscheidenden Schlag versetzt. Der Rückzug scheint begonnen zu haben. 
> Leider ist es dem Gegner dabei gelungen, durch unsere Unachtsamkeit, einige Prototypen mit enormer Feuerkraft zu entwenden. Rücken Sie also vorsichtig vor und versuchen Sie, diese Einheiten unschädlich zu machen, um größeren Schaden abzuwenden. Zerstören Sie auch alle weiteren Einheiten der Golraten.

Missionshilfe EN:

> Your last operation struck a decisive blow against the Golraths. They appear to be on the retreat. 
> Unfortunately, our inattention allowed the enemy to get away with some prototype weapons of enormous firing power. Advance cautiously and try to neutralise these units so that greater losses can be avoided. Destroy any other Golrath units as well.

#### Fraktion 2

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 3 (Endauswahl 0, Bedingung 2): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 3 vernichten“

Missionshilfe DE (0x230129):

> Sie haben den Golraten bei Ihrem letzten Einsatz einen entscheidenden Schlag versetzt. Der Rückzug scheint begonnen zu haben. 
> Leider ist es dem Gegner dabei gelungen, durch unsere Unachtsamkeit, einige Prototypen mit enormer Feuerkraft zu entwenden. Rücken Sie also vorsichtig vor und versuchen Sie, diese Einheiten unschädlich zu machen, um größeren Schaden abzuwenden. Zerstören Sie auch alle weiteren Einheiten der Golraten.

Missionshilfe EN:

> Your last operation struck a decisive blow against the Golraths. They appear to be on the retreat. 
> Unfortunately, our inattention allowed the enemy to get away with some prototype weapons of enormous firing power. Advance cautiously and try to neutralise these units so that greater losses can be avoided. Destroy any other Golrath units as well.

### `nimm2\paradis` – Multi: Paradis / Multi: Paradise

id 5, Titelindex 18, Fraktionen zuweisbar 2 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 0 (Endauswahl 1, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 3 vernichten“

Missionshilfe DE (0x230138):

> Es ist soweit! Die Golraten wurden weit zurückgedrängt. Sie haben ihre verstreuten Kräfte auf einer vorgelagerten Insel gesammelt und erwarten ihre Vernichtung.
> Nehmen Sie Ihren Auftrag aber nicht zu leicht, denn ein verwundetes Tier, das sich bedrängt fühlt, kann ungeahnte Kräfte entwickeln.

Missionshilfe EN:

> You have done it! The Golraths have been pushed far back. They have collected their scattered forces on an island off the coast and await their final destruction.
> But do not be too lax in carrying out this operation, for a wounded animal with its back to the wall can develop unexpected strength.

#### Fraktion 2

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 3 (Endauswahl 0, Bedingung 2): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 3 vernichten“

Missionshilfe DE (0x230139):

> Es ist soweit! Die Golraten wurden weit zurückgedrängt. Sie haben ihre verstreuten Kräfte auf einer vorgelagerten Insel gesammelt und erwarten ihre Vernichtung.
> Nehmen Sie Ihren Auftrag aber nicht zu leicht, denn ein verwundetes Tier, das sich bedrängt fühlt, kann ungeahnte Kräfte entwickeln.

Missionshilfe EN:

> You have done it! The Golraths have been pushed far back. They have collected their scattered forces on an island off the coast and await their final destruction.
> But do not be too lax in carrying out this operation, for a wounded animal with its back to the wall can develop unexpected strength.

## Piraten (luke.cgn)

### `luke\l00_inselkampf` – Multi: Inselkampf / Multi: Island Battle

id 1, Titelindex 43, Fraktionen zuweisbar 2 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 0 (Endauswahl 1, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 3 vernichten“

Missionshilfe DE (0x2302C8):

> Piraten haben die Handelswege in der "Togoya-Passage" blockiert. 
> Unsere Langstreckensensoren haben das Signal der Piraten verloren. Im letzten aufgefangenem Funkspruch waren eindeutig
> Kampfgeräusche zu vernehmen. Wir sind auf die Rohstoff-Lieferungen des Planeten angewiesen.
> Begeben Sie sich unverzüglich ins das kritische Gebiet. Dort wartet bereits ein kleiner Außenposten auf Sie.
> Wegtreten !

Missionshilfe EN:

> Pirates have blocked the trade routes in the "Togoya Passage". 
> Our long-range sensors have lost the pirates' signal. Sounds of fighting could clearly be heard in the last radio message we picked up. We are dependent on the raw material supplies from the planet.
> Move immediately into the critical area. A small outpost is already waiting for you there.
> Dismissed!

#### Fraktion 2

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 3 (Endauswahl 0, Bedingung 2): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 3 vernichten“

Missionshilfe DE (0x2302C9):

> Piraten haben die Handelswege in der "Togoya-Passage" blockiert. 
> Unsere Langstreckensensoren haben das Signal der Piraten verloren. Im letzten aufgefangenem Funkspruch waren eindeutig
> Kampfgeräusche zu vernehmen. Wir sind auf die Rohstoff-Lieferungen des Planeten angewiesen.
> Begeben Sie sich unverzüglich ins das kritische Gebiet. Dort wartet bereits ein kleiner Außenposten auf Sie.
> Wegtreten !

Missionshilfe EN:

> Pirates have blocked the trade routes in the "Togoya Passage". 
> Our long-range sensors have lost the pirates' signal. Sounds of fighting could clearly be heard in the last radio message we picked up. We are dependent on the raw material supplies from the planet.
> Move immediately into the critical area. A small outpost is already waiting for you there.
> Dismissed!

### `luke\l01_dejavu` – Multi: Deja Vu / Multi: Déja Vu

id 2, Titelindex 44, Fraktionen zuweisbar 2 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 0 (Endauswahl 1, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 3 vernichten“

Missionshilfe DE (0x2302D8):

> Gute Arbeit auf den Inseln, weiter so ! Allerdings scheint es so, als ob dies nur ein kleiner Außenposten der Piraten war. Das ganze Ausmaß ihrer Truppenstärke ist uns zum jetzigen Zeitpunkt noch nicht bekannt, jedoch gibt es Hinweise darauf, daß sich die Piraten an die Küste zurückgezogen haben und ihre Truppen in den Bergen sammeln wollen. Wir dürfen das nicht zulassen !
> Daher ist es Ihre Aufgabe, zusammenzuarbeiten, um die Piraten auch diesmal in Ihre Schranken zu weisen! Sie werden an der Küste abgesetzt und sondieren das Terrain. Rechnen Sie mit Feindkontakt !
> Wir können und werden es nicht tolerieren, daß dieses Gesindel uns unsere Rohstoff-Vorkommen streitig macht ! Sie haben Ihre Befehle ! Wegtreten !

Missionshilfe EN:

> Good work on the island, keep it up! However, it appears that this was only a small pirate outpost. At this point in time, we do not have exact information about their troop strength, but there are indications that the pirates have pulled back to the coast and are planning to assemble their troops in the mountains. We must not allow this to happen!
> It is your objective to cooperate to put the pirates in their place! You will be put ashore on the coast and can reconnoitre the terrain. Be prepared for enemy contact!!
> We cannot and will not tolerate this mob of ruffians challenging us for our raw material deposits! You have your orders! Dismissed!

#### Fraktion 2

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 3 (Endauswahl 0, Bedingung 2): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 3 vernichten“

Missionshilfe DE (0x2302D9):

> Gute Arbeit auf den Inseln, weiter so ! Allerdings scheint es so, als ob dies nur ein kleiner Außenposten der Piraten war. Das ganze Ausmaß ihrer Truppenstärke ist uns zum jetzigen Zeitpunkt noch nicht bekannt, jedoch gibt es Hinweise darauf, daß sich die Piraten an die Küste zurückgezogen haben und ihre Truppen in den Bergen sammeln wollen. Wir dürfen das nicht zulassen !
> Daher ist es Ihre Aufgabe, zusammenzuarbeiten, um die Piraten auch diesmal in Ihre Schranken zu weisen! Sie werden an der Küste abgesetzt und sondieren das Terrain. Rechnen Sie mit Feindkontakt !
> Wir können und werden es nicht tolerieren, daß dieses Gesindel uns unsere Rohstoff-Vorkommen streitig macht ! Sie haben Ihre Befehle ! Wegtreten !

Missionshilfe EN:

> Good work on the island, keep it up! However, it appears that this was only a small pirate outpost. At this point in time, we do not have exact information about their troop strength, but there are indications that the pirates have pulled back to the coast and are planning to assemble their troops in the mountains. We must not allow this to happen!
> It is your objective to cooperate to put the pirates in their place! You will be put ashore on the coast and can reconnoitre the terrain. Be prepared for enemy contact!!
> We cannot and will not tolerate this mob of ruffians challenging us for our raw material deposits! You have your orders! Dismissed!

### `luke\l02_gebirgsjäger` – Multi: Gebirgsjäger / Multi: Mountain Troops

id 3, Titelindex 45, Fraktionen zuweisbar 2 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 0 (Endauswahl 1, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 3 vernichten“

Missionshilfe DE (0x2302E8):

> Respekt ! Natürlich wußten wir von dem Lager , aber da wir immer noch nicht den Maulwurf in unseren Reihen identifiziert haben, werden wir wohl weiterhin vorsichtig sein, was  Informationen betrifft, die wir per CommSignal senden. Seien Sie also ständig auf der Hut !
> Unsere Langstrecken Sensoren haben aus den Bergen Funksignale aufgespürt, die nicht natürlichen Ursprungs sein können. Da in diesen abgeschiedenen Regionen jedoch keine uns bekannte intelligente Lebensform beheimatet ist, ist damit zu rechnen, daß es sich um ein weiteres Lager der Piraten handelt. Sorgen Sie dafür, das keiner entkommt.
> Da ist allerdings noch etwas: eine Tetra-Band-Anlayse der Region ergab erschreckend hohe Werte. Unsere Ingenieure sind sich noch nicht ganz einig um was es sich handelt. Seien Sie also vorsichtig !
> Sie werden in einiger Entfernung vom unbekannten Signal abgesetzt. Die Rohstoffvorkommen in dieser Region sind extrem rar. Kommen Sie also besser mit dem aus, was Sie haben. Um besser im dem unwegsamen Terrain manövrieren zu können ist davon auszugehen, daß die Piraten bereits Straßenbaulich aktiv waren. Nutzen Sie diese Wege zu Ihrem Vorteil.
> Wegtreten !

Missionshilfe EN:

> Our respect! Naturally we knew about the camp, but since we still haven't identified the mole in our ranks, we will continue to be cautious about sending information by CommSignal. Be constantly on your guard! 
> Our long-range sensors have picked up radio signals from the mountains which cannot be of natural origin. But since we are not aware of any intelligent life forms living in these remote regions, we must assume that they are coming from yet another pirate camp. Make sure that no-one escapes.
> One more thing: a tetraband analysis of the region showed horrifyingly high values. Our engineers cannot agree on what the cause is. So be careful! 
> You will be dropped off some distance from the unknown signal. The raw material deposits in this region are extremely rare. It would be best to make do with what you have with you. We can assume that the pirates have already built roads in this inaccessible terrain so that they can manoeuvre better. Use these roads to your own advantage.
> Dismissed!

#### Fraktion 2

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 3): Fraktion 3 hat keine Einheiten mehr
- Niederlage, Trigger 3 (Endauswahl 0, Bedingung 2): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktion 3 vernichten“

Missionshilfe DE (0x2302E9):

> Respekt ! Natürlich wußten wir von dem Lager , aber da wir immer noch nicht den Maulwurf in unseren Reihen identifiziert haben, werden wir wohl weiterhin vorsichtig sein, was  Informationen betrifft, die wir per CommSignal senden. Seien Sie also ständig auf der Hut !
> Unsere Langstrecken Sensoren haben aus den Bergen Funksignale aufgespürt, die nicht natürlichen Ursprungs sein können. Da in diesen abgeschiedenen Regionen jedoch keine uns bekannte intelligente Lebensform beheimatet ist, ist damit zu rechnen, daß es sich um ein weiteres Lager der Piraten handelt. Sorgen Sie dafür, das keiner entkommt.
> Da ist allerdings noch etwas: eine Tetra-Band-Anlayse der Region ergab erschreckend hohe Werte. Unsere Ingenieure sind sich noch nicht ganz einig um was es sich handelt. Seien Sie also vorsichtig !
> Sie werden in einiger Entfernung vom unbekannten Signal abgesetzt. Die Rohstoffvorkommen in dieser Region sind extrem rar. Kommen Sie also besser mit dem aus, was Sie haben. Um besser im dem unwegsamen Terrain manövrieren zu können ist davon auszugehen, daß die Piraten bereits Straßenbaulich aktiv waren. Nutzen Sie diese Wege zu Ihrem Vorteil.
> Wegtreten !

Missionshilfe EN:

> Our respect! Naturally we knew about the camp, but since we still haven't identified the mole in our ranks, we will continue to be cautious about sending information by CommSignal. Be constantly on your guard! 
> Our long-range sensors have picked up radio signals from the mountains which cannot be of natural origin. But since we are not aware of any intelligent life forms living in these remote regions, we must assume that they are coming from yet another pirate camp. Make sure that no-one escapes.
> One more thing: a tetraband analysis of the region showed horrifyingly high values. Our engineers cannot agree on what the cause is. So be careful! 
> You will be dropped off some distance from the unknown signal. The raw material deposits in this region are extremely rare. It would be best to make do with what you have with you. We can assume that the pirates have already built roads in this inaccessible terrain so that they can manoeuvre better. Use these roads to your own advantage.
> Dismissed!

### `luke\l03_dasnest` – Multi: Das Nest / Multi: The Nest

id 4, Titelindex 46, Fraktionen zuweisbar 2 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 0 (Endauswahl 1, Bedingung 3): Fraktion 3 hat kein(e) Laserspule (ARM 345) mehr
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 1): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Laserspule zerstören“

Missionshilfe DE (0x2302F8):

> Glückwunsch, wir haben´s fast geschafft!
> Unsere Ingenieure sind jetzt der Übezeugung, daß eine neuartige Waffe die Ursache für die hohen Tetraband-Werte  ist. Spionage-Satelliten, die wir eigens zu diesem Zweck in die Umlaufbahn gebracht haben, haben diese Fotos von einer offensichtlich geheimen Forschungsanlage gemacht. Noch ist unklar, woher die Piraten über das technische Know-How und die Ausrüstung für eine solche Anlage verfügen. Ihr primäres Ziel besteht in der Zerstörung dieser Anlage. 
> Da unsere Ressourcen und Nachschub knapp sind , sind Sie ein weiteres mal auf sich gestellt. Verscherzen Sie es sich also besser nicht !
> Sie werden in den eisigen Höhen des Anuma Massives kämpfen. Wir rechnen zwar nicht mit schweren Angriffen der Piraten, jedoch soll ihre Basis schwer einzunehmen sein.
> Tun Sie Ihr Bestes!

Missionshilfe EN:

> Congratulations, we have almost done it!
> Our engineers are now convinced that a new type of weapon is the cause of the high tetraband values. Spy satellites which we have put into orbit for just this purpose have relayed back photos of what is obviously a secret research facility. It is still unclear where the pirates got the technical expertise and the equipment for such a facility. Your primary objective is the destruction of this facility. 
> As our resources and supplies are low, you will be on your own once again. It would be best not to relax! 
> You will be fighting in the icy heights of the Anuma Massif. We do not believe that there will be a major attack by the pirates, but their base will supposedly be difficult to take.
> Do your best!

#### Fraktion 2

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 3): Fraktion 3 hat kein(e) Laserspule (ARM 345) mehr
- Niederlage, Trigger 3 (Endauswahl 0, Bedingung 2): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Laserspule zerstören“

Missionshilfe DE (0x2302F9):

> Glückwunsch, wir haben´s fast geschafft!
> Unsere Ingenieure sind jetzt der Übezeugung, daß eine neuartige Waffe die Ursache für die hohen Tetraband-Werte  ist. Spionage-Satelliten, die wir eigens zu diesem Zweck in die Umlaufbahn gebracht haben, haben diese Fotos von einer offensichtlich geheimen Forschungsanlage gemacht. Noch ist unklar, woher die Piraten über das technische Know-How und die Ausrüstung für eine solche Anlage verfügen. Ihr primäres Ziel besteht in der Zerstörung dieser Anlage. 
> Da unsere Ressourcen und Nachschub knapp sind , sind Sie ein weiteres mal auf sich gestellt. Verscherzen Sie es sich also besser nicht !
> Sie werden in den eisigen Höhen des Anuma Massives kämpfen. Wir rechnen zwar nicht mit schweren Angriffen der Piraten, jedoch soll ihre Basis schwer einzunehmen sein.
> Tun Sie Ihr Bestes!

Missionshilfe EN:

> Congratulations, we have almost done it!
> Our engineers are now convinced that a new type of weapon is the cause of the high tetraband values. Spy satellites which we have put into orbit for just this purpose have relayed back photos of what is obviously a secret research facility. It is still unclear where the pirates got the technical expertise and the equipment for such a facility. Your primary objective is the destruction of this facility. 
> As our resources and supplies are low, you will be on your own once again. It would be best not to relax! 
> You will be fighting in the icy heights of the Anuma Massif. We do not believe that there will be a major attack by the pirates, but their base will supposedly be difficult to take.
> Do your best!

## Einzel- und Mehrspielerkarten (level.dat)

### `asgard` – Multi: Asgard / Multi: Asgard

Karte, Titelindex 35, Fraktionen zuweisbar 4 / aktiv 4.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230248):

> Wir sind auf die Rohstoffe dieses Sektors angewiesen - sichern Sie unsere Gebietshoheit.

Missionshilfe EN:

> We are dependent on the raw materials in this sector - secure our control of the territory.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230249):

> Wir sind auf die Rohstoffe dieses Sektors angewiesen - sichern Sie unsere Gebietshoheit.

Missionshilfe EN:

> We are dependent on the raw materials in this sector - secure our control of the territory.

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x23024A):

> Wir sind auf die Rohstoffe dieses Sektors angewiesen - sichern Sie unsere Gebietshoheit.

Missionshilfe EN:

> We are dependent on the raw materials in this sector - secure our control of the territory.

#### Fraktion 4

- Sieg, Trigger 7 (Endauswahl 1, Bedingung 24): (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 6 (Endauswahl 0, Bedingung 34): Fraktion 4 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x23024B):

> Wir sind auf die Rohstoffe dieses Sektors angewiesen - sichern Sie unsere Gebietshoheit.

Missionshilfe EN:

> We are dependent on the raw materials in this sector - secure our control of the territory.

### `badewanne` – Multi: Kattara / Multi: Kattara

Karte, Titelindex 11, Fraktionen zuweisbar 4 / aktiv 4.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300C8):

> Es gilt gegen die 3 Gegner vorzugehen und sich die Rohstoffe zwischen den Lagern zu sichern. Sollte man nicht derjenige sein, der diese Rohstoffe erreicht, ist es sinnvoll, Verbündete zu suchen, um nicht überrollt zu werden.

Missionshilfe EN:

> You must move against your 3 opponents and secure the raw materials found between the camps. If you fail to reach this field, it would be wise for you to seek allies so that you are not overrun.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300C9):

> Es gilt gegen die 3 Gegner vorzugehen und sich die Rohstoffe zwischen den Lagern zu sichern. Sollte man nicht derjenige sein, der diese Rohstoffe erreicht, ist es sinnvoll, Verbündete zu suchen, um nicht überrollt zu werden.

Missionshilfe EN:

> You must move against your 3 opponents and secure the raw materials found between the camps. If you fail to reach this field, it would be wise for you to seek allies so that you are not overrun.

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300CA):

> Es gilt gegen die 3 Gegner vorzugehen und sich die Rohstoffe zwischen den Lagern zu sichern. Sollte man nicht derjenige sein, der diese Rohstoffe erreicht, ist es sinnvoll, Verbündete zu suchen, um nicht überrollt zu werden.

Missionshilfe EN:

> You must move against your 3 opponents and secure the raw materials found between the camps. If you fail to reach this field, it would be wise for you to seek allies so that you are not overrun.

#### Fraktion 4

- Sieg, Trigger 7 (Endauswahl 1, Bedingung 24): (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 6 (Endauswahl 0, Bedingung 34): Fraktion 4 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300CB):

> Es gilt gegen die 3 Gegner vorzugehen und sich die Rohstoffe zwischen den Lagern zu sichern. Sollte man nicht derjenige sein, der diese Rohstoffe erreicht, ist es sinnvoll, Verbündete zu suchen, um nicht überrollt zu werden.

Missionshilfe EN:

> You must move against your 3 opponents and secure the raw materials found between the camps. If you fail to reach this field, it would be wise for you to seek allies so that you are not overrun.

### `fata morgana` – Multi: Fata Morgana / Multi: Mirage

Karte, Titelindex 38, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230278):

> Die Golraten planen mit der Eroberung Gotols den zweiten, wichtigen xenitliefernden Planeten in ihre Hände zu bekommen. Zerstören Sie ihren Brückenkopf bevor größerer Schaden entsteht.

Missionshilfe EN:

> The Golraths are planning to get their hands on a second important source of xenite by conquering the planet Gotol. Destroy their bridgehead before any greater damage can be done.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230279):

> Endlich sind wir in der Lage, das Herz der avarianischen Xenitfördung an uns zu reißen. Expandieren Sie soweit wie möglich in Ihrem Planquadrat, so dass wir die Avares auf diesem Planeten auslöschen können.

Missionshilfe EN:

> At last we are in a position to grasp the heart of the Avarian xenite production for ourselves. Expand as much as you can in your plan quadrant so that we can exterminate the Avares on this planet.

### `gemezel` – Multi: Halmaherra / Multi: Halmaherra

Karte, Titelindex 13, Fraktionen zuweisbar 4 / aktiv 4.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300E8):

> Es bietet sich an, die Position gegen den Gegner abzusichern und danach mit dem Wettrüsten zu beginnen. Hier hilft nur Masse oder überlegene Technologie. Es ist zu vermuten, dass es zu einer kostspieligen Materialschlacht kommen wird.

Missionshilfe EN:

> It is advisable to take up your position against the enemy and to begin building up arms. Nothing will help you except sheer mass or superior technology. There will probably be a costly battle of matériel.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300E9):

> Es bietet sich an, die Position gegen den Gegner abzusichern und danach mit dem Wettrüsten zu beginnen. Hier hilft nur Masse oder überlegene Technologie. Es ist zu vermuten, dass es zu einer kostspieligen Materialschlacht kommen wird.

Missionshilfe EN:

> It is advisable to take up your position against the enemy and to begin building up arms. Nothing will help you except sheer mass or superior technology. There will probably be a costly battle of matériel.

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300EA):

> Es bietet sich an, die Position gegen den Gegner abzusichern und danach mit dem Wettrüsten zu beginnen. Hier hilft nur Masse oder überlegene Technologie. Es ist zu vermuten, dass es zu einer kostspieligen Materialschlacht kommen wird.

Missionshilfe EN:

> It is advisable to take up your position against the enemy and to begin building up arms. Nothing will help you except mass or superior technology. There will probably be a costly battle of matériel.

#### Fraktion 4

- Sieg, Trigger 7 (Endauswahl 1, Bedingung 24): (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 6 (Endauswahl 0, Bedingung 34): Fraktion 4 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300EB):

> Es bietet sich an, die Position gegen den Gegner abzusichern und danach mit dem Wettrüsten zu beginnen. Hier hilft nur Masse oder überlegene Technologie. Es ist zu vermuten, dass es zu einer kostspieligen Materialschlacht kommen wird.

Missionshilfe EN:

> It is advisable to take up your position against the enemy and to begin building up arms. Nothing will help you except sheer mass or superior technology. There will probably be a costly battle of matériel.

### `hargonatoll` – Multi: Hargonatoll / Multi: Hargon Atoll

Karte, Titelindex 37, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230268):

> Planet: Golkarath 
> Zielgebiet: Ein Atoll südlich des Hauptkontinents
> Klima: Angenehm mild
> Rohstoffe: Kompakt und knapp
> Besonderheit: Dieses Szenario spielt zu der Zeit auf Golkarath, als der Planet mehr und mehr von Meteorstürmen heimgesucht und langsam zu der unwirtlichen Welt wurde, die er heute ist. Truppen von Ares haben einen Brückenkopf errichtet und wollen ihn zum sicheren Standbein einer Invasion machen. Damals scheiterte die Expedition.  
>
> Durch unser Zögern bei der Landung im Süden der Insel haben wir unsere Präsenz auf dem Hargonatoll stark gefährdet. Retten Sie, was noch zu retten ist!

Missionshilfe EN:

> Planet: Golkarath 
> Target area: An atoll south of the main continent 
> Climate: pleasantly mild
> Raw materials: compact and scarce
> Special characteristic: This scenario takes place on Golkarath at the time the planet was suffering more and more frequently from meteor storms and slowly turning into the desolation that it is today. Troops from ARES have established a bridgehead and want to develop it into a secure base for an invasion. The expedition at that time failed.  
>
> We have put our presence on the Hargon Atoll into great peril by our hesitation during the landing on the south part of the island. Save whatever you can!

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230269):

> Planet: Golkarath 
> Zielgebiet: Das strahlende Hargonatoll
> Klima: 
> Rohstoffe: Kompakt und knapp
> Besonderheit: Dieses Szenario spielt zu der Zeit auf Golkarath als der Planet mehr und mehr von Meteorstürmen heimgesucht wurde und langsam zu der unwirtlichen Welt wurde, die er heute ist. Truppen von Ares haben einen Brückenkopf errichtet und wollen ihn zum sicheren Standbein einer Invasion machen. Damals scheiterte die Expedition.
>
> Unsere Geschwindigkeit in Kombination mit strikter Härte hat uns in eine ausgezeichnete Position gebracht. Nutzen Sie die Gunst der Stunde und vertreiben Sie die Allianz vom Hargonatoll!!

Missionshilfe EN:

> Planet: Golkarath 
> Target area: The radioactive Hargon Stoll
> Climate: 
> Raw materials: compact and scarce
> Special characteristic: This scenario takes place on Golkarath at the time the planet was suffering more and more frequently from meteor storms and slowly turning into the desolation that it is today. Troops from ARES have established a bridgehead and want to develop it into a secure base for an invasion. The expedition at that time failed.
>
> Our speed in combination with absolute ruthlessness has put us into an excellent position. Take advantage of this favourable moment and drive the Alliance from the Hargon Atoll!!

### `ignis` – Multi: Ignis / Multi: Ignis

Karte, Titelindex 36, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230258):

> Planet: Golkarath 
> Zielgebiet: Die Ignisquellen
> Klima: Glühende Hitze und Meteorschauer herrschen in dieser Gegend vor
> Rohstoffe: Kompakt und knapp
>
> Durch einen Schwachpunkt in der Orbitalverteidigung von Golkarath ist es uns gelungen einen Brückenkopf auf den Heimatplaneten der Golraten zu errichten. Ihre Aufgabe ist es, ein Nebental in der Nähe unseres Landungspunktes von einer kleinen Basis der Golrathen zu säubern.

Missionshilfe EN:

> Planet: Golkarath 
> Target area: The Ignis springs
> Climate: Burning heat and meteor showers predominate in this area 
> Raw materials: compact and scarce 
>
> We have broken through a weak point in the orbital defence of Golkarath and established a bridgehead on the Golraths' home planet. Your objective is the elimination of a small Golrath base in a side valley near our landing point.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230259):

> Planet: Mutter Golkarath
> Zielgebiet: Die heiligen Ignisquellen
> Klima: Glühende Hitze - der Zorn Asgarads regnet auf uns herab
> Rohstoffe: Kompakt und knapp
>
> Überraschenderweise haben wir eine kleine, unbedeutende Basis der Allianz auf unserem Planeten entdeckt. Schalten Sie diese aus - schnell und ohne Gnade.

Missionshilfe EN:

> Planet: Mother Golkarath
> Target area: The sacred Ignis springs
> Climate: Burning heat - The wrath of Asgarad rains down upon us 
> Raw materials: compact and scarce
>
> To our surprise, we have discovered a small, insignificant base of the Alliance on our planet. Eliminate it - quickly and mercilessly.

### `kreuzgang` – Multi: Kreuzgang / Multi: Cloister

Karte, Titelindex 26, Fraktionen zuweisbar 4 / aktiv 4.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301B8):

> Erobern Sie die Rohstoffvorkommen und graben Sie Ihren Feinden damit die Reserven ab.

Missionshilfe EN:

> Capture the raw material deposits and force your enemy to use up his reserves.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301B9):

> Erobern Sie die Rohstoffvorkommen und graben Sie Ihren Feinden damit die Reserven ab.

Missionshilfe EN:

> Capture the raw material deposits and force your enemy to use up his reserves.

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301BA):

> Erobern Sie die Rohstoffvorkommen und graben Sie Ihren Feinden damit die Reserven ab.

Missionshilfe EN:

> Capture the raw material deposits and force your enemy to use up his reserves.

#### Fraktion 4

- Sieg, Trigger 7 (Endauswahl 1, Bedingung 24): (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 6 (Endauswahl 0, Bedingung 34): Fraktion 4 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301BB):

> Erobern Sie die Rohstoffvorkommen und graben Sie Ihren Feinden damit die Reserven ab.

Missionshilfe EN:

> Capture the raw material deposits and force your enemy to use up his reserves.

### `lavam` – Multi: Lavaregen / Multi: Lavaregen

Karte, Titelindex 10, Fraktionen zuweisbar 3 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300B8):

> In einer dunklen Stunde gilt es, die beiden Gegner gegeneinander auszuspielen und sich einen Vorteil zu verschaffen. Die Ausgangsbedingungen sind für alle schwierig, da das Terrain nur schwer abzusichern ist.

Missionshilfe EN:

> In a dark hour, you must play off your two opponents against each other and put yourself at an advantage. Starting conditions are difficult for all as the terrain cannot be secured easily.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300B9):

> In einer dunklen Stunde gilt es, die beiden Gegner gegeneinander auszuspielen und sich einen Vorteil zu verschaffen. Die Ausgangsbedingungen sind für alle schwierig, da das Terrain nur schwer abzusichern ist.

Missionshilfe EN:

> In a dark hour, you must play off your two opponents against each other and put yourself at an advantage. Starting conditions are difficult for all as the terrain cannot be secured easily.

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300BA):

> In einer dunklen Stunde gilt es, die beiden Gegner gegeneinander auszuspielen und sich einen Vorteil zu verschaffen. Die Ausgangsbedingungen sind für alle schwierig, da das Terrain nur schwer abzusichern ist.

Missionshilfe EN:

> In a dark hour, you must play off your two opponents against each other and put yourself at an advantage. Starting conditions are difficult for all as the terrain cannot be secured easily.

### `lucas03` – Multi: Massiver Angriff / Multi: Massive Attack

Karte, Titelindex 42, Fraktionen zuweisbar 5 / aktiv 5.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktionen 2, 3 und 4 ausschalten“

Missionshilfe DE (0x2302B8):

> Feindliche Einheiten befinden sich ganz in Ihrer Nähe - seien Sie auf der Hut!

Missionshilfe EN:

> Enemy units in your immediate vicinity - extreme caution is advised!

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktionen 1, 3 und 4 ausschalten“

Missionshilfe DE (0x2302B9):

> Feindliche Einheiten befinden sich ganz in Ihrer Nähe - seien Sie auf der Hut!

Missionshilfe EN:

> Enemy units in your immediate vicinity - extreme caution is advised!

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktionen 1, 2 und 4 ausschalten“

Missionshilfe DE (0x2302BA):

> Feindliche Einheiten befinden sich ganz in Ihrer Nähe - seien Sie auf der Hut!

Missionshilfe EN:

> Enemy units in your immediate vicinity - extreme caution is advised!

#### Fraktion 4

- Sieg, Trigger 7 (Endauswahl 1, Bedingung 24): (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 6 (Endauswahl 0, Bedingung 34): Fraktion 4 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Fraktionen 1, 2 und 3 ausschalten“

Missionshilfe DE (0x2302BB):

> Feindliche Einheiten befinden sich ganz in Ihrer Nähe - seien Sie auf der Hut!

Missionshilfe EN:

> Enemy units in your immediate vicinity - extreme caution is advised!

#### Fraktion 5

- Sieg: kein Sieg-Trigger
- Abdeckung 3b: **kein Sieg** – erzeugt: „-“

Missionshilfe DE (0x2302BC):

> Feindliche Einheiten befinden sich ganz in Ihrer Nähe - seien Sie auf der Hut!

Missionshilfe EN:

> Enemy units in your immediate vicinity - extreme caution is advised!

### `map08` – Multi: Das Stauwehr / Multi: The Dam

Karte, Titelindex 22, Fraktionen zuweisbar 3 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230178):

> Irgendjemand hat in dem Gebiet den Flusslauf aufgestaut. Wir sind nicht gewillt, dies zu tolerieren. Bringen Sie den Staudamm dauerhaft unter Kontrolle.

Missionshilfe EN:

> Someone has dammed up the river in the area. We are not willing to tolerate this. Secure permanent control of the dam.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230179):

> Irgendjemand hat in dem Gebiet den Flusslauf aufgestaut. Wir sind nicht gewillt, dies zu tolerieren. Bringen Sie den Staudamm dauerhaft unter Kontrolle.

Missionshilfe EN:

> Someone has dammed up the river in the area. We are not willing to tolerate this. Secure permanent control of the dam.

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x23017A):

> Der von uns errichtete Staudamm hat scheinbar einiges Interesse geweckt. Bauen Sie in der Umgebung des Staudamms eine Basis auf und sichern Sie das Gelände rund um den Staudamm.

Missionshilfe EN:

> The dam we erected has apparently awakened someone's interest. Construct a base near the dam and secure the area around the dam.

### `midgard` – Multi: Midgard / Multi: Midgard

Karte, Titelindex 34, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230238):

> Planet: Urdon 
> Zielgebiet: Areal am Midgardkamm
> Klima: Klirrende Kälte
> Rohstoffe: Verstreut aber reichlich
>
> Wir versuchen die weitere Ausbreitung der ansässigen Bevölkerung stark einzudämmen um diese rohstoffhaltige Gegend unter Kontrolle halten zu können

Missionshilfe EN:

> Planet: Urdon 
> Target area: Area at Midgard Ridge
> Climate: Freezing cold
> Raw materials: scattered, but considerable
>
> We are attempting to limit closely the further spread of the local population so that we can keep this area, which is rich in raw materials, under our control

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230239):

> Planet: Urdon 
> Zielgebiet: Siedlungen am Midgardkamm
> Klima: Klirrende Kälte
> Rohstoffe: Verstreut aber reichlich
>
> Ares versucht den Midgardkamm zu entvölkern - uns bleibt nichts anderes übrig, als uns ihnen entgegenzustellen. Ob unser Volk überlebt oder nicht, hängt nunmehr nur von unserer Strategie und Stärke ab. Retten Sie unser Volk!

Missionshilfe EN:

> Planet: Urdon 
> Target area: Settlements at Midgard Ridge
> Climate: Freezing cold
> Raw materials: scattered, but considerable 
>
> ARES is attempting to depopulate the Midgard Ridge - we have no choice but to oppose them. Only our strategy and our strength will determine whether our people will survive. Save our people!

### `mittelpunkt` – Multi: Ahaggar / Multi: Ahaggar

Karte, Titelindex 12, Fraktionen zuweisbar 4 / aktiv 4.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300D8):

> Verteidigen Sie Ihr Hochplateau und schwärmen Sie aus, um in der kargen Wüste Ihre Vorräte aufzubessern. Durch die Vernichtung eines Gegners wächst Ihre Xenitausbeute, da Sie alle aus dem selben Rohstoffvorrat schöpfen.

Missionshilfe EN:

> Defend your high plateau and swarm out into the barren desert to increase your stores of supplies. If you destroy an opponent, your own xenite yield will increase because all of you mine the same raw material deposit.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300D9):

> Verteidigen Sie Ihr Hochplateau und schwärmen Sie aus, um in der kargen Wüste Ihre Vorräte aufzubessern. Durch die Vernichtung eines Gegners wächst Ihre Xenitausbeute, da Sie alle aus dem selben Rohstoffvorrat schöpfen.

Missionshilfe EN:

> Defend your high plateau and swarm out into the barren desert to increase your stores of supplies. If you destroy an opponent, your own xenite yield will increase because all of you mine the same raw material deposit.

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300DA):

> Verteidigen Sie Ihr Hochplateau und schwärmen Sie aus, um in der kargen Wüste Ihre Vorräte aufzubessern. Durch die Vernichtung eines Gegners wächst Ihre Xenitausbeute, da Sie alle aus dem selben Rohstoffvorrat schöpfen.

Missionshilfe EN:

> Defend your high plateau and swarm out into the barren desert to increase your stores of supplies. If you destroy an opponent, your own xenite yield will increase because all of you mine the same raw material deposit.

#### Fraktion 4

- Sieg, Trigger 7 (Endauswahl 1, Bedingung 24): (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 6 (Endauswahl 0, Bedingung 34): Fraktion 4 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2300DB):

> Verteidigen Sie Ihr Hochplateau und schwärmen Sie aus, um in der kargen Wüste Ihre Vorräte aufzubessern. Durch die Vernichtung eines Gegners wächst Ihre Xenitausbeute, da Sie alle aus dem selben Rohstoffvorrat schöpfen.

Missionshilfe EN:

> Defend your high plateau and swarm out into the barren desert to increase your stores of supplies. If you destroy an opponent, your own xenite yield will increase because all of you mine the same raw material deposit.

### `multi1` – Multi: Wüstenfüchse / Multi: Desert Fox

Karte, Titelindex 24, Fraktionen zuweisbar 3 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230198):

> Bauen Sie eine solide Basis und verteidigen Sie Ihr Hoheitsgebiet mit allen Mitteln. Es wurde Feindkontakt gemeldet, also machen Sie sich auf Überraschungen gefasst.

Missionshilfe EN:

> Erect a solid base and defend your territory with all the means at your disposal. There have been reports of enemy contact, so be prepared for surprises.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230199):

> Bauen Sie eine solide Basis und verteidigen Sie Ihr Hoheitsgebiet mit allen Mitteln. Es wurde Feindkontakt gemeldet, also machen Sie sich auf Überraschungen gefasst.

Missionshilfe EN:

> Erect a solid base and defend your territory with all the means at your disposal. There have been reports of enemy contact, so be prepared for surprises.

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x23019A):

> Bauen Sie eine solide Basis und verteidigen Sie Ihr Hoheitsgebiet mit allen Mitteln. Es wurde Feindkontakt gemeldet, also machen Sie sich auf Überraschungen gefasst.

Missionshilfe EN:

> Erect a solid base and defend your territory with all the means at your disposal. There have been reports of enemy contact, so be prepared for surprises.

### `muspelheim` – Multi: Muspelheim / Multi: Muspelheim

Karte, Titelindex 30, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301F8):

> Die Furten über die Eisflüsse müssen unter allen Umständen gesichert werden. Sorgen Sie für einen gut gesicherten Übergang und zerstören Sie alle feindlichen Einheiten.

Missionshilfe EN:

> The fords over the ice rivers must be secured at all costs. Provide for a well-secured crossing and destroy all enemy units.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301F9):

> Die Furten über die Eisflüsse müssen unter allen Umständen gesichert werden. Sorgen Sie für einen gut gesicherten Übergang und zerstören Sie alle feindlichen Einheiten.

Missionshilfe EN:

> The fords over the ice rivers must be secured at all costs. Provide for a well-secured crossing and destroy all enemy units.

### `niflheim` – Multi: Niflheim / Multi: Niflheim

Karte, Titelindex 32, Fraktionen zuweisbar 3 / aktiv 3.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230218):

> Unsere Feinde sind in diesem Gebiet aktiv. Suchen und zerstören Sie jegliche Feindeinheiten.

Missionshilfe EN:

> Our enemies are active in this area. Search out and destroy all enemy units.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230219):

> Unsere Feinde sind in diesem Gebiet aktiv. Suchen und zerstören Sie jegliche Feindeinheiten.

Missionshilfe EN:

> Our enemies are active in this area. Search out and destroy all enemy units.

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x23021A):

> Unsere Feinde sind in diesem Gebiet aktiv. Suchen und zerstören Sie jegliche Feindeinheiten.

Missionshilfe EN:

> Our enemies are active in this area. Search out and destroy all enemy units.

### `schweinebucht` – Multi: Die Schweinebucht / Multi: The Bay of Pigs

Karte, Titelindex 23, Fraktionen zuweisbar 4 / aktiv 4.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230188):

> Unsere Feinde sind in diesem Gebiet aktiv und etablieren in der Nähe unseres Stützpunktes ihr Lager. Verhindern Sie ihre Ausbreitung.

Missionshilfe EN:

> Our enemies are active in this area and are establishing their camp near our base. Contain them.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x230189):

> Unsere Feinde sind in diesem Gebiet aktiv und etablieren in der Nähe unseres Stützpunktes ihr Lager. Verhindern Sie ihre Ausbreitung.

Missionshilfe EN:

> Our enemies are active in this area and are establishing their camp near our base. Contain them.

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x23018A):

> Unsere Feinde sind in diesem Gebiet aktiv und etablieren in der Nähe unseres Stützpunktes ihr Lager. Verhindern Sie ihre Ausbreitung.

Missionshilfe EN:

> Our enemies are active in this area and are establishing their camp near our base. Contain them.

#### Fraktion 4

- Sieg, Trigger 7 (Endauswahl 1, Bedingung 24): (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 6 (Endauswahl 0, Bedingung 34): Fraktion 4 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x23018B):

> Unsere Feinde sind in diesem Gebiet aktiv und etablieren in der Nähe unseres Stützpunktes ihr Lager. Verhindern Sie ihre Ausbreitung.

Missionshilfe EN:

> Our enemies are active in this area and are establishing their camp near our base. Contain them.

### `springquell` – Multi: Springquell / Multi: Fountain

Karte, Titelindex 25, Fraktionen zuweisbar 4 / aktiv 4.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301A8):

> Erobern Sie die Rohstoffvorkommen und halten Sie diese.

Missionshilfe EN:

> Capture the raw material deposits and hold them.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301A9):

> Erobern Sie die Rohstoffvorkommen und halten Sie diese.

Missionshilfe EN:

> Capture the raw material deposits and hold them.

#### Fraktion 3

- Sieg, Trigger 5 (Endauswahl 1, Bedingung 23): (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 4 (Endauswahl 0, Bedingung 33): Fraktion 3 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301AA):

> Erobern Sie die Rohstoffvorkommen und halten Sie diese.

Missionshilfe EN:

> Capture the raw material deposits and hold them.

#### Fraktion 4

- Sieg, Trigger 7 (Endauswahl 1, Bedingung 24): (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet) UND (Fraktion 3 oder 4 inaktiv, oder 3 mit 4 verbündet)
- Niederlage, Trigger 6 (Endauswahl 0, Bedingung 34): Fraktion 4 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301AB):

> Erobern Sie die Rohstoffvorkommen und halten Sie diese.

Missionshilfe EN:

> Capture the raw material deposits and hold them.

### `stromschnelle` – Multi: Stromschnelle / Multi: Rapids

Karte, Titelindex 28, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301D8):

> Erobern Sie die Rohstoffvorkommen und graben Sie Ihren Feinden damit die Reserven ab.

Missionshilfe EN:

> Capture the raw material deposits and force your enemy to use up his reserves.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301D9):

> Erobern Sie die Rohstoffvorkommen und graben Sie Ihren Feinden damit die Reserven ab.

Missionshilfe EN:

> Capture the raw material deposits and force your enemy to use up his reserves.

### `tunguska` – Multi: Tunguska / Multi: Tunguska

Karte, Titelindex 27, Fraktionen zuweisbar 2 / aktiv 2.

#### Fraktion 1

- Sieg, Trigger 1 (Endauswahl 1, Bedingung 21): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 1 oder 3 inaktiv, oder 1 mit 3 verbündet) UND (Fraktion 1 oder 4 inaktiv, oder 1 mit 4 verbündet)
- Niederlage, Trigger 0 (Endauswahl 0, Bedingung 31): Fraktion 1 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301C8):

> Finden Sie Ihren Gegner und vertreiben Sie ihn aus Ihrem Gebiet.

Missionshilfe EN:

> Find your enemy and drive him out of your area.

#### Fraktion 2

- Sieg, Trigger 3 (Endauswahl 1, Bedingung 22): (Fraktion 1 oder 2 inaktiv, oder 1 mit 2 verbündet) UND (Fraktion 2 oder 3 inaktiv, oder 2 mit 3 verbündet) UND (Fraktion 2 oder 4 inaktiv, oder 2 mit 4 verbündet)
- Niederlage, Trigger 2 (Endauswahl 0, Bedingung 32): Fraktion 2 (eigene) hat keine Einheiten mehr
- Abdeckung 3b: **gut** – erzeugt: „Alle Gegner ausschalten“

Missionshilfe DE (0x2301C9):

> Finden Sie Ihren Gegner und vertreiben Sie ihn aus Ihrem Gebiet.

Missionshilfe EN:

> Find your enemy and drive him out of your area.
