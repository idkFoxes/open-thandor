/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/data.h
 */

#ifndef THANDOR_GAMEPLAY_ARMY_DATA_H
#define THANDOR_GAMEPLAY_ARMY_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern GraphicsOffscreenRenderModelListToTextureSourceProc *g_GraphicsOffscreenRenderModelListToTextureSource;

/* 00485A08..00485A24: origin X/Y/Z, projection scale, view angles 0/1, projection shift; passed whole to
   g_GraphicsOffscreenRenderModelListToTextureSource */
extern GraphicsOffscreenViewParameters g_ArmyPreviewViewParameters;

extern AngleTurn32 g_ArmyPreviewAuxiliaryOrientation[2];

/* 00485A2C..00485A34: horizontalExtent = primary colour ARGB, verticalExtent = secondary colour ARGB; passed whole
   to g_GraphicsOffscreenRenderModelListToTextureSource, which forwards them as the scene colour pairs of
   Graphics_SetSceneBoundsAndColors */
extern GraphicsOffscreenSceneExtents g_ArmyPreviewSceneExtents;

extern ModelRuntimeNode *g_ArmyPreviewModelNode; /* (one-entry model list) */

extern int32_t g_ModelBoundsMinimumX;

extern int32_t g_ModelBoundsMaximumX;

extern int32_t g_ModelBoundsMinimumY;

extern int32_t g_ModelBoundsMaximumY;

extern int32_t g_ModelBoundsMinimumZ;

extern int32_t g_ModelBoundsMaximumZ;

extern ArmyPlacementContactCallbackTable5 g_ArmyPlacementContactKindDispatchTable;

extern ArmyRuntimeSlot *g_ArmyRuntimeSlots;

extern ArmyGraphicsBinding g_ArmyGraphicsBindings[8];

extern uint64_t g_ArmyPreviewAlphaPremultiplyMmxLut256[256]; /* uint64_t[256] MMX qword per alpha a: three 16-bit lanes (a * 0x101) >> 4, alpha lane 0; PMULHW premultiply of the 2x2 downsample in the army preview (gameplay/army/runtime.c) */

extern uint64_t g_ArmyPreviewAverageAlphaReciprocalMmxLut256[256]; /* uint64_t[256] MMX qword per average alpha a: three lanes ~0x3FF0/a (reciprocal), fourth lane a; un-premultiplies the averaged army preview pixel (gameplay/army/runtime.c) */

extern uint64_t g_ArmyPreviewDownsampleAlphaRoundingBiasMmx;

extern ArmyCommandGeneration g_ArmyCommandGenerationStandard;

extern uint32_t g_ArmyRuntimeDepthBinClassByModelClass[24]; /* uint32_t[24] depth-bin/occupancy class per model runtime class (0x88/0x90/0xA0/0xC0; 0x90 = structure), copied to ArmyRuntimeSlot.depthBinClass; gameplay/army runtime and placement */

extern ArmyRuntimeOrderHandlerMatrix11x24 g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes;

extern GraphicsFixedVec3 g_ArmySuspensionBlendVectorA;

extern GraphicsFixedVec3 g_ArmySuspensionBlendVectorB;

extern GraphicsFixedMatrix3x4 g_ArmySuspensionRotationMatrixScratchA;

extern GraphicsFixedMatrix3x4 g_ArmySuspensionRotationMatrixScratchB;

extern GraphicsFixedMatrix3x4 g_ArmySuspensionRotationMatrixComposedScratch; /* 00520F28 g_ArmySuspensionRotationMatrixComposedScratch; followed by 0x90 code alignment fill up to 00520F60 */

extern RuntimeMaintenanceCallbackPhasesTyped g_RuntimeMaintenanceCallbackPhases;

extern InGameSimulationStepBatchTicks g_InGameSimulationStepTicks;

extern uint32_t g_InGamePointerModePreviewArmyIds[8]; /* uint32_t[8]: preview army asset id per pointer mode (0 = none); gameplay/input/world.c */

extern ArmyPlacementCandidateCount g_ArmyPlacementLateRejectionCount;

#endif
