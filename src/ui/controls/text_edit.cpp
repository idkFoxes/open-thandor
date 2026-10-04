/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/text_edit.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/text_edit.h>
#include <thandor/thandor.h>

/* Module data. */

AudioMixerGainQ15 g_UiSoundGainQ15 = 32768;

static const uint32_t g_UiTextEditActiveTextStyle = 0;

static const uint32_t g_UiTextEditInactiveTextStyle = 0;

static const uint32_t g_UiTextEditDisabledTextStyle = 0;

/* UiFrameDelayFrames, 8: frames per caret blink phase of a focused text edit, reloaded into the counter byte of editStateFlags */
static const UiFrameDelayFrames g_UiTextEditCaretBlinkPhaseStep = 8;

/* Implementation ownership: ui/controls/text_edit. */

/* Removes the selected range of the numeric text edit: the code units from selectionEnd up to the buffer
   end move down to selectionStart and the freed tail is zero-filled. Cursor and selection collapse at
   selectionStart. */
static void UiNumericTextEdit_RemoveSelectedRange(UiNumericTextControl *control)

{
  UiTextCodeUnitIndex selectionEnd;
  int removedCount;
  int movedCount;
  uint16_t *sourceCursor;
  uint16_t *destinationCursor;

  selectionEnd = control->selectionEnd;
  removedCount = selectionEnd - control->selectionStart;
  sourceCursor = control->textBuffer + selectionEnd;
  destinationCursor = control->textBuffer + control->selectionStart;
  for (movedCount = UI_NUMERIC_TEXT_BUFFER_UNITS - selectionEnd; movedCount != 0; movedCount--) {
    *destinationCursor = *sourceCursor;
    sourceCursor++;
    destinationCursor++;
  }
  for (; removedCount != 0; removedCount--) {
    *destinationCursor = 0;
    destinationCursor++;
  }
  control->cursorIndex = control->selectionStart;
  control->selectionEnd = control->selectionStart;
}

/* Keyboard handler of the numeric text edit (keyboardEvent slot of g_UiNumericTextEditControlVtable): inserts
   digits, '-' (signed values) and A-F (hexadecimal values), edits and moves the cursor and Shift selection,
   and after every handled key parses and commits the value. Enter queues the action when the control acts on
   Enter only; everything else goes to UiNode_DefaultKeyboardEventMoveFocusNext. Returns false: consumed.
*/
Bool8 UiNumericTextEditControl_HandleKeyboardAndCommit(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiNumericTextControl *control)

