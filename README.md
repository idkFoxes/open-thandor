# Thandor: The Invasion

Got this game back then and can't forget it, so this project rebuilds it in C to fix things, replace the AI,
change the graphics API, fix sound and so on. I hope some of you want to join this effort too.

## Current state

**All of `thandor.exe` is reimplemented in C.** The build runs on its own: no original machine code is executed
and the original executable is not needed. Its data (tables, UI templates, strings) is compiled in as ordinary C
variables of the modules (`src/<area>/<module>/data.c`); only the game's data files (`*.PCK`, `thandor.dat`,
movies) come from an installation.

### Progress

| | | |
|---|---|---|
| Functions reimplemented in C | `████████████████████` | **100 %** (2,068 / 2,068) |
| Original data compiled in as C variables (verified byte for byte against the original when it was converted) | `████████████████████` | **100 %** (885,420 bytes, 2,393 pointers) |
| Original data typed and named (bytes with content) | `████████████████████` | **99.9 %** (171,500 / 171,692) |
| Functions with a header comment (what, why, who calls it) | `████████████████████` | **100 %** |
| Functions without raw memory offsets | `████████████████████` | **100 %** |
| Functions without placeholder names | `████████████████████` | **100 %** |
| Functions without `goto` or an endless loop (`while (true)`, `for (;;)`) | `███████████████████░` | **95.2 %** (100 left, 39 of them only a shared failure exit) |
| Functions that pass all readability checks | `███████████████████░` | **95.2 %** (1,968) |
| Magic hex numbers named (constants, fixed-point shifts) | `████████████████████` | **99.9 %** (5,182 → 2 left) |

The readability numbers come from [`tools/readability_report.py`](tools/readability_report.py). What it counts:
a placeholder is an identifier such as `reserved28_2F`, `unknown…` or `arg0` (the graphics term "opaque" and
string symbols named after their text are not); a raw offset is `*(T *)(p + 0x..)` or `(int)&x` address arithmetic. Hex numbers are counted in code only,
without original addresses, the values of `#define`s (that is where they get their name), bit masks (one run of
four or more set bits such as `0xff`, `0x3fffffff`, `0xffff0000`: they read best as hex; 331 of them) and the
sample codec's MMX tables, whose offsets are genuine table positions. The 2 left are occupancy and production
bits whose meaning is not known yet. Control flow was restructured byte-identically too (611 -> 362 gotos and
endless loops): what is left mostly keeps one shared failure exit or jumps into shared blocks, and MSVC only emits
the original code for that form; changing it is the next stage, checked by behaviour tests. The data share was
measured with the report of the former data generator (removed with it): of the bytes that hold content (not
zero storage, not the tables computed at startup), the part written with a real type and named fields, as text, as
a UI template or as a jump table; the rest are code fragments between data and 16 bytes nobody uses.

### What works

| Area | Status |
|---|---|
| Single player: menus, campaigns, skirmish, AI, save/load, movies, sound | playable. An automated run starts all 56 missions (on "strong" and the highest game speed): 50 play without problems, 1 level file is missing from the original data, and the 5 levels that need the units carried over from the previous level play when reached through that level. A campaign run wins every level in turn and reaches the campaign end in all four campaigns (tutorial 3 levels, Luke 4, Nimm2 5, Hansolo 20 on the winning path); units are carried over into tutorial 2 and 3 and Hansolo 9, 13 and 23 |
| Software renderer, DirectDraw | working; software rasterizer and blitters verified bit-exact against the original code ([details](docs/software_raster.md)). The original's Glide (3dfx) and Direct3D renderers were removed: the software renderer is the only renderer |
| Multiplayer (LAN, UDP) | works in a local two-instance test: lobby, map and faction choice, briefing, in-game commands |
| Map editor | hidden in the original; opened by a hotkey on the `experimental/map-editor` branch |

### How correctness is kept

- **Behaviour parity with the original.** Where the decompiled C differed from the original machine code, it was
  fixed from the disassembly (for example the water flow, double clicks, spinlocks
  that were not atomic). Bugs and quirks of the original game are kept and marked "Original quirk" in the code.
- **Differential self-tests** run parts of the C code and the original machine code on the same inputs:
  bilinear stretching, water simulation, movie decoder (they need `thandor_original.exe` next to the exe). The
  rasterizer, blitter and blend-scaling compares needed the original image mapped at its address and were
  retired with it, after they had confirmed those functions.
- **Byte-identical refactoring.** Every readability pass so far (names, constants, comments, typed structs and
  parameter types, fixed-point helpers such as `FIXED_MUL_SHR`, structured loops instead of goto) was checked
  function by function: the generated machine code of our build did not change. Restructuring that
  changes code (loops, splitting large functions, helpers) is the next stage and needs behaviour tests instead.
- **Game runs**: a scripted skirmish after every change, plus the all-missions run
  ([`run_all_maps.py`](tools/test/run_all_maps.py)) and the campaign run with carried-over units
  ([`run_campaign_chain.py`](tools/test/run_campaign_chain.py)) for larger changes.

## Building

Requirements: Windows, Visual Studio 2022 or newer with the C++ workload (MSVC x86 and Windows SDK), CMake 3.25+
and Ninja on the `PATH` (a "Developer Command Prompt" is not needed; [`cmake/msvc-x86.cmake`](cmake/msvc-x86.cmake)
finds the compiler via `vswhere`).

```bat
cmake --preset release
cmake --build --preset release
```

The result is `cmake-build-msvc-release\thandor.exe` (32-bit). Other presets: `debug` and `test` (adds test aids:
windowed mode, several instances, scripted input, starting any campaign level, winning a level automatically).
The optional differential self-tests want `thandor_original.exe` next to the exe.
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
