# Call graph: `core/math/interpolation`

[Source](../../../../src/core/math/interpolation.c) · [Header](../../../../include/thandor/core/math/interpolation.h) · [Graphviz](interpolation.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-core-math-interpolation)

12 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0053CA30` **[`WorldMotionSpline_EvaluateAndApplyAtTime`](../../../../src/core/math/interpolation.c#L5)** — local: [`CubicSpline_EvaluateValueQ12`](../../../../src/core/math/interpolation.c#L624), [`CubicSpline_EvaluateDerivativeQ12`](../../../../src/core/math/interpolation.c#L646), [`WorldMotionSpline_ClearCachedDerivatives`](../../../../src/core/math/interpolation.c#L603); cross: [`WorldRuntime_SetPosition60AndDistanceFromPosition80`](../../../../src/world/runtime/core.c#L503) → [`world/runtime/core`](../../world/runtime/core.md), [`WorldRuntime_SetMotionParameters6CThrough78Clamped`](../../../../src/world/runtime/core.c#L529) → [`world/runtime/core`](../../world/runtime/core.md)
- `0x0053CBB0` **[`WorldMotionSpline_EvaluateAndApplyOriginDistanceAtTimeCf`](../../../../src/core/math/interpolation.c#L104)** — local: [`CubicSpline_EvaluateValueQ12`](../../../../src/core/math/interpolation.c#L624), [`CubicSpline_EvaluateDerivativeQ12`](../../../../src/core/math/interpolation.c#L646), [`WorldMotionSpline_ClearCachedDerivatives`](../../../../src/core/math/interpolation.c#L603); cross: [`WorldRuntime_SetPosition80AndRebuildPosition60FromAngles`](../../../../src/world/runtime/core.c#L569) → [`world/runtime/core`](../../world/runtime/core.md)
- `0x0053CD10` **[`WorldMotionSpline_BuildSixChannelCurves`](../../../../src/core/math/interpolation.c#L195)** — local: [`CubicSpline_BuildNaturalCoefficientSystem`](../../../../src/core/math/interpolation.c#L427), [`CubicSpline_SolveCoefficientSystem`](../../../../src/core/math/interpolation.c#L371)
- `0x004CCC00` **[`InterpolationState_SetNegatedTargetAndRescaleProgress`](../../../../src/core/math/interpolation.c#L284)**
- `0x004CCC80` **[`InterpolationStateTable_Advance256ByTicks`](../../../../src/core/math/interpolation.c#L326)**
- `0x0053D230` **[`CubicSpline_SolveCoefficientSystem`](../../../../src/core/math/interpolation.c#L371)** — local: [`CubicSpline_ForwardEliminateColumn`](../../../../src/core/math/interpolation.c#L541), [`CubicSpline_BackSubstituteRow`](../../../../src/core/math/interpolation.c#L575)
- `0x0053CF10` **[`CubicSpline_BuildNaturalCoefficientSystem`](../../../../src/core/math/interpolation.c#L427)**
- `0x0053D160` **[`CubicSpline_ForwardEliminateColumn`](../../../../src/core/math/interpolation.c#L541)**
- `0x0053D1C0` **[`CubicSpline_BackSubstituteRow`](../../../../src/core/math/interpolation.c#L575)**
- `0x0053CA10` **[`WorldMotionSpline_ClearCachedDerivatives`](../../../../src/core/math/interpolation.c#L603)**
- `0x0053D2E0` **[`CubicSpline_EvaluateValueQ12`](../../../../src/core/math/interpolation.c#L624)**
- `0x0053D320` **[`CubicSpline_EvaluateDerivativeQ12`](../../../../src/core/math/interpolation.c#L646)**

## Called by

- [`assets/rom/runtime`](../../assets/rom/runtime.md): [`FrontendRomTransition_ProcessPendingRecord`](../../../../src/assets/rom/runtime.c#L203) → [`WorldMotionSpline_EvaluateAndApplyAtTime`](../../../../src/core/math/interpolation.c#L5); [`FrontendRomTransition_InitializeFromRecord`](../../../../src/assets/rom/runtime.c#L743) → [`WorldMotionSpline_BuildSixChannelCurves`](../../../../src/core/math/interpolation.c#L195)
- [`gameplay/selection/overlay`](../../gameplay/selection/overlay.md): [`InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf`](../../../../src/gameplay/selection/overlay.c#L5) → [`InterpolationState_SetNegatedTargetAndRescaleProgress`](../../../../src/core/math/interpolation.c#L284)
- [`gameplay/session/runtime`](../../gameplay/session/runtime.md): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../../../../src/gameplay/session/runtime.c#L87) → [`InterpolationStateTable_Advance256ByTicks`](../../../../src/core/math/interpolation.c#L326)
- [`ui/frontend/runtime`](../../ui/frontend/runtime.md): [`FrontendRuntime_UpdatePointerContextAndSceneViewCf`](../../../../src/ui/frontend/runtime.c#L1587) → [`WorldMotionSpline_BuildSixChannelCurves`](../../../../src/core/math/interpolation.c#L195); [`Frontend_Init`](../../../../src/ui/frontend/runtime.c#L2749) → [`WorldMotionSpline_ClearCachedDerivatives`](../../../../src/core/math/interpolation.c#L603)
- [`world/effects/maintenance`](../../world/effects/maintenance.md): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../../../../src/world/effects/maintenance.c#L114) → [`InterpolationState_SetNegatedTargetAndRescaleProgress`](../../../../src/core/math/interpolation.c#L284)
- [`world/shots/maintenance`](../../world/shots/maintenance.md): [`ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects`](../../../../src/world/shots/maintenance.c#L166) → [`InterpolationState_SetNegatedTargetAndRescaleProgress`](../../../../src/core/math/interpolation.c#L284)
