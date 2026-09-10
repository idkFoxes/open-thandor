# Thandor: The Invasion

Got this game back then and cant forget it, so I am also working on a decompilation to fix some things, replace AI, change graphics api, fix sound and so on.

Here I post and update my ghidra project. I hope some of you want to join this effort too.

# Thandor source tree (based on /ghidra/thandor.exeV523.gzf)

This tree is organized as a normal C project: public headers under [`include/thandor`](include/thandor), implementations under [`src`](src), parent headers that aggregate child modules, and per-leaf call graphs under [`docs/callgraphs`](docs/callgraphs).

## Navigation

- [Module tree](docs/MODULE_TREE.md) — every leaf module with direct `.c`, `.h`, and call-graph links.
- [Source file guide](docs/SOURCE_FILE_GUIDE.md) — what each source/header pair owns, plus direct callers and outgoing module dependencies.
- [Umbrella header](include/thandor/thandor.h) — top-level public include.
- [Shared curated contracts](include/thandor/core/contracts.h) — compact high-value recovered contracts.
- [Full V523 type layer](include/thandor/generated/v523_types.h) — generated type declarations used by the split modules.

## Scope

- **2033 semantically named functions** are included.
- 38 deliberately opaque/no-op entries are omitted from the curated API.
- **104 leaf submodules** are used.
- Address comments use the function entry VA from the GZF; duplicate same-name records inside switch bodies are not treated as alternate function starts.

## Layout

[`include/thandor/thandor.h`](include/thandor/thandor.h) is the umbrella header. Parent modules such as [`gameplay/ai.h`](include/thandor/gameplay/ai.h), [`world/terrain.h`](include/thandor/world/terrain.h), [`graphics/render.h`](include/thandor/graphics/render.h), and [`ui/frontend.h`](include/thandor/ui/frontend.h) aggregate their child headers.

Implementation bodies stay close to the current V523 decompiler output while surrounding comments are organized for navigation: address, ownership, purpose, local calls, and cross-module calls. Start with the [module tree](docs/MODULE_TREE.md) for ownership or the [source file guide](docs/SOURCE_FILE_GUIDE.md) when tracing callers.
