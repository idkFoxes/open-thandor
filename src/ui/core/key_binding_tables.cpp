/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/key_binding_tables.cpp
 * Project code (not in the original game)
 */

#include <thandor/ui/core/key_binding_tables.h>
#include <thandor/thandor.h>
#include <thandor/ui/ingame/hotkeys.h>
#include <thandor/ui/ingame/key_commands.h>
#include <thandor/ui/frontend/state.h>

uint32_t KeyBindingTables_Apply()
{
  uint32_t dropped = InGameHotkeys_ApplyBindings() + InGameKeyCommands_ApplyBindings() + FrontendHotkeys_ApplyBindings();
  if (dropped != 0) {
    Thandor_Log("key bindings: %u key command records did not fit into the tables", dropped);
  }
  return dropped;
}

static void KeyBindingTables_OnBindingsChanged()
{
  KeyBindingTables_Apply();
}

void KeyBindingTables_Install()
{
  KeyBindings_AddChangeListener(KeyBindingTables_OnBindingsChanged);
  KeyBindingTables_Apply();
}
