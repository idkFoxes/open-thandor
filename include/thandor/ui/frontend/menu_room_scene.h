/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/menu_room_scene.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_MENU_ROOM_SCENE_H
#define THANDOR_UI_FRONTEND_MENU_ROOM_SCENE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>
#include <thandor/assets/rom/runtime.h>

/* Submodule: ui/frontend/menu_room_scene. */

/* Elapsed-tick value beyond every flight's last keyframe time: ends the flight on the next frame. */
#define FRONTEND_ROM_TRANSITION_SKIP_TICKS 0x10000000

/* Model-node descriptor list scanned by RomRuntime_ApplyIndexedDescriptor: 0x10-byte entries whose first dword
   holds the kind in its low 4 bits and the entry index above them; kind 4 is a point light. */
#define ROM_NODE_DESCRIPTOR_KIND_MASK 0xf
#define ROM_NODE_DESCRIPTOR_KIND_LIGHT 4
/* runtimeFlags bits of a ROM record's runtime root node (FrontendRomTransition_ActivateRecordById,
   RomRuntime_UpdateRecordVisibilityAndDescriptors; read by the frontend menu-room hit test) */
#define ROM_NODE_FLAG_ACTION_TARGET 0x20 /* linked from an entry of the active record */
#define ROM_NODE_FLAG_HIDDEN 0x40 /* neither the active record nor in its visibleRecordMask */

/* Functions are grouped by semantic ownership. */

void FrontendRomActionTable_ExecuteRecord
          (uint32_t reservedZero0,uint32_t reservedZero1,FrontendBooleanState32 suppressActivationSound,
          RomRecordTableIndex recordIndex);

Bool8 RomRuntime_BuildAllRegistryNodeTrees(WorldRuntimeContext *worldRuntime);

void FrontendRomTransition_ProcessPendingRecord(void);

void FrontendRomTransition_RequestStop(void);

uint32_t FrontendRomTransition_ActivateRecordById(RomRecordId recordId,WorldRuntimeContext *worldRuntime);

Bool8 RomRuntime_UpdateRecordVisibilityAndDescriptors(RomVisibilityFrontendValue frontendValue,RomRecordId recordId);

ModelRuntimeNode * RomRuntime_BuildNodeTreeRecursive
          (PackedArgb32 stateTintArgb,RomSerializedNodeHeader *romNodeRecord,
          WorldRuntimeContext *worldObjectArray);

void FrontendRomTransition_InitializeFromRecord(FrontendBooleanState32 transitionEnabled,FrontendRomActionEntry *entry);

void RomRuntime_ApplyIndexedDescriptor(RomRecordTableIndex entryIndex,RomAssetRecordPrefix *record);

extern uint32_t g_FrontendRomTransitionPageAction;
extern uintptr_t g_FrontendActiveRomRecord;
extern uint32_t g_FrontendRomTransitionElapsedTicks;
extern uintptr_t g_FrontendRomTransitionSplineKeyframes;
extern uint32_t g_FrontendRomTransitionSplineKeyframeCount;
extern uint32_t g_FrontendRomTransitionTargetRecordId;

extern DirectSoundVoiceSet *g_FrontendMenuSoundVoiceSets[100]; /* 100 menu sound slots, slot 0 unused, 1..99 = sound\menueNN.sam */

#endif /* THANDOR_UI_FRONTEND_MENU_ROOM_SCENE_H */
