/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/key_bindings.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_CORE_KEY_BINDINGS_H
#define THANDOR_UI_CORE_KEY_BINDINGS_H

#include <thandor/core/types.h>
#include <thandor/assets/text/types.h>
#include <thandor/ui/controls/types.h>

/* The key bindings (docs/plans/key_rebinding.md): one entry per action with its stable name, context, description
   text, current keys and defaults. The defaults are the keys of the game's own dispatch tables and key handlers
   (the self-test "keybindings" checks every default against them); each entry names its dispatcher (source) and the
   action number of the source's table. Changes are local settings: they never reach the simulation or the network
   (lockstep sends commands, not keys). The map editor's keys are not listed. */

enum class KeyBindingSection : uint8_t {
    InGameGeneral,   /* TEXT_ID_KEYS_SECTION_GENERAL "Allgemein im Spiel" */
    UnitsAndGroups,  /* TEXT_ID_KEYS_SECTION_UNITS "Einheiten und Gruppen" */
    Camera,          /* TEXT_ID_KEYS_SECTION_CAMERA "Kamera" */
    MenusAndMovies   /* TEXT_ID_KEYS_SECTION_MENUS "Menues und Filme" */
};
inline constexpr int KEY_BINDING_SECTION_COUNT = 4;

/* Where a key is handled; a conflict is only possible between actions of one context (Menus also against Ui). */
enum class KeyBindingContext : uint8_t {
    InGame,    /* the world view's key commands, the in-game hotkeys and the held camera keys */
    Menus,     /* the frontend hotkeys */
    EndMovie,  /* the end-movie commands (fixed) */
    Ui         /* the general UI keys and the intro movies (fixed) */
};

/* Where an entry's keys are handled; sourceAction is the number the source's table gives (its action enum), 0 for
   sources without a table. */
enum class KeyBindingSource : uint8_t {
    InGameHotkeys,      /* ui/ingame/hotkeys.cpp, InGameHotkeys_FindBinding (InGameHotkeyAction) */
    InGameKeyCommands,  /* ui/ingame/key_commands.cpp, InGameKeyCommands_FindBinding (InGameKeyCommandAction);
                           the world view gets a key before the in-game hotkeys */
    CameraHeldKeys,     /* ui/ingame/root_frame.cpp InGameUiRoot_ApplyCameraKeys: polled while held
                           (g_KeyboardSpecialKeyDown; KeyBindings_HeldDown with Keyboard_KeyHeld for any key);
                           the numpad digits map to the same keys */
    FrontendHotkeys,    /* ui/frontend/state.cpp, FrontendHotkeys_FindBinding (FrontendHotkeyAction) */
    EndMovieCommands,   /* ui/frontend/end_movie_commands.cpp, EndMovieCommands_FindBinding (EndMovieCommandAction) */
    IntroMovies,        /* platform/bootstrap/runtime.cpp IntroMovie_PollSkipRequest */
    UiControls          /* ui/controls: focus (Tab), buttons (Space, Enter, Esc), lists, sliders */
};

/* Every action, in display order (sorted by section), then the hidden fixed actions. The values are array indices,
   not stored anywhere (the ini uses KeyBindings_Name). */
