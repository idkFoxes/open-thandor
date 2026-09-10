# Thandor: The Invasion

Got this game back then and cant forget it, so I am also working on a decompilation to fix some things, replace AI, change graphics api, fix sound and so on.

Here I post and update my ghidra project. I hope some of you want to join this effort too.


# Thandor curated V523 full source tree

This tree is organized as a normal C project: public headers under `include/thandor`, implementations under `src`, parent headers that aggregate child modules, and per-leaf call graphs under `docs/callgraphs`.

## Scope

- Full V523 GZF symbol payload scanned for executable addresses.
- **2033 semantically named functions** are included, each exactly once.
- 38 deliberately opaque/no-op entries are omitted
- **104 leaf submodules** are used.
- Address comments use the function entry VA from the GZF; duplicate same-name records inside switch bodies are not treated as alternate function starts.

## Layout

`include/thandor/thandor.h` is the umbrella header. Parent modules such as `gameplay/ai.h`, `world/terrain.h`, `graphics/render.h`, and `ui/frontend.h` aggregate their child headers. `FUNCTION_INDEX.csv` is the complete address/module index. `docs/callgraphs` contains a Markdown and Graphviz graph for every leaf submodule.

The implementation bodies stay close to the current V523 decompiler output while the surrounding comments are rewritten for navigation: address, ownership, purpose, local calls, and cross-module calls.
