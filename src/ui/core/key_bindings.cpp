/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/key_bindings.cpp
 * Project code (not in the original game)
 */

#include <thandor/ui/core/key_bindings.h>
#include <thandor/assets/text/resources.h>
#include <thandor/platform/input/devices.h>
#include <thandor/thandor.h>

#include <cstring>

/* Module data. */

static bool g_KeyBindingsEnglish = false;

static constexpr UiKeyboardStateMask BIND_NONE = KEYBOARD_STATE_NONE;
static constexpr UiKeyboardStateMask BIND_SHIFT = KEYBOARD_STATE_SHIFT;
static constexpr UiKeyboardStateMask BIND_CTRL = KEYBOARD_STATE_CTRL;
static constexpr UiKeyboardStateMask BIND_ALT = KEYBOARD_STATE_ALT;
static constexpr UiKeyboardStateMask BIND_MODIFIERS = KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT;

static constexpr uint32_t KeyChar(char asciiCode)
{
  return KEYBOARD_KEY_CODE_CHAR(static_cast<uint32_t>(asciiCode));
}

static constexpr uint32_t KEY_BINDING_GROUP_KEY = KEYBOARD_KEY_CODE_CHAR('1');

static constexpr KeyChord NO_CHORD = {0, BIND_NONE};

using Ctx = KeyBindingContext;
using Sec = KeyBindingSection;
using Src = KeyBindingSource;
static constexpr uint8_t FIXED = KEY_BINDING_FLAG_FIXED;
static constexpr uint8_t HIDDEN = KEY_BINDING_FLAG_FIXED | KEY_BINDING_FLAG_HIDDEN;

/* The default entries, in KeyAction order (keys is filled from defaults at startup and by the resets). Action
   numbers: InGameHotkeyAction (ui/ingame/hotkeys.cpp), InGameKeyCommandAction (ui/ingame/key_commands.cpp),
   FrontendHotkeyAction (ui/frontend/state.cpp), EndMovieCommandAction (ui/frontend/end_movie_commands.cpp). Not
   listed: the map editor's keys (ui/ingame/camera_commands.cpp, ui/ingame/editor_keyboard.cpp). */
#define KB(name, context, section, source, range, flags, action, text, ...) \
    {name, Ctx::context, Sec::section, Src::source, range, flags, action, text, {}, {__VA_ARGS__}, NO_CHORD}
#define KB_PERMANENT(name, context, section, source, range, flags, action, text, permanentChord, ...) \
    {name, Ctx::context, Sec::section, Src::source, range, flags, action, text, {}, {__VA_ARGS__}, permanentChord}