enum class KeyAction : uint8_t {
    /* Allgemein im Spiel */
    GameMenu,              /* game_menu: Esc (fixed) */
    Objectives,            /* objectives: F1 */
    Save,                  /* save: F2 */
    QuitMenu,              /* quit_menu: Alt+F4 (fixed) */
    LeaveGame,             /* leave_game: Alt+Q */
    Pause,                 /* pause: P */
    Faster,                /* faster: G */
    Slower,                /* slower: Alt+G */
    SidePanel,             /* side_panel: Tab */
    Chat,                  /* chat: Enter */
    MessageWindow,         /* message_window: C */
    ScoreOverview,         /* score_overview: O */
    InfoLine,              /* info_line: Ctrl+I */
    Screenshot,            /* screenshot: Alt+P */
    NotificationGo,        /* notification_go: Space */
    NotificationBack,      /* notification_back: Backspace */
    ObjectiveToggle,       /* objective_toggle: Z (the objective overlay, ObjectiveOverlay_ToggleSetting) */
    /* Einheiten und Gruppen (the group actions: range 1..8, only the modifiers are editable) */
    GroupSelect,           /* group_select: 1..8 */
    GroupSelectAdd,        /* group_select_add: Shift+1..8 */
    GroupStore,            /* group_store: Ctrl+1..8, Alt+1..8 */
    GroupStoreAdd,         /* group_store_add: Ctrl+Shift+1..8, Alt+Shift+1..8 */
    Stop,                  /* stop: S, Alt+Shift+S */
    StopMovement,          /* stop_movement: Shift+S */
    CancelAttack,          /* cancel_attack: Alt+S */
    SelfDestruct,          /* self_destruct: Alt+D */
    SelectAirports,        /* select_airports: A */
    UpgradePage,           /* upgrade_page: F */
    /* Kamera */
    CameraLeft,            /* camera_left: Left */
    CameraRight,           /* camera_right: Right */
    CameraUp,              /* camera_up: Up */
    CameraDown,            /* camera_down: Down */
    CameraTiltMinus,       /* camera_tilt_minus: Page Up (pitch - step) */
    CameraTiltPlus,        /* camera_tilt_plus: Page Down (pitch + step) */
    CameraRotateMinus,     /* camera_rotate_minus: Insert (heading - step) */
    CameraRotatePlus,      /* camera_rotate_plus: Delete (heading + step) */
    CameraZoomIn,          /* camera_zoom_in: Home (distance - step) */
    CameraZoomOut,         /* camera_zoom_out: End (distance + step) */
    CameraSelection,       /* camera_selection: Num 5 */
    CameraHq,              /* camera_hq: B */
    CameraNotification,    /* camera_notification: Alt+Space */
    CameraFree,            /* camera_free: Alt+C */
    /* Menues und Filme */
    MenuLeave,             /* menu_leave: Alt+Q, permanent Alt+F4 */
    UiNextControl,         /* ui_next_control: Tab (fixed) */
    UiPressButton,         /* ui_press_button: Space (fixed) */
    UiConfirmCancel,       /* ui_confirm_cancel: Enter, Esc (fixed) */
    UiList,                /* ui_list: arrows, Page Up/Down, Home, End (fixed) */
    UiSlider,              /* ui_slider: arrows, Ctrl+arrows (fixed) */
    IntroSkipAll,          /* intro_skip_all: Esc (fixed) */
    IntroSkip,             /* intro_skip: any key (fixed) */
    EndMovieSkip,          /* end_movie_skip: Alt+Q (fixed, character code) */
    EndMovieScreenshot,    /* end_movie_screenshot: Ctrl+P (fixed, character code) */
    /* hidden and fixed (not shown; a chord on them is "not allowed") */
    CheatXenite,           /* cheat_xenite: Ctrl+Alt+X */
    CheatEnergy,           /* cheat_energy: Ctrl+Alt+E */
    CheatFastBuild,        /* cheat_fast_build: Ctrl+Alt+Z */
    CheatOccupancy,        /* cheat_occupancy: Ctrl+Alt+V */
    FactionSetupReady,     /* faction_setup_ready: Ctrl+F1 (an eighth colour in the faction cycle) */
    Count
};
inline constexpr int KEY_ACTION_COUNT = static_cast<int>(KeyAction::Count);

/* Pseudo key codes of fixed entries that stand for several keys (outside every KEYBOARD_KEY_CODE_* family). */
inline constexpr uint32_t KEY_BINDING_CODE_ARROWS = 0x7F000001;  /* the four arrow keys ("Pfeiltasten") */
inline constexpr uint32_t KEY_BINDING_CODE_ANY_KEY = 0x7F000002; /* any key ("Beliebige Taste") */

/* One key combination: a KEYBOARD_KEY_CODE_* key-down code (or a KEY_BINDING_CODE_* pseudo code, or a character
   code 'a'..'z' of the end-movie table) and the modifiers it needs (KEYBOARD_STATE_SHIFT / _CTRL / _ALT, always
   both sides). keyCode 0: no key. */
struct KeyChord {
    uint32_t keyCode;
    UiKeyboardStateMask modifiers;
};

inline constexpr int KEY_BINDING_MAX_KEYS = 5;

inline constexpr uint8_t KEY_BINDING_FLAG_FIXED = 0x01;  /* not editable (shown greyed) */
inline constexpr uint8_t KEY_BINDING_FLAG_HIDDEN = 0x02; /* not shown (cheats, Ctrl+F1); always fixed */

/* One action. keys holds 0..KEY_BINDING_MAX_KEYS chords (keyCode 0 ends the list; none = unbound), the current
   binding; the permanent chord (if any) is always among them, as the last one. rangeLength > 1: every chord stands
   for keyCode .. keyCode + rangeLength - 1 (the group keys 1..8; keyCode is always KEYBOARD_KEY_CODE_CHAR('1')). */
