/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/model/definitions.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/model/definitions.h>
#include <thandor/thandor.h>

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
  int linkedSlotsRemaining;
  PckModelDefinitionIdCatalog definitionId;
  bool technologyLocked;
  ModelDefinitionLookupEaxCf5 lookupResult;
  
  linkedSlotsRemaining = 8;
  definitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
  do {
    modelDefinitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
    if (modelDefinitionId != 0) {
      technologyLocked = ModelDefinition_IsFactionTechnologyUnlockedCf
                        (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                         modelDefinitionId);
      if (!technologyLocked) {
        definitionId = modelDefinitionId;
      }
    }
    linkedDefinitionList = linkedDefinitionList + 4;
    linkedSlotsRemaining = linkedSlotsRemaining + -1;
  } while (linkedSlotsRemaining != 0);
  lookupResult = ModelDefinitionRegistry_FindByIdWithErrorCf(definitionId);
  return THANDOR_BITCAST(qword, ModelDefinitionLookupEaxCf5, ((THANDOR_BITCAST(ModelDefinitionLookupEaxCf5, qword, lookupResult) & 0xFFFFFFFFFFull) & 0xffffffff));
}


static void ModelDefinitionHierarchy_UnlockFrom(FactionRuntimeIndex factionIndex,byte *node)
{
  dword i;
  ModelDefinition_UnlockLinkedTechnologyForFactionCf
            (factionIndex,ModelDefinition_SelectFactionUnlockedLinkedIdCf
                                    (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node));
  for (i = 0; i < *(dword *)(node + 8); i++) {
    ModelDefinitionHierarchy_UnlockFrom(factionIndex,*(byte **)(node + 0xc + i * 4));
  }
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
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  ModelDefinitionHierarchy_UnlockFrom(factionIndex,*(byte **)(uintptr_t)(definitionNode + 0xc));
}


/* True (CF set) as soon as one node's technology reports CF from ModelDefinition_IsFactionTechnologyUnlockedCf. */
static bool ModelDefinitionHierarchy_AnyTechnologyCfFrom(dword *technologyMasks,byte *node)
{
  dword i;
  if (ModelDefinition_IsFactionTechnologyUnlockedCf
                (technologyMasks,*(PckModelDefinitionIdCatalog *)(node + 0x20))) {
    return true;
  }
  for (i = 0; i < *(dword *)(node + 8); i++) {
    if (ModelDefinitionHierarchy_AnyTechnologyCfFrom(technologyMasks,*(byte **)(node + 0xc + i * 4))) {
      return true;
    }
  }
  return false;
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
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  return ModelDefinitionHierarchy_AnyTechnologyCfFrom
                   (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                    *(byte **)(uintptr_t)(definitionNode + 0xc));
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
  AssetRecordCount recordsRemaining;
  ModelDefinitionResolvePhaseView280 *definition;
  ModelDefinitionRecordPrefix *definitionCursor;
  StatusValueEaxCf5 registrationResult;
  StatusValueEaxCf5 failureResult;
  
  registrationStatusCode = 0x3d;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_MDL) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_MDL_0008000A)) {
    recordsRemaining = (asset->recordCountHeader).recordCount;
    definition = (ModelDefinitionResolvePhaseView280 *)(asset + 1);
    while( true ) {
      if (recordsRemaining == 0) {
        registrationResult.carry = false;
        registrationResult.valueOrError = registrationStatusCode;
        return registrationResult;
      }
      registrationResult = ModelDefinition_RegisterAndResolveReferencesCf(definition,asset);
      registrationStatusCode = registrationResult.valueOrError;
      if (registrationResult.carry) break;
      definition = (ModelDefinitionResolvePhaseView280 *)
                   (definition->reserved010_017 + (definition->byteSize - 0x10));
      recordsRemaining = recordsRemaining - 1;
    }
  }
  failureResult.carry = true;
  failureResult.valueOrError = registrationStatusCode;
  return failureResult;
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
  ModelLookupPayloadEaxEcxEdxCf13 payloadResult;
  
  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  packedKeyEntryCursor =
       (uint *)(modelDefinition->reserved00_AF + modelDefinition->packedLookupTableRelativeOffset);
  while( true ) {
    if (entriesRemaining == 0) {
      /* not found: EAX, ECX and EDX zero, CF set (the decompiled 13-byte constant was cut to
         64 bits and lost the CF byte) */
      payloadResult.payloadEax = 0;
      payloadResult.payloadEcx = 0;
      payloadResult.payloadEdx = 0;
      payloadResult.carry = true;
      return payloadResult;
    }
    if ((keyClass | keyIndex << 4) == *packedKeyEntryCursor) break;
    packedKeyEntryCursor = packedKeyEntryCursor + 4;
    entriesRemaining = entriesRemaining - 1;
  }
  payloadResult.carry = false;
  THANDOR_WRITE_PART(payloadResult, 0, 12, *(undefined1 (*) [12])(packedKeyEntryCursor + 1));
  return payloadResult;
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
  ModelLookupEntryEaxCf5 foundResult;
  ModelLookupEntryEaxCf5 notFoundResult;
  
  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  packedKeyEntryCursor =
       (ModelPackedPointRecord *)
       (modelDefinition->reserved00_AF + modelDefinition->packedLookupTableRelativeOffset);
  while( true ) {
    if (entriesRemaining == 0) {
      notFoundResult.carry = true;
      notFoundResult.entry = packedKeyEntryCursor;
      return notFoundResult;
    }
    if ((keyClass | keyIndex << 4) == packedKeyEntryCursor->packedLookupKey) break;
    packedKeyEntryCursor = packedKeyEntryCursor + 1;
    entriesRemaining = entriesRemaining - 1;
  }
  foundResult.carry = false;
  foundResult.entry = packedKeyEntryCursor;
  return foundResult;
}


