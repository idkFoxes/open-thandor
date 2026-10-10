/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/key_bindings.cpp
 * Project code (not in the original game)
 */

#include <thandor/ui/core/key_bindings.h>
#include <thandor/assets/text/resources.h>
#include <thandor/platform/input/devices.h>

/* Module data. */

static bool g_KeyBindingsEnglish = false;

static constexpr UiKeyboardStateMask BIND_NONE = KEYBOARD_STATE_NONE;
static constexpr UiKeyboardStateMask BIND_SHIFT = KEYBOARD_STATE_SHIFT;
static constexpr UiKeyboardStateMask BIND_CTRL = KEYBOARD_STATE_CTRL;
static constexpr UiKeyboardStateMask BIND_ALT = KEYBOARD_STATE_ALT;

static constexpr uint32_t KeyChar(char asciiCode)
{
  return KEYBOARD_KEY_CODE_CHAR(static_cast<uint32_t>(asciiCode));
}

/* The rows, by section. Action numbers: InGameHotkeyAction (ui/ingame/hotkeys.cpp), InGameKeyCommandAction
   (ui/ingame/key_commands.cpp), FrontendHotkeyAction (ui/frontend/state.cpp), EndMovieCommandAction
   (ui/frontend/end_movie_commands.cpp). Not listed: the cheat keys (Ctrl+Alt+X/E/Z, Ctrl+Alt+V), the hidden
   Ctrl+F1 of the faction setup page (an eighth colour in the faction cycle) and the map editor's keys
   (ui/ingame/camera_commands.cpp, ui/ingame/editor_keyboard.cpp). */
