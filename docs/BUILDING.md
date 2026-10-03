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

- nothing from the original executable: its data (globals, tables, UI templates) is compiled in as C
  variables of the modules (`src/<area>/<module>/data.c`). Only the optional differential self-tests that run
  copies of original code (`relaxcmp`, `stretchcmp`, the movie decoder compare `OPEN_THANDOR_MOVIECMP=1`; developer
  tools only) read `thandor_original.exe` next to the executable.
- the game's `*.PCK` files and `thandor.dat` from the installation,
- optionally `flm\` with the full-length movies from the CD (`Ende*.flm`, `Intro2.flm`); the
  packages hold only still-image stand-ins for them.

The game always draws with its software renderer and presents through DirectDraw; the original's 3dfx Glide
and Direct3D renderers (and their `-GLIDE` and `-D3DALL` options) were removed, and the display settings list
only DirectDraw adapters. A `THANDOR.cfg` that still names an adapter index past that list starts on adapter 0.

## Developer tools (`THANDOR_DEV_TOOLS`)

All test and debug aids of the port - the self-tests, the input scripts, automatic screenshots, the determinism
state hash, campaign starts and AUTOWIN, the level-script log, the movie player, dump and compare, windowed mode,
a second instance, the UDP port and datagram log, the watchdog - are compiled in only with the CMake option
`THANDOR_DEV_TOOLS` (default `OFF`):

```bat
cmake -S . -B build-test -G Ninja -DCMAKE_C_COMPILER=cl -DCMAKE_BUILD_TYPE=RelWithDebInfo -DTHANDOR_DEV_TOOLS=ON
cmake --build build-test
```

or the preset `test` (`cmake --preset test && cmake --build --preset test`). This test build is what
`tools/test` drives (`run_checks.py` takes `build-test/thandor.exe`). The tools live in `src/platform/debug` and
`src/platform/selftest`; the game reaches them only through the hooks in
[`include/thandor/platform/debug/hooks.h`](../include/thandor/platform/debug/hooks.h). With the option `OFF` those
sources are not compiled, every hook is a macro that expands to nothing (or passes the original value through),
no `OPEN_THANDOR_*` variable is read and the game runs the original code paths. The old option name
`THANDOR_TEST_AIDS=ON` is still accepted and switches `THANDOR_DEV_TOOLS` on.

In a build with the developer tools each tool is switched on at run time by its environment variable; without
the variable it does nothing:

| Variable | Effect |
|---|---|
| `OPEN_THANDOR_SELFTEST=codec\|movieenc\|numberformat\|fixedmath\|keymap\|trianglesetup\|path\|stretch\|stretchcmp\|relaxcmp\|scanaddr\|pcx\|crash` | run one self-test and exit (results in `thandor.log`; `pcx` decodes `pcxtest.pcx`, written with the expected result by `tools/test/pcx_check.py`; `codec` and `movieenc` log hashes of the save-game encoder and the FLM encoders/decoder output - compare them between two builds after touching those; `scanaddr` also reads `OPEN_THANDOR_SCANFILES` and `OPEN_THANDOR_DUMPTEXT`) |
| `OPEN_THANDOR_MOVIE=<name>\|all` | play `flm\<name>.flm`, or every name in `movies.txt`, max. 10 s each, with name and frame counter top left (`OPEN_THANDOR_MOVIE_START`, `_STRETCH`; `OPEN_THANDOR_MOVIEEXPORT=<name>[,...]` writes frames and audio to `moviedump\`); the player is in [`src/platform/debug/movie_player.c`](../src/platform/debug/movie_player.c) |
| `OPEN_THANDOR_MOVIEDUMP=1` / `OPEN_THANDOR_MOVIECMP=1` | log every decoded movie frame (every tenth also to `moviedump\`) / compare the frame decoder with the original machine code |
| `OPEN_THANDOR_AUTOSHOT=<ms>` | save the framebuffer every <ms> to `shots\shot_NNNN.bmp` (a failed capture is logged) |
| `OPEN_THANDOR_SCRIPT=<file>` | replay timed input (`<ms> click x y`, `rclick`, `move`, `key <vk>`, `keydown <vk>` / `keyup <vk>` for held keys such as Alt+P, `type <text>` types the rest of the line into a text field as the window procedure delivers it - space as VK_SPACE, letters and digits as key-down plus WM_CHAR (`Keyboard_OnChar`) -, `shot` saves the framebuffer now as `shots\script_NNNN.bmp`, `quit`); the real mouse is ignored meanwhile |
| `OPEN_THANDOR_STATEHASH=<steps>` | determinism test: state hash per simulation step to `statehash.txt` (`_SEED`, `_DETAIL`, `_PAUSE_AT`, `_SPEED`, `OPEN_THANDOR_ARENA_ORDERS`; see below and [`src/platform/debug/statehash.c`](../src/platform/debug/statehash.c)) |
| `OPEN_THANDOR_WINDOWED=1` | normal window instead of full-screen exclusive (desktop colour depth, non-exclusive mouse); position with `OPEN_THANDOR_WINDOW_X` / `OPEN_THANDOR_WINDOW_Y` (default 0,0) |
| `OPEN_THANDOR_MULTI_INSTANCE=1` | allow a second instance although a game window exists |
| `OPEN_THANDOR_NET_PORT=<n>` | bind this instance's UDP socket to port n; it still addresses the peer's game port |
| `OPEN_THANDOR_NETLOG=1` | log every datagram sent and received |
| `OPEN_THANDOR_LIST_SCENARIOS=1` | log the names of all single games and campaigns when the "Choose game" page opens, then quit |
| `OPEN_THANDOR_CAMPAIGN=<name>` | start that campaign when the "Choose game" page opens (with `-KARTE="-"` to get there); `OPEN_THANDOR_CAMPAIGN_LEVEL=<n>` starts it at its n-th level, without the units the previous level would carry over |
| `OPEN_THANDOR_AUTOWIN=<s>` | end a campaign level as won after <s> seconds: fires the level's end trigger that moves the campaign forward (a later level, else the campaign end) after moving local mobile units that stand outside the exit zone into it; `OPEN_THANDOR_AUTOWIN_LEVELS=<k>` limits it to the first k levels of the run |
| `OPEN_THANDOR_WATCHDOG=<s>` | log the main thread's stack to `thandor.log` every <s> seconds |

Always on in this build: the level-script log (its conditions and triggers, and which end trigger fired) and how
many units a campaign carries over into a level. The crash and hang reports (`crash.log`, `hang.log`) are part of
every build.

Unattended test: `python tools/test/run_game.py <game dir> 120 --args '-NOINTRO -KARTE="mittelpunkt"' --script tools/test/skirmish_start.txt` starts a skirmish on Ahaggar, plays two minutes, and reports the log, crashes and a contact sheet of snapshots. `tools/test/skirmish_move.txt` also selects the starting vehicle and sends it to two points (left click on the unit, then on the ground), which exercises path finding; its `ingame` line waits until the level has loaded, and the times after it count from that moment. Command-line options need a leading `-` (`-NOINTRO`, `-KARTE="<level>"`).

### Local multiplayer

`python tools/test/run_multiplayer.py <game dir> 180 --host-script tools/test/mp_host_create.txt --client-script
tools/test/mp_client_join.txt` starts a host and a client side by side with the windowed, second-instance and
UDP port switches; copy the test build's `thandor.exe` into the game directory first.

### Automated checks

`python tools/test/run_all_maps.py <game dir> --jobs 10 --minutes 1` starts every campaign level and single
game (ten at a time, each in its own linked copy of the game directory), sets the computer opponents to
"stark" and the game speed to its maximum, lets each run a minute and reports per mission whether it loaded,
ended early, hung or crashed; screenshots and a contact sheet per mission go to `<game dir>/soak/`.

Determinism test (the safety net for changes that alter the machine code): `python tools/test/run_determinism.py
<game dir> --reference tools/test/determinism_reference` generates three test levels (tools/test/make_arena.py:
flat map, two players), plays each for 1200 simulation steps with a fixed random seed and compares a hash of the
game state after every step (test aid `OPEN_THANDOR_STATEHASH`, platform/debug/statehash.c) with the stored
reference. Scenarios: `battle` (every unit and building type on both sides, move orders at steps 100 and 500),
`turrets` (every armed static defence with power plants), `production` (factories, labs, storage; units queued
and research started at step 20). Without `--reference` each scenario runs twice in parallel and the runs must
match each other; `--detail <tick>` writes every army's values at the first differing tick. After an intended
behaviour change, save new references with `--save-reference tools/test/determinism_reference`.
The arena scenarios have no computer opponents; `python tools/test/compare_map_hash.py stromschnelle 1400 <dir>
<dir2> ...` plays a stock map with strong computer opponents under the same state hash in several game
directories at once (game speed 5 from the first step, `OPEN_THANDOR_STATEHASH_SPEED`) and reports the first
differing step - put the previous build into one directory to compare versions.
All behaviour checks after a build in one command: `python tools/test/run_checks.py <game dir> --old
<previous build-test/thandor.exe>` runs in parallel, each in its own linked copy `<game dir>_chk_<name>` with its
own UDP ports: the determinism references, the AI state hash on stromschnelle (two copies of the new build plus
the old one), a pixel compare new vs old of the paused in-game frame and the choose-game page (only with
`--old`), save and load of a skirmish (`skirmish_save.txt`, `choose_load.txt`), the text edit keys in the save
dialog (`textedit_save.txt` turns the prefilled "Multi Ahaggar" into "Test Edit 42" with Home/End, Shift+Home/End,
Ctrl+Left/Right, Delete, Backspace and typing; `save\Test Edit 42.sve` must exist), the two-instance multiplayer
test, the campaign carry-over pairs (`run_campaign_chain.py pairs`, 5 workers, every
target level must receive units) and the single maps (`--map-jobs`, `--map-minutes`; ENDED is only a warning, the
strong computer opponents can win a map in time). `--new` defaults to `build-test/thandor.exe`, `--skip a,b`
leaves checks out. It prints one table (check, result, details, duration), writes each check's output to
`<game dir>/checks/` and exits non-zero when a check failed; it only stops game processes it started.
`python tools/test/run_campaign_chain.py <game dir> pairs` plays into each level that needs the previous
level's units (tutorial 2 and 3, Hansolo 9, 13, 23) through the real level change and checks that units
arrive; `... campaigns [--only tutorial,luke]` wins every level of each campaign in turn up to the campaign
end. The script command `clickuntilnextlevel x y <ms>` clicks until the next level has loaded, then the times
restart at 0. Results go to `<game dir>/chain/`; worker k uses UDP port `--port-base` (940) + k.

## Generated files and tools

| File | Produced by | Notes |
|---|---|---|
| `include/thandor/generated/types.h` | Ghidra export, ordered by `tools/sort_types.py` | Re-run the script after pasting a new export. |
| `src/<area>/<module>/data.c`, `include/thandor/<area>/<module>/data.h` | hand-written | The data of the original image (globals, tables, UI templates, strings) as ordinary C variables, original addresses in the comments. `include/thandor/generated/image_data.h` includes all module data headers. |
| `include/thandor/generated/ui_templates.h`, `proc_types.h` | hand-written (once generated) | UI template layouts; function pointer types of the data and callbacks. |
| `include/thandor/generated/imports.h` | `python tools/gen_imports.py` | KERNEL32/USER32 import prototypes from `ghidra/export/imports.jsonl`. |
| `ghidra/export/*.jsonl` | `tools/ghidra/ExportBuildData.java` (headless, see below) | Function-signature types, string values, labels, imports and struct layouts that Ghidra's C export omits (read by `gen_imports.py`, `check_layouts.py` and `tools/data`). Committed, so the tools do not need Ghidra. |
| `include/thandor/core/ghidra.h` | hand-written | `CONCATxy`, `SUBxy`, `CARRYx`, partial access, `THANDOR_BITCAST`, `THANDOR_CONTAINER_OF`. |

One-shot rewriters used on `src/` (safe to re-run on a fresh decompiler export):

- `tools/fix_partial_access.py` — `x._off_size_` → `THANDOR_PART` / `THANDOR_READ_PART` / `THANDOR_WRITE_PART`
- `tools/fix_abi_casts.py <msvc.log>` — register-image struct casts flagged by C2440 → `THANDOR_BITCAST`
- `tools/fix_stack_refs.py` — leftover `stack0x...` slots → per-function `thandor_stack_frame`

## Check tools (`tools/data`)

`param_ret_scan.py` and `scanaddr_analyze.py` take the original entry address of a C function from the
`/* Address: 0x... */` comment directly above it (`common.function_map()`). `param_ret_scan.py` also
needs `--original <thandor.exe>` and `--asm <dir>` (per-function disassembly, default `build-data\asm`).
Produce the disassembly once with Ghidra:

```bat
"%GHIDRA_HOME%\support\analyzeHeadless.bat" %TEMP%\ghproj thandor -import ghidra\thandor.exeV537.gzf -noanalysis ^
    -scriptPath tools\ghidra -postScript DumpDisassembly.java build-data\asm * -deleteProject
