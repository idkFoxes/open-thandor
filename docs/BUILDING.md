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

## SDL3 platform backend (`THANDOR_PLATFORM_SDL3`)

With the CMake option `THANDOR_PLATFORM_SDL3` (default `OFF`) the game uses SDL3 instead of Win32, DirectDraw,
DirectInput, WinMM timers and DirectSound for the window, the event pump, keyboard and mouse, the periodic timers,
the video presentation and the audio (`src/platform/sdl3`, interface
[`include/thandor/platform/sdl3/platform.h`](../include/thandor/platform/sdl3/platform.h)). The software renderer
is unchanged: it draws into a memory framebuffer (RGB565 or XRGB8888) that is presented letterboxed through an
SDL renderer, fullscreen on the desktop (or in a window with the developer tools' `OPEN_THANDOR_WINDOWED=1`).
The display settings list one adapter, "SDL", with the modes from 640x480 up to the desktop size in 16 and
32 bits. SDL3 is found with `find_package(SDL3)`, e.g. from vcpkg; `SDL3.dll` is copied next to `thandor.exe`:

```bat
cmake -S . -B build-sdl -G Ninja -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_BUILD_TYPE=RelWithDebInfo ^
      -DTHANDOR_PLATFORM_SDL3=ON -DCMAKE_PREFIX_PATH=<vcpkg>/installed/x86-windows
cmake --build build-sdl
```

### GPU rasterization (`THANDOR_RENDERER_SDL_GPU`)

With `THANDOR_RENDERER_SDL_GPU=ON` as well (needs `THANDOR_PLATFORM_SDL3`, default `OFF`) the build can rasterize
the 3D view on the GPU through SDL_GPU (Direct3D 12) instead of the software rasterizer
(`src/platform/sdl3/gpu_renderer.cpp`). The software renderer stays the default; the GPU path is switched on at run
time with `OPEN_THANDOR_GPU=1` or the command-line option `-GPU`. Only the rasterization of the primitive queues
moves: lighting, fog, projection, sorting and the simulation stay on the CPU, the finished 3D view is copied back
into the framebuffer, and the overlays, the UI and the cursor are drawn on it as before. Each triangle is rebuilt
from the software rasterizer's own fixed-point setup, so the picture matches the software renderer apart from
rounding (blend tables, 16-bit quantization, single edge pixels). The shaders
(`src/platform/sdl3/shaders/primitives.hlsl`) are compiled to DXBC with `fxc` from the Windows SDK during the
build. Without a Direct3D 12 device the software renderer stays (logged in `thandor.log`).

With the developer tools, `OPEN_THANDOR_GPU=compare` runs both rasterizers on every frame, shows the software
picture and every `OPEN_THANDOR_GPU_COMPARE_MS` milliseconds (default 5000) writes the 3D view of both as
`shots\gpucmp_NNNN_sw.bmp` / `_gpu.bmp` with a difference image `_diff.bmp` and logs the difference; both modes log
the per-scene times every 10 seconds.

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

## Source files that are not ordinary module code

| File | Notes |
|---|---|
| `include/thandor/generated/types.h` | The game structures shared by all modules (once exported from the decompilation, now maintained by hand; to be split into the module headers). |
| `src/<area>/<module>/*.c` | The data of the original image (globals, tables, UI templates, strings) are ordinary C variables in the file that owns them ("Module data" section after the includes, vtables in a "Class vtables" section at the end), declared in that file's header. The original addresses are listed in `docs/original_addresses.txt`. |
| `include/thandor/generated/ui_templates.h`, `proc_types.h` | UI template layouts; function pointer types of the data and callbacks. |
| `include/thandor/generated/imports.h` | KERNEL32/USER32/... import prototypes (replaced by the SDK headers in the 64-bit step). |
| `include/thandor/core/x86_emulation.h` | What the original's x86 code does, in portable C: `THANDOR_CONTAINER_OF`, atomic exchange, x87 rounding, CPUID, the MMX lane operations. |

The decompilation this project started from (Ghidra project and exports, `ghidra/`) and the tools that read
it were removed from the tree after the code no longer needed them; they are in the git history.

## Data tools (`tools/data`)

| Script | Output |
|---|---|
| `fld.py`, `lev.py`, `pck.py`, `mdl2obj.py` | readers/converters for the game's file formats |
| `symbolize.py <crash_raw.log> <thandor.map>` | names for the raw crash dump |

## Known TODOs

Search for `TODO` in the tree. The main groups:

- **Multiplayer**: network command codes are distances between original handler addresses (see
  `include/thandor/network/protocol/commands.h`); received commands are resolved to the recovered C
  handlers through explicit command tables (`CommandDispatch_ResolveHandler`). Not yet tested in a real
  networked game.
- **Stack frames** (`thandor_stack_frame`): `richtext.c` keeps unrecovered stack locals in an unreachable function.
