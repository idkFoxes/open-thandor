/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_TYPES_H
#define THANDOR_GRAPHICS_RENDER_TYPES_H

#include <stdint.h>
#include <stddef.h> /* offsetof */
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/memory/types.h>
#include <thandor/core/types.h>
#include <thandor/core/flags.h> /* THANDOR_FLAG_ENUM: GraphicsPrimitiveDispatchFlags */
#include <thandor/ui/controls/types.h>

struct GraphicsShadingRuntimeRecord;
struct GraphicsProjectedPoint2i;
struct GraphicsOffscreenSceneExtents;
struct GraphicsOffscreenViewParameters;
struct GraphicsProjectedVertexSource;
struct GraphicsPrimitivePacket;
struct GraphicsPrimitiveVertexRaw;
union GraphicsPrimitiveRadixBucket;
struct GraphicsPrimitiveQueueNode;
struct GraphicsTriangleInput;
struct GraphicsWideFixed;
struct GraphicsFixedRect;
struct GraphicsPrimitiveQueue;
struct GraphicsFixedVec2;
struct GraphicsSceneBounds8;
struct GeneratedTextureRenderContextView;
struct GraphicsProjectedPointPair;
struct ModelProjectedBoundsPixels;
struct FrontendModelPointerContext;
struct GeneratedTextureSampleWorkRecord;
struct GeneratedTextureScratchRuntime;
struct FieldGridAsset;
struct FrontendModelPointerHitContext;
struct GameEntityRuntime;
struct GraphicsTextureSetEntry;
struct GraphicsTextureSourceAsset;
struct ModelRuntimeNode;
struct WorldRuntimeContext;

using PackedRgb24 = uint32_t;

using GraphicsTransitionTickCount = int;

using GraphicsRadiusQ12 = int;

struct GraphicsShadingRuntimeRecord {
    GraphicsWorldCoordinateQ12 worldXQ12; 
    GraphicsWorldCoordinateQ12 worldYQ12; 
    GraphicsWorldCoordinateQ12 worldZQ12; 
    PackedRgb24 packedColorRgbActive; 
    uint64_t squaredRadiusQ24; 
    GraphicsTransitionTickCount radiusTransitionDurationTicks; 
    GraphicsTransitionTickCount radiusTransitionElapsedTicks; 
    GraphicsRadiusQ12 targetRadiusQ12; 
    uint8_t opaque24_3B[24]; 
    uint32_t serializationToggleDword; 
};

enum {
    FRONTEND_MODEL_POINTER_CONTEXT_SUPPRESS_BUILTIN_ACTION_RESOLUTION=16,
    FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK=32,
    FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION=64,
    FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_ORBIT=256, /* right drag: heading and pitch, with the left button distance */
    FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_FREE=512, /* right drag: the modifier keys choose move, heading, pitch or distance */
    FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY=4096,
    FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_PAN=32768, /* right drag: move, with Ctrl heading and pitch */
    FRONTEND_MODEL_POINTER_CONTEXT_HIT_DISTANCE_TO_BOUNDS_CENTER=524288, /* hit metric measured to the bounding box centre, not the node origin */
    FRONTEND_MODEL_POINTER_CONTEXT_ALLOW_NON_FACTION_MODELS=4194304,
    FRONTEND_MODEL_POINTER_CONTEXT_HIDE_PANEL=67108864, /* = WORLD_RUNTIME_FLAG_HIDE_PANEL */
    FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_ZOOM=1073741824, /* = WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM */
    FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_TILT=2147483648 /* = WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT */
};
using FrontendModelPointerContextFlags = int;

enum {
    GRAPHICS_STATE_DISABLED=0,
    GRAPHICS_STATE_ENABLED=1
};
using GraphicsBooleanState = int;

using GraphicsSceneExtentFixed = int;

using DepthIntervalRadius32 = int;

using GraphicsPrimitiveBackendCoordinate = int;

using GraphicsProjectionShift = uint32_t;

using GraphicsViewAngle16 = uint32_t;

