# Call graph: `world/pathing/influence`

[Source](../../../../src/world/pathing/influence.c) · [Header](../../../../include/thandor/world/pathing/influence.h) · [Graphviz](influence.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-world-pathing-influence) · [Full changelog](../../../../CHANGELOG_FULL.md#module-world-pathing-influence)

19 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00527330` **[`GridInfluence_AddLowDistanceBands`](../../../../src/world/pathing/influence.c#L5)** — local: [`GridInfluence_SetLowDistanceBandsAroundWorldPoint`](../../../../src/world/pathing/influence.c#L185)
- `0x00527380` **[`GridInfluence_RemoveLowDistanceBands`](../../../../src/world/pathing/influence.c#L31)** — local: [`GridInfluence_ClearLowDistanceBandsAroundWorldPoint`](../../../../src/world/pathing/influence.c#L461)
- `0x00528070` **[`GridInfluence_AddHighDistanceBands`](../../../../src/world/pathing/influence.c#L57)** — local: [`GridInfluence_SetHighDistanceBandsAroundWorldPoint`](../../../../src/world/pathing/influence.c#L323)
- `0x005280D0` **[`GridInfluence_RemoveHighDistanceBands`](../../../../src/world/pathing/influence.c#L90)** — local: [`GridInfluence_ClearHighDistanceBandsAroundWorldPoint`](../../../../src/world/pathing/influence.c#L600)
- `0x00527B50` **[`GridInfluence_AddNoOp`](../../../../src/world/pathing/influence.c#L116)**
- `0x00527B60` **[`GridInfluence_RemoveNoOp`](../../../../src/world/pathing/influence.c#L127)**
- `0x00535A30` **[`GridInfluence_ClearDistanceBandsAndRefreshEntities`](../../../../src/world/pathing/influence.c#L138)**
- `0x00535330` **[`GridInfluence_SetLowDistanceBandsAroundWorldPoint`](../../../../src/world/pathing/influence.c#L185)** — local: [`GridInfluence_SetLowDistanceBandsDiagonalPositive`](../../../../src/world/pathing/influence.c#L791), [`GridInfluence_SetLowDistanceBandsDiagonalNegative`](../../../../src/world/pathing/influence.c#L739)
- `0x00535780` **[`GridInfluence_SetHighDistanceBandsAroundWorldPoint`](../../../../src/world/pathing/influence.c#L323)** — local: [`GridInfluence_SetHighDistanceBandsDiagonalPositive`](../../../../src/world/pathing/influence.c#L897), [`GridInfluence_SetHighDistanceBandsDiagonalNegative`](../../../../src/world/pathing/influence.c#L844)
- `0x00535CC0` **[`GridInfluence_ClearLowDistanceBandsAroundWorldPoint`](../../../../src/world/pathing/influence.c#L461)** — local: [`GridInfluence_ClearLowDistanceBandsDiagonalPositive`](../../../../src/world/pathing/influence.c#L1003), [`GridInfluence_ClearLowDistanceBandsDiagonalNegative`](../../../../src/world/pathing/influence.c#L950)
- `0x00536110` **[`GridInfluence_ClearHighDistanceBandsAroundWorldPoint`](../../../../src/world/pathing/influence.c#L600)** — local: [`GridInfluence_ClearHighDistanceBandsDiagonalPositive`](../../../../src/world/pathing/influence.c#L1109), [`GridInfluence_ClearHighDistanceBandsDiagonalNegative`](../../../../src/world/pathing/influence.c#L1056)
- `0x00535190` **[`GridInfluence_SetLowDistanceBandsDiagonalNegative`](../../../../src/world/pathing/influence.c#L739)**
- `0x00535260` **[`GridInfluence_SetLowDistanceBandsDiagonalPositive`](../../../../src/world/pathing/influence.c#L791)**
- `0x005355E0` **[`GridInfluence_SetHighDistanceBandsDiagonalNegative`](../../../../src/world/pathing/influence.c#L844)**
- `0x005356B0` **[`GridInfluence_SetHighDistanceBandsDiagonalPositive`](../../../../src/world/pathing/influence.c#L897)**
- `0x00535B20` **[`GridInfluence_ClearLowDistanceBandsDiagonalNegative`](../../../../src/world/pathing/influence.c#L950)**
- `0x00535BF0` **[`GridInfluence_ClearLowDistanceBandsDiagonalPositive`](../../../../src/world/pathing/influence.c#L1003)**
- `0x00535F70` **[`GridInfluence_ClearHighDistanceBandsDiagonalNegative`](../../../../src/world/pathing/influence.c#L1056)**
- `0x00536040` **[`GridInfluence_ClearHighDistanceBandsDiagonalPositive`](../../../../src/world/pathing/influence.c#L1109)**

## Called by

- [`gameplay/session/runtime`](../../gameplay/session/runtime.md): [`InGameRuntime_InitializeNewSession`](../../../../src/gameplay/session/runtime.c#L718) → [`GridInfluence_ClearDistanceBandsAndRefreshEntities`](../../../../src/world/pathing/influence.c#L138); [`InGameRuntime_InitializeLoadedSession`](../../../../src/gameplay/session/runtime.c#L1085) → [`GridInfluence_ClearDistanceBandsAndRefreshEntities`](../../../../src/world/pathing/influence.c#L138); [`InGameRuntime_UpdateSimulationAndNetworkTick`](../../../../src/gameplay/session/runtime.c#L2235) → [`GridInfluence_ClearDistanceBandsAndRefreshEntities`](../../../../src/world/pathing/influence.c#L138)
- [`world/pathing/grid`](grid.md): [`EntityPathing_RebuildOverlappingGroupRoutes`](../../../../src/world/pathing/grid.c#L844) → [`GridInfluence_SetLowDistanceBandsAroundWorldPoint`](../../../../src/world/pathing/influence.c#L185); [`EntityPathing_UpdateRouteSegment`](../../../../src/world/pathing/grid.c#L1112) → [`GridInfluence_SetLowDistanceBandsAroundWorldPoint`](../../../../src/world/pathing/influence.c#L185)
