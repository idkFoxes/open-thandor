/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/movie_decoder.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_MOVIE_DECODER_H
#define THANDOR_PLATFORM_DEBUG_MOVIE_DECODER_H

#include <stdint.h>
#include <thandor/generated/types.h>

/* Debug tools for the movie frame decoder, hooked into Movie_AdvanceFrame around Movie_DecodeFrame4x4Delta.
   OPEN_THANDOR_MOVIEDUMP=1 logs every decoded frame and writes every tenth frame to moviedump\frame_NNNN.bmp;
   OPEN_THANDOR_MOVIECMP=1 decodes every frame a second time with the original machine code and logs where the
   results differ. Both read their switch once and do nothing when it is not set. */

/* Logs the decoded frame (consumed bytes, stream state, checksum); every tenth frame also as a BMP. */
void DebugMovieDecoder_DumpFrame(MovieRuntime *movie, uint32_t consumedBytes);

/* Runs the original decoder into a shadow buffer; call before the C decoder. */
void DebugMovieDecoder_CompareBefore(MovieRuntime *movie, uint32_t height, uint32_t width, uint8_t *encoded);

/* Compares the C decoder's result with the shadow buffer and logs differences; call after the C decoder. */
void DebugMovieDecoder_CompareAfter(MovieRuntime *movie, uint32_t height, uint32_t width, uint32_t consumed);

#endif /* THANDOR_PLATFORM_DEBUG_MOVIE_DECODER_H */
