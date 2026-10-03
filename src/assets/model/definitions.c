/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/model/definitions.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/model/definitions.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/model/definitions. */

/* Picks the upgrade stage a faction can build: of the eight linked model-definition ids at +0x20 the last
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

/* Walks the model-definition tree below definitionNode depth-first and, for every node, unlocks for the
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

/* Walks the model-definition hierarchy below definitionNode and tests each definition's technology
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


/* Checks that the asset is an 'mdl' of converter version 0x8000A, then registers each of its variable-size
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


/* Looks up the model's packed point table (entry count +0xE8, offset +0xE4, 0x10-byte ModelPackedPointRecord
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


/* Looks up the model's packed point table (entry count +0xE8, offset +0xE4, 0x10-byte entries) for the key
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


/* Intersects the current model-space pick ray (g_ModelRaycastLocalOrigin*, g_ModelRaycastLocalDirection*Q28,
   limited to g_ModelRaycastMaximumDistance) with one triangle: first the plane distance along the ray (plane
   through the weighted centre (2*v0 + v1 + v2) / 4), then an inside test of the hit point against the edges.
   Returns true on a hit and stores the Q12 distance along the ray in *outDistanceQ12; returns false on a miss
   and leaves *outDistanceQ12 unchanged. Called for each triangle by ModelNodeRuntime_RaycastHierarchyNearest.
*/
bool ModelMesh_IntersectTriangleRayDistance(ModelRaycastTriangleDescriptor *triangle,Q12 *outDistanceQ12)

