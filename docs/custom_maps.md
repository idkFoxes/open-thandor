# Custom maps (`maps\<name>.thmap`, single player)

Experimental. A custom map is a map editor project file `maps\<name>.thmap` next to `thandor.exe`, as the
[Thandor Map Editor](https://github.com/idkFoxes/thandor-map-editor) saves it. The game lists it in the *Eigene Karten*
tab of *Spiel auswählen* (English: *Custom maps*) in a local game only. Network games (host and client) neither list
nor send custom maps.
Code:
- `src/assets/scenario/custom_maps.cpp` (`custom_maps.h`): scan, list, texts, activation
- `src/assets/scenario/custom_project.cpp` (`custom_project.h`): reading a project
- `third_party/map_editor_project_format`: the editor's project-format reader, with miniz for the ZIP form

The folder format of earlier experimental builds (`maps\<name>\<name>.lev`, `<name>.fld`, `map.ini`) is gone. Such
folders are ignored, with one log line each (`custom map folder "<name>" ignored: ...`).

## Project file

```
thandor.exe
maps\
  my_asgard.thmap
```

The map name is the file stem (`my_asgard`). The editor writes the file in one of two forms, and the game reads both:

| Form | Recognised by | Manifest |
|---|---|---|
| ZIP | the file starts with a ZIP signature | `map.ini` at the root: first line `[map]`, `key = value` lines, `format = 1`, UTF-8 without NUL |
| legacy PCK | anything else; must be a stored PCK | `editor/project.txt`: first line `THANDOR_MAP_EDITOR_PROJECT_V1`, `_V2` or `_V3`, `key=value` lines, values `%`-encoded |

Manifest keys the game reads:

| Key | Use |
|---|---|
| `level_entry` | required: `level/<x>.lev`, the level ([level_format.md](level_format.md)) |
| `field_entry` | required: `level/<x>.fld`, the expanded field grid ([field_grid_format.md](field_grid_format.md)) |
| `string_entry` | optional: `*.str`, the level text (title, description, task lines) |
| `display_name` | optional: the title (UTF-8) |
| `author` | optional: shown below the description as `Autor: ...` (German) or `Author: ...` |
| `terrain_crc.<n>` | ZIP only: the CRC32 of the original `textures/terrain/<letter>.tga`. A TGA that differs from it (or has no CRC) replaces that ground texture. |

Every other key (`map_name`, the editor's `default_human_faction`, ...) is ignored. Every other entry is ignored too
(the editor's `editor/*` files, previews), except the terrain files below.

### Rules

A project that breaks any of these rules is not listed. `thandor.log` gets one line per project with the reason:
`custom map "<name>" skipped: ...`.

* **Name:** 1..30 characters `A-Z a-z 0-9 _ -`. It must not be the name of an original level: no record of
  `level\level.dat`, and no `level\<name>.lev` in a package or loose. A custom map never hides or replaces an
  original.
* **Size:** at most 32 MiB per file, 16 MiB per entry, 64 MiB unpacked in total and 4096 entries. Entry paths with
  `..`, a drive letter or an absolute path are rejected. ZIP entries must be stored or deflated and not encrypted.
  PCK entries may use any method of the stock packages.
* **Field grid path:** the level's field grid path (+0xB0) must name the project's field entry. For example, the
  path `level\my_asgard` (no extension, case ignored) goes with `field_entry = level/my_asgard.fld`. The level
  cannot load a field grid from anywhere else. `<x>` does not have to be the map name.
* **Checks:** the level header (magic, converter version), the game's field grid check and `CustomMap_Validate`
  (`custom_map_validate.h`: sizes, offsets, counts and indices of the level and the grid). The game runs every
  check before the map is listed, and runs them again before each load.
* **Count:** at most 200 custom maps are listed (`CUSTOM_MAP_LIMIT`). Further files are logged and skipped.
* The result of the checks is kept per file while its size and write time stay the same.

### Terrain files

While its map runs, a project can also supply ground textures:
* `gfx/boden/*.gfx` and `gfx/boden/*.pal`: used only when the magic matches and the allocation size equals the byte
  count.
* `gfx/boden/*.dat`
* the ZIP form's `textures/terrain/a..z.tga` (256x256): each is converted to `<+0xB4 ground path><letter>.gfx`.

A level that points its ground texture path (+0xB4) at its own `gfx\boden\...` name therefore needs no stock file.
No other entry of a project is ever served.

## In the game

* **Tab button:** *Spiel auswählen* has a fourth button on the left, *Eigene Karten* (English: *Custom maps*), below
  *Kampagnen*, with the size and spacing of the other three (`customMapsTabButton`, action
  `FRONTEND_ACTION_OPEN_CUSTOM_MAPS`, `ui/frontend/scenario_selection.cpp`). It shows the same list as *Einzelspiel*
  (same columns, selection, description box with `Autor: ...`, *Starten* and double click), but only with the custom
  maps; the label above the list reads *Eigene Karten:* / *Custom maps:*. The *Einzelspiel* list holds only the
  original maps. The button is shown only in a local game with custom maps on: it is hidden in network games (host
  and client) and in "Original Thandor" mode (`OT_FEATURE_CUSTOM_MAPS` off).
* **List:** custom maps are sorted in the *Eigene Karten* list by title like the original maps in *Einzelspiel*. Their
  title carries the suffix ` (eigene Karte)` (German texts) or ` (custom map)` (every other language). The suffix
  shows only in the list, not in the briefing, the in-game title or the save name. The other columns (planet, size,
  players) come from the level's catalog record (+0x100..+0x1FF), like an original map's from `level.dat`.
* **Texts:** the title is `display_name`. Without one, the title is STR string 0 with leading blanks removed, and
  without that, the map name. The description block line *k* is STR string *k + 1*: line 0 is the description, and
  the further lines are the faction tasks and briefing lines. The current text language picks the STR block with
  the matching country code (49 de, 44 en, 33 fr, 39 it, 34 es), else block 44, else the first block. A change of
  language takes effect at once. Characters outside Latin-1 become `?`. Rich-text commands that carry a payload are
  dropped. A title is cut to 64 characters and a line to 1024.
* **Text ids:** a custom map gets its own title index `0x2D0 + slot` (`CUSTOM_MAP_TITLE_INDEX_BASE`). The slot is a
  hash of the lowercased name, 0..0x7FF, probing on to the next free slot. The title is text id `0x2500 + slot`, the
  description block `0x230010 + 0x10 * index`. No game text page uses these ids. The loaded copy of the level
  carries this index instead of the file's. Its identifier becomes `maps\<name>.`.
* **Start:** from the list, or with `-KARTE="<name>"` (as for an original map; a name that is no original map is
  looked up in the *Eigene Karten* list, which the page then shows). The level path is the virtual `maps\<name>.lev`.
* **Hint row:** while no custom map is listed (no `maps` folder, no `.thmap` file in it, or every file rejected),
  the *Eigene Karten* list holds one dark grey row *Eigene Karten: keine gefunden* / *Custom maps: none found*
  (appended behind the sorted rows, so it would always stay the last row). Selected, it shows in the description box:
  *Keine eigenen Karten gefunden. Lege .thmap-Dateien des Thandor Map Editors in den Ordner maps neben thandor.exe.*
  (English: *No custom maps found. Put Thandor Map Editor .thmap files into the maps folder next to thandor.exe.*).
  When the scan rejected files, their number follows: *Keine eigenen Karten gefunden (3 abgelehnt, siehe
  thandor.log). Lege .thmap-Dateien in den Ordner maps neben thandor.exe.* / *No custom maps found (3 files
  rejected, see thandor.log). Put .thmap files into the maps folder next to thandor.exe.* Below the text the link
  *Map Editor auf GitHub* / *Map Editor on GitHub* opens <https://github.com/idkFoxes/thandor-map-editor> in the
  browser (`SdlPlatform_OpenUrl`; with `OPEN_THANDOR_OPEN_URL=log` it is only logged). The row is no map: *Starten*
  is greyed out while it is selected, a double click or Enter on it starts nothing, and `-KARTE` never selects it
  (its identifier is the unit 0x01, which no command line name contains). German texts for German, English for
  every other language, like the list suffix. Not shown in network games and in "Original Thandor" mode. Texts:
  `TEXT_ID_CUSTOM_MAPS_HINT_TITLE` / `_DESCRIPTION` / `_LINK` (0x7F0300..0x7F0302, block up to 0x7F030F), resolved
  by `CustomMaps_TryResolveText`; the link: `FrontendCustomMapsHintLink_Draw` / `_HandleLeftPress`
  (`ui/frontend/scenario_selection.cpp`).
* **Serving:** the file's level, its field grid and its terrain files come from memory while the project is
  *active*. It becomes active when its map is loaded (list, `-KARTE`, *Neustart*, loading a save) and stays active
  through the session. Only one project is active at a time. The active project is released:
  * when the session ends
  * when the scenario list is built again
  * when any other level is loaded

  With no active project nothing is served, so a project never shadows an original file: the stock `gfx\boden\...`
  and `level\...` files load as before. After the session the project's data is freed. The next start reads the
  file again and checks it again.
* **Objective overlay:** custom maps get the generated objective line, never the hand-written one of an original
  level.
* **Memory:** the scan and the active project use native memory only (no arena). Without a `maps` folder the scan
  allocates nothing; the list then only gets the hint row (a native row array, no arena), and levels, saves and
  the game state are unchanged.
* **Original Thandor mode** (`OT_FEATURE_CUSTOM_MAPS` off): `maps\` is neither scanned nor listed, and the hint
  row is not shown.

### Saved games

A save stores the loaded copy of the level (with `maps\<name>.`) and the field grid. It lists and loads like any
other save. Its title comes from the map while the map is listed, otherwise it reads *Eigene Karte* /
*Custom map*. Loading a save activates the project again if `maps\<name>.thmap` is there and passes its checks.

Tested (2026-11-15, two separate processes each: save, then load in a new process):

| Map | `.thmap` present | `.thmap` deleted |
|---|---|---|
| ZIP map on stock terrain (`Eis_Zip`, from Asgard) | loads and runs | loads and runs (the log says the map cannot be loaded; the save does not need it) |
| ZIP map with its own terrain (`Eis_Terrain`, `--custom-terrain`) | loads and runs, 30 files served | no crash: the ground textures `gfx\boden\edit_eis_terrain*.gfx` are missing, so the game stops with its normal fatal error for a missing package file (`fatal error 0x00000001, last path ...`), as the original does for a missing file |

*Neustart* after a loaded save loads `maps\<name>.lev` again. If that map is gone or now fails its checks, the
game goes back to *Spiel auswählen* and logs the reason (the original stopped with a fatal error there).

### Not supported (yet)

* `default_human_faction` of the editor manifest: the game keeps its own faction choice.
* Project files other than the level, the field grid, the STR and the ground textures (models, sounds, movies) are
  not served.

## Creating test maps

Tool: `tools/data/custom_map.py` (uses `lev.py`, `fld.py`, `pck.py` as modules). It only reads the game
directory it builds from.

```
python tools/data/custom_map.py list    GAME_DIR
python tools/data/custom_map.py build   GAME_DIR ORIGINAL NAME OUTDIR [--legacy] [--title T] [--description D]
                                        [--description-en D] [--author A] [--start-xenite N] [--start-tritium N]
                                        [--custom-terrain]
python tools/data/custom_map.py check   FILE.thmap ...
python tools/data/custom_map.py corrupt SRC.thmap OUT_DIR KIND
```

* `list` prints every original level in the PCK packages of `GAME_DIR` (and loose under `GAME_DIR\level`).
* `build` copies one original (`asgard`, `s01_insel`, `level\nimm2\paradis`, ...) into `OUTDIR\maps\NAME.thmap`:
  * ZIP by default (`map.ini`), or with `--legacy` a stored PCK V3 (`editor/project.txt`), the two forms the editor
    writes.
  * Copy the file into `maps` next to `thandor.exe` to try it.
  * `NAME`: 1..30 characters `A-Z a-z 0-9 _ -`, not the base name of an original level.

  Example:

  ```
  python tools/data/custom_map.py build ..\ot-soak asgard my_asgard out --title "Mein Asgard" --description "Eine eigene Karte." --author Ich --start-xenite 9999
  ```

* `check` verifies project files as far as the file formats go. The exit code is 1 when any file has a problem.
  The game checks more (`CustomMap_Validate`, the field grid check).
* `corrupt` writes `OUT_DIR\bad_<kind>.thmap`, a copy of a project broken in exactly one way (`KIND` = `all` for
  every kind).

### What `build` writes

| Entry | Content |
|---|---|
| `map.ini` / `editor/project.txt` | `level_entry`, `field_entry`, `string_entry`, `display_name` (`--title`), `author`; the legacy form also `map_name` |
| `level/name.lev` | the original level, byte for byte, except: the identifier at +0x100 becomes `NAME.` (the game replaces it by `maps\NAME.` anyway); the field grid path (+0xB0) becomes `level\NAME`, in a new 0x100-byte slot appended to the prefix, so the placement table moves up and +0x04 (size) and +0xDC (prefix size) follow. `--start-xenite` / `--start-tritium` (whole units) replace the start resources of every active faction (+0x14 / +0x18 of the player slots). With `--custom-terrain`, the ground texture path (+0xB4) becomes `gfx\boden\edit_NAME` in a second appended slot. |
| `level/name.fld` | the original field grid, expanded |
| `level/name.str` | STR with block 49 (string 0 = ` <title>`, 1 = `--description`, default `Abgeleitet von <original>`), plus block 44 with `--description-en` |
| `gfx/boden/edit_name*` | only with `--custom-terrain`: copies of the original's ground texture files (`.dat`, `.pal`, `a..z.gfx`) |

### Corruption kinds

| KIND | Defect | The game logs |
|---|---|---|
| `truncated` | the file cut in half | `invalid map project ZIP` / `PCK declared size mismatch` |
| `traversal` | an extra entry `../../evil.gfx` | `invalid map project path component` |
| `bad_lev` | LEV magic `xxx\0` | `invalid project LEV` |
| `bad_fld` | FLD width and height 55 | `fld: grid 55x55, sides must be 4k+3 in 59..139` |
| `field_mismatch` | field entry renamed to `level/other.fld` | `LEV field grid path does not match the project's field entry` |
| `no_manifest` | manifest removed | `missing map project entry: map.ini` |
| `oversized` | padded to 32 MiB + 1 byte | `file too large (33554433 bytes, at most 33554432)` |

## Self-test

`OPEN_THANDOR_SELFTEST=custommaps` checks:
* `CustomMap_Validate` against every stock level, broken levels and a fuzz run
* projects built in memory:
  * ZIP and legacy PCK V3
  * a damaged ZIP, path traversal in both forms, an oversized file
  * broken LEV and FLD entries, a field path mismatch, a missing manifest, unsupported manifest versions
* invalid names, collisions with original levels, an old map folder
* the scan, the texts, serving and its release
* the hint row: the rejected count in its text, and without a `maps` folder the row behind the stock rows (not
  counted, no level path, identifier 0x01) with its texts
* fuzz runs over mutated ZIP and PCK projects
