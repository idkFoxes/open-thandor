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
ModelDefinitionRecordPrefix *
ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList)

{
  PckModelDefinitionIdCatalog PVar1;
  ModelDefinitionRecordPrefix *pMVar2;
  int iVar3;
  int extraout_ECX;
  PckModelDefinitionIdCatalog extraout_EDX;
  PckModelDefinitionIdCatalog definitionId;
  bool bVar4;
  
  iVar3 = 8;
  definitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
  do {
    bVar4 = false;
    if (*(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20) != 0) {
      PVar1 = ModelDefinition_IsFactionTechnologyUnlockedCf
                        (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                         *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20));
      iVar3 = extraout_ECX;
      definitionId = extraout_EDX;
      if (!bVar4) {
        definitionId = PVar1;
      }
    }
    linkedDefinitionList = linkedDefinitionList + 4;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  pMVar2 = ModelDefinitionRegistry_FindByIdWithErrorCf(definitionId);
  return pMVar2;
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
void __fastcall
ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
          (int param_1,undefined4 param_2,FactionRuntimeIndex factionIndex,
          ModelDefinitionHierarchyNodeAddress32 definitionNode)

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
ModelTechnologyHierarchyCfVolatileContinuityResult
ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  undefined4 in_EAX;
  int iVar1;
  undefined4 in_ECX;
  dword in_EDX;
  int unaff_EBP;
  uint unaff_ESI;
  bool bVar2;
  ModelTechnologyHierarchyCfVolatileContinuityResult MVar3;
  ModelTechnologyHierarchyCfVolatileContinuityResult MVar4;
  
  iVar1 = *(int *)(definitionNode + 0xc);
  bVar2 = 0xffaf05df < (uint)(factionIndex * 0x740);
  do {
    iVar1 = ModelDefinition_IsFactionTechnologyUnlockedCf
                      (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                       *(PckModelDefinitionIdCatalog *)(iVar1 + 0x20));
    if (bVar2) {
      MVar4.preservedEcxCallerValue = in_ECX;
      MVar4.preservedEaxDefinitionNodeAddress = in_EAX;
      MVar4.preservedEdxCallerValue = in_EDX;
      return MVar4;
    }
    iVar1 = iVar1 + 1;
    while( true ) {
      if (unaff_EBP != 0) break;
      iVar1 = iVar1 + -1;
      if (iVar1 == 0) {
        MVar3.preservedEcxCallerValue = in_ECX;
        MVar3.preservedEaxDefinitionNodeAddress = in_EAX;
        MVar3.preservedEdxCallerValue = in_EDX;
        return MVar3;
      }
    }
    iVar1 = *(int *)(unaff_ESI + 0xc);
    bVar2 = 0xfffffffb < unaff_ESI;
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
dword ModelAsset_PrepareRecords(ModelAssetHeader *asset)

{
  dword registrationStatusCode;
  int extraout_ECX;
  ModelDefinitionRecordPrefix *definitionCursor;
  bool bVar1;
  undefined8 uVar2;
  
  registrationStatusCode = 0x3d;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_MDL) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_MDL_0008000A)) {
    definitionCursor = (ModelDefinitionRecordPrefix *)(asset + 1);
    bVar1 = false;
    if ((asset->recordCountHeader).recordCount != 0) {
      do {
        uVar2 = ModelDefinition_RegisterAndResolveReferencesCf
                          ((ModelDefinitionResolvePhaseView280 *)definitionCursor,asset);
        registrationStatusCode = (dword)uVar2;
        if (bVar1) {
          return registrationStatusCode;
        }
        bVar1 = CARRY4((uint)definitionCursor,definitionCursor->byteSize);
        definitionCursor =
             (ModelDefinitionRecordPrefix *)
             ((int)&definitionCursor->byteSize + definitionCursor->byteSize);
      } while (extraout_ECX != 1);
    }
    return registrationStatusCode;
  }
  return registrationStatusCode;
}

/* Address: 0x004BE670.
   Ownership: assets/model/definitions.
   Purpose: Scans the model lookup table at offsets +0xE4/+0xE8 for packed key (groupIndex << 4) | itemIndex. On
   success it returns the three payload dwords from the matching 0x10-byte entry in EAX, ECX, and EDX with CF
   clear; on failure it zeros those registers and sets CF. Saved ids, relocated pointers, attachment selectors, and
   runtime class ids remain separate. Key index and key class remain separate 32-bit domains. Typed parameters: p0
   keyIndex→ModelLookupKeyIndex_V338, p1 keyClass→ModelLookupKeyClass_V338.
*/
undefined8
ModelLookupTable_FindPackedKeyEntryRegsCf
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResourceHitTestAndRenderView210 *modelDefinition)

