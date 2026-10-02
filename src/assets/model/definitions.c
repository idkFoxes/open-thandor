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
   looked up in the registry. Returns that definition.
   Original quirk: an unregistered id is not reported; the result is then the error code
   FATAL_ERROR_MODEL_DEFINITION_MISSING cast to a pointer (what the original left in EAX), and the registry
   miss still writes g_PackageLastErrorPath.
*/
ModelDefinitionRecordPrefix *ModelDefinition_SelectFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList)

{
  PckModelDefinitionIdCatalog linkedDefinitionId;
  int linkedSlotsRemaining;
  PckModelDefinitionIdCatalog selectedDefinitionId;
  bool technologyLocked;
  ModelDefinitionRecordPrefix *selectedDefinition;

  selectedDefinitionId = ((ArmyModelTreeNode *)linkedDefinitionList)->linkedDefinitionIds[0];
  for (linkedSlotsRemaining = MODEL_LINKED_DEFINITION_COUNT; linkedSlotsRemaining != 0; linkedSlotsRemaining--) {
    /* the list cursor advances by one id, so linkedDefinitionIds[0] is the current slot */
    linkedDefinitionId = ((ArmyModelTreeNode *)linkedDefinitionList)->linkedDefinitionIds[0];
    if (linkedDefinitionId != 0) {
      /* true (CF set) while the technology is still locked */
      technologyLocked = ModelDefinition_IsFactionTechnologyLocked
                        (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                         linkedDefinitionId);
      if (!technologyLocked) {
        selectedDefinitionId = linkedDefinitionId;
      }
    }
    linkedDefinitionList = linkedDefinitionList + 4;
  }
  selectedDefinition = ModelDefinitionRegistry_FindById(selectedDefinitionId);
  if (selectedDefinition == NULL) {
    /* Original quirk: the error code of the failed lookup is returned as the definition */
    selectedDefinition = (ModelDefinitionRecordPrefix *)FATAL_ERROR_MODEL_DEFINITION_MISSING;
  }
  return selectedDefinition;
}


/* Recursive part of ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology: unlocks the technology of the
   linked definition the faction can select at this node (linked ids at +0x20), then does the same for every
   child (child count +0x08, children +0x0C + 4*i). */
static void ModelDefinitionHierarchy_UnlockFrom(FactionRuntimeIndex factionIndex,ArmyModelTreeNode *node)
{
  uint32_t childIndex;
  ModelDefinition_UnlockLinkedTechnologyForFaction
            (factionIndex,ModelDefinition_SelectFactionUnlockedLinkedId
                                    (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node));
  for (childIndex = 0; childIndex < node->childCount; childIndex++) {
    ModelDefinitionHierarchy_UnlockFrom(factionIndex,node->children[childIndex]);
  }
}

/* Address: 0x0051DB00.
   Walks the model-definition tree below definitionNode depth-first and, for every node, unlocks for the
   faction the technology granted by the linked definition the faction can currently select
   (ModelDefinition_SelectFactionUnlockedLinkedId). Used when an army is created with
   ARMY_CREATE_UNLOCK_TECHNOLOGY.
*/
void ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  ModelDefinitionHierarchy_UnlockFrom(
       factionIndex,(ArmyModelTreeNode *)((ArmyAssetRecordPrefix *)(uintptr_t)definitionNode)->rootNodeOffsetOrPointer);
}


/* Recursive part of ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction: true (CF set) as soon as
   this node's definition id (+0x20) or one in its subtree (child count +0x08, children +0x0C + 4*i) names a
   technology the faction has not unlocked yet. */
static bool ModelDefinitionHierarchy_AnyTechnologyFrom(uint32_t *technologyMasks,ArmyModelTreeNode *node)
{
  uint32_t childIndex;
  /* true from this check means the technology is still locked */
  if (ModelDefinition_IsFactionTechnologyLocked
                (technologyMasks,node->linkedDefinitionIds[0])) {
    return true;
  }
  for (childIndex = 0; childIndex < node->childCount; childIndex++) {
    if (ModelDefinitionHierarchy_AnyTechnologyFrom(technologyMasks,node->children[childIndex])) {
      return true;
    }
  }
  return false;
}

