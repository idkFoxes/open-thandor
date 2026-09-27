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
   Local calls: ModelDefinition_IsFactionTechnologyUnlocked, ModelDefinitionRegistry_FindByIdWithError.
*/
ModelDefinitionResult __thandor_eax_cf_preserve_ecx_edx
ModelDefinition_SelectFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList)

{
  PckModelDefinitionIdCatalog modelDefinitionId;
  int linkedSlotsRemaining;
  PckModelDefinitionIdCatalog definitionId;
  bool technologyLocked;
  ModelDefinitionResult lookupResult;
  
  linkedSlotsRemaining = 8;
  definitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
  do {
    modelDefinitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
    if (modelDefinitionId != 0) {
      technologyLocked = ModelDefinition_IsFactionTechnologyUnlocked
                        (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                         modelDefinitionId);
      if (!technologyLocked) {
        definitionId = modelDefinitionId;
      }
    }
    linkedDefinitionList = linkedDefinitionList + 4;
    linkedSlotsRemaining = linkedSlotsRemaining + -1;
  } while (linkedSlotsRemaining != 0);
  lookupResult = ModelDefinitionRegistry_FindByIdWithError(definitionId);
  lookupResult.notFound = false; /* the original ends with CLC after the lookup */
  return lookupResult;
}


static void ModelDefinitionHierarchy_UnlockFrom(FactionRuntimeIndex factionIndex,uint8_t *node)
{
  uint32_t i;
  ModelDefinition_UnlockLinkedTechnologyForFaction
            (factionIndex,ModelDefinition_SelectFactionUnlockedLinkedId
                                    (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node));
  for (i = 0; i < *(uint32_t *)(node + 8); i++) {
    ModelDefinitionHierarchy_UnlockFrom(factionIndex,*(uint8_t **)(node + 0xc + i * 4));
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
   Local calls: ModelDefinition_SelectFactionUnlockedLinkedId,
   ModelDefinition_UnlockLinkedTechnologyForFaction.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  ModelDefinitionHierarchy_UnlockFrom(factionIndex,*(uint8_t **)(uintptr_t)(definitionNode + 0xc));
}


/* True (CF set) as soon as one node's technology reports CF from ModelDefinition_IsFactionTechnologyUnlocked. */
static bool ModelDefinitionHierarchy_AnyTechnologyFrom(uint32_t *technologyMasks,uint8_t *node)
{
  uint32_t i;
  if (ModelDefinition_IsFactionTechnologyUnlocked
                (technologyMasks,*(PckModelDefinitionIdCatalog *)(node + 0x20))) {
    return true;
  }
  for (i = 0; i < *(uint32_t *)(node + 8); i++) {
    if (ModelDefinitionHierarchy_AnyTechnologyFrom(technologyMasks,*(uint8_t **)(node + 0xc + i * 4))) {
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
   Local calls: ModelDefinition_IsFactionTechnologyUnlocked.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  return ModelDefinitionHierarchy_AnyTechnologyFrom
                   (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                    *(uint8_t **)(uintptr_t)(definitionNode + 0xc));
}


/* Address: 0x00528950.
   Ownership: assets/model/definitions.
   Purpose: Validates the 'mdl' magic and converter version 0x0008000A, then prepares recordCount variable-size
   records beginning at +0x200. Each successful record advances by its leading byte-size dword. The per-record
   preparer receives the asset base for stored-offset relocation. Payload fields remain opaque. Role: Walks
   variable-size MDL records and registers each model definition.
   Local calls: ModelDefinition_RegisterAndResolveReferences.
*/
StatusResult __thandor_void_preserve_ecx_edx ModelAsset_PrepareRecords(ModelAssetHeader *asset)

{
  uint32_t registrationStatusCode;
  AssetRecordCount recordsRemaining;
  ModelDefinitionResolvePhaseView280 *definition;
  ModelDefinitionRecordPrefix *definitionCursor;
  StatusResult registrationResult;
  StatusResult failureResult;
  
  registrationStatusCode = 0x3d;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_MDL) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_MDL_0008000A)) {
    recordsRemaining = (asset->recordCountHeader).recordCount;
    definition = (ModelDefinitionResolvePhaseView280 *)(asset + 1);
    while( true ) {
      if (recordsRemaining == 0) {
        registrationResult.failed = false;
        registrationResult.valueOrError = registrationStatusCode;
        return registrationResult;
      }
      registrationResult = ModelDefinition_RegisterAndResolveReferences(definition,asset);
      registrationStatusCode = registrationResult.valueOrError;
      if (registrationResult.failed) break;
      definition = (ModelDefinitionResolvePhaseView280 *)
                   (definition->reserved010_017 + (definition->byteSize - 0x10));
      recordsRemaining = recordsRemaining - 1;
    }
  }
  failureResult.failed = true;
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
ModelLookupPayloadResult
ModelLookupTable_FindPackedKeyEntryRegs
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResourceHitTestAndRenderView210 *modelDefinition)

