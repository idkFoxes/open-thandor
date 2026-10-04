/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/selection/preview_markers.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SELECTION_PREVIEW_MARKERS_H
#define THANDOR_GAMEPLAY_SELECTION_PREVIEW_MARKERS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/selection/preview_markers. */

/* Tints of the ghost army that previews a placement or command-mode command
   (InGameWorldOverlay_RebuildOrReleaseTransientMarkers). */
#define OVERLAY_PREVIEW_TINT_ARGB 0xCFFFFFFF /* translucent white */
#define OVERLAY_PREVIEW_TINT_MULTI_CANDIDATE_ARGB 0x4FFFFFFF /* fainter: the placement has several candidates */
#define OVERLAY_PREVIEW_TINT_BLOCKED_MASK 0xFF707070 /* darkens the preview when the placement would fail */
/* Capacities of g_InGameOwnedEntityTransientEffectMarkers and g_InGameCommandTargetTransientEffectMarkers. */
#define OVERLAY_OWNED_MARKER_CAPACITY 32
#define OVERLAY_COMMAND_TARGET_MARKER_CAPACITY 128

/* Functions are grouped by semantic ownership. */

void InGameWorldOverlay_RebuildOrReleaseTransientMarkers
          (GraphicsBooleanState releaseMode,WorldRuntimeContext *worldRuntime);

void InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
          (Q12 scaleQ12,void *sourceWorldNode,Q12 worldYQ12,Q12 worldXQ12,void *effectDefinition,
          void *inGameRuntime);

extern intptr_t g_InGamePendingPlacementArmyAsset; /* ArmyAssetRecordPrefix * staged for placement, 0 when none */
extern uint32_t g_InGameCommandPreviewArmyAssetId;

#endif /* THANDOR_GAMEPLAY_SELECTION_PREVIEW_MARKERS_H */
