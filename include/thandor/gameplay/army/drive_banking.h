/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/drive_banking.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_DRIVE_BANKING_H
#define THANDOR_GAMEPLAY_ARMY_DRIVE_BANKING_H

#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Glider banking (runtime class 17, classLinkState.modelLinkOrState): the bank elevation is level at the quarter turn,
   drops by ARMY_GLIDER_BANK_STEP_ANGLE16 per tick while turning down to ARMY_GLIDER_BANK_MAX_ANGLE16
   and recovers by ARMY_GLIDER_BANK_RECOVER_ANGLE16; the bank heading turns at most
   ARMY_GLIDER_BANK_TURN_LIMIT_ANGLE16 per tick */
#define ARMY_GLIDER_BANK_LEVEL_ANGLE16 FIXED_ANGLE16_QUARTER_TURN
inline constexpr int ARMY_GLIDER_BANK_MAX_ANGLE16 = 0x3800;
inline constexpr int ARMY_GLIDER_BANK_STEP_ANGLE16 = 0x100;
inline constexpr int ARMY_GLIDER_BANK_RECOVER_ANGLE16 = 0x40;
inline constexpr int ARMY_GLIDER_BANK_TURN_LIMIT_ANGLE16 = 0x400;
/* ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation: the three child parts spin by this angle16 per tick
   (about 1/96 turn) while behaviour bit 0 is set */
inline constexpr int ARMY_SPIN_CHILD_STEP_ANGLE16 = 0x2aa;

void ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime );

#endif /* THANDOR_GAMEPLAY_ARMY_DRIVE_BANKING_H */
