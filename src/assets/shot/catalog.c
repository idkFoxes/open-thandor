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
   Ownership: assets/shot/catalog.
   Purpose: Validates the 'sht' magic and converter version 0x00060006, then prepares entryCount fixed 0x2E0-byte
   entries beginning at +0x200. Preparation stops on the first CF-set entry failure. Invalid headers are copied to
   the package last-error path. Payload fields remain opaque. Role: Walks the SHT asset table and registers every
   shot definition.
   Local calls: ShotDefinition_RegisterAndResolveReferencesCf.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime].
*/
StatusResult __thandor_void_preserve_ecx_edx ShotAsset_PrepareEntries(ShotAssetHeader *asset)

{
  uint32_t registrationStatusCode;
  AssetRecordCount entriesRemaining;
  ShotDefinition *definition;
  ShotDefinition *definitionCursor;
  StatusResult registrationResult;
  StatusResult failureResult;
  
  registrationStatusCode = 0x43;
  if (((asset->entryCountHeader).common.magic == ASSET_MAGIC_SHT) &&
     ((asset->entryCountHeader).common.converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    entriesRemaining = (asset->entryCountHeader).entryCount;
    definition = (ShotDefinition *)(asset + 1);
    while( true ) {
      if (entriesRemaining == 0) {
        registrationResult.failed = false;
        registrationResult.valueOrError = registrationStatusCode;
        return registrationResult;
      }
      registrationResult = ShotDefinition_RegisterAndResolveReferencesCf(definition);
      registrationStatusCode = registrationResult.valueOrError;
      if (registrationResult.failed) break;
      definition = definition + 1;
      entriesRemaining = entriesRemaining - 1;
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
   Ownership: assets/shot/catalog.
   Purpose: Validates the 31 terrain-material indices embedded in every registered shot definition; invalid or
   unloaded material references return error 0x46.
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
  registrySlotsRemaining = 0x100;
  do {
    definition = *registryCursor;
    if (definition != (ShotDefinition *)0x0) {
      materialIndexCursor = definition->terrainMaterialIndices31;
      materialIndicesRemaining = 0x1f;
      do {
        materialIndex = *materialIndexCursor;
        materialIndexCursor = materialIndexCursor + 1;
        if ((0x19 < materialIndex) ||
           ((-1 < materialIndex && (g_TerrainMaterialTextureSets[materialIndex] == (GraphicsTextureSet *)0x0)
            ))) {
          (*g_WideNumberFormatUtf16)
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x100 - registrySlotsRemaining,g_PackageLastErrorPath);
          failureResult.failed = true;
          failureResult.valueOrError = 0x46;
          return failureResult;
        }
        materialIndicesRemaining = materialIndicesRemaining + -1;
      } while (materialIndicesRemaining != 0);
    }
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      successResult.failed = false;
      successResult.valueOrError = (uint32_t)materialIndex;
      return successResult;
    }
  } while( true );
}


/* Address: 0x0052B860.
   Ownership: assets/shot/catalog.
   Purpose: Scans the 256-slot shot-definition registry. On a miss it formats the unresolved identifier into
   g_PackageLastErrorPath and returns error 0x44 with CF set.
*/
ShotDefinitionResult __thandor_eax_cf_preserve_ecx_edx
ShotDefinitionRegistry_FindByIdWithErrorCf(PckShotDefinitionIdCatalog definitionId)

{
  ShotDefinition *registeredDefinition;
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;
  ShotDefinitionResult notFoundResult;
  ShotDefinitionResult foundResult;
  ShotDefinition *candidateDefinition;
  
  registryCursor = g_ShotDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  while ((registeredDefinition = *registryCursor, registeredDefinition == (ShotDefinition *)0x0 ||
         (registeredDefinition->definitionId != definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
      notFoundResult.notFound = true;
      notFoundResult.definitionOrError = (ShotDefinition *)0x44;
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
   Ownership: assets/shot/catalog.
   Purpose: Registers one fixed 0x2E0-byte shot definition in the 256-slot registry, loads or reuses its .spr
   asset, resolves three primary effect identifiers plus the exact 31-entry and 8-entry effect-reference arrays,
   and preserves the original CF/EAX failure contract. Role: Registers a 0x2E0 SHT record, loads its visual SPR and
   resolves all effect references. Inputs: Serialized ShotDefinition: trajectory mode, launch speed, effect tables,
   periodic fields and resource path. Outputs: ShotDefinition with SpriteAsset, primary/launch/secondary and
   material-impact EffectDefinition pointers. Edges: Package_LoadEntry -> SpriteAsset registration -> effect
   registry lookups.
   Cross-module calls: ShotRuntime_FindDefinitionByIdCf [world/shots/runtime], WidePath_SetExtensionCode
   [core/text/path], Package_LoadEntry [assets/package/runtime], SpriteAssetRegistry_FindById
   [assets/sprite/catalog], SpriteAsset_RegisterAndRelocatePointers [assets/sprite/catalog], Resource_Release
   [assets/resource/runtime].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
ShotDefinition_RegisterAndResolveReferencesCf(ShotDefinition *definition)

{
  ShotDefinition *asset;
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
  slotsRemainingOrIndex = 0x100;
  existingLookup = ShotRuntime_FindDefinitionByIdCf(definition->definitionId);
  asset = existingLookup.definitionOrError;
  if (existingLookup.notFound) {
    do {
      if (*registrySlotCursor == (ShotDefinition *)0x0) {
        *registrySlotCursor = definition;
        extensionFailed = WidePath_SetExtensionCode(0x727073,definition->resourcePathUtf16);
        if (extensionFailed) goto ShotDefinition_ReturnReferenceResolutionResult;
        loadResult = Package_LoadEntry(definition->resourcePathUtf16);
        asset = loadResult.bufferOrError;
        if (loadResult.failed) goto ShotDefinition_ReturnReferenceResolutionResult;
        existingSprite = SpriteAssetRegistry_FindById(asset->targetClassImpactDamageQ12[2]);
        if (existingSprite == (SpriteAssetHeader *)0x0) {
          definition->ownedNestedResourcePresent = definition->ownedNestedResourcePresent + 1;
          definition->ownedNestedResource = asset;
          spriteRegisterResult = SpriteAsset_RegisterAndRelocatePointers((SpriteAssetHeader *)asset);
          asset = (ShotDefinition *)spriteRegisterResult.assetOrError;
          if (spriteRegisterResult.failed) goto ShotDefinition_ReturnReferenceResolutionResult;
        }
        else {
          definition->ownedNestedResource = existingSprite;
          Resource_Release(asset);
        }
        effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                          ((PckEffectDefinitionIdCatalog)definition->launchEffectDefinition);
        asset = (ShotDefinition *)effectLookup.definitionOrError;
        if (effectLookup.notFound) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->launchEffectDefinition = (EffectDefinition *)asset;
        effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                          ((PckEffectDefinitionIdCatalog)definition->secondaryEffectDefinition);
        asset = (ShotDefinition *)effectLookup.definitionOrError;
        if (effectLookup.notFound) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->secondaryEffectDefinition = (EffectDefinition *)asset;
        effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                          ((PckEffectDefinitionIdCatalog)definition->primaryEffectDefinition);
        asset = (ShotDefinition *)effectLookup.definitionOrError;
        if (effectLookup.notFound) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->primaryEffectDefinition = (EffectDefinition *)asset;
        slotsRemainingOrIndex = 0;
        referencesRemaining = 0x1f;
        do {
          effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                            ((PckEffectDefinitionIdCatalog)
                             definition->terrainImpactEffectDefinitions31[slotsRemainingOrIndex]);
          asset = (ShotDefinition *)effectLookup.definitionOrError;
          if (effectLookup.notFound) goto ShotDefinition_ReturnReferenceResolutionResult;
          definition->terrainImpactEffectDefinitions31[slotsRemainingOrIndex] = (EffectDefinition *)asset;
          slotsRemainingOrIndex = slotsRemainingOrIndex + 1;
          referencesRemaining = referencesRemaining + -1;
        } while (referencesRemaining != 0);
        slotsRemainingOrIndex = 0;
        referencesRemaining = 8;
        do {
          effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                            ((PckEffectDefinitionIdCatalog)
                             definition->targetClassImpactEffectDefinitions8[slotsRemainingOrIndex]);
          asset = (ShotDefinition *)effectLookup.definitionOrError;
          if (effectLookup.notFound) goto ShotDefinition_ReturnReferenceResolutionResult;
          definition->targetClassImpactEffectDefinitions8[slotsRemainingOrIndex] = (EffectDefinition *)asset;
          slotsRemainingOrIndex = slotsRemainingOrIndex + 1;
          referencesRemaining = referencesRemaining + -1;
        } while (referencesRemaining != 0);
        successResult.failed = false;
        successResult.valueOrError = (uint32_t)asset;
        return successResult;
      }
      registrySlotCursor = registrySlotCursor + 1;
      slotsRemainingOrIndex = slotsRemainingOrIndex + -1;
    } while (slotsRemainingOrIndex != 0);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x100,g_PackageLastErrorPath);
    asset = (ShotDefinition *)0x45;
  }
  else {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    asset = (ShotDefinition *)0x4d;
  }
ShotDefinition_ReturnReferenceResolutionResult:
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)asset;
  return failureResult;
}