static KeyBinding g_KeyBindings[KEY_ACTION_COUNT] = {
    /* Allgemein im Spiel */
    KB("game_menu", InGame, InGameGeneral, InGameHotkeys, 1, FIXED, 4, TEXT_ID_KEYS_GAME_MENU, {KEYBOARD_KEY_CODE_ESCAPE, BIND_NONE}),
    KB("objectives", InGame, InGameGeneral, InGameHotkeys, 1, 0, 5, TEXT_ID_KEYS_OBJECTIVES, {KEYBOARD_KEY_CODE_FUNCTION(1), BIND_NONE}),
    KB("save", InGame, InGameGeneral, InGameHotkeys, 1, 0, 6, TEXT_ID_KEYS_SAVE, {KEYBOARD_KEY_CODE_FUNCTION(2), BIND_NONE}),
    KB("quit_menu", InGame, InGameGeneral, InGameHotkeys, 1, FIXED, 7, TEXT_ID_KEYS_QUIT_MENU, {KEYBOARD_KEY_CODE_FUNCTION(4), BIND_ALT}),
    KB("leave_game", InGame, InGameGeneral, InGameHotkeys, 1, 0, 1, TEXT_ID_KEYS_LEAVE_GAME, {KeyChar('q'), BIND_ALT}),
    KB("pause", InGame, InGameGeneral, InGameHotkeys, 1, 0, 8, TEXT_ID_KEYS_PAUSE, {KeyChar('p'), BIND_NONE}),
    KB("faster", InGame, InGameGeneral, InGameHotkeys, 1, 0, 10, TEXT_ID_KEYS_FASTER, {KeyChar('g'), BIND_NONE}),
    KB("slower", InGame, InGameGeneral, InGameHotkeys, 1, 0, 9, TEXT_ID_KEYS_SLOWER, {KeyChar('g'), BIND_ALT}),
    KB("side_panel", InGame, InGameGeneral, InGameHotkeys, 1, 0, 11, TEXT_ID_KEYS_SIDE_PANEL, {KEYBOARD_KEY_CODE_TAB, BIND_NONE}),
    KB("chat", InGame, InGameGeneral, InGameHotkeys, 1, 0, 2, TEXT_ID_KEYS_CHAT_LINE, {KEYBOARD_KEY_CODE_ENTER, BIND_NONE}),
    KB("message_window", InGame, InGameGeneral, InGameHotkeys, 1, 0, 3, TEXT_ID_KEYS_MESSAGE_WINDOW, {KeyChar('c'), BIND_NONE}),
    KB("score_overview", InGame, InGameGeneral, InGameKeyCommands, 1, 0, 17, TEXT_ID_KEYS_SCORE_OVERVIEW, {KeyChar('o'), BIND_NONE}),
    KB("info_line", InGame, InGameGeneral, InGameKeyCommands, 1, 0, 19, TEXT_ID_KEYS_INFO_LINE, {KeyChar('i'), BIND_CTRL}),
    KB("screenshot", InGame, InGameGeneral, InGameHotkeys, 1, 0, 12, TEXT_ID_KEYS_SCREENSHOT, {KeyChar('p'), BIND_ALT}),
    KB("notification_go", InGame, InGameGeneral, InGameKeyCommands, 1, 0, 5, TEXT_ID_KEYS_NOTIFICATION_GO, {KEYBOARD_KEY_CODE_SPACE, BIND_NONE}),
    KB("notification_back", InGame, InGameGeneral, InGameKeyCommands, 1, 0, 6, TEXT_ID_KEYS_NOTIFICATION_BACK, {KEYBOARD_KEY_CODE_BACKSPACE, BIND_NONE}),
    /* not in the original: the objective overlay on/off (docs/plans/objective_overlay.md O5); no original record, so
       all its chords go into the spare records of the in-game hotkey table. Z is free in the in-game context. */
    KB("objective_toggle", InGame, InGameGeneral, InGameHotkeys, 1, 0, 16, TEXT_ID_KEYS_OBJECTIVE_TOGGLE, {KeyChar('z'), BIND_NONE}),
    /* Einheiten und Gruppen */
    KB("group_select", InGame, UnitsAndGroups, InGameKeyCommands, 8, 0, 1, TEXT_ID_KEYS_GROUP_RECALL, {KEY_BINDING_GROUP_KEY, BIND_NONE}),
    KB("group_select_add", InGame, UnitsAndGroups, InGameKeyCommands, 8, 0, 2, TEXT_ID_KEYS_GROUP_ADD_TO_SELECTION, {KEY_BINDING_GROUP_KEY, BIND_SHIFT}),
    KB("group_store", InGame, UnitsAndGroups, InGameKeyCommands, 8, 0, 3, TEXT_ID_KEYS_GROUP_STORE,
       {KEY_BINDING_GROUP_KEY, BIND_CTRL}, {KEY_BINDING_GROUP_KEY, BIND_ALT}),
    KB("group_store_add", InGame, UnitsAndGroups, InGameKeyCommands, 8, 0, 4, TEXT_ID_KEYS_GROUP_ADD_SELECTION,
       {KEY_BINDING_GROUP_KEY, BIND_CTRL | BIND_SHIFT}, {KEY_BINDING_GROUP_KEY, BIND_ALT | BIND_SHIFT}),
    KB("stop", InGame, UnitsAndGroups, InGameKeyCommands, 1, 0, 10, TEXT_ID_KEYS_STOP, {KeyChar('s'), BIND_NONE}, {KeyChar('s'), BIND_ALT | BIND_SHIFT}),
    KB("stop_movement", InGame, UnitsAndGroups, InGameKeyCommands, 1, 0, 11, TEXT_ID_KEYS_STOP_MOVEMENT, {KeyChar('s'), BIND_SHIFT}),
    KB("cancel_attack", InGame, UnitsAndGroups, InGameKeyCommands, 1, 0, 12, TEXT_ID_KEYS_CANCEL_ATTACK, {KeyChar('s'), BIND_ALT}),
    KB("self_destruct", InGame, UnitsAndGroups, InGameKeyCommands, 1, 0, 15, TEXT_ID_KEYS_SELF_DESTRUCT, {KeyChar('d'), BIND_ALT}),
    /* A: every own model of runtime class 22 (the airport, "Flughafen" 310/1310/2310) */
    KB("select_airports", InGame, UnitsAndGroups, InGameKeyCommands, 1, 0, 14, TEXT_ID_KEYS_SELECT_AIRPORTS, {KeyChar('a'), BIND_NONE}),
    KB("upgrade_page", InGame, UnitsAndGroups, InGameKeyCommands, 1, 0, 13, TEXT_ID_KEYS_UPGRADE_PAGE, {KeyChar('f'), BIND_NONE}),
    /* Kamera: one row per held key (direction, tilt, rotation, zoom) */
    KB("camera_left", InGame, Camera, CameraHeldKeys, 1, 0, 0, TEXT_ID_KEYS_CAMERA_LEFT, {KEYBOARD_KEY_CODE_LEFT, BIND_NONE}),
    KB("camera_right", InGame, Camera, CameraHeldKeys, 1, 0, 0, TEXT_ID_KEYS_CAMERA_RIGHT, {KEYBOARD_KEY_CODE_RIGHT, BIND_NONE}),
    KB("camera_up", InGame, Camera, CameraHeldKeys, 1, 0, 0, TEXT_ID_KEYS_CAMERA_UP, {KEYBOARD_KEY_CODE_UP, BIND_NONE}),
    KB("camera_down", InGame, Camera, CameraHeldKeys, 1, 0, 0, TEXT_ID_KEYS_CAMERA_DOWN, {KEYBOARD_KEY_CODE_DOWN, BIND_NONE}),
    KB("camera_tilt_minus", InGame, Camera, CameraHeldKeys, 1, 0, 0, TEXT_ID_KEYS_CAMERA_TILT_MINUS, {KEYBOARD_KEY_CODE_PAGE_UP, BIND_NONE}),
    KB("camera_tilt_plus", InGame, Camera, CameraHeldKeys, 1, 0, 0, TEXT_ID_KEYS_CAMERA_TILT_PLUS, {KEYBOARD_KEY_CODE_PAGE_DOWN, BIND_NONE}),
    KB("camera_rotate_minus", InGame, Camera, CameraHeldKeys, 1, 0, 0, TEXT_ID_KEYS_CAMERA_ROTATE_MINUS, {KEYBOARD_KEY_CODE_INSERT, BIND_NONE}),
    KB("camera_rotate_plus", InGame, Camera, CameraHeldKeys, 1, 0, 0, TEXT_ID_KEYS_CAMERA_ROTATE_PLUS, {KEYBOARD_KEY_CODE_DELETE, BIND_NONE}),
    KB("camera_zoom_in", InGame, Camera, CameraHeldKeys, 1, 0, 0, TEXT_ID_KEYS_CAMERA_ZOOM_IN, {KEYBOARD_KEY_CODE_HOME, BIND_NONE}),
    KB("camera_zoom_out", InGame, Camera, CameraHeldKeys, 1, 0, 0, TEXT_ID_KEYS_CAMERA_ZOOM_OUT, {KEYBOARD_KEY_CODE_END, BIND_NONE}),
    KB("camera_selection", InGame, Camera, InGameKeyCommands, 1, 0, 8, TEXT_ID_KEYS_CAMERA_SELECTION, {KEYBOARD_KEY_CODE_NUMPAD_5, BIND_NONE}),
    /* B: the first own model of runtime class 11 (the headquarters, "Hauptquartier" 301/1301/2301) */
    KB("camera_hq", InGame, Camera, InGameKeyCommands, 1, 0, 9, TEXT_ID_KEYS_CAMERA_HQ, {KeyChar('b'), BIND_NONE}),
    KB("camera_notification", InGame, Camera, InGameKeyCommands, 1, 0, 7, TEXT_ID_KEYS_CAMERA_NOTIFICATION, {KEYBOARD_KEY_CODE_SPACE, BIND_ALT}),
    KB("camera_free", InGame, Camera, InGameKeyCommands, 1, 0, 16, TEXT_ID_KEYS_CAMERA_FREE, {KeyChar('c'), BIND_ALT}),
    /* Menues und Filme */
    KB_PERMANENT("menu_leave", Menus, MenusAndMovies, FrontendHotkeys, 1, 0, 1, TEXT_ID_KEYS_MENU_LEAVE,
                 (KeyChord{KEYBOARD_KEY_CODE_FUNCTION(4), BIND_ALT}), {KeyChar('q'), BIND_ALT},
                 {KEYBOARD_KEY_CODE_FUNCTION(4), BIND_ALT}),
    KB("ui_next_control", Ui, MenusAndMovies, UiControls, 1, FIXED, 0, TEXT_ID_KEYS_UI_NEXT_CONTROL, {KEYBOARD_KEY_CODE_TAB, BIND_NONE}),
    KB("ui_press_button", Ui, MenusAndMovies, UiControls, 1, FIXED, 0, TEXT_ID_KEYS_UI_PRESS_BUTTON, {KEYBOARD_KEY_CODE_SPACE, BIND_NONE}),
    KB("ui_confirm_cancel", Ui, MenusAndMovies, UiControls, 1, FIXED, 0, TEXT_ID_KEYS_UI_CONFIRM_CANCEL,
       {KEYBOARD_KEY_CODE_ENTER, BIND_NONE}, {KEYBOARD_KEY_CODE_ESCAPE, BIND_NONE}),
    KB("ui_list", Ui, MenusAndMovies, UiControls, 1, FIXED, 0, TEXT_ID_KEYS_UI_LIST,
       {KEY_BINDING_CODE_ARROWS, BIND_NONE}, {KEYBOARD_KEY_CODE_PAGE_UP, BIND_NONE}, {KEYBOARD_KEY_CODE_PAGE_DOWN, BIND_NONE},
       {KEYBOARD_KEY_CODE_HOME, BIND_NONE}, {KEYBOARD_KEY_CODE_END, BIND_NONE}),
    KB("ui_slider", Ui, MenusAndMovies, UiControls, 1, FIXED, 0, TEXT_ID_KEYS_UI_SLIDER,
       {KEY_BINDING_CODE_ARROWS, BIND_NONE}, {KEY_BINDING_CODE_ARROWS, BIND_CTRL}),
    KB("intro_skip_all", Ui, MenusAndMovies, IntroMovies, 1, FIXED, 0, TEXT_ID_KEYS_INTRO_SKIP_ALL, {KEYBOARD_KEY_CODE_ESCAPE, BIND_NONE}),
    KB("intro_skip", Ui, MenusAndMovies, IntroMovies, 1, FIXED, 0, TEXT_ID_KEYS_INTRO_SKIP, {KEY_BINDING_CODE_ANY_KEY, BIND_NONE}),
    /* the end-movie table holds character codes (see EndMovieCommands_FindBinding) */
    KB("end_movie_skip", EndMovie, MenusAndMovies, EndMovieCommands, 1, FIXED, 1, TEXT_ID_KEYS_END_MOVIE_SKIP, {'q', BIND_ALT}),
    KB("end_movie_screenshot", EndMovie, MenusAndMovies, EndMovieCommands, 1, FIXED, 2, TEXT_ID_KEYS_END_MOVIE_SCREENSHOT, {'p', BIND_CTRL}),
    /* hidden: the cheat keys and the hidden Ctrl+F1 of the faction setup page (an eighth colour in the faction cycle) */
    KB("cheat_xenite", InGame, InGameGeneral, InGameHotkeys, 1, HIDDEN, 13, TEXT_RESOURCE_ID_NONE, {KeyChar('x'), BIND_CTRL | BIND_ALT}),
    KB("cheat_energy", InGame, InGameGeneral, InGameHotkeys, 1, HIDDEN, 14, TEXT_RESOURCE_ID_NONE, {KeyChar('e'), BIND_CTRL | BIND_ALT}),
    KB("cheat_fast_build", InGame, InGameGeneral, InGameHotkeys, 1, HIDDEN, 15, TEXT_RESOURCE_ID_NONE, {KeyChar('z'), BIND_CTRL | BIND_ALT}),
    KB("cheat_occupancy", InGame, InGameGeneral, InGameKeyCommands, 1, HIDDEN, 18, TEXT_RESOURCE_ID_NONE, {KeyChar('v'), BIND_CTRL | BIND_ALT}),
    KB("faction_setup_ready", Menus, MenusAndMovies, FrontendHotkeys, 1, HIDDEN, 2, TEXT_RESOURCE_ID_NONE, {KEYBOARD_KEY_CODE_FUNCTION(1), BIND_CTRL}),
};
#undef KB
#undef KB_PERMANENT

