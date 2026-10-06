/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/targeting.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/targeting.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* "Go to" action of the active in-game notification. In state 27 it only cancels (restores the camera, see
   InGameTargetingContext_CancelAndRestoreState). In state 7 it saves the camera state (unless bit 0x10 of the
   world runtimeFlags is set) and then, by payload kind: TECHNOLOGY_UNLOCK_POSITION selects the own model standing at
   the payload position (locally or as INGAME_COMMAND_SELECT_MODEL_AND_ARMY) and ends the interaction;
   FACTION_IMPACT_ANCHOR remembers the position and, like ARMY_CREATED, moves the camera onto the terrain point
   there and switches the interaction to state 27. Payload kinds 1, 2, 3 are handled, the rest
   do nothing.
*/
void InGameTargetingContext_AdvanceOrResolveTarget(InGameTargetingRootTraversalView *targetingContext)

{
  InGameTargetingRootTraversalView *root;
  InGameNotificationPayloadKind payloadKind;
  WorldOwnerListNode *ownerNode;
  ArmyRuntimeSlot *ownerArmy;
  CommandPayload modelToken;
  CommandPayload armyToken;
  FixedVectorQ12 nearestTerrainPoint;

  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    InGameTargetingContext_CancelAndRestoreState(targetingContext);
    return;
  }
  if (targetingContext->actionState != INGAME_TARGETING_OBSERVED_ADVANCE_OR_RESOLVE) {
    return;
  }
  /* walk up to the in-game root node */
  root = targetingContext;
  while (root->base.parent != UI_NODE_NONE) {
    root = UiNode_As<InGameTargetingRootTraversalView>(root->base.parent.get());
  }
  payloadKind = root->activeNotificationPayload.payloadKind;
  if ((root->worldRuntime.runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) == 0) {
    WorldRuntime_CaptureMotionStateToSnapshot(&root->worldRuntime);
  }
  switch(payloadKind) {
  case TECHNOLOGY_UNLOCK_POSITION:
    /* own model at exactly the payload position. Note: "modelToken" holds the rebased army offset and
       "armyToken" the rebased model offset (names follow the callee's parameter order). */
    for (ownerNode = root->worldRuntime.ownerListHead; ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
      if ((ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) ||
          (ownerNode->worldXQ12 != root->activeNotificationPayload.worldXQ12)) {
        continue;
      }
      ownerArmy = WorldOwnerNode_ModelRuntime(ownerNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
      if ((ownerNode->worldYQ12 != root->activeNotificationPayload.worldYQ12) ||
          (root->worldRuntime.activeFactionRuntimeIndex != ownerArmy->factionIndex)) {
        continue;
      }
      modelToken = ArmyRuntime_Token(ownerArmy);
      armyToken = (int)((uintptr_t)ownerNode->runtimePayload - g_ModelRuntimeRebaseDelta);
      InGameCommand_Issue<FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel>(0,armyToken,modelToken);
      root->notificationButtonCursorFrame = NOTIFICATION_INTERACTION_NONE;
      return;
    }
    break;
  case FACTION_IMPACT_ANCHOR:
    root->targetingWorldXQ12 = root->activeNotificationPayload.worldXQ12;
    root->targetingWorldYQ12 = root->activeNotificationPayload.worldYQ12;
    /* falls through */
  case ARMY_CREATED:
    FieldGrid_GetNearestTerrainPoint
              (root->activeNotificationPayload.worldYQ12,root->activeNotificationPayload.worldXQ12,
               root->worldRuntime.fieldGrid,&nearestTerrainPoint);
    WorldRuntime_PointCameraAtTarget
              (root->worldRuntime.motion.pitchAngle,root->activeNotificationPayload.headingAngle,
               root->worldRuntime.motion.targetDistanceQ12,nearestTerrainPoint.zQ12,
               root->activeNotificationPayload.worldYQ12,root->activeNotificationPayload.worldXQ12,
               &root->worldRuntime);
    root->worldRuntime.runtimeFlags = root->worldRuntime.runtimeFlags & ~WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO;
    root->notificationButtonCursorFrame = 27; /* next click cancels */
    break;
  default:
    break;
  }
}

/* Ends a notification "go to" (state 27): resets the state to idle, walks up to the in-game root, clears bit 0x10
   of its world runtimeFlags and restores the camera saved by InGameTargetingContext_AdvanceOrResolveTarget.
   Also the queued UI action handler for INGAME_PAGE10[14] (0x100E).
*/
void InGameTargetingContext_CancelAndRestoreState(InGameTargetingRootTraversalView *targetingContext)

{
  WorldRuntimeFlags *runtimeFlagsField;
  UiNodeBase *parentCursor;

  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    targetingContext->actionState = INGAME_TARGETING_OBSERVED_IDLE;
    parentCursor = targetingContext->base.parent;
    while (parentCursor != UI_NODE_NONE) {
      targetingContext = UiNode_As<InGameTargetingRootTraversalView>(targetingContext->base.parent.get());
      parentCursor = targetingContext->base.parent;
    }
    runtimeFlagsField = &targetingContext->worldRuntime.runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & ~WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO;
    WorldRuntime_RestoreMotionStateFromSnapshot(&targetingContext->worldRuntime);
  }
}
