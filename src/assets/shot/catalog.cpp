/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/shot/catalog.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/shot/catalog.h>
#include <thandor/thandor.h>

/* Module data. */

ShotDefinition *g_ShotDefinitionRegistry[256] = {0};

/* Implementation ownership: assets/shot/catalog. */

/* Registers every shot definition of a loaded SHT asset: checks the 'sht' magic and converter version
   0x60006, then hands each 0x2E0-byte record after the 0x200-byte header to
   ShotDefinition_RegisterAndResolveReferences, stopping at the first failure. An invalid header leaves the
   asset path in g_PackageLastErrorPath and fails with FATAL_ERROR_SHOT_ASSET_INVALID.
   Returns 0 on success, otherwise the error code (the original's success return value was never used by its
   callers).
*/
uint32_t ShotAsset_PrepareEntries(ShotAssetHeader *asset)

{
  uint32_t registrationError;
  AssetRecordCount entriesRemaining;
  ShotDefinition *definition;

  if ((asset->entryCountHeader.common.magic == ASSET_MAGIC_SHT) &&
     (asset->entryCountHeader.common.converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    definition = (ShotDefinition *)(asset + 1);
    for (entriesRemaining = asset->entryCountHeader.entryCount; entriesRemaining != 0; entriesRemaining--) {
      registrationError = ShotDefinition_RegisterAndResolveReferences(definition);
      if (registrationError != 0) {
        return registrationError;
      }
      definition++;
    }
    return 0;
  }
  Package_SetLastErrorPath((uint16_t *)asset);
  return FATAL_ERROR_SHOT_ASSET_INVALID;
}


/* Runs after the level's terrain materials are loaded: checks that each of the 31 terrain-material indices
   of every registered shot definition is negative (no material) or names a loaded material. Otherwise the
   registry index of the offending shot is written to g_PackageLastErrorPath and the check fails with
   FATAL_ERROR_SHOT_TERRAIN_MATERIAL_INVALID. Returns 0 on success, otherwise that error code (the original's
   success return value, the last index checked, was read by no caller).
*/
uint32_t ShotDefinitions_ValidateTerrainMaterialReferences(void)

{
  TerrainMaterialIndex materialIndex;
  int registrySlotsRemaining;
  int materialIndicesRemaining;
  ShotDefinition *definition;
  TerrainMaterialIndex *materialIndexCursor;
  ShotDefinition **registryCursor;

  registryCursor = g_ShotDefinitionRegistry;
  for (registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
       registrySlotsRemaining--) {
    definition = *registryCursor;
    if (definition != NULL) {
      materialIndexCursor = definition->terrainMaterialIndices31;
      for (materialIndicesRemaining = SHOT_TERRAIN_MATERIAL_REFERENCE_COUNT; materialIndicesRemaining != 0;
           materialIndicesRemaining--) {
        materialIndex = *materialIndexCursor;
        materialIndexCursor++;
        if (TERRAIN_MATERIAL_COUNT - 1 < materialIndex ||
            (-1 < materialIndex && g_TerrainMaterialTextureSets[materialIndex] == NULL)) {
          g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                     SHOT_DEFINITION_REGISTRY_SLOT_COUNT - registrySlotsRemaining,g_PackageLastErrorPath);
          return FATAL_ERROR_SHOT_TERRAIN_MATERIAL_INVALID;
        }
      }
    }
    registryCursor++;
  }
  return 0;
}


/* Looks up a registered shot definition by id: returns 0 and stores it in *outDefinition. On a miss the id is
   written as decimal text to g_PackageLastErrorPath for the fatal-error message, *outDefinition is left untouched
   and FATAL_ERROR_SHOT_ID_NOT_FOUND is returned.
*/
uint32_t ShotDefinitionRegistry_FindByIdWithError
          (PckShotDefinitionIdCatalog definitionId,ShotDefinition **outDefinition)