/* Address: 0x0051DA60.
   Walks the model-definition hierarchy below definitionNode and tests each definition's technology
   requirement against the faction's technology masks. Returns false (CF clear) when every definition in the
   tree is unlocked, true (CF set) as soon as one is still locked: ModelDefinition_IsFactionTechnologyLocked
   reports a locked technology with CF set.
*/
bool ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  return ModelDefinitionHierarchy_AnyTechnologyFrom
                   (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                    (ArmyModelTreeNode *)((ArmyAssetRecordPrefix *)(uintptr_t)definitionNode)->rootNodeOffsetOrPointer);
}


/* Address: 0x00528950.
   Checks that the asset is an 'mdl' of converter version 0x8000A, then registers each of its variable-size
   model-definition records (starting at +0x200, each prefixed with its byte size) and resolves their
   references against the asset base. Returns true on success; returns false with the error code in *outError
   (untouched on success) for an invalid header or at the first record that fails. (The original's success
   EAX, the last registration's value, was read by no caller.)
*/
bool ModelAsset_PrepareRecords(ModelAssetHeader *asset,uint32_t *outError)

{
  uint32_t registrationStatusCode;
  AssetRecordCount recordsRemaining;
  ModelDefinitionResolveView *definition;

  registrationStatusCode = FATAL_ERROR_MODEL_ASSET_INVALID;
  if (asset->recordCountHeader.common.magic == ASSET_MAGIC_MDL &&
      asset->recordCountHeader.common.converterVersion == PCK_CONVERTER_MDL_0008000A) {
    definition = (ModelDefinitionResolveView *)(asset + 1);
    for (recordsRemaining = asset->recordCountHeader.recordCount; recordsRemaining != 0; recordsRemaining--) {
      if (!ModelDefinition_RegisterAndResolveReferences(definition,asset,&registrationStatusCode)) break;
      /* advance by the record's leading byte size */
      definition = (ModelDefinitionResolveView *)((uint8_t *)definition + definition->byteSize);
    }
    if (recordsRemaining == 0) {
      return true;
    }
  }
  *outError = registrationStatusCode;
  return false;
}


/* Address: 0x004BE670.
   Looks up the model's packed point table (entry count +0xE8, offset +0xE4, 0x10-byte ModelPackedPointRecord
   entries) for the key (keyIndex << 4) | keyClass. Returns true when an entry matches and stores its local
   position (the dwords at +4, +8, +0xC) in *outLocalPosition; returns false and stores (0, 0, 0) when no entry
   matches. outLocalPosition may be NULL when only presence matters. Called directly by
   ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters (a model class-init callback table slot) and by the
   army platform-lowering step in gameplay/army/runtime.c.
*/
bool ModelLookupTable_GetPackedPointPosition
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResource *modelDefinition,GraphicsFixedVec3 *outLocalPosition)

{
  int entriesRemaining;
  ModelPackedPointRecord *entryCursor;

  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  entryCursor =
       (ModelPackedPointRecord *)((uint8_t *)modelDefinition + modelDefinition->packedLookupTableRelativeOffset);
  for (; entriesRemaining != 0; entriesRemaining--) {
    if ((keyClass | keyIndex << 4) == entryCursor->packedLookupKey) {
      if (outLocalPosition != NULL) {
        *outLocalPosition = entryCursor->localPosition;
      }
      return true;
    }
    entryCursor++; /* next 0x10-byte entry */
  }
  if (outLocalPosition != NULL) {
    outLocalPosition->x = 0;
    outLocalPosition->y = 0;
    outLocalPosition->z = 0;
  }
  return false;
}


/* Address: 0x004BE6F0.
   Looks up the model's packed point table (entry count +0xE8, offset +0xE4, 0x10-byte entries) for the key
   (keyIndex << 4) | keyClass. Returns true and stores the matching entry in *outEntry when one matches;
   otherwise returns false and stores the address just past the table's last entry in *outEntry (one caller,
   ArmyPlacement_CanPlaceAnchoredModel, reads it anyway).
*/
bool ModelLookupTable_FindPackedPoint(ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResource *modelDefinition,ModelPackedPointRecord **outEntry)

