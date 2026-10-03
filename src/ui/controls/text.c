/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/text.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/text.h>
#include <thandor/thandor.h>

/* Module data. */

UiTooltipState g_UiTooltipState = {.countdownFrames = 8};

AudioMixerGainQ15 g_UiSoundGainQ15 = 32768;

/* UiFrameDelayFrames, 8: frames of the activation pulse after Enter on a list/text list before its action is queued (src/ui/controls/lists.c, text.c). */
const UiFrameDelayFrames g_UiListActivationPulseFrames = 8;

const uint32_t g_UiTextStyleNormal = 0;

const int32_t g_UiWindowFrameInset = 2;

const uint32_t g_UiListTextStyle = 0;

static void *const g_Utf16StringCompareAsciiCaseInsensitiveFlags = (void *)Utf16String_CompareAsciiCaseInsensitiveFlags;

uint16_t g_GraphicsAdapterFormatScratch0Utf16[16] = {0};

uint16_t g_GraphicsAdapterFormatScratch1Utf16[16] = {0};

static const UiFrameDelayFrames g_UiTooltipDelayFrames = 12;

static const uint32_t g_UiTooltipTextStyle = 0;

/* UiPackedTextStyle, 0x10000 (palette byte 1): text style of the selected/highlighted row or item (src/ui/controls/text.c). */
static const UiPackedTextStyle g_UiTextStyleSelected = 0x10000;

/* UiPackedTextStyle, 0x20000 (palette byte 2): text style of disabled items (src/ui/controls/text.c). */
static const UiPackedTextStyle g_UiTextStyleDisabled = 0x20000;

static const uint32_t g_UiTextStyleAlternate = 0;

static const uint32_t g_UiTextEditActiveTextStyle = 0;

static const uint32_t g_UiTextEditInactiveTextStyle = 0;

static const uint32_t g_UiTextEditDisabledTextStyle = 0;

/* UiFrameDelayFrames, 8: frames per caret blink phase of a focused text edit, reloaded into the counter byte of editStateFlags (src/ui/controls/text.c). */
static const UiFrameDelayFrames g_UiTextEditCaretBlinkPhaseStep = 8;

/* 1 KiB expansion scratch of UiPointerList_CompareExpandedText */
static uint16_t g_UiPointerListExpandedLeftTextUtf16[512] = {0};

/* 1 KiB expansion scratch of UiPointerList_CompareExpandedText */
static uint16_t g_UiPointerListExpandedRightTextUtf16[512] = {0};

static uint16_t g_UiNumericPairFirstValueScratchUtf16[16] = {0};

static uint16_t g_UiNumericPairSecondValueScratchUtf16[16] = {0};

/* Implementation ownership: ui/controls/text. */

/* Per-frame tooltip delay: while the pointer rests on an enabled control (no button held), counts the
   delay down and prepares the tooltip text when it expires. While a node has captured the pointer or the
   hovered control is disabled, re-evaluates the hover target at the last pointer position instead.
*/
void UiTooltip_TickCountdown(void)

{
  if ((g_UiPointerCaptureTarget == UI_NODE_NONE) &&
     ((g_UiTooltipState.targetNode == NULL ||
      (((g_UiTooltipState.targetNode)->nodeFlags & UI_NODE_SUPPRESSED) == 0)))) {
    if (g_UiTooltipState.countdownFrames != 0) {
      g_UiTooltipState.countdownFrames--;
      if (g_UiTooltipState.countdownFrames == 0) {
        UiTooltip_PrepareTargetText(g_UiTooltipState.targetNode);
      }
    }
    return;
  }
  UiTooltip_UpdateHoverTarget(g_UiTooltipState.pointerY,g_UiTooltipState.pointerX);
}


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
           (control->activationSound != NULL)) {
          g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
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
     (control->activationSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
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
     (control->activationSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
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
     (control->activationSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
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


/* Draws a graphics-adapter option button (drawClipped slot of g_UiGraphicsAdapterTextButtonVtable): patches
   rich-text payloads 0 and 1 of its text and draws it as a text button. The values are the two dwords stored
   just before the node (control[-1].packedTextStyle at -8 is the adapter index or first number,
   control[-1].textResourceId at -0xC the second number). State bit 0x80: only the first number; bit 0x800:
   the adapter's driver description and as device name text 0x111 (every adapter is a software renderer
   device; the original showed a hardware renderer device's own name instead); neither: both numbers.
*/
void UiGraphicsAdapterTextButton_DrawFormattedAdapterText
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextButtonControl *control)

{
  UiPackedTextStyle adapterIndex;
  uint16_t *stream;
  uint16_t *resolvedText;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve(control->textResourceId);
    stream = resolvedText;
    if (((control->selectable).stateFlags & UI_ADAPTER_TEXT_BUTTON_SINGLE_NUMBER) != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].packedTextStyle,
                 g_GraphicsAdapterFormatScratch0Utf16);
      RichTextCommandStream_PatchPayloadBySelector(0,g_GraphicsAdapterFormatScratch0Utf16,stream);
      UiTextButtonControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,control);
      return;
    }
    if (((control->selectable).stateFlags & UI_ADAPTER_TEXT_BUTTON_ADAPTER_NAME) == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].packedTextStyle,
                 g_GraphicsAdapterFormatScratch0Utf16);
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].textResourceId,
                 g_GraphicsAdapterFormatScratch1Utf16);
      RichTextCommandStream_PatchPayloadBySelector(0,g_GraphicsAdapterFormatScratch0Utf16,stream);
      RichTextCommandStream_PatchPayloadBySelector(1,g_GraphicsAdapterFormatScratch1Utf16,stream);
      UiTextButtonControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,control);
      return;
    }
    adapterIndex = control[-1].packedTextStyle;
    RichTextCommandStream_PatchPayloadBySelector
              (0,g_GraphicsAdapters[adapterIndex].driverDescriptionUtf16,stream);
    resolvedText = TextResource_Resolve(TEXT_ID_PRIMARY_DISPLAY_ADAPTER);
    RichTextCommandStream_PatchPayloadBySelector(1,resolvedText,stream);
    UiTextButtonControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,control);
  }
  return;
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
     (control->activationSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
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


/* Tail of UiPointerList_SortByExpandedTextFieldAscending: selects the row that now holds selectedRecord (the first row if it
   is gone) and scrolls that row into view. */
static void UiPointerList_ReselectRecordAfterSort(void *selectedRecord,UiPointerListControl *control)

{
  void **rowSlotCursor;
  UiListRowCount remainingRows;
  int selectedRowTop;

  rowSlotCursor = control->rowSlots;
  remainingRows = control->rowCount;
  selectedRowTop = 0;
  do {
    if (selectedRecord == *rowSlotCursor) break;
    selectedRowTop = selectedRowTop + control->rowHeight;
    rowSlotCursor++;
    remainingRows--;
  } while (remainingRows != 0);
  if (remainingRows == 0) {
    /* Not found: select the first row (the row top stays past the last row, as in the original). */
    rowSlotCursor = control->rowSlots;
  }
  control->selectedRowSlot = rowSlotCursor;
  UiScrollableControl_ClampOffsetsToViewport
            (selectedRowTop + 1 + control->rowHeight,(control->base).rightOffset,selectedRowTop,0,
             (UiScrollableControl *)(control->base).parent);
}


/* Sorts the rows of a pointer list in ascending order of the rich text found fieldOffset bytes into each row
   record (expanded, ASCII case-insensitive), with an exchange sort that moves the smallest remaining row to
   the front in each pass. The previously selected record stays selected and is scrolled into view. Called
   by the scenario catalogue (assets/scenario/catalog.c, field offset 0x74).
*/
void UiPointerList_SortByExpandedTextFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *swappedRecord;
  int lastRowIndex;
  int comparisonsLeft;
  int remainingPasses;
  void **passAnchorSlot;
  void **rowSlotCursor;
  int textOrder;
  void *selectedRecord;

  if (control->rowSlots != NULL) {
    lastRowIndex = control->rowCount - 1;
    if ((lastRowIndex != 0) && (-1 < lastRowIndex)) {
      selectedRecord = *control->selectedRowSlot;
      passAnchorSlot = control->rowSlots;
      for (remainingPasses = lastRowIndex; remainingPasses != 0; remainingPasses--) {
        rowSlotCursor = passAnchorSlot;
        for (comparisonsLeft = remainingPasses; comparisonsLeft != 0; comparisonsLeft--) {
          rowSlotCursor++;
          textOrder = UiPointerList_CompareExpandedText
                            ((uint16_t *)((uint8_t *)*rowSlotCursor + fieldOffset),
                             (uint16_t *)((uint8_t *)*passAnchorSlot + fieldOffset));
          if (textOrder >= 0) {
            LOCK();
            swappedRecord = *rowSlotCursor;
            *rowSlotCursor = *passAnchorSlot;
            UNLOCK();
            *passAnchorSlot = swappedRecord;
          }
        }
        passAnchorSlot++;
      }
      UiPointerList_ReselectRecordAfterSort(selectedRecord,control);
    }
  }
}


/* Draws a text button showing two numbers (drawClipped slot of g_UiNumericPairTextButtonVtable, e.g. a
   display resolution): formats firstValue and secondValue as decimal into rich-text payloads 0 and 1 of its
   text, then draws it as a text button.
*/
void UiNumericPairTextButton_DrawFormattedValues
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNumericPairTextButton *control)

{
  uint16_t *resolvedText;

  if (((control->base).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve((control->base).textResourceId);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->firstValue,
               g_UiNumericPairFirstValueScratchUtf16);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->secondValue,
               g_UiNumericPairSecondValueScratchUtf16);
    RichTextCommandStream_PatchPayloadBySelector(0,g_UiNumericPairFirstValueScratchUtf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(1,g_UiNumericPairSecondValueScratchUtf16,resolvedText);
    UiTextButtonControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,&control->base);
  }
  return;
}


/* Draws a text button with two text payloads (drawClipped slot of g_UiPayloadPairTextButtonVtable): patches
   firstPayload and secondPayload into rich-text payloads 0 and 1 of its text, then draws it as a text
   button.
*/
void UiPayloadPairTextButton_DrawFormattedPayloads
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPayloadPairTextButton *control)

{
  uint16_t *resolvedText;

  if (((control->base).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve((control->base).textResourceId);
    RichTextCommandStream_PatchPayloadBySelector(0,control->firstPayload,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(1,control->secondPayload,resolvedText);
    UiTextButtonControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,&control->base);
  }
  return;
}


/* Draws the tooltip once its delay has expired: a one-line box (win.gfx left cap 0xBC, tiled middle 0xBD,
   right cap 0xBE) centred above the hovered control, kept inside its root window, and moved below the
   control when there is no room above. Drawn last in the frame, over everything. With no root open at all,
   the whole screen is darkened (ARGB 0x80000000: black at half alpha).
*/
void UiTooltip_Draw(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
              UiPixelCoordinate clipLeft)

{
  int targetRight;
  UiNodeBase *tooltipTarget;
  UiNodeBase *rootNode;
  uint16_t *commandStream;
  uint32_t edgeTileWidth;
  int32_t frameLeft;
  int32_t frameRight;
  int frameWidth;
  int targetLeft;
  int middleEnd;
  int tileX;
  int frameTop;
  Bool8 framebufferUnavailable;
  RichTextExtent textExtent;
  uint16_t *resolvedText;
  GraphicsTextureLogicalSize tileSize;
  
  tooltipTarget = g_UiTooltipState.targetNode;
  if ((g_UiTooltipState.targetNode != NULL) && (g_UiTooltipState.countdownFrames == 0)) {
    rootNode = UiNode_GetRoot(g_UiTooltipState.targetNode);
    /* the dword just before the node: a text resource id, or with UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16 the
       text itself */
    commandStream = (uint16_t *)tooltipTarget[-1].nodeFlags;
    if ((tooltipTarget->nodeFlags & UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)commandStream);
      commandStream = resolvedText;
    }
    textExtent = RichTextCommandStream_MeasureLine(g_UiTooltipTextStyle,commandStream);
    tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TOOLTIP_LEFT,g_UiWindowTextureSource);
    edgeTileWidth = tileSize.logicalWidthPixels;
    frameWidth = textExtent.widthPixels + edgeTileWidth * 2;
    targetLeft = tooltipTarget->left;
    frameTop = tooltipTarget->top - tileSize.logicalHeightPixels;
    targetRight = tooltipTarget->right;
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      frameLeft = ((targetLeft + targetRight) - frameWidth) >> 1;
      if (rootNode == UI_NODE_NONE) {
        rootNode = tooltipTarget;
      }
      frameRight = frameWidth + frameLeft;
      if (frameLeft < rootNode->left) {
        frameRight = frameRight - (frameLeft - rootNode->left);
        frameLeft = rootNode->left;
      }
      if (rootNode->right < frameRight) {
        frameLeft = frameLeft - (frameRight - rootNode->right);
        frameRight = rootNode->right;
      }
      if (frameTop < rootNode->top) {
        frameTop = frameTop + tooltipTarget->layoutHeight + tileSize.logicalHeightPixels;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,frameTop,frameLeft,UI_WINDOW_SUBRESOURCE_TOOLTIP_LEFT,
                 g_UiWindowTextureSource,
                 g_FramebufferAccess);
      middleEnd = frameRight - edgeTileWidth;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,frameTop,middleEnd,
                 UI_WINDOW_SUBRESOURCE_TOOLTIP_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      if (clipRight < middleEnd) {
        middleEnd = clipRight;
      }
      tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TOOLTIP_MIDDLE,g_UiWindowTextureSource);
      tileX = edgeTileWidth + frameLeft;
      do {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,middleEnd,clipTop,clipLeft,frameTop,tileX,
                   UI_WINDOW_SUBRESOURCE_TOOLTIP_MIDDLE,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        tileX = tileX + tileSize.logicalWidthPixels;
      } while (tileX < middleEnd);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiTooltipTextStyle,commandStream,frameTop + 3, /* text inset */
                 edgeTileWidth + frameLeft);
      g_GraphicsFramebufferEndAccess();
    }
  }
  if (g_UiRootNode == UI_ROOT_STACK_END) {
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      g_GraphicsFramebufferFillRectArgb
                (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0,
                 UI_TOOLTIP_NO_ROOT_DIM_ARGB,g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
    }
  }
  return;
}