{
  int entriesRemaining;
  uint32_t *packedKeyEntryCursor;
  ModelLookupPayloadResult payloadResult;
  
  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  packedKeyEntryCursor =
       (uint32_t *)(modelDefinition->reserved00_AF + modelDefinition->packedLookupTableRelativeOffset);
  while( true ) {
    if (entriesRemaining == 0) {
      /* not found: EAX, ECX and EDX zero, CF set (the decompiled 13-byte constant was cut to
         64 bits and lost the CF byte) */
      payloadResult.payload4 = 0;
      payloadResult.payload8 = 0;
      payloadResult.payload12 = 0;
      payloadResult.notFound = true;
      return payloadResult;
    }
    if ((keyClass | keyIndex << 4) == *packedKeyEntryCursor) break;
    packedKeyEntryCursor = packedKeyEntryCursor + 4;
    entriesRemaining = entriesRemaining - 1;
  }
  payloadResult.notFound = false;
  payloadResult.payload4 = packedKeyEntryCursor[1];
  payloadResult.payload8 = packedKeyEntryCursor[2];
  payloadResult.payload12 = packedKeyEntryCursor[3];
  return payloadResult;
}


/* Address: 0x004BE6F0.
   Ownership: assets/model/definitions.
   Purpose: Scans the same 0x10-byte model lookup table for packed key (groupIndex << 4) | itemIndex. CF is clear
   when a matching entry exists and set when the table is empty or no key matches. Saved ids, relocated pointers,
   attachment selectors, and runtime class ids remain separate. Key index and key class remain separate 32-bit
   domains. Typed parameters: p0 keyIndex→ModelLookupKeyIndex_V338, p1 keyClass→ModelLookupKeyClass_V338.
*/
ModelLookupEntryResult __thandor_eax_cf_preserve_ecx_edx
ModelLookupTable_ContainsPackedKey
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResourceHitTestAndRenderView210 *modelDefinition)

{
  ModelPackedPointRecord *packedKeyEntryCursor;
  int entriesRemaining;
  ModelLookupEntryResult foundResult;
  ModelLookupEntryResult notFoundResult;
  
  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  packedKeyEntryCursor =
       (ModelPackedPointRecord *)
       (modelDefinition->reserved00_AF + modelDefinition->packedLookupTableRelativeOffset);
  while( true ) {
    if (entriesRemaining == 0) {
      notFoundResult.notFound = true;
      notFoundResult.entry = packedKeyEntryCursor;
      return notFoundResult;
    }
    if ((keyClass | keyIndex << 4) == packedKeyEntryCursor->packedLookupKey) break;
    packedKeyEntryCursor = packedKeyEntryCursor + 1;
    entriesRemaining = entriesRemaining - 1;
  }
  foundResult.notFound = false;
  foundResult.entry = packedKeyEntryCursor;
  return foundResult;
}


/* Address: 0x0050AEA0.
   Ownership: assets/model/definitions.
   Purpose: Intersects the shared model-space ray with one exact 0x40-byte triangle record. EAX is Q12 distance; CF
   set means hit, CF clear means no hit.
*/
MeshRayTriangleResult __thandor_eax_cf_preserve_ecx_edx
ModelMesh_IntersectTriangleRayDistance(ModelRaycastTriangleDescriptor *triangle)

