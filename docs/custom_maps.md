# Custom maps (`maps\<name>\`, single player)

Experimental. A custom map is a folder under `maps\` next to `thandor.exe`. The game lists it in the *Einzelspiel*
tab of *Spiel auswählen* in a local game only; network games (host and client) neither list nor send custom maps.
The map editor does not know them (yet). Code: `src/assets/scenario/custom_maps.cpp` (`custom_maps.h`).

## Folder layout

```
thandor.exe
maps\
  my_asgard\
    my_asgard.lev      level (docs/level_format.md)
    my_asgard.fld      field grid, expanded (loose) form (docs/field_grid_format.md)
    map.ini            optional: title and description (UTF-8)
```

* The folder name is the map name: 1..30 characters `A-Z a-z 0-9 _ -`. It must not be the name of an original
  level (a record of `level\level.dat` or a `level\<name>.lev` in a package or loose), so a custom map never hides
  or replaces an original.
* In the `.lev`, the field grid path (+0xB0) must be exactly `maps\<name>\<name>` (case ignored, no extension) -
  the map cannot load files from other folders. The identifier (+0x100) and the title index (+0x170) of the file
  are not used (see below). `tools/data/custom_map.py derive` writes such a level (section "Creating test maps").
* At most 200 custom maps are listed (`CUSTOM_MAP_LIMIT`); further folders are logged and skipped.

## map.ini

UTF-8 (with or without BOM), section `[map]`, `key = value` lines; `;` and `#` start comment lines; a value may be
put in double quotes. Unknown keys (such as `source`) are ignored.

```ini
[map]
title = Mein Asgard
description = Eine eigene Karte.\nZweite Zeile.
title.en = My Asgard
description.en = A custom map.
```

* Language keys: `title.<lang>` and `description.<lang>` with `<lang>` = `de`, `en`, `fr`, `it`, `es` (the languages
  of the language choice, `Game_ApplyLanguage`). The current text language picks `title.<lang>`, else `title`, else
  the folder name; the description likewise (else none). Changing the language takes effect at once.
* `\n` in a description is a line break. Characters outside Latin-1 become `?` (the game fonts have Latin-1 only).
  A title is cut to 64 characters, a description to 1024.
* Without `map.ini` (or without a usable `[map]` section) the folder name is the title; the log says so.

## In the game

* **List:** the custom maps are sorted into the *Einzelspiel* list by title like the original maps. Their title
  carries the suffix ` (eigene Karte)` (German texts) or ` (custom map)` (every other language) - only in the list,
  not in the briefing, the in-game title or the save name. The other columns come from the catalog record of the
  `.lev` (+0x100..+0x1FF), like an original map's from `level.dat`.
* **Start:** from the list or with `-KARTE="<name>"` (as for an original map). The game loads
  `maps\<name>\<name>.lev` and, through its field grid path, `maps\<name>\<name>.fld`.
* **Checks:** every time the list is built (each opening of *Spiel auswählen*), every folder is checked: the name,
  the collision with an original, the three files, the level header and field grid path, the game's field grid
  check and `CustomMap_Validate` (`custom_map_validate.h`). A map that fails is not listed; `thandor.log` has one
  line per folder with the reason (`custom map "<name>" skipped: ...`). The result of the file checks is kept while
  the files' sizes and write times stay the same.
* **Texts:** a custom map gets its own title index `0x2D0 + slot` (slot = a hash of the lowercased name, 0..0x7FF,
  probing on; `CUSTOM_MAP_TITLE_INDEX_BASE`): the title is text id `0x2500 + slot` and the description block
  `0x230010 + 0x10 * index` (every line of the block - description, faction tasks, briefing - shows the
  description). No game text page uses these ids. The loaded copy of the level carries this index instead of the
  file's; the identifier becomes `maps\<name>.`.
* **Saved games:** a save stores the loaded copy, so it lists and loads like any other save, without the map
  folder. Its title comes from the map while the map is there, else it reads *Eigene Karte* / *Custom map*.
  *Neustart* after loading such a save loads `maps\<name>\<name>.lev` again.
* **Missing files:** when the level of a *Neustart* can no longer be loaded (any level whose file is missing, or a
  custom map that is gone or now fails its checks), the game goes back to *Spiel auswählen* and logs the reason;
  the original stopped with a fatal error there.
* **Objective overlay:** custom maps get the generated objective line, never the hand-written one of an original
  level.
* **Memory:** the scan uses native memory only (no arena); without a `maps` folder nothing is allocated and the
  game behaves exactly as before.

## Creating test maps

