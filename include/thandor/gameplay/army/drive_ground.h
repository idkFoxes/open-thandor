/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/drive_ground.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_DRIVE_GROUND_H
#define THANDOR_GAMEPLAY_ARMY_DRIVE_GROUND_H

#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Tracked vehicles: the track texture U offsets are kept within +-one texture width (Q20) */
#define ARMY_TRACK_TEXTURE_U_WRAP 0x100000

void ArmyGroundMovement_ApplyRecoilTilt (ModelRuntimeGroundMovementSteeringView *modelRuntime,ModelRuntimeNode *rootNode,int recoilTilt);

void ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime );

void ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementTrackView *modelRuntime);

void ArmyRuntimeClass_UpdateGroundMovement (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime );

void ArmyRuntimeClass_UpdateWaterSurfaceMovement (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime );

#endif /* THANDOR_GAMEPLAY_ARMY_DRIVE_GROUND_H */