{
  UiTextCodeUnitIndex *selectionBoundary;
  uint16_t displacedCodeUnit;
  Bool8 characterAccepted;
  Bool8 recomputeLayout;
  Bool8 normalizeSelection;
  UiTextCodeUnitIndex codeUnitIndex;
  int shiftCount;
  uint32_t insertIndex;
  uint16_t *sourceCursor;
  uint16_t *destinationCursor;
  Bool8 delegatedResult;

  if ((((control->editStateFlags & UI_NUMERIC_TEXT_READ_ONLY) != 0) ||
      (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0)) || ((keyboardStateMask & KEYBOARD_STATE_ALT) != 0)) {
    delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return delegatedResult;
  }
  /* Handled keys end in the shared tail: optional layout recompute, then parse/commit, invalidate
     and the interaction sound. */
  recomputeLayout = true;
  normalizeSelection = false;
  if ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0) {
    /* Character: digits always, '-' for signed values, A-F/a-f for hexadecimal ones. */
    if (keyCode == '-') {
      characterAccepted = (control->editStateFlags & UI_NUMERIC_TEXT_SIGNED_VALUE) != 0;
    }
    else if (keyCode < '0') {
      characterAccepted = false;
    }
    else if (keyCode <= '9') {
      characterAccepted = true;
    }
    else if ((keyCode < 'A') || (('F' < keyCode && ((keyCode < 'a' || ('f' < keyCode)))))) {
      characterAccepted = false;
    }
    else {
      characterAccepted = (control->editStateFlags & UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT) != 0;
    }
    if (!characterAccepted) {
      delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
      return delegatedResult;
    }
    insertIndex = control->cursorIndex;
    if ((insertIndex != control->selectionStart) || (insertIndex != control->selectionEnd)) {
      UiNumericTextEdit_RemoveSelectedRange(control);
      insertIndex = control->selectionStart;
    }
    /* At most 14 code units: the insertion shifts the text up to index 13 only. */
    if (insertIndex < UI_NUMERIC_TEXT_BUFFER_UNITS - 2) {
      control->cursorIndex++;
      control->selectionStart++;
      control->selectionEnd++;
      if ((control->editStateFlags & UI_NUMERIC_TEXT_OVERWRITE_MODE) == 0) {
        do {
          LOCK();
          displacedCodeUnit = control->textBuffer[insertIndex];
          control->textBuffer[insertIndex] = (uint16_t)keyCode;
          keyCode = (UiKeyboardEventCode)displacedCodeUnit;
          UNLOCK();
          insertIndex++;
        } while (insertIndex < UI_NUMERIC_TEXT_BUFFER_UNITS - 2);
      }
      else {
        control->textBuffer[insertIndex] = (uint16_t)keyCode;
      }
    }
  }
  else {
    if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      /* Ctrl+Left/Right act as Home/End (with or without Shift); other Ctrl keys are not handled. */
      if (keyCode == KEYBOARD_KEY_CODE_LEFT) {
        keyCode = KEYBOARD_KEY_CODE_HOME;
      }
      else if (keyCode == KEYBOARD_KEY_CODE_RIGHT) {
        keyCode = KEYBOARD_KEY_CODE_END;
      }
      else {
        delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
        return delegatedResult;
      }
    }
    if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) == 0) {
      switch (keyCode) {
      case KEYBOARD_KEY_CODE_BACKSPACE:
      case KEYBOARD_KEY_CODE_DELETE:
        codeUnitIndex = control->cursorIndex;
        if ((codeUnitIndex != control->selectionStart) || (codeUnitIndex != control->selectionEnd)) {
          /* Backspace/Delete with a selection: remove the selected range, zero-fill the tail. */
          UiNumericTextEdit_RemoveSelectedRange(control);
        }
        else if (keyCode == KEYBOARD_KEY_CODE_BACKSPACE) {
          /* Backspace: remove the code unit before the cursor. */
          if (control->cursorIndex == 0) {
            recomputeLayout = false;
            break;
          }
          sourceCursor = control->textBuffer + codeUnitIndex;
          destinationCursor = control->textBuffer + (codeUnitIndex - 1);
          for (shiftCount = UI_NUMERIC_TEXT_BUFFER_UNITS - codeUnitIndex; shiftCount != 0; shiftCount--) {
            *destinationCursor = *sourceCursor;
            sourceCursor++;
            destinationCursor++;
          }
          control->cursorIndex--;
          control->selectionStart = control->cursorIndex;
          control->selectionEnd = control->cursorIndex;
        }
        else {
          /* Delete: remove the code unit at the cursor. */
          if (control->textBuffer[codeUnitIndex] == 0) {
            recomputeLayout = false;
            break;
          }
          sourceCursor = control->textBuffer + codeUnitIndex + 1;
          destinationCursor = control->textBuffer + codeUnitIndex;
          for (shiftCount = UI_NUMERIC_TEXT_BUFFER_UNITS - 1 - codeUnitIndex; shiftCount != 0; shiftCount--) {
            *destinationCursor = *sourceCursor;
            sourceCursor++;
            destinationCursor++;
          }
        }
        break;
      case KEYBOARD_KEY_CODE_INSERT:
        control->editStateFlags = control->editStateFlags ^ UI_NUMERIC_TEXT_OVERWRITE_MODE;
        recomputeLayout = false;
        break;
      case KEYBOARD_KEY_CODE_HOME:
      case KEYBOARD_KEY_CODE_END:
      case KEYBOARD_KEY_CODE_LEFT:
      case KEYBOARD_KEY_CODE_RIGHT:
        /* Cursor movement (Home/End/Left/Right) collapses the selection at the cursor. */
        if (keyCode == KEYBOARD_KEY_CODE_HOME) {
          control->cursorIndex = 0;
        }
        else if (keyCode == KEYBOARD_KEY_CODE_END) {
          while (control->textBuffer[control->cursorIndex] != 0) {
            control->cursorIndex++;
          }
        }
        else if (keyCode == KEYBOARD_KEY_CODE_LEFT) {
          if (control->cursorIndex != 0) {
            control->cursorIndex--;
          }
        }
        else if (control->textBuffer[control->cursorIndex] != 0) {
          control->cursorIndex++;
        }
        control->selectionStart = control->cursorIndex;
        control->selectionEnd = control->cursorIndex;
        break;
      case KEYBOARD_KEY_CODE_ENTER:
        if ((control->editStateFlags & UI_NUMERIC_TEXT_ACTION_ON_ENTER_ONLY) == 0) {
          delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
          return delegatedResult;
        }
        UiActionQueue_Enqueue(control->actionId,control);
        if (((control->editStateFlags & UI_NUMERIC_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
           (control->activationSound != nullptr)) {
          g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
        }
        return false;
      default:
        /* Raw letter/digit key codes (KEYBOARD_KEY_CODE_CHAR) are swallowed; the characters arrive separately. */
        if ((keyCode & KEYBOARD_KEY_CODE_CHAR(0)) == KEYBOARD_KEY_CODE_CHAR(0)) {
          return false;
        }
        delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
        return delegatedResult;
      }
    }
    else {
      /* Shift: extend the selection from the cursor. */
      switch (keyCode) {
      case KEYBOARD_KEY_CODE_HOME:
        codeUnitIndex = control->cursorIndex;
        if (codeUnitIndex == 0) {
          recomputeLayout = false;
          break;
        }
        control->cursorIndex = 0;
        if (codeUnitIndex == control->selectionStart) {
          control->selectionStart = 0;
        }
        else {
          control->selectionEnd = 0;
        }
        normalizeSelection = true;
        break;
      case KEYBOARD_KEY_CODE_END:
        codeUnitIndex = control->cursorIndex;
        if (control->textBuffer[codeUnitIndex] == 0) {
          recomputeLayout = false;
          break;
        }
        /* NOTE: as in the original, the cursor restarts at 0 and ends at the number of code units
           that followed it, not at the end of the text. */
        control->cursorIndex = 0;
        selectionBoundary = &control->selectionStart;
        if (codeUnitIndex == control->selectionEnd) {
          selectionBoundary = &control->selectionEnd;
        }
        do {
          *selectionBoundary = *selectionBoundary + 1;
          control->cursorIndex++;
          codeUnitIndex++;
        } while (control->textBuffer[codeUnitIndex] != 0);
        normalizeSelection = true;
        break;
      case KEYBOARD_KEY_CODE_LEFT:
        codeUnitIndex = control->cursorIndex;
        if (codeUnitIndex == 0) {
          recomputeLayout = false;
          break;
        }
        control->cursorIndex--;
        if (codeUnitIndex == control->selectionStart) {
          control->selectionStart--;
        }
        else {
          control->selectionEnd--;
        }
        break;
      case KEYBOARD_KEY_CODE_RIGHT:
        codeUnitIndex = control->cursorIndex;
        if (control->textBuffer[codeUnitIndex] == 0) {
          recomputeLayout = false;
          break;
        }
        control->cursorIndex++;
        if (codeUnitIndex == control->selectionEnd) {
          control->selectionEnd++;
        }
        else {
          control->selectionStart++;
        }
        break;
      default:
        delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
        return delegatedResult;
      }
      if ((normalizeSelection) && (control->selectionEnd < control->selectionStart)) {
        /* Home/End may cross the anchor: keep selectionStart <= selectionEnd. */
        LOCK();
        codeUnitIndex = control->selectionEnd;
        control->selectionEnd = control->selectionStart;
        UNLOCK();
        control->selectionStart = codeUnitIndex;
      }
    }
  }
  if (recomputeLayout) {
    UiTextEditControl_RecomputeLayoutAndClampScroll((UiTextEditControl *)control);
  }
  UiNumericTextControl_ParseAndCommitValue(control);
  UiNode_InvalidateRoot(&control->base);
  if (((control->editStateFlags & UI_NUMERIC_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
     (control->activationSound != nullptr)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
  }
  return false;
}

/* Helpers of the text edit keyboard handlers. The text edit controls share the UiTextEditControl header
   (flags, cursor, selection); only their code unit buffers differ, so the buffer and its size in code units
   are passed separately. */

/* True for the characters typed with AltGr (Ctrl+Alt) on a German keyboard: @ | ~ { [ ] } backslash and, in
   Windows-1252, 0xB2/0xB3 (superscript two/three), 0xB5 (micro sign) and 0x80 (euro sign). The keyboard
   handlers treat them as text without the modifier checks. */
static Bool8 UiTextEdit_IsAltGrCharacter(UiKeyboardEventCode keyCode)

{
  return (keyCode == '@') || (keyCode == '|') || (keyCode == '~') || (keyCode == CP1252_SUPERSCRIPT_TWO) ||
         (keyCode == CP1252_SUPERSCRIPT_THREE) || (keyCode == '{') || (keyCode == '[') || (keyCode == ']') ||
         (keyCode == '}') || (keyCode == '\\') || (keyCode == CP1252_MICRO_SIGN) || (keyCode == CP1252_EURO_SIGN);
}

/* True when the key is a shortcut rather than text: any key with Alt, or a letter typed with Ctrl or Alt. */
static Bool8 UiTextEdit_IsModifierShortcut(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode)

{
  return ((keyboardStateMask & KEYBOARD_STATE_ALT) != 0) ||
         (((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0) &&
          ((keyboardStateMask & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0) &&
          ('@' < keyCode) && ((keyCode < '[') || (('`' < keyCode) && (keyCode < '{'))));
}

/* True for Home, End, Left and Right. */
static Bool8 UiTextEdit_IsCursorMovementKey(UiKeyboardEventCode keyCode)

{
  return (keyCode == KEYBOARD_KEY_CODE_HOME) || (keyCode == KEYBOARD_KEY_CODE_END) ||
         (keyCode == KEYBOARD_KEY_CODE_LEFT) || (keyCode == KEYBOARD_KEY_CODE_RIGHT);
}

/* True unless the selection is collapsed at the cursor. */
static Bool8 UiTextEdit_HasSelection(const UiTextEditControl *edit)

{
  return (edit->cursorIndex != edit->selectionStart) || (edit->cursorIndex != edit->selectionEnd);
}

/* Copies count code units front to back from source to destination (a move towards the buffer start). */
static void UiTextEdit_MoveCodeUnitsDown(uint16_t *destination,const uint16_t *source,uint32_t count)

{
  for (; count != 0; count--) {
    *destination = *source;
    source++;
    destination++;
  }
}

/* Keeps selectionStart <= selectionEnd after the moving selection end crossed the anchor. */
static void UiTextEdit_OrderSelection(UiTextEditControl *edit)

{
  UiTextCodeUnitIndex formerSelectionEnd;

  if (edit->selectionEnd < edit->selectionStart) {
    formerSelectionEnd = edit->selectionEnd;
    edit->selectionEnd = edit->selectionStart;
    edit->selectionStart = formerSelectionEnd;
  }
}

/* Removes the selected range: the code units from selectionEnd up to the buffer end move down to
   selectionStart and the freed tail is zero-filled. Cursor and selection collapse at selectionStart. */
static void UiTextEdit_RemoveSelectedRange(UiTextEditControl *edit,uint16_t *buffer,uint32_t bufferUnits)

{
  uint32_t removedCount;
  uint16_t *zeroFillCursor;

  removedCount = edit->selectionEnd - edit->selectionStart;
  UiTextEdit_MoveCodeUnitsDown(buffer + edit->selectionStart,buffer + edit->selectionEnd,
                               bufferUnits - edit->selectionEnd);
  for (zeroFillCursor = buffer + (bufferUnits - removedCount); removedCount != 0; removedCount--) {
    *zeroFillCursor = 0;
    zeroFillCursor++;
  }
  edit->cursorIndex = edit->selectionStart;
  edit->selectionEnd = edit->selectionStart;
}

/* Types one code unit at the cursor, replacing a selection first. Only below insertLimit: the cursor and
   the collapsed selection advance; in insert mode the text from the cursor up to insertLimit - 1 shifts up
   by one (the code unit at insertLimit - 1 falls off), in overwrite mode the code unit at the cursor is
   replaced. */
static void UiTextEdit_InsertCodeUnit(UiTextEditControl *edit,uint16_t *buffer,uint32_t bufferUnits,
          uint32_t insertLimit,Bool8 overwriteMode,uint16_t codeUnit)

{
  uint32_t insertIndex;
  uint16_t displacedCodeUnit;

  if (UiTextEdit_HasSelection(edit)) {
    UiTextEdit_RemoveSelectedRange(edit,buffer,bufferUnits);
  }
  insertIndex = edit->cursorIndex;
  if (insertIndex < insertLimit) {
    edit->cursorIndex++;
    edit->selectionStart++;
    edit->selectionEnd++;
    if (!overwriteMode) {
      do {
        displacedCodeUnit = buffer[insertIndex];
        buffer[insertIndex] = codeUnit;
        codeUnit = displacedCodeUnit;
        insertIndex++;
      } while (insertIndex < insertLimit);
    }
    else {
      buffer[insertIndex] = codeUnit;
    }
  }
}

/* Backspace (deleteBefore) or Delete: removes the selection, or else the code unit before / at the cursor
   (the text above it moves down). Returns false when there was nothing to delete: Backspace at the text
   start or Delete at the text end. */
static Bool8 UiTextEdit_DeleteAtCursor(UiTextEditControl *edit,uint16_t *buffer,uint32_t bufferUnits,
          Bool8 deleteBefore)

{
  UiTextCodeUnitIndex cursorIndex;

  if (UiTextEdit_HasSelection(edit)) {
    UiTextEdit_RemoveSelectedRange(edit,buffer,bufferUnits);
    return true;
  }
  cursorIndex = edit->cursorIndex;
  if (deleteBefore) {
    if (cursorIndex == 0) {
      return false;
    }
    UiTextEdit_MoveCodeUnitsDown(buffer + (cursorIndex - 1),buffer + cursorIndex,bufferUnits - cursorIndex);
    edit->cursorIndex--;
    edit->selectionStart = edit->cursorIndex;
    edit->selectionEnd = edit->cursorIndex;
  }
  else {
    if (buffer[cursorIndex] == 0) {
      return false;
    }
    UiTextEdit_MoveCodeUnitsDown(buffer + cursorIndex,buffer + cursorIndex + 1,bufferUnits - 1 - cursorIndex);
  }
  return true;
}

/* Home/End/Left/Right without Shift: moves the cursor and collapses the selection at it. */
static void UiTextEdit_MoveCursorAndCollapseSelection(UiTextEditControl *edit,const uint16_t *buffer,
          UiKeyboardEventCode keyCode)

{
  if (keyCode == KEYBOARD_KEY_CODE_HOME) {
    edit->cursorIndex = 0;
  }
  else if (keyCode == KEYBOARD_KEY_CODE_END) {
    while (buffer[edit->cursorIndex] != 0) {
      edit->cursorIndex++;
    }
  }
  else if (keyCode == KEYBOARD_KEY_CODE_LEFT) {
    if (edit->cursorIndex != 0) {
      edit->cursorIndex--;
    }
  }
  else if (buffer[edit->cursorIndex] != 0) {
    edit->cursorIndex++;
  }
  edit->selectionStart = edit->cursorIndex;
  edit->selectionEnd = edit->cursorIndex;
}

/* Shift+Home/End/Left/Right: moves the cursor together with the selection end at the cursor. Home and End
   may move that end across the anchor; the selection is then reordered. Returns false when the cursor
   could not move. */
static Bool8 UiTextEdit_ExtendSelectionByKey(UiTextEditControl *edit,const uint16_t *buffer,
          UiKeyboardEventCode keyCode)

{
  UiTextCodeUnitIndex formerCursorIndex;
  UiTextCodeUnitIndex scanIndex;
  UiTextCodeUnitIndex *selectionBoundary;

  formerCursorIndex = edit->cursorIndex;
  if (keyCode == KEYBOARD_KEY_CODE_HOME) {
    if (formerCursorIndex == 0) {
      return false;
    }
    edit->cursorIndex = 0;
    if (formerCursorIndex == edit->selectionStart) {
      edit->selectionStart = 0;
    }
    else {
      edit->selectionEnd = 0;
    }
    UiTextEdit_OrderSelection(edit);
  }
  else if (keyCode == KEYBOARD_KEY_CODE_END) {
    if (buffer[formerCursorIndex] == 0) {
      return false;
    }
    /* NOTE: as in the original, the cursor restarts at 0 and ends at the number of code units
       that followed it, not at the end of the text. */
    edit->cursorIndex = 0;
    selectionBoundary = &edit->selectionStart;
    if (formerCursorIndex == edit->selectionEnd) {
      selectionBoundary = &edit->selectionEnd;
    }
    scanIndex = formerCursorIndex;
    do {
      *selectionBoundary = *selectionBoundary + 1;
      edit->cursorIndex++;
      scanIndex++;
    } while (buffer[scanIndex] != 0);
    UiTextEdit_OrderSelection(edit);
  }
  else if (keyCode == KEYBOARD_KEY_CODE_LEFT) {
    if (formerCursorIndex == 0) {
      return false;
    }
    edit->cursorIndex--;
    if (formerCursorIndex == edit->selectionStart) {
      edit->selectionStart--;
    }
    else {
      edit->selectionEnd--;
    }
  }
  else {
    if (buffer[formerCursorIndex] == 0) {
      return false;
    }
    edit->cursorIndex++;
    if (formerCursorIndex == edit->selectionEnd) {
      edit->selectionEnd++;
    }
    else {
      edit->selectionStart++;
    }
  }
  return true;
}

/* Path characters: letters, digits, '-' and '.'; '*' and '?' only with UI_PATH_TEXT_ALLOW_WILDCARDS,
   ':' and the backslash not with UI_PATH_TEXT_NAME_ONLY. */
static Bool8 UiPathTextEdit_IsPathCharacterAccepted(const UiPathTextEditControl *control,UiKeyboardEventCode keyCode)

{
  if ((keyCode == '*') || (keyCode == '?')) {
    return (control->editStateFlags & UI_PATH_TEXT_ALLOW_WILDCARDS) != 0;
  }
  if ((keyCode == '-') || (keyCode == '.')) {
    return true;
  }
  if (keyCode < '0') {
    return false;
  }
  if (keyCode <= '9') {
    return true;
  }
  if ((keyCode == ':') || (keyCode == '\\')) {
    return (control->editStateFlags & UI_PATH_TEXT_NAME_ONLY) == 0;
  }
  if (keyCode < 'A') {
    return false;
  }
  if (keyCode <= 'Z') {
    return true;
  }
  return ('a' <= keyCode) && (keyCode <= 'z');
}

/* Start of the path segment before the cursor (cursorIndex != 0): just after the nearest '\' or '.' at
   index cursorIndex - 2 down to 1, else 0. */
static UiTextCodeUnitIndex UiPathTextEdit_FindPreviousSegmentStart(const uint16_t *pathBuffer,
          UiTextCodeUnitIndex cursorIndex)

{
  UiTextCodeUnitIndex scanIndex;

  if (cursorIndex == 1) {
    return 0;
  }
  for (scanIndex = cursorIndex - 2; scanIndex != 0; scanIndex--) {
    if ((pathBuffer[scanIndex] == '\\') || (pathBuffer[scanIndex] == '.')) {
      return scanIndex + 1;
    }
  }
  return 0;
}

/* Start of the next path segment: just after the first '\' or '.' at or after the cursor, else the end of
   the text. */
static UiTextCodeUnitIndex UiPathTextEdit_FindNextSegmentStart(const uint16_t *pathBuffer,
          UiTextCodeUnitIndex cursorIndex)

{
  UiTextCodeUnitIndex scanIndex;

  for (scanIndex = cursorIndex; pathBuffer[scanIndex] != 0; scanIndex++) {
    if ((pathBuffer[scanIndex] == '\\') || (pathBuffer[scanIndex] == '.')) {
      return scanIndex + 1;
    }
  }
  return scanIndex;
}

/* Ctrl+Left/Right: moves the cursor to the previous/next path segment start. Without Shift the selection
   collapses there; with Shift the selection end at the cursor moves along and the selection is reordered.
   Ctrl+Left at the text start does nothing. */
static void UiPathTextEdit_JumpToSegment(UiPathTextEditControl *control,Bool8 towardsStart,Bool8 extendSelection)

{
  UiTextCodeUnitIndex formerCursorIndex;
  UiTextCodeUnitIndex segmentStart;
  UiTextCodeUnitIndex *selectionBoundary;

  formerCursorIndex = control->cursorIndex;
  if (towardsStart && (formerCursorIndex == 0)) {
    return;
  }
  /* The selection end at the cursor moves. */
  selectionBoundary = &control->selectionStart;
  if (formerCursorIndex != control->selectionStart) {
    selectionBoundary = &control->selectionEnd;
  }
  if (towardsStart) {
    segmentStart = UiPathTextEdit_FindPreviousSegmentStart(control->pathBuffer,formerCursorIndex);
  }
  else {
    segmentStart = UiPathTextEdit_FindNextSegmentStart(control->pathBuffer,formerCursorIndex);
  }
  control->cursorIndex = segmentStart;
  if (extendSelection) {
    *selectionBoundary = segmentStart;
    UiTextEdit_OrderSelection((UiTextEditControl *)control);
  }
  else {
    control->selectionStart = segmentStart;
    control->selectionEnd = segmentStart;
  }
}

/* Plays the activation sound when the control has UI_TEXT_EDIT_PLAY_INTERACTION_SOUND and a sound. */
static void UiPathTextEdit_PlayInteractionSound(UiPathTextEditControl *control)

{
  if (((control->editStateFlags & UI_TEXT_EDIT_PLAY_INTERACTION_SOUND) != 0) &&
     (control->activationSound != nullptr)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
  }
}

/* Keyboard handler of the DOS path edit (keyboardEvent slot of g_UiPathTextEditControlVtable): inserts the
   characters a DOS 8.3 path may contain, edits and moves the cursor and Shift selection, and Ctrl+Left/Right
   jump between path segments. After every handled key the path is validated and, unless the control acts on
   Enter only, its action is queued. Unhandled keys go to UiNode_DefaultKeyboardEventMoveFocusNext.
   Returns false: consumed.
*/
Bool8 UiPathTextEditControl_HandleKeyboardAndValidate(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiPathTextEditControl *control)

{
  UiTextEditControl *edit;
  Bool8 isAltGrCharacter;
  Bool8 recomputeLayout;

  edit = (UiTextEditControl *)control;
  if (((control->editStateFlags & UI_TEXT_EDIT_READ_ONLY) != 0) ||
     (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
    return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
  }
  /* AltGr characters skip the modifier checks; of them, only the backslash passes the path character
     filter below. */
  isAltGrCharacter = UiTextEdit_IsAltGrCharacter(keyCode);
  if (!isAltGrCharacter && UiTextEdit_IsModifierShortcut(keyboardStateMask,keyCode)) {
    return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
  }
  /* Handled keys end in the shared tail: optional layout recompute, then validity update, action,
     invalidate and the interaction sound. */
  recomputeLayout = true;
  if ((isAltGrCharacter) || ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0)) {
    if (!UiPathTextEdit_IsPathCharacterAccepted(control,keyCode)) {
      return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    }
    UiTextEdit_InsertCodeUnit(edit,control->pathBuffer,UI_PATH_TEXT_BUFFER_UNITS,UI_PATH_TEXT_BUFFER_UNITS - 2,
                              (control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) != 0,(uint16_t)keyCode);
  }
  else if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    /* Ctrl+Left/Right: jump to the previous/next path segment ('\' or '.'), Shift extends. */
    if ((keyCode != KEYBOARD_KEY_CODE_LEFT) && (keyCode != KEYBOARD_KEY_CODE_RIGHT)) {
      return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    }
    UiPathTextEdit_JumpToSegment(control,keyCode == KEYBOARD_KEY_CODE_LEFT,
                                 (keyboardStateMask & KEYBOARD_STATE_SHIFT) != 0);
  }
  else if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
    /* Shift: extend the selection from the cursor. */
    if (!UiTextEdit_IsCursorMovementKey(keyCode)) {
      return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    }
    recomputeLayout = UiTextEdit_ExtendSelectionByKey(edit,control->pathBuffer,keyCode);
  }
  else {
    switch (keyCode) {
    case KEYBOARD_KEY_CODE_BACKSPACE:
    case KEYBOARD_KEY_CODE_DELETE:
      recomputeLayout = UiTextEdit_DeleteAtCursor(edit,control->pathBuffer,UI_PATH_TEXT_BUFFER_UNITS,
                                                  keyCode == KEYBOARD_KEY_CODE_BACKSPACE);
      break;
    case KEYBOARD_KEY_CODE_INSERT:
      control->editStateFlags = control->editStateFlags ^ UI_TEXT_EDIT_OVERWRITE_MODE;
      recomputeLayout = false;
      break;
    case KEYBOARD_KEY_CODE_HOME:
    case KEYBOARD_KEY_CODE_END:
    case KEYBOARD_KEY_CODE_LEFT:
    case KEYBOARD_KEY_CODE_RIGHT:
      UiTextEdit_MoveCursorAndCollapseSelection(edit,control->pathBuffer,keyCode);
      break;
    case KEYBOARD_KEY_CODE_ENTER:
      if ((control->editStateFlags & UI_TEXT_EDIT_ACTION_ON_ENTER_ONLY) == 0) {
        return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
      }
      UiActionQueue_Enqueue(control->actionId,control);
      UiPathTextEdit_PlayInteractionSound(control);
      return false;
    default:
      /* Raw letter/digit key codes (KEYBOARD_KEY_CODE_CHAR) are swallowed; the characters arrive separately. */
      if ((keyCode & KEYBOARD_KEY_CODE_CHAR(0)) == KEYBOARD_KEY_CODE_CHAR(0)) {
        return false;
      }
      return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    }
  }
  if (recomputeLayout) {
    UiTextEditControl_RecomputeLayoutAndClampScroll(edit);
  }
  UiPathTextControl_UpdateDos83Validity(control);
  if ((control->editStateFlags & UI_TEXT_EDIT_ACTION_ON_ENTER_ONLY) == 0) {
    UiActionQueue_Enqueue(control->actionId,control);
  }
  UiNode_InvalidateRoot(&control->base);
  UiPathTextEdit_PlayInteractionSound(control);
  return false;
}

/* Ctrl+Left target from cursorIndex != 0. After a space: back over the spaces, stopping just after the
   previous word. Otherwise: back to the start of the current word; when that start lies after a single
   space (the code unit before the space is not a space), the target is that space instead. */
static UiTextCodeUnitIndex UiRequiredTextEdit_FindPreviousWordStop(const uint16_t *textBuffer,
          UiTextCodeUnitIndex cursorIndex)

{
  UiTextCodeUnitIndex wordStop;

  wordStop = cursorIndex - 1;
  if (textBuffer[cursorIndex - 1] == ' ') {
    while ((wordStop != 0) && (textBuffer[wordStop - 1] == ' ')) {
      wordStop--;
    }
    return wordStop;
  }
  while ((wordStop != 0) && (textBuffer[wordStop - 1] != ' ')) {
    wordStop--;
  }
  if ((1 < (int)wordStop) && (textBuffer[wordStop - 2] != ' ')) {
    wordStop--;
  }
  return wordStop;
}

/* Ctrl+Right target. With skipSpaces: forward over the spaces at the cursor. Otherwise: just past the next
   space, or back onto that space when another space follows it; the end of the text when no space follows. */
static UiTextCodeUnitIndex UiRequiredTextEdit_FindNextWordStop(const uint16_t *textBuffer,
          UiTextCodeUnitIndex cursorIndex,Bool8 skipSpaces)

{
  UiTextCodeUnitIndex wordStop;

  wordStop = cursorIndex;
  if (skipSpaces) {
    while (textBuffer[wordStop] == ' ') {
      wordStop++;
    }
    return wordStop;
  }
  while (textBuffer[wordStop] != 0) {
    wordStop++;
    if (textBuffer[wordStop - 1] == ' ') {
      if (textBuffer[wordStop] == ' ') {
        wordStop--;
      }
      return wordStop;
    }
  }
  return wordStop;
}

/* Ctrl+Left/Right: moves the cursor to the previous/next word stop. Without Shift the selection collapses
   there; with Shift the selection end at the cursor moves along and the selection is reordered. Ctrl+Left at
   the text start does nothing. */
static void UiRequiredTextEdit_JumpToWord(UiRequiredTextEditControl *control,Bool8 towardsStart,
          Bool8 extendSelection)

{
  UiTextCodeUnitIndex formerCursorIndex;
  UiTextCodeUnitIndex wordStop;
  UiTextCodeUnitIndex *selectionBoundary;
  Bool8 skipSpaces;

  formerCursorIndex = control->cursorIndex;
  if (towardsStart && (formerCursorIndex == 0)) {
    return;
  }
  /* The selection end at the cursor moves. */
  selectionBoundary = &control->selectionStart;
  if (formerCursorIndex != control->selectionStart) {
    selectionBoundary = &control->selectionEnd;
  }
  if (towardsStart) {
    wordStop = UiRequiredTextEdit_FindPreviousWordStop(control->textBuffer,formerCursorIndex);
  }
  else {
    /* NOTE: as in the original, with Shift and the cursor at selectionStart the space-skipping case is
       not taken even if the cursor is on a space. */
    skipSpaces = control->textBuffer[formerCursorIndex] == ' ';
    if (extendSelection && (formerCursorIndex == control->selectionStart)) {
      skipSpaces = false;
    }
    wordStop = UiRequiredTextEdit_FindNextWordStop(control->textBuffer,formerCursorIndex,skipSpaces);
  }
  control->cursorIndex = wordStop;
  if (extendSelection) {
    *selectionBoundary = wordStop;
    UiTextEdit_OrderSelection((UiTextEditControl *)control);
  }
  else {
    control->selectionStart = wordStop;
    control->selectionEnd = wordStop;
  }
}

/* Plays the activation sound when the control has UI_REQUIRED_TEXT_PLAY_INTERACTION_SOUND and a sound. */
static void UiRequiredTextEdit_PlayInteractionSound(UiRequiredTextEditControl *control)

{
  if (((control->editStateFlags & UI_REQUIRED_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
     (control->activationSound != nullptr)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
  }
}

/* Keyboard handler of the free-text edit that must not stay empty (keyboardEvent slot of
   g_UiRequiredTextEditControlVtable): inserts any character, edits and moves the cursor and Shift selection,
   Ctrl+Left/Right jump between space-separated words, and Escape clears the text when the control allows it.
   After every handled key the non-empty validity is updated and, unless the control acts on Enter only, its
   action is queued. Unhandled keys go to UiNode_DefaultKeyboardEventMoveFocusNext. Returns false: consumed.
*/
Bool8 UiRequiredTextEditControl_HandleKeyboardAndValidate
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiRequiredTextEditControl *control)

{
  UiTextEditControl *edit;
  UiTextCodeUnitIndex clearIndex;
  Bool8 isAltGrCharacter;
  Bool8 recomputeLayout;

  edit = (UiTextEditControl *)control;
  if (((control->editStateFlags & UI_REQUIRED_TEXT_READ_ONLY) != 0) ||
     (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
    return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
  }
  /* Characters typed with AltGr are inserted without the modifier checks. */
  isAltGrCharacter = UiTextEdit_IsAltGrCharacter(keyCode);
  if (!isAltGrCharacter && UiTextEdit_IsModifierShortcut(keyboardStateMask,keyCode)) {
    return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
  }
  /* Handled keys end in the shared tail: optional layout recompute, then validity update, action,
     invalidate and the interaction sound. */
  recomputeLayout = true;
  if ((isAltGrCharacter) || ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0)) {
    /* Insert the character, replacing a selection. */
    UiTextEdit_InsertCodeUnit(edit,control->textBuffer,control->bufferCapacityCodeUnits,
                              control->bufferCapacityCodeUnits - 1,
                              (control->editStateFlags & UI_REQUIRED_TEXT_OVERWRITE_MODE) != 0,(uint16_t)keyCode);
  }
  else if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    /* Ctrl+Left/Right: jump between space-separated words, Shift extends the selection. */
    if ((keyCode != KEYBOARD_KEY_CODE_LEFT) && (keyCode != KEYBOARD_KEY_CODE_RIGHT)) {
      return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    }
    UiRequiredTextEdit_JumpToWord(control,keyCode == KEYBOARD_KEY_CODE_LEFT,
                                  (keyboardStateMask & KEYBOARD_STATE_SHIFT) != 0);
  }
  else if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
    /* Shift: extend the selection from the cursor. */
    if (!UiTextEdit_IsCursorMovementKey(keyCode)) {
      return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    }
    recomputeLayout = UiTextEdit_ExtendSelectionByKey(edit,control->textBuffer,keyCode);
  }
  else {
    switch (keyCode) {
    case KEYBOARD_KEY_CODE_BACKSPACE:
    case KEYBOARD_KEY_CODE_DELETE:
      recomputeLayout = UiTextEdit_DeleteAtCursor(edit,control->textBuffer,control->bufferCapacityCodeUnits,
                                                  keyCode == KEYBOARD_KEY_CODE_BACKSPACE);
      break;
    case KEYBOARD_KEY_CODE_INSERT:
      control->editStateFlags = control->editStateFlags ^ UI_REQUIRED_TEXT_OVERWRITE_MODE;
      recomputeLayout = false;
      break;
    case KEYBOARD_KEY_CODE_HOME:
    case KEYBOARD_KEY_CODE_END:
    case KEYBOARD_KEY_CODE_LEFT:
    case KEYBOARD_KEY_CODE_RIGHT:
      UiTextEdit_MoveCursorAndCollapseSelection(edit,control->textBuffer,keyCode);
      break;
    case KEYBOARD_KEY_CODE_ENTER:
      if ((control->editStateFlags & UI_REQUIRED_TEXT_ACTION_ON_ENTER_ONLY) == 0) {
        return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
      }
      UiActionQueue_Enqueue(control->actionId,control);
      UiRequiredTextEdit_PlayInteractionSound(control);
      return false;
    case KEYBOARD_KEY_CODE_ESCAPE:
      if ((control->editStateFlags & UI_REQUIRED_TEXT_ESCAPE_CLEARS_AND_QUEUES_ACTION) == 0) {
        return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
      }
      for (clearIndex = 0; clearIndex != control->bufferCapacityCodeUnits; clearIndex++) {
        control->textBuffer[clearIndex] = 0;
      }
      control->cursorIndex = 0;
      control->selectionStart = 0;
      control->selectionEnd = 0;
      UiActionQueue_Enqueue(control->actionId,control);
      UiRequiredTextEdit_PlayInteractionSound(control);
      return false;
    default:
      /* Raw letter/digit key codes (KEYBOARD_KEY_CODE_CHAR) are swallowed; the characters arrive separately. */
      if ((keyCode & KEYBOARD_KEY_CODE_CHAR(0)) == KEYBOARD_KEY_CODE_CHAR(0)) {
        return false;
      }
      return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    }
  }
  if (recomputeLayout) {
    UiTextEditControl_RecomputeLayoutAndClampScroll(edit);
  }
  UiTextControl_UpdateNonEmptyValidity(edit);
  if ((control->editStateFlags & UI_REQUIRED_TEXT_ACTION_ON_ENTER_ONLY) == 0) {
    UiActionQueue_Enqueue(control->actionId,control);
  }
  UiNode_InvalidateRoot(&control->base);
  UiRequiredTextEdit_PlayInteractionSound(control);
  return false;
}

/* Relocation of a loaded numeric text edit (relocate slot of g_UiNumericTextEditControlVtable): makes it a
   fallback focus target unless it is the preferred one, hides the caret, rebuilds the text from the value,
   selects all of it and relocates the children.
*/
void UiNumericTextEditControl_RelocateAndRebuildText
          (UiSerializedRelocationDelta relocationDelta,UiNumericTextControl *control)

{
  uint16_t *textCursor;
  uint16_t currentCodeUnit;
  UiNodeFlags *nodeFlagsField;
  
  if (((control->base).nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) == 0) {
    nodeFlagsField = &(control->base).nodeFlags;
    *nodeFlagsField = *nodeFlagsField | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  /* clears the caret phase and the blink frame counter (top byte) */
  control->editStateFlags = control->editStateFlags & (UI_STATE_FLAGS_MASK & ~UI_TEXT_EDIT_CARET_VISIBLE_PHASE);
  UiNumericTextControl_RebuildTextFromValue(control);
  textCursor = control->textBuffer;
  control->cursorIndex = 0;
  control->selectionStart = 0;
  control->selectionEnd = 0;
  currentCodeUnit = *textCursor;
  while (currentCodeUnit != 0) {
    textCursor++;
    control->selectionEnd++;
    currentCodeUnit = *textCursor;
  }
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}

/* Draws a text edit (drawClipped slot of g_UiNumericTextEditControlVtable, g_UiPathTextEditControlVtable and
   g_UiRequiredTextEditControlVtable): the optional win.gfx frame and tiled interior, the selection highlight,
   the text in the active, invalid-value or disabled style, and in the visible caret phase the insert or
   overwrite caret with its shadow. The clip rectangle is narrowed to the text area first.
*/
void UiTextEditControl_DrawTextSelectionAndCaret
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextEditControl *control)

{
  UiTextCodeUnitCount selectionEnd;
  UiPixelCoordinate selectionStartWidth;
  UiPixelCoordinate selectionEndWidth;
  UiPixelCoordinate caretWidth;
  int styleVerticalOffset;
  int textAreaLeft;
  int textAreaTop;
  int textAreaRight;
  int textAreaBottom;
  int rightEdgeOffset;
  int bottomEdgeOffset;
  int textOffsetX;
  int textOffsetY;
  int selectionRight;
  int caretX;
  int caretY;
  uint32_t tileBottom;
  uint32_t borderWidth;
  uint32_t caretFrame;
  UiPackedTextStyle packedStyle;
  GraphicsTextureLogicalSize cornerTileSize;
  uint32_t fontLineHeight;
  GraphicsTextureSourceAsset *caretTextureSource;
  SoftwareFramebufferAccess *caretFramebuffer;

  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_FRAMED_CHROME) == 0) {
    borderWidth = 0;
    textAreaLeft = (control->base).left;
    textAreaTop = (control->base).top;
    textAreaRight = (control->base).right;
    textAreaBottom = (control->base).bottom;
  }
  else {
    rightEdgeOffset = (control->base).layoutWidth;
    bottomEdgeOffset = (control->base).layoutHeight;
    cornerTileSize = g_GraphicsTextureSourceGetLogicalSize
                       (UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,g_UiWindowTextureSource);
    tileBottom = cornerTileSize.logicalHeightPixels;
    borderWidth = cornerTileSize.logicalWidthPixels;
    rightEdgeOffset = rightEdgeOffset - borderWidth;
    bottomEdgeOffset = bottomEdgeOffset - tileBottom;
    if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_TILED_INTERIOR) != 0) {
      UiWindow_BlitTiledInterior
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_INTERIOR,bottomEdgeOffset,
                 rightEdgeOffset,
                 tileBottom,borderWidth,control);
    }
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,(control->base).left,
               UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME,
               g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,
               rightEdgeOffset + (control->base).left,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_TOP_RIGHT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeOffset + (control->base).top,
               (control->base).left,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_BOTTOM_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeOffset + (control->base).top,
               rightEdgeOffset + (control->base).left,
               UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,g_UiWindowTextureSource,
               g_FramebufferAccess);
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_TOP,
               rightEdgeOffset,0,borderWidth,control);
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_LEFT,
               bottomEdgeOffset,tileBottom,0,control);
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_RIGHT,
               bottomEdgeOffset,tileBottom,rightEdgeOffset,control);
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_BOTTOM,
               rightEdgeOffset,bottomEdgeOffset,borderWidth,control);
    textAreaTop = tileBottom + (control->base).top;
    textAreaRight = rightEdgeOffset + (control->base).left;
    textAreaLeft = borderWidth + (control->base).left;
    textAreaBottom = bottomEdgeOffset + (control->base).top;
  }
  if (clipLeft < textAreaLeft) {
    clipLeft = textAreaLeft;
  }
  if (clipTop < textAreaTop) {
    clipTop = textAreaTop;
  }
  FontGlyph_GetLogicalSizeActiveFont(0,&fontLineHeight);
  if (textAreaRight < clipRight) {
    clipRight = textAreaRight;
  }
  textOffsetX = (borderWidth - control->horizontalScrollPixels) + 2;
  if (textAreaBottom < clipBottom) {
    clipBottom = textAreaBottom;
  }
  textOffsetY = (int)((control->base).layoutHeight - fontLineHeight) >> 1;
  selectionEnd = control->selectionEnd;
  if (((control->editStateFlags & UI_TEXT_EDIT_READ_ONLY) == 0) &&
     (control->selectionStart != selectionEnd)) {
    selectionStartWidth = UiTextEditControl_MeasurePrefixWidth(control->selectionStart,control);
    selectionEndWidth = UiTextEditControl_MeasurePrefixWidth(selectionEnd,control);
    if (selectionStartWidth == 0) {
      selectionStartWidth = -2;
    }
    selectionRight = selectionEndWidth + textOffsetX;
    if (clipRight < selectionEndWidth + textOffsetX) {
      selectionRight = clipRight;
    }
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TEXT_SELECTION,selectionRight,
               textOffsetY - 1,selectionStartWidth + textOffsetX,control);
  }
  packedStyle = g_UiTextEditActiveTextStyle;
  if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
    control->editStateFlags = control->editStateFlags & ~UI_TEXT_EDIT_CARET_VISIBLE_PHASE;
  }
  if ((control->editStateFlags & UI_TEXT_EDIT_READ_ONLY) != 0) {
    control->editStateFlags = control->editStateFlags & ~UI_TEXT_EDIT_CARET_VISIBLE_PHASE;
  }
  if ((control->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    packedStyle = g_UiTextEditInactiveTextStyle;
  }
  if (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    packedStyle = g_UiTextEditDisabledTextStyle;
  }
  if ((control->editStateFlags & UI_TEXT_EDIT_CARET_VISIBLE_PHASE) == 0) {
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,packedStyle,control->textBuffer,
               textOffsetY + (control->base).top,textOffsetX + (control->base).left);
  }
  else {
    caretWidth = UiTextEditControl_MeasurePrefixWidth(control->cursorIndex,control);
    caretX = caretWidth - 2 + textOffsetX + (control->base).left;
    caretFrame = UI_WINDOW_SUBRESOURCE_CARET_INSERT;
    if ((control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) != 0) {
      caretFrame = UI_WINDOW_SUBRESOURCE_CARET_OVERWRITE;
    }
    styleVerticalOffset = (int)(packedStyle << 16) >> 24; /* signed byte 1 of the packed style */
    caretY = textOffsetY - 1 + (control->base).top + styleVerticalOffset;
    if ((control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) != 0) {
      caretY++;
    }
    caretTextureSource = g_UiWindowTextureSource;
    caretFramebuffer = g_FramebufferAccess;
    /* the shadow always uses the insert caret, also in overwrite mode (as in the original) */
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,caretY,caretX + styleVerticalOffset,
               TEXT_SHADOW_COLOR_ARGB,UI_WINDOW_SUBRESOURCE_CARET_INSERT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,packedStyle,control->textBuffer,
               textOffsetY + (control->base).top,textOffsetX + (control->base).left);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,textOffsetY + (control->base).top - 1,caretX,
               caretFrame,
               caretTextureSource,caretFramebuffer);
  }
  g_GraphicsFramebufferEndAccess();
}