{
  ModelPackedPointRecord *entryCursor;
  int entriesRemaining;

  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  entryCursor =
       (ModelPackedPointRecord *)((uint8_t *)modelDefinition + modelDefinition->packedLookupTableRelativeOffset);
  for (; entriesRemaining != 0; entriesRemaining--) {
    if ((keyClass | keyIndex << 4) == entryCursor->packedLookupKey) {
      *outEntry = entryCursor;
      return true;
    }
    entryCursor++;
  }
  *outEntry = entryCursor;
  return false;
}


/* Address: 0x0050AEA0.
   Intersects the current model-space pick ray (g_ModelRaycastLocalOrigin*, g_ModelRaycastLocalDirection*Q28,
   limited to g_ModelRaycastMaximumDistance) with one triangle: first the plane distance along the ray (plane
   through the weighted centre (2*v0 + v1 + v2) / 4), then an inside test of the hit point against the edges.
   Returns true on a hit and stores the Q12 distance along the ray in *outDistanceQ12; returns false on a miss
   and leaves *outDistanceQ12 unchanged. Called for each triangle by ModelNodeRuntime_RaycastHierarchyNearest.
*/
bool ModelMesh_IntersectTriangleRayDistance(ModelRaycastTriangleDescriptor *triangle,Q12 *outDistanceQ12)

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
  Q12 hitDistanceQ12;
  bool hitFound;
  bool planeOutOfRange;

  normalX = triangle->planeNormalX << (Q28_SHIFT - Q12_SHIFT);
  normalY = triangle->planeNormalY << (Q28_SHIFT - Q12_SHIFT);
  normalZ = triangle->planeNormalZ << (Q28_SHIFT - Q12_SHIFT);
  vertexA = triangle->vertex0;
  vertexB = triangle->vertex1;
  vertexC = triangle->vertex2;
  planeOffsetDot = (int64_t)normalY *
          (int64_t)((vertexA->y * 2 + vertexB->y + vertexC->y >> 2) - g_ModelRaycastLocalOriginY) +
          (int64_t)((vertexA->x * 2 + vertexB->x + vertexC->x >> 2) - g_ModelRaycastLocalOriginX) *
          (int64_t)normalX +
          (int64_t)((vertexA->z * 2 + vertexB->z + vertexC->z >> 2) - g_ModelRaycastLocalOriginZ) *
          (int64_t)normalZ;
  offsetHighOrCrossZ = (uint32_t)(planeOffsetDot >> 32);
  productScratch = (int64_t)(int)g_ModelRaycastLocalDirectionYQ28 * (int64_t)normalY +
          (int64_t)(int)g_ModelRaycastLocalDirectionXQ28 * (int64_t)normalX +
          (int64_t)(int)g_ModelRaycastLocalDirectionZQ28 * (int64_t)normalZ;
  directionDotOrCrossY = FIXED_PRODUCT_SHR(productScratch,Q28_SHIFT);
  if (directionDotOrCrossY != 0) {
    scaledHighOrEdge1X =
         FIXED_MUL_HIGH((int)g_ModelRaycastMaximumDistance,(int)directionDotOrCrossY);
    halfOffsetOrEdge1Y = (int)offsetHighOrCrossZ >> 1;
    /* Range test: the plane distance must lie within the maximum distance on the ray's side. */
    if ((int64_t)planeOffsetDot < 0) {
      planeOutOfRange = (int)offsetHighOrCrossZ < scaledHighOrEdge1X ||
          (halfOffsetOrEdge1Y <= (int)-directionDotOrCrossY && halfOffsetOrEdge1Y <= (int)directionDotOrCrossY);
    }
    else {
      planeOutOfRange = scaledHighOrEdge1X < (int)offsetHighOrCrossZ ||
             ((int)-directionDotOrCrossY <= halfOffsetOrEdge1Y && (int)directionDotOrCrossY <= halfOffsetOrEdge1Y);
    }
    if (planeOutOfRange) {
      return false;
    }
    hitDistanceQ12 =
         (int)((int64_t)planeOffsetDot / (int64_t)(int)directionDotOrCrossY); /* IDIV of EDX:EAX */
    vertexA = triangle->vertex0;
    negVertex0XOrEdge2X = -vertexA->x;
    hitRelativeX = FIXED_MUL_SHR(hitDistanceQ12,(int)g_ModelRaycastLocalDirectionXQ28,Q28_SHIFT) + g_ModelRaycastLocalOriginX + negVertex0XOrEdge2X;
    negVertex0YOrEdge2Y = -vertexA->y;
    hitRelativeY = FIXED_MUL_SHR((int)g_ModelRaycastLocalDirectionYQ28,hitDistanceQ12,Q28_SHIFT) + g_ModelRaycastLocalOriginY + negVertex0YOrEdge2Y;
    negVertex0ZOrEdge2Z = -vertexA->z;
    hitRelativeZ = FIXED_MUL_SHR((int)g_ModelRaycastLocalDirectionZQ28,hitDistanceQ12,Q28_SHIFT) + g_ModelRaycastLocalOriginZ + negVertex0ZOrEdge2Z;
    vertexA = triangle->vertex1;
    vertexB = triangle->vertex2;
    scaledHighOrEdge1X = negVertex0XOrEdge2X + vertexA->x;
    halfOffsetOrEdge1Y = negVertex0YOrEdge2Y + vertexA->y;
    edge1Z = negVertex0ZOrEdge2Z + vertexA->z;
    negVertex0XOrEdge2X = negVertex0XOrEdge2X + vertexB->x;
    negVertex0YOrEdge2Y = negVertex0YOrEdge2Y + vertexB->y;
    negVertex0ZOrEdge2Z = negVertex0ZOrEdge2Z + vertexB->z;
    productScratch = (int64_t)normalY * (int64_t)edge1Z - (int64_t)normalZ * (int64_t)halfOffsetOrEdge1Y;
    distanceOrCrossX = FIXED_PRODUCT_SHR(productScratch,Q28_SHIFT);
    productScratch = (int64_t)normalZ * (int64_t)scaledHighOrEdge1X - (int64_t)normalX * (int64_t)edge1Z;
    directionDotOrCrossY = FIXED_PRODUCT_SHR(productScratch,Q28_SHIFT);
    productScratch =
         (int64_t)normalX * (int64_t)halfOffsetOrEdge1Y - (int64_t)normalY * (int64_t)scaledHighOrEdge1X;
    offsetHighOrCrossZ = FIXED_PRODUCT_SHR(productScratch,Q28_SHIFT);
    productScratch = (int64_t)hitRelativeY * (int64_t)(int)directionDotOrCrossY +
                     (int64_t)hitRelativeX * (int64_t)(int)distanceOrCrossX +
                     (int64_t)hitRelativeZ * (int64_t)(int)offsetHighOrCrossZ;
    edge2Dot = (int64_t)negVertex0YOrEdge2Y * (int64_t)(int)directionDotOrCrossY +
               (int64_t)(int)distanceOrCrossX * (int64_t)negVertex0XOrEdge2X +
               (int64_t)negVertex0ZOrEdge2Z * (int64_t)(int)offsetHighOrCrossZ;
    scaledHighOrEdge1X = (int)((uint64_t)edge2Dot >> 32);
    hitCrossXOrEdge2HitDot = (int64_t)normalY * (int64_t)hitRelativeZ - (int64_t)normalZ * (int64_t)hitRelativeY;
    hitCrossY = (int64_t)normalZ * (int64_t)hitRelativeX - (int64_t)normalX * (int64_t)hitRelativeZ;
    hitCrossZ = (int64_t)normalX * (int64_t)hitRelativeY - (int64_t)normalY * (int64_t)hitRelativeX;
    hitCrossXOrEdge2HitDot = (int64_t)negVertex0YOrEdge2Y *
             (int64_t)(int)FIXED_PRODUCT_SHR(hitCrossY,Q28_SHIFT) +
             (int64_t)negVertex0XOrEdge2X *
             (int64_t)(int)FIXED_PRODUCT_SHR(hitCrossXOrEdge2HitDot,Q28_SHIFT) +
             (int64_t)negVertex0ZOrEdge2Z *
             (int64_t)(int)FIXED_PRODUCT_SHR(hitCrossZ,Q28_SHIFT);
    distanceOrCrossX = (uint32_t)hitCrossXOrEdge2HitDot;
    /* Inside test: both barycentric dots and their sum's difference to edge2Dot carry edge2Dot's sign. */
    if (edge2Dot < 0) {
      hitFound = ((hitCrossXOrEdge2HitDot < 0) && (productScratch < 0)) &&
         (distanceOrCrossX = (uint32_t)(hitCrossXOrEdge2HitDot + productScratch),
         (int)((scaledHighOrEdge1X - (int)((uint64_t)(hitCrossXOrEdge2HitDot + productScratch) >> 32)) -
               (uint32_t)((uint32_t)edge2Dot < distanceOrCrossX)) < 0);
    }
    else {
      hitFound = ((-1 < hitCrossXOrEdge2HitDot) && (-1 < productScratch)) &&
            (distanceOrCrossX = (uint32_t)(hitCrossXOrEdge2HitDot + productScratch),
            -1 < (int)((scaledHighOrEdge1X - (int)((uint64_t)(hitCrossXOrEdge2HitDot + productScratch) >> 32)) -
                      (uint32_t)((uint32_t)edge2Dot < distanceOrCrossX)));
    }
    if (hitFound) {
      *outDistanceQ12 = hitDistanceQ12;
      return true;
    }
  }
  return false;
}


