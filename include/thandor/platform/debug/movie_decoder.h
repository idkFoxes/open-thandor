/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/movie_decoder.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_MOVIE_DECODER_H
#define THANDOR_PLATFORM_DEBUG_MOVIE_DECODER_H

#include <stdint.h>
#include <thandor/movie/runtime/types.h>

/* Debug tool for the movie frame decoder, hooked into Movie_AdvanceFrame after Movie_DecodeFrame4x4Delta:
   OPEN_THANDOR_MOVIEDUMP=1 logs every decoded frame and writes every tenth frame to moviedump\frame_NNNN.bmp. It
   reads its switch once and does nothing when it is not set. */

/* Logs the decoded frame (consumed bytes, stream state, checksum); every tenth frame also as a BMP. */
void DebugMovieDecoder_DumpFrame(MovieRuntime *movie, uint32_t consumedBytes);

#endif /* THANDOR_PLATFORM_DEBUG_MOVIE_DECODER_H */
