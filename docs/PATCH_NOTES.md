# Patch notes

Player-facing changes of Open Thandor compared with the original Thandor 1.05 (Patch 5). The developer view of the
code changes is in the [plans](plans) and the [recovery changelog](../CHANGELOG.md).

Every version lists the changes in English and in German (*Deutsch*).

## 1.0.8 - "Thandor Patch 8" (release candidate)

### English

A mission map for the campaigns, an *Original Thandor* switch, improved pathfinding, rebindable keys, an objective
box, autosave, custom maps from the map editor, and fixes for the crashes reported with 1.0.7.

#### New

- **Campaign map:** the campaigns tab of *Spiel auswählen* shows one button per campaign with your progress
  (for example `Die Invasion 7/24`) and, below it, the map of the selected campaign. Every mission has its original
  number; won missions are green, missions you may play are raised buttons, locked ones are dimmed. Where the
  campaign branches, both paths are shown. A double line marks a mission that takes over the units of the mission
  before it (*Einheitenübernahme*). A finished campaign gets a check mark on its button.
- Below the map: the mission title, its description and a status line that says why a mission is open or locked.
- **Start any mission you have reached:** every won or unlocked mission can be started (or replayed) with
  *Starten* or a double click. Replaying keeps all progress behind it; winning a branching mission with the other
  ending also opens the other branch. A mission that takes over units gets the units of your last win of the
  mission before it.
- **Progress without savegames:** every won mission is recorded automatically and unlocks the next one(s). The
  progress is kept in `save\kampagne.dat` and survives quitting the game; a damaged file is set aside as
  `kampagne.bad` and the progress starts empty. Uninstalling keeps the file, like your saves.
- **"Original Thandor" switch:** the new checkbox *Original Thandor* at the top of the *Erweitert* page (off by
  default) makes the game play like the original: the campaigns tab shows the old list, units move exactly as in
  the original, the objective box, autosave, game recordings, the high resolution font, custom maps and the louder
  movie sound are off, and the original keys apply (key bindings cannot be edited, the *Tastatur* tab of the
  in-game help shows the original page). The checkboxes of these additions are greyed out and keep their settings
  for when the switch is off again. Bug and crash fixes stay.
- **Improved pathfinding:** with *Verbesserte Wegfindung* on the *Erweitert* page (on by default) units keep
  driving through curves instead of stopping to turn, wait for or give way to their own units instead of getting
  stuck behind them, gliders keep their speed at a new route point, and a group sent to one point spreads out to
  formation places so that all of them arrive. Switched off, units move exactly as in the original. A network game
  uses it when the host has it on and every other player runs Open Thandor without *Original Thandor*; one player
  with the original game means the original movement for everybody.
- **Objective box:** a small box in the top left corner of the game shows the mission objective and its progress;
  it moves down only while an original text line would be covered. A click on it opens the mission help. It is on
  by default; *Ziel anzeigen* on the *Erweitert* page or the key `Z` switches it off and on.
- **Autosave:** with *Automatisch speichern* on the *Erweitert* page (on by default) a single player game (skirmish,
  campaign, tutorial) is saved after every minute of play to `save\autosave.sve`, always the same file. Pauses, open
  windows and the briefing do not count. The save lists show it as *Autosave -* and the mission title, and a short
  notice *Spiel automatisch gespeichert* appears at the bottom left. Network games are never saved automatically.
- **Key bindings:** the new button *Tastenbelegung* on the options page lists every action with its keys, and the
  *Tastatur* tab of the in-game help (F1) opens the same list. Click a key to give the action another key or
  combination with Ctrl, Alt and Shift (the status line names the action a key is already taken by), right click to
  remove it; *Standard* brings back the original keys. Esc, Alt+F4 and the cheats stay fixed. The bindings are
  saved in `thandor.ini` (section `[keys]`).
- **Language choice:** *Sprache:* on the *Erweitert* page switches the game's texts at once between *Automatisch*
  (the Windows language) and the languages in your game's text files (Deutsch, English, Français, Italiano,
  Español). Open Thandor's own texts are German for Deutsch and English for every other language.
