/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/key_binding_tables.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_CORE_KEY_BINDING_TABLES_H
#define THANDOR_UI_CORE_KEY_BINDING_TABLES_H

#include <array>
#include <cstddef>
#include <thandor/core/types.h>
#include <thandor/ui/core/key_bindings.h>
#include <thandor/ui/core/key_dispatch.h>

/* The key command tables driven by the key bindings (docs/plans/key_rebinding.md, P2): the in-game hotkeys
   (ui/ingame/hotkeys.cpp), the world view's key commands (ui/ingame/key_commands.cpp) and the frontend hotkeys
   (ui/frontend/state.cpp). Each table keeps its original records as a constant; the working table holds them in
   the original order (so the first-match precedence stays), then spare records, then the terminator (code 0).
   Applying the bindings rewrites commandCode and modifierClassFlags of every original record from the chord of
   its action that the record stood for in the defaults (the record's default chord index; a range record keeps
   its offset, so the group rows keep their digit and only take the modifiers); a chord index the action no longer
   has gives UI_KEY_COMMAND_CODE_UNBOUND. Chords beyond the action's default count go into the spare records, in
   action order. Records of fixed actions (Esc, Alt+F4, the cheats) and records without an entry stay as they are.
   With the default bindings every table equals the original. */

/* The action whose entry has this source and table action number; KeyAction::Count when none has. */
inline KeyAction KeyBindingTable_FindAction(KeyBindingSource source,uint32_t sourceAction)
{
  for (int index = 0; index < KeyBindings_Count(); index++) {
    const KeyBinding &binding = KeyBindings_Get(static_cast<KeyAction>(index));
    if ((binding.source == source) && (binding.sourceAction == sourceAction)) {
      return static_cast<KeyAction>(index);
    }
  }
  return KeyAction::Count;
}

/* The number of chords of a chord list (keyCode 0 ends it). */
inline int KeyBindingTable_ChordCount(const KeyChord (&chords)[KEY_BINDING_MAX_KEYS])
{
  int count = 0;
  while ((count < KEY_BINDING_MAX_KEYS) && (chords[count].keyCode != 0)) {
    count++;
  }
  return count;
}

/* The default chord of binding a record {code, modifiers} stands for, and the record's offset in the chord's range;
   -1 when no default chord has this key and these modifiers. */
inline int KeyBindingTable_DefaultChordOf(const KeyBinding &binding,uint32_t code,UiKeyboardStateMask modifiers,
                                          uint32_t *outOffset)
{
  for (int chordIndex = 0; chordIndex < KeyBindingTable_ChordCount(binding.defaults); chordIndex++) {
    const KeyChord &chord = binding.defaults[chordIndex];
    if ((chord.modifiers == modifiers) && (code >= chord.keyCode) && ((code - chord.keyCode) < binding.rangeLength)) {
      *outOffset = code - chord.keyCode;
      return chordIndex;
    }
  }
  return -1;
}

/* The working table at compile time: the original records, unbound spare records, the terminator. */
template <typename Action, std::size_t Capacity, std::size_t OriginalCount>
constexpr std::array<UiKeyCommandRecord<Action>, Capacity> KeyBindingTable_Make(
    const UiKeyCommandRecord<Action> (&originals)[OriginalCount])
{
  static_assert(Capacity > OriginalCount, "the table needs room for its terminator");
  std::array<UiKeyCommandRecord<Action>, Capacity> table{};
  for (std::size_t index = 0; index < Capacity - 1; index++) {
    table[index] = (index < OriginalCount) ? originals[index] :
                   UiKeyCommandRecord<Action>{UI_KEY_COMMAND_CODE_UNBOUND, KEYBOARD_STATE_NONE, Action{}};
  }
  table[Capacity - 1] = UiKeyCommandRecord<Action>{0, KEYBOARD_STATE_NONE, Action{}};
  return table;
}

/* Rewrites the working table from the current bindings of source (see above). Returns the number of records that
   did not fit into the spare records (0 unless the table is too small). */
template <typename Action, std::size_t Capacity, std::size_t OriginalCount>
uint32_t KeyBindingTable_Apply(std::array<UiKeyCommandRecord<Action>, Capacity> &table,
                               const UiKeyCommandRecord<Action> (&originals)[OriginalCount],KeyBindingSource source)
{
  static_assert(Capacity > OriginalCount, "the table needs room for its terminator");
  for (std::size_t index = 0; index < OriginalCount; index++) {
    UiKeyCommandRecord<Action> record = originals[index];
    KeyAction action = KeyBindingTable_FindAction(source,static_cast<uint32_t>(record.action));
    if (action != KeyAction::Count) {
      const KeyBinding &binding = KeyBindings_Get(action);
      uint32_t offset = 0;
      int chordIndex = KeyBindingTable_DefaultChordOf(binding,record.commandCode,record.modifierClassFlags,&offset);
      if (!KeyBinding_IsFixed(binding) && (chordIndex >= 0)) {
        const KeyChord &chord = binding.keys[chordIndex];
        if (chord.keyCode != 0) {
          record.commandCode = chord.keyCode + offset;
          record.modifierClassFlags = chord.modifiers;
        }
        else {
          record.commandCode = UI_KEY_COMMAND_CODE_UNBOUND;
        }
      }
    }
    table[index] = record;
  }
  std::size_t spare = OriginalCount;
  uint32_t dropped = 0;
  for (int index = 0; index < KeyBindings_Count(); index++) {
    const KeyBinding &binding = KeyBindings_Get(static_cast<KeyAction>(index));
    if ((binding.source != source) || KeyBinding_IsFixed(binding)) {
      continue;
    }
    for (int chordIndex = KeyBindingTable_ChordCount(binding.defaults); chordIndex < KeyBindingTable_ChordCount(binding.keys);
         chordIndex++) {
      for (uint32_t offset = 0; offset < binding.rangeLength; offset++) {
        if (spare + 1 < Capacity) {
          table[spare++] = UiKeyCommandRecord<Action>{binding.keys[chordIndex].keyCode + offset,
                                                      binding.keys[chordIndex].modifiers,
                                                      static_cast<Action>(binding.sourceAction)};
        }
        else {
          dropped++;
        }
      }
    }
  }
  for (; spare + 1 < Capacity; spare++) {
    table[spare] = UiKeyCommandRecord<Action>{UI_KEY_COMMAND_CODE_UNBOUND, KEYBOARD_STATE_NONE, Action{}};
  }
  table[Capacity - 1] = UiKeyCommandRecord<Action>{0, KEYBOARD_STATE_NONE, Action{}};
  return dropped;
}

/* Applies the bindings to the three tables (InGameHotkeys_ApplyBindings, InGameKeyCommands_ApplyBindings,
   FrontendHotkeys_ApplyBindings); logs records that did not fit. Returns their number. */
uint32_t KeyBindingTables_Apply();

/* Registers KeyBindingTables_Apply as a key binding change listener and applies once (program start, before the
   self-tests and the ini). */
void KeyBindingTables_Install();

#endif /* THANDOR_UI_CORE_KEY_BINDING_TABLES_H */
