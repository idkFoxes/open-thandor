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
     imagecmp       checks src/generated/image_data.c against thandor_original.exe
     scanaddr       decodes the packages next to the executable and writes original-range dwords to
                    scanaddr.txt (OPEN_THANDOR_SCANFILES=a;b;... scans those files instead,
                    OPEN_THANDOR_DUMPTEXT=<dir> also writes the decoded *.str / *.txt entries there)
     stretchcmp     C bilinear stretches against the original machine code
     rastercmp      software triangle rasterizer handlers against the original (raster.c, mapped build)
     blendscalecmp  SoftwareTexture_BilinearBlendScaleSubresources against the original (blendscale.c, mapped build)
     blitcmp        texture-source blits, ARGB fills, mask-buffer step against the original (blit.c, mapped build)
     relaxcmp       the four water relaxation passes against the original (relax.c)
     crash          writes to address 0 to exercise the crash handler, then (if it returns) starts the game
   The *cmp tests read the original bytes from thandor_original.exe next to the executable. */

/* Runs the self-test that name (the value of OPEN_THANDOR_SELFTEST, may be NULL) selects.
   Returns nonzero when a test ran (the caller then exits), 0 for NULL, "crash" or an unknown name. */
int SelfTest_Run(const char *name);

/* Differential tests against the original machine code (raster.c, blendscale.c, blit.c, relax.c). */
void Thandor_SelfTestRasterCompare(void);
void Thandor_SelfTestBlendScaleCompare(void);
void Thandor_SelfTestBlitCompare(void);
void Thandor_SelfTestRelaxCompare(void);

#endif /* THANDOR_PLATFORM_SELFTEST_SELFTEST_H */
