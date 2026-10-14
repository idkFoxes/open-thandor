/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/key_bindings_ini.cpp
 * Project code (not in the original game)
 */

#include <thandor/ui/core/key_bindings_ini.h>
#include <thandor/ui/core/key_bindings.h>
#include <thandor/core/settings/persistent.h>
#include <thandor/platform/bootstrap/image.h>

#include <string>

/* Module data. */

static bool s_KeyBindingsIniLoading = false; /* between beginLoad and endLoad: changes are not saved */
static int s_KeyBindingsIniProblemCount = 0;

static constexpr uint32_t KEY_BINDINGS_INI_VALUE_CAPACITY = 256; /* five chords of the longest token */

/* The actions [keys] holds: the editable, shown ones. */
static bool KeyBindingsIni_IsWritten(const KeyBinding &binding)
{
  return !KeyBinding_IsFixed(binding) && !KeyBinding_IsHidden(binding);
}

static void KeyBindingsIni_BeginLoad()
{
  s_KeyBindingsIniLoading = true;
  s_KeyBindingsIniProblemCount = 0;
  KeyBindings_ResetAllDefaults();
}

static void KeyBindingsIni_ParseLine(const char *key,const char *value)
{
  KeyAction action;
  if (!KeyBindings_FindByName(key,&action)) {
    return; /* unknown names are ignored */
  }
  const KeyBinding &binding = KeyBindings_Get(action);
  if (!KeyBindingsIni_IsWritten(binding)) {
    return; /* fixed and hidden actions keep their keys */
  }
  KeyChord chords[KEY_BINDING_MAX_KEYS];
  const int count = KeyBindings_ParseTokens(value,binding.rangeLength,chords,KEY_BINDING_MAX_KEYS);
  if ((count < 0) || !KeyBindings_SetRaw(action,chords,count)) {
    Thandor_Log("keys: [keys] %s = \"%s\" is not valid, default kept",binding.name,value);
    s_KeyBindingsIniProblemCount++;
    KeyBindings_ResetDefaults(action); /* also after an earlier line of the same name */
  }
}

static void KeyBindingsIni_ReportConflict(KeyAction unbound,KeyAction kept)
{
  if (kept == KeyAction::Count) {
    Thandor_Log("keys: [keys] %s: a key that is not allowed, unbound",KeyBindings_Name(unbound));
  }
  else {
    Thandor_Log("keys: [keys] %s: a key already used by %s, unbound",KeyBindings_Name(unbound),
                KeyBindings_Name(kept));
  }
  s_KeyBindingsIniProblemCount++;
}

static void KeyBindingsIni_EndLoad()
{
  KeyBindings_ResolveConflicts(KeyBindingsIni_ReportConflict);
  KeyBindings_NotifyChanged(); /* the dispatch follows; not saved (s_KeyBindingsIniLoading) */
  s_KeyBindingsIniLoading = false;
}

static uint32_t KeyBindingsIni_Format(char *out,uint32_t capacity)
{
  std::string text;
  if ((capacity == 0) || KeyBindings_AllDefault()) {
    return 0; /* the section is omitted: the defaults */
  }
  text = "; key bindings: action = keys (Ctrl, Alt, Shift + key; several joined with \", \"; none = unbound)\r\n"
         "; written in full once one differs from the defaults; without this section the defaults apply\r\n";
  for (int index = 0; index < KeyBindings_Count(); index++) {
    const KeyAction action = static_cast<KeyAction>(index);
    const KeyBinding &binding = KeyBindings_Get(action);
    char value[KEY_BINDINGS_INI_VALUE_CAPACITY];
    if (!KeyBindingsIni_IsWritten(binding)) {
      continue;
    }
    KeyBindings_FormatTokens(action,value,sizeof value);
    text += binding.name;
    text += " = ";
    text += value;
    text += "\r\n";
  }
  if (text.size() + 1 > capacity) {
    return 0;
  }
  text.copy(out,text.size());
  out[text.size()] = 0;
  return static_cast<uint32_t>(text.size());
}

/* A change from the menus or in-game: thandor.ini is written at once. */
static void KeyBindingsIni_Save()
{
  if (s_KeyBindingsIniLoading) {
    return;
  }
  PersistentSettings_MarkDirty();
  PersistentSettings_Flush();
}

void KeyBindingsIni_Register()
{
  PersistentSettings_RegisterSection("keys",KeyBindingsIni_ParseLine,KeyBindingsIni_Format,KeyBindingsIni_BeginLoad,
                                     KeyBindingsIni_EndLoad);
  KeyBindings_AddChangeListener(KeyBindingsIni_Save);
}

int KeyBindingsIni_LastLoadProblemCount()
{
  return s_KeyBindingsIniProblemCount;
}
