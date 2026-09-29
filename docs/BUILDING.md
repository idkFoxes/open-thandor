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

- nothing from the original executable: its data (globals, tables, UI templates) is compiled in from
  `src/generated/image_data.c`. Only the `mapped` build and the differential self-tests
  (`rastercmp`, `blitcmp`, `blendscalecmp`, `relaxcmp`, `imagecmp`) read `thandor_original.exe`.
- the game's `*.PCK` files and `thandor.dat` from the installation,
- optionally `flm\` with the full-length movies from the CD (`Ende*.flm`, `Intro2.flm`); the
  packages hold only still-image stand-ins for them.

Environment switches for testing:

| Variable | Effect |
|---|---|
| `OPEN_THANDOR_SELFTEST=codec\|path\|stretch\|stretchcmp\|scanaddr\|crash` | run one self-test and exit (results in `thandor.log`) |
| `OPEN_THANDOR_MOVIE=<name>\|all` | play `flm\<name>.flm`, or every name in `movies.txt`, max. 10 s each, with name and frame counter top left (`OPEN_THANDOR_MOVIE_START`, `_STRETCH`; `OPEN_THANDOR_MOVIEEXPORT=<name>[,...]` writes frames and audio to `moviedump\`); the player is in [`src/platform/debug/movie_player.c`](../src/platform/debug/movie_player.c) |
| `OPEN_THANDOR_AUTOSHOT=<ms>` | save the framebuffer every <ms> to `shots\shot_NNNN.bmp` |
| `OPEN_THANDOR_SCRIPT=<file>` | replay timed input (`<ms> click x y`, `rclick`, `move`, `key <vk>`, `quit`) |
| `OPEN_THANDOR_POISON=1` | overwrite all original instructions with INT3 (needs `code_starts.bin`, see below) |

Unattended test: `python tools/test/run_game.py <game dir> 120 --args '-NOINTRO -KARTE="mittelpunkt"' --script tools/test/skirmish_start.txt` starts a skirmish on Ahaggar, plays two minutes, and reports the log, crashes and a contact sheet of snapshots. `tools/test/skirmish_move.txt` also selects the starting vehicle and sends it to two points (left click on the unit, then on the ground), which exercises path finding; its `ingame` line waits until the level has loaded, and the times after it count from that moment. Command-line options need a leading `-` (`-NOINTRO`, `-KARTE="<level>"`).

### Test build (windowed, local multiplayer)

The test aids for running two instances on one machine are compiled in only with
`-DTHANDOR_TEST_AIDS=ON` (preset `test`: `cmake --preset test && cmake --build --preset test`); the
default build leaves them out and compiles to the same code as before they existed. With the test build:

| Variable | Effect |
|---|---|
| `OPEN_THANDOR_WINDOWED=1` | normal window instead of full-screen exclusive (desktop colour depth, software renderer only, non-exclusive mouse); position with `OPEN_THANDOR_WINDOW_X` / `OPEN_THANDOR_WINDOW_Y` (default 0,0) |
| `OPEN_THANDOR_MULTI_INSTANCE=1` | allow a second instance although a game window exists |
| `OPEN_THANDOR_NET_PORT=<n>` | bind this instance's UDP socket to port n; it still addresses the peer's game port |
| `OPEN_THANDOR_NETLOG=1` | log every datagram sent and received |
| `OPEN_THANDOR_LIST_SCENARIOS=1` | log the names of all single games and campaigns when the "Choose game" page opens, then quit |
| `OPEN_THANDOR_CAMPAIGN=<name>` | start that campaign when the "Choose game" page opens (with `-KARTE="-"` to get there); `OPEN_THANDOR_CAMPAIGN_LEVEL=<n>` starts it at its n-th level, without the units the previous level would carry over |
| `OPEN_THANDOR_AUTOWIN=<s>` | end a campaign level as won after <s> seconds: fires the level's end trigger that moves the campaign forward (a later level, else the campaign end) after moving local mobile units that stand outside the exit zone into it; `OPEN_THANDOR_AUTOWIN_LEVELS=<k>` limits it to the first k levels of the run |

In the test build the real mouse is also ignored while `OPEN_THANDOR_SCRIPT` drives the game, and a
failed `OPEN_THANDOR_AUTOSHOT` capture is logged. `python tools/test/run_multiplayer.py <game dir> 180
--host-script tools/test/mp_host_create.txt --client-script tools/test/mp_client_join.txt` starts a host
and a client side by side with these switches; copy the test build's `thandor.exe` into the game
directory first. The test build also logs the level script (its conditions and triggers, and which end
trigger fired) and how many units a campaign carries over into a level.

`python tools/test/run_all_maps.py <game dir> --jobs 10 --minutes 1` starts every campaign level and single
game (ten at a time, each in its own linked copy of the game directory), sets the computer opponents to
"stark" and the game speed to its maximum, lets each run a minute and reports per mission whether it loaded,
ended early, hung or crashed; screenshots and a contact sheet per mission go to `<game dir>/soak/`.

`python tools/test/run_campaign_chain.py <game dir> pairs` plays into each level that needs the previous
level's units (tutorial 2 and 3, Hansolo 9, 13, 23) through the real level change and checks that units
arrive; `... campaigns [--only tutorial,luke]` wins every level of each campaign in turn up to the campaign
end. The script command `clickuntilnextlevel x y <ms>` clicks until the next level has loaded, then the times
restart at 0. Results go to `<game dir>/chain/`.

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
- **Multiplayer**: network command codes are distances between original handler addresses (see
  `include/thandor/network/protocol/commands.h`); received commands are resolved to the recovered C
  handlers through the function map (`CommandDispatch_ResolveHandler`). Not yet tested in a real
  networked game.
- **Five string literals** (`TODO: verify text` in `src/generated/globals.c`) have no string data in the program
  database and are still reconstructed from their labels.
- **Stack frames** (`thandor_stack_frame`): `richtext.c` keeps unrecovered stack locals in an unreachable function.