/* Address: 0x0050AEA0.
   Ownership: assets/model/definitions.
   Purpose: Intersects the shared model-space ray with one exact 0x40-byte triangle record. EAX is Q12 distance; CF
   set means hit, CF clear means no hit.
*/
TerrainDistanceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelMesh_IntersectTriangleRayDistanceCf(ModelRaycastTriangleDescriptor *triangle)

{
  int edge1Z;
  int negVertex0XOrEdge2X;
  int negVertex0YOrEdge2Y;
  int negVertex0ZOrEdge2Z;
  GraphicsFixedVec3 *vertexA;
  GraphicsFixedVec3 *vertexB;
  GraphicsFixedVec3 *vertexC;
  ulonglong planeOffsetDot;
  longlong productScratch;
  longlong edge2Dot;
  longlong hitCrossXOrEdge2HitDot;
  longlong hitCrossY;
  longlong hitCrossZ;
  int normalX;
  uint distanceOrCrossX;
  int normalY;
  uint directionDotOrCrossY;
  int normalZ;
  int scaledHighOrEdge1X;
  int hitRelativeX;
  int hitRelativeY;
  int hitRelativeZ;
  uint offsetHighOrCrossZ;
  int halfOffsetOrEdge1Y;
  TerrainDistanceEaxCf5 missResult;
  TerrainDistanceEaxCf5 hitResult;
  
  normalX = triangle->planeNormalX << 0x10;
  normalY = triangle->planeNormalY << 0x10;
  normalZ = triangle->planeNormalZ << 0x10;
  vertexA = triangle->vertex0;
  vertexB = triangle->vertex1;
  vertexC = triangle->vertex2;
  planeOffsetDot = (longlong)normalY *
          (longlong)((vertexA->y * 2 + vertexB->y + vertexC->y >> 2) - g_ModelRaycastLocalOriginY) +
          (longlong)((vertexA->x * 2 + vertexB->x + vertexC->x >> 2) - g_ModelRaycastLocalOriginX) *
          (longlong)normalX +
          (longlong)((vertexA->z * 2 + vertexB->z + vertexC->z >> 2) - g_ModelRaycastLocalOriginZ) *
          (longlong)normalZ;
  offsetHighOrCrossZ = (uint)(planeOffsetDot >> 0x20);
  productScratch = (longlong)(int)g_ModelRaycastLocalDirectionYQ28 * (longlong)normalY +
          (longlong)(int)g_ModelRaycastLocalDirectionXQ28 * (longlong)normalX +
          (longlong)(int)g_ModelRaycastLocalDirectionZQ28 * (longlong)normalZ;
  directionDotOrCrossY = (int)((ulonglong)productScratch >> 0x20) << 4 | (uint)productScratch >> 0x1c;
  distanceOrCrossX = g_ModelRaycastMaximumDistance;
  if (directionDotOrCrossY != 0) {
    scaledHighOrEdge1X = (int)((ulonglong)((longlong)(int)g_ModelRaycastMaximumDistance * (longlong)(int)directionDotOrCrossY)
                  >> 0x20);
    distanceOrCrossX = (uint)((longlong)(int)g_ModelRaycastMaximumDistance * (longlong)(int)directionDotOrCrossY);
    halfOffsetOrEdge1Y = (int)offsetHighOrCrossZ >> 1;
    if ((longlong)planeOffsetDot < 0) {
      if (((int)offsetHighOrCrossZ < scaledHighOrEdge1X) ||
         ((halfOffsetOrEdge1Y <= (int)-directionDotOrCrossY && (distanceOrCrossX = (uint)planeOffsetDot, halfOffsetOrEdge1Y <= (int)directionDotOrCrossY))))
      goto LAB_0050afa0;
    }
    else if ((scaledHighOrEdge1X < (int)offsetHighOrCrossZ) ||
            (((int)-directionDotOrCrossY <= halfOffsetOrEdge1Y && (distanceOrCrossX = (uint)planeOffsetDot, (int)directionDotOrCrossY <= halfOffsetOrEdge1Y))))
    goto LAB_0050afa0;
    hitResult.distanceQ12 =
         (int)((longlong)((ulonglong)offsetHighOrCrossZ << 0x20 | planeOffsetDot & 0xffffffff) / (longlong)(int)directionDotOrCrossY);
    vertexA = triangle->vertex0;
    negVertex0XOrEdge2X = -vertexA->x;
    hitRelativeX = ((int)((ulonglong)
                    ((longlong)hitResult.distanceQ12 * (longlong)(int)g_ModelRaycastLocalDirectionXQ28) >>
                   0x20) << 4 |
             (uint)((longlong)hitResult.distanceQ12 * (longlong)(int)g_ModelRaycastLocalDirectionXQ28) >>
             0x1c) + g_ModelRaycastLocalOriginX + negVertex0XOrEdge2X;
    negVertex0YOrEdge2Y = -vertexA->y;
    hitRelativeY = ((int)((ulonglong)
                    ((longlong)(int)g_ModelRaycastLocalDirectionYQ28 * (longlong)hitResult.distanceQ12) >>
                   0x20) << 4 |
             (uint)((longlong)(int)g_ModelRaycastLocalDirectionYQ28 * (longlong)hitResult.distanceQ12) >>
             0x1c) + g_ModelRaycastLocalOriginY + negVertex0YOrEdge2Y;
    negVertex0ZOrEdge2Z = -vertexA->z;
    hitRelativeZ = ((int)((ulonglong)
                    ((longlong)(int)g_ModelRaycastLocalDirectionZQ28 * (longlong)hitResult.distanceQ12) >>
                   0x20) << 4 |
             (uint)((longlong)(int)g_ModelRaycastLocalDirectionZQ28 * (longlong)hitResult.distanceQ12) >>
             0x1c) + g_ModelRaycastLocalOriginZ + negVertex0ZOrEdge2Z;
    vertexA = triangle->vertex1;
    vertexB = triangle->vertex2;
    scaledHighOrEdge1X = negVertex0XOrEdge2X + vertexA->x;
    halfOffsetOrEdge1Y = negVertex0YOrEdge2Y + vertexA->y;
    edge1Z = negVertex0ZOrEdge2Z + vertexA->z;
    negVertex0XOrEdge2X = negVertex0XOrEdge2X + vertexB->x;
    negVertex0YOrEdge2Y = negVertex0YOrEdge2Y + vertexB->y;
    negVertex0ZOrEdge2Z = negVertex0ZOrEdge2Z + vertexB->z;
    productScratch = (longlong)normalY * (longlong)edge1Z - (longlong)normalZ * (longlong)halfOffsetOrEdge1Y;
    distanceOrCrossX = (int)((ulonglong)productScratch >> 0x20) << 4 | (uint)productScratch >> 0x1c;
    productScratch = (longlong)normalZ * (longlong)scaledHighOrEdge1X - (longlong)normalX * (longlong)edge1Z;
    directionDotOrCrossY = (int)((ulonglong)productScratch >> 0x20) << 4 | (uint)productScratch >> 0x1c;
    productScratch = (longlong)normalX * (longlong)halfOffsetOrEdge1Y - (longlong)normalY * (longlong)scaledHighOrEdge1X;
    offsetHighOrCrossZ = (int)((ulonglong)productScratch >> 0x20) << 4 | (uint)productScratch >> 0x1c;
    productScratch = (longlong)hitRelativeY * (longlong)(int)directionDotOrCrossY + (longlong)hitRelativeX * (longlong)(int)distanceOrCrossX +
            (longlong)hitRelativeZ * (longlong)(int)offsetHighOrCrossZ;
    edge2Dot = (longlong)negVertex0YOrEdge2Y * (longlong)(int)directionDotOrCrossY + (longlong)(int)distanceOrCrossX * (longlong)negVertex0XOrEdge2X +
             (longlong)negVertex0ZOrEdge2Z * (longlong)(int)offsetHighOrCrossZ;
    scaledHighOrEdge1X = (int)((ulonglong)edge2Dot >> 0x20);
    hitCrossXOrEdge2HitDot = (longlong)normalY * (longlong)hitRelativeZ - (longlong)normalZ * (longlong)hitRelativeY;
    hitCrossY = (longlong)normalZ * (longlong)hitRelativeX - (longlong)normalX * (longlong)hitRelativeZ;
    hitCrossZ = (longlong)normalX * (longlong)hitRelativeY - (longlong)normalY * (longlong)hitRelativeX;
    hitCrossXOrEdge2HitDot = (longlong)negVertex0YOrEdge2Y *
             (longlong)(int)((int)((ulonglong)hitCrossY >> 0x20) << 4 | (uint)hitCrossY >> 0x1c) +
             (longlong)negVertex0XOrEdge2X *
             (longlong)(int)((int)((ulonglong)hitCrossXOrEdge2HitDot >> 0x20) << 4 | (uint)hitCrossXOrEdge2HitDot >> 0x1c) +
             (longlong)negVertex0ZOrEdge2Z *
             (longlong)(int)((int)((ulonglong)hitCrossZ >> 0x20) << 4 | (uint)hitCrossZ >> 0x1c);
    distanceOrCrossX = (uint)hitCrossXOrEdge2HitDot;
    if (edge2Dot < 0) {
      if (((hitCrossXOrEdge2HitDot < 0) && (productScratch < 0)) &&
         (distanceOrCrossX = (uint)(hitCrossXOrEdge2HitDot + productScratch),
         (int)((scaledHighOrEdge1X - (int)((ulonglong)(hitCrossXOrEdge2HitDot + productScratch) >> 0x20)) - (uint)((uint)edge2Dot < distanceOrCrossX)
              ) < 0)) goto LAB_0050b1b8;
    }
    else if (((-1 < hitCrossXOrEdge2HitDot) && (-1 < productScratch)) &&
            (distanceOrCrossX = (uint)(hitCrossXOrEdge2HitDot + productScratch),
            -1 < (int)((scaledHighOrEdge1X - (int)((ulonglong)(hitCrossXOrEdge2HitDot + productScratch) >> 0x20)) -
                      (uint)((uint)edge2Dot < distanceOrCrossX)))) {
LAB_0050b1b8:
      hitResult.carry = true;
      return hitResult;
    }
  }
LAB_0050afa0:
  missResult.carry = false;
  missResult.distanceQ12 = distanceOrCrossX;
  return missResult;
}