using GraphicsScreenCoordinate = int;

using GraphicsPlaneNormalFixed = int;

/* The render flag word of a primitive packet (GraphicsPrimitivePacket.renderFlags) and of an MDL mesh triangle
   (GraphicsTriangleInput.renderFlags, copied into the packet by GraphicsPrimitiveQueue_AppendTriangle). The bit
   values are file data (MDL) and select the raster handler; bits without an enumerator stay valid. The
   enumerators are also visible unscoped (using enum below), with their former constant names. */
enum class GraphicsPrimitiveDispatchFlags : uint32_t {
    /* Packet bits 12..17 select the raster handler ((flags & 0x3f000) >> 12); the software queue renderers
       shift by 12 and index their handler table. Bit 16 marks a textured packet, bits 12..14 the blend mode. */
    GRAPHICS_PRIMITIVE_RASTER_HANDLER_MASK = 0x3f000,
    GRAPHICS_PRIMITIVE_FLAG_TEXTURED = 0x10000,
    GRAPHICS_PRIMITIVE_FLAG_FORCE_TRANSLUCENT = 0x20000,
    GRAPHICS_PRIMITIVE_FLAG_FLAT_SHADED = 0x8000, /* set by SoftwareRenderer_PrepareTrianglePacket when all three vertex colours are equal */
    GRAPHICS_PRIMITIVE_BLEND_MASK = 0x7000,
    GRAPHICS_PRIMITIVE_BLEND_OPAQUE = 0,
    GRAPHICS_PRIMITIVE_BLEND_TRANSLUCENT = 0x1000,
    GRAPHICS_PRIMITIVE_BLEND_ADDITIVE = 0x2000,
    GRAPHICS_PRIMITIVE_BLEND_MODE_4 = 0x4000, /* kept by GraphicsPrimitiveQueue_SetVertexColors like opaque */
    GRAPHICS_PRIMITIVE_BLEND_ALPHA_DEPTH_WRITE = 0x6000, /* blend mode 6: alpha-blended with depth writes */
    /* bits 28..29: subtracted from an opaque packet's sort key (GraphicsPrimitiveQueue_RenderSortKey) */
    GRAPHICS_PRIMITIVE_SORT_KEY_FLAG_BITS = 0x30000000,
    /* Model triangle bits, read by ModelRender_SubmitTriangle*, ModelRender_PrepareProjectedVertex* */
    MODEL_TRIANGLE_PALETTE_BANK_MASK = 0x1FF, /* material colour: index into the node's palette asset */
    /* The palette bank mask ModelRender_SubmitTriangleAlternatePath uses: keeps bits 16..31 too */
    MODEL_TRIANGLE_PALETTE_BANK_WIDE_MASK = 0xffff01ff,
    MODEL_TRIANGLE_UNLIT = 0x200, /* vertices take the node tint instead of lighting */
    MODEL_TRIANGLE_DOUBLE_SIDED = 0x400, /* drawn without the back-face test */
    MODEL_TRIANGLE_LIGHTING_SCALED = 0x800, /* lit by ModelRender_ComputeVertexIntensityScaledPath */
    MODEL_TRIANGLE_FLAT_SHADED = 0x8000, /* lit with the triangle normal; never reuses a cached vertex colour */
    MODEL_TRIANGLE_VERTEX_CACHE_FLAGS = 0x8E00 /* the bits a projected vertex's cached colour was computed for */
};
THANDOR_FLAG_ENUM(GraphicsPrimitiveDispatchFlags);
using enum GraphicsPrimitiveDispatchFlags;

using DepthBinMask32 = uint32_t;

using GraphicsPrimitiveTextureCoordinateFixed = int;

using GraphicsPrimitiveQueueCapacity = uint32_t;

using ModelMeshGroupAddress32 = intptr_t; /* address of a mesh group / record, pointer-sized (5f) */

using GraphicsAssetSubresourceCount = uint32_t;

using GraphicsPrimitiveScreenCoordinate = int;

