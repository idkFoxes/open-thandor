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
dword ShotAsset_PrepareEntries(ShotAssetHeader *asset)

{
  dword registrationStatusCode;
  undefined4 extraout_EAX;
  int extraout_ECX;
  ShotDefinition *definitionCursor;
  bool bVar1;
  undefined8 uVar2;
  
  registrationStatusCode = 0x43;
  if (((asset->entryCountHeader).common.magic == ASSET_MAGIC_SHT) &&
     ((asset->entryCountHeader).common.converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    definitionCursor = (ShotDefinition *)(asset + 1);
    bVar1 = false;
    if ((asset->entryCountHeader).entryCount != 0) {
      do {
        uVar2 = ShotDefinition_RegisterAndResolveReferencesCf(definitionCursor);
        registrationStatusCode = (dword)uVar2;
        if (bVar1) {
          return registrationStatusCode;
        }
        bVar1 = (ShotDefinition *)0xfffffd1f < definitionCursor;
        definitionCursor = definitionCursor + 1;
      } while (extraout_ECX != 1);
    }
    return registrationStatusCode;
  }
  Package_SetLastErrorPath((word *)asset);
  return extraout_EAX;
}

/* Address: 0x0052B7E0.
   Ownership: assets/shot/catalog.
   Purpose: Validates the 31 terrain-material indices embedded in every registered shot definition; invalid or
   unloaded material references return error 0x46.
*/
dword __cdecl ShotDefinitions_ValidateTerrainMaterialReferences(void)

{
  dword in_EAX;
  int iVar1;
  int iVar2;
  ShotDefinition *shotDefinition1;
  ShotDefinition **ppSVar3;
  
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
          return 0x46;
        }
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
    }
    ppSVar3 = ppSVar3 + 1;
    iVar1 = iVar1 + -1;
    if (iVar1 == 0) {
      return in_EAX;
    }
  } while( true );
}

/* Address: 0x0052B860.
   Ownership: assets/shot/catalog.
   Purpose: Scans the 256-slot shot-definition registry. On a miss it formats the unresolved identifier into
   g_PackageLastErrorPath and returns error 0x44 with CF set.
*/
ShotDefinition * ShotDefinitionRegistry_FindByIdWithErrorCf(PckShotDefinitionIdCatalog definitionId)

{
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;
  ShotDefinition *candidateDefinition;
  ShotDefinition *shotDefinition1;
  
  registryCursor = g_ShotDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  while( true ) {
    shotDefinition1 = *registryCursor;
    if ((shotDefinition1 != (ShotDefinition *)0x0) &&
       (shotDefinition1->definitionId == definitionId)) break;
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
      return (ShotDefinition *)0x44;
    }
  }
  return shotDefinition1;
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
undefined8
ShotDefinition_ComputeLaunchAnglesRegs
          (Q12 point0X,Q12 point0Y,Q12 point0Z,Q12 point1X,Q12 point1Y,Q12 point1Z,
          ShotDefinition *definition)

