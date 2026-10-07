/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/key_dispatch.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_CORE_KEY_DISPATCH_H
#define THANDOR_UI_CORE_KEY_DISPATCH_H

#include <thandor/core/types.h>
#include <thandor/platform/input/devices.h>

/* Key command tables: records {key code, modifier class, continuation} ending with a key code 0 terminator. The
   original game has three ways to match a record's modifier class against the held modifiers
   (KEYBOARD_STATE_*, left and right keys as separate bits):
     ExactShiftIgnored  in-game hotkeys, end-movie commands: class 0 = neither Ctrl nor Alt held, otherwise
                        exactly the class's Ctrl/Alt combination; Shift is ignored on both sides
     ExactWithShift     in-game key commands, frontend hotkeys: class 0 = no modifier held, otherwise Shift held
                        exactly when the class has Shift and Ctrl/Alt exactly as the class asks
     AnyOfMask          camera and editor keyboard: class 0 = neither Ctrl nor Alt held, otherwise any modifier
                        bit of the class held */
enum class UiKeyModifierRule {
    ExactShiftIgnored,
    ExactWithShift,
    AnyOfMask
};

/* One record of a key command table: key code, modifier class and the action the table's dispatcher runs.
   Action is the table's own enum class over uint32_t (actions 1..N; 0 in the terminator), so a record keeps the
   original's 12 bytes {key code, modifier class, continuation}; the original continuation addresses are listed in
   docs/original_addresses.txt (kind continuation). */
template <typename Action>
struct UiKeyCommandRecord {
    uint32_t commandCode;
    uint32_t modifierClassFlags;
    Action action;
};

/* True when the held modifiers fit the record's modifier class under the rule. */
inline Bool8 UiKeyModifiers_Match(uint32_t classFlags, uint32_t heldFlags, UiKeyModifierRule rule)
{
    if (rule == UiKeyModifierRule::AnyOfMask) {
        if (classFlags == 0) {
            return (heldFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) == 0;
        }
        return (heldFlags & classFlags) != 0;
    }
    if (rule == UiKeyModifierRule::ExactShiftIgnored) {
        classFlags &= ~(uint32_t)KEYBOARD_STATE_SHIFT;
        heldFlags &= ~(uint32_t)KEYBOARD_STATE_SHIFT;
    }
    if (classFlags == 0) {
        return (heldFlags & KEYBOARD_STATE_ANY_MODIFIER) == 0;
    }
    if ((classFlags & KEYBOARD_STATE_SHIFT) != 0) {
        if ((heldFlags & KEYBOARD_STATE_SHIFT) == 0) return false;
    }
    else if ((heldFlags & KEYBOARD_STATE_SHIFT) != 0) {
        return false;
    }
    if ((classFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) == 0) {
        return (heldFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) == 0;
    }
    if ((classFlags & KEYBOARD_STATE_ALT) == 0) {
        return ((heldFlags & KEYBOARD_STATE_CTRL) != 0) && ((heldFlags & KEYBOARD_STATE_ALT) == 0);
    }
    if ((classFlags & KEYBOARD_STATE_CTRL) == 0) {
        return ((heldFlags & KEYBOARD_STATE_CTRL) == 0) && ((heldFlags & KEYBOARD_STATE_ALT) != 0);
    }
    return ((heldFlags & KEYBOARD_STATE_CTRL) != 0) && ((heldFlags & KEYBOARD_STATE_ALT) != 0);
}

/* The first record with this key code whose modifier class matches, or nullptr when the scan reaches the
   terminator (commandCode 0; its other fields are never read). Record is UiKeyCommandRecord<Action> or any record
   with the members commandCode and modifierClassFlags. */
template <typename Record>
inline Record *UiCommandDispatch_Find(Record *records, uint32_t keyCode, uint32_t heldFlags, UiKeyModifierRule rule)
{
    for (Record *record = records; record->commandCode != 0; record++) {
        if ((record->commandCode == keyCode) && UiKeyModifiers_Match(record->modifierClassFlags, heldFlags, rule)) {
            return record;
        }
    }
    return nullptr;
}

#endif /* THANDOR_UI_CORE_KEY_DISPATCH_H */
