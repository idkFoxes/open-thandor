/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/shot/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/shot/catalog.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/shot/catalog. */

/* Address: 0x0052B4D0.
   Registers every shot definition of a loaded SHT asset: checks the 'sht' magic and converter version
   0x60006, then hands each 0x2E0-byte record after the 0x200-byte header to
   ShotDefinition_RegisterAndResolveReferences, stopping at the first failure. An invalid header leaves the
   asset path in g_PackageLastErrorPath and fails with FATAL_ERROR_SHOT_ASSET_INVALID.
*/
StatusResult ShotAsset_PrepareEntries(ShotAssetHeader *asset)

{
  uint32_t registrationStatusCode;
  AssetRecordCount entriesRemaining;
  ShotDefinition *definition;
  StatusResult registrationResult;
  StatusResult failureResult;

  registrationStatusCode = FATAL_ERROR_SHOT_ASSET_INVALID;
  if ((asset->entryCountHeader.common.magic == ASSET_MAGIC_SHT) &&
     (asset->entryCountHeader.common.converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    entriesRemaining = asset->entryCountHeader.entryCount;
    definition = (ShotDefinition *)(asset + 1);
    while( true ) {
      if (entriesRemaining == 0) {
        /* success hands back EAX as it was: the error code preset or the last registration result */
        registrationResult.failed = false;
        registrationResult.valueOrError = registrationStatusCode;
        return registrationResult;
      }
      registrationResult = ShotDefinition_RegisterAndResolveReferences(definition);
      registrationStatusCode = registrationResult.valueOrError;
      if (registrationResult.failed) break;
      definition++;
      entriesRemaining--;
    }
  }
  else {
    Package_SetLastErrorPath((uint16_t *)asset);
  }
  failureResult.failed = true;
  failureResult.valueOrError = registrationStatusCode;
  return failureResult;
}


/* Address: 0x0052B7E0.
   Runs after the level's terrain materials are loaded: checks that each of the 31 terrain-material indices
   of every registered shot definition is negative (no material) or names a loaded material. Otherwise the
   registry index of the offending shot is written to g_PackageLastErrorPath and the check fails with
   FATAL_ERROR_SHOT_TERRAIN_MATERIAL_INVALID.
*/
StatusResult ShotDefinitions_ValidateTerrainMaterialReferences(void)

{
  /* EAX: success returns the last index checked (the caller's EAX when the registry is empty); callers
     test CF only. */
  TerrainMaterialIndex materialIndex = 0;
  int registrySlotsRemaining;
  int materialIndicesRemaining;
  ShotDefinition *definition;
  TerrainMaterialIndex *materialIndexCursor;
  ShotDefinition **registryCursor;
  StatusResult successResult;
  StatusResult failureResult;

  registryCursor = g_ShotDefinitionRegistry;
  registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT;
  do {
    definition = *registryCursor;
    if (definition != NULL) {
      materialIndexCursor = definition->terrainMaterialIndices31;
      materialIndicesRemaining = SHOT_TERRAIN_MATERIAL_REFERENCE_COUNT;
      do {
        materialIndex = *materialIndexCursor;
        materialIndexCursor++;
        if ((TERRAIN_MATERIAL_COUNT - 1 < materialIndex) ||
           ((-1 < materialIndex && (g_TerrainMaterialTextureSets[materialIndex] == NULL)))) {
          g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                     SHOT_DEFINITION_REGISTRY_SLOT_COUNT - registrySlotsRemaining,g_PackageLastErrorPath);
          failureResult.failed = true;
          failureResult.valueOrError = FATAL_ERROR_SHOT_TERRAIN_MATERIAL_INVALID;
          return failureResult;
        }
        materialIndicesRemaining--;
      } while (materialIndicesRemaining != 0);
    }
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      successResult.failed = false;
      successResult.valueOrError = (uint32_t)materialIndex;
      return successResult;
    }
  } while( true );
}


