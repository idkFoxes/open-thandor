/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/autoshot.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_AUTOSHOT_H
#define THANDOR_PLATFORM_DEBUG_AUTOSHOT_H

/* Automatic screenshots (test aid). Reads OPEN_THANDOR_AUTOSHOT=<milliseconds>: at that interval the game's own
   framebuffer is saved to shots\shot_NNNN.bmp. */

/* Called from the message pump (Win32_PumpMessages); saves a shot when the interval has passed. */
void DebugAutoShot_Tick(void);

/* Saves the framebuffer now as shots\script_NNNN.bmp (the script command "shot"); creates the folder. */
void DebugAutoShot_SaveNow(void);

#endif /* THANDOR_PLATFORM_DEBUG_AUTOSHOT_H */