/* Address: 0x005289C0.
   Looks a model definition up by id in the 768-slot registry and returns 0 with its build costs, the three
   dwords at record +0x188 (*outEnergyLoadQ4), +0x180 (*outBuildTicks) and +0x184 (*outXeniteCostQ4), which
   ArmyAssetRecord_RelocateModelTree adds to an army record. On a miss the id is formatted into
   g_PackageLastErrorPath, the out-parameters are left unchanged and FATAL_ERROR_MODEL_DEFINITION_MISSING is
   returned.
*/
uint32_t ModelDefinitionRegistry_FindBuildCostsById
          (PckModelDefinitionIdCatalog definitionId,uint32_t *outEnergyLoadQ4,uint32_t *outBuildTicks,
           uint32_t *outXeniteCostQ4)

{
  ModelDefinitionRecordPrefix *registeredDefinition;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;

  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = MODEL_DEFINITION_REGISTRY_SLOT_COUNT;
  while (registeredDefinition = *registryCursor, registeredDefinition == NULL || definitionId != registeredDefinition->definitionId) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
      return FATAL_ERROR_MODEL_DEFINITION_MISSING;
    }
  }
  *outBuildTicks = ((ModelDefinition *)registeredDefinition)->buildTicks;
  *outEnergyLoadQ4 = ((ModelDefinition *)registeredDefinition)->buildEnergyLoadQ4;
  *outXeniteCostQ4 = ((ModelDefinition *)registeredDefinition)->xeniteValueQ4;
  return 0;
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
  registrySlotsRemaining = MODEL_DEFINITION_REGISTRY_SLOT_COUNT;
  while ((candidateDefinition = *registryCursor,
         candidateDefinition == NULL ||
         (runtimeClassId != ((ModelDefinition *)candidateDefinition)->requiredTechnologyBit))) {
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
PckModelDefinitionIdCatalog ModelDefinition_SelectFactionUnlockedLinkedId
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList)

