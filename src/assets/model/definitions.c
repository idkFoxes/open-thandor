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
   Picks the upgrade stage a faction can build: of the eight linked model-definition ids at +0x20 the last
   non-zero one whose technology the faction has unlocked wins (the first id is the fallback), and it is
   looked up in the registry. CF is always clear, even when the lookup fails.
*/
ModelDefinitionResult __thandor_eax_cf_preserve_ecx_edx
ModelDefinition_SelectFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList)

{
  PckModelDefinitionIdCatalog linkedDefinitionId;
  int linkedSlotsRemaining;
  PckModelDefinitionIdCatalog selectedDefinitionId;
  bool technologyLocked;
  ModelDefinitionResult lookupResult;

  linkedSlotsRemaining = 8;
  selectedDefinitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
  do {
    linkedDefinitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
    if (linkedDefinitionId != 0) {
      /* despite its name the check returns true (CF set) when the technology is still locked */
      technologyLocked = ModelDefinition_IsFactionTechnologyUnlocked
                        (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                         linkedDefinitionId);
      if (!technologyLocked) {
        selectedDefinitionId = linkedDefinitionId;
      }
    }
    linkedDefinitionList = linkedDefinitionList + 4;
    linkedSlotsRemaining--;
  } while (linkedSlotsRemaining != 0);
  lookupResult = ModelDefinitionRegistry_FindByIdWithError(selectedDefinitionId);
  lookupResult.notFound = false; /* the original ends with CLC after the lookup */
  return lookupResult;
}


/* Recursive part of ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology: unlocks the technology of the
   linked definition the faction can select at this node (linked ids at +0x20), then does the same for every
   child (child count +0x08, children +0x0C + 4*i). */
static void ModelDefinitionHierarchy_UnlockFrom(FactionRuntimeIndex factionIndex,uint8_t *node)
{
  uint32_t childIndex;
  ModelDefinition_UnlockLinkedTechnologyForFaction
            (factionIndex,ModelDefinition_SelectFactionUnlockedLinkedId
                                    (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node));
  for (childIndex = 0; childIndex < *(uint32_t *)(node + 8); childIndex++) {
    ModelDefinitionHierarchy_UnlockFrom(factionIndex,*(uint8_t **)(node + 0xc + childIndex * 4));
  }
}

/* Address: 0x0051DB00.
   Walks the model-definition tree below definitionNode depth-first and, for every node, unlocks for the
   faction the technology granted by the linked definition the faction can currently select
   (ModelDefinition_SelectFactionUnlockedLinkedId). Used when an army is created with
   ARMY_CREATE_UNLOCK_TECHNOLOGY.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  ModelDefinitionHierarchy_UnlockFrom(factionIndex,*(uint8_t **)(uintptr_t)(definitionNode + 0xc));
}


/* Recursive part of ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction: true (CF set) as soon as
   this node's definition id (+0x20) or one in its subtree (child count +0x08, children +0x0C + 4*i) names a
   technology the faction has not unlocked yet. */
static bool ModelDefinitionHierarchy_AnyTechnologyFrom(uint32_t *technologyMasks,uint8_t *node)
{
  uint32_t childIndex;
  /* true from this check means the technology is still locked */
  if (ModelDefinition_IsFactionTechnologyUnlocked
                (technologyMasks,*(PckModelDefinitionIdCatalog *)(node + 0x20))) {
    return true;
  }
  for (childIndex = 0; childIndex < *(uint32_t *)(node + 8); childIndex++) {
    if (ModelDefinitionHierarchy_AnyTechnologyFrom
                  (technologyMasks,*(uint8_t **)(node + 0xc + childIndex * 4))) {
      return true;
    }
  }
  return false;
}

/* Address: 0x0051DA60.
   Walks the model-definition hierarchy below definitionNode and tests each definition's technology
   requirement against the faction's technology masks. Returns false (CF clear) when every definition in the
   tree is unlocked, true (CF set) as soon as one is still locked: ModelDefinition_IsFactionTechnologyUnlocked
   reports a locked technology with CF set.
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
   Checks that the asset is an 'mdl' of converter version 0x8000A, then registers each of its variable-size
   model-definition records (starting at +0x200, each prefixed with its byte size) and resolves their
   references against the asset base. Stops with CF set at the first record that fails.
*/
StatusResult __thandor_void_preserve_ecx_edx ModelAsset_PrepareRecords(ModelAssetHeader *asset)

