/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/model/definitions.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/model/definitions.h>

/* Implementation ownership: assets/model/definitions. */

/* Address: 0x0051B3C0.
   Ownership: assets/model/definitions.
   Purpose: Scans the eight linked model-definition identifiers, retains the faction-unlocked selection, and
   resolves the selected identifier through the model-definition registry, preserving carry status. It is distinct
   from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed ArmyAssetId,
   ModelDefinitionId, and TechnologyId domains. Typed parameters: p3
   linkedDefinitionList→ModelLinkedDefinitionListAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: ModelDefinition_IsFactionTechnologyUnlockedCf, ModelDefinitionRegistry_FindByIdWithErrorCf.
*/
ModelDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList)

{
  PckModelDefinitionIdCatalog modelDefinitionId;
  int iVar1;
  PckModelDefinitionIdCatalog definitionId;
  bool bVar2;
  ModelDefinitionLookupEaxCf5 MVar3;
  
  iVar1 = 8;
  definitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
  do {
    modelDefinitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
    if (modelDefinitionId != 0) {
      bVar2 = ModelDefinition_IsFactionTechnologyUnlockedCf
                        (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                         modelDefinitionId);
      if (!bVar2) {
        definitionId = modelDefinitionId;
      }
    }
    linkedDefinitionList = linkedDefinitionList + 4;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  MVar3 = ModelDefinitionRegistry_FindByIdWithErrorCf(definitionId);
  return (ModelDefinitionLookupEaxCf5)((uint5)MVar3 & 0xffffffff);
}


/* Address: 0x0051DB00.
   Ownership: assets/model/definitions.
   Purpose: Traverses the linked model-definition hierarchy, selects each faction-unlocked linked identifier, and
   applies its linked technology unlock to the faction. It is distinct from FrontendPlayerIndex_V306,
   PlayerRuntimeId, active-faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId
   domains. Typed parameters: p3 definitionNode→ModelDefinitionHierarchyNodeAddress32_V345. Calling convention,
   complete VariableStorage serialization, function bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Local calls: ModelDefinition_SelectFactionUnlockedLinkedIdCf,
   ModelDefinition_UnlockLinkedTechnologyForFactionCf.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  PckModelDefinitionIdCatalog modelDefinitionId;
  int iVar1;
  int unaff_EBP;
  int unaff_ESI;
  ModelLinkedDefinitionListAddress32 linkedDefinitionList;
  
  iVar1 = 0;
  linkedDefinitionList = *(ModelLinkedDefinitionListAddress32 *)(definitionNode + 0xc);
  do {
    modelDefinitionId =
         ModelDefinition_SelectFactionUnlockedLinkedIdCf(factionIndex,linkedDefinitionList);
    ModelDefinition_UnlockLinkedTechnologyForFactionCf(factionIndex,modelDefinitionId);
    iVar1 = iVar1 + 1;
    while( true ) {
      if (unaff_EBP != 0) break;
      iVar1 = iVar1 + -1;
      if (iVar1 == 0) {
        return;
      }
    }
    linkedDefinitionList = *(ModelLinkedDefinitionListAddress32 *)(unaff_ESI + 0xc);
    unaff_ESI = unaff_ESI + 4;
    unaff_EBP = unaff_EBP + -1;
  } while( true );
}


/* Address: 0x0051DA60.
   Ownership: assets/model/definitions.
   Purpose: Traverses the linked model-definition hierarchy and tests every definition technology requirement
   against the selected faction masks, preserving the carry-status result. It is distinct from
   FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed ArmyAssetId,
   ModelDefinitionId, and TechnologyId domains. Typed parameters: p3
   definitionNode→ModelDefinitionHierarchyNodeAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: ModelDefinition_IsFactionTechnologyUnlockedCf.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  int iVar1;
  int unaff_EBP;
  int unaff_ESI;
  int iVar2;
  bool bVar3;
  
  iVar1 = 0;
  iVar2 = *(int *)(definitionNode + 0xc);
  do {
    bVar3 = ModelDefinition_IsFactionTechnologyUnlockedCf
                      (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                       *(PckModelDefinitionIdCatalog *)(iVar2 + 0x20));
    if (bVar3) {
      return true;
    }
    iVar1 = iVar1 + 1;
    while( true ) {
      if (unaff_EBP != 0) break;
      iVar1 = iVar1 + -1;
      if (iVar1 == 0) {
        return false;
      }
    }
    iVar2 = *(int *)(unaff_ESI + 0xc);
    unaff_ESI = unaff_ESI + 4;
    unaff_EBP = unaff_EBP + -1;
  } while( true );
}


/* Address: 0x00528950.
   Ownership: assets/model/definitions.
   Purpose: Validates the 'mdl' magic and converter version 0x0008000A, then prepares recordCount variable-size
   records beginning at +0x200. Each successful record advances by its leading byte-size dword. The per-record
   preparer receives the asset base for stored-offset relocation. Payload fields remain opaque. Role: Walks
   variable-size MDL records and registers each model definition.
   Local calls: ModelDefinition_RegisterAndResolveReferencesCf.
*/
StatusValueEaxCf5 __thandor_void_preserve_ecx_edx ModelAsset_PrepareRecords(ModelAssetHeader *asset)

{
  dword registrationStatusCode;
  AssetRecordCount AVar1;
  ModelDefinitionResolvePhaseView280 *definition;
  ModelDefinitionRecordPrefix *definitionCursor;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  
  registrationStatusCode = 0x3d;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_MDL) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_MDL_0008000A)) {
    AVar1 = (asset->recordCountHeader).recordCount;
    definition = (ModelDefinitionResolvePhaseView280 *)(asset + 1);
    while( true ) {
      if (AVar1 == 0) {
        SVar2.carry = false;
        SVar2.valueOrError = registrationStatusCode;
        return SVar2;
      }
      SVar2 = ModelDefinition_RegisterAndResolveReferencesCf(definition,asset);
      registrationStatusCode = SVar2.valueOrError;
      if (SVar2.carry) break;
      definition = (ModelDefinitionResolvePhaseView280 *)
                   (definition->reserved010_017 + (definition->byteSize - 0x10));
      AVar1 = AVar1 - 1;
    }
  }
  SVar3.carry = true;
  SVar3.valueOrError = registrationStatusCode;
  return SVar3;
}


