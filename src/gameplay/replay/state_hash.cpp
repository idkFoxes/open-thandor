#include <thandor/gameplay/replay/compact_stream.h>
#include <thandor/thandor.h>
#include <unordered_set>
// Selected authoritative state plus every model/node animation pose. This is a
// divergence detector, not a serialization substitute or a proof of equality.
namespace {
struct Hash {
  std::uint64_t h = 14695981039346656037ull;
  template <class T> void add(T v) {
    auto w = static_cast<std::uint32_t>(v);
    for (int i = 0; i < 4; ++i) {
      auto b = static_cast<unsigned char>(w >> (i * 8));
      h = thandor::replay::hashBytes(h, &b, 1);
    }
  }
};
std::uint32_t armyIndex(const ArmyRuntimeSlot *p) {
  if (!p)
    return 0;
  auto a = reinterpret_cast<std::uintptr_t>(p), b = reinterpret_cast<std::uintptr_t>(g_ArmyRuntimeSlots);
  if (a < b || (a - b) % sizeof(ArmyRuntimeSlot) ||
      (a - b) / sizeof(ArmyRuntimeSlot) >= ARMY_RUNTIME_SLOT_COUNT)
    return ~0u;
  return static_cast<std::uint32_t>((a - b) / sizeof(ArmyRuntimeSlot) + 1);
}
void nodeHash(Hash &h, const ModelRuntimeNode *n, std::unordered_set<const ModelRuntimeNode *> &seen) {
  h.add(n != nullptr);
  if (!n)
    return;
  if (!seen.insert(n).second) {
    h.add(0xbad1u);
    return;
  }
  h.add(n->modelPayload.localTranslationXQ12);
  h.add(n->modelPayload.localTranslationYQ12);
  h.add(n->modelPayload.localTranslationZQ12);
  h.add(n->modelPayload.localRotationAngle0);
  h.add(n->modelPayload.localRotationAngle1);
  h.add(n->modelPayload.localRotationAngle2);
  h.add(n->modelPayload.meshGroupMask);
  h.add(n->modelPayload.worldRotationAngle0);
  h.add(n->modelPayload.worldRotationAngle1);
  h.add(n->modelPayload.worldRotationAngle2);
  h.add(n->worldTransform.translation.x);
  h.add(n->worldTransform.translation.y);
  h.add(n->worldTransform.translation.z);
  h.add(n->modelScaleQ12);
  h.add(n->primaryAnimatedSubresourceIndex);
  h.add(n->primaryTextureOffsetU);
  h.add(n->primaryTextureOffsetV);
  h.add(n->secondaryAnimatedSubresourceIndex);
  h.add(n->secondaryTextureOffsetU);
  h.add(n->secondaryTextureOffsetV);
  h.add(n->childCount);
  for (unsigned i = 0; i < 13; ++i)
    nodeHash(h, n->childNodes[i], seen);
}
void modelHash(Hash &h, const ModelRuntimeSlot *m, std::unordered_set<const ModelRuntimeSlot *> &seen) {
  h.add(m != nullptr);
  if (!m)
    return;
  if (!seen.insert(m).second) {
    h.add(0xbad2u);
    return;
  }
  const auto *d = m->definitionOrSavedId.runtimeDefinition.get();
  h.add(d != nullptr);
  if (d) {
    h.add(d->definitionId);
    h.add(d->runtimeClassId);
  }
  h.add(m->health);
  h.add(m->attachmentCount);
  h.add(m->researchTechnologyId);
  h.add(m->researchDurationTicks);
  h.add(m->researchElapsedTicks);
  h.add(m->researchEnergyLoadQ4);
  h.add(m->researchXeniteCostQ4);
  h.add(m->classState.stateFlags);
  h.add(m->classState.behaviorState);
  h.add(m->classState.energyLoadQ4);
  h.add(m->classState.shotEmitterTimerTicks);
  h.add(m->classState.effectEmitterTimerTicks);
  for (auto timer : m->destructionEffectTimers)
    h.add(timer);
  for (unsigned i = 0; i < 6; ++i) {
    const auto &a = m->attachments[i];
    h.add(a.childNodeIndex);
    h.add(a.childLocalRotationAngle0);
    h.add(a.childLocalRotationAngle1);
    h.add(a.childLocalRotationAngle2);
    modelHash(h, a.childModelRuntimeOrSavedOffset, seen);
  }
}
} // namespace
std::uint64_t GameplayReplay_StateHash() {
  Hash h;
  if (!g_InGameRuntimeRoot)
    return h.h;
  h.add(g_GameFactionRuntimeImage.tail.simulationTick);
  h.add(g_FrontendPlayerRuntimeBlockCount);
  for (unsigned i = 0; i < g_FrontendPlayerRuntimeBlockCount && i < 8; ++i) {
    h.add(g_FrontendPlayerRuntimeBlocks[i].playerRuntimeId);
    h.add(g_FrontendPlayerRuntimeBlocks[i].factionAssignment.factionAssignmentIndex);
  }
  h.add(g_SessionNetworkTickCounter);
  h.add(g_InGameSimulationStepTicks);
  h.add(g_UiCommandRuntimeFlags);
  h.add(g_RandomGeneratorState.primarySeed);
  h.add(g_RandomGeneratorState.secondarySeed);
  std::unordered_set<const ModelRuntimeNode *> nodes;
  std::unordered_set<const ModelRuntimeSlot *> models;
  for (auto *n = g_InGameRuntimeRoot->worldRuntime.ownerListHead.get(); n; n = n->nextNode) {
    h.add(n->ownerClassId);
    h.add(n->worldXQ12);
    h.add(n->worldYQ12);
    h.add(n->worldZQ12);
    if (n->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)
      nodeHash(h, ModelView_Cast<ModelRuntimeNode>(n), nodes);
  }
  for (unsigned i = 0; i < ARMY_RUNTIME_SLOT_COUNT; ++i) {
    const auto &a = g_ArmyRuntimeSlots[i];
    if (!a.modelNodeRuntime)
      continue;
    h.add(i);
    h.add(a.armyAssetId);
    h.add(a.factionIndex);
    h.add(a.movementStateFlags);
    h.add(a.commandModeFlags);
    h.add(a.commandGeneration);
    h.add(a.commandCoordinate0Q12);
    h.add(a.commandCoordinate1Q12);
    h.add(a.commandCoordinate2Q12);
    h.add(armyIndex(a.commandTargetArmyRuntime));
    h.add(a.actionVector0Q12);
    h.add(a.actionVector1Q12);
    h.add(a.actionVector2Q12);
    h.add(a.movementPosition0Q12);
    h.add(a.movementPosition1Q12);
    h.add(a.movementTarget0Q12);
    h.add(a.movementTarget1Q12);
    h.add(a.aiUnitState);
    h.add(a.movementRetryCountdown);
    modelHash(h, a.modelRuntimeOrSavedOffset.modelRuntime, models);
  }
  for (const auto &f : g_GameFactionRuntimeImage.records) {
    h.add(f.xeniteCurrentQ4);
    h.add(f.tritiumCurrentQ4);
    h.add(f.suppliedEnergyDemandQ4);
    h.add(f.unpoweredEnergyDemandQ4);
    h.add(f.packedRelationStates);
    h.add(f.runtimeFlags);
    h.add(f.primaryArmyAssetCount);
    h.add(f.secondaryArmyAssetCount);
    for (auto w : f.technologyMasks256Bits)
      h.add(w);
    for (auto w : f.aiPressureValues)
      h.add(w);
  }
  const auto *grid = g_InGameRuntimeRoot->worldRuntime.fieldGrid.get();
  if (grid) {
    h.add(grid->gridWidth);
    h.add(grid->gridHeight);
    auto count = static_cast<std::uint64_t>(grid->gridWidth) * grid->gridHeight;
    if (count <= 16u * 1024u * 1024u)
      for (std::uint64_t i = 0; i < count; ++i) {
        const auto &c = grid->cells[i];
        h.add(c.terrainHeight);
        h.add(c.waterSurfaceDelta);
        h.add(ToBits(c.flagsAndMaterial) & ~(0x10000000u | 0x10000u));
        h.add(c.occupancyMask);
        h.add(c.occupancyMask >> 32);
        h.add(c.resourceExtractionDescriptor);
      }
  }
  return h.h;
}
