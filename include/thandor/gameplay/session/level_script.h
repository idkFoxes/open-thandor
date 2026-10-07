/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/level_script.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_LEVEL_SCRIPT_H
#define THANDOR_GAMEPLAY_SESSION_LEVEL_SCRIPT_H

#include <thandor/core/contracts.h>
#include <thandor/gameplay/session/types.h> /* InGameScheduledCondition* for the postfix evaluator */

/* Level script (InGameLevelConditionStorage.schedule): 64 condition records of 16 bytes and 16 end
   triggers of 8 bytes, evaluated by InGameConditionRuntime_UpdateScheduledRecords. */
inline constexpr int INGAME_SCHEDULED_CONDITION_COUNT = 64;
inline constexpr int INGAME_END_CONDITION_TRIGGER_COUNT = 16;
/* Tokens of a BOOLEAN_POSTFIX_EXPRESSION condition (the bytes after its kind byte); any other byte pushes the
   satisfied bit of the condition with that index onto the bit stack. */
inline constexpr int INGAME_CONDITION_TOKEN_END = 0xFC;
inline constexpr int INGAME_CONDITION_TOKEN_NOT = 0xFD;
inline constexpr int INGAME_CONDITION_TOKEN_AND = 0xFE;
inline constexpr int INGAME_CONDITION_TOKEN_OR = 0xFF;

/* Evaluates a BOOLEAN_POSTFIX_EXPRESSION condition: the tokens after its kind byte run on a bit stack.
   0xFC end, 0xFD NOT, 0xFE AND, 0xFF OR, anything else pushes the satisfied bit of the condition with that index.
   Returns the bit stack; bit 0 is the result. The original has two copies that differ only in the width of the
   bit stack: the level script (InGameScheduledCondition_Holds) uses an unsigned stack, the relation prediction
   (GameFactionRelations_PredictConditionHolds) a signed int one (arithmetic shift on pops). BitStack keeps that
   difference; it only shows with more than 31 pending operands. */
template <typename BitStack>
BitStack InGameScheduledCondition_EvaluatePostfixExpression
          (InGameLevelConditionStorage *levelConditionStorage,const uint8_t *expression)
{
  BitStack bitStack;
  uint8_t token;

  bitStack = INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED;
  for (; *expression != INGAME_CONDITION_TOKEN_END; expression++) {
    token = *expression;
    if (token == INGAME_CONDITION_TOKEN_OR) {
      bitStack = bitStack >> 1 | bitStack & 1;
    }
    else if (token == INGAME_CONDITION_TOKEN_AND) {
      bitStack = bitStack >> 1 & (bitStack | ~1u);
    }
    else if (token == INGAME_CONDITION_TOKEN_NOT) {
      bitStack = bitStack ^ 1;
    }
    else {
      bitStack = ((levelConditionStorage->schedule).conditions[token].statusAndKind.raw &
                  INGAME_SCHEDULED_CONDITION_SATISFIED) + bitStack * 2;
    }
  }
  return bitStack;
}

void InGameConditionRuntime_UpdateScheduledRecords();

extern uint16_t g_SessionEndMoviePathUtf16[17];
extern uint16_t g_FlmEnde0001FlmPathUtf16[17];

#endif /* THANDOR_GAMEPLAY_SESSION_LEVEL_SCRIPT_H */