using GraphicsElapsedTickCount = uint32_t;

using ModelRuntimeCount = uint32_t;

using GraphicsDistanceAttenuationTableAddress32 = intptr_t; /* pointer-sized (5f) */

using GraphicsProjectedCoordinate = int;

using DepthIntervalCenter32 = int;

using GraphicsShadingRecordCount = uint32_t;

using GraphicsProjectionScale = uint32_t;

using GraphicsPrimitiveDepthFixed = int;

using GraphicsPixelDimension = uint32_t;

struct GraphicsProjectedPointPair { /* defined here because FieldGridCell embeds it */
    GraphicsPrimitiveBackendCoordinate projectedX; // First projected component.
    GraphicsPrimitiveBackendCoordinate projectedY; // Second projected component.
};

struct GraphicsProjectedPoint2i {
    GraphicsProjectedCoordinate x; 
    GraphicsProjectedCoordinate y; 
};

struct GraphicsOffscreenSceneExtents {
    GraphicsSceneExtentFixed horizontalExtent; 
    GraphicsSceneExtentFixed verticalExtent; 
};

struct GraphicsOffscreenViewParameters {
    GraphicsWorldCoordinateQ12 originX; 
    GraphicsWorldCoordinateQ12 originY; 
    GraphicsWorldCoordinateQ12 originZ; 
    GraphicsProjectionScale projectionScale; 
    GraphicsViewAngle16 viewAngle0; 
    GraphicsViewAngle16 viewAngle1; 
    GraphicsProjectionShift projectionShift; 
};

struct GraphicsProjectedVertexSource {
    uint8_t reserved00_0B[12]; // Unresolved prefix retained.
    uint32_t texturedPacketAttributes[5]; // Five consecutive dwords copied into textured packet slots 0..4. Numeric representation is intentionally unchanged.
    GraphicsPrimitiveBackendCoordinate backendCoord0; // Projected backend coordinate 0.
    GraphicsPrimitiveBackendCoordinate backendCoord1; // Projected backend coordinate 1.
    GraphicsPrimitiveDepthFixed depth; // Projected primitive depth.
    PackedArgb32 vertexColorArgb; // Packed ARGB vertex color written by model lighting preparation and consumed by primitive-queue vertex-color/material packet paths.
    GraphicsPrimitiveScreenCoordinate screenX; // Projected screen X.
    GraphicsPrimitiveScreenCoordinate screenY; // Projected screen Y.
};

struct GraphicsPrimitiveVertexRaw {
    GraphicsPrimitiveScreenCoordinate screenX; 
    GraphicsPrimitiveScreenCoordinate screenY; 
    GraphicsPrimitiveBackendCoordinate backendCoord0; 
    GraphicsPrimitiveBackendCoordinate backendCoord1; 
    GraphicsPrimitiveDepthFixed depth; 
    GraphicsPrimitiveTextureCoordinateFixed textureU; 
    GraphicsPrimitiveTextureCoordinateFixed textureV; 
    PackedArgb32 diffuseColor; 
};

struct GraphicsPrimitivePacket {
    struct GraphicsPrimitiveVertexRaw vertices[3]; 
    PackedArgb32 modulationColor; 
    Ptr32<struct GraphicsTextureSetEntry> textureEntry; 
    GraphicsPrimitiveDispatchFlags renderFlags; 
    uint8_t reserved6C_7F[20];
};

union GraphicsPrimitiveRadixBucket {
    uint32_t count; 
    Ptr32<struct GraphicsPrimitiveQueueNode> writeCursor; 
};

struct GraphicsPrimitiveQueueNode {
    uint32_t sortKey; 
    Ptr32<struct GraphicsPrimitivePacket> packet; 
    Ptr32<struct GraphicsPrimitiveQueueNode> next; 
    Ptr32<struct GraphicsPrimitiveQueueNode> previous; 
};

/* 5f-format: GraphicsTriangleInput.vertex0/vertex1/vertex2 - an MDL mesh triangle record (0x40-byte stride) whose
   vertex addresses are 32-bit slots in the loaded file image; with 8-byte pointers this layout no longer matches */
