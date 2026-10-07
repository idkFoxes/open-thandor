/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/menu_room_scene.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_MENU_ROOM_SCENE_H
#define THANDOR_UI_FRONTEND_MENU_ROOM_SCENE_H

#include <thandor/assets/rom/types.h>
#include <thandor/core/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/model/types.h>
#include <thandor/core/contracts.h>
#include <thandor/assets/rom/runtime.h>

#include <atomic>

/* Elapsed-tick value beyond every flight's last keyframe time: ends the flight on the next frame. */
inline constexpr int32_t FRONTEND_ROM_TRANSITION_SKIP_TICKS = 0x10000000;

/* Model-node descriptor list scanned by RomRuntime_ApplyIndexedDescriptor: 0x10-byte entries whose first dword
   holds the kind in its low 4 bits and the entry index above them; kind 4 is a point light. */
inline constexpr int32_t ROM_NODE_DESCRIPTOR_KIND_MASK = 0xf;
inline constexpr int32_t ROM_NODE_DESCRIPTOR_KIND_LIGHT = 4;
/* runtimeFlags bits of a ROM record's runtime root node (FrontendRomTransition_ActivateRecordById,
   RomRuntime_UpdateRecordVisibilityAndDescriptors; read by the frontend menu-room hit test) */
inline constexpr int32_t ROM_NODE_FLAG_ACTION_TARGET = 0x20; /* linked from an entry of the active record */
inline constexpr int32_t ROM_NODE_FLAG_HIDDEN = 0x40; /* neither the active record nor in its visibleRecordMask */

void FrontendRomActionTable_ExecuteRecord
          (uint32_t reservedZero0,uint32_t reservedZero1,FrontendBooleanState32 suppressActivationSound,
          RomRecordTableIndex recordIndex);

Bool8 RomRuntime_BuildAllRegistryNodeTrees(WorldRuntimeContext *worldRuntime);

void FrontendRomTransition_ProcessPendingRecord();

void FrontendRomTransition_RequestStop();

uint32_t FrontendRomTransition_ActivateRecordById(RomRecordId recordId,WorldRuntimeContext *worldRuntime);

Bool8 RomRuntime_UpdateRecordVisibilityAndDescriptors(RomVisibilityFrontendValue frontendValue,RomRecordId recordId);

ModelRuntimeNode * RomRuntime_BuildNodeTreeRecursive
          (PackedArgb32 stateTintArgb,RomSerializedNodeHeader *romNodeRecord,
          WorldRuntimeContext *worldObjectArray);

void FrontendRomTransition_InitializeFromRecord(FrontendBooleanState32 transitionEnabled,FrontendRomActionEntry *entry);

void RomRuntime_ApplyIndexedDescriptor(RomRecordTableIndex entryIndex,RomAssetRecordPrefix *record);

extern FrontendPageAction g_FrontendRomTransitionPageAction;
extern RomRecord *g_FrontendActiveRomRecord; /* the active ROM action table (a menu-room location) */
extern std::atomic<uint32_t> g_FrontendRomTransitionElapsedTicks; /* advanced by the 256 Hz timer thread */
extern WorldMotionSplineKeyframe *g_FrontendRomTransitionSplineKeyframes; /* keyframes of the running camera flight */
extern uint32_t g_FrontendRomTransitionSplineKeyframeCount;
extern std::atomic<uint32_t> g_FrontendRomTransitionTargetRecordId; /* read by the 256 Hz timer thread */

extern SoundVoiceSet *g_FrontendMenuSoundVoiceSets[100]; /* 100 menu sound slots, slot 0 unused, 1..99 = sound\menueNN.sam */

#endif /* THANDOR_UI_FRONTEND_MENU_ROOM_SCENE_H */