static uint32_t g_KeyBindingsChangeCount = 0;
static KeyBindingsChangeListener g_KeyBindingsListeners[KEY_BINDINGS_MAX_LISTENERS] = {};

/* Fills keys from defaults once (before the first access). */
static void KeyBindings_EnsureInitialized()
{
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    for (KeyBinding &binding : g_KeyBindings) {
      std::memcpy(binding.keys,binding.defaults,sizeof(binding.keys));
    }
  }
}

static KeyBinding &KeyBindings_Entry(KeyAction action)
{
  KeyBindings_EnsureInitialized();
  return g_KeyBindings[static_cast<int>(action)];
}

static bool KeyChord_Equal(KeyChord left,KeyChord right)
{
  return (left.keyCode == right.keyCode) && (left.modifiers == right.modifiers);
}

/* Shift / Ctrl / Alt of either side widened to both sides; every other bit dropped. */
static UiKeyboardStateMask KeyChord_NormalizeModifiers(UiKeyboardStateMask modifiers)
{
  UiKeyboardStateMask normalized = BIND_NONE;
  if (Any(modifiers & KEYBOARD_STATE_SHIFT)) normalized = normalized | BIND_SHIFT;
  if (Any(modifiers & KEYBOARD_STATE_CTRL)) normalized = normalized | BIND_CTRL;
  if (Any(modifiers & KEYBOARD_STATE_ALT)) normalized = normalized | BIND_ALT;
  return normalized;
}

/* ---- conflict simulation ---- */

enum class KeyMatchRule : uint8_t {
  ShiftIgnoredUnlessShift, /* in-game hotkeys, end movie: Ctrl/Alt exact; Shift required when the chord has it */
  ExactWithShift,          /* key commands, frontend hotkeys */
  Held                     /* camera, UI keys: a chord without modifiers matches any; else its modifiers held */
};

static KeyMatchRule KeyBindings_RuleOf(KeyBindingSource source)
{
  switch (source) {
  case KeyBindingSource::InGameHotkeys:
  case KeyBindingSource::EndMovieCommands: return KeyMatchRule::ShiftIgnoredUnlessShift;
  case KeyBindingSource::InGameKeyCommands:
  case KeyBindingSource::FrontendHotkeys: return KeyMatchRule::ExactWithShift;
  case KeyBindingSource::CameraHeldKeys:
  case KeyBindingSource::IntroMovies:
  case KeyBindingSource::UiControls: return KeyMatchRule::Held;
  }
  return KeyMatchRule::ExactWithShift;
}

