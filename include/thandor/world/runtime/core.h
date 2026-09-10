#ifndef THANDOR_WORLD_RUNTIME_CORE_H
#define THANDOR_WORLD_RUNTIME_CORE_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/runtime/core. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00532FA0 */
undefined8 WorldLightingRuntime_UpdateInterpolatedTerrainLighting(void);

/* 0x0050D100 */
void WorldRuntime_SetPosition60AndDistanceFromPosition80 (Q12 positionZ,Q12 positionY,Q12 positionX,WorldRuntimeContext *runtime);

/* 0x0050D150 */
void WorldRuntime_SetMotionParameters6CThrough78Clamped (WorldMotionValue78 value78,AngleTurn32 pitchAngle,AngleTurn32 headingAngle, UQ12 magnitude,WorldRuntimeContext *runtime);

/* 0x0050D1E0 */
void WorldRuntime_SetPosition80AndRebuildPosition60FromAngles (AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 distance,Q12 originZ, Q12 originY,Q12 originX,WorldRuntimeContext *runtime);

/* 0x0050D2C0 */
void WorldRuntime_RestoreMotionStateFromSnapshot(WorldRuntimeContext *worldRuntime);

/* 0x0050D670 */
void WorldRuntime_AttachFieldGridAsset(FieldGridAsset *asset,WorldRuntimeContext *world);

/* 0x00561E30 */
void WorldRuntime_AdjustFieldOriginWrappedClamped (undefined4 param_1,undefined4 param_2,Q12 deltaWorldY,Q12 deltaWorldX);

/* 0x004BE760 */
Q12 WorldRuntime_InterpolateTerrainHeightOrSentinel (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

/* 0x004BE790 */
Q12 WorldRuntime_InterpolateWaterSurfaceHeightOrSentinel (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

/* 0x004BE7C0 */
dword WorldRuntime_InterpolateTopSurfaceHeightOrSentinel (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

/* 0x0050A610 */
void WorldRuntimeNode_IsPositionInsideBoundsCf (WorldRuntimeNode *runtimeNode,WorldRuntimeExtendedMapControlAddress32 boundsControl);

/* 0x0050D260 */
void WorldRuntime_CaptureMotionStateToSnapshot(WorldRuntimeContext *worldRuntime);

/* 0x0050D330 */
undefined8 WorldRuntime_MotionStateMatchesSnapshotCf(WorldRuntimeContext *worldRuntime);

/* 0x0050D4F0 */
undefined4 WorldRuntime_CommitScalar7CFrom8C(WorldRuntimeContext *world);

/* 0x0050D510 */
void WorldRuntime_AttachObjectArray (WorldObjectRecordCount count,WorldObjectRecord *objectArray, WorldRuntimeContext *world);

/* 0x0050D540 */
undefined4 WorldRuntime_SetFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world);

/* 0x0050D560 */
undefined4 WorldRuntime_AddFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world);

/* 0x0050D580 */
undefined4 WorldRuntime_ClearFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world);

/* 0x0050D5A0 */
undefined4 WorldRuntime_ToggleFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world);

/* 0x0050D610 */
undefined8 WorldRuntime_GetVector0Regs(WorldRuntimeContext *world);

/* 0x0050D630 */
undefined8 WorldRuntime_GetVector1Regs(WorldRuntimeContext *world);

/* 0x0050D650 */
WorldRuntimeControlFlags WorldRuntime_GetFlagsCf(WorldRuntimeContext *world);

/* 0x0050D6A0 */
FieldGridAsset * WorldRuntime_GetFieldGridAsset(WorldRuntimeContext *world);

/* 0x0050D6D0 */
dword WorldRuntime_GetPendingToken(WorldRuntimeContext *world);

/* 0x0050D6F0 */
dword WorldRuntime_TakePendingToken(WorldRuntimeContext *world);

/* 0x0050D710 */
void WorldRuntime_AttachAndClearDwordArray (WorldWorkspaceElementCount count,dword *array,WorldRuntimeContext *world);

