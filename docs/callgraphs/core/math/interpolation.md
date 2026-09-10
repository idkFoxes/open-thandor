# Call graph: `core/math/interpolation`

12 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0053CA30` **WorldMotionSpline_EvaluateAndApplyAtTime** — local: `CubicSpline_EvaluateValueQ12`, `CubicSpline_EvaluateDerivativeQ12`, `WorldMotionSpline_ClearCachedDerivatives`; cross: `WorldRuntime_SetPosition60AndDistanceFromPosition80` → `world/runtime/core`, `WorldRuntime_SetMotionParameters6CThrough78Clamped` → `world/runtime/core`
- `0x0053CBB0` **WorldMotionSpline_EvaluateAndApplyOriginDistanceAtTimeCf** — local: `CubicSpline_EvaluateValueQ12`, `CubicSpline_EvaluateDerivativeQ12`, `WorldMotionSpline_ClearCachedDerivatives`; cross: `WorldRuntime_SetPosition80AndRebuildPosition60FromAngles` → `world/runtime/core`
- `0x0053CD10` **WorldMotionSpline_BuildSixChannelCurves** — local: `CubicSpline_BuildNaturalCoefficientSystem`, `CubicSpline_SolveCoefficientSystem`
- `0x004CCC00` **InterpolationState_SetNegatedTargetAndRescaleProgress**
- `0x004CCC80` **InterpolationStateTable_Advance256ByTicks**
- `0x0053D230` **CubicSpline_SolveCoefficientSystem** — local: `CubicSpline_ForwardEliminateColumn`, `CubicSpline_BackSubstituteRow`
- `0x0053CF10` **CubicSpline_BuildNaturalCoefficientSystem**
- `0x0053D160` **CubicSpline_ForwardEliminateColumn**
- `0x0053D1C0` **CubicSpline_BackSubstituteRow**
- `0x0053CA10` **WorldMotionSpline_ClearCachedDerivatives**
- `0x0053D2E0` **CubicSpline_EvaluateValueQ12**
- `0x0053D320` **CubicSpline_EvaluateDerivativeQ12**