{
  ShotDefinition *registeredDefinition;
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;

  registryCursor = g_ShotDefinitionRegistry;
  for (registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
       registrySlotsRemaining--) {
    registeredDefinition = *registryCursor;
    if (registeredDefinition != NULL && registeredDefinition->definitionId == definitionId) {
      *outDefinition = registeredDefinition;
      return 0;
    }
    registryCursor++;
  }
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
  return FATAL_ERROR_SHOT_ID_NOT_FOUND;
}


/* Computes the heading and elevation (Angle16) at which a shot of this definition must leave launchPoint to reach
   targetPoint. Ballistic shots solve the projectile equation (gravity = ballisticDivisorQ12) and take the high arc,
   the low arc only when the height difference is below 1.0; fixed-range shots always go straight up (0, 0x4000);
   all others aim directly, raised by the definition's elevation offset and capped at straight up. The callers
   pass the points in Z, Y, X order (Z = height). Called directly by the weapon aiming code (gameplay/army/combat.c,
   movement.c, runtime.c) and the shot launch in world/shots/runtime.c.
*/
ShotLaunchAngles ShotDefinition_ComputeLaunchAngles
          (Q12 targetZ,Q12 targetY,Q12 targetX,Q12 launchZ,Q12 launchY,Q12 launchX,
          ShotDefinition *definition)

{
  int64_t discriminant;
  int rangeTimesDivisor;
  int launchSpeedSquared;
  int heightDelta;
  uint32_t computedElevation;
  uint32_t computedHeading;
  ShotLaunchAngles clampedAngles;
  ShotLaunchAngles fixedRangeAngles;
  FixedLengthAngle planarLengthAngle;
  ShotLaunchAngles launchAngles;
  FixedVectorAngles vectorAngles;

  heightDelta = targetZ - launchZ;
  if (definition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    planarLengthAngle = FixedMath_Vector2AngleAndLength(targetY - launchY,targetX - launchX);
    computedHeading = planarLengthAngle.angle & FIXED_ANGLE16_MASK;
    /* tan(elevation) = (v^2 +- sqrt(v^4 - 2*g*h*v^2 - (g*r)^2)) / (g*r) */
    launchSpeedSquared = definition->launchSpeedQ12 * definition->launchSpeedQ12;
    rangeTimesDivisor = planarLengthAngle.length * definition->ballisticDivisorQ12;
    discriminant =
         (int64_t)(launchSpeedSquared + definition->ballisticDivisorQ12 * heightDelta * -2) *
         (int64_t)launchSpeedSquared -
         (int64_t)rangeTimesDivisor * (int64_t)rangeTimesDivisor;
    if (discriminant < 0) {
      discriminant = 0; /* target out of reach */
    }
    computedElevation =
         FIXED_UINT64_SQRT(discriminant);
    if ((-Q12_ONE < heightDelta) && (heightDelta < Q12_ONE)) {
      computedElevation = -computedElevation;
    }
    computedElevation = FixedMath_Atan2Angle16(launchSpeedSquared + computedElevation,rangeTimesDivisor);
  }
  else {
    if (definition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
      fixedRangeAngles.headingAngle = 0;
      fixedRangeAngles.elevationAngle = FIXED_ANGLE16_QUARTER_TURN;
      return fixedRangeAngles;
    }
    vectorAngles = FixedMath_VectorToAngles(heightDelta,targetY - launchY,targetX - launchX);
    computedHeading = vectorAngles.azimuthAngle;
    computedElevation = vectorAngles.elevationAngle + definition->elevationOffsetAngle16;
    if (FIXED_ANGLE16_QUARTER_TURN < (int)computedElevation) {
      clampedAngles.elevationAngle = FIXED_ANGLE16_QUARTER_TURN;
      clampedAngles.headingAngle = computedHeading;
      return clampedAngles;
    }
  }
  launchAngles.elevationAngle = computedElevation;
  launchAngles.headingAngle = computedHeading;
  return launchAngles;
}


/* Returns how far a shot of this definition reaches, used when a model's weapons are summed up for target
   selection: ballistic shots 9/8 of speed^2 / divisor, fixed-range shots their stored range (mode2SelectionRangeQ12), all
   others speed times the flight time, where the ramp-up ticks count only one third.
*/
uint32_t ShotDefinition_ComputeSelectionRange(ShotDefinition *definition)

