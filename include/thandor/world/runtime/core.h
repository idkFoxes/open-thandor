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
   makes the view ray test only the secondary field surface (set by UiCommandModeG_SetSecondarySurfaceOnly). */
#define WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA 0x40000
/* with UNLIMITED_CAMERA: clamp distance and pitch to the g_WorldMotionAlternate* range instead of not at all */
#define WORLD_RUNTIME_FLAG_ALTERNATE_CAMERA_RANGE 0x200
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
   set them per tool through the UiCommandModeG_Show/Hide* helpers). */
#define WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS 0x400 /* SelectionOverlay_RenderSelectedArmyMetrics */
#define WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER 0x100000 /* SelectionOverlay_DrawWorldPointMarker */
#define WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS 0x200000 /* SelectionOverlay_DrawTerrainPointMarkers */
#define WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS 0x800000 /* SelectionOverlay_DrawGridVertexMarkers */
#define WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS 0x2000000 /* SelectionOverlay_DrawResourceCellMarkers */
/* More WorldRuntimeContext.runtimeFlags bits read by FrontendModelPointerContext_RenderWorldViewQueuesClipped:
   the view draws the attached field grid (terrain pass), and the spatial sound listener follows the camera. */
#define WORLD_RUNTIME_FLAG_DRAW_TERRAIN 0x4000
#define WORLD_RUNTIME_FLAG_SOUND_LISTENER 0x10000
/* WorldOwnerListNode.runtimeFlags bit: the node is linked into its world's owner list. */
#define WORLD_OWNER_NODE_LINKED 0x80000000
/* Range of the auxiliary elevation angle (fieldRegion.auxiliaryElevationAngle) set by WorldRuntime_TurnAuxiliaryAnglesClamped */
#define WORLD_AUXILIARY_ELEVATION_MINIMUM (-0x4000) /* a quarter turn down */
#define WORLD_AUXILIARY_ELEVATION_MAXIMUM (-0x1000)
/* Smallest camera magnitude WorldRuntime_SetCameraAnglesAndMagnitudeClamped accepts (0.25 in Q12) */
#define WORLD_MOTION_MINIMUM_MAGNITUDE_Q12 0x400
/* Height returned by WorldRuntime_InterpolateTopSurfaceHeightOrSentinel when no field grid is attached. */
#define WORLD_HEIGHT_NO_FIELD_GRID 0x7ffff000
/* WorldLightingRuntime_UpdateInterpolatedTerrainLighting: one wrap of a 16-bit half of a packed field-region
   pair, added to the lower endpoint so the blend runs forward through the wrap */
#define WORLD_LIGHTING_PACKED_HALF_WRAP 0x10000
/* Functions are grouped by semantic ownership. */

void WorldLightingRuntime_UpdateInterpolatedTerrainLighting(void);

void WorldRuntime_SetCameraPositionKeepingTarget
          (Q12 positionZ,Q12 positionY,Q12 positionX,WorldRuntimeContext *runtime);

void WorldRuntime_SetCameraAnglesAndMagnitudeClamped
          (WorldMotionValue78 projectionShift,AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 magnitude
          ,WorldRuntimeContext *runtime);

void WorldRuntime_PointCameraAtTarget
          (AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 distance,Q12 originZ,Q12 originY,
          Q12 originX,WorldRuntimeContext *runtime);

void WorldRuntime_RestoreMotionStateFromSnapshot(WorldRuntimeContext *worldRuntime);

void WorldRuntime_AttachFieldGridAsset(FieldGridAsset *asset,WorldRuntimeContext *world);

void WorldRuntime_TurnAuxiliaryAnglesClamped
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,Q12 deltaElevationAngle,Q12 deltaAzimuthAngle);

uint32_t WorldRuntime_InterpolateTopSurfaceHeightOrSentinel (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

bool WorldRuntimeNode_IsPositionInsideBounds
          (WorldOwnerListNode *runtimeNode,WorldRuntimeExtendedMapControlView *boundsControl);

void WorldRuntime_CaptureMotionStateToSnapshot(WorldRuntimeContext *worldRuntime);

void WorldRuntime_CommitCameraTargetDistance(WorldRuntimeContext *world);

void WorldRuntime_AttachObjectArray
          (WorldObjectRecordCount count,WorldObjectRecord *objectArray,WorldRuntimeContext *world);

WorldCameraPosition WorldRuntime_GetCameraPosition(WorldRuntimeContext *world);

WorldCameraOrientation WorldRuntime_GetCameraOrientation(WorldRuntimeContext *world);

void WorldRuntime_AttachAndClearDwordArray(WorldWorkspaceElementCount count,uint32_t *array,WorldRuntimeContext *world);

WorldObjectRecord *WorldObjectArray_AllocateFreeRecord(WorldRuntimeContext *worldRuntime);

void WorldRuntime_LinkOwnerListNode(WorldOwnerListNode *node);

void WorldRuntime_UnlinkOwnerListNode(WorldOwnerListNode *node);

void WorldRuntime_ForEachOwnerListNode(void *callbackContext,WorldRuntimeNodeTraversalCallback *callback,
          WorldRuntimeContext *world);

RuntimeHexSegmentImage __cdecl RuntimeHexSegment_GetLightImageAndToggleFlag(void);

void __cdecl RuntimeHexSegment_ToggleLightImageFlag(void);

RuntimeHexSegmentImage RuntimeHexSegment_GetFieldImage(InGameFieldImageSaveContext58 *fieldImageContext);

void RuntimeHexSegment_AfterFieldImageNoOp(InGameFieldImageSaveContext58 *fieldImageContext);

void WorldRuntimeNode_ClearOwnedModelReferencesCallback(void *releasedObject,WorldOwnerListNode *node);

void WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries(void *sourceRuntime,WorldRuntimeContext *worldRuntime);

void ArmyRuntimeClass_NoOpTickUpdateForClass5
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

void ArmyRuntimeClass_NoOpTickUpdateForClass6
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

void UnifiedRuntimeDefault_OneArgNoOpC(ModelRuntimeSlot *modelRuntime);

void UnifiedRuntimeDefault_TwoArgNoOpB (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

bool UnifiedRuntimeDefault_TwoArgSuccess
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime);

void UnifiedRuntimeDefault_TwoArgNoOpD(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void WorldRuntimeNode_ClearDetachedEntityReferencesCallback(void *detachedObject,WorldOwnerListNode *node);

void WorldRuntimeNode_ReleaseShutdownBindingsCallback(WorldRuntimeContext *shutdownContext,WorldOwnerListNode *node);

void WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(WorldRuntimeContext *worldRuntime);

void WorldRuntime_SetTerrainLightingConfiguration(PackedArgb32 lightingColor13CArgb,PackedArgb32 lightingColor138Argb,
          PackedArgb32 lightingColor134Argb,PackedArgb32 lightingColor130Argb,
          PackedArgb32 secondaryColorArgb,PackedArgb32 lightingColor128Argb,
          PackedArgb32 baseColorArgb,PackedArgb32 rampStepColorArgb,WorldRuntimeContext *worldRuntime
          );

void WorldRuntime_RecomputeFieldRegionNormalsAndLighting
          (FieldGridDimensionCells auxiliaryElevationAngle,FieldGridDimensionCells auxiliaryAzimuthAngle,
          Q12 lightElevationAngle,Q12 lightAzimuthAngle,WorldRuntimeContext *worldRuntime);

void WorldRuntime_ClearFieldGridDirtyFlag(WorldRuntimeContext *world);

#endif /* THANDOR_WORLD_RUNTIME_CORE_H */