static const KeyBinding g_KeyBindings[] = {
    /* Allgemein im Spiel */
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 4, {{KEYBOARD_KEY_CODE_ESCAPE, BIND_NONE}}, TEXT_ID_KEYS_GAME_MENU},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 5, {{KEYBOARD_KEY_CODE_FUNCTION(1), BIND_NONE}}, TEXT_ID_KEYS_OBJECTIVES},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 6, {{KEYBOARD_KEY_CODE_FUNCTION(2), BIND_NONE}}, TEXT_ID_KEYS_SAVE},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 7, {{KEYBOARD_KEY_CODE_FUNCTION(4), BIND_ALT}}, TEXT_ID_KEYS_QUIT_MENU},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 1, {{KeyChar('q'), BIND_ALT}}, TEXT_ID_KEYS_LEAVE_GAME},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 8, {{KeyChar('p'), BIND_NONE}}, TEXT_ID_KEYS_PAUSE},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 10, {{KeyChar('g'), BIND_NONE}}, TEXT_ID_KEYS_FASTER},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 9, {{KeyChar('g'), BIND_ALT}}, TEXT_ID_KEYS_SLOWER},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 11, {{KEYBOARD_KEY_CODE_TAB, BIND_NONE}}, TEXT_ID_KEYS_SIDE_PANEL},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 2, {{KEYBOARD_KEY_CODE_ENTER, BIND_NONE}}, TEXT_ID_KEYS_CHAT_LINE},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 3, {{KeyChar('c'), BIND_NONE}}, TEXT_ID_KEYS_MESSAGE_WINDOW},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameKeyCommands, 1, 17, {{KeyChar('o'), BIND_NONE}}, TEXT_ID_KEYS_SCORE_OVERVIEW},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameKeyCommands, 1, 19, {{KeyChar('i'), BIND_CTRL}}, TEXT_ID_KEYS_INFO_LINE},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameHotkeys, 1, 12, {{KeyChar('p'), BIND_ALT}}, TEXT_ID_KEYS_SCREENSHOT},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameKeyCommands, 1, 5, {{KEYBOARD_KEY_CODE_SPACE, BIND_NONE}}, TEXT_ID_KEYS_NOTIFICATION_GO},
    {KeyBindingSection::InGameGeneral, KeyBindingSource::InGameKeyCommands, 1, 6, {{KEYBOARD_KEY_CODE_BACKSPACE, BIND_NONE}}, TEXT_ID_KEYS_NOTIFICATION_BACK},
    /* Einheiten und Gruppen */
    {KeyBindingSection::UnitsAndGroups, KeyBindingSource::InGameKeyCommands, 8, 1, {{KeyChar('1'), BIND_NONE}}, TEXT_ID_KEYS_GROUP_RECALL},
    {KeyBindingSection::UnitsAndGroups, KeyBindingSource::InGameKeyCommands, 8, 2, {{KeyChar('1'), BIND_SHIFT}}, TEXT_ID_KEYS_GROUP_ADD_TO_SELECTION},
    {KeyBindingSection::UnitsAndGroups, KeyBindingSource::InGameKeyCommands, 8, 3, {{KeyChar('1'), BIND_CTRL}, {KeyChar('1'), BIND_ALT}}, TEXT_ID_KEYS_GROUP_STORE},
    {KeyBindingSection::UnitsAndGroups, KeyBindingSource::InGameKeyCommands, 8, 4,
     {{KeyChar('1'), BIND_CTRL | BIND_SHIFT}, {KeyChar('1'), BIND_ALT | BIND_SHIFT}}, TEXT_ID_KEYS_GROUP_ADD_SELECTION},
    {KeyBindingSection::UnitsAndGroups, KeyBindingSource::InGameKeyCommands, 1, 10, {{KeyChar('s'), BIND_NONE}, {KeyChar('s'), BIND_ALT | BIND_SHIFT}}, TEXT_ID_KEYS_STOP},
    {KeyBindingSection::UnitsAndGroups, KeyBindingSource::InGameKeyCommands, 1, 11, {{KeyChar('s'), BIND_SHIFT}}, TEXT_ID_KEYS_STOP_MOVEMENT},
    {KeyBindingSection::UnitsAndGroups, KeyBindingSource::InGameKeyCommands, 1, 12, {{KeyChar('s'), BIND_ALT}}, TEXT_ID_KEYS_CANCEL_ATTACK},
    {KeyBindingSection::UnitsAndGroups, KeyBindingSource::InGameKeyCommands, 1, 15, {{KeyChar('d'), BIND_ALT}}, TEXT_ID_KEYS_SELF_DESTRUCT},
    /* A: every own model of runtime class 22 (the airport, "Flughafen" 310/1310/2310) */
    {KeyBindingSection::UnitsAndGroups, KeyBindingSource::InGameKeyCommands, 1, 14, {{KeyChar('a'), BIND_NONE}}, TEXT_ID_KEYS_SELECT_AIRPORTS},
    {KeyBindingSection::UnitsAndGroups, KeyBindingSource::InGameKeyCommands, 1, 13, {{KeyChar('f'), BIND_NONE}}, TEXT_ID_KEYS_UPGRADE_PAGE},
    /* Kamera */
    {KeyBindingSection::Camera, KeyBindingSource::CameraHeldKeys, 1, 0, {{KEY_BINDING_CODE_ARROWS, BIND_NONE}}, TEXT_ID_KEYS_CAMERA_SCROLL},
    {KeyBindingSection::Camera, KeyBindingSource::CameraHeldKeys, 1, 0,
     {{KEYBOARD_KEY_CODE_PAGE_UP, BIND_NONE}, {KEYBOARD_KEY_CODE_PAGE_DOWN, BIND_NONE}}, TEXT_ID_KEYS_CAMERA_TILT},
    {KeyBindingSection::Camera, KeyBindingSource::CameraHeldKeys, 1, 0,
     {{KEYBOARD_KEY_CODE_INSERT, BIND_NONE}, {KEYBOARD_KEY_CODE_DELETE, BIND_NONE}}, TEXT_ID_KEYS_CAMERA_ROTATE},
    {KeyBindingSection::Camera, KeyBindingSource::CameraHeldKeys, 1, 0,
     {{KEYBOARD_KEY_CODE_HOME, BIND_NONE}, {KEYBOARD_KEY_CODE_END, BIND_NONE}}, TEXT_ID_KEYS_CAMERA_ZOOM},
    {KeyBindingSection::Camera, KeyBindingSource::InGameKeyCommands, 1, 8, {{KEYBOARD_KEY_CODE_NUMPAD_5, BIND_NONE}}, TEXT_ID_KEYS_CAMERA_SELECTION},
    /* B: the first own model of runtime class 11 (the headquarters, "Hauptquartier" 301/1301/2301) */
    {KeyBindingSection::Camera, KeyBindingSource::InGameKeyCommands, 1, 9, {{KeyChar('b'), BIND_NONE}}, TEXT_ID_KEYS_CAMERA_HQ},
    {KeyBindingSection::Camera, KeyBindingSource::InGameKeyCommands, 1, 7, {{KEYBOARD_KEY_CODE_SPACE, BIND_ALT}}, TEXT_ID_KEYS_CAMERA_NOTIFICATION},
    {KeyBindingSection::Camera, KeyBindingSource::InGameKeyCommands, 1, 16, {{KeyChar('c'), BIND_ALT}}, TEXT_ID_KEYS_CAMERA_FREE},
    /* Menues und Filme */
    {KeyBindingSection::MenusAndMovies, KeyBindingSource::FrontendHotkeys, 1, 1,
     {{KeyChar('q'), BIND_ALT}, {KEYBOARD_KEY_CODE_FUNCTION(4), BIND_ALT}}, TEXT_ID_KEYS_MENU_LEAVE},
    {KeyBindingSection::MenusAndMovies, KeyBindingSource::UiControls, 1, 0, {{KEYBOARD_KEY_CODE_TAB, BIND_NONE}}, TEXT_ID_KEYS_UI_NEXT_CONTROL},
    {KeyBindingSection::MenusAndMovies, KeyBindingSource::UiControls, 1, 0, {{KEYBOARD_KEY_CODE_SPACE, BIND_NONE}}, TEXT_ID_KEYS_UI_PRESS_BUTTON},
    {KeyBindingSection::MenusAndMovies, KeyBindingSource::UiControls, 1, 0,
     {{KEYBOARD_KEY_CODE_ENTER, BIND_NONE}, {KEYBOARD_KEY_CODE_ESCAPE, BIND_NONE}}, TEXT_ID_KEYS_UI_CONFIRM_CANCEL},
    {KeyBindingSection::MenusAndMovies, KeyBindingSource::UiControls, 1, 0,
     {{KEY_BINDING_CODE_ARROWS, BIND_NONE}, {KEYBOARD_KEY_CODE_PAGE_UP, BIND_NONE}, {KEYBOARD_KEY_CODE_PAGE_DOWN, BIND_NONE},
      {KEYBOARD_KEY_CODE_HOME, BIND_NONE}, {KEYBOARD_KEY_CODE_END, BIND_NONE}}, TEXT_ID_KEYS_UI_LIST},
    {KeyBindingSection::MenusAndMovies, KeyBindingSource::UiControls, 1, 0,
     {{KEY_BINDING_CODE_ARROWS, BIND_NONE}, {KEY_BINDING_CODE_ARROWS, BIND_CTRL}}, TEXT_ID_KEYS_UI_SLIDER},
    {KeyBindingSection::MenusAndMovies, KeyBindingSource::IntroMovies, 1, 0, {{KEYBOARD_KEY_CODE_ESCAPE, BIND_NONE}}, TEXT_ID_KEYS_INTRO_SKIP_ALL},
    {KeyBindingSection::MenusAndMovies, KeyBindingSource::IntroMovies, 1, 0, {{KEY_BINDING_CODE_ANY_KEY, BIND_NONE}}, TEXT_ID_KEYS_INTRO_SKIP},
    /* the end-movie table holds character codes (see EndMovieCommands_FindBinding) */
    {KeyBindingSection::MenusAndMovies, KeyBindingSource::EndMovieCommands, 1, 1, {{'q', BIND_ALT}}, TEXT_ID_KEYS_END_MOVIE_SKIP},
    {KeyBindingSection::MenusAndMovies, KeyBindingSource::EndMovieCommands, 1, 2, {{'p', BIND_CTRL}}, TEXT_ID_KEYS_END_MOVIE_SCREENSHOT},
};

