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

/* Types (split out by tools/dev/split_types.py). */

typedef struct KeyboardInputEvent KeyboardInputEvent, *PKeyboardInputEvent;
typedef struct KeyboardAsciiCaseTransformCallbackTable3 KeyboardAsciiCaseTransformCallbackTable3, *PKeyboardAsciiCaseTransformCallbackTable3;

using KeyboardVirtualKeyCode = uint32_t;

using KeyboardEventRingIndex = uint32_t;

using KeyboardCharacterCode = uint32_t;

struct KeyboardInputEvent {
    UiKeyboardEventCode keyCode;
    UiKeyboardStateMask stateMask;
};

struct KeyboardAsciiCaseTransformCallbackTable3 {
    Ptr32<Bool8 (uint32_t, uint32_t)> compareCaseInsensitiveFlags; /* returns true when upper(right) < upper(left) (Keyboard_CompareAsciiCaseInsensitiveFlags) */
    Ptr32<uint32_t (uint32_t)> toUpper; 
    Ptr32<uint32_t (uint32_t)> toLower; 
};
using KeyboardFlushEventsProc = void ();
using KeyboardReadEventProc = Bool8 (uint32_t *outKeyCode, uint32_t *outStateMask);
using PointerFlushEventsProc = void ();
using PointerSetPositionProc = void (int32_t positionY, int32_t positionX);

#endif /* THANDOR_PLATFORM_INPUT_TYPES_H */
