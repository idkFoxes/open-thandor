/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/music_choice.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_MUSIC_CHOICE_H
#define THANDOR_UI_INGAME_MUSIC_CHOICE_H

#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

uint32_t InGameMusic_ComputeTrackSuitabilityScore(MusicTrackClassId trackClassId,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_UI_INGAME_MUSIC_CHOICE_H */
