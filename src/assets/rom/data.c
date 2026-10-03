/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/rom/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/assets/rom/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 005456F8 g_FrontendRomTransitionPageAction */
__declspec(align(8)) uint32_t g_FrontendRomTransitionPageAction = 0;

/* 005456FC g_FrontendActiveRomRecord */
__declspec(align(4)) uint32_t g_FrontendActiveRomRecord = 0;

/* 0054571C g_FrontendRomTransitionElapsedTicks */
__declspec(align(4)) uint32_t g_FrontendRomTransitionElapsedTicks = 0;

/* 00545720 g_FrontendRomTransitionSplineKeyframes */
__declspec(align(16)) uint32_t g_FrontendRomTransitionSplineKeyframes = 0;

/* 00545724 g_FrontendRomTransitionSplineKeyframeCount */
__declspec(align(4)) uint32_t g_FrontendRomTransitionSplineKeyframeCount = 0;

/* 00545728 g_FrontendRomTransitionTargetRecordId */
__declspec(align(8)) uint32_t g_FrontendRomTransitionTargetRecordId = 0;

/* 00545778 g_RomRegistrySlots */
__declspec(align(8)) RomRegistrySlot *g_RomRegistrySlots = 0;

/* 00545784 g_FrontendMenuSoundVoiceSets: the 100 frontend menu sound slots (slot 0 unused, Frontend_Init
   loads sound\menueNN.sam into slots 1..99; ROM action records select one by activationSoundIndex) */
__declspec(align(4)) DirectSoundVoiceSet *g_FrontendMenuSoundVoiceSets[100] = {0};

/* 00545AA4 u_engine_zentrale_rom_00545aa4 */
__declspec(align(4)) uint16_t u_engine_zentrale_rom_00545aa4[20] = L"engine\\zentrale.rom";
