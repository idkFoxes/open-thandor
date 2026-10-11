/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/improved/arrival.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/pathing/improved/arrival.h>
#include <thandor/thandor.h>
#include <thandor/world/pathing/mode.h>
#include <thandor/world/pathing/scratch_grid.h>

/* open-thandor (docs/plans/pathfinding.md 3 cause 6, 5 package P3): arrival for groups. Without it every unit of
   a point order tries to reach the 0x1000 box of the same point; only a few fit, the others re-plan forever. */

/* Slots of the hex spiral tried per point order: centre + rings 1..6 (127 > 32 entries even with blocked cells). */
static constexpr int FORMATION_SLOT_LIMIT = 127;

/* A unit counts as near its target (arrival rules apply) within the target box plus this many unit spacings. */
static constexpr int ARRIVAL_NEAR_SPACINGS = 4;

/* Same final target (a point order): the resolved targets of two units differ by at most this much on each axis. */
static constexpr int ARRIVAL_SAME_TARGET_Q12 = 0x200;

enum ArrivalNeighbour {
  ARRIVAL_NEIGHBOUR_NONE = 0,
  ARRIVAL_NEIGHBOUR_TOUCHING = 1,   /* touches an arrived friendly */
  ARRIVAL_NEIGHBOUR_AT_TARGET = 2,  /* touches an arrived friendly that stands on or shares its target */
};

static int64_t Arrival_DistanceSquared(int64_t deltaX,int64_t deltaY)
{
  return deltaX * deltaX + deltaY * deltaY;
}

/* The definition of a live, mobile army with a footprint (else NULL). */
static ModelDefinition *Arrival_MobileDefinition(ArmyRuntimeSlot *army)
{
  ModelRuntimeSlot *model;
  ModelDefinition *definition;

  if (army->modelNodeRuntime == nullptr) {
    return nullptr;
  }
  model = army->modelRuntimeOrSavedOffset.modelRuntime;
  if ((model == nullptr) || (model->health == 0)) {
    return nullptr;
  }
  definition = model->definitionOrSavedId.runtimeDefinition;
  if ((definition == nullptr) || (definition->accelerationPerTick == 0) || (definition->footprintRadius == 0)) {
    return nullptr;
  }
  return definition;
}

/* Looks for arrived (inactive) mobile units of the same faction that touch the unit at (x, y). Slot order, so the
   result is the same on every peer. */
static ArrivalNeighbour Arrival_FindArrivedNeighbour(ArmyRuntimeSlot *self,int radius,Q12 x,Q12 y,Q12 targetX,
                                                     Q12 targetY)
{
  ArrivalNeighbour result = ARRIVAL_NEIGHBOUR_NONE;
  int slotIndex;

  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    ArmyRuntimeSlot *other = &g_ArmyRuntimeSlots[slotIndex];
    ArmyMovementRuntime *otherMovement;
    ModelDefinition *otherDefinition;
    int64_t touch;
    Q12 otherX;
    Q12 otherY;

    if ((other == self) || (other->factionIndex != self->factionIndex) ||
        Any(other->movementStateFlags & ARMY_MOVEMENT_ACTIVE)) {
      continue;
    }
    otherDefinition = Arrival_MobileDefinition(other);
    if (otherDefinition == nullptr) {
      continue;
    }
    otherX = other->modelNodeRuntime->worldTransform.translation.x;
    otherY = other->modelNodeRuntime->worldTransform.translation.y;
    touch = (int64_t)radius + (int64_t)otherDefinition->footprintRadius + PATHING_ARRIVAL_MARGIN_Q12;
    if (Arrival_DistanceSquared((int64_t)otherX - x,(int64_t)otherY - y) > touch * touch) {
      continue;
    }
    otherMovement = ModelView_Cast<ArmyMovementRuntime>(other);
    if ((Arrival_DistanceSquared((int64_t)otherX - targetX,(int64_t)otherY - targetY) <= touch * touch) ||
        ((otherMovement->movementTargetWorldXQ12 - targetX <= ARRIVAL_SAME_TARGET_Q12) &&
         (targetX - otherMovement->movementTargetWorldXQ12 <= ARRIVAL_SAME_TARGET_Q12) &&
         (otherMovement->movementTargetWorldYQ12 - targetY <= ARRIVAL_SAME_TARGET_Q12) &&
         (targetY - otherMovement->movementTargetWorldYQ12 <= ARRIVAL_SAME_TARGET_Q12))) {
      return ARRIVAL_NEIGHBOUR_AT_TARGET;
    }
    result = ARRIVAL_NEIGHBOUR_TOUCHING;
  }
  return result;
}

