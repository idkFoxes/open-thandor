# Thandor: The Invasion

Got this game back then and can't forget it, so this project rebuilds it in C++ to fix things, replace the AI,
change the graphics API, fix sound and so on. I hope some of you want to join this effort too.

## Current state

**All of `thandor.exe` is reimplemented in readable C++ (C++20), as a 64-bit program on SDL3.** No original machine
code is executed and the original executable is not needed. Its data (tables, UI templates, strings) is compiled in
as ordinary variables of the modules; only the game's data files (`*.PCK`, movies) come from an
installation. The settings are kept in a readable `thandor.ini` (the original `thandor.dat` is taken over once,
see [docs/BUILDING.md](docs/BUILDING.md#settings-file-thandorini)). Save games of the original game load, and the game writes them in the original format.

- 1,919 original functions in about 160,000 lines (198 source files, 52 modules).
- Every function has a header comment (what it does, who calls it); names, constants and structure types are
  readable throughout, control flow is structured (no `goto`, no endless loops left from the decompilation).
- The code no longer refers to the original binary: the original addresses of all functions and data are kept in
  one list, [docs/original_addresses.txt](docs/original_addresses.txt), for comparisons with the original.
- Bugs and quirks of the original game are kept on purpose and marked "Original quirk" in the code.

### What works

| Area | Status |
|---|---|
| Single player: menus, campaigns, skirmish, AI, save/load, movies, sound | playable. An automated run starts all 56 missions (on "strong" and the highest game speed): 50 play without problems, 1 level file is missing from the original data, and the 5 levels that need the units carried over from the previous level play when reached through that level. A campaign run wins every level in turn and reaches the campaign end in all four campaigns (tutorial 3 levels, Luke 4, Nimm2 5, Hansolo 20 on the winning path); units are carried over into tutorial 2 and 3 and Hansolo 9, 13 and 23 |
| Graphics | renderer chosen in the display settings: Vulkan (default) or DirectX 12 through SDL_GPU (3D view rasterized on the GPU, frames presented through the same API), or the software renderer ([details](docs/software_raster.md)); window, borderless or exclusive fullscreen ([BUILDING](docs/BUILDING.md#renderer-and-display-mode-display-settings)). The original's Glide (3dfx) and Direct3D renderers were removed |
| Platform | 64-bit (x64) only, on SDL3: window, input, timers, video presentation and audio; the original's DirectDraw, DirectInput, DirectSound and WinMM code was removed |
| Multiplayer (LAN, UDP) | works in a local two-instance test: lobby, map and faction choice, briefing, in-game commands; protocol-compatible with the original game |
| Map editor | hidden in the original; opened by a hotkey on the `experimental/map-editor` branch |

### Where it is going

Done so far in the current step (turning the reimplementation into a maintainable code base): clean-up and one
switch for all developer tools, the original addresses out of the code, the module data next to its code, the
port to C++, 64-bit only (the original structure layouts are kept with 32-bit pointer fields, `Ptr32`, so save
games, levels and the network protocol stay compatible), the SDL3 platform layer, the GPU renderers on SDL_GPU
(Vulkan by default, DirectX 12, software as the reference; chosen in the game's display settings together with
window, borderless or exclusive fullscreen), settings in a readable `thandor.ini`, a second compiler (MinGW-w64 GCC
next to MSVC), every large multi-job file split into one file per job, duplicated code merged and the big `types.h`
split into the modules' own type headers.

Next:

1. Step 8 (in progress): a full code review and idiomatic C++ step by step, following the
   [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) ([plan](docs/plans/step8_idiomatic_cpp.md)).
2. Step 9 (later): the UI and the 2D overlays drawn on the GPU as well, the basis for UI scaling at 1440p and 4K
   ([plan](docs/plans/step9_gpu_ui.md)).
3. Step 10 (later): a patch installer (Inno Setup 7, classic style) that installs Open Thandor as version 1.0.6 onto an existing Thandor
   installation, in the style of the original Patch 5 installer ([plan](docs/plans/step10_installer.md)).

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

The main compiler is MinGW-w64 GCC (x86_64, SEH; tested with GCC 15.2): CMake 3.25+, Ninja, `g++` on the `PATH` or
`MINGW_ROOT` set ([`cmake/mingw-x64.cmake`](cmake/mingw-x64.cmake)), SDL3 with Vulkan from
`vcpkg install sdl3[vulkan]:x64-mingw-dynamic`, and dxc for the Vulkan shaders, `vcpkg install directx-dxc:x64-windows`.
CMake finds them through the environment variable `VCPKG_ROOT` (the vcpkg directory).

```bat
set PATH=C:\mingw64\bin;%PATH%
set VCPKG_ROOT=C:\path\to\vcpkg
cmake --preset mingw-release
cmake --build --preset mingw-release
```

The result is `build-mingw-release\thandor.exe` (x64) with `SDL3.dll` next to it. `mingw-test` (`build-mingw-test`)
adds the developer tools: self-tests, windowed mode, several instances, scripted input, starting any campaign level,
winning a level automatically, the determinism state hash (the default build has none of them); the automated checks
use this build.

MSVC (Visual Studio 2022 or newer with the C++ workload, SDL3 from `vcpkg install sdl3[vulkan]:x64-windows`) is the
second compiler, kept building for the Visual Studio debugger: presets `release`, `debug`, `test` (`build-test`) and
`gpu-test`; CLion and Visual Studio pick the presets up from [`CMakePresets.json`](CMakePresets.json). Details in
[docs/BUILDING.md](docs/BUILDING.md).

To play, copy `thandor.exe` and `SDL3.dll` into a **copy** of an installed Thandor directory (the game data is not
part of this repository) and start it there, e.g. `thandor.exe -NOINTRO`. Developer tools, test switches and the data tools are
described in [docs/BUILDING.md](docs/BUILDING.md).

## Source tree

Public headers under [`include/thandor`](include/thandor), implementations under [`src`](src), grouped by area
(`assets`, `audio`, `core`, `gameplay`, `graphics`, `movie`, `network`, `platform`, `ui`, `world`).

- [Module tree](docs/MODULE_TREE.md) - every module with its `.cpp` and `.h` files.
- [Source file guide](docs/SOURCE_FILE_GUIDE.md) - what each source/header pair owns, its callers and dependencies.
- Types: each module's structures are in `include/thandor/<area>/<module>/types.h`, the common ones in [core/types.h](include/thandor/core/types.h).
- File formats: [levels](docs/level_format.md), [field grids](docs/field_grid_format.md).

## Contributors

- **idkFoxes** - reverse engineering of the game and the original decompilation this project grew from.
- **Crankerer** - compilable build, the C/C++ reimplementation and its verification
  (branch [`build/msvc-x86`](https://github.com/idkFoxes/open-thandor/tree/build/msvc-x86)).

Contributions are welcome.
