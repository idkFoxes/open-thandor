/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/key_bindings_panel.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_CORE_KEY_BINDINGS_PANEL_H
#define THANDOR_UI_CORE_KEY_BINDINGS_PANEL_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/controls/text_buttons.h> /* UiPixelRect */
#include <thandor/ui/core/key_bindings.h>

/* The key binding panel (docs/plans/key_rebinding.md, P4): one node subtree in module storage (below 2 GB, so its
   links fit the 32-bit node fields) that a host links into its own tree at run time - the frontend options page
   "Tastenbelegung" and the in-game "Tastatur" overlay of the help window use the same panel. It holds a scroll frame
   with the binding list (section titles; per action the description on the left and the key cell on the right),
   a status line and the buttons "Standard" (all defaults) and "Zurueck" (the host's close callback).

   Editing: a left click on an editable row starts the capture, the key cell shows "Taste druecken... (Esc:
   abbrechen)" and the list takes the keyboard focus, so every key goes to the panel before the world view's key
   commands and the hotkeys (the root's keyboard fallback). The combination is taken when the keys are released:
   the first key with a key-down code plus every modifier held together with it (the following character event is
   swallowed); Esc alone cancels; a key without a key-down code is "nicht erlaubt". Group rows (1..8) also take the
   modifiers alone (press and release Ctrl: Ctrl+1..8). Then KeyBindings_Set: the status line shows the new keys,
   "Taste bereits belegt: <Aktion>" or "Taste nicht erlaubt" (the old binding stays). A right click on a row
   clears its binding. Fixed rows are greyed and not editable. The panel follows every binding change
   (KeyBindings_AddChangeListener while attached).

   The held keys the in-game camera polls (g_KeyboardSpecialKeyDown) do not pass through the keyboard focus: a host
   that shows the panel over a running game should not apply the camera keys while KeyBindingsPanel_Capturing(). */

/* KeyBindingsPanel_Attach flags. */
inline constexpr uint32_t KEY_BINDINGS_PANEL_CENTERED = 0x01;   /* rect is relative to the parent's centre, else to
                                                                   its top-left corner */
inline constexpr uint32_t KEY_BINDINGS_PANEL_BACKGROUND = 0x02; /* tiled window background and frame (a modal overlay);
                                                                   without it the host's background shows */
inline constexpr uint32_t KEY_BINDINGS_PANEL_TITLE = 0x04;      /* the title "Tastenbelegung" in the top line */

/* The smallest useful size; the frontend page's list area (608 x 284 with the buttons) is a good one. */
inline constexpr int32_t KEY_BINDINGS_PANEL_MIN_WIDTH = 520;
inline constexpr int32_t KEY_BINDINGS_PANEL_MIN_HEIGHT = 240;

/* Called (from the UI action queue, at the end of the frame) when "Zurueck" is pressed, or Esc while no capture
   runs; context is the value given to KeyBindingsPanel_Attach. The host detaches the panel (or hides its page). */
using KeyBindingsPanelCloseCallback = void (*)(void *context);

/* Links the panel as the last child of parent (drawn last, hit first), sized by rect (parent-relative pixel
   offsets, see KEY_BINDINGS_PANEL_CENTERED; the edges follow the parent on a later layout), lays it out, gives the
   list the keyboard focus and shows the hint in the status line. An attached panel is detached first. */
void KeyBindingsPanel_Attach(UiNodeBase *parent,UiPixelRect rect,uint32_t flags,KeyBindingsPanelCloseCallback onClose,
                             void *context);

/* Unlinks the panel (a running capture is cancelled; a keyboard focus inside it goes to the parent's first focus
   target, a pointer capture inside it is dropped). Nothing happens when it is not attached. */
void KeyBindingsPanel_Detach();

/* Rebuilds the rows and redraws (also after a language switch); the change listener calls it. */
void KeyBindingsPanel_Refresh();

bool KeyBindingsPanel_IsAttached();

/* The panel's root node (its subtree), for hosts that test the keyboard focus or pointer capture. */
UiNodeBase *KeyBindingsPanel_Root();

/* True while a capture runs (the key cell shows the prompt). */
bool KeyBindingsPanel_Capturing();

/* The capture logic of the panel, without UI (the self-test "keybindingspanel" drives it). The panel feeds every
   keyboard event of the focus walk into KeyCapture_OnKeyEvent and, once per frame tick, the held modifiers
   (g_KeyboardStateMask) and the held state of the captured key into KeyCapture_OnTick. */
enum class KeyCaptureResult : uint8_t {
    Pending,  /* still capturing */
    Commit,   /* every key released: KeyCapture_Chord is the combination */
    Cancel,   /* Esc alone */
    Rejected  /* a character without a key-down code (e.g. a key of the layout's punctuation) */
};
struct KeyCapture {
    bool active;
    uint8_t rangeLength;               /* > 1: a group row; the modifiers alone also make a chord (key '1') */
    uint32_t keyCode;                  /* the captured key-down code, 0 until the first one */
    UiKeyboardStateMask modifiers;     /* the modifiers held together with the key (peak) */
    UiKeyboardStateMask modifierPeak;  /* before a key: the modifiers held since the last time none was held */
};

void KeyCapture_Begin(KeyCapture *capture,uint8_t rangeLength);

/* One keyboard event (key-down code or character code with the modifier state of its moment). Only the first key
   counts; every later event (its character event, further keys) is swallowed (Pending). */
KeyCaptureResult KeyCapture_OnKeyEvent(KeyCapture *capture,uint32_t keyCode,UiKeyboardStateMask stateMask);

/* One frame tick: heldModifiers the modifier state now, keyHeld whether the captured key is still down. Commit when
   the key and every modifier are released (group rows: also when modifiers alone were pressed and released). */
KeyCaptureResult KeyCapture_OnTick(KeyCapture *capture,UiKeyboardStateMask heldModifiers,bool keyHeld);

/* The captured combination (modifiers widened to both sides). */
KeyChord KeyCapture_Chord(const KeyCapture *capture);

/* True for the codes Keyboard_OnKeyDown queues (the 0x10000 / 0x20000 / 0x30000 families, Space and the numpad
   operators * / + -); any other code is a character event. */
bool KeyCapture_IsKeyDownCode(uint32_t keyCode);

/* Applies a captured chord to action through KeyBindings_Set and writes the status line text for the result
   (UTF-16 with terminator, cut to capacity): "<Aktion>: <Tasten>", "Taste bereits belegt: <Aktion>" or "Taste
   nicht erlaubt". Returns the result of KeyBindings_Set. The panel uses it; the self-test checks the texts. */
KeyBindingResult KeyBindingsPanel_ApplyChord(KeyAction action,KeyChord chord,uint16_t *status,uint32_t capacity);

#endif /* THANDOR_UI_CORE_KEY_BINDINGS_PANEL_H */