{
  GraphicsFixedVec3 *vertex0;
  GraphicsFixedVec3 *vertex1;
  GraphicsFixedVec3 *vertex2;
  int normalX;
  int normalY;
  int normalZ;
  int64_t planeOffsetDot;
  int planeOffsetHigh;
  int halfPlaneOffsetHigh;
  int64_t directionProduct;
  int directionDot;
  int maximumDistanceHigh;
  bool planeOutOfRange;
  Q12 hitDistanceQ12;
  int negVertex0X;
  int negVertex0Y;
  int negVertex0Z;
  int hitRelativeX;
  int hitRelativeY;
  int hitRelativeZ;
  int edge1X;
  int edge1Y;
  int edge1Z;
  int edge2X;
  int edge2Y;
  int edge2Z;
  int normalCrossEdge1X;
  int normalCrossEdge1Y;
  int normalCrossEdge1Z;
  int64_t hitDotNormalCrossEdge1;
  int64_t edge2DotNormalCrossEdge1;
  int64_t normalCrossHitX;
  int64_t normalCrossHitY;
  int64_t normalCrossHitZ;
  int64_t edge2DotNormalCrossHit;
  int64_t insideRemainder;
  bool hitFound;

  normalX = triangle->planeNormalX << (Q28_SHIFT - Q12_SHIFT);
  normalY = triangle->planeNormalY << (Q28_SHIFT - Q12_SHIFT);
  normalZ = triangle->planeNormalZ << (Q28_SHIFT - Q12_SHIFT);
  vertex0 = triangle->vertex0;
  vertex1 = triangle->vertex1;
  vertex2 = triangle->vertex2;
  planeOffsetDot = (int64_t)normalY *
          (int64_t)(((vertex0->y * 2 + vertex1->y + vertex2->y) >> 2) - g_ModelRaycastLocalOrigin.y) +
          (int64_t)(((vertex0->x * 2 + vertex1->x + vertex2->x) >> 2) - g_ModelRaycastLocalOrigin.x) *
          (int64_t)normalX +
          (int64_t)(((vertex0->z * 2 + vertex1->z + vertex2->z) >> 2) - g_ModelRaycastLocalOrigin.z) *
          (int64_t)normalZ;
  planeOffsetHigh = (int)((uint64_t)planeOffsetDot >> 32);
  directionProduct = (int64_t)g_ModelRaycastLocalDirectionQ28.y * (int64_t)normalY +
          (int64_t)g_ModelRaycastLocalDirectionQ28.x * (int64_t)normalX +
          (int64_t)g_ModelRaycastLocalDirectionQ28.z * (int64_t)normalZ;
  directionDot = (int)FIXED_PRODUCT_SHR(directionProduct,Q28_SHIFT);
  if (directionDot == 0) {
    return false;
  }
  maximumDistanceHigh = FIXED_MUL_HIGH((int)g_ModelRaycastMaximumDistance,directionDot);
  halfPlaneOffsetHigh = planeOffsetHigh >> 1;
  /* Range test: the plane distance must lie within the maximum distance on the ray's side. */
  if (planeOffsetDot < 0) {
    planeOutOfRange = planeOffsetHigh < maximumDistanceHigh ||
        (halfPlaneOffsetHigh <= -directionDot && halfPlaneOffsetHigh <= directionDot);
  }
  else {
    planeOutOfRange = maximumDistanceHigh < planeOffsetHigh ||
        (-directionDot <= halfPlaneOffsetHigh && directionDot <= halfPlaneOffsetHigh);
  }
  if (planeOutOfRange) {
    return false;
  }
  hitDistanceQ12 = (int)(planeOffsetDot / (int64_t)directionDot); /* 64-by-32-bit signed division */

  /* hit point relative to vertex 0 */
  negVertex0X = -vertex0->x;
  hitRelativeX = FIXED_MUL_SHR(hitDistanceQ12,g_ModelRaycastLocalDirectionQ28.x,Q28_SHIFT) + g_ModelRaycastLocalOrigin.x + negVertex0X;
  negVertex0Y = -vertex0->y;
  hitRelativeY = FIXED_MUL_SHR(g_ModelRaycastLocalDirectionQ28.y,hitDistanceQ12,Q28_SHIFT) + g_ModelRaycastLocalOrigin.y + negVertex0Y;
  negVertex0Z = -vertex0->z;
  hitRelativeZ = FIXED_MUL_SHR(g_ModelRaycastLocalDirectionQ28.z,hitDistanceQ12,Q28_SHIFT) + g_ModelRaycastLocalOrigin.z + negVertex0Z;
  edge1X = negVertex0X + vertex1->x;
  edge1Y = negVertex0Y + vertex1->y;
  edge1Z = negVertex0Z + vertex1->z;
  edge2X = negVertex0X + vertex2->x;
  edge2Y = negVertex0Y + vertex2->y;
  edge2Z = negVertex0Z + vertex2->z;

  /* normal x edge1 (Q28 normal, so shifted back by 28) */
  normalCrossEdge1X = (int)FIXED_PRODUCT_SHR((int64_t)normalY * (int64_t)edge1Z - (int64_t)normalZ * (int64_t)edge1Y,Q28_SHIFT);
  normalCrossEdge1Y = (int)FIXED_PRODUCT_SHR((int64_t)normalZ * (int64_t)edge1X - (int64_t)normalX * (int64_t)edge1Z,Q28_SHIFT);
  normalCrossEdge1Z = (int)FIXED_PRODUCT_SHR((int64_t)normalX * (int64_t)edge1Y - (int64_t)normalY * (int64_t)edge1X,Q28_SHIFT);
  hitDotNormalCrossEdge1 = (int64_t)hitRelativeY * (int64_t)normalCrossEdge1Y +
                           (int64_t)hitRelativeX * (int64_t)normalCrossEdge1X +
                           (int64_t)hitRelativeZ * (int64_t)normalCrossEdge1Z;
  edge2DotNormalCrossEdge1 = (int64_t)edge2Y * (int64_t)normalCrossEdge1Y +
                             (int64_t)normalCrossEdge1X * (int64_t)edge2X +
                             (int64_t)edge2Z * (int64_t)normalCrossEdge1Z;

  /* normal x hit point, dotted with edge2 */
  normalCrossHitX = (int64_t)normalY * (int64_t)hitRelativeZ - (int64_t)normalZ * (int64_t)hitRelativeY;
  normalCrossHitY = (int64_t)normalZ * (int64_t)hitRelativeX - (int64_t)normalX * (int64_t)hitRelativeZ;
  normalCrossHitZ = (int64_t)normalX * (int64_t)hitRelativeY - (int64_t)normalY * (int64_t)hitRelativeX;
  edge2DotNormalCrossHit = (int64_t)edge2Y * (int64_t)(int)FIXED_PRODUCT_SHR(normalCrossHitY,Q28_SHIFT) +
                           (int64_t)edge2X * (int64_t)(int)FIXED_PRODUCT_SHR(normalCrossHitX,Q28_SHIFT) +
                           (int64_t)edge2Z * (int64_t)(int)FIXED_PRODUCT_SHR(normalCrossHitZ,Q28_SHIFT);

  /* Inside test: both barycentric dots and the remainder edge2DotNormalCrossEdge1 - (their sum) carry the sign
     of edge2DotNormalCrossEdge1 (the 64-bit subtraction wraps like the original SUB/SBB pair). */
  insideRemainder = (int64_t)((uint64_t)edge2DotNormalCrossEdge1 -
                              (uint64_t)(edge2DotNormalCrossHit + hitDotNormalCrossEdge1));
  if (edge2DotNormalCrossEdge1 < 0) {
    hitFound = edge2DotNormalCrossHit < 0 && hitDotNormalCrossEdge1 < 0 && insideRemainder < 0;
  }
  else {
    hitFound = edge2DotNormalCrossHit >= 0 && hitDotNormalCrossEdge1 >= 0 && insideRemainder >= 0;
  }
  if (!hitFound) {
    return false;
  }
  *outDistanceQ12 = hitDistanceQ12;
  return true;
}


