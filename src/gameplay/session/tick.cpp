/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/tick.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/tick.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Periodic timer callback of the in-game session: counts the network tick countdown down to zero and advances the
   periodic clock while no resource registration is in progress.
   It runs on the timer thread, the counters are read and written by the main loop as well. The original accesses
   them plainly; here they are accessed through std::atomic_ref (layout unchanged) because the plain
   read-modify-write raced with the main loop's reload of the countdown (a reload between the read and the write
   was overwritten by the decremented old value). The countdown is decremented only while it is non-zero, by a
   compare-exchange that retries with the reloaded value; the arithmetic is the original's.
*/
void InGameRuntime_PeriodicCountdownAndClockTick()

{
  std::atomic_ref<uint32_t> countdown = InGameTick_NetworkTickCountdown();
  uint32_t remaining = countdown.load(std::memory_order_relaxed);

  while ((remaining != 0) && !countdown.compare_exchange_weak(remaining,remaining - 1,std::memory_order_relaxed)) {
  }
  if (InGameTick_ResourceRegistrationBusyCount().load(std::memory_order_relaxed) == 0) {
    InGameTick_PeriodicClockTick().fetch_add(1,std::memory_order_relaxed);
  }
}

/* Whether the current simulation step is a network interval boundary. The original divides by
   g_SessionNetworkTickInterval directly; an interval of 0 is treated as 1 here (every step a boundary) so a
   bad value cannot divide by zero. The join ack only accepts the original's even 2..14, so valid sessions
   compute exactly the original remainder. */
static bool InGameTick_IsNetworkIntervalBoundary()
{
  uint32_t tickInterval;

  tickInterval = g_SessionNetworkTickInterval;
  if (tickInterval == 0) {
    tickInterval = 1;
  }
  return g_SessionNetworkTickCounter % tickInterval == 0;
}

/* Network lockstep of a simulation step on the host or in single player. Returns false when the step has to wait:
   the periodic timer has not counted down yet, or (host, interval boundary) the collected command batch could not
   be broadcast because a peer has not submitted yet. */
static bool InGameTick_RunHostOrLocalLockstep()

{
  void *packet;
  void *packetEndpoint;

  if (InGameTick_NetworkTickCountdown().load(std::memory_order_relaxed) != 0) {
    return false;
  }
  InGameTick_NetworkTickCountdown().store(INGAME_TIMER_TICKS_PER_SIMULATION_STEP,std::memory_order_relaxed);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
    return true;
  }
  if (InGameTick_IsNetworkIntervalBoundary()) {
    /* interval boundary: the batch must be out before it is executed, else wait for the peers */
    while (g_HostCommandBatchSyncSentThisInterval == 0) {
      if (!UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
        if (FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(1)) {
          return false;
        }
        break;
      }
      FrontendTransfer_HostHandleCommandSubmitOrWaitAck
                (static_cast<NetworkSessionContext *>(packetEndpoint),static_cast<FrontendTransferPacketUnion *>(packet));
    }
    FrontendTransfer_DispatchStagedCommandRecords();
    g_HostCommandBatchSyncSentThisInterval = 0;
  }
  else {
    /* within the interval: handle sync requests and broadcast the batch as soon as every peer is ready */
    while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
      FrontendTransfer_HostHandleCommandSubmitOrWaitAck
                (static_cast<NetworkSessionContext *>(packetEndpoint),static_cast<FrontendTransferPacketUnion *>(packet));
    }
    if ((g_HostCommandBatchSyncSentThisInterval == 0) &&
        !FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(0)) {
      g_HostCommandBatchSyncSentThisInterval = 1;
    }
  }
  return true;
}

/* Network lockstep of a simulation step on a client. Returns false when the step has to wait: at an interval
   boundary until the host's command batch has arrived and was executed, otherwise until the periodic timer has
   counted down. */
static bool InGameTick_RunClientLockstep()

{
  void *packet;
  void *packetEndpoint;

  if (InGameTick_IsNetworkIntervalBoundary()) {
    /* interval boundary: wait for the host's command batch, then execute it */
    if (!UiRuntimeRecordRing_ContainsId(g_FrontendSessionToken)) {
      return false;
    }
    while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
      if (FrontendNetwork_HandleCommandBatchAndPlayerTimeout
                    (static_cast<NetworkSessionContext *>(packetEndpoint),static_cast<FrontendTransferPacketUnion *>(packet))) {
        break;
      }
    }
    if (FrontendTransfer_ConsumeProcessedFlag()) {
      return false;
    }
  }
  else if (InGameTick_NetworkTickCountdown().load(std::memory_order_relaxed) != 0) {
    return false;
  }
  InGameTick_NetworkTickCountdown().store(INGAME_TIMER_TICKS_PER_SIMULATION_STEP,std::memory_order_relaxed);
  return true;
}

/* Runs the terrainStateRefresh callback of every node of the world owner list, starting at firstNode. */
static void InGameTick_RefreshEntityTerrainStates(WorldRuntimeContext *worldRuntime,ModelRuntimeNode *firstNode)

