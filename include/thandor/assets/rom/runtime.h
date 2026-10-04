/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/rom/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_ROM_RUNTIME_H
#define THANDOR_ASSETS_ROM_RUNTIME_H

#include <thandor/assets/rom/types.h>
#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/world/camera/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/rom/runtime. */

/* Frontend ROM action table (a RomRecord, g_FrontendActiveRomRecord): the 0x200-byte RomRecord header with the
   entry count in entryCount, followed by 0x200-byte FrontendRomActionEntry entries. */
#define FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE 0x200
#define FRONTEND_ROM_ACTION_ENTRY_SIZE 0x200
/* One 0x200-byte entry of the frontend ROM action table (FrontendRomActionTable_ExecuteRecord; some code
   also reads it as RomAssetRecordPrefix[]: record[2].recordId = +0x20, record[3] = +0x24..+0x2F). */
typedef struct FrontendRomActionEntry {
    uint8_t unknown00_1B[0x1c];
    RomRecordId linkedRecordId;          /* +0x1C searched by RomRecordTable_FindRecordById/FindIndexById; its
                                            runtime node gets ROM_NODE_FLAG_ACTION_TARGET */
    int32_t pageAction;                  /* +0x20 FRONTEND_PAGE_ACTION_*, negative closes the menu-room view */
    uint32_t keyframeCount;              /* +0x24 */
    RomRecordId targetRecordId;          /* +0x28 */
    uint32_t activationSoundIndex;       /* +0x2C into g_FrontendMenuSoundVoiceSets, 0 = none */
    uint8_t unknown30_3F[0x10];
    WorldMotionSplineKeyframe keyframes[14]; /* +0x40 camera flight; keyframe 0 is the current camera */
} FrontendRomActionEntry;
/* Light of a ROM record (0x10-byte entries of RomRecord.lights), placed at the matching ROM_NODE_DESCRIPTOR_KIND_LIGHT
   descriptor of the record's root sprite (RomRuntime_ApplyIndexedDescriptor). */
typedef struct RomRecordLight {
    PackedRgb24 packedColorRgb;          /* +0x00 */
    GraphicsRadiusQ12 radiusQ12;         /* +0x04 */
    uint8_t unknown08_0F[8];
} RomRecordLight;
/* A registered ROM record (a menu-room location; g_RomRegistrySlots[].record, g_FrontendActiveRomRecord):
   this 0x200-byte header, then entryCount FrontendRomActionEntry entries. Some code reads the header
   as RomAssetRecordPrefix[] (12-byte elements), e.g. record[3].byteSize = +0x24. */
typedef struct RomRecord {
    RomRecordByteSize byteSize;          /* +0x00 */
    uint32_t rootNodeOffsetOrPointer;    /* +0x04 serialized sprite-node tree (RomSerializedNodeHeader) */
    RomRecordId recordId;                /* +0x08 */
    uint32_t unknown0C;
    uint32_t visibleRecordMask[4];       /* +0x10 one bit per record id: records shown together with this one.
                                            Covers ids 0..127 only; its users (FrontendRomTransition_ActivateRecordById,
                                            RomRuntime_UpdateRecordVisibilityAndDescriptors) index it with id >> 5
                                            unchecked, like the original, and nothing
                                            limits the ids at registration: a record id >= 128 would test a bit of the
                                            camera pose from +0x20 on. */
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

/* g_RomRegistrySlots: fixed array of 256 {record, runtime root node} slots (RomAssetRecord_RegisterAndRelocate). */
#define ROM_REGISTRY_SLOT_COUNT 256

/* Functions are grouped by semantic ownership. */

uint32_t RomAsset_PrepareRecords(RomAssetHeader *asset);

void FrontendRomRegistry_ClearAndReleaseNestedResources(void);

RomAssetRecordPrefix * RomRegistry_FindRecordBySlotValue(RomRegistrySlotValue slotValue);

void * RomRecordTable_FindRecordById(RomRecordId recordId,void *recordTable);

RomRecordTableIndex RomRecordTable_FindIndexById(RomRecordId recordId,void *table);

uint32_t RomAssetRecord_RegisterAndRelocate(RomAssetRecordPrefix *record,RomAssetHeader *assetBase);

Bool8 RomRegistry_FindSlotValueByRecordId(RomRecordId recordId,WorldRuntimeNode **outRootNode);

RomAssetRecordPrefix * RomRegistry_FindRecordById(RomRecordId recordId);

extern RomRegistrySlot *g_RomRegistrySlots;

extern uint16_t g_EngineZentraleRomPathUtf16[20];

#endif /* THANDOR_ASSETS_ROM_RUNTIME_H */