{
  int edge1Z;
  int negVertex0XOrEdge2X;
  int negVertex0YOrEdge2Y;
  int negVertex0ZOrEdge2Z;
  GraphicsFixedVec3 *vertexA;
  GraphicsFixedVec3 *vertexB;
  GraphicsFixedVec3 *vertexC;
  uint64_t planeOffsetDot;
  int64_t productScratch;
  int64_t edge2Dot;
  int64_t hitCrossXOrEdge2HitDot;
  int64_t hitCrossY;
  int64_t hitCrossZ;
  int normalX;
  uint32_t distanceOrCrossX;
  int normalY;
  uint32_t directionDotOrCrossY;
  int normalZ;
  int scaledHighOrEdge1X;
  int hitRelativeX;
  int hitRelativeY;
  int hitRelativeZ;
  uint32_t offsetHighOrCrossZ;
  int halfOffsetOrEdge1Y;
  MeshRayTriangleResult missResult;
  MeshRayTriangleResult hitResult;
  bool hitFound;

  normalX = triangle->planeNormalX << 0x10;
  normalY = triangle->planeNormalY << 0x10;
  normalZ = triangle->planeNormalZ << 0x10;
  vertexA = triangle->vertex0;
  vertexB = triangle->vertex1;
  vertexC = triangle->vertex2;
  planeOffsetDot = (int64_t)normalY *
          (int64_t)((vertexA->y * 2 + vertexB->y + vertexC->y >> 2) - g_ModelRaycastLocalOriginY) +
          (int64_t)((vertexA->x * 2 + vertexB->x + vertexC->x >> 2) - g_ModelRaycastLocalOriginX) *
          (int64_t)normalX +
          (int64_t)((vertexA->z * 2 + vertexB->z + vertexC->z >> 2) - g_ModelRaycastLocalOriginZ) *
          (int64_t)normalZ;
  offsetHighOrCrossZ = (uint32_t)(planeOffsetDot >> 0x20);
  productScratch = (int64_t)(int)g_ModelRaycastLocalDirectionYQ28 * (int64_t)normalY +
          (int64_t)(int)g_ModelRaycastLocalDirectionXQ28 * (int64_t)normalX +
          (int64_t)(int)g_ModelRaycastLocalDirectionZQ28 * (int64_t)normalZ;
  directionDotOrCrossY = (int)((uint64_t)productScratch >> 0x20) << 4 | (uint32_t)productScratch >> 0x1c;
  distanceOrCrossX = g_ModelRaycastMaximumDistance;
  if (directionDotOrCrossY != 0) {
    scaledHighOrEdge1X = (int)((uint64_t)((int64_t)(int)g_ModelRaycastMaximumDistance * (int64_t)(int)directionDotOrCrossY)
                  >> 0x20);
    distanceOrCrossX = (uint32_t)((int64_t)(int)g_ModelRaycastMaximumDistance * (int64_t)(int)directionDotOrCrossY);
    halfOffsetOrEdge1Y = (int)offsetHighOrCrossZ >> 1;
    if ((int64_t)planeOffsetDot < 0) {
      if (((int)offsetHighOrCrossZ < scaledHighOrEdge1X) ||
         ((halfOffsetOrEdge1Y <= (int)-directionDotOrCrossY && (distanceOrCrossX = (uint32_t)planeOffsetDot, halfOffsetOrEdge1Y <= (int)directionDotOrCrossY))))
      goto ModelMesh_IntersectTriangleRayDistance_ReturnMiss;
    }
    else if ((scaledHighOrEdge1X < (int)offsetHighOrCrossZ) ||
            (((int)-directionDotOrCrossY <= halfOffsetOrEdge1Y && (distanceOrCrossX = (uint32_t)planeOffsetDot, (int)directionDotOrCrossY <= halfOffsetOrEdge1Y))))
    goto ModelMesh_IntersectTriangleRayDistance_ReturnMiss;
    hitResult.distanceQ12 =
         (int)((int64_t)planeOffsetDot / (int64_t)(int)directionDotOrCrossY); /* IDIV of EDX:EAX */
    vertexA = triangle->vertex0;
    negVertex0XOrEdge2X = -vertexA->x;
    hitRelativeX = ((int)((uint64_t)
                    ((int64_t)hitResult.distanceQ12 * (int64_t)(int)g_ModelRaycastLocalDirectionXQ28) >>
                   0x20) << 4 |
             (uint32_t)((int64_t)hitResult.distanceQ12 * (int64_t)(int)g_ModelRaycastLocalDirectionXQ28) >>
             0x1c) + g_ModelRaycastLocalOriginX + negVertex0XOrEdge2X;
    negVertex0YOrEdge2Y = -vertexA->y;
    hitRelativeY = ((int)((uint64_t)
                    ((int64_t)(int)g_ModelRaycastLocalDirectionYQ28 * (int64_t)hitResult.distanceQ12) >>
                   0x20) << 4 |
             (uint32_t)((int64_t)(int)g_ModelRaycastLocalDirectionYQ28 * (int64_t)hitResult.distanceQ12) >>
             0x1c) + g_ModelRaycastLocalOriginY + negVertex0YOrEdge2Y;
    negVertex0ZOrEdge2Z = -vertexA->z;
    hitRelativeZ = ((int)((uint64_t)
                    ((int64_t)(int)g_ModelRaycastLocalDirectionZQ28 * (int64_t)hitResult.distanceQ12) >>
                   0x20) << 4 |
             (uint32_t)((int64_t)(int)g_ModelRaycastLocalDirectionZQ28 * (int64_t)hitResult.distanceQ12) >>
             0x1c) + g_ModelRaycastLocalOriginZ + negVertex0ZOrEdge2Z;
    vertexA = triangle->vertex1;
    vertexB = triangle->vertex2;
    scaledHighOrEdge1X = negVertex0XOrEdge2X + vertexA->x;
    halfOffsetOrEdge1Y = negVertex0YOrEdge2Y + vertexA->y;
    edge1Z = negVertex0ZOrEdge2Z + vertexA->z;
    negVertex0XOrEdge2X = negVertex0XOrEdge2X + vertexB->x;
    negVertex0YOrEdge2Y = negVertex0YOrEdge2Y + vertexB->y;
    negVertex0ZOrEdge2Z = negVertex0ZOrEdge2Z + vertexB->z;
    productScratch = (int64_t)normalY * (int64_t)edge1Z - (int64_t)normalZ * (int64_t)halfOffsetOrEdge1Y;
    distanceOrCrossX = (int)((uint64_t)productScratch >> 0x20) << 4 | (uint32_t)productScratch >> 0x1c;
    productScratch = (int64_t)normalZ * (int64_t)scaledHighOrEdge1X - (int64_t)normalX * (int64_t)edge1Z;
    directionDotOrCrossY = (int)((uint64_t)productScratch >> 0x20) << 4 | (uint32_t)productScratch >> 0x1c;
    productScratch = (int64_t)normalX * (int64_t)halfOffsetOrEdge1Y - (int64_t)normalY * (int64_t)scaledHighOrEdge1X;
    offsetHighOrCrossZ = (int)((uint64_t)productScratch >> 0x20) << 4 | (uint32_t)productScratch >> 0x1c;
    productScratch = (int64_t)hitRelativeY * (int64_t)(int)directionDotOrCrossY + (int64_t)hitRelativeX * (int64_t)(int)distanceOrCrossX +
            (int64_t)hitRelativeZ * (int64_t)(int)offsetHighOrCrossZ;
    edge2Dot = (int64_t)negVertex0YOrEdge2Y * (int64_t)(int)directionDotOrCrossY + (int64_t)(int)distanceOrCrossX * (int64_t)negVertex0XOrEdge2X +
             (int64_t)negVertex0ZOrEdge2Z * (int64_t)(int)offsetHighOrCrossZ;
    scaledHighOrEdge1X = (int)((uint64_t)edge2Dot >> 0x20);
    hitCrossXOrEdge2HitDot = (int64_t)normalY * (int64_t)hitRelativeZ - (int64_t)normalZ * (int64_t)hitRelativeY;
    hitCrossY = (int64_t)normalZ * (int64_t)hitRelativeX - (int64_t)normalX * (int64_t)hitRelativeZ;
    hitCrossZ = (int64_t)normalX * (int64_t)hitRelativeY - (int64_t)normalY * (int64_t)hitRelativeX;
    hitCrossXOrEdge2HitDot = (int64_t)negVertex0YOrEdge2Y *
             (int64_t)(int)((int)((uint64_t)hitCrossY >> 0x20) << 4 | (uint32_t)hitCrossY >> 0x1c) +
             (int64_t)negVertex0XOrEdge2X *
             (int64_t)(int)((int)((uint64_t)hitCrossXOrEdge2HitDot >> 0x20) << 4 | (uint32_t)hitCrossXOrEdge2HitDot >> 0x1c) +
             (int64_t)negVertex0ZOrEdge2Z *
             (int64_t)(int)((int)((uint64_t)hitCrossZ >> 0x20) << 4 | (uint32_t)hitCrossZ >> 0x1c);
    distanceOrCrossX = (uint32_t)hitCrossXOrEdge2HitDot;
    /* Inside test: both barycentric dots and their sum's difference to edge2Dot carry edge2Dot's sign. */
    if (edge2Dot < 0) {
      hitFound = ((hitCrossXOrEdge2HitDot < 0) && (productScratch < 0)) &&
         (distanceOrCrossX = (uint32_t)(hitCrossXOrEdge2HitDot + productScratch),
         (int)((scaledHighOrEdge1X - (int)((uint64_t)(hitCrossXOrEdge2HitDot + productScratch) >> 0x20)) - (uint32_t)((uint32_t)edge2Dot < distanceOrCrossX)
              ) < 0);
    }
    else {
      hitFound = ((-1 < hitCrossXOrEdge2HitDot) && (-1 < productScratch)) &&
            (distanceOrCrossX = (uint32_t)(hitCrossXOrEdge2HitDot + productScratch),
            -1 < (int)((scaledHighOrEdge1X - (int)((uint64_t)(hitCrossXOrEdge2HitDot + productScratch) >> 0x20)) -
                      (uint32_t)((uint32_t)edge2Dot < distanceOrCrossX)));
    }
    if (hitFound) {
      hitResult.hit = true;
      return hitResult;
    }
  }
ModelMesh_IntersectTriangleRayDistance_ReturnMiss:
  missResult.hit = false;
  missResult.distanceQ12 = distanceOrCrossX;
  return missResult;
}