/* Does a press with the held modifiers (normalized) reach a chord with chordModifiers (normalized)? */
static bool KeyMatch_Modifiers(KeyMatchRule rule,UiKeyboardStateMask chordModifiers,UiKeyboardStateMask held)
{
  switch (rule) {
  case KeyMatchRule::ExactWithShift:
    return chordModifiers == held;
  case KeyMatchRule::ShiftIgnoredUnlessShift:
    if (Any(chordModifiers & BIND_SHIFT) && !Any(held & BIND_SHIFT)) {
      return false;
    }
    return (chordModifiers & (BIND_CTRL | BIND_ALT)) == (held & (BIND_CTRL | BIND_ALT));
  case KeyMatchRule::Held:
    return (held & chordModifiers) == chordModifiers;
  }
  return false;
}

/* The key codes a chord stands for: the range, or the four arrows; none for KEY_BINDING_CODE_ANY_KEY. */
static int KeyChord_Expand(KeyChord chord,uint8_t rangeLength,uint32_t (&outKeys)[8])
{
  if (chord.keyCode == KEY_BINDING_CODE_ANY_KEY || chord.keyCode == 0) {
    return 0;
  }
  if (chord.keyCode == KEY_BINDING_CODE_ARROWS) {
    outKeys[0] = KEYBOARD_KEY_CODE_LEFT;
    outKeys[1] = KEYBOARD_KEY_CODE_RIGHT;
    outKeys[2] = KEYBOARD_KEY_CODE_UP;
    outKeys[3] = KEYBOARD_KEY_CODE_DOWN;
    return 4;
  }
  int count = (rangeLength < 1) ? 1 : (rangeLength > 8 ? 8 : rangeLength);
  for (int offset = 0; offset < count; offset++) {
    outKeys[offset] = chord.keyCode + static_cast<uint32_t>(offset);
  }
  return count;
}

/* One side of a conflict test: a chord, its range, its dispatcher's modifier rule and dispatch priority. */
enum class KeyDispatchPriority : uint8_t {
  First,  /* the world view's key commands: get a key before the other tables */
  Table,  /* the in-game hotkeys, the frontend hotkeys, the end-movie table */
  Held,   /* polled or handled by the controls: always reached */
  Press   /* a bare press (KeyBindings_FindConflict without an action) */
};

struct KeySide {
  KeyChord chord;
  uint8_t rangeLength;
  KeyMatchRule rule;
  KeyDispatchPriority priority;
};

static KeyDispatchPriority KeyBindings_PriorityOf(KeyBindingSource source)
{
  switch (source) {
  case KeyBindingSource::InGameKeyCommands: return KeyDispatchPriority::First;
  case KeyBindingSource::InGameHotkeys:
  case KeyBindingSource::FrontendHotkeys:
  case KeyBindingSource::EndMovieCommands: return KeyDispatchPriority::Table;
  case KeyBindingSource::CameraHeldKeys:
  case KeyBindingSource::IntroMovies:
  case KeyBindingSource::UiControls: return KeyDispatchPriority::Held;
  }
  return KeyDispatchPriority::Table;
}

static bool KeySide_IsTable(const KeySide &side)
{
  return (side.priority == KeyDispatchPriority::First) || (side.priority == KeyDispatchPriority::Table);
}

/* Simulated dispatch of the two chords' own presses (each chord's key with exactly its modifiers): they conflict
   when a press reaches both (both tables of one priority, or a held key and anything), or when a press does not
   reach its own chord because the other one's table gets the key first. A press that only also reaches the other
   chord's dispatcher after its own table took it is no conflict (Pause = S and Shift+S of the world view: the world
   view gets Shift+S first, S still pauses). */
static bool KeySide_Conflict(const KeySide &a,const KeySide &b)
{
  uint32_t keysA[8];
  uint32_t keysB[8];
  int countA = KeyChord_Expand(a.chord,a.rangeLength,keysA);
  int countB = KeyChord_Expand(b.chord,b.rangeLength,keysB);
  bool sameKey = false;
  for (int i = 0; i < countA && !sameKey; i++) {
    for (int j = 0; j < countB; j++) {
      if (keysA[i] == keysB[j]) {
        sameKey = true;
        break;
      }
    }
  }
  if (!sameKey) {
    return false;
  }
  for (int which = 0; which < 2; which++) {
    if ((which == 1) && (a.priority == KeyDispatchPriority::Press)) {
      break; /* a bare press has no chord of its own to be reached by b's press */
    }
    const KeySide &owner = (which == 0) ? a : b;
    const KeySide &other = (which == 0) ? b : a;
    UiKeyboardStateMask press = owner.chord.modifiers;
    if (!KeyMatch_Modifiers(other.rule,other.chord.modifiers,press)) {
      continue;
    }
    if (KeySide_IsTable(owner) && KeySide_IsTable(other) && (owner.priority < other.priority)) {
      continue; /* the owner's table gets its press first */
    }
    return true;
  }
  return false;
}

static bool KeyBindings_SameContext(KeyBindingContext query,KeyBindingContext other)
{
  if (query == other) {
    return true;
  }
  /* the frontend hotkeys and the general UI keys reach the same menus */
  return ((query == KeyBindingContext::Menus) && (other == KeyBindingContext::Ui)) ||
         ((query == KeyBindingContext::Ui) && (other == KeyBindingContext::Menus));
}

/* Esc (any modifiers), Alt+F4 and Alt+Tab (with any other modifiers). */
static bool KeyChord_IsReserved(KeyChord chord)
{
  if (chord.keyCode == KEYBOARD_KEY_CODE_ESCAPE) {
    return true;
  }
  if (Any(chord.modifiers & BIND_ALT) &&
      ((chord.keyCode == static_cast<uint32_t>(KEYBOARD_KEY_CODE_FUNCTION(4))) || (chord.keyCode == KEYBOARD_KEY_CODE_TAB))) {
    return true;
  }
  return false;
}