{
  int entriesRemaining;
  uint *packedKeyEntryCursor;
  
  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  packedKeyEntryCursor =
       (uint *)(modelDefinition->reserved00_AF + modelDefinition->packedLookupTableRelativeOffset);
  while( true ) {
    if (entriesRemaining == 0) {
      return 0;
    }
    if ((keyClass | keyIndex << 4) == *packedKeyEntryCursor) break;
    packedKeyEntryCursor = packedKeyEntryCursor + 4;
    entriesRemaining = entriesRemaining - 1;
  }
  return CONCAT44(packedKeyEntryCursor[3],packedKeyEntryCursor[1]);
}

/* Address: 0x004BE6F0.
   Ownership: assets/model/definitions.
   Purpose: Scans the same 0x10-byte model lookup table for packed key (groupIndex << 4) | itemIndex. CF is clear
   when a matching entry exists and set when the table is empty or no key matches. Saved ids, relocated pointers,
   attachment selectors, and runtime class ids remain separate. Key index and key class remain separate 32-bit
   domains. Typed parameters: p0 keyIndex→ModelLookupKeyIndex_V338, p1 keyClass→ModelLookupKeyClass_V338.
*/
void ModelLookupTable_ContainsPackedKeyCf
               (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
               ModelResourceHitTestAndRenderView210 *modelDefinition)

{
  ModelPackedPointRecord *packedKeyEntryCursor;
  int entriesRemaining;
  
  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  packedKeyEntryCursor =
       (ModelPackedPointRecord *)
       (modelDefinition->reserved00_AF + modelDefinition->packedLookupTableRelativeOffset);
  while( true ) {
    if (entriesRemaining == 0) {
      return;
    }
    if ((keyClass | keyIndex << 4) == packedKeyEntryCursor->packedLookupKey) break;
    packedKeyEntryCursor = packedKeyEntryCursor + 1;
    entriesRemaining = entriesRemaining - 1;
  }
  return;
}

