/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/runtime/core.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_RUNTIME_CORE_H
#define THANDOR_WORLD_RUNTIME_CORE_H

#include <thandor/core/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

/* WorldRuntimeContext.runtimeFlags bits (world/runtime/core, world/camera):
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
/* Height returned by WorldRuntime_InterpolateTopSurfaceHeightOrSentinel when no field grid is attached. */
#define WORLD_HEIGHT_NO_FIELD_GRID 0x7ffff000

void WorldRuntime_AttachFieldGridAsset(FieldGridAsset *asset,WorldRuntimeContext *world);

uint32_t WorldRuntime_InterpolateTopSurfaceHeightOrSentinel (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

void WorldRuntime_AttachAndClearDwordArray(WorldWorkspaceElementCount count,uintptr_t *array,WorldRuntimeContext *world);

#endif /* THANDOR_WORLD_RUNTIME_CORE_H */