bool KeyBindings_IsBindableKey(uint32_t keyCode)
{
  uint32_t family = keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK;
  uint32_t index = keyCode & KEYBOARD_KEY_CODE_INDEX_MASK;

  switch (keyCode) {
  case KEYBOARD_KEY_CODE_SPACE:
  case KEYBOARD_KEY_CODE_ENTER:
  case KEYBOARD_KEY_CODE_TAB:
  case KEYBOARD_KEY_CODE_BACKSPACE:
  case KEYBOARD_KEY_CODE_PRINT:
  case KEYBOARD_KEY_CODE_PAUSE:
  case KEYBOARD_KEY_CODE_INSERT:
  case KEYBOARD_KEY_CODE_DELETE:
  case KEYBOARD_KEY_CODE_HOME:
  case KEYBOARD_KEY_CODE_END:
  case KEYBOARD_KEY_CODE_PAGE_UP:
  case KEYBOARD_KEY_CODE_PAGE_DOWN:
  case KEYBOARD_KEY_CODE_UP:
  case KEYBOARD_KEY_CODE_DOWN:
  case KEYBOARD_KEY_CODE_LEFT:
  case KEYBOARD_KEY_CODE_RIGHT:
  case KEYBOARD_KEY_CODE_NUMPAD_5:
  case '*':
  case '/':
  case '+':
  case '-':
    return true;
  default:
    break;
  }
  if (family == static_cast<uint32_t>(KEYBOARD_KEY_CODE_FUNCTION(0))) {
    return (index >= 1) && (index <= 12);
  }
  if (family == static_cast<uint32_t>(KEYBOARD_KEY_CODE_CHAR(0))) {
    return ((index >= '0') && (index <= '9')) || ((index >= 'a') && (index <= 'z'));
  }
  return false;
}

static constexpr KeyBindingResult KEY_BINDING_RESULT_OK = {KeyBindingStatus::Ok, KeyAction::Count};
static constexpr KeyBindingResult KEY_BINDING_RESULT_NOT_ALLOWED = {KeyBindingStatus::NotAllowed, KeyAction::Count};

/* The conflict scan. Actions for which skip(index) is true are ignored. */
template <typename Skip>
static KeyBindingResult KeyBindings_Scan(KeyBindingContext context,const KeySide &candidate,Skip skip)
{
  KeyBindings_EnsureInitialized();
  for (int index = 0; index < KEY_ACTION_COUNT; index++) {
    const KeyBinding &other = g_KeyBindings[index];
    if (skip(index) || !KeyBindings_SameContext(context,other.context)) {
      continue;
    }
    for (int keyIndex = 0; (keyIndex < KEY_BINDING_MAX_KEYS) && (other.keys[keyIndex].keyCode != 0); keyIndex++) {
      KeySide otherSide = {other.keys[keyIndex], other.rangeLength, KeyBindings_RuleOf(other.source),
                           KeyBindings_PriorityOf(other.source)};
      if (KeySide_Conflict(candidate,otherSide)) {
        if (KeyBinding_IsHidden(other)) {
          return KEY_BINDING_RESULT_NOT_ALLOWED;
        }
        return {KeyBindingStatus::Conflict, static_cast<KeyAction>(index)};
      }
    }
  }
  return KEY_BINDING_RESULT_OK;
}

/* Validity of a chord apart from conflicts (key bindable, not reserved). */
static bool KeyChord_Allowed(KeyChord chord,uint8_t rangeLength)
{
  if (rangeLength > 1) {
    return chord.keyCode == KEY_BINDING_GROUP_KEY;
  }
  return KeyBindings_IsBindableKey(chord.keyCode) && !KeyChord_IsReserved(chord);
}

KeyBindingResult KeyBindings_FindConflict(KeyBindingContext context,KeyChord chord,uint8_t rangeLength,KeyAction ignore)
{
  chord.modifiers = KeyChord_NormalizeModifiers(chord.modifiers);
  if (!KeyChord_Allowed(chord,rangeLength)) {
    return KEY_BINDING_RESULT_NOT_ALLOWED;
  }
  int ignoreIndex = static_cast<int>(ignore);
  KeySide candidate = {chord, rangeLength, KeyMatchRule::ExactWithShift, KeyDispatchPriority::Press};
  if (ignoreIndex < KEY_ACTION_COUNT) {
    candidate.rule = KeyBindings_RuleOf(g_KeyBindings[ignoreIndex].source);
    candidate.priority = KeyBindings_PriorityOf(g_KeyBindings[ignoreIndex].source);
  }
  return KeyBindings_Scan(context,candidate,[ignoreIndex](int index) { return index == ignoreIndex; });
}

/* The chord as an action takes it (range actions keep their key). */
static KeyChord KeyBindings_AdaptChord(const KeyBinding &binding,KeyChord chord)
{
  chord.modifiers = KeyChord_NormalizeModifiers(chord.modifiers);
  if (binding.rangeLength > 1) {
    chord.keyCode = KEY_BINDING_GROUP_KEY;
  }
  return chord;
}

KeyBindingResult KeyBindings_Check(KeyAction action,KeyChord chord)
{
  if (static_cast<int>(action) >= KEY_ACTION_COUNT) {
    return KEY_BINDING_RESULT_NOT_ALLOWED;
  }
  const KeyBinding &binding = KeyBindings_Entry(action);
  if (KeyBinding_IsFixed(binding)) {
    return {KeyBindingStatus::Fixed, KeyAction::Count};
  }
  chord = KeyBindings_AdaptChord(binding,chord);
  if (KeyChord_Equal(chord,binding.permanent)) {
    return KEY_BINDING_RESULT_OK;
  }
  return KeyBindings_FindConflict(binding.context,chord,binding.rangeLength,action);
}

static void KeyBindings_Changed()
{
  g_KeyBindingsChangeCount++;
  for (KeyBindingsChangeListener listener : g_KeyBindingsListeners) {
    if (listener != nullptr) {
      listener();
    }
  }
}

/* Replaces keys by chords (count, plus the permanent chord when missing); true when they differ. */
static bool KeyBindings_Store(KeyBinding &binding,const KeyChord *chords,int count)
{
  KeyChord keys[KEY_BINDING_MAX_KEYS] = {};
  int stored = 0;
  for (int index = 0; index < count; index++) {
    bool duplicate = false;
    for (int earlier = 0; earlier < stored; earlier++) {
      duplicate = duplicate || KeyChord_Equal(keys[earlier],chords[index]);
    }
    if (!duplicate && !KeyChord_Equal(chords[index],binding.permanent) && (stored < KEY_BINDING_MAX_KEYS)) {
      keys[stored++] = chords[index];
    }
  }
  if ((binding.permanent.keyCode != 0) && (stored < KEY_BINDING_MAX_KEYS)) {
    keys[stored++] = binding.permanent;
  }
  bool changed = std::memcmp(keys,binding.keys,sizeof(keys)) != 0;
  std::memcpy(binding.keys,keys,sizeof(keys));
  return changed;
}