/* Closes the UI roots of an ending session: pops the front root until the stack is empty; a root that vetoes
   its close stops the loop and is reported by returning true. The original also stops at the dword after
   g_UiRootNode (g_UiWindowTextureSource), which is never a root, so in practice this pops every
   root.
*/
Bool8 UiRootStack_PopUntilWindowTextureBoundary(void)

{
  Bool8 popStopped;

  while (((GraphicsTextureSourceAsset *)g_UiRootNode != g_UiWindowTextureSource &&
         (g_UiRootNode != UI_ROOT_STACK_END))) {
    popStopped = UiRootStack_Pop(g_UiRootNode);
    if (popStopped) {
      return true;
    }
  }
  return false;
}


/* Relocation of a loaded framed text button (relocate slot of g_UiFramedTextButtonControlVtable): an inset-framed
   button (UI_BUTTON_FRAME_INSET) grows its layout offsets by g_UiWindowFrameInset on every side, so the frame
   lies outside the authored box; then the children are relocated.
*/
void UiFramedTextButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiFramedTextButtonControl *control)

{
  int32_t *bottomOffsetField;
  int32_t *frameEdgeOffsetField;
  int32_t *edgeOffsetField;
  int32_t *trailingEdgeOffsetField;
  int frameInset;
  
  frameInset = g_UiWindowFrameInset;
  if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) != 0) {
    edgeOffsetField = &(control->selectable).base.leftOffset;
    *edgeOffsetField = *edgeOffsetField - g_UiWindowFrameInset;
    frameEdgeOffsetField = &(control->selectable).base.topOffset;
    *frameEdgeOffsetField = *frameEdgeOffsetField - frameInset;
    trailingEdgeOffsetField = &(control->selectable).base.rightOffset;
    *trailingEdgeOffsetField = *trailingEdgeOffsetField + frameInset;
    bottomOffsetField = &(control->selectable).base.bottomOffset;
    *bottomOffsetField = *bottomOffsetField + frameInset;
  }
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  return;
}


/* Draws a framed text button (drawClipped slot of g_UiFramedTextButtonControlVtable): the normal, selected or disabled
   win.gfx frame (plain or inset), its text centred in the state's style, with the focus mark and its shadow
   behind the text while it has keyboard focus, then the children unless the button is suppressed.
*/
void UiFramedTextButtonControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiFramedTextButtonControl *control)

{
  uint32_t cornerWidth;
  uint16_t *commandStream;
  uint32_t framePiece;
  uint32_t textWidth;
  uint32_t cornerHeight;
  int rightCornerX;
  int bottomCornerY;
  int focusMarkTop;
  int focusTileX;
  int focusMarkRightX;
  uint32_t textStyle;
  Bool8 framebufferUnavailable;
  Bool8 drawFrame;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize cornerTileSize;
  GraphicsTextureLogicalSize rightCapSize;
  GraphicsTextureLogicalSize leftCapSize;
  GraphicsTextureLogicalSize middleTileSize;
  int textX;
  int textY;

  framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
  if (framebufferUnavailable) {
    drawFrame = false;
  }
  else if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    drawFrame = true;
    if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
      framePiece = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME;
    }
    else {
      framePiece = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      framePiece = framePiece + UI_WINDOW_FRAME_PIECE_COUNT;
    }
  }
  else {
    /* Suppressed: the disabled frame, unless UI_BUTTON_HIDDEN_WHILE_SUPPRESSED hides it. */
    drawFrame = ((control->selectable).stateFlags & UI_BUTTON_HIDDEN_WHILE_SUPPRESSED) == 0;
    if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
      framePiece = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME_DISABLED;
    }
    else {
      framePiece = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME_DISABLED;
    }
  }
  if (drawFrame) {
    cornerTileSize = g_GraphicsTextureSourceGetLogicalSize(framePiece,g_UiWindowTextureSource);
    cornerHeight = cornerTileSize.logicalHeightPixels;
    cornerWidth = cornerTileSize.logicalWidthPixels;
    rightCornerX = (control->selectable).base.layoutWidth - cornerWidth;
    bottomCornerY = (control->selectable).base.layoutHeight - cornerHeight;
    if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) ||
       (((control->selectable).stateFlags & UI_BUTTON_NO_FRAME_WHILE_SUPPRESSED) == 0)) {
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
                 (control->selectable).base.left,framePiece,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
                 rightCornerX + (control->selectable).base.left,
                 framePiece + UI_WINDOW_FRAME_TOP_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomCornerY + (control->selectable).base.top,
                 (control->selectable).base.left,framePiece + UI_WINDOW_FRAME_BOTTOM_LEFT,
                 g_UiWindowTextureSource,
                 g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomCornerY + (control->selectable).base.top,
                 rightCornerX + (control->selectable).base.left,
                 framePiece + UI_WINDOW_FRAME_BOTTOM_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_TOP,rightCornerX,
                 0,cornerWidth,control);
      UiWindow_BlitTiledVerticalEdge
                (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_LEFT,
                 bottomCornerY,cornerHeight,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_RIGHT,
                 bottomCornerY,cornerHeight,rightCornerX,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_BOTTOM,
                 rightCornerX,bottomCornerY,cornerWidth,control);
    }
    commandStream = TextResource_Resolve(control->textResourceId);
    textExtent = RichTextCommandStream_MeasureLine(control->packedTextStyle,commandStream);
    textWidth = textExtent.widthPixels;
    /* centred offsets inside the button */
    textX = (int)((control->selectable).base.layoutWidth - textWidth) >> 1;
    textY = (int)((control->selectable).base.layoutHeight - textExtent.heightPixels) >> 1;
    textStyle = g_UiTextStyleDisabled;
    if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
      textStyle = g_UiTextStyleNormal;
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
        textStyle = g_UiTextStyleSelected;
      }
    }
    /* packedTextStyle may override the font byte (bits 24-31) and the palette byte (bits 16-23) */
    if (((control->selectable).stateFlags & UI_BUTTON_OWN_STYLE_FONT) == 0) {
      control->packedTextStyle = control->packedTextStyle & UI_TEXT_STYLE_PALETTE_BYTE;
    }
    else {
      textStyle = textStyle & ~UI_TEXT_STYLE_FONT_BYTE;
    }
    if (((control->selectable).stateFlags & UI_BUTTON_OWN_STYLE_PALETTE) == 0) {
      control->packedTextStyle = control->packedTextStyle & UI_TEXT_STYLE_FONT_BYTE;
    }
    else {
      textStyle = textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE;
    }
    textStyle = textStyle | control->packedTextStyle;
    if ((((control->selectable).base.nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) ||
       (((control->selectable).stateFlags & UI_BUTTON_NO_FOCUS_MARK) != 0)) {
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,
                 textY + (control->selectable).base.top,
                 textX + (control->selectable).base.left);
    }
    else {
      textX = textX + (control->selectable).base.left;
      textY = textY + (control->selectable).base.top;
      /* focus mark shadow: left cap, right cap, then the middle tiles up to the right cap */
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,textY,textX - 2,
                 TEXT_SHADOW_COLOR_ARGB,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      rightCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                           g_UiWindowTextureSource);
      leftCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                          g_UiWindowTextureSource);
      focusTileX = textX - 2 + leftCapSize.logicalWidthPixels;
      middleTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                             g_UiWindowTextureSource);
      focusMarkRightX = ((textX + 4) - rightCapSize.logicalWidthPixels) + textWidth;
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,textY,focusMarkRightX,TEXT_SHADOW_COLOR_ARGB,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      if (clipRight < focusMarkRightX) {
        focusMarkRightX = clipRight;
      }
      do {
        g_GraphicsTextureSourceBlitModulatedSourceAlpha
                  (clipBottom,focusMarkRightX,clipTop,clipLeft,textY,focusTileX,
                   TEXT_SHADOW_COLOR_ARGB,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        focusTileX = focusTileX + middleTileSize.logicalWidthPixels;
      } while (focusTileX < focusMarkRightX);
      focusMarkTop = textY - 1;
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,textY,textX);
      /* the focus mark itself, one pixel up and to the left of its shadow */
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,focusMarkTop,textX - 3,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      rightCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                           g_UiWindowTextureSource);
      leftCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                          g_UiWindowTextureSource);
      focusTileX = textX - 3 + leftCapSize.logicalWidthPixels;
      middleTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                             g_UiWindowTextureSource);
      focusMarkRightX = ((textX + 3) - rightCapSize.logicalWidthPixels) + textWidth;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,focusMarkTop,focusMarkRightX,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      if (clipRight < focusMarkRightX) {
        focusMarkRightX = clipRight;
      }
      do {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,focusMarkRightX,clipTop,clipLeft,focusMarkTop,focusTileX,
                   UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        focusTileX = focusTileX + middleTileSize.logicalWidthPixels;
      } while (focusTileX < focusMarkRightX);
    }
  }
  if (!framebufferUnavailable) {
    g_GraphicsFramebufferEndAccess();
  }
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    UiContainer_DrawIntersectingChildren
              (clipBottom,clipRight,clipTop,clipLeft,(UiNodeBase *)control);
  }
  return;
}


/* Primary button press on a framed button (nonRightPress slot of g_UiFramedTextButtonControlVtable and
   g_UiWindowControlVtable). A momentary button only shows itself pressed (the action follows on release); a
   persistent toggle button flips its selected state, a persistent radio-style button becomes selected unless
   it already is. Both of those play the activation sound when enabled and queue the action.
*/
void UiFramedTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control)

{
  UiSelectableStateFlags *selectedStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  UiSelectableStateFlags *toggleStateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
      if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
          (control->activationSound != NULL)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,NULL);
      }
      toggleStateFlagsField = &(control->selectable).stateFlags;
      *toggleStateFlagsField = *toggleStateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
          (control->activationSound != NULL)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,NULL);
      }
      selectedStateFlagsField = &(control->selectable).stateFlags;
      *selectedStateFlagsField = *selectedStateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
  return;
}


/* Primary button release on a framed button (nonRightRelease slot of g_UiFramedTextButtonControlVtable and
   g_UiWindowControlVtable): a momentary button that is still pressed (the pointer stayed on it) plays the
   activation sound when enabled, pops back up and queues its action.
*/
void UiFramedTextButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  
  if (((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
      (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0)) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
        (control->activationSound != NULL)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
    }
    stateFlagsField = &(control->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}


/* True when the point lies inside the framed button's box (with UI_BUTTON_FRAME_INSET: inside its frame,
   g_UiWindowFrameInset pixels in from every edge). */
static Bool8 UiFramedTextButtonControl_ContainsPoint
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFramedTextButtonControl *control)

{
  int relativeX;
  int relativeY;

  relativeX = pointerX - (control->selectable).base.left;
  relativeY = pointerY - (control->selectable).base.top;
  if ((pointerX < (control->selectable).base.left) || (pointerY < (control->selectable).base.top) ||
      ((control->selectable).base.layoutWidth <= relativeX) ||
      ((control->selectable).base.layoutHeight <= relativeY)) {
    return false;
  }
  if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
    return true;
  }
  return (g_UiWindowFrameInset <= relativeX) && (g_UiWindowFrameInset <= relativeY) &&
         (relativeX + g_UiWindowFrameInset < (control->selectable).base.layoutWidth) &&
         (relativeY + g_UiWindowFrameInset < (control->selectable).base.layoutHeight);
}


/* Primary-button drag with a framed button captured (nonRightDrag slot of g_UiFramedTextButtonControlVtable and
   g_UiWindowControlVtable): a momentary button shows itself pressed while the pointer is inside its box
   (inside the frame for UI_BUTTON_FRAME_INSET) and released while it is outside.
*/
void UiFramedTextButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control)

{
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0)) {
    if (!UiFramedTextButtonControl_ContainsPoint(pointerY,pointerX,control)) {
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
        (control->selectable).stateFlags = (control->selectable).stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
        UiNode_InvalidateRoot((UiNodeBase *)control);
      }
    }
    else if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      (control->selectable).stateFlags = (control->selectable).stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
}


/* Hit test of a framed button (hitTest slot of g_UiFramedTextButtonControlVtable and g_UiWindowControlVtable): the
   control itself when the point lies inside its box (for UI_BUTTON_FRAME_INSET: inside the frame), else
   UI_NODE_NONE. Suppressed buttons are never hit; children are not tested.
*/
UiNodeBase * UiFramedTextButtonControl_HitTestRect
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFramedTextButtonControl *control)

