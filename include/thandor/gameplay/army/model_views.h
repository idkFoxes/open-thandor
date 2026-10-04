/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/model_views.h
 */

#ifndef THANDOR_GAMEPLAY_ARMY_MODEL_VIEWS_H
#define THANDOR_GAMEPLAY_ARMY_MODEL_VIEWS_H

/* The class-specific views of the model runtime, model definition and model node as overlays of the memory they
   view (core/slot.h THANDOR_SLOT_OVERLAY), so the class dispatch tables (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes,
   g_RuntimeMaintenanceCallbackPhases) accept functions that take the view:
   - ModelRuntime*View: a 0x200-byte view of a ModelRuntimeSlot (same object, first dword its definition); the
     dispatchers pass the model runtime itself.
   - ModelRuntimePlacementClass14View: the class-14 form of ModelRuntimePlacementValidationView (same object).
   - ModelDefinition: the full record behind a ModelDefinitionRecordPrefix (its first 0xC bytes are the prefix).
   - ShotModelRuntimeNode / EffectModelRuntimeNode: a ModelRuntimeNode of a shot / effect (runtimePayload read as
     the shot / effect runtime); the maintenance phases pass the node itself. */

#include <thandor/core/slot.h>
#include <thandor/assets/model/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/world/effects/types.h>
#include <thandor/world/model/types.h>
#include <thandor/world/shots/types.h>

#ifdef __cplusplus
THANDOR_SLOT_OVERLAY(ModelRuntimeUpdateView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimeWeaponAimStateView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimeVerticalDeploymentView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimeTimedTargetProjectileView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimeTimedEffectsUpdateView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimeLinkedChildSpawnAndBuildView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimeGroundMovementTrackView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimeGroundMovementSteeringView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimeDestroyEffectsView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimeClass21UpdateView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimeClass14UpdateView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimeArticulatedMovementDefinitionView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimePlacementValidationView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ModelRuntimePlacementClass14View, ModelRuntimePlacementValidationView);
THANDOR_SLOT_OVERLAY(ModelDefinition, ModelDefinitionRecordPrefix);
THANDOR_SLOT_OVERLAY(ShotModelRuntimeNode, ModelRuntimeNode);
THANDOR_SLOT_OVERLAY(EffectModelRuntimeNode, ModelRuntimeNode);
#endif /* __cplusplus */

#endif /* THANDOR_GAMEPLAY_ARMY_MODEL_VIEWS_H */