/* Looks a model definition up by id in the 768-slot registry and returns 0 with its build costs, the three
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
  for (; registrySlotsRemaining != 0; registrySlotsRemaining--) {
    registeredDefinition = *registryCursor;
    if (registeredDefinition != NULL && registeredDefinition->definitionId == definitionId) {
      *outBuildTicks = ((ModelDefinition *)registeredDefinition)->buildTicks;
      *outEnergyLoadQ4 = ((ModelDefinition *)registeredDefinition)->buildEnergyLoadQ4;
      *outXeniteCostQ4 = ((ModelDefinition *)registeredDefinition)->xeniteValueQ4;
      return 0;
    }
    registryCursor++;
  }
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
  return FATAL_ERROR_MODEL_DEFINITION_MISSING;
}


/* Returns the first registered model definition whose runtime class id (dword +0x1C0) equals runtimeClassId,
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

/* Same selection as ModelDefinition_SelectFactionUnlockedLinkedDefinition, but returns the chosen id
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

/* Puts the definition into the first free slot of the model registry. Returns 0, or the fatal error code
   for a duplicate id (the id is left in g_PackageLastErrorPath) or a full registry (the slot count is
   left there). */
static uint32_t ModelDefinition_ClaimRegistrySlot(ModelDefinitionResolveView *definition)
{
  int slotIndex;

  if (ModelDefinitionRegistry_FindById(definition->definitionId) != NULL) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    return FATAL_ERROR_MODEL_ID_DUPLICATE;
  }
  for (slotIndex = 0; slotIndex < MODEL_DEFINITION_REGISTRY_SLOT_COUNT; slotIndex++) {
    if (g_ModelDefinitionRegistry[slotIndex] == NULL) {
      g_ModelDefinitionRegistry[slotIndex] = (ModelDefinitionRecordPrefix *)definition;
      return 0;
    }
  }
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,MODEL_DEFINITION_REGISTRY_SLOT_COUNT,g_PackageLastErrorPath);
  return FATAL_ERROR_MODEL_REGISTRY_FULL;
}

/* Replaces the serialized shot and effect ids of the definition by the registered definitions, in record
   order. Returns 0, or the error code of the first failed lookup (later fields keep their ids). */
