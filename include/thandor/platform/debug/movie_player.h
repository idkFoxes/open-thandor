/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/movie_player.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_MOVIE_PLAYER_H
#define THANDOR_PLATFORM_DEBUG_MOVIE_PLAYER_H

/* Debug movie tools, started from Game_PlayIntroMovies by environment switches.
   OPEN_THANDOR_MOVIE=<name>|all plays flm\<name>.flm or every name in movies.txt with a name and frame counter
   overlay (OPEN_THANDOR_MOVIE_STRETCH=1: full screen, OPEN_THANDOR_MOVIE_START=<n>: resume the list at n);
   OPEN_THANDOR_MOVIEEXPORT=<name>[,<name>...] writes the decoded frames and audio to moviedump\.
   Both exit the process when done. */

/* Plays one movie or the list in movies.txt ("all"), then exits the process. */
void DebugMovie_Run(const char *which);

/* Writes moviedump\<name>.rgb, .wav and .txt for flm\<name>.flm. */
void DebugMovie_ExportOne(const char *name);

#endif /* THANDOR_PLATFORM_DEBUG_MOVIE_PLAYER_H */
