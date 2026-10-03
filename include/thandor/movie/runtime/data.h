/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/movie/runtime/data.h
 */

#ifndef THANDOR_MOVIE_RUNTIME_DATA_H
#define THANDOR_MOVIE_RUNTIME_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern LocaleCopyDefaultComputerLabelUtf16Proc *g_LocaleCopyDefaultComputerLabelUtf16;

extern FileSystemGetPositionProc *g_FileSystemGetPosition;

extern uint32_t g_MovieChromaLumaToArgb[1024][32];

extern uint64_t g_MovieDeltaRgbHighNibbleMask2Pixels;

extern MovieRuntime *g_ActiveMovie;

extern uint32_t g_MoviePlaybackCurrentFrame;

extern UiCommandDispatchRecord g_EndMovieCommandDispatchRecords[3]; /* 2 records + the terminator record [2] at 005658D8 (command code 0 ends the dispatch scan, which also reads its modifier class 0x90909090; the original's record ended after those 8 bytes, the original code followed at 005658E0, so .continuationEntryAddress of [2] is not original data and never read) */

#endif