{
  uint32_t registrationStatusCode;
  AssetRecordCount recordsRemaining;
  ModelDefinitionResolvePhaseView280 *definition;
  StatusResult registrationResult;
  StatusResult failureResult;

  registrationStatusCode = FATAL_ERROR_MODEL_ASSET_INVALID;
  if ((asset->recordCountHeader.common.magic == ASSET_MAGIC_MDL) &&
     (asset->recordCountHeader.common.converterVersion == PCK_CONVERTER_MDL_0008000A)) {
    recordsRemaining = asset->recordCountHeader.recordCount;
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
      /* advance by the record's leading byte size (reserved010_017 lies at +0x10) */
      definition = (ModelDefinitionResolvePhaseView280 *)
                   (definition->reserved010_017 + (definition->byteSize - 0x10));
      recordsRemaining--;
    }
  }
  failureResult.failed = true;
  failureResult.valueOrError = registrationStatusCode;
  return failureResult;
}


/* Address: 0x004BE670.
   Looks up the model's packed point table (entry count +0xE8, offset +0xE4, 0x10-byte ModelPackedPointRecord
   entries) for the key (keyIndex << 4) | keyClass and returns the entry's local position (the dwords at +4, +8,
   +0xC) in EAX/ECX/EDX with CF clear; all three zero with CF set when no entry matches. Called directly by
   ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters (a model class-init callback table slot) and by the
   army platform-lowering step in gameplay/army/runtime.c.
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
    packedKeyEntryCursor = packedKeyEntryCursor + 4; /* next 0x10-byte entry */
    entriesRemaining--;
  }
  payloadResult.notFound = false;
  payloadResult.payload4 = packedKeyEntryCursor[1];
  payloadResult.payload8 = packedKeyEntryCursor[2];
  payloadResult.payload12 = packedKeyEntryCursor[3];
  return payloadResult;
}


/* Address: 0x004BE6F0.
   Looks up the model's packed point table (entry count +0xE8, offset +0xE4, 0x10-byte entries) for the key
   (keyIndex << 4) | keyClass and returns the matching entry with CF clear. When no entry matches, CF is set
   and EAX points just past the table.
*/
ModelLookupEntryResult __thandor_eax_cf_preserve_ecx_edx
ModelLookupTable_ContainsPackedKey
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResourceHitTestAndRenderView210 *modelDefinition)

{
  ModelPackedPointRecord *entryCursor;
  int entriesRemaining;
  ModelLookupEntryResult foundResult;
  ModelLookupEntryResult notFoundResult;

  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  entryCursor =
       (ModelPackedPointRecord *)
       (modelDefinition->reserved00_AF + modelDefinition->packedLookupTableRelativeOffset);
  while( true ) {
    if (entriesRemaining == 0) {
      notFoundResult.notFound = true;
      notFoundResult.entry = entryCursor;
      return notFoundResult;
    }
    if ((keyClass | keyIndex << 4) == entryCursor->packedLookupKey) break;
    entryCursor++;
    entriesRemaining--;
  }
  foundResult.notFound = false;
  foundResult.entry = entryCursor;
  return foundResult;
}


/* Address: 0x0050AEA0.
   Intersects the current model-space pick ray (g_ModelRaycastLocalOrigin*, g_ModelRaycastLocalDirection*Q28,
   limited to g_ModelRaycastMaximumDistance) with one triangle: first the plane distance along the ray (plane
   through the weighted centre (2*v0 + v1 + v2) / 4), then an inside test of the hit point against the edges.
   On a hit returns the Q12 distance with CF set; on a miss CF is clear and EAX holds scratch. Called for each
   triangle by ModelNodeRuntime_RaycastHierarchyNearest.
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
   Looks a model definition up by id in the 768-slot registry and returns the three dwords at record
   +0x188 (EAX), +0x180 (ECX) and +0x184 (EDX), which ArmyAssetRecord_RelocateModelTree adds to an army
   record. On a miss the id is formatted into g_PackageLastErrorPath and FATAL_ERROR_MODEL_DEFINITION_MISSING
   is returned with CF set.
*/
BuildMetricResult
ModelDefinitionRegistry_FindBuildMetricTupleById(PckModelDefinitionIdCatalog definitionId)

{
  ModelDefinitionRecordPrefix *registeredDefinition;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  BuildMetricResult missResult;
  BuildMetricResult foundResult;

  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 768;
  while ((registeredDefinition = *registryCursor, registeredDefinition == NULL ||
         (definitionId != registeredDefinition->definitionId))) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
      missResult.metric1 = definitionId;
      missResult.metric0 = FATAL_ERROR_MODEL_DEFINITION_MISSING;
      missResult.metric2 = 0;
      missResult.notFound = true;
      return missResult;
    }
  }
  /* [0x20] steps over 0x20 twelve-byte record prefixes: offsets +0x180, +0x188 and +0x184 */
  foundResult.metric1 = registeredDefinition[0x20].byteSize;
  foundResult.metric0 = registeredDefinition[0x20].definitionId;
  foundResult.metric2 = registeredDefinition[0x20].flags;
  foundResult.notFound = false;
  return foundResult;
}


