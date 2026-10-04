# Building

The tree builds a 64-bit Windows executable (`thandor.exe`, x64) with MSVC or MinGW-w64 GCC
([below](#mingw-w64-gcc)) and SDL3. The 32-bit build and the
original's Win32/DirectX platform code (DirectDraw, DirectInput, DirectSound, WinMM timers, the Win32 message pump)
were removed once the x64 build reproduced every determinism hash and pixel of the 32-bit one; they are in the git
history.

Requirements:

- Visual Studio 2022 or newer with the C++ workload (MSVC x64 tools and a Windows 10/11 SDK)
- CMake 3.25 or newer and Ninja (both come with Visual Studio)
- SDL3 for x64, e.g. from vcpkg: `vcpkg install sdl3:x64-windows`

SDL3 is found with `find_package(SDL3)`: pass its install prefix in `CMAKE_PREFIX_PATH`
(`-DCMAKE_PREFIX_PATH=<vcpkg>/installed/x64-windows`), or set the environment variable `VCPKG_ROOT` to the vcpkg
directory, whose `installed/x64-windows` CMake then searches as well. The build copies `SDL3.dll` next to
`thandor.exe`; the game needs it there.

From a vcvars64 prompt:

```bat
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake -S . -B build-rel -G Ninja -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_BUILD_TYPE=RelWithDebInfo ^
      -DCMAKE_PREFIX_PATH=<vcpkg>/installed/x64-windows
cmake --build build-rel
```

or with a preset, which finds MSVC x64 without a vcvars prompt (`cmake/msvc-x64.cmake`; SDL3 through `VCPKG_ROOT`
or `-DCMAKE_PREFIX_PATH=...` on the command line):

| Preset | Build |
|---|---|
| `release` | RelWithDebInfo (`cmake-build-msvc-release`) |
| `debug` | Debug (`cmake-build-msvc-debug`) |
| `test` | RelWithDebInfo with the developer tools, in `build-test` (what `tools/test` runs) |
| `gpu-test` | as `test`, plus the SDL_GPU rasterizer (`THANDOR_RENDERER_SDL_GPU`, see below) |

```bat
cmake --preset test
cmake --build --preset test
```

CMake stops with an error for anything but MSVC x64 or MinGW-w64 GCC x64. Use `RelWithDebInfo` for testing: it is optimized (`/O2`) and
keeps the PDB, so `crash.log` shows function names and lines. `Debug` (`/Od`) is much slower in game; `Release`
drops the PDB. Set `LINK=/MAP` before building to get `thandor.map` for `tools/data/symbolize.py`
(`symbolize.py crash_raw.log <build>\thandor.map`).

The original data layouts keep their pointers in 32-bit fields (`Ptr32<T>`,
[`include/thandor/core/ptr32.h`](../include/thandor/core/ptr32.h)), so savegames and assets keep their format; the
executable is linked `/LARGEADDRESSAWARE:NO` (every address below 2 GB) at the fixed base `0x10000000`.

### MinGW-w64 GCC

The same tree builds with MinGW-w64 GCC for x64 (x86_64-w64-mingw32, SEH exceptions; tested with the MinGW-Builds
GCC 15.2 in `C:\mingw64`). Requirements: the toolchain (`g++`, `ld`, `windres`, `addr2line`) and Ninja (the
MinGW-Builds distribution has `ninja.exe` in its `bin`), CMake 3.25+, and SDL3 for MinGW:
`vcpkg install sdl3:x64-mingw-dynamic` (vcpkg's `windres` step fails when the vcpkg path contains a space; build
with `--x-buildtrees-root=C:/vcbt` or another path without spaces then).

```bat
set PATH=C:\mingw64\bin;%PATH%
set VCPKG_ROOT=C:\path\to\vcpkg
cmake --preset mingw-test
cmake --build --preset mingw-test
```

| Preset | Build |
|---|---|
| `mingw-release` | RelWithDebInfo (`build-mingw-release`) |
| `mingw-test` | RelWithDebInfo with the developer tools (`build-mingw-test`) |
| `mingw-gpu-test` | as `mingw-test`, plus the SDL_GPU rasterizer (`build-mingw-gpu-test`) |

[`cmake/mingw-x64.cmake`](../cmake/mingw-x64.cmake) takes the toolchain from the cache variable or environment
variable `MINGW_ROOT` (the directory with `bin\g++.exe`), else the `g++` on the `PATH`, else `C:\mingw64`,
`C:\msys64\mingw64` or `C:\msys64\ucrt64`. SDL3 comes from `CMAKE_PREFIX_PATH` or
`%VCPKG_ROOT%\installed\x64-mingw-dynamic`. The C and C++ runtimes are linked statically (`-static`), so only
`SDL3.dll` goes next to `thandor.exe`; the executable uses the Universal CRT like the MSVC build.

What keeps the two builds identical in behaviour (the self-test hashes and the determinism references match):

- GCC flags: `-fno-strict-aliasing` (the recovered code reinterprets memory), `-fwrapv` (signed overflow wraps as
  on MSVC), `-ffp-contract=off` (no fused multiply-add; MSVC x64 does not contract), `-mms-bitfields` (MSVC struct
  and bit-field layout; [`src/core/layout_checks.cpp`](../src/core/layout_checks.cpp) checks the sizes and pointer
  offsets of every layout at compile time). `-fpermissive` lets the casts of pointers to 32-bit integers through
  (they truncate as on MSVC, which warns C4311/C4312 for the same lines); `-Wall` without `-Wparentheses`,
  `-Wsign-compare` and `-Wcomment`, which the decompiled code triggers by the hundreds.
- Function arguments and operands are evaluated in an unspecified order, and MSVC and GCC differ: an expression with
  two state-changing calls (two random draws, two reads of a stream) gets explicit temporaries in MSVC's order.
- `THANDOR_ALIGN(n)` (core/contracts.h) instead of `__declspec(align(n))`; the crash handler's guarded stack walk
  (`__try`) is MSVC-only; `_ReturnAddress` becomes `__builtin_return_address(0)`.
- Linker: `--image-base=0x10000000 --disable-dynamicbase --disable-high-entropy-va`. GNU ld marks every 64-bit image
  large-address aware (its `--disable-large-address-aware` is for 32-bit images only), so the build runs
  [`cmake/pe_not_large_address_aware.cpp`](../cmake/pe_not_large_address_aware.cpp) on `thandor.exe` after the
  link: it clears `IMAGE_FILE_LARGE_ADDRESS_AWARE` and fails if the image is relocatable. Check with
  `objdump -p thandor.exe`: `Characteristics` without 0x20, `ImageBase 0000000010000000`, `DllCharacteristics`
  only `NX_COMPAT`.

Crash and hang logs of a GCC build: there is no PDB, so `crash.log`, `hang.log` and the watchdog give each frame as
`thandor.exe+0xOFFSET` (absolute address `0x10000000 + OFFSET`, the image base is fixed and logged as `module base`).
Symbolize with the executable's DWARF line information:

```bat
addr2line -f -C -i -e build-mingw-test\thandor.exe 0x100516AA 0x1000146C
python tools\data\symbolize.py crash_raw.log build-mingw-test\thandor.exe
```

`symbolize.py` calls `addr2line` for an `.exe` and reads a linker map otherwise; the GCC build also writes
`thandor.map` (`-Wl,-Map`), which lists only global symbols (no `static` functions), so prefer the executable.

With `THANDOR_RENDERER_SDL_GPU` a GCC build compiles the shaders with `fxc` when it finds the Windows SDK, and
otherwise uses the headers fxc made, committed in
[`src/platform/sdl3/shaders/compiled/`](../src/platform/sdl3/shaders/compiled) (`-DTHANDOR_GPU_PRECOMPILED_SHADERS=ON`
forces them). After a change of `primitives.hlsl` regenerate them: build the MSVC preset `gpu-test` and copy
`<build dir>\gpu_shaders\gpu_shader_*.h` there.

## Running

Next to `thandor.exe` the game needs:

- `SDL3.dll` (the build puts it next to `thandor.exe`; copy both into the game directory),
- the game's `*.PCK` files from the installation (and its `thandor.dat` for the old settings, see below),
- optionally `flm\` with the full-length movies from the CD (`Ende*.flm`, `Intro2.flm`); the
  packages hold only still-image stand-ins for them.

Nothing of the original executable is needed: its data (globals, tables, UI templates) is compiled in.

### Settings file (`thandor.ini`)

The settings live in `thandor.ini`, a text file with one commented key per setting
([`src/core/settings/persistent.cpp`](../src/core/settings/persistent.cpp)). The game looks for it in the current
directory, then next to `thandor.exe`, and rewrites it when a setting changed in its menus. It can be edited by
hand while the game is not running: a missing key means the game's default, unknown keys are ignored, values are
decimal or `0x` hex, switches `true`/`false`, volumes 0..32768 or a percentage (`effects_volume = 50%`),
names UTF-8 in quotes (at most 20 characters).

Without a `thandor.ini` the game reads the original binary `thandor.dat` (200 bytes) of the installation once and
writes its settings as `thandor.ini` on the next save; `thandor.dat` itself is never changed or deleted. Delete
`thandor.ini` to take the settings from `thandor.dat` again. The settings of a typical installation, migrated:

```ini
; Open Thandor settings. The game rewrites this file when a setting changes in its menus.
; A missing key means the game's default; unknown keys are ignored.

[display]
; graphics adapter, 0 = the first (default 0)
adapter = 0
; screen width in pixels (default 640)
width = 1280
; screen height in pixels (default 480)
height = 800
; colour depth in bits (default 16)
bits_per_pixel = 32
; renderer: vulkan (default), d3d12 or software
renderer = vulkan
; fullscreen (default), borderless or window
display_mode = fullscreen

[graphics]
; terrain shading (default true)
shading = true
; shading grid half size (default 32)
shading_grid_half_size = 128
; shading texture size, 2 x shading_grid_half_size (default 64)
shading_texture_size = 256
; shading depth (default 16)
shading_depth = 32
; texture quality: high, medium or low
texture_quality = high
; model detail distance, 8.8 fixed point (default 65536)
model_detail = 262144

[sound]
; sound effects (default true)
effects = true
; music (default true)
music = true
; swap the left and right channel (default false)
reverse_stereo = false
; sound effects volume, 0..32768 = 0..100 % (default 32768); here 19.3 %
effects_volume = 6315
; music volume, 0..32768 = 0..100 % (default 32768); here 37.9 %
music_volume = 12408
; movie volume, 0..32768 = 0..100 % (default 32768); here 18.2 %
movie_volume = 5973
; second movie volume, 0..32768 = 0..100 % (default 32768); here 18.8 %
movie_alternate_volume = 6144

[game]
; game speed in percent (default 100)
speed_percent = 120
; map scroll speed (default 32)
scroll_step = 30
; bits: 0x1 automatic zoom off, 0x2 automatic rotation off, 0x4 right button does not scroll / side panel hidden (default 0x0)
map_mouse_options = 0x0
; bits: 0x1 link rotation with zoom, 0x2 link rotation with tilt, 0x4 hide panel (default 0x0)
link_panel_options = 0x2
; player name, at most 20 characters
player_name = "Spieler"
; name of a hosted network game, at most 20 characters
game_name = "Thandorspiel"
; text language as telephone country code (49 German, 44 English, ...), 0 = from Windows
language_country_code = 0

[network]
; player count of a hosted network game (default 4)
players = 8
```

Internally the game still works on the original 200-byte layout (`PersistentSettings_Read`/`Write` with the
`PERSISTENT_SETTING_*` offsets of
[`include/thandor/core/settings/persistent.h`](../include/thandor/core/settings/persistent.h)); each key of the
table in `persistent.cpp` maps to one offset. The self-test `OPEN_THANDOR_SELFTEST=settings` checks the format.

## Platform layer (SDL3)

The window, the event pump, keyboard and mouse, the periodic timers, the video presentation and the audio run on
SDL3 (`src/platform/sdl3`, interface
[`include/thandor/platform/sdl3/platform.h`](../include/thandor/platform/sdl3/platform.h)); the game reaches them
through the original's function slots (`g_Win32PumpMessages`, `g_TimerRegisterPeriodic`, `g_GraphicsSetDisplayMode`,
`g_GraphicsFramebufferPresent`, `g_Sound*`, `g_Pointer*`). The game always draws with its software renderer into a
memory framebuffer (RGB565 or XRGB8888) that is presented letterboxed through an SDL renderer, fullscreen on the
desktop (or in a window with the developer tools' `OPEN_THANDOR_WINDOWED=1`). The original's 3dfx Glide and
Direct3D renderers (and their `-GLIDE` and `-D3DALL` options) were removed. The display settings list one adapter,
"SDL", with the modes from 640x480 up to the desktop size in 16 and 32 bits; a `THANDOR.cfg` that still names an
adapter index past that list starts on adapter 0. The network code stays on WinSock (UDP).

### GPU rasterization (`THANDOR_RENDERER_SDL_GPU`)

With the CMake option `THANDOR_RENDERER_SDL_GPU=ON` (default `OFF`; preset `gpu-test`) the build can rasterize
the 3D view on the GPU through SDL_GPU (Direct3D 12) instead of the software rasterizer
(`src/platform/sdl3/gpu_renderer.cpp`). The software renderer stays the default; the GPU path is switched on at run
time with `OPEN_THANDOR_GPU=1` or the command-line option `-GPU`. Only the rasterization of the primitive queues
moves: lighting, fog, projection, sorting and the simulation stay on the CPU, the finished 3D view is copied back
into the framebuffer, and the overlays, the UI and the cursor are drawn on it as before. Each triangle is rebuilt
from the software rasterizer's own fixed-point setup, so the picture matches the software renderer apart from
rounding (blend tables, 16-bit quantization, single edge pixels). The shaders
(`src/platform/sdl3/shaders/primitives.hlsl`) are compiled to DXBC with `fxc` from the Windows SDK during the
build (a GCC build without `fxc` uses the committed headers, see [MinGW-w64 GCC](#mingw-w64-gcc)). Without a Direct3D 12 device the software renderer stays (logged in `thandor.log`).

With the developer tools, `OPEN_THANDOR_GPU=compare` runs both rasterizers on every frame, shows the software
picture and every `OPEN_THANDOR_GPU_COMPARE_MS` milliseconds (default 5000) writes the 3D view of both as
`shots\gpucmp_NNNN_sw.bmp` / `_gpu.bmp` with a difference image `_diff.bmp` and logs the difference; both modes log
the per-scene times every 10 seconds.

## Developer tools (`THANDOR_DEV_TOOLS`)

All test and debug aids of the port - the self-tests, the input scripts, automatic screenshots, the determinism
state hash, campaign starts and AUTOWIN, the level-script log, the movie player, export and dump, windowed mode,
a second instance, the UDP port and datagram log, the watchdog - are compiled in only with the CMake option
`THANDOR_DEV_TOOLS` (default `OFF`):

```bat
cmake -S . -B build-test -G Ninja -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_BUILD_TYPE=RelWithDebInfo ^
      -DTHANDOR_DEV_TOOLS=ON -DCMAKE_PREFIX_PATH=<vcpkg>/installed/x64-windows
cmake --build build-test
```

or the preset `test` (`cmake --preset test && cmake --build --preset test`). This test build is what
`tools/test` drives (`run_checks.py` takes `build-test/thandor.exe` and the `SDL3.dll` next to it). The tools live in `src/platform/debug` and
`src/platform/selftest`; the game reaches them only through the hooks in
[`include/thandor/platform/debug/hooks.h`](../include/thandor/platform/debug/hooks.h). With the option `OFF` those
sources are not compiled, every hook is a macro that expands to nothing (or passes the original value through),
no `OPEN_THANDOR_*` variable is read and the game runs the original code paths. The old option name
`THANDOR_TEST_AIDS=ON` is still accepted and switches `THANDOR_DEV_TOOLS` on.

In a build with the developer tools each tool is switched on at run time by its environment variable; without
the variable it does nothing:

| Variable | Effect |
|---|---|
| `OPEN_THANDOR_SELFTEST=codec\|movieenc\|numberformat\|fixedmath\|keymap\|trianglesetup\|path\|stretch\|scanaddr\|pcx\|settings\|crash` | run one self-test and exit (results in `thandor.log`; `settings` checks the `thandor.ini` reader and writer and that the `thandor.dat` of the current directory comes back unchanged through it; `pcx` decodes `pcxtest.pcx`, written with the expected result by `tools/test/pcx_check.py`; `codec`, `movieenc`, `numberformat`, `fixedmath`, `keymap` and `trianglesetup` log hashes of the save-game encoder, the FLM encoders/decoder, the number formatter, the fixed-point math, the keyboard layer and the triangle setup - compare them between two builds after touching those; `scanaddr` also reads `OPEN_THANDOR_SCANFILES` and `OPEN_THANDOR_DUMPTEXT`). The differential tests against the original machine code (`relaxcmp`, `stretchcmp`, `OPEN_THANDOR_MOVIECMP`) were removed with the 32-bit build |
| `OPEN_THANDOR_MOVIE=<name>\|all` | play `flm\<name>.flm`, or every name in `movies.txt`, max. 10 s each, with name and frame counter top left (`OPEN_THANDOR_MOVIE_START`, `_STRETCH`; `OPEN_THANDOR_MOVIEEXPORT=<name>[,...]` writes the frames to `moviedump\`); the player is in [`src/platform/debug/movie_player.cpp`](../src/platform/debug/movie_player.cpp) |
| `OPEN_THANDOR_MOVIEDUMP=1` | log every decoded movie frame (every tenth also to `moviedump\`) |
| `OPEN_THANDOR_AUTOSHOT=<ms>` | save the framebuffer every <ms> to `shots\shot_NNNN.bmp` (a failed capture is logged) |
| `OPEN_THANDOR_SCRIPT=<file>` | replay timed input (`<ms> click x y`, `rclick`, `move`, `key <vk>`, `keydown <vk>` / `keyup <vk>` for held keys such as Alt+P, `type <text>` types the rest of the line into a text field as the window procedure delivers it - space as VK_SPACE, letters and digits as key-down plus WM_CHAR (`Keyboard_OnChar`) -, `shot` saves the framebuffer now as `shots\script_NNNN.bmp`, `quit`); the real mouse is ignored meanwhile |
| `OPEN_THANDOR_STATEHASH=<steps>` | determinism test: state hash per simulation step to `statehash.txt` (`_SEED`, `_DETAIL`, `_PAUSE_AT`, `_SPEED`, `OPEN_THANDOR_ARENA_ORDERS`; see below and [`src/platform/debug/statehash.cpp`](../src/platform/debug/statehash.cpp)) |
| `OPEN_THANDOR_WINDOWED=1` | normal window instead of full screen (desktop colour depth, absolute mouse position, normal process priority); position with `OPEN_THANDOR_WINDOW_X` / `OPEN_THANDOR_WINDOW_Y` (default 0,0) |
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
game state after every step (test aid `OPEN_THANDOR_STATEHASH`, platform/debug/statehash.cpp) with the stored
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
| `include/thandor/<area>/<module>/types.h` | The game structures, UI template layouts and function pointer types of the module (once exported from the decompilation as one `generated/types.h`, split by `tools/dev/split_types.py`). The common ones (Bool8, fixed-point scalars, angles, vectors, ids) are in `include/thandor/core/types.h`. |
| `src/<area>/<module>/*.cpp` | The data of the original image (globals, tables, UI templates, strings) are ordinary C variables in the file that owns them ("Module data" section after the includes, vtables in a "Class vtables" section at the end), declared in that file's header. The original addresses are listed in `docs/original_addresses.txt`. |
| `include/thandor/generated/imports.h` | KERNEL32/USER32/... import prototypes (replaced by the SDK headers in the 64-bit step). |
| `include/thandor/core/x86_emulation.h` | What the original's x86 code does, in portable C: `THANDOR_CONTAINER_OF`, atomic exchange, x87 rounding, CPUID, the MMX lane operations. |

The decompilation this project started from (Ghidra project and exports, `ghidra/`) and the tools that read
it were removed from the tree after the code no longer needed them; they are in the git history.

## Data tools (`tools/data`)

| Script | Output |
|---|---|
| `fld.py`, `lev.py`, `pck.py`, `mdl2obj.py` | readers/converters for the game's file formats |
| `symbolize.py <crash_raw.log> <thandor.map>` | names for the raw crash dump |

## Documentation generator (`tools/docs`)

`python tools/docs/gen_docs.py` regenerates [MODULE_TREE.md](MODULE_TREE.md), [SOURCE_FILE_GUIDE.md](SOURCE_FILE_GUIDE.md)
and the file column of `original_addresses.txt` from the tree (file and function comments, includes, a caller scan
by name); `--check` only reports whether they are out of date. Run it after moving, splitting or renaming files.

## Known TODOs

Search for `TODO` in the tree. The main groups:

- **Multiplayer**: network command codes are distances between original handler addresses (see
  `include/thandor/network/protocol/commands.h`); received commands are resolved to the recovered C
  handlers through explicit command tables (`CommandDispatch_ResolveHandler`). Not yet tested in a real
  networked game.
