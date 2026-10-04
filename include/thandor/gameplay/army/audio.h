/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/audio.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_AUDIO_H
#define THANDOR_GAMEPLAY_ARMY_AUDIO_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/audio. */

/* Functions are grouped by semantic ownership. */

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
