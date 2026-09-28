/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/runtime/core.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_RUNTIME_CORE_H
#define THANDOR_WORLD_RUNTIME_CORE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/runtime/core. */

/* WorldObjectRecord.common.allocationFlags value of a record in use (WorldObjectArray_AllocateFreeRecord). */
#define WORLD_OBJECT_RECORD_ALLOCATED 0x40000000
/* WorldRuntimeContext.runtimeFlags bits (world/runtime/core, world/motion/runtime):
   UNLIMITED_CAMERA skips the camera distance and pitch limits; FIELD_GRID_DIRTY is cleared whenever the
   camera state or the field grid is (re)set (WorldRuntime_ClearFieldGridDirtyFlag); SECONDARY_SURFACE_ONLY
   makes the view ray test only the secondary field surface (set by UiCommandModeG_SetNodeFlag01000000). */
#define WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA 0x40000
#define WORLD_RUNTIME_FLAG_FIELD_GRID_DIRTY 0x800
#define WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY 0x1000000
/* Mirrors PERSISTENT_SETTING_SHADING_ENABLED (session start and the in-game shading option); also toggled by
   Alt+S in InGameCameraCommand_DispatchByCodeAndModifierFlags. */
#define WORLD_RUNTIME_FLAG_SHADING_ENABLED 0x20000
/* Mirror the gameplay options of PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS (bits 0, 1, 2; in-game
   settings page and session start) */
#define WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM 0x40000000
#define WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT 0x80000000
#define WORLD_RUNTIME_FLAG_HIDE_PANEL 0x4000000
/* WorldRuntimeContext.runtimeFlags bits that switch on world view overlays (tested in
   FrontendModelPointerContext_RenderWorldViewQueuesClipped; the editor mode tabs InGameCommandModeG_Select0..5
   set them per tool through the UiCommandModeG_Set/ClearNodeFlag* helpers). */
#define WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS 0x400 /* SelectionOverlay_RenderSelectedArmyMetrics */
#define WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER 0x100000 /* SelectionOverlay_DrawMarkerACForWorldSurfacePoint */
#define WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS 0x200000 /* SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints */
#define WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS 0x800000 /* SelectionOverlay_DrawMarkerAEForVisibleProjectedGridVertices */
#define WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS 0x2000000 /* SelectionOverlay_DrawMarkerB1B2ForProjectedVertexMask1800 */
/* More WorldRuntimeContext.runtimeFlags bits read by FrontendModelPointerContext_RenderWorldViewQueuesClipped:
   the view draws the attached field grid (terrain pass), and the spatial sound listener follows the camera. */
#define WORLD_RUNTIME_FLAG_DRAW_TERRAIN 0x4000
#define WORLD_RUNTIME_FLAG_SOUND_LISTENER 0x10000
/* WorldOwnerListNode100.runtimeFlags bit: the node is linked into its world's owner list. */
#define WORLD_OWNER_NODE_LINKED 0x80000000
/* Height returned by the WorldRuntime_Interpolate*HeightOrSentinel functions when no field grid is attached. */
#define WORLD_HEIGHT_NO_FIELD_GRID 0x7ffff000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00532FA0 */
void __thandor_void_preserve_eax_ecx_edx
WorldLightingRuntime_UpdateInterpolatedTerrainLighting(void);

/* 0x0050D100 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetPosition60AndDistanceFromPosition80
          (Q12 positionZ,Q12 positionY,Q12 positionX,WorldRuntimeContext *runtime);

/* 0x0050D150 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetMotionParameters6CThrough78Clamped
          (WorldMotionValue78 value78,AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 magnitude
          ,WorldRuntimeContext *runtime);

/* 0x0050D1E0 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
          (AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 distance,Q12 originZ,Q12 originY,
          Q12 originX,WorldRuntimeContext *runtime);

/* 0x0050D2C0 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_RestoreMotionStateFromSnapshot(WorldRuntimeContext *worldRuntime);

/* 0x0050D670 */
void __thandor_preserve_eax
WorldRuntime_AttachFieldGridAsset(FieldGridAsset *asset,WorldRuntimeContext *world);

/* 0x00561E30 */
void __thandor_preserve_eax_edx
WorldRuntime_AdjustFieldOriginWrappedClamped
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,Q12 deltaElevationAngle,Q12 deltaAzimuthAngle);

