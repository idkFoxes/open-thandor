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

/* The sound slot of a sound index (step 13 X5b): WorldRuntimeContext.dwordArray keeps the SpatialSoundSlot
   addresses as pointer-sized integers. ArmySound_VoiceSetRef is the same address as the voice-set reference the
   play functions take (the slot's first field, voiceSet). The caller checks the index and the array, as before;
   the index keeps its own type (template) so the address arithmetic is unchanged. */
template <class Index> inline SpatialSoundSlot *ArmySound_Slot(const WorldRuntimeContext *worldRuntime,Index index)
{
    return reinterpret_cast<SpatialSoundSlot *>(worldRuntime->dwordArray[index]);
}
template <class Index> inline SoundVoiceSet **ArmySound_VoiceSetRef(const WorldRuntimeContext *worldRuntime,
          Index index)
{
    return reinterpret_cast<SoundVoiceSet **>(worldRuntime->dwordArray[index]);
}

void ArmyRuntimeAudio_UpdateTurnAndMoveSounds
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

void ArmyRuntime_UpdateLoopingPositionedSound(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint(FactionRuntimeIndex factionIndex,Q12 worldYQ12,Q12 worldXQ12,
          SoundAssetIndex soundAssetIndex,WorldRuntimeContext *worldContext);

void ModelRuntime_PlayDefinitionOneShotSound(ModelRuntimeSlot *modelRuntime,uint32_t soundAssetIndex,
          WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_AUDIO_H */
