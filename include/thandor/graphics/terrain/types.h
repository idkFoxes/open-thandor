/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/terrain/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_TERRAIN_TYPES_H
#define THANDOR_GRAPHICS_TERRAIN_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>

struct TerrainMaterialSuffixEntry;
struct TerrainProjectedVertexWorkRecord;
struct TriangleBarycentricWeightsQ12;
struct TerrainProjectedRowSpan;

struct TerrainMaterialSuffixEntry {
    uint16_t lowercaseLetterUtf16;
    uint16_t terminator;
};

struct TerrainProjectedVertexWorkRecord {
    int surfacePacketIndex; // Index into g_TerrainSurfacePacketTablePayload; proven by TerrainProjectedQuad_QueueAsTwoTriangles -> TerrainProjectedTriangle_ClipInterpolateAndQueueTextured and 32-byte table stride.
    PackedArgb32 basePackedColor; // Packed color multiplier read by both shading variants
    uint8_t reserved08_0B[4]; // Unresolved
    struct GraphicsProjectedPointPair projectedPointA; // Projected screen point (X, Y) for viewPointA.
    struct GraphicsFixedVec3 viewPointA; // First transformed view-space point
    uint8_t reserved20_2B[12]; // Unresolved
    struct GraphicsProjectedPointPair projectedPointB; // Projected screen point (X, Y) for viewPointB.
    struct GraphicsFixedVec3 viewPointB; // Second transformed view-space point
    struct GraphicsFixedVec3 sourcePoint; // Mutable source point temporarily offset before second projection
    Q12 secondaryProjectionDepthQ12; // Q12 depth/extent added to sourcePoint.z for the secondary projection
    uint32_t projectionFlags; // Projection/clip and shading flags
    uint32_t secondaryOffset; // FieldGridCell.persistedAux54: index 0..255 of the terrain direction record whose first three dwords are the offset added to sourcePoint for the second projection (the original held the record's address)
    PackedArgb32 packedColorA; // Packed color input A
    PackedArgb32 packedColorB; // Packed color input B
    PackedArgb32 shadedColorA; // Computed packed shaded color A
    PackedArgb32 shadedColorB; // Computed packed shaded color B
    uint32_t lightingLookupIndexOrSentinel; // Packed-light lookup selector; 0xff selects dynamic compact-light accumulation
    uint8_t reserved6C_7F[20]; // Unresolved tail to proven 0x80 grid stride
};

/* The primitive queue takes the terrain vertices with the GraphicsProjectedVertexSource type (the model vertex
   record) and reads them back as TerrainProjectedVertexWorkRecords: the same 0x80-byte record either way. */
static inline GraphicsProjectedVertexSource *TerrainVertex_AsProjectedSource(TerrainProjectedVertexWorkRecord *vertex)
{
    return reinterpret_cast<GraphicsProjectedVertexSource *>(vertex);
}
static inline const TerrainProjectedVertexWorkRecord *TerrainVertex_FromProjectedSource(const GraphicsProjectedVertexSource *vertex)
{
    return reinterpret_cast<const TerrainProjectedVertexWorkRecord *>(vertex);
}

struct TriangleBarycentricWeightsQ12 {
    Q12 weightVertexB_Q12; // Interpolation weight applied to vertex B relative to vertex C.
    Q12 weightVertexA_Q12; // Interpolation weight applied to vertex A relative to vertex C.
};
struct TerrainProjectedRowSpan {
    int firstColumn;
    int endColumnExclusive;
};

#endif /* THANDOR_GRAPHICS_TERRAIN_TYPES_H */
