/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/keymatch_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=keymatch: the safety net for merging the six key command matchers into
   UiKeyModifiers_Match / UiCommandDispatch_Find (include/thandor/ui/core/key_dispatch.h). Literal copies of the
   four old predicates (taken before the call sites were converted) are compared with the helper for every
   modifier class used in the six tables and every held combination 0..0x3F:
     R1  ExactShiftIgnored  ui/ingame/hotkeys.cpp, ui/frontend/end_movie_commands.cpp
     R2  ExactWithShift     ui/ingame/key_commands.cpp (InGameKeyCommand_ModifiersMatch)
     R2' ExactWithShift     ui/frontend/state.cpp (a Shift-only class falls into its Ctrl branch)
     R3  AnyOfMask          ui/ingame/camera_commands.cpp, ui/ingame/editor_keyboard.cpp
   A class/rule pair that differs logs a mismatch line with "in its tables" or "not in its tables"; only the
   Shift-only class 0x03 differs for R1 and R2', and no R1 or R2' table holds it. UiCommandDispatch_Find is
   checked against the old scan loop on a small table. One hash line over all helper results. */

#include <stdint.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>
#include <thandor/ui/core/key_dispatch.h>

/* R1: ui/ingame/hotkeys.cpp / ui/frontend/end_movie_commands.cpp (loop body, true = record accepted). */
static bool KeymatchOld_R1(uint32_t flags, uint32_t modifierFlags)
{
    if (flags == 0) {
        if ((modifierFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0) return false;
    }
    else if ((flags & KEYBOARD_STATE_ALT) == 0) {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) != 0)) return false;
    }
    else if ((flags & KEYBOARD_STATE_CTRL) == 0) {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) != 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) return false;
    }
    else {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) return false;
    }
    return true;
}

/* R2: InGameKeyCommand_ModifiersMatch (ui/ingame/key_commands.cpp). */
static bool KeymatchOld_R2(uint32_t classFlags, uint32_t modifierFlags)
{
    if (classFlags == 0) {
        return (modifierFlags & KEYBOARD_STATE_ANY_MODIFIER) == 0;
    }
    if ((classFlags & KEYBOARD_STATE_SHIFT) != 0) {
        if ((modifierFlags & KEYBOARD_STATE_SHIFT) == 0) return false;
    }
    else if ((modifierFlags & KEYBOARD_STATE_SHIFT) != 0) {
        return false;
    }
    if ((classFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) == 0) {
        return (modifierFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) == 0;
    }
    if ((classFlags & KEYBOARD_STATE_ALT) == 0) {
        return ((modifierFlags & KEYBOARD_STATE_CTRL) != 0) && ((modifierFlags & KEYBOARD_STATE_ALT) == 0);
    }
    if ((classFlags & KEYBOARD_STATE_CTRL) == 0) {
        return ((modifierFlags & KEYBOARD_STATE_CTRL) == 0) && ((modifierFlags & KEYBOARD_STATE_ALT) != 0);
    }
    return ((modifierFlags & KEYBOARD_STATE_CTRL) != 0) && ((modifierFlags & KEYBOARD_STATE_ALT) != 0);
}

/* R2': FrontendRuntime_DispatchCommandByCodeAndModifierFlags (ui/frontend/state.cpp, loop body). */
static bool KeymatchOld_R2Frontend(uint32_t flags, uint32_t modifierFlags)
{
    if (flags == 0) {
        if ((modifierFlags & KEYBOARD_STATE_ANY_MODIFIER) != 0) return false;
    }
    else {
        if ((flags & KEYBOARD_STATE_SHIFT) != 0) {
            if ((modifierFlags & KEYBOARD_STATE_SHIFT) == 0) return false;
        }
        else if ((modifierFlags & KEYBOARD_STATE_SHIFT) != 0) {
            return false;
        }
        if ((flags & KEYBOARD_STATE_ALT) == 0) {
            if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) != 0)) return false;
        }
        else if ((flags & KEYBOARD_STATE_CTRL) == 0) {
            if (((modifierFlags & KEYBOARD_STATE_CTRL) != 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) return false;
        }
        else {
            if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) return false;
        }
    }
    return true;
}

/* R3: camera_commands.cpp condition / InGameEditorKeyboard_RecordMatches (editor_keyboard.cpp) without the key. */
static bool KeymatchOld_R3(uint32_t requiredModifiers, uint32_t modifierFlags)
{
    return (requiredModifiers == 0) ? ((modifierFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) == 0)
                                    : ((modifierFlags & requiredModifiers) != 0);
}

struct KeymatchRuleCase {
    const char *name;
    bool (*oldMatch)(uint32_t, uint32_t);
    UiKeyModifierRule rule;
    /* the classes the rule's own tables use (bit n = g_KeymatchClasses[n]) */
    uint32_t ownClasses;
};