{
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (UiFramedTextButtonControl_ContainsPoint(pointerY,pointerX,control))) {
    return (UiNodeBase *)control;
  }
  return UI_NODE_NONE;
}


/* Draws a framed icon-and-text button (drawClipped slot of g_UiWindowControlVtable): the normal, selected or
   disabled win.gfx frame, the text centred in the right three quarters (with the focus mark while focused),
   and the icon, vertically centred and ending at the quarter line, over its shadow copy shifted by the normal
   or selected iconDrawOffsets (no shift while suppressed). Children are not drawn.
*/
void UiWindowControl_DrawFramedTextAndChrome
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiWindowControl *control)

{
  char iconOffsetX;
  char iconOffsetY;
  uint32_t cornerWidth;
  uint32_t cornerHeight;
  uint16_t *commandStream;
  uint32_t framePiece;
  uint32_t textWidth;
  int rightCornerX;
  int bottomCornerY;
  int buttonWidth;
  int textX;
  int textY;
  int styleVerticalOffset;
  int shadowMarkLeftX;
  int shadowMarkTop;
  int focusMarkTop;
  int focusTileX;
  int focusMarkRightX;
  int iconX;
  int iconY;
  int iconShadowX;
  int iconShadowY;
  uint32_t textStyle;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize cornerTileSize;
  GraphicsTextureLogicalSize rightCapSize;
  GraphicsTextureLogicalSize leftCapSize;
  GraphicsTextureLogicalSize middleTileSize;
  GraphicsTextureLogicalSize iconSize;

  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
      framePiece = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME;
    }
    else {
      framePiece = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      framePiece = framePiece + UI_WINDOW_FRAME_PIECE_COUNT;
    }
  }
  else {
    if (((control->selectable).stateFlags & UI_BUTTON_HIDDEN_WHILE_SUPPRESSED) != 0) {
      g_GraphicsFramebufferEndAccess();
      return;
    }
    if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
      framePiece = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME_DISABLED;
    }
    else {
      framePiece = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME_DISABLED;
    }
  }
  cornerTileSize = g_GraphicsTextureSourceGetLogicalSize(framePiece,g_UiWindowTextureSource);
  cornerHeight = cornerTileSize.logicalHeightPixels;
  cornerWidth = cornerTileSize.logicalWidthPixels;
  rightCornerX = (control->selectable).base.layoutWidth - cornerWidth;
  bottomCornerY = (control->selectable).base.layoutHeight - cornerHeight;
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,(control->selectable).base.left,
             framePiece,
             g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
             rightCornerX + (control->selectable).base.left,framePiece + UI_WINDOW_FRAME_TOP_RIGHT,
             g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,bottomCornerY + (control->selectable).base.top,
             (control->selectable).base.left,framePiece + UI_WINDOW_FRAME_BOTTOM_LEFT,
             g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,bottomCornerY + (control->selectable).base.top,
             rightCornerX + (control->selectable).base.left,
             framePiece + UI_WINDOW_FRAME_BOTTOM_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
  UiWindow_BlitTiledHorizontalEdge
            (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_TOP,rightCornerX,0,
             cornerWidth,control);
  UiWindow_BlitTiledVerticalEdge
            (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_LEFT,bottomCornerY,
             cornerHeight,0,control);
  UiWindow_BlitTiledVerticalEdge
            (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_RIGHT,bottomCornerY,
             cornerHeight,rightCornerX,control);
  UiWindow_BlitTiledHorizontalEdge
            (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_BOTTOM,rightCornerX,
             bottomCornerY,cornerWidth,control);
  commandStream = TextResource_Resolve(control->textResourceId);
  buttonWidth = (control->selectable).base.layoutWidth;
  textExtent = RichTextCommandStream_MeasureLine(control->packedTextStyle,commandStream);
  textWidth = textExtent.widthPixels;
  /* offsets inside the button: centred vertically and in the right three quarters */
  textY = (int)((control->selectable).base.layoutHeight - textExtent.heightPixels) >> 1;
  textX = ((int)(((uint32_t)(buttonWidth * 3) >> 2) - textWidth) >> 1) +
          ((uint32_t)(control->selectable).base.layoutWidth >> 2);
  textStyle = g_UiTextStyleDisabled;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    textStyle = g_UiTextStyleNormal;
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      textStyle = g_UiTextStyleSelected;
    }
  }
  /* packedTextStyle may override the font byte (bits 24-31) and the palette byte (bits 16-23) */
  if (((control->selectable).stateFlags & UI_BUTTON_OWN_STYLE_FONT) == 0) {
    control->packedTextStyle = control->packedTextStyle & UI_TEXT_STYLE_PALETTE_BYTE;
  }
  else {
    textStyle = textStyle & ~UI_TEXT_STYLE_FONT_BYTE;
  }
  if (((control->selectable).stateFlags & UI_BUTTON_OWN_STYLE_PALETTE) == 0) {
    control->packedTextStyle = control->packedTextStyle & UI_TEXT_STYLE_FONT_BYTE;
  }
  else {
    textStyle = textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE;
  }
  textStyle = textStyle | control->packedTextStyle;
  if (((control->selectable).base.nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,
               textY + (control->selectable).base.top,
               textX + (control->selectable).base.left);
  }
  else {
    textX = textX + (control->selectable).base.left;
    textY = textY + (control->selectable).base.top;
    /* focus mark shadow, shifted by the style's vertical offset: left cap, right cap, then the middle tiles */
    styleVerticalOffset = (int)(textStyle << 16) >> 24; /* signed byte 1 of the packed style */
    shadowMarkLeftX = styleVerticalOffset - 3 + textX;
    shadowMarkTop = styleVerticalOffset - 1 + textY;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,shadowMarkTop,shadowMarkLeftX,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    rightCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                         g_UiWindowTextureSource);
    leftCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                        g_UiWindowTextureSource);
    focusTileX = shadowMarkLeftX + leftCapSize.logicalWidthPixels;
    middleTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                           g_UiWindowTextureSource);
    focusMarkRightX = ((shadowMarkLeftX + 6) - rightCapSize.logicalWidthPixels) + textWidth;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,shadowMarkTop,focusMarkRightX,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    if (clipRight < focusMarkRightX) {
      focusMarkRightX = clipRight;
    }
    do {
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,focusMarkRightX,clipTop,clipLeft,shadowMarkTop,focusTileX,TEXT_SHADOW_COLOR_ARGB,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      focusTileX = focusTileX + middleTileSize.logicalWidthPixels;
    } while (focusTileX < focusMarkRightX);
    focusMarkTop = textY - 1;
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,textY,textX);
    /* the focus mark itself */
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,focusMarkTop,textX - 3,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,g_UiWindowTextureSource,
               g_FramebufferAccess);
    rightCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                         g_UiWindowTextureSource);
    leftCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                        g_UiWindowTextureSource);
    focusTileX = textX - 3 + leftCapSize.logicalWidthPixels;
    middleTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                           g_UiWindowTextureSource);
    focusMarkRightX = ((textX + 3) - rightCapSize.logicalWidthPixels) + textWidth;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,focusMarkTop,focusMarkRightX,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,g_UiWindowTextureSource,
               g_FramebufferAccess);
    if (clipRight < focusMarkRightX) {
      focusMarkRightX = clipRight;
    }
    do {
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,focusMarkRightX,clipTop,clipLeft,focusMarkTop,focusTileX,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      focusTileX = focusTileX + middleTileSize.logicalWidthPixels;
    } while (focusTileX < focusMarkRightX);
  }
  iconSize = g_GraphicsTextureSourceGetLogicalSize(control->iconSubresource,control->iconTextureSource);
  iconX = (((uint32_t)(control->selectable).base.layoutWidth >> 2) - iconSize.logicalWidthPixels) +
          (control->selectable).base.left;
  iconY = ((int)((control->selectable).base.layoutHeight - iconSize.logicalHeightPixels) >> 1) +
          (control->selectable).base.top;
  iconShadowX = iconX;
  iconShadowY = iconY;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      iconOffsetX = control->iconDrawOffsets.normalX;
      iconOffsetY = control->iconDrawOffsets.normalY;
    }
    else {
      iconOffsetX = control->iconDrawOffsets.selectedX;
      iconOffsetY = control->iconDrawOffsets.selectedY;
    }
    iconShadowX = iconX + iconOffsetX;
    iconShadowY = iconY + iconOffsetY;
  }
  g_GraphicsTextureSourceBlitModulatedSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,iconShadowY,iconShadowX,TEXT_SHADOW_COLOR_ARGB,
             control->iconSubresource,
             control->iconTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,iconY,iconX,control->iconSubresource,
             control->iconTextureSource,g_FramebufferAccess);
  g_GraphicsFramebufferEndAccess();
}


/* Relocation of a loaded text button (relocate slot of g_UiTextButtonControlVtable,
   g_UiGraphicsAdapterTextButtonVtable, g_UiNumericPairTextButtonVtable and g_UiPayloadPairTextButtonVtable):
   only the children need relocating; the text resource id and style are plain values.
*/
void UiTextButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiTextButtonControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  return;
}


/* Primary button press on a text button (nonRightPress slot of g_UiTextButtonControlVtable,
   g_UiGraphicsAdapterTextButtonVtable, g_UiNumericPairTextButtonVtable and g_UiPayloadPairTextButtonVtable).
   Only opaque pixels of the button graphic count. A checkbox (toggle) flips its checked state and leaves the
   alternate state; a radio-style button becomes selected unless it already is. Either way the activation
   sound plays when enabled and the action is queued.
*/
void UiTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextButtonControl *control)

{
  UiSelectableStateFlags *clearedStateFlagsField;
  Bool8 pixelHit;
  UiSelectableStateFlags *toggleStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) == 0) {
      pixelHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,UI_WINDOW_SUBRESOURCE_PUSH_BUTTON,g_UiWindowTextureSource);
      if (pixelHit) {
        if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
            (control->activationSound != NULL)) {
          g_SoundPlayOneShot
                    (g_UiSoundGainQ15,g_UiSoundGainQ15,
                     control->activationSound,NULL);
        }
        if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
          stateFlagsField = &(control->selectable).stateFlags;
          *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
          UiActionQueue_Enqueue((control->selectable).actionId,control);
          UiNode_InvalidateRoot((UiNodeBase *)control);
          return;
        }
      }
    }
    else {
      pixelHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,UI_WINDOW_SUBRESOURCE_CHECKBOX,g_UiWindowTextureSource);
      if (pixelHit) {
        if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
            (control->activationSound != NULL)) {
          g_SoundPlayOneShot
                    (g_UiSoundGainQ15,g_UiSoundGainQ15,
                     control->activationSound,NULL);
        }
        toggleStateFlagsField = &(control->selectable).stateFlags;
        *toggleStateFlagsField = *toggleStateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
        clearedStateFlagsField = &(control->selectable).stateFlags;
        *clearedStateFlagsField = *clearedStateFlagsField & ~UI_BUTTON_ALTERNATE_STATE;
        UiActionQueue_Enqueue((control->selectable).actionId,control);
        UiNode_InvalidateRoot((UiNodeBase *)control);
      }
    }
  }
  return;
}


/* Keyboard handler of a text button (keyboardEvent slot of g_UiTextButtonControlVtable,
   g_UiGraphicsAdapterTextButtonVtable, g_UiNumericPairTextButtonVtable and g_UiPayloadPairTextButtonVtable):
   Space on the focused button activates it like a pointer press (checkbox toggles, radio-style button gets
   selected) unless UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION is set. Everything else, including Space on
   an already selected radio-style button, goes to UiNode_DefaultKeyboardEventMoveFocusNext. Returns
   false: consumed.
*/
Bool8 UiTextButtonControl_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextButtonControl *control)

{
  UiSelectableStateFlags *selectedStateFlagsField;
  Bool8 delegatedResult;
  UiSelectableStateFlags *stateFlagsField;
  UiSelectableStateFlags *selectionStateFlagsField;
  
  if ((((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) && (keyCode == KEYBOARD_KEY_CODE_SPACE)) &&
      (control == (UiTextButtonControl *)g_UiKeyboardFocusNode)) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION) == 0)) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
      if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
          (control->activationSound != NULL)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,NULL);
      }
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
      selectionStateFlagsField = &(control->selectable).stateFlags;
      *selectionStateFlagsField = *selectionStateFlagsField & ~UI_BUTTON_ALTERNATE_STATE;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return false;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
          (control->activationSound != NULL)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,NULL);
      }
      selectedStateFlagsField = &(control->selectable).stateFlags;
      *selectedStateFlagsField = *selectedStateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return false;
    }
  }
  delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext
                    (keyboardStateMask,keyCode,(UiNodeBase *)control);
  return delegatedResult;
}


/* Draws an image panel (drawClipped slot of g_UiImagePanelControlVtable): its texture aligned in the layout
   box by panelFlags (centre/right/bottom), optionally over a drop shadow, or stretched over the whole box,
   clipped to the panel; then the children. A panel without a texture or a suppressed one draws nothing, not
   even its children.
*/
void UiImagePanelControl_DrawAlignedTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiImagePanelControl *control)

