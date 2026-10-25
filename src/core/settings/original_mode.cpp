/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/settings/original_mode.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/settings/original_mode.h>
#include <thandor/core/settings/persistent.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Not in the original: the "Original Thandor" master switch, [game] original_thandor (PERSISTENT_SETTING_ORIGINAL_THANDOR,
   default off). The state is not cached here: the settings image is the one place it lives, so a reload of thandor.ini
   and the menu's write are seen at once. OPEN_THANDOR_ORIGINAL (developer tools only) overrides it. */

static constexpr int ORIGINAL_MODE_MAX_LISTENERS = 8;

static OriginalModeChangeListener *s_OriginalModeListeners[ORIGINAL_MODE_MAX_LISTENERS];
static int s_OriginalModeListenerCount;

bool OriginalMode_IsOn()
{
  const bool saved = PersistentSettings_Read(PERSISTENT_ORIGINAL_THANDOR_OFF,PERSISTENT_SETTING_ORIGINAL_THANDOR) ==
                     PERSISTENT_ORIGINAL_THANDOR_ON;

  return DebugHook_OriginalMode(saved ? 1 : 0) != 0;
}

/* Saves the choice even when the environment overrides it (the override is for test runs; the player's file keeps
   what the player chose); the listeners hear only of a change of the state the features see. */
void OriginalMode_Set(bool on)
{
  const bool before = OriginalMode_IsOn();
  bool after;

  PersistentSettings_WriteOriginalThandor(on);
  after = OriginalMode_IsOn();
  Thandor_Log("original mode: [game] original_thandor = %s%s",on ? "on" : "off",
              after != on ? " (overridden by OPEN_THANDOR_ORIGINAL)" : "");
  if (after == before) {
    return;
  }
  for (int index = 0; index < s_OriginalModeListenerCount; index++) {
    s_OriginalModeListeners[index](after);
  }
}

bool OtFeature_Active(OtFeature feature)
{
  /* every feature follows the one switch; the parameter lets a later per-feature exception stay a local change */
  (void)feature;
  return !OriginalMode_IsOn();
}

void OriginalMode_AddChangeListener(OriginalModeChangeListener *listener)
{
  if (listener == nullptr) {
    return;
  }
  for (int index = 0; index < s_OriginalModeListenerCount; index++) {
    if (s_OriginalModeListeners[index] == listener) {
      return; /* registered twice (e.g. a module initialized again): called once */
    }
  }
  if (s_OriginalModeListenerCount < ORIGINAL_MODE_MAX_LISTENERS) {
    s_OriginalModeListeners[s_OriginalModeListenerCount++] = listener;
  }
  else {
    Thandor_Log("original mode: listener table full, a listener is not called");
  }
}