struct GraphicsTriangleInput {
    Ptr32<struct GraphicsProjectedVertexSource> vertex0; 
    GraphicsPrimitiveTextureCoordinateFixed textureU0; 
    GraphicsPrimitiveTextureCoordinateFixed textureV0; 
    Ptr32<struct GraphicsProjectedVertexSource> vertex1; 
    GraphicsPrimitiveTextureCoordinateFixed textureU1; 
    GraphicsPrimitiveTextureCoordinateFixed textureV1; 
    Ptr32<struct GraphicsProjectedVertexSource> vertex2; 
    GraphicsPrimitiveTextureCoordinateFixed textureU2; 
    GraphicsPrimitiveTextureCoordinateFixed textureV2; 
    GraphicsPlaneNormalFixed planeNormalXQ12; 
    GraphicsPlaneNormalFixed planeNormalYQ12; 
    GraphicsPlaneNormalFixed planeNormalZQ12; 
    GraphicsSubresourceIndex subresourceIndex; 
    GraphicsPrimitiveDispatchFlags renderFlags; 
};

struct GraphicsWideFixed {
    uint32_t low; 
    int32_t high; 
};

struct GraphicsFixedRect {
    GraphicsSceneExtentFixed minX; 
    GraphicsSceneExtentFixed minY; 
    GraphicsSceneExtentFixed maxX; 
    GraphicsSceneExtentFixed maxY; 
};

struct GraphicsPrimitiveQueue {
    uint32_t capacity; 
    uint32_t count; 
    Ptr32<struct GraphicsPrimitivePacket> packetPool; 
    Ptr32<struct GraphicsPrimitiveQueueNode> radixScratchPool; 
    Ptr32<struct GraphicsPrimitiveQueueNode> traversalCursor; 
    uint32_t reserved14; 
    uint32_t reserved18; 
    uint32_t reserved1C; 
    struct GraphicsPrimitiveQueueNode primaryNodes[1]; 
};

struct GraphicsFixedVec2 {
    int32_t component0; 
    int32_t component1; 
};

struct GraphicsSceneBounds8 {
    int32_t bound0; 
    int32_t bound1; 
    int32_t bound2; 
    int32_t bound3; 
    int32_t bound4; 
    int32_t bound5; 
    int32_t bound6; 
    int32_t bound7; 
};

struct ModelProjectedBoundsPixels {
    UiPixelCoordinate minX; // Minimum projected pixel X.
    UiPixelCoordinate minY; // Minimum projected pixel Y.
    UiPixelCoordinate maxX; // Maximum projected pixel X.
    UiPixelCoordinate maxY; // Maximum projected pixel Y.
};

