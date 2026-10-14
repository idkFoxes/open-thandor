/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/key_bindings_ini.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_CORE_KEY_BINDINGS_INI_H
#define THANDOR_UI_CORE_KEY_BINDINGS_INI_H

/* The key bindings in thandor.ini, section [keys] (docs/plans/key_rebinding.md), through the section hook of
   core/settings/persistent.h.
   Load (every PersistentSettings_ParseIni, also a load without thandor.ini): the bindings start from the defaults;
   each "name = tokens" line of a known, editable action sets that action (KeyBindings_ParseTokens, KeyBindings_SetRaw);
   an invalid value keeps the default and logs one line; fixed / hidden actions and unknown names are ignored. Then
   KeyBindings_ResolveConflicts unbinds the later of two conflicting chords (one log line each) and the listeners are
   notified (without saving).
   Save: nothing while every binding is the default (the section is omitted); otherwise the whole section, every
   editable shown action as "name = tokens" ("none" when unbound). Every change after the load marks the settings
   dirty and flushes thandor.ini at once (menus and in-game). */

/* Registers [keys] and the save listener. Must run before the first PersistentSettings_Load (ProcessEntry_RunGame);
   calling it again changes nothing. */
void KeyBindingsIni_Register();

/* The number of problems the last load logged: invalid values plus chords unbound by conflicts (self-test). */
int KeyBindingsIni_LastLoadProblemCount();

#endif /* THANDOR_UI_CORE_KEY_BINDINGS_INI_H */