/* Address: 0x004BE670.
   Ownership: assets/model/definitions.
   Purpose: Scans the model lookup table at offsets +0xE4/+0xE8 for packed key (groupIndex << 4) | itemIndex. On
   success it returns the three payload dwords from the matching 0x10-byte entry in EAX, ECX, and EDX with CF
   clear; on failure it zeros those registers and sets CF. Saved ids, relocated pointers, attachment selectors, and
   runtime class ids remain separate. Key index and key class remain separate 32-bit domains. Typed parameters: p0
   keyIndex→ModelLookupKeyIndex_V338, p1 keyClass→ModelLookupKeyClass_V338.
*/
ModelLookupPayloadEaxEcxEdxCf13
ModelLookupTable_FindPackedKeyEntryRegsCf
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResourceHitTestAndRenderView210 *modelDefinition)

{
  int entriesRemaining;
  uint *packedKeyEntryCursor;
  ModelLookupPayloadEaxEcxEdxCf13 MVar1;
  
  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  packedKeyEntryCursor =
       (uint *)(modelDefinition->reserved00_AF + modelDefinition->packedLookupTableRelativeOffset);
  while( true ) {
    if (entriesRemaining == 0) {
      return (ModelLookupPayloadEaxEcxEdxCf13)(ZEXT513(0x100000000) << 0x40);
    }
    if ((keyClass | keyIndex << 4) == *packedKeyEntryCursor) break;
    packedKeyEntryCursor = packedKeyEntryCursor + 4;
    entriesRemaining = entriesRemaining - 1;
  }
  MVar1.carry = false;
  MVar1._0_12_ = *(undefined1 (*) [12])(packedKeyEntryCursor + 1);
  return MVar1;
}


/* Address: 0x004BE6F0.
   Ownership: assets/model/definitions.
   Purpose: Scans the same 0x10-byte model lookup table for packed key (groupIndex << 4) | itemIndex. CF is clear
   when a matching entry exists and set when the table is empty or no key matches. Saved ids, relocated pointers,
   attachment selectors, and runtime class ids remain separate. Key index and key class remain separate 32-bit
   domains. Typed parameters: p0 keyIndex→ModelLookupKeyIndex_V338, p1 keyClass→ModelLookupKeyClass_V338.
*/
ModelLookupEntryEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelLookupTable_ContainsPackedKeyCf
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResourceHitTestAndRenderView210 *modelDefinition)

{
  ModelPackedPointRecord *packedKeyEntryCursor;
  int entriesRemaining;
  ModelLookupEntryEaxCf5 MVar1;
  ModelLookupEntryEaxCf5 MVar2;
  
  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  packedKeyEntryCursor =
       (ModelPackedPointRecord *)
       (modelDefinition->reserved00_AF + modelDefinition->packedLookupTableRelativeOffset);
  while( true ) {
    if (entriesRemaining == 0) {
      MVar2.carry = true;
      MVar2.entry = packedKeyEntryCursor;
      return MVar2;
    }
    if ((keyClass | keyIndex << 4) == packedKeyEntryCursor->packedLookupKey) break;
    packedKeyEntryCursor = packedKeyEntryCursor + 1;
    entriesRemaining = entriesRemaining - 1;
  }
  MVar1.carry = false;
  MVar1.entry = packedKeyEntryCursor;
  return MVar1;
}