struct FrontendModelPointerContext {
    struct UiNodeBase base; // Accepted frontend UI-node prefix.
    FrontendModelPointerContextFlags contextFlags; // Directly observed model-pointer selection and action-routing flags.
    uint32_t activeFactionRuntimeIndex; // WorldRuntimeContext.activeFactionRuntimeIndex of the in-game world view; unused by the frontend paths.
    Ptr32<struct FieldGridAsset> fieldGrid; // Field grid consumed by generated-texture and selection-overlay paths.
    uint32_t worldObjectArray; // WorldRuntimeContext.objectArray of the in-game world view; cleared by relocation.
    uint32_t renderedPrimitiveCount; // Accumulated primitive count for this draw pass; shares storage with a world-runtime token view.
    Q12 hitReferenceWorldXQ12; // Subtracted from model world-transform X when computing the hit metric.
    Q12 hitReferenceWorldYQ12; // Subtracted from model world-transform Y when computing the hit metric.
    Q12 hitReferenceWorldZQ12; // Subtracted from model world-transform Z when computing the hit metric.
    GraphicsProjectionScale projectionScale; // Projection scale copied into Graphics_SetViewProjectionParameters; same storage as world motion magnitude.
    GraphicsViewAngle16 viewAngle0; // Primary view angle copied into graphics projection state.
    GraphicsViewAngle16 viewAngle1; // Secondary view angle copied into graphics projection state.
    GraphicsProjectionShift projectionShift; // Projection shift copied into graphics projection state.
    UQ12 committedDistanceOrSoundZOffset; // World-motion committed distance; quarter-scaled into listener Z by the frontend draw path.
    GraphicsWorldCoordinateQ12 targetPositionXQ12; // World-motion target/origin X; relocation clears it.
    GraphicsWorldCoordinateQ12 targetPositionYQ12; // World-motion target/origin Y; relocation clears it.
    GraphicsWorldCoordinateQ12 targetPositionZQ12; // World-motion target/origin Z; relocation clears it.
    UQ12 targetDistanceQ12; // World-motion target distance.
    AngleTurn32 minimumPitchAngle; // Minimum pitch clamp; relocation installs -0x4000 default when zero.
    AngleTurn32 maximumPitchAngle; // Maximum pitch clamp; relocation installs +0x4000 default when zero.
    uint32_t minimumDistanceQ12; // Lower camera-distance clamp; relocation installs 0x400 when zero.
    uint32_t maximumDistanceOrSurfaceLimitQ12; // Upper distance/surface limit; relocation installs 0x7f000 when zero.
    UiPixelCoordinate capturedPointerX; // Captured pointer X used as drag origin.
    UiPixelCoordinate capturedPointerY; // Captured pointer Y used as drag origin.
    UiPointerWheelDelta capturedWheelDelta; // Captured wheel/button delta state.
    uint32_t worldObjectCount; // WorldRuntimeContext.objectCount of the in-game world view; cleared by relocation.
    uint32_t clearTransientStateCallback; // WorldRuntimeContext.fieldRegion.clearTransientStateCallback, called after each right-drag step; the menu room installs none.
    uint32_t selectedResourceMarkerIndex; // Low byte is SelectionOverlay_DrawResourceCellMarkers' selected resource (0 Xenite, 1 Tritium).
    AngleTurn32 auxiliaryOrientationAngle0; // First auxiliary orientation angle in the frontend draw view; overlaps world field-region width.
    AngleTurn32 auxiliaryOrientationAngle1; // Second auxiliary orientation angle in the frontend draw view; overlaps world field-region height.
    uint32_t workspaceDwordArray; // WorldRuntimeContext.dwordArray of the in-game world view; unused by the frontend paths.
    uint32_t workspaceDwordCount; // WorldRuntimeContext.dwordArrayCount of the in-game world view; unused by the frontend paths.
    Ptr32<struct GraphicsPrimitiveQueue> activePrimitiveQueue; // Primitive queue captured from GraphicsPrimitiveQueue_ResetGlobal for all model/terrain passes.
    uint32_t runtimeControlFlags; // WorldRuntimeContext.runtimeControlFlags of the in-game world view; unused by the frontend paths.
    Ptr32<RuntimeSpinLockValue> renderSpinLock; // Spin lock acquired around graphics queue construction.
    Ptr32<void ()> renderSpinLockReleaseCallback; // Callback passed to g_SpinLockReleaseAndInvoke between rendering stages.
    Ptr32<struct ModelRuntimeNode> candidateModelListHead; // Head traversed through ModelRuntimeNode.common.nextNode.
    uint32_t activePlayerRuntimeId; // WorldRuntimeContext.selection.activePlayerRuntimeId of the in-game world view; not consumed by the frontend selection paths.
    Ptr32<struct ModelRuntimeNode> selectedModelNode; // Model-node half of the model selector's result.
    int selectedHitMetric; // Hit-metric half of the model selector's result.
    uint32_t surfaceHitWorldX; // Terrain point under the cursor: world X interpolated by the terrain triangle pick; passed to the pointer callbacks.
    uint32_t surfaceHitWorldY; // Terrain point under the cursor: world Y interpolated by the terrain triangle pick; passed to the pointer callbacks.
    uint32_t surfaceHitDepth; // View depth of the terrain hit (WORLD_POINTER_NO_HIT when none); passed to the pointer callbacks.
    Q12 cursorWorldXQ12; // Cursor override X converted from pixels to Q12 for overlay hit state.
    Q12 cursorWorldYQ12; // Cursor override Y converted from pixels to Q12 for overlay hit state.
    Ptr32<struct GameEntityRuntime> selectedOverlayEntity; // Optional selected entity used by SelectionInfo/army overlay rendering; relocation clears it.
    Ptr32<Bool8 (UiKeyboardStateMask, UiActionId, struct UiRootNode *)> keyboardFallback; // root keyboard fallback callback; the bool result is the status
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> hoverCursorCallback; // Pointer move with no button held: returns the cursor frame (surface hit depth/Y/X, hit metric, hit model, context).
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> heldButtonCursorCallback; // Pointer move while a non-right button is held (ROUTE_TO_SECONDARY_CALLBACK): returns the cursor frame.
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> buttonPressCallback; // Non-right button press (FrontendModelPointerContext_NonRightPress).
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> buttonDragCallback; // Non-right button drag (FrontendModelPointerContext_NonRightDrag).
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> buttonReleaseCallback; // Non-right button release (FrontendModelPointerContext_NonRightRelease).
    Ptr32<void (struct FrontendModelPointerContext *)> rightClickCallback; // Invoked on right release when rightButtonHeldTicks < 7 (unsigned), i.e. a click rather than a camera drag; receives the context.
    uint32_t rightButtonHeldTicks; // Ticks the right button has been held: cleared on press, counted by FrontendModelPointerContext_Tick, compared against 7 on release.
    GraphicsSceneExtentFixed sceneBound0; // Exact bound0 input copied by Graphics_SetSceneBoundsAndColors; axis interpretation remains unresolved.
    GraphicsSceneExtentFixed sceneBound1; // Exact bound1 input copied by Graphics_SetSceneBoundsAndColors; axis interpretation remains unresolved.
    GraphicsSceneExtentFixed sceneBound2; // Exact bound2 input copied by Graphics_SetSceneBoundsAndColors; axis interpretation remains unresolved.
    GraphicsSceneExtentFixed sceneBound3; // Exact bound3 input copied by Graphics_SetSceneBoundsAndColors; axis interpretation remains unresolved.
    GraphicsSceneExtentFixed sceneBound4; // Exact bound4 input copied by Graphics_SetSceneBoundsAndColors; axis interpretation remains unresolved.
    GraphicsSceneExtentFixed sceneBound5; // Exact bound5 input copied by Graphics_SetSceneBoundsAndColors; axis interpretation remains unresolved.
    GraphicsSceneExtentFixed sceneBound6; // Exact bound6 input copied by Graphics_SetSceneBoundsAndColors; axis interpretation remains unresolved.
    GraphicsSceneExtentFixed sceneBound7; // Exact bound7 input copied by Graphics_SetSceneBoundsAndColors; axis interpretation remains unresolved.
    uint8_t reserved140_15B[28]; // Observed but not semantically resolved in this pass.
    Ptr32<void (GraphicsBooleanState, struct WorldRuntimeContext *)> renderPhaseCallback; // In-game world-overlay render phase callback stored immediately after WorldRuntimeContext; the installed target is InGameWorldOverlay_RebuildOrReleaseTransientMarkers.
    UiPixelCoordinate dragFrameStartX; // Pointer X of the non-right press (NonRightPress); first corner of the selection-overlay drag frame in DrawClipped.
    UiPixelCoordinate dragFrameStartY; // Pointer Y of the non-right press (NonRightPress); first corner of the selection-overlay drag frame in DrawClipped.
    UiPixelCoordinate dragFrameEndX; // Pointer X of the latest non-right drag (NonRightDrag); second corner of the drag frame.
    UiPixelCoordinate dragFrameEndY; // Pointer Y of the latest non-right drag (NonRightDrag); second corner of the drag frame.
    Ptr32<int> terrainMarkerCoordinatePairs; // Pointer consumed as int coordinate pairs by SelectionOverlay_DrawTerrainPointMarkers.
    int terrainMarkerPointCount; // Point count paired with terrainMarkerCoordinatePairs.
    uint32_t reserved178; // Trailing dword; never accessed.
};

