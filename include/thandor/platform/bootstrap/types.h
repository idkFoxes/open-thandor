/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/bootstrap/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_BOOTSTRAP_TYPES_H
#define THANDOR_PLATFORM_BOOTSTRAP_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/scenario/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct HINSTANCE__ HINSTANCE__, *PHINSTANCE__;
typedef struct LevelArchivePathTemplate18 LevelArchivePathTemplate18, *PLevelArchivePathTemplate18;
typedef struct PatchArchivePathTemplate18 PatchArchivePathTemplate18, *PPatchArchivePathTemplate18;
typedef struct GameDataAuxState GameDataAuxState, *PGameDataAuxState;
typedef struct DynamicModuleEntry DynamicModuleEntry, *PDynamicModuleEntry;
typedef struct DynamicApiBinding DynamicApiBinding, *PDynamicApiBinding;
typedef struct CommandLineArgumentMirrorState500 CommandLineArgumentMirrorState500, *PCommandLineArgumentMirrorState500;
typedef struct CommandLineWideArguments CommandLineWideArguments, *PCommandLineWideArguments;

typedef struct HINSTANCE__ *HINSTANCE;

struct HINSTANCE__ {
    int unused;
};

using CommandLineOptionLengthBytes = uint32_t;
#pragma pack(push, 1) /* packed layout: no alignment padding */
struct LevelArchivePathTemplate18 {
    uint16_t prefixCodeUnits[5]; 
    union Utf16DecimalDigitPair4 decimalDigits; 
    uint16_t suffixCodeUnits[5]; 
};
#pragma pack(pop)
#pragma pack(push, 1) /* packed layout: no alignment padding */
struct PatchArchivePathTemplate18 {
    uint16_t prefixCodeUnits[5]; 
    union Utf16DecimalDigitPair4 decimalDigits; 
    uint16_t suffixCodeUnits[5]; 
};
#pragma pack(pop)

struct GameDataAuxState {
    uint32_t pairPressureMatrix8x8[64]; 
};

struct DynamicModuleEntry {
    HINSTANCE module; 
    char *name; 
};

struct DynamicApiBinding {
    void **destination; 
    char *moduleName; 
};

struct CommandLineArgumentMirrorState500 {
    char executablePath[256]; 
    char argument1[256]; 
    char argument2[256]; 
    char argument3[256]; 
    char optionBuffer[256]; 
};

struct CommandLineWideArguments {
    uint16_t argument1[256]; 
    uint16_t argument2[256]; 
    uint16_t argument3[256]; 
};
using CommandLineFindOptionProc = uint8_t * (uint32_t length, char * option);

#endif /* THANDOR_PLATFORM_BOOTSTRAP_TYPES_H */
