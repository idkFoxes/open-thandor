/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/army/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/assets/army/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(8)) ArmyAssetRecordPrefix *g_ArmyAssetRecordRegistry[768] = {0};

__declspec(align(16)) UiCommandDispatchRecord g_InGameKeyboardDispatchRecords[37] = {
    /*  0 */ {.commandCode = 0x30071, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x56F1C0},
    /*  1 */ {.commandCode = 0x30069, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56E670},
    /*  2 */ {.commandCode = 0x20002, .continuationEntryAddress = 0x56E6A0},
    /*  3 */ {.commandCode = 0x30070, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56F160},
    /*  4 */ {.commandCode = 0x30069, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x56E5E0},
    /*  5 */ {.commandCode = 0x30065, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x56E6E0},
    /*  6 */ {.commandCode = 0x30075, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x56E720},
    /*  7 */ {.commandCode = 0x30075, .continuationEntryAddress = 0x56E720},
    /*  8 */ {.commandCode = 0x10012, .continuationEntryAddress = 0x56ED80},
    /*  9 */ {.commandCode = 0x1001A, .continuationEntryAddress = 0x56EDC0},
    /* 10 */ {.commandCode = 0x10014, .continuationEntryAddress = 0x56E7C0},
    /* 11 */ {.commandCode = 0x10016, .continuationEntryAddress = 0x56E930},
    /* 12 */ {.commandCode = 0x10011, .continuationEntryAddress = 0x56EAA0},
    /* 13 */ {.commandCode = 0x10019, .continuationEntryAddress = 0x56EC10},
    /* 14 */ {.commandCode = 0x10014, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56E7C0},
    /* 15 */ {.commandCode = 0x10016, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56E930},
    /* 16 */ {.commandCode = 0x10011, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56EAA0},
    /* 17 */ {.commandCode = 0x10019, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x56EC10},
    /* 18 */ {.commandCode = 0x30061, .continuationEntryAddress = 0x56EE00},
    /* 19 */ {.commandCode = 0x30068, .continuationEntryAddress = 0x56EE20},
    /* 20 */ {.commandCode = 0x30067, .continuationEntryAddress = 0x56EE40},
    /* 21 */ {.commandCode = 0x30073, .continuationEntryAddress = 0x56EE60},
    /* 22 */ {.commandCode = 0x30070, .continuationEntryAddress = 0x56EEB0},
    /* 23 */ {.commandCode = 0x30066, .continuationEntryAddress = 0x56EED0},
    /* 24 */ {.commandCode = 0x30074, .continuationEntryAddress = 0x56EEF0},
    /* 25 */ {.commandCode = 0x3006C, .continuationEntryAddress = 0x56F020},
    /* 26 */ {.commandCode = 0x3006E, .continuationEntryAddress = 0x56EFB0},
    /* 27 */ {.commandCode = 0x30076, .continuationEntryAddress = 0x56F090},
    /* 28 */ {.commandCode = 0x30065, .continuationEntryAddress = 0x56F100},
    /* 29 */ {.commandCode = 0x30062, .continuationEntryAddress = 0x56F120},
    /* 30 */ {.commandCode = 0x30063, .continuationEntryAddress = 0x56EF70},
    /* 31 */ {.commandCode = 0x30064, .continuationEntryAddress = 0x56EF90},
    /* 32 */ {.commandCode = 0x30077, .continuationEntryAddress = 0x56EF10},
    /* 33 */ {.commandCode = 0x30071, .continuationEntryAddress = 0x56EF30},
    /* 34 */ {.commandCode = 0x30079, .continuationEntryAddress = 0x56EF50},
    /* 35 */ {.commandCode = 0x30072, .continuationEntryAddress = 0x56F140},
    /* 36: terminator (key code 0 ends the scan; the other two dwords are 0x90 fill) */
    {.commandCode = 0, .modifierClassFlags = 0x90909090, .continuationEntryAddress = 0x90909090}};
