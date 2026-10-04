/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/input/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_INPUT_TYPES_H
#define THANDOR_PLATFORM_INPUT_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct KeyboardInputEvent KeyboardInputEvent, *PKeyboardInputEvent;
typedef struct KeyboardAsciiCaseTransformCallbackTable3 KeyboardAsciiCaseTransformCallbackTable3, *PKeyboardAsciiCaseTransformCallbackTable3;

typedef uint32_t KeyboardVirtualKeyCode;

typedef uint32_t KeyboardEventRingIndex;

typedef uint32_t KeyboardCharacterCode;

struct KeyboardInputEvent {
    UiKeyboardEventCode keyCode;
    UiKeyboardStateMask stateMask;
};

struct KeyboardAsciiCaseTransformCallbackTable3 {
    Ptr32<Bool8 (uint32_t, uint32_t)> compareCaseInsensitiveFlags; /* returns true when upper(right) < upper(left) (Keyboard_CompareAsciiCaseInsensitiveFlags) */
    Ptr32<uint32_t (uint32_t)> toUpper; 
    Ptr32<uint32_t (uint32_t)> toLower; 
};
typedef void KeyboardFlushEventsProc();
typedef Bool8 KeyboardReadEventProc(uint32_t *outKeyCode, uint32_t *outStateMask);
typedef void PointerFlushEventsProc();
typedef void PointerSetPositionProc(int32_t positionY, int32_t positionX);

#endif /* THANDOR_PLATFORM_INPUT_TYPES_H */