/* Address: 0x0052B860.
   Looks up a registered shot definition by id. On a miss the id is written as decimal text to
   g_PackageLastErrorPath for the fatal-error message and FATAL_ERROR_SHOT_ID_NOT_FOUND is returned with CF set.
*/
ShotDefinitionResult ShotDefinitionRegistry_FindByIdWithError(PckShotDefinitionIdCatalog definitionId)

{
  ShotDefinition *registeredDefinition;
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;
  ShotDefinitionResult notFoundResult;
  ShotDefinitionResult foundResult;
  
  registryCursor = g_ShotDefinitionRegistry;
  registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT;
  while ((registeredDefinition = *registryCursor, registeredDefinition == NULL ||
         (registeredDefinition->definitionId != definitionId))) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
      notFoundResult.notFound = true;
      notFoundResult.definitionOrError = (ShotDefinition *)FATAL_ERROR_SHOT_ID_NOT_FOUND;
      return notFoundResult;
    }
  }
  foundResult.notFound = false;
  foundResult.definitionOrError = registeredDefinition;
  return foundResult;
}


/* Address: 0x0052B8C0.
   Computes the heading and elevation (Angle16) at which a shot of this definition must leave launchPoint to reach
   targetPoint. Ballistic shots solve the projectile equation (gravity = ballisticDivisorQ12) and take the high arc,
   the low arc only when the height difference is below 1.0; fixed-range shots always go straight up (0, 0x4000);
   all others aim directly, raised by the definition's elevation offset and capped at straight up. The callers
   pass the points in Z, Y, X order (Z = height). Called directly by the weapon aiming code (gameplay/army/combat.c,
   movement.c, runtime.c) and the shot launch in world/shots/runtime.c.
*/
ShotLaunchAnglesEaxEdx8 ShotDefinition_ComputeLaunchAnglesRegs
          (Q12 targetZ,Q12 targetY,Q12 targetX,Q12 launchZ,Q12 launchY,Q12 launchX,
          ShotDefinition *definition)

{
  int64_t discriminant;
  int rangeTimesDivisor;
  int launchSpeedSquared;
  int heightDelta;
  uint32_t computedElevation;
  uint32_t computedHeading;
  ShotLaunchAnglesEaxEdx8 clampedAngles;
  ShotLaunchAnglesEaxEdx8 fixedRangeAngles;
  FixedLengthAngleEaxEdx8 planarLengthAngle;
  ShotLaunchAnglesEaxEdx8 launchAngles;
  FixedVectorAngles vectorAngles;

  heightDelta = targetZ - launchZ;
  if (definition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    planarLengthAngle = FixedMath_Vector2AngleAndLengthRegs(targetY - launchY,targetX - launchX);
    computedHeading = planarLengthAngle.angle & 0xffff;
    /* tan(elevation) = (v^2 +- sqrt(v^4 - 2*g*h*v^2 - (g*r)^2)) / (g*r) */
    launchSpeedSquared = definition->launchSpeedQ12 * definition->launchSpeedQ12;
    rangeTimesDivisor = planarLengthAngle.length * definition->ballisticDivisorQ12;
    discriminant = (int64_t)(launchSpeedSquared + definition->ballisticDivisorQ12 * heightDelta * -2) * (int64_t)launchSpeedSquared -
            (int64_t)rangeTimesDivisor * (int64_t)rangeTimesDivisor;
    if (discriminant < 0) {
      discriminant = 0; /* target out of reach */
    }
    computedElevation = FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)discriminant >> 0x20),(UInt64Half32)discriminant);
    if ((-0x1000 < heightDelta) && (heightDelta < 0x1000)) {
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
    vectorAngles = FixedMath_VectorToAngles3Regs(heightDelta,targetY - launchY,targetX - launchX);
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


/* Address: 0x0052BCE0.
   Returns how far a shot of this definition reaches, used when a model's weapons are summed up for target
   selection: ballistic shots 9/8 of speed^2 / divisor, fixed-range shots their stored range (+0x27C), all
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
    /* -0xAAA / 0x1000 = -2/3 in Q12: lifetime - 2/3 of the ramp duration */
    selectionRangeQ12 =
         (((int)(definition->trajectoryRampDurationTicks * -0xaaa) >> 0xc) +
         definition->projectileLifetimeTicks) * definition->launchSpeedQ12;
  }
  return selectionRangeQ12;
}

