/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/improved/yield.cpp
 * Project code (not in the original game)
 */

/* Improved pathing package P2: yielding to friendly blockers (see world/pathing/yield.h). Reached only behind
   Pathing_Improved(). Integer arithmetic only; no random numbers; the order of all decisions follows the fixed
   order of the movement updates and of the owner list, so every peer decides the same. */

#include <thandor/world/pathing/yield.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>
#include <thandor/world/pathing/mode.h>

namespace {

/* Gap kept between the two footprints after a side step. */
constexpr int PATHING_YIELD_CLEARANCE_Q12 = 0x200;
/* Shortest side step, and the extra distance of each further try. */
constexpr int PATHING_YIELD_MIN_SIDE_STEP_Q12 = 0x400;
constexpr int PATHING_YIELD_SIDE_STEP_EXTRA_Q12 = 0x400;
/* Side-step distances tried per side (collision: 3 from the needed clearance on; corridor: 1..6 steps). */
constexpr int PATHING_YIELD_COLLISION_TRIES = 3;
constexpr int PATHING_YIELD_CORRIDOR_TRIES = 6;
/* Saved low distance band bytes of the obstacle stamp (rows x columns of the box around the blocker). */
constexpr int PATHING_YIELD_SAVE_CAPACITY = 0x4000;

/* The obstacle stamp of one re-plan (PathingYield_BeginReplan .. EndReplan). */
uint8_t s_SavedLowBands[PATHING_YIELD_SAVE_CAPACITY];
bool s_ObstacleStamped = false;
int s_SavedFirstRow = 0;
int s_SavedRowCount = 0;
int s_SavedFirstColumn = 0;
int s_SavedColumnCount = 0;

/* The entity planned by the running EntityPathing_RebuildOverlappingGroupRoutes and its target. */
Q12 s_PlannerWorldXQ12 = 0;
Q12 s_PlannerWorldYQ12 = 0;
Q12 s_PlannerTargetXQ12 = 0;
Q12 s_PlannerTargetYQ12 = 0;
WorldRuntimeContext *s_PlannerWorldRuntime = nullptr;

}  // namespace

/* The army slot index of an army (any view of the slot), -1 when it is not a slot of the pool. */
static int PathingYield_SlotIndex(const void *army)
{
  uintptr_t offset;

  if ((army == nullptr) || (g_ArmyRuntimeSlots == nullptr)) {
    return -1;
  }
  offset = reinterpret_cast<uintptr_t>(army) - reinterpret_cast<uintptr_t>(g_ArmyRuntimeSlots);
  if ((offset % sizeof(ArmyRuntimeSlot)) != 0) {
    return -1;
  }
  offset = offset / sizeof(ArmyRuntimeSlot);
  if (offset >= (uintptr_t)ARMY_RUNTIME_SLOT_COUNT) {
    return -1;
  }
  return (int)offset;
}

/* Integer square root (floor) of a 64-bit value. */
static uint32_t PathingYield_SquareRoot(uint64_t value)
{
  uint64_t root = 0;
  uint64_t bit = (uint64_t)1 << 62;

  while (bit > value) {
    bit >>= 2;
  }
  while (bit != 0) {
    if (value >= root + bit) {
      value -= root + bit;
      root = (root >> 1) + bit;
    }
    else {
      root >>= 1;
    }
    bit >>= 2;
  }
  return (uint32_t)root;
}

static int64_t PathingYield_DistanceSquared(int ax,int ay,int bx,int by)
{
  int64_t dx = (int64_t)ax - (int64_t)bx;
  int64_t dy = (int64_t)ay - (int64_t)by;

  return dx * dx + dy * dy;
}

