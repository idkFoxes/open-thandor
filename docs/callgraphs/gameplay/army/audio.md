# Call graph: `gameplay/army/audio`

10 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0051D5F0` **ArmyGraphics_CopyFrontendPlayerPaletteAndTexture**
- `0x00520740` **ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantA** — cross: `TerrainGrid_TestProjectedCellMaskBits01Cf` → `world/terrain/grid`, `SpatialSound_UpdateDesiredPositionedGains` → `audio/spatial/runtime`
- `0x00520E60` **ArmyRuntimeAudio_DispatchPositionedSoundVariant** — cross: `ArmyRuntimeClass_UpdatePositionedSoundsVariantB` → `gameplay/army/runtime`, `ArmyRuntimeClass_UpdatePositionedSoundsVariantA` → `gameplay/army/runtime`
- `0x00523240` **ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantB** — cross: `TerrainGrid_TestProjectedCellMaskBits01Cf` → `world/terrain/grid`, `SpatialSound_UpdateDesiredPositionedGains` → `audio/spatial/runtime`
- `0x00523DD0` **ArmyRuntimeAudio_UpdateMovementProjectedLoopingSound** — cross: `TerrainGrid_TestProjectedCellMaskBits01Cf` → `world/terrain/grid`, `SpatialSound_UpdateDesiredPositionedGains` → `audio/spatial/runtime`
- `0x00524410` **ArmyRuntimeAudio_UpdateConditionalProjectedSound** — cross: `TerrainGrid_TestProjectedCellMaskBits01Cf` → `world/terrain/grid`, `SpatialSound_UpdateDesiredPositionedGains` → `audio/spatial/runtime`
- `0x00524DA0` **ArmyRuntimeAudio_UpdatePrimaryAndSecondaryProjectedSounds** — cross: `TerrainGrid_TestProjectedCellMaskBits01Cf` → `world/terrain/grid`, `SpatialSound_UpdateDesiredPositionedGains` → `audio/spatial/runtime`
- `0x00526490` **ArmyRuntimeAudio_UpdateAssetProjectedSound** — cross: `TerrainGrid_TestProjectedCellMaskBits01Cf` → `world/terrain/grid`, `SpatialSound_UpdateDesiredPositionedGains` → `audio/spatial/runtime`
- `0x00526EB0` **ArmyRuntimeAudio_UpdateTerrainContactAndArticulatedProjectedSounds** — cross: `TerrainGrid_TestProjectedCellMaskBits01Cf` → `world/terrain/grid`, `SpatialSound_UpdateDesiredPositionedGains` → `audio/spatial/runtime`
- `0x00527B20` **ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled** — cross: `ArmyRuntime_UpdateLoopingPositionedSound` → `gameplay/army/runtime`
