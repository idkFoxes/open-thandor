# Call graph: `gameplay/faction/relations`

[Source](../../../../src/gameplay/faction/relations.c) · [Header](../../../../include/thandor/gameplay/faction/relations.h) · [Graphviz](relations.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-gameplay-faction-relations) · [Developer notes](../../../../CHANGELOG.md#module-gameplay-faction-relations) · [Full changelog](../../../../CHANGELOG_FULL.md#module-gameplay-faction-relations)

12 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0053C010` **[`GameFactionRelations_UpdateAllPairsForFaction`](../../../../src/gameplay/faction/relations.c#L5)** — local: [`GameFactionRelations_TestPairTransitionAllowedCf`](../../../../src/gameplay/faction/relations.c#L128), [`GameFactionRelations_IsResetEligibleStateCf`](../../../../src/gameplay/faction/relations.c#L337), [`GameFactionRelations_MaybeResetPairState`](../../../../src/gameplay/faction/relations.c#L437), [`GameFactionRelations_MaybeAdvancePairStateRare`](../../../../src/gameplay/faction/relations.c#L359), [`GameFactionRelations_MaybeAdvancePairStateCommon`](../../../../src/gameplay/faction/relations.c#L398)
- `0x00560E30` **[`PlayerPairList_InsertRange`](../../../../src/gameplay/faction/relations.c#L71)** — local: [`PlayerPairList_InsertUnique`](../../../../src/gameplay/faction/relations.c#L460)
- `0x00560E70` **[`PlayerPairList_RemoveRange`](../../../../src/gameplay/faction/relations.c#L99)** — local: [`PlayerPairList_RemoveFirstMatch`](../../../../src/gameplay/faction/relations.c#L504)
- `0x0053C3D0` **[`GameFactionRelations_TestPairTransitionAllowedCf`](../../../../src/gameplay/faction/relations.c#L128)** — local: [`GameFactionRelations_BuildEligibleFactionMask`](../../../../src/gameplay/faction/relations.c#L167), [`GameFactionRelations_EvaluateTransitionRulesCf`](../../../../src/gameplay/faction/relations.c#L209); cross: [`GameFactionRuntime_GetPackedStateNibble`](../../../../src/gameplay/faction/runtime.c#L353) → [`gameplay/faction/runtime`](runtime.md)
- `0x0053C090` **[`GameFactionRelations_BuildEligibleFactionMask`](../../../../src/gameplay/faction/relations.c#L167)** — cross: [`GameFactionRuntime_GetPackedStateNibble`](../../../../src/gameplay/faction/runtime.c#L353) → [`gameplay/faction/runtime`](runtime.md)
- `0x0053C0F0` **[`GameFactionRelations_EvaluateTransitionRulesCf`](../../../../src/gameplay/faction/relations.c#L209)**
- `0x0053C490` **[`GameFactionRelations_IsResetEligibleStateCf`](../../../../src/gameplay/faction/relations.c#L337)** — cross: [`GameFactionRuntime_GetPackedStateNibble`](../../../../src/gameplay/faction/runtime.c#L353) → [`gameplay/faction/runtime`](runtime.md)
- `0x0053C4D0` **[`GameFactionRelations_MaybeAdvancePairStateRare`](../../../../src/gameplay/faction/relations.c#L359)** — cross: [`GameFactionRuntime_AdvancePairwiseRelationState`](../../../../src/gameplay/faction/runtime.c#L5) → [`gameplay/faction/runtime`](runtime.md)
- `0x0053C540` **[`GameFactionRelations_MaybeAdvancePairStateCommon`](../../../../src/gameplay/faction/relations.c#L398)** — cross: [`GameFactionRuntime_AdvancePairwiseRelationState`](../../../../src/gameplay/faction/runtime.c#L5) → [`gameplay/faction/runtime`](runtime.md)
- `0x0053C5B0` **[`GameFactionRelations_MaybeResetPairState`](../../../../src/gameplay/faction/relations.c#L437)** — cross: [`GameFactionRuntime_ResetPairwiseRelationState`](../../../../src/gameplay/faction/runtime.c#L66) → [`gameplay/faction/runtime`](runtime.md)
- `0x00560EB0` **[`PlayerPairList_InsertUnique`](../../../../src/gameplay/faction/relations.c#L460)**
- `0x00560F50` **[`PlayerPairList_RemoveFirstMatch`](../../../../src/gameplay/faction/relations.c#L504)**

## Called by

- [`gameplay/ai/planning`](../ai/planning.md): [`AiRuntime_DispatchFactionPlanningPhase`](../../../../src/gameplay/ai/planning.c#L300) → [`GameFactionRelations_UpdateAllPairsForFaction`](../../../../src/gameplay/faction/relations.c#L5)
- [`ui/ingame/runtime`](../../ui/ingame/runtime.md): [`InGameUiCommand_UpdateInteractionByMode`](../../../../src/ui/ingame/runtime.c#L4348) → [`PlayerPairList_RemoveRange`](../../../../src/gameplay/faction/relations.c#L99); [`InGameUiCommand_UpdateInteractionByMode`](../../../../src/ui/ingame/runtime.c#L4348) → [`PlayerPairList_InsertRange`](../../../../src/gameplay/faction/relations.c#L71)
