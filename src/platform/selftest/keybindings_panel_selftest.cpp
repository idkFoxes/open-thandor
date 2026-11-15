/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/keybindings_panel_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=keybindingspanel: the capture logic and the status texts of the key binding panel
   (ui/core/key_bindings_panel.h), without UI.
   1. KeyCapture: a key with Ctrl held is taken on release (its character event swallowed), a modifier pressed and
      released before the key does not count, Shift pressed while a held key is down counts (peak), Esc alone
      cancels and Esc with a modifier is captured, a character without key-down code is rejected, a group row
      takes the modifiers alone, an ordinary row does not, and the plain-ASCII key-down codes (Space, numpad
      operators) are key-down codes.
   2. KeyBindingsPanel_ApplyChord in German and English: a free key is set and the status names the action and the
      key, a used key gives "Taste bereits belegt: <Aktion>", Esc gives "Taste nicht erlaubt"; the bindings are back
      at their defaults afterwards.
   One line per failed check and a summary line. */

#include <stdint.h>
#include <cstring>
#include <string>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>
#include <thandor/assets/text/resources.h>
#include <thandor/ui/core/key_bindings.h>
#include <thandor/ui/core/key_bindings_panel.h>

static uint32_t g_KeyBindingsPanelTestFailures = 0;
static uint32_t g_KeyBindingsPanelTestChecks = 0;

static void KeyBindingsPanelTest_Check(bool ok,const char *what)
{
  g_KeyBindingsPanelTestChecks++;
  if (!ok) {
    g_KeyBindingsPanelTestFailures++;
    Thandor_Log("keybindingspanel FAIL %s",what);
  }
}

static bool KeyBindingsPanelTest_ChordIs(const KeyCapture &capture,uint32_t keyCode,UiKeyboardStateMask modifiers)
{
  const KeyChord chord = KeyCapture_Chord(&capture);
  return chord.keyCode == keyCode && chord.modifiers == modifiers;
}

