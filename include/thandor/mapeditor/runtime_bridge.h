#pragma once

#include <stdint.h>

struct MapEditorRuntimeAngles16 {
    uint32_t headingAngle;
    uint32_t elevationAngle;
};

uint32_t MapEditorRuntime_FixedAtan2Angle16(int32_t y, int32_t x);
uint32_t MapEditorRuntime_UInt64Sqrt(uint32_t high, uint32_t low);
MapEditorRuntimeAngles16 MapEditorRuntime_VectorToAnglesQ12(int32_t zQ12, int32_t yQ12, int32_t xQ12);

MapEditorRuntimeAngles16 MapEditorRuntime_ComputeShotLaunchAnglesQ12(
    int32_t targetZQ12, int32_t targetYQ12, int32_t targetXQ12,
    int32_t launchZQ12, int32_t launchYQ12, int32_t launchXQ12,
    int32_t trajectoryMode, int32_t launchSpeedQ12, int32_t ballisticDivisorQ12,
    int32_t elevationOffsetAngle16);

uint32_t MapEditorRuntime_ComputeShotSelectionRangeQ12(
    int32_t trajectoryMode, int32_t launchSpeedQ12, int32_t ballisticDivisorQ12,
    uint32_t projectileLifetimeTicks, uint32_t trajectoryRampDurationTicks,
    int32_t mode2SelectionRangeQ12);

void* MapEditorRuntime_Allocate(uint32_t bytes);
void MapEditorRuntime_Free(void* memory);