/* Address: 0x0050AEA0.
   Ownership: assets/model/definitions.
   Purpose: Intersects the shared model-space ray with one exact 0x40-byte triangle record. EAX is Q12 distance; CF
   set means hit, CF clear means no hit.
*/
TerrainDistanceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelMesh_IntersectTriangleRayDistanceCf(ModelRaycastTriangleDescriptor *triangle)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  GraphicsFixedVec3 *pGVar5;
  GraphicsFixedVec3 *pGVar6;
  GraphicsFixedVec3 *pGVar7;
  ulonglong uVar8;
  longlong lVar9;
  longlong lVar10;
  longlong lVar11;
  longlong lVar12;
  longlong lVar13;
  int iVar14;
  uint uVar15;
  int iVar16;
  uint uVar17;
  int iVar18;
  int iVar19;
  int iVar20;
  int iVar21;
  int iVar22;
  uint uVar23;
  int iVar24;
  TerrainDistanceEaxCf5 TVar25;
  TerrainDistanceEaxCf5 TVar26;
  
  iVar14 = triangle->planeNormalX << 0x10;
  iVar16 = triangle->planeNormalY << 0x10;
  iVar18 = triangle->planeNormalZ << 0x10;
  pGVar5 = triangle->vertex0;
  pGVar6 = triangle->vertex1;
  pGVar7 = triangle->vertex2;
  uVar8 = (longlong)iVar16 *
          (longlong)((pGVar5->y * 2 + pGVar6->y + pGVar7->y >> 2) - g_ModelRaycastLocalOriginY) +
          (longlong)((pGVar5->x * 2 + pGVar6->x + pGVar7->x >> 2) - g_ModelRaycastLocalOriginX) *
          (longlong)iVar14 +
          (longlong)((pGVar5->z * 2 + pGVar6->z + pGVar7->z >> 2) - g_ModelRaycastLocalOriginZ) *
          (longlong)iVar18;
  uVar23 = (uint)(uVar8 >> 0x20);
  lVar9 = (longlong)g_ModelRaycastLocalDirectionYQ28 * (longlong)iVar16 +
          (longlong)g_ModelRaycastLocalDirectionXQ28 * (longlong)iVar14 +
          (longlong)g_ModelRaycastLocalDirectionZQ28 * (longlong)iVar18;
  uVar17 = (int)((ulonglong)lVar9 >> 0x20) << 4 | (uint)lVar9 >> 0x1c;
  uVar15 = g_ModelRaycastMaximumDistance;
  if (uVar17 != 0) {
    iVar19 = (int)((ulonglong)((longlong)(int)g_ModelRaycastMaximumDistance * (longlong)(int)uVar17)
                  >> 0x20);
    uVar15 = (uint)((longlong)(int)g_ModelRaycastMaximumDistance * (longlong)(int)uVar17);
    iVar24 = (int)uVar23 >> 1;
    if ((longlong)uVar8 < 0) {
      if (((int)uVar23 < iVar19) ||
         ((iVar24 <= (int)-uVar17 && (uVar15 = (uint)uVar8, iVar24 <= (int)uVar17))))
      goto LAB_0050afa0;
    }
    else if ((iVar19 < (int)uVar23) ||
            (((int)-uVar17 <= iVar24 && (uVar15 = (uint)uVar8, (int)uVar17 <= iVar24))))
    goto LAB_0050afa0;
    TVar26.distanceQ12 =
         (int)((longlong)((ulonglong)uVar23 << 0x20 | uVar8 & 0xffffffff) / (longlong)(int)uVar17);
    pGVar5 = triangle->vertex0;
    iVar2 = -pGVar5->x;
    iVar20 = ((int)((ulonglong)
                    ((longlong)TVar26.distanceQ12 * (longlong)g_ModelRaycastLocalDirectionXQ28) >>
                   0x20) << 4 |
             (uint)((longlong)TVar26.distanceQ12 * (longlong)g_ModelRaycastLocalDirectionXQ28) >>
             0x1c) + g_ModelRaycastLocalOriginX + iVar2;
    iVar3 = -pGVar5->y;
    iVar21 = ((int)((ulonglong)
                    ((longlong)g_ModelRaycastLocalDirectionYQ28 * (longlong)TVar26.distanceQ12) >>
                   0x20) << 4 |
             (uint)((longlong)g_ModelRaycastLocalDirectionYQ28 * (longlong)TVar26.distanceQ12) >>
             0x1c) + g_ModelRaycastLocalOriginY + iVar3;
    iVar4 = -pGVar5->z;
    iVar22 = ((int)((ulonglong)
                    ((longlong)g_ModelRaycastLocalDirectionZQ28 * (longlong)TVar26.distanceQ12) >>
                   0x20) << 4 |
             (uint)((longlong)g_ModelRaycastLocalDirectionZQ28 * (longlong)TVar26.distanceQ12) >>
             0x1c) + g_ModelRaycastLocalOriginZ + iVar4;
    pGVar5 = triangle->vertex1;
    pGVar6 = triangle->vertex2;
    iVar19 = iVar2 + pGVar5->x;
    iVar24 = iVar3 + pGVar5->y;
    iVar1 = iVar4 + pGVar5->z;
    iVar2 = iVar2 + pGVar6->x;
    iVar3 = iVar3 + pGVar6->y;
    iVar4 = iVar4 + pGVar6->z;
    lVar9 = (longlong)iVar16 * (longlong)iVar1 - (longlong)iVar18 * (longlong)iVar24;
    uVar15 = (int)((ulonglong)lVar9 >> 0x20) << 4 | (uint)lVar9 >> 0x1c;
    lVar9 = (longlong)iVar18 * (longlong)iVar19 - (longlong)iVar14 * (longlong)iVar1;
    uVar17 = (int)((ulonglong)lVar9 >> 0x20) << 4 | (uint)lVar9 >> 0x1c;
    lVar9 = (longlong)iVar14 * (longlong)iVar24 - (longlong)iVar16 * (longlong)iVar19;
    uVar23 = (int)((ulonglong)lVar9 >> 0x20) << 4 | (uint)lVar9 >> 0x1c;
    lVar9 = (longlong)iVar21 * (longlong)(int)uVar17 + (longlong)iVar20 * (longlong)(int)uVar15 +
            (longlong)iVar22 * (longlong)(int)uVar23;
    lVar10 = (longlong)iVar3 * (longlong)(int)uVar17 + (longlong)(int)uVar15 * (longlong)iVar2 +
             (longlong)iVar4 * (longlong)(int)uVar23;
    iVar19 = (int)((ulonglong)lVar10 >> 0x20);
    lVar11 = (longlong)iVar16 * (longlong)iVar22 - (longlong)iVar18 * (longlong)iVar21;
    lVar12 = (longlong)iVar18 * (longlong)iVar20 - (longlong)iVar14 * (longlong)iVar22;
    lVar13 = (longlong)iVar14 * (longlong)iVar21 - (longlong)iVar16 * (longlong)iVar20;
    lVar11 = (longlong)iVar3 *
             (longlong)(int)((int)((ulonglong)lVar12 >> 0x20) << 4 | (uint)lVar12 >> 0x1c) +
             (longlong)iVar2 *
             (longlong)(int)((int)((ulonglong)lVar11 >> 0x20) << 4 | (uint)lVar11 >> 0x1c) +
             (longlong)iVar4 *
             (longlong)(int)((int)((ulonglong)lVar13 >> 0x20) << 4 | (uint)lVar13 >> 0x1c);
    uVar15 = (uint)lVar11;
    if (lVar10 < 0) {
      if (((lVar11 < 0) && (lVar9 < 0)) &&
         (uVar15 = (uint)(lVar11 + lVar9),
         (int)((iVar19 - (int)((ulonglong)(lVar11 + lVar9) >> 0x20)) - (uint)((uint)lVar10 < uVar15)
              ) < 0)) goto LAB_0050b1b8;
    }
    else if (((-1 < lVar11) && (-1 < lVar9)) &&
            (uVar15 = (uint)(lVar11 + lVar9),
            -1 < (int)((iVar19 - (int)((ulonglong)(lVar11 + lVar9) >> 0x20)) -
                      (uint)((uint)lVar10 < uVar15)))) {
LAB_0050b1b8:
      TVar26.carry = true;
      return TVar26;
    }
  }