{
  PckModelDefinitionIdCatalog linkedDefinitionId;
  int linkedSlotsRemaining;
  PckModelDefinitionIdCatalog selectedDefinitionId;
  bool technologyLocked;

  selectedDefinitionId = ((ArmyModelTreeNode *)linkedDefinitionList)->linkedDefinitionIds[0];
  for (linkedSlotsRemaining = MODEL_LINKED_DEFINITION_COUNT; linkedSlotsRemaining != 0; linkedSlotsRemaining--) {
    /* the list cursor advances by one id, so linkedDefinitionIds[0] is the current slot */
    linkedDefinitionId = ((ArmyModelTreeNode *)linkedDefinitionList)->linkedDefinitionIds[0];
    if (linkedDefinitionId != 0) {
      /* true (CF set) means the technology is still locked */
      technologyLocked = ModelDefinition_IsFactionTechnologyLocked
                        (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,
                         linkedDefinitionId);
      if (!technologyLocked) {
        selectedDefinitionId = linkedDefinitionId;
      }
    }
    linkedDefinitionList = linkedDefinitionList + 4;
  }
  return selectedDefinitionId;
}


/* Serialized model node tree: flags +0x04 (low nibble 0 = has a sprite), sprite path +0x38, sprite
   asset +0x30, owned-copy count +0x34, child count +0x14, child offsets +0x18 + 4*i (relative to the
   asset, relocated in place). Loads or reuses each node's sprite; true (CF) with *error on failure. */
