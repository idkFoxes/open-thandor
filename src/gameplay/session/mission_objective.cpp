/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/mission_objective.cpp
 * Project code (not in the original game)
 */

/* open-thandor (objective overlay, docs/plans/objective_overlay.md 3): the local faction's objective from the level
   script. Display only: every function here reads; the live values are the ones InGameScheduledCondition_Holds
   (level_script.cpp) compares, read the same way (no writes, no random draws, no arena allocation). */

#include <thandor/gameplay/session/mission_objective.h>

#include <cstring>
#include <thandor/thandor.h>
#include <thandor/assets/army/catalog.h>
#include <thandor/assets/model/definitions.h>
#include <thandor/assets/text/resources.h>
#include <thandor/assets/text/richtext.h>
#include <thandor/gameplay/session/level_script.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/ui/frontend/common.h>
#include <thandor/ui/ingame/technology.h>

namespace {

/* InGameEndConditionTriggerRecord8.movieVariantSelector of a victory (as the campaign map reads it) */
constexpr uint8_t OBJECTIVE_VICTORY_MOVIE_VARIANT = 1;
/* faction slots of g_GameFactionRuntimeImage */
constexpr uint32_t OBJECTIVE_FACTION_SLOT_COUNT = 8;
/* nested BOOLEAN_POSTFIX_EXPRESSION operands followed at most this deep */
constexpr int OBJECTIVE_EXPRESSION_DEPTH_LIMIT = 4;
/* leaves of one expression tree kept (15 tokens per record, a few nested records) */
constexpr int OBJECTIVE_LEAF_CAPACITY = 32;
/* code units of a name inserted into a template */
constexpr size_t OBJECTIVE_NAME_UNITS = 48;

/* ---- plain UTF-16 text ---------------------------------------------------------------------------------------- */

/* Appends text without its rich-text commands (a fixed space or line break becomes a blank) to out[length..],
   keeping room for the terminator; returns the new length. out is terminated. */
size_t Objective_AppendPlain(uint16_t *out,size_t length,size_t capacity,const uint16_t *text)
{
  if (capacity == 0) {
    return 0;
  }
  while ((text != nullptr) && (*text != 0) && (length + 1 < capacity)) {
    const uint16_t unit = *text;
    if ((unit & RICHTEXT_COMMAND_FLAG) == 0) {
      out[length++] = unit;
      text++;
      continue;
    }
    int recordUnits = 1;
    switch (unit & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      recordUnits = RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      break;
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
      recordUnits = RICHTEXT_RECORD_UNITS_INLINE_VALUE;
      break;
    case RICHTEXT_OP_CALL_NESTED:
    case RICHTEXT_OP_JUMP_NESTED:
      recordUnits = RICHTEXT_RECORD_UNITS_NESTED;
      break;
    case RICHTEXT_OP_INLINE_IMAGE:
      recordUnits = RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      break;
    case RICHTEXT_OP_FIXED_SPACE:
    case RICHTEXT_OP_LINE_BREAK:
      if ((length != 0) && (out[length - 1] != ' ')) {
        out[length++] = ' ';
      }
      break;
    default:
      break;
    }
    text += recordUnits;
  }
  out[length] = 0;
  return length;
}

size_t Objective_AppendNumber(uint16_t *out,size_t length,size_t capacity,uint32_t value,int minimumDigits)
{
  char digits[12];
  int count = 0;

  do {
    digits[count++] = (char)('0' + value % 10);
    value /= 10;
  } while ((value != 0) && (count < (int)sizeof digits));
  while (count < minimumDigits) {
    digits[count++] = '0';
  }
  while ((count > 0) && (length + 1 < capacity)) {
    out[length++] = (uint16_t)digits[--count];
  }
  out[length] = 0;
  return length;
}

/* One value of a template: a name (text != nullptr) or a number. */
struct ObjectiveArgument {
  const uint16_t *text = nullptr;
  uint32_t number = 0;
};

/* Fills in template templateId: "%s" takes the next argument's text, "%u" its number, "%02u" its number with two
   digits, "%%" is a percent sign. Arguments are taken in order. out is terminated and cut to capacity. */
size_t Objective_Format(uint16_t *out,size_t capacity,TextResourceId templateId,const ObjectiveArgument *arguments,
                        int argumentCount)
{
  size_t length = 0;
  int argumentIndex = 0;
  uint16_t *templateText = nullptr;

  if (capacity == 0) {
    return 0;
  }
  out[0] = 0;
  if (!TextResource_TryResolve(templateId,&templateText)) {
    return 0;
  }
  for (const uint16_t *source = templateText; (*source != 0) && (length + 1 < capacity); source++) {
    if (source[0] == '%') {
      const ObjectiveArgument *argument = (argumentIndex < argumentCount) ? &arguments[argumentIndex] : nullptr;
      if (source[1] == '%') {
        out[length++] = '%';
        source++;
        continue;
      }
      if (source[1] == 's') {
        length = Objective_AppendPlain(out,length,capacity,(argument != nullptr) ? argument->text : nullptr);
        argumentIndex++;
        source++;
        continue;
      }
      if (source[1] == 'u') {
        length = Objective_AppendNumber(out,length,capacity,(argument != nullptr) ? argument->number : 0,1);
        argumentIndex++;
        source++;
        continue;
      }
      if ((source[1] == '0') && (source[2] == '2') && (source[3] == 'u')) {
        length = Objective_AppendNumber(out,length,capacity,(argument != nullptr) ? argument->number : 0,2);
        argumentIndex++;
        source += 3;
        continue;
      }
    }
    out[length++] = *source;
  }
  out[length] = 0;
  return length;
}

/* ---- names ---------------------------------------------------------------------------------------------------- */

/* The plain text of resourceId into out; false when the id is missing or its text is empty. */
bool Objective_ResolvePlain(TextResourceId resourceId,uint16_t *out,size_t capacity)
{
  uint16_t *text = nullptr;

  out[0] = 0;
  if (!TextResource_TryResolve(resourceId,&text)) {
    return false;
  }
  size_t length = Objective_AppendPlain(out,0,capacity,text);
  /* trim blanks at both ends */
  size_t start = 0;
  while ((start < length) && (out[start] == ' ')) {
    start++;
  }
  while ((length > start) && (out[length - 1] == ' ')) {
    length--;
  }
  std::memmove(out,out + start,(length - start) * sizeof(uint16_t));
  out[length - start] = 0;
  return out[0] != 0;
}

/* The faction's colour name (TEXT_ID_FACTION_NAME_BASE + colorIndex, as the results table shows it), else its
   number. */
void Objective_FactionName(uint32_t faction,uint16_t *out,size_t capacity)
{
  if ((faction < OBJECTIVE_FACTION_SLOT_COUNT) &&
      Objective_ResolvePlain((TextResourceId)(TEXT_ID_FACTION_NAME_BASE +
                                              (int32_t)g_GameFactionRuntimeImage.records[faction].colorIndex),
                             out,capacity)) {
    return;
  }
  Objective_AppendNumber(out,0,capacity,faction,1);
}

/* The model name of an army asset's root definition (TEXT_ID_MODEL_NAME_BASE + nameTextIndex, the name of the
   selection panel), else "Einheiten". Registry lookups without side effects. */
void Objective_UnitName(uint32_t armyAssetId,uint16_t *out,size_t capacity)
{
  ArmyAssetRecordPrefix *record = ArmyAssetRegistry_FindRecordById((PckArmyAssetIdCatalog)armyAssetId);
  if ((record != nullptr) && (record->rootNodeOffsetOrPointer != 0)) {
    const ArmyModelTreeNode *root = Thandor_U32ToPointer<ArmyModelTreeNode>(record->rootNodeOffsetOrPointer);
    ModelDefinitionRecordPrefix *definition = ModelDefinitionRegistry_LookupById(root->linkedDefinitionIds[0]);
    if ((definition != nullptr) &&
        Objective_ResolvePlain((TextResourceId)(TEXT_ID_MODEL_NAME_BASE + (int32_t)definition->nameTextIndex),out,
                               capacity)) {
      return;
    }
  }
  if (!Objective_ResolvePlain(TEXT_ID_OBJECTIVE_UNIT_FALLBACK,out,capacity)) {
    out[0] = 0;
  }
}

/* ---- conditions ----------------------------------------------------------------------------------------------- */

enum class ObjectiveNameKind : uint8_t { NONE, FACTION, UNIT };

/* One describable part of a victory condition. */
struct ObjectiveItem {
  TextResourceId templateId = TEXT_RESOURCE_ID_NONE;
  ObjectiveNameKind nameKind = ObjectiveNameKind::NONE;
  uint32_t nameParam = 0;
  uint32_t sourceKind = 0; /* the condition kind it came from */
  MissionObjectiveProgress progress;
};

uint32_t Objective_ConditionKind(const InGameScheduledConditionRecord10 &condition)
{
  return condition.statusAndKind.raw & INGAME_SCHEDULED_CONDITION_KIND_MASK;
}

/* The item of one (non-expression) condition seen from localFaction; false when it cannot be described. */
bool Objective_DescribeCondition(const InGameScheduledConditionRecord10 &condition,int conditionIndex,
                                 uint32_t localFaction,bool negated,ObjectiveItem *item)
{
  const uint32_t *operands = condition.payload.operands;
  const uint32_t kind = Objective_ConditionKind(condition);

  *item = ObjectiveItem{};
  item->sourceKind = kind;
  item->progress.conditionIndex = (uint8_t)conditionIndex;
  if (negated) {
    /* the only negated condition of the stock levels: NOT (Tritium rate of a faction >= n), n small */
    if ((kind != INGAME_SCHEDULED_CONDITION_TRITIUM_EXTRACTION_RATE_AT_LEAST) || (operands[0] == localFaction)) {
      return false;
    }
    item->templateId = TEXT_ID_OBJECTIVE_STOP_TRITIUM;
    item->nameKind = ObjectiveNameKind::FACTION;
    item->nameParam = operands[0];
    return true;
  }
  switch (kind) {
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY:
    if ((operands[0] == localFaction) || (operands[0] >= OBJECTIVE_FACTION_SLOT_COUNT)) {
      return false;
    }
    /* one enemy: "Gegner <name> vernichten"; Objective_DescribeVictory makes several "Alle Gegner vernichten" */
    item->templateId = TEXT_ID_OBJECTIVE_DESTROY_FACTION;
    item->nameKind = ObjectiveNameKind::FACTION;
    item->nameParam = operands[0];
    item->progress.kind = MissionObjectiveProgressKind::ENEMY_UNITS;
    item->progress.factionMask = (uint8_t)(1u << operands[0]);
    item->progress.faction = operands[0];
    return true;
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_COMMAND_GROUP_A_ARMY:
    if (operands[0] == localFaction) {
      return false;
    }
    item->templateId = TEXT_ID_OBJECTIVE_DESTROY_MAIN_FORCE;
    return true;
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY_OF_ASSET:
    if (operands[0] == localFaction) {
      return false;
    }
    item->templateId = TEXT_ID_OBJECTIVE_DESTROY_TYPE;
    item->nameKind = ObjectiveNameKind::UNIT;
    item->nameParam = operands[2];
    item->progress.kind = MissionObjectiveProgressKind::ASSET_REMAINING;
    item->progress.faction = operands[0];
    item->progress.armyAssetId = operands[2];
    return true;
  case INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8:
    /* the pair (op1, op0) ends when either is gone or op1 allied op0: the other faction of the pair */
    if (operands[1] == localFaction) {
      item->nameParam = operands[0];
    }
    else if (operands[0] == localFaction) {
      item->nameParam = operands[1];
    }
    else {
      return false;
    }
    item->templateId = TEXT_ID_OBJECTIVE_ELIMINATE_FACTION;
    item->nameKind = ObjectiveNameKind::FACTION;
    return true;
  case INGAME_SCHEDULED_CONDITION_XENITE_AT_LEAST:
    item->templateId = TEXT_ID_OBJECTIVE_COLLECT_XENITE;
    item->progress.kind = MissionObjectiveProgressKind::XENITE;
    item->progress.faction = operands[0];
    item->progress.target = operands[1];
    return true;
  case INGAME_SCHEDULED_CONDITION_TRITIUM_AT_LEAST:
    item->templateId = TEXT_ID_OBJECTIVE_COLLECT_TRITIUM;
    item->progress.kind = MissionObjectiveProgressKind::TRITIUM;
    item->progress.faction = operands[0];
    item->progress.target = operands[1];
    return true;
  case INGAME_SCHEDULED_CONDITION_TRITIUM_EXTRACTION_RATE_AT_LEAST:
    item->templateId = TEXT_ID_OBJECTIVE_RAISE_TRITIUM_RATE;
    item->progress.kind = MissionObjectiveProgressKind::TRITIUM_RATE;
    item->progress.faction = operands[0];
    item->progress.target = operands[1];
    return true;
  case INGAME_SCHEDULED_CONDITION_ARMY_OF_ASSET_COUNT_AT_LEAST:
    if (operands[0] != localFaction) {
      return false;
    }
    item->templateId = TEXT_ID_OBJECTIVE_BUILD_TYPE;
    item->nameKind = ObjectiveNameKind::UNIT;
    item->nameParam = operands[2];
    item->progress.kind = MissionObjectiveProgressKind::ASSET_COUNT;
    item->progress.faction = operands[0];
    item->progress.armyAssetId = operands[2];
    item->progress.target = operands[1];
    return true;
  case INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST:
    item->templateId = TEXT_ID_OBJECTIVE_CONTROL_TERRAIN;
    item->progress.kind = MissionObjectiveProgressKind::TERRAIN_PERCENT;
    item->progress.cellByteOffset = operands[0];
    item->progress.target = operands[1];
    return true;
  case INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED:
    item->templateId = TEXT_ID_OBJECTIVE_HOLD_OUT;
    item->progress.kind = MissionObjectiveProgressKind::COUNTDOWN;
    return true;
  case INGAME_SCHEDULED_CONDITION_XENITE_STORAGE_LIMIT_AT_MOST_0FA0:
    item->templateId = TEXT_ID_OBJECTIVE_SMALL_XENITE_STORAGE;
    return true;
  case INGAME_SCHEDULED_CONDITION_NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A:
    item->templateId = TEXT_ID_OBJECTIVE_KEEP_UNITS_TOGETHER;
    return true;
  default:
    return false;
  }
}

/* The leaves of a condition: itself, or the operands of its postfix expression (nested expressions followed).
   simple stays true while every expression on the way is a valid chain of one operator (AND or OR) whose NOTs
   only negate single operands (NOT x, NOT NOT x): then the condition is that operator over the (negated) leaves. */
struct ObjectiveLeaves {
  int conditions[OBJECTIVE_LEAF_CAPACITY] = {};
  bool negated[OBJECTIVE_LEAF_CAPACITY] = {};
  int count = 0;
  int binaryOperator = 0; /* 0 none yet, else INGAME_CONDITION_TOKEN_AND or _OR */
  bool simple = true;
};

void Objective_CollectLeaves(const InGameConditionSchedule &schedule,int conditionIndex,int depth,
                             ObjectiveLeaves *leaves)
{
  const InGameScheduledConditionRecord10 &condition = schedule.conditions[conditionIndex];

  if (Objective_ConditionKind(condition) != INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION) {
    if (leaves->count < OBJECTIVE_LEAF_CAPACITY) {
      leaves->negated[leaves->count] = false;
      leaves->conditions[leaves->count++] = conditionIndex;
    }
    else {
      leaves->simple = false;
    }
    return;
  }
  if (depth >= OBJECTIVE_EXPRESSION_DEPTH_LIMIT) {
    leaves->simple = false;
    return;
  }
  /* bytes 1..15 of the record, up to the end token */
  const uint8_t *record = reinterpret_cast<const uint8_t *>(&condition);
  int stackDepth = 0;
  bool ended = false;
  int lastLeaf = -1; /* the leaf the previous token pushed: a NOT right after it negates that leaf */
  for (int byteIndex = 1; byteIndex < (int)sizeof(InGameScheduledConditionRecord10); byteIndex++) {
    const int token = record[byteIndex];
    if (token == INGAME_CONDITION_TOKEN_END) {
      ended = true;
      break;
    }
    if (token == INGAME_CONDITION_TOKEN_NOT) {
      if (lastLeaf >= 0) {
        leaves->negated[lastLeaf] = !leaves->negated[lastLeaf];
      }
      else {
        leaves->simple = false; /* NOT over a sub-expression */
      }
      continue;
    }
    lastLeaf = -1;
    if ((token == INGAME_CONDITION_TOKEN_AND) || (token == INGAME_CONDITION_TOKEN_OR)) {
      if ((leaves->binaryOperator != 0) && (leaves->binaryOperator != token)) {
        leaves->simple = false;
      }
      leaves->binaryOperator = token;
      if (stackDepth < 2) {
        leaves->simple = false;
      }
      else {
        stackDepth--;
      }
      continue;
    }
    if (token >= INGAME_SCHEDULED_CONDITION_COUNT) {
      leaves->simple = false; /* rejected by InGameLevelScript_Validate; not followed */
      continue;
    }
    stackDepth++;
    if (token == conditionIndex) {
      leaves->simple = false; /* refers to itself */
      continue;
    }
    const int countBefore = leaves->count;
    Objective_CollectLeaves(schedule,token,depth + 1,leaves);
    /* only a plain condition operand can be negated in place */
    if ((leaves->count == countBefore + 1) && (leaves->conditions[countBefore] == token)) {
      lastLeaf = countBefore;
    }
  }
  if (!ended || (stackDepth != 1)) {
    leaves->simple = false;
  }
}

bool Objective_SameItem(const ObjectiveItem &left,const ObjectiveItem &right)
{
  return (left.templateId == right.templateId) && (left.nameKind == right.nameKind) &&
         ((left.nameKind == ObjectiveNameKind::NONE) || (left.nameParam == right.nameParam));
}

/* The text of one item into out. */
void Objective_FormatItem(const ObjectiveItem &item,uint16_t *out,size_t capacity)
{
  uint16_t name[OBJECTIVE_NAME_UNITS];
  ObjectiveArgument argument;

  name[0] = 0;
  if (item.nameKind == ObjectiveNameKind::FACTION) {
    Objective_FactionName(item.nameParam,name,OBJECTIVE_NAME_UNITS);
  }
  else if (item.nameKind == ObjectiveNameKind::UNIT) {
    Objective_UnitName(item.nameParam,name,OBJECTIVE_NAME_UNITS);
  }
  argument.text = name;
  Objective_Format(out,capacity,item.templateId,&argument,1);
}

/* Line 1 and the progress of the victory condition conditionIndex (plan 3b). */
void Objective_DescribeVictory(const InGameConditionSchedule &schedule,int conditionIndex,uint32_t localFaction,
                               MissionObjective *objective)
{
  ObjectiveLeaves leaves;
  ObjectiveItem items[OBJECTIVE_LEAF_CAPACITY];
  int itemCount = 0;
  bool describable = true;

  Objective_CollectLeaves(schedule,conditionIndex,0,&leaves);
  for (int leafIndex = 0; leafIndex < leaves.count; leafIndex++) {
    ObjectiveItem item;
    if (!Objective_DescribeCondition(schedule.conditions[leaves.conditions[leafIndex]],leaves.conditions[leafIndex],
                                     localFaction,leaves.negated[leafIndex],&item)) {
      describable = false;
      continue;
    }
    /* "no army" of several enemies is one objective: count them together */
    bool merged = false;
    for (int itemIndex = 0; itemIndex < itemCount; itemIndex++) {
      if ((item.progress.kind == MissionObjectiveProgressKind::ENEMY_UNITS) &&
          (items[itemIndex].progress.kind == MissionObjectiveProgressKind::ENEMY_UNITS)) {
        if ((items[itemIndex].progress.factionMask | item.progress.factionMask) !=
            items[itemIndex].progress.factionMask) {
          items[itemIndex].templateId = TEXT_ID_OBJECTIVE_DESTROY_ALL_ENEMIES;
          items[itemIndex].nameKind = ObjectiveNameKind::NONE;
          items[itemIndex].progress.factionMask |= item.progress.factionMask;
        }
        merged = true;
        break;
      }
      if (Objective_SameItem(items[itemIndex],item)) {
        merged = true;
        break;
      }
    }
    if (!merged) {
      items[itemCount++] = item;
    }
  }
  /* "eliminate X" for several opponents, all required: "Alle Gegner ausschalten" */
  if (leaves.binaryOperator == INGAME_CONDITION_TOKEN_AND) {
    int eliminateCount = 0;
    for (int itemIndex = 0; itemIndex < itemCount; itemIndex++) {
      eliminateCount += (items[itemIndex].templateId == TEXT_ID_OBJECTIVE_ELIMINATE_FACTION) ? 1 : 0;
    }
    if (eliminateCount >= 2) {
      int kept = 0;
      bool placed = false;
      for (int itemIndex = 0; itemIndex < itemCount; itemIndex++) {
        if (items[itemIndex].templateId == TEXT_ID_OBJECTIVE_ELIMINATE_FACTION) {
          if (placed) {
            continue;
          }
          items[itemIndex].templateId = TEXT_ID_OBJECTIVE_ELIMINATE_ALL;
          items[itemIndex].nameKind = ObjectiveNameKind::NONE;
          placed = true;
        }
        items[kept++] = items[itemIndex];
      }
      itemCount = kept;
    }
  }
  /* the first measurable part */
  for (int itemIndex = 0; itemIndex < itemCount; itemIndex++) {
    if (items[itemIndex].progress.kind != MissionObjectiveProgressKind::NONE) {
      objective->progress = items[itemIndex].progress;
      break;
    }
  }
  const bool single = describable && leaves.simple && (itemCount == 1);
  const bool pair = describable && leaves.simple && (itemCount == 2) && (leaves.binaryOperator != 0);
  if (single) {
    Objective_FormatItem(items[0],objective->line1,MISSION_OBJECTIVE_LINE_UNITS);
  }
  else if (pair) {
    uint16_t first[MISSION_OBJECTIVE_LINE_UNITS];
    uint16_t second[MISSION_OBJECTIVE_LINE_UNITS];
    ObjectiveArgument arguments[2];
    Objective_FormatItem(items[0],first,MISSION_OBJECTIVE_LINE_UNITS);
    Objective_FormatItem(items[1],second,MISSION_OBJECTIVE_LINE_UNITS);
    arguments[0].text = first;
    arguments[1].text = second;
    Objective_Format(objective->line1,MISSION_OBJECTIVE_LINE_UNITS,
                     (leaves.binaryOperator == INGAME_CONDITION_TOKEN_AND) ? TEXT_ID_OBJECTIVE_AND : TEXT_ID_OBJECTIVE_OR,
                     arguments,2);
  }
  else {
    const bool several = (leaves.count > 1) || (itemCount > 1);
    Objective_Format(objective->line1,MISSION_OBJECTIVE_LINE_UNITS,
                     several ? TEXT_ID_OBJECTIVE_MULTIPLE : TEXT_ID_OBJECTIVE_SEE_BRIEFING,nullptr,0);
  }
}

/* ---- live values ---------------------------------------------------------------------------------------------- */

/* Models of the world owner list whose army belongs to a faction in factionMask (and is of armyAssetId unless
   anyAsset); the walk of InGameScheduledCondition_Holds. */
uint32_t Objective_CountModels(uint32_t factionMask,bool anyAsset,uint32_t armyAssetId)
{
  uint32_t count = 0;

  for (WorldOwnerListNode *worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead; worldNode != nullptr;
       worldNode = worldNode->nextNode) {
    if (worldNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    const ArmyRuntimeSlot *army = WorldOwnerNode_ModelRuntime(worldNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    if ((army->factionIndex < OBJECTIVE_FACTION_SLOT_COUNT) && ((factionMask >> army->factionIndex & 1) != 0) &&
        (anyAsset || (army->armyAssetId == armyAssetId))) {
      count++;
    }
  }
  return count;
}

/* The occupied share in percent, computed like the TERRAIN_OCCUPANCY condition; false without a field grid. */
bool Objective_OccupiedPercent(uint32_t cellByteOffset,uint32_t *outPercent)
{
  const FieldGridAsset *fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  if ((fieldGrid == nullptr) || (cellByteOffset >= OBJECTIVE_FACTION_SLOT_COUNT)) {
    return false;
  }
  const uint32_t cellCount = fieldGrid->gridWidth * fieldGrid->gridHeight;
  if (cellCount == 0) {
    return false;
  }
  const uint8_t *cellBytes = reinterpret_cast<const uint8_t *>(fieldGrid->cells) + cellByteOffset;
  uint32_t occupiedCellCount = 0;
  for (uint32_t cellIndex = 0; cellIndex < cellCount; cellIndex++, cellBytes += sizeof(FieldGridCell)) {
    uint32_t occupancyLow = 0;
    std::memcpy(&occupancyLow,cellBytes + offsetof(FieldGridCell,occupancyMask),sizeof occupancyLow);
    occupiedCellCount += ((occupancyLow & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) != 0) ? 1 : 0;
  }
  *outPercent = (uint32_t)(((uint64_t)occupiedCellCount * 100) / (uint64_t)cellCount);
  return true;
}

/* a Q4 amount in whole units, negative as 0 */
uint32_t Objective_WholeUnits(int32_t valueQ4)
{
  return (valueQ4 <= 0) ? 0u : (uint32_t)valueQ4 >> Q4_SHIFT;
}

/* an extraction rate (Q4 per economy run) in whole units per minute of game time, negative as 0: the economy
   (InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState) runs in world job phase 0, once every 8
   simulation steps, and pays rate * step ticks; 20 steps per second give 150 runs per minute at the normal speed */
uint32_t Objective_RatePerMinute(int32_t rateQ4PerRun)
{
  constexpr uint64_t ECONOMY_RUNS_PER_MINUTE = 20 * 60 / 8;
  return (rateQ4PerRun <= 0) ? 0u : (uint32_t)(((uint64_t)rateQ4PerRun * ECONOMY_RUNS_PER_MINUTE) >> Q4_SHIFT);
}

} // namespace

MissionObjective MissionObjective_Build(const InGameLevelConditionStorage *levelConditionStorage,uint32_t localFaction)
{
  MissionObjective objective;

  objective.localFaction = localFaction;
  objective.storage = levelConditionStorage;
  objective.shortTextId = TEXT_RESOURCE_ID_NONE;
  if ((levelConditionStorage == nullptr) || (localFaction >= OBJECTIVE_FACTION_SLOT_COUNT)) {
    return objective;
  }
  const InGameConditionSchedule &schedule = levelConditionStorage->schedule;
  int victoryCondition = -1;
  for (int triggerIndex = 0; triggerIndex < INGAME_END_CONDITION_TRIGGER_COUNT; triggerIndex++) {
    const InGameEndConditionTriggerRecord8ReferenceView &trigger = schedule.triggers[triggerIndex];
    if (((static_cast<uint8_t>(trigger.stateFlags) & static_cast<uint8_t>(INGAME_END_CONDITION_TRIGGER_ACTIVE)) == 0) ||
        (trigger.movieVariantSelector != OBJECTIVE_VICTORY_MOVIE_VARIANT) ||
        (trigger.factionRuntimeIndex != localFaction) ||
        (trigger.conditionIndex >= INGAME_SCHEDULED_CONDITION_COUNT)) {
      continue;
    }
    if (victoryCondition < 0) {
      victoryCondition = trigger.conditionIndex;
      objective.triggerIndex = (uint8_t)triggerIndex;
    }
    else if (trigger.conditionIndex != victoryCondition) {
      objective.moreTriggers++;
    }
  }
  if (victoryCondition < 0) {
    return objective;
  }
  Objective_DescribeVictory(schedule,victoryCondition,localFaction,&objective);
  objective.source = MissionObjectiveSource::GENERATED;

  /* the hand-written short objective of the level, keyed by its file base name */
  constexpr size_t LEVEL_NAME_UNITS = sizeof(LevelAssetHeader::levelFileNameUtf16) / sizeof(uint16_t);
  uint16_t levelName[LEVEL_NAME_UNITS + 1] = {};
  std::memcpy(levelName,levelConditionStorage->levelImage.header.levelFileNameUtf16,
              sizeof(LevelAssetHeader::levelFileNameUtf16));
  /* the stock headers hold the base name with a trailing dot ("s07_landunt."): cut at the last dot */
  for (size_t unitIndex = LEVEL_NAME_UNITS; unitIndex > 0; unitIndex--) {
    if (levelName[unitIndex - 1] == '.') {
      levelName[unitIndex - 1] = 0;
      break;
    }
  }
  /* a custom map ("maps\<name>.", custom_maps.h) has no hand-written objective, even when its name or script is a
     stock level's */
  const TextResourceId shortTextId =
      CustomMaps_IsCustomIdentifier(levelName) ? TEXT_RESOURCE_ID_NONE : TextResource_MissionObjective(levelName,localFaction);
  uint16_t shortText[MISSION_OBJECTIVE_LINE_UNITS];
  if ((shortTextId != TEXT_RESOURCE_ID_NONE) &&
      Objective_ResolvePlain(shortTextId,shortText,MISSION_OBJECTIVE_LINE_UNITS)) {
    std::memcpy(objective.line1,shortText,sizeof objective.line1);
    objective.shortTextId = shortTextId;
    objective.source = MissionObjectiveSource::SHORT_TEXT;
    return objective;
  }
  if (objective.moreTriggers != 0) {
    uint16_t first[MISSION_OBJECTIVE_LINE_UNITS];
    ObjectiveArgument arguments[2];
    std::memcpy(first,objective.line1,sizeof first);
    arguments[0].text = first;
    arguments[1].number = objective.moreTriggers;
    Objective_Format(objective.line1,MISSION_OBJECTIVE_LINE_UNITS,TEXT_ID_OBJECTIVE_MORE,arguments,2);
  }
  return objective;
}

bool MissionObjective_FormatProgress(const MissionObjective *objective,uint16_t *out,size_t capacity)
{
  ObjectiveArgument arguments[2];
  TextResourceId templateId = TEXT_RESOURCE_ID_NONE;
  int argumentCount = 0;

  if ((out == nullptr) || (capacity == 0)) {
    return false;
  }
  out[0] = 0;
  if ((objective == nullptr) || (objective->source == MissionObjectiveSource::NONE)) {
    return false;
  }
  const MissionObjectiveProgress &progress = objective->progress;
  switch (progress.kind) {
  case MissionObjectiveProgressKind::ENEMY_UNITS:
  case MissionObjectiveProgressKind::ASSET_REMAINING:
  case MissionObjectiveProgressKind::ASSET_COUNT:
  case MissionObjectiveProgressKind::TERRAIN_PERCENT:
    if (g_InGameRuntimeRoot == nullptr) {
      return false;
    }
    break;
  case MissionObjectiveProgressKind::XENITE:
  case MissionObjectiveProgressKind::TRITIUM:
  case MissionObjectiveProgressKind::TRITIUM_RATE:
    if (progress.faction >= OBJECTIVE_FACTION_SLOT_COUNT) {
      return false;
    }
    break;
  case MissionObjectiveProgressKind::COUNTDOWN:
    if ((objective->storage == nullptr) || (progress.conditionIndex >= INGAME_SCHEDULED_CONDITION_COUNT)) {
      return false;
    }
    break;
  default:
    return false;
  }
  switch (progress.kind) {
  case MissionObjectiveProgressKind::ENEMY_UNITS:
    templateId = TEXT_ID_OBJECTIVE_PROGRESS_ENEMY_UNITS;
    arguments[0].number = Objective_CountModels(progress.factionMask,true,0);
    argumentCount = 1;
    break;
  case MissionObjectiveProgressKind::ASSET_REMAINING:
    templateId = TEXT_ID_OBJECTIVE_PROGRESS_REMAINING;
    arguments[0].number = (progress.faction < OBJECTIVE_FACTION_SLOT_COUNT)
                              ? Objective_CountModels(1u << progress.faction,false,progress.armyAssetId) : 0;
    argumentCount = 1;
    break;
  case MissionObjectiveProgressKind::ASSET_COUNT:
    templateId = TEXT_ID_OBJECTIVE_PROGRESS_COUNT;
    arguments[0].number = (progress.faction < OBJECTIVE_FACTION_SLOT_COUNT)
                              ? Objective_CountModels(1u << progress.faction,false,progress.armyAssetId) : 0;
    arguments[1].number = progress.target;
    argumentCount = 2;
    break;
  case MissionObjectiveProgressKind::TERRAIN_PERCENT:
    if (!Objective_OccupiedPercent(progress.cellByteOffset,&arguments[0].number)) {
      return false;
    }
    templateId = TEXT_ID_OBJECTIVE_PROGRESS_PERCENT;
    arguments[1].number = progress.target;
    argumentCount = 2;
    break;
  case MissionObjectiveProgressKind::XENITE:
    templateId = TEXT_ID_OBJECTIVE_PROGRESS_XENITE;
    arguments[0].number = Objective_WholeUnits((int32_t)g_GameFactionRuntimeImage.records[progress.faction].xeniteCurrentQ4);
    arguments[1].number = Objective_WholeUnits((int32_t)progress.target);
    argumentCount = 2;
    break;
  case MissionObjectiveProgressKind::TRITIUM:
    templateId = TEXT_ID_OBJECTIVE_PROGRESS_TRITIUM;
    arguments[0].number = Objective_WholeUnits((int32_t)g_GameFactionRuntimeImage.records[progress.faction].tritiumCurrentQ4);
    arguments[1].number = Objective_WholeUnits((int32_t)progress.target);
    argumentCount = 2;
    break;
  case MissionObjectiveProgressKind::TRITIUM_RATE:
    templateId = TEXT_ID_OBJECTIVE_PROGRESS_RATE;
    arguments[0].number =
        Objective_RatePerMinute((int32_t)g_GameFactionRuntimeImage.records[progress.faction].tritiumExtractionRateQ4PerTick);
    arguments[1].number = Objective_RatePerMinute((int32_t)progress.target);
    argumentCount = 2;
    break;
  case MissionObjectiveProgressKind::COUNTDOWN: {
    /* operand 1 is in seconds: it loses the step ticks once every 20 simulation steps (20 steps per second) */
    const int32_t remaining =
        (int32_t)objective->storage->schedule.conditions[progress.conditionIndex].payload.operands[1];
    const uint32_t seconds = (remaining > 0) ? (uint32_t)remaining : 0u;
    templateId = TEXT_ID_OBJECTIVE_PROGRESS_COUNTDOWN;
    arguments[0].number = seconds / 60;
    arguments[1].number = seconds % 60;
    argumentCount = 2;
    break;
  }
  default:
    return false;
  }
  return Objective_Format(out,capacity,templateId,arguments,argumentCount) != 0;
}
