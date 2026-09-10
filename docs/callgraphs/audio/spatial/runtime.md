# Call graph: `audio/spatial/runtime`

10 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0050B5D0` **SpatialSoundPool_Init**
- `0x0050B600` **SpatialSound_RebuildListenerTransformFromPose** — cross: `FixedTransform_BuildRotationBasis` → `core/math/fixed`, `FixedTransform_Compose` → `core/math/fixed`
- `0x0050B6E0` **SpatialSound_PlayPositionedOneShot** — cross: `FixedTransform_ApplyPoint` → `core/math/fixed`, `FixedMath_VectorToAnglesAndLength3Regs` → `core/math/fixed`
- `0x0050B7D0` **SpatialSound_UpdateDesiredPositionedGains** — cross: `FixedTransform_ApplyPoint` → `core/math/fixed`, `FixedMath_VectorToAnglesAndLength3Regs` → `core/math/fixed`
- `0x0050B8C0` **SpatialSoundSlot_CreateFromSampleAsset**
- `0x0050B940` **SpatialSoundSlot_CreateFromPcm**
- `0x0050B9D0` **SpatialSoundSlot_ReleaseSample**
- `0x0050BA00` **SpatialSoundSlot_ReleasePcm**
- `0x0050BA30` **SpatialSoundPool_ClearDesiredGains**
- `0x0050BA60` **SpatialSoundPool_ApplyDesiredGains**
