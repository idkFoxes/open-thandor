# Call graph: `gameplay/ai/technology`

9 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0053BD80` **AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue** — local: `AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf`, `AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8`; cross: `ModelDefinitionRegistry_FindByRuntimeClassId` → `assets/model/definitions`, `AiPrimaryWorkspace_HasEntryByIdCf` → `gameplay/ai/workspaces`
- `0x00538000` **AiTechnologyCandidate_IsCurrentlyAvailableCf**
- `0x00538140` **AiTechnologyPlanning_AddCandidateRecord**
- `0x0053BCC0` **AiTechnologyScore_ComputeFactionScaledCandidateValue**
- `0x0053BD60` **AiTechnologyScore_ReturnBaseCandidateValueForKind2**
- `0x0053BEA0` **AiTechnologyScore_ReturnBaseCandidateValueForKind4**
- `0x0053BEC0` **AiTechnologyScore_ComputeCategoryCompatibleCandidateValue** — cross: `AiArmyCandidate_ComputeAverageCompatibleAssetScore` → `gameplay/ai/planning`
- `0x0053BC00` **AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf**
- `0x0053BC20` **AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8**