KeyBindingResult KeyBindings_Set(KeyAction action,KeyChord chord)
{
  KeyBindingResult result = KeyBindings_Check(action,chord);
  if (result.status != KeyBindingStatus::Ok) {
    return result;
  }
  KeyBinding &binding = KeyBindings_Entry(action);
  chord = KeyBindings_AdaptChord(binding,chord);
  if (KeyBindings_Store(binding,&chord,1)) {
    KeyBindings_Changed();
  }
  return result;
}

bool KeyBindings_Clear(KeyAction action)
{
  if (static_cast<int>(action) >= KEY_ACTION_COUNT) {
    return false;
  }
  KeyBinding &binding = KeyBindings_Entry(action);
  if (KeyBinding_IsFixed(binding)) {
    return false;
  }
  if (KeyBindings_Store(binding,nullptr,0)) {
    KeyBindings_Changed();
  }
  return true;
}

static bool KeyBindings_RestoreDefaults(KeyBinding &binding)
{
  if (std::memcmp(binding.keys,binding.defaults,sizeof(binding.keys)) == 0) {
    return false;
  }
  std::memcpy(binding.keys,binding.defaults,sizeof(binding.keys));
  return true;
}

void KeyBindings_ResetDefaults(KeyAction action)
{
  if (static_cast<int>(action) >= KEY_ACTION_COUNT) {
    return;
  }
  if (KeyBindings_RestoreDefaults(KeyBindings_Entry(action))) {
    KeyBindings_Changed();
  }
}

void KeyBindings_ResetAllDefaults()
{
  bool changed = false;
  KeyBindings_EnsureInitialized();
  for (KeyBinding &binding : g_KeyBindings) {
    changed = KeyBindings_RestoreDefaults(binding) || changed;
  }
  if (changed) {
    KeyBindings_Changed();
  }
}

bool KeyBindings_IsDefault(KeyAction action)
{
  if (static_cast<int>(action) >= KEY_ACTION_COUNT) {
    return true;
  }
  const KeyBinding &binding = KeyBindings_Entry(action);
  return std::memcmp(binding.keys,binding.defaults,sizeof(binding.keys)) == 0;
}

bool KeyBindings_AllDefault()
{
  for (int index = 0; index < KEY_ACTION_COUNT; index++) {
    if (!KeyBindings_IsDefault(static_cast<KeyAction>(index))) {
      return false;
    }
  }
  return true;
}

bool KeyBindings_SetRaw(KeyAction action,const KeyChord *chords,int count)
{
  if ((static_cast<int>(action) >= KEY_ACTION_COUNT) || (count < 0) || (count > KEY_BINDING_MAX_KEYS)) {
    return false;
  }
  KeyBinding &binding = KeyBindings_Entry(action);
  if (KeyBinding_IsFixed(binding)) {
    return false;
  }
  KeyChord adapted[KEY_BINDING_MAX_KEYS];
  for (int index = 0; index < count; index++) {
    adapted[index] = chords[index];
    adapted[index].modifiers = KeyChord_NormalizeModifiers(adapted[index].modifiers);
    if (!KeyChord_Equal(adapted[index],binding.permanent) && !KeyChord_Allowed(adapted[index],binding.rangeLength)) {
      return false;
    }
  }
  int needed = count + ((binding.permanent.keyCode != 0) ? 1 : 0);
  for (int index = 0; index < count; index++) {
    if (KeyChord_Equal(adapted[index],binding.permanent)) {
      needed--;
    }
  }
  if (needed > KEY_BINDING_MAX_KEYS) {
    return false;
  }
  KeyBindings_Store(binding,adapted,count);
  return true;
}

int KeyBindings_ResolveConflicts(void (*report)(KeyAction unbound,KeyAction kept))
{
  int unboundCount = 0;
  KeyBindings_EnsureInitialized();
  for (int index = 0; index < KEY_ACTION_COUNT; index++) {
    KeyBinding &binding = g_KeyBindings[index];
    if (KeyBinding_IsFixed(binding)) {
      continue;
    }
    KeyChord kept[KEY_BINDING_MAX_KEYS];
    int keptCount = 0;
    for (int keyIndex = 0; (keyIndex < KEY_BINDING_MAX_KEYS) && (binding.keys[keyIndex].keyCode != 0); keyIndex++) {
      KeyChord chord = binding.keys[keyIndex];
      KeyBindingResult result = KEY_BINDING_RESULT_OK;
      if (!KeyChord_Equal(chord,binding.permanent)) {
        if (!KeyChord_Allowed(chord,binding.rangeLength)) {
          result = KEY_BINDING_RESULT_NOT_ALLOWED;
        }
        else {
          /* against the fixed actions and the earlier ones */
          KeySide candidate = {chord, binding.rangeLength, KeyBindings_RuleOf(binding.source),
                               KeyBindings_PriorityOf(binding.source)};
          result = KeyBindings_Scan(binding.context,candidate,[index](int other) {
            return (other == index) || ((other > index) && !KeyBinding_IsFixed(g_KeyBindings[other]));
          });
        }
      }
      if (result.status == KeyBindingStatus::Ok) {
        kept[keptCount++] = chord;
      }
      else {
        unboundCount++;
        if (report != nullptr) {
          report(static_cast<KeyAction>(index),result.conflictAction);
        }
      }
    }
    KeyBindings_Store(binding,kept,keptCount);
  }
  return unboundCount;
}

bool KeyBindings_AddChangeListener(KeyBindingsChangeListener listener)
{
  for (KeyBindingsChangeListener existing : g_KeyBindingsListeners) {
    if (existing == listener) {
      return true;
    }
  }
  for (KeyBindingsChangeListener &slot : g_KeyBindingsListeners) {
    if (slot == nullptr) {
      slot = listener;
      return true;
    }
  }
  return false;
}

void KeyBindings_RemoveChangeListener(KeyBindingsChangeListener listener)
{
  for (KeyBindingsChangeListener &slot : g_KeyBindingsListeners) {
    if (slot == listener) {
      slot = nullptr;
    }
  }
}

uint32_t KeyBindings_ChangeCount()
{
  return g_KeyBindingsChangeCount;
}

void KeyBindings_NotifyChanged()
{
  KeyBindings_Changed();
}

/* A key that types a character into a text edit: the digits and letters (KEYBOARD_KEY_CODE_CHAR), Space and the
   numpad operators (plain ASCII codes). */
static bool KeyBindings_TypesText(uint32_t keyCode)
{
  return ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == static_cast<uint32_t>(KEYBOARD_KEY_CODE_CHAR(0))) ||
         (keyCode < static_cast<uint32_t>(KEYBOARD_KEY_CODE_SPECIAL(0)));
}