{
  int clippedRight;
  int slackWidth;
  int clippedLeft;
  int drawX;
  int clippedBottom;
  int slackHeight;
  int clippedTop;
  int drawY;
  Bool8 framebufferUnavailable;
  GraphicsTextureLogicalSize textureSize;
  
  if (((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    clippedLeft = (control->base).left;
    if ((control->base).left < clipLeft) {
      clippedLeft = clipLeft;
    }
    clippedTop = (control->base).top;
    if ((control->base).top < clipTop) {
      clippedTop = clipTop;
    }
    clippedRight = (control->base).right;
    if (clipRight < (control->base).right) {
      clippedRight = clipRight;
    }
    clippedBottom = (control->base).bottom;
    if (clipBottom < (control->base).bottom) {
      clippedBottom = clipBottom;
    }
    drawX = (control->base).left;
    drawY = (control->base).top;
    if (control->textureSource != NULL) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                        (control->subresource,control->textureSource);
      slackWidth = (control->base).layoutWidth - textureSize.logicalWidthPixels;
      slackHeight = (control->base).layoutHeight - textureSize.logicalHeightPixels;
      if ((control->panelFlags & UI_IMAGE_PANEL_ALIGN_RIGHT) != 0) {
        drawX = drawX + slackWidth;
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_ALIGN_BOTTOM) != 0) {
        drawY = drawY + slackHeight;
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_CENTER_X) != 0) {
        drawX = drawX + (slackWidth >> 1);
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_CENTER_Y) != 0) {
        drawY = drawY + (slackHeight >> 1);
      }
      framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
      if (!framebufferUnavailable) {
        if ((control->panelFlags & UI_IMAGE_PANEL_DROP_SHADOW) != 0) {
          g_GraphicsTextureSourceBlitModulatedSourceAlpha
                    (clippedBottom,clippedRight,clippedTop,clippedLeft,control->shadowOffsetY + drawY,
                     control->shadowOffsetX + drawX,TEXT_SHADOW_COLOR_ARGB,control->subresource,
                     control->textureSource,g_FramebufferAccess);
        }
        if ((control->panelFlags & UI_IMAGE_PANEL_STRETCH) == 0) {
          g_GraphicsTextureSourceBlitSourceAlpha
                    (clippedBottom,clippedRight,clippedTop,clippedLeft,drawY,drawX,control->subresource,
                     control->textureSource,g_FramebufferAccess);
        }
        else {
          g_GraphicsTextureSourceStretchDirectColorBilinear
                    ((control->base).layoutHeight,(control->base).layoutWidth,(control->base).top,(control->base).left,
                     control->subresource,control->textureSource,
                     g_FramebufferAccess);
        }
        g_GraphicsFramebufferEndAccess();
      }
      UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
    }
  }
  return;
}


/* Hit test of an image panel (hitTest slot of g_UiImagePanelControlVtable and g_UiArmyMetricsPanelVtable):
   the point must lie on an opaque pixel of the aligned texture (unless UI_IMAGE_PANEL_HIT_WHOLE_BOX), then
   the children are tested. UI_NODE_NONE for UI_IMAGE_PANEL_NEVER_HIT or a miss.
*/
UiNodeBase * UiImagePanelControl_HitTestAlignedTextureAndChildren(int pointerY,int pointerX,
                                                                  UiImagePanelControl *control)

{
  Bool8 childrenAlreadyRetried;
  Bool8 skipTextureTest;
  UiNodeBase *hitNode;
  int slackWidth;
  int drawX;
  int slackHeight;
  int drawY;
  Bool8 opaqueHit;
  GraphicsTextureLogicalSize textureSize;
  
  hitNode = UI_NODE_NONE;
  childrenAlreadyRetried = false;
  if ((control->panelFlags & UI_IMAGE_PANEL_NEVER_HIT) != 0) {
    return UI_NODE_NONE;
  }
  /* The first pass skips the opaque-texture test when children may be hit outside the bounds;
     a retry (the children hit test returned the panel itself) always runs it. */
  skipTextureTest = ((control->base).nodeFlags & UI_NODE_ALLOW_CHILD_HIT_TEST_OUTSIDE_BOUNDS) != 0;
  do {
    if ((!skipTextureTest) && ((control->panelFlags & UI_IMAGE_PANEL_HIT_WHOLE_BOX) == 0)) {
      drawX = (control->base).left;
      drawY = (control->base).top;
      if (control->textureSource == NULL) {
        return hitNode;
      }
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                        (control->subresource,control->textureSource);
      slackWidth = (control->base).layoutWidth - textureSize.logicalWidthPixels;
      slackHeight = (control->base).layoutHeight - textureSize.logicalHeightPixels;
      if ((control->panelFlags & UI_IMAGE_PANEL_ALIGN_RIGHT) != 0) {
        drawX = drawX + slackWidth;
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_ALIGN_BOTTOM) != 0) {
        drawY = drawY + slackHeight;
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_CENTER_X) != 0) {
        drawX = drawX + (slackWidth >> 1);
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_CENTER_Y) != 0) {
        drawY = drawY + (slackHeight >> 1);
      }
      opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,drawY,drawX,control->subresource,
                         control->textureSource);
      if (!opaqueHit) {
        return UI_NODE_NONE;
      }
    }
    skipTextureTest = false;
    hitNode = UiContainer_HitTestChildren(pointerY,pointerX,&control->base);
    if (childrenAlreadyRetried) {
      return hitNode;
    }
    childrenAlreadyRetried = true;
  } while (hitNode == &control->base);
  return hitNode;
}


/* Draws one row of fill panel tiles at tileTop, starting at the panel's left edge: a single tile, or with
   UI_FILL_PANEL_TILE_X tiles every tileWidth pixels while they start left of clipRight. Each tile is drawn
   over its drop shadow when UI_FILL_PANEL_DROP_SHADOW is set. */
static void UiFillPanelControl_DrawTileRow
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int32_t tileTop,uint32_t tileWidth,UiFillPanelControl *control)

{
  int tileLeft;

  tileLeft = (control->base).left;
  do {
    if ((control->fillFlags & UI_FILL_PANEL_DROP_SHADOW) != 0) {
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,
                 control->shadowOffsetY + tileTop,
                 control->shadowOffsetX + tileLeft,TEXT_SHADOW_COLOR_ARGB,control->subresourceOrFillArgb,
                 control->textureSource,g_FramebufferAccess);
    }
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,tileTop,tileLeft,control->subresourceOrFillArgb,
               control->textureSource,g_FramebufferAccess);
    if ((control->fillFlags & UI_FILL_PANEL_TILE_X) == 0) {
      return;
    }
    tileLeft = tileLeft + tileWidth;
  } while (tileLeft < clipRight);
}


/* Draws a fill panel (drawClipped slot of g_UiFillPanelControlVtable): without a texture, a rectangle in the
   ARGB colour subresourceOrFillArgb; with one, its subresource once or tiled across and/or down the box
   (fillFlags), each tile optionally over a drop shadow. Then the children.
*/
void UiFillPanelControl_DrawColorOrTiledTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiFillPanelControl *control)

{
  int controlRight;
  int controlBottom;
  uint32_t tileWidth;
  int controlLeft;
  uint32_t tileHeight;
  int32_t tileTop;
  Bool8 framebufferUnavailable;
  GraphicsTextureLogicalSize tileSize;

  controlRight = (control->base).right;
  controlBottom = (control->base).bottom;
  controlLeft = (control->base).left;
  tileTop = (control->base).top;
  if (control->textureSource == NULL) {
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      g_GraphicsFramebufferFillRectArgb
                (clipBottom,clipRight,clipTop,clipLeft,controlBottom,controlRight,tileTop,controlLeft,
                 control->subresourceOrFillArgb,
                 g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
    }
  }
  else {
    if (controlRight < clipRight) {
      clipRight = controlRight;
    }
    if (controlBottom < clipBottom) {
      clipBottom = controlBottom;
    }
    tileSize = g_GraphicsTextureSourceGetLogicalSize
                      (control->subresourceOrFillArgb,control->textureSource);
    tileHeight = tileSize.logicalHeightPixels;
    tileWidth = tileSize.logicalWidthPixels;
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      /* Tile rows left to right (UI_FILL_PANEL_TILE_X), then top to bottom (UI_FILL_PANEL_TILE_Y). */
      do {
        UiFillPanelControl_DrawTileRow(clipBottom,clipRight,clipTop,clipLeft,tileTop,tileWidth,control);
        if ((control->fillFlags & UI_FILL_PANEL_TILE_Y) == 0) break;
        tileTop = tileTop + tileHeight;
      } while (tileTop < clipBottom);
      g_GraphicsFramebufferEndAccess();
    }
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
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
     (control->activationSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
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


/* The label's rich-text command stream: the text itself, or the resolved text resource. */
static uint16_t *UiSingleLineTextControl_GetCommandStream(UiSingleLineTextControl *control)

{
  if ((control->labelFlags & UI_LABEL_TEXT_IS_STREAM) == 0) {
    return TextResource_Resolve((TextResourceId)control->text);
  }
  return control->text;
}


/* One piece of the keyboard-focus mark at (markTop, x): drawn as its shadow (TEXT_SHADOW_COLOR_ARGB) or as
   itself. */
static void UiSingleLineTextControl_BlitFocusMarkPiece
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int markTop,int x,uint32_t subresource,Bool8 shadow)

{
  if (shadow) {
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,markTop,x,TEXT_SHADOW_COLOR_ARGB,subresource,
               g_UiWindowTextureSource,g_FramebufferAccess);
  }
  else {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,markTop,x,subresource,g_UiWindowTextureSource,
               g_FramebufferAccess);
  }
}


/* Draws the keyboard-focus mark (or its shadow) behind a label line: the left cap at markLeft, the right cap
   at markLeft + lineWidth - capWidth, and the middle tiled from the end of the left cap up to the right cap
   (or clipRight). */
static void UiSingleLineTextControl_DrawFocusMark
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int markTop,int markLeft,int lineWidth,int capWidth,Bool8 shadow)

{
  int rightCapX;
  int tileX;
  GraphicsTextureLogicalSize tileSize;

  rightCapX = (lineWidth + markLeft) - capWidth;
  UiSingleLineTextControl_BlitFocusMarkPiece
            (clipBottom,clipRight,clipTop,clipLeft,markTop,markLeft,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,shadow);
  UiSingleLineTextControl_BlitFocusMarkPiece
            (clipBottom,clipRight,clipTop,clipLeft,markTop,rightCapX,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,shadow);
  tileX = capWidth + markLeft;
  if (rightCapX <= clipRight) {
    clipRight = rightCapX;
  }
  tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,g_UiWindowTextureSource);
  do {
    UiSingleLineTextControl_BlitFocusMarkPiece
              (clipBottom,clipRight,clipTop,clipLeft,markTop,tileX,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,shadow);
    tileX = tileX + tileSize.logicalWidthPixels;
  } while (tileX < clipRight);
}


/* Draws a single-line label that forwards its focus to a child (drawClipped slot of
   g_UiFocusProxyControlVtable): measures the line, aligns it by labelFlags (room for the focus-mark caps
   when there is a focusChild), draws the focus mark and its shadow while the label has keyboard focus, then
   the line (disabled style when the focus child is suppressed). While the label holds the keyboard focus,
   the focus is lent to focusChild for drawing the children, so the child draws itself focused.
*/
void UiSingleLineTextControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiSingleLineTextControl *control)

