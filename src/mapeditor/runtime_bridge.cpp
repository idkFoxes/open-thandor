#include <thandor/mapeditor/runtime_bridge.h>

#include <thandor/assets/shot/types.h>
#include <thandor/core/math/fixed_trig.h>
#include <thandor/core/math/fixed_vector.h>
#include <thandor/core/memory/allocator.h>
#include <thandor/world/shots/ballistics.h>

uint32_t MapEditorRuntime_FixedAtan2Angle16(int32_t y, int32_t x)
{
    return FixedMath_Atan2Angle16(y, x);
}

uint32_t MapEditorRuntime_UInt64Sqrt(uint32_t high, uint32_t low)
{
    return FixedMath_UInt64Sqrt(high, low);
}

MapEditorRuntimeAngles16 MapEditorRuntime_VectorToAnglesQ12(int32_t zQ12, int32_t yQ12, int32_t xQ12)
{
    const FixedVectorAngles result = FixedMath_VectorToAngles(zQ12, yQ12, xQ12);
    return { result.azimuthAngle, result.elevationAngle };
}

MapEditorRuntimeAngles16 MapEditorRuntime_ComputeShotLaunchAnglesQ12(
    int32_t targetZQ12, int32_t targetYQ12, int32_t targetXQ12,
    int32_t launchZQ12, int32_t launchYQ12, int32_t launchXQ12,
    int32_t trajectoryMode, int32_t launchSpeedQ12, int32_t ballisticDivisorQ12,
    int32_t elevationOffsetAngle16)
{
    ShotDefinition definition{};
    definition.trajectoryMode = trajectoryMode;
    definition.launchSpeedQ12 = launchSpeedQ12;
    definition.ballisticDivisorQ12 = ballisticDivisorQ12;
    definition.elevationOffsetAngle16 = elevationOffsetAngle16;
    const ShotLaunchAngles result = ShotDefinition_ComputeLaunchAngles(
        targetZQ12, targetYQ12, targetXQ12, launchZQ12, launchYQ12, launchXQ12, &definition);
    return { result.headingAngle, result.elevationAngle };
}

uint32_t MapEditorRuntime_ComputeShotSelectionRangeQ12(
    int32_t trajectoryMode, int32_t launchSpeedQ12, int32_t ballisticDivisorQ12,
    uint32_t projectileLifetimeTicks, uint32_t trajectoryRampDurationTicks,
    int32_t mode2SelectionRangeQ12)
{
    ShotDefinition definition{};
    definition.trajectoryMode = trajectoryMode;
    definition.launchSpeedQ12 = launchSpeedQ12;
    definition.ballisticDivisorQ12 = ballisticDivisorQ12;
    definition.projectileLifetimeTicks = projectileLifetimeTicks;
    definition.trajectoryRampDurationTicks = trajectoryRampDurationTicks;
    definition.mode2SelectionRangeQ12 = mode2SelectionRangeQ12;
    return ShotDefinition_ComputeSelectionRange(&definition);
}

void* MapEditorRuntime_Allocate(uint32_t bytes)
{
    void* payload = nullptr;
    if (bytes == 0U || g_MemoryApi.alloc == nullptr || g_MemoryApi.alloc(bytes, &payload) != 0U) return nullptr;
    return payload;
}

void MapEditorRuntime_Free(void* memory)
{
    if (memory != nullptr && g_MemoryApi.free != nullptr) g_MemoryApi.free(memory);
}
