/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/rom/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_ROM_RUNTIME_H
#define THANDOR_ASSETS_ROM_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/rom/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* Frontend ROM action table (a RomRecord, g_FrontendActiveRomRecord): the 0x200-byte RomRecord header with the
   entry count at +0x3C, followed by 0x200-byte FrontendRomActionEntry entries. */
#define FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE 0x200
#define FRONTEND_ROM_ACTION_ENTRY_SIZE 0x200
/* One 0x200-byte entry of the frontend ROM action table (FrontendRomActionTable_ExecuteRecord; Ghidra also
   views it as RomAssetRecordPrefix[]: record[2].recordId = +0x20, record[3] = +0x24..+0x2F). */
typedef struct FrontendRomActionEntry {
    uint8_t unknown00_1B[0x1c];
    RomRecordId linkedRecordId;          /* +0x1C searched by RomRecordTable_FindRecordById/FindIndexById; its
                                            runtime node gets ROM_NODE_FLAG_ACTION_TARGET */
    int32_t pageAction;                  /* +0x20 FRONTEND_PAGE_ACTION_*, negative closes the menu-room view */
    uint32_t keyframeCount;              /* +0x24 */
    RomRecordId targetRecordId;          /* +0x28 */
    uint32_t activationSoundIndex;       /* +0x2C into g_FrontendMenuSoundVoiceSetTable100, 0 = none */
    uint8_t unknown30_3F[0x10];
    WorldMotionSplineKeyframe keyframes[14]; /* +0x40 camera flight; keyframe 0 is the current camera */
} FrontendRomActionEntry;
/* Light of a ROM record (0x10 bytes from +0x50), placed at the matching ROM_NODE_DESCRIPTOR_KIND_LIGHT
   descriptor of the record's root sprite (RomRuntime_ApplyIndexedDescriptor). */
typedef struct RomRecordLight {
    PackedRgb24 packedColorRgb;          /* +0x00 */
    GraphicsRadiusQ12 radiusQ12;         /* +0x04 */
    uint8_t unknown08_0F[8];
} RomRecordLight;
/* A registered ROM record (a menu-room location; g_RomRegistrySlots[].record, g_FrontendActiveRomRecord):
   this 0x200-byte header, then entryCount FrontendRomActionEntry entries. Ghidra views the header as
   RomAssetRecordPrefix[] (12-byte elements), e.g. record[3].byteSize = +0x24. */
typedef struct RomRecord {
    RomRecordByteSize byteSize;          /* +0x00 */
    uint32_t rootNodeOffsetOrPointer;    /* +0x04 serialized sprite-node tree (RomSerializedNodeHeader) */
    RomRecordId recordId;                /* +0x08 */
    uint32_t unknown0C;
    uint32_t visibleRecordMask[4];       /* +0x10 one bit per record id: records shown together with this one */
    Q12 cameraXQ12;                      /* +0x20 camera pose while the record is active (keyframe channels 0..5) */
    Q12 cameraYQ12;                      /* +0x24 */
    Q12 cameraZQ12;                      /* +0x28 */
    UQ12 cameraMagnitudeQ12;             /* +0x2C */
    AngleTurn32 cameraHeadingAngle;      /* +0x30 */
    AngleTurn32 cameraPitchAngle;        /* +0x34 */
    uint32_t lightCount;                 /* +0x38 */
    RomRecordTableCount entryCount;      /* +0x3C */
    PackedArgb32 nodeTintArgb;           /* +0x40 tint of the record's runtime nodes */
    uint8_t unknown44_4F[12];
    RomRecordLight lights[27];           /* +0x50 */
} RomRecord;
/* Elapsed-tick value beyond every flight's last keyframe time: ends the flight on the next frame. */
#define FRONTEND_ROM_TRANSITION_SKIP_TICKS 0x10000000
/* g_RomRegistrySlots: fixed array of 256 {record, runtime root node} slots (RomAssetRecord_RegisterAndRelocate). */
#define ROM_REGISTRY_SLOT_COUNT 256
/* Model-node descriptor list scanned by RomRuntime_ApplyIndexedDescriptor: 0x10-byte entries whose first dword
   holds the kind in its low 4 bits and the entry index above them; kind 4 is a point light. */
#define ROM_NODE_DESCRIPTOR_KIND_MASK 0xf
#define ROM_NODE_DESCRIPTOR_KIND_LIGHT 4
/* runtimeFlags bits of a ROM record's runtime root node (FrontendRomTransition_ActivateRecordById,
   RomRuntime_UpdateRecordVisibilityAndDescriptors; read by the frontend menu-room hit test) */
#define ROM_NODE_FLAG_ACTION_TARGET 0x20 /* linked from an entry of the active record */
#define ROM_NODE_FLAG_HIDDEN 0x40 /* neither the active record nor in its visibleRecordMask */

/* 0x005452A0 */
void FrontendRomActionTable_ExecuteRecord
          (uint32_t reservedZero0,uint32_t reservedZero1,FrontendBooleanState32 suppressActivationSound,
          RomRecordTableIndex recordIndex);

/* 0x00546450 */
StatusResult RomAsset_PrepareRecords(RomAssetHeader *asset);

/* 0x005466A0 */
bool RomRuntime_BuildAllRegistryNodeTrees(WorldRuntimeContext *worldRuntime);

/* 0x00547FC0 */
void FrontendRomTransition_ProcessPendingRecord(void);

/* 0x00547400 */
void FrontendRomRegistry_ClearAndReleaseNestedResources(void);

/* 0x00548720 */
void FrontendRomTransition_RequestStop(void);

/* 0x005487F0 */
RomAssetRecordPrefix * RomRegistry_FindRecordBySlotValue(RomRegistrySlotValue slotValue);

/* 0x00548840 */
uint32_t RomRegistry_FindSlotValueByRecord(RomAssetRecordPrefix *record);

/* 0x00548890 */
void * RomRecordTable_FindRecordById(RomRecordId recordId,void *recordTable);

/* 0x005488D0 */
RomRecordTableIndex RomRecordTable_FindIndexById(RomRecordId recordId,void *table);

/* 0x005484D0 */
StatusResult FrontendRomTransition_ActivateRecordById(RomRecordId recordId,WorldRuntimeContext *worldRuntime);

/* 0x00548600 */
bool RomRuntime_UpdateRecordVisibilityAndDescriptors(RomVisibilityFrontendValue frontendValue,RomRecordId recordId);

/* 0x00546330 */
StatusResult RomAssetRecord_RegisterAndRelocate(RomAssetRecordPrefix *record,RomAssetHeader *assetBase);

/* 0x005464C0 */
ModelNodeCreateResult RomRuntime_BuildNodeTreeRecursive
          (PackedArgb32 stateTintArgb,RomSerializedNodeHeader *romNodeRecord,
          WorldRuntimeContext *worldObjectArray);

/* 0x005483C0 */
void FrontendRomTransition_InitializeFromRecord(FrontendBooleanState32 transitionEnabled,FrontendRomActionEntry *entry);

/* 0x005487A0 */
StatusResult RomRegistry_FindSlotValueByRecordId(RomRecordId recordId);

/* 0x00548410 */
void RomRuntime_ApplyIndexedDescriptor(RomRecordTableIndex entryIndex,RomAssetRecordPrefix *record);

/* 0x00548740 */
RomRecordResult RomRegistry_FindRecordById(RomRecordId recordId);

#endif /* THANDOR_ASSETS_ROM_RUNTIME_H */
