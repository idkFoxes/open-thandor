# Patch notes

Player-facing changes of Open Thandor compared with the original Thandor 1.05 (Patch 5). The developer view of the
code changes is in the [plans](plans) and the [recovery changelog](../CHANGELOG.md).

*Deutsch weiter unten.*

## 1.0.6 - "Thandor Patch 6"

The first release of Open Thandor: the whole game rebuilt in C++ as a 64-bit Windows program. It installs as a
patch onto an existing Thandor installation and plays the original campaigns, missions, saves and maps.

### New

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

### Fixed

- Runs as a native 64-bit program on Windows 10 and 11; no DirectDraw, Glide or old DirectX runtime needed.
- Sound, music and movies play through SDL3 (no DirectSound problems on modern Windows).
- The original cheats work again, also on keyboards with AltGr.
- Many crashes and hangs on damaged or unusual files are fixed: broken levels, maps, models, sounds, pictures,
  campaigns and savegames are rejected with a message instead of crashing the game, and own maps of any size no
  longer overflow the pathfinding.
- Saving is safe: the game writes a temporary file and replaces the old save only when everything is written; a
  locked save file shows an error instead of ending the game.
- Rare crashes in the original code (division by zero, reads past tables, timer races) are fixed.

### Kept on purpose

- The game rules, the AI and the original quirks of the game stay as they were: savegames, levels and the network
  protocol are compatible with the original game.

---

## Deutsch

### 1.0.6 - "Thandor Patch 6"

Die erste Version von Open Thandor: das ganze Spiel in C++ neu gebaut, als 64-Bit-Programm für Windows. Es wird als
Patch in eine vorhandene Thandor-Installation installiert und spielt die Original-Kampagnen, Missionen, Spielstände
und Karten.

**Neu**

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

**Behoben**

- Läuft als echtes 64-Bit-Programm unter Windows 10 und 11; kein DirectDraw, Glide oder altes DirectX nötig.
- Sound, Musik und Filme laufen über SDL3 (keine DirectSound-Probleme unter aktuellem Windows).
- Die Original-Cheats funktionieren wieder, auch auf Tastaturen mit AltGr.
- Viele Abstürze und Hänger bei beschädigten oder ungewöhnlichen Dateien sind behoben: kaputte Level, Karten,
  Modelle, Sounds, Bilder, Kampagnen und Spielstände werden mit einer Meldung abgelehnt, statt das Spiel abstürzen
  zu lassen, und eigene Karten beliebiger Größe bringen die Wegfindung nicht mehr zum Überlaufen.
- Sicheres Speichern: Das Spiel schreibt erst eine temporäre Datei und ersetzt den alten Spielstand erst, wenn alles
  geschrieben ist; ein gesperrter Spielstand zeigt eine Fehlermeldung, statt das Spiel zu beenden.
- Seltene Abstürze im Original-Code (Division durch null, Lesen hinter Tabellen, Timer-Races) sind behoben.

**Bewusst beibehalten**

- Spielregeln, KI und die Eigenheiten des Originals bleiben, wie sie waren: Spielstände, Level und das
  Netzwerkprotokoll sind mit dem Originalspiel kompatibel.
