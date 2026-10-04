# Thandor: The Invasion

Got this game back then and can't forget it, so this project rebuilds it in C to fix things, replace the AI,
change the graphics API, fix sound and so on. I hope some of you want to join this effort too.

## Current state

**All of `thandor.exe` is reimplemented in readable C.** The build runs on its own: no original machine code is
executed and the original executable is not needed. Its data (tables, UI templates, strings) is compiled in as
ordinary C variables of the modules; only the game's data files (`*.PCK`, `thandor.dat`, movies) come from an
installation.

- 1,919 original functions in about 130,000 lines of C, 50 modules.
- Every function has a header comment (what it does, who calls it); names, constants and structure types are
  readable throughout, control flow is structured (no `goto`, no endless loops left from the decompilation).
- The code no longer refers to the original binary: the original addresses of all functions and data are kept in
  one list, [docs/original_addresses.txt](docs/original_addresses.txt), for comparisons with the original.
- Bugs and quirks of the original game are kept on purpose and marked "Original quirk" in the code.

### What works

| Area | Status |
|---|---|
| Single player: menus, campaigns, skirmish, AI, save/load, movies, sound | playable. An automated run starts all 56 missions (on "strong" and the highest game speed): 50 play without problems, 1 level file is missing from the original data, and the 5 levels that need the units carried over from the previous level play when reached through that level. A campaign run wins every level in turn and reaches the campaign end in all four campaigns (tutorial 3 levels, Luke 4, Nimm2 5, Hansolo 20 on the winning path); units are carried over into tutorial 2 and 3 and Hansolo 9, 13 and 23 |
| Graphics | software renderer presented through SDL3 ([details](docs/software_raster.md)), optionally rasterized on the GPU (SDL_GPU). The original's Glide (3dfx) and Direct3D renderers were removed |
| Platform | 64-bit (x64) only, on SDL3: window, input, timers, video presentation and audio; the original's DirectDraw, DirectInput, DirectSound and WinMM code was removed |
| Multiplayer (LAN, UDP) | works in a local two-instance test: lobby, map and faction choice, briefing, in-game commands; protocol-compatible with the original game |
| Map editor | hidden in the original; opened by a hotkey on the `experimental/map-editor` branch |

### Where it is going

The current step turns the reimplementation into a maintainable code base:

1. Clean-up: dead code, review findings, one switch for all developer tools (done).
2. Original addresses out of the code (in progress: the address list is done, comments are being rewritten).
3. Restructuring: code moves to the module it belongs to, large files are split, duplicates merged, the module
   `data.c` files and the big `types.h` dissolve into the modules' own headers.
4. Clang, then a stepwise port to C++ following the
   [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).
5. 64-bit only, with explicit file formats (save games, levels and the network protocol stay compatible) and a
   platform layer (64-bit only and the SDL3 platform layer are done).

After that the platform layer gets an SDL3 backend: first the software renderer's image shown through SDL, later a
hardware renderer on SDL_GPU.

### How correctness is kept

The project gave up byte-identical machine code in favour of readable code; behaviour is checked instead
([`tools/test/run_checks.py`](tools/test/run_checks.py), run after every change):

- **Determinism**: scripted battles with a fixed random seed write a hash of the game state every simulation step;
  every build must produce the same hash sequence as the reference.
- **AI hash, pixels, save/load, text editing**: AI decisions per step, screenshots of fixed scenes, a saved and
  reloaded game, the in-game text editor.
- **Multiplayer**: two instances play a game against each other over UDP.
- **Campaign and all missions**: the campaign chain with carried-over units and a start of every mission
  ([`run_all_maps.py`](tools/test/run_all_maps.py), [`run_campaign_chain.py`](tools/test/run_campaign_chain.py)).
- **Self-tests** for the number formatting, fixed-point math, key mapping, triangle setup, the archive codec and the
  movie encoder compare their output between builds.

Where the decompiled C differed from the original machine code it was fixed from the disassembly (for example the
water flow, double clicks, spinlocks that were not atomic).

## Building

Requirements: Windows, Visual Studio 2022 or newer with the C++ workload (MSVC x64 and Windows SDK), CMake 3.25+
and Ninja on the `PATH` (a "Developer Command Prompt" is not needed; [`cmake/msvc-x64.cmake`](cmake/msvc-x64.cmake)
finds the compiler via `vswhere`), and SDL3 for x64, e.g. `vcpkg install sdl3:x64-windows`. CMake finds SDL3 through
the environment variable `VCPKG_ROOT` (the vcpkg directory) or `-DCMAKE_PREFIX_PATH=<vcpkg>/installed/x64-windows`.

```bat
set VCPKG_ROOT=C:\path\to\vcpkg
cmake --preset release
cmake --build --preset release
```

The result is `cmake-build-msvc-release\thandor.exe` (x64) with `SDL3.dll` next to it. Other presets: `debug`,
`test` (`THANDOR_DEV_TOOLS=ON`, builds into `build-test` and adds the developer tools: self-tests, windowed mode,
several instances, scripted input, starting any campaign level, winning a level automatically, the determinism
state hash; the default build has none of them) and `gpu-test` (`test` plus the SDL_GPU rasterizer,
`THANDOR_RENDERER_SDL_GPU=ON`). CLion and Visual Studio pick the presets up from
[`CMakePresets.json`](CMakePresets.json).

To play, copy `thandor.exe` and `SDL3.dll` into a **copy** of an installed Thandor directory (the game data is not
part of this repository) and start it there, e.g. `thandor.exe -NOINTRO`. Developer tools, test switches and the data tools are
described in [docs/BUILDING.md](docs/BUILDING.md).

## Source tree

Public headers under [`include/thandor`](include/thandor), implementations under [`src`](src), grouped by area
(`assets`, `audio`, `core`, `gameplay`, `graphics`, `movie`, `network`, `platform`, `ui`, `world`).

- [Module tree](docs/MODULE_TREE.md) - every module with its `.c` and `.h` files.
- [Source file guide](docs/SOURCE_FILE_GUIDE.md) - what each source/header pair owns, its callers and dependencies.
- [Types](include/thandor/generated/types.h) - the game structures, still shared by all modules (to be split up).
- File formats: [levels](docs/level_format.md), [field grids](docs/field_grid_format.md).

## Contributors

- **idkFoxes** - reverse engineering of the game and the original decompilation this project grew from.
- **Crankerer** - compilable build, the C reimplementation and its verification
  (branch [`build/msvc-x86`](https://github.com/idkFoxes/open-thandor/tree/build/msvc-x86)).

Contributions are welcome.
