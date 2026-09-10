# Call graph: `gameplay/faction/runtime`

28 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0055F790` **GameFactionRuntime_AdvancePairwiseRelationState** — local: `GameFactionRuntime_IsRecentTimedRelationStateCf`, `GameFactionRuntime_ApplyPairwiseRelationTransition`
- `0x0055F910` **GameFactionRuntime_ResetPairwiseRelationState** — local: `GameFactionRuntime_ApplyPairwiseRelationTransition`
- `0x00565320` **OldUnitRuntime_RebuildScenarioReplayTables** — local: `OldUnitRuntime_ResetPendingTables`; cross: `FixedMath_Length2` → `core/math/fixed`
- `0x005130B0` **GameFactionRuntime_RebaseLoadedArmyReferences** — cross: `ArmyAssetRegistry_FindByIdCf` → `assets/army/catalog`
- `0x00513960` **GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables**
- `0x00513CA0` **GameFactionRuntime_TestCapabilityBitClearCf**
- `0x00513CD0` **GameFactionRuntime_GetPackedStateNibble**
- `0x00513D70` **GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10** — cross: `Technology_UnlockForFaction` → `gameplay/technology/runtime`, `InGameOtherPlayerCommand_RebuildTargetEntries` → `ui/ingame/runtime`
- `0x00514510` **GameFactionRuntime_UpdateImpactAlertAnchorAndNotify** — cross: `InGameNotificationQueue_InsertPriorityRecord` → `ui/ingame/runtime`
- `0x00514730` **GameFactionRuntime_RecomputeProgressAndScoreMetrics** — cross: `ArmyAssetRegistry_FindByIdCf` → `assets/army/catalog`
- `0x00514900` **GameFactionRuntime_FindRuntimeGroupIndexCf**
- `0x0051B800` **FactionRuntime_HasArmyAssetOrActiveStructureCf**
- `0x0051C4C0` **GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel**
- `0x0051C680` **GameEntityRuntime_ResolveCommandTargetPositionCf**
- `0x0052A4D0` **GameEntityRuntime_ApplyImpactDamageAndFactionRelationState** — cross: `ArmyRuntime_ApplyDamageAndPropagateToParent` → `gameplay/army/combat`
- `0x00560110` **GameFactionRuntime_RegisterArmyAssetPointers** — cross: `ArmyAssetRegistry_FindByIdCf` → `assets/army/catalog`
- `0x00560160` **GameFactionRuntime_CancelQueuedArmyAssetsAndRefund** — cross: `ArmyAssetRegistry_FindByIdCf` → `assets/army/catalog`
- `0x00560400` **GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer** — cross: `ArmyAssetRegistry_FindByIdCf` → `assets/army/catalog`, `UiCommandSpriteVariantA_RebuildGrid` → `ui/ingame/commands`
- `0x00560620` **GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid** — cross: `UiCommandSpriteVariantA_RebuildGrid` → `ui/ingame/commands`
- `0x005606A0` **GameFactionRuntime_SellArmyAssetAndRefundSevenEighths** — cross: `ArmyAssetRegistry_FindByIdCf` → `assets/army/catalog`, `UiCommandSpriteVariantA_RebuildGrid` → `ui/ingame/commands`
- `0x00561F80` **PlayerRuntime_ResolveAndStoreState8094** — cross: `ArmyRuntime_CreateInstanceFromAssetCf` → `gameplay/army/runtime`
- `0x00561FF0` **PlayerRuntime_SetState8090**
- `0x00562020` **PlayerRuntime_SetState8094**
- `0x005622C0` **PlayerRuntime_ClearState8094**
- `0x00565590` **OldUnitRuntime_MergeMasksAndReplayRecords** — cross: `ArmyRuntime_CreateInstanceFromAssetCf` → `gameplay/army/runtime`, `WorldRuntime_ForEachNodeInOwnerListD8` → `world/runtime/core`, `FieldGrid_ClassifyCellFlagsToRuntimeByte` → `world/terrain/grid`
- `0x00513D00` **GameFactionRuntime_IsRecentTimedRelationStateCf**
- `0x00565650` **OldUnitRuntime_ResetPendingTables**
- `0x00513EE0` **GameFactionRuntime_ApplyPairwiseRelationTransition** — cross: `InGameNotificationQueue_InsertPriorityRecord` → `ui/ingame/runtime`, `ModelRuntimeHierarchy_SetCommandTargetRecursive` → `world/model/hierarchy`, `UiCommandSpriteVariantA_RebuildGrid` → `ui/ingame/commands`, `UiCatalogGroup42_RebuildGrid` → `ui/ingame/technology`, `UiCatalogGroup48_RebuildGrid` → `ui/ingame/technology`, `InGameOtherPlayerCommand_RebuildTargetEntries` → `ui/ingame/runtime`
