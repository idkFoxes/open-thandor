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
     hexscan        golden hashes of the hexagonal radius scans in src/world/terrain (hexscan_selftest.cpp): the
                    overlay A/B, occupancy, flatten, height-band, auxiliary placement and sight drivers at seeded
                    points on a synthetic field grid, one hash line per driver
     keymatch       the key command modifier matcher (keymatch_selftest.cpp): literal copies of the four old
                    predicates against UiKeyModifiers_Match for every table class and held 0..0x3F, and
                    UiCommandDispatch_Find against the old scan; logs mismatches and one hash line
     uitemplate     the bytes of the five UI template images (InGameUiImage, FrontendUiImage,
                    DisplaySettingsUiImage, FourValueDialogUiImage, FatalErrorUiImage) and of an in-game root
                    copied and relocated from its template (uitemplate_selftest.cpp), pointers to module objects
                    hashed by name and offset and links by image offset; one hash line per image
     tables         hashes of the tables computed at startup (sine table built with sin(), .sam cosine
                    matrices, lighting/shading/software factor tables), to compare builds or compilers
     sam            the .sam decoder on LCG bytes and an encoder round trip of a synthetic waveform, one hash
                    line (no game files needed)
     uiatlas        packs every image of every 'gfx' asset in the packages of the current directory into the GPU UI
                    texture cache without a GPU device (uiatlas_selftest.cpp): logs pages, texels, packing fill and
                    a hash of the converted texels, writes uiatlas.txt (one line per asset) and checks per-frame
                    validation, re-conversion of a changed image, streaming regions and release eviction
     crash          writes to address 0 to exercise the crash handler, then (if it returns) starts the game
   The differential tests that ran the original machine code (stretchcmp, relaxcmp and the movie decoder
   compare) needed the 32-bit original exe and were removed with the 32-bit build; they had confirmed those
   functions before. */

/* Runs the self-test that name (the value of OPEN_THANDOR_SELFTEST, may be NULL) selects.
   Returns nonzero when a test ran (the caller then exits), 0 for NULL, "crash" or an unknown name. */
int SelfTest_Run(const char *name);

/* OPEN_THANDOR_SELFTEST=raster (raster_selftest.cpp), called by SelfTest_Run. */
void Thandor_SelfTestRaster();

/* OPEN_THANDOR_SELFTEST=hexscan (hexscan_selftest.cpp), called by SelfTest_Run. */
void Thandor_SelfTestHexScan(void);

/* OPEN_THANDOR_SELFTEST=keymatch (keymatch_selftest.cpp), called by SelfTest_Run. */
void Thandor_SelfTestKeyMatch(void);

/* OPEN_THANDOR_SELFTEST=uitemplate (uitemplate_selftest.cpp), called by SelfTest_Run. */
void Thandor_SelfTestUiTemplate();

/* OPEN_THANDOR_SELFTEST=uiatlas (uiatlas_selftest.cpp), called by SelfTest_Run. */
void Thandor_SelfTestUiAtlas();

#endif /* THANDOR_PLATFORM_SELFTEST_SELFTEST_H */