/* Key names, German / English. */
struct KeyNamePair {
  uint32_t keyCode;
  const char16_t *german;
  const char16_t *english;
};

static const KeyNamePair g_KeyNames[] = {
    {KEYBOARD_KEY_CODE_ESCAPE, u"Esc", u"Esc"},
    {KEYBOARD_KEY_CODE_ENTER, u"Eingabe", u"Enter"},
    {KEYBOARD_KEY_CODE_TAB, u"Tab", u"Tab"},
    {KEYBOARD_KEY_CODE_BACKSPACE, u"Rücktaste", u"Backspace"},
    {KEYBOARD_KEY_CODE_SPACE, u"Leertaste", u"Space"},
    {KEYBOARD_KEY_CODE_PRINT, u"Druck", u"Print"},
    {KEYBOARD_KEY_CODE_PAUSE, u"Pause", u"Pause"},
    {KEYBOARD_KEY_CODE_INSERT, u"Einfg", u"Insert"},
    {KEYBOARD_KEY_CODE_DELETE, u"Entf", u"Delete"},
    {KEYBOARD_KEY_CODE_HOME, u"Pos1", u"Home"},
    {KEYBOARD_KEY_CODE_END, u"Ende", u"End"},
    {KEYBOARD_KEY_CODE_PAGE_UP, u"Bild auf", u"Page Up"},
    {KEYBOARD_KEY_CODE_PAGE_DOWN, u"Bild ab", u"Page Down"},
    {KEYBOARD_KEY_CODE_UP, u"Pfeil auf", u"Up"},
    {KEYBOARD_KEY_CODE_DOWN, u"Pfeil ab", u"Down"},
    {KEYBOARD_KEY_CODE_LEFT, u"Pfeil links", u"Left"},
    {KEYBOARD_KEY_CODE_RIGHT, u"Pfeil rechts", u"Right"},
    {KEYBOARD_KEY_CODE_NUMPAD_5, u"Num 5", u"Num 5"},
    {KEY_BINDING_CODE_ARROWS, u"Pfeiltasten", u"Arrow keys"},
    {KEY_BINDING_CODE_ANY_KEY, u"Beliebige Taste", u"Any key"},
};