/* Address: 0x0052BD50.
   Returns in EBX the shot speed used to lead a moving target: the launch speed (+0x0C) for unguided shots that
   do not fly a direct line, INT32_MAX (no lead) for direct-line or guided (+0x290 non-zero) shots. Called
   directly by the target aim-point computation in gameplay/army/runtime.c.
   Original register convention: result in EBX, CF set on failure; EAX, ECX and EDX preserved.
*/
ShotRangeLimitResult ShotDefinition_GetModeRangeLimitEbx(ShotDefinition *definition)

{
  uint32_t rangeLimit;
  ShotRangeLimitResult limitResult;

  rangeLimit = INT32_MAX;
  if ((definition->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) &&
     (definition->guidanceTurnLimitAngle16 == 0)) {
    rangeLimit = definition->launchSpeedQ12;
  }
  limitResult.failed = false;
  limitResult.rangeLimitQ12 = rangeLimit;
  return limitResult;
}


/* Address: 0x0052BD80.
   Returns the extra lead time for unguided lead-adjusted shots (trajectory mode 3, +0x290 zero): about two
   thirds (0xAB / 256) of the ramp-up ticks (+0x270), during which the shot is still accelerating; 0 for all
   other shots. The aim-point computation in gameplay/army/runtime.c multiplies it by the target's speed.
*/
uint32_t ShotDefinition_ComputeMode3LeadAdjustment(ShotDefinition *definition)

{
  uint32_t leadAdjustmentQ12;
  
  leadAdjustmentQ12 = 0;
  if ((definition->trajectoryMode == SHOT_TRAJECTORY_LEAD_ADJUSTED) &&
     (definition->guidanceTurnLimitAngle16 == 0)) {
    leadAdjustmentQ12 = definition->trajectoryRampDurationTicks * 0xab >> 8;
  }
  return leadAdjustmentQ12;
}


/* Address: 0x0052B350.
   Registers one 0x2E0-byte shot definition in the first free registry slot, loads its sprite (switching the
   resource path to .spr; an already registered sprite with the same id is reused and the fresh load released)
   and replaces the effect ids of the launch, secondary and primary effects and of the 31 terrain-impact and 8
   target-class-impact effects by their registered definitions. CF/EAX report a duplicate id, a full registry
   or the first failing load or lookup.
*/
StatusResult ShotDefinition_RegisterAndResolveReferences(ShotDefinition *definition)