{
  UiPackedTextStyle packedStyleOverride;
  uint16_t *commandStream;
  UiNodeBase *focusLendTarget;
  int capWidth;
  uint32_t textStyle;
  int lineWidth;
  int alignOffsetY;
  int alignOffsetX;
  Bool8 framebufferUnavailable;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize tileSize;

  textStyle = g_UiTextStyleNormal;
  alignOffsetX = 0;
  alignOffsetY = 0;
  if ((((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) ||
     ((control->labelFlags & UI_LABEL_HIDE_WHILE_SUPPRESSED) == 0)) {
    if ((control->labelFlags & UI_LABEL_OWN_STYLE_FONT) == 0) {
      control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_FONT_BYTE;
    }
    else {
      textStyle = g_UiTextStyleNormal & ~UI_TEXT_STYLE_FONT_BYTE;
    }
    if ((control->labelFlags & UI_LABEL_OWN_STYLE_PALETTE) == 0) {
      control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_PALETTE_BYTE;
    }
    else {
      textStyle = textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE;
    }
    packedStyleOverride = control->styleOverride;
    commandStream = UiSingleLineTextControl_GetCommandStream(control);
    textExtent = RichTextCommandStream_MeasureLine((textStyle | packedStyleOverride) & (UI_TEXT_STYLE_FONT_BYTE|UI_TEXT_STYLE_PALETTE_BYTE),
                                                   commandStream);
    lineWidth = (int)(g_UiTextStyleNormal << 16) >> 24; /* signed byte 1 of the packed style */
    if (lineWidth < 0) {
      lineWidth = -lineWidth;
    }
    lineWidth = textExtent.widthPixels + lineWidth;
    tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,g_UiWindowTextureSource);
    capWidth = (int)tileSize.logicalWidthPixels;
    /* Original quirk: until the framebuffer is accessed, the node the focus is lent to below is the cap
       width reinterpreted as a pointer. */
    focusLendTarget = (UiNodeBase *)(uintptr_t)tileSize.logicalWidthPixels;
    if (control->focusChild != NULL) {
      lineWidth = lineWidth + capWidth * 2;
    }
    if ((control->labelFlags & UI_LABEL_ALIGN_RIGHT) != 0) {
      alignOffsetX = (control->base).layoutWidth - lineWidth;
    }
    if ((control->labelFlags & UI_LABEL_ALIGN_BOTTOM) != 0) {
      alignOffsetY = (control->base).layoutHeight - tileSize.logicalHeightPixels;
    }
    if ((control->labelFlags & UI_LABEL_CENTER_Y) != 0) {
      alignOffsetY = (int)((control->base).layoutHeight - tileSize.logicalHeightPixels) >> 1;
    }
    if ((control->labelFlags & UI_LABEL_CENTER_X) != 0) {
      alignOffsetX = ((control->base).layoutWidth - lineWidth) >> 1;
    }
    lineWidth--;
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) != 0) {
        /* The focus mark: first its shadow one pixel down and right, then the mark itself. */
        UiSingleLineTextControl_DrawFocusMark
                  (clipBottom,clipRight,clipTop,clipLeft,alignOffsetY + 1 + (control->base).top,
                   alignOffsetX + 1 + (control->base).left,lineWidth,capWidth,true);
        UiSingleLineTextControl_DrawFocusMark
                  (clipBottom,clipRight,clipTop,clipLeft,alignOffsetY + (control->base).top,
                   alignOffsetX + (control->base).left,lineWidth,capWidth,false);
      }
      if (control->focusChild != NULL) {
        alignOffsetX = capWidth + alignOffsetX;
        alignOffsetY++;
      }
      commandStream = UiSingleLineTextControl_GetCommandStream(control);
      focusLendTarget = control->focusChild;
      textStyle = g_UiTextStyleNormal;
      if ((focusLendTarget != NULL) && ((focusLendTarget->nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
        textStyle = g_UiTextStyleDisabled;
      }
      if ((control->labelFlags & UI_LABEL_OWN_STYLE_FONT) == 0) {
        control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_FONT_BYTE;
      }
      else {
        textStyle = textStyle & ~UI_TEXT_STYLE_FONT_BYTE;
      }
      if ((control->labelFlags & UI_LABEL_OWN_STYLE_PALETTE) == 0) {
        control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_PALETTE_BYTE;
      }
      else {
        textStyle = textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE;
      }
      control->styleOverride = control->styleOverride & (UI_TEXT_STYLE_FONT_BYTE|UI_TEXT_STYLE_PALETTE_BYTE);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,textStyle | control->styleOverride,
                 commandStream,alignOffsetY + (control->base).top,alignOffsetX + (control->base).left);
      g_GraphicsFramebufferEndAccess();
    }
    /* NOTE: as in the original, focusLendTarget is the focus child only when the framebuffer could be
       accessed (else it still holds the cap width), and a focused label without a focus child dereferences
       NULL here. */
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = focusLendTarget;
      focusLendTarget->nodeFlags = focusLendTarget->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
      UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
      focusLendTarget->nodeFlags = focusLendTarget->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
      g_UiKeyboardFocusNode = &control->base;
    }
    else {
      UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
    }
  }
}


/* Draws the visible rows of a text list (drawClipped slot of g_UiTextListControlVtable): each row's rich
   text in the list style, the selected row over a highlight bar as wide as its text plus 6 pixels (with end
   caps while the list has keyboard focus).
*/
void UiTextListControl_DrawRowsAndSelection
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextListControl *control)

{
  int firstVisibleRow;
  int rowTop;
  uint32_t lastVisibleRow;
  int highlightWidth;
  uint16_t **lastRowSlot;
  uint16_t **rowSlot;
  Bool8 framebufferUnavailable;
  RichTextExtent rowExtent;
  GraphicsTextureLogicalSize capSize;

  if (control->rowCount != 0) {
    firstVisibleRow = (clipTop - (control->base).top) / (int)control->rowHeight;
    if (firstVisibleRow < 0) {
      firstVisibleRow = 0;
    }
    rowSlot = control->rowTextSlots + firstVisibleRow;
    lastVisibleRow = (int)((clipBottom - (control->base).top) + control->rowHeight) / (int)control->rowHeight;
    rowTop = firstVisibleRow * control->rowHeight;
    if (control->rowCount <= lastVisibleRow) {
      lastVisibleRow = control->rowCount - 1;
    }
    lastRowSlot = control->rowTextSlots + lastVisibleRow;
    if (rowSlot <= lastRowSlot) {
      framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
      if (!framebufferUnavailable) {
        do {
          if (rowSlot == control->selectedRowSlot) {
            rowExtent = RichTextCommandStream_MeasureLine(g_UiListTextStyle,*rowSlot);
            highlightWidth = rowExtent.widthPixels + 6;
            if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_LIST_SELECTION,highlightWidth,
                         rowTop,0,control);
            }
            else {
              capSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_LEFT,
                                                              g_UiWindowTextureSource);
              highlightWidth = highlightWidth - capSize.logicalWidthPixels;
              UiWindow_BlitTiledHorizontalEdge
                        (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_MIDDLE,
                         highlightWidth,rowTop,
                         capSize.logicalWidthPixels,control);
              g_GraphicsTextureSourceBlitSourceAlpha
                        (clipBottom,clipRight,clipTop,clipLeft,rowTop + (control->base).top,
                         (control->base).left,UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_LEFT,g_UiWindowTextureSource,
                         g_FramebufferAccess);
              g_GraphicsTextureSourceBlitSourceAlpha
                        (clipBottom,clipRight,clipTop,clipLeft,rowTop + (control->base).top,
                         highlightWidth + (control->base).left,UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_RIGHT,
                         g_UiWindowTextureSource,
                         g_FramebufferAccess);
            }
          }
          RichTextCommandStream_DrawSingleLine
                    (clipBottom,clipRight,clipTop,clipLeft,g_UiListTextStyle,*rowSlot,
                     rowTop + 1 + (control->base).top,(control->base).left + 3);
          rowSlot++;
          rowTop = rowTop + control->rowHeight;
        } while (rowSlot <= lastRowSlot);
        g_GraphicsFramebufferEndAccess();
      }
    }
  }
  return;
}


/* Primary button press on a text list (nonRightPress slot of g_UiTextListControlVtable): a click on the text
   of a row (its width plus 6 pixels) selects that row, scrolls it into view, queues the action and plays the
   selection sound when enabled. A double click also marks the selection confirmed and repeats the action for
   the already selected row; a single click on it does nothing.
*/
void UiTextListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextListControl *control)

{
  uint32_t rowIndex;
  int controlLeft;
  int rowTop;
  RichTextExtent rowExtent;
  uint16_t **clickedRowSlot;

  if (((control->base).top > pointerY) || ((control->base).left > pointerX)) {
    return;
  }
  controlLeft = (control->base).left;
  rowIndex = (uint32_t)(pointerY - (control->base).top) / control->rowHeight;
  if (rowIndex >= control->rowCount) {
    return;
  }
  clickedRowSlot = control->rowTextSlots + rowIndex;
  rowExtent = RichTextCommandStream_MeasureLine(g_UiListTextStyle,*clickedRowSlot);
  if (pointerX - controlLeft >= (int)(rowExtent.widthPixels + 6)) {
    return;
  }
  control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_SELECTION_CONFIRMED;
  if (((control->base).nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) == 0) {
    /* A single click: not confirmed, and nothing to do on the already selected row. */
    control->listStateFlags = control->listStateFlags & ~UI_TEXT_LIST_SELECTION_CONFIRMED;
    if (clickedRowSlot == control->selectedRowSlot) {
      return;
    }
  }
  control->selectedRowSlot = clickedRowSlot;
  rowTop = rowIndex * control->rowHeight;
  UiScrollableControl_ClampOffsetsToViewport
            (rowTop + 1 + control->rowHeight,(control->base).rightOffset,rowTop,0,
             (UiScrollableControl *)(control->base).parent);
  UiActionQueue_Enqueue(control->actionId,control);
  if (((control->listStateFlags & UI_TEXT_LIST_PLAY_SELECTION_SOUND) != 0) &&
     (control->activationSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
  }
}


/* Keyboard handler of a text list (keyboardEvent slot of g_UiTextListControlVtable): with type search, a
   character selects the first row whose first code unit is not below it, ASCII case-insensitive (meant for
   sorted lists; the last row when there is none); Enter confirms the selection
   and queues the action; Home/End/Page Up/Page Down/Up/Down move the selection. A changed selection plays
   the selection sound when enabled, is scrolled into view and arms the deferred action, which
   UiTextListControl_TickActivationPulse queues after g_UiListActivationPulseFrames frames. Other keys go to
   UiNode_DefaultKeyboardEventMoveFocusNext. Returns false: consumed.
*/
Bool8 UiTextListControl_HandleKeyboardNavigationAndSearch
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextListControl *control)

{
  uint16_t **previousSelectedSlot;
  uint16_t **scanSlot;
  uint16_t **selectedSlot;
  int pageUpRow;
  int selectedRowTop;
  int pulseFrames;
  uint32_t pageDownRow;
  UiListRowCount remainingRows;
  uint16_t **candidateSlot;
  Bool8 rowBelowKey;
  UiScrollableViewportSize viewportSize;

  previousSelectedSlot = control->selectedRowSlot;
  if ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0) {
    if (((keyboardStateMask & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0) ||
       ((control->listStateFlags & UI_TEXT_LIST_TYPE_SEARCH_ENABLED) == 0)) {
      return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    }
    remainingRows = control->rowCount;
    scanSlot = control->rowTextSlots;
    if (remainingRows != 0) {
      /* Stops at the first row that is not below the typed character, else at the last row. */
      do {
        candidateSlot = scanSlot;
        rowBelowKey = g_KeyboardAsciiCaseTransformCallbacks3.compareCaseInsensitiveFlags
                          (keyCode,*(uint32_t *)*candidateSlot);
        if (!rowBelowKey) break;
        remainingRows--;
        scanSlot = candidateSlot + 1;
      } while (remainingRows != 0);
      control->selectedRowSlot = candidateSlot;
    }
  }
  else if (keyCode == KEYBOARD_KEY_CODE_ENTER) {
    control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_SELECTION_CONFIRMED;
    UiActionQueue_Enqueue(control->actionId,control);
  }
  else if (keyCode == KEYBOARD_KEY_CODE_HOME) {
    control->selectedRowSlot = control->rowTextSlots;
  }
  else if (keyCode == KEYBOARD_KEY_CODE_END) {
    control->selectedRowSlot = control->rowTextSlots + (control->rowCount - 1);
  }
  else if (keyCode == KEYBOARD_KEY_CODE_PAGE_UP) {
    viewportSize = UiScrollableControl_GetViewportSize((UiScrollableControl *)(control->base).parent);
    pageUpRow = ((uint32_t)((int)control->selectedRowSlot - (int)control->rowTextSlots) >> 2) -
            ((int)(viewportSize.height / control->rowHeight) - 1);
    if (pageUpRow < 0) {
      pageUpRow = 0;
    }
    control->selectedRowSlot = control->rowTextSlots + pageUpRow;
  }
  else if (keyCode == KEYBOARD_KEY_CODE_PAGE_DOWN) {
    viewportSize = UiScrollableControl_GetViewportSize((UiScrollableControl *)(control->base).parent);
    pageDownRow = ((uint32_t)((int)control->selectedRowSlot - (int)control->rowTextSlots) >> 2) +
            (int)(viewportSize.height / control->rowHeight) - 1;
    if (control->rowCount <= pageDownRow) {
      pageDownRow = control->rowCount - 1;
    }
    control->selectedRowSlot = control->rowTextSlots + pageDownRow;
  }
  else if (keyCode == KEYBOARD_KEY_CODE_UP) {
    if (control->rowTextSlots <= control->selectedRowSlot - 1) {
      control->selectedRowSlot--;
    }
  }
  else if (keyCode == KEYBOARD_KEY_CODE_DOWN) {
    if (((uint32_t)((int)control->selectedRowSlot - (int)control->rowTextSlots) >> 2) + 1 <
        control->rowCount) {
      control->selectedRowSlot++;
    }
  }
  else {
    return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
  }
  selectedSlot = control->selectedRowSlot;
  if (selectedSlot != previousSelectedSlot) {
    if (((control->listStateFlags & UI_TEXT_LIST_PLAY_SELECTION_SOUND) != 0) &&
       (control->activationSound != NULL)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
    }
    selectedRowTop = ((uint32_t)((int)selectedSlot - (int)control->rowTextSlots) >> 2) * control->rowHeight;
    UiScrollableControl_ClampOffsetsToViewport
              (selectedRowTop + control->rowHeight + 1,(control->base).rightOffset,selectedRowTop,0,
               (UiScrollableControl *)(control->base).parent);
    /* arm the deferred action: the frame counter lives in the top byte */
    pulseFrames = g_UiListActivationPulseFrames;
    control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_DEFERRED_ACTION_PENDING;
    control->listStateFlags = control->listStateFlags & UI_LIST_FLAGS_MASK;
    control->listStateFlags = control->listStateFlags | pulseFrames << UI_LIST_COUNTDOWN_SHIFT;
  }
  return false;
}


/* Per-frame tick of a text list (tick slot of g_UiTextListControlVtable): while a keyboard selection change
   is pending, counts its frame counter (top byte of listStateFlags) down and queues the list's action when it
   reaches 0, so quick key repeats queue only one action.
*/
void UiTextListControl_TickActivationPulse(UiTextListControl *control)