/* Appends to a bounded UTF-16 buffer; keeps room for the terminator. */
struct KeyNameWriter {
  uint16_t *out;
  uint32_t capacity;
  uint32_t length;

  void Put(uint16_t codeUnit)
  {
    if (length + 1 < capacity) {
      out[length] = codeUnit;
      length++;
    }
  }
  void Put(const char16_t *text)
  {
    for (; *text != 0; text++) {
      Put(static_cast<uint16_t>(*text));
    }
  }
  void Terminate()
  {
    if (capacity > 0) {
      out[length] = 0;
    }
  }
};

/* The name of the key alone (no modifiers); false for a key without a name. */
static bool KeyName_PutKey(KeyNameWriter &writer,uint32_t keyCode)
{
  for (const KeyNamePair &name : g_KeyNames) {
    if (name.keyCode == keyCode) {
      writer.Put(g_KeyBindingsEnglish ? name.english : name.german);
      return true;
    }
  }
  uint32_t family = keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK;
  uint32_t index = keyCode & KEYBOARD_KEY_CODE_INDEX_MASK;
  if ((family == KEYBOARD_KEY_CODE_FUNCTION(0)) && (index >= 1) && (index <= 12)) {
    writer.Put('F');
    if (index >= 10) {
      writer.Put(static_cast<uint16_t>('0' + index / 10));
    }
    writer.Put(static_cast<uint16_t>('0' + index % 10));
    return true;
  }
  /* key-down codes of digits and letters, and the plain characters of the end-movie table */
  if ((family == KEYBOARD_KEY_CODE_CHAR(0)) || (family == 0)) {
    if ((index >= '0') && (index <= '9')) {
      writer.Put(static_cast<uint16_t>(index));
      return true;
    }
    if ((index >= 'a') && (index <= 'z')) {
      writer.Put(static_cast<uint16_t>(index - 'a' + 'A'));
      return true;
    }
    if ((index >= 'A') && (index <= 'Z')) {
      writer.Put(static_cast<uint16_t>(index));
      return true;
    }
    if ((family == 0) && ((index == '*') || (index == '/') || (index == '+') || (index == '-'))) {
      writer.Put(static_cast<uint16_t>(index));
      return true;
    }
  }
  return false;
}

