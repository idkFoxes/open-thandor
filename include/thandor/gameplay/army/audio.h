#ifndef THANDOR_GAMEPLAY_ARMY_AUDIO_H
#define THANDOR_GAMEPLAY_ARMY_AUDIO_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/audio. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0051D5F0 */
undefined8 ArmyGraphics_CopyFrontendPlayerPaletteAndTexture (FrontendPlayerRuntimeId frontendPlayerRuntimeId, ArmyGraphicsAssetAddress32 armyGraphicsAsset);

/* 0x00520740 */
void ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantA (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00520E60 */
void ArmyRuntimeAudio_DispatchPositionedSoundVariant (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00523240 */
void ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantB (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00523DD0 */
void ArmyRuntimeAudio_UpdateMovementProjectedLoopingSound (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00524410 */
void ArmyRuntimeAudio_UpdateConditionalProjectedSound (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00524DA0 */
void ArmyRuntimeAudio_UpdatePrimaryAndSecondaryProjectedSounds (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00526490 */
void ArmyRuntimeAudio_UpdateAssetProjectedSound (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00526EB0 */
void ArmyRuntimeAudio_UpdateTerrainContactAndArticulatedProjectedSounds (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00527B20 */
void ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_AUDIO_H */