{
  if ((control->listStateFlags & UI_TEXT_LIST_DEFERRED_ACTION_PENDING) == 0) {
    return;
  }
  control->listStateFlags = control->listStateFlags - UI_STATE_FRAME_COUNTER_UNIT;
  if ((control->listStateFlags & UI_LIST_COUNTDOWN_MASK) == 0) {
    control->listStateFlags =
         control->listStateFlags &
         (UI_LIST_FLAGS_MASK & ~(UI_TEXT_LIST_DEFERRED_ACTION_PENDING|UI_TEXT_LIST_SELECTION_CONFIRMED));
    UiActionQueue_Enqueue(control->actionId,control);
  }
}


/* Enables a text list whose action id matches (unsuppressActionId slot of g_UiTextListControlVtable), then
   passes the id on to the children.
*/
void UiTextListControl_UnsuppressIfActionId(UiActionId actionId,UiTextListControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags & ~UI_NODE_SUPPRESSED;
  }
  UiContainer_UnsuppressActionId(actionId,&control->base);
  return;
}


/* Disables a text list whose action id matches (suppressActionId slot of g_UiTextListControlVtable), then
   passes the id on to the children.
*/
void UiTextListControl_SuppressIfActionId(UiActionId actionId,UiTextListControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags | UI_NODE_SUPPRESSED;
  }
  UiContainer_SuppressActionId(actionId,&control->base);
  return;
}


/* Fills a pointer list whose rows are rich-text strings (rowPointers, one per row) and selects row 0. The
   list's size follows its content: one list-font line plus 1 pixel per row, and the widest measured row
   plus 6 pixels; the parent (the scrollable frame) is laid out again for the new size.
*/
void UiPointerList_InitializeMeasuredTextRows(UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control)

{
  UiNodeBase *parentNode;
  UiPixelExtent rowHeightPixels;
  uint32_t maximumTextWidthPixels;
  RichTextExtent measuredTextExtent;
  uint32_t lineHeight;
  UiNodeVtable *parentVtable;

  FontGlyph_GetLogicalSizeForStyle(g_UiListTextStyle,0,&lineHeight);
  rowHeightPixels = lineHeight + 1;
  control->rowHeight = rowHeightPixels;
  control->rowCount = rowCount;
  control->rowSlots = rowPointers;
  maximumTextWidthPixels = 0;
  control->selectedRowSlot = rowPointers;
  (control->base).bottomOffset = rowHeightPixels * rowCount + 1;
  for (; rowCount != 0; rowCount--) {
    measuredTextExtent = RichTextCommandStream_MeasureLine(g_UiListTextStyle,*rowPointers);
    if (maximumTextWidthPixels < measuredTextExtent.widthPixels) {
      maximumTextWidthPixels = measuredTextExtent.widthPixels;
    }
    rowPointers++;
  }
  parentNode = (control->base).parent;
  parentVtable = parentNode->vtable;
  (control->base).rightOffset = maximumTextWidthPixels + 6;
  (control->base).leftOffset = 0;
  (control->base).topOffset = 0;
  parentVtable->layout(parentNode);
  return;
}


/* Draws a wrapped multi-line label (drawClipped slot of g_UiListOffsetControlVtable): its text (a text
   resource or a command stream) wrapped at wrapWidth, which follows the layout width unless
   UI_LABEL_KEEP_WRAP_WIDTH, in g_UiTextStyleNormal with the label's font/palette overrides; then the
   children.
*/
void UiWrappedTextControl_DrawClipped(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiWrappedTextControl *control)

{
  UiPackedTextStyle packedStyleOverride;
  uint16_t *commandStream;
  uint32_t textStyle;
  Bool8 framebufferUnavailable;
  uint16_t *resolvedText;
  
  framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
  if (!framebufferUnavailable) {
    if ((control->labelFlags & UI_LABEL_KEEP_WRAP_WIDTH) == 0) {
      control->wrapWidth = (control->base).layoutWidth;
    }
    textStyle = g_UiTextStyleNormal;
    if ((control->labelFlags & UI_LABEL_OWN_STYLE_FONT) == 0) {
      control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_FONT_BYTE;
    }
    else {
      textStyle = g_UiTextStyleNormal & ~UI_TEXT_STYLE_FONT_BYTE;
    }
    if ((control->labelFlags & UI_LABEL_OWN_STYLE_PALETTE) == 0) {
      control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_PALETTE_BYTE;
    }
    else {
      textStyle = textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE;
    }
    packedStyleOverride = control->styleOverride;
    commandStream = control->text;
    if ((control->labelFlags & UI_LABEL_TEXT_IS_STREAM) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)commandStream);
      commandStream = resolvedText;
    }
    RichTextCommandStream_DrawWrappedBlock
              (clipBottom,clipRight,clipTop,clipLeft,(textStyle | packedStyleOverride) & (UI_TEXT_STYLE_FONT_BYTE|UI_TEXT_STYLE_PALETTE_BYTE),
               commandStream,control->wrapWidth,(control->base).top,(control->base).left);
    g_GraphicsFramebufferEndAccess();
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
  return;
}


/* Draws a nine-slice panel (drawClipped slot of g_UiNineSlicePanelControlVtable) from eight consecutive
   frames starting at firstFrameSubresource: 0 top-left and 1 top-right corner, 2 top edge, 3 left edge,
   4 right edge, 5 bottom-left and 6 bottom-right corner, 7 bottom edge (edges tiled), and centerSubresource
   tiled over the interior; then the children. GRAPHICS_TILED_BLIT_ONE_TILE keeps an edge one tile thick.
*/
void UiNineSlicePanelControl_DrawTextureFrameAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNineSlicePanelControl *control)

{
  GraphicsSubresourceIndex baseTextureFrame;
  uint32_t slice4Width;
  int leftEdgeX;
  int topEdgeY;
  int bottomEdgeY;
  int rightEdgeX;
  Bool8 framebufferUnavailable;
  GraphicsTextureLogicalSize slice0Size;
  GraphicsTextureLogicalSize slice1Size;
  GraphicsTextureLogicalSize slice2Size;
  GraphicsTextureLogicalSize slice3Size;
  GraphicsTextureLogicalSize slice4Or5Size;
  GraphicsTextureLogicalSize slice6Size;
  GraphicsTextureLogicalSize slice7Size;
  
  if (((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      baseTextureFrame = control->firstFrameSubresource;
      slice0Size = g_GraphicsTextureSourceGetLogicalSize
                        (baseTextureFrame,control->textureSource);
      slice1Size = g_GraphicsTextureSourceGetLogicalSize
                        (baseTextureFrame + 1,
                         control->textureSource);
      slice2Size = g_GraphicsTextureSourceGetLogicalSize
                        (baseTextureFrame + 2,
                         control->textureSource);
      slice3Size = g_GraphicsTextureSourceGetLogicalSize
                         (baseTextureFrame + 3,
                          control->textureSource);
      slice4Or5Size = g_GraphicsTextureSourceGetLogicalSize
                         (baseTextureFrame + 4,
                          control->textureSource);
      slice4Width = slice4Or5Size.logicalWidthPixels;
      slice4Or5Size = g_GraphicsTextureSourceGetLogicalSize
                         (baseTextureFrame + 5,
                          control->textureSource);
      slice6Size = g_GraphicsTextureSourceGetLogicalSize
                         (baseTextureFrame + 6,
                          control->textureSource);
      slice7Size = g_GraphicsTextureSourceGetLogicalSize
                         (baseTextureFrame + 7,
                          control->textureSource);
      leftEdgeX = (control->base).left;
      topEdgeY = (control->base).top;
      bottomEdgeY = (control->base).bottom;
      rightEdgeX = (control->base).right - slice1Size.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,topEdgeY,leftEdgeX,baseTextureFrame,
                 control->textureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,topEdgeY,rightEdgeX,
                 baseTextureFrame + 1,
                 control->textureSource,g_FramebufferAccess);
      leftEdgeX = leftEdgeX + slice0Size.logicalWidthPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,rightEdgeX,topEdgeY,leftEdgeX,
                 baseTextureFrame + 2,
                 control->textureSource,g_FramebufferAccess);
      leftEdgeX = leftEdgeX - slice0Size.logicalWidthPixels;
      topEdgeY = topEdgeY + slice0Size.logicalHeightPixels;
      bottomEdgeY = bottomEdgeY - slice4Or5Size.logicalHeightPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY,GRAPHICS_TILED_BLIT_ONE_TILE,topEdgeY,leftEdgeX,
                 baseTextureFrame + 3,
                 control->textureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY,leftEdgeX,
                 baseTextureFrame + 5,
                 control->textureSource,g_FramebufferAccess);
      rightEdgeX = (rightEdgeX + slice1Size.logicalWidthPixels) - slice4Width;
      topEdgeY = (topEdgeY - slice0Size.logicalHeightPixels) + slice1Size.logicalHeightPixels;
      bottomEdgeY = (bottomEdgeY + slice4Or5Size.logicalHeightPixels) - slice6Size.logicalHeightPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY,GRAPHICS_TILED_BLIT_ONE_TILE,topEdgeY,rightEdgeX,
                 baseTextureFrame + 4,control->textureSource,
                 g_FramebufferAccess);
      rightEdgeX = (rightEdgeX + slice4Width) - slice6Size.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY,rightEdgeX,
                 baseTextureFrame + 6,
                 control->textureSource,g_FramebufferAccess);
      leftEdgeX = leftEdgeX + slice4Or5Size.logicalWidthPixels;
      bottomEdgeY = (bottomEdgeY + slice6Size.logicalHeightPixels) - slice7Size.logicalHeightPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,rightEdgeX,bottomEdgeY,leftEdgeX,
                 baseTextureFrame + 7,
                 control->textureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY,
                 (rightEdgeX + slice6Size.logicalWidthPixels) - slice4Width,
                 (topEdgeY - slice1Size.logicalHeightPixels) + slice2Size.logicalHeightPixels,
                 (leftEdgeX - slice4Or5Size.logicalWidthPixels) + slice3Size.logicalWidthPixels,
                 control->centerSubresource,control->textureSource,
                 g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
    }
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
  return;
}


/* Relocation of a loaded resource gauge (relocate slot of g_UiFormattedContainerVtable): when it has a
   tooltip, points payloads 0, 1 (and 2 with UI_GAUGE_HAS_MARKER) of the tooltip text (the resource id stored
   just before the node) at the gauge's own value strings, so the tooltip always shows the current numbers;
   the value strings start out empty. Then the children are relocated.
*/
void UiFormattedContainer_RelocateWithPatchedTextPayloads
          (UiSerializedRelocationDelta relocationDelta,UiFormattedContainer *control)

