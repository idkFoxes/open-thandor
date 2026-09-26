# Building

The tree builds a 32-bit Windows executable (`thandor.exe`) with MSVC.

```bat
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat"
cmake -S . -B build-rel -G Ninja -DCMAKE_C_COMPILER=cl -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build-rel
```

Use `RelWithDebInfo` for testing: it is optimized (`/O2`) and keeps the PDB, so `crash.log` shows
function names and lines. `Debug` (`/Od`) is much slower in game; `Release` drops the PDB.
Set `LINK=/MAP` before building to get `thandor.map` for `tools/data/symbolize.py`.

## Running

Next to `thandor.exe` the game currently needs:

- `thandor_original.exe` — a copy of the original executable. Its data (globals, tables, UI
  templates) is still mapped at 0x400000; no original machine code runs (see `OPEN_THANDOR_POISON`).
- the game's `*.PCK` files and `thandor.dat` from the installation,
- optionally `flm\` with the full-length movies from the CD (`Ende*.flm`, `Intro2.flm`); the
  packages hold only still-image stand-ins for them.

Environment switches for testing:

| Variable | Effect |
|---|---|
| `OPEN_THANDOR_SELFTEST=codec\|path\|stretch\|stretchcmp\|scanaddr\|crash` | run one self-test and exit (results in `thandor.log`) |
| `OPEN_THANDOR_MOVIE=<name>\|all` | play `flm\<name>.flm`, or every name in `movies.txt`, max. 10 s each (`OPEN_THANDOR_MOVIE_START`, `_STRETCH`, `OPEN_THANDOR_MOVIEDUMP`) |
| `OPEN_THANDOR_AUTOSHOT=<ms>` | save the framebuffer every <ms> to `shots\shot_NNNN.bmp` |
| `OPEN_THANDOR_SCRIPT=<file>` | replay timed input (`<ms> click x y`, `rclick`, `move`, `key <vk>`, `quit`) |
| `OPEN_THANDOR_POISON=1` | overwrite all original instructions with INT3 (needs `code_starts.bin`, see below) |

Unattended test: `python tools/test/run_game.py <game dir> 120 --args '-NOINTRO -KARTE="mittelpunkt"' --script tools/test/skirmish_start.txt` starts a skirmish on Ahaggar, plays two minutes, and reports the log, crashes and a contact sheet of snapshots. Command-line options need a leading `-` (`-NOINTRO`, `-KARTE="<level>"`).

## Generated files and tools

| File | Produced by | Notes |
|---|---|---|
| `include/thandor/generated/types.h` | Ghidra export, ordered by `tools/sort_types.py` | Re-run the script after pasting a new export. |
| `include/thandor/generated/globals.h`, `src/generated/globals.c` | `python tools/gen_globals.py ghidra/thandor.exeV537.c` | Globals, recovered function-pointer signatures (`tools/infer_signatures.py`), Ghidra-invented symbols. Hand corrections: the UI template types. |
| `include/thandor/data/recovered.h` | hand-written | Named data the code used to reach through raw addresses. |
| `ghidra/export/*.jsonl` | `tools/ghidra/ExportBuildData.java` (headless, see below) | Function-signature types, string values and data symbols that Ghidra's C export omits. Committed, so regenerating does not need Ghidra. |
| `include/thandor/core/ghidra.h` | hand-written | `CONCATxy`, `SUBxy`, `CARRYx`, partial access, `THANDOR_BITCAST`, `THANDOR_CONTAINER_OF`. |

One-shot rewriters used on `src/` (safe to re-run on a fresh decompiler export):

- `tools/fix_partial_access.py` — `x._off_size_` → `THANDOR_PART` / `THANDOR_READ_PART` / `THANDOR_WRITE_PART`
- `tools/fix_abi_casts.py <msvc.log>` — register-image struct casts flagged by C2440 → `THANDOR_BITCAST`
- `tools/fix_stack_refs.py` — leftover `stack0x...` slots → per-function `thandor_stack_frame`

## Data layout tools (`tools/data`)

These analyse which parts of the original image the C code still depends on. All take
`--original <thandor.exe>`, `--asm <dir>` (per-function disassembly) and `--work <dir>` (default
`build-data`). Produce the disassembly once with Ghidra:

```bat
"%GHIDRA_HOME%\support\analyzeHeadless.bat" %TEMP%\ghproj thandor -import ghidra\thandor.exeV537.gzf -noanalysis ^
    -scriptPath tools\ghidra -postScript DumpDisassembly.java build-data\asm * -deleteProject
```

| Script | Output |
|---|---|
| `typelayout.py` | `typelayout.json`: offset, size and kind of every struct field (compiler `offsetof`); lets the generator write typed initializers |
| `gen_image_data.py` | `src/generated/image_data.c` + `include/thandor/generated/image_data.h`: the original data as C (run `globalmap.py`, `layout.py`, `typelayout.py` first; check with the `imagecmp` self-test) |
| `globalmap.py` | `globalmap.txt`: address and compiler `sizeof` of every address-defined object (needs `cl` from a vcvars prompt). Run first. |
| `rawaddr.py` | check: original-image address literals left in `src/` (expected: 0) |
| `layout.py` | `layout.tsv`, `members.tsv`: every data byte assigned to an object; globals typed too small; bytes in no object |
| `reach.py` | `reach.tsv`: objects the C code needs, following pointers from the named roots |
| `code_starts.py` | `code_starts.bin` for `OPEN_THANDOR_POISON=1` |
| `scanaddr_analyze.py <scanaddr.txt>` | files (packages, saves) that store original addresses, from the `scanaddr` self-test |
| `param_ret_scan.py` | check: functions reached through pointers that take more parameters than the original pops |
| `symbolize.py <crash_raw.log> <thandor.map>` | names for the raw crash dump |

## Refreshing the Ghidra export

Needs Ghidra 12 and JDK 21+ (`JAVA_HOME`):

```bat
"%GHIDRA_HOME%\support\analyzeHeadless.bat" %TEMP%\ghproj thandor -import ghidra\thandor.exeV537.gzf -noanalysis ^
    -scriptPath tools\ghidra -postScript ExportBuildData.java ghidra\export -deleteProject
python tools\gen_globals.py ghidra\thandor.exeV537.c
```

## Known TODOs

Search for `TODO` in the tree. The main groups:

- **Independence from the original executable**: the data still lives in the mapped image; next is
  generating it as C definitions (see `tools/data`).
- **Multiplayer command dispatch** (`THANDOR_CODE_AT`): command ids are offsets from handler code
  addresses of the original; needs a real handler table.
- **Five string literals** (`TODO: verify text` in `src/generated/globals.c`) have no string data in the program
  database and are still reconstructed from their labels.
- **Stack frames** (`thandor_stack_frame`): `richtext.c` keeps unrecovered stack locals in an unreachable function.