/* Address: 0x0050AEA0.
   Ownership: assets/model/definitions.
   Purpose: Intersects the shared model-space ray with one exact 0x40-byte triangle record. EAX is Q12 distance; CF
   set means hit, CF clear means no hit.
*/
Q12 ModelMesh_IntersectTriangleRayDistanceCf(ModelRaycastTriangleDescriptor *triangle)

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
  Q12 QVar15;
  int iVar16;
  int iVar17;
  uint uVar18;
  uint uVar19;
  int iVar20;
  int iVar21;
  int iVar22;
  int iVar23;
  int iVar24;
  uint uVar25;
  int iVar26;
  
  iVar14 = triangle->planeNormalX << 0x10;
  iVar17 = triangle->planeNormalY << 0x10;
  iVar20 = triangle->planeNormalZ << 0x10;
  pGVar5 = triangle->vertex0;
  pGVar6 = triangle->vertex1;
  pGVar7 = triangle->vertex2;
  uVar8 = (longlong)iVar17 *
          (longlong)((pGVar5->y * 2 + pGVar6->y + pGVar7->y >> 2) - g_ModelRaycastLocalOriginY) +
          (longlong)((pGVar5->x * 2 + pGVar6->x + pGVar7->x >> 2) - g_ModelRaycastLocalOriginX) *
          (longlong)iVar14 +
          (longlong)((pGVar5->z * 2 + pGVar6->z + pGVar7->z >> 2) - g_ModelRaycastLocalOriginZ) *
          (longlong)iVar20;
  uVar25 = (uint)(uVar8 >> 0x20);
  lVar9 = (longlong)g_ModelRaycastLocalDirectionYQ28 * (longlong)iVar17 +
          (longlong)g_ModelRaycastLocalDirectionXQ28 * (longlong)iVar14 +
          (longlong)g_ModelRaycastLocalDirectionZQ28 * (longlong)iVar20;
  uVar18 = (int)((ulonglong)lVar9 >> 0x20) << 4 | (uint)lVar9 >> 0x1c;
  uVar19 = g_ModelRaycastMaximumDistance;
  if (uVar18 != 0) {
    iVar21 = (int)((ulonglong)((longlong)(int)g_ModelRaycastMaximumDistance * (longlong)(int)uVar18)
                  >> 0x20);
    QVar15 = (Q12)((longlong)(int)g_ModelRaycastMaximumDistance * (longlong)(int)uVar18);
    iVar26 = (int)uVar25 >> 1;
    if ((longlong)uVar8 < 0) {
      if ((int)uVar25 < iVar21) {
        return QVar15;
      }
      if ((iVar26 <= (int)-uVar18) && (iVar26 <= (int)uVar18)) {
        return (Q12)uVar8;
      }
    }
    else {
      if (iVar21 < (int)uVar25) {
        return QVar15;
      }
      if (((int)-uVar18 <= iVar26) && ((int)uVar18 <= iVar26)) {
        return (Q12)uVar8;
      }
    }
    iVar16 = (int)((longlong)((ulonglong)uVar25 << 0x20 | uVar8 & 0xffffffff) /
                  (longlong)(int)uVar18);
    pGVar5 = triangle->vertex0;
    iVar2 = -pGVar5->x;
    iVar22 = ((int)((ulonglong)((longlong)iVar16 * (longlong)g_ModelRaycastLocalDirectionXQ28) >>
                   0x20) << 4 |
             (uint)((longlong)iVar16 * (longlong)g_ModelRaycastLocalDirectionXQ28) >> 0x1c) +
             g_ModelRaycastLocalOriginX + iVar2;
    iVar3 = -pGVar5->y;
    iVar23 = ((int)((ulonglong)((longlong)g_ModelRaycastLocalDirectionYQ28 * (longlong)iVar16) >>
                   0x20) << 4 |
             (uint)((longlong)g_ModelRaycastLocalDirectionYQ28 * (longlong)iVar16) >> 0x1c) +
             g_ModelRaycastLocalOriginY + iVar3;
    iVar4 = -pGVar5->z;
    iVar24 = ((int)((ulonglong)((longlong)g_ModelRaycastLocalDirectionZQ28 * (longlong)iVar16) >>
                   0x20) << 4 |
             (uint)((longlong)g_ModelRaycastLocalDirectionZQ28 * (longlong)iVar16) >> 0x1c) +
             g_ModelRaycastLocalOriginZ + iVar4;
    pGVar5 = triangle->vertex1;
    pGVar6 = triangle->vertex2;
    iVar21 = iVar2 + pGVar5->x;
    iVar26 = iVar3 + pGVar5->y;
    iVar1 = iVar4 + pGVar5->z;
    iVar2 = iVar2 + pGVar6->x;
    iVar3 = iVar3 + pGVar6->y;
    iVar4 = iVar4 + pGVar6->z;
    lVar9 = (longlong)iVar17 * (longlong)iVar1 - (longlong)iVar20 * (longlong)iVar26;
    uVar19 = (int)((ulonglong)lVar9 >> 0x20) << 4 | (uint)lVar9 >> 0x1c;
    lVar9 = (longlong)iVar20 * (longlong)iVar21 - (longlong)iVar14 * (longlong)iVar1;
    uVar18 = (int)((ulonglong)lVar9 >> 0x20) << 4 | (uint)lVar9 >> 0x1c;
    lVar9 = (longlong)iVar14 * (longlong)iVar26 - (longlong)iVar17 * (longlong)iVar21;
    uVar25 = (int)((ulonglong)lVar9 >> 0x20) << 4 | (uint)lVar9 >> 0x1c;
    lVar9 = (longlong)iVar23 * (longlong)(int)uVar18 + (longlong)iVar22 * (longlong)(int)uVar19 +
            (longlong)iVar24 * (longlong)(int)uVar25;
    lVar10 = (longlong)iVar3 * (longlong)(int)uVar18 + (longlong)(int)uVar19 * (longlong)iVar2 +
             (longlong)iVar4 * (longlong)(int)uVar25;
    iVar21 = (int)((ulonglong)lVar10 >> 0x20);
    lVar11 = (longlong)iVar17 * (longlong)iVar24 - (longlong)iVar20 * (longlong)iVar23;
    lVar12 = (longlong)iVar20 * (longlong)iVar22 - (longlong)iVar14 * (longlong)iVar24;
    lVar13 = (longlong)iVar14 * (longlong)iVar23 - (longlong)iVar17 * (longlong)iVar22;
    lVar11 = (longlong)iVar3 *
             (longlong)(int)((int)((ulonglong)lVar12 >> 0x20) << 4 | (uint)lVar12 >> 0x1c) +
             (longlong)iVar2 *
             (longlong)(int)((int)((ulonglong)lVar11 >> 0x20) << 4 | (uint)lVar11 >> 0x1c) +
             (longlong)iVar4 *
             (longlong)(int)((int)((ulonglong)lVar13 >> 0x20) << 4 | (uint)lVar13 >> 0x1c);
    uVar19 = (uint)lVar11;
    if (lVar10 < 0) {
      if (((lVar11 < 0) && (lVar9 < 0)) &&
         (uVar19 = (uint)(lVar11 + lVar9),
         (int)((iVar21 - (int)((ulonglong)(lVar11 + lVar9) >> 0x20)) - (uint)((uint)lVar10 < uVar19)
              ) < 0)) {
        return iVar16;
      }
    }
    else if (((-1 < lVar11) && (-1 < lVar9)) &&
            (uVar19 = (uint)(lVar11 + lVar9),
            -1 < (int)((iVar21 - (int)((ulonglong)(lVar11 + lVar9) >> 0x20)) -
                      (uint)((uint)lVar10 < uVar19)))) {
      return iVar16;
    }
  }
  return uVar19;
}