/* Primary button press on a text edit (nonRightPress slot of the numeric, path and required text edit
   vtables): unless read-only, starts a pointer selection by placing the cursor and an empty selection at the
   pointer; plays the interaction sound when enabled.
*/
void UiTextEditControl_BeginSelectionAtPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control)

{
  UiTextCodeUnitIndex cursorIndexAtPointer;
  
  if ((control->editStateFlags & UI_TEXT_EDIT_READ_ONLY) == 0) {
    control->editStateFlags = control->editStateFlags | UI_TEXT_EDIT_POINTER_SELECTION_ACTIVE;
    cursorIndexAtPointer = UiTextEditControl_FindCursorIndexAtX(pointerX,control);
    control->cursorIndex = cursorIndexAtPointer;
    control->selectionStart = cursorIndexAtPointer;
    control->selectionEnd = cursorIndexAtPointer;
  }
  if (((control->editStateFlags & UI_TEXT_EDIT_PLAY_INTERACTION_SOUND) != 0) &&
     (control->activationSound != nullptr)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
  }
  return;
}

/* Primary-button drag over a text edit (nonRightDrag slot of the numeric, path and required text edit
   vtables): while a pointer selection is active, moves the cursor and the selection end it sits on to the
   pointer, keeps selectionStart <= selectionEnd, lays the control out again (scroll) and redraws it.
*/
void UiTextEditControl_UpdateSelectionFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control)