{
  uint32_t selectionRangeQ12;

  if (definition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    selectionRangeQ12 =
         (uint32_t)((int)(((int64_t)definition->launchSpeedQ12 * (int64_t)definition->launchSpeedQ12)
                     / (int64_t)definition->ballisticDivisorQ12) * 9) >> 3;
  }
  else if (definition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
    selectionRangeQ12 = definition->mode2SelectionRangeQ12;
  }
  else {
    /* lifetime - 2/3 of the ramp duration (-2/3 in Q12 = -0xAAA) */
    selectionRangeQ12 =
         (((int)(definition->trajectoryRampDurationTicks * -(Q12_ONE * 2 / 3)) >> Q12_SHIFT) +
         definition->projectileLifetimeTicks) * definition->launchSpeedQ12;
  }
  return selectionRangeQ12;
}

/* Returns the shot speed used to lead a moving target: the launch speed (launchSpeedQ12) for unguided shots
   that do not fly a direct line, INT32_MAX (no lead) for direct-line or guided (guidanceTurnLimitAngle16
   non-zero) shots. Called directly by the target aim-point computation in gameplay/army/runtime.c.
*/
Q12 ShotDefinition_GetLeadSpeed(ShotDefinition *definition)

{
  uint32_t leadSpeedQ12;

  leadSpeedQ12 = INT32_MAX;
  if ((definition->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) &&
     (definition->guidanceTurnLimitAngle16 == 0)) {
    leadSpeedQ12 = definition->launchSpeedQ12;
  }
  return (Q12)leadSpeedQ12;
}


/* Returns the extra lead time for unguided lead-adjusted shots (trajectory mode 3, guidanceTurnLimitAngle16
   zero): about two thirds (0xAB / 256) of the ramp-up ticks (trajectoryRampDurationTicks), during which the shot is still accelerating; 0 for all
   other shots. The aim-point computation in gameplay/army/runtime.c multiplies it by the target's speed.
*/
uint32_t ShotDefinition_ComputeRampUpLeadTime(ShotDefinition *definition)

{
  uint32_t leadAdjustmentQ12;
  
  leadAdjustmentQ12 = 0;
  if ((definition->trajectoryMode == SHOT_TRAJECTORY_LEAD_ADJUSTED) &&
     (definition->guidanceTurnLimitAngle16 == 0)) {
    leadAdjustmentQ12 = definition->trajectoryRampDurationTicks * 171 >> 8;
  }
  return leadAdjustmentQ12;
}


/* Loads the sprite of a freshly registered shot definition: switches the resource path to .spr, loads it and
   either registers it as owned by the definition or, when a sprite with the same id is already registered,
   reuses that one and releases the fresh load. Returns 0 or the error code. */
static uint32_t ShotDefinition_LoadSprite(ShotDefinition *definition)
{
  SpriteAssetHeader *loadedSprite;
  SpriteAssetHeader *existingSprite;
  uint32_t loadErrorCode;
  uint32_t spriteRegisterError;

  if (WidePath_SetExtensionCode(ASSET_MAGIC_SPR,definition->resourcePathUtf16)) {
    /* Original quirk: the original returns the leftover error code of the earlier duplicate-id lookup, so a
       failing .spr extension switch returns FATAL_ERROR_SHOT_ID_NOT_FOUND */
    return FATAL_ERROR_SHOT_ID_NOT_FOUND;
  }
  loadedSprite = (SpriteAssetHeader *)Package_LoadEntry(definition->resourcePathUtf16,&loadErrorCode);
  if (loadedSprite == NULL) {
    return loadErrorCode;
  }
  existingSprite = SpriteAssetRegistry_FindById(loadedSprite->registryHeader.registryId);
  if (existingSprite == NULL) {
    definition->ownedNestedResourcePresent++;
    definition->ownedNestedResource = loadedSprite;
    spriteRegisterError = SpriteAsset_RegisterAndRelocatePointers(loadedSprite);
    if (spriteRegisterError != 0) {
      return spriteRegisterError;
    }
  }
  else {
    definition->ownedNestedResource = existingSprite;
    Resource_Release(loadedSprite);
  }
  return 0;
}