/* A FrontendModelPointerContext as the world runtime context it shares its storage with (see the field comments
   above): the menu room node of the frontend (ROM transitions, menu room scene, debug overlay) and the world view
   node of the in-game UI (InGameUi_WorldRuntime). */
inline WorldRuntimeContext *FrontendModelPointerContext_AsWorldRuntime(FrontendModelPointerContext *context)
{
  return reinterpret_cast<WorldRuntimeContext *>(context);
}

/* GraphicsShadingGeneratedTexture_* view of the FrontendModelPointerContext it is called with; the spans follow
   that struct's offsets (0x54, 0xB8, 0xC8 on 32-bit), so the view stays right where pointers are wider (5f). */
struct GeneratedTextureRenderContextView {
    uint8_t reserved00_53[__builtin_offsetof(struct FrontendModelPointerContext, fieldGrid)]; // Unresolved prefix; caller is FrontendModelPointerContext_DrawClipped control object.
    Ptr32<struct FieldGridAsset> fieldGrid; // World/terrain grid consumed by generated-texture surface probes.
    uint8_t reserved58_B7[__builtin_offsetof(struct FrontendModelPointerContext, auxiliaryOrientationAngle0) -
                          __builtin_offsetof(struct FrontendModelPointerContext, fieldGrid) - THANDOR_PTR32_BYTES]; // Unresolved context fields.
    AngleTurn32 lightAzimuthAngle; // Azimuth of the shadow-casting light direction (second FixedMath_DirectionFromAnglesScaled argument).
    AngleTurn32 lightElevationAngle; // Elevation of the shadow-casting light direction (first argument).
    uint8_t reservedC0_C7[__builtin_offsetof(struct FrontendModelPointerContext, activePrimitiveQueue) -
                          __builtin_offsetof(struct FrontendModelPointerContext, auxiliaryOrientationAngle1) - sizeof(AngleTurn32)]; // Unresolved context fields.
    Ptr32<struct GraphicsPrimitiveQueue> projectedPointBlockPool; // FrontendModelPointerContext.activePrimitiveQueue: the queue whose packet pool the reserve/rollback helpers take blocks from.
};

