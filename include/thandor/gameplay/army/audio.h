/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/audio.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_AUDIO_H
#define THANDOR_GAMEPLAY_ARMY_AUDIO_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/audio. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0051D5F0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyGraphics_CopyFrontendPlayerPaletteAndTexture
          (FrontendPlayerRuntimeId frontendPlayerRuntimeId,
          ArmyGraphicsAssetAddress32 armyGraphicsAsset);

/* 0x00520740 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantA
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00520E60 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_DispatchPositionedSoundVariant
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00523240 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantB
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00523DD0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateMovementProjectedLoopingSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00524410 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateConditionalProjectedSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00524DA0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdatePrimaryAndSecondaryProjectedSounds
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00526490 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateAssetProjectedSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00526EB0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateTerrainContactAndArticulatedProjectedSounds
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00527B20 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_AUDIO_H */