LAB_0050afa0:
  TVar25.carry = false;
  TVar25.distanceQ12 = uVar15;
  return TVar25;
}


/* Address: 0x005289C0.
   Ownership: assets/model/definitions.
   Purpose: Finds a model definition by identifier in the 768-slot registry and returns its runtime descriptor at
   record +0x188; error 0x3E reports a miss.
*/
ModelBuildMetricEaxEcxEdxCf13
ModelDefinitionRegistry_FindBuildMetricTupleByIdCf(PckModelDefinitionIdCatalog definitionId)

{
  ModelDefinitionRecordPrefix *pMVar1;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  ModelBuildMetricEaxEcxEdxCf13 MVar2;
  ModelBuildMetricEaxEcxEdxCf13 MVar3;
  ModelDefinitionRuntimeSemanticView280 *candidateDefinition;
  
  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 0x300;
  while ((pMVar1 = *registryCursor, pMVar1 == (ModelDefinitionRecordPrefix *)0x0 ||
         (definitionId != pMVar1->definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
      MVar2.metric1 = definitionId;
      MVar2.metric0 = 0x3e;
      MVar2.metric2 = 0;
      MVar2.carry = true;
      return MVar2;
    }
  }
  MVar3.metric1 = pMVar1[0x20].byteSize;
  MVar3.metric0 = pMVar1[0x20].definitionId;
  MVar3.metric2 = pMVar1[0x20].flags;
  MVar3.carry = false;
  return MVar3;
}


/* Address: 0x0053BA00.
   Ownership: assets/model/definitions.
   Purpose: Returns the first ModelDefinitionRecordPrefix whose runtime class field matches runtimeClassId. One
   stdcall stack argument; preserved EDX is loop state, not a return value.
*/
ModelDefinitionRecordPrefix *
ModelDefinitionRegistry_FindByRuntimeClassId(ModelRuntimeClassId runtimeClassId)

{
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  ModelDefinitionRecordPrefix *candidateDefinition;
  
  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 0x300;
  while ((candidateDefinition = *registryCursor,
         candidateDefinition == (ModelDefinitionRecordPrefix *)0x0 ||
         (runtimeClassId != candidateDefinition[0x25].flags))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      return (ModelDefinitionRecordPrefix *)0x0;
    }
  }
  return candidateDefinition;
}