/* Union of the modifier classes in the six tables (terminators excluded, their class is never read). */
static const uint32_t g_KeymatchClasses[] = {0x00, 0x03, 0x0C, 0x0F, 0x30, 0x33, 0x3C};

static const KeymatchRuleCase g_KeymatchRuleCases[] = {
    /* hotkeys 0/0x30/0x3C, end movie 0x0C/0x30 */
    {"R1", KeymatchOld_R1, UiKeyModifierRule::ExactShiftIgnored, (1u << 0) | (1u << 2) | (1u << 4) | (1u << 6)},
    /* key_commands: 0, 0x03, 0x0C, 0x0F, 0x30, 0x33, 0x3C */
    {"R2", KeymatchOld_R2, UiKeyModifierRule::ExactWithShift, 0x7Fu},
    /* frontend state: 0x0C, 0x30 */
    {"R2'", KeymatchOld_R2Frontend, UiKeyModifierRule::ExactWithShift, (1u << 2) | (1u << 4)},
    /* camera 0/0x0C/0x30, editor 0/0x0C/0x30 */
    {"R3", KeymatchOld_R3, UiKeyModifierRule::AnyOfMask, (1u << 0) | (1u << 2) | (1u << 4)},
};

struct KeymatchTestRecord {
    uint32_t commandCode;
    uint32_t modifierClassFlags;
    uint32_t continuation;
};

static uint32_t KeymatchTest_Hash(uint32_t hash, uint32_t value)
{
    int i;
    for (i = 0; i < 4; i++) {
        hash = (hash ^ ((value >> (i * 8)) & 0xffu)) * 16777619u;
    }
    return hash;
}

void Thandor_SelfTestKeyMatch(void)
{
    /* key 0x71 three times (Alt, Ctrl, none), key 0x70 with Shift-only and Ctrl+Shift, then the terminator */
    static const KeymatchTestRecord table[] = {
        {0x71, 0x30, 1}, {0x71, 0x0C, 2}, {0x71, 0x00, 3}, {0x70, 0x03, 4}, {0x70, 0x0F, 5}, {0, 0x90909090u, 6}};
    uint32_t hash = 2166136261u;
    unsigned mismatchesInTables = 0;
    unsigned mismatchesOther = 0;
    unsigned findMismatches = 0;
    unsigned ruleIndex;
    unsigned classIndex;
    uint32_t held;
    uint32_t key;

    for (ruleIndex = 0; ruleIndex < sizeof g_KeymatchRuleCases / sizeof g_KeymatchRuleCases[0]; ruleIndex++) {
        const KeymatchRuleCase *ruleCase = &g_KeymatchRuleCases[ruleIndex];
        for (classIndex = 0; classIndex < sizeof g_KeymatchClasses / sizeof g_KeymatchClasses[0]; classIndex++) {
            uint32_t classFlags = g_KeymatchClasses[classIndex];
            bool inTables = (ruleCase->ownClasses & (1u << classIndex)) != 0;
            unsigned differing = 0;
            for (held = 0; held <= KEYBOARD_STATE_ANY_MODIFIER; held++) {
                bool oldResult = ruleCase->oldMatch(classFlags, held);
                bool newResult = UiKeyModifiers_Match(classFlags, held, ruleCase->rule) != 0;
                hash = KeymatchTest_Hash(hash, newResult ? 1u : 0u);
                if (oldResult != newResult) {
                    differing++;
                }
            }
            if (differing != 0) {
                Thandor_Log("keymatch mismatch %s class 0x%02X: %u of 64 held combinations (%s)", ruleCase->name,
                            classFlags, differing, inTables ? "in its tables" : "not in its tables");
                if (inTables) {
                    mismatchesInTables++;
                }
                else {
                    mismatchesOther++;
                }
            }
        }
    }
    /* UiCommandDispatch_Find against the old R2 scan (key_commands.cpp) on the small table */
    for (key = 0x6F; key <= 0x72; key++) {
        for (held = 0; held <= KEYBOARD_STATE_ANY_MODIFIER; held++) {
            const KeymatchTestRecord *record = table;
            const KeymatchTestRecord *found;
            uint32_t oldResult;
            uint32_t newResult;
            while ((record->commandCode != 0) &&
                   ((record->commandCode != key) || !KeymatchOld_R2(record->modifierClassFlags, held))) {
                record++;
            }
            oldResult = record->commandCode == 0 ? 0 : record->continuation;
            found = UiCommandDispatch_Find(table, key, held, UiKeyModifierRule::ExactWithShift);
            newResult = found == nullptr ? 0 : found->continuation;
            hash = KeymatchTest_Hash(hash, newResult);
            if (oldResult != newResult) {
                findMismatches++;
            }
        }
    }
    Thandor_Log("keymatch: %u mismatches in table classes, %u outside them, %u find mismatches, hash %08X",
                mismatchesInTables, mismatchesOther, findMismatches, hash);
}