/* Address: 0x005289C0.
   Ownership: assets/model/definitions.
   Purpose: Finds a model definition by identifier in the 768-slot registry and returns its runtime descriptor at
   record +0x188; error 0x3E reports a miss.
*/
void * ModelDefinitionRegistry_FindBuildMetricTupleByIdCf(PckModelDefinitionIdCatalog definitionId)

{
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  ModelDefinitionRuntimeSemanticView280 *candidateDefinition;
  
  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 0x300;
  while ((candidateDefinition = (ModelDefinitionRuntimeSemanticView280 *)*registryCursor,
         candidateDefinition == (ModelDefinitionRuntimeSemanticView280 *)0x0 ||
         (definitionId != candidateDefinition->definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
      return (void *)0x3e;
    }
  }
  return candidateDefinition->buildMetricTuple188;
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
PckModelDefinitionIdCatalog
ModelDefinition_SelectFactionUnlockedLinkedIdCf
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList)

{
  PckModelDefinitionIdCatalog PVar1;
  int iVar2;
  int extraout_ECX;
  PckModelDefinitionIdCatalog extraout_EDX;
  PckModelDefinitionIdCatalog PVar3;
  bool bVar4;
  
  iVar2 = 8;
  PVar3 = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
  do {
    bVar4 = false;
    if (*(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20) != 0) {
      PVar1 = ModelDefinition_IsFactionTechnologyUnlockedCf
                        (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                         *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20));
      iVar2 = extraout_ECX;
      PVar3 = extraout_EDX;
      if (!bVar4) {
        PVar3 = PVar1;
      }
    }
    linkedDefinitionList = linkedDefinitionList + 4;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return PVar3;
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
undefined8
ModelDefinition_RegisterAndResolveReferencesCf
          (ModelDefinitionResolvePhaseView280 *definition,ModelAssetHeader *asset)

{
  dword dVar1;
  dword dVar2;
  dword resolverStatusOrSentinel;
  SpriteAssetHeader *loadedSpriteAsset;
  SpriteAssetHeader *pSVar3;
  ShotDefinition *resolvedShotDefinition2C;
  EffectDefinition *resolvedEffectDefinition80ToB8;
  ShotDefinition *resolvedShotDefinition168;
  EffectDefinition *resolvedEffectDefinitionTail;
  dword gridDerivedScalarCarrier;
  MdlChildCount extraout_ECX;
  MdlChildCount extraout_ECX_00;
  MdlChildCount MVar4;
  undefined4 in_EDX;
  int extraout_EDX;
  int iVar5;
  int iVar6;
  int extraout_EDX_00;
  int extraout_EDX_01;
  ModelDefinitionRecordPrefix **registrySlotCursor;
  MdlSerializedNodeHeader38 *serializedNodeCursor;
  undefined1 in_CF;
  bool bVar7;
  
  registrySlotCursor = g_ModelDefinitionRegistry;
  ModelDefinitionRegistry_FindByIdWithErrorCf(definition->definitionId);
  iVar5 = extraout_EDX;
  if ((bool)in_CF) {
    do {
      if (*registrySlotCursor == (ModelDefinitionRecordPrefix *)0x0) {
        dVar1 = definition->serializedNodeOffsetOrPointer64;
        *registrySlotCursor = (ModelDefinitionRecordPrefix *)definition;
        bVar7 = false;
        if (dVar1 == 0) goto ModelDefinition_ResolveShotAndEffectReferences;
        definition->serializedNodeOffsetOrPointer64 =
             (dword)((asset->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                    (definition->serializedNodeOffsetOrPointer64 - 0x28));
        serializedNodeCursor =
             (MdlSerializedNodeHeader38 *)
             ((asset->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
             (dVar1 - 0x28));
        iVar5 = 0;
        goto ModelDefinition_LoadOrReuseSpriteReference;
      }
      registrySlotCursor = registrySlotCursor + 1;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x300,g_PackageLastErrorPath);
    resolverStatusOrSentinel = 0x3f;
  }
  else {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    resolverStatusOrSentinel = MODEL_DEFINITION_REFERENCE_FAILURE_SENTINEL_0x4B;
  }
ModelDefinition_ReturnReferenceResolutionResult:
  return CONCAT44(in_EDX,resolverStatusOrSentinel);
ModelDefinition_LoadOrReuseSpriteReference:
  MVar4 = serializedNodeCursor->childCount;
  iVar6 = 0;
  bVar7 = false;
  if ((serializedNodeCursor->nodeFlags & 0xf) == 0) {
    resolverStatusOrSentinel =
         WidePath_SetExtensionCode(0x727073,(word *)(serializedNodeCursor + 1));
    if ((bVar7) ||
       (loadedSpriteAsset = Package_LoadEntry((word *)(serializedNodeCursor + 1)),
       resolverStatusOrSentinel = (dword)loadedSpriteAsset, bVar7))
    goto ModelDefinition_ReturnReferenceResolutionResult;
    pSVar3 = SpriteAssetRegistry_FindById((loadedSpriteAsset->registryHeader).registryId);
    bVar7 = false;
    if (pSVar3 == (SpriteAssetHeader *)0x0) {
      serializedNodeCursor->ownedNestedResourcePresent =
           serializedNodeCursor->ownedNestedResourcePresent + 1;
      (serializedNodeCursor->spriteAssetReference).spriteAsset = loadedSpriteAsset;
      resolverStatusOrSentinel = (dword)SpriteAsset_RegisterAndRelocatePointers(loadedSpriteAsset);
      MVar4 = extraout_ECX;
      iVar6 = extraout_EDX_00;
      if (bVar7) goto ModelDefinition_ReturnReferenceResolutionResult;
    }
    else {
      (serializedNodeCursor->spriteAssetReference).spriteAsset = pSVar3;
      Resource_Release(loadedSpriteAsset);
      MVar4 = extraout_ECX_00;
      iVar6 = extraout_EDX_01;
    }
  }
  iVar5 = iVar5 + 1;
  while (MVar4 == 0) {
    bVar7 = (undefined1 *)0xfffffff3 < &stack0xffffffd8;
    iVar5 = iVar5 + -1;
    if (iVar5 == 0) goto ModelDefinition_ResolveShotAndEffectReferences;
  }
  serializedNodeCursor->childSerializedOffsets[iVar6] =
       (SerializedRelativeByteOffset)
       ((asset->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
       (serializedNodeCursor->childSerializedOffsets[iVar6] - 0x28));
  serializedNodeCursor =
       (MdlSerializedNodeHeader38 *)serializedNodeCursor->childSerializedOffsets[iVar6];
  goto ModelDefinition_LoadOrReuseSpriteReference;
ModelDefinition_ResolveShotAndEffectReferences:
  resolvedShotDefinition2C =
       ShotDefinitionRegistry_FindByIdWithErrorCf
                 ((PckShotDefinitionIdCatalog)definition->shotDefinitionReference2C);
  resolverStatusOrSentinel = (dword)resolvedShotDefinition2C;
  if (!bVar7) {
    definition->shotDefinitionReference2C = resolvedShotDefinition2C;
    resolvedEffectDefinition80ToB8 =
         EffectDefinitionRegistry_FindByIdWithErrorCf
                   ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference80);
    resolverStatusOrSentinel = (dword)resolvedEffectDefinition80ToB8;
    if (!bVar7) {
      definition->effectDefinitionReference80 = resolvedEffectDefinition80ToB8;
      resolverStatusOrSentinel =
           (dword)EffectDefinitionRegistry_FindByIdWithErrorCf
                            ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference88);
      if (!bVar7) {
        definition->effectDefinitionReference88 = (EffectDefinition *)resolverStatusOrSentinel;
        resolverStatusOrSentinel =
             (dword)EffectDefinitionRegistry_FindByIdWithErrorCf
                              ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference90
                              );
        if (!bVar7) {
          definition->effectDefinitionReference90 = (EffectDefinition *)resolverStatusOrSentinel;
          resolverStatusOrSentinel =
               (dword)EffectDefinitionRegistry_FindByIdWithErrorCf
                                ((PckEffectDefinitionIdCatalog)
                                 definition->effectDefinitionReference98);
          if (!bVar7) {
            definition->effectDefinitionReference98 = (EffectDefinition *)resolverStatusOrSentinel;
            resolverStatusOrSentinel =
                 (dword)EffectDefinitionRegistry_FindByIdWithErrorCf
                                  ((PckEffectDefinitionIdCatalog)
                                   definition->effectDefinitionReferenceA0);
            if (!bVar7) {
              definition->effectDefinitionReferenceA0 = (EffectDefinition *)resolverStatusOrSentinel
              ;
              resolverStatusOrSentinel =
                   (dword)EffectDefinitionRegistry_FindByIdWithErrorCf
                                    ((PckEffectDefinitionIdCatalog)
                                     definition->effectDefinitionReferenceA8);
              if (!bVar7) {
                definition->effectDefinitionReferenceA8 =
                     (EffectDefinition *)resolverStatusOrSentinel;
                resolverStatusOrSentinel =
                     (dword)EffectDefinitionRegistry_FindByIdWithErrorCf
                                      ((PckEffectDefinitionIdCatalog)
                                       definition->effectDefinitionReferenceB0);
                if (!bVar7) {
                  definition->effectDefinitionReferenceB0 =
                       (EffectDefinition *)resolverStatusOrSentinel;
                  resolverStatusOrSentinel =
                       (dword)EffectDefinitionRegistry_FindByIdWithErrorCf
                                        ((PckEffectDefinitionIdCatalog)
                                         definition->effectDefinitionReferenceB8);
                  if (!bVar7) {
                    definition->effectDefinitionReferenceB8 =
                         (EffectDefinition *)resolverStatusOrSentinel;
                    bVar7 = definition->shotDefinitionReference168 != (ShotDefinition *)0xffffffff;
                    if (definition->shotDefinitionReference168 != (ShotDefinition *)0xffffffff) {
                      resolvedShotDefinition168 =
                           ShotDefinitionRegistry_FindByIdWithErrorCf
                                     ((PckShotDefinitionIdCatalog)
                                      definition->shotDefinitionReference168);
                      resolverStatusOrSentinel = (dword)resolvedShotDefinition168;
                      if (bVar7) goto ModelDefinition_ReturnReferenceResolutionResult;
                      definition->shotDefinitionReference168 = resolvedShotDefinition168;
                      bVar7 = false;
                    }
                    resolvedEffectDefinitionTail =
                         EffectDefinitionRegistry_FindByIdWithErrorCf
                                   ((PckEffectDefinitionIdCatalog)
                                    definition->effectDefinitionReference174);
                    resolverStatusOrSentinel = (dword)resolvedEffectDefinitionTail;
                    if (!bVar7) {
                      definition->effectDefinitionReference174 = resolvedEffectDefinitionTail;
                      resolverStatusOrSentinel =
                           (dword)EffectDefinitionRegistry_FindByIdWithErrorCf
                                            ((PckEffectDefinitionIdCatalog)
                                             definition->effectDefinitionReference58);
                      if (!bVar7) {
                        definition->effectDefinitionReference58 =
                             (EffectDefinition *)resolverStatusOrSentinel;
                        resolverStatusOrSentinel =
                             (dword)EffectDefinitionRegistry_FindByIdWithErrorCf
                                              ((PckEffectDefinitionIdCatalog)
                                               definition->effectDefinitionReference190);
                        if (!bVar7) {
                          definition->effectDefinitionReference190 =
                               (EffectDefinition *)resolverStatusOrSentinel;
                          resolverStatusOrSentinel =
                               (dword)EffectDefinitionRegistry_FindByIdWithErrorCf
                                                ((PckEffectDefinitionIdCatalog)
                                                 definition->effectDefinitionReference254);
                          if (!bVar7) {
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
                              if (definition->placementCallbackSelector278 == 1) {
                                resolverStatusOrSentinel =
                                     (&g_GridTerrainClassBit24MaxWaterSurfaceDelta)[dVar1];
                                dVar1 = (&g_GridTerrainClassBit28MaxTriangle0NormalAngleHigh16)
                                        [dVar1];
                                *(dword *)(definition->reserved0C0_0DB + 0xc) =
                                     resolverStatusOrSentinel;
                                definition->runtimeValue24 = dVar1;
                              }
                              else if (definition->placementCallbackSelector278 == 4) {
                                resolverStatusOrSentinel =
                                     *(dword *)(&g_ModelTraversalClass4SecondaryThresholdTable3 +
                                               (dVar1 - 1) * 4);
                                definition->runtimeValue24 =
                                     (&g_GridTerrainClassBit25MaxSelectedNormalAngleHigh16)
                                     [dVar1 - 1];
                                definition->runtimeValue268 = resolverStatusOrSentinel;
                              }
                              else {
                                iVar5 = dVar1 - 4;
                                resolverStatusOrSentinel =
                                     (&g_GridTerrainClassBit28MinWaterSurfaceDelta)[iVar5];
                                dVar1 = (&g_GridTerrainClassBit28MaxTriangle0NormalAngleHigh16)
                                        [iVar5];
                                dVar2 = *(dword *)(&g_ModelTraversalFallbackSecondaryThresholdTable3
                                                  + iVar5 * 4);
                                definition->runtimeValue198 = resolverStatusOrSentinel;
                                definition->runtimeValue24 = dVar1;
                                definition->runtimeValue268 = dVar2;
                              }
                            }
                            return CONCAT44(in_EDX,resolverStatusOrSentinel);
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
void ModelDefinition_UnlockLinkedTechnologyForFactionCf
               (FactionRuntimeIndex factionIndex,PckModelDefinitionIdCatalog modelDefinitionId)

{
  ModelDefinitionRecordPrefix *pMVar1;
  undefined1 in_CF;
  
  pMVar1 = ModelDefinitionRegistry_FindByIdWithErrorCf(modelDefinitionId);
  if (!(bool)in_CF) {
    Technology_UnlockForFaction(0,0,pMVar1[0x25].definitionId,factionIndex);
  }
  return;
}

/* Address: 0x0052AD90.
   Ownership: assets/model/definitions.
   Purpose: CF clear means the bit is unlocked; lookup failure or a clear bit returns CF set while preserving EAX.
   Local calls: ModelDefinitionRegistry_FindByIdWithErrorCf.
*/
undefined4
ModelDefinition_IsFactionTechnologyUnlockedCf
          (dword *factionTechnologyMasks,PckModelDefinitionIdCatalog modelDefinitionId)

{
  undefined4 in_EAX;
  ModelDefinitionRecordPrefix *modelDefinition1;
  undefined1 in_CF;
  
  modelDefinition1 = ModelDefinitionRegistry_FindByIdWithErrorCf(modelDefinitionId);
  if ((!(bool)in_CF) &&
     ((factionTechnologyMasks[modelDefinition1[0x25].flags >> 5] &
      1 << ((byte)modelDefinition1[0x25].flags & 0x1f)) != 0)) {
    return in_EAX;
  }
  return in_EAX;
}

/* Address: 0x00528E20.
   Ownership: assets/model/definitions.
   Purpose: Scans the 768-slot model-definition registry. On a miss it formats the unresolved identifier into
   g_PackageLastErrorPath and returns error 0x3E with CF set.
*/
ModelDefinitionRecordPrefix *
ModelDefinitionRegistry_FindByIdWithErrorCf(PckModelDefinitionIdCatalog definitionId)

{
  ModelDefinitionRecordPrefix *arg4;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  ModelDefinitionRecordPrefix *candidateDefinition;
  
  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 0x300;
  while( true ) {
    arg4 = *registryCursor;
    if ((arg4 != (ModelDefinitionRecordPrefix *)0x0) && (arg4->definitionId == definitionId)) break;
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)arg4,g_PackageLastErrorPath);
      return (ModelDefinitionRecordPrefix *)0x3e;
    }
  }
  return arg4;
}