static bool ModelDefinition_ResolveNodeSprites(MdlSerializedNodeHeader *node,uint8_t *asset,uint32_t *error)
{
  uint32_t childIndex;
  if ((node->nodeFlags & 0xf) == 0) {
    uint16_t *spritePath = (uint16_t *)(node + 1);
    SpriteAssetHeader *loadedSprite;
    SpriteAssetHeader *registered;
    /* ".spr". The original tests its CF (JC at 0x005286C6), but WidePath_SetExtensionCode always
       returns with CLC (0x0040F314), so that branch is dead. The error exit at 0x00528677 only drops the
       walk's stack frames before MOV ESP,EBP; returning up the recursion is equivalent. */
    WidePath_SetExtensionCode(ASSET_MAGIC_SPR,spritePath);
    loadedSprite = Package_LoadEntry(spritePath,error);
    if (loadedSprite == NULL) {
      return true;
    }
    registered = SpriteAssetRegistry_FindById(loadedSprite->registryHeader.registryId);
    if (registered == NULL) {
      /* first use of this sprite: the node owns the loaded copy and registers it */
      uint32_t relocateError;
      node->ownedNestedResourcePresent++;
      node->spriteAssetReference.spriteAsset = loadedSprite;
      relocateError = SpriteAsset_RegisterAndRelocatePointers(loadedSprite);
      if (relocateError != 0) {
        *error = relocateError;
        return true;
      }
    }
    else {
      /* already registered by another model: share it and drop the fresh copy */
      node->spriteAssetReference.spriteAsset = registered;
      Resource_Release(loadedSprite);
    }
  }
  for (childIndex = 0; childIndex < (uint32_t)node->childCount; childIndex++) {
    /* relocate the child offset to a pointer in place */
    node->childSerializedOffsets[childIndex] = node->childSerializedOffsets[childIndex] + (int)(uintptr_t)asset;
    if (ModelDefinition_ResolveNodeSprites
                  ((MdlSerializedNodeHeader *)(uintptr_t)node->childSerializedOffsets[childIndex],asset,error)) {
      return true;
    }
  }
  return false;
}

/* Address: 0x00528600.
   Registers one MDL model definition in the first free slot of the 768-slot registry and turns its
   serialized references into runtime pointers: the node tree is relocated by the asset base and its
   sprites are loaded or reused, the shot and effect ids are resolved through their registries, and the
   terrain-class dependent placement values are copied from the grid tables. Returns true on success; a
   duplicate id, a full registry or any failed load/lookup returns false with its error code in *outError
   (untouched on success; the original's success EAX was read by no caller).
*/
bool ModelDefinition_RegisterAndResolveReferences
          (ModelDefinitionResolveView *definition,ModelAssetHeader *asset,uint32_t *outError)

