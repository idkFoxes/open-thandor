/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/savegame.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_SAVEGAME_H
#define THANDOR_GAMEPLAY_SESSION_SAVEGAME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/savegame. */

/* InGameSavePackageHeader.campaignIndex without a campaign */
#define INGAME_SAVE_NO_CAMPAIGN 0xffffffffu

/* The 0x200-byte header at the start of a save-game package, patched by InGameSaveGame_WritePackage
   after the entries are written (the save path's directory is split off behind the header, at +0x200). */
typedef struct InGameSavePackageHeader {
    uint8_t reserved000_0FF[0x100];
    uint16_t saveNameUtf16[0x38]; /* +0x100 file name of the save path */
    uint32_t levelTitleTextId; /* +0x170 */
    uint8_t reserved174_18F[0x1C];
    uint32_t campaignIndex; /* +0x190 -1 without campaign */
    uint8_t reserved194_1BF[0x2C];
    uint16_t dateTimeTextUtf16[0x18]; /* +0x1C0 "date, time" */
    uint32_t packedDate; /* +0x1F0 */
    uint32_t packedTime; /* +0x1F4 */
    uint8_t reserved1F8_1FF[8];
} InGameSavePackageHeader;

/* Functions are grouped by semantic ownership. */

Bool8 InGameSaveGame_WritePackage(void *worldView,void *savePath); /* returns true on failure */

extern uint16_t g_CampagneHexPathUtf16[13];
extern uint16_t g_OldunitHexPathUtf16[12];
extern uint8_t g_InGameResourceRegistrationBusyCount;

void ResourceRegistrationRuntime_RebaseLoadedRecords(ResourceRegistrationRuntimeImage *runtimeImage);

Bool8 SavedLevel_LoadRuntimePools(WorldRuntimeContext *worldRuntime,uint32_t *outError);

extern uint16_t g_EffectHexPathUtf16[11];
extern uint16_t g_ShotHexPathUtf16[9];
extern uint16_t g_ModulHexPathUtf16[10];
extern uint16_t g_LightHexPathUtf16[10];
extern uint16_t g_WidgetHexPathUtf16[11];
extern uint16_t g_ArmyHexPathUtf16[9];

#endif /* THANDOR_GAMEPLAY_SESSION_SAVEGAME_H */