struct KeyBinding {
    const char *name; /* stable ini name, e.g. "pause" */
    KeyBindingContext context;
    KeyBindingSection section;
    KeyBindingSource source;
    uint8_t rangeLength;
    uint8_t flags;    /* KEY_BINDING_FLAG_* */
    uint32_t sourceAction;
    TextResourceId textId; /* TEXT_ID_KEYS_*: the action description (one per action) */
    KeyChord keys[KEY_BINDING_MAX_KEYS];
    KeyChord defaults[KEY_BINDING_MAX_KEYS];
    KeyChord permanent; /* keyCode 0: none; else kept by Set / Clear (Alt+F4 of menu_leave) */
};

/* The number of actions (KEY_ACTION_COUNT, hidden ones included) and one action. */
int KeyBindings_Count();
const KeyBinding &KeyBindings_Get(KeyAction action);
inline bool KeyBinding_IsFixed(const KeyBinding &binding) { return (binding.flags & KEY_BINDING_FLAG_FIXED) != 0; }
inline bool KeyBinding_IsHidden(const KeyBinding &binding) { return (binding.flags & KEY_BINDING_FLAG_HIDDEN) != 0; }

/* Result of a change or a conflict query. */
enum class KeyBindingStatus : uint8_t {
    Ok,          /* applied (or: no conflict) */
    Conflict,    /* the chord is used by conflictAction ("Taste bereits belegt: <Aktion>") */
    NotAllowed,  /* reserved key, key without a key-down code, or a hidden action's key ("Taste nicht erlaubt") */
    Fixed        /* the action itself is fixed */
};
struct KeyBindingResult {
    KeyBindingStatus status;
    KeyAction conflictAction; /* KeyAction::Count unless status is Conflict */
};

/* True for a key-down code that may be bound: letters, digits, F1..F12, Space, Enter, Tab, Backspace, Print,
   Pause, the navigation keys, Num 5 and the numpad operators * / + -. Esc is not bindable. */
bool KeyBindings_IsBindableKey(uint32_t keyCode);

/* The first action of the context (Menus: also the Ui actions) whose keys conflict with chord (expanded over
   1..rangeLength when rangeLength > 1), ignoring the action ignore; chord is taken as ignore's chord (its
   dispatcher's rule), or as a bare press when ignore is KeyAction::Count. Simulated dispatch of each chord's own
   press (its key with exactly its modifiers), under the dispatchers' modifier rules (in-game hotkeys and end movie:
   Ctrl/Alt exact, Shift ignored unless the chord has Shift; key commands and frontend hotkeys: exact with Shift;
   camera and UI keys: held, a chord without modifiers matches any) and order (the world view's key commands before
   the in-game hotkeys; held keys always): a conflict is a press that reaches both, or a press that no longer reaches
   its own chord. NotAllowed for Esc, Alt+F4, Alt+Tab, keys that are not bindable and chords of hidden actions.
   Changes nothing. */
KeyBindingResult KeyBindings_FindConflict(KeyBindingContext context,KeyChord chord,uint8_t rangeLength = 1,
                                          KeyAction ignore = KeyAction::Count);

/* KeyBindings_Set without applying it (Fixed for a fixed action). */
KeyBindingResult KeyBindings_Check(KeyAction action,KeyChord chord);

/* Binds action to chord alone (all other chords go; the permanent chord stays). Range actions take only the
   modifiers (the key stays KEYBOARD_KEY_CODE_CHAR('1')). The binding is unchanged unless the status is Ok.
   Modifier bits of one side are widened to both sides. Notifies the listeners on a change. */
KeyBindingResult KeyBindings_Set(KeyAction action,KeyChord chord);

/* Unbinds action (the permanent chord stays). False for a fixed action. Notifies on a change. */
bool KeyBindings_Clear(KeyAction action);

/* Restores the defaults of one action / of every action. Notifies on a change. */
void KeyBindings_ResetDefaults(KeyAction action);
void KeyBindings_ResetAllDefaults();

/* True when the action's / every action's keys equal the defaults. */
bool KeyBindings_IsDefault(KeyAction action);
bool KeyBindings_AllDefault();

/* For the ini loader (P3): sets the chords of an action without conflict check and without notification (the
   permanent chord is appended when missing; duplicates are dropped). False (nothing changed) for a fixed action,
   too many chords or a chord whose key is not bindable / not allowed. */
