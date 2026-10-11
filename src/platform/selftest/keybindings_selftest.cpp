/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/keybindings_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=keybindings: the key binding list (ui/core/key_bindings.h) against the game's real key
   handling. Every key of every row (each member of a 1..8 range, each arrow key of KEY_BINDING_CODE_ARROWS) is
   pressed through the keyboard layer (Keyboard_OnKeyDown with the modifier keys held, or Keyboard_OnChar for the
   character codes of the end-movie table) and the queued event must carry the row's key code; then the row's
   dispatcher must give the row's action:
     InGameHotkeys      InGameHotkeys_FindBinding, and the world view's table must not take the key first
     InGameKeyCommands  InGameKeyCommands_FindBinding
     CameraHeldKeys     the g_KeyboardSpecialKeyDown entry InGameUiRoot_ApplyCameraKeys polls is set, and neither
                        in-game table takes the key
     FrontendHotkeys    FrontendHotkeys_FindBinding
     EndMovieCommands   EndMovieCommands_FindBinding
     IntroMovies        Esc is KEYBOARD_KEY_CODE_ESCAPE (IntroMovie_PollSkipRequest)
   Every action of the in-game tables except the cheats, and the frontend's Leave, must have a row. Every row's keys
   and every single key must format to a non-empty name, and every text id (rows, section titles, page title) must
   resolve to a non-empty text, in German and in English. One line per failed check and a summary line. */

#include <stdint.h>
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
    if (InGameHotkeys_FindBinding(eventCode,eventState) != binding.action) {
      KeyBindingsTest_Fail(row,keyCode,mods,"in-game hotkey table gives another action");
    }
    if (InGameKeyCommands_FindBinding(eventCode,eventState) != 0) {
      KeyBindingsTest_Fail(row,keyCode,mods,"the world view's key command table takes the key first");
    }
    break;
  case KeyBindingSource::InGameKeyCommands:
    if (InGameKeyCommands_FindBinding(eventCode,eventState) != binding.action) {
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
    if (FrontendHotkeys_FindBinding(eventCode,eventState) != binding.action) {
      KeyBindingsTest_Fail(row,keyCode,mods,"frontend hotkey table gives another action");
    }
    break;
  case KeyBindingSource::EndMovieCommands:
    if (EndMovieCommands_FindBinding(eventCode,eventState) != binding.action) {
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
static void KeyBindingsTest_CheckNames(const KeyBinding *table,uint32_t count,const char *language)
{
  uint16_t name[96];
  uint32_t row;

  for (row = 0; row < count; row++) {
    const KeyBinding &binding = table[row];
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

void Thandor_SelfTestKeyBindings()
{
  uint32_t count = 0;
  const KeyBinding *table = KeyBindings_Table(&count);
  uint32_t keysChecked = 0;
  uint32_t row;
  bool hotkeyListed[16] = {};
  bool keyCommandListed[20] = {};
  bool frontendLeaveListed = false;
  int previousSection = 0;

  g_KeyBindingsTestFailures = 0;
  for (row = 0; row < count; row++) {
    const KeyBinding &binding = table[row];
    if (static_cast<int>(binding.section) < previousSection) {
      KeyBindingsTest_Fail(row,0,0,"rows not sorted by section");
    }
    previousSection = static_cast<int>(binding.section);
    if ((binding.rangeLength == 0) || (binding.keys[0].keyCode == 0)) {
      KeyBindingsTest_Fail(row,0,0,"row without keys");
      continue;
    }
    if ((binding.source == KeyBindingSource::InGameHotkeys) && (binding.action < 16)) {
      hotkeyListed[binding.action] = true;
    }
    if ((binding.source == KeyBindingSource::InGameKeyCommands) && (binding.action < 20)) {
      keyCommandListed[binding.action] = true;
    }
    if ((binding.source == KeyBindingSource::FrontendHotkeys) && (binding.action == 1)) {
      frontendLeaveListed = true;
    }
    for (int keyIndex = 0; (keyIndex < KEY_BINDING_MAX_KEYS) && (binding.keys[keyIndex].keyCode != 0); keyIndex++) {
      const KeyBindingKey &key = binding.keys[keyIndex];
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
  /* InGameHotkeyAction 1..12 (13..15 are cheats), InGameKeyCommandAction 1..19 without the cheat 18 */
  for (uint32_t action = 1; action <= 12; action++) {
    if (!hotkeyListed[action]) {
      Thandor_Log("keybindings FAIL in-game hotkey action %u has no row", action);
      g_KeyBindingsTestFailures++;
    }
  }
  for (uint32_t action = 1; action <= 19; action++) {
    if ((action != 18) && !keyCommandListed[action]) {
      Thandor_Log("keybindings FAIL in-game key command action %u has no row", action);
      g_KeyBindingsTestFailures++;
    }
  }
  if (!frontendLeaveListed) {
    Thandor_Log("keybindings FAIL frontend action Leave has no row");
    g_KeyBindingsTestFailures++;
  }
  bool english = KeyBindings_English();
  KeyBindings_SetEnglish(false);
  KeyBindingsTest_CheckNames(table,count,"German");
  KeyBindings_SetEnglish(true);
  KeyBindingsTest_CheckNames(table,count,"English");
  KeyBindings_SetEnglish(english);
  for (int section = 0; section < KEY_BINDING_SECTION_COUNT; section++) {
    if (!KeyBindingsTest_TextPresent(KeyBindings_SectionTitle(static_cast<KeyBindingSection>(section)))) {
      Thandor_Log("keybindings FAIL section %d title empty", section);
      g_KeyBindingsTestFailures++;
    }
  }
  Thandor_Log("keybindings: %u rows, %u keys pressed, %u failures", count, keysChecked, g_KeyBindingsTestFailures);
}
