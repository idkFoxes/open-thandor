/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/script.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_SCRIPT_H
#define THANDOR_PLATFORM_DEBUG_SCRIPT_H

/* Scripted input (test aid). Reads OPEN_THANDOR_SCRIPT=<file> and replays its timed commands (click, rclick,
   move, drag, key, shot, quit, layout, ingame, clickuntilingame, clickuntilnextlevel) as pointer and key
   input; see script.cpp for the format. Also defines g_TestAidInGameFrames and g_TestAidSessionCount (declared
   in thandor/platform/debug/test_aids.h). */

/* Called from the message pump (SdlPlatform_PumpEvents); runs the script lines that are due. */
void DebugScript_Tick(void);

#endif /* THANDOR_PLATFORM_DEBUG_SCRIPT_H */
