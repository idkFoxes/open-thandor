/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/music_choice.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_MUSIC_CHOICE_H
#define THANDOR_UI_INGAME_MUSIC_CHOICE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/music_choice. */

/* Functions are grouped by semantic ownership. */

uint32_t InGameMusic_ComputeTrackSuitabilityScore(MusicTrackClassId trackClassId,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_UI_INGAME_MUSIC_CHOICE_H */
