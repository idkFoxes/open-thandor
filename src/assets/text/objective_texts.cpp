/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/objective_texts.cpp
 * Project code (not in the original game)
 */

/* The hand-written short objectives of the stock missions (docs/plans/objective_overlay.md 3a), one line per level
   file and faction in German and English, ids TEXT_ID_MISSION_OBJECTIVE_SHORT_BASE.. (resources.h).
   Stub: the table is not written yet, so every level falls back to the objective generated from its victory
   triggers (gameplay/session/mission_objective.cpp). This file is replaced as a whole when the table is filled. */

#include <thandor/assets/text/resources.h>

TextResourceId TextResource_MissionObjective(const uint16_t *levelName,uint32_t faction)
{
  (void)levelName;
  (void)faction;
  return TEXT_RESOURCE_ID_NONE;
}

bool TextResource_TryResolveMissionObjective(TextResourceId resourceId,uint16_t **outText)
{
  (void)resourceId;
  (void)outText;
  return false;
}
