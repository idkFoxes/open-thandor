/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/test_aids.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_TEST_AIDS_H
#define THANDOR_PLATFORM_DEBUG_TEST_AIDS_H

/* Test aids (developer tools, THANDOR_DEV_TOOLS; game code reaches them through thandor/platform/debug/hooks.h):
   scripted-input detection, a local two-instance network test and
   a windowed mode. Environment switches: OPEN_THANDOR_SCRIPT, OPEN_THANDOR_MULTI_INSTANCE,
   OPEN_THANDOR_NET_PORT, OPEN_THANDOR_NETLOG, OPEN_THANDOR_WINDOWED, OPEN_THANDOR_WINDOW_X,
   OPEN_THANDOR_WINDOW_Y. The script test aid counters are defined in platform/debug/script.cpp. */

/* Test aid (OPEN_THANDOR_SCRIPT `ingame`): counts in-game session frames, i.e. frames after the
   level has finished loading. */
extern volatile unsigned g_TestAidInGameFrames;
/* Test aid (script `clickuntilnextlevel`, OPEN_THANDOR_AUTOWIN): number of in-game sessions started so far
   (counted through DebugHook_SessionStarted). */
extern volatile unsigned g_TestAidSessionCount;
/* Test aids for a local two-instance network test (not in the original):
   OPEN_THANDOR_MULTI_INSTANCE=1 lets a second instance start although a game window exists;
   OPEN_THANDOR_NET_PORT=<n> binds this instance's UDP socket to port n instead of the game port. */
int Thandor_TestAidAllowSecondInstance();
/* Nonzero when OPEN_THANDOR_SCRIPT is set (scripted input; the real mouse is then ignored). */
int Thandor_TestAidScriptActive();
/* Nonzero when OPEN_THANDOR_STATEHASH is set (determinism test). The world overlay then builds no transient
   markers: they come from the render path, take effect pool slots and world objects (and possibly random
   numbers) shared with the simulation, so how many frames fall between two steps would change the hashes. */
int Thandor_TestAidStateHashActive();
/* 1 while InGameTick_RunSimulationStep runs (test builds); with OPEN_THANDOR_STATEHASH set, effect creation and
   session random draws outside a step are logged with their caller (Thandor_TestAidNoteOutsideStep). */
extern volatile int g_TestAidInSimulationStep;
void Thandor_TestAidNoteOutsideStep(const char *what, void *caller);
unsigned Thandor_TestAidNetworkBindPort(unsigned gamePort);
/* OPEN_THANDOR_NETLOG=1: logs every datagram (direction, sockaddr_in, size, first dwords). */
void Thandor_TestAidLogDatagram(const char *direction, const void *sockaddrIn, unsigned byteCount,
                                const void *buffer);
/* Windowed test aid (not in the original), so two instances fit side by side on one monitor:
   OPEN_THANDOR_WINDOWED=1 runs the game in a normal window at OPEN_THANDOR_WINDOW_X / OPEN_THANDOR_WINDOW_Y
   (default 0,0) instead of full screen, sized to the display mode, in the desktop's colour depth (the SDL3
   backend: platform/sdl3/platform.cpp, video.cpp). Off by default. */
int Thandor_TestAidWindowed();

#endif /* THANDOR_PLATFORM_DEBUG_TEST_AIDS_H */