```

| Script | Output |
|---|---|
| `scanaddr_analyze.py <scanaddr.txt>` | files (packages, saves) that store original function entries or labeled data addresses, from the `scanaddr` self-test |
| `param_ret_scan.py` | check: functions reached through pointers that take more parameters than the original pops |
| `unresolved_registers.py` | check: functions that still read register values the decompiler could not resolve (`in_EAX`, `unaff_EBX`, ...) |
| `symbolize.py <crash_raw.log> <thandor.map>` | names for the raw crash dump |

## Refreshing the Ghidra export

Needs Ghidra 12 and JDK 21+ (`JAVA_HOME`):

```bat
"%GHIDRA_HOME%\support\analyzeHeadless.bat" %TEMP%\ghproj thandor -import ghidra\thandor.exeV537.gzf -noanalysis ^
    -scriptPath tools\ghidra -postScript ExportBuildData.java ghidra\export -deleteProject
python tools\gen_imports.py
```

## Known TODOs

Search for `TODO` in the tree. The main groups:

- **Multiplayer**: network command codes are distances between original handler addresses (see
  `include/thandor/network/protocol/commands.h`); received commands are resolved to the recovered C
  handlers through explicit command tables (`CommandDispatch_ResolveHandler`). Not yet tested in a real
  networked game.
- **Stack frames** (`thandor_stack_frame`): `richtext.c` keeps unrecovered stack locals in an unreachable function.