Tool: `tools/data/custom_map.py` (uses `lev.py`, `fld.py`, `pck.py` as modules). It never writes to the game
directory it reads from.

```
python tools/data/custom_map.py list    GAME_DIR
python tools/data/custom_map.py derive  GAME_DIR ORIGINAL NAME OUTDIR [--title T] [--description D]
                                        [--title-en T] [--description-en D] [--start-xenite N] [--start-tritium N]
python tools/data/custom_map.py check   MAPDIR ...
python tools/data/custom_map.py corrupt SRC_MAPDIR OUT_MAPDIR KIND
```

* `list` prints every original level found in the PCK packages of `GAME_DIR` (and loose under `GAME_DIR\level`).
* `derive` copies one original (`asgard`, `s01_insel`, `level\nimm2\paradis`, ...) into `OUTDIR\maps\NAME\`.
  Copy that `maps` folder next to `thandor.exe` to try it. `NAME`: 1..30 characters `A-Z a-z 0-9 _ -`, not the
  base name of an original level. Example:

  ```
  python tools/data/custom_map.py derive ..\ot-soak asgard my_asgard out --title "Mein Asgard" --title-en "My Asgard" --start-xenite 9999
  ```

* `check` verifies map folders (exit code 1 when any has a problem).
* `corrupt` copies a map folder to `OUT_MAPDIR` (files, identifier and field path renamed to the base name of
  `OUT_MAPDIR`) and breaks exactly one thing. With `KIND` = `all`, `OUT_MAPDIR` is a parent folder that receives
  `bad_<kind>` for every kind. An existing target is only replaced when it holds nothing but map files.

### What `derive` writes

| File | Content |
|---|---|
| `NAME.lev` | the original level ([level_format.md](level_format.md)), byte for byte, except: identifier at +0x100 = `NAME.` (with the trailing dot, as every stock identifier); field grid path (+0xB0) = **`maps\NAME\NAME`** - relative to the game directory, without extension (the loader appends `.fld`), the same convention as the stock `level\asgard`; this path lives in a new 0x100-byte slot appended to the prefix, so the placement table moves up by 0x100 and +0x04 (size) and +0xDC (prefix size) follow; the old 0x40-byte path slot keeps the original text, unreferenced. With `--start-xenite` / `--start-tritium` (whole units) the start resources of every active faction (player slot +0x14 / +0x18, Q4) are replaced - a visible sign that the custom map was loaded. |
| `NAME.fld` | the original field grid, expanded (0x200 header + w*h*0x80 cells, worldX/worldY filled in), i.e. the loose form of [field_grid_format.md](field_grid_format.md), not the method-2 PCK form |
| `map.ini` | UTF-8 (written without BOM; `check` also accepts a BOM), CRLF; section `[map]` with `title`, `description`, optional `title.en`, `description.en`, plus `source` (the original's PCK path, informational) |

The title text index (+0x170) and every other field stay the original's; the catalog record is not added to any
`level.dat`.

### What `check` verifies

* `NAME.lev`, `NAME.fld`, `map.ini` exist (`NAME` = folder name, which must be a valid map name).
* `.lev`: `lev.parse` + `lev.check` (magic, converter, sizes, table/path spans), identifier = `NAME.` (trailing dot
  optional, case-insensitive), field grid path = `maps\NAME\NAME` (case-insensitive), terminated, with room for
  the 5 code units of the `.fld` rewrite before the next string or the prefix end.
* `.fld`: magic, converter 0x00060006, width and height 4..1024, file size = allocation size =
  0x200 + w*h*0x80.
* `map.ini`: valid UTF-8, `[map]` section with a non-empty `title`.

### Corruption kinds

| KIND | Defect |
|---|---|
| `truncated` | `.lev` cut to 0x400 bytes (shorter than the fixed 0x800-byte prefix) |
| `truncated_fld` | `.fld` cut in the middle of the cell array |
| `bad_magic` | `.lev` magic `xxx\0` |
| `bad_magic_fld` | `.fld` magic `xxx\0` |
| `huge_dims` | `.fld` width and height 0x10000 (size no longer matches) |
| `bad_path` | `.lev` field path offset (+0xB0) past the end of the file |
| `unterminated_path` | `.lev` field path points at 0x40 non-NUL bytes appended to the file (allocation size updated) |
| `traversal_path` | `.lev` field path `..\..\..\NAME` (outside the game directory) |
| `missing_fld` | `.fld` deleted |
| `bad_ini` | `map.ini` not UTF-8, no `[map]` section |
| `missing_ini` | `map.ini` deleted |