static uint32_t ModelDefinition_ResolveShotAndEffectIds(ModelDefinitionResolveView *definition)
{
  /* each effect field holds its serialized id until the lookup replaces it by the definition */
  EffectDefinition **const destructionEffects[] = {
    &definition->destructionEffect0,&definition->destructionEffect1,
    &definition->destructionEffect2,&definition->destructionEffect3,
    &definition->destructionEffect4,&definition->destructionEffect5,
    &definition->destructionEffect6,&definition->destructionEffect7
  };
  EffectDefinition **const emitterAndRemovalEffects[] = {
    &definition->emitterEffectDefinitionReference,&definition->waterEmitterEffectDefinitionReference,
    &definition->removalEffectDefinitionReference,&definition->damageEffectDefinitionReference
  };
  uint32_t status;
  uint32_t fieldIndex;
  ShotDefinition *resolvedShot;
  ShotDefinition *resolvedEmitterShot;

  status = ShotDefinitionRegistry_FindByIdWithError
                     ((PckShotDefinitionIdCatalog)definition->shotDefinitionReference,&resolvedShot);
  if (status != 0) return status;
  definition->shotDefinitionReference = resolvedShot;
  for (fieldIndex = 0; fieldIndex < 8; fieldIndex++) {
    status = EffectDefinitionRegistry_FindById
                       ((PckEffectDefinitionIdCatalog)*destructionEffects[fieldIndex],destructionEffects[fieldIndex]);
    if (status != 0) return status;
  }
  /* -1: the definition has no shot at +0x168 */
  if (definition->emitterShotDefinitionReference != (ShotDefinition *)(intptr_t)-1) {
    status = ShotDefinitionRegistry_FindByIdWithError
                       ((PckShotDefinitionIdCatalog)definition->emitterShotDefinitionReference,&resolvedEmitterShot);
    if (status != 0) return status;
    definition->emitterShotDefinitionReference = resolvedEmitterShot;
  }
  for (fieldIndex = 0; fieldIndex < 4; fieldIndex++) {
    status = EffectDefinitionRegistry_FindById
                       ((PckEffectDefinitionIdCatalog)*emitterAndRemovalEffects[fieldIndex],
                        emitterAndRemovalEffects[fieldIndex]);
    if (status != 0) return status;
  }
  return 0;
}

/* Copies the terrain-class dependent placement values from the grid tables. Negative classes keep the
   serialized values; the contact kind at +0x278 selects which grid tables the class at +0x264 indexes
   (kind 4 from class 1, the fallback tables from class 4). */
static void ModelDefinition_CopyTerrainClassValues(ModelDefinitionResolveView *definition)
{
  uint32_t terrainClass;
  uint32_t footprintRadius;

  terrainClass = definition->terrainTraversalClass;
  if (-1 < (int)definition->footprintRadiusClass) {
    /* Original quirk: no upper bound on the class; the original would read past the 8 radius offsets into
       g_GridInfluenceSquaredThreshold for a class >= 8. The shipped model definitions only use -1..7. */
    footprintRadius =g_GridInfluenceRadiusOffset[definition->footprintRadiusClass];
    definition->footprintRadius = footprintRadius;
    definition->footprintRadiusCopy = footprintRadius;
  }
  if ((int)terrainClass < 0) return;
  /* All lookups index one threshold table from different entries; the class is not bounded, so a class
     outside the run of its entry reads the neighbouring entries (as in the original). */
  if (definition->placementContactKindIndex == 1) {
    uint32_t maxWaterSurfaceDelta =
         g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT24_MAX_WATER_SURFACE_DELTA + terrainClass];
    uint32_t maxNormalAngle =
         g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT28_MAX_TRIANGLE0_NORMAL_ANGLE + terrainClass];
    ((ModelDefinition *)definition)->classParameterCC = maxWaterSurfaceDelta;
    definition->runtimeValue24 = maxNormalAngle;
  }
  else if (definition->placementContactKindIndex == 4) {
    uint32_t secondaryThreshold =
         g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_CLASS4_SECONDARY + (terrainClass - 1)];
    definition->runtimeValue24 =
         g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT25_MAX_SELECTED_NORMAL_ANGLE + (terrainClass - 1)];
    definition->traversalSecondaryThreshold = secondaryThreshold;
  }
  else {
    int fallbackIndex = terrainClass - 4;
    uint32_t minWaterSurfaceDelta =
         g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT28_MIN_WATER_SURFACE_DELTA + fallbackIndex];
    uint32_t maxNormalAngle =
         g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT28_MAX_TRIANGLE0_NORMAL_ANGLE + fallbackIndex];
    uint32_t secondaryThreshold =
         g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_FALLBACK_SECONDARY + fallbackIndex];
    definition->waterDamageThreshold = minWaterSurfaceDelta;
    definition->runtimeValue24 = maxNormalAngle;
    definition->traversalSecondaryThreshold = secondaryThreshold;
  }
}