static void KeyBindingsPanelTest_Capture()
{
  const uint32_t keyP = KEYBOARD_KEY_CODE_CHAR('p');
  KeyCapture capture = {};

  /* Ctrl held, P down (key event with Ctrl, then its character 'p'), P up, Ctrl still held, Ctrl up */
  KeyCapture_Begin(&capture,1);
  KeyBindingsPanelTest_Check(KeyCapture_OnTick(&capture,KEYBOARD_STATE_LEFT_CTRL,false) == KeyCaptureResult::Pending,
                             "Ctrl alone is pending");
  KeyBindingsPanelTest_Check(KeyCapture_OnKeyEvent(&capture,keyP,KEYBOARD_STATE_LEFT_CTRL) == KeyCaptureResult::Pending,
                             "key down is pending");
  KeyBindingsPanelTest_Check(KeyCapture_OnKeyEvent(&capture,'p',KEYBOARD_STATE_LEFT_CTRL) == KeyCaptureResult::Pending,
                             "character event swallowed");
  KeyBindingsPanelTest_Check(KeyCapture_OnTick(&capture,KEYBOARD_STATE_LEFT_CTRL,false) == KeyCaptureResult::Pending,
                             "Ctrl still held is pending");
  KeyBindingsPanelTest_Check(KeyCapture_OnTick(&capture,KEYBOARD_STATE_NONE,false) == KeyCaptureResult::Commit,
                             "release commits");
  KeyBindingsPanelTest_Check(KeyBindingsPanelTest_ChordIs(capture,keyP,KEYBOARD_STATE_CTRL),"chord Ctrl+P (both sides)");
  KeyBindingsPanelTest_Check(KeyCapture_OnKeyEvent(&capture,keyP,KEYBOARD_STATE_NONE) == KeyCaptureResult::Pending &&
                             !capture.active,"no capture after the commit");

  /* Shift pressed and released before the key does not count */
  KeyCapture_Begin(&capture,1);
  KeyCapture_OnTick(&capture,KEYBOARD_STATE_LEFT_SHIFT,false);
  KeyCapture_OnTick(&capture,KEYBOARD_STATE_NONE,false);
  KeyCapture_OnKeyEvent(&capture,keyP,KEYBOARD_STATE_NONE);
  KeyBindingsPanelTest_Check(KeyCapture_OnTick(&capture,KEYBOARD_STATE_NONE,false) == KeyCaptureResult::Commit &&
                             KeyBindingsPanelTest_ChordIs(capture,keyP,KEYBOARD_STATE_NONE),"earlier Shift ignored");

  /* a held key (Left) with Shift added while it is held: Shift+Left, taken when both are released */
  KeyCapture_Begin(&capture,1);
  KeyCapture_OnKeyEvent(&capture,KEYBOARD_KEY_CODE_LEFT,KEYBOARD_STATE_NONE);
  KeyBindingsPanelTest_Check(KeyCapture_OnTick(&capture,KEYBOARD_STATE_RIGHT_SHIFT,true) == KeyCaptureResult::Pending,
                             "held key pending");
  KeyBindingsPanelTest_Check(KeyCapture_OnTick(&capture,KEYBOARD_STATE_NONE,true) == KeyCaptureResult::Pending,
                             "held key without modifier pending");
  KeyBindingsPanelTest_Check(KeyCapture_OnTick(&capture,KEYBOARD_STATE_NONE,false) == KeyCaptureResult::Commit &&
                             KeyBindingsPanelTest_ChordIs(capture,KEYBOARD_KEY_CODE_LEFT,KEYBOARD_STATE_SHIFT),
                             "peak Shift+Left");

  /* Esc alone cancels; Esc with Alt is captured (KeyBindings_Set then refuses it) */
  KeyCapture_Begin(&capture,1);
  KeyBindingsPanelTest_Check(KeyCapture_OnKeyEvent(&capture,KEYBOARD_KEY_CODE_ESCAPE,KEYBOARD_STATE_NONE) ==
                             KeyCaptureResult::Cancel && !capture.active,"Esc cancels");
  KeyCapture_Begin(&capture,1);
  KeyCapture_OnKeyEvent(&capture,KEYBOARD_KEY_CODE_ESCAPE,KEYBOARD_STATE_LEFT_ALT);
  KeyBindingsPanelTest_Check(KeyCapture_OnTick(&capture,KEYBOARD_STATE_NONE,false) == KeyCaptureResult::Commit &&
                             KeyBindingsPanelTest_ChordIs(capture,KEYBOARD_KEY_CODE_ESCAPE,KEYBOARD_STATE_ALT),
                             "Alt+Esc captured");

  /* a character without key-down code ('#') */
  KeyCapture_Begin(&capture,1);
  KeyBindingsPanelTest_Check(KeyCapture_OnKeyEvent(&capture,'#',KEYBOARD_STATE_NONE) == KeyCaptureResult::Rejected &&
                             !capture.active,"character rejected");

  /* modifiers alone: a group row takes Ctrl+Alt (key 1), an ordinary row keeps waiting */
  KeyCapture_Begin(&capture,8);
  KeyCapture_OnTick(&capture,KEYBOARD_STATE_LEFT_CTRL,false);
  KeyCapture_OnTick(&capture,KEYBOARD_STATE_LEFT_CTRL | KEYBOARD_STATE_RIGHT_ALT,false);
  KeyCapture_OnTick(&capture,KEYBOARD_STATE_RIGHT_ALT,false);
  KeyBindingsPanelTest_Check(KeyCapture_OnTick(&capture,KEYBOARD_STATE_NONE,false) == KeyCaptureResult::Commit &&
                             KeyBindingsPanelTest_ChordIs(capture,KEYBOARD_KEY_CODE_CHAR('1'),
                                                          KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT),
                             "group row takes modifiers alone");
  KeyCapture_Begin(&capture,1);
  KeyCapture_OnTick(&capture,KEYBOARD_STATE_LEFT_CTRL,false);
  KeyBindingsPanelTest_Check(KeyCapture_OnTick(&capture,KEYBOARD_STATE_NONE,false) == KeyCaptureResult::Pending &&
                             capture.active,"ordinary row ignores modifiers alone");
  /* a group row with a digit: the digit's modifiers (KeyBindings_Set keeps the key 1) */
  KeyCapture_Begin(&capture,8);
  KeyCapture_OnKeyEvent(&capture,KEYBOARD_KEY_CODE_CHAR('3'),KEYBOARD_STATE_LEFT_SHIFT);
  KeyBindingsPanelTest_Check(KeyCapture_OnTick(&capture,KEYBOARD_STATE_NONE,false) == KeyCaptureResult::Commit &&
                             KeyBindingsPanelTest_ChordIs(capture,KEYBOARD_KEY_CODE_CHAR('3'),KEYBOARD_STATE_SHIFT),
                             "group row with a digit");

  KeyBindingsPanelTest_Check(KeyCapture_IsKeyDownCode(KEYBOARD_KEY_CODE_SPACE) && KeyCapture_IsKeyDownCode('*') &&
                             KeyCapture_IsKeyDownCode(KEYBOARD_KEY_CODE_FUNCTION(5)) &&
                             KeyCapture_IsKeyDownCode(KEYBOARD_KEY_CODE_NUMPAD_5) && !KeyCapture_IsKeyDownCode('a') &&
                             !KeyCapture_IsKeyDownCode(0xE4),"key-down codes");
}