/* Replaces the effect definition id stored in *effectReference by the registered effect definition.
   On a failed lookup the field keeps the id and the lookup's error code is returned. */
static uint32_t ShotDefinition_ResolveEffectReference(EffectDefinition **effectReference)
{
  uint32_t effectLookupError;
  EffectDefinition *resolvedEffect;

  effectLookupError = EffectDefinitionRegistry_FindById
                    ((PckEffectDefinitionIdCatalog)*effectReference,&resolvedEffect);
  if (effectLookupError != 0) {
    return effectLookupError;
  }
  *effectReference = resolvedEffect;
  return 0;
}

/* Resolves the launch, secondary and primary effects and the 31 terrain-impact and 8 target-class-impact
   effects of a shot definition (the effect pointer fields hold effect definition ids until then), stopping at
   the first failing lookup. Returns 0 or that lookup's error code. */
static uint32_t ShotDefinition_ResolveEffectReferences(ShotDefinition *definition)
{
  uint32_t effectLookupError;
  int referenceIndex;

  effectLookupError = ShotDefinition_ResolveEffectReference(&definition->launchEffectDefinition);
  if (effectLookupError != 0) {
    return effectLookupError;
  }
  effectLookupError = ShotDefinition_ResolveEffectReference(&definition->secondaryEffectDefinition);
  if (effectLookupError != 0) {
    return effectLookupError;
  }
  effectLookupError = ShotDefinition_ResolveEffectReference(&definition->primaryEffectDefinition);
  if (effectLookupError != 0) {
    return effectLookupError;
  }
  for (referenceIndex = 0; referenceIndex < SHOT_TERRAIN_MATERIAL_REFERENCE_COUNT; referenceIndex++) {
    effectLookupError =
         ShotDefinition_ResolveEffectReference(&definition->terrainImpactEffectDefinitions31[referenceIndex]);
    if (effectLookupError != 0) {
      return effectLookupError;
    }
  }
  for (referenceIndex = 0; referenceIndex < SHOT_TARGET_CLASS_IMPACT_COUNT; referenceIndex++) {
    effectLookupError =
         ShotDefinition_ResolveEffectReference(&definition->targetClassImpactEffectDefinitions8[referenceIndex]);
    if (effectLookupError != 0) {
      return effectLookupError;
    }
  }
  return 0;
}

/* Registers one 0x2E0-byte shot definition in the first free registry slot, loads its sprite (switching the
   resource path to .spr; an already registered sprite with the same id is reused and the fresh load released)
   and replaces the effect ids of the launch, secondary and primary effects and of the 31 terrain-impact and 8
   target-class-impact effects by their registered definitions. Returns 0 on success, otherwise the error code
   of a duplicate id, a full registry or the first failing load or lookup. (The original's success return value,
   the last resolved effect definition, is still in targetClassImpactEffectDefinitions8[7].)
   Original quirk: a failing sprite load or effect lookup leaves the definition registered in its slot.
*/
uint32_t ShotDefinition_RegisterAndResolveReferences(ShotDefinition *definition)

{
  int slotsRemaining;
  ShotDefinition **registrySlotCursor;
  uint32_t spriteError;

  if (ShotRuntime_FindDefinitionById(definition->definitionId) != NULL) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    return FATAL_ERROR_SHOT_ID_DUPLICATE;
  }
  registrySlotCursor = g_ShotDefinitionRegistry;
  for (slotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (*registrySlotCursor == NULL) {
      *registrySlotCursor = definition;
      spriteError = ShotDefinition_LoadSprite(definition);
      if (spriteError != 0) {
        return spriteError;
      }
      return ShotDefinition_ResolveEffectReferences(definition);
    }
    registrySlotCursor++;
  }
  /* the registry capacity goes to the error text */
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,SHOT_DEFINITION_REGISTRY_SLOT_COUNT,
                          g_PackageLastErrorPath);
  return FATAL_ERROR_SHOT_REGISTRY_FULL;
}

