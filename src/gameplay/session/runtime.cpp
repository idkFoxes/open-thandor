/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

THANDOR_ALIGN(8) SessionNetworkRoleFlags g_SessionNetworkRoleFlags = 0;

/* uint32_t network lockstep interval in simulation steps (2 * the frontend speed slider value); sent in the join ack */
THANDOR_ALIGN(4) uint32_t g_SessionNetworkTickInterval = 2;

THANDOR_ALIGN(4) InGameRuntimeRoot *g_InGameRuntimeRoot = nullptr;

uint32_t g_SessionNetworkTickCounter = 0;

uint32_t g_InGameNetworkTickCountdown = 0;

RuntimeSpinLockValue g_InGameStateTickSpinLock = 0;

InGameSimulationStepBatchTicks g_InGameSimulationStepTicks = 0;