/* 0x0050D740 */
dword * WorldRuntime_GetDwordArray(WorldRuntimeContext *world);

/* 0x0050D7D0 */
int WorldObjectArray_AllocateFreeRecordCf(WorldRuntimeContext *worldRuntime);

/* 0x0050D830 */
undefined4 WorldRuntime_LinkNodeIntoOwnerListD8(WorldRuntimeNode *node);

/* 0x0050D880 */
undefined8 WorldRuntime_UnlinkNodeFromOwnerListD8(WorldRuntimeNode *node);

/* 0x0050D8F0 */
void WorldRuntime_ForEachNodeInOwnerListD8 (void *callbackContext,WorldRuntimeNodeTraversalCallback *callback, WorldRuntimeContext *world);

/* 0x0050EC80 */
RuntimeImagePointerByteSizeEdxEax8 __cdecl RuntimeHexSegment_GetLightImageAndToggleFlagRegs(void);

/* 0x0050ECA0 */
void __cdecl RuntimeHexSegment_ToggleLightImageFlag(void);

/* 0x0050ECB0 */
ResourceRegistrationImagePair RuntimeHexSegment_GetFieldImageRegs(InGameFieldImageSaveContext58 *fieldImageContext);

/* 0x0050ECD0 */
void RuntimeHexSegment_AfterFieldImageNoOp(InGameFieldImageSaveContext58 *fieldImageContext);

/* 0x0051BFA0 */
void WorldRuntimeNode_ClearOwnedModelReferencesCallback(void *releasedObject,WorldRuntimeNode *node);

/* 0x0051D500 */
void WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries (void *sourceRuntime,WorldRuntimeContext *worldRuntime);

/* 0x005233F0 */
void UnifiedRuntimeTable_Method5_TwoArgNoOp (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00523400 */
void UnifiedRuntimeTable_Method6_TwoArgNoOp (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00527B70 */
void UnifiedRuntimeDefault_OneArgNoOpC(ModelRuntimeSlot *modelRuntime);

/* 0x00527BA0 */
void UnifiedRuntimeDefault_TwoArgNoOpB (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

/* 0x00527BB0 */
undefined4 UnifiedRuntimeDefault_OneArgReturnZero(void *context);

/* 0x00527BE0 */
void UnifiedRuntimeDefault_TwoArgSuccessCf (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00527BF0 */
void UnifiedRuntimeDefault_TwoArgNoOpD (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00529430 */
void WorldRuntimeNode_ClearDetachedEntityReferencesCallback (void *detachedObject,WorldRuntimeNode *node);

/* 0x00565110 */
void WorldRuntimeNode_ReleaseShutdownBindingsCallback (WorldRuntimeContext *shutdownContext,WorldRuntimeNode *node);

/* 0x0050D3B0 */
void WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(WorldRuntimeContext *worldRuntime);

/* 0x0050D760 */
void WorldRuntime_SetTerrainLightingConfiguration (PackedArgb32 lightingColor13CArgb,PackedArgb32 lightingColor138Argb, PackedArgb32 lightingColor134Argb,PackedArgb32 lightingColor130Argb, PackedArgb32 rampColor12CArgb,PackedArgb32 lightingColor128Argb, PackedArgb32 rampColor124Argb,PackedArgb32 baseColorArgb, WorldRuntimeContext *worldRuntime);

/* 0x0050D5C0 */
void WorldRuntime_RecomputeFieldRegionNormalsAndLighting (FieldGridDimensionCells gridHeight,FieldGridDimensionCells gridWidth, Q12 originWorldYQ12,Q12 originWorldXQ12,WorldRuntimeContext *worldRuntime);

/* 0x0050D6B0 */
void WorldRuntime_ClearFieldGridDirtyFlag(WorldRuntimeContext *world);

#endif /* THANDOR_WORLD_RUNTIME_CORE_H */