/* Address: 0x005289C0.
   Ownership: assets/model/definitions.
   Purpose: Finds a model definition by identifier in the 768-slot registry and returns its runtime descriptor at
   record +0x188; error 0x3E reports a miss.
*/
BuildMetricResult
ModelDefinitionRegistry_FindBuildMetricTupleById(PckModelDefinitionIdCatalog definitionId)

{
  ModelDefinitionRecordPrefix *registeredDefinition;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  BuildMetricResult missResult;
  BuildMetricResult foundResult;
  ModelDefinitionRuntimeSemanticView280 *candidateDefinition;
  
  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 0x300;
  while ((registeredDefinition = *registryCursor, registeredDefinition == (ModelDefinitionRecordPrefix *)0x0 ||
         (definitionId != registeredDefinition->definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
      missResult.metric1 = definitionId;
      missResult.metric0 = 0x3e;
      missResult.metric2 = 0;
      missResult.notFound = true;
      return missResult;
    }
  }
  foundResult.metric1 = registeredDefinition[0x20].byteSize;
  foundResult.metric0 = registeredDefinition[0x20].definitionId;
  foundResult.metric2 = registeredDefinition[0x20].flags;
  foundResult.notFound = false;
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
   Local calls: ModelDefinition_IsFactionTechnologyUnlocked.
*/
PckModelDefinitionIdCatalog __thandor_eax_preserve_ecx_edx
ModelDefinition_SelectFactionUnlockedLinkedId
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
      technologyLocked = ModelDefinition_IsFactionTechnologyUnlocked
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
static bool ModelDefinition_ResolveNodeSprites(MdlSerializedNodeHeader38 *node,uint8_t *asset,uint32_t *error)
{
  uint32_t i;
  if ((node->nodeFlags & 0xf) == 0) {
    uint16_t *spritePath = (uint16_t *)(node + 1);
    PackageLoadResult loaded;
    SpriteAssetHeader *registered;
    WidePath_SetExtensionCode(0x727073,spritePath); /* ".spr" */
    loaded = Package_LoadEntry(spritePath);
    if (loaded.failed) {
      *error = (uint32_t)loaded.bufferOrError;
      return true;
    }
    registered = SpriteAssetRegistry_FindById
                           (((SpriteAssetHeader *)loaded.bufferOrError)->registryHeader.registryId);
    if (registered == (SpriteAssetHeader *)0x0) {
      SpriteRegisterResult relocated;
      node->ownedNestedResourcePresent = node->ownedNestedResourcePresent + 1;
      (node->spriteAssetReference).spriteAsset = (SpriteAssetHeader *)loaded.bufferOrError;
      relocated = SpriteAsset_RegisterAndRelocatePointers((SpriteAssetHeader *)loaded.bufferOrError);
      if (relocated.failed) {
        *error = (uint32_t)relocated.assetOrError;
        return true;
      }
    }
    else {
      (node->spriteAssetReference).spriteAsset = registered;
      Resource_Release(loaded.bufferOrError);
    }
  }
  for (i = 0; i < (uint32_t)node->childCount; i++) {
    node->childSerializedOffsets[i] = node->childSerializedOffsets[i] + (int)(uintptr_t)asset;
    if (ModelDefinition_ResolveNodeSprites
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
   Local calls: ModelDefinitionRegistry_FindByIdWithError.
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], Package_LoadEntry [assets/package/runtime],
   SpriteAssetRegistry_FindById [assets/sprite/catalog], SpriteAsset_RegisterAndRelocatePointers
   [assets/sprite/catalog], Resource_Release [assets/resource/runtime], ShotDefinitionRegistry_FindByIdWithError
   [assets/shot/catalog].
*/

StatusResult __thandor_eax_cf_preserve_ecx_edx
ModelDefinition_RegisterAndResolveReferences
          (ModelDefinitionResolvePhaseView280 *definition,ModelAssetHeader *asset)

{
  uint32_t nodeOffsetOrGridClass;
  MdlChildCount nodeChildCount;
  uint32_t secondaryThreshold;
  uint32_t resolverStatusOrSentinel;
  SpriteAssetHeader *loadedSpriteAsset;
  SpriteAssetHeader *registeredSpriteAsset;
  ShotDefinition *resolvedShotDefinition;
  ShotDefinition *resolvedShotDefinition2C;
  EffectDefinition *resolvedEffectDefinition80ToB8;
  ShotDefinition *resolvedShotDefinition168;
  EffectDefinition *resolvedEffectDefinitionTail;
  uint32_t gridDerivedScalarCarrier;
  int slotsRemainingOrClassIndex;
  ModelDefinitionRecordPrefix **registrySlotCursor;
  MdlSerializedNodeHeader38 *serializedNodeCursor;
  bool resolveFailed;
  ModelDefinitionResult existingLookup;
  StatusResult failureResult;
  PackageLoadResult spriteLoadResult;
  SpriteRegisterResult spriteRelocateResult;
  ShotDefinitionResult shotLookup;
  EffectDefinitionResult effectLookup;
  StatusResult successResult;
  
  registrySlotCursor = g_ModelDefinitionRegistry;
  slotsRemainingOrClassIndex = 0x300;
  existingLookup = ModelDefinitionRegistry_FindByIdWithError(definition->definitionId);
  if (!existingLookup.notFound) {
    /* Duplicate identifier. */
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    resolverStatusOrSentinel = MODEL_DEFINITION_REFERENCE_FAILURE_SENTINEL_0x4B;
    goto ModelDefinition_ReturnReferenceResolutionResult;
  }
  while (*registrySlotCursor != (ModelDefinitionRecordPrefix *)0x0) {
    registrySlotCursor = registrySlotCursor + 1;
    slotsRemainingOrClassIndex = slotsRemainingOrClassIndex + -1;
    if (slotsRemainingOrClassIndex == 0) {
      /* Registry full. */
      g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x300,g_PackageLastErrorPath);
      resolverStatusOrSentinel = 0x3f;
      goto ModelDefinition_ReturnReferenceResolutionResult;
    }
  }
  nodeOffsetOrGridClass = definition->serializedNodeOffsetOrPointer64;
  *registrySlotCursor = (ModelDefinitionRecordPrefix *)definition;
  if (nodeOffsetOrGridClass != 0) {
    definition->serializedNodeOffsetOrPointer64 =
         (uint32_t)((asset->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                (definition->serializedNodeOffsetOrPointer64 - 0x28));
    serializedNodeCursor =
         (MdlSerializedNodeHeader38 *)
         ((asset->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
         (nodeOffsetOrGridClass - 0x28));
    /* Rewritten from the assembly (0x0052869F-0x00528744): the node tree walk kept its
       {node, nextChild, remaining} frames on the machine stack; Ghidra only followed child 0. */
    if (ModelDefinition_ResolveNodeSprites(serializedNodeCursor,(uint8_t *)asset,&resolverStatusOrSentinel)) {
      goto ModelDefinition_ReturnReferenceResolutionResult;
    }
  }
  shotLookup = ShotDefinitionRegistry_FindByIdWithError
                     ((PckShotDefinitionIdCatalog)definition->shotDefinitionReference2C);
  resolvedShotDefinition2C = shotLookup.definitionOrError;
  resolverStatusOrSentinel = (uint32_t)resolvedShotDefinition2C;
  if (!shotLookup.notFound) {
    definition->shotDefinitionReference2C = resolvedShotDefinition2C;
    effectLookup = EffectDefinitionRegistry_FindByIdWithError
                       ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference80);
    resolvedEffectDefinition80ToB8 = effectLookup.definitionOrError;
    resolverStatusOrSentinel = (uint32_t)resolvedEffectDefinition80ToB8;
    if (!effectLookup.notFound) {
      definition->effectDefinitionReference80 = resolvedEffectDefinition80ToB8;
      effectLookup = EffectDefinitionRegistry_FindByIdWithError
                         ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference88);
      resolverStatusOrSentinel = (uint32_t)effectLookup.definitionOrError;
      if (!effectLookup.notFound) {
        definition->effectDefinitionReference88 = (EffectDefinition *)resolverStatusOrSentinel;
        effectLookup = EffectDefinitionRegistry_FindByIdWithError
                           ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference90);
        resolverStatusOrSentinel = (uint32_t)effectLookup.definitionOrError;
        if (!effectLookup.notFound) {
          definition->effectDefinitionReference90 = (EffectDefinition *)resolverStatusOrSentinel;
          effectLookup = EffectDefinitionRegistry_FindByIdWithError
                             ((PckEffectDefinitionIdCatalog)definition->effectDefinitionReference98)
          ;
          resolverStatusOrSentinel = (uint32_t)effectLookup.definitionOrError;
          if (!effectLookup.notFound) {
            definition->effectDefinitionReference98 = (EffectDefinition *)resolverStatusOrSentinel;
            effectLookup = EffectDefinitionRegistry_FindByIdWithError
                               ((PckEffectDefinitionIdCatalog)
                                definition->effectDefinitionReferenceA0);
            resolverStatusOrSentinel = (uint32_t)effectLookup.definitionOrError;
            if (!effectLookup.notFound) {
              definition->effectDefinitionReferenceA0 = (EffectDefinition *)resolverStatusOrSentinel
              ;
              effectLookup = EffectDefinitionRegistry_FindByIdWithError
                                 ((PckEffectDefinitionIdCatalog)
                                  definition->effectDefinitionReferenceA8);
              resolverStatusOrSentinel = (uint32_t)effectLookup.definitionOrError;
              if (!effectLookup.notFound) {
                definition->effectDefinitionReferenceA8 =
                     (EffectDefinition *)resolverStatusOrSentinel;
                effectLookup = EffectDefinitionRegistry_FindByIdWithError
                                   ((PckEffectDefinitionIdCatalog)
                                    definition->effectDefinitionReferenceB0);
                resolverStatusOrSentinel = (uint32_t)effectLookup.definitionOrError;
                if (!effectLookup.notFound) {
                  definition->effectDefinitionReferenceB0 =
                       (EffectDefinition *)resolverStatusOrSentinel;
                  effectLookup = EffectDefinitionRegistry_FindByIdWithError
                                     ((PckEffectDefinitionIdCatalog)
                                      definition->effectDefinitionReferenceB8);
                  resolverStatusOrSentinel = (uint32_t)effectLookup.definitionOrError;
                  if (!effectLookup.notFound) {
                    definition->effectDefinitionReferenceB8 =
                         (EffectDefinition *)resolverStatusOrSentinel;
                    if (definition->shotDefinitionReference168 != (ShotDefinition *)0xffffffff) {
                      shotLookup = ShotDefinitionRegistry_FindByIdWithError
                                         ((PckShotDefinitionIdCatalog)
                                          definition->shotDefinitionReference168);
                      resolvedShotDefinition168 = shotLookup.definitionOrError;
                      resolverStatusOrSentinel = (uint32_t)resolvedShotDefinition168;
                      if (shotLookup.notFound) goto ModelDefinition_ReturnReferenceResolutionResult;
                      definition->shotDefinitionReference168 = resolvedShotDefinition168;
                    }
                    effectLookup = EffectDefinitionRegistry_FindByIdWithError
                                       ((PckEffectDefinitionIdCatalog)
                                        definition->effectDefinitionReference174);
                    resolvedEffectDefinitionTail = effectLookup.definitionOrError;
                    resolverStatusOrSentinel = (uint32_t)resolvedEffectDefinitionTail;
                    if (!effectLookup.notFound) {
                      definition->effectDefinitionReference174 = resolvedEffectDefinitionTail;
                      effectLookup = EffectDefinitionRegistry_FindByIdWithError
                                         ((PckEffectDefinitionIdCatalog)
                                          definition->effectDefinitionReference58);
                      resolverStatusOrSentinel = (uint32_t)effectLookup.definitionOrError;
                      if (!effectLookup.notFound) {
                        definition->effectDefinitionReference58 =
                             (EffectDefinition *)resolverStatusOrSentinel;
                        effectLookup = EffectDefinitionRegistry_FindByIdWithError
                                           ((PckEffectDefinitionIdCatalog)
                                            definition->effectDefinitionReference190);
                        resolverStatusOrSentinel = (uint32_t)effectLookup.definitionOrError;
                        if (!effectLookup.notFound) {
                          definition->effectDefinitionReference190 =
                               (EffectDefinition *)resolverStatusOrSentinel;
                          effectLookup = EffectDefinitionRegistry_FindByIdWithError
                                             ((PckEffectDefinitionIdCatalog)
                                              definition->effectDefinitionReference254);
                          resolverStatusOrSentinel = (uint32_t)effectLookup.definitionOrError;
                          if (!effectLookup.notFound) {
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
                                *(uint32_t *)(definition->reserved0C0_0DB + 0xc) =
                                     resolverStatusOrSentinel;
                                definition->runtimeValue24 = nodeOffsetOrGridClass;
                              }
                              else if (definition->placementContactKindIndex278 == 4) {
                                resolverStatusOrSentinel =
                                     *(uint32_t *)(&g_ModelTraversalClass4SecondaryThresholdTable3 +
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
                                secondaryThreshold = *(uint32_t *)(&g_ModelTraversalFallbackSecondaryThresholdTable3
                                                  + slotsRemainingOrClassIndex * 4);
                                definition->runtimeValue198 = resolverStatusOrSentinel;
                                definition->runtimeValue24 = nodeOffsetOrGridClass;
                                definition->runtimeValue268 = secondaryThreshold;
                              }
                            }
                            successResult.failed = false;
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
ModelDefinition_ReturnReferenceResolutionResult:
  failureResult.failed = true;
  failureResult.valueOrError = resolverStatusOrSentinel;
  return failureResult;
}


/* Address: 0x0052ADE0.
   Ownership: assets/model/definitions.
   Purpose: The original lookup/unlock CF contract is preserved.
   Local calls: ModelDefinitionRegistry_FindByIdWithError.
   Cross-module calls: Technology_UnlockForFaction [gameplay/technology/runtime].
*/
void __thandor_preserve_eax
ModelDefinition_UnlockLinkedTechnologyForFaction
          (FactionRuntimeIndex factionIndex,PckModelDefinitionIdCatalog modelDefinitionId)

{
  ModelDefinitionResult lookupResult;
  
  lookupResult = ModelDefinitionRegistry_FindByIdWithError(modelDefinitionId);
  if (!lookupResult.notFound) {
    Technology_UnlockForFaction(0,0,lookupResult.modelDefinition[0x25].definitionId,factionIndex);
  }
  return;
}


/* Address: 0x0052AD90.
   Ownership: assets/model/definitions.
   Purpose: CF clear means the bit is unlocked; lookup failure or a clear bit returns CF set while preserving EAX.
   Local calls: ModelDefinitionRegistry_FindByIdWithError.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ModelDefinition_IsFactionTechnologyUnlocked
          (uint32_t *factionTechnologyMasks,PckModelDefinitionIdCatalog modelDefinitionId)

{
  uint32_t technologyBitIndex;
  ModelDefinitionResult lookupResult;
  
  lookupResult = ModelDefinitionRegistry_FindByIdWithError(modelDefinitionId);
  if ((!lookupResult.notFound) &&
     (technologyBitIndex = lookupResult.modelDefinition[0x25].flags,
     (factionTechnologyMasks[technologyBitIndex >> 5] & 1 << ((uint8_t)technologyBitIndex & 0x1f)) != 0)) {
    return false;
  }
  return true;
}


/* Address: 0x00528E20.
   Ownership: assets/model/definitions.
   Purpose: Scans the 768-slot model-definition registry. On a miss it formats the unresolved identifier into
   g_PackageLastErrorPath and returns error 0x3E with CF set.
*/
ModelDefinitionResult __thandor_eax_cf_preserve_ecx_edx
ModelDefinitionRegistry_FindByIdWithError(PckModelDefinitionIdCatalog definitionId)

{
  ModelDefinitionRecordPrefix *registeredDefinition;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  ModelDefinitionResult missResult;
  ModelDefinitionResult foundResult;
  ModelDefinitionRecordPrefix *candidateDefinition;
  
  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 0x300;
  while ((registeredDefinition = *registryCursor, registeredDefinition == (ModelDefinitionRecordPrefix *)0x0 ||
         (registeredDefinition->definitionId != definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)registeredDefinition,g_PackageLastErrorPath);
      missResult.notFound = true;
      missResult.modelDefinition = (ModelDefinitionRecordPrefix *)0x3e;
      return missResult;
    }
  }
  foundResult.notFound = false;
  foundResult.modelDefinition = registeredDefinition;
  return foundResult;
}