{
  UiTextCodeUnitIndex previousCursorIndex;
  uint32_t pointerCursorIndex;

  if ((control->editStateFlags & UI_TEXT_EDIT_POINTER_SELECTION_ACTIVE) != 0) {
    previousCursorIndex = control->cursorIndex;
    pointerCursorIndex = UiTextEditControl_FindCursorIndexAtX(pointerX,control);
    control->cursorIndex = pointerCursorIndex;
    if (previousCursorIndex == control->selectionStart) {
      control->selectionStart = pointerCursorIndex;
    }
    else {
      control->selectionEnd = pointerCursorIndex;
    }
    UiTextEdit_OrderSelection(control);
    (control->base).vtable->layout(&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
}

/* Relocation of a loaded DOS path edit (relocate slot of g_UiPathTextEditControlVtable): makes it a fallback
   focus target unless it is the preferred one, hides the caret, validates the path, selects all of it and
   relocates the children.
*/
void UiPathTextEditControl_RelocateAndValidateDos83
          (UiSerializedRelocationDelta relocationDelta,UiPathTextEditControl *control)

{
  uint16_t *textCursor;
  UiNodeFlags *nodeFlagsField;
  uint16_t currentCodeUnit;
  
  if (((control->base).nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) == 0) {
    nodeFlagsField = &(control->base).nodeFlags;
    *nodeFlagsField = *nodeFlagsField | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  /* clears the caret phase and the blink frame counter (top byte) */
  control->editStateFlags = control->editStateFlags & (UI_STATE_FLAGS_MASK & ~UI_TEXT_EDIT_CARET_VISIBLE_PHASE);
  UiPathTextControl_UpdateDos83Validity(control);
  textCursor = control->pathBuffer;
  control->cursorIndex = 0;
  control->selectionStart = 0;
  control->selectionEnd = 0;
  currentCodeUnit = *textCursor;
  while (currentCodeUnit != 0) {
    textCursor++;
    control->selectionEnd++;
    currentCodeUnit = *textCursor;
  }
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}

/* Relocation of a loaded required text edit (relocate slot of g_UiRequiredTextEditControlVtable): makes it a
   fallback focus target unless it is the preferred one, hides the caret, updates the non-empty validity,
   selects all of the text and relocates the children.
*/
void UiRequiredTextEditControl_RelocateAndValidateNonEmpty
          (UiSerializedRelocationDelta relocationDelta,UiRequiredTextEditControl *control)

{
  uint16_t *textCursor;
  UiNodeFlags *nodeFlagsField;
  uint16_t currentCodeUnit;
  
  if (((control->base).nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) == 0) {
    nodeFlagsField = &(control->base).nodeFlags;
    *nodeFlagsField = *nodeFlagsField | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  /* clears the caret phase and the blink frame counter (top byte) */
  control->editStateFlags = control->editStateFlags & (UI_STATE_FLAGS_MASK & ~UI_TEXT_EDIT_CARET_VISIBLE_PHASE);
  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)control);
  textCursor = control->textBuffer;
  control->cursorIndex = 0;
  control->selectionStart = 0;
  control->selectionEnd = 0;
  currentCodeUnit = *textCursor;
  while (currentCodeUnit != 0) {
    textCursor++;
    control->selectionEnd++;
    currentCodeUnit = *textCursor;
  }
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}

/* Primary button release on a text edit (nonRightRelease slot of the numeric, path and required text edit
   vtables): ends the pointer selection and plays the interaction sound when enabled.
*/
void UiTextEditControl_EndSelection
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
               UiTextEditControl *control)

{
  control->editStateFlags = control->editStateFlags & ~UI_TEXT_EDIT_POINTER_SELECTION_ACTIVE;
  if (((control->editStateFlags & UI_TEXT_EDIT_PLAY_INTERACTION_SOUND) != 0) &&
     (control->activationSound != nullptr)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
  }
  return;
}

/* Disables a text edit whose action id matches (suppressActionId slot of the numeric, path and required text
   edit vtables): suppresses it, takes the keyboard focus away from it and redraws. Children are not visited.
*/
void UiTextEditControl_SuppressIfActionId(UiActionId actionId,UiTextEditControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags | UI_NODE_SUPPRESSED;
    UiKeyboardFocus_ReleaseNode(&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

/* Enables a text edit whose action id matches (unsuppressActionId slot of the numeric, path and required text
   edit vtables): clears the suppression, gives it the keyboard focus if nothing has it and redraws.
*/
void UiTextEditControl_UnsuppressIfActionId(UiActionId actionId,UiTextEditControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags & ~UI_NODE_SUPPRESSED;
    UiKeyboardFocus_AcquireIfNone(&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

/* Per-frame tick of a text edit (tick slot of the numeric, path and required text edit vtables): while it
   has the keyboard focus, counts the blink frames in the top byte of editStateFlags down; when they run out
   the caret phase flips, the counter restarts at g_UiTextEditCaretBlinkPhaseStep and the edit is redrawn.
*/
void UiTextEditControl_TickCaretBlink(UiTextEditControl *control)

{
  int blinkPhaseIncrement;
  UiTextEditStateFlags *stateFlagsField;
  UiTextEditStateFlags previousStateFlags;

  if (control == (UiTextEditControl *)g_UiKeyboardFocusNode) {
    stateFlagsField = &control->editStateFlags;
    previousStateFlags = *stateFlagsField;
    *stateFlagsField = *stateFlagsField - UI_STATE_FRAME_COUNTER_UNIT;
    /* the subtraction borrowed: an unsigned compare, so the counter byte runs 0xFF..0 */
    if ((uint32_t)previousStateFlags < (uint32_t)UI_STATE_FRAME_COUNTER_UNIT) {
      blinkPhaseIncrement = g_UiTextEditCaretBlinkPhaseStep * UI_STATE_FRAME_COUNTER_UNIT;
      control->editStateFlags = control->editStateFlags ^ UI_TEXT_EDIT_CARET_VISIBLE_PHASE;
      control->editStateFlags = control->editStateFlags + blinkPhaseIncrement;
      UiNode_InvalidateRoot(&control->base);
    }
  }
  return;
}

/* Rewrites the text of a numeric text edit from currentValue: clears the 16-code-unit buffer, writes a '-'
   for a negative signed value, then the magnitude in decimal or upper-case hexadecimal, and updates the
   range validity. Called by UiNumericTextEditControl_RelocateAndRebuildText.
*/
void UiNumericTextControl_RebuildTextFromValue(UiNumericTextControl *control)

{
  uint32_t remainingValue;
  int8_t rotateShift;
  uint32_t highBitIndex;
  uint32_t hexDigitsLeft;
  uint32_t digitCodeUnit;
  uint16_t *outputCursor;
  
  outputCursor = control->textBuffer;
  remainingValue = control->currentValue;
  outputCursor[0] = 0;
  outputCursor[1] = 0;
  control->textBuffer[2] = 0;
  control->textBuffer[3] = 0;
  control->textBuffer[4] = 0;
  control->textBuffer[5] = 0;
  control->textBuffer[6] = 0;
  control->textBuffer[7] = 0;
  control->textBuffer[8] = 0;
  control->textBuffer[9] = 0;
  control->textBuffer[10] = 0;
  control->textBuffer[11] = 0;
  control->textBuffer[12] = 0;
  control->textBuffer[13] = 0;
  control->textBuffer[14] = 0;
  control->textBuffer[15] = 0;
  if (((control->editStateFlags & UI_NUMERIC_TEXT_SIGNED_VALUE) != 0) && ((int)remainingValue < 0)) {
    *outputCursor = '-';
    remainingValue = 0 - remainingValue;
    outputCursor = control->textBuffer + 1;
  }
  if ((control->editStateFlags & UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT) == 0) {
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,remainingValue,outputCursor);
  }
  else {
    highBitIndex = 31;
    if (remainingValue != 0) {
      while (remainingValue >> highBitIndex == 0) {
        highBitIndex--;
      }
    }
    if (remainingValue != 0) {
      /* NOTE: faithful to the original (highest set bit, masked to a multiple of 4, rotate right, digit
         count = masked bit / 4): the leading nonzero hex digit is dropped, and a value below 0x10 gives a
         digit count of 0, which the count-down loop wraps (runaway write). */
      rotateShift = (int8_t)(highBitIndex & UI_NUMERIC_TEXT_HEX_DIGIT_BIT_MASK);
      hexDigitsLeft = (highBitIndex & UI_NUMERIC_TEXT_HEX_DIGIT_BIT_MASK) >> 2;
      remainingValue = remainingValue >> rotateShift | remainingValue << (32 - rotateShift);
      do {
        digitCodeUnit = (remainingValue >> 28) + '0';
        if ('9' < digitCodeUnit) {
          digitCodeUnit = digitCodeUnit + ('A' - '9' - 1);
        }
        *outputCursor = (uint16_t)digitCodeUnit;
        hexDigitsLeft--;
        remainingValue = remainingValue << 4;
        outputCursor++;
      } while (hexDigitsLeft != 0);
    }
    else {
      *outputCursor = '0';
    }
  }
  UiNumericTextControl_UpdateRangeValidity(control);
  return;
}

/* Digit value of a hexadecimal digit '0'-'9', 'A'-'F' or 'a'-'f'; false for any other code unit. */
static Bool8 UiNumericTextControl_TryHexDigitValue(uint32_t codeUnit,uint32_t *digitValue)

{
  uint32_t value;

  if (codeUnit < '0') {
    return false;
  }
  value = codeUnit - '0';
  if (9 < value) {
    value = codeUnit - ('A' - 10);
    if (value < 10) {
      return false;
    }
    if (15 < value) {
      value = codeUnit - ('a' - 10);
      if ((value < 10) || (15 < value)) {
        return false;
      }
    }
  }
  *digitValue = value;
  return true;
}

/* Parses the text of a numeric text edit (optional '-', then decimal or hexadecimal digits) into
   currentValue, queues the action unless the control acts on Enter only, and updates the range validity.
   Empty text, a bad digit or a '-' on an unsigned control only clears UI_NUMERIC_TEXT_VALUE_VALID (the value
   stays). Called after every handled key by UiNumericTextEditControl_HandleKeyboardAndCommit.
*/
void UiNumericTextControl_ParseAndCommitValue(UiNumericTextControl *control)

{
  uint32_t parsedValue;
  uint32_t codeUnit;
  uint32_t digitValue;
  int sign;
  uint16_t *textCursor;

  textCursor = control->textBuffer;
  sign = 1;
  if (*textCursor == 0) {
    /* Empty text: invalid. */
    control->editStateFlags = control->editStateFlags & ~UI_NUMERIC_TEXT_VALUE_VALID;
    return;
  }
  if (*textCursor == '-') {
    sign = -1;
    textCursor = control->textBuffer + 1;
  }
  parsedValue = 0;
  if ((control->editStateFlags & UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT) == 0) {
    for (; *textCursor != 0; textCursor++) {
      codeUnit = (uint32_t)*textCursor;
      if ((codeUnit < '0') || (9 < codeUnit - '0')) {
        control->editStateFlags = control->editStateFlags & ~UI_NUMERIC_TEXT_VALUE_VALID;
        return;
      }
      parsedValue = parsedValue * 10 + (codeUnit - '0');
    }
  }
  else {
    for (; *textCursor != 0; textCursor++) {
      if (!UiNumericTextControl_TryHexDigitValue((uint32_t)*textCursor,&digitValue)) {
        control->editStateFlags = control->editStateFlags & ~UI_NUMERIC_TEXT_VALUE_VALID;
        return;
      }
      parsedValue = parsedValue << 4 | digitValue & 0xf;
    }
  }
  if (sign < 0) {
    parsedValue = 0 - parsedValue;
    if ((control->editStateFlags & UI_NUMERIC_TEXT_SIGNED_VALUE) == 0) {
      /* A negative value for an unsigned control: invalid. */
      control->editStateFlags = control->editStateFlags & ~UI_NUMERIC_TEXT_VALUE_VALID;
      return;
    }
  }
  control->currentValue = parsedValue;
  if ((control->editStateFlags & UI_NUMERIC_TEXT_ACTION_ON_ENTER_ONLY) == 0) {
    UiActionQueue_Enqueue(control->actionId,control);
  }
  UiNumericTextControl_UpdateRangeValidity(control);
}

/* Sets UI_NUMERIC_TEXT_VALUE_VALID exactly when currentValue lies within minimumValue..maximumValue,
   compared signed for UI_NUMERIC_TEXT_SIGNED_VALUE and unsigned otherwise. Called by
   UiNumericTextControl_RebuildTextFromValue and UiNumericTextControl_ParseAndCommitValue.
*/
void UiNumericTextControl_UpdateRangeValidity(UiNumericTextControl *control)

{
  uint32_t currentNumericValue;
  Bool8 outOfRange;

  currentNumericValue = control->currentValue;
  if ((control->editStateFlags & UI_NUMERIC_TEXT_SIGNED_VALUE) == 0) {
    outOfRange = (currentNumericValue < (uint32_t)control->minimumValue) ||
                 ((uint32_t)control->maximumValue < currentNumericValue);
  }
  else {
    outOfRange = ((int)currentNumericValue < control->minimumValue) ||
                 (control->maximumValue < (int)currentNumericValue);
  }
  if (outOfRange) {
    control->editStateFlags = control->editStateFlags & ~UI_NUMERIC_TEXT_VALUE_VALID;
    return;
  }
  control->editStateFlags = control->editStateFlags | UI_NUMERIC_TEXT_VALUE_VALID;
  return;
}

/* Width in pixels of the first prefixLength code units of a text edit's text (fewer if the text ends
   before), measured glyph by glyph in g_UiTextEditActiveTextStyle. Used for the selection, caret and scroll
   positions by the text edit draw and layout functions.
*/
UiPixelCoordinate UiTextEditControl_MeasurePrefixWidth(UiTextCodeUnitCount prefixLength,UiTextEditControl *control)

{
  int accumulatedWidth;
  uint32_t glyphIndex;
  uint32_t glyphWidth;

  glyphIndex = 0;
  accumulatedWidth = 0;
  if (prefixLength != 0) {
    do {
      if (control->textBuffer[glyphIndex] == 0) {
        return accumulatedWidth;
      }
      glyphWidth = FontGlyph_GetLogicalSizeForStyle
                        (g_UiTextEditActiveTextStyle,(uint32_t)control->textBuffer[glyphIndex],nullptr);
      glyphIndex++;
      accumulatedWidth = accumulatedWidth + glyphWidth;
    } while (glyphIndex < prefixLength);
  }
  return accumulatedWidth;
}

/* Cursor index for a pointer x in a text edit: the index of the glyph under the pointer (the first one whose
   right edge reaches it), or the text length past the end. Accounts for the horizontal scroll and, with
   UI_TEXT_EDIT_DRAW_FRAMED_CHROME, the frame width. Used by the pointer press and drag handlers.
*/
UiTextCodeUnitCount UiTextEditControl_FindCursorIndexAtX(UiPixelCoordinate pointerX,UiTextEditControl *control)

{
  int nextTextIndex;
  int targetOffsetX;
  int currentTextIndex;
  int measuredPrefixWidthPixels;
  GraphicsTextureLogicalSize decorationSize;
  uint32_t glyphWidth;

  targetOffsetX = (pointerX - (control->base).left) + control->horizontalScrollPixels;
  if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_FRAMED_CHROME) != 0) {
    decorationSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME,
                                                           g_UiWindowTextureSource);
    targetOffsetX = targetOffsetX - decorationSize.logicalWidthPixels;
  }
  measuredPrefixWidthPixels = 0;
  nextTextIndex = 0;
  do {
    currentTextIndex = nextTextIndex;
    if (control->textBuffer[currentTextIndex] == 0) {
      return currentTextIndex;
    }
    glyphWidth = FontGlyph_GetLogicalSizeForStyle
                      (g_UiTextEditActiveTextStyle,(uint32_t)control->textBuffer[currentTextIndex],nullptr);
    measuredPrefixWidthPixels = measuredPrefixWidthPixels + glyphWidth;
    nextTextIndex = currentTextIndex + 1;
  } while (measuredPrefixWidthPixels < targetOffsetX);
  return currentTextIndex;
}

/* Sets UI_TEXT_EDIT_VALUE_VALID of a DOS path edit from g_FileSystemValidateDos83Path, which gets the
   control's UI_PATH_TEXT_ALLOW_WILDCARDS and UI_PATH_TEXT_NAME_ONLY bits shifted down to bits 0-1. Called by
   the path edit's keyboard handler and relocation.
*/
void UiPathTextControl_UpdateDos83Validity(UiPathTextEditControl *control)

{
  Bool8 validatorRejected;

  validatorRejected = g_FileSystemValidateDos83Path
                          (control->editStateFlags >> 1 & 3,(uint8_t *)control->pathBuffer);
  if (validatorRejected) {
    control->editStateFlags = control->editStateFlags & ~UI_TEXT_EDIT_VALUE_VALID;
  }
  else {
    control->editStateFlags = control->editStateFlags | UI_TEXT_EDIT_VALUE_VALID;
  }
  return;
}

/* A text control's value is valid (UI_TEXT_EDIT_VALUE_VALID) exactly when its text is not empty.
*/
void UiTextControl_UpdateNonEmptyValidity(UiTextEditControl *control)

{
  if (control->textBuffer[0] == 0) {
    control->editStateFlags = control->editStateFlags & ~UI_TEXT_EDIT_VALUE_VALID;
  }
  else {
    control->editStateFlags = control->editStateFlags | UI_TEXT_EDIT_VALUE_VALID;
  }
  return;
}

/* Layout of a text edit (layout slot of the numeric, path and required text edit vtables; also called after
   every edit): refreshes the layout size and scrolls horizontally just enough to keep the glyphs before and
   after the cursor visible, or not at all while the whole text (plus caret and frame) fits.
*/
void UiTextEditControl_RecomputeLayoutAndClampScroll(UiTextEditControl *control)

{
  UiTextCodeUnitCount prefixLength;
  UiPixelOffset cursorWidth;
  UiPixelOffset minScrollOffset;
  UiPixelCoordinate fullTextWidth;
  int cursorOverflow;
  int contentWidth;
  UiPixelOffset maxScrollOffset;
  uint32_t glyphWidth;
  GraphicsTextureLogicalSize decorationSize;

  (control->base).layoutWidth = (control->base).right - (control->base).left;
  (control->base).layoutHeight = (control->base).bottom - (control->base).top;
  prefixLength = control->cursorIndex;
  cursorWidth = UiTextEditControl_MeasurePrefixWidth(prefixLength,control);
  maxScrollOffset = cursorWidth;
  if (prefixLength != 0) {
    glyphWidth = FontGlyph_GetLogicalSizeActiveFont((uint32_t)control->textBuffer[prefixLength - 1],nullptr);
    maxScrollOffset = cursorWidth - glyphWidth;
  }
  cursorOverflow = cursorWidth - (control->base).layoutWidth;
  if (control->textBuffer[prefixLength] != 0) {
    glyphWidth = FontGlyph_GetLogicalSizeActiveFont((uint32_t)control->textBuffer[prefixLength],nullptr);
    cursorOverflow = cursorOverflow + glyphWidth;
  }
  /* NOTE: as in the original (a fixed count of 16), only the first 16 code units (the numeric text buffer)
     count, also for the longer path and required text edits. */
  fullTextWidth = UiTextEditControl_MeasurePrefixWidth(UI_NUMERIC_TEXT_BUFFER_UNITS,control);
  decorationSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_CARET_OVERWRITE,g_UiWindowTextureSource);
  minScrollOffset = cursorOverflow + decorationSize.logicalWidthPixels;
  contentWidth = fullTextWidth + decorationSize.logicalWidthPixels;
  if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_FRAMED_CHROME) != 0) {
    decorationSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME,
                                                           g_UiWindowTextureSource);
    contentWidth = contentWidth + decorationSize.logicalWidthPixels * 2;
    minScrollOffset = minScrollOffset + decorationSize.logicalWidthPixels * 2;
  }
  if ((control->base).layoutWidth < contentWidth) {
    if ((int)maxScrollOffset < (int)control->horizontalScrollPixels) {
      control->horizontalScrollPixels = maxScrollOffset;
    }
    else if ((int)control->horizontalScrollPixels < (int)minScrollOffset) {
      control->horizontalScrollPixels = minScrollOffset;
    }
  }
  else {
    control->horizontalScrollPixels = 0;
  }
}