{
  uint32_t nodeOffsetOrGridClass;
  uint32_t secondaryThreshold;
  uint32_t resolverStatusOrSentinel;
  ShotDefinition *resolvedShot;
  ShotDefinition *resolvedEmitterShot;
  int slotsRemainingOrClassIndex;
  ModelDefinitionRecordPrefix **registrySlotCursor;
  MdlSerializedNodeHeader *serializedNodeCursor;

  registrySlotCursor = g_ModelDefinitionRegistry;
  slotsRemainingOrClassIndex = MODEL_DEFINITION_REGISTRY_SLOT_COUNT;
  if (ModelDefinitionRegistry_FindById(definition->definitionId) != NULL) {
    /* duplicate id: the id is left in g_PackageLastErrorPath */
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    resolverStatusOrSentinel = FATAL_ERROR_MODEL_ID_DUPLICATE;
    goto ReturnFailure;
  }
  while (*registrySlotCursor != NULL) {
    registrySlotCursor++;
    slotsRemainingOrClassIndex--;
    if (slotsRemainingOrClassIndex == 0) {
      /* registry full: the slot count is left in g_PackageLastErrorPath */
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,MODEL_DEFINITION_REGISTRY_SLOT_COUNT,g_PackageLastErrorPath);
      resolverStatusOrSentinel = FATAL_ERROR_MODEL_REGISTRY_FULL;
      goto ReturnFailure;
    }
  }
  nodeOffsetOrGridClass = definition->rootNodeOffsetOrPointer;
  *registrySlotCursor = (ModelDefinitionRecordPrefix *)definition;
  if (nodeOffsetOrGridClass != 0) {
    /* asset start + serialized offset */
    definition->rootNodeOffsetOrPointer =
         (uint32_t)((uint8_t *)asset + definition->rootNodeOffsetOrPointer);
    serializedNodeCursor = (MdlSerializedNodeHeader *)((uint8_t *)asset + nodeOffsetOrGridClass);
    /* Rewritten from the assembly (0x0052869F-0x00528744): the node tree walk kept its
       {node, nextChild, remaining} frames on the machine stack; Ghidra only followed child 0. */
    if (ModelDefinition_ResolveNodeSprites(serializedNodeCursor,(uint8_t *)asset,&resolverStatusOrSentinel)) {
      goto ReturnFailure;
    }
  }
  resolverStatusOrSentinel = ShotDefinitionRegistry_FindByIdWithError
                     ((PckShotDefinitionIdCatalog)definition->shotDefinitionReference,&resolvedShot);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  definition->shotDefinitionReference = resolvedShot;
  /* each effect field holds its serialized id until the lookup replaces it by the definition */
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->destructionEffect0,&definition->destructionEffect0);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->destructionEffect1,&definition->destructionEffect1);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->destructionEffect2,&definition->destructionEffect2);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->destructionEffect3,&definition->destructionEffect3);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->destructionEffect4,&definition->destructionEffect4);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->destructionEffect5,&definition->destructionEffect5);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->destructionEffect6,&definition->destructionEffect6);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->destructionEffect7,&definition->destructionEffect7);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  /* -1: the definition has no shot at +0x168 */
  if (definition->emitterShotDefinitionReference != (ShotDefinition *)0xffffffff) {
    resolverStatusOrSentinel = ShotDefinitionRegistry_FindByIdWithError
                       ((PckShotDefinitionIdCatalog)definition->emitterShotDefinitionReference,&resolvedEmitterShot);
    if (resolverStatusOrSentinel != 0) goto ReturnFailure;
    definition->emitterShotDefinitionReference = resolvedEmitterShot;
  }
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->emitterEffectDefinitionReference,
                      &definition->emitterEffectDefinitionReference);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->waterEmitterEffectDefinitionReference,
                      &definition->waterEmitterEffectDefinitionReference);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->removalEffectDefinitionReference,
                      &definition->removalEffectDefinitionReference);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  resolverStatusOrSentinel = EffectDefinitionRegistry_FindById
                     ((PckEffectDefinitionIdCatalog)definition->damageEffectDefinitionReference,
                      &definition->damageEffectDefinitionReference);
  if (resolverStatusOrSentinel != 0) goto ReturnFailure;
  /* negative grid classes keep the serialized values; the contact kind at +0x278 selects which grid tables
     the class at +0x264 indexes (kind 4 from class 1, the fallback tables from class 4) */
  nodeOffsetOrGridClass = definition->terrainTraversalClass;
  if (-1 < (int)definition->footprintRadiusClass) {
    resolverStatusOrSentinel = (&g_GridInfluenceRadiusOffset0)[definition->footprintRadiusClass];
    definition->footprintRadius = resolverStatusOrSentinel;
    definition->footprintRadiusCopy = resolverStatusOrSentinel;
  }
  if (-1 < (int)nodeOffsetOrGridClass) {
    if (definition->placementContactKindIndex == 1) {
      resolverStatusOrSentinel = (&g_GridTerrainClassBit24MaxWaterSurfaceDelta)[nodeOffsetOrGridClass];
      nodeOffsetOrGridClass = (&g_GridTerrainClassBit28MaxTriangle0NormalAngleHigh16)[nodeOffsetOrGridClass];
      ((ModelDefinition *)definition)->classParameterCC = resolverStatusOrSentinel;
      definition->runtimeValue24 = nodeOffsetOrGridClass;
    }
    else if (definition->placementContactKindIndex == 4) {
      resolverStatusOrSentinel =
           g_ModelTraversalClass4SecondaryThresholdTable3[nodeOffsetOrGridClass - 1];
      definition->runtimeValue24 =
           (&g_GridTerrainClassBit25MaxSelectedNormalAngleHigh16)[nodeOffsetOrGridClass - 1];
      definition->traversalSecondaryThreshold = resolverStatusOrSentinel;
    }
    else {
      slotsRemainingOrClassIndex = nodeOffsetOrGridClass - 4;
      resolverStatusOrSentinel = (&g_GridTerrainClassBit28MinWaterSurfaceDelta)[slotsRemainingOrClassIndex];
      nodeOffsetOrGridClass = (&g_GridTerrainClassBit28MaxTriangle0NormalAngleHigh16)[slotsRemainingOrClassIndex];
      secondaryThreshold =
           g_ModelTraversalFallbackSecondaryThresholdTable3[slotsRemainingOrClassIndex];
      definition->waterDamageThreshold = resolverStatusOrSentinel;
      definition->runtimeValue24 = nodeOffsetOrGridClass;
      definition->traversalSecondaryThreshold = secondaryThreshold;
    }
  }
  return true;