- **Louder movies:** the CD movies that were mixed much quieter than the rest (the intro and the campaign story
  movies) are raised towards the level of the other movies. With *Original Thandor* on they play at their original
  level.
- **Custom maps (experimental, single player):** every map editor project `maps\<name>.thmap` next to the game
  (ZIP or the editor's older PCK form) appears in the list of the new button *Eigene Karten* below *Kampagnen* on
  *Spiel auswählen* (*Einzelspiel* shows only the original maps), with the title, description and author from the
  project. A project may bring its own ground textures. Every project is checked before it is listed; a rejected one
  gets one line in `thandor.log`. Without any custom map the list shows a hint row that says where `.thmap` files go
  (and how many were rejected), with a link to the Thandor Map Editor on GitHub. The button is not shown in network
  games or while *Original Thandor* is on. Details: [custom_maps.md](custom_maps.md).
- **GitHub link in the main menu:** a small *GitHub* link next to the version number at the bottom right opens the
  project page (github.com/idkFoxes/open-thandor) in your browser; it lights up under the mouse pointer.
- **Record games for bug reports:** with *Partien für Fehlerberichte aufzeichnen* on the *Erweitert* page (off by
  default, off while *Original Thandor* is on) every single player game is recorded to
  `recordings\<date>_<time>_<map>.otr` in the game folder (the newest 10, at most 200 MB together). After a crash,
  `crash.log` names the recording of that game: attach it to your bug report, and the game can be replayed up to
  the crash with the same version. Network games are not recorded; loading a savegame or moving on to the next
  campaign mission starts a new recording. While a game is recorded, the short-lived selection and target markers
  in the world are not shown.
- **High resolution font:** with *Hochaufgelöste Schrift* on the *Erweitert* page (off by default, off while
  *Original Thandor* is on) Vulkan and DirectX 12 draw the game's text at UI scale 2x and 3x with glyphs redrawn in
  high resolution instead of the original pixels enlarged. Letters keep their exact size and place, so lines,
  line breaks and the layout stay as before. The software renderer and UI scale 1x are not affected.

#### Fixed

- Crash with DirectX 12 during a mission, most likely in scenes with much vegetation (seen with 1.0.7): the game
  now needs far fewer graphics driver resources per frame.
- Crash when the UI scale was changed on the *Erweitert* page after a game (1.0.7).
- Rare crashes of the computer opponent while it chose a target for a group of units (1.0.7). Groups are also no
  longer sent against a unit that is already destroyed.
- Crash on a mouse click during the end movie of a mission, and several smaller problems when a game ends or the
  next one starts (for example commands of an ended network game that reached the next one, or map editor keys
  that stayed active after a game closed with the editor open).
- **Stuck mouse button:** a held mouse button is now released when the game window loses the focus or is
  minimized, and no button press or release is lost when many mouse events arrive at once (seen on a Steam Deck).
  In window mode the click that activates the window now also reaches the game.
- Vulkan and DirectX 12: if the graphics device is lost or stops working, the game starts the renderer again and,
  after repeated failures, switches to the next renderer instead of showing nothing. Fixed a rare crash in the UI
  graphics cache, and the video memory no longer grows without limit while the window is minimized.
- **More robust `thandor.ini`:** settings the game does not know (for example of a newer version) are kept when the
  file is saved; a file saved as UTF-16 or with old Mac line ends is read correctly; a value out of range falls back
  to its default with a line in `thandor.log`. The file is written to a temporary file first, so a crash or a full
  disk no longer leaves it empty or half written.
- A minimized game with the sound off no longer runs slower than real time on Windows 11.
- Route planning is faster (same routes), which shortens the small hitches with many moving units.
- **Black screen during the intro:** with the display mode *Vollbild* (exclusive fullscreen) the screen stayed
  black during the intro movie on some systems. The intro and the end movies of the missions now always play in a
  borderless fullscreen window; the game itself goes back to *Vollbild* right after the movie.

#### Kept on purpose

- After a win the campaign goes on to the next mission through the briefing room, exactly as before. The campaign
  map changes neither savegames, levels nor campaigns.
- In a network game the host can start every unlocked mission except one that takes over units: such a mission is
  only reached by playing the mission before it.
- The map is used with the mouse; with more than six campaigns (add-ons) the tab shows the old list.

### Deutsch

Eine Missionskarte für die Kampagnen, ein Schalter *Original Thandor*, verbesserte Wegfindung, frei belegbare Tasten,
ein Zielfenster, automatisches Speichern, eigene Karten aus dem Karteneditor und Korrekturen für die mit 1.0.7
gemeldeten Abstürze.

#### Neu

- **Kampagnenkarte:** Die Kampagnen-Registerkarte unter *Spiel auswählen* zeigt eine Schaltfläche je Kampagne mit
  dem Fortschritt (zum Beispiel `Die Invasion 7/24`) und darunter die Karte der gewählten Kampagne. Jede Mission
  trägt ihre Originalnummer; gewonnene Missionen sind grün, spielbare erhaben, gesperrte abgeblendet. Verzweigt
  sich die Kampagne, sind beide Wege zu sehen. Eine Doppellinie zeigt eine Mission, die die Einheiten der
  vorherigen übernimmt (*Einheitenübernahme*). Eine fertige Kampagne bekommt ein Häkchen auf ihrer Schaltfläche.
- Unter der Karte: Titel und Beschreibung der Mission und eine Statuszeile, die sagt, warum eine Mission offen oder
  gesperrt ist.
- **Jede erreichte Mission starten:** Jede gewonnene oder freigeschaltete Mission lässt sich mit *Starten* oder
  einem Doppelklick (erneut) spielen. Dabei bleibt aller Fortschritt erhalten; wer eine verzweigende Mission mit
  dem anderen Ende gewinnt, schaltet auch den anderen Zweig frei. Eine Mission mit Einheitenübernahme bekommt die
  Einheiten des letzten Siegs in der Mission davor.
- **Fortschritt ohne Spielstände:** Jede gewonnene Mission wird von selbst vermerkt und schaltet die nächste(n)
  frei. Der Fortschritt steht in `save\kampagne.dat` und bleibt nach dem Beenden erhalten; eine beschädigte Datei
  wird als `kampagne.bad` beiseitegelegt, und der Fortschritt beginnt leer. Die Deinstallation lässt die Datei wie
  die Spielstände liegen.
- **Schalter "Original Thandor":** Das neue Kästchen *Original Thandor* oben auf der Seite *Erweitert* (anfangs aus)
  lässt das Spiel wie das Original laufen: Die Kampagnen-Registerkarte zeigt die alte Liste, Einheiten fahren genau
  wie im Original, Zielfenster, automatisches Speichern, Aufzeichnungen, hochaufgelöste Schrift, eigene Karten und
  der lautere Filmton sind aus, und es gelten die Original-Tasten (die Tastenbelegung lässt sich nicht ändern, die
  Registerkarte *Tastatur* der Spielhilfe zeigt die Originalseite). Die Kästchen dieser Neuerungen sind
  abgeblendet und behalten ihre Einstellung für später. Fehler- und Absturzbehebungen bleiben.
- **Verbesserte Wegfindung:** Mit *Verbesserte Wegfindung* auf der Seite *Erweitert* (anfangs an) fahren Einheiten
  Kurven, statt zum Drehen anzuhalten, warten auf eigene Einheiten oder weichen ihnen aus, statt hinter ihnen
  festzuhängen, Gleiter behalten an einem neuen Wegpunkt ihr Tempo, und eine Gruppe, die zu einem Punkt geschickt
  wird, verteilt sich auf Formationsplätze, so dass alle ankommen. Ausgeschaltet fahren Einheiten genau wie im
  Original. Ein Netzwerkspiel nutzt sie, wenn der Host sie eingeschaltet hat und alle anderen Spieler Open Thandor
  ohne *Original Thandor* spielen; ein Spieler mit dem Originalspiel bedeutet für alle die Original-Bewegung.
- **Zielfenster:** Ein kleines Fenster oben links im Spiel zeigt das Missionsziel und den Fortschritt; es rückt nur
  nach unten, solange es eine Textzeile des Originals verdecken würde. Ein Klick darauf öffnet die Missionshilfe.
  Es ist anfangs an; *Ziel anzeigen* auf der Seite *Erweitert* oder die Taste `Z` schaltet es aus und wieder ein.
- **Automatisches Speichern:** Mit *Automatisch speichern* auf der Seite *Erweitert* (anfangs an) wird ein
  Einzelspieler-Spiel (freies Spiel, Kampagne, Tutorial) nach jeder Minute Spielzeit in `save\autosave.sve`
  gespeichert, immer dieselbe Datei. Pausen, offene Fenster und die Einsatzbesprechung zählen nicht mit. Die
  Spielstandlisten zeigen den Spielstand als *Autosave -* mit dem Missionstitel, und unten links erscheint kurz der
  Hinweis *Spiel automatisch gespeichert*. Netzwerkspiele werden nie automatisch gespeichert.
- **Tastenbelegung:** Die neue Schaltfläche *Tastenbelegung* auf der Optionsseite listet jede Aktion mit ihren
  Tasten, und die Registerkarte *Tastatur* der Spielhilfe (F1) öffnet dieselbe Liste. Ein Klick auf eine Taste gibt
  der Aktion eine andere Taste oder Kombination mit Strg, Alt und Umschalt (ist sie schon belegt, nennt die
  Statuszeile die Aktion), ein Rechtsklick entfernt sie; *Standard* stellt die Original-Tasten wieder her. Esc,
  Alt+F4 und die Cheats bleiben fest. Die Belegung steht in `thandor.ini` (Abschnitt `[keys]`).
- **Sprachauswahl:** *Sprache:* auf der Seite *Erweitert* schaltet die Texte des Spiels sofort um, zwischen
  *Automatisch* (die Sprache von Windows) und den Sprachen in den Textdateien Ihres Spiels (Deutsch, English,
  Français, Italiano, Español). Die eigenen Texte von Open Thandor sind bei Deutsch deutsch, sonst englisch.
- **Lautere Filme:** Die CD-Filme, die viel leiser abgemischt sind als der Rest (das Intro und die Filme der
  Kampagnengeschichte), werden an die Lautstärke der übrigen Filme angehoben. Mit *Original Thandor* laufen sie in
  ihrer Originallautstärke.
- **Eigene Karten (experimentell, nur Einzelspieler):** Jedes Karteneditor-Projekt `maps\<Name>.thmap` neben dem
  Spiel (ZIP oder die ältere PCK-Form des Editors) erscheint in der Liste der neuen Schaltfläche *Eigene Karten*
  unter *Kampagnen* auf *Spiel auswählen* (*Einzelspiel* zeigt nur die Original-Karten), mit Titel, Beschreibung und
  Autor aus dem Projekt. Ein Projekt darf eigene Bodentexturen mitbringen. Jedes Projekt wird vor dem Anzeigen
  geprüft; ein abgelehntes erhält eine Zeile in `thandor.log`. Ohne eigene Karte zeigt die Liste eine Hinweiszeile,
  die sagt, wohin `.thmap`-Dateien gehören (und wie viele abgelehnt wurden), mit einem Link zum Thandor Map Editor
  auf GitHub. In Netzwerkspielen und solange *Original Thandor* an ist, fehlt die Schaltfläche. Einzelheiten:
  [custom_maps.md](custom_maps.md).
- **GitHub-Link im Hauptmenü:** Ein kleiner Link *GitHub* neben der Versionsnummer unten rechts öffnet die
  Projektseite (github.com/idkFoxes/open-thandor) im Browser; unter dem Mauszeiger leuchtet er auf.
- **Partien für Fehlerberichte aufzeichnen:** Mit *Partien für Fehlerberichte aufzeichnen* auf der Seite
  *Erweitert* (anfangs aus, aus, solange *Original Thandor* an ist) wird jede Einzelspieler-Partie als
  `recordings\<Datum>_<Uhrzeit>_<Karte>.otr` im Spielordner aufgezeichnet (die neuesten 10, zusammen höchstens
  200 MB). Nach einem Absturz nennt `crash.log` die Aufzeichnung dieser Partie: Legen Sie sie Ihrer Fehlermeldung
  bei, dann lässt sich die Partie mit derselben Version bis zum Absturz nachspielen. Netzwerkspiele werden nicht
  aufgezeichnet; das Laden eines Spielstands oder die nächste Kampagnenmission beginnt eine neue Aufzeichnung.
  Während einer Aufzeichnung fehlen die kurzlebigen Auswahl- und Zielmarkierungen in der Welt.
- **Hochaufgelöste Schrift:** Mit *Hochaufgelöste Schrift* auf der Seite *Erweitert* (anfangs aus, aus, solange
  *Original Thandor* an ist) zeichnen Vulkan und DirectX 12 die Schrift des Spiels bei UI-Skalierung 2x und 3x mit
  hochaufgelöst nachgezeichneten Buchstaben statt mit vergrößerten Originalpixeln. Die Buchstaben behalten Größe und
  Position genau, Zeilen, Zeilenumbrüche und das Layout bleiben wie bisher. Software-Renderer und UI-Skalierung 1x
  sind nicht betroffen.

#### Behoben

- Absturz mit DirectX 12 während einer Mission, am ehesten in Szenen mit viel Bewuchs (gesehen mit 1.0.7): Das
  Spiel braucht jetzt je Bild weit weniger Ressourcen des Grafiktreibers.
- Absturz beim Ändern der UI-Skalierung auf der Seite *Erweitert* nach einem Spiel (1.0.7).
- Seltene Abstürze des Computergegners, während er ein Ziel für eine Einheitengruppe wählte (1.0.7). Außerdem
  werden Gruppen nicht mehr gegen eine schon zerstörte Einheit geschickt.
- Absturz bei einem Mausklick während des Abschlussfilms einer Mission und mehrere kleinere Probleme beim Ende eines
  Spiels oder beim Start des nächsten (zum Beispiel Befehle eines beendeten Netzwerkspiels, die im nächsten ankamen,
  oder Tasten des Karteneditors, die nach einem mit offenem Editor beendeten Spiel aktiv blieben).
- **Hängende Maustaste:** Eine gedrückte Maustaste wird jetzt losgelassen, wenn das Spielfenster den Fokus verliert
  oder minimiert wird, und bei vielen gleichzeitigen Mausereignissen geht kein Drücken oder Loslassen mehr verloren
  (gesehen auf einem Steam Deck). Im Fenstermodus erreicht jetzt auch der Klick, der das Fenster aktiviert, das
  Spiel.
- Vulkan und DirectX 12: Fällt das Grafikgerät aus oder arbeitet es nicht mehr, startet das Spiel die Darstellung
  neu und wechselt nach wiederholten Fehlern zum nächsten Verfahren, statt nichts mehr zu zeigen. Ein seltener
  Absturz im Grafikspeicher der Oberfläche ist behoben, und der Grafikspeicher wächst bei minimiertem Fenster nicht
  mehr unbegrenzt.
- **Robustere `thandor.ini`:** Einstellungen, die das Spiel nicht kennt (etwa von einer neueren Version), bleiben
  beim Speichern erhalten; eine als UTF-16 oder mit alten Mac-Zeilenenden gespeicherte Datei wird richtig gelesen;
  ein Wert außerhalb seines Bereichs gilt als Standardwert, mit einer Zeile in `thandor.log`. Die Datei wird erst in
  eine temporäre Datei geschrieben, so dass ein Absturz oder eine volle Festplatte sie nicht mehr leer oder halb
  geschrieben zurücklässt.
- Ein minimiertes Spiel mit abgeschaltetem Ton läuft unter Windows 11 nicht mehr langsamer als in Echtzeit.
- Die Wegplanung ist schneller (mit denselben Wegen), das verkürzt die kleinen Ruckler bei vielen fahrenden
  Einheiten.
- **Schwarzer Bildschirm beim Intro:** Mit dem Anzeigemodus *Vollbild* (exklusives Vollbild) blieb der Bildschirm
  auf manchen Systemen während des Introfilms schwarz. Das Intro und die Abschlussfilme der Missionen laufen jetzt
  immer in einem randlosen Vollbildfenster; das Spiel selbst kehrt gleich nach dem Film zu *Vollbild* zurück.

#### Bewusst beibehalten

- Nach einem Sieg geht die Kampagne wie bisher über den Besprechungsraum zur nächsten Mission weiter. Die
  Kampagnenkarte ändert weder Spielstände noch Level oder Kampagnen.
- Im Netzwerkspiel kann der Host jede freigeschaltete Mission starten, außer einer mit Einheitenübernahme: Sie wird
  nur über die Mission davor erreicht.
- Die Karte wird mit der Maus bedient; bei mehr als sechs Kampagnen (Add-ons) zeigt die Registerkarte die alte
  Liste.

## 1.0.7 - "Thandor Patch 7"

### English

A hotfix for graphics cards with much video memory, and a wider zoom range for large screens.

#### Fixed

- Crash at start with Vulkan and a black 3D view with DirectX 12 on some graphics cards (seen on NVIDIA RTX cards
  with 16 GB): the game now leaves the graphics driver more address space.

#### New

- **Wider zoom range:** you can zoom further out (about 1.7 times the old view width) and further in, which suits
  1440p, 4K and wide screens.
- If the game crashes while starting the graphics, the next start automatically falls back to the next renderer
  (Vulkan -> DirectX 12 -> Software) and saves that choice.
- `thandor.log` names the graphics card and its driver version, which helps with bug reports.
- The installer is now called "Thandor Patch 7" (`Thandor-Patch-7.exe`). It needs an installed full version of
  Thandor; Patch 5 is not required.

### Deutsch

Ein Hotfix für Grafikkarten mit viel Grafikspeicher und ein größerer Zoombereich für große Bildschirme.

#### Behoben

- Absturz beim Start mit Vulkan und schwarze 3D-Ansicht mit DirectX 12 auf manchen Grafikkarten (gesehen bei NVIDIA
  RTX mit 16 GB): Das Spiel lässt dem Grafiktreiber jetzt mehr Adressraum.

#### Neu

- **Größerer Zoombereich:** Man kann weiter herauszoomen (etwa 1,7-mal so breite Sicht wie bisher) und näher
  heranzoomen, passend für 1440p, 4K und breite Bildschirme.
- Stürzt das Spiel beim Start der Grafik ab, nimmt es beim nächsten Start automatisch das nächste Verfahren
  (Vulkan -> DirectX 12 -> Software) und speichert diese Wahl.
- `thandor.log` nennt die Grafikkarte und ihre Treiberversion, das hilft bei Fehlermeldungen.
- Das Setup heißt jetzt "Thandor Patch 7" (`Thandor-Patch-7.exe`). Es braucht eine installierte Vollversion von
  Thandor; Patch 5 wird nicht vorausgesetzt.

## 1.0.6 - "Thandor Patch 6"

### English

The first release of Open Thandor: the whole game rebuilt in C++ as a 64-bit Windows program. It installs as a
patch onto an existing Thandor installation and plays the original campaigns, missions, saves and maps.

#### New

- **Modern graphics:** Vulkan (default) or DirectX 12 on the graphics card, or the software renderer of the
  original. Chosen in *Optionen -> Grafik -> Anzeige*, switched without a restart.
- **Any resolution** your display offers, from 640x480 up to 4K (the original listed only the ten smallest), in a
  window, a borderless window or exclusive fullscreen.
- **UI scaling** for large displays (*Erweitert*: Auto, 1x, 2x, 3x): menus and the side panel stay readable at
  1440p and 4K while the 3D view uses the full resolution.
- **VSync and frame limit** (*Erweitert*): no tearing by default; optional limits of 60, 120 or 144 frames per
  second.
- **Settings in a readable `thandor.ini`** next to the game; the old `thandor.dat` is taken over once and never
  changed.
- **Installer and uninstaller:** keeps the old `thandor.exe` as `thandor-1.05.exe` and puts it back on uninstall;
  game data, saves and settings are never touched.
- The version number (1.0.6) is shown in the main menu and in the multiplayer session title.

#### Fixed

- Runs as a native 64-bit program on Windows 10 and 11; no DirectDraw, Glide or old DirectX runtime needed.
- Sound, music and movies play through SDL3 (no DirectSound problems on modern Windows).
- The original cheats work again, also on keyboards with AltGr.
- Many crashes and hangs on damaged or unusual files are fixed: broken levels, maps, models, sounds, pictures,
  campaigns and savegames are rejected with a message instead of crashing the game, and own maps of any size no
  longer overflow the pathfinding.
- Saving is safe: the game writes a temporary file and replaces the old save only when everything is written; a
  locked save file shows an error instead of ending the game.
- Rare crashes in the original code (division by zero, reads past tables, timer races) are fixed.

#### Kept on purpose

- The game rules, the AI and the original quirks of the game stay as they were: savegames, levels and the network
  protocol are compatible with the original game.

### Deutsch

Die erste Version von Open Thandor: das ganze Spiel in C++ neu gebaut, als 64-Bit-Programm für Windows. Es wird als
Patch in eine vorhandene Thandor-Installation installiert und spielt die Original-Kampagnen, Missionen, Spielstände
und Karten.

#### Neu

- **Moderne Grafik:** Vulkan (Standard) oder DirectX 12 auf der Grafikkarte, oder der Software-Renderer des
  Originals. Auswahl unter *Optionen -> Grafik -> Anzeige*, Wechsel ohne Neustart.
- **Jede Auflösung** des Bildschirms von 640x480 bis 4K (das Original zeigte nur die zehn kleinsten), im Fenster,
  als Vollbildfenster oder im exklusiven Vollbild.
- **UI-Skalierung** für große Bildschirme (*Erweitert*: Auto, 1x, 2x, 3x): Menüs und Seitenleiste bleiben bei 1440p
  und 4K lesbar, die 3D-Ansicht nutzt die volle Auflösung.
- **VSync und Bildratenbegrenzung** (*Erweitert*): standardmäßig kein Tearing; wahlweise 60, 120 oder 144 Bilder
  pro Sekunde.
- **Einstellungen in einer lesbaren `thandor.ini`** neben dem Spiel; die alte `thandor.dat` wird einmal übernommen
  und nie verändert.
- **Installer und Deinstallation:** Die alte `thandor.exe` wird als `thandor-1.05.exe` gesichert und bei der
  Deinstallation zurückgelegt; Spieldaten, Spielstände und Einstellungen bleiben unberührt.
- Die Versionsnummer (1.0.6) steht im Hauptmenü und im Titel einer Mehrspieler-Sitzung.

#### Behoben

- Läuft als echtes 64-Bit-Programm unter Windows 10 und 11; kein DirectDraw, Glide oder altes DirectX nötig.
- Sound, Musik und Filme laufen über SDL3 (keine DirectSound-Probleme unter aktuellem Windows).
- Die Original-Cheats funktionieren wieder, auch auf Tastaturen mit AltGr.
- Viele Abstürze und Hänger bei beschädigten oder ungewöhnlichen Dateien sind behoben: kaputte Level, Karten,
  Modelle, Sounds, Bilder, Kampagnen und Spielstände werden mit einer Meldung abgelehnt, statt das Spiel abstürzen
  zu lassen, und eigene Karten beliebiger Größe bringen die Wegfindung nicht mehr zum Überlaufen.
- Sicheres Speichern: Das Spiel schreibt erst eine temporäre Datei und ersetzt den alten Spielstand erst, wenn alles
  geschrieben ist; ein gesperrter Spielstand zeigt eine Fehlermeldung, statt das Spiel zu beenden.
- Seltene Abstürze im Original-Code (Division durch null, Lesen hinter Tabellen, Timer-Races) sind behoben.

#### Bewusst beibehalten

- Spielregeln, KI und die Eigenheiten des Originals bleiben, wie sie waren: Spielstände, Level und das
  Netzwerkprotokoll sind mit dem Originalspiel kompatibel.