static void KeyName_PutModifiers(KeyNameWriter &writer,UiKeyboardStateMask modifiers)
{
  if (Any(modifiers & KEYBOARD_STATE_CTRL)) {
    writer.Put(g_KeyBindingsEnglish ? u"Ctrl+" : u"Strg+");
  }
  if (Any(modifiers & KEYBOARD_STATE_ALT)) {
    writer.Put(u"Alt+");
  }
  if (Any(modifiers & KEYBOARD_STATE_SHIFT)) {
    writer.Put(g_KeyBindingsEnglish ? u"Shift+" : u"Umschalt+");
  }
}

const KeyBinding *KeyBindings_Table(uint32_t *outCount)
{
  *outCount = static_cast<uint32_t>(sizeof(g_KeyBindings) / sizeof(g_KeyBindings[0]));
  return g_KeyBindings;
}

TextResourceId KeyBindings_SectionTitle(KeyBindingSection section)
{
  switch (section) {
  case KeyBindingSection::InGameGeneral: return TEXT_ID_KEYS_SECTION_GENERAL;
  case KeyBindingSection::UnitsAndGroups: return TEXT_ID_KEYS_SECTION_UNITS;
  case KeyBindingSection::Camera: return TEXT_ID_KEYS_SECTION_CAMERA;
  case KeyBindingSection::MenusAndMovies: return TEXT_ID_KEYS_SECTION_MENUS;
  }
  return TEXT_ID_KEYS_SECTION_GENERAL;
}

uint32_t KeyName_Format(uint32_t keyCode,UiKeyboardStateMask modifiers,uint16_t *out,uint32_t capacity)
{
  KeyNameWriter writer{out, capacity, 0};
  KeyName_PutModifiers(writer,modifiers);
  if (!KeyName_PutKey(writer,keyCode)) {
    writer.length = 0;
  }
  writer.Terminate();
  return writer.length;
}

uint32_t KeyBinding_FormatKeys(const KeyBinding &binding,uint16_t *out,uint32_t capacity)
{
  KeyNameWriter writer{out, capacity, 0};
  for (int keyIndex = 0; (keyIndex < KEY_BINDING_MAX_KEYS) && (binding.keys[keyIndex].keyCode != 0); keyIndex++) {
    const KeyBindingKey &key = binding.keys[keyIndex];
    if (keyIndex > 0) {
      writer.Put(u" / ");
    }
    KeyName_PutModifiers(writer,key.modifiers);
    if (!KeyName_PutKey(writer,key.keyCode)) {
      writer.length = 0;
      break;
    }
    if (binding.rangeLength > 1) {
      writer.Put(u"..");
      if (!KeyName_PutKey(writer,key.keyCode + binding.rangeLength - 1)) {
        writer.length = 0;
        break;
      }
    }
  }
  writer.Terminate();
  return writer.length;
}

void KeyBindings_SetEnglish(bool english)
{
  g_KeyBindingsEnglish = english;
  TextResource_SetKeyBindingTextsEnglish(english);
}

bool KeyBindings_English()
{
  return g_KeyBindingsEnglish;
}
