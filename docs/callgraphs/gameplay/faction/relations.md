# Call graph: `gameplay/faction/relations`

12 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0053C010` **GameFactionRelations_UpdateAllPairsForFaction** — local: `GameFactionRelations_TestPairTransitionAllowedCf`, `GameFactionRelations_IsResetEligibleStateCf`, `GameFactionRelations_MaybeResetPairState`, `GameFactionRelations_MaybeAdvancePairStateRare`, `GameFactionRelations_MaybeAdvancePairStateCommon`
- `0x00560E30` **PlayerPairList_InsertRange** — local: `PlayerPairList_InsertUnique`
- `0x00560E70` **PlayerPairList_RemoveRange** — local: `PlayerPairList_RemoveFirstMatch`
- `0x0053C3D0` **GameFactionRelations_TestPairTransitionAllowedCf** — local: `GameFactionRelations_BuildEligibleFactionMask`, `GameFactionRelations_EvaluateTransitionRulesCf`; cross: `GameFactionRuntime_GetPackedStateNibble` → `gameplay/faction/runtime`
- `0x0053C090` **GameFactionRelations_BuildEligibleFactionMask** — cross: `GameFactionRuntime_GetPackedStateNibble` → `gameplay/faction/runtime`
- `0x0053C0F0` **GameFactionRelations_EvaluateTransitionRulesCf**
- `0x0053C490` **GameFactionRelations_IsResetEligibleStateCf** — cross: `GameFactionRuntime_GetPackedStateNibble` → `gameplay/faction/runtime`
- `0x0053C4D0` **GameFactionRelations_MaybeAdvancePairStateRare** — cross: `GameFactionRuntime_AdvancePairwiseRelationState` → `gameplay/faction/runtime`
- `0x0053C540` **GameFactionRelations_MaybeAdvancePairStateCommon** — cross: `GameFactionRuntime_AdvancePairwiseRelationState` → `gameplay/faction/runtime`
- `0x0053C5B0` **GameFactionRelations_MaybeResetPairState** — cross: `GameFactionRuntime_ResetPairwiseRelationState` → `gameplay/faction/runtime`
- `0x00560EB0` **PlayerPairList_InsertUnique**
- `0x00560F50` **PlayerPairList_RemoveFirstMatch**