/* Address: 0x005289C0.
   Ownership: assets/model/definitions.
   Purpose: Finds a model definition by identifier in the 768-slot registry and returns its runtime descriptor at
   record +0x188; error 0x3E reports a miss.
*/
ModelBuildMetricEaxEcxEdxCf13
ModelDefinitionRegistry_FindBuildMetricTupleByIdCf(PckModelDefinitionIdCatalog definitionId)

{
  ModelDefinitionRecordPrefix *registeredDefinition;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  ModelBuildMetricEaxEcxEdxCf13 missResult;
  ModelBuildMetricEaxEcxEdxCf13 foundResult;
  ModelDefinitionRuntimeSemanticView280 *candidateDefinition;
  
  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 0x300;
  while ((registeredDefinition = *registryCursor, registeredDefinition == (ModelDefinitionRecordPrefix *)0x0 ||
         (definitionId != registeredDefinition->definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
      missResult.metric1 = definitionId;
      missResult.metric0 = 0x3e;
      missResult.metric2 = 0;
      missResult.carry = true;
      return missResult;
    }
  }
  foundResult.metric1 = registeredDefinition[0x20].byteSize;
  foundResult.metric0 = registeredDefinition[0x20].definitionId;
  foundResult.metric2 = registeredDefinition[0x20].flags;
  foundResult.carry = false;
  return foundResult;
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
  int linkedSlotsRemaining;
  PckModelDefinitionIdCatalog selectedDefinitionId;
  bool technologyLocked;
  
  linkedSlotsRemaining = 8;
  selectedDefinitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
  do {
    modelDefinitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
    if (modelDefinitionId != 0) {
      technologyLocked = ModelDefinition_IsFactionTechnologyUnlockedCf
                        (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                         modelDefinitionId);
      if (!technologyLocked) {
        selectedDefinitionId = modelDefinitionId;
      }
    }
    linkedDefinitionList = linkedDefinitionList + 4;
    linkedSlotsRemaining = linkedSlotsRemaining + -1;
  } while (linkedSlotsRemaining != 0);
  return selectedDefinitionId;
}


/* Serialized model node tree: flags +0x04 (low nibble 0 = has a sprite), sprite path +0x38, sprite
   asset +0x30, owned-copy count +0x34, child count +0x14, child offsets +0x18 + 4*i (relative to the
   asset, relocated in place). Loads or reuses each node's sprite; true (CF) with *error on failure. */
static bool ModelDefinition_ResolveNodeSpritesCf(MdlSerializedNodeHeader38 *node,byte *asset,dword *error)
{
  dword i;
  if ((node->nodeFlags & 0xf) == 0) {
    word *spritePath = (word *)(node + 1);
    PackageLoadEntryEaxCf5 loaded;
    SpriteAssetHeader *registered;
    WidePath_SetExtensionCode(0x727073,spritePath); /* ".spr" */
    loaded = Package_LoadEntry(spritePath);
    if (loaded.carry) {
      *error = (dword)loaded.bufferOrError;
      return true;
    }
    registered = SpriteAssetRegistry_FindById
                           (((SpriteAssetHeader *)loaded.bufferOrError)->registryHeader.registryId);
    if (registered == (SpriteAssetHeader *)0x0) {
      SpriteRegisterRelocateEaxCf5 relocated;
      node->ownedNestedResourcePresent = node->ownedNestedResourcePresent + 1;
      (node->spriteAssetReference).spriteAsset = (SpriteAssetHeader *)loaded.bufferOrError;
      relocated = SpriteAsset_RegisterAndRelocatePointers((SpriteAssetHeader *)loaded.bufferOrError);
      if (relocated.carry) {
        *error = (dword)relocated.assetOrError;
        return true;
      }
    }
    else {
      (node->spriteAssetReference).spriteAsset = registered;
      Resource_Release(loaded.bufferOrError);
    }
  }
  for (i = 0; i < (dword)node->childCount; i++) {
    node->childSerializedOffsets[i] = node->childSerializedOffsets[i] + (int)(uintptr_t)asset;
    if (ModelDefinition_ResolveNodeSpritesCf
                  ((MdlSerializedNodeHeader38 *)(uintptr_t)node->childSerializedOffsets[i],asset,error)) {
      return true;
    }
  }
  return false;
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
  dword nodeOffsetOrGridClass;
  MdlChildCount nodeChildCount;
  dword secondaryThreshold;
  dword resolverStatusOrSentinel;
  SpriteAssetHeader *loadedSpriteAsset;
  SpriteAssetHeader *registeredSpriteAsset;
  ShotDefinition *resolvedShotDefinition;
  ShotDefinition *resolvedShotDefinition2C;
  EffectDefinition *resolvedEffectDefinition80ToB8;
  ShotDefinition *resolvedShotDefinition168;
  EffectDefinition *resolvedEffectDefinitionTail;
  dword gridDerivedScalarCarrier;
  int slotsRemainingOrClassIndex;
  ModelDefinitionRecordPrefix **registrySlotCursor;
  MdlSerializedNodeHeader38 *serializedNodeCursor;
  bool resolveFailed;
  ModelDefinitionLookupEaxCf5 existingLookup;
  StatusValueEaxCf5 failureResult;
  PackageLoadEntryEaxCf5 spriteLoadResult;
  SpriteRegisterRelocateEaxCf5 spriteRelocateResult;
  ShotDefinitionLookupEaxCf5 shotLookup;
  EffectDefinitionLookupEaxCf5 effectLookup;
  StatusValueEaxCf5 successResult;
  
  registrySlotCursor = g_ModelDefinitionRegistry;
  slotsRemainingOrClassIndex = 0x300;
  existingLookup = ModelDefinitionRegistry_FindByIdWithErrorCf(definition->definitionId);
  if (existingLookup.carry) {
    do {
      if (*registrySlotCursor == (ModelDefinitionRecordPrefix *)0x0) {
        nodeOffsetOrGridClass = definition->serializedNodeOffsetOrPointer64;
        *registrySlotCursor = (ModelDefinitionRecordPrefix *)definition;
        if (nodeOffsetOrGridClass == 0) goto ModelDefinition_ResolveShotAndEffectReferences;
        definition->serializedNodeOffsetOrPointer64 =
             (dword)((asset->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                    (definition->serializedNodeOffsetOrPointer64 - 0x28));
        serializedNodeCursor =
             (MdlSerializedNodeHeader38 *)
             ((asset->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
             (nodeOffsetOrGridClass - 0x28));
        /* Rewritten from the assembly (0x0052869F-0x00528744): the node tree walk kept its
           {node, nextChild, remaining} frames on the machine stack; Ghidra only followed child 0. */
        if (ModelDefinition_ResolveNodeSpritesCf(serializedNodeCursor,(byte *)asset,&resolverStatusOrSentinel)) {
          goto ModelDefinition_ReturnReferenceResolutionResult;
        }
        goto ModelDefinition_ResolveShotAndEffectReferences;
      }
      registrySlotCursor = registrySlotCursor + 1;
      slotsRemainingOrClassIndex = slotsRemainingOrClassIndex + -1;
    } while (slotsRemainingOrClassIndex != 0);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x300,g_PackageLastErrorPath);
    resolverStatusOrSentinel = 0x3f;
  }
  else {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    resolverStatusOrSentinel = MODEL_DEFINITION_REFERENCE_FAILURE_SENTINEL_0x4B;
  }
ModelDefinition_ReturnReferenceResolutionResult:
  failureResult.carry = true;
  failureResult.valueOrError = resolverStatusOrSentinel;
  return failureResult;
ModelDefinition_ResolveShotAndEffectReferences:
  shotLookup = ShotDefinitionRegistry_FindByIdWithErrorCf
                     ((PckShotDefinitionIdCatalog)definition->shotDefinitionReference2C);
  resolvedShotDefinition2C = shotLookup.definitionOrError;
  resolverStatusOrSentinel = (dword)resolvedShotDefinition2C;
  if (!shotLookup.carry) {
    definition->shotDefinitionReference2C = resolvedShotDefinition2C;
    effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                       ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference80);
    resolvedEffectDefinition80ToB8 = effectLookup.definitionOrError;
    resolverStatusOrSentinel = (dword)resolvedEffectDefinition80ToB8;
    if (!effectLookup.carry) {
      definition->effectDefinitionReference80 = resolvedEffectDefinition80ToB8;
      effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                         ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference88);
      resolverStatusOrSentinel = (dword)effectLookup.definitionOrError;
      if (!effectLookup.carry) {
        definition->effectDefinitionReference88 = (EffectDefinition *)resolverStatusOrSentinel;
        effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                           ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference90);
        resolverStatusOrSentinel = (dword)effectLookup.definitionOrError;
        if (!effectLookup.carry) {
          definition->effectDefinitionReference90 = (EffectDefinition *)resolverStatusOrSentinel;
          effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                             ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference98)
          ;
          resolverStatusOrSentinel = (dword)effectLookup.definitionOrError;
          if (!effectLookup.carry) {
            definition->effectDefinitionReference98 = (EffectDefinition *)resolverStatusOrSentinel;
            effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                               ((PckEffectDefinitionIdCatalog)
                                definition->effectDefinitionReferenceA0);
            resolverStatusOrSentinel = (dword)effectLookup.definitionOrError;
            if (!effectLookup.carry) {
              definition->effectDefinitionReferenceA0 = (EffectDefinition *)resolverStatusOrSentinel
              ;
              effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                                 ((PckEffectDefinitionIdCatalog)
                                  definition->effectDefinitionReferenceA8);
              resolverStatusOrSentinel = (dword)effectLookup.definitionOrError;
              if (!effectLookup.carry) {
                definition->effectDefinitionReferenceA8 =
                     (EffectDefinition *)resolverStatusOrSentinel;
                effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                                   ((PckEffectDefinitionIdCatalog)
                                    definition->effectDefinitionReferenceB0);
                resolverStatusOrSentinel = (dword)effectLookup.definitionOrError;
                if (!effectLookup.carry) {
                  definition->effectDefinitionReferenceB0 =
                       (EffectDefinition *)resolverStatusOrSentinel;
                  effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                                     ((PckEffectDefinitionIdCatalog)
                                      definition->effectDefinitionReferenceB8);
                  resolverStatusOrSentinel = (dword)effectLookup.definitionOrError;
                  if (!effectLookup.carry) {
                    definition->effectDefinitionReferenceB8 =
                         (EffectDefinition *)resolverStatusOrSentinel;
                    if (definition->shotDefinitionReference168 != (ShotDefinition *)0xffffffff) {
                      shotLookup = ShotDefinitionRegistry_FindByIdWithErrorCf
                                         ((PckShotDefinitionIdCatalog)
                                          definition->shotDefinitionReference168);
                      resolvedShotDefinition168 = shotLookup.definitionOrError;
                      resolverStatusOrSentinel = (dword)resolvedShotDefinition168;
                      if (shotLookup.carry) goto ModelDefinition_ReturnReferenceResolutionResult;
                      definition->shotDefinitionReference168 = resolvedShotDefinition168;
                    }
                    effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                                       ((PckEffectDefinitionIdCatalog)
                                        definition->effectDefinitionReference174);
                    resolvedEffectDefinitionTail = effectLookup.definitionOrError;
                    resolverStatusOrSentinel = (dword)resolvedEffectDefinitionTail;
                    if (!effectLookup.carry) {
                      definition->effectDefinitionReference174 = resolvedEffectDefinitionTail;
                      effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                                         ((PckEffectDefinitionIdCatalog)
                                          definition->effectDefinitionReference58);
                      resolverStatusOrSentinel = (dword)effectLookup.definitionOrError;
                      if (!effectLookup.carry) {
                        definition->effectDefinitionReference58 =
                             (EffectDefinition *)resolverStatusOrSentinel;
                        effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                                           ((PckEffectDefinitionIdCatalog)
                                            definition->effectDefinitionReference190);
                        resolverStatusOrSentinel = (dword)effectLookup.definitionOrError;
                        if (!effectLookup.carry) {
                          definition->effectDefinitionReference190 =
                               (EffectDefinition *)resolverStatusOrSentinel;
                          effectLookup = EffectDefinitionRegistry_FindByIdWithErrorCf
                                             ((PckEffectDefinitionIdCatalog)
                                              definition->effectDefinitionReference254);
                          resolverStatusOrSentinel = (dword)effectLookup.definitionOrError;
                          if (!effectLookup.carry) {
                            definition->effectDefinitionReference254 =
                                 (EffectDefinition *)resolverStatusOrSentinel;
                            nodeOffsetOrGridClass = definition->gridClassification264;
                            if (-1 < (int)definition->gridClassification260) {
                              resolverStatusOrSentinel =
                                   (&g_GridInfluenceRadiusOffset0)
                                   [definition->gridClassification260];
                              definition->placementRadiusOrClearanceDC = resolverStatusOrSentinel;
                              definition->gridDerivedRuntimeValue1A0 = resolverStatusOrSentinel;
                            }
                            if (-1 < (int)nodeOffsetOrGridClass) {
                              if (definition->placementContactKindIndex278 == 1) {
                                resolverStatusOrSentinel =
                                     (&g_GridTerrainClassBit24MaxWaterSurfaceDelta)[nodeOffsetOrGridClass];
                                nodeOffsetOrGridClass = (&g_GridTerrainClassBit28MaxTriangle0NormalAngleHigh16)
                                        [nodeOffsetOrGridClass];
                                *(dword *)(definition->reserved0C0_0DB + 0xc) =
                                     resolverStatusOrSentinel;
                                definition->runtimeValue24 = nodeOffsetOrGridClass;
                              }
                              else if (definition->placementContactKindIndex278 == 4) {
                                resolverStatusOrSentinel =
                                     *(dword *)(&g_ModelTraversalClass4SecondaryThresholdTable3 +
                                               (nodeOffsetOrGridClass - 1) * 4);
                                definition->runtimeValue24 =
                                     (&g_GridTerrainClassBit25MaxSelectedNormalAngleHigh16)
                                     [nodeOffsetOrGridClass - 1];
                                definition->runtimeValue268 = resolverStatusOrSentinel;
                              }
                              else {
                                slotsRemainingOrClassIndex = nodeOffsetOrGridClass - 4;
                                resolverStatusOrSentinel =
                                     (&g_GridTerrainClassBit28MinWaterSurfaceDelta)[slotsRemainingOrClassIndex];
                                nodeOffsetOrGridClass = (&g_GridTerrainClassBit28MaxTriangle0NormalAngleHigh16)
                                        [slotsRemainingOrClassIndex];
                                secondaryThreshold = *(dword *)(&g_ModelTraversalFallbackSecondaryThresholdTable3
                                                  + slotsRemainingOrClassIndex * 4);
                                definition->runtimeValue198 = resolverStatusOrSentinel;
                                definition->runtimeValue24 = nodeOffsetOrGridClass;
                                definition->runtimeValue268 = secondaryThreshold;
                              }
                            }
                            successResult.carry = false;
                            successResult.valueOrError = resolverStatusOrSentinel;
                            return successResult;
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
  ModelDefinitionLookupEaxCf5 lookupResult;
  
  lookupResult = ModelDefinitionRegistry_FindByIdWithErrorCf(modelDefinitionId);
  if (!lookupResult.carry) {
    Technology_UnlockForFaction(0,0,lookupResult.modelDefinition[0x25].definitionId,factionIndex);
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
  uint technologyBitIndex;
  ModelDefinitionLookupEaxCf5 lookupResult;
  
  lookupResult = ModelDefinitionRegistry_FindByIdWithErrorCf(modelDefinitionId);
  if ((!lookupResult.carry) &&
     (technologyBitIndex = lookupResult.modelDefinition[0x25].flags,
     (factionTechnologyMasks[technologyBitIndex >> 5] & 1 << ((byte)technologyBitIndex & 0x1f)) != 0)) {
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
  ModelDefinitionRecordPrefix *registeredDefinition;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  ModelDefinitionLookupEaxCf5 missResult;
  ModelDefinitionLookupEaxCf5 foundResult;
  ModelDefinitionRecordPrefix *candidateDefinition;
  
  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 0x300;
  while ((registeredDefinition = *registryCursor, registeredDefinition == (ModelDefinitionRecordPrefix *)0x0 ||
         (registeredDefinition->definitionId != definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)registeredDefinition,g_PackageLastErrorPath);
      missResult.carry = true;
      missResult.modelDefinition = (ModelDefinitionRecordPrefix *)0x3e;
      return missResult;
    }
  }
  foundResult.carry = false;
  foundResult.modelDefinition = registeredDefinition;
  return foundResult;
}