{
  uint16_t *stream;
  uint16_t *resolvedText;
  
  if (((control->base).nodeFlags & UI_NODE_TOOLTIP_ELIGIBLE) != 0) {
    resolvedText = TextResource_Resolve(((TextResourceId *)control)[-1]);
    stream = resolvedText;
    RichTextCommandStream_PatchPayloadBySelector(0,control->currentValueTextUtf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(1,control->limitValueTextUtf16,stream);
    if ((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) {
      RichTextCommandStream_PatchPayloadBySelector(2,((UiFormattedContainerWithMarker *)control)->markerValueTextUtf16,
                                                   stream);
      *(uint32_t *)((UiFormattedContainerWithMarker *)control)->markerValueTextUtf16 = 0;
    }
  }
  *(uint32_t *)control->currentValueTextUtf16 = 0;
  *(uint32_t *)control->limitValueTextUtf16 = 0;
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}


/* Fill colour variant of a gauge (frame offset 3..18, three frames each) by the fill percentage: rising from
   80% on, or for the two-sided scale also rising the further it falls below 40%. */
static int UiFormattedContainer_FillVariantOffset(uint32_t fillPercent,Bool8 twoSidedScale)

{
  if (!twoSidedScale) {
    if (fillPercent <= 79) {
      return 3;
    }
    if (fillPercent <= 83) {
      return 6;
    }
    if (fillPercent <= 87) {
      return 9;
    }
    if (fillPercent <= 91) {
      return 12;
    }
    if (fillPercent <= 95) {
      return 15;
    }
    return 18;
  }
  if (fillPercent <= 7) {
    return 18;
  }
  if (fillPercent <= 15) {
    return 15;
  }
  if (fillPercent <= 23) {
    return 12;
  }
  if (fillPercent <= 31) {
    return 9;
  }
  if (fillPercent <= 39) {
    return 6;
  }
  if (fillPercent <= 85) {
    return 3;
  }
  if (fillPercent <= 87) {
    return 6;
  }
  if (fillPercent <= 89) {
    return 9;
  }
  if (fillPercent <= 91) {
    return 12;
  }
  if (fillPercent <= 93) {
    return 15;
  }
  return 18;
}


/* Draws a resource gauge (drawClipped slot of g_UiFormattedContainerVtable): the empty bar (frames
   firstFrameSubresource + 0/1/2: left cap, tiled middle, right cap), the fill up to currentValue in one of six
   colour variants (+3, +6, ... +18, chosen by the fill percentage of the limit or marker), and marker frame
   +21 at limitValue and, with UI_GAUGE_HAS_MARKER, at markerValue. The scale starts at
   initialScaleRange and grows by factors of 4 until every value fits. Afterwards (also when nothing is
   drawn) the value strings for the tooltip are refreshed.
*/
void UiFormattedContainer_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiFormattedContainer *control)

{
  int currentValue;
  int limitValue;
  uint32_t fillPercent;
  int scaleRange;
  int scaleLimit;
  uint32_t textureFrame;
  int barEndX;
  int barSpan;
  int limitMarkerX;
  int fillEndX;
  int barStartX;
  GraphicsTextureLogicalSize frameSize;
  int markerOffsetX;

  if (control->limitValue != 0 && !g_GraphicsFramebufferBeginAccess()) {
    if (clipLeft < (control->base).left) {
      clipLeft = (control->base).left;
    }
    if (clipTop < (control->base).top) {
      clipTop = (control->base).top;
    }
    if ((control->base).right < clipRight) {
      clipRight = (control->base).right;
    }
    if ((control->base).bottom < clipBottom) {
      clipBottom = (control->base).bottom;
    }
    textureFrame = control->firstFrameSubresource;
    barStartX = (control->base).left;
    barEndX = (control->base).right;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,barStartX,textureFrame,
               control->textureSource,g_FramebufferAccess);
    frameSize = g_GraphicsTextureSourceGetLogicalSize
                       (textureFrame,control->textureSource);
    barStartX = barStartX + frameSize.logicalWidthPixels;
    frameSize = g_GraphicsTextureSourceGetLogicalSize
                       (textureFrame + UI_GAUGE_FRAME_END_CAP,control->textureSource);
    barEndX = barEndX - frameSize.logicalWidthPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,barEndX,
               textureFrame + UI_GAUGE_FRAME_END_CAP,
               control->textureSource,g_FramebufferAccess);
    if (barStartX < barEndX) {
      GraphicsTextureSource_BlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,barEndX,
                 (control->base).top,barStartX,textureFrame + UI_GAUGE_FRAME_TRACK,
                 control->textureSource,g_FramebufferAccess);
      currentValue = control->currentValue;
      scaleRange = control->initialScaleRange;
      barSpan = barEndX - barStartX;
      /* Grow the scale by factors of 4 until it covers the value, the limit and the marker. */
      while ((scaleRange < currentValue) || (scaleRange < control->limitValue) ||
             (((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) &&
              (scaleRange < ((UiFormattedContainerWithMarker *)control)->markerValue))) {
        scaleRange = scaleRange << 2;
      }
      if ((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) {
        markerOffsetX =
             (int)(((int64_t)((UiFormattedContainerWithMarker *)control)->markerValue * (int64_t)barSpan) /
                   (int64_t)scaleRange);
      }
      limitValue = control->limitValue;
      scaleLimit = control->limitValue;
      if (((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) &&
          (((UiFormattedContainerWithMarker *)control)->markerValue < scaleLimit)) {
        scaleLimit = ((UiFormattedContainerWithMarker *)control)->markerValue;
      }
      fillPercent = (uint32_t)(((int64_t)currentValue * 100) / (int64_t)scaleLimit);
      textureFrame = UiFormattedContainer_FillVariantOffset
                       (fillPercent,(control->gaugeFlags & UI_GAUGE_TWO_SIDED_SCALE) != 0) +
                     control->firstFrameSubresource;
      if (control->currentValue != 0) {
        frameSize = g_GraphicsTextureSourceGetLogicalSize
                           (textureFrame,control->textureSource);
        barStartX = barStartX - frameSize.logicalWidthPixels;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,barStartX,textureFrame,
                   control->textureSource,g_FramebufferAccess);
        barStartX = barStartX + frameSize.logicalWidthPixels;
        fillEndX = (int)(((int64_t)currentValue * (int64_t)barSpan) / (int64_t)scaleRange) + barStartX;
        GraphicsTextureSource_BlitTiledSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,fillEndX,
                   (control->base).top,barStartX,
                   textureFrame + UI_GAUGE_FRAME_TRACK,control->textureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,fillEndX,
                   textureFrame + UI_GAUGE_FRAME_END_CAP,
                   control->textureSource,g_FramebufferAccess);
      }
      limitMarkerX = barStartX + (int)(((int64_t)limitValue * (int64_t)barSpan) / (int64_t)scaleRange);
      textureFrame = control->firstFrameSubresource + UI_GAUGE_FRAME_MARKER;
      frameSize = g_GraphicsTextureSourceGetLogicalSize
                         (textureFrame,control->textureSource);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,
                 limitMarkerX - ((int)frameSize.logicalWidthPixels >> 1),textureFrame,
                 control->textureSource,g_FramebufferAccess);
      if ((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) {
        textureFrame = control->firstFrameSubresource + UI_GAUGE_FRAME_MARKER;
        frameSize = g_GraphicsTextureSourceGetLogicalSize
                           (textureFrame,control->textureSource);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,
                   (barStartX + markerOffsetX) - ((int)frameSize.logicalWidthPixels >> 1),textureFrame,
                   control->textureSource,g_FramebufferAccess);
      }
    }
    g_GraphicsFramebufferEndAccess();
  }
  /* the tooltip value strings, also when nothing was drawn */
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->currentValue,
             control->currentValueTextUtf16);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->limitValue,
             control->limitValueTextUtf16);
  if ((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,((UiFormattedContainerWithMarker *)control)->markerValue,
               ((UiFormattedContainerWithMarker *)control)->markerValueTextUtf16);
  }
  return;
}


/* Draws the army metrics panel (drawClipped slot of g_UiArmyMetricsPanelVtable): the aligned panel texture
   like an image panel, then, when an entity is attached, its runtime metrics via
   SelectionPanel_RenderArmyRuntimeMetrics with the info-panel graphics temporarily installed as the
   selection-panel graphics; then the children.
*/
void UiArmyMetricsPanel_DrawTextureMetricsAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiArmyMetricsPanel *control)

{
  GraphicsTextureSourceAsset *savedTextureSource;
  void *savedPanelData;
  int clippedRight;
  int slackWidth;
  int clippedLeft;
  int drawX;
  int clippedBottom;
  int slackHeight;
  int clippedTop;
  int drawY;
  Bool8 framebufferUnavailable;
  GraphicsTextureLogicalSize textureSize;
  
  if (((control->base).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    clippedLeft = (control->base).base.left;
    if ((control->base).base.left < clipLeft) {
      clippedLeft = clipLeft;
    }
    clippedTop = (control->base).base.top;
    if ((control->base).base.top < clipTop) {
      clippedTop = clipTop;
    }
    clippedRight = (control->base).base.right;
    if (clipRight < (control->base).base.right) {
      clippedRight = clipRight;
    }
    clippedBottom = (control->base).base.bottom;
    if (clipBottom < (control->base).base.bottom) {
      clippedBottom = clipBottom;
    }
    drawX = (control->base).base.left;
    drawY = (control->base).base.top;
    if ((control->base).textureSource != NULL) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                        ((control->base).subresource,(control->base).textureSource);
      slackWidth = (control->base).base.layoutWidth - textureSize.logicalWidthPixels;
      slackHeight = (control->base).base.layoutHeight - textureSize.logicalHeightPixels;
      if (((control->base).panelFlags & UI_IMAGE_PANEL_ALIGN_RIGHT) != 0) {
        drawX = drawX + slackWidth;
      }
      if (((control->base).panelFlags & UI_IMAGE_PANEL_ALIGN_BOTTOM) != 0) {
        drawY = drawY + slackHeight;
      }
      if (((control->base).panelFlags & UI_IMAGE_PANEL_CENTER_X) != 0) {
        drawX = drawX + (slackWidth >> 1);
      }
      if (((control->base).panelFlags & UI_IMAGE_PANEL_CENTER_Y) != 0) {
        drawY = drawY + (slackHeight >> 1);
      }
      framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
      if (!framebufferUnavailable) {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clippedBottom,clippedRight,clippedTop,clippedLeft,drawY,drawX,(control->base).subresource,
                   (control->base).textureSource,g_FramebufferAccess);
        g_GraphicsFramebufferEndAccess();
        savedPanelData = g_SelectionPanelData;
        savedTextureSource = g_SelectionPanelTextureSource;
        /* the original writes both straight back here */
        g_SelectionPanelTextureSource = savedTextureSource;
        g_SelectionPanelData = savedPanelData;
        if (control->entity != NULL) {
          g_SelectionPanelTextureSource = g_InfoPanelTextureSource;
          g_SelectionPanelData = g_InfoPanelData;
          SelectionPanel_RenderArmyRuntimeMetrics
                    (clipBottom,clipRight,clipTop,clipLeft,(control->base).base.bottom,(control->base).base.right,
                     (control->base).base.top,(control->base).base.left,control->entity);
          g_SelectionPanelTextureSource = savedTextureSource;
          g_SelectionPanelData = savedPanelData;
        }
      }
      UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base.base);
    }
  }
  return;
}


/* Draws a software texture preview (drawClipped slot of g_UiSoftwareTexturePreviewControlVtable): the
   outgoing and incoming subresources blended bilinearly and scaled over the layout box (a crossfade driven by
   the control's blend buffers), then the children.
*/
void UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiSoftwareTexturePreviewControl *control)

