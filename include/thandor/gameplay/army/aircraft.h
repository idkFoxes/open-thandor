/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/aircraft.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_AIRCRAFT_H
#define THANDOR_GAMEPLAY_ARMY_AIRCRAFT_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Aircraft (class 21) state in model runtime classState.behaviorState (ArmyRuntimeClass_UpdateAircraft) */
inline constexpr int ARMY_AIRCRAFT_STATE_NO_PAD = 0; /* no home pad left: stays where it is */
inline constexpr int ARMY_AIRCRAFT_STATE_PARKED = 1; /* parked on the home pad */
inline constexpr int ARMY_AIRCRAFT_STATE_LANDING = 2; /* landing arc back onto the pad */
inline constexpr int ARMY_AIRCRAFT_STATE_TAKING_OFF = 3; /* take-off arc, then off the map */
inline constexpr int ARMY_AIRCRAFT_STATE_APPROACH = 4; /* off the map: lines up behind the attack point */
inline constexpr int ARMY_AIRCRAFT_STATE_ATTACK_RUN = 5; /* flies over the attack point, dropping its effects */
inline constexpr int ARMY_AIRCRAFT_STATE_RETURNING = 6; /* off the map again: lines up for the landing arc */
/* Aircraft pad (class 22) hangar state in model runtime classState.classStateB0
   (ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode) */
inline constexpr int ARMY_PAD_HANGAR_IDLE = 0;
inline constexpr int ARMY_PAD_HANGAR_OPENING = 1; /* hatch texture scrolls open */
inline constexpr int ARMY_PAD_HANGAR_LIFTING = 2; /* platform (child 0) moves up */
inline constexpr int ARMY_PAD_HANGAR_READY = 3; /* platform up: a parked aircraft takes off */
inline constexpr int ARMY_PAD_HANGAR_LOWERING = 4;
inline constexpr int ARMY_PAD_HANGAR_CLOSING = 5;
inline constexpr int ARMY_PAD_HANGAR_CLOSED = 6; /* back to idle on the next update */

/* primaryTextureOffsetV of a fully open door or hatch (texture V offset, 0 = closed) */
inline constexpr int ARMY_DOOR_TEXTURE_OPEN_V = 0x80000;
/* Parking position of an aircraft that has flown off the map (ArmyRuntimeClass_UpdateAircraft) */
inline constexpr int ARMY_AIRCRAFT_OFF_MAP_X_Q12 = -0x100000;
inline constexpr int ARMY_AIRCRAFT_OFF_MAP_Y_Q12 = 0x100000;
/* ArmyAssetRecord.flags bits that select the production class able to build a queued secondary asset */
inline constexpr int ARMY_ASSET_FLAG_BUILT_AT_AIRCRAFT_PAD = 0x8; /* class-22 aircraft pad */
inline constexpr int ARMY_ASSET_FLAG_BUILT_BY_CLASS11 = 0x10; /* production class 11 */
inline constexpr int ARMY_ASSET_FLAGS_BUILT_BY_FACTORY = 0xee; /* unit factory (class 13): matched against its classParameterC4 */
inline constexpr int ARMY_ASSET_FLAG_PRODUCTION_MASK = 0xfe; /* bits 1-7: built by some production class (bit 0 = enabled);
                                                     such assets get the panel preview textures */
/* impact damage of a model crushed by a structure placed over it or run over by a vehicle
   (ArmyRuntime_ClassCommandHandlerGroupA, ArmyRuntime_HandleCollisionPartner) */
inline constexpr int ARMY_CRUSH_IMPACT_DAMAGE = 0x100000;

void ArmyRuntimeClass_UpdateAircraft (WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime);

void ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode (WorldRuntimeContext *worldRuntime, ModelRuntimeLinkedChildSpawnAndBuildView *modelRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_AIRCRAFT_H */
