/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/runtime/flags.h
 */

#ifndef THANDOR_WORLD_RUNTIME_FLAGS_H
#define THANDOR_WORLD_RUNTIME_FLAGS_H

#include <stdint.h>
#include <thandor/core/flags.h>

/* Not part of the original: the one flag word at +0x4C of a world view, behind its UiNodeBase. The in-game world
   view reads it as WorldRuntimeContext.runtimeFlags, the menu room and the render paths as
   FrontendModelPointerContext.contextFlags (FrontendModelPointerContextFlags is this type), the terrain renderer
   through the same FrontendModelPointerContext. Each family keeps its names (`using enum`); a bit that two
   families name (0x10, 0x200, 0x800, 0x400000, 0x4000000, 0x40000000, 0x80000000) has an enumerator per name with
   the same value. Bits without a name stay valid (fixed underlying type). */
enum class WorldRuntimeFlags : uint32_t {
    /* Low four bits: the camera motion a right drag or the wheel is doing
       (FrontendModelPointerContext_DispatchWorldCameraPointerInput / _PointerWheel) */
    FRONTEND_CAMERA_MOTION_MOVE = 0x1,
    FRONTEND_CAMERA_MOTION_HEADING = 0x2,
    FRONTEND_CAMERA_MOTION_DISTANCE = 0x4,
    FRONTEND_CAMERA_MOTION_PITCH = 0x8,
    FRONTEND_CAMERA_MOTION_MASK = 0xF,

    /* 0x10, two names for one bit: in game it is set while the camera shows the target of a notification "go to"
       (InGameTargetingContext_AdvanceOrResolveTarget, cleared by InGameTargetingContext_CancelAndRestoreState; the
       world input then only shows the busy cursor); the frontend pointer context reads it as "no built-in action
       resolution". */
    WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO = 0x10,
    FRONTEND_MODEL_POINTER_CONTEXT_SUPPRESS_BUILTIN_ACTION_RESOLUTION = 0x10,
    FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK = 0x20,
    FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION = 0x40,
    /* set once a captured pointer moved far enough to start a drag selection */
    WORLD_RUNTIME_FLAG_DRAG_SELECTING = 0x80,
    FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_ORBIT = 0x100, /* right drag: heading and pitch, with the left button distance */
    /* 0x200, two names: right drag where the modifier keys choose move, heading, pitch or distance; with
       WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA the camera clamps distance and pitch to the g_WorldMotionAlternate* range
       instead of not at all */
    FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_FREE = 0x200,
    WORLD_RUNTIME_FLAG_ALTERNATE_CAMERA_RANGE = 0x200,
    /* world view overlays (tested in FrontendModelPointerContext_RenderWorldViewQueuesClipped; the editor mode tabs
       InGameCommandModeG_Select0..5 set them per tool through the UiCommandModeG_Show/Hide* helpers) */
    WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS = 0x400, /* SelectionOverlay_RenderSelectedArmyMetrics */
    /* 0x800, two names: cleared whenever the camera state or the field grid is (re)set
       (WorldRuntime_ClearFieldGridDirtyFlag); FrontendModelPointerContext_RenderWorldViewQueuesClipped sets it
       after a frame that covered the whole view. While it is set (and the field grid is unchanged)
       TerrainProjectedGrid_TransformShadeAndQueue reuses the row spans and point-A projections of the previous
       frame. */
    WORLD_RUNTIME_FLAG_FIELD_GRID_DIRTY = 0x800,
    TERRAIN_RENDER_REUSE_PROJECTION = 0x800,
    FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY = 0x1000,
    /* Up to 640 pixels wide the dialog pages cover the menu room, so opening one also stops the room's 3D
       rendering: FrontendModelPointerContext_RenderWorldViewQueuesClipped returns at once while this is set in
       menuRoomModelView's contextFlags. */
    FRONTEND_MENU_ROOM_RENDER_SUPPRESSED = 0x2000,
    /* the view draws the attached field grid (terrain pass) */
    WORLD_RUNTIME_FLAG_DRAW_TERRAIN = 0x4000,
    FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_PAN = 0x8000, /* right drag: move, with Ctrl heading and pitch */
    WORLD_RUNTIME_FLAG_SOUND_LISTENER = 0x10000, /* the spatial sound listener follows the camera */
    /* Mirrors PERSISTENT_SETTING_SHADING_ENABLED (session start and the in-game shading option); also toggled by
       Alt+S in InGameCameraCommand_DispatchByCodeAndModifierFlags. */
    WORLD_RUNTIME_FLAG_SHADING_ENABLED = 0x20000,
    /* skips the camera distance and pitch limits */
    WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA = 0x40000,
    FRONTEND_MODEL_POINTER_CONTEXT_HIT_DISTANCE_TO_BOUNDS_CENTER = 0x80000, /* hit metric measured to the bounding box
                                                                               centre, not the node origin */
    WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER = 0x100000, /* SelectionOverlay_DrawWorldPointMarker */
    WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS = 0x200000, /* SelectionOverlay_DrawTerrainPointMarkers */
    /* 0x400000, two names: the frontend pointer context also hits models of other factions; in game it is set
       while the map editor is active (InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState) */
    FRONTEND_MODEL_POINTER_CONTEXT_ALLOW_NON_FACTION_MODELS = 0x400000,
    INGAME_WORLD_FLAG_EDITOR = 0x400000,
    WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS = 0x800000, /* SelectionOverlay_DrawGridVertexMarkers */
    /* makes the view ray test only the secondary field surface (set by UiCommandModeG_SetSecondarySurfaceOnly) */
    WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY = 0x1000000,
    WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS = 0x2000000, /* SelectionOverlay_DrawResourceCellMarkers */
    /* 0x4000000, 0x40000000, 0x80000000: mirror the gameplay options of
       PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS (bits 0, 1, 2; in-game settings page and session start);
       the frontend pointer context names them too */
    WORLD_RUNTIME_FLAG_HIDE_PANEL = 0x4000000,
    FRONTEND_MODEL_POINTER_CONTEXT_HIDE_PANEL = 0x4000000,
    /* set on a pointer press while interaction flag 0x80 is held: the release replaces the selection instead of
       selecting a single army (InGameWorldInput_BeginPointerCapture / _CommitPointerAction) */
    WORLD_RUNTIME_FLAG_REPLACE_SELECTION = 0x8000000,
    WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM = 0x40000000,
    FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_ZOOM = 0x40000000,
    WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT = 0x80000000,
    FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_TILT = 0x80000000,
};
THANDOR_FLAG_ENUM(WorldRuntimeFlags);
using enum WorldRuntimeFlags;

using FrontendModelPointerContextFlags = WorldRuntimeFlags;

#endif /* THANDOR_WORLD_RUNTIME_FLAGS_H */