struct GeneratedTextureSampleWorkRecord {
    struct GraphicsFixedVec3 worldPoint;
    int32_t textureCoordinateOffsetQ20;
    uint32_t reserved10;
    Q12 terrainRayDistanceQ12;
};

struct GeneratedTextureScratchRuntime {
    int32_t projectedMinX;
    int32_t projectedMinY;
    int32_t projectedMaxX;
    int32_t projectedMaxY;
    uint32_t downsampleBorderOffset;
    uint32_t reserved14;
    struct GraphicsFixedMatrix3x4 modelToGeneratedTextureTransform;
    struct GraphicsFixedMatrix3x4 generatedTextureBasisTransform;
    struct GeneratedTextureSampleWorkRecord samples[12];
    struct GraphicsFixedVec3 currentModelOriginQ12;
    uint32_t reserved1A4;
};
using GraphicsOffscreenRenderModelListToTextureSourceProc = GraphicsTextureSourceAsset * (GraphicsOffscreenSceneExtents * sceneExtents, AngleTurn32 * auxiliaryOrientationAngles, GraphicsOffscreenViewParameters * viewParameters, GraphicsPixelDimension outputHeight, GraphicsPixelDimension outputWidth, ModelRuntimeCount modelCount, ModelRuntimeNode * * modelNodes);
using GraphicsPrimitiveQueueRadixSortProc = void (GraphicsBooleanState halveVertexRgb, GraphicsPrimitiveQueue * queue);

#endif /* THANDOR_GRAPHICS_RENDER_TYPES_H */
