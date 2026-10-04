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

/* Faction graphics ('gfx') texture source asset layout used by ArmyGraphics_CopyFrontendPlayerPaletteAndTexture */
#define ARMY_GRAPHICS_PLAYER_IMAGE_SUBRESOURCE 0x71 /* image replaced by the frontend player's picture */
#define ARMY_GRAPHICS_PALETTE_TABLE_OFFSET 0x200    /* first palette, from the asset start */
#define ARMY_GRAPHICS_PALETTE_BYTES 0x800           /* 256 entries of 8 bytes */
#define ARMY_GRAPHICS_PLAYER_IMAGE_DWORDS 0x400     /* 0x1000 bytes of pixel data */
/* Functions are grouped by semantic ownership. */

void ArmyGraphics_CopyFrontendPlayerPaletteAndTexture(FrontendPlayerRuntimeId frontendPlayerRuntimeId,
          ArmyGraphicsAssetAddress32 armyGraphicsAsset);

void ArmyRuntimeAudio_UpdateTrackedTurnAndMoveSounds
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeAudio_DispatchPositionedSoundVariant(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeAudio_UpdateGliderTurnAndMoveSounds
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeAudio_UpdateTurretTurnSound
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeAudio_UpdateStructureFactorySound(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeAudio_UpdateUnitFactorySounds
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeAudio_UpdateAssetProjectedSound(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeAudio_UpdateLinkedChildPadSounds
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeClass_UpdateGroundPositionedSounds(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntimeClass_UpdateWaterPositionedSounds(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntime_UpdateLoopingPositionedSound(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint(FactionRuntimeIndex factionIndex,Q12 worldYQ12,Q12 worldXQ12,
          SoundAssetIndex soundAssetIndex,WorldRuntimeContext *worldContext);

void ModelRuntime_PlayDefinitionSecondaryOneShotSound(ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldRuntime);

void ModelRuntime_PlayDefinitionPrimaryOneShotSound(ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldContext);

#endif /* THANDOR_GAMEPLAY_ARMY_AUDIO_H */