{
  ModelRuntimeNode *modelNode;

  for (modelNode = firstNode; modelNode != nullptr;
      modelNode = static_cast<ModelRuntimeNode *>((modelNode->common).nextNode)) {
    (*(&g_RuntimeMaintenanceCallbackPhases.terrainStateRefresh.army)
      [modelNode->ownerClassId])(worldRuntime,modelNode);
  }
}

/* The world job of this step, chosen by tickPhase (simulationTick & 7), so each job runs every 8th step. */
static void InGameTick_RunWorldJob(InGameRuntimeRoot *inGameRoot,uint32_t tickPhase)

{
  WorldRuntimeContext *worldRuntime;
  WorldOwnerListNode *worldNode;
  ModelRuntimeNode *firstModelNode;

  worldRuntime = &inGameRoot->worldRuntime;
  switch(tickPhase) {
  case 0:
    InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState();
    InGameHud_UpdateCurrentFactionMetricCache();
    GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10();
    WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
    break;
  case 1:
    TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(worldRuntime->fieldGrid);
    break;
  case 2:
    AiFactionRuntime_RebuildPlanningCapacityState();
    break;
  case 3:
    /* field-grid clamp; with entities present: their terrain state, then the clamp once more */
    firstModelNode = static_cast<ModelRuntimeNode *>(worldRuntime->ownerListHead);
    FieldGrid_ApplyByteClampLookupToCells(worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid);
    if (firstModelNode != nullptr) {
      InGameTick_RefreshEntityTerrainStates(worldRuntime,firstModelNode);
      FieldGrid_ApplyByteClampLookupToCells(worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid);
    }
    break;
  case 4:
    GridInfluence_ClearDistanceBandsAndRefreshEntities(worldRuntime->ownerListHead);
    break;
  case 5:
    TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(worldRuntime->fieldGrid);
    break;
  case 6:
    AiFactionRuntime_RebuildPlanningCapacityState();
    break;
  case 7:
    /* occupancy rebuild: clear the mask bits, let every entity mark its cells, then refresh their terrain state */
    worldNode = worldRuntime->ownerListHead;
    FieldGrid_ClearOccupancyMaskBits0To6AllCells(worldRuntime->fieldGrid);
    if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED)) {
      FieldGrid_SetOccupancyMaskByteBit0AllCells(worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid);
    }
    if (worldNode != nullptr) {
      for (; worldNode != nullptr; worldNode = worldNode->nextNode) {
        (*(&g_RuntimeMaintenanceCallbackPhases.occupancyRebuild.army)[worldNode->ownerClassId])
                  (worldRuntime,worldNode);
      }
      InGameTick_RefreshEntityTerrainStates(worldRuntime,static_cast<ModelRuntimeNode *>(worldRuntime->ownerListHead));
      FieldGrid_ApplyByteClampLookupToCells(worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid);
      GridScratch_PropagateFieldOccupancyMaskNeighborhood(worldRuntime->fieldGrid);
    }
    break;
  }
}

/* Full simulation step (steps 3-5 of InGameRuntime_UpdateSimulationAndNetworkTick): advances simulationTick, updates
   every entity, every 20 steps the scripted conditions, then the world job of this step. */
static void InGameTick_RunSimulationStep(InGameRuntimeRoot *inGameRoot)

{
  uint32_t tickPhase;
  WorldRuntimeContext *worldRuntime;
  WorldOwnerListNode *worldNode;

  DebugHook_SimulationStepBegin();
  g_GameFactionRuntimeImage.tail.simulationTick++;
  worldRuntime = &inGameRoot->worldRuntime;
  tickPhase = g_GameFactionRuntimeImage.tail.simulationTick & 7;
  /* per-entity update of every model, shot and effect, dispatched by owner class */
  for (worldNode = worldRuntime->ownerListHead; worldNode != nullptr; worldNode = worldNode->nextNode) {
    (*(&g_RuntimeMaintenanceCallbackPhases.primaryUpdate.army)[worldNode->ownerClassId])(worldRuntime,worldNode);
  }
  if (g_GameFactionRuntimeImage.tail.simulationTick % 20 == 0) {
    InGameConditionRuntime_UpdateScheduledRecords();
  }
  InGameTick_RunWorldJob(inGameRoot,tickPhase);
  DebugHook_SimulationStepEnd();
}

/* Reduced update while UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE (0x04) is set; it also runs while
   paused. Every second step it re-seats every model on the terrain, and every 16th step it refreshes either the
   influence distance bands or the terrain/runtime classification masks. */
static void InGameTick_RunReducedUpdate(InGameRuntimeRoot *inGameRoot)

