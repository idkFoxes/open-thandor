/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/music_choice.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/music_choice.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Scores how well one of the level's music tracks (by sample number) fits the situation of the active faction:
   sums three army definition values over the faction's armies (definitionClassValue78 weighted 3 for armies
   with bit 0 of commandModeFlags) plus 50 per army with definition flag 0x10, and weights them by the track's number band (below 20, 50,
   70, or above). Track number 0 scores 0. The in-game music picks the best of the level's four tracks.
*/
uint32_t InGameMusic_ComputeTrackSuitabilityScore(MusicTrackClassId trackClassId,WorldRuntimeContext *worldRuntime)

{
  int activeFactionIndex;
  uint32_t suitabilityScore;
  ArmyAssetRecord *armyDefinition;
  ArmyRuntimeSlot *armyRuntime;
  int flag10Bonus;
  int registryWeight;
  ArmyAssetRecordPrefix *foundArmyAsset;
  int flag10BonusSum;
  int class74Sum;
  int weightedClass78Sum;
  int class70Sum;
  WorldOwnerListNode *ownerListNode;
  
  suitabilityScore = 0;
  class70Sum = 0;
  weightedClass78Sum = 0;
  class74Sum = 0;
  flag10BonusSum = 0;
  if (trackClassId != 0) {
    activeFactionIndex = worldRuntime->activeFactionRuntimeIndex;
    for (ownerListNode = worldRuntime->ownerListHead; ownerListNode != nullptr;
        ownerListNode = ownerListNode->nextNode) {
      if (ownerListNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
        continue;
      }
      armyRuntime = WorldOwnerNode_ModelRuntime(ownerListNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
      if (activeFactionIndex != armyRuntime->factionIndex) {
        continue;
      }
      if (ArmyAssetRegistry_FindById(armyRuntime->armyAssetId,&foundArmyAsset) == 0) {
        armyDefinition = ModelView_Cast<ArmyAssetRecord>(foundArmyAsset);
        registryWeight = 1;
        if ((armyRuntime->commandModeFlags & 1) != 0) {
          registryWeight = 3;
        }
        class70Sum = class70Sum + armyDefinition->definitionClassValue70;
        weightedClass78Sum = weightedClass78Sum + registryWeight * armyDefinition->definitionClassValue78;
        flag10Bonus = 50;
        if ((armyDefinition->flags & ARMY_ASSET_FLAG_BUILT_BY_CLASS11) == 0) {
          flag10Bonus = 0;
        }
        class74Sum = class74Sum + armyDefinition->definitionClassValue74;
        flag10BonusSum = flag10BonusSum + flag10Bonus;
      }
    }
    /* weights are Q8 (256 = 1) */
    if (trackClassId < 20) {
      suitabilityScore = flag10BonusSum * 128 + class74Sum * 256 + weightedClass78Sum * 640 + class70Sum * 256;
    }
    else if (trackClassId < 50) {
      suitabilityScore = flag10BonusSum * -256 + class74Sum * 64 + 204800 + weightedClass78Sum * 16 + class70Sum * 128;
    }
    else if (trackClassId < 70) {
      suitabilityScore = flag10BonusSum * 128 + class74Sum * 256 + weightedClass78Sum * 32 + class70Sum * 768;
    }
    else {
      suitabilityScore = flag10BonusSum * 256 + class74Sum * 512 + weightedClass78Sum * 384 + class70Sum * 16;
    }
  }
  return suitabilityScore;
}