/* Address: 0x0053BA00.
   Returns the first registered model definition whose runtime class id (dword +0x1C0) equals runtimeClassId,
   or NULL. Called directly by the AI planning and technology code (gameplay/ai/planning.c, technology.c).
*/
ModelDefinitionRecordPrefix *
ModelDefinitionRegistry_FindByRuntimeClassId(ModelRuntimeClassId runtimeClassId)

{
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  ModelDefinitionRecordPrefix *candidateDefinition;

  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 768;
  /* [0x25].flags: 0x25 twelve-byte record prefixes plus 4 = offset +0x1C0 */
  while ((candidateDefinition = *registryCursor,
         candidateDefinition == NULL ||
         (runtimeClassId != candidateDefinition[0x25].flags))) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      return NULL;
    }
  }
  return candidateDefinition;
}

/* Address: 0x0051B430.
   Same selection as ModelDefinition_SelectFactionUnlockedLinkedDefinition, but returns the chosen id
   itself: the last non-zero of the eight linked ids at +0x20 whose technology the faction has unlocked,
   or the first id when none is. CF is always clear.
*/
PckModelDefinitionIdCatalog __thandor_eax_preserve_ecx_edx
ModelDefinition_SelectFactionUnlockedLinkedId
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList)

{
  PckModelDefinitionIdCatalog linkedDefinitionId;
  int linkedSlotsRemaining;
  PckModelDefinitionIdCatalog selectedDefinitionId;
  bool technologyLocked;

  linkedSlotsRemaining = 8;
  selectedDefinitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
  do {
    linkedDefinitionId = *(PckModelDefinitionIdCatalog *)(linkedDefinitionList + 0x20);
    if (linkedDefinitionId != 0) {
      /* true (CF set) means the technology is still locked */
      technologyLocked = ModelDefinition_IsFactionTechnologyUnlocked
                        (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                         linkedDefinitionId);
      if (!technologyLocked) {
        selectedDefinitionId = linkedDefinitionId;
      }
    }
    linkedDefinitionList = linkedDefinitionList + 4;
    linkedSlotsRemaining--;
  } while (linkedSlotsRemaining != 0);
  return selectedDefinitionId;
}


/* Serialized model node tree: flags +0x04 (low nibble 0 = has a sprite), sprite path +0x38, sprite
   asset +0x30, owned-copy count +0x34, child count +0x14, child offsets +0x18 + 4*i (relative to the
   asset, relocated in place). Loads or reuses each node's sprite; true (CF) with *error on failure. */
static bool ModelDefinition_ResolveNodeSprites(MdlSerializedNodeHeader38 *node,uint8_t *asset,uint32_t *error)
{
  uint32_t childIndex;
  if ((node->nodeFlags & 0xf) == 0) {
    uint16_t *spritePath = (uint16_t *)(node + 1);
    PackageLoadResult loaded;
    SpriteAssetHeader *registered;
    /* ".spr". The original tests its CF (JC at 0x005286C6), but WidePath_SetExtensionCode always
       returns with CLC (0x0040F314), so that branch is dead. The error exit at 0x00528677 only drops the
       walk's stack frames before MOV ESP,EBP; returning up the recursion is equivalent. */
    WidePath_SetExtensionCode(0x727073,spritePath);
    loaded = Package_LoadEntry(spritePath);
    if (loaded.failed) {
      *error = (uint32_t)loaded.bufferOrError;
      return true;
    }
    registered = SpriteAssetRegistry_FindById
                           (((SpriteAssetHeader *)loaded.bufferOrError)->registryHeader.registryId);
    if (registered == NULL) {
      /* first use of this sprite: the node owns the loaded copy and registers it */
      SpriteRegisterResult relocated;
      node->ownedNestedResourcePresent++;
      (node->spriteAssetReference).spriteAsset = (SpriteAssetHeader *)loaded.bufferOrError;
      relocated = SpriteAsset_RegisterAndRelocatePointers((SpriteAssetHeader *)loaded.bufferOrError);
      if (relocated.failed) {
        *error = (uint32_t)relocated.assetOrError;
        return true;
      }
    }
    else {
      /* already registered by another model: share it and drop the fresh copy */
      (node->spriteAssetReference).spriteAsset = registered;
      Resource_Release(loaded.bufferOrError);
    }
  }
  for (childIndex = 0; childIndex < (uint32_t)node->childCount; childIndex++) {
    /* relocate the child offset to a pointer in place */
    node->childSerializedOffsets[childIndex] = node->childSerializedOffsets[childIndex] + (int)(uintptr_t)asset;
    if (ModelDefinition_ResolveNodeSprites
                  ((MdlSerializedNodeHeader38 *)(uintptr_t)node->childSerializedOffsets[childIndex],asset,error)) {
      return true;
    }
  }
  return false;
}

