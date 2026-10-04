/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/pool.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_POOL_H
#define THANDOR_GAMEPLAY_ARMY_POOL_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/pool. */

/* ArmyRuntimeSlot.runtimeFlags bit set when the army's health (actionVector2Q12) drops to zero
   (ArmyRuntime_ApplyImpactDamageAndFinalizeState and the other damage helpers). */
#define ARMY_RUNTIME_FLAG_DESTROYED 0x8
/* g_ArmyRuntimeSlots: a 0x48000-byte pool of 0x120-byte ArmyRuntimeSlot entries (ArmyRuntime_InitializePoolAndGraphics) */
#define ARMY_RUNTIME_SLOT_COUNT 0x400
/* g_ArmyGraphicsBindings: texture set and palette per faction slot 0-7 */
#define ARMY_GRAPHICS_BINDING_COUNT 8
/* ArmyRuntime_CreateInstanceFromAsset creationFlags */
#define ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION 0x2 /* owned by the active faction: +1 on builtCount of the
                                                     faction's selected model definition */
#define ARMY_CREATE_UNLOCK_TECHNOLOGY 0x4 /* ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology */

/* Army model runtime classState.stateFlags bits set and tested by the class update callbacks
   (ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive, the production slots 11/13/22) */
#define ARMY_MODEL_STATE_SWITCHED_OFF 0x1         /* powered down: no Energy demand, health decays to 3/4 */
#define ARMY_MODEL_STATE_DISMANTLING 0x10         /* being recycled: health drains, Xenite (xeniteValueQ4 >> 5) is refunded */
#define ARMY_MODEL_STATE_DESTRUCTION_STARTED 0x20 /* destruction effect spawned; skips the attachment channel ticks */
#define ARMY_MODEL_STATE_RESEARCHING 0x40         /* technology research in progress (researchTechnologyId) */
#define ARMY_MODEL_STATE_RESEARCH_UNPAID 0x80     /* research queued, Xenite not yet paid */
#define ARMY_MODEL_STATE_PRODUCING 0x100          /* a queued secondary army asset is being built */
#define ARMY_MODEL_STATE_DISMANTLED 0x200         /* dismantling finished (toggled together with DISMANTLING) */
#define ARMY_MODEL_STATE_NO_REGENERATION 0x400    /* health does not regenerate */
#define ARMY_MODEL_STATE_RALLY_POINT_SET 0x800    /* class 13: the exit point (classLinkState.classState78/7C) was set
                                                     by the player */
/* SWITCHED_OFF | DESTROYED: the model does nothing this tick */
#define ARMY_MODEL_STATE_INACTIVE_MASK (ARMY_MODEL_STATE_SWITCHED_OFF | ARMY_RUNTIME_FLAG_DESTROYED)
/* INACTIVE_MASK | RESEARCHING | RESEARCH_UNPAID: a production class may start a new build */
#define ARMY_MODEL_STATE_BUILD_BLOCKING_MASK \
          (ARMY_MODEL_STATE_INACTIVE_MASK | ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_RESEARCH_UNPAID)

/* g_ArmyRuntimeDepthBinClassByModelClass entries (ArmyRuntimeSlot.depthBinClass): the occupancy bits an army
   marks in its faction's byte of the field cells. Structure classes (4, 11, 13-16, 20, 22, 23) use 0x90. */
#define ARMY_DEPTH_BIN_STRUCTURE_BIT 0x10
#define ARMY_DEPTH_BIN_CLASS_STRUCTURE 0x90   /* persistent bit 7 | ARMY_DEPTH_BIN_STRUCTURE_BIT */

/* Functions are grouped by semantic ownership. */

Bool8 ArmyRuntime_InitializePoolAndGraphics(void *ownerContext,uint16_t *graphicsBasePath,uint32_t *outError);

void ArmyRuntime_ShutdownPoolAndGraphics(void);

void ArmyRuntimePool_ConvertPointersToOffsetsForSave(void);

void ArmyRuntimePool_RebaseAfterLoad(void);

void ArmyRuntime_DestroyInstanceAndRefreshUi(WorldRuntimeContext *worldRuntime,GameEntityRuntime *entityRuntime);

ArmyRuntimeSlot *ArmyRuntime_CreateInstanceFromAsset
          (WorldObjectAllocationFlags creationFlags,AngleTurn32 orientationAngle,Q12 worldXQ12,
          Q12 worldYQ12,FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime,uint32_t *outError);

void ArmyRuntime_InitializeTerrainOccupancyFlags (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

extern ArmyRuntimeSlot *g_ArmyRuntimeSlots;
extern ArmyGraphicsBinding g_ArmyGraphicsBindings[8];
extern const uint32_t g_ArmyRuntimeDepthBinClassByModelClass[24]; /* uint32_t[24] depth-bin/occupancy class per model runtime class (0x88/0x90/0xA0/0xC0; 0x90 = structure), copied to ArmyRuntimeSlot.depthBinClass; gameplay/army runtime and placement */

extern void *g_ArmyRuntimeRebaseBaseMinusOne;

/* Faction graphics ('gfx') texture source asset layout used by ArmyGraphics_CopyFrontendPlayerPaletteAndTexture */
#define ARMY_GRAPHICS_PLAYER_IMAGE_SUBRESOURCE 0x71 /* image replaced by the frontend player's picture */
#define ARMY_GRAPHICS_PALETTE_TABLE_OFFSET 0x200    /* first palette, from the asset start */
#define ARMY_GRAPHICS_PALETTE_BYTES 0x800           /* 256 entries of 8 bytes */
#define ARMY_GRAPHICS_PLAYER_IMAGE_DWORDS 0x400     /* 0x1000 bytes of pixel data */

void ArmyGraphics_CopyFrontendPlayerPaletteAndTexture(FrontendPlayerRuntimeId frontendPlayerRuntimeId,
          ArmyGraphicsAssetAddress32 armyGraphicsAsset);

#endif /* THANDOR_GAMEPLAY_ARMY_POOL_H */