/* The scratch cell under a world point (the formula of the planning code), false off the inner grid. */
static bool PathingYield_CellOfWorldPoint(Q12 worldXQ12,Q12 worldYQ12,int *outRow,int *outColumn)
{
  uint32_t rowTerm;
  int column;
  int row;

  rowTerm = (int)((uint64_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  column = (int)((((int)((uint64_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                 (uint32_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - rowTerm) +
                 GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  row = (int)(rowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if ((column < 1) || (row < 1) || (column >= (int)g_GridScratchWidth - 1) ||
      (row >= (int)g_GridScratchHeight - 1)) {
    return false;
  }
  *outRow = row;
  *outColumn = column;
  return true;
}

/* True when the army is an own-faction (same factionIndex as mover) unit of a moving class that can yield. */
static bool PathingYield_IsFriendlyMobileUnit(const ArmyRuntimeSlot *mover,const ModelRuntimeSlot *model,
                                             ArmyRuntimeSlot **outArmy)
{
  ArmyRuntimeSlot *army;
  ModelDefinition *definition;

  army = model->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  if ((army == nullptr) || (army == mover) || (army->factionIndex != mover->factionIndex) ||
      (army->modelNodeRuntime == nullptr) || (PathingYield_SlotIndex(army) < 0)) {
    return false;
  }
  definition = model->definitionOrSavedId.runtimeDefinition;
  if (definition->accelerationPerTick == 0) {
    return false;
  }
  switch (definition->runtimeClassId) {
  case MODEL_RUNTIME_CLASS_01_GROUND:
  case MODEL_RUNTIME_CLASS_02_TRACKED:
  case MODEL_RUNTIME_CLASS_03_ARTICULATED_WALKER:
  case MODEL_RUNTIME_CLASS_17_DEPLOYING_GLIDER:
  case MODEL_RUNTIME_CLASS_18:
  case MODEL_RUNTIME_CLASS_19_WATER_SURFACE:
    break;
  default:
    return false;
  }
  *outArmy = army;
  return true;
}

/* True while the army drives its side step (the flag is dropped once its route point is another one). */
static bool PathingYield_SideStepRunning(ArmyRuntimeSlot *army)
{
  PathingUnitState *state;
  ArmyMovementRuntime *movement;
  int slotIndex;

  slotIndex = PathingYield_SlotIndex(army);
  if (slotIndex < 0) {
    return false;
  }
  state = &g_PathingUnitState[slotIndex];
  if ((state->flags & PATHING_UNIT_SIDE_STEP) == 0) {
    return false;
  }
  movement = ModelView_Cast<ArmyMovementRuntime>(army);
  if (Any(movement->movementStateFlags & ARMY_MOVEMENT_ACTIVE) &&
      (movement->movementWorldXQ12 == state->sideStepX) && (movement->movementWorldYQ12 == state->sideStepY)) {
    return true;
  }
  state->flags = (uint8_t)(state->flags & ~PATHING_UNIT_SIDE_STEP);
  return false;
}

/* True when a model of footprint radius at the point overlaps no model but the unit's own. */
static bool PathingYield_PointFreeOfModels
          (Q12 worldXQ12,Q12 worldYQ12,uint32_t radiusQ12,const ModelRuntimeSlot *unitModel,
          WorldRuntimeContext *worldRuntime)
{
  WorldOwnerListNode *ownerNode;
  ModelRuntimeSlot *candidate;

  for (ownerNode = worldRuntime->ownerListHead; ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    candidate = WorldOwnerNode_ModelRuntime(ownerNode);
    if (candidate == unitModel) {
      continue;
    }
    /* the radius test takes the point Y first */
    if (ArmyCollision_TestPointWithinExpandedRuntimeRadius((Q12)radiusQ12,worldYQ12,worldXQ12,candidate)) {
      return false;
    }
  }
  return true;
}

/* True when the point is a usable side-step target: inside the grid, its cell open for the given blocking mask
   and explored by the faction (presenceMask), and no other model in the way. */
static bool PathingYield_SideStepPointUsable
          (Q12 worldXQ12,Q12 worldYQ12,const ModelRuntimeSlot *unitModel,uint32_t blockingMask,
          uint32_t presenceMask,WorldRuntimeContext *worldRuntime)
{
  GridScratchCell *cell;
  uint32_t stateMask;
  int row;
  int column;

  if ((g_GridScratchPrimary == nullptr) || !PathingYield_CellOfWorldPoint(worldXQ12,worldYQ12,&row,&column)) {
    return false;
  }
  cell = g_GridScratchPrimary + (int32_t)(row * g_GridScratchWidth) + column;
  stateMask = (uint32_t)cell->stateMask;
  if (((stateMask & (blockingMask | GRID_SCRATCH_BLOCKED)) != 0) || ((stateMask & presenceMask) == 0)) {
    return false;
  }
  return PathingYield_PointFreeOfModels
           (worldXQ12,worldYQ12,unitModel->definitionOrSavedId.runtimeDefinition->footprintRadius,unitModel,
            worldRuntime);
}

/* The point distance away from (x, y) along side (+1 right, -1 left) of the direction (dirX, dirY) of length
   directionLength. "Right" is the perpendicular (dirY, -dirX). */
static void PathingYield_SidePoint(int worldX,int worldY,int dirX,int dirY,uint32_t directionLength,int side,
                                   int distance,Q12 *outX,Q12 *outY)
{
  *outX = (Q12)(worldX + (int)(((int64_t)side * dirY * distance) / (int64_t)directionLength));
  *outY = (Q12)(worldY + (int)(((int64_t)side * -dirX * distance) / (int64_t)directionLength));
}

void PathingYield_IssueSideStep(ArmyMovementRuntime *movementRuntime,Q12 worldXQ12,Q12 worldYQ12)
{
  PathingUnitState *state;
  int slotIndex;

  slotIndex = PathingYield_SlotIndex(movementRuntime);
  if ((slotIndex < 0) || Any(movementRuntime->movementStateFlags & ARMY_MOVEMENT_LOCKED)) {
    return;
  }
  ArmyRuntime_SetPendingMoveTarget(worldYQ12,worldXQ12,movementRuntime);
  state = &g_PathingUnitState[slotIndex];
  state->flags = (uint8_t)((state->flags | PATHING_UNIT_SIDE_STEP) & ~PATHING_UNIT_WAITING);
  state->sideStepX = worldXQ12;
  state->sideStepY = worldYQ12;
  DebugHook_PathingEvent('S',movementRuntime,nullptr);
}

/* Side step of unit (army, model) out of a line with direction (dirX, dirY) through (lineX, lineY): first to
   preferredSide, at least needPreferred far, then to the other side, at least needOther far. The candidate points
   are tested with the primary grid's static obstacles for the unit's class. Returns true when one was issued. */
static bool PathingYield_TrySideStep
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *army,ModelRuntimeSlot *model,int dirX,int dirY,
          int preferredSide,int needPreferred,int needOther)
{
  ModelDefinition *definition;
  ModelRuntimeNode *node;
  uint32_t directionLength;
  uint32_t blockingMask;
  uint32_t presenceMask;
  int sideIndex;
  int tryIndex;
  int side;
  int distance;
  Q12 pointX;
  Q12 pointY;

  if (Any(army->movementStateFlags & ARMY_MOVEMENT_LOCKED)) {
    return false;
  }
  directionLength = PathingYield_SquareRoot((uint64_t)((int64_t)dirX * dirX + (int64_t)dirY * dirY));
  if (directionLength == 0) {
    return false;
  }
  definition = model->definitionOrSavedId.runtimeDefinition;
  node = army->modelNodeRuntime;
  blockingMask = (uint32_t)(GRID_SCRATCH_LOW_BAND0 << ((uint8_t)definition->footprintRadiusClass & 31)) |
                 (uint32_t)(GRID_SCRATCH_TERRAIN_CLASS_BIT24 << ((uint8_t)definition->terrainTraversalClass & 31));
  presenceMask = 1u << ((uint8_t)army->factionIndex & 31);
  for (sideIndex = 0; sideIndex < 2; sideIndex++) {
    side = (sideIndex == 0) ? preferredSide : -preferredSide;
    distance = (sideIndex == 0) ? needPreferred : needOther;
    if (distance < PATHING_YIELD_MIN_SIDE_STEP_Q12) {
      distance = PATHING_YIELD_MIN_SIDE_STEP_Q12;
    }
    for (tryIndex = 0; tryIndex < PATHING_YIELD_COLLISION_TRIES; tryIndex++) {
      PathingYield_SidePoint((node->worldTransform).translation.x,(node->worldTransform).translation.y,dirX,dirY,
                             directionLength,side,distance,&pointX,&pointY);
      if (PathingYield_SideStepPointUsable(pointX,pointY,model,blockingMask,presenceMask,worldRuntime)) {
        PathingYield_IssueSideStep(ModelView_Cast<ArmyMovementRuntime>(army),pointX,pointY);
        return true;
      }
      distance += PATHING_YIELD_SIDE_STEP_EXTRA_Q12;
    }
  }
  return false;
}

/* The direction an army drives in: towards its route point, or (standing on it) towards fallbackX/Y. */
static void PathingYield_Direction(ArmyRuntimeSlot *army,int fallbackX,int fallbackY,int *outX,int *outY)
{
  ArmyMovementRuntime *movement;
  int worldX;
  int worldY;

  movement = ModelView_Cast<ArmyMovementRuntime>(army);
  worldX = (army->modelNodeRuntime->worldTransform).translation.x;
  worldY = (army->modelNodeRuntime->worldTransform).translation.y;
  *outX = movement->movementWorldXQ12 - worldX;
  *outY = movement->movementWorldYQ12 - worldY;
  if ((*outX == 0) && (*outY == 0)) {
    *outX = fallbackX - worldX;
    *outY = fallbackY - worldY;
  }
}

/* Lateral offset of the point (px, py) from the line through (x, y) with direction (dirX, dirY): returns the
   absolute offset and stores the side (+1 right incl. on the line, -1 left). */
static int PathingYield_LateralOffset(int worldX,int worldY,int dirX,int dirY,int pointX,int pointY,int *outSide)
{
  int64_t sideDot;
  uint32_t directionLength;

  sideDot = ((int64_t)pointX - worldX) * dirY - ((int64_t)pointY - worldY) * dirX;
  *outSide = (sideDot >= 0) ? 1 : -1;
  directionLength = PathingYield_SquareRoot((uint64_t)((int64_t)dirX * dirX + (int64_t)dirY * dirY));
  if (directionLength == 0) {
    return 0;
  }
  if (sideDot < 0) {
    sideDot = -sideDot;
  }
  return (int)(sideDot / (int64_t)directionLength);
}

ModelRuntimeSlot *PathingYield_FindBlockingRuntime
          (Q12 nextWorldYQ12,Q12 nextWorldXQ12,RuntimeCollisionQueryView *currentRuntime,
          WorldRuntimeContext *worldRuntime)
{
  ModelRuntimeNode *currentModelNode;
  ModelRuntimeNode *candidateModelNode;
  ModelRuntimeSlot *candidateModelRuntime;
  ModelRuntimeNode *candidateNode;
  uint32_t clearanceRadiusQ12;
  int currentX;
  int currentY;

  currentModelNode = currentRuntime->modelNodeRuntime;
  clearanceRadiusQ12 = currentRuntime->modelDefinition->footprintRadius;
  if (clearanceRadiusQ12 == 0) {
    return nullptr;
  }
  currentX = (currentModelNode->worldTransform).translation.x;
  currentY = (currentModelNode->worldTransform).translation.y;
  /* the scan of ArmyCollision_FindBlockingRuntimeForCurrentUnit, in the same order */
  for (candidateModelNode = ModelView_Cast<ModelRuntimeNode>(worldRuntime->ownerListHead); candidateModelNode != nullptr;
      candidateModelNode = ModelView_Cast<ModelRuntimeNode>((candidateModelNode->common).nextNode)) {
    if (candidateModelNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    if (!DepthBinMasks_Overlap(currentModelNode->depthBinMaskFar,currentModelNode->depthBinMaskNear,
                               candidateModelNode->depthBinMaskFar,candidateModelNode->depthBinMaskNear)) {
      continue;
    }
    candidateModelRuntime = (candidateModelNode->runtimePayload).modelRuntime;
    if (currentModelNode == candidateModelNode) {
      continue;
    }
    if ((candidateModelRuntime == currentRuntime->linkedRuntime) ||
        (ModelView_Cast<ModelRuntimeSlot>(currentRuntime) ==
         (candidateModelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime)) {
      continue;
    }
    if (!ArmyCollision_TestPointWithinExpandedRuntimeRadius
           (clearanceRadiusQ12,nextWorldYQ12,nextWorldXQ12,candidateModelRuntime)) {
      continue;
    }
    /* move apart: already overlapping and the step leads away from it */
    candidateNode = candidateModelRuntime->rootModelNodeOrSavedOffset.modelNode;
    if (ArmyCollision_TestPointWithinExpandedRuntimeRadius(clearanceRadiusQ12,currentY,currentX,candidateModelRuntime) &&
        (PathingYield_DistanceSquared(nextWorldXQ12,nextWorldYQ12,(candidateNode->worldTransform).translation.x,
                                      (candidateNode->worldTransform).translation.y) >
         PathingYield_DistanceSquared(currentX,currentY,(candidateNode->worldTransform).translation.x,
                                      (candidateNode->worldTransform).translation.y))) {
      continue;
    }
    return candidateModelRuntime;
  }
  return nullptr;
}

bool PathingYield_HandleBlocked
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *moverModel,ModelRuntimeSlot *blocker)
{
  ArmyRuntimeSlot *mover;
  ArmyRuntimeSlot *other;
  PathingUnitState *moverState;
  PathingUnitState *otherState;
  ModelRuntimeSlot *stepModel;
  ArmyRuntimeSlot *stepArmy;
  ArmyRuntimeSlot *partnerArmy;
  int moverSlot;
  int otherSlot;
  int moverX;
  int moverY;
  int otherX;
  int otherY;
  int dirX;
  int dirY;
  int lateral;
  int side;
  int radiusSum;
  bool otherActive;
  bool headOn;

  mover = moverModel->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  moverSlot = PathingYield_SlotIndex(mover);
  if (moverSlot < 0) {
    return false;
  }
  if (!Any(mover->movementStateFlags & ARMY_MOVEMENT_ACTIVE) ||
      !PathingYield_IsFriendlyMobileUnit(mover,blocker,&other)) {
    PathingYield_HandleFree(mover);
    return false;
  }
  otherSlot = PathingYield_SlotIndex(other);
  moverState = &g_PathingUnitState[moverSlot];
  otherState = &g_PathingUnitState[otherSlot];
  otherActive = Any(other->movementStateFlags & ARMY_MOVEMENT_ACTIVE);
  headOn = otherActive && ((otherState->flags & PATHING_UNIT_WAITING) != 0) &&
           (otherState->blockerSlot == moverSlot + 1);
  if (otherActive && !headOn &&
      (ModelView_Cast<ModelRuntimeGroundMovementSteeringView>(blocker)->movementControl.movementAdvancePerTickQ12 == 0)) {
    /* a friendly that has a move but stands (turning on the spot, or blocked itself): the original immediate
       re-plan when this unit moved since its last plan, but no retry wait longer than the wait budget (instead of
       the original's 64 steps) */
    PathingYield_HandleFree(mover);
    if (mover->movementRetryCountdown > (uint32_t)PATHING_YIELD_WAIT_STEPS) {
      mover->movementRetryCountdown = PATHING_YIELD_WAIT_STEPS;
    }
    return false;
  }
  moverState->flags = (uint8_t)(moverState->flags | PATHING_UNIT_WAITING);
  moverState->blockerSlot = (uint16_t)(otherSlot + 1);
  if (moverState->waitSteps >= PATHING_YIELD_WAIT_STEPS) {
    /* budget used up: re-plan at once, with the blocker as an obstacle (PathingYield_BeginReplan) */
    mover->movementRetryCountdown = 0;
    return false;
  }
  moverState->waitSteps++;
  moverX = (mover->modelNodeRuntime->worldTransform).translation.x;
  moverY = (mover->modelNodeRuntime->worldTransform).translation.y;
  otherX = (other->modelNodeRuntime->worldTransform).translation.x;
  otherY = (other->modelNodeRuntime->worldTransform).translation.y;
  radiusSum = (int)moverModel->definitionOrSavedId.runtimeDefinition->footprintRadius +
              (int)blocker->definitionOrSavedId.runtimeDefinition->footprintRadius + PATHING_YIELD_CLEARANCE_Q12;
  if (!otherActive) {
    /* idle friendly: it steps out of my line, away from it */
    PathingYield_Direction(mover,otherX,otherY,&dirX,&dirY);
    lateral = PathingYield_LateralOffset(moverX,moverY,dirX,dirY,otherX,otherY,&side);
    PathingYield_TrySideStep(worldRuntime,other,blocker,dirX,dirY,side,radiusSum - lateral,radiusSum + lateral);
  }
  else if (headOn && !PathingYield_SideStepRunning(mover) && !PathingYield_SideStepRunning(other)) {
    /* head-on: the lower slot index steps to its right */
    if (moverSlot < otherSlot) {
      stepArmy = mover;
      stepModel = moverModel;
      partnerArmy = other;
    }
    else {
      stepArmy = other;
      stepModel = blocker;
      partnerArmy = mover;
    }
    otherX = (partnerArmy->modelNodeRuntime->worldTransform).translation.x;
    otherY = (partnerArmy->modelNodeRuntime->worldTransform).translation.y;
    moverX = (stepArmy->modelNodeRuntime->worldTransform).translation.x;
    moverY = (stepArmy->modelNodeRuntime->worldTransform).translation.y;
    PathingYield_Direction(stepArmy,otherX,otherY,&dirX,&dirY);
    lateral = PathingYield_LateralOffset(moverX,moverY,dirX,dirY,otherX,otherY,&side);
    /* to the right: farther when the partner already is on the right */
    PathingYield_TrySideStep(worldRuntime,stepArmy,stepModel,dirX,dirY,1,
                             (side > 0) ? radiusSum + lateral : radiusSum - lateral,
                             (side > 0) ? radiusSum - lateral : radiusSum + lateral);
  }
  mover->movementStateFlags = mover->movementStateFlags & ~ARMY_MOVEMENT_ROUTE_POINT_REACHED;
  return true;
}

void PathingYield_HandleFree(ArmyRuntimeSlot *moverArmy)
{
  PathingUnitState *state;
  int slotIndex;

  slotIndex = PathingYield_SlotIndex(moverArmy);
  if (slotIndex < 0) {
    return;
  }
  state = &g_PathingUnitState[slotIndex];
  if ((state->flags & PATHING_UNIT_WAITING) != 0) {
    state->flags = (uint8_t)(state->flags & ~PATHING_UNIT_WAITING);
    state->waitSteps = 0;
    state->blockerSlot = 0;
  }
}

void PathingYield_BeginReplan(ArmyRuntimeSlot *plannerArmy,int startRow,int startColumn)
{
  PathingUnitState *state;
  ArmyRuntimeSlot *blockerArmy;
  ModelRuntimeSlot *blockerModel;
  ModelRuntimeNode *blockerNode;
  GridScratchCell *cell;
  uint32_t blockerRadius;
  int slotIndex;
  int centerRow;
  int centerColumn;
  int reach;
  int halfRows;
  int halfColumns;
  int lastRow;
  int lastColumn;
  int row;
  int column;
  uint8_t *saved;

  s_ObstacleStamped = false;
  slotIndex = PathingYield_SlotIndex(plannerArmy);
  if ((slotIndex < 0) || (g_GridScratchPrimary == nullptr)) {
    return;
  }
  state = &g_PathingUnitState[slotIndex];
  if (((state->flags & PATHING_UNIT_WAITING) == 0) || (state->waitSteps < PATHING_YIELD_WAIT_STEPS) ||
      (state->blockerSlot == 0) || (state->blockerSlot > ARMY_RUNTIME_SLOT_COUNT)) {
    return;
  }
  blockerArmy = &g_ArmyRuntimeSlots[state->blockerSlot - 1];
  if (blockerArmy->modelNodeRuntime == nullptr) {
    return;
  }
  blockerModel = blockerArmy->modelRuntimeOrSavedOffset.modelRuntime;
  if (blockerModel == nullptr) {
    return;
  }
  blockerNode = blockerArmy->modelNodeRuntime;
  blockerRadius = blockerModel->definitionOrSavedId.runtimeDefinition->footprintRadius;
  if ((blockerRadius == 0) ||
      !PathingYield_CellOfWorldPoint((blockerNode->worldTransform).translation.x,
                                     (blockerNode->worldTransform).translation.y,&centerRow,&centerColumn)) {
    return;
  }
  /* box around the largest ring: one scratch row is 500 world Y, one column 0x240 world X, half a column of skew
     per row */
  reach = (int)blockerRadius + (int)g_GridInfluenceRadiusOffset[7] + GRID_FOOTPRINT_RADIUS_MARGIN;
  halfRows = reach / GRID_SCRATCH_ROW_BELOW_WORLD_Y + 3;
  halfColumns = reach / GRID_SCRATCH_COLUMN_WORLD_X + halfRows / 2 + 3;
  s_SavedFirstRow = centerRow - halfRows;
  s_SavedFirstColumn = centerColumn - halfColumns;
  if (s_SavedFirstRow < 0) {
    s_SavedFirstRow = 0;
  }
  if (s_SavedFirstColumn < 0) {
    s_SavedFirstColumn = 0;
  }
  lastRow = centerRow + halfRows;
  lastColumn = centerColumn + halfColumns;
  if (lastRow > (int)g_GridScratchHeight - 1) {
    lastRow = (int)g_GridScratchHeight - 1;
  }
  if (lastColumn > (int)g_GridScratchWidth - 1) {
    lastColumn = (int)g_GridScratchWidth - 1;
  }
  s_SavedRowCount = lastRow - s_SavedFirstRow + 1;
  s_SavedColumnCount = lastColumn - s_SavedFirstColumn + 1;
  if ((s_SavedRowCount <= 0) || (s_SavedColumnCount <= 0) ||
      (s_SavedRowCount * s_SavedColumnCount > PATHING_YIELD_SAVE_CAPACITY)) {
    return;
  }
  saved = s_SavedLowBands;
  for (row = s_SavedFirstRow; row <= lastRow; row++) {
    cell = g_GridScratchPrimary + (int32_t)(row * g_GridScratchWidth) + s_SavedFirstColumn;
    for (column = s_SavedFirstColumn; column <= lastColumn; column++) {
      *saved++ = (uint8_t)(((uint32_t)cell->stateMask & GRID_SCRATCH_LOW_DISTANCE_BANDS) >> 8);
      cell++;
    }
  }
  s_ObstacleStamped = true;
  GridInfluence_SetLowDistanceBandsAroundWorldPoint
            (blockerRadius,(blockerNode->worldTransform).translation.y,(blockerNode->worldTransform).translation.x);
  /* the planner's own start cell keeps its bits, so the line test and the backtrack can leave it */
  if ((startRow >= s_SavedFirstRow) && (startRow <= lastRow) && (startColumn >= s_SavedFirstColumn) &&
      (startColumn <= lastColumn)) {
    cell = g_GridScratchPrimary + (int32_t)(startRow * g_GridScratchWidth) + startColumn;
    cell->stateMask = (GridScratchStateMask)(((uint32_t)cell->stateMask & ~GRID_SCRATCH_LOW_DISTANCE_BANDS) |
                      ((uint32_t)s_SavedLowBands[(startRow - s_SavedFirstRow) * s_SavedColumnCount +
                                                 (startColumn - s_SavedFirstColumn)] << 8));
  }
}

void PathingYield_EndReplan(ArmyRuntimeSlot *plannerArmy)
{
  PathingUnitState *state;
  GridScratchCell *cell;
  const uint8_t *saved;
  int slotIndex;
  int row;
  int column;

  if (s_ObstacleStamped && (g_GridScratchPrimary != nullptr)) {
    saved = s_SavedLowBands;
    for (row = s_SavedFirstRow; row < s_SavedFirstRow + s_SavedRowCount; row++) {
      cell = g_GridScratchPrimary + (int32_t)(row * g_GridScratchWidth) + s_SavedFirstColumn;
      for (column = 0; column < s_SavedColumnCount; column++) {
        cell->stateMask = (GridScratchStateMask)(((uint32_t)cell->stateMask & ~GRID_SCRATCH_LOW_DISTANCE_BANDS) |
                          ((uint32_t)*saved++ << 8));
        cell++;
      }
    }
  }
  s_ObstacleStamped = false;
  slotIndex = PathingYield_SlotIndex(plannerArmy);
  if (slotIndex < 0) {
    return;
  }
  /* a new plan starts a new wait budget */
  state = &g_PathingUnitState[slotIndex];
  state->flags = (uint8_t)(state->flags & ~PATHING_UNIT_WAITING);
  state->waitSteps = 0;
  state->blockerSlot = 0;
}

void PathingYield_NoteGroupPlan(GameEntityRuntime *routeEntityRuntime,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
                                WorldRuntimeContext *worldRuntime)
{
  ModelRuntimeNode *node;

  node = (routeEntityRuntime->common).ownership.modelNode;
  s_PlannerWorldXQ12 = (node->worldTransform).translation.x;
  s_PlannerWorldYQ12 = (node->worldTransform).translation.y;
  s_PlannerTargetXQ12 = targetWorldXQ12;
  s_PlannerTargetYQ12 = targetWorldYQ12;
  s_PlannerWorldRuntime = worldRuntime;
}

bool PathingYield_FindCorridorSideStep
          (EntityPathingRouteEntityRuntimeView *routeEntityRuntime,Q12 *outWorldXQ12,Q12 *outWorldYQ12)
{
  ArmyRuntimeSlot *army;
  ModelRuntimeSlot *model;
  uint32_t directionLength;
  int unitX;
  int unitY;
  int dirX;
  int dirY;
  int side;
  int sideIndex;
  int tryIndex;
  Q12 pointX;
  Q12 pointY;

  army = ModelView_Cast<ArmyRuntimeSlot>(routeEntityRuntime->movementRuntime);
  if ((PathingYield_SlotIndex(army) < 0) || (s_PlannerWorldRuntime == nullptr)) {
    return false;
  }
  model = army->modelRuntimeOrSavedOffset.modelRuntime;
  if (model == nullptr) {
    return false;
  }
  unitX = (routeEntityRuntime->modelNode->worldTransform).translation.x;
  unitY = (routeEntityRuntime->modelNode->worldTransform).translation.y;
  dirX = s_PlannerTargetXQ12 - s_PlannerWorldXQ12;
  dirY = s_PlannerTargetYQ12 - s_PlannerWorldYQ12;
  if ((dirX == 0) && (dirY == 0)) {
    return false;
  }
  directionLength = PathingYield_SquareRoot((uint64_t)((int64_t)dirX * dirX + (int64_t)dirY * dirY));
  (void)PathingYield_LateralOffset(s_PlannerWorldXQ12,s_PlannerWorldYQ12,dirX,dirY,unitX,unitY,&side);
  for (sideIndex = 0; sideIndex < 2; sideIndex++) {
    for (tryIndex = 1; tryIndex <= PATHING_YIELD_CORRIDOR_TRIES; tryIndex++) {
      PathingYield_SidePoint(unitX,unitY,dirX,dirY,directionLength,(sideIndex == 0) ? side : -side,
                             tryIndex * PATHING_YIELD_SIDE_STEP_EXTRA_Q12,&pointX,&pointY);
      /* the masks EntityPathing_UpdateRouteSegment set for this unit, on the copied grid with the corridor */
      if (PathingYield_SideStepPointUsable(pointX,pointY,model,(uint32_t)g_GridPathBlockingMask,
                                           (uint32_t)g_GridPathEntityClassMask,s_PlannerWorldRuntime)) {
        *outWorldXQ12 = pointX;
        *outWorldYQ12 = pointY;
        return true;
      }
    }
  }
  return false;
}