/* Registers one MDL model definition in the first free slot of the 768-slot registry and turns its
   serialized references into runtime pointers: the node tree is relocated by the asset base and its
   sprites are loaded or reused, the shot and effect ids are resolved through their registries, and the
   terrain-class dependent placement values are copied from the grid tables. Returns true on success; a
   duplicate id, a full registry or any failed load/lookup returns false with its error code in *outError
   (untouched on success; the original's success EAX was read by no caller).
*/
bool ModelDefinition_RegisterAndResolveReferences
          (ModelDefinitionResolveView *definition,ModelAssetHeader *asset,uint32_t *outError)

{
  uint32_t status;
  uint32_t rootNodeOffset;

  status = ModelDefinition_ClaimRegistrySlot(definition);
  if (status != 0) {
    *outError = status;
    return false;
  }
  rootNodeOffset = definition->rootNodeOffsetOrPointer;
  if (rootNodeOffset != 0) {
    /* asset start + serialized offset */
    definition->rootNodeOffsetOrPointer = (uint32_t)((uint8_t *)asset + rootNodeOffset);
    /* Rewritten from the assembly (0x0052869F-0x00528744): the node tree walk kept its
       {node, nextChild, remaining} frames on the machine stack; Ghidra only followed child 0. */
    if (ModelDefinition_ResolveNodeSprites
                  ((MdlSerializedNodeHeader *)((uint8_t *)asset + rootNodeOffset),(uint8_t *)asset,&status)) {
      *outError = status;
      return false;
    }
  }
  status = ModelDefinition_ResolveShotAndEffectIds(definition);
  if (status != 0) {
    *outError = status;
    return false;
  }
  ModelDefinition_CopyTerrainClassValues(definition);
  return true;
}


/* Unlocks for the faction the technology that the model definition grants (record +0x1C4), so building
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


/* Tests whether the faction may use the model definition: the technology bit it requires (record +0x1C0)
   must be set in the faction's 256-bit technology masks. True (CF set) means locked
   (bit clear or unknown id); false (CF clear) means unlocked.
*/
bool ModelDefinition_IsFactionTechnologyLocked
          (uint32_t *factionTechnologyMasks,PckModelDefinitionIdCatalog modelDefinitionId)

{
  uint32_t technologyBitIndex;
  ModelDefinitionRecordPrefix *modelDefinition;

  modelDefinition = ModelDefinitionRegistry_FindById(modelDefinitionId);
  if (modelDefinition == NULL) {
    return true;
  }
  technologyBitIndex = ((ModelDefinition *)modelDefinition)->requiredTechnologyBit;
  return (factionTechnologyMasks[technologyBitIndex >> 5] & 1 << ((uint8_t)technologyBitIndex & 31)) == 0;
}


/* Looks a model definition up by id in the 768-slot registry. On a miss it writes a number into
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
  registeredDefinition = NULL;
  for (; registrySlotsRemaining != 0; registrySlotsRemaining--) {
    registeredDefinition = *registryCursor;
    if (registeredDefinition != NULL && registeredDefinition->definitionId == definitionId) {
      return registeredDefinition;
    }
    registryCursor++;
  }
  /* Original quirk: the number formatted is the last registry slot's content, not the missing id
     (PUSH EAX at 0x00528E59) */
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)registeredDefinition,g_PackageLastErrorPath);
  return NULL;
}

