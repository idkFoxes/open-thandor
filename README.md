# Thandor: The Invasion

Got this game back then and can't forget it, so this project rebuilds it in C to fix things, replace the AI,
change the graphics API, fix sound and so on. I hope some of you want to join this effort too.

## Current state

**All of `thandor.exe` is reimplemented in C.** The build runs on its own: no original machine code is executed
and the original executable is not needed. Its data (tables, UI templates, strings) is compiled in from
[`src/generated/image_data.c`](src/generated/image_data.c); only the game's data files (`*.PCK`, `thandor.dat`,
movies) come from an installation.

### Progress

| | | |
|---|---|---|
| Functions reimplemented in C | `████████████████████` | **100 %** (2,068 / 2,068) |
| Original data compiled in, verified byte for byte | `████████████████████` | **100 %** (885,420 bytes, 2,393 pointers) |
| Functions with a header comment (what, why, who calls it) | `████████████████████` | **100 %** |
| Functions without raw memory offsets | `████████████████████` | **99.0 %** |
| Functions without placeholder names | `███████████████████░` | **94.9 %** |
| Functions without `goto` / `while (true)` | `██████████████████░░` | **91.7 %** |
| Functions that pass all readability checks | `█████████████████░░░` | **86.7 %** (1,793) |
| Magic hex numbers named (constants, fixed-point shifts, masks) | `████████████████░░░░` | **77 %** (5,513 → 1,280 left) |

The readability numbers come from [`tools/readability_report.py`](tools/readability_report.py). The hex numbers
left are mostly bit patterns (MMX lane and colour masks), packed text codes and flags whose meaning is not known
yet; original addresses and the sample codec's MMX tables are not counted.

### What works

| Area | Status |
|---|---|
| Single player: menus, campaigns, skirmish, AI, save/load, movies, sound | playable. An automated run starts all 56 missions (on "strong" and the highest game speed); 50 play without problems, 5 campaign levels need the units carried over from the previous level, 1 level file is missing from the original data |
| Software renderer, DirectDraw, Direct3D | working; software rasterizer and blitters verified bit-exact against the original code ([details](docs/software_raster.md)) |
| Glide (3dfx) | builds; the vertex output was fixed from the original code, not tested on 3dfx hardware |
| Multiplayer (LAN, UDP) | works in a local two-instance test: lobby, map and faction choice, briefing, in-game commands |
| Map editor | hidden in the original; opened by a hotkey on the `experimental/map-editor` branch |

### How correctness is kept

- **Behaviour parity with the original.** Where the decompiled C differed from the original machine code, it was
  fixed from the disassembly (for example the water flow, double clicks, the Direct3D texture binding, spinlocks
  that were not atomic). Bugs and quirks of the original game are kept and marked "Original quirk" in the code.
- **Differential self-tests** run parts of the C code and the original machine code on the same inputs:
  rasterizer, blitters, bilinear scaling, water simulation, movie decoder. `imagecmp` checks the compiled-in data
  against the original executable.
- **Byte-identical refactoring.** Every readability pass so far (names, constants, comments, typed structs and
  parameter types, fixed-point helpers such as `FIXED_MUL_SHR`) was checked function by function: the generated
  machine code of our build did not change. Restructuring that
  changes code (loops, splitting large functions, helpers) is the next stage and needs behaviour tests instead.
- **Game runs**: a scripted skirmish after every change, plus the all-missions run for larger changes.

## Building

Requirements: Windows, Visual Studio 2022 or newer with the C++ workload (MSVC x86 and Windows SDK), CMake 3.25+
and Ninja on the `PATH` (a "Developer Command Prompt" is not needed; [`cmake/msvc-x86.cmake`](cmake/msvc-x86.cmake)
finds the compiler via `vswhere`).

```bat
cmake --preset release
cmake --build --preset release
```

The result is `cmake-build-msvc-release\thandor.exe` (32-bit). Other presets: `debug`, `test` (adds test aids:
windowed mode, several instances, scripted input, starting any campaign level) and `mapped` (maps the original
image; only needed for the differential self-tests, which also want `thandor_original.exe` next to the exe).
CLion and Visual Studio pick the presets up from [`CMakePresets.json`](CMakePresets.json).

To play, copy `thandor.exe` into a **copy** of an installed Thandor directory (the game data is not part of this
repository) and start it there, e.g. `thandor.exe -NOINTRO`. Test switches, self-tests and the data tools are
described in [docs/BUILDING.md](docs/BUILDING.md).

## Source tree

A normal C project: public headers under [`include/thandor`](include/thandor), implementations under
[`src`](src), parent headers that aggregate child modules, and per-module call graphs under
[`docs/callgraphs`](docs/callgraphs).

- [Module tree](docs/MODULE_TREE.md) - every module with its `.c`, `.h` and call graph.
- [Source file guide](docs/SOURCE_FILE_GUIDE.md) - what each source/header pair owns, its callers and dependencies.
- [Umbrella header](include/thandor/thandor.h) - top-level include; parent headers such as
  [`gameplay/ai.h`](include/thandor/gameplay/ai.h) or [`world/terrain.h`](include/thandor/world/terrain.h)
  aggregate their child headers.
- [Types](include/thandor/generated/types.h) - the recovered game structures, shared by all modules.

Every function starts with a comment giving its original address, what it does and who calls it. 2,068 functions
in 104 modules.

## Contributors

- **idkFoxes** - reverse engineering of the game and the original decompilation this project grew from.
- **Crankerer** - compilable build, the C reimplementation and its verification
  (branch [`build/msvc-x86`](https://github.com/idkFoxes/open-thandor/tree/build/msvc-x86)).

Contributions are welcome.
