/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/savegame.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_SAVEGAME_H
#define THANDOR_GAMEPLAY_SESSION_SAVEGAME_H

#include <thandor/assets/package/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/session/types.h>
#include <thandor/core/contracts.h>

/* InGameSavePackageHeader.campaignIndex without a campaign */
inline constexpr uint32_t INGAME_SAVE_NO_CAMPAIGN = 0xffffffffu;

/* The 0x200-byte header at the start of a save-game package, patched by InGameSaveGame_WritePackage
   after the entries are written (the save path's directory is split off behind the header, at +0x200). */
struct InGameSavePackageHeader {
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
};

uint32_t InGameSaveGame_WritePackage(void *worldView,void *savePath); /* 0, or the error code of a failed save */

uint32_t InGameSaveGame_CreatePackage(void *packagePath,EngineFileHandle *outHandle); /* 0 or an error code */

ResourceRegistrationImagePair InGameSaveGame_PrepareRegistrationRecords (ResourceRegistrationRuntimeImageSavedView *runtimeImage);

ResourceRegistrationImagePair InGameSaveGame_PrepareFactionImage();

ResourceRegistrationImagePair InGameSaveGame_PrepareEffectSlots();

ResourceRegistrationImagePair InGameSaveGame_PrepareShotSlots();

void InGameSaveGame_StoreCameraAsPlayerStart(ResourceRegistrationRuntimeImage *runtimeImage);

void ArmyRuntimePool_ConvertPointersToOffsetsForSave();

RuntimeHexSegmentImage RuntimeHexSegment_GetLightImageAndToggleFlag();

void RuntimeHexSegment_ToggleLightImageFlag();

RuntimeHexSegmentImage RuntimeHexSegment_GetFieldImage(InGameFieldImageSaveContext58 *fieldImageContext);

void RuntimeHexSegment_AfterFieldImageNoOp(InGameFieldImageSaveContext58 *fieldImageContext);

extern uint16_t g_CampagneHexPathUtf16[13];
extern uint16_t g_OldunitHexPathUtf16[12];
extern uint8_t g_InGameResourceRegistrationBusyCount;

extern uint16_t g_EffectHexPathUtf16[11];
extern uint16_t g_ShotHexPathUtf16[9];
extern uint16_t g_ModulHexPathUtf16[10];
extern uint16_t g_LightHexPathUtf16[10];
extern uint16_t g_WidgetHexPathUtf16[11];
extern uint16_t g_ArmyHexPathUtf16[9];

extern uint8_t *g_EffectRuntimeRebaseBaseMinusOne;
extern uint8_t *g_RuntimeObjectRebaseBaseMinusOne;

/* open-thandor (docs/plans/pathfinding.md 4, D3): the Open Thandor extension entry "otpath.hex" of a save
   package. Written only while the session runs the improved pathing mode (a save of the original mode has no
   such entry and stays byte-identical to the original format); read by InGameRuntime_InitializeLoadedSession
   (absent or invalid: original mode). Older Open Thandor builds and the original ignore the unknown entry and
   load the save in the original mode. Layout: this header, then slotCount records of recordBytes bytes
   (g_PathingUnitState, indexed like g_ArmyRuntimeSlots). Little-endian. */
struct PathingSaveEntryHeader {
    uint32_t magic;        /* PATHING_SAVE_MAGIC */
    uint16_t version;      /* PATHING_SAVE_VERSION */
    uint8_t mode;          /* PathingMode (PATHING_MODE_IMPROVED; any other value loads as original) */
    uint8_t reserved07;    /* 0 */
    uint32_t slotCount;    /* ARMY_RUNTIME_SLOT_COUNT */
    uint32_t recordBytes;  /* sizeof(PathingUnitState) */
};
static_assert(sizeof(PathingSaveEntryHeader) == 16);

inline constexpr uint32_t PATHING_SAVE_MAGIC = 0x4854504FU; /* "OPTH" */
inline constexpr uint16_t PATHING_SAVE_VERSION = 1;

extern uint16_t g_OtpathHexPathUtf16[12];

#endif /* THANDOR_GAMEPLAY_SESSION_SAVEGAME_H */
