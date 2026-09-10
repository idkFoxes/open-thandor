# Call graph: `world/motion/runtime`

11 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0050D050` **WorldRuntime_ApplyEdgeScrollAndGetCursorFrame** — local: `WorldRuntime_TranslateCameraByScreenDelta`; cross: `PersistentSettings_ReadDword` → `core/settings/persistent`, `WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface` → `world/runtime/core`
- `0x0050C7F0` **WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn** — cross: `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `WorldRuntime_ClearFieldGridDirtyFlag` → `world/runtime/core`
- `0x0050C850` **WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn** — cross: `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `WorldRuntime_ClearFieldGridDirtyFlag` → `world/runtime/core`
- `0x0050C8C0` **WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading** — cross: `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `WorldRuntime_ClearFieldGridDirtyFlag` → `world/runtime/core`
- `0x0050C920` **WorldMotion_AdjustHeadingAndRecomputePosition** — cross: `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `WorldRuntime_ClearFieldGridDirtyFlag` → `world/runtime/core`
- `0x0050C990` **WorldMotion_AdjustHeadingAndClearFieldGridDirty** — cross: `WorldRuntime_ClearFieldGridDirtyFlag` → `world/runtime/core`
- `0x0050C9C0` **WorldMotion_AdjustDistanceClampAndRecomputePosition** — cross: `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `WorldRuntime_ClearFieldGridDirtyFlag` → `world/runtime/core`
- `0x0050CA80` **WorldMotion_AdjustPositionMagnitudeClamp** — cross: `WorldRuntime_ClearFieldGridDirtyFlag` → `world/runtime/core`
- `0x0050CB10` **WorldMotion_AdjustPitchClampAndRecomputePosition** — cross: `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `WorldRuntime_ClearFieldGridDirtyFlag` → `world/runtime/core`
- `0x0050CBE0` **WorldMotion_AdjustPitchClampAndClearFieldGridDirty** — cross: `WorldRuntime_ClearFieldGridDirtyFlag` → `world/runtime/core`
- `0x0050C770` **WorldRuntime_TranslateCameraByScreenDelta** — cross: `FixedMath_SinCosScaled` → `core/math/fixed`, `WorldRuntime_ClearFieldGridDirtyFlag` → `world/runtime/core`
