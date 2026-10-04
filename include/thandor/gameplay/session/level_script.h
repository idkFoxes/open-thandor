/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/level_script.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_LEVEL_SCRIPT_H
#define THANDOR_GAMEPLAY_SESSION_LEVEL_SCRIPT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/level_script. */

/* Level script (InGameLevelConditionStorage.schedule): 64 condition records of 16 bytes and 16 end
   triggers of 8 bytes, evaluated by InGameConditionRuntime_UpdateScheduledRecords. */
#define INGAME_SCHEDULED_CONDITION_COUNT 64
#define INGAME_END_CONDITION_TRIGGER_COUNT 16
/* Tokens of a BOOLEAN_POSTFIX_EXPRESSION condition (the bytes after its kind byte); any other byte pushes the
   satisfied bit of the condition with that index onto the bit stack. */
#define INGAME_CONDITION_TOKEN_END 0xFC
#define INGAME_CONDITION_TOKEN_NOT 0xFD
#define INGAME_CONDITION_TOKEN_AND 0xFE
#define INGAME_CONDITION_TOKEN_OR 0xFF

/* Functions are grouped by semantic ownership. */

void InGameConditionRuntime_UpdateScheduledRecords(void);

extern uint16_t g_SessionEndMoviePathUtf16[17];
extern uint16_t g_FlmEnde0001FlmPathUtf16[17];

#endif /* THANDOR_GAMEPLAY_SESSION_LEVEL_SCRIPT_H */