{
  Bool8 framebufferUnavailable;
  
  if ((((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (control->textureSource != NULL)) {
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      SoftwareTexture_BilinearBlendScaleSubresources
                ((control->base).layoutHeight,(control->base).layoutWidth,(control->base).top,(control->base).left,
                 control->blendedSourcePixels,control->blendFactorPixels,
                 control->incomingSubresource,
                 control->outgoingSubresource,(int *)control->textureSource,
                 (int *)g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
    }
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
  return;
}


/* Primary button press on a software texture preview (nonRightPress slot of
   g_UiSoftwareTexturePreviewControlVtable): queues the control's action.
*/
void UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control)

{
  UiActionQueue_Enqueue(control->actionId,control);
  return;
}


/* Secondary button press on a software texture preview (rightPress slot of
   g_UiSoftwareTexturePreviewControlVtable): queues the control's action, like the primary button.
*/
void UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control)

{
  UiActionQueue_Enqueue(control->actionId,control);
  return;
}


/* Keyboard handler of a software texture preview (keyboardEvent slot of
   g_UiSoftwareTexturePreviewControlVtable): Tab moves the focus on, any other key queues the control's action.
   Always consumed (returns false).
*/
Bool8 UiSoftwareTexturePreviewControl_HandleKeyboardActivation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSoftwareTexturePreviewControl *control)

{
  if (keyCode != KEYBOARD_KEY_CODE_TAB) {
    UiActionQueue_Enqueue(control->actionId,control);
    return false;
  }
  UiKeyboardFocus_MoveNext();
  return false;
}


/* The tooltip-eligible node under the pointer in the top root, or NULL: none while a node holds the pointer
   capture, no root is open, the pointer is outside the root's box or the hit node is not eligible. */
static UiNodeBase *UiTooltip_FindEligibleNodeAt(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiNodeBase *hitTestNode;

  if ((g_UiPointerCaptureTarget != UI_NODE_NONE) || (g_UiRootNode == UI_ROOT_STACK_END)) {
    return NULL;
  }
  if (((g_UiRootNode->base).left > pointerX) || ((g_UiRootNode->base).top > pointerY) ||
      (pointerX >= (g_UiRootNode->base).right) || (pointerY >= (g_UiRootNode->base).bottom)) {
    return NULL;
  }
  hitTestNode = (*((g_UiRootNode->base).vtable)->hitTest)(pointerY,pointerX,&g_UiRootNode->base);
  if ((hitTestNode == UI_NODE_NONE) || ((hitTestNode->nodeFlags & UI_NODE_TOOLTIP_ELIGIBLE) == 0)) {
    return NULL;
  }
  return hitTestNode;
}


/* Tracks which node the tooltip belongs to: remembers the pointer position and takes the node under the
   pointer in the top root (only while no button holds a capture, and only tooltip-eligible nodes). When the
   target changes, the tooltip delay starts over and the text of the previous target is prepared again.
*/
void UiTooltip_UpdateHoverTarget(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiNodeBase *previousTarget;
  UiNodeBase *hoveredNode;

  previousTarget = g_UiTooltipState.targetNode;
  g_UiTooltipState.pointerX = pointerX;
  g_UiTooltipState.pointerY = pointerY;
  g_UiTooltipState.targetNode = NULL;
  hoveredNode = UiTooltip_FindEligibleNodeAt(pointerY,pointerX);
  if (hoveredNode != NULL) {
    g_UiTooltipState.targetNode = hoveredNode;
    if (hoveredNode == previousTarget) {
      return;
    }
  }
  g_UiTooltipState.countdownFrames = g_UiTooltipDelayFrames;
  UiTooltip_PrepareTargetText(previousTarget);
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


/* Prepares the tooltip of node (NULL: nothing to do): the dword stored just before the node is its tooltip,
   either a UTF-16 text pointer (UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) or a text resource id. The text is
   measured in the tooltip style and the UI is redrawn.
*/
void UiTooltip_PrepareTargetText(UiNodeBase *node)

{
  uint16_t *commandStream;
  uint16_t *resolvedText;

  if (node != NULL) {
    /* the last field of the (virtual) node before this one = the dword at node - 4 */
    commandStream = (uint16_t *)node[-1].nodeFlags;
    if ((node->nodeFlags & UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)commandStream);
      commandStream = resolvedText;
    }
    RichTextCommandStream_MeasureLine(g_UiTooltipTextStyle,commandStream);
    /* the size of window piece 0xBC is queried but not used */
    g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TOOLTIP_LEFT,g_UiWindowTextureSource);
    UiRootStack_InvalidateAll();
  }
  return;
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
                        (g_UiTextEditActiveTextStyle,(uint32_t)control->textBuffer[glyphIndex],NULL);
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
                      (g_UiTextEditActiveTextStyle,(uint32_t)control->textBuffer[currentTextIndex],NULL);
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


/* Compares two rich-text streams for the pointer-list sorts: both are expanded (nested streams inlined) into
   1 KiB scratch buffers and compared with Utf16String_CompareAsciiCaseInsensitiveFlags. Returns the
   comparator's order of leftText relative to rightText: -1 when less, 0 when equal, 1 when greater (a string
   that ends first compares as equal-or-greater, see the comparator). Called by
   UiPointerList_SortByExpandedTextFieldAscending.
*/
int UiPointerList_CompareExpandedText(uint16_t *rightText,uint16_t *leftText)

{
  RichTextCommandStream_CopyExpanded
            (UI_POINTER_LIST_COMPARE_SCRATCH_BYTES,g_UiPointerListExpandedLeftTextUtf16,leftText,NULL);
  RichTextCommandStream_CopyExpanded
            (UI_POINTER_LIST_COMPARE_SCRATCH_BYTES,g_UiPointerListExpandedRightTextUtf16,rightText,NULL);
  /* The order is the comparator's result, unchanged. */
  return (*(int (*)(uint16_t *,uint16_t *))g_Utf16StringCompareAsciiCaseInsensitiveFlags)
            (g_UiPointerListExpandedRightTextUtf16,g_UiPointerListExpandedLeftTextUtf16);
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
    glyphWidth = FontGlyph_GetLogicalSizeActiveFont((uint32_t)control->textBuffer[prefixLength - 1],NULL);
    maxScrollOffset = cursorWidth - glyphWidth;
  }
  cursorOverflow = cursorWidth - (control->base).layoutWidth;
  if (control->textBuffer[prefixLength] != 0) {
    glyphWidth = FontGlyph_GetLogicalSizeActiveFont((uint32_t)control->textBuffer[prefixLength],NULL);
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


/* Draws a text button (drawClipped slot of g_UiTextButtonControlVtable; also called by the adapter, numeric-pair
   and payload-pair buttons after patching their text): the push-button or checkbox graphic for its state
   (pressed/checked, alternate, disabled), then its text 6 pixels right of the graphic, vertically centred,
   with the focus mark and its shadow while it has keyboard focus. Children are not drawn.
*/
void UiTextButtonControl_DrawClipped(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextButtonControl *control)

{
  uint16_t *commandStream;
  uint32_t buttonFrame;
  int focusMarkTop;
  int focusTileX;
  int focusMarkRightX;
  uint32_t textStyle;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize buttonSize;
  GraphicsTextureLogicalSize rightCapSize;
  GraphicsTextureLogicalSize leftCapSize;
  GraphicsTextureLogicalSize middleTileSize;
  int textX;
  int textY;

  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  buttonFrame = UI_WINDOW_SUBRESOURCE_PUSH_BUTTON;
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    buttonFrame = UI_WINDOW_SUBRESOURCE_PUSH_BUTTON + 2;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
    buttonFrame = buttonFrame - (UI_WINDOW_SUBRESOURCE_PUSH_BUTTON - UI_WINDOW_SUBRESOURCE_CHECKBOX);
    if (((control->selectable).stateFlags & UI_BUTTON_ALTERNATE_STATE) != 0) {
      buttonFrame = UI_WINDOW_SUBRESOURCE_CHECKBOX + 4;
    }
  }
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    if (((control->selectable).stateFlags & UI_BUTTON_HIDDEN_WHILE_SUPPRESSED) != 0) {
      g_GraphicsFramebufferEndAccess();
      return;
    }
    buttonFrame++;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
             (control->selectable).base.left,buttonFrame,g_UiWindowTextureSource,g_FramebufferAccess);
  buttonSize = g_GraphicsTextureSourceGetLogicalSize(buttonFrame,g_UiWindowTextureSource);
  /* text offsets inside the control: 6 pixels right of the graphic, vertically centred on it */
  textX = buttonSize.logicalWidthPixels + 6;
  commandStream = TextResource_Resolve(control->textResourceId);
  textExtent = RichTextCommandStream_MeasureLine(control->packedTextStyle,commandStream);
  textY = (int)(buttonSize.logicalHeightPixels - textExtent.heightPixels) >> 1;
  textStyle = g_UiTextStyleDisabled;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    textStyle = g_UiTextStyleNormal;
    if (((control->selectable).stateFlags & UI_BUTTON_ALTERNATE_STATE) != 0) {
      textStyle = g_UiTextStyleAlternate;
    }
  }
  /* packedTextStyle may override the font byte (bits 24-31) and the palette byte (bits 16-23) */
  if (((control->selectable).stateFlags & UI_BUTTON_OWN_STYLE_FONT) == 0) {
    control->packedTextStyle = control->packedTextStyle & UI_TEXT_STYLE_PALETTE_BYTE;
  }
  else {
    textStyle = textStyle & ~UI_TEXT_STYLE_FONT_BYTE;
  }
  if (((control->selectable).stateFlags & UI_BUTTON_OWN_STYLE_PALETTE) == 0) {
    control->packedTextStyle = control->packedTextStyle & UI_TEXT_STYLE_FONT_BYTE;
  }
  else {
    textStyle = textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE;
  }
  textStyle = textStyle | control->packedTextStyle;
  if ((((control->selectable).base.nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) ||
     (((control->selectable).stateFlags & UI_BUTTON_NO_FOCUS_MARK) != 0)) {
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,
               textY + (control->selectable).base.top,textX + (control->selectable).base.left);
  }
  else {
    textX = textX + (control->selectable).base.left;
    textY = textY + (control->selectable).base.top;
    /* focus mark shadow: left cap, right cap, then the middle tiles up to the right cap */
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,textY,textX - 2,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    rightCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                         g_UiWindowTextureSource);
    leftCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                        g_UiWindowTextureSource);
    focusTileX = textX - 2 + leftCapSize.logicalWidthPixels;
    middleTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                           g_UiWindowTextureSource);
    focusMarkRightX = ((textX + 4) - rightCapSize.logicalWidthPixels) + textExtent.widthPixels;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,textY,focusMarkRightX,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    if (clipRight < focusMarkRightX) {
      focusMarkRightX = clipRight;
    }
    do {
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,focusMarkRightX,clipTop,clipLeft,textY,focusTileX,TEXT_SHADOW_COLOR_ARGB,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      focusTileX = focusTileX + middleTileSize.logicalWidthPixels;
    } while (focusTileX < focusMarkRightX);
    focusMarkTop = textY - 1;
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,textY,textX);
    /* the focus mark itself, one pixel up and to the left of its shadow */
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,focusMarkTop,textX - 3,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    rightCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                         g_UiWindowTextureSource);
    leftCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                        g_UiWindowTextureSource);
    focusTileX = textX - 3 + leftCapSize.logicalWidthPixels;
    middleTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                           g_UiWindowTextureSource);
    focusMarkRightX = ((textX + 3) - rightCapSize.logicalWidthPixels) + textExtent.widthPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,focusMarkTop,focusMarkRightX,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,g_UiWindowTextureSource,
               g_FramebufferAccess);
    if (clipRight < focusMarkRightX) {
      focusMarkRightX = clipRight;
    }
    do {
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,focusMarkRightX,clipTop,clipLeft,focusMarkTop,focusTileX,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      focusTileX = focusTileX + middleTileSize.logicalWidthPixels;
    } while (focusTileX < focusMarkRightX);
  }
  g_GraphicsFramebufferEndAccess();
}


/* Class vtables. */

UiNodeVtable g_UiGraphicsAdapterTextButtonVtable = {
        .relocate = (void *)UiTextButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiGraphicsAdapterTextButton_DrawFormattedAdapterText,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiTextButtonControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

UiNodeVtable g_UiFramedTextButtonControlVtable = {
        .relocate = (void *)UiFramedTextButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiFramedTextButtonControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiFramedTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiFramedTextButtonControl_NonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiFramedTextButtonControl_NonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiFramedTextButtonControl_HitTestRect,
        .keyboardEvent = (void *)UiSelectableControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

UiNodeVtable g_UiWindowControlVtable = {
        .relocate = (void *)UiWindowControl_RelocateWithFrameInset,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiWindowControl_DrawFramedTextAndChrome,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiFramedTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiFramedTextButtonControl_NonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiFramedTextButtonControl_NonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiFramedTextButtonControl_HitTestRect,
        .keyboardEvent = (void *)UiSelectableControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

UiNodeVtable g_UiTextButtonControlVtable = {
        .relocate = (void *)UiTextButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiTextButtonControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiTextButtonControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

UiNodeVtable g_UiImagePanelControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiImagePanelControl_DrawAlignedTextureAndChildren,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiImagePanelControl_HitTestAlignedTextureAndChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

UiNodeVtable g_UiNumericTextEditControlVtable = {
        .relocate = (void *)UiNumericTextEditControl_RelocateAndRebuildText,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiTextEditControl_DrawTextSelectionAndCaret,
        .layout = (void *)UiTextEditControl_RecomputeLayoutAndClampScroll,
        .nonRightPress = (void *)UiTextEditControl_BeginSelectionAtPointer,
        .nonRightRelease = (void *)UiTextEditControl_EndSelection,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiTextEditControl_UpdateSelectionFromPointer,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNumericTextEditControl_HandleKeyboardAndCommit,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiTextEditControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiTextEditControl_UnsuppressIfActionId,
        .tick = (void *)UiTextEditControl_TickCaretBlink,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

UiNodeVtable g_UiPathTextEditControlVtable = {
        .relocate = (void *)UiPathTextEditControl_RelocateAndValidateDos83,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiTextEditControl_DrawTextSelectionAndCaret,
        .layout = (void *)UiTextEditControl_RecomputeLayoutAndClampScroll,
        .nonRightPress = (void *)UiTextEditControl_BeginSelectionAtPointer,
        .nonRightRelease = (void *)UiTextEditControl_EndSelection,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiTextEditControl_UpdateSelectionFromPointer,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiPathTextEditControl_HandleKeyboardAndValidate,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiTextEditControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiTextEditControl_UnsuppressIfActionId,
        .tick = (void *)UiTextEditControl_TickCaretBlink,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

UiNodeVtable g_UiRequiredTextEditControlVtable = {
        .relocate = (void *)UiRequiredTextEditControl_RelocateAndValidateNonEmpty,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiTextEditControl_DrawTextSelectionAndCaret,
        .layout = (void *)UiTextEditControl_RecomputeLayoutAndClampScroll,
        .nonRightPress = (void *)UiTextEditControl_BeginSelectionAtPointer,
        .nonRightRelease = (void *)UiTextEditControl_EndSelection,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiTextEditControl_UpdateSelectionFromPointer,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiRequiredTextEditControl_HandleKeyboardAndValidate,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiTextEditControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiTextEditControl_UnsuppressIfActionId,
        .tick = (void *)UiTextEditControl_TickCaretBlink,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

UiNodeVtable g_UiTextListControlVtable = {
    .relocate = (void *)UiContainer_RelocateChildren,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiTextListControl_DrawRowsAndSelection,
    .layout = (void *)UiContainer_LayoutChildren,
    .nonRightPress = (void *)UiTextListControl_SelectRowFromPointer,
    .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
    .rightPress = (void *)UiNode_ForwardRightPressToParent,
    .rightRelease = (void *)UiNode_DefaultRightRelease,
    .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
    .rightDrag = (void *)UiNode_DefaultRightDrag,
    .pointerMove = (void *)UiNode_DefaultPointerMove,
    .hitTest = (void *)UiContainer_HitTestChildren,
    .keyboardEvent = (void *)UiTextListControl_HandleKeyboardNavigationAndSearch,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiTextListControl_SuppressIfActionId,
    .unsuppressActionId = (void *)UiTextListControl_UnsuppressIfActionId,
    .tick = (void *)UiTextListControl_TickActivationPulse,
    .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

UiNodeVtable g_UiNineSlicePanelControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiNineSlicePanelControl_DrawTextureFrameAndChildren,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

UiNodeVtable g_UiNumericPairTextButtonVtable = {
        .relocate = (void *)UiTextButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiNumericPairTextButton_DrawFormattedValues,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiTextButtonControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

UiNodeVtable g_UiPayloadPairTextButtonVtable = {
        .relocate = (void *)UiTextButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiPayloadPairTextButton_DrawFormattedPayloads,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiTextButtonControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

UiNodeVtable g_UiFormattedContainerVtable = {
        .relocate = (void *)UiFormattedContainer_RelocateWithPatchedTextPayloads,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiFormattedContainer_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

UiNodeVtable g_UiArmyMetricsPanelVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiArmyMetricsPanel_DrawTextureMetricsAndChildren,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiImagePanelControl_HitTestAlignedTextureAndChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

UiNodeVtable g_UiSoftwareTexturePreviewControlVtable = {
    .relocate = (void *)UiContainer_RelocateChildren,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren,
    .layout = (void *)UiContainer_LayoutChildren,
    .nonRightPress = (void *)UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress,
    .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
    .rightPress = (void *)UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress,
    .rightRelease = (void *)UiNode_DefaultRightRelease,
    .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
    .rightDrag = (void *)UiNode_DefaultRightDrag,
    .pointerMove = (void *)UiNode_DefaultPointerMove,
    .hitTest = (void *)UiContainer_HitTestChildren,
    .keyboardEvent = (void *)UiSoftwareTexturePreviewControl_HandleKeyboardActivation,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiContainer_SuppressActionId,
    .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
    .tick = (void *)UiNode_DefaultTick,
    .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};