/* Address: 0x0051B430.
   Ownership: assets/model/definitions.
   Purpose: Scans the eight linked model-definition identifiers and returns the faction-unlocked selected
   identifier while preserving the verified carry-status convention. It is distinct from FrontendPlayerIndex_V306,
   PlayerRuntimeId, active-faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId
   domains. Typed parameters: p3 linkedDefinitionList→ModelLinkedDefinitionListAddress32_V345. Calling convention,
   complete VariableStorage serialization, function bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Local calls: ModelDefinition_IsFactionTechnologyUnlockedCf.
*/
PckModelDefinitionIdCatalog __thandor_eax_preserve_ecx_edx
ModelDefinition_SelectFactionUnlockedLinkedIdCf
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList)

{
  PckModelDefinitionIdCatalog modelDefinitionId;
  int iVar1;
  PckModelDefinitionIdCatalog PVar2;
  bool bVar3;
  
  iVar1 = 8;
  PVar2 = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
  do {
    modelDefinitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
    if (modelDefinitionId != 0) {
      bVar3 = ModelDefinition_IsFactionTechnologyUnlockedCf
                        (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                         modelDefinitionId);
      if (!bVar3) {
        PVar2 = modelDefinitionId;
      }
    }
    linkedDefinitionList = linkedDefinitionList + 4;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return PVar2;
}


/* Address: 0x00528600.
   Ownership: assets/model/definitions.
   Purpose: Registers one variable-size model definition in the fixed 768-slot registry, relocates its embedded
   record chain by the asset base, loads or reuses referenced sprite assets, resolves verified shot/effect
   identifiers, and derives verified mode-dependent fields. Duplicate, capacity, load, or reference failures return
   through CF/EAX. Role: Registers one MDL definition and resolves its SPR, linked MDL, SHT and EFF references.
   Inputs: Variable-size MDL record beginning after the 0x200-byte image header. Outputs: ModelDefinition whose
   serialized IDs/offsets are replaced by runtime pointers.
   Local calls: ModelDefinitionRegistry_FindByIdWithErrorCf.
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], Package_LoadEntry [assets/package/runtime],
   SpriteAssetRegistry_FindById [assets/sprite/catalog], SpriteAsset_RegisterAndRelocatePointers
   [assets/sprite/catalog], Resource_Release [assets/resource/runtime], ShotDefinitionRegistry_FindByIdWithErrorCf
   [assets/shot/catalog].
*/

StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelDefinition_RegisterAndResolveReferencesCf
          (ModelDefinitionResolvePhaseView280 *definition,ModelAssetHeader *asset)

{
  dword dVar1;
  MdlChildCount MVar2;
  dword dVar3;
  dword resolverStatusOrSentinel;
  SpriteAssetHeader *loadedSpriteAsset;
  SpriteAssetHeader *pSVar4;
  ShotDefinition *pSVar5;
  ShotDefinition *resolvedShotDefinition2C;
  EffectDefinition *resolvedEffectDefinition80ToB8;
  ShotDefinition *resolvedShotDefinition168;
  EffectDefinition *resolvedEffectDefinitionTail;
  dword gridDerivedScalarCarrier;
  int iVar6;
  ModelDefinitionRecordPrefix **registrySlotCursor;
  MdlSerializedNodeHeader38 *serializedNodeCursor;
  bool bVar7;
  ModelDefinitionLookupEaxCf5 MVar8;
  StatusValueEaxCf5 SVar9;
  PackageLoadEntryEaxCf5 PVar10;
  SpriteRegisterRelocateEaxCf5 SVar11;
  ShotDefinitionLookupEaxCf5 SVar12;
  EffectDefinitionLookupEaxCf5 EVar13;
  StatusValueEaxCf5 SVar14;
  
  registrySlotCursor = g_ModelDefinitionRegistry;
  iVar6 = 0x300;
  MVar8 = ModelDefinitionRegistry_FindByIdWithErrorCf(definition->definitionId);
  if (MVar8.carry) {
    do {
      if (*registrySlotCursor == (ModelDefinitionRecordPrefix *)0x0) {
        dVar1 = definition->serializedNodeOffsetOrPointer64;
        *registrySlotCursor = (ModelDefinitionRecordPrefix *)definition;
        if (dVar1 == 0) goto ModelDefinition_ResolveShotAndEffectReferences;
        definition->serializedNodeOffsetOrPointer64 =
             (dword)((asset->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                    (definition->serializedNodeOffsetOrPointer64 - 0x28));
        serializedNodeCursor =
             (MdlSerializedNodeHeader38 *)
             ((asset->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
             (dVar1 - 0x28));
        iVar6 = 0;
        resolverStatusOrSentinel = (dword)asset;
        goto ModelDefinition_LoadOrReuseSpriteReference;
      }
      registrySlotCursor = registrySlotCursor + 1;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x300,g_PackageLastErrorPath);
    resolverStatusOrSentinel = 0x3f;
  }
  else {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    resolverStatusOrSentinel = MODEL_DEFINITION_REFERENCE_FAILURE_SENTINEL_0x4B;
  }
ModelDefinition_ReturnReferenceResolutionResult:
  SVar9.carry = true;
  SVar9.valueOrError = resolverStatusOrSentinel;
  return SVar9;
ModelDefinition_LoadOrReuseSpriteReference:
  MVar2 = serializedNodeCursor->childCount;
  pSVar5 = (ShotDefinition *)resolverStatusOrSentinel;
  if ((serializedNodeCursor->nodeFlags & 0xf) == 0) {
    bVar7 = WidePath_SetExtensionCode(0x727073,(word *)(serializedNodeCursor + 1));
    if (bVar7) goto ModelDefinition_ReturnReferenceResolutionResult;
    PVar10 = Package_LoadEntry((word *)(serializedNodeCursor + 1));
    loadedSpriteAsset = PVar10.bufferOrError;
    resolverStatusOrSentinel = (dword)loadedSpriteAsset;
    if (PVar10.carry) goto ModelDefinition_ReturnReferenceResolutionResult;
    pSVar4 = SpriteAssetRegistry_FindById((loadedSpriteAsset->registryHeader).registryId);
    if (pSVar4 == (SpriteAssetHeader *)0x0) {
      serializedNodeCursor->ownedNestedResourcePresent =
           serializedNodeCursor->ownedNestedResourcePresent + 1;
      (serializedNodeCursor->spriteAssetReference).spriteAsset = loadedSpriteAsset;
      SVar11 = SpriteAsset_RegisterAndRelocatePointers(loadedSpriteAsset);
      resolverStatusOrSentinel = (dword)SVar11.assetOrError;
      if (SVar11.carry) goto ModelDefinition_ReturnReferenceResolutionResult;
    }
    else {
      (serializedNodeCursor->spriteAssetReference).spriteAsset = pSVar4;
      Resource_Release(loadedSpriteAsset);
    }
  }
  iVar6 = iVar6 + 1;
  while (MVar2 == 0) {
    iVar6 = iVar6 + -1;
    if (iVar6 == 0) goto ModelDefinition_ResolveShotAndEffectReferences;
  }
  serializedNodeCursor->childSerializedOffsets[0] =
       (int)pSVar5->terrainImpactEffectDefinitions31 +
       (serializedNodeCursor->childSerializedOffsets[0] - 0x14);
  serializedNodeCursor =
       (MdlSerializedNodeHeader38 *)serializedNodeCursor->childSerializedOffsets[0];
  resolverStatusOrSentinel = (dword)pSVar5;
  goto ModelDefinition_LoadOrReuseSpriteReference;
ModelDefinition_ResolveShotAndEffectReferences:
  SVar12 = ShotDefinitionRegistry_FindByIdWithErrorCf
                     ((PckShotDefinitionIdCatalog)definition->shotDefinitionReference2C);
  resolvedShotDefinition2C = SVar12.definitionOrError;
  resolverStatusOrSentinel = (dword)resolvedShotDefinition2C;
  if (!SVar12.carry) {
    definition->shotDefinitionReference2C = resolvedShotDefinition2C;
    EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                       ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference80);
    resolvedEffectDefinition80ToB8 = EVar13.definitionOrError;
    resolverStatusOrSentinel = (dword)resolvedEffectDefinition80ToB8;
    if (!EVar13.carry) {
      definition->effectDefinitionReference80 = resolvedEffectDefinition80ToB8;
      EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                         ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference88);
      resolverStatusOrSentinel = (dword)EVar13.definitionOrError;
      if (!EVar13.carry) {
        definition->effectDefinitionReference88 = (EffectDefinition *)resolverStatusOrSentinel;
        EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                           ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference90);
        resolverStatusOrSentinel = (dword)EVar13.definitionOrError;
        if (!EVar13.carry) {
          definition->effectDefinitionReference90 = (EffectDefinition *)resolverStatusOrSentinel;
          EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                             ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference98)
          ;
          resolverStatusOrSentinel = (dword)EVar13.definitionOrError;
          if (!EVar13.carry) {
            definition->effectDefinitionReference98 = (EffectDefinition *)resolverStatusOrSentinel;
            EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                               ((PckEffectDefinitionIdCatalog)
                                definition->effectDefinitionReferenceA0);
            resolverStatusOrSentinel = (dword)EVar13.definitionOrError;
            if (!EVar13.carry) {
              definition->effectDefinitionReferenceA0 = (EffectDefinition *)resolverStatusOrSentinel
              ;
              EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                                 ((PckEffectDefinitionIdCatalog)
                                  definition->effectDefinitionReferenceA8);
              resolverStatusOrSentinel = (dword)EVar13.definitionOrError;
              if (!EVar13.carry) {
                definition->effectDefinitionReferenceA8 =
                     (EffectDefinition *)resolverStatusOrSentinel;
                EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                                   ((PckEffectDefinitionIdCatalog)
                                    definition->effectDefinitionReferenceB0);
                resolverStatusOrSentinel = (dword)EVar13.definitionOrError;
                if (!EVar13.carry) {
                  definition->effectDefinitionReferenceB0 =
                       (EffectDefinition *)resolverStatusOrSentinel;
                  EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                                     ((PckEffectDefinitionIdCatalog)
                                      definition->effectDefinitionReferenceB8);
                  resolverStatusOrSentinel = (dword)EVar13.definitionOrError;
                  if (!EVar13.carry) {
                    definition->effectDefinitionReferenceB8 =
                         (EffectDefinition *)resolverStatusOrSentinel;
                    if (definition->shotDefinitionReference168 != (ShotDefinition *)0xffffffff) {
                      SVar12 = ShotDefinitionRegistry_FindByIdWithErrorCf
                                         ((PckShotDefinitionIdCatalog)
                                          definition->shotDefinitionReference168);
                      resolvedShotDefinition168 = SVar12.definitionOrError;
                      resolverStatusOrSentinel = (dword)resolvedShotDefinition168;
                      if (SVar12.carry) goto ModelDefinition_ReturnReferenceResolutionResult;
                      definition->shotDefinitionReference168 = resolvedShotDefinition168;
                    }
                    EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                                       ((PckEffectDefinitionIdCatalog)
                                        definition->effectDefinitionReference174);
                    resolvedEffectDefinitionTail = EVar13.definitionOrError;
                    resolverStatusOrSentinel = (dword)resolvedEffectDefinitionTail;
                    if (!EVar13.carry) {
                      definition->effectDefinitionReference174 = resolvedEffectDefinitionTail;
                      EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                                         ((PckEffectDefinitionIdCatalog)
                                          definition->effectDefinitionReference58);
                      resolverStatusOrSentinel = (dword)EVar13.definitionOrError;
                      if (!EVar13.carry) {
                        definition->effectDefinitionReference58 =
                             (EffectDefinition *)resolverStatusOrSentinel;
                        EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                                           ((PckEffectDefinitionIdCatalog)
                                            definition->effectDefinitionReference190);
                        resolverStatusOrSentinel = (dword)EVar13.definitionOrError;
                        if (!EVar13.carry) {
                          definition->effectDefinitionReference190 =
                               (EffectDefinition *)resolverStatusOrSentinel;
                          EVar13 = EffectDefinitionRegistry_FindByIdWithErrorCf
                                             ((PckEffectDefinitionIdCatalog)
                                              definition->effectDefinitionReference254);
                          resolverStatusOrSentinel = (dword)EVar13.definitionOrError;
                          if (!EVar13.carry) {
                            definition->effectDefinitionReference254 =
                                 (EffectDefinition *)resolverStatusOrSentinel;
                            dVar1 = definition->gridClassification264;
                            if (-1 < (int)definition->gridClassification260) {
                              resolverStatusOrSentinel =
                                   (&g_GridInfluenceRadiusOffset0)
                                   [definition->gridClassification260];
                              definition->placementRadiusOrClearanceDC = resolverStatusOrSentinel;
                              definition->gridDerivedRuntimeValue1A0 = resolverStatusOrSentinel;
                            }
                            if (-1 < (int)dVar1) {
                              if (definition->placementContactKindIndex278 == 1) {
                                resolverStatusOrSentinel =
                                     (&g_GridTerrainClassBit24MaxWaterSurfaceDelta)[dVar1];
                                dVar1 = (&g_GridTerrainClassBit28MaxTriangle0NormalAngleHigh16)
                                        [dVar1];
                                *(dword *)(definition->reserved0C0_0DB + 0xc) =
                                     resolverStatusOrSentinel;
                                definition->runtimeValue24 = dVar1;
                              }
                              else if (definition->placementContactKindIndex278 == 4) {
                                resolverStatusOrSentinel =
                                     *(dword *)(&g_ModelTraversalClass4SecondaryThresholdTable3 +
                                               (dVar1 - 1) * 4);
                                definition->runtimeValue24 =
                                     (&g_GridTerrainClassBit25MaxSelectedNormalAngleHigh16)
                                     [dVar1 - 1];
                                definition->runtimeValue268 = resolverStatusOrSentinel;
                              }
                              else {
                                iVar6 = dVar1 - 4;
                                resolverStatusOrSentinel =
                                     (&g_GridTerrainClassBit28MinWaterSurfaceDelta)[iVar6];
                                dVar1 = (&g_GridTerrainClassBit28MaxTriangle0NormalAngleHigh16)
                                        [iVar6];
                                dVar3 = *(dword *)(&g_ModelTraversalFallbackSecondaryThresholdTable3
                                                  + iVar6 * 4);
                                definition->runtimeValue198 = resolverStatusOrSentinel;
                                definition->runtimeValue24 = dVar1;
                                definition->runtimeValue268 = dVar3;
                              }
                            }
                            SVar14.carry = false;
                            SVar14.valueOrError = resolverStatusOrSentinel;
                            return SVar14;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  goto ModelDefinition_ReturnReferenceResolutionResult;
}


/* Address: 0x0052ADE0.
   Ownership: assets/model/definitions.
   Purpose: The original lookup/unlock CF contract is preserved.
   Local calls: ModelDefinitionRegistry_FindByIdWithErrorCf.
   Cross-module calls: Technology_UnlockForFaction [gameplay/technology/runtime].
*/
void __thandor_preserve_eax
ModelDefinition_UnlockLinkedTechnologyForFactionCf
          (FactionRuntimeIndex factionIndex,PckModelDefinitionIdCatalog modelDefinitionId)

{
  ModelDefinitionLookupEaxCf5 MVar1;
  
  MVar1 = ModelDefinitionRegistry_FindByIdWithErrorCf(modelDefinitionId);
  if (!MVar1.carry) {
    Technology_UnlockForFaction(0,0,MVar1.modelDefinition[0x25].definitionId,factionIndex);
  }
  return;
}


/* Address: 0x0052AD90.
   Ownership: assets/model/definitions.
   Purpose: CF clear means the bit is unlocked; lookup failure or a clear bit returns CF set while preserving EAX.
   Local calls: ModelDefinitionRegistry_FindByIdWithErrorCf.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ModelDefinition_IsFactionTechnologyUnlockedCf
          (dword *factionTechnologyMasks,PckModelDefinitionIdCatalog modelDefinitionId)

{
  uint uVar1;
  ModelDefinitionLookupEaxCf5 MVar2;
  
  MVar2 = ModelDefinitionRegistry_FindByIdWithErrorCf(modelDefinitionId);
  if ((!MVar2.carry) &&
     (uVar1 = MVar2.modelDefinition[0x25].flags,
     (factionTechnologyMasks[uVar1 >> 5] & 1 << ((byte)uVar1 & 0x1f)) != 0)) {
    return false;
  }
  return true;
}


/* Address: 0x00528E20.
   Ownership: assets/model/definitions.
   Purpose: Scans the 768-slot model-definition registry. On a miss it formats the unresolved identifier into
   g_PackageLastErrorPath and returns error 0x3E with CF set.
*/
ModelDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelDefinitionRegistry_FindByIdWithErrorCf(PckModelDefinitionIdCatalog definitionId)

{
  ModelDefinitionRecordPrefix *arg4;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  ModelDefinitionLookupEaxCf5 MVar1;
  ModelDefinitionLookupEaxCf5 MVar2;
  ModelDefinitionRecordPrefix *candidateDefinition;
  
  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 0x300;
  while ((arg4 = *registryCursor, arg4 == (ModelDefinitionRecordPrefix *)0x0 ||
         (arg4->definitionId != definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)arg4,g_PackageLastErrorPath);
      MVar1.carry = true;
      MVar1.modelDefinition = (ModelDefinitionRecordPrefix *)0x3e;
      return MVar1;
    }
  }
  MVar2.carry = false;
  MVar2.modelDefinition = arg4;
  return MVar2;
}