bool PathingArrival_TestArrived(ArmyMovementRuntime *movementRuntime,bool planning)
{
  ArmyRuntimeSlot *self = ModelView_Cast<ArmyRuntimeSlot>(movementRuntime);
  ptrdiff_t slotIndex = self - g_ArmyRuntimeSlots;
  PathingUnitState *state;
  ModelDefinition *definition;
  ArrivalNeighbour neighbour;
  int64_t nearLimit;
  int radius;
  Q12 x;
  Q12 y;

  if ((slotIndex < 0) || (slotIndex >= ARMY_RUNTIME_SLOT_COUNT)) {
    return false;
  }
  state = &g_PathingUnitState[slotIndex];
  if (Any(movementRuntime->movementStateFlags & ARMY_MOVEMENT_ACTIVE)) {
    state->flags = (uint8_t)(state->flags & ~PATHING_UNIT_GROUP_ARRIVED);
  }
  else if ((state->flags & PATHING_UNIT_GROUP_ARRIVED) != 0) {
    return true;
  }
  if (Any(movementRuntime->movementStateFlags &
          (ARMY_MOVEMENT_LOCKED | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_WAYPOINTS_QUEUED))) {
    state->failedPlans = 0;
    return false;
  }
  definition = Arrival_MobileDefinition(self);
  if (definition == nullptr) {
    return false;
  }
  radius = (int)definition->footprintRadius;
  x = self->modelNodeRuntime->worldTransform.translation.x;
  y = self->modelNodeRuntime->worldTransform.translation.y;
  nearLimit = ARMY_MOVEMENT_TARGET_RADIUS_Q12 + (int64_t)ARRIVAL_NEAR_SPACINGS * (2 * radius + PATHING_ARRIVAL_MARGIN_Q12);
  if (Arrival_DistanceSquared((int64_t)x - movementRuntime->movementTargetWorldXQ12,
                              (int64_t)y - movementRuntime->movementTargetWorldYQ12) > nearLimit * nearLimit) {
    state->failedPlans = 0;
    return false;
  }
  neighbour = Arrival_FindArrivedNeighbour(self,radius,x,y,movementRuntime->movementTargetWorldXQ12,
                                           movementRuntime->movementTargetWorldYQ12);
  if (neighbour == ARRIVAL_NEIGHBOUR_AT_TARGET) {
    state->failedPlans = 0;
    return true;
  }
  if (!planning) {
    return false;
  }
  if (neighbour == ARRIVAL_NEIGHBOUR_NONE) {
    state->failedPlans = 0;
    return false;
  }
  state->failedPlans = (uint16_t)(state->failedPlans + 1);
  if (state->failedPlans >= PATHING_ARRIVAL_FAILED_PLAN_LIMIT) {
    state->failedPlans = 0;
    return true;
  }
  return false;
}

bool PathingArrival_Finish(ArmyMovementRuntime *movementRuntime,Q12 *outWorldXQ12,Q12 *outWorldYQ12)
{
  ArmyRuntimeSlot *self = ModelView_Cast<ArmyRuntimeSlot>(movementRuntime);
  ptrdiff_t slotIndex = self - g_ArmyRuntimeSlots;

  /* the same flags as the original arrival in ArmyRuntime_UpdateMovementAndWaypoints */
  movementRuntime->movementStateFlags =
       movementRuntime->movementStateFlags &
       ~(ARMY_MOVEMENT_DIRECT | ARMY_MOVEMENT_ORDERED | ARMY_MOVEMENT_TARGET_FOLLOWING | ARMY_MOVEMENT_ROUTE_POINT_REACHED |
         ARMY_MOVEMENT_WAYPOINTS_QUEUED | ARMY_MOVEMENT_ACTIVE);
  if ((slotIndex >= 0) && (slotIndex < ARMY_RUNTIME_SLOT_COUNT)) {
    g_PathingUnitState[slotIndex].flags = (uint8_t)(g_PathingUnitState[slotIndex].flags | PATHING_UNIT_GROUP_ARRIVED);
    g_PathingUnitState[slotIndex].failedPlans = 0;
  }
  *outWorldXQ12 = movementRuntime->modelNodeRuntime->worldTransform.translation.x;
  *outWorldYQ12 = movementRuntime->modelNodeRuntime->worldTransform.translation.y;
  return true;
}

/* Offset of hex spiral slot slotIndex (0 = centre, ring k holds 6k slots) for the given spacing. */
static void Formation_SlotOffset(int slotIndex,int spacing,int *outDeltaX,int *outDeltaY)
{
  /* unit vectors at 60 degree steps, in 1/1000 */
  static const int directionX[6] = {1000, 500, -500, -1000, -500, 500};
  static const int directionY[6] = {0, 866, 866, 0, -866, -866};
  int ring = 1;
  int firstInRing = 1;
  int position;
  int side;
  int step;

  if (slotIndex == 0) {
    *outDeltaX = 0;
    *outDeltaY = 0;
    return;
  }
  while (slotIndex >= firstInRing + 6 * ring) {
    firstInRing = firstInRing + 6 * ring;
    ring++;
  }
  position = slotIndex - firstInRing;
  side = position / ring;
  step = position % ring;
  /* from corner ring * direction[side] towards corner ring * direction[side + 1] */
  *outDeltaX = (int)(((int64_t)(ring * directionX[side] + step * directionX[(side + 2) % 6]) * spacing) / 1000);
  *outDeltaY = (int)(((int64_t)(ring * directionY[side] + step * directionY[(side + 2) % 6]) * spacing) / 1000);
}