/* 0x004BE760 */
Q12 WorldRuntime_InterpolateTerrainHeightOrSentinel (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

/* 0x004BE790 */
Q12 WorldRuntime_InterpolateWaterSurfaceHeightOrSentinel (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

/* 0x004BE7C0 */
uint32_t WorldRuntime_InterpolateTopSurfaceHeightOrSentinel (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

/* 0x0050A610 */
bool __thandor_cf_preserve_eax_ecx_edx
WorldRuntimeNode_IsPositionInsideBounds
          (WorldOwnerListNode100 *runtimeNode,WorldRuntimeExtendedMapControlView170 *boundsControl);

/* 0x0050D260 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_CaptureMotionStateToSnapshot(WorldRuntimeContext *worldRuntime);

/* 0x0050D330 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_MotionStateMatchesSnapshot(WorldRuntimeContext *worldRuntime);

/* 0x0050D4F0 */
void __thandor_preserve_eax WorldRuntime_CommitScalar7CFrom8C(WorldRuntimeContext *world);

/* 0x0050D510 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_AttachObjectArray
          (WorldObjectRecordCount count,WorldObjectRecord *objectArray,WorldRuntimeContext *world);

/* 0x0050D540 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world);

/* 0x0050D560 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_AddFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world);

/* 0x0050D580 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_ClearFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world);

/* 0x0050D5A0 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_ToggleFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world);

/* 0x0050D610 */
WorldVector0EaxEcxEdx12 WorldRuntime_GetVector0Regs(WorldRuntimeContext *world);

/* 0x0050D630 */
WorldVector1EaxEcxEdx12 WorldRuntime_GetVector1Regs(WorldRuntimeContext *world);

/* 0x0050D650 */
WorldFlagsResult __thandor_eax_cf_preserve_ecx_edx
WorldRuntime_GetFlags(WorldRuntimeContext *world);

/* 0x0050D6A0 */
FieldGridAsset * WorldRuntime_GetFieldGridAsset(WorldRuntimeContext *world);

/* 0x0050D6D0 */
uint32_t WorldRuntime_GetPendingToken(WorldRuntimeContext *world);

/* 0x0050D6F0 */
uint32_t WorldRuntime_TakePendingToken(WorldRuntimeContext *world);

/* 0x0050D710 */
void __thandor_void_preserve_eax_ecx
WorldRuntime_AttachAndClearDwordArray
          (WorldWorkspaceElementCount count,uint32_t *array,WorldRuntimeContext *world);

/* 0x0050D740 */
uint32_t * WorldRuntime_GetDwordArray(WorldRuntimeContext *world);

/* 0x0050D7D0 */
WorldObjectAllocResult __thandor_eax_cf_preserve_ecx_edx
WorldObjectArray_AllocateFreeRecord(WorldRuntimeContext *worldRuntime);

/* 0x0050D830 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_LinkNodeIntoOwnerListD8(WorldOwnerListNode100 *node);

/* 0x0050D880 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_UnlinkNodeFromOwnerListD8(WorldOwnerListNode100 *node);

/* 0x0050D8F0 */
void __thandor_preserve_eax_edx
WorldRuntime_ForEachNodeInOwnerListD8
          (void *callbackContext,WorldRuntimeNodeTraversalCallback *callback,
          WorldRuntimeContext *world);

/* 0x0050EC80 */
RuntimeImagePointerByteSizeEdxEax8 __cdecl RuntimeHexSegment_GetLightImageAndToggleFlagRegs(void);

/* 0x0050ECA0 */
void __cdecl RuntimeHexSegment_ToggleLightImageFlag(void);

/* 0x0050ECB0 */
ResourceRegistrationImagePair RuntimeHexSegment_GetFieldImageRegs(InGameFieldImageSaveContext58 *fieldImageContext);

/* 0x0050ECD0 */
void RuntimeHexSegment_AfterFieldImageNoOp(InGameFieldImageSaveContext58 *fieldImageContext);

/* 0x0051BFA0 */
void __thandor_preserve_eax_edx
WorldRuntimeNode_ClearOwnedModelReferencesCallback(void *releasedObject,WorldOwnerListNode100 *node);

/* 0x0051D500 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
          (void *sourceRuntime,WorldRuntimeContext *worldRuntime);

/* 0x005233F0 */
void UnifiedRuntimeTable_Method5_TwoArgNoOp
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime);

/* 0x00523400 */
void UnifiedRuntimeTable_Method6_TwoArgNoOp
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime);

/* 0x00527B70 */
void __thandor_void_preserve_eax_ecx_edx
UnifiedRuntimeDefault_OneArgNoOpC(ModelRuntimeSlot *modelRuntime);

/* 0x00527BA0 */
void UnifiedRuntimeDefault_TwoArgNoOpB (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

/* 0x00527BB0 */
uint32_t __thandor_eax_preserve_ecx_edx UnifiedRuntimeDefault_OneArgReturnZero(void *context);

/* 0x00527BE0 */
bool __thandor_cf_preserve_eax_ecx_edx
UnifiedRuntimeDefault_TwoArgSuccess
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView200 *modelRuntime);

/* 0x00527BF0 */
void __thandor_void_preserve_eax_ecx_edx
UnifiedRuntimeDefault_TwoArgNoOpD(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00529430 */
void __thandor_preserve_eax_edx
WorldRuntimeNode_ClearDetachedEntityReferencesCallback
          (void *detachedObject,WorldOwnerListNode100 *node);

/* 0x00565110 */
void __thandor_preserve_eax_edx
WorldRuntimeNode_ReleaseShutdownBindingsCallback
          (WorldRuntimeContext *shutdownContext,WorldOwnerListNode100 *node);

/* 0x0050D3B0 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(WorldRuntimeContext *worldRuntime);

/* 0x0050D760 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetTerrainLightingConfiguration
          (PackedArgb32 lightingColor13CArgb,PackedArgb32 lightingColor138Argb,
          PackedArgb32 lightingColor134Argb,PackedArgb32 lightingColor130Argb,
          PackedArgb32 rampColor12CArgb,PackedArgb32 lightingColor128Argb,
          PackedArgb32 rampColor124Argb,PackedArgb32 baseColorArgb,WorldRuntimeContext *worldRuntime
          );

/* 0x0050D5C0 */
void __thandor_void_preserve_ecx_edx
WorldRuntime_RecomputeFieldRegionNormalsAndLighting
          (FieldGridDimensionCells auxiliaryElevationAngle,FieldGridDimensionCells auxiliaryAzimuthAngle,
          Q12 lightElevationAngle,Q12 lightAzimuthAngle,WorldRuntimeContext *worldRuntime);

/* 0x0050D6B0 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_ClearFieldGridDirtyFlag(WorldRuntimeContext *world);

#endif /* THANDOR_WORLD_RUNTIME_CORE_H */
