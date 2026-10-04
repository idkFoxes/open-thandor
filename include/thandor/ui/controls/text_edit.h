/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/text_edit.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_TEXT_EDIT_H
#define THANDOR_UI_CONTROLS_TEXT_EDIT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/text_edit. */

/* editStateFlags bits of UiPathTextEditControl beyond UiTextEditStateFlags; bits 1-2 are passed on to
   g_FileSystemValidateDos83Path. */
#define UI_PATH_TEXT_ALLOW_WILDCARDS 0x02 /* accept '*' and '?' */
#define UI_PATH_TEXT_NAME_ONLY 0x04 /* refuse ':' and '\' */

/* Code units of the fixed text buffers (the terminator included). */
#define UI_NUMERIC_TEXT_BUFFER_UNITS 16
#define UI_PATH_TEXT_BUFFER_UNITS 256
/* Byte capacity of each expanded-text scratch buffer of UiPointerList_CompareExpandedText. */
#define UI_POINTER_LIST_COMPARE_SCRATCH_BYTES 0x400

/* The top byte of editStateFlags (text edits) and listStateFlags (text lists) counts frames down. */
#define UI_STATE_FRAME_COUNTER_UNIT 0x1000000
#define UI_STATE_FLAGS_MASK 0xffffff /* the flag bits below the counter */

/* Functions are grouped by semantic ownership. */

Bool8 UiNumericTextEditControl_HandleKeyboardAndCommit(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiNumericTextControl *control);

Bool8 UiPathTextEditControl_HandleKeyboardAndValidate(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiPathTextEditControl *control);

Bool8 UiRequiredTextEditControl_HandleKeyboardAndValidate
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiRequiredTextEditControl *control);

void UiNumericTextEditControl_RelocateAndRebuildText
          (UiSerializedRelocationDelta relocationDelta,UiNumericTextControl *control);

void UiTextEditControl_DrawTextSelectionAndCaret
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextEditControl *control);

void UiTextEditControl_BeginSelectionAtPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control);

void UiTextEditControl_UpdateSelectionFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control);

void UiPathTextEditControl_RelocateAndValidateDos83
          (UiSerializedRelocationDelta relocationDelta,UiPathTextEditControl *control);

void UiRequiredTextEditControl_RelocateAndValidateNonEmpty
          (UiSerializedRelocationDelta relocationDelta,UiRequiredTextEditControl *control);

void UiTextEditControl_EndSelection
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiTextEditControl *control);

void UiTextEditControl_SuppressIfActionId(UiActionId actionId,UiTextEditControl *control);

void UiTextEditControl_UnsuppressIfActionId(UiActionId actionId,UiTextEditControl *control);

void UiTextEditControl_TickCaretBlink(UiTextEditControl *control);

void UiNumericTextControl_RebuildTextFromValue(UiNumericTextControl *control);

void UiNumericTextControl_ParseAndCommitValue(UiNumericTextControl *control);

void UiNumericTextControl_UpdateRangeValidity(UiNumericTextControl *control);

UiPixelCoordinate UiTextEditControl_MeasurePrefixWidth(UiTextCodeUnitCount prefixLength,UiTextEditControl *control);

UiTextCodeUnitCount UiTextEditControl_FindCursorIndexAtX(UiPixelCoordinate pointerX,UiTextEditControl *control);

void UiPathTextControl_UpdateDos83Validity(UiPathTextEditControl *control);

void UiTextControl_UpdateNonEmptyValidity(UiTextEditControl *control);

void UiTextEditControl_RecomputeLayoutAndClampScroll(UiTextEditControl *control);

extern UiNodeVtable g_UiNumericTextEditControlVtable;
extern UiNodeVtable g_UiPathTextEditControlVtable;
extern UiNodeVtable g_UiRequiredTextEditControlVtable;

extern AudioMixerGainQ15 g_UiSoundGainQ15;

#endif /* THANDOR_UI_CONTROLS_TEXT_EDIT_H */