bool PathingFormation_AssignPointSlots(Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection,
                                       Q12 slotWorldX[32],Q12 slotWorldY[32])
{
  int order[32];
  int64_t distance[32];
  int mobileCount = 0;
  int maxRadius = 0;
  int spacing;
  int entryIndex;
  int orderIndex;
  int slotIndex;
  int slotCount;
  int64_t sumX = 0;
  int64_t sumY = 0;
  int64_t approachX;
  int64_t approachY;
  int64_t slotProjection[32];
  int slotDeltaX[32];
  int slotDeltaY[32];
  ModelDefinition *largestDefinition = nullptr;

  if (g_GridScratchPrimary == nullptr) {
    return false;
  }
  for (entryIndex = 0; entryIndex < 32; entryIndex++) {
    ArmyRuntimeSlot *army = ModelView_Cast<ArmyRuntimeSlot>(selection->entries[entryIndex]);
    ModelDefinition *definition;
    int insertAt;

    slotWorldX[entryIndex] = targetWorldX;
    slotWorldY[entryIndex] = targetWorldY;
    if (army == nullptr) {
      continue;
    }
    definition = Arrival_MobileDefinition(army);
    if (definition == nullptr) {
      continue;
    }
    if ((int)definition->footprintRadius > maxRadius) {
      maxRadius = (int)definition->footprintRadius;
      largestDefinition = definition;
    }
    sumX = sumX + army->modelNodeRuntime->worldTransform.translation.x;
    sumY = sumY + army->modelNodeRuntime->worldTransform.translation.y;
    distance[entryIndex] =
         Arrival_DistanceSquared((int64_t)army->modelNodeRuntime->worldTransform.translation.x - targetWorldX,
                                 (int64_t)army->modelNodeRuntime->worldTransform.translation.y - targetWorldY);
    /* insertion by distance, ties keep the entry order */
    insertAt = mobileCount;
    while ((insertAt > 0) && (distance[order[insertAt - 1]] > distance[entryIndex])) {
      order[insertAt] = order[insertAt - 1];
      insertAt--;
    }
    order[insertAt] = entryIndex;
    mobileCount++;
  }
  if (mobileCount < 2) {
    return false;
  }
  spacing = 2 * maxRadius + PATHING_ARRIVAL_MARGIN_Q12;
  /* the first mobileCount passable slots of the spiral (for the largest unit) */
  approachX = (int64_t)targetWorldX - sumX / mobileCount;
  approachY = (int64_t)targetWorldY - sumY / mobileCount;
  slotCount = 0;
  for (slotIndex = 0; (slotIndex < FORMATION_SLOT_LIMIT) && (slotCount < mobileCount); slotIndex++) {
    int deltaX;
    int deltaY;
    int insertAt;
    int64_t projection;

    Formation_SlotOffset(slotIndex,spacing,&deltaX,&deltaY);
    if (GridScratch_TestProjectedCellMaskBands(targetWorldY + deltaY,targetWorldX + deltaX,
                                               (uint8_t)largestDefinition->footprintRadiusClass,
                                               (uint8_t)largestDefinition->terrainTraversalClass)) {
      continue;
    }
    /* far side first: the first units to arrive take the slots beyond the target, so the later ones need not pass
       them (ties keep the spiral order) */
    projection = (int64_t)deltaX * approachX + (int64_t)deltaY * approachY;
    insertAt = slotCount;
    while ((insertAt > 0) && (slotProjection[insertAt - 1] < projection)) {
      slotProjection[insertAt] = slotProjection[insertAt - 1];
      slotDeltaX[insertAt] = slotDeltaX[insertAt - 1];
      slotDeltaY[insertAt] = slotDeltaY[insertAt - 1];
      insertAt--;
    }
    slotProjection[insertAt] = projection;
    slotDeltaX[insertAt] = deltaX;
    slotDeltaY[insertAt] = deltaY;
    slotCount++;
  }
  /* nearest unit first */
  for (orderIndex = 0; orderIndex < slotCount; orderIndex++) {
    entryIndex = order[orderIndex];
    slotWorldX[entryIndex] = targetWorldX + slotDeltaX[orderIndex];
    slotWorldY[entryIndex] = targetWorldY + slotDeltaY[orderIndex];
  }
  return true;
}
