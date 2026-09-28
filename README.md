# Thandor: The Invasion

Got this game back then and cant forget it, so I am also working on a decompilation to fix some things, replace AI, change graphics api, fix sound and so on.

Here I post and update my Ghidra project. I hope some of you want to join this effort too.

## Current state

**All of `thandor.exe` is reimplemented in C.** The build runs on its own: no original machine code is executed and the original executable is not needed; its data (tables, UI templates, strings) is compiled in from [`src/generated/image_data.c`](src/generated/image_data.c). Only the game's data files (`*.PCK`, `thandor.dat`, movies) come from an installation.

| Area | Status |
|---|---|
| Single player (menus, campaign, skirmish, AI, save/load, movies, sound) | playable; the automated test plays a skirmish |
| Software renderer, DirectDraw/Direct3D | working; the software rasterizer and blitters are verified bit-exact against the original code ([details](docs/software_raster.md)) |
| Glide (3dfx) | builds, but vertex output is known wrong |
| Multiplayer | known broken (command dispatch), not worked on yet |

**Readability.** The code was made readable top-down from `WinMain`, in two stages: stage 1 only renames, adds named constants and comments, and must leave the generated machine code byte-identical (checked per function against the previous build); stage 2 (restructuring: helpers, loops, typed structs instead of raw offsets) comes later. **Stage 1 is done for all ~2,200 functions**: every function has a header saying what it does and which caller, table slot or command code reaches it. Many bodies still carry Ghidra structure (pointer temporaries, raw offsets), which is the stage-2 work.

**How correctness is checked.** Differential self-tests run parts of the C code and the original machine code on the same random inputs (rasterizer, blitters, water simulation, movie decoder); `imagecmp` checks the compiled-in data against the original image; a scripted skirmish runs after every change. Places where the decompiled C turned out to differ from the original were fixed from the disassembly (for example the water flow, double clicks, the Direct3D texture binding, spinlocks that were not atomic); quirks and bugs of the original game are kept and only commented.

### Ghidra checkpoint

The public source tree is currently based on **V537**.

- [V537 Ghidra project](ghidra/thandor.exeV537.gzf) — current `.gzf` checkpoint.
- [V537 raw decompiler export](ghidra/thandor.exeV537.c) — unsplit decompiler C used as the current source authority.
- [Developer changelog](CHANGELOG.md) — short, impact-sorted notes for actionable gameplay/runtime changes; decompiler cleanup is summarized once per represented submodule.
- [Full V523 → V537 recovery changelog](CHANGELOG_FULL.md) — complete recovery record across all 104 submodules.

## Building

Requirements: Windows, Visual Studio 2022 or newer with the C++ workload (MSVC x86 and Windows SDK), CMake 3.25+ and Ninja on the `PATH` (a "Developer Command Prompt" is not needed; [`cmake/msvc-x86.cmake`](cmake/msvc-x86.cmake) finds the compiler via `vswhere`).

```bat
cmake --preset release
cmake --build --preset release
```

The result is `cmake-build-msvc-release\thandor.exe` (32-bit). Other presets: `debug`, and `mapped` (maps the original image; only needed for the differential self-tests, which also want `thandor_original.exe` next to the exe). CLion and Visual Studio pick the presets up from [`CMakePresets.json`](CMakePresets.json).

To play, copy `thandor.exe` into a **copy** of an installed Thandor directory (the game data is not part of this repository) and start it there, e.g. `thandor.exe -NOINTRO`. Test switches, self-tests and the data tools are described in [docs/BUILDING.md](docs/BUILDING.md).

## Source tree

This tree is organized as a normal C project: public headers under [`include/thandor`](include/thandor), implementations under [`src`](src), parent headers that aggregate child modules, and per-leaf call graphs under [`docs/callgraphs`](docs/callgraphs).

### Navigation

- [Module tree](docs/MODULE_TREE.md) — every leaf module with direct `.c`, `.h`, call-graph, developer-note, and full-changelog links where applicable.
- [Source file guide](docs/SOURCE_FILE_GUIDE.md) — what each source/header pair owns, plus direct callers and outgoing module dependencies.
- [Umbrella header](include/thandor/thandor.h) — top-level public include.
- [Shared contracts](include/thandor/core/contracts.h) — common scalar contracts used by the split tree.
- [Generated types](include/thandor/generated/types.h) — the single cumulative recovered type/ABI authority. It is updated in place so Git shows type and structure recovery as a normal file diff instead of per-version delta headers.

## Scope

- **2,068 semantically named functions** are included.
- **104 leaf submodules** are used.
- 2 proven unreferenced no-op owners are deliberately omitted from the public split API.
- Address comments use the function entry VA from the current [V537 Ghidra project](ghidra/thandor.exeV537.gzf); duplicate same-name records inside switch bodies are not treated as alternate function starts.

## Layout

[`include/thandor/thandor.h`](include/thandor/thandor.h) is the umbrella header. Parent modules such as [`gameplay/ai.h`](include/thandor/gameplay/ai.h), [`world/terrain.h`](include/thandor/world/terrain.h), [`graphics/render.h`](include/thandor/graphics/render.h), and [`ui/frontend.h`](include/thandor/ui/frontend.h) aggregate their child headers.

Implementation bodies stay close to the current [V537 decompiler export](ghidra/thandor.exeV537.c) while surrounding comments are organized for navigation: address, ownership, purpose, local calls, and cross-module calls. Start with the [module tree](docs/MODULE_TREE.md) for ownership or the [source file guide](docs/SOURCE_FILE_GUIDE.md) when tracing callers.
