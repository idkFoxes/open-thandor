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
StatusResult __thandor_void_preserve_ecx_edx ShotAsset_PrepareEntries(ShotAssetHeader *asset)

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
StatusResult __thandor_eax_cf_preserve_ecx_edx
ShotDefinitions_ValidateTerrainMaterialReferences(void)

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
ShotDefinitionResult __thandor_eax_cf_preserve_ecx_edx
ShotDefinitionRegistry_FindByIdWithError(PckShotDefinitionIdCatalog definitionId)

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
   Ownership: assets/shot/catalog.
   Purpose: Computes the launch azimuth and elevation pair for two 3D points using the shot definition's trajectory
   mode. Mode 1 uses the verified ballistic discriminant path, mode 2 returns the fixed pair (0,0x4000), and other
   modes use direct vector angles with the stored elevation offset and 0x4000 clamp. Role: Computes
   azimuth/elevation launch angles according to ShotDefinition trajectory mode. Inputs: Source/target coordinates,
   launch speed, ballistic divisor, elevation/range fields. Outputs: Register-pair launch angles.
   Cross-module calls: FixedMath_Vector2AngleAndLengthRegs [core/math/fixed], FixedMath_UInt64Sqrt
   [core/math/fixed], FixedMath_Atan2Angle16 [core/math/fixed], FixedMath_VectorToAngles3Regs [core/math/fixed].
*/
ShotLaunchAnglesEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
ShotDefinition_ComputeLaunchAnglesRegs
          (Q12 point0X,Q12 point0Y,Q12 point0Z,Q12 point1X,Q12 point1Y,Q12 point1Z,
          ShotDefinition *definition)

{
  int64_t discriminant;
  int rangeTimesDivisor;
  int launchSpeedSquared;
  int deltaX;
  uint32_t computedElevation;
  uint32_t computedHeading;
  ShotLaunchAnglesEaxEdx8 clampedAngles;
  ShotLaunchAnglesEaxEdx8 fixedRangeAngles;
  FixedLengthAngleEaxEdx8 planarLengthAngle;
  ShotLaunchAnglesEaxEdx8 launchAngles;
  FixedVectorAngles vectorAngles;
  
  deltaX = point0X - point1X;
  if (definition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    planarLengthAngle = FixedMath_Vector2AngleAndLengthRegs(point0Y - point1Y,point0Z - point1Z);
    computedHeading = planarLengthAngle.angle & 0xffff;
    launchSpeedSquared = definition->launchSpeedQ12 * definition->launchSpeedQ12;
    rangeTimesDivisor = planarLengthAngle.length * definition->ballisticDivisorQ12;
    discriminant = (int64_t)(launchSpeedSquared + definition->ballisticDivisorQ12 * deltaX * -2) * (int64_t)launchSpeedSquared -
            (int64_t)rangeTimesDivisor * (int64_t)rangeTimesDivisor;
    if (discriminant < 0) {
      discriminant = 0;
    }
    computedElevation = FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)discriminant >> 0x20),(UInt64Half32)discriminant);
    if ((-0x1000 < deltaX) && (deltaX < 0x1000)) {
      computedElevation = -computedElevation;
    }
    computedElevation = FixedMath_Atan2Angle16(launchSpeedSquared + computedElevation,rangeTimesDivisor);
  }
  else {
    if (definition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
      fixedRangeAngles.headingAngle = 0;
      fixedRangeAngles.elevationAngle = 0x4000;
      return fixedRangeAngles;
    }
    vectorAngles = FixedMath_VectorToAngles3Regs(deltaX,point0Y - point1Y,point0Z - point1Z);
    computedHeading = vectorAngles.azimuthAngle;
    computedElevation = vectorAngles.elevationAngle + definition->elevationOffsetAngle16;
    if (0x4000 < (int)computedElevation) {
      clampedAngles.elevationAngle = 0x4000;
      clampedAngles.headingAngle = computedHeading;
      return clampedAngles;
    }
  }
  launchAngles.elevationAngle = computedElevation;
  launchAngles.headingAngle = computedHeading;
  return launchAngles;
}


/* Address: 0x0052BCE0.
   Ownership: assets/shot/catalog.
   Purpose: Returns the exact trajectory-mode-dependent range contribution used while accumulating selection
   information. Mode 1 derives it from speed squared and the ballistic divisor, mode 2 returns the stored +0x27C
   value, and other modes combine +0x270, +0xD0, and +0x0C. The stock corpus contains 170 records and 136 unique
   ids across five shot banks; duplicate ids are aliases/variants, not permission to invent distinct gameplay
   meanings.
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
    selectionRangeQ12 =
         (((int)(definition->trajectoryRampDurationTicks * -0xaaa) >> 0xc) +
         definition->projectileLifetimeTicks) * definition->launchSpeedQ12;
  }
  return selectionRangeQ12;
}

/* Address: 0x0052BD50.
   Ownership: assets/shot/catalog.
   Purpose: Custom-ABI ShotDefinition probe. EBX defaults to 0x7FFFFFFF; when trajectory mode at +0x00 is nonzero
   and flag +0x290 is zero, EBX receives dword +0x0C. EDX is preserved and the EBX result cannot be represented by
   an ordinary C return type.
*/
ShotRangeLimitResult __thandor_ebx_cf_preserve_eax_ecx_edx
ShotDefinition_GetModeRangeLimitEbx(ShotDefinition *definition)

{
  uint32_t rangeLimit;
  ShotRangeLimitResult limitResult;
  
  rangeLimit = 0x7fffffff;
  if ((definition->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) &&
     (definition->guidanceTurnLimitAngle16 == 0)) {
    rangeLimit = definition->launchSpeedQ12;
  }
  limitResult.failed = false;
  limitResult.rangeLimitQ12 = rangeLimit;
  return limitResult;
}


/* Address: 0x0052BD80.
   Ownership: assets/shot/catalog.
   Purpose: Returns zero except for trajectory mode 3 with the verified +0x290 flag clear; that path returns the
   unsigned scaled +0x270 value multiplied by 0xAB and shifted right eight.
*/
uint32_t __thandor_eax_preserve_ecx_edx
ShotDefinition_ComputeMode3LeadAdjustment(ShotDefinition *definition)

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
StatusResult __thandor_eax_cf_preserve_ecx_edx
ShotDefinition_RegisterAndResolveReferences(ShotDefinition *definition)

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