bool KeyBindings_HeldDown(KeyAction action)
{
  if (static_cast<int>(action) >= KEY_ACTION_COUNT) {
    return false;
  }
  const KeyBinding &binding = KeyBindings_Entry(action);
  UiKeyboardStateMask held = KeyChord_NormalizeModifiers(g_KeyboardStateMask);
  /* while a text edit (chat, message, save name) has the keyboard focus its typed keys only type; the special keys
     (arrows etc.) still count, as the original's camera keys move the camera while the chat edit has the focus */
  const bool textEditFocused = (g_UiKeyboardFocusNode != nullptr) && (g_UiKeyboardFocusNode != UI_NODE_NONE) &&
                               (g_UiKeyboardFocusNode->vtable == &g_UiRequiredTextEditControlVtable);
  for (int keyIndex = 0; (keyIndex < KEY_BINDING_MAX_KEYS) && (binding.keys[keyIndex].keyCode != 0); keyIndex++) {
    const KeyChord &chord = binding.keys[keyIndex];
    if (textEditFocused && KeyBindings_TypesText(chord.keyCode)) {
      continue;
    }
    if (Keyboard_KeyHeld(chord.keyCode) && KeyMatch_Modifiers(KeyMatchRule::Held,chord.modifiers,held)) {
      return true;
    }
  }
  return false;
}

int KeyBindings_Count()
{
  return KEY_ACTION_COUNT;
}

const KeyBinding &KeyBindings_Get(KeyAction action)
{
  if (static_cast<int>(action) >= KEY_ACTION_COUNT) {
    action = KeyAction::GameMenu;
  }
  return KeyBindings_Entry(action);
}

/* ---- ini tokens ---- */

struct KeyTokenName {
  uint32_t keyCode;
  const char *token;
};

static const KeyTokenName g_KeyTokens[] = {
    {KEYBOARD_KEY_CODE_ESCAPE, "Esc"},
    {KEYBOARD_KEY_CODE_ENTER, "Enter"},
    {KEYBOARD_KEY_CODE_TAB, "Tab"},
    {KEYBOARD_KEY_CODE_BACKSPACE, "Backspace"},
    {KEYBOARD_KEY_CODE_SPACE, "Space"},
    {KEYBOARD_KEY_CODE_PRINT, "Print"},
    {KEYBOARD_KEY_CODE_PAUSE, "Pause"},
    {KEYBOARD_KEY_CODE_INSERT, "Insert"},
    {KEYBOARD_KEY_CODE_DELETE, "Delete"},
    {KEYBOARD_KEY_CODE_HOME, "Home"},
    {KEYBOARD_KEY_CODE_END, "End"},
    {KEYBOARD_KEY_CODE_PAGE_UP, "PageUp"},
    {KEYBOARD_KEY_CODE_PAGE_DOWN, "PageDown"},
    {KEYBOARD_KEY_CODE_UP, "Up"},
    {KEYBOARD_KEY_CODE_DOWN, "Down"},
    {KEYBOARD_KEY_CODE_LEFT, "Left"},
    {KEYBOARD_KEY_CODE_RIGHT, "Right"},
    {KEYBOARD_KEY_CODE_NUMPAD_5, "Num5"},
    {'*', "Num*"},
    {'/', "Num/"},
    {'+', "Num+"},
    {'-', "Num-"},
    {KEY_BINDING_CODE_ARROWS, "Arrows"},
    {KEY_BINDING_CODE_ANY_KEY, "AnyKey"},
};

static char KeyToken_Lower(char c)
{
  return ((c >= 'A') && (c <= 'Z')) ? static_cast<char>(c - 'A' + 'a') : c;
}

/* Case-insensitive compare of text[0..length) with token. */
static bool KeyToken_Equal(const char *text,uint32_t length,const char *token)
{
  uint32_t index = 0;
  for (; index < length; index++) {
    if ((token[index] == 0) || (KeyToken_Lower(text[index]) != KeyToken_Lower(token[index]))) {
      return false;
    }
  }
  return token[index] == 0;
}

static bool KeyToken_StartsWith(const char *text,uint32_t length,const char *prefix)
{
  uint32_t prefixLength = static_cast<uint32_t>(std::strlen(prefix));
  return (length >= prefixLength) && KeyToken_Equal(text,prefixLength,prefix);
}

const char *KeyBindings_Name(KeyAction action)
{
  return (static_cast<int>(action) < KEY_ACTION_COUNT) ? g_KeyBindings[static_cast<int>(action)].name : "";
}

bool KeyBindings_FindByName(const char *name,KeyAction *outAction)
{
  if (name == nullptr) {
    return false;
  }
  uint32_t length = static_cast<uint32_t>(std::strlen(name));
  for (int index = 0; index < KEY_ACTION_COUNT; index++) {
    if (KeyToken_Equal(name,length,g_KeyBindings[index].name)) {
      *outAction = static_cast<KeyAction>(index);
      return true;
    }
  }
  return false;
}

struct KeyTokenWriter {
  char *out;
  uint32_t capacity;
  uint32_t length;

  void Put(char c)
  {
    if (length + 1 < capacity) {
      out[length] = c;
      length++;
    }
  }
  void Put(const char *text)
  {
    for (; *text != 0; text++) {
      Put(*text);
    }
  }
  void Terminate()
  {
    if (capacity > 0) {
      out[length] = 0;
    }
  }
};

/* The key alone; false for a key without a token. */
static bool KeyToken_PutKey(KeyTokenWriter &writer,uint32_t keyCode)
{
  for (const KeyTokenName &name : g_KeyTokens) {
    if (name.keyCode == keyCode) {
      writer.Put(name.token);
      return true;
    }
  }
  uint32_t family = keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK;
  uint32_t index = keyCode & KEYBOARD_KEY_CODE_INDEX_MASK;
  if ((family == static_cast<uint32_t>(KEYBOARD_KEY_CODE_FUNCTION(0))) && (index >= 1) && (index <= 12)) {
    writer.Put('F');
    if (index >= 10) {
      writer.Put(static_cast<char>('0' + index / 10));
    }
    writer.Put(static_cast<char>('0' + index % 10));
    return true;
  }
  /* key-down codes of digits and letters, and the plain characters of the end-movie table */
  if ((family == static_cast<uint32_t>(KEYBOARD_KEY_CODE_CHAR(0))) || (family == 0)) {
    if ((index >= '0') && (index <= '9')) {
      writer.Put(static_cast<char>(index));
      return true;
    }
    if ((index >= 'a') && (index <= 'z')) {
      writer.Put(static_cast<char>(index - 'a' + 'A'));
      return true;
    }
  }
  return false;
}

