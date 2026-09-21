/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/shot/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/shot/catalog.h>

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
StatusValueEaxCf5 __thandor_void_preserve_ecx_edx ShotAsset_PrepareEntries(ShotAssetHeader *asset)

{
  dword registrationStatusCode;
  AssetRecordCount AVar1;
  ShotDefinition *definition;
  ShotDefinition *definitionCursor;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  
  registrationStatusCode = 0x43;
  if (((asset->entryCountHeader).common.magic == ASSET_MAGIC_SHT) &&
     ((asset->entryCountHeader).common.converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    AVar1 = (asset->entryCountHeader).entryCount;
    definition = (ShotDefinition *)(asset + 1);
    while( true ) {
      if (AVar1 == 0) {
        SVar2.carry = false;
        SVar2.valueOrError = registrationStatusCode;
        return SVar2;
      }
      SVar2 = ShotDefinition_RegisterAndResolveReferencesCf(definition);
      registrationStatusCode = SVar2.valueOrError;
      if (SVar2.carry) break;
      definition = definition + 1;
      AVar1 = AVar1 - 1;
    }
  }
  else {
    Package_SetLastErrorPath((word *)asset);
  }
  SVar3.carry = true;
  SVar3.valueOrError = registrationStatusCode;
  return SVar3;
}


/* Address: 0x0052B7E0.
   Ownership: assets/shot/catalog.
   Purpose: Validates the 31 terrain-material indices embedded in every registered shot definition; invalid or
   unloaded material references return error 0x46.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ShotDefinitions_ValidateTerrainMaterialReferences(void)

{
  uint in_EAX;
  int iVar1;
  int iVar2;
  ShotDefinition *shotDefinition1;
  ShotDefinition **ppSVar3;
  StatusValueEaxCf5 SVar4;
  StatusValueEaxCf5 SVar5;
  
  ppSVar3 = g_ShotDefinitionRegistry;
  iVar1 = 0x100;
  do {
    shotDefinition1 = *ppSVar3;
    if (shotDefinition1 != (ShotDefinition *)0x0) {
      iVar2 = 0x1f;
      do {
        in_EAX = shotDefinition1->terrainMaterialIndices31[0];
        shotDefinition1 = (ShotDefinition *)&shotDefinition1->reservedDword04;
        if ((0x19 < (int)in_EAX) ||
           ((-1 < (int)in_EAX && (g_TerrainMaterialTextureSets[in_EAX] == (GraphicsTextureSet *)0x0)
            ))) {
          (*g_WideNumberFormatUtf16)
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x100 - iVar1,g_PackageLastErrorPath);
          SVar5.carry = true;
          SVar5.valueOrError = 0x46;
          return SVar5;
        }
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
    }
    ppSVar3 = ppSVar3 + 1;
    iVar1 = iVar1 + -1;
    if (iVar1 == 0) {
      SVar4.carry = false;
      SVar4.valueOrError = in_EAX;
      return SVar4;
    }
  } while( true );
}


/* Address: 0x0052B860.
   Ownership: assets/shot/catalog.
   Purpose: Scans the 256-slot shot-definition registry. On a miss it formats the unresolved identifier into
   g_PackageLastErrorPath and returns error 0x44 with CF set.
*/
ShotDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ShotDefinitionRegistry_FindByIdWithErrorCf(PckShotDefinitionIdCatalog definitionId)

{
  ShotDefinition *pSVar1;
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;
  ShotDefinitionLookupEaxCf5 SVar2;
  ShotDefinitionLookupEaxCf5 SVar3;
  ShotDefinition *candidateDefinition;
  
  registryCursor = g_ShotDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  while ((pSVar1 = *registryCursor, pSVar1 == (ShotDefinition *)0x0 ||
         (pSVar1->definitionId != definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
      SVar2.carry = true;
      SVar2.definitionOrError = (ShotDefinition *)0x44;
      return SVar2;
    }
  }
  SVar3.carry = false;
  SVar3.definitionOrError = pSVar1;
  return SVar3;
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
  longlong lVar1;
  int x;
  int iVar2;
  int x_00;
  dword dVar3;
  uint uVar4;
  ShotLaunchAnglesEaxEdx8 SVar5;
  ShotLaunchAnglesEaxEdx8 SVar6;
  FixedLengthAngleEaxEdx8 FVar7;
  ShotLaunchAnglesEaxEdx8 SVar8;
  FixedMathVectorAnglesRegs8 FVar9;
  
  x_00 = point0X - point1X;
  if (definition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    FVar7 = FixedMath_Vector2AngleAndLengthRegs(point0Y - point1Y,point0Z - point1Z);
    uVar4 = FVar7.angle & 0xffff;
    iVar2 = definition->launchSpeedQ12 * definition->launchSpeedQ12;
    x = FVar7.length * definition->ballisticDivisorQ12;
    lVar1 = (longlong)(iVar2 + definition->ballisticDivisorQ12 * x_00 * -2) * (longlong)iVar2 -
            (longlong)x * (longlong)x;
    if (lVar1 < 0) {
      lVar1 = 0;
    }
    dVar3 = FixedMath_UInt64Sqrt((UInt64Half32)((ulonglong)lVar1 >> 0x20),(UInt64Half32)lVar1);
    if ((-0x1000 < x_00) && (x_00 < 0x1000)) {
      dVar3 = -dVar3;
    }
    dVar3 = FixedMath_Atan2Angle16(iVar2 + dVar3,x);
  }
  else {
    if (definition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
      SVar6.headingAngle = 0;
      SVar6.elevationAngle = 0x4000;
      return SVar6;
    }
    FVar9 = FixedMath_VectorToAngles3Regs(x_00,point0Y - point1Y,point0Z - point1Z);
    uVar4 = FVar9.ecx;
    dVar3 = FVar9.edx + definition->elevationOffsetAngle16;
    if (0x4000 < (int)dVar3) {
      SVar5.elevationAngle = 0x4000;
      SVar5.headingAngle = uVar4;
      return SVar5;
    }
  }
  SVar8.elevationAngle = dVar3;
  SVar8.headingAngle = uVar4;
  return SVar8;
}


/* Address: 0x0052BCE0.
   Ownership: assets/shot/catalog.
   Purpose: Returns the exact trajectory-mode-dependent range contribution used while accumulating selection
   information. Mode 1 derives it from speed squared and the ballistic divisor, mode 2 returns the stored +0x27C
   value, and other modes combine +0x270, +0xD0, and +0x0C. The stock corpus contains 170 records and 136 unique
   ids across five shot banks; duplicate ids are aliases/variants, not permission to invent distinct gameplay
   meanings.
*/
dword ShotDefinition_ComputeSelectionRange(ShotDefinition *definition)

{
  uint selectionRangeQ12;
  
  if (definition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    selectionRangeQ12 =
         (uint)((int)(((longlong)definition->launchSpeedQ12 * (longlong)definition->launchSpeedQ12)
                     / (longlong)definition->ballisticDivisorQ12) * 9) >> 3;
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
ShotModeRangeLimitEbxCf5 __thandor_ebx_cf_preserve_eax_ecx_edx
ShotDefinition_GetModeRangeLimitEbx(ShotDefinition *definition)

{
  uint uVar1;
  ShotModeRangeLimitEbxCf5 SVar2;
  
  uVar1 = 0x7fffffff;
  if ((definition->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) &&
     (definition->guidanceTurnLimitAngle16 == 0)) {
    uVar1 = definition->launchSpeedQ12;
  }
  SVar2.carry = false;
  SVar2.rangeLimitQ12 = uVar1;
  return SVar2;
}


/* Address: 0x0052BD80.
   Ownership: assets/shot/catalog.
   Purpose: Returns zero except for trajectory mode 3 with the verified +0x290 flag clear; that path returns the
   unsigned scaled +0x270 value multiplied by 0xAB and shifted right eight.
*/
dword __thandor_eax_preserve_ecx_edx
ShotDefinition_ComputeMode3LeadAdjustment(ShotDefinition *definition)

{
  uint leadAdjustmentQ12;
  
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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ShotDefinition_RegisterAndResolveReferencesCf(ShotDefinition *definition)

{
  ShotDefinition *asset;
  SpriteAssetHeader *pSVar1;
  int iVar2;
  int iVar3;
  ShotDefinition **registrySlotCursor;
  bool bVar4;
  ShotDefinitionLookupEaxCf5 SVar5;
  StatusValueEaxCf5 SVar6;
  PackageLoadEntryEaxCf5 PVar7;
  SpriteRegisterRelocateEaxCf5 SVar8;
  EffectDefinitionLookupEaxCf5 EVar9;
  
  registrySlotCursor = g_ShotDefinitionRegistry;
  iVar2 = 0x100;
  SVar5 = ShotRuntime_FindDefinitionByIdCf(definition->definitionId);
  asset = SVar5.definitionOrError;
  if (SVar5.carry) {
    do {
      if (*registrySlotCursor == (ShotDefinition *)0x0) {
        *registrySlotCursor = definition;
        bVar4 = WidePath_SetExtensionCode(0x727073,definition->resourcePathUtf16);
        if (bVar4) goto ShotDefinition_ReturnReferenceResolutionResult;
        PVar7 = Package_LoadEntry(definition->resourcePathUtf16);
        asset = PVar7.bufferOrError;
        if (PVar7.carry) goto ShotDefinition_ReturnReferenceResolutionResult;
        pSVar1 = SpriteAssetRegistry_FindById(asset->targetClassImpactDamageQ12[2]);
        if (pSVar1 == (SpriteAssetHeader *)0x0) {
          definition->ownedNestedResourcePresent = definition->ownedNestedResourcePresent + 1;
          definition->ownedNestedResource = asset;
          SVar8 = SpriteAsset_RegisterAndRelocatePointers((SpriteAssetHeader *)asset);
          asset = (ShotDefinition *)SVar8.assetOrError;
          if (SVar8.carry) goto ShotDefinition_ReturnReferenceResolutionResult;
        }
        else {
          definition->ownedNestedResource = pSVar1;
          Resource_Release(asset);
        }
        EVar9 = EffectDefinitionRegistry_FindByIdWithErrorCf
                          ((PckEffectDefinitionIdCatalog)definition->launchEffectDefinition);
        asset = (ShotDefinition *)EVar9.definitionOrError;
        if (EVar9.carry) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->launchEffectDefinition = (EffectDefinition *)asset;
        EVar9 = EffectDefinitionRegistry_FindByIdWithErrorCf
                          ((PckEffectDefinitionIdCatalog)definition->secondaryEffectDefinition);
        asset = (ShotDefinition *)EVar9.definitionOrError;
        if (EVar9.carry) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->secondaryEffectDefinition = (EffectDefinition *)asset;
        EVar9 = EffectDefinitionRegistry_FindByIdWithErrorCf
                          ((PckEffectDefinitionIdCatalog)definition->primaryEffectDefinition);
        asset = (ShotDefinition *)EVar9.definitionOrError;
        if (EVar9.carry) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->primaryEffectDefinition = (EffectDefinition *)asset;
        iVar2 = 0;
        iVar3 = 0x1f;
        goto ShotDefinition_ResolveNextPrimaryEffectReference;
      }
      registrySlotCursor = registrySlotCursor + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x100,g_PackageLastErrorPath);
    asset = (ShotDefinition *)0x45;
  }
  else {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    asset = (ShotDefinition *)0x4d;
  }
  goto ShotDefinition_ReturnReferenceResolutionResult;
  while( true ) {
    definition->terrainImpactEffectDefinitions31[iVar2] = (EffectDefinition *)asset;
    iVar2 = iVar2 + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) break;
ShotDefinition_ResolveNextPrimaryEffectReference:
    EVar9 = EffectDefinitionRegistry_FindByIdWithErrorCf
                      ((PckEffectDefinitionIdCatalog)
                       definition->terrainImpactEffectDefinitions31[iVar2]);
    asset = (ShotDefinition *)EVar9.definitionOrError;
    if (EVar9.carry) goto ShotDefinition_ReturnReferenceResolutionResult;
  }
  iVar2 = 0;
  iVar3 = 8;
  while( true ) {
    EVar9 = EffectDefinitionRegistry_FindByIdWithErrorCf
                      ((PckEffectDefinitionIdCatalog)
                       definition->targetClassImpactEffectDefinitions8[iVar2]);
    asset = (ShotDefinition *)EVar9.definitionOrError;
    if (EVar9.carry) break;
    definition->targetClassImpactEffectDefinitions8[iVar2] = (EffectDefinition *)asset;
    iVar2 = iVar2 + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) {
      return (StatusValueEaxCf5)((uint5)EVar9 & 0xffffffff);
    }
  }
ShotDefinition_ReturnReferenceResolutionResult:
  SVar6.carry = true;
  SVar6.valueOrError = (dword)asset;
  return SVar6;
}