/* True when text starts with the text of prefixId and contains the text of partId (TEXT_RESOURCE_ID_NONE: no
   check). */
static bool KeyBindingsPanelTest_StatusIs(const uint16_t *status,TextResourceId prefixId,TextResourceId partId)
{
  const uint16_t *prefix = TextResource_Resolve(prefixId);
  size_t length = 0;

  while (prefix[length] != 0) {
    if (status[length] != prefix[length]) {
      return false;
    }
    length++;
  }
  if (partId == TEXT_RESOURCE_ID_NONE) {
    return status[length] == 0;
  }
  const uint16_t *part = TextResource_Resolve(partId);
  for (const uint16_t *start = status; *start != 0; start++) {
    size_t index = 0;
    while (part[index] != 0 && start[index] == part[index]) {
      index++;
    }
    if (part[index] == 0) {
      return true;
    }
  }
  return false;
}

static void KeyBindingsPanelTest_Apply(bool english)
{
  uint16_t status[192];
  KeyBindingResult result;

  KeyBindings_SetEnglish(english);
  KeyBindings_ResetAllDefaults();
  /* F5 is free: Pause = F5, the status starts with the action's text */
  result = KeyBindingsPanel_ApplyChord(KeyAction::Pause,KeyChord{KEYBOARD_KEY_CODE_FUNCTION(5),KEYBOARD_STATE_NONE},
                                       status,192);
  {
    const uint16_t *description = TextResource_Resolve(KeyBindings_Get(KeyAction::Pause).textId);
    uint16_t keyName[32];
    const uint32_t descriptionLength = static_cast<uint32_t>(std::char_traits<char16_t>::length(
                                           reinterpret_cast<const char16_t *>(description)));
    const uint32_t nameLength = KeyName_Format(KEYBOARD_KEY_CODE_FUNCTION(5),KEYBOARD_STATE_NONE,keyName,32);
    KeyBindingsPanelTest_Check(result.status == KeyBindingStatus::Ok &&
                               std::memcmp(status,description,descriptionLength * sizeof(uint16_t)) == 0 &&
                               status[descriptionLength] == u':' && status[descriptionLength + 1] == u' ' &&
                               std::memcmp(status + descriptionLength + 2,keyName,(nameLength + 1) * sizeof(uint16_t)) == 0,
                               "Ok status \"<Aktion>: F5\"");
  }
  KeyBindingsPanelTest_Check(KeyBindings_Get(KeyAction::Pause).keys[0].keyCode == KEYBOARD_KEY_CODE_FUNCTION(5),
                             "Ok applied");
  /* S is Stop's: conflict, Pause keeps F5 */
  result = KeyBindingsPanel_ApplyChord(KeyAction::Pause,KeyChord{KEYBOARD_KEY_CODE_CHAR('s'),KEYBOARD_STATE_NONE},
                                       status,192);
  KeyBindingsPanelTest_Check(result.status == KeyBindingStatus::Conflict &&
                             KeyBindingsPanelTest_StatusIs(status,TEXT_ID_KEYS_PANEL_CONFLICT,TEXT_ID_KEYS_STOP),
                             "Conflict status names Stop");
  KeyBindingsPanelTest_Check(KeyBindings_Get(KeyAction::Pause).keys[0].keyCode == KEYBOARD_KEY_CODE_FUNCTION(5),
                             "Conflict keeps the old key");
  /* Esc: not allowed */
  result = KeyBindingsPanel_ApplyChord(KeyAction::Pause,KeyChord{KEYBOARD_KEY_CODE_ESCAPE,KEYBOARD_STATE_ALT},
                                       status,192);
  KeyBindingsPanelTest_Check(result.status == KeyBindingStatus::NotAllowed &&
                             KeyBindingsPanelTest_StatusIs(status,TEXT_ID_KEYS_PANEL_NOT_ALLOWED,TEXT_RESOURCE_ID_NONE),
                             "NotAllowed status");
  /* a fixed action */
  result = KeyBindingsPanel_ApplyChord(KeyAction::UiNextControl,KeyChord{KEYBOARD_KEY_CODE_FUNCTION(6),
                                                                          KEYBOARD_STATE_NONE},status,192);
  KeyBindingsPanelTest_Check(result.status == KeyBindingStatus::Fixed &&
                             KeyBindingsPanelTest_StatusIs(status,TEXT_ID_KEYS_PANEL_NOT_ALLOWED,TEXT_RESOURCE_ID_NONE),
                             "Fixed status");
  /* a group row: Alt+Shift (from Alt+Shift+4) */
  result = KeyBindingsPanel_ApplyChord(KeyAction::GroupSelect,KeyChord{KEYBOARD_KEY_CODE_CHAR('4'),
                                                                        KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT |
                                                                        KEYBOARD_STATE_SHIFT},status,192);
  KeyBindingsPanelTest_Check(result.status == KeyBindingStatus::Ok &&
                             KeyBindings_Get(KeyAction::GroupSelect).keys[0].keyCode == KEYBOARD_KEY_CODE_CHAR('1'),
                             "group row keeps 1..8");
  /* a tiny buffer is cut, not overrun */
  uint16_t tiny[4] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF};
  KeyBindings_ResetAllDefaults();
  KeyBindingsPanel_ApplyChord(KeyAction::Pause,KeyChord{KEYBOARD_KEY_CODE_FUNCTION(5),KEYBOARD_STATE_NONE},tiny,3);
  KeyBindingsPanelTest_Check(tiny[2] == 0 && tiny[3] == 0xFFFF,"status cut to capacity");
  KeyBindings_ResetAllDefaults();
  KeyBindingsPanelTest_Check(KeyBindings_AllDefault(),"defaults restored");
}

void Thandor_SelfTestKeyBindingsPanel()
{
  const bool english = KeyBindings_English();

  g_KeyBindingsPanelTestFailures = 0;
  g_KeyBindingsPanelTestChecks = 0;
  KeyBindingsPanelTest_Capture();
  KeyBindingsPanelTest_Apply(false);
  KeyBindingsPanelTest_Apply(true);
  KeyBindings_SetEnglish(english);
  Thandor_Log("keybindingspanel: %u checks, %u failures",g_KeyBindingsPanelTestChecks,g_KeyBindingsPanelTestFailures);
}
