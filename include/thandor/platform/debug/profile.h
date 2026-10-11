/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/profile.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_PROFILE_H
#define THANDOR_PLATFORM_DEBUG_PROFILE_H

/* Step profiler (developer tools, THANDOR_DEV_TOOLS). OPEN_THANDOR_STEP_PROFILE=<N> (1 = 100) measures the wall
   time of every simulation step (including the lockstep right before it, where the players' commands run), of
   every in-game frame and of the profiled sections (DebugProfileSection, entered
   and left through DebugHook_ProfileEnter/Leave; inclusive times, nested sections count in both; work between two
   steps counts to the next one) and writes every
   N steps one block to OPEN_THANDOR_STEP_PROFILE_FILE (default stepprofile.txt in the working directory):
     W <firstTick> <lastTick> <steps> step <mean> <p99> <max> frame <mean> <p99> <max> [us]
     S <name> <calls/step> <us/step> <max us in one step> <max calls in one step>
   and the same as "C" (cumulative since the session start) before it. OPEN_THANDOR_STEP_PROFILE_RAW=<file> also
   writes one line per step: <tick> <step us> <us per section...>. Reads the clock only; nothing changes the
   simulation. */

void DebugProfile_StepBegin();
void DebugProfile_StepEnd(unsigned simulationTick);
void DebugProfile_FrameBegin();
void DebugProfile_FrameEnd();
void DebugProfile_SessionStarted();
void DebugProfile_Enter(int section);
void DebugProfile_Leave(int section);

#endif /* THANDOR_PLATFORM_DEBUG_PROFILE_H */