UiNodeVtable g_UiNumericTextEditControlVtable = {
        .relocate = THANDOR_FN(UiNumericTextEditControl_RelocateAndRebuildText),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiTextEditControl_DrawTextSelectionAndCaret),
        .layout = THANDOR_FN(UiTextEditControl_RecomputeLayoutAndClampScroll),
        .nonRightPress = THANDOR_FN(UiTextEditControl_BeginSelectionAtPointer),
        .nonRightRelease = THANDOR_FN(UiTextEditControl_EndSelection),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiTextEditControl_UpdateSelectionFromPointer),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
        .keyboardEvent = THANDOR_FN(UiNumericTextEditControl_HandleKeyboardAndCommit),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiTextEditControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiTextEditControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiTextEditControl_TickCaretBlink),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent)};

UiNodeVtable g_UiPathTextEditControlVtable = {
        .relocate = THANDOR_FN(UiPathTextEditControl_RelocateAndValidateDos83),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiTextEditControl_DrawTextSelectionAndCaret),
        .layout = THANDOR_FN(UiTextEditControl_RecomputeLayoutAndClampScroll),
        .nonRightPress = THANDOR_FN(UiTextEditControl_BeginSelectionAtPointer),
        .nonRightRelease = THANDOR_FN(UiTextEditControl_EndSelection),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiTextEditControl_UpdateSelectionFromPointer),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
        .keyboardEvent = THANDOR_FN(UiPathTextEditControl_HandleKeyboardAndValidate),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiTextEditControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiTextEditControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiTextEditControl_TickCaretBlink),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent)};

UiNodeVtable g_UiRequiredTextEditControlVtable = {
        .relocate = THANDOR_FN(UiRequiredTextEditControl_RelocateAndValidateNonEmpty),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiTextEditControl_DrawTextSelectionAndCaret),
        .layout = THANDOR_FN(UiTextEditControl_RecomputeLayoutAndClampScroll),
        .nonRightPress = THANDOR_FN(UiTextEditControl_BeginSelectionAtPointer),
        .nonRightRelease = THANDOR_FN(UiTextEditControl_EndSelection),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiTextEditControl_UpdateSelectionFromPointer),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
        .keyboardEvent = THANDOR_FN(UiRequiredTextEditControl_HandleKeyboardAndValidate),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiTextEditControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiTextEditControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiTextEditControl_TickCaretBlink),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent)};
