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
/* The owner-list and generic views of a world node (every world node has the ModelRuntimeNode shape: common
   links, model payload, runtimePayload). */
THANDOR_SLOT_OVERLAY(WorldOwnerListNode, ModelRuntimeNode);
THANDOR_SLOT_OVERLAY(WorldRuntimeNode, ModelRuntimeNode);
/* The army-side views (step 13 X5b): the walker, collision-query and linked-child mask views are laid over a
   model runtime (the class code passes its model runtime); ArmyMovementRuntime is the movement view of an army
   runtime slot (ModelRuntimeSlot.ownerArmyRuntimeOrSavedOffset, GameEntityRuntime.linkedEntityRuntime). */
THANDOR_SLOT_OVERLAY(ArmyArticulatedRuntimeSlotView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(RuntimeCollisionQueryView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ArmyRuntimeLinkedChildMaskSlotView, ModelRuntimeSlot);
THANDOR_SLOT_OVERLAY(ArmyMovementRuntime, ArmyRuntimeSlot);
/* The full army asset record behind the registry's ArmyAssetRecordPrefix (same leading fields). */
THANDOR_SLOT_OVERLAY(ArmyAssetRecord, ArmyAssetRecordPrefix);

/* GameEntityRuntime is the common entity view of both record families (army runtime slots and model runtimes,
   see GameEntityOwnershipState10), so it is not registered with one base; ModelView_Cast accepts it against a
   view of either. */
template <class T> constexpr bool thandor_army_is_entity_record()
{
    return thandor_slot_is_view_of<T, ModelRuntimeSlot>() || thandor_slot_is_view_of<T, ArmyRuntimeSlot>();
}

/* ModelView_Cast<To>(p): the named cast between two views of the same record (step 13 X5; replaces the C-style
   casts between them). It compiles only when one type is registered as a view of the other (THANDOR_SLOT_OVERLAY,
   THANDOR_SLOT_PREFIX in core/slot.h), both are views of a ModelRuntimeSlot or both of an ArmyRuntimeSlot, or one
   is GameEntityRuntime and the other a view of either record, so it cannot reach an unrelated record. The
   conversion is the reinterpret_cast the C-style cast was (same address, no adjustment). */
template <class To, class From> inline To *ModelView_Cast(From *record)
{
    using ToBare = std::remove_cv_t<To>;
    using FromBare = std::remove_cv_t<From>;
    static_assert(thandor_slot_is_view_of<ToBare, FromBare>() || thandor_slot_is_view_of<FromBare, ToBare>() ||
                      (thandor_slot_is_view_of<ToBare, ModelRuntimeSlot>() &&
                       thandor_slot_is_view_of<FromBare, ModelRuntimeSlot>()) ||
                      (thandor_slot_is_view_of<ToBare, ArmyRuntimeSlot>() &&
                       thandor_slot_is_view_of<FromBare, ArmyRuntimeSlot>()) ||
                      (std::is_same_v<ToBare, GameEntityRuntime> && thandor_army_is_entity_record<FromBare>()) ||
                      (std::is_same_v<FromBare, GameEntityRuntime> && thandor_army_is_entity_record<ToBare>()),
                  "ModelView_Cast: the types are not registered views of one record");
    return reinterpret_cast<To *>(record);
}
/* The same for a typed 32-bit pointer field (the load, then the view cast). */
template <class To, class From> inline To *ModelView_Cast(const Ptr32<From> &field)
{
    return ModelView_Cast<To>(field.get());
}
#endif /* __cplusplus */

#endif /* THANDOR_GAMEPLAY_ARMY_MODEL_VIEWS_H */