/* Address: 0x00528600.
   Registers one MDL model definition in the first free slot of the 768-slot registry and turns its
   serialized references into runtime pointers: the node tree is relocated by the asset base and its
   sprites are loaded or reused, the shot and effect ids are resolved through their registries, and the
   terrain-class dependent placement values are copied from the grid tables. A duplicate id, a full
   registry or any failed load/lookup returns its error code with CF set.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
ModelDefinition_RegisterAndResolveReferences
          (ModelDefinitionResolvePhaseView280 *definition,ModelAssetHeader *asset)

{
  uint32_t nodeOffsetOrGridClass;
  uint32_t secondaryThreshold;
  uint32_t resolverStatusOrSentinel;
  ShotDefinition *resolvedShotDefinition2C;
  EffectDefinition *resolvedEffectDefinition80ToB8;
  ShotDefinition *resolvedShotDefinition168;
  EffectDefinition *resolvedEffectDefinitionTail;
  int slotsRemainingOrClassIndex;
  ModelDefinitionRecordPrefix **registrySlotCursor;
  MdlSerializedNodeHeader38 *serializedNodeCursor;
  ModelDefinitionResult existingLookup;
  StatusResult failureResult;
  ShotDefinitionResult shotLookup;
  EffectDefinitionResult effectLookup;
  StatusResult successResult;

  registrySlotCursor = g_ModelDefinitionRegistry;
  slotsRemainingOrClassIndex = MODEL_DEFINITION_REGISTRY_SLOT_COUNT;
  existingLookup = ModelDefinitionRegistry_FindByIdWithError(definition->definitionId);
  if (!existingLookup.notFound) {
    /* duplicate id: the id is left in g_PackageLastErrorPath */
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    resolverStatusOrSentinel = FATAL_ERROR_MODEL_ID_DUPLICATE;
    goto ModelDefinition_ReturnReferenceResolutionResult;
  }
  while (*registrySlotCursor != NULL) {
    registrySlotCursor++;
    slotsRemainingOrClassIndex--;
    if (slotsRemainingOrClassIndex == 0) {
      /* registry full: the slot count is left in g_PackageLastErrorPath */
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,MODEL_DEFINITION_REGISTRY_SLOT_COUNT,g_PackageLastErrorPath);
      resolverStatusOrSentinel = FATAL_ERROR_MODEL_REGISTRY_FULL;
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
                    /* -1: the definition has no shot at +0x168 */
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
                            /* negative grid classes keep the serialized values; the contact kind at +0x278
                               selects which grid tables the class at +0x264 indexes (kind 4 from class 1,
                               the fallback tables from class 4) */
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
   Unlocks for the faction the technology that the model definition grants (record +0x1C4), so building
   that model makes its successor technology available. An unknown id is silently ignored.
*/
void __thandor_preserve_eax
ModelDefinition_UnlockLinkedTechnologyForFaction
          (FactionRuntimeIndex factionIndex,PckModelDefinitionIdCatalog modelDefinitionId)

{
  ModelDefinitionResult lookupResult;

  lookupResult = ModelDefinitionRegistry_FindByIdWithError(modelDefinitionId);
  if (!lookupResult.notFound) {
    /* [0x25] steps over 0x25 twelve-byte record prefixes: .definitionId is record +0x1C4 */
    Technology_UnlockForFaction(0,0,lookupResult.modelDefinition[0x25].definitionId,factionIndex);
  }
}


/* Address: 0x0052AD90.
   Tests whether the faction may use the model definition: the technology bit it requires (record +0x1C0)
   must be set in the faction's 256-bit technology masks. Despite the name, true (CF set) means LOCKED
   (bit clear or unknown id); false (CF clear) means unlocked.
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
   Looks a model definition up by id in the 768-slot registry. On a miss it writes a number into
   g_PackageLastErrorPath for the error message and returns FATAL_ERROR_MODEL_DEFINITION_MISSING with CF set.
*/
ModelDefinitionResult __thandor_eax_cf_preserve_ecx_edx
ModelDefinitionRegistry_FindByIdWithError(PckModelDefinitionIdCatalog definitionId)

{
  ModelDefinitionRecordPrefix *registeredDefinition;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  ModelDefinitionResult missResult;
  ModelDefinitionResult foundResult;

  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = 768;
  while ((registeredDefinition = *registryCursor, registeredDefinition == NULL ||
         (registeredDefinition->definitionId != definitionId))) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      /* the original formats EAX, i.e. the last registry slot, not the missing id (PUSH EAX at 0x00528E59) */
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)registeredDefinition,g_PackageLastErrorPath);
      missResult.notFound = true;
      missResult.modelDefinition = (ModelDefinitionRecordPrefix *)FATAL_ERROR_MODEL_DEFINITION_MISSING;
      return missResult;
    }
  }
  foundResult.notFound = false;
  foundResult.modelDefinition = registeredDefinition;
  return foundResult;
}

