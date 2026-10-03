/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/rom/data.h
 */

#ifndef THANDOR_ASSETS_ROM_DATA_H
#define THANDOR_ASSETS_ROM_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint32_t g_FrontendRomTransitionPageAction; /* 005456F8 g_FrontendRomTransitionPageAction */

extern uint32_t g_FrontendActiveRomRecord; /* 005456FC g_FrontendActiveRomRecord */

extern uint32_t g_FrontendRomTransitionElapsedTicks; /* 0054571C g_FrontendRomTransitionElapsedTicks */

extern uint32_t g_FrontendRomTransitionSplineKeyframes; /* 00545720 g_FrontendRomTransitionSplineKeyframes */

extern uint32_t g_FrontendRomTransitionSplineKeyframeCount; /* 00545724 g_FrontendRomTransitionSplineKeyframeCount */

extern uint32_t g_FrontendRomTransitionTargetRecordId; /* 00545728 g_FrontendRomTransitionTargetRecordId */

extern RomRegistrySlot *g_RomRegistrySlots; /* 00545778 g_RomRegistrySlots */

extern DirectSoundVoiceSet *g_FrontendMenuSoundVoiceSets[100]; /* 00545784 g_FrontendMenuSoundVoiceSets: 100 menu sound slots, slot 0 unused, 1..99 = sound\menueNN.sam */

extern uint16_t u_engine_zentrale_rom_00545aa4[20]; /* 00545AA4 u_engine_zentrale_rom_00545aa4 */

#endif
