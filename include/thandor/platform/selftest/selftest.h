/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/selftest/selftest.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_SELFTEST_SELFTEST_H
#define THANDOR_PLATFORM_SELFTEST_SELFTEST_H

/* Self-tests and data tools, started from WinMain (after the precomputed tables are built) when the
   environment variable OPEN_THANDOR_SELFTEST names one; the process then exits without entering
   ProcessEntry. Results go to the log (Thandor_Log). Values:
     codec          round-trips synthetic save-sized data through the PCK encoder/decoder, checks guard bytes
     path           WidePath_SplitParentAndLeaf on a few fixed paths
     stretch        a 4x2 -> 8x4 SoftwareTextureSource_StretchDirectColorBilinear32, logs the rows
     scanaddr       decodes the packages next to the executable and writes original-range dwords to
                    scanaddr.txt (OPEN_THANDOR_SCANFILES=a;b;... scans those files instead,
                    OPEN_THANDOR_DUMPTEXT=<dir> also writes the decoded *.str / *.txt entries there)
     pcx            decodes pcxtest.pcx next to the executable, logs size and hash (tools/test/pcx_check.py)
     settings       thandor.ini parser/writer on a fixed text; thandor.dat of the current directory -> ini ->
                    image must be identical (migration), and a thandor.ini there must match that thandor.dat
     icon           the .ico parser of the window icon on a synthetic icon file (pixels of 32/24/4-bit images,
                    masks, PNG and broken entries, choice of the 100% image and of the alternate sizes)
     numberformat, fixedmath, keymap, trianglesetup, movieenc
                    hash tests: log a hash over many results of the number formatter, the fixed-point math,
                    the keyboard layer, the software triangle setup and the movie encoder/decoder, to
                    compare two builds
     raster         golden hashes of the software renderer (raster_selftest.cpp): every entry of the 32-bit and
                    auxiliary triangle handler tables and every blit/fill/copy path draws seeded random
                    primitives with synthetic textures into a synthetic framebuffer, one hash line per group
     crash          writes to address 0 to exercise the crash handler, then (if it returns) starts the game
   The differential tests that ran the original machine code (stretchcmp, relaxcmp and the movie decoder
   compare) needed the 32-bit original exe and were removed with the 32-bit build; they had confirmed those
   functions before. */

/* Runs the self-test that name (the value of OPEN_THANDOR_SELFTEST, may be NULL) selects.
   Returns nonzero when a test ran (the caller then exits), 0 for NULL, "crash" or an unknown name. */
int SelfTest_Run(const char *name);

/* OPEN_THANDOR_SELFTEST=raster (raster_selftest.cpp), called by SelfTest_Run. */
void Thandor_SelfTestRaster(void);

#endif /* THANDOR_PLATFORM_SELFTEST_SELFTEST_H */
