/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_RUNTIME_H
#define THANDOR_GAMEPLAY_SESSION_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/runtime. */

extern InGameSimulationStepBatchTicks g_InGameSimulationStepTicks;

extern uint32_t g_SessionNetworkTickCounter;

extern uint32_t g_InGameNetworkTickCountdown;
extern uint32_t g_InGameStateTickSpinLock;

extern InGameRuntimeRoot *g_InGameRuntimeRoot;

extern SessionNetworkRoleFlags g_SessionNetworkRoleFlags;
extern uint32_t g_SessionNetworkTickInterval; /* uint32_t network lockstep interval in simulation steps (2 * the frontend speed slider value); sent in the join ack */

#endif /* THANDOR_GAMEPLAY_SESSION_RUNTIME_H */