{
  ShotDefinition *valueOrError;
  SpriteAssetHeader *existingSprite;
  int slotsRemainingOrIndex;
  int referencesRemaining;
  ShotDefinition **registrySlotCursor;
  bool extensionFailed;
  ShotDefinitionResult existingLookup;
  StatusResult failureResult;
  PackageLoadResult loadResult;
  SpriteRegisterResult spriteRegisterResult;
  EffectDefinitionResult effectLookup;
  StatusResult successResult;

  registrySlotCursor = g_ShotDefinitionRegistry;
  slotsRemainingOrIndex = SHOT_DEFINITION_REGISTRY_SLOT_COUNT;
  existingLookup = ShotRuntime_FindDefinitionById(definition->definitionId);
  valueOrError = existingLookup.definitionOrError;
  if (existingLookup.notFound) {
    do {
      if (*registrySlotCursor == NULL) {
        *registrySlotCursor = definition;
        extensionFailed = WidePath_SetExtensionCode(ASSET_MAGIC_SPR,definition->resourcePathUtf16);
        if (extensionFailed) goto ShotDefinition_ReturnReferenceResolutionResult;
        loadResult = Package_LoadEntry(definition->resourcePathUtf16);
        valueOrError = loadResult.bufferOrError;
        if (loadResult.failed) goto ShotDefinition_ReturnReferenceResolutionResult;
        /* the loaded file is a sprite asset; this reads its registry id at +0xB8 */
        existingSprite = SpriteAssetRegistry_FindById(valueOrError->targetClassImpactDamageQ12[2]);
        if (existingSprite == NULL) {
          definition->ownedNestedResourcePresent++;
          definition->ownedNestedResource = valueOrError;
          spriteRegisterResult = SpriteAsset_RegisterAndRelocatePointers((SpriteAssetHeader *)valueOrError);
          valueOrError = (ShotDefinition *)spriteRegisterResult.assetOrError;
          if (spriteRegisterResult.failed) goto ShotDefinition_ReturnReferenceResolutionResult;
        }
        else {
          definition->ownedNestedResource = existingSprite;
          Resource_Release(valueOrError);
        }
        /* the effect pointer fields hold effect definition ids until they are resolved here */
        effectLookup = EffectDefinitionRegistry_FindByIdWithError
                          ((PckEffectDefinitionIdCatalog)definition->launchEffectDefinition);
        valueOrError = (ShotDefinition *)effectLookup.definitionOrError;
        if (effectLookup.notFound) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->launchEffectDefinition = (EffectDefinition *)valueOrError;
        effectLookup = EffectDefinitionRegistry_FindByIdWithError
                          ((PckEffectDefinitionIdCatalog)definition->secondaryEffectDefinition);
        valueOrError = (ShotDefinition *)effectLookup.definitionOrError;
        if (effectLookup.notFound) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->secondaryEffectDefinition = (EffectDefinition *)valueOrError;
        effectLookup = EffectDefinitionRegistry_FindByIdWithError
                          ((PckEffectDefinitionIdCatalog)definition->primaryEffectDefinition);
        valueOrError = (ShotDefinition *)effectLookup.definitionOrError;
        if (effectLookup.notFound) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->primaryEffectDefinition = (EffectDefinition *)valueOrError;
        slotsRemainingOrIndex = 0;
        referencesRemaining = SHOT_TERRAIN_MATERIAL_REFERENCE_COUNT;
        do {
          effectLookup = EffectDefinitionRegistry_FindByIdWithError
                            ((PckEffectDefinitionIdCatalog)
                             definition->terrainImpactEffectDefinitions31[slotsRemainingOrIndex]);
          valueOrError = (ShotDefinition *)effectLookup.definitionOrError;
          if (effectLookup.notFound) goto ShotDefinition_ReturnReferenceResolutionResult;
          definition->terrainImpactEffectDefinitions31[slotsRemainingOrIndex] =
               (EffectDefinition *)valueOrError;
          slotsRemainingOrIndex++;
          referencesRemaining--;
        } while (referencesRemaining != 0);
        slotsRemainingOrIndex = 0;
        referencesRemaining = 8;
        do {
          effectLookup = EffectDefinitionRegistry_FindByIdWithError
                            ((PckEffectDefinitionIdCatalog)
                             definition->targetClassImpactEffectDefinitions8[slotsRemainingOrIndex]);
          valueOrError = (ShotDefinition *)effectLookup.definitionOrError;
          if (effectLookup.notFound) goto ShotDefinition_ReturnReferenceResolutionResult;
          definition->targetClassImpactEffectDefinitions8[slotsRemainingOrIndex] =
               (EffectDefinition *)valueOrError;
          slotsRemainingOrIndex++;
          referencesRemaining--;
        } while (referencesRemaining != 0);
        successResult.failed = false;
        successResult.valueOrError = (uint32_t)valueOrError;
        return successResult;
      }
      registrySlotCursor++;
      slotsRemainingOrIndex--;
    } while (slotsRemainingOrIndex != 0);
    /* the registry capacity goes to the error text */
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,SHOT_DEFINITION_REGISTRY_SLOT_COUNT,
                            g_PackageLastErrorPath);
    valueOrError = (ShotDefinition *)FATAL_ERROR_SHOT_REGISTRY_FULL;
  }
  else {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    valueOrError = (ShotDefinition *)FATAL_ERROR_SHOT_ID_DUPLICATE;
  }
ShotDefinition_ReturnReferenceResolutionResult:
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)valueOrError;
  return failureResult;
}