ReturnFailure:
  *outError = resolverStatusOrSentinel;
  return false;
}


/* Address: 0x0052ADE0.
   Unlocks for the faction the technology that the model definition grants (record +0x1C4), so building
   that model makes its successor technology available. An unknown id is silently ignored.
*/
void ModelDefinition_UnlockLinkedTechnologyForFaction
          (FactionRuntimeIndex factionIndex,PckModelDefinitionIdCatalog modelDefinitionId)

{
  ModelDefinitionRecordPrefix *modelDefinition;

  modelDefinition = ModelDefinitionRegistry_FindById(modelDefinitionId);
  if (modelDefinition != NULL) {
    Technology_UnlockForFaction
              (0,0,((ModelDefinition *)modelDefinition)->researchTechnologyIds[0],factionIndex);
  }
}


/* Address: 0x0052AD90.
   Tests whether the faction may use the model definition: the technology bit it requires (record +0x1C0)
   must be set in the faction's 256-bit technology masks. True (CF set) means locked
   (bit clear or unknown id); false (CF clear) means unlocked.
*/
bool ModelDefinition_IsFactionTechnologyLocked
          (uint32_t *factionTechnologyMasks,PckModelDefinitionIdCatalog modelDefinitionId)

{
  uint32_t technologyBitIndex;
  ModelDefinitionRecordPrefix *modelDefinition;

  modelDefinition = ModelDefinitionRegistry_FindById(modelDefinitionId);
  if ((modelDefinition != NULL) &&
     (technologyBitIndex = ((ModelDefinition *)modelDefinition)->requiredTechnologyBit,
     (factionTechnologyMasks[technologyBitIndex >> 5] & 1 << ((uint8_t)technologyBitIndex & 31)) != 0)) {
    return false;
  }
  return true;
}


/* Address: 0x00528E20.
   Looks a model definition up by id in the 768-slot registry. On a miss it writes a number into
   g_PackageLastErrorPath for the error message and returns NULL (the original returned
   FATAL_ERROR_MODEL_DEFINITION_MISSING with CF set; callers that passed that code on now supply it
   themselves). A found definition is never NULL.
*/
ModelDefinitionRecordPrefix *ModelDefinitionRegistry_FindById(PckModelDefinitionIdCatalog definitionId)

{
  ModelDefinitionRecordPrefix *registeredDefinition;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;

  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = MODEL_DEFINITION_REGISTRY_SLOT_COUNT;
  while (registeredDefinition = *registryCursor, registeredDefinition == NULL || registeredDefinition->definitionId != definitionId) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      /* the original formats EAX, i.e. the last registry slot, not the missing id (PUSH EAX at 0x00528E59) */
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)registeredDefinition,g_PackageLastErrorPath);
      return NULL;
    }
  }
  return registeredDefinition;
}

