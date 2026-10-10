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

/* The key binding list of the options menu ("Tastenbelegung"): one row per action with its key(s) and the text id of
   its description, grouped in four sections. Read-only for now: the keys are what the game's own dispatch tables and
   key handlers use (the self-test "keybindings" checks every row against them); the rows name their dispatcher
   (source) and its action number so that rebinding can be added later. Cheats and the map editor are not listed. */

enum class KeyBindingSection : uint8_t {
    InGameGeneral,   /* TEXT_ID_KEYS_SECTION_GENERAL "Allgemein im Spiel" */
    UnitsAndGroups,  /* TEXT_ID_KEYS_SECTION_UNITS "Einheiten und Gruppen" */
    Camera,          /* TEXT_ID_KEYS_SECTION_CAMERA "Kamera" */
    MenusAndMovies   /* TEXT_ID_KEYS_SECTION_MENUS "Menues und Filme" */
};
inline constexpr int KEY_BINDING_SECTION_COUNT = 4;

/* Where a row's keys are handled; action is the number the source's table gives (its action enum), 0 for sources
   without a table. */
enum class KeyBindingSource : uint8_t {
    InGameHotkeys,      /* ui/ingame/hotkeys.cpp, InGameHotkeys_FindBinding (InGameHotkeyAction) */
    InGameKeyCommands,  /* ui/ingame/key_commands.cpp, InGameKeyCommands_FindBinding (InGameKeyCommandAction);
                           the world view gets a key before the in-game hotkeys */
    CameraHeldKeys,     /* ui/ingame/root_frame.cpp InGameUiRoot_ApplyCameraKeys: polled while held
                           (g_KeyboardSpecialKeyDown); the numpad digits map to the same keys */
    FrontendHotkeys,    /* ui/frontend/state.cpp, FrontendHotkeys_FindBinding (FrontendHotkeyAction) */
    EndMovieCommands,   /* ui/frontend/end_movie_commands.cpp, EndMovieCommands_FindBinding (EndMovieCommandAction) */
    IntroMovies,        /* platform/bootstrap/runtime.cpp IntroMovie_PollSkipRequest */
    UiControls          /* ui/controls: focus (Tab), buttons (Space, Enter, Esc), lists, sliders */
};

/* Pseudo key codes of rows that stand for several keys (outside every KEYBOARD_KEY_CODE_* family). */
inline constexpr uint32_t KEY_BINDING_CODE_ARROWS = 0x7F000001;  /* the four arrow keys ("Pfeiltasten") */
inline constexpr uint32_t KEY_BINDING_CODE_ANY_KEY = 0x7F000002; /* any key ("Beliebige Taste") */

/* One key of a row: a KEYBOARD_KEY_CODE_* code (or a KEY_BINDING_CODE_* pseudo code, or a character code 'a'..'z'
   of the end-movie table) and the modifiers it needs (KEYBOARD_STATE_SHIFT / _CTRL / _ALT, both sides). */
struct KeyBindingKey {
    uint32_t keyCode;
    UiKeyboardStateMask modifiers;
};

inline constexpr int KEY_BINDING_MAX_KEYS = 5;

/* One row. keys holds 1..KEY_BINDING_MAX_KEYS alternatives (keyCode 0 ends the list); shown joined with " / ".
   rangeLength > 1: every key stands for keyCode .. keyCode + rangeLength - 1 (the group keys 1..8). */
struct KeyBinding {
    KeyBindingSection section;
    KeyBindingSource source;
    uint8_t rangeLength;
    uint32_t action;
    KeyBindingKey keys[KEY_BINDING_MAX_KEYS];
    TextResourceId textId; /* TEXT_ID_KEYS_*: the action description */
};

/* The rows in display order (sorted by section); *outCount receives their number. */
const KeyBinding *KeyBindings_Table(uint32_t *outCount);

/* TEXT_ID_KEYS_SECTION_* of a section. */
TextResourceId KeyBindings_SectionTitle(KeyBindingSection section);

/* Writes the name of one key with its modifiers ("Strg+Umschalt+1", "Alt+F4", "Bild auf", "Num 5"; English:
   "Ctrl+Shift+1", ...) as UTF-16 with terminator into out (capacity code units, terminator included; cut to fit).
   Returns the number of code units written without the terminator, 0 for a key without a name (out then holds an
   empty string when capacity > 0). */
uint32_t KeyName_Format(uint32_t keyCode,UiKeyboardStateMask modifiers,uint16_t *out,uint32_t capacity);

/* Writes all keys of a row ("Strg+1..8 / Alt+1..8"), like KeyName_Format. */
uint32_t KeyBinding_FormatKeys(const KeyBinding &binding,uint16_t *out,uint32_t capacity);

/* Language of the key names and of the TEXT_ID_KEYS_* texts: false (the default) German, true English. */
void KeyBindings_SetEnglish(bool english);
bool KeyBindings_English();

#endif /* THANDOR_UI_CORE_KEY_BINDINGS_H */