{
  ModelRuntimeNode *modelNode;
  ModelDefinition *modelDefinition;

  modelNode = static_cast<ModelRuntimeNode *>((inGameRoot->worldRuntime).ownerListHead);
  g_GameFactionRuntimeImage.tail.simulationTick++;
  if ((g_GameFactionRuntimeImage.tail.simulationTick & 1) != 0) {
    return;
  }
  for (; modelNode != nullptr; modelNode = static_cast<ModelRuntimeNode *>((modelNode->common).nextNode)) {
    if (modelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      modelDefinition = (((modelNode->runtimePayload).modelRuntime)->definitionOrSavedId).runtimeDefinition;
      g_ArmyPlacementContactKindDispatchTable.callbacks[modelDefinition->placementContactKindIndex]
                (modelDefinition->placementHeightOffsetQ12,
                 (modelNode->worldTransform).translation.y,
                 (modelNode->worldTransform).translation.x,modelNode,
                 &inGameRoot->worldRuntime);
      ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
    }
  }
  if ((g_GameFactionRuntimeImage.tail.simulationTick & INGAME_REDUCED_GRID_REFRESH_TICK_MASK) == 0) {
    if ((g_GameFactionRuntimeImage.tail.simulationTick & 2) == 0) {
      GridInfluence_ClearDistanceBandsAndRefreshEntities((inGameRoot->worldRuntime).ownerListHead);
    }
    else {
      GridScratch_RebuildTerrainAndRuntimeClassificationMasks(&inGameRoot->worldRuntime);
    }
  }
}

/* One simulation step of the running game: the heart of the game loop. It is not called from a fixed place in
   the frame; the UI runtime calls it as its synchronization hook (UiRuntime_SetSynchronizationHooks) every time
   it releases the in-game tick lock, and the loading loops and movie playback call it directly. Pacing therefore
   happens here: the call is ignored while the lock is held, while the simulation is more than two steps ahead of
   the renderer (g_InGamePendingSimulationTicks, two are consumed per drawn frame) and until the 80 Hz periodic
   timer has counted g_InGameNetworkTickCountdown down to zero (at most 20 steps per second).

   Order of one step:
   1. Network lockstep (every g_SessionNetworkTickInterval steps is an interval boundary):
      - host: takes in the peers' command submissions (FrontendTransfer_HostHandleCommandSubmitOrWaitAck),
        broadcasts the collected command batch once every peer has submitted (at the boundary it waits for
        the peers until then) and at the boundary executes the staged batch
        (FrontendTransfer_DispatchStagedCommandRecords: player commands take effect here);
      - client: at the boundary waits for the host's batch and executes it
        (FrontendNetwork_HandleCommandBatchAndPlayerTimeout);
      - single player: only the timer pacing. A step that has to wait returns before anything below.
   2. Nothing more while the session waits for players or is paused (UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS,
      UI_COMMAND_RUNTIME_FLAG_PAUSED); otherwise simulationTick is advanced.
   3. Per-entity update: every node of the world owner list (models = units and buildings, shots, effects)
      is dispatched through g_RuntimeMaintenanceCallbackPhases.primaryUpdate[ownerClassId]
      (ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers, ShotModelRuntimeMaintenance_UpdateProjectile...,
      EffectModelRuntimeMaintenance_UpdateLifecycle...): unit hierarchy, AI and timers, projectile motion and
      hits, effect lifetimes.
   4. Every 20 steps: the level's scripted conditions and end triggers
      (InGameConditionRuntime_UpdateScheduledRecords).
   5. One of eight world jobs by simulationTick & 7, so each runs every 8th step:
      0 faction economy (resources, energy), faction metric cache, technology sync, terrain lighting;
      1 and 5 terrain height relaxation (forward / reverse); 2 and 6 faction AI planning;
      3 field-grid clamp and per-entity terrain state (terrainStateRefresh callbacks);
      4 influence distance bands; 7 occupancy rebuild (occupancyRebuild and terrainStateRefresh callbacks).
   While g_UiCommandRuntimeFlags bit 2 (0x04, origin not identified) is set, steps 2-5 are replaced by a
   reduced update that ignores pause and only
   re-seats every model on the terrain every second step (g_ArmyPlacementContactKindDispatchTable) and refreshes
   the influence / classification grids every 16th step.
*/
void InGameRuntime_UpdateSimulationAndNetworkTick()

{
  bool lockAlreadyHeld;
  bool stepDue;
  InGameRuntimeRoot *inGameRoot;

  lockAlreadyHeld = g_SpinLockTryAcquire(&g_InGameStateTickSpinLock);
  inGameRoot = g_InGameRuntimeRoot;
  if (lockAlreadyHeld) {
    return;
  }
  if (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS)) {
    /* do not run ahead of the renderer by more than a few steps */
    if (2 < (int)g_InGamePendingSimulationTicks) {
      g_SpinLockRelease(&g_InGameStateTickSpinLock);
      return;
    }
    g_InGamePendingSimulationTicks++;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    stepDue = InGameTick_RunHostOrLocalLockstep();
  }
  else {
    stepDue = InGameTick_RunClientLockstep();
  }
  if (!stepDue) {
    g_SpinLockRelease(&g_InGameStateTickSpinLock);
    return;
  }
  g_SessionNetworkTickCounter++;
  if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE)) {
    InGameTick_RunReducedUpdate(inGameRoot);
  }
  else if (!Any(g_UiCommandRuntimeFlags &
           (UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS | UI_COMMAND_RUNTIME_FLAG_PAUSED))) {
    InGameTick_RunSimulationStep(inGameRoot);
  }
  g_SpinLockRelease(&g_InGameStateTickSpinLock);
}
