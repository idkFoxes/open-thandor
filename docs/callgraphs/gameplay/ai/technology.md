# Call graph: `gameplay/ai/technology`

[Source](../../../../src/gameplay/ai/technology.c) · [Header](../../../../include/thandor/gameplay/ai/technology.h) · [Graphviz](technology.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-gameplay-ai-technology) · [Developer notes](../../../../CHANGELOG.md#module-gameplay-ai-technology) · [Full changelog](../../../../CHANGELOG_FULL.md#module-gameplay-ai-technology)

9 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0053BD80` **[`AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue`](../../../../src/gameplay/ai/technology.c#L5)** — local: [`AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf`](../../../../src/gameplay/ai/technology.c#L316), [`AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8`](../../../../src/gameplay/ai/technology.c#L331); cross: [`ModelDefinitionRegistry_FindByRuntimeClassId`](../../../../src/assets/model/definitions.c#L416) → [`assets/model/definitions`](../../assets/model/definitions.md), [`AiPrimaryWorkspace_HasEntryByIdCf`](../../../../src/gameplay/ai/workspaces.c#L1282) → [`gameplay/ai/workspaces`](workspaces.md)
- `0x00538000` **[`AiTechnologyCandidate_IsCurrentlyAvailableCf`](../../../../src/gameplay/ai/technology.c#L77)**
- `0x00538140` **[`AiTechnologyPlanning_AddCandidateRecord`](../../../../src/gameplay/ai/technology.c#L155)**
- `0x0053BCC0` **[`AiTechnologyScore_ComputeFactionScaledCandidateValue`](../../../../src/gameplay/ai/technology.c#L212)**
- `0x0053BD60` **[`AiTechnologyScore_ReturnBaseCandidateValueForKind2`](../../../../src/gameplay/ai/technology.c#L250)**
- `0x0053BEA0` **[`AiTechnologyScore_ReturnBaseCandidateValueForKind4`](../../../../src/gameplay/ai/technology.c#L264)**
- `0x0053BEC0` **[`AiTechnologyScore_ComputeCategoryCompatibleCandidateValue`](../../../../src/gameplay/ai/technology.c#L278)** — cross: [`AiArmyCandidate_ComputeAverageCompatibleAssetScore`](../../../../src/gameplay/ai/planning.c#L129) → [`gameplay/ai/planning`](planning.md)
- `0x0053BC00` **[`AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf`](../../../../src/gameplay/ai/technology.c#L316)**
- `0x0053BC20` **[`AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8`](../../../../src/gameplay/ai/technology.c#L331)**

## Called by

- [`gameplay/ai/workspaces`](workspaces.md): [`AiPlanning_RebuildFactionWorkspaces`](../../../../src/gameplay/ai/workspaces.c#L70) → [`AiTechnologyCandidate_IsCurrentlyAvailableCf`](../../../../src/gameplay/ai/technology.c#L77); [`AiPlanning_RebuildFactionWorkspaces`](../../../../src/gameplay/ai/workspaces.c#L70) → [`AiTechnologyPlanning_AddCandidateRecord`](../../../../src/gameplay/ai/technology.c#L155)