bool KeyBindings_SetRaw(KeyAction action,const KeyChord *chords,int count);

/* After KeyBindings_SetRaw: walks the actions in order and unbinds every chord of an action that conflicts with an
   earlier action (or is not allowed); report (may be nullptr) gets each unbound action and the earlier one. Returns
   the number of unbound chords. Does not notify. */
int KeyBindings_ResolveConflicts(void (*report)(KeyAction unbound,KeyAction kept));

/* Change notification: every applied change bumps the counter and calls the listeners (P2: rewrite the dispatch
   tables, P3: save). KeyBindings_NotifyChanged does both by hand (after SetRaw / ResolveConflicts). At most
   KEY_BINDINGS_MAX_LISTENERS listeners; adding one twice keeps one. */
inline constexpr int KEY_BINDINGS_MAX_LISTENERS = 4;
using KeyBindingsChangeListener = void (*)();
bool KeyBindings_AddChangeListener(KeyBindingsChangeListener listener);
void KeyBindings_RemoveChangeListener(KeyBindingsChangeListener listener);
uint32_t KeyBindings_ChangeCount();
void KeyBindings_NotifyChanged();

/* True while a chord of a held-key action (the camera) is held: the key is down (Keyboard_KeyHeld, every key-down
   code, released on focus loss) and the chord's modifiers are held (a chord without modifiers ignores them). While a
   text edit has the keyboard focus the keys that type text (letters, digits, Space, numpad operators) do not count;
   the special keys do (the original moves the camera with the arrow keys while the chat edit has the focus). */
bool KeyBindings_HeldDown(KeyAction action);

/* ini tokens (English, canonical): "P", "Alt+G", "Ctrl+Shift+F1", "Ctrl+1..8" (range actions), "Left", "PageUp",
   "Num5", "Num*" (numpad operators), "Space", "Esc"; modifiers in the order Ctrl, Alt, Shift. The name of an action
   is its stable ini name; KeyBindings_FindByName ignores ASCII case. */
const char *KeyBindings_Name(KeyAction action);
bool KeyBindings_FindByName(const char *name,KeyAction *outAction);

/* Writes one chord's token with terminator (cut to capacity); returns its length, 0 for a key without a token. */
uint32_t KeyChord_FormatToken(KeyChord chord,uint8_t rangeLength,char *out,uint32_t capacity);

/* Parses one token (surrounding blanks and ASCII case ignored). rangeLength > 1 wants the digit 1 (with or without
   "..8"); rangeLength 1 rejects "..". False for an invalid token. */
bool KeyChord_ParseToken(const char *text,uint8_t rangeLength,KeyChord *outChord);

/* The action's keys as an ini value: the tokens joined with ", " (the permanent chord left out), "none" when
   unbound. Returns the length. */
uint32_t KeyBindings_FormatTokens(KeyAction action,char *out,uint32_t capacity);

/* Parses an ini value ("S, Alt+Shift+S", "none") for an action with this rangeLength into chords; returns their
   number (0 for "none"), -1 for an invalid value or more than capacity chords. */
int KeyBindings_ParseTokens(const char *text,uint8_t rangeLength,KeyChord *outChords,int capacity);

/* TEXT_ID_KEYS_SECTION_* of a section. */
TextResourceId KeyBindings_SectionTitle(KeyBindingSection section);

/* Writes the name of one key with its modifiers ("Strg+Umschalt+1", "Alt+F4", "Bild auf", "Num 5"; English:
   "Ctrl+Shift+1", ...) as UTF-16 with terminator into out (capacity code units, terminator included; cut to fit).
   Returns the number of code units written without the terminator, 0 for a key without a name (out then holds an
   empty string when capacity > 0). */
uint32_t KeyName_Format(uint32_t keyCode,UiKeyboardStateMask modifiers,uint16_t *out,uint32_t capacity);

/* Writes all keys of an action ("Strg+1..8 / Alt+1..8"), like KeyName_Format; 0 when unbound. */
uint32_t KeyBinding_FormatKeys(const KeyBinding &binding,uint16_t *out,uint32_t capacity);

/* Language of the key names and of the TEXT_ID_KEYS_* texts: false (the default) German, true English. */
void KeyBindings_SetEnglish(bool english);
bool KeyBindings_English();

#endif /* THANDOR_UI_CORE_KEY_BINDINGS_H */
