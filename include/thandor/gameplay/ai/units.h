#ifndef THANDOR_GAMEPLAY_AI_UNITS_H
#define THANDOR_GAMEPLAY_AI_UNITS_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/units. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053B0E0 */
void AiUnitBehavior_UpdateWorkspace01Entities (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053B4C0 */
void AiUnitBehavior_SelectBestAnchorAction (undefined4 definitionOrAsset,ArmyRuntimeSlot *armyRuntimeSlot, FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053B1D0 */
AiCandidateScoreEaxPreservedEdxCarrier64 AiUnitBehavior_ComputeWorkspace05DistanceScore (AiCandidateScore32 currentBestScore,undefined4 definitionOrAsset, ArmyRuntimeSlot *armyRuntimeSlot);

/* 0x0053B260 */
AiCandidateScoreEaxPreservedEdxCarrier64 AiUnitBehavior_ComputeFactionAnchorDistanceScore (FactionRuntimeIndex factionIndex,AiCandidateScore32 currentBestScore, undefined4 definitionOrAsset,ArmyRuntimeSlot *armyRuntimeSlot);

/* 0x0053B330 */
AiCandidateScoreEaxPreservedEdxCarrier64 AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore (AiCandidateScore32 currentBestScore,undefined4 definitionOrAsset, ArmyRuntimeSlot *armyRuntimeSlot);

/* 0x0053B3E0 */
void AiUnitCommand_AssignWorkspacePoint (dword *workspacePoint,ArmyRuntimeSlot *armyRuntime, WorldRuntimeContext *worldRuntimeContext);

/* 0x0053B420 */
void AiUnitCommand_AssignFactionAnchorPoint (FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime, WorldRuntimeContext *worldRuntimeContext);

/* 0x0053B480 */
void AiUnitBehavior_CollectUnassignedEntity (undefined4 selectionSource,GameEntityRuntime *entityRuntime, WorldRuntimeContext *worldRuntimeContext);

/* 0x0053B620 */
void AiUnitBehavior_UpdateSpecialClass12Entity (undefined4 unusedDefinitionOrAssetArgument,ArmyRuntimeSlot *armyRuntime, FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_AI_UNITS_H */
