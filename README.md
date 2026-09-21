# Thandor: The Invasion

Got this game back then and cant forget it, so I am also working on a decompilation to fix some things, replace AI, change graphics api, fix sound and so on.

Here I post and update my Ghidra project. I hope some of you want to join this effort too.

## Current reverse-engineering checkpoint

The public source tree is currently based on **V537**.

- [V537 Ghidra project](ghidra/thandor.exeV537.gzf) — current `.gzf` checkpoint.
- [V537 raw decompiler export](ghidra/thandor.exeV537.c) — unsplit decompiler C used as the current source authority.
- [Developer changelog](CHANGELOG.md) — short, impact-sorted notes for actionable gameplay/runtime changes; decompiler cleanup is summarized once per represented submodule.
- [Full V523 → V537 recovery changelog](CHANGELOG_FULL.md) — complete recovery record across all 104 submodules.

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
