# Custom maps (`maps\<name>\`, single player)

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