{
  longlong lVar1;
  int x;
  uint extraout_ECX;
  int iVar2;
  int extraout_ECX_00;
  int x_00;
  int extraout_EDX;
  dword dVar3;
  uint uVar4;
  int extraout_EDX_00;
  FixedLengthAngleEdxEax8 FVar5;
  
  x_00 = point0X - point1X;
  if (definition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    FVar5 = FixedMath_Vector2AngleAndLengthRegs(point0Y - point1Y,point0Z - point1Z);
    uVar4 = (uint)(FVar5 >> 0x20) & 0xffff;
    iVar2 = definition->launchSpeedQ12 * definition->launchSpeedQ12;
    x = (int)FVar5 * definition->ballisticDivisorQ12;
    lVar1 = (longlong)(iVar2 + definition->ballisticDivisorQ12 * x_00 * -2) * (longlong)iVar2 -
            (longlong)x * (longlong)x;
    if (lVar1 < 0) {
      lVar1 = 0;
    }
    dVar3 = FixedMath_UInt64Sqrt((UInt64Half32)((ulonglong)lVar1 >> 0x20),(UInt64Half32)lVar1);
    if ((extraout_EDX_00 < x_00) && (x_00 < 0x1000)) {
      dVar3 = -dVar3;
    }
    dVar3 = FixedMath_Atan2Angle16(extraout_ECX_00 + dVar3,x);
  }
  else {
    if (definition->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
      return 0x400000000000;
    }
    FixedMath_VectorToAngles3Regs(x_00,point0Y - point1Y,point0Z - point1Z);
    dVar3 = extraout_EDX + definition->elevationOffsetAngle16;
    uVar4 = extraout_ECX;
    if (0x4000 < (int)dVar3) {
      return CONCAT44(0x4000,extraout_ECX);
    }
  }
  return CONCAT44(dVar3,uVar4);
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
void ShotDefinition_GetModeRangeLimitEbx(ShotDefinition *definition)

{
  return;
}

/* Address: 0x0052BD80.
   Ownership: assets/shot/catalog.
   Purpose: Returns zero except for trajectory mode 3 with the verified +0x290 flag clear; that path returns the
   unsigned scaled +0x270 value multiplied by 0xAB and shifted right eight.
*/
dword ShotDefinition_ComputeMode3LeadAdjustment(ShotDefinition *definition)

{
  uint leadAdjustmentQ12;
  
  leadAdjustmentQ12 = 0;
  if ((definition->trajectoryMode == SHOT_TRAJECTORY_LEAD_ADJUSTED) &&
     (definition->mode3LeadDisabled == 0)) {
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
undefined8 ShotDefinition_RegisterAndResolveReferencesCf(ShotDefinition *definition)

{
  SpriteAssetHeader *pSVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 in_EDX;
  int extraout_EDX;
  int iVar2;
  SpriteAssetHeader *asset;
  int extraout_EDX_00;
  int extraout_EDX_01;
  ShotDefinition **registrySlotCursor;
  undefined1 in_CF;
  bool bVar3;
  undefined1 uVar4;
  undefined1 uVar5;
  
  registrySlotCursor = g_ShotDefinitionRegistry;
  ShotRuntime_FindDefinitionByIdCf(definition->definitionId);
  iVar2 = extraout_EDX;
  if ((bool)in_CF) {
    do {
      bVar3 = false;
      if (*registrySlotCursor == (ShotDefinition *)0x0) {
        *registrySlotCursor = definition;
        pSVar1 = (SpriteAssetHeader *)
                 WidePath_SetExtensionCode(0x727073,definition->resourcePathUtf16);
        if (bVar3) goto ShotDefinition_ReturnReferenceResolutionResult;
        pSVar1 = Package_LoadEntry(definition->resourcePathUtf16);
        if (bVar3) goto ShotDefinition_ReturnReferenceResolutionResult;
        pSVar1 = SpriteAssetRegistry_FindById((pSVar1->registryHeader).registryId);
        uVar4 = false;
        if (pSVar1 == (SpriteAssetHeader *)0x0) {
          definition->ownedNestedResourcePresent = definition->ownedNestedResourcePresent + 1;
          definition->ownedNestedResource = asset;
          pSVar1 = SpriteAsset_RegisterAndRelocatePointers(asset);
          uVar5 = 0;
          if ((bool)uVar4) goto ShotDefinition_ReturnReferenceResolutionResult;
        }
        else {
          definition->ownedNestedResource = pSVar1;
          Resource_Release(asset);
          uVar5 = uVar4;
        }
        pSVar1 = (SpriteAssetHeader *)
                 EffectDefinitionRegistry_FindByIdWithErrorCf
                           ((PckEffectDefinitionIdCatalog)definition->launchEffectDefinition);
        if ((bool)uVar5) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->launchEffectDefinition = (EffectDefinition *)pSVar1;
        pSVar1 = (SpriteAssetHeader *)
                 EffectDefinitionRegistry_FindByIdWithErrorCf
                           ((PckEffectDefinitionIdCatalog)definition->secondaryEffectDefinition);
        if ((bool)uVar5) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->secondaryEffectDefinition = (EffectDefinition *)pSVar1;
        pSVar1 = (SpriteAssetHeader *)
                 EffectDefinitionRegistry_FindByIdWithErrorCf
                           ((PckEffectDefinitionIdCatalog)definition->primaryEffectDefinition);
        if ((bool)uVar5) goto ShotDefinition_ReturnReferenceResolutionResult;
        definition->primaryEffectDefinition = (EffectDefinition *)pSVar1;
        iVar2 = 0;
        goto ShotDefinition_ResolveNextPrimaryEffectReference;
      }
      registrySlotCursor = registrySlotCursor + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x100,g_PackageLastErrorPath);
    pSVar1 = (SpriteAssetHeader *)0x45;
  }
  else {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    pSVar1 = (SpriteAssetHeader *)0x4d;
  }
  goto ShotDefinition_ReturnReferenceResolutionResult;
  while( true ) {
    definition->terrainImpactEffectDefinitions31[extraout_ECX] = (EffectDefinition *)pSVar1;
    iVar2 = extraout_ECX + 1;
    if (extraout_EDX_00 == 1) break;
ShotDefinition_ResolveNextPrimaryEffectReference:
    bVar3 = false;
    pSVar1 = (SpriteAssetHeader *)
             EffectDefinitionRegistry_FindByIdWithErrorCf
                       ((PckEffectDefinitionIdCatalog)
                        definition->terrainImpactEffectDefinitions31[iVar2]);
    if (bVar3) goto ShotDefinition_ReturnReferenceResolutionResult;
  }
  iVar2 = 0;
  while( true ) {
    bVar3 = false;
    pSVar1 = (SpriteAssetHeader *)
             EffectDefinitionRegistry_FindByIdWithErrorCf
                       ((PckEffectDefinitionIdCatalog)
                        definition->targetClassImpactEffectDefinitions8[iVar2]);
    if (bVar3) break;
    definition->targetClassImpactEffectDefinitions8[extraout_ECX_00] = (EffectDefinition *)pSVar1;
    iVar2 = extraout_ECX_00 + 1;
    if (extraout_EDX_01 == 1) {
      return CONCAT44(in_EDX,pSVar1);
    }
  }
ShotDefinition_ReturnReferenceResolutionResult:
  return CONCAT44(in_EDX,pSVar1);
}
