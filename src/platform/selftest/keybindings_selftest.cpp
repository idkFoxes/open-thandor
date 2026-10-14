/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/keybindings_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=keybindings: the key bindings (ui/core/key_bindings.h).
   1. The defaults against the game's real key handling. Every key of every action (each member of a 1..8 range,
      each arrow key of KEY_BINDING_CODE_ARROWS) is pressed through the keyboard layer (Keyboard_OnKeyDown with the
      modifier keys held, or Keyboard_OnChar for the character codes of the end-movie table) and the queued event
      must carry the key code; then the action's dispatcher must give the action's number:
        InGameHotkeys      InGameHotkeys_FindBinding, and the world view's table must not take the key first
        InGameKeyCommands  InGameKeyCommands_FindBinding
        CameraHeldKeys     the g_KeyboardSpecialKeyDown entry InGameUiRoot_ApplyCameraKeys polls is set,
                           KeyBindings_HeldDown gives the action, and neither in-game table takes the key
        FrontendHotkeys    FrontendHotkeys_FindBinding
        EndMovieCommands   EndMovieCommands_FindBinding
        IntroMovies        Esc is KEYBOARD_KEY_CODE_ESCAPE (IntroMovie_PollSkipRequest)
      Every action of the four tables must have an entry. Every visible entry's keys and every single key must
      format to a non-empty name, and every text id (entries, section titles, page title) must resolve to a
      non-empty text, in German and in English.
   2. The model rules: the defaults are conflict-free, names are unique, ini tokens round-trip (every bindable key
      with every modifier combination, every editable action's value), and Set / Clear / Reset follow the conflict
      policy (docs/plans/key_rebinding.md) with change notification. The bindings are back at their defaults
      afterwards.
   One line per failed check and a summary line. */

#include <stdint.h>
#include <cstring>
#include <initializer_list>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>
#include <thandor/platform/win32_constants.h>
#include <thandor/platform/input/devices.h>
#include <thandor/assets/text/resources.h>
#include <thandor/ui/core/key_bindings.h>
#include <thandor/ui/ingame/hotkeys.h>
#include <thandor/ui/ingame/key_commands.h>
#include <thandor/ui/frontend/state.h>
#include <thandor/ui/frontend/end_movie_commands.h>

static uint32_t g_KeyBindingsTestFailures = 0;

static void KeyBindingsTest_Fail(uint32_t row,uint32_t keyCode,uint32_t modifiers,const char *what)
{
  g_KeyBindingsTestFailures++;
  Thandor_Log("keybindings FAIL row %u key 0x%05X mod 0x%02X: %s", row, keyCode, modifiers, what);
}

/* The virtual key whose key-down event carries keyCode (no modifier held), 0 when none does. Modifier and lock
   keys are skipped (they change the state mask and queue no event). */
static uint32_t KeyBindingsTest_VirtualKeyOf(uint32_t keyCode)
{
  uint32_t virtualKey;
  uint32_t eventCode;
  UiKeyboardStateMask eventState;

  for (virtualKey = 1; virtualKey < 256; virtualKey++) {
    if (((virtualKey >= VK_SHIFT) && (virtualKey <= VK_MENU)) || (virtualKey == VK_CAPITAL) ||
        (virtualKey == VK_NUMLOCK) || (virtualKey == VK_SCROLL) || ((virtualKey >= VK_LSHIFT) && (virtualKey <= VK_RMENU))) {
      continue;
    }
    Keyboard_OnKeyDown(virtualKey);
    bool queued = Keyboard_ReadNextEvent(&eventCode,&eventState);
    Keyboard_OnKeyUp(virtualKey);
    if (queued && (eventCode == keyCode)) {
      return virtualKey;
    }
  }
  return 0;
}

static void KeyBindingsTest_HoldModifiers(UiKeyboardStateMask modifiers,bool down)
{
  void (*const press)(KeyboardVirtualKeyCode) = down ? Keyboard_OnKeyDown : Keyboard_OnKeyUp;
  if (Any(modifiers & KEYBOARD_STATE_SHIFT)) press(VK_LSHIFT);
  if (Any(modifiers & KEYBOARD_STATE_CTRL)) press(VK_LCONTROL);
  if (Any(modifiers & KEYBOARD_STATE_ALT)) press(VK_LMENU);
}

static bool KeyBindingsTest_IsCameraKey(uint32_t keyCode)
{
  static const uint32_t cameraKeys[] = {
      KEYBOARD_KEY_CODE_LEFT, KEYBOARD_KEY_CODE_RIGHT, KEYBOARD_KEY_CODE_UP, KEYBOARD_KEY_CODE_DOWN,
      KEYBOARD_KEY_CODE_PAGE_UP, KEYBOARD_KEY_CODE_PAGE_DOWN, KEYBOARD_KEY_CODE_INSERT, KEYBOARD_KEY_CODE_DELETE,
      KEYBOARD_KEY_CODE_HOME, KEYBOARD_KEY_CODE_END};
  for (uint32_t cameraKey : cameraKeys) {
    if (cameraKey == keyCode) {
      return true;
    }
  }
  return false;
}

/* Presses one key of a row and checks the event and the row's dispatcher. Returns 1 (one key checked). */
static uint32_t KeyBindingsTest_CheckKey(uint32_t row,const KeyBinding &binding,uint32_t keyCode,
                                         UiKeyboardStateMask modifiers)
{
  uint32_t mods = ToBits(modifiers);
  uint32_t eventCode = 0;
  UiKeyboardStateMask eventState = KEYBOARD_STATE_NONE;
  bool queued;

  Keyboard_FlushEvents();
  if ((keyCode >= 'a') && (keyCode <= 'z')) {
    /* a character event: Ctrl+letter arrives as control character 1..26 (WM_CHAR, SdlInput's Ctrl+A..Z),
       Alt+letter as the letter (WM_SYSCHAR) */
    KeyBindingsTest_HoldModifiers(modifiers,true);
    Keyboard_OnChar(Any(modifiers & KEYBOARD_STATE_CTRL) ? (keyCode - 'a' + 1) : keyCode);
    queued = Keyboard_ReadNextEvent(&eventCode,&eventState);
    KeyBindingsTest_HoldModifiers(modifiers,false);
  }
  else {
    uint32_t virtualKey = KeyBindingsTest_VirtualKeyOf(keyCode);
    if (virtualKey == 0) {
      KeyBindingsTest_Fail(row,keyCode,mods,"no virtual key produces this key code");
      return 1;
    }
    KeyBindingsTest_HoldModifiers(modifiers,true);
    Keyboard_OnKeyDown(virtualKey);
    queued = Keyboard_ReadNextEvent(&eventCode,&eventState);
    if ((binding.source == KeyBindingSource::CameraHeldKeys) &&
        (g_KeyboardSpecialKeyDown[keyCode & KEYBOARD_KEY_CODE_INDEX_MASK] == 0)) {
      KeyBindingsTest_Fail(row,keyCode,mods,"camera key not marked as held");
    }
    if ((binding.source == KeyBindingSource::CameraHeldKeys) && !KeyBindings_HeldDown(static_cast<KeyAction>(row))) {
      KeyBindingsTest_Fail(row,keyCode,mods,"KeyBindings_HeldDown does not see the held camera key");
    }
    Keyboard_OnKeyUp(virtualKey);
    KeyBindingsTest_HoldModifiers(modifiers,false);
  }
  if (!queued || (eventCode != keyCode)) {
    KeyBindingsTest_Fail(row,keyCode,mods,"the key press queues another key code");
    return 1;
  }
  /* the held left-side keys give the left bits only */
  if ((Any(eventState & KEYBOARD_STATE_SHIFT) != Any(modifiers & KEYBOARD_STATE_SHIFT)) ||
      (Any(eventState & KEYBOARD_STATE_CTRL) != Any(modifiers & KEYBOARD_STATE_CTRL)) ||
      (Any(eventState & KEYBOARD_STATE_ALT) != Any(modifiers & KEYBOARD_STATE_ALT))) {
    KeyBindingsTest_Fail(row,keyCode,mods,"the event carries other modifiers");
  }
  switch (binding.source) {
  case KeyBindingSource::InGameHotkeys:
    if (InGameHotkeys_FindBinding(eventCode,eventState) != binding.sourceAction) {
      KeyBindingsTest_Fail(row,keyCode,mods,"in-game hotkey table gives another action");
    }
    if (InGameKeyCommands_FindBinding(eventCode,eventState) != 0) {
      KeyBindingsTest_Fail(row,keyCode,mods,"the world view's key command table takes the key first");
    }
    break;
  case KeyBindingSource::InGameKeyCommands:
    if (InGameKeyCommands_FindBinding(eventCode,eventState) != binding.sourceAction) {
      KeyBindingsTest_Fail(row,keyCode,mods,"in-game key command table gives another action");
    }
    break;
  case KeyBindingSource::CameraHeldKeys:
    if (!KeyBindingsTest_IsCameraKey(keyCode)) {
      KeyBindingsTest_Fail(row,keyCode,mods,"not a key InGameUiRoot_ApplyCameraKeys polls");
    }
    if ((InGameKeyCommands_FindBinding(eventCode,eventState) != 0) || (InGameHotkeys_FindBinding(eventCode,eventState) != 0)) {
      KeyBindingsTest_Fail(row,keyCode,mods,"an in-game key table also uses the camera key");
    }
    break;
  case KeyBindingSource::FrontendHotkeys:
    if (FrontendHotkeys_FindBinding(eventCode,eventState) != binding.sourceAction) {
      KeyBindingsTest_Fail(row,keyCode,mods,"frontend hotkey table gives another action");
    }
    break;
  case KeyBindingSource::EndMovieCommands:
    if (EndMovieCommands_FindBinding(eventCode,eventState) != binding.sourceAction) {
      KeyBindingsTest_Fail(row,keyCode,mods,"end-movie command table gives another action");
    }
    break;
  case KeyBindingSource::IntroMovies:
    if (keyCode != KEYBOARD_KEY_CODE_ESCAPE) {
      KeyBindingsTest_Fail(row,keyCode,mods,"the intros only test Esc");
    }
    break;
  case KeyBindingSource::UiControls:
    break;
  }
  return 1;
}

static bool KeyBindingsTest_TextPresent(TextResourceId textId)
{
  uint16_t *text = nullptr;
  return TextResource_TryResolve(textId,&text) && (text != nullptr) && (text[0] != 0);
}

/* Names and texts in the current language. */
static void KeyBindingsTest_CheckNames(const char *language)
{
  uint16_t name[96];

  for (uint32_t row = 0; row < (uint32_t)KeyBindings_Count(); row++) {
    const KeyBinding &binding = KeyBindings_Get(static_cast<KeyAction>(row));
    if (KeyBinding_IsHidden(binding)) {
      continue;
    }
    if (KeyBinding_FormatKeys(binding,name,96) == 0) {
      KeyBindingsTest_Fail(row,binding.keys[0].keyCode,ToBits(binding.keys[0].modifiers),language);
    }
    for (int keyIndex = 0; (keyIndex < KEY_BINDING_MAX_KEYS) && (binding.keys[keyIndex].keyCode != 0); keyIndex++) {
      for (uint32_t offset = 0; offset < binding.rangeLength; offset++) {
        if (KeyName_Format(binding.keys[keyIndex].keyCode + offset,binding.keys[keyIndex].modifiers,name,96) == 0) {
          KeyBindingsTest_Fail(row,binding.keys[keyIndex].keyCode + offset,ToBits(binding.keys[keyIndex].modifiers),
                               "key name empty");
        }
      }
    }
    if ((binding.textId < (TextResourceId)TEXT_ID_KEY_BINDING_BASE) ||
        (binding.textId >= (TextResourceId)(TEXT_ID_KEY_BINDING_BASE + TEXT_ID_KEY_BINDING_COUNT)) ||
        !KeyBindingsTest_TextPresent(binding.textId)) {
      KeyBindingsTest_Fail(row,0,0,"action text missing");
    }
  }
  for (TextResourceId textId = TEXT_ID_KEY_BINDING_BASE; textId < (TextResourceId)(TEXT_ID_KEY_BINDING_BASE + TEXT_ID_KEY_BINDING_COUNT);
       textId++) {
    if (!KeyBindingsTest_TextPresent(textId)) {
      Thandor_Log("keybindings FAIL text 0x%06X empty (%s)", textId, language);
      g_KeyBindingsTestFailures++;
    }
  }
}

/* ---- part 2: the model rules ---- */

static uint32_t g_KeyBindingsTestNotifications = 0;

static void KeyBindingsTest_Listener()
{
  g_KeyBindingsTestNotifications++;
}

static void KeyBindingsTest_ModelFail(const char *what)
{
  g_KeyBindingsTestFailures++;
  Thandor_Log("keybindings FAIL model: %s", what);
}

static constexpr KeyChord KeyBindingsTest_Chord(uint32_t keyCode,UiKeyboardStateMask modifiers = KEYBOARD_STATE_NONE)
{
  return KeyChord{keyCode, modifiers};
}

static constexpr uint32_t KeyBindingsTest_Char(char c)
{
  return KEYBOARD_KEY_CODE_CHAR(static_cast<uint32_t>(c));
}

/* Set must give status (and conflict action). */
static void KeyBindingsTest_ExpectSet(KeyAction action,KeyChord chord,KeyBindingStatus status,
                                      KeyAction conflict,const char *what)
{
  KeyBindingResult result = KeyBindings_Set(action,chord);
  if ((result.status != status) || (result.conflictAction != conflict)) {
    g_KeyBindingsTestFailures++;
    Thandor_Log("keybindings FAIL model: %s (status %d, conflict %d)", what, (int)result.status,
                (int)result.conflictAction);
  }
}

/* The action's keys must be exactly chords. */
static void KeyBindingsTest_ExpectKeys(KeyAction action,std::initializer_list<KeyChord> chords,const char *what)
{
  const KeyBinding &binding = KeyBindings_Get(action);
  int index = 0;
  bool equal = true;
  for (const KeyChord &chord : chords) {
    equal = equal && (index < KEY_BINDING_MAX_KEYS) && (binding.keys[index].keyCode == chord.keyCode) &&
            (binding.keys[index].modifiers == chord.modifiers);
    index++;
  }
  equal = equal && ((index >= KEY_BINDING_MAX_KEYS) || (binding.keys[index].keyCode == 0));
  if (!equal) {
    KeyBindingsTest_ModelFail(what);
  }
}

static uint32_t KeyBindingsTest_TokenRoundTrips()
{
  static const UiKeyboardStateMask modifierSets[] = {
      KEYBOARD_STATE_NONE, KEYBOARD_STATE_SHIFT, KEYBOARD_STATE_CTRL, KEYBOARD_STATE_ALT,
      KEYBOARD_STATE_CTRL | KEYBOARD_STATE_SHIFT, KEYBOARD_STATE_ALT | KEYBOARD_STATE_SHIFT,
      KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT, KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT | KEYBOARD_STATE_SHIFT};
  uint32_t checked = 0;
  char token[64];
  KeyChord parsed;

  /* every bindable key-down code (scanned over the families) with every modifier combination */
  for (uint32_t family : {0u, 0x10000u, 0x20000u, 0x30000u}) {
    for (uint32_t index = 0; index < 0x80; index++) {
      uint32_t keyCode = family + index;
      if (!KeyBindings_IsBindableKey(keyCode)) {
        continue;
      }
      for (UiKeyboardStateMask modifiers : modifierSets) {
        checked++;
        if ((KeyChord_FormatToken(KeyChord{keyCode, modifiers},1,token,sizeof(token)) == 0) ||
            !KeyChord_ParseToken(token,1,&parsed) || (parsed.keyCode != keyCode) || (parsed.modifiers != modifiers)) {
          g_KeyBindingsTestFailures++;
          Thandor_Log("keybindings FAIL token round trip key 0x%05X mod 0x%02X \"%s\"", keyCode, ToBits(modifiers), token);
        }
      }
    }
  }
  /* every editable action's ini value */
  for (int action = 0; action < KeyBindings_Count(); action++) {
    const KeyBinding &binding = KeyBindings_Get(static_cast<KeyAction>(action));
    KeyChord chords[KEY_BINDING_MAX_KEYS];
    if (KeyBinding_IsFixed(binding)) {
      continue;
    }
    checked++;
    KeyBindings_FormatTokens(static_cast<KeyAction>(action),token,sizeof(token));
    int count = KeyBindings_ParseTokens(token,binding.rangeLength,chords,KEY_BINDING_MAX_KEYS);
    bool equal = count >= 0;
    int keyIndex = 0;
    for (int chordIndex = 0; equal && chordIndex < count; chordIndex++, keyIndex++) {
      equal = (binding.keys[keyIndex].keyCode == chords[chordIndex].keyCode) &&
              (binding.keys[keyIndex].modifiers == chords[chordIndex].modifiers);
    }
    /* only the permanent chord may follow */
    if (equal && (keyIndex < KEY_BINDING_MAX_KEYS) && (binding.keys[keyIndex].keyCode != 0)) {
      equal = (binding.keys[keyIndex].keyCode == binding.permanent.keyCode) &&
              (binding.keys[keyIndex].modifiers == binding.permanent.modifiers);
    }
    if (!equal) {
      g_KeyBindingsTestFailures++;
      Thandor_Log("keybindings FAIL ini value round trip %s = \"%s\"", binding.name, token);
    }
  }
  /* fixed spellings */
  struct TokenCase {
    const char *text;
    uint8_t rangeLength;
    bool valid;
    KeyChord chord;
  };
  static const TokenCase cases[] = {
      {"Ctrl+1..8", 8, true, {KEYBOARD_KEY_CODE_CHAR('1'), KEYBOARD_STATE_CTRL}},
      {" alt+shift+1 ", 8, true, {KEYBOARD_KEY_CODE_CHAR('1'), KEYBOARD_STATE_ALT | KEYBOARD_STATE_SHIFT}},
      {"Ctrl+2..8", 8, false, {}},
      {"Ctrl+1..8", 1, false, {}},
      {"alt+g", 1, true, {KEYBOARD_KEY_CODE_CHAR('g'), KEYBOARD_STATE_ALT}},
      {"Shift+Num+", 1, true, {'+', KEYBOARD_STATE_SHIFT}},
      {"PageUp", 1, true, {KEYBOARD_KEY_CODE_PAGE_UP, KEYBOARD_STATE_NONE}},
      {"F12", 1, true, {KEYBOARD_KEY_CODE_FUNCTION(12), KEYBOARD_STATE_NONE}},
      {"F13", 1, false, {}},
      {"F01", 1, false, {}},
      {"Ctrl+", 1, false, {}},
      {"Hyper+A", 1, false, {}},
  };
  for (const TokenCase &tokenCase : cases) {
    checked++;
    KeyChord chord{};
    bool valid = KeyChord_ParseToken(tokenCase.text,tokenCase.rangeLength,&chord);
    if ((valid != tokenCase.valid) ||
        (valid && ((chord.keyCode != tokenCase.chord.keyCode) || (chord.modifiers != tokenCase.chord.modifiers)))) {
      g_KeyBindingsTestFailures++;
      Thandor_Log("keybindings FAIL token \"%s\"", tokenCase.text);
    }
  }
  KeyChord chords[KEY_BINDING_MAX_KEYS];
  if (KeyBindings_ParseTokens(" None ",1,chords,KEY_BINDING_MAX_KEYS) != 0) {
    KeyBindingsTest_ModelFail("none is not an empty binding");
  }
  if (KeyBindings_ParseTokens("S, Alt+Shift+S",1,chords,KEY_BINDING_MAX_KEYS) != 2 ||
      chords[1].modifiers != (KEYBOARD_STATE_ALT | KEYBOARD_STATE_SHIFT)) {
    KeyBindingsTest_ModelFail("two chords not parsed");
  }
  if (KeyBindings_ParseTokens("S,",1,chords,KEY_BINDING_MAX_KEYS) != -1 ||
      KeyBindings_ParseTokens("A, B",1,chords,1) != -1) {
    KeyBindingsTest_ModelFail("an empty chord or too many chords accepted");
  }
  checked += 3;
  return checked;
}

static void KeyBindingsTest_Rules()
{
  using enum KeyAction;
  const KeyBindingStatus OK = KeyBindingStatus::Ok;
  const KeyBindingStatus CONFLICT = KeyBindingStatus::Conflict;
  const KeyBindingStatus NOT_ALLOWED = KeyBindingStatus::NotAllowed;
  const KeyAction NONE = KeyAction::Count;

  /* names: unique, found again */
  for (int action = 0; action < KeyBindings_Count(); action++) {
    KeyAction found = NONE;
    if (!KeyBindings_FindByName(KeyBindings_Name(static_cast<KeyAction>(action)),&found) ||
        (static_cast<int>(found) != action)) {
      g_KeyBindingsTestFailures++;
      Thandor_Log("keybindings FAIL name %s not unique", KeyBindings_Name(static_cast<KeyAction>(action)));
    }
  }
  KeyAction found = NONE;
  if (!KeyBindings_FindByName("CAMERA_LEFT",&found) || (found != CameraLeft) || KeyBindings_FindByName("camera",&found)) {
    KeyBindingsTest_ModelFail("FindByName");
  }
  if (KeyBindings_ResolveConflicts(nullptr) != 0 || !KeyBindings_AllDefault()) {
    KeyBindingsTest_ModelFail("the defaults conflict");
  }

  g_KeyBindingsTestNotifications = 0;
  KeyBindings_AddChangeListener(KeyBindingsTest_Listener);
  uint32_t changeCount = KeyBindings_ChangeCount();

  /* conflicts and reserved keys; nothing changes */
  KeyBindingsTest_ExpectSet(Pause,KeyBindingsTest_Chord(KeyBindingsTest_Char('s')),CONFLICT,Stop,"Pause = S");
  KeyBindingsTest_ExpectSet(Pause,KeyBindingsTest_Chord(KeyBindingsTest_Char('c'),KEYBOARD_STATE_ALT),CONFLICT,CameraFree,
                            "Pause = Alt+C (world view key)");
  KeyBindingsTest_ExpectSet(Pause,KeyBindingsTest_Chord(KEYBOARD_KEY_CODE_ESCAPE),NOT_ALLOWED,NONE,"Pause = Esc");
  KeyBindingsTest_ExpectSet(Pause,KeyBindingsTest_Chord(KEYBOARD_KEY_CODE_FUNCTION(4),KEYBOARD_STATE_ALT | KEYBOARD_STATE_CTRL),
                            NOT_ALLOWED,NONE,"Pause = Ctrl+Alt+F4");
  KeyBindingsTest_ExpectSet(Pause,KeyBindingsTest_Chord(KeyBindingsTest_Char('x'),KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT),
                            NOT_ALLOWED,NONE,"Pause = Ctrl+Alt+X (cheat)");
  KeyBindingsTest_ExpectSet(Pause,KeyBindingsTest_Chord(0x41),NOT_ALLOWED,NONE,"Pause = a character code");
  KeyBindingsTest_ExpectSet(GameMenu,KeyBindingsTest_Chord(KeyBindingsTest_Char('k')),KeyBindingStatus::Fixed,NONE,
                            "GameMenu is fixed");
  KeyBindingsTest_ExpectSet(Chat,KeyBindingsTest_Chord(KEYBOARD_KEY_CODE_TAB),CONFLICT,SidePanel,"Chat = Tab");
  KeyBindingsTest_ExpectSet(MenuLeave,KeyBindingsTest_Chord(KEYBOARD_KEY_CODE_SPACE),CONFLICT,UiPressButton,
                            "MenuLeave = Space");
  KeyBindingsTest_ExpectSet(MenuLeave,KeyBindingsTest_Chord(KEYBOARD_KEY_CODE_FUNCTION(1),KEYBOARD_STATE_CTRL),NOT_ALLOWED,NONE,
                            "MenuLeave = Ctrl+F1 (hidden)");
  KeyBindingsTest_ExpectSet(GroupStore,KeyBindingsTest_Chord(KeyBindingsTest_Char('5'),KEYBOARD_STATE_SHIFT),CONFLICT,
                            GroupSelectAdd,"GroupStore = Shift+1..8");
  KeyBindingsTest_ExpectSet(CameraLeft,KeyBindingsTest_Chord(KeyBindingsTest_Char('a')),CONFLICT,SelectAirports,
                            "CameraLeft = A");
  if (KeyBindings_ChangeCount() != changeCount || g_KeyBindingsTestNotifications != 0 || !KeyBindings_AllDefault()) {
    KeyBindingsTest_ModelFail("a refused change changed something");
  }

  /* applied changes */
  KeyBindingsTest_ExpectSet(Pause,KeyBindingsTest_Chord(KeyBindingsTest_Char('k'),KEYBOARD_STATE_LEFT_SHIFT),OK,NONE,
                            "Pause = Shift+K");
  KeyBindingsTest_ExpectKeys(Pause,{{KeyBindingsTest_Char('k'), KEYBOARD_STATE_SHIFT}},"Shift+K widened to both sides");
  if (KeyBindings_ChangeCount() != changeCount + 1 || g_KeyBindingsTestNotifications != 1 || KeyBindings_IsDefault(Pause)) {
    KeyBindingsTest_ModelFail("Set did not notify once");
  }
  KeyBindingsTest_ExpectSet(Pause,KeyBindingsTest_Chord(KeyBindingsTest_Char('k'),KEYBOARD_STATE_SHIFT),OK,NONE,
                            "Pause = Shift+K again");
  if (g_KeyBindingsTestNotifications != 1) {
    KeyBindingsTest_ModelFail("an unchanged Set notified");
  }
  KeyBindingResult conflict = KeyBindings_FindConflict(KeyBindingContext::InGame,
                                                       KeyBindingsTest_Chord(KeyBindingsTest_Char('k'),KEYBOARD_STATE_SHIFT));
  if (conflict.status != CONFLICT || conflict.conflictAction != Pause ||
      KeyBindings_FindConflict(KeyBindingContext::Menus,KeyBindingsTest_Chord(KeyBindingsTest_Char('k'),KEYBOARD_STATE_SHIFT)).status != OK) {
    KeyBindingsTest_ModelFail("FindConflict by context");
  }
  KeyBindingsTest_ExpectSet(Faster,KeyBindingsTest_Chord(KeyBindingsTest_Char('p')),OK,NONE,"Faster = P (freed)");
  KeyBindingsTest_ExpectSet(Slower,KeyBindingsTest_Chord(KeyBindingsTest_Char('p'),KEYBOARD_STATE_SHIFT),CONFLICT,Faster,
                            "Slower = Shift+P (the hotkey Faster = P ignores Shift)");
  KeyBindingsTest_ExpectSet(StopMovement,KeyBindingsTest_Chord(KeyBindingsTest_Char('p'),KEYBOARD_STATE_SHIFT),OK,NONE,
                            "StopMovement = Shift+P (the world view gets Shift+P first, P still reaches Faster)");
  KeyBindingsTest_ExpectSet(Faster,KeyBindingsTest_Chord(KeyBindingsTest_Char('d'),KEYBOARD_STATE_ALT),CONFLICT,SelfDestruct,
                            "Faster = Alt+D (the world view would take it first)");
  KeyBindingsTest_ExpectSet(Stop,KeyBindingsTest_Chord(KeyBindingsTest_Char('x')),OK,NONE,"Stop = X");
  KeyBindingsTest_ExpectKeys(Stop,{{KeyBindingsTest_Char('x'), KEYBOARD_STATE_NONE}},"Stop = X drops Alt+Shift+S");
  KeyBindingsTest_ExpectSet(GroupStore,KeyBindingsTest_Chord(0,KEYBOARD_STATE_CTRL),OK,NONE,"GroupStore = Ctrl");
  KeyBindingsTest_ExpectKeys(GroupStore,{{KeyBindingsTest_Char('1'), KEYBOARD_STATE_CTRL}},"GroupStore keeps 1..8");
  KeyBindingsTest_ExpectSet(GroupSelect,KeyBindingsTest_Chord(0,KEYBOARD_STATE_ALT),OK,NONE,"GroupSelect = Alt (freed)");
  KeyBindingsTest_ExpectSet(CameraLeft,KeyBindingsTest_Chord(KeyBindingsTest_Char('w')),OK,NONE,"CameraLeft = W");
  KeyBindingsTest_ExpectSet(Pause,KeyBindingsTest_Chord(KeyBindingsTest_Char('w'),KEYBOARD_STATE_CTRL),CONFLICT,CameraLeft,
                            "Pause = Ctrl+W (held camera key)");
  KeyBindingsTest_ExpectSet(MenuLeave,KeyBindingsTest_Chord(KeyBindingsTest_Char('p')),OK,NONE,"MenuLeave = P");
  KeyBindingsTest_ExpectKeys(MenuLeave,{{KeyBindingsTest_Char('p'), KEYBOARD_STATE_NONE},
                                        {KEYBOARD_KEY_CODE_FUNCTION(4), KEYBOARD_STATE_ALT}},"MenuLeave keeps Alt+F4");
  if (!KeyBindings_Clear(MenuLeave) || KeyBindings_Clear(GameMenu)) {
    KeyBindingsTest_ModelFail("Clear");
  }
  KeyBindingsTest_ExpectKeys(MenuLeave,{{KEYBOARD_KEY_CODE_FUNCTION(4), KEYBOARD_STATE_ALT}},"Clear keeps Alt+F4");
  char value[64];
  KeyBindings_FormatTokens(MenuLeave,value,sizeof(value));
  if (std::strcmp(value,"none") != 0) {
    KeyBindingsTest_ModelFail("an unbound action's value is not none");
  }
  KeyBindings_Clear(Pause);
  KeyBindingsTest_ExpectKeys(Pause,{},"Clear unbinds");
  KeyBindings_ResetDefaults(Pause);
  if (!KeyBindings_IsDefault(Pause) || KeyBindings_AllDefault()) {
    KeyBindingsTest_ModelFail("ResetDefaults of one action");
  }
  KeyBindings_ResetAllDefaults();
  if (!KeyBindings_AllDefault()) {
    KeyBindingsTest_ModelFail("ResetAllDefaults");
  }

  /* the ini path: raw values, then the later action of a conflict loses its chord */
  KeyChord rawS = KeyBindingsTest_Chord(KeyBindingsTest_Char('s'));
  KeyChord rawEsc = KeyBindingsTest_Chord(KEYBOARD_KEY_CODE_ESCAPE);
  uint32_t notifications = g_KeyBindingsTestNotifications;
  if (!KeyBindings_SetRaw(Pause,&rawS,1) || KeyBindings_SetRaw(Pause,&rawEsc,1) || KeyBindings_SetRaw(GameMenu,&rawS,1)) {
    KeyBindingsTest_ModelFail("SetRaw validation");
  }
  if (KeyBindings_ResolveConflicts(nullptr) != 1) {
    KeyBindingsTest_ModelFail("ResolveConflicts count");
  }
  KeyBindingsTest_ExpectKeys(Pause,{rawS},"ResolveConflicts keeps the earlier action");
  KeyBindingsTest_ExpectKeys(Stop,{{KeyBindingsTest_Char('s'), KEYBOARD_STATE_ALT | KEYBOARD_STATE_SHIFT}},
                             "ResolveConflicts unbinds the later chord");
  if (g_KeyBindingsTestNotifications != notifications) {
    KeyBindingsTest_ModelFail("SetRaw / ResolveConflicts notified");
  }
  KeyBindings_NotifyChanged();
  if (g_KeyBindingsTestNotifications != notifications + 1) {
    KeyBindingsTest_ModelFail("NotifyChanged");
  }
  KeyBindings_RemoveChangeListener(KeyBindingsTest_Listener);
  KeyBindings_ResetAllDefaults();
}

void Thandor_SelfTestKeyBindings()
{
  uint32_t count = (uint32_t)KeyBindings_Count();
  uint32_t keysChecked = 0;
  uint32_t row;
  bool hotkeyListed[16] = {};
  bool keyCommandListed[20] = {};
  bool frontendListed[3] = {};
  bool endMovieListed[3] = {};
  int previousSection = 0;

  g_KeyBindingsTestFailures = 0;
  KeyBindings_ResetAllDefaults();
  for (row = 0; row < count; row++) {
    const KeyBinding &binding = KeyBindings_Get(static_cast<KeyAction>(row));
    if (!KeyBinding_IsHidden(binding)) {
      if (static_cast<int>(binding.section) < previousSection) {
        KeyBindingsTest_Fail(row,0,0,"entries not sorted by section");
      }
      previousSection = static_cast<int>(binding.section);
    }
    if ((binding.rangeLength == 0) || (binding.keys[0].keyCode == 0)) {
      KeyBindingsTest_Fail(row,0,0,"entry without keys");
      continue;
    }
    if ((binding.source == KeyBindingSource::InGameHotkeys) && (binding.sourceAction < 16)) {
      hotkeyListed[binding.sourceAction] = true;
    }
    if ((binding.source == KeyBindingSource::InGameKeyCommands) && (binding.sourceAction < 20)) {
      keyCommandListed[binding.sourceAction] = true;
    }
    if ((binding.source == KeyBindingSource::FrontendHotkeys) && (binding.sourceAction < 3)) {
      frontendListed[binding.sourceAction] = true;
    }
    if ((binding.source == KeyBindingSource::EndMovieCommands) && (binding.sourceAction < 3)) {
      endMovieListed[binding.sourceAction] = true;
    }
    for (int keyIndex = 0; (keyIndex < KEY_BINDING_MAX_KEYS) && (binding.keys[keyIndex].keyCode != 0); keyIndex++) {
      const KeyChord &key = binding.keys[keyIndex];
      if (key.keyCode == KEY_BINDING_CODE_ANY_KEY) {
        continue;
      }
      if (key.keyCode == KEY_BINDING_CODE_ARROWS) {
        static const uint32_t arrows[] = {KEYBOARD_KEY_CODE_LEFT, KEYBOARD_KEY_CODE_RIGHT, KEYBOARD_KEY_CODE_UP,
                                          KEYBOARD_KEY_CODE_DOWN};
        for (uint32_t arrow : arrows) {
          keysChecked += KeyBindingsTest_CheckKey(row,binding,arrow,key.modifiers);
        }
        continue;
      }
      for (uint32_t offset = 0; offset < binding.rangeLength; offset++) {
        keysChecked += KeyBindingsTest_CheckKey(row,binding,key.keyCode + offset,key.modifiers);
      }
    }
  }
  Keyboard_FlushEvents();
  /* InGameHotkeyAction 1..15, InGameKeyCommandAction 1..19, FrontendHotkeyAction 1..2, EndMovieCommandAction 1..2 */
  for (uint32_t action = 1; action <= 15; action++) {
    if (!hotkeyListed[action]) {
      Thandor_Log("keybindings FAIL in-game hotkey action %u has no entry", action);
      g_KeyBindingsTestFailures++;
    }
  }
  for (uint32_t action = 1; action <= 19; action++) {
    if (!keyCommandListed[action]) {
      Thandor_Log("keybindings FAIL in-game key command action %u has no entry", action);
      g_KeyBindingsTestFailures++;
    }
  }
  for (uint32_t action = 1; action <= 2; action++) {
    if (!frontendListed[action] || !endMovieListed[action]) {
      Thandor_Log("keybindings FAIL frontend / end-movie action %u has no entry", action);
      g_KeyBindingsTestFailures++;
    }
  }
  bool english = KeyBindings_English();
  KeyBindings_SetEnglish(false);
  KeyBindingsTest_CheckNames("German");
  KeyBindings_SetEnglish(true);
  KeyBindingsTest_CheckNames("English");
  KeyBindings_SetEnglish(english);
  for (int section = 0; section < KEY_BINDING_SECTION_COUNT; section++) {
    if (!KeyBindingsTest_TextPresent(KeyBindings_SectionTitle(static_cast<KeyBindingSection>(section)))) {
      Thandor_Log("keybindings FAIL section %d title empty", section);
      g_KeyBindingsTestFailures++;
    }
  }
  uint32_t tokensChecked = KeyBindingsTest_TokenRoundTrips();
  KeyBindingsTest_Rules();
  Thandor_Log("keybindings: %u actions, %u keys pressed, %u tokens, %u failures", count, keysChecked, tokensChecked,
              g_KeyBindingsTestFailures);
}