static void KeyToken_PutChord(KeyTokenWriter &writer,KeyChord chord,uint8_t rangeLength,bool *outOk)
{
  if (Any(chord.modifiers & KEYBOARD_STATE_CTRL)) writer.Put("Ctrl+");
  if (Any(chord.modifiers & KEYBOARD_STATE_ALT)) writer.Put("Alt+");
  if (Any(chord.modifiers & KEYBOARD_STATE_SHIFT)) writer.Put("Shift+");
  *outOk = KeyToken_PutKey(writer,chord.keyCode);
  if (*outOk && (rangeLength > 1)) {
    writer.Put("..");
    *outOk = KeyToken_PutKey(writer,chord.keyCode + rangeLength - 1);
  }
}

uint32_t KeyChord_FormatToken(KeyChord chord,uint8_t rangeLength,char *out,uint32_t capacity)
{
  KeyTokenWriter writer{out, capacity, 0};
  bool ok = false;
  KeyToken_PutChord(writer,chord,rangeLength,&ok);
  if (!ok) {
    writer.length = 0;
  }
  writer.Terminate();
  return writer.length;
}

/* The key code of a key token (text[0..length)), 0 when there is none. */
static uint32_t KeyToken_ParseKey(const char *text,uint32_t length)
{
  for (const KeyTokenName &name : g_KeyTokens) {
    if ((name.keyCode < KEY_BINDING_CODE_ARROWS) && KeyToken_Equal(text,length,name.token)) {
      return name.keyCode;
    }
  }
  if (length == 1) {
    char c = KeyToken_Lower(text[0]);
    if (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z'))) {
      return KEYBOARD_KEY_CODE_CHAR(static_cast<uint32_t>(c));
    }
    return 0;
  }
  if (((length == 2) || (length == 3)) && (KeyToken_Lower(text[0]) == 'f')) {
    uint32_t number = 0;
    for (uint32_t index = 1; index < length; index++) {
      if ((text[index] < '0') || (text[index] > '9')) {
        return 0;
      }
      number = number * 10 + static_cast<uint32_t>(text[index] - '0');
    }
    if ((number >= 1) && (number <= 12) && (text[1] != '0')) {
      return KEYBOARD_KEY_CODE_FUNCTION(number);
    }
  }
  return 0;
}

static bool KeyToken_IsBlank(char c)
{
  return (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n');
}

/* Parses text[0..length) (blanks trimmed here). */
static bool KeyChord_ParseSpan(const char *text,uint32_t length,uint8_t rangeLength,KeyChord *outChord)
{
  while ((length > 0) && KeyToken_IsBlank(text[0])) {
    text++;
    length--;
  }
  while ((length > 0) && KeyToken_IsBlank(text[length - 1])) {
    length--;
  }
  UiKeyboardStateMask modifiers = BIND_NONE;
  for (;;) {
    if (KeyToken_StartsWith(text,length,"ctrl+")) {
      modifiers = modifiers | BIND_CTRL;
      text += 5;
      length -= 5;
    }
    else if (KeyToken_StartsWith(text,length,"alt+")) {
      modifiers = modifiers | BIND_ALT;
      text += 4;
      length -= 4;
    }
    else if (KeyToken_StartsWith(text,length,"shift+")) {
      modifiers = modifiers | BIND_SHIFT;
      text += 6;
      length -= 6;
    }
    else {
      break;
    }
  }
  if (rangeLength > 1) {
    /* "1" or "1..<last>" */
    if ((length == 0) || (text[0] != '1')) {
      return false;
    }
    if (length > 1) {
      char last = static_cast<char>('0' + rangeLength);
      if ((rangeLength > 9) || (length != 4) || (text[1] != '.') || (text[2] != '.') || (text[3] != last)) {
        return false;
      }
    }
    *outChord = {KEY_BINDING_GROUP_KEY, modifiers};
    return true;
  }
  uint32_t keyCode = KeyToken_ParseKey(text,length);
  if (keyCode == 0) {
    return false;
  }
  *outChord = {keyCode, modifiers};
  return true;
}

bool KeyChord_ParseToken(const char *text,uint8_t rangeLength,KeyChord *outChord)
{
  if (text == nullptr) {
    return false;
  }
  return KeyChord_ParseSpan(text,static_cast<uint32_t>(std::strlen(text)),rangeLength,outChord);
}

uint32_t KeyBindings_FormatTokens(KeyAction action,char *out,uint32_t capacity)
{
  KeyTokenWriter writer{out, capacity, 0};
  if (static_cast<int>(action) < KEY_ACTION_COUNT) {
    const KeyBinding &binding = KeyBindings_Entry(action);
    bool any = false;
    for (int keyIndex = 0; (keyIndex < KEY_BINDING_MAX_KEYS) && (binding.keys[keyIndex].keyCode != 0); keyIndex++) {
      if (KeyChord_Equal(binding.keys[keyIndex],binding.permanent)) {
        continue;
      }
      if (any) {
        writer.Put(", ");
      }
      bool ok = false;
      KeyToken_PutChord(writer,binding.keys[keyIndex],binding.rangeLength,&ok);
      any = true;
    }
    if (!any) {
      writer.Put("none");
    }
  }
  writer.Terminate();
  return writer.length;
}

int KeyBindings_ParseTokens(const char *text,uint8_t rangeLength,KeyChord *outChords,int capacity)
{
  if (text == nullptr) {
    return -1;
  }
  uint32_t length = static_cast<uint32_t>(std::strlen(text));
  uint32_t start = 0;
  while ((start < length) && KeyToken_IsBlank(text[start])) {
    start++;
  }
  uint32_t end = length;
  while ((end > start) && KeyToken_IsBlank(text[end - 1])) {
    end--;
  }
  if (KeyToken_Equal(text + start,end - start,"none")) {
    return 0;
  }
  int count = 0;
  uint32_t position = start;
  for (;;) {
    uint32_t separator = position;
    while ((separator < end) && (text[separator] != ',')) {
      separator++;
    }
    if (count >= capacity) {
      return -1;
    }
    if (!KeyChord_ParseSpan(text + position,separator - position,rangeLength,&outChords[count])) {
      return -1;
    }
    count++;
    if (separator >= end) {
      break;
    }
    position = separator + 1;
  }
  return count;
}

/* ---- display names ---- */

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
    const KeyChord &key = binding.keys[keyIndex];
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
