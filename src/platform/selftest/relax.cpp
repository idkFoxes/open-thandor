/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/relax.cpp
 */

/*
OPEN_THANDOR_SELFTEST=relaxcmp: differential test of the four water relaxation passes
TerrainGrid_RelaxNeighborHeights{ForwardWithSignGate,ReverseWithSignGate,Forward,Reverse}
(world/terrain/grid.c, 0x505AA0..0x505F9B) against a copy of the ORIGINAL machine code.

Each run builds a field grid of 3..40 x 3..40 cells with random terrain heights, water deltas (some
negative) and the FIELD_CELL_FLUID_SOURCE_EXCLUDED / RECEIVER_EXCLUDED bits, runs both versions on
identical copies and compares the whole grid. The passes use no absolute data, so any build works;
thandor_original.exe must be next to the exe.
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "thandor/platform/bootstrap/image.h"
#include "thandor/platform/debug/original_code.h"
#include "thandor/world/terrain/water_relaxation.h"

typedef void (__stdcall *OriginalRelaxProc)(void *fieldGrid);
typedef void (*CurrentRelaxProc)(FieldGridAsset *fieldGrid);

static void Relax_Current0(FieldGridAsset *g) { TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(g); }
static void Relax_Current1(FieldGridAsset *g) { TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(g); }
static void Relax_Current2(FieldGridAsset *g) { TerrainGrid_RelaxNeighborHeightsForward(g); }
static void Relax_Current3(FieldGridAsset *g) { TerrainGrid_RelaxNeighborHeightsReverse(g); }

void Thandor_SelfTestRelaxCompare(void)
{
    static const struct {
        const char *name;
        unsigned start;
        unsigned end;
        CurrentRelaxProc current;
    } passes[4] = {
        {"ForwardWithSignGate", 0x505aa0, 0x505bd8, Relax_Current0},
        {"ReverseWithSignGate", 0x505be0, 0x505d28, Relax_Current1},
        {"Forward", 0x505d30, 0x505e5b, Relax_Current2},
        {"Reverse", 0x505e60, 0x505f9b, Relax_Current3},
    };
    unsigned seed = 4711;
    int pass;
    int totalFailures = 0;

    for (pass = 0; pass < 4; pass++) {
        OriginalRelaxProc original =
            (OriginalRelaxProc)Thandor_LoadOriginalCodeCopy(passes[pass].start, passes[pass].end - passes[pass].start);
        int run;
        int failures = 0;
        if (original == NULL) {
            Thandor_Log("relaxcmp: could not load the original code");
            return;
        }
        for (run = 0; run < 200; run++) {
            uint32_t width;
            uint32_t height;
            size_t bytes;
            uint8_t *mine;
            uint8_t *theirs;
            uint32_t i;
            seed = seed * 1103515245u + 12345u;
            width = 3 + (seed >> 16) % 38;
            seed = seed * 1103515245u + 12345u;
            height = 3 + (seed >> 16) % 38;
            bytes = 0x200 + (size_t)width * height * 0x80;
            mine = (uint8_t *)calloc(1, bytes);
            theirs = (uint8_t *)malloc(bytes);
            if (mine == NULL || theirs == NULL) {
                Thandor_Log("relaxcmp: allocation failed");
                return;
            }
            *(uint32_t *)(mine + 0xb8) = width;
            *(uint32_t *)(mine + 0xbc) = height;
            for (i = 0; i < width * height; i++) {
                uint8_t *cell = mine + 0x200 + (size_t)i * 0x80;
                uint32_t flags;
                seed = seed * 1103515245u + 12345u;
                *(int32_t *)(cell + 0x48) = (int32_t)((seed >> 8) % 0x40000) - 0x10000;
                seed = seed * 1103515245u + 12345u;
                *(int32_t *)(cell + 0x4c) = (int32_t)((seed >> 8) % 0x20000) - 0x4000;
                seed = seed * 1103515245u + 12345u;
                flags = seed & 0x9fffffffu;
                if ((seed >> 28) % 5 == 0) {
                    flags |= 0x40000000u;
                }
                if ((seed >> 24) % 5 == 0) {
                    flags |= 0x20000000u;
                }
                *(uint32_t *)(cell + 0x50) = flags;
            }
            memcpy(theirs, mine, bytes);
            passes[pass].current((FieldGridAsset *)mine);
            original(theirs);
            if (memcmp(mine, theirs, bytes) != 0) {
                if (failures < 3) {
                    for (i = 0; i < width * height; i++) {
                        if (memcmp(mine + 0x200 + (size_t)i * 0x80, theirs + 0x200 + (size_t)i * 0x80, 0x80) != 0) {
                            Thandor_Log("relaxcmp: %s run %d (%ux%u): first difference at cell %u (x %u, y %u)",
                                        passes[pass].name, run, width, height, i, i % width, i / width);
                            break;
                        }
                    }
                }
                failures++;
            }
            free(mine);
            free(theirs);
        }
        Thandor_Log("relaxcmp: %s: %d of 200 runs differ", passes[pass].name, failures);
        totalFailures += failures;
    }
    Thandor_Log("relaxcmp: %d failures", totalFailures);
}
