/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/text.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/text.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/controls/text. */

/* Address: 0x004B0200.
   Per-frame tooltip delay: while the pointer rests on an enabled control (no button held), counts the
   delay down and prepares the tooltip text when it expires. While a node has captured the pointer or the
   hovered control is disabled, re-evaluates the hover target at the last pointer position instead.
*/
void UiTooltip_TickCountdown(void)

{
  if ((g_UiPointerCaptureTarget == UI_NODE_NONE) &&
     ((g_UiTooltipState.targetNode == NULL ||
      (((g_UiTooltipState.targetNode)->nodeFlags & UI_NODE_SUPPRESSED) == 0)))) {
    if ((g_UiTooltipState.countdownFrames != 0) &&
       (g_UiTooltipState.countdownFrames--,
       g_UiTooltipState.countdownFrames == 0)) {
      UiTooltip_PrepareTargetText(g_UiTooltipState.targetNode);
    }
    return;
  }
  UiTooltip_UpdateHoverTarget(g_UiTooltipState.pointerY,g_UiTooltipState.pointerX);
  return;
}


/* Address: 0x004B5F20.
   Keyboard handler of the numeric text edit (keyboardEvent slot of g_UiNumericTextEditControlVtable): inserts
   digits, '-' (signed values) and A-F (hexadecimal values), edits and moves the cursor and Shift selection,
   and after every handled key parses and commits the value. Enter queues the action when the control acts on
   Enter only; everything else goes to UiNode_DefaultKeyboardEventMoveFocusNext. CF clear: consumed.
*/
bool UiNumericTextEditControl_HandleKeyboardAndCommit(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiNumericTextControl *control)

{
  UiTextCodeUnitIndex *selectionBoundary;
  uint16_t displacedCodeUnit;
  bool characterAccepted;
  bool recomputeLayout;
  bool normalizeSelection;
  UiTextCodeUnitIndex codeUnitIndex;
  int countOrIndex;
  int shiftCountOrScanIndex;
  uint32_t insertIndex;
  uint16_t *sourceCursor;
  uint16_t *destinationCursor;
  bool delegatedResult;

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
      codeUnitIndex = control->selectionEnd;
      countOrIndex = codeUnitIndex - control->selectionStart;
      sourceCursor = control->textBuffer + codeUnitIndex;
      destinationCursor = control->textBuffer + control->selectionStart;
      for (shiftCountOrScanIndex = UI_NUMERIC_TEXT_BUFFER_UNITS - codeUnitIndex; shiftCountOrScanIndex != 0;
           shiftCountOrScanIndex--) {
        *destinationCursor = *sourceCursor;
        sourceCursor++;
        destinationCursor++;
      }
      for (; countOrIndex != 0; countOrIndex--) {
        *destinationCursor = 0;
        destinationCursor++;
      }
      insertIndex = control->selectionStart;
      control->cursorIndex = insertIndex;
      control->selectionEnd = insertIndex;
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
          codeUnitIndex = control->selectionEnd;
          countOrIndex = codeUnitIndex - control->selectionStart;
          sourceCursor = control->textBuffer + codeUnitIndex;
          destinationCursor = control->textBuffer + control->selectionStart;
          for (shiftCountOrScanIndex = UI_NUMERIC_TEXT_BUFFER_UNITS - codeUnitIndex; shiftCountOrScanIndex != 0;
               shiftCountOrScanIndex--) {
            *destinationCursor = *sourceCursor;
            sourceCursor++;
            destinationCursor++;
          }
          for (; countOrIndex != 0; countOrIndex--) {
            *destinationCursor = 0;
            destinationCursor++;
          }
          control->cursorIndex = control->selectionStart;
          control->selectionEnd = control->selectionStart;
        }
        else if (keyCode == KEYBOARD_KEY_CODE_BACKSPACE) {
          /* Backspace: remove the code unit before the cursor. */
          if (control->cursorIndex == 0) {
            recomputeLayout = false;
            break;
          }
          sourceCursor = control->textBuffer + codeUnitIndex;
          destinationCursor = control->textBuffer + (codeUnitIndex - 1);
          for (countOrIndex = UI_NUMERIC_TEXT_BUFFER_UNITS - codeUnitIndex; countOrIndex != 0; countOrIndex--) {
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
          for (countOrIndex = UI_NUMERIC_TEXT_BUFFER_UNITS - 1 - codeUnitIndex; countOrIndex != 0; countOrIndex--) {
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
          g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
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
          shiftCountOrScanIndex = codeUnitIndex + 1;
          codeUnitIndex++;
        } while (control->textBuffer[shiftCountOrScanIndex] != 0);
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
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
  }
  return false;
}


/* Address: 0x004B68C0.
   Keyboard handler of the DOS path edit (keyboardEvent slot of g_UiPathTextEditControlVtable): inserts the
   characters a DOS 8.3 path may contain, edits and moves the cursor and Shift selection, and Ctrl+Left/Right
   jump between path segments. After every handled key the path is validated and, unless the control acts on
   Enter only, its action is queued. Unhandled keys go to UiNode_DefaultKeyboardEventMoveFocusNext.
   CF clear: consumed.
*/
bool UiPathTextEditControl_HandleKeyboardAndValidate(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiPathTextEditControl *control)

{
  UiTextCodeUnitIndex *selectionBoundary;
  uint16_t displacedCodeUnit;
  UiTextCodeUnitIndex segmentBoundaryIndex;
  UiTextCodeUnitIndex codeUnitIndex;
  int countOrIndex;
  int shiftCountOrScanIndex;
  uint32_t insertIndex;
  uint16_t *sourceCursor;
  uint16_t *destinationCursor;
  bool isAltGrCharacter;
  bool characterAccepted;
  bool recomputeLayout;
  bool normalizeSelection;
  bool delegatedResult;

  if (((control->editStateFlags & UI_TEXT_EDIT_READ_ONLY) != 0) ||
     (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
    delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return delegatedResult;
  }
  /* Characters typed with AltGr (Ctrl+Alt) on a German keyboard skip the modifier checks: @ | ~ { [ ] }
     backslash and, in Windows-1252, 0xB2/0xB3 (superscript two/three), 0xB5 (micro sign) and 0x80 (euro
     sign). Of these, only the backslash passes the path character filter below. */
  isAltGrCharacter =
       (((keyCode == '@') ||
        ((((keyCode == '|' || (keyCode == '~')) || (keyCode == 0xb2)) ||
         ((keyCode == 0xb3 || (keyCode == '{')))))) || (keyCode == '[')) ||
       (((keyCode == ']' || (keyCode == '}')) ||
        ((keyCode == '\\' || ((keyCode == 0xb5 || (keyCode == 0x80)))))));
  /* Letters typed with Ctrl or Alt are shortcuts, not text. */
  if (!isAltGrCharacter &&
      ((keyboardStateMask & KEYBOARD_STATE_ALT) != 0 ||
       ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0 &&
        (keyboardStateMask & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0 &&
        '@' < keyCode && (keyCode < '[' || ('`' < keyCode && keyCode < '{'))))) {
    delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return delegatedResult;
  }
  /* Handled keys end in the shared tail: optional selection normalization and layout recompute,
     then validity update, action, invalidate and the interaction sound. */
  recomputeLayout = true;
  normalizeSelection = false;
  if ((isAltGrCharacter) || ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0)) {
    /* Path characters: letters, digits, '-' and '.'; '*' and '?' only with UI_PATH_TEXT_ALLOW_WILDCARDS,
       ':' and the backslash not with UI_PATH_TEXT_NAME_ONLY. */
    if ((keyCode == '*') || (keyCode == '?')) {
      characterAccepted = (control->editStateFlags & UI_PATH_TEXT_ALLOW_WILDCARDS) != 0;
    }
    else if ((keyCode == '-') || (keyCode == '.')) {
      characterAccepted = true;
    }
    else if (keyCode < '0') {
      characterAccepted = false;
    }
    else if (keyCode <= '9') {
      characterAccepted = true;
    }
    else if ((keyCode == ':') || (keyCode == '\\')) {
      characterAccepted = (control->editStateFlags & UI_PATH_TEXT_NAME_ONLY) == 0;
    }
    else if (keyCode < 'A') {
      characterAccepted = false;
    }
    else if (keyCode <= 'Z') {
      characterAccepted = true;
    }
    else {
      characterAccepted = ('a' <= keyCode) && (keyCode <= 'z');
    }
    if (!characterAccepted) {
      delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
      return delegatedResult;
    }
    insertIndex = control->cursorIndex;
    if ((insertIndex != control->selectionStart) || (insertIndex != control->selectionEnd)) {
      codeUnitIndex = control->selectionEnd;
      countOrIndex = codeUnitIndex - control->selectionStart;
      sourceCursor = control->pathBuffer + codeUnitIndex;
      destinationCursor = control->pathBuffer + control->selectionStart;
      for (shiftCountOrScanIndex = UI_PATH_TEXT_BUFFER_UNITS - codeUnitIndex; shiftCountOrScanIndex != 0;
           shiftCountOrScanIndex--) {
        *destinationCursor = *sourceCursor;
        sourceCursor++;
        destinationCursor++;
      }
      for (; countOrIndex != 0; countOrIndex--) {
        *destinationCursor = 0;
        destinationCursor++;
      }
      insertIndex = control->selectionStart;
      control->cursorIndex = insertIndex;
      control->selectionEnd = insertIndex;
    }
    if (insertIndex < UI_PATH_TEXT_BUFFER_UNITS - 2) {
      control->cursorIndex++;
      control->selectionStart++;
      control->selectionEnd++;
      if ((control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) == 0) {
        do {
          LOCK();
          displacedCodeUnit = control->pathBuffer[insertIndex];
          control->pathBuffer[insertIndex] = (uint16_t)keyCode;
          keyCode = (UiKeyboardEventCode)displacedCodeUnit;
          UNLOCK();
          insertIndex++;
        } while (insertIndex < UI_PATH_TEXT_BUFFER_UNITS - 2);
      }
      else {
        control->pathBuffer[insertIndex] = (uint16_t)keyCode;
      }
    }
  }
  else if ((keyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
    if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) == 0) {
      switch (keyCode) {
      case KEYBOARD_KEY_CODE_BACKSPACE:
      case KEYBOARD_KEY_CODE_DELETE:
        codeUnitIndex = control->cursorIndex;
        if ((codeUnitIndex != control->selectionStart) || (codeUnitIndex != control->selectionEnd)) {
          /* Backspace/Delete with a selection: remove the selected range, zero-fill the tail. */
          codeUnitIndex = control->selectionEnd;
          countOrIndex = codeUnitIndex - control->selectionStart;
          sourceCursor = control->pathBuffer + codeUnitIndex;
          destinationCursor = control->pathBuffer + control->selectionStart;
          for (shiftCountOrScanIndex = UI_PATH_TEXT_BUFFER_UNITS - codeUnitIndex; shiftCountOrScanIndex != 0;
               shiftCountOrScanIndex--) {
            *destinationCursor = *sourceCursor;
            sourceCursor++;
            destinationCursor++;
          }
          for (; countOrIndex != 0; countOrIndex--) {
            *destinationCursor = 0;
            destinationCursor++;
          }
          control->cursorIndex = control->selectionStart;
          control->selectionEnd = control->selectionStart;
        }
        else if (keyCode == KEYBOARD_KEY_CODE_BACKSPACE) {
          /* Backspace: remove the code unit before the cursor. */
          if (control->cursorIndex == 0) {
            recomputeLayout = false;
            break;
          }
          sourceCursor = control->pathBuffer + codeUnitIndex;
          destinationCursor = control->pathBuffer + (codeUnitIndex - 1);
          for (countOrIndex = UI_PATH_TEXT_BUFFER_UNITS - codeUnitIndex; countOrIndex != 0; countOrIndex--) {
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
          if (control->pathBuffer[codeUnitIndex] == 0) {
            recomputeLayout = false;
            break;
          }
          sourceCursor = control->pathBuffer + codeUnitIndex + 1;
          destinationCursor = control->pathBuffer + codeUnitIndex;
          for (countOrIndex = UI_PATH_TEXT_BUFFER_UNITS - 1 - codeUnitIndex; countOrIndex != 0; countOrIndex--) {
            *destinationCursor = *sourceCursor;
            sourceCursor++;
            destinationCursor++;
          }
        }
        break;
      case KEYBOARD_KEY_CODE_INSERT:
        control->editStateFlags = control->editStateFlags ^ UI_TEXT_EDIT_OVERWRITE_MODE;
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
          while (control->pathBuffer[control->cursorIndex] != 0) {
            control->cursorIndex++;
          }
        }
        else if (keyCode == KEYBOARD_KEY_CODE_LEFT) {
          if (control->cursorIndex != 0) {
            control->cursorIndex--;
          }
        }
        else if (control->pathBuffer[control->cursorIndex] != 0) {
          control->cursorIndex++;
        }
        control->selectionStart = control->cursorIndex;
        control->selectionEnd = control->cursorIndex;
        break;
      case KEYBOARD_KEY_CODE_ENTER:
        if ((control->editStateFlags & UI_TEXT_EDIT_ACTION_ON_ENTER_ONLY) == 0) {
          delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
          return delegatedResult;
        }
        UiActionQueue_Enqueue(control->actionId,control);
        if (((control->editStateFlags & UI_TEXT_EDIT_PLAY_INTERACTION_SOUND) != 0) &&
           (control->activationSound != NULL)) {
          g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
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
        if (control->pathBuffer[codeUnitIndex] == 0) {
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
          shiftCountOrScanIndex = codeUnitIndex + 1;
          codeUnitIndex++;
        } while (control->pathBuffer[shiftCountOrScanIndex] != 0);
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
        if (control->pathBuffer[codeUnitIndex] == 0) {
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
    }
  }
  else {
    /* Ctrl+Left/Right: jump to the previous/next path segment ('\' or '.'), Shift extends. */
    if ((keyCode != KEYBOARD_KEY_CODE_LEFT) && (keyCode != KEYBOARD_KEY_CODE_RIGHT)) {
      delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
      return delegatedResult;
    }
    if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) == 0) {
      if (keyCode == KEYBOARD_KEY_CODE_LEFT) {
        codeUnitIndex = control->cursorIndex;
        if (codeUnitIndex != 0) {
          segmentBoundaryIndex = 0;
          if (codeUnitIndex != 1) {
            for (countOrIndex = codeUnitIndex - 2;
                ((segmentBoundaryIndex = 0, countOrIndex != 0 && (segmentBoundaryIndex = countOrIndex + 1,
                                                                  control->pathBuffer[countOrIndex] != '\\'))
                && (control->pathBuffer[countOrIndex] != '.')); countOrIndex--) {
            }
          }
          control->cursorIndex = segmentBoundaryIndex;
          control->selectionStart = segmentBoundaryIndex;
          control->selectionEnd = segmentBoundaryIndex;
        }
      }
      else {
        codeUnitIndex = control->cursorIndex;
        do {
          segmentBoundaryIndex = codeUnitIndex;
          if ((control->pathBuffer[codeUnitIndex] == 0) ||
             (segmentBoundaryIndex = codeUnitIndex + 1, control->pathBuffer[codeUnitIndex] == '\\')) break;
          sourceCursor = control->pathBuffer + codeUnitIndex;
          codeUnitIndex = segmentBoundaryIndex;
        } while (*sourceCursor != '.');
        control->cursorIndex = segmentBoundaryIndex;
        control->selectionStart = segmentBoundaryIndex;
        control->selectionEnd = segmentBoundaryIndex;
      }
    }
    else if (keyCode == KEYBOARD_KEY_CODE_LEFT) {
      codeUnitIndex = control->cursorIndex;
      if (codeUnitIndex != 0) {
        /* The selection end at the cursor moves. */
        selectionBoundary = &control->selectionStart;
        if (codeUnitIndex != control->selectionStart) {
          selectionBoundary = &control->selectionEnd;
        }
        segmentBoundaryIndex = 0;
        if (codeUnitIndex != 1) {
          for (shiftCountOrScanIndex = codeUnitIndex - 2;
              ((segmentBoundaryIndex = 0, shiftCountOrScanIndex != 0 &&
                (segmentBoundaryIndex = shiftCountOrScanIndex + 1,
                 control->pathBuffer[shiftCountOrScanIndex] != '\\')) &&
              (control->pathBuffer[shiftCountOrScanIndex] != '.')); shiftCountOrScanIndex--) {
          }
        }
        control->cursorIndex = segmentBoundaryIndex;
        *selectionBoundary = segmentBoundaryIndex;
        normalizeSelection = true;
      }
    }
    else {
      codeUnitIndex = control->cursorIndex;
      selectionBoundary = &control->selectionStart;
      if (codeUnitIndex != control->selectionStart) {
        selectionBoundary = &control->selectionEnd;
      }
      do {
        segmentBoundaryIndex = codeUnitIndex;
        if ((control->pathBuffer[codeUnitIndex] == 0) ||
           (segmentBoundaryIndex = codeUnitIndex + 1, control->pathBuffer[codeUnitIndex] == '\\')) break;
        sourceCursor = control->pathBuffer + codeUnitIndex;
        codeUnitIndex = segmentBoundaryIndex;
      } while (*sourceCursor != '.');
      control->cursorIndex = segmentBoundaryIndex;
      *selectionBoundary = segmentBoundaryIndex;
      normalizeSelection = true;
    }
  }
  if ((normalizeSelection) && (control->selectionEnd < control->selectionStart)) {
    /* Keep selectionStart <= selectionEnd when the moving end crossed the anchor. */
    LOCK();
    codeUnitIndex = control->selectionEnd;
    control->selectionEnd = control->selectionStart;
    UNLOCK();
    control->selectionStart = codeUnitIndex;
  }
  if (recomputeLayout) {
    UiTextEditControl_RecomputeLayoutAndClampScroll((UiTextEditControl *)control);
  }
  UiPathTextControl_UpdateDos83Validity(control);
  if ((control->editStateFlags & UI_TEXT_EDIT_ACTION_ON_ENTER_ONLY) == 0) {
    UiActionQueue_Enqueue(control->actionId,control);
  }
  UiNode_InvalidateRoot(&control->base);
  if (((control->editStateFlags & UI_TEXT_EDIT_PLAY_INTERACTION_SOUND) != 0) &&
     (control->activationSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
  }
  return false;
}


/* Address: 0x004B7110.
   Keyboard handler of the free-text edit that must not stay empty (keyboardEvent slot of
   g_UiRequiredTextEditControlVtable): inserts any character, edits and moves the cursor and Shift selection,
   Ctrl+Left/Right jump between space-separated words, and Escape clears the text when the control allows it.
   After every handled key the non-empty validity is updated and, unless the control acts on Enter only, its
   action is queued. Unhandled keys go to UiNode_DefaultKeyboardEventMoveFocusNext. CF clear: consumed.
*/
bool UiRequiredTextEditControl_HandleKeyboardAndValidate
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiRequiredTextEditControl *control)

{
  UiTextCodeUnitIndex *selectionBoundary;
  uint16_t displacedCodeUnit;
  UiTextCodeUnitIndex codeUnitIndex;
  UiTextCodeUnitIndex wordBoundaryIndex;
  UiTextCodeUnitIndex scanIndex;
  UiTextCodeUnitCount remainingCodeUnits;
  int countOrIndex;
  int shiftCountOrScanIndex;
  uint32_t insertLimit;
  uint32_t insertIndex;
  uint16_t *sourceCursor;
  uint16_t *destinationCursor;
  bool isAltGrCharacter;
  bool recomputeLayout;
  bool normalizeSelection;
  bool delegatedResult;

  insertIndex = control->cursorIndex;
  if (((control->editStateFlags & UI_REQUIRED_TEXT_READ_ONLY) != 0) ||
     (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
    delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return delegatedResult;
  }
  /* Characters typed with AltGr (Ctrl+Alt, see the path edit above) are inserted without the modifier
     checks. */
  isAltGrCharacter =
       (((keyCode == '@') ||
        ((((keyCode == '|' || (keyCode == '~')) || (keyCode == 0xb2)) ||
         ((keyCode == 0xb3 || (keyCode == '{')))))) || (keyCode == '[')) ||
       (((keyCode == ']' || (keyCode == '}')) ||
        ((keyCode == '\\' || ((keyCode == 0xb5 || (keyCode == 0x80)))))));
  /* Letters typed with Ctrl or Alt are shortcuts, not text. */
  if (!isAltGrCharacter &&
      ((keyboardStateMask & KEYBOARD_STATE_ALT) != 0 ||
       ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0 &&
        (keyboardStateMask & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0 &&
        '@' < keyCode && (keyCode < '[' || ('`' < keyCode && keyCode < '{'))))) {
    delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return delegatedResult;
  }
  /* Handled keys end in the shared tail: optional selection normalization and layout recompute,
     then validity update, action, invalidate and the interaction sound. */
  recomputeLayout = true;
  normalizeSelection = false;
  if ((isAltGrCharacter) || ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0)) {
    /* Insert the character, replacing a selection. */
    insertLimit = control->bufferCapacityCodeUnits - 1;
    if ((insertIndex != control->selectionStart) || (insertIndex != control->selectionEnd)) {
      codeUnitIndex = control->selectionEnd;
      countOrIndex = codeUnitIndex - control->selectionStart;
      sourceCursor = control->textBuffer + codeUnitIndex;
      destinationCursor = control->textBuffer + control->selectionStart;
      for (shiftCountOrScanIndex = control->bufferCapacityCodeUnits - codeUnitIndex; shiftCountOrScanIndex != 0;
           shiftCountOrScanIndex--) {
        *destinationCursor = *sourceCursor;
        sourceCursor++;
        destinationCursor++;
      }
      for (; countOrIndex != 0; countOrIndex--) {
        *destinationCursor = 0;
        destinationCursor++;
      }
      insertIndex = control->selectionStart;
      control->cursorIndex = insertIndex;
      control->selectionEnd = insertIndex;
    }
    if (insertIndex < insertLimit) {
      control->cursorIndex++;
      control->selectionStart++;
      control->selectionEnd++;
      if ((control->editStateFlags & UI_REQUIRED_TEXT_OVERWRITE_MODE) == 0) {
        do {
          LOCK();
          displacedCodeUnit = control->textBuffer[insertIndex];
          control->textBuffer[insertIndex] = (uint16_t)keyCode;
          keyCode = (UiKeyboardEventCode)displacedCodeUnit;
          UNLOCK();
          insertIndex++;
        } while (insertIndex < insertLimit);
      }
      else {
        control->textBuffer[insertIndex] = (uint16_t)keyCode;
      }
    }
  }
  else if ((keyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
    if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) == 0) {
      switch (keyCode) {
      case KEYBOARD_KEY_CODE_BACKSPACE:
      case KEYBOARD_KEY_CODE_DELETE:
        codeUnitIndex = control->cursorIndex;
        if ((codeUnitIndex != control->selectionStart) || (codeUnitIndex != control->selectionEnd)) {
          /* Backspace/Delete with a selection: remove the selected range, zero-fill the tail. */
          codeUnitIndex = control->selectionEnd;
          countOrIndex = codeUnitIndex - control->selectionStart;
          sourceCursor = control->textBuffer + codeUnitIndex;
          destinationCursor = control->textBuffer + control->selectionStart;
          for (shiftCountOrScanIndex = control->bufferCapacityCodeUnits - codeUnitIndex; shiftCountOrScanIndex != 0;
               shiftCountOrScanIndex--) {
            *destinationCursor = *sourceCursor;
            sourceCursor++;
            destinationCursor++;
          }
          for (; countOrIndex != 0; countOrIndex--) {
            *destinationCursor = 0;
            destinationCursor++;
          }
          control->cursorIndex = control->selectionStart;
          control->selectionEnd = control->selectionStart;
        }
        else if (keyCode == KEYBOARD_KEY_CODE_BACKSPACE) {
          /* Backspace: remove the code unit before the cursor. */
          if (control->cursorIndex == 0) {
            recomputeLayout = false;
            break;
          }
          sourceCursor = control->textBuffer + codeUnitIndex;
          destinationCursor = control->textBuffer + (codeUnitIndex - 1);
          for (countOrIndex = control->bufferCapacityCodeUnits - codeUnitIndex; countOrIndex != 0; countOrIndex--) {
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
          countOrIndex = control->bufferCapacityCodeUnits - codeUnitIndex;
          sourceCursor = control->textBuffer + codeUnitIndex + 1;
          destinationCursor = control->textBuffer + codeUnitIndex;
          while (--countOrIndex != 0) {
            *destinationCursor = *sourceCursor;
            sourceCursor++;
            destinationCursor++;
          }
        }
        break;
      case KEYBOARD_KEY_CODE_INSERT:
        control->editStateFlags = control->editStateFlags ^ UI_REQUIRED_TEXT_OVERWRITE_MODE;
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
        if ((control->editStateFlags & UI_REQUIRED_TEXT_ACTION_ON_ENTER_ONLY) == 0) {
          delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
          return delegatedResult;
        }
        UiActionQueue_Enqueue(control->actionId,control);
        if (((control->editStateFlags & UI_REQUIRED_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
           (control->activationSound != NULL)) {
          g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
        }
        return false;
      case KEYBOARD_KEY_CODE_ESCAPE:
        if ((control->editStateFlags & UI_REQUIRED_TEXT_ESCAPE_CLEARS_AND_QUEUES_ACTION) == 0) {
          delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
          return delegatedResult;
        }
        sourceCursor = control->textBuffer;
        for (remainingCodeUnits = control->bufferCapacityCodeUnits; remainingCodeUnits != 0; remainingCodeUnits--) {
          *sourceCursor = 0;
          sourceCursor++;
        }
        control->cursorIndex = 0;
        control->selectionStart = 0;
        control->selectionEnd = 0;
        UiActionQueue_Enqueue(control->actionId,control);
        if (((control->editStateFlags & UI_REQUIRED_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
           (control->activationSound != NULL)) {
          g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
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
          shiftCountOrScanIndex = codeUnitIndex + 1;
          codeUnitIndex++;
        } while (control->textBuffer[shiftCountOrScanIndex] != 0);
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
    }
  }
  else {
    /* Ctrl+Left/Right: jump between space-separated words, Shift extends the selection. */
    if ((keyCode != KEYBOARD_KEY_CODE_LEFT) && (keyCode != KEYBOARD_KEY_CODE_RIGHT)) {
      delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
      return delegatedResult;
    }
    codeUnitIndex = control->cursorIndex;
    if ((keyboardStateMask & KEYBOARD_STATE_SHIFT) == 0) {
      if (keyCode == KEYBOARD_KEY_CODE_LEFT) {
        if (codeUnitIndex != 0) {
          if (control->textBuffer[codeUnitIndex - 1] == ' ') {
            do {
              wordBoundaryIndex = codeUnitIndex - 1;
              if (wordBoundaryIndex == 0) break;
              countOrIndex = codeUnitIndex - 2;
              codeUnitIndex = wordBoundaryIndex;
            } while (control->textBuffer[countOrIndex] == ' ');
          }
          else {
            do {
              scanIndex = codeUnitIndex;
              wordBoundaryIndex = scanIndex - 1;
              if (wordBoundaryIndex == 0) break;
              codeUnitIndex = wordBoundaryIndex;
            } while (control->textBuffer[scanIndex - 2] != ' ');
            if ((1 < (int)wordBoundaryIndex) && (control->textBuffer[scanIndex - 3] != ' ')) {
              wordBoundaryIndex = scanIndex - 2;
            }
          }
          control->cursorIndex = wordBoundaryIndex;
          control->selectionStart = wordBoundaryIndex;
          control->selectionEnd = wordBoundaryIndex;
        }
      }
      else {
        if (control->textBuffer[codeUnitIndex] == ' ') {
          for (; (control->textBuffer[codeUnitIndex] != 0 && (control->textBuffer[codeUnitIndex] == ' '));
              codeUnitIndex++) {
          }
        }
        else {
          /* Past the next space; back onto it when another space follows. */
          while (control->textBuffer[codeUnitIndex] != 0) {
            codeUnitIndex++;
            if (control->textBuffer[codeUnitIndex - 1] == ' ') {
              if (control->textBuffer[codeUnitIndex] == ' ') {
                codeUnitIndex--;
              }
              break;
            }
          }
        }
        control->cursorIndex = codeUnitIndex;
        control->selectionStart = codeUnitIndex;
        control->selectionEnd = codeUnitIndex;
      }
    }
    else if (keyCode == KEYBOARD_KEY_CODE_LEFT) {
      if (codeUnitIndex != 0) {
        /* The selection end at the cursor moves. */
        selectionBoundary = &control->selectionStart;
        if (codeUnitIndex != control->selectionStart) {
          selectionBoundary = &control->selectionEnd;
        }
        if (control->textBuffer[codeUnitIndex - 1] == ' ') {
          do {
            wordBoundaryIndex = codeUnitIndex - 1;
            if (wordBoundaryIndex == 0) break;
            shiftCountOrScanIndex = codeUnitIndex - 2;
            codeUnitIndex = wordBoundaryIndex;
          } while (control->textBuffer[shiftCountOrScanIndex] == ' ');
        }
        else {
          do {
            scanIndex = codeUnitIndex;
            wordBoundaryIndex = scanIndex - 1;
            if (wordBoundaryIndex == 0) break;
            codeUnitIndex = wordBoundaryIndex;
          } while (control->textBuffer[scanIndex - 2] != ' ');
          if ((1 < (int)wordBoundaryIndex) && (control->textBuffer[scanIndex - 3] != ' ')) {
            wordBoundaryIndex = scanIndex - 2;
          }
        }
        control->cursorIndex = wordBoundaryIndex;
        *selectionBoundary = wordBoundaryIndex;
        normalizeSelection = true;
      }
    }
    else {
      /* NOTE: as in the original, when the cursor is at selectionStart the space-skipping case is
         not taken even if the cursor is on a space. */
      selectionBoundary = &control->selectionStart;
      if ((codeUnitIndex != control->selectionStart) &&
         (selectionBoundary = &control->selectionEnd, control->textBuffer[codeUnitIndex] == ' ')) {
        for (; (control->textBuffer[codeUnitIndex] != 0 && (control->textBuffer[codeUnitIndex] == ' '));
            codeUnitIndex++) {
        }
      }
      else {
        while (control->textBuffer[codeUnitIndex] != 0) {
          codeUnitIndex++;
          if (control->textBuffer[codeUnitIndex - 1] == ' ') {
            if (control->textBuffer[codeUnitIndex] == ' ') {
              codeUnitIndex--;
            }
            break;
          }
        }
      }
      control->cursorIndex = codeUnitIndex;
      *selectionBoundary = codeUnitIndex;
      normalizeSelection = true;
    }
  }
  if ((normalizeSelection) && (control->selectionEnd < control->selectionStart)) {
    /* Keep selectionStart <= selectionEnd when the moving end crossed the anchor. */
    LOCK();
    codeUnitIndex = control->selectionEnd;
    control->selectionEnd = control->selectionStart;
    UNLOCK();
    control->selectionStart = codeUnitIndex;
  }
  if (recomputeLayout) {
    UiTextEditControl_RecomputeLayoutAndClampScroll((UiTextEditControl *)control);
  }
  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)control);
  if ((control->editStateFlags & UI_REQUIRED_TEXT_ACTION_ON_ENTER_ONLY) == 0) {
    UiActionQueue_Enqueue(control->actionId,control);
  }
  UiNode_InvalidateRoot(&control->base);
  if (((control->editStateFlags & UI_REQUIRED_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
     (control->activationSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
  }
  return false;
}


/* Address: 0x004227B0.
   Draws a graphics-adapter option button (drawClipped slot of g_UiGraphicsAdapterTextButtonVtable): patches
   rich-text payloads 0 and 1 of its text and draws it as a text button. The values are the two dwords stored
   just before the node (control[-1].packedTextStyle at -8 is the adapter index or first number,
   control[-1].textResourceId at -0xC the second number). State bit 0x80: only the first number; bit 0x800:
   the adapter's driver description and device name (text 0x111 for the primary adapter, whose GUID is 0);
   neither: both numbers.
*/
void UiGraphicsAdapterTextButton_DrawFormattedAdapterText
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control)

{
  UiPackedTextStyle adapterIndex;
  GraphicsAdapterRecord *adapterRecords;
  uint16_t *stream;
  uint16_t *replacementPayload;
  TextResolveResult resolvedText;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve(control->textResourceId);
    adapterRecords = g_GraphicsAdapters;
    stream = resolvedText.text;
    if (((control->selectable).stateFlags & UI_ADAPTER_TEXT_BUTTON_SINGLE_NUMBER) != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].packedTextStyle,
                 (uint16_t *)&g_GraphicsAdapterFormatScratch0Utf16);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_GraphicsAdapterFormatScratch0Utf16,stream);
      UiTextButtonControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,control);
      return;
    }
    if (((control->selectable).stateFlags & UI_ADAPTER_TEXT_BUTTON_ADAPTER_NAME) == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].packedTextStyle,
                 (uint16_t *)&g_GraphicsAdapterFormatScratch0Utf16);
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].textResourceId,
                 (uint16_t *)&g_GraphicsAdapterFormatScratch1Utf16);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_GraphicsAdapterFormatScratch0Utf16,stream);
      RichTextCommandStream_PatchPayloadBySelector(1,&g_GraphicsAdapterFormatScratch1Utf16,stream);
      UiTextButtonControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,control);
      return;
    }
    adapterIndex = control[-1].packedTextStyle;
    RichTextCommandStream_PatchPayloadBySelector
              (0,g_GraphicsAdapters[adapterIndex].driverDescriptionUtf16,stream);
    if (adapterRecords[adapterIndex].deviceGuid.Data1 == 0) {
      resolvedText = TextResource_Resolve(TEXT_ID_PRIMARY_DISPLAY_ADAPTER);
      replacementPayload = resolvedText.text;
    }
    else {
      replacementPayload = adapterRecords[adapterIndex].deviceNameUtf16;
    }
    RichTextCommandStream_PatchPayloadBySelector(1,replacementPayload,stream);
    UiTextButtonControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,control);
  }
  return;
}


/* Address: 0x004B58F0.
   Relocation of a loaded numeric text edit (relocate slot of g_UiNumericTextEditControlVtable): makes it a
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


/* Address: 0x004B5960.
   Draws a text edit (drawClipped slot of g_UiNumericTextEditControlVtable, g_UiPathTextEditControlVtable and
   g_UiRequiredTextEditControlVtable): the optional win.gfx frame and tiled interior, the selection highlight,
   the text in the active, invalid-value or disabled style, and in the visible caret phase the insert or
   overwrite caret with its shadow. The clip rectangle is narrowed to the text area first.
*/
void UiTextEditControl_DrawTextSelectionAndCaret
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextEditControl *control)

{
  UiTextCodeUnitCount prefixLength;
  UiPixelCoordinate startOrCaretWidth;
  UiPixelCoordinate selectionEndWidth;
  int styleVerticalOffset;
  int leftOrTextOffsetX;
  uint32_t tileBottom;
  int topOrTextOffsetY;
  int bottomOrCaretY;
  UiPackedTextStyle packedStyle;
  int rightOrCaretX;
  bool framebufferUnavailable;
  TextureSizeResult cornerTileSize;
  GlyphSizeResult fontSize;
  uint32_t borderWidthOrCaretFrame;
  GraphicsTextureSourceAsset *caretTextureSource;
  SoftwareFramebufferAccess *caretFramebuffer;
  
  framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
  if (!framebufferUnavailable) {
    if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_FRAMED_CHROME) == 0) {
      borderWidthOrCaretFrame = 0;
      leftOrTextOffsetX = (control->base).left;
      topOrTextOffsetY = (control->base).top;
      rightOrCaretX = (control->base).right;
      bottomOrCaretY = (control->base).bottom;
    }
    else {
      rightOrCaretX = (control->base).layoutWidth;
      bottomOrCaretY = (control->base).layoutHeight;
      cornerTileSize = g_GraphicsTextureSourceGetLogicalSize
                         (UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,g_UiWindowTextureSource);
      tileBottom = cornerTileSize.logicalHeightPixels;
      borderWidthOrCaretFrame = cornerTileSize.logicalWidthPixels;
      rightOrCaretX = rightOrCaretX - borderWidthOrCaretFrame;
      bottomOrCaretY = bottomOrCaretY - tileBottom;
      if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_TILED_INTERIOR) != 0) {
        UiWindow_BlitTiledInterior
                  (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_INTERIOR,bottomOrCaretY,
                   rightOrCaretX,
                   tileBottom,borderWidthOrCaretFrame,control);
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,(control->base).left,
                 UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,
                 rightOrCaretX + (control->base).left,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_TOP_RIGHT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,bottomOrCaretY + (control->base).top,
                 (control->base).left,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_BOTTOM_LEFT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,bottomOrCaretY + (control->base).top,
                 rightOrCaretX + (control->base).left,
                 UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_TOP,
                 rightOrCaretX,0,borderWidthOrCaretFrame,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_LEFT,
                 bottomOrCaretY,tileBottom,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_RIGHT,
                 bottomOrCaretY,tileBottom,rightOrCaretX,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME + UI_WINDOW_FRAME_BOTTOM,
                 rightOrCaretX,bottomOrCaretY,borderWidthOrCaretFrame,control);
      topOrTextOffsetY = tileBottom + (control->base).top;
      rightOrCaretX = rightOrCaretX + (control->base).left;
      leftOrTextOffsetX = borderWidthOrCaretFrame + (control->base).left;
      bottomOrCaretY = bottomOrCaretY + (control->base).top;
    }
    if (clipRight < leftOrTextOffsetX) {
      clipRight = leftOrTextOffsetX;
    }
    if (clipBottom < topOrTextOffsetY) {
      clipBottom = topOrTextOffsetY;
    }
    fontSize = FontGlyph_GetLogicalSizeActiveRegs(0);
    if (rightOrCaretX < clipLeft) {
      clipLeft = rightOrCaretX;
    }
    leftOrTextOffsetX = (borderWidthOrCaretFrame - control->horizontalScrollPixels) + 2;
    if (bottomOrCaretY < clipTop) {
      clipTop = bottomOrCaretY;
    }
    topOrTextOffsetY = (int)((control->base).layoutHeight - fontSize.lineHeight) >> 1;
    prefixLength = control->selectionEnd;
    if (((control->editStateFlags & UI_TEXT_EDIT_READ_ONLY) == 0) &&
       (control->selectionStart != prefixLength)) {
      startOrCaretWidth = UiTextEditControl_MeasurePrefixWidth(control->selectionStart,control);
      selectionEndWidth = UiTextEditControl_MeasurePrefixWidth(prefixLength,control);
      if (startOrCaretWidth == 0) {
        startOrCaretWidth = -2;
      }
      rightOrCaretX = selectionEndWidth + leftOrTextOffsetX;
      if (clipLeft < selectionEndWidth + leftOrTextOffsetX) {
        rightOrCaretX = clipLeft;
      }
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_TEXT_SELECTION,rightOrCaretX,
                 topOrTextOffsetY - 1,startOrCaretWidth + leftOrTextOffsetX,control);
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
                (clipTop,clipLeft,clipBottom,clipRight,packedStyle,control->textBuffer,
                 topOrTextOffsetY + (control->base).top,leftOrTextOffsetX + (control->base).left);
    }
    else {
      startOrCaretWidth = UiTextEditControl_MeasurePrefixWidth(control->cursorIndex,control);
      rightOrCaretX = startOrCaretWidth - 2 + leftOrTextOffsetX + (control->base).left;
      borderWidthOrCaretFrame = UI_WINDOW_SUBRESOURCE_CARET_INSERT;
      if ((control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) != 0) {
        borderWidthOrCaretFrame = UI_WINDOW_SUBRESOURCE_CARET_OVERWRITE;
      }
      styleVerticalOffset = (int)(packedStyle << 0x10) >> 0x18;
      bottomOrCaretY = topOrTextOffsetY - 1 + (control->base).top + styleVerticalOffset;
      if ((control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) != 0) {
        bottomOrCaretY++;
      }
      caretTextureSource = g_UiWindowTextureSource;
      caretFramebuffer = g_FramebufferAccess;
      /* the shadow always uses the insert caret, also in overwrite mode (as in the original) */
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,bottomOrCaretY,rightOrCaretX + styleVerticalOffset,
                 TEXT_SHADOW_COLOR_ARGB,UI_WINDOW_SUBRESOURCE_CARET_INSERT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,packedStyle,control->textBuffer,
                 topOrTextOffsetY + (control->base).top,leftOrTextOffsetX + (control->base).left);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,topOrTextOffsetY + (control->base).top - 1,rightOrCaretX,
                 borderWidthOrCaretFrame,
                 caretTextureSource,caretFramebuffer);
    }
    g_GraphicsFramebufferEndAccess();
  }
  return;
}


/* Address: 0x004B5DF0.
   Primary button press on a text edit (nonRightPress slot of the numeric, path and required text edit
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
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
  }
  return;
}


/* Address: 0x004B5EA0.
   Primary-button drag over a text edit (nonRightDrag slot of the numeric, path and required text edit
   vtables): while a pointer selection is active, moves the cursor and the selection end it sits on to the
   pointer, keeps selectionStart <= selectionEnd, lays the control out again (scroll) and redraws it.
*/
void UiTextEditControl_UpdateSelectionFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control)

{
  UiTextCodeUnitIndex previousCursorOrSelectionStart;
  UiNodeVtable *nodeVtable;
  uint32_t movedBoundaryIndex;
  
  if ((control->editStateFlags & UI_TEXT_EDIT_POINTER_SELECTION_ACTIVE) != 0) {
    previousCursorOrSelectionStart = control->cursorIndex;
    movedBoundaryIndex = UiTextEditControl_FindCursorIndexAtX(pointerX,control);
    control->cursorIndex = movedBoundaryIndex;
    if (previousCursorOrSelectionStart == control->selectionStart) {
      control->selectionStart = movedBoundaryIndex;
      movedBoundaryIndex = control->selectionEnd;
    }
    else {
      control->selectionEnd = movedBoundaryIndex;
    }
    nodeVtable = (control->base).vtable;
    if (movedBoundaryIndex < control->selectionStart) {
      LOCK();
      previousCursorOrSelectionStart = control->selectionStart;
      control->selectionStart = movedBoundaryIndex;
      UNLOCK();
      control->selectionEnd = previousCursorOrSelectionStart;
    }
    nodeVtable->layout(&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}


/* Address: 0x004B6850.
   Relocation of a loaded DOS path edit (relocate slot of g_UiPathTextEditControlVtable): makes it a fallback
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


/* Address: 0x004B70A0.
   Relocation of a loaded required text edit (relocate slot of g_UiRequiredTextEditControlVtable): makes it a
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


/* Address: 0x004BB5C0.
   Sorts the rows of a pointer list in ascending order of the rich text found fieldOffset bytes into each row
   record (expanded, ASCII case-insensitive), with an exchange sort that moves the smallest remaining row to
   the front in each pass. The previously selected record stays selected and is scrolled into view. Called
   by the scenario catalogue (assets/scenario/catalog.c, field offset 0x74).
*/
void UiPointerList_SortByExpandedTextFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *swappedRecord;
  int comparisonsOrRowTop;
  int remainingPasses;
  UiListRowCount remainingRows;
  void **passAnchorSlot;
  void **rowSlotCursor;
  TextCompareResult compareFlags;
  void *selectedRecord;
  
  rowSlotCursor = control->rowSlots;
  if (rowSlotCursor != NULL) {
    comparisonsOrRowTop = control->rowCount - 1;
    if ((comparisonsOrRowTop != 0) && (-1 < comparisonsOrRowTop)) {
      selectedRecord = *control->selectedRowSlot;
      passAnchorSlot = rowSlotCursor;
      remainingPasses = comparisonsOrRowTop;
      do {
        do {
          rowSlotCursor++;
          compareFlags = UiPointerList_CompareExpandedTextFlags
                            ((uint16_t *)((uint8_t *)*rowSlotCursor + fieldOffset),
                             (uint16_t *)((uint8_t *)*passAnchorSlot + fieldOffset));
          if (!compareFlags.less) {
            LOCK();
            swappedRecord = *rowSlotCursor;
            *rowSlotCursor = *passAnchorSlot;
            UNLOCK();
            *passAnchorSlot = swappedRecord;
          }
          comparisonsOrRowTop--;
        } while (comparisonsOrRowTop != 0);
        rowSlotCursor = passAnchorSlot + 1;
        comparisonsOrRowTop = remainingPasses - 1;
        passAnchorSlot = rowSlotCursor;
        remainingPasses = comparisonsOrRowTop;
      } while (comparisonsOrRowTop != 0);
      rowSlotCursor = control->rowSlots;
      remainingRows = control->rowCount;
      comparisonsOrRowTop = 0;
      do {
        if (selectedRecord == *rowSlotCursor) break;
        comparisonsOrRowTop = comparisonsOrRowTop + control->rowHeight;
        rowSlotCursor++;
        remainingRows--;
      } while (remainingRows != 0);
      if (remainingRows == 0) {
        /* Not found: select the first row (the row top stays past the last row, as in the original). */
        rowSlotCursor = control->rowSlots;
      }
      control->selectedRowSlot = rowSlotCursor;
      UiScrollableControl_ClampOffsetsToViewport
                (comparisonsOrRowTop + 1 + control->rowHeight,(control->base).rightOffset,comparisonsOrRowTop,0,
                 (UiScrollableControl *)(control->base).parent);
    }
  }
  return;
}


/* Address: 0x004BB6B0.
   Descending counterpart of UiPointerList_SortByExpandedTextFieldAscending: each pass moves the largest
   remaining row (by the expanded rich text at fieldOffset) to the front; the selected record stays selected
   and is scrolled into view. No caller was found in the source or in the image's tables.
*/
void UiPointerList_SortByExpandedTextFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *swappedRecord;
  int comparisonsOrRowTop;
  int remainingPasses;
  UiListRowCount remainingRows;
  void **passAnchorSlot;
  void **rowSlotCursor;
  TextCompareResult compareFlags;
  void *selectedRecord;
  
  rowSlotCursor = control->rowSlots;
  if (rowSlotCursor != NULL) {
    comparisonsOrRowTop = control->rowCount - 1;
    if ((comparisonsOrRowTop != 0) && (-1 < comparisonsOrRowTop)) {
      selectedRecord = *control->selectedRowSlot;
      passAnchorSlot = rowSlotCursor;
      remainingPasses = comparisonsOrRowTop;
      do {
        do {
          rowSlotCursor++;
          compareFlags = UiPointerList_CompareExpandedTextFlags
                            ((uint16_t *)((uint8_t *)*rowSlotCursor + fieldOffset),
                             (uint16_t *)((uint8_t *)*passAnchorSlot + fieldOffset));
          if (compareFlags.less || compareFlags.equal) {
            LOCK();
            swappedRecord = *rowSlotCursor;
            *rowSlotCursor = *passAnchorSlot;
            UNLOCK();
            *passAnchorSlot = swappedRecord;
          }
          comparisonsOrRowTop--;
        } while (comparisonsOrRowTop != 0);
        rowSlotCursor = passAnchorSlot + 1;
        comparisonsOrRowTop = remainingPasses - 1;
        passAnchorSlot = rowSlotCursor;
        remainingPasses = comparisonsOrRowTop;
      } while (comparisonsOrRowTop != 0);
      rowSlotCursor = control->rowSlots;
      remainingRows = control->rowCount;
      comparisonsOrRowTop = 0;
      do {
        if (selectedRecord == *rowSlotCursor) break;
        comparisonsOrRowTop = comparisonsOrRowTop + control->rowHeight;
        rowSlotCursor++;
        remainingRows--;
      } while (remainingRows != 0);
      if (remainingRows == 0) {
        /* Not found: select the first row (the row top stays past the last row, as in the original). */
        rowSlotCursor = control->rowSlots;
      }
      control->selectedRowSlot = rowSlotCursor;
      UiScrollableControl_ClampOffsetsToViewport
                (comparisonsOrRowTop + 1 + control->rowHeight,(control->base).rightOffset,comparisonsOrRowTop,0,
                 (UiScrollableControl *)(control->base).parent);
    }
  }
  return;
}


/* Address: 0x005156A0.
   Draws a text button showing two numbers (drawClipped slot of g_UiNumericPairTextButtonVtable, e.g. a
   display resolution): formats firstValue and secondValue as decimal into rich-text payloads 0 and 1 of its
   text, then draws it as a text button.
*/
void UiNumericPairTextButton_DrawFormattedValues
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNumericPairTextButton *control)

{
  TextResolveResult resolvedText;

  if (((control->base).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve((control->base).textResourceId);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->firstValue,
               (uint16_t *)&g_UiNumericPairFirstValueScratchUtf16);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->secondValue,
               (uint16_t *)&g_UiNumericPairSecondValueScratchUtf16);
    RichTextCommandStream_PatchPayloadBySelector(0,&g_UiNumericPairFirstValueScratchUtf16,resolvedText.text);
    RichTextCommandStream_PatchPayloadBySelector(1,&g_UiNumericPairSecondValueScratchUtf16,resolvedText.text);
    UiTextButtonControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,&control->base);
  }
  return;
}


/* Address: 0x00515780.
   Draws a text button with two text payloads (drawClipped slot of g_UiPayloadPairTextButtonVtable): patches
   firstPayload and secondPayload into rich-text payloads 0 and 1 of its text, then draws it as a text
   button.
*/
void UiPayloadPairTextButton_DrawFormattedPayloads
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPayloadPairTextButton *control)

{
  TextResolveResult resolvedText;

  if (((control->base).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve((control->base).textResourceId);
    RichTextCommandStream_PatchPayloadBySelector(0,control->firstPayload,resolvedText.text);
    RichTextCommandStream_PatchPayloadBySelector(1,control->secondPayload,resolvedText.text);
    UiTextButtonControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,&control->base);
  }
  return;
}


/* Address: 0x004B0320.
   Draws the tooltip once its delay has expired: a one-line box (win.gfx left cap 0xBC, tiled middle 0xBD,
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
  int frameWidthOrMiddleEnd;
  int targetLeftOrTileX;
  int frameTop;
  bool framebufferUnavailable;
  RichTextExtentRegs textExtent;
  TextResolveResult resolvedText;
  TextureSizeResult tileSize;
  
  tooltipTarget = g_UiTooltipState.targetNode;
  if ((g_UiTooltipState.targetNode != NULL) && (g_UiTooltipState.countdownFrames == 0)) {
    rootNode = UiNode_GetRoot(g_UiTooltipState.targetNode);
    /* the dword just before the node: a text resource id, or with UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16 the
       text itself */
    commandStream = (uint16_t *)tooltipTarget[-1].nodeFlags;
    if ((tooltipTarget->nodeFlags & UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)commandStream);
      commandStream = resolvedText.text;
    }
    textExtent = RichTextCommandStream_MeasureRegs(g_UiTooltipTextStyle,commandStream);
    tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TOOLTIP_LEFT,g_UiWindowTextureSource);
    edgeTileWidth = tileSize.logicalWidthPixels;
    frameWidthOrMiddleEnd = textExtent.widthPixels + edgeTileWidth * 2;
    targetLeftOrTileX = tooltipTarget->left;
    frameTop = tooltipTarget->top - tileSize.logicalHeightPixels;
    targetRight = tooltipTarget->right;
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      frameLeft = (targetLeftOrTileX + targetRight) - frameWidthOrMiddleEnd >> 1;
      if (rootNode == UI_NODE_NONE) {
        rootNode = tooltipTarget;
      }
      frameRight = frameWidthOrMiddleEnd + frameLeft;
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
      frameWidthOrMiddleEnd = frameRight - edgeTileWidth;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,frameTop,frameWidthOrMiddleEnd,
                 UI_WINDOW_SUBRESOURCE_TOOLTIP_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      if (clipRight < frameWidthOrMiddleEnd) {
        frameWidthOrMiddleEnd = clipRight;
      }
      tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TOOLTIP_MIDDLE,g_UiWindowTextureSource);
      targetLeftOrTileX = edgeTileWidth + frameLeft;
      do {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,frameWidthOrMiddleEnd,clipTop,clipLeft,frameTop,targetLeftOrTileX,
                   UI_WINDOW_SUBRESOURCE_TOOLTIP_MIDDLE,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        targetLeftOrTileX = targetLeftOrTileX + tileSize.logicalWidthPixels;
      } while (targetLeftOrTileX < frameWidthOrMiddleEnd);
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


/* Address: 0x004B0F90.
   Closes the UI roots of an ending session: pops the front root until the stack is empty; a root that vetoes
   its close stops the loop and is reported as CF (true). The original also stops at the dword after
   g_UiRootNode (0x004B0E34, the window texture source), which is never a root, so in practice this pops every
   root.
*/
bool UiRootStack_PopUntilWindowTextureBoundary(void)

{
  bool popStopped;

  while (((GraphicsTextureSourceAsset *)g_UiRootNode != g_UiWindowTextureSource &&
         (g_UiRootNode != UI_ROOT_STACK_END))) {
    popStopped = UiRootStack_Pop(g_UiRootNode);
    if (popStopped) {
      return true;
    }
  }
  return false;
}


/* Address: 0x004B1DD0.
   Relocation of a loaded framed text button (relocate slot of g_UiNodeVtable_004B1D80): an inset-framed
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


/* Address: 0x004B1E10.
   Draws a framed text button (drawClipped slot of g_UiNodeVtable_004B1D80): the normal, selected or disabled
   win.gfx frame (plain or inset), its text centred in the state's style, with the focus mark and its shadow
   behind the text while it has keyboard focus, then the children unless the button is suppressed.
*/
void UiFramedTextButtonControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiFramedTextButtonControl *control)

{
  uint32_t cornerWidth;
  uint16_t *commandStream;
  uint32_t framePieceOrTextWidth;
  int focusTileXOrTop;
  uint32_t cornerHeight;
  int bottomCornerYOrTextX;
  uint32_t textStyle;
  int rightCornerXOrTextY;
  bool framebufferUnavailable;
  bool drawFrame;
  RichTextExtentRegs textExtent;
  TextResolveResult resolvedText;
  TextureSizeResult tileSizeOrEndCapSize;
  TextureSizeResult focusTileSize;
  int textX;
  int textY;

  framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
  if (framebufferUnavailable) {
    drawFrame = false;
  }
  else if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    drawFrame = true;
    if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
      framePieceOrTextWidth = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME;
    }
    else {
      framePieceOrTextWidth = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      framePieceOrTextWidth = framePieceOrTextWidth + UI_WINDOW_FRAME_PIECE_COUNT;
    }
  }
  else {
    /* Suppressed: the disabled frame, unless UI_BUTTON_HIDDEN_WHILE_SUPPRESSED hides it. */
    drawFrame = ((control->selectable).stateFlags & UI_BUTTON_HIDDEN_WHILE_SUPPRESSED) == 0;
    if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
      framePieceOrTextWidth = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME_DISABLED;
    }
    else {
      framePieceOrTextWidth = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME_DISABLED;
    }
  }
  if (drawFrame) {
    tileSizeOrEndCapSize = g_GraphicsTextureSourceGetLogicalSize(framePieceOrTextWidth,g_UiWindowTextureSource);
    cornerHeight = tileSizeOrEndCapSize.logicalHeightPixels;
    cornerWidth = tileSizeOrEndCapSize.logicalWidthPixels;
    rightCornerXOrTextY = (control->selectable).base.layoutWidth - cornerWidth;
    bottomCornerYOrTextX = (control->selectable).base.layoutHeight - cornerHeight;
    if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) ||
       (((control->selectable).stateFlags & UI_BUTTON_NO_FRAME_WHILE_SUPPRESSED) == 0)) {
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                 (control->selectable).base.left,framePieceOrTextWidth,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                 rightCornerXOrTextY + (control->selectable).base.left,
                 framePieceOrTextWidth + UI_WINDOW_FRAME_TOP_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,bottomCornerYOrTextX + (control->selectable).base.top,
                 (control->selectable).base.left,framePieceOrTextWidth + UI_WINDOW_FRAME_BOTTOM_LEFT,
                 g_UiWindowTextureSource,
                 g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,bottomCornerYOrTextX + (control->selectable).base.top,
                 rightCornerXOrTextY + (control->selectable).base.left,
                 framePieceOrTextWidth + UI_WINDOW_FRAME_BOTTOM_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,framePieceOrTextWidth + UI_WINDOW_FRAME_TOP,rightCornerXOrTextY,
                 0,cornerWidth,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,framePieceOrTextWidth + UI_WINDOW_FRAME_LEFT,
                 bottomCornerYOrTextX,cornerHeight,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,framePieceOrTextWidth + UI_WINDOW_FRAME_RIGHT,
                 bottomCornerYOrTextX,cornerHeight,rightCornerXOrTextY,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,framePieceOrTextWidth + UI_WINDOW_FRAME_BOTTOM,
                 rightCornerXOrTextY,bottomCornerYOrTextX,cornerWidth,control);
    }
    resolvedText = TextResource_Resolve(control->textResourceId);
    commandStream = resolvedText.text;
    textExtent = RichTextCommandStream_MeasureRegs(control->packedTextStyle,commandStream);
    framePieceOrTextWidth = textExtent.widthPixels;
    bottomCornerYOrTextX = (int)((control->selectable).base.layoutWidth - framePieceOrTextWidth) >> 1;
    rightCornerXOrTextY = (int)((control->selectable).base.layoutHeight - textExtent.heightPixels) >> 1;
    textStyle = g_UiTextStyleDisabled;
    if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
       (textStyle = g_UiTextStyleNormal,
       ((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
      textStyle = g_UiTextStyleSelected;
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
                (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,
                 rightCornerXOrTextY + (control->selectable).base.top,
                 bottomCornerYOrTextX + (control->selectable).base.left);
    }
    else {
      bottomCornerYOrTextX = bottomCornerYOrTextX + (control->selectable).base.left;
      rightCornerXOrTextY = rightCornerXOrTextY + (control->selectable).base.top;
      textX = bottomCornerYOrTextX;
      textY = rightCornerXOrTextY;
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,rightCornerXOrTextY,bottomCornerYOrTextX - 2,
                 TEXT_SHADOW_COLOR_ARGB,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      tileSizeOrEndCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                                   g_UiWindowTextureSource);
      focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                            g_UiWindowTextureSource);
      focusTileXOrTop = bottomCornerYOrTextX - 2 + focusTileSize.logicalWidthPixels;
      focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                            g_UiWindowTextureSource);
      bottomCornerYOrTextX =
           ((bottomCornerYOrTextX + 4) - tileSizeOrEndCapSize.logicalWidthPixels) + framePieceOrTextWidth;
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,rightCornerXOrTextY,bottomCornerYOrTextX,TEXT_SHADOW_COLOR_ARGB,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      if (clipLeft < bottomCornerYOrTextX) {
        bottomCornerYOrTextX = clipLeft;
      }
      do {
        g_GraphicsTextureSourceBlitModulatedSourceAlpha
                  (clipTop,bottomCornerYOrTextX,clipBottom,clipRight,rightCornerXOrTextY,focusTileXOrTop,
                   TEXT_SHADOW_COLOR_ARGB,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        focusTileXOrTop = focusTileXOrTop + focusTileSize.logicalWidthPixels;
      } while (focusTileXOrTop < bottomCornerYOrTextX);
      focusTileXOrTop = textY - 1;
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,textY,textX);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,focusTileXOrTop,textX - 3,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      tileSizeOrEndCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                                   g_UiWindowTextureSource);
      focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                            g_UiWindowTextureSource);
      rightCornerXOrTextY = textX - 3 + focusTileSize.logicalWidthPixels;
      focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                            g_UiWindowTextureSource);
      bottomCornerYOrTextX = ((textX + 3) - tileSizeOrEndCapSize.logicalWidthPixels) + framePieceOrTextWidth;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,focusTileXOrTop,bottomCornerYOrTextX,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      if (clipLeft < bottomCornerYOrTextX) {
        bottomCornerYOrTextX = clipLeft;
      }
      do {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,bottomCornerYOrTextX,clipBottom,clipRight,focusTileXOrTop,rightCornerXOrTextY,
                   UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        rightCornerXOrTextY = rightCornerXOrTextY + focusTileSize.logicalWidthPixels;
      } while (rightCornerXOrTextY < bottomCornerYOrTextX);
    }
  }
  if (!framebufferUnavailable) {
    g_GraphicsFramebufferEndAccess();
  }
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    UiContainer_DrawIntersectingChildren
              (clipTop,clipLeft,clipBottom,clipRight,(UiNodeBase *)control);
  }
  return;
}


/* Address: 0x004B22A0.
   Primary button press on a framed button (nonRightPress slot of g_UiNodeVtable_004B1D80 and
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
          (control->activationSoundId != 0)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)control->activationSoundId);
      }
      toggleStateFlagsField = &(control->selectable).stateFlags;
      *toggleStateFlagsField = *toggleStateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
          (control->activationSoundId != 0)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)control->activationSoundId);
      }
      selectedStateFlagsField = &(control->selectable).stateFlags;
      *selectedStateFlagsField = *selectedStateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
  return;
}


/* Address: 0x004B2380.
   Primary button release on a framed button (nonRightRelease slot of g_UiNodeVtable_004B1D80 and
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
        (control->activationSoundId != 0)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control->activationSoundId);
    }
    stateFlagsField = &(control->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}


/* Address: 0x004B23F0.
   Primary-button drag with a framed button captured (nonRightDrag slot of g_UiNodeVtable_004B1D80 and
   g_UiWindowControlVtable): a momentary button shows itself pressed while the pointer is inside its box
   (inside the frame for UI_BUTTON_FRAME_INSET) and released while it is outside.
*/
void UiFramedTextButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control)

{
  UiSelectableStateFlags *clearedStateFlagsField;
  int32_t *edgeField;
  int relativeX;
  int relativeY;
  UiSelectableStateFlags *stateFlagsField;
  
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0)) {
    edgeField = &(control->selectable).base.left;
    relativeX = pointerX - *edgeField;
    if (((pointerX < *edgeField) ||
        (((edgeField = &(control->selectable).base.top, relativeY = pointerY - *edgeField, pointerY < *edgeField
          || ((control->selectable).base.layoutWidth <= relativeX)) ||
         ((control->selectable).base.layoutHeight <= relativeY)))) ||
       ((((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) != 0 &&
        (((relativeX < g_UiWindowFrameInset || (relativeY < g_UiWindowFrameInset)) ||
         (((control->selectable).base.layoutWidth <= relativeX + g_UiWindowFrameInset ||
          ((control->selectable).base.layoutHeight <= relativeY + g_UiWindowFrameInset)))))))) {
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
        clearedStateFlagsField = &(control->selectable).stateFlags;
        *clearedStateFlagsField = *clearedStateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
        UiNode_InvalidateRoot((UiNodeBase *)control);
      }
    }
    else if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return;
    }
  }
  return;
}


/* Address: 0x004B24C0.
   Hit test of a framed button (hitTest slot of g_UiNodeVtable_004B1D80 and g_UiWindowControlVtable): the
   control itself when the point lies inside its box (for UI_BUTTON_FRAME_INSET: inside the frame), else
   UI_NODE_NONE. Suppressed buttons are never hit; children are not tested.
*/
UiNodeBase * UiFramedTextButtonControl_HitTestRect
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFramedTextButtonControl *control)

{
  int32_t *edgeField;
  UiFramedTextButtonControl *hitResult;
  int relativeX;
  int relativeY;
  
  hitResult = (UiFramedTextButtonControl *)UI_NODE_NONE;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    edgeField = &(control->selectable).base.left;
    relativeX = pointerX - *edgeField;
    if (((((*edgeField <= pointerX) &&
          (edgeField = &(control->selectable).base.top, relativeY = pointerY - *edgeField, *edgeField <= pointerY
          )) && (relativeX < (control->selectable).base.layoutWidth)) &&
        (relativeY < (control->selectable).base.layoutHeight)) &&
       ((((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0 ||
        (((g_UiWindowFrameInset <= relativeX && (g_UiWindowFrameInset <= relativeY)) &&
         ((relativeX + g_UiWindowFrameInset < (control->selectable).base.layoutWidth &&
          (relativeY + g_UiWindowFrameInset < (control->selectable).base.layoutHeight)))))))) {
      hitResult = control;
    }
  }
  return (UiNodeBase *)hitResult;
}


/* Address: 0x004B27D0.
   Draws a framed icon-and-text button (drawClipped slot of g_UiWindowControlVtable): the normal, selected or
   disabled win.gfx frame, the text centred in the right three quarters (with the focus mark while focused),
   and the icon, vertically centred and ending at the quarter line, over its shadow copy shifted by the normal
   or selected iconDrawOffsets (no shift while suppressed). Children are not drawn.
*/
void UiWindowControl_DrawFramedTextAndChrome
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiWindowControl *control)

{
  char iconOffsetX;
  char iconOffsetY;
  uint32_t cornerWidth;
  uint16_t *commandStream;
  int styleOffsetOrIconY;
  uint32_t framePieceOrTextWidth;
  int focusTileX;
  uint32_t cornerHeight;
  int bottomCornerYOrTextX;
  uint32_t textStyle;
  int focusCoordOrIconX;
  int rightCornerXOrTextY;
  bool framebufferUnavailable;
  RichTextExtentRegs textExtent;
  TextResolveResult resolvedText;
  TextureSizeResult tileSizeOrEndCapSize;
  TextureSizeResult focusTileSize;
  
  framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
  if (framebufferUnavailable) {
    return;
  }
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
      framePieceOrTextWidth = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME;
    }
    else {
      framePieceOrTextWidth = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      framePieceOrTextWidth = framePieceOrTextWidth + UI_WINDOW_FRAME_PIECE_COUNT;
    }
  }
  else {
    if (((control->selectable).stateFlags & UI_BUTTON_HIDDEN_WHILE_SUPPRESSED) != 0) {
      g_GraphicsFramebufferEndAccess();
      return;
    }
    if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
      framePieceOrTextWidth = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME_DISABLED;
    }
    else {
      framePieceOrTextWidth = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME_DISABLED;
    }
  }
  tileSizeOrEndCapSize = g_GraphicsTextureSourceGetLogicalSize(framePieceOrTextWidth,g_UiWindowTextureSource);
  cornerHeight = tileSizeOrEndCapSize.logicalHeightPixels;
  cornerWidth = tileSizeOrEndCapSize.logicalWidthPixels;
  rightCornerXOrTextY = (control->selectable).base.layoutWidth - cornerWidth;
  bottomCornerYOrTextX = (control->selectable).base.layoutHeight - cornerHeight;
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,(control->selectable).base.left,
             framePieceOrTextWidth,
             g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
             rightCornerXOrTextY + (control->selectable).base.left,framePieceOrTextWidth + UI_WINDOW_FRAME_TOP_RIGHT,
             g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipTop,clipLeft,clipBottom,clipRight,bottomCornerYOrTextX + (control->selectable).base.top,
             (control->selectable).base.left,framePieceOrTextWidth + UI_WINDOW_FRAME_BOTTOM_LEFT,
             g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipTop,clipLeft,clipBottom,clipRight,bottomCornerYOrTextX + (control->selectable).base.top,
             rightCornerXOrTextY + (control->selectable).base.left,
             framePieceOrTextWidth + UI_WINDOW_FRAME_BOTTOM_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
  UiWindow_BlitTiledHorizontalEdge
            (clipTop,clipLeft,clipBottom,clipRight,framePieceOrTextWidth + UI_WINDOW_FRAME_TOP,rightCornerXOrTextY,0,
             cornerWidth,control);
  UiWindow_BlitTiledVerticalEdge
            (clipTop,clipLeft,clipBottom,clipRight,framePieceOrTextWidth + UI_WINDOW_FRAME_LEFT,bottomCornerYOrTextX,
             cornerHeight,0,control);
  UiWindow_BlitTiledVerticalEdge
            (clipTop,clipLeft,clipBottom,clipRight,framePieceOrTextWidth + UI_WINDOW_FRAME_RIGHT,bottomCornerYOrTextX,
             cornerHeight,rightCornerXOrTextY,control);
  UiWindow_BlitTiledHorizontalEdge
            (clipTop,clipLeft,clipBottom,clipRight,framePieceOrTextWidth + UI_WINDOW_FRAME_BOTTOM,rightCornerXOrTextY,
             bottomCornerYOrTextX,cornerWidth,control);
  resolvedText = TextResource_Resolve(control->textResourceId);
  commandStream = resolvedText.text;
  bottomCornerYOrTextX = (control->selectable).base.layoutWidth;
  textExtent = RichTextCommandStream_MeasureRegs(control->packedTextStyle,commandStream);
  framePieceOrTextWidth = textExtent.widthPixels;
  rightCornerXOrTextY = (int)((control->selectable).base.layoutHeight - textExtent.heightPixels) >> 1;
  bottomCornerYOrTextX = ((int)(((uint32_t)(bottomCornerYOrTextX * 3) >> 2) - framePieceOrTextWidth) >> 1) +
                         ((uint32_t)(control->selectable).base.layoutWidth >> 2);
  textStyle = g_UiTextStyleDisabled;
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (textStyle = g_UiTextStyleNormal, ((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    textStyle = g_UiTextStyleSelected;
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
              (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,
               rightCornerXOrTextY + (control->selectable).base.top,
               bottomCornerYOrTextX + (control->selectable).base.left);
  }
  else {
    bottomCornerYOrTextX = bottomCornerYOrTextX + (control->selectable).base.left;
    rightCornerXOrTextY = rightCornerXOrTextY + (control->selectable).base.top;
    styleOffsetOrIconY = (int)(textStyle << 0x10) >> 0x18;
    focusCoordOrIconX = styleOffsetOrIconY - 3 + bottomCornerYOrTextX;
    styleOffsetOrIconY = styleOffsetOrIconY - 1 + rightCornerXOrTextY;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,styleOffsetOrIconY,focusCoordOrIconX,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    tileSizeOrEndCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                                 g_UiWindowTextureSource);
    focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                          g_UiWindowTextureSource);
    focusTileX = focusCoordOrIconX + focusTileSize.logicalWidthPixels;
    focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                          g_UiWindowTextureSource);
    focusCoordOrIconX = ((focusCoordOrIconX + 6) - tileSizeOrEndCapSize.logicalWidthPixels) + framePieceOrTextWidth;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,styleOffsetOrIconY,focusCoordOrIconX,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    if (clipLeft < focusCoordOrIconX) {
      focusCoordOrIconX = clipLeft;
    }
    do {
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipTop,focusCoordOrIconX,clipBottom,clipRight,styleOffsetOrIconY,focusTileX,TEXT_SHADOW_COLOR_ARGB,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      focusTileX = focusTileX + focusTileSize.logicalWidthPixels;
    } while (focusTileX < focusCoordOrIconX);
    focusCoordOrIconX = rightCornerXOrTextY - 1;
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,rightCornerXOrTextY,bottomCornerYOrTextX);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,focusCoordOrIconX,bottomCornerYOrTextX - 3,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,g_UiWindowTextureSource,
               g_FramebufferAccess);
    tileSizeOrEndCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                                 g_UiWindowTextureSource);
    focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                          g_UiWindowTextureSource);
    rightCornerXOrTextY = bottomCornerYOrTextX - 3 + focusTileSize.logicalWidthPixels;
    focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                          g_UiWindowTextureSource);
    bottomCornerYOrTextX =
         ((bottomCornerYOrTextX + 3) - tileSizeOrEndCapSize.logicalWidthPixels) + framePieceOrTextWidth;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,focusCoordOrIconX,bottomCornerYOrTextX,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,g_UiWindowTextureSource,
               g_FramebufferAccess);
    if (clipLeft < bottomCornerYOrTextX) {
      bottomCornerYOrTextX = clipLeft;
    }
    do {
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,bottomCornerYOrTextX,clipBottom,clipRight,focusCoordOrIconX,rightCornerXOrTextY,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      rightCornerXOrTextY = rightCornerXOrTextY + focusTileSize.logicalWidthPixels;
    } while (rightCornerXOrTextY < bottomCornerYOrTextX);
  }
  tileSizeOrEndCapSize = g_GraphicsTextureSourceGetLogicalSize
                     (control->iconSubresource,control->iconTextureSource);
  focusCoordOrIconX =
       (((uint32_t)(control->selectable).base.layoutWidth >> 2) - tileSizeOrEndCapSize.logicalWidthPixels) +
       (control->selectable).base.left;
  styleOffsetOrIconY = ((int)((control->selectable).base.layoutHeight - tileSizeOrEndCapSize.logicalHeightPixels) >> 1) +
                       (control->selectable).base.top;
  bottomCornerYOrTextX = focusCoordOrIconX;
  rightCornerXOrTextY = styleOffsetOrIconY;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      iconOffsetX = control->iconDrawOffsets.normalX;
      iconOffsetY = control->iconDrawOffsets.normalY;
    }
    else {
      iconOffsetX = control->iconDrawOffsets.selectedX;
      iconOffsetY = control->iconDrawOffsets.selectedY;
    }
    bottomCornerYOrTextX = focusCoordOrIconX + iconOffsetX;
    rightCornerXOrTextY = styleOffsetOrIconY + iconOffsetY;
  }
  g_GraphicsTextureSourceBlitModulatedSourceAlpha
            (clipTop,clipLeft,clipBottom,clipRight,rightCornerXOrTextY,bottomCornerYOrTextX,TEXT_SHADOW_COLOR_ARGB,
             control->iconSubresource,
             control->iconTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipTop,clipLeft,clipBottom,clipRight,styleOffsetOrIconY,focusCoordOrIconX,control->iconSubresource,
             control->iconTextureSource,g_FramebufferAccess);
  g_GraphicsFramebufferEndAccess();
  return;
}


/* Address: 0x004B2E40.
   Relocation of a loaded text button (relocate slot of g_UiNodeVtable_004B2CE0,
   g_UiGraphicsAdapterTextButtonVtable, g_UiNumericPairTextButtonVtable and g_UiPayloadPairTextButtonVtable):
   only the children need relocating; the text resource id and style are plain values.
*/
void UiTextButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiTextButtonControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  return;
}


/* Address: 0x004B31B0.
   Primary button press on a text button (nonRightPress slot of g_UiNodeVtable_004B2CE0,
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
  bool pixelHit;
  UiSelectableStateFlags *toggleStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) == 0) {
      pixelHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,UI_WINDOW_SUBRESOURCE_PUSH_BUTTON,g_UiWindowTextureSource);
      if (pixelHit) {
        if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
            (control->activationSoundId != 0)) {
          g_SoundPlayOneShot
                    (g_UiSoundGainQ15,g_UiSoundGainQ15,
                     (DirectSoundVoiceSet *)control->activationSoundId);
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
            (control->activationSoundId != 0)) {
          g_SoundPlayOneShot
                    (g_UiSoundGainQ15,g_UiSoundGainQ15,
                     (DirectSoundVoiceSet *)control->activationSoundId);
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


/* Address: 0x004B32C0.
   Keyboard handler of a text button (keyboardEvent slot of g_UiNodeVtable_004B2CE0,
   g_UiGraphicsAdapterTextButtonVtable, g_UiNumericPairTextButtonVtable and g_UiPayloadPairTextButtonVtable):
   Space on the focused button activates it like a pointer press (checkbox toggles, radio-style button gets
   selected) unless UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION is set. Everything else, including Space on
   an already selected radio-style button, goes to UiNode_DefaultKeyboardEventMoveFocusNext. CF clear:
   consumed.
*/
bool UiTextButtonControl_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextButtonControl *control)

{
  UiSelectableStateFlags *selectedStateFlagsField;
  bool delegatedResult;
  UiSelectableStateFlags *stateFlagsField;
  UiSelectableStateFlags *selectionStateFlagsField;
  
  if ((((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) && (keyCode == KEYBOARD_KEY_CODE_SPACE)) &&
      (control == (UiTextButtonControl *)g_UiKeyboardFocusNode)) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION) == 0)) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
      if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
          (control->activationSoundId != 0)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)control->activationSoundId);
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
          (control->activationSoundId != 0)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)control->activationSoundId);
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


/* Address: 0x004B37C0.
   Draws an image panel (drawClipped slot of g_UiImagePanelControlVtable): its texture aligned in the layout
   box by panelFlags (centre/right/bottom), optionally over a drop shadow, or stretched over the whole box,
   clipped to the panel; then the children. A panel without a texture or a suppressed one draws nothing, not
   even its children.
*/
void UiImagePanelControl_DrawAlignedTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiImagePanelControl *control)

{
  int clippedRight;
  int slackWidth;
  int clippedLeft;
  int drawX;
  int clippedBottom;
  int slackHeight;
  int clippedTop;
  int drawY;
  bool framebufferUnavailable;
  TextureSizeResult textureSize;
  
  if (((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    clippedLeft = (control->base).left;
    if ((control->base).left < clipRight) {
      clippedLeft = clipRight;
    }
    clippedTop = (control->base).top;
    if ((control->base).top < clipBottom) {
      clippedTop = clipBottom;
    }
    clippedRight = (control->base).right;
    if (clipLeft < (control->base).right) {
      clippedRight = clipLeft;
    }
    clippedBottom = (control->base).bottom;
    if (clipTop < (control->base).bottom) {
      clippedBottom = clipTop;
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
      UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,&control->base);
    }
  }
  return;
}


/* Address: 0x004B3960.
   Hit test of an image panel (hitTest slot of g_UiImagePanelControlVtable and g_UiArmyMetricsPanelVtable):
   the point must lie on an opaque pixel of the aligned texture (unless UI_IMAGE_PANEL_HIT_WHOLE_BOX), then
   the children are tested. UI_NODE_NONE for UI_IMAGE_PANEL_NEVER_HIT or a miss.
*/
UiNodeBase * UiImagePanelControl_HitTestAlignedTextureAndChildren(int pointerY,int pointerX,
                                                                  UiImagePanelControl *control)

{
  bool childrenAlreadyRetried;
  bool skipTextureTest;
  UiNodeBase *hitNode;
  int slackWidth;
  int drawX;
  int slackHeight;
  int drawY;
  bool opaqueHit;
  TextureSizeResult textureSize;
  
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


/* Address: 0x004B3AA0.
   Draws a fill panel (drawClipped slot of g_UiFillPanelControlVtable): without a texture, a rectangle in the
   ARGB colour subresourceOrFillArgb; with one, its subresource once or tiled across and/or down the box
   (fillFlags), each tile optionally over a drop shadow. Then the children.
*/
void UiFillPanelControl_DrawColorOrTiledTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiFillPanelControl *control)

{
  int controlRight;
  int controlBottom;
  uint32_t tileWidth;
  int tileLeft;
  uint32_t tileHeight;
  int32_t tileTop;
  bool framebufferUnavailable;
  TextureSizeResult tileSize;
  
  controlRight = (control->base).right;
  controlBottom = (control->base).bottom;
  tileLeft = (control->base).left;
  tileTop = (control->base).top;
  if (control->textureSource == NULL) {
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      g_GraphicsFramebufferFillRectArgb
                (clipTop,clipLeft,clipBottom,clipRight,controlBottom,controlRight,tileTop,tileLeft,
                 control->subresourceOrFillArgb,
                 g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
    }
  }
  else {
    if (controlRight < clipLeft) {
      clipLeft = controlRight;
    }
    if (controlBottom < clipTop) {
      clipTop = controlBottom;
    }
    tileSize = g_GraphicsTextureSourceGetLogicalSize
                      (control->subresourceOrFillArgb,control->textureSource);
    tileHeight = tileSize.logicalHeightPixels;
    tileWidth = tileSize.logicalWidthPixels;
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      /* Tile rows left to right (UI_FILL_PANEL_TILE_X), then top to bottom (UI_FILL_PANEL_TILE_Y). */
      while( true ) {
        if ((control->fillFlags & UI_FILL_PANEL_DROP_SHADOW) != 0) {
          g_GraphicsTextureSourceBlitModulatedSourceAlpha
                    (clipTop,clipLeft,clipBottom,clipRight,
                     control->shadowOffsetY + tileTop,
                     control->shadowOffsetX + tileLeft,TEXT_SHADOW_COLOR_ARGB,control->subresourceOrFillArgb,
                     control->textureSource,g_FramebufferAccess);
        }
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,tileTop,tileLeft,control->subresourceOrFillArgb,
                   control->textureSource,g_FramebufferAccess);
        if ((control->fillFlags & UI_FILL_PANEL_TILE_X) != 0) {
          tileLeft = tileLeft + tileWidth;
          if (tileLeft < clipLeft) continue;
          tileLeft = (control->base).left;
        }
        if (((control->fillFlags & UI_FILL_PANEL_TILE_Y) == 0) || (tileTop = tileTop + tileHeight,
                                                                   clipTop <= tileTop)) break;
      }
      g_GraphicsFramebufferEndAccess();
    }
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,&control->base);
  return;
}


/* Address: 0x004B5E60.
   Primary button release on a text edit (nonRightRelease slot of the numeric, path and required text edit
   vtables): ends the pointer selection and plays the interaction sound when enabled.
*/
void UiTextEditControl_EndSelection
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
               UiTextEditControl *control)

{
  control->editStateFlags = control->editStateFlags & ~UI_TEXT_EDIT_POINTER_SELECTION_ACTIVE;
  if (((control->editStateFlags & UI_TEXT_EDIT_PLAY_INTERACTION_SOUND) != 0) &&
     (control->activationSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
  }
  return;
}


/* Address: 0x004B6480.
   Disables a text edit whose action id matches (suppressActionId slot of the numeric, path and required text
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


/* Address: 0x004B64B0.
   Enables a text edit whose action id matches (unsuppressActionId slot of the numeric, path and required text
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


/* Address: 0x004B64E0.
   Per-frame tick of a text edit (tick slot of the numeric, path and required text edit vtables): while it
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
    /* NOTE: the original tests the borrow of the SUB (unsigned); this compare of the int-sized enum is
       signed and also fires every frame while the counter byte is 0x80 or more. */
    if (previousStateFlags < UI_STATE_FRAME_COUNTER_UNIT) {
      blinkPhaseIncrement = g_UiTextEditCaretBlinkPhaseStep * UI_STATE_FRAME_COUNTER_UNIT;
      control->editStateFlags = control->editStateFlags ^ UI_TEXT_EDIT_CARET_VISIBLE_PHASE;
      control->editStateFlags = control->editStateFlags + blinkPhaseIncrement;
      UiNode_InvalidateRoot(&control->base);
    }
  }
  return;
}


/* Address: 0x004B95E0.
   Draws a single-line label that forwards its focus to a child (drawClipped slot of
   g_UiFocusProxyControlVtable): measures the line, aligns it by labelFlags (room for the focus-mark caps
   when there is a focusChild), draws the focus mark and its shadow while the label has keyboard focus, then
   the line (disabled style when the focus child is suppressed). While the label holds the keyboard focus,
   the focus is lent to focusChild for drawing the children, so the child draws itself focused.
*/
void UiSingleLineTextControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiSingleLineTextControl *control)

{
  UiPackedTextStyle packedStyleOverride;
  UiNodeBase *streamOrCapWidthOrChild;
  UiNodeBase *capWidthOrStream;
  int rightCapXOrFrameTop;
  uint32_t textStyle;
  int lineWidth;
  int alignOffsetY;
  int frameTopOrTileX;
  int alignOffsetX;
  int tileX;
  bool framebufferUnavailable;
  RichTextExtentRegs textExtent;
  TextResolveResult resolvedText;
  TextureSizeResult tileSize;
  UiPixelCoordinate originalClipLeft;
  UiPixelCoordinate restoredClipLeft;
  
  originalClipLeft = clipLeft;
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
    streamOrCapWidthOrChild = (UiNodeBase *)control->text;
    if ((control->labelFlags & UI_LABEL_TEXT_IS_STREAM) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)streamOrCapWidthOrChild);
      streamOrCapWidthOrChild = (UiNodeBase *)resolvedText.text;
    }
    textExtent = RichTextCommandStream_MeasureRegs((textStyle | packedStyleOverride) & (UI_TEXT_STYLE_FONT_BYTE|UI_TEXT_STYLE_PALETTE_BYTE),
                                                   (uint16_t *)streamOrCapWidthOrChild);
    lineWidth = (int)(g_UiTextStyleNormal << 0x10) >> 0x18;
    if (lineWidth < 0) {
      lineWidth = -lineWidth;
    }
    lineWidth = textExtent.widthPixels + lineWidth;
    tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,g_UiWindowTextureSource);
    streamOrCapWidthOrChild = (UiNodeBase *)tileSize.logicalWidthPixels;
    if (control->focusChild != NULL) {
      lineWidth = lineWidth + (int)streamOrCapWidthOrChild * 2;
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
      alignOffsetX = (control->base).layoutWidth - lineWidth >> 1;
    }
    lineWidth--;
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      restoredClipLeft = clipLeft;
      if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) != 0) {
        tileX = alignOffsetX + 1 + (control->base).left;
        frameTopOrTileX = alignOffsetY + 1 + (control->base).top;
        rightCapXOrFrameTop = lineWidth + tileX;
        g_GraphicsTextureSourceBlitModulatedSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,frameTopOrTileX,tileX,TEXT_SHADOW_COLOR_ARGB,
                   UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        rightCapXOrFrameTop = rightCapXOrFrameTop - (int)streamOrCapWidthOrChild;
        g_GraphicsTextureSourceBlitModulatedSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,frameTopOrTileX,rightCapXOrFrameTop,TEXT_SHADOW_COLOR_ARGB,
                   UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        tileX = (int)streamOrCapWidthOrChild + tileX;
        if (rightCapXOrFrameTop <= clipLeft) {
          clipLeft = rightCapXOrFrameTop;
        }
        tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                         g_UiWindowTextureSource);
        capWidthOrStream = streamOrCapWidthOrChild;
        do {
          g_GraphicsTextureSourceBlitModulatedSourceAlpha
                    (clipTop,clipLeft,clipBottom,clipRight,frameTopOrTileX,tileX,TEXT_SHADOW_COLOR_ARGB,
                     UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                     g_UiWindowTextureSource,g_FramebufferAccess);
          tileX = tileX + tileSize.logicalWidthPixels;
        } while (tileX < clipLeft);
        frameTopOrTileX = alignOffsetX + (control->base).left;
        rightCapXOrFrameTop = alignOffsetY + (control->base).top;
        streamOrCapWidthOrChild = capWidthOrStream;
        restoredClipLeft = originalClipLeft;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,originalClipLeft,clipBottom,clipRight,rightCapXOrFrameTop,frameTopOrTileX,
                   UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        lineWidth = (lineWidth + frameTopOrTileX) - (int)capWidthOrStream;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,originalClipLeft,clipBottom,clipRight,rightCapXOrFrameTop,lineWidth,
                   UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        frameTopOrTileX = (int)capWidthOrStream + frameTopOrTileX;
        clipLeft = originalClipLeft;
        if (lineWidth <= originalClipLeft) {
          clipLeft = lineWidth;
        }
        tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                         g_UiWindowTextureSource);
        do {
          g_GraphicsTextureSourceBlitSourceAlpha
                    (clipTop,clipLeft,clipBottom,clipRight,rightCapXOrFrameTop,frameTopOrTileX,
                     UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,g_UiWindowTextureSource,
                     g_FramebufferAccess);
          frameTopOrTileX = frameTopOrTileX + tileSize.logicalWidthPixels;
        } while (frameTopOrTileX < clipLeft);
      }
      clipLeft = restoredClipLeft;
      if (control->focusChild != NULL) {
        alignOffsetX = (int)streamOrCapWidthOrChild + alignOffsetX;
        alignOffsetY++;
      }
      capWidthOrStream = (UiNodeBase *)control->text;
      if ((control->labelFlags & UI_LABEL_TEXT_IS_STREAM) == 0) {
        resolvedText = TextResource_Resolve((TextResourceId)capWidthOrStream);
        capWidthOrStream = (UiNodeBase *)resolvedText.text;
      }
      streamOrCapWidthOrChild = control->focusChild;
      textStyle = g_UiTextStyleNormal;
      if ((streamOrCapWidthOrChild != NULL) && ((streamOrCapWidthOrChild->nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
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
                (clipTop,clipLeft,clipBottom,clipRight,textStyle | control->styleOverride,
                 (uint16_t *)capWidthOrStream,alignOffsetY + (control->base).top,alignOffsetX + (control->base).left);
      g_GraphicsFramebufferEndAccess();
    }
    /* NOTE: as in the original (EDX), streamOrCapWidthOrChild is the focus child only when the framebuffer
       could be accessed (else it still holds the cap width), and a focused label without a focus child
       dereferences NULL here. */
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = streamOrCapWidthOrChild;
      streamOrCapWidthOrChild->nodeFlags = streamOrCapWidthOrChild->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
      UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,&control->base);
      streamOrCapWidthOrChild->nodeFlags = streamOrCapWidthOrChild->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
      g_UiKeyboardFocusNode = &control->base;
    }
    else {
      UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,&control->base);
    }
  }
  return;
}


/* Address: 0x004B9E90.
   Draws the visible rows of a text list (drawClipped slot of g_UiTextListControlVtable): each row's rich
   text in the list style, the selected row over a highlight bar as wide as its text plus 6 pixels (with end
   caps while the list has keyboard focus).
*/
void UiTextListControl_DrawRowsAndSelection
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextListControl *control)

{
  int firstRowOrRowTop;
  uint32_t lastVisibleRow;
  int highlightWidth;
  uint16_t **lastRowSlot;
  uint16_t **rowSlot;
  bool framebufferUnavailable;
  RichTextExtentRegs rowExtent;
  TextureSizeResult capSize;
  
  if (control->rowCount != 0) {
    firstRowOrRowTop = (clipBottom - (control->base).top) / (int)control->rowHeight;
    if (firstRowOrRowTop < 0) {
      firstRowOrRowTop = 0;
    }
    rowSlot = control->rowTextSlots + firstRowOrRowTop;
    lastVisibleRow = (int)((clipTop - (control->base).top) + control->rowHeight) / (int)control->rowHeight;
    firstRowOrRowTop = firstRowOrRowTop * control->rowHeight;
    if (control->rowCount <= lastVisibleRow) {
      lastVisibleRow = control->rowCount - 1;
    }
    lastRowSlot = control->rowTextSlots + lastVisibleRow;
    if (rowSlot <= lastRowSlot) {
      framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
      if (!framebufferUnavailable) {
        do {
          if (rowSlot == control->selectedRowSlot) {
            rowExtent = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,*rowSlot);
            highlightWidth = rowExtent.widthPixels + 6;
            if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_LIST_SELECTION,highlightWidth,
                         firstRowOrRowTop,0,control);
            }
            else {
              capSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_LEFT,
                                                              g_UiWindowTextureSource);
              highlightWidth = highlightWidth - capSize.logicalWidthPixels;
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_MIDDLE,
                         highlightWidth,firstRowOrRowTop,
                         capSize.logicalWidthPixels,control);
              g_GraphicsTextureSourceBlitSourceAlpha
                        (clipTop,clipLeft,clipBottom,clipRight,firstRowOrRowTop + (control->base).top,
                         (control->base).left,UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_LEFT,g_UiWindowTextureSource,
                         g_FramebufferAccess);
              g_GraphicsTextureSourceBlitSourceAlpha
                        (clipTop,clipLeft,clipBottom,clipRight,firstRowOrRowTop + (control->base).top,
                         highlightWidth + (control->base).left,UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_RIGHT,
                         g_UiWindowTextureSource,
                         g_FramebufferAccess);
            }
          }
          RichTextCommandStream_DrawSingleLine
                    (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,*rowSlot,
                     firstRowOrRowTop + 1 + (control->base).top,(control->base).left + 3);
          rowSlot++;
          firstRowOrRowTop = firstRowOrRowTop + control->rowHeight;
        } while (rowSlot <= lastRowSlot);
        g_GraphicsFramebufferEndAccess();
      }
    }
  }
  return;
}


/* Address: 0x004BA040.
   Primary button press on a text list (nonRightPress slot of g_UiTextListControlVtable): a click on the text
   of a row (its width plus 6 pixels) selects that row, scrolls it into view, queues the action and plays the
   selection sound when enabled. A double click also marks the selection confirmed and repeats the action for
   the already selected row; a single click on it does nothing.
*/
void UiTextListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextListControl *control)

{
  int32_t *topField;
  int32_t *leftField;
  uint32_t rowIndex;
  int controlLeftOrRowTop;
  RichTextExtentRegs rowExtent;
  uint16_t **clickedRowSlot;
  
  topField = &(control->base).top;
  if (((*topField <= pointerY) &&
      (leftField = &(control->base).left, controlLeftOrRowTop = *leftField, *leftField <= pointerX)) &&
     (rowIndex = (uint32_t)(pointerY - *topField) / control->rowHeight, rowIndex < control->rowCount)) {
    clickedRowSlot = control->rowTextSlots + rowIndex;
    rowExtent = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,*clickedRowSlot);
    if (pointerX - controlLeftOrRowTop < (int)(rowExtent.widthPixels + 6)) {
      control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_SELECTION_CONFIRMED;
      if ((((control->base).nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) ||
         (control->listStateFlags = control->listStateFlags & ~UI_TEXT_LIST_SELECTION_CONFIRMED,
         clickedRowSlot != control->selectedRowSlot)) {
        control->selectedRowSlot = clickedRowSlot;
        controlLeftOrRowTop = rowIndex * control->rowHeight;
        UiScrollableControl_ClampOffsetsToViewport
                  (controlLeftOrRowTop + 1 + control->rowHeight,(control->base).rightOffset,controlLeftOrRowTop,0,
                   (UiScrollableControl *)(control->base).parent);
        UiActionQueue_Enqueue(control->actionId,control);
        if (((control->listStateFlags & UI_TEXT_LIST_PLAY_SELECTION_SOUND) != 0) &&
           (control->activationSound != NULL)) {
          g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
        }
      }
    }
  }
  return;
}


/* Address: 0x004BA130.
   Keyboard handler of a text list (keyboardEvent slot of g_UiTextListControlVtable): with type search, a
   character selects the first row whose first code unit is not below it, ASCII case-insensitive (meant for
   sorted lists; the last row when there is none); Enter confirms the selection
   and queues the action; Home/End/Page Up/Page Down/Up/Down move the selection. A changed selection plays
   the selection sound when enabled, is scrolled into view and arms the deferred action, which
   UiTextListControl_TickActivationPulse queues after g_UiListActivationPulseFrames frames. Other keys go to
   UiNode_DefaultKeyboardEventMoveFocusNext. CF clear: consumed.
*/
bool UiTextListControl_HandleKeyboardNavigationAndSearch
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextListControl *control)

{
  uint16_t **previousSelectedSlot;
  uint16_t **slotCursorOrSelection;
  int rowIndexOrTopOrPulse;
  uint32_t pageDownRow;
  UiListRowCount remainingRows;
  uint16_t **candidateSlot;
  bool delegatedOrMismatch;
  UiScrollableContentDimensionsEdxEax8 contentSize;
  
  previousSelectedSlot = control->selectedRowSlot;
  if ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0) {
    if (((keyboardStateMask & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0) ||
       ((control->listStateFlags & UI_TEXT_LIST_TYPE_SEARCH_ENABLED) == 0)) {
      delegatedOrMismatch = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
      return delegatedOrMismatch;
    }
    remainingRows = control->rowCount;
    slotCursorOrSelection = control->rowTextSlots;
    if (remainingRows != 0) {
      do {
        candidateSlot = slotCursorOrSelection;
        delegatedOrMismatch = g_KeyboardAsciiCaseTransformCallbacks3.compareCaseInsensitiveFlags
                          (keyCode,*(uint32_t *)*candidateSlot);
        if (!delegatedOrMismatch) break;
        remainingRows--;
        slotCursorOrSelection = candidateSlot + 1;
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
    contentSize = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)(control->base).parent);
    rowIndexOrTopOrPulse = ((uint32_t)((int)control->selectedRowSlot - (int)control->rowTextSlots) >> 2) -
            ((int)((contentSize >> 0x20) / (uint64_t)control->rowHeight) - 1);
    if (rowIndexOrTopOrPulse < 0) {
      rowIndexOrTopOrPulse = 0;
    }
    control->selectedRowSlot = control->rowTextSlots + rowIndexOrTopOrPulse;
  }
  else if (keyCode == KEYBOARD_KEY_CODE_PAGE_DOWN) {
    contentSize = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)(control->base).parent);
    pageDownRow = ((uint32_t)((int)control->selectedRowSlot - (int)control->rowTextSlots) >> 2) +
            (int)((contentSize >> 0x20) / (uint64_t)control->rowHeight) - 1;
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
    delegatedOrMismatch = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return delegatedOrMismatch;
  }
  slotCursorOrSelection = control->selectedRowSlot;
  if (slotCursorOrSelection != previousSelectedSlot) {
    if (((control->listStateFlags & UI_TEXT_LIST_PLAY_SELECTION_SOUND) != 0) &&
       (control->activationSound != NULL)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
    }
    rowIndexOrTopOrPulse =
         ((uint32_t)((int)slotCursorOrSelection - (int)control->rowTextSlots) >> 2) * control->rowHeight;
    UiScrollableControl_ClampOffsetsToViewport
              (rowIndexOrTopOrPulse + control->rowHeight + 1,(control->base).rightOffset,rowIndexOrTopOrPulse,0,
               (UiScrollableControl *)(control->base).parent);
    /* arm the deferred action: the frame counter lives in the top byte */
    rowIndexOrTopOrPulse = g_UiListActivationPulseFrames;
    control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_DEFERRED_ACTION_PENDING;
    control->listStateFlags = control->listStateFlags & UI_LIST_FLAGS_MASK;
    control->listStateFlags = control->listStateFlags | rowIndexOrTopOrPulse << UI_LIST_COUNTDOWN_SHIFT;
  }
  return false;
}


/* Address: 0x004BA390.
   Per-frame tick of a text list (tick slot of g_UiTextListControlVtable): while a keyboard selection change
   is pending, counts its frame counter (top byte of listStateFlags) down and queues the list's action when it
   reaches 0, so quick key repeats queue only one action.
*/
void UiTextListControl_TickActivationPulse(UiTextListControl *control)

{
  if (((control->listStateFlags & UI_TEXT_LIST_DEFERRED_ACTION_PENDING) != 0) &&
     (control->listStateFlags = control->listStateFlags - UI_STATE_FRAME_COUNTER_UNIT,
     (control->listStateFlags & UI_LIST_COUNTDOWN_MASK) == 0)) {
    control->listStateFlags =
         control->listStateFlags &
         (UI_LIST_FLAGS_MASK & ~(UI_TEXT_LIST_DEFERRED_ACTION_PENDING|UI_TEXT_LIST_SELECTION_CONFIRMED));
    UiActionQueue_Enqueue(control->actionId,control);
  }
  return;
}


/* Address: 0x004BA3D0.
   Enables a text list whose action id matches (unsuppressActionId slot of g_UiTextListControlVtable), then
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


/* Address: 0x004BA400.
   Disables a text list whose action id matches (suppressActionId slot of g_UiTextListControlVtable), then
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


/* Address: 0x004BA430.
   Fills a pointer list whose rows are rich-text strings (rowPointers, one per row) and selects row 0. The
   list's size follows its content: one list-font line plus 1 pixel per row, and the widest measured row
   plus 6 pixels; the parent (the scrollable frame) is laid out again for the new size.
*/
void UiPointerList_InitializeMeasuredTextRows(UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control)

{
  UiNodeBase *parentNode;
  UiPixelExtent rowHeightPixels;
  uint32_t maximumTextWidthPixels;
  RichTextExtentRegs measuredTextExtent;
  GlyphSizeResult fontSize;
  UiNodeVtable *parentVtable;
  
  fontSize = FontGlyph_GetLogicalSizeForStyleRegs(g_UiListTextStyle,0);
  rowHeightPixels = fontSize.lineHeight + 1;
  control->rowHeight = rowHeightPixels;
  control->rowCount = rowCount;
  control->rowSlots = rowPointers;
  maximumTextWidthPixels = 0;
  control->selectedRowSlot = rowPointers;
  (control->base).bottomOffset = rowHeightPixels * rowCount + 1;
  for (; rowCount != 0; rowCount--) {
    measuredTextExtent = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,*rowPointers);
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


/* Address: 0x004BC490.
   Draws a wrapped multi-line label (drawClipped slot of g_UiListOffsetControlVtable): its text (a text
   resource or a command stream) wrapped at wrapWidth, which follows the layout width unless
   UI_LABEL_KEEP_WRAP_WIDTH, in g_UiTextStyleNormal with the label's font/palette overrides; then the
   children.
*/
void UiWrappedTextControl_DrawClipped(UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiWrappedTextControl *control)

{
  UiPackedTextStyle packedStyleOverride;
  uint16_t *commandStream;
  uint32_t textStyle;
  bool framebufferUnavailable;
  TextResolveResult resolvedText;
  
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
      commandStream = resolvedText.text;
    }
    RichTextCommandStream_DrawWrappedBlock
              (clipTop,clipLeft,clipBottom,clipRight,(textStyle | packedStyleOverride) & (UI_TEXT_STYLE_FONT_BYTE|UI_TEXT_STYLE_PALETTE_BYTE),
               commandStream,control->wrapWidth,(control->base).top,(control->base).left);
    g_GraphicsFramebufferEndAccess();
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,&control->base);
  return;
}


/* Address: 0x004BCC80.
   Draws a nine-slice panel (drawClipped slot of g_UiNineSlicePanelControlVtable) from eight consecutive
   frames starting at firstFrameSubresource: 0 top-left and 1 top-right corner, 2 top edge, 3 left edge,
   4 right edge, 5 bottom-left and 6 bottom-right corner, 7 bottom edge (edges tiled), and centerSubresource
   tiled over the interior; then the children. GRAPHICS_TILED_BLIT_ONE_TILE keeps an edge one tile thick.
*/
void UiNineSlicePanelControl_DrawTextureFrameAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNineSlicePanelControl *control)

{
  GraphicsSubresourceIndex baseTextureFrame;
  uint32_t slice4Width;
  int leftEdgeX;
  int topEdgeY;
  int bottomEdgeY;
  int rightEdgeX;
  bool framebufferUnavailable;
  TextureSizeResult slice0Size;
  TextureSizeResult slice1Size;
  TextureSizeResult slice2Size;
  TextureSizeResult slice3Size;
  TextureSizeResult slice4Or5Size;
  TextureSizeResult slice6Size;
  TextureSizeResult slice7Size;
  
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
                (clipTop,clipLeft,clipBottom,clipRight,topEdgeY,leftEdgeX,baseTextureFrame,
                 control->textureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,topEdgeY,rightEdgeX,
                 baseTextureFrame + 1,
                 control->textureSource,g_FramebufferAccess);
      leftEdgeX = leftEdgeX + slice0Size.logicalWidthPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,rightEdgeX,topEdgeY,leftEdgeX,
                 baseTextureFrame + 2,
                 control->textureSource,g_FramebufferAccess);
      leftEdgeX = leftEdgeX - slice0Size.logicalWidthPixels;
      topEdgeY = topEdgeY + slice0Size.logicalHeightPixels;
      bottomEdgeY = bottomEdgeY - slice4Or5Size.logicalHeightPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY,GRAPHICS_TILED_BLIT_ONE_TILE,topEdgeY,leftEdgeX,
                 baseTextureFrame + 3,
                 control->textureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY,leftEdgeX,
                 baseTextureFrame + 5,
                 control->textureSource,g_FramebufferAccess);
      rightEdgeX = (rightEdgeX + slice1Size.logicalWidthPixels) - slice4Width;
      topEdgeY = (topEdgeY - slice0Size.logicalHeightPixels) + slice1Size.logicalHeightPixels;
      bottomEdgeY = (bottomEdgeY + slice4Or5Size.logicalHeightPixels) - slice6Size.logicalHeightPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY,GRAPHICS_TILED_BLIT_ONE_TILE,topEdgeY,rightEdgeX,
                 baseTextureFrame + 4,control->textureSource,
                 g_FramebufferAccess);
      rightEdgeX = (rightEdgeX + slice4Width) - slice6Size.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY,rightEdgeX,
                 baseTextureFrame + 6,
                 control->textureSource,g_FramebufferAccess);
      leftEdgeX = leftEdgeX + slice4Or5Size.logicalWidthPixels;
      bottomEdgeY = (bottomEdgeY + slice6Size.logicalHeightPixels) - slice7Size.logicalHeightPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,rightEdgeX,bottomEdgeY,leftEdgeX,
                 baseTextureFrame + 7,
                 control->textureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY,
                 (rightEdgeX + slice6Size.logicalWidthPixels) - slice4Width,
                 (topEdgeY - slice1Size.logicalHeightPixels) + slice2Size.logicalHeightPixels,
                 (leftEdgeX - slice4Or5Size.logicalWidthPixels) + slice3Size.logicalWidthPixels,
                 control->centerSubresource,control->textureSource,
                 g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
    }
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,&control->base);
  return;
}


/* Address: 0x00515830.
   Relocation of a loaded resource gauge (relocate slot of g_UiFormattedContainerVtable): when it has a
   tooltip, points payloads 0, 1 (and 2 with UI_GAUGE_HAS_MARKER) of the tooltip text (the resource id stored
   just before the node) at the gauge's own value strings, so the tooltip always shows the current numbers;
   the value strings start out empty. Then the children are relocated.
*/
void UiFormattedContainer_RelocateWithPatchedTextPayloads
          (UiSerializedRelocationDelta relocationDelta,UiFormattedContainer *control)

{
  uint16_t *stream;
  TextResolveResult resolvedText;
  
  if (((control->base).nodeFlags & UI_NODE_TOOLTIP_ELIGIBLE) != 0) {
    resolvedText = TextResource_Resolve(((TextResourceId *)control)[-1]);
    stream = resolvedText.text;
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


/* Address: 0x005158B0.
   Draws a resource gauge (drawClipped slot of g_UiFormattedContainerVtable): the empty bar (frames
   firstFrameSubresource + 0/1/2: left cap, tiled middle, right cap), the fill up to currentValue in one of six
   colour variants (+3, +6, ... +18, chosen by the fill percentage of the limit or marker), and marker frame
   +21 at limitValue and, with UI_GAUGE_HAS_MARKER, at markerValue. The scale starts at
   initialScaleRange and grows by factors of 4 until every value fits. Afterwards (also when nothing is
   drawn) the value strings for the tooltip are refreshed.
*/
void UiFormattedContainer_DrawClipped
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiFormattedContainer *control)

{
  int primaryValue;
  int secondaryValue;
  uint32_t fillPercent;
  int scaleRange;
  int scaleLimit;
  uint32_t textureFrame;
  int barEndOrSpanOrMarkerX;
  int variantOrFillEnd;
  int barStartX;
  bool framebufferUnavailable;
  TextureSizeResult frameSize;
  int tertiaryMarkerOffset;
  
  if (control->limitValue != 0 &&
      (framebufferUnavailable = g_GraphicsFramebufferBeginAccess(), !framebufferUnavailable)) {
    if (clipRight < (control->base).left) {
      clipRight = (control->base).left;
    }
    if (clipBottom < (control->base).top) {
      clipBottom = (control->base).top;
    }
    if ((control->base).right < clipLeft) {
      clipLeft = (control->base).right;
    }
    if ((control->base).bottom < clipTop) {
      clipTop = (control->base).bottom;
    }
    textureFrame = control->firstFrameSubresource;
    barStartX = (control->base).left;
    barEndOrSpanOrMarkerX = (control->base).right;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,barStartX,textureFrame,
               control->textureSource,g_FramebufferAccess);
    frameSize = g_GraphicsTextureSourceGetLogicalSize
                       (textureFrame,control->textureSource);
    barStartX = barStartX + frameSize.logicalWidthPixels;
    frameSize = g_GraphicsTextureSourceGetLogicalSize
                       (textureFrame + UI_GAUGE_FRAME_END_CAP,control->textureSource);
    barEndOrSpanOrMarkerX = barEndOrSpanOrMarkerX - frameSize.logicalWidthPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,barEndOrSpanOrMarkerX,
               textureFrame + UI_GAUGE_FRAME_END_CAP,
               control->textureSource,g_FramebufferAccess);
    if (barStartX < barEndOrSpanOrMarkerX) {
      GraphicsTextureSource_BlitTiledSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,barEndOrSpanOrMarkerX,
                 (control->base).top,barStartX,textureFrame + UI_GAUGE_FRAME_TRACK,
                 control->textureSource,g_FramebufferAccess);
      primaryValue = control->currentValue;
      scaleRange = control->initialScaleRange;
      barEndOrSpanOrMarkerX = barEndOrSpanOrMarkerX - barStartX;
      /* Grow the scale by factors of 4 until it covers the value, the limit and the marker. */
      while( true ) {
        for (; (scaleRange < primaryValue || (scaleRange < control->limitValue)); scaleRange = scaleRange << 2) {
        }
        if (((control->gaugeFlags & UI_GAUGE_HAS_MARKER) == 0) ||
           (((UiFormattedContainerWithMarker *)control)->markerValue <= scaleRange)) break;
        scaleRange = scaleRange << 2;
      }
      if ((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) {
        tertiaryMarkerOffset =
             (int)(((int64_t)((UiFormattedContainerWithMarker *)control)->markerValue * (int64_t)barEndOrSpanOrMarkerX) /
                   (int64_t)scaleRange);
      }
      secondaryValue = control->limitValue;
      scaleLimit = control->limitValue;
      if (((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) &&
          (((UiFormattedContainerWithMarker *)control)->markerValue < scaleLimit)) {
        scaleLimit = ((UiFormattedContainerWithMarker *)control)->markerValue;
      }
      fillPercent = (uint32_t)(((int64_t)primaryValue * 100) / (int64_t)scaleLimit);
      /* Fill colour variant (frame offset 3..18, three frames each) by the fill percentage: rising from 80%
         on, or for the two-sided scale also falling below 40%. */
      if ((control->gaugeFlags & UI_GAUGE_TWO_SIDED_SCALE) == 0) {
        variantOrFillEnd = 3;
        if ((((79 < fillPercent) && (variantOrFillEnd = 6, 83 < fillPercent)) &&
             (variantOrFillEnd = 9, 87 < fillPercent)) &&
           ((variantOrFillEnd = 12, 91 < fillPercent && (variantOrFillEnd = 15, 95 < fillPercent)))) {
          variantOrFillEnd = 18;
        }
      }
      else {
        variantOrFillEnd = 18;
        if (((((7 < fillPercent) && (variantOrFillEnd = 15, 15 < fillPercent)) &&
              (variantOrFillEnd = 12, 23 < fillPercent)) &&
            ((((variantOrFillEnd = 9, 31 < fillPercent && (variantOrFillEnd = 6, 39 < fillPercent)) &&
              ((variantOrFillEnd = 3, 85 < fillPercent && ((variantOrFillEnd = 6, 87 < fillPercent &&
                                                            (variantOrFillEnd = 9, 89 < fillPercent))))))
             && (variantOrFillEnd = 12, 91 < fillPercent)))) && (variantOrFillEnd = 15, 93 < fillPercent)) {
          variantOrFillEnd = 18;
        }
      }
      textureFrame = variantOrFillEnd + control->firstFrameSubresource;
      if (control->currentValue != 0) {
        frameSize = g_GraphicsTextureSourceGetLogicalSize
                           (textureFrame,control->textureSource);
        barStartX = barStartX - frameSize.logicalWidthPixels;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,barStartX,textureFrame,
                   control->textureSource,g_FramebufferAccess);
        barStartX = barStartX + frameSize.logicalWidthPixels;
        variantOrFillEnd =
             (int)(((int64_t)primaryValue * (int64_t)barEndOrSpanOrMarkerX) / (int64_t)scaleRange) + barStartX;
        GraphicsTextureSource_BlitTiledSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,GRAPHICS_TILED_BLIT_ONE_TILE,variantOrFillEnd,
                   (control->base).top,barStartX,
                   textureFrame + UI_GAUGE_FRAME_TRACK,control->textureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,variantOrFillEnd,
                   textureFrame + UI_GAUGE_FRAME_END_CAP,
                   control->textureSource,g_FramebufferAccess);
      }
      barEndOrSpanOrMarkerX =
           barStartX + (int)(((int64_t)secondaryValue * (int64_t)barEndOrSpanOrMarkerX) / (int64_t)scaleRange);
      textureFrame = control->firstFrameSubresource + UI_GAUGE_FRAME_MARKER;
      frameSize = g_GraphicsTextureSourceGetLogicalSize
                         (textureFrame,control->textureSource);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,
                 barEndOrSpanOrMarkerX - ((int)frameSize.logicalWidthPixels >> 1),textureFrame,
                 control->textureSource,g_FramebufferAccess);
      if ((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) {
        textureFrame = control->firstFrameSubresource + UI_GAUGE_FRAME_MARKER;
        frameSize = g_GraphicsTextureSourceGetLogicalSize
                           (textureFrame,control->textureSource);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,
                   (barStartX + tertiaryMarkerOffset) - ((int)frameSize.logicalWidthPixels >> 1),textureFrame,
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


/* Address: 0x00516D10.
   Draws the army metrics panel (drawClipped slot of g_UiArmyMetricsPanelVtable): the aligned panel texture
   like an image panel, then, when an entity is attached, its runtime metrics via
   SelectionPanel_RenderArmyRuntimeMetrics with the info-panel graphics temporarily installed as the
   selection-panel graphics; then the children.
*/
void UiArmyMetricsPanel_DrawTextureMetricsAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiArmyMetricsPanel *control)

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
  bool framebufferUnavailable;
  TextureSizeResult textureSize;
  
  if (((control->base).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    clippedLeft = (control->base).base.left;
    if ((control->base).base.left < clipRight) {
      clippedLeft = clipRight;
    }
    clippedTop = (control->base).base.top;
    if ((control->base).base.top < clipBottom) {
      clippedTop = clipBottom;
    }
    clippedRight = (control->base).base.right;
    if (clipLeft < (control->base).base.right) {
      clippedRight = clipLeft;
    }
    clippedBottom = (control->base).base.bottom;
    if (clipTop < (control->base).base.bottom) {
      clippedBottom = clipTop;
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
                    (clipTop,clipLeft,clipBottom,clipRight,(control->base).base.bottom,(control->base).base.right,
                     (control->base).base.top,(control->base).base.left,control->entity);
          g_SelectionPanelTextureSource = savedTextureSource;
          g_SelectionPanelData = savedPanelData;
        }
      }
      UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,&control->base.base);
    }
  }
  return;
}


/* Address: 0x00519110.
   Draws a software texture preview (drawClipped slot of g_UiSoftwareTexturePreviewControlVtable): the
   outgoing and incoming subresources blended bilinearly and scaled over the layout box (a crossfade driven by
   the control's blend buffers), then the children.
*/
void UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiSoftwareTexturePreviewControl *control)

{
  bool framebufferUnavailable;
  
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
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,&control->base);
  return;
}


/* Address: 0x00519190.
   Primary button press on a software texture preview (nonRightPress slot of
   g_UiSoftwareTexturePreviewControlVtable): queues the control's action.
*/
void UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control)

{
  UiActionQueue_Enqueue(control->actionId,control);
  return;
}


/* Address: 0x005191B0.
   Secondary button press on a software texture preview (rightPress slot of
   g_UiSoftwareTexturePreviewControlVtable): queues the control's action, like the primary button.
*/
void UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control)

{
  UiActionQueue_Enqueue(control->actionId,control);
  return;
}


/* Address: 0x005191D0.
   Keyboard handler of a software texture preview (keyboardEvent slot of
   g_UiSoftwareTexturePreviewControlVtable): Tab moves the focus on, any other key queues the control's action.
   Always consumed (CF clear).
*/
bool UiSoftwareTexturePreviewControl_HandleKeyboardActivation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSoftwareTexturePreviewControl *control)

{
  if (keyCode != KEYBOARD_KEY_CODE_TAB) {
    UiActionQueue_Enqueue(control->actionId,control);
    return false;
  }
  UiKeyboardFocus_MoveNext();
  return false;
}


/* Address: 0x004B0150.
   Tracks which node the tooltip belongs to: remembers the pointer position and takes the node under the
   pointer in the top root (only while no button holds a capture, and only tooltip-eligible nodes). When the
   target changes, the tooltip delay starts over and the text of the previous target is prepared again.
*/
void UiTooltip_UpdateHoverTarget(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiNodeBase *node;
  UiNodeBase *hitTestNode;
  
  node = g_UiTooltipState.targetNode;
  g_UiTooltipState.pointerX = pointerX;
  g_UiTooltipState.pointerY = pointerY;
  g_UiTooltipState.targetNode = NULL;
  if ((((((g_UiPointerCaptureTarget == UI_NODE_NONE) &&
         (g_UiRootNode != UI_ROOT_STACK_END)) && ((g_UiRootNode->base).left <= pointerX)) &&
       (((g_UiRootNode->base).top <= pointerY && (pointerX < (g_UiRootNode->base).right)))) &&
      ((pointerY < (g_UiRootNode->base).bottom &&
       ((hitTestNode = (*((g_UiRootNode->base).vtable)->hitTest)
                                 (pointerY,pointerX,&g_UiRootNode->base),
        hitTestNode != UI_NODE_NONE &&
        ((hitTestNode->nodeFlags & UI_NODE_TOOLTIP_ELIGIBLE) != 0)))))) &&
     (g_UiTooltipState.targetNode = hitTestNode, hitTestNode == node)) {
    return;
  }
  g_UiTooltipState.countdownFrames = g_UiTooltipDelayFrames;
  UiTooltip_PrepareTargetText(node);
  return;
}


/* Address: 0x004B6520.
   Rewrites the text of a numeric text edit from currentValue: clears the 16-code-unit buffer, writes a '-'
   for a negative signed value, then the magnitude in decimal or upper-case hexadecimal, and updates the
   range validity. Called by UiNumericTextEditControl_RelocateAndRebuildText.
*/
void UiNumericTextControl_RebuildTextFromValue(UiNumericTextControl *control)

{
  uint32_t remainingValue;
  int8_t rotateShift;
  uint32_t highBitOrDigitsLeft;
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
    remainingValue = -remainingValue;
    outputCursor = control->textBuffer + 1;
  }
  if ((control->editStateFlags & UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT) == 0) {
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,remainingValue,outputCursor);
  }
  else {
    highBitOrDigitsLeft = 0x1f;
    if (remainingValue != 0) {
      for (; remainingValue >> highBitOrDigitsLeft == 0; highBitOrDigitsLeft--) {
      }
    }
    if (remainingValue != 0) {
      /* NOTE: faithful to the original (BSR; AND 0x1c; ROR; SHR 2): the leading nonzero hex digit
         is dropped, and a value below 0x10 gives a digit count of 0, which the DEC/JNZ loop wraps
         (runaway write). */
      rotateShift = (int8_t)(highBitOrDigitsLeft & 0x1c);
      highBitOrDigitsLeft = (highBitOrDigitsLeft & 0x1c) >> 2;
      remainingValue = remainingValue >> rotateShift | remainingValue << 0x20 - rotateShift;
      do {
        digitCodeUnit = (remainingValue >> 0x1c) + '0';
        if ('9' < digitCodeUnit) {
          digitCodeUnit = digitCodeUnit + ('A' - '9' - 1);
        }
        *outputCursor = (uint16_t)digitCodeUnit;
        highBitOrDigitsLeft--;
        remainingValue = remainingValue << 4;
        outputCursor++;
      } while (highBitOrDigitsLeft != 0);
    }
    else {
      *outputCursor = '0';
    }
  }
  UiNumericTextControl_UpdateRangeValidity(control);
  return;
}


/* Address: 0x004B65F0.
   Parses the text of a numeric text edit (optional '-', then decimal or hexadecimal digits) into
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
  if (*textCursor != 0) {
    if (*textCursor == '-') {
      sign = -1;
      textCursor = control->textBuffer + 1;
    }
    if ((control->editStateFlags & UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT) == 0) {
      parsedValue = 0;
      for (; codeUnit = (uint32_t)*textCursor, codeUnit != 0; textCursor++) {
        if ((codeUnit < '0') || (9 < codeUnit - '0')) {
          control->editStateFlags = control->editStateFlags & ~UI_NUMERIC_TEXT_VALUE_VALID;
          return;
        }
        parsedValue = parsedValue * 10 + (codeUnit - '0');
      }
    }
    else {
      parsedValue = 0;
      for (; codeUnit = (uint32_t)*textCursor, codeUnit != 0; textCursor++) {
        /* digit value of '0'-'9', 'A'-'F' or 'a'-'f' */
        digitValue = codeUnit - '0';
        if ((codeUnit < '0') ||
           ((9 < digitValue &&
            ((digitValue = codeUnit - ('A' - 10), digitValue < 10 ||
             ((0xf < digitValue && ((digitValue = codeUnit - ('a' - 10), digitValue < 10 ||
                                     (0xf < digitValue)))))))))) {
          control->editStateFlags = control->editStateFlags & ~UI_NUMERIC_TEXT_VALUE_VALID;
          return;
        }
        parsedValue = parsedValue << 4 | digitValue & 0xf;
      }
    }
    if ((-1 < sign) ||
       (parsedValue = -parsedValue, (control->editStateFlags & UI_NUMERIC_TEXT_SIGNED_VALUE) != 0)) {
      control->currentValue = parsedValue;
      if ((control->editStateFlags & UI_NUMERIC_TEXT_ACTION_ON_ENTER_ONLY) == 0) {
        UiActionQueue_Enqueue(control->actionId,control);
      }
      UiNumericTextControl_UpdateRangeValidity(control);
      return;
    }
  }
  /* Empty text, or a negative value for an unsigned control: invalid. */
  control->editStateFlags = control->editStateFlags & ~UI_NUMERIC_TEXT_VALUE_VALID;
  return;
}


/* Address: 0x004B0250.
   Prepares the tooltip of node (NULL: nothing to do): the dword stored just before the node is its tooltip,
   either a UTF-16 text pointer (UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) or a text resource id. The text is
   measured in the tooltip style and the UI is redrawn.
*/
void UiTooltip_PrepareTargetText(UiNodeBase *node)

{
  uint16_t *commandStream;
  TextResolveResult resolvedText;

  if (node != NULL) {
    /* the last field of the (virtual) node before this one = the dword at node - 4 */
    commandStream = (uint16_t *)node[-1].nodeFlags;
    if ((node->nodeFlags & UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)commandStream);
      commandStream = resolvedText.text;
    }
    RichTextCommandStream_MeasureRegs(g_UiTooltipTextStyle,commandStream);
    /* the size of window piece 0xBC is queried but not used */
    g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TOOLTIP_LEFT,g_UiWindowTextureSource);
    UiRootStack_InvalidateAll();
  }
  return;
}


/* Address: 0x004B66E0.
   Sets UI_NUMERIC_TEXT_VALUE_VALID exactly when currentValue lies within minimumValue..maximumValue,
   compared signed for UI_NUMERIC_TEXT_SIGNED_VALUE and unsigned otherwise. Called by
   UiNumericTextControl_RebuildTextFromValue and UiNumericTextControl_ParseAndCommitValue.
*/
void UiNumericTextControl_UpdateRangeValidity(UiNumericTextControl *control)

{
  uint32_t currentNumericValue;
  bool outOfRange;

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


/* Address: 0x004B6740.
   Width in pixels of the first prefixLength code units of a text edit's text (fewer if the text ends
   before), measured glyph by glyph in g_UiTextEditActiveTextStyle. Used for the selection, caret and scroll
   positions by the text edit draw and layout functions.
*/
UiPixelCoordinate UiTextEditControl_MeasurePrefixWidth(UiTextCodeUnitCount prefixLength,UiTextEditControl *control)

{
  int accumulatedWidth;
  uint32_t glyphIndex;
  GlyphSizeResult glyphSize;
  
  glyphIndex = 0;
  accumulatedWidth = 0;
  if (prefixLength != 0) {
    do {
      if (control->textBuffer[glyphIndex] == 0) {
        return accumulatedWidth;
      }
      glyphSize = FontGlyph_GetLogicalSizeForStyleRegs
                        (g_UiTextEditActiveTextStyle,(uint32_t)control->textBuffer[glyphIndex]);
      glyphIndex++;
      accumulatedWidth = accumulatedWidth + glyphSize.width;
    } while (glyphIndex < prefixLength);
  }
  return accumulatedWidth;
}


/* Address: 0x004B6790.
   Cursor index for a pointer x in a text edit: the index of the glyph under the pointer (the first one whose
   right edge reaches it), or the text length past the end. Accounts for the horizontal scroll and, with
   UI_TEXT_EDIT_DRAW_FRAMED_CHROME, the frame width. Used by the pointer press and drag handlers.
*/
UiTextCodeUnitCount UiTextEditControl_FindCursorIndexAtX(UiPixelCoordinate pointerX,UiTextEditControl *control)

{
  int nextTextIndex;
  int targetOffsetX;
  int currentTextIndex;
  int measuredPrefixWidthPixels;
  TextureSizeResult decorationSize;
  GlyphSizeResult glyphSize;
  
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
    glyphSize = FontGlyph_GetLogicalSizeForStyleRegs
                      (g_UiTextEditActiveTextStyle,(uint32_t)control->textBuffer[currentTextIndex]);
    measuredPrefixWidthPixels = measuredPrefixWidthPixels + glyphSize.width;
    nextTextIndex = currentTextIndex + 1;
  } while (measuredPrefixWidthPixels < targetOffsetX);
  return currentTextIndex;
}


/* Address: 0x004B7010.
   Sets UI_TEXT_EDIT_VALUE_VALID of a DOS path edit from g_FileSystemValidateDos83Path, which gets the
   control's UI_PATH_TEXT_ALLOW_WILDCARDS and UI_PATH_TEXT_NAME_ONLY bits shifted down to bits 0-1. Called by
   the path edit's keyboard handler and relocation.
*/
void UiPathTextControl_UpdateDos83Validity(UiPathTextEditControl *control)

{
  bool validatorRejected;

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


/* Address: 0x004B78F0.
   A text control's value is valid (UI_TEXT_EDIT_VALUE_VALID) exactly when its text is not empty.
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


/* Address: 0x004BB570.
   Compares two rich-text streams for the pointer-list sorts: both are expanded (nested streams inlined) into
   1 KiB scratch buffers and compared with Utf16String_CompareAsciiCaseInsensitiveFlags; the result is its
   flags (CF: leftText < rightText, ZF: equal). Called by the UiPointerList_SortByExpandedTextField* sorts.
*/
TextCompareResult UiPointerList_CompareExpandedTextFlags(uint16_t *rightText,uint16_t *leftText)

{
  TextCompareResult compareFlags;

  RichTextCommandStream_CopyExpanded
            (UI_POINTER_LIST_COMPARE_SCRATCH_BYTES,(uint16_t *)&g_UiPointerListExpandedLeftTextUtf16,leftText);
  RichTextCommandStream_CopyExpanded
            (UI_POINTER_LIST_COMPARE_SCRATCH_BYTES,(uint16_t *)&g_UiPointerListExpandedRightTextUtf16,rightText);
  /* CF and ZF are the comparator's: nothing after the call changes the flags (0x004BB5A5). */
  compareFlags = (*(TextCompareResult (*)(uint16_t *,uint16_t *))g_Utf16StringCompareAsciiCaseInsensitiveFlags)
            ((uint16_t *)&g_UiPointerListExpandedRightTextUtf16,(uint16_t *)&g_UiPointerListExpandedLeftTextUtf16);
  return compareFlags;
}


/* Address: 0x004B5D00.
   Layout of a text edit (layout slot of the numeric, path and required text edit vtables; also called after
   every edit): refreshes the layout size and scrolls horizontally just enough to keep the glyphs before and
   after the cursor visible, or not at all while the whole text (plus caret and frame) fits.
*/
void UiTextEditControl_RecomputeLayoutAndClampScroll(UiTextEditControl *control)

{
  UiTextCodeUnitCount prefixLength;
  UiPixelOffset cursorWidthOrMinScroll;
  UiPixelCoordinate fullTextWidth;
  int overflowOrContentWidth;
  UiPixelOffset maxScrollOffset;
  GlyphSizeResult glyphSize;
  TextureSizeResult decorationSize;
  
  (control->base).layoutWidth = (control->base).right - (control->base).left;
  (control->base).layoutHeight = (control->base).bottom - (control->base).top;
  prefixLength = control->cursorIndex;
  cursorWidthOrMinScroll = UiTextEditControl_MeasurePrefixWidth(prefixLength,control);
  maxScrollOffset = cursorWidthOrMinScroll;
  if (prefixLength != 0) {
    glyphSize = FontGlyph_GetLogicalSizeActiveRegs((uint32_t)control->textBuffer[prefixLength - 1]);
    maxScrollOffset = cursorWidthOrMinScroll - glyphSize.width;
  }
  overflowOrContentWidth = cursorWidthOrMinScroll - (control->base).layoutWidth;
  if (control->textBuffer[prefixLength] != 0) {
    glyphSize = FontGlyph_GetLogicalSizeActiveRegs((uint32_t)control->textBuffer[prefixLength]);
    overflowOrContentWidth = overflowOrContentWidth + glyphSize.width;
  }
  /* NOTE: as in the original (PUSH 0x10), only the first 16 code units (the numeric text buffer) count, also
     for the longer path and required text edits. */
  fullTextWidth = UiTextEditControl_MeasurePrefixWidth(UI_NUMERIC_TEXT_BUFFER_UNITS,control);
  decorationSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_CARET_OVERWRITE,g_UiWindowTextureSource);
  cursorWidthOrMinScroll = overflowOrContentWidth + decorationSize.logicalWidthPixels;
  overflowOrContentWidth = fullTextWidth + decorationSize.logicalWidthPixels;
  if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_FRAMED_CHROME) != 0) {
    decorationSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME,
                                                           g_UiWindowTextureSource);
    overflowOrContentWidth = overflowOrContentWidth + decorationSize.logicalWidthPixels * 2;
    cursorWidthOrMinScroll = cursorWidthOrMinScroll + decorationSize.logicalWidthPixels * 2;
  }
  if ((control->base).layoutWidth < overflowOrContentWidth) {
    if ((int)maxScrollOffset < (int)control->horizontalScrollPixels) {
      control->horizontalScrollPixels = maxScrollOffset;
    }
    else if ((int)control->horizontalScrollPixels < (int)cursorWidthOrMinScroll) {
      control->horizontalScrollPixels = cursorWidthOrMinScroll;
    }
  }
  else {
    control->horizontalScrollPixels = 0;
  }
  return;
}


/* Address: 0x004B2E60.
   Draws a text button (drawClipped slot of g_UiNodeVtable_004B2CE0; also called by the adapter, numeric-pair
   and payload-pair buttons after patching their text): the push-button or checkbox graphic for its state
   (pressed/checked, alternate, disabled), then its text 6 pixels right of the graphic, vertically centred,
   with the focus mark and its shadow while it has keyboard focus. Children are not drawn.
*/
void UiTextButtonControl_DrawClipped(UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control)

{
  uint16_t *commandStream;
  uint32_t buttonFrame;
  int textXOrFocusEnd;
  int focusTileXOrTop;
  int textYOrTileX;
  uint32_t textStyle;
  bool framebufferUnavailable;
  RichTextExtentRegs textExtent;
  TextResolveResult resolvedText;
  TextureSizeResult skinSizeOrEndCapSize;
  TextureSizeResult focusTileSize;
  int textX;
  int textY;
  
  framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
  if (framebufferUnavailable) {
    return;
  }
  buttonFrame = UI_WINDOW_SUBRESOURCE_PUSH_BUTTON;
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    buttonFrame = UI_WINDOW_SUBRESOURCE_PUSH_BUTTON + 2;
  }
  if ((((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) &&
     (buttonFrame = buttonFrame - (UI_WINDOW_SUBRESOURCE_PUSH_BUTTON - UI_WINDOW_SUBRESOURCE_CHECKBOX),
      ((control->selectable).stateFlags & UI_BUTTON_ALTERNATE_STATE) != 0)) {
    buttonFrame = UI_WINDOW_SUBRESOURCE_CHECKBOX + 4;
  }
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    if (((control->selectable).stateFlags & UI_BUTTON_HIDDEN_WHILE_SUPPRESSED) != 0) {
      g_GraphicsFramebufferEndAccess();
      return;
    }
    buttonFrame++;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
             (control->selectable).base.left,buttonFrame,g_UiWindowTextureSource,g_FramebufferAccess);
  skinSizeOrEndCapSize = g_GraphicsTextureSourceGetLogicalSize(buttonFrame,g_UiWindowTextureSource);
  textXOrFocusEnd = skinSizeOrEndCapSize.logicalWidthPixels + 6;
  resolvedText = TextResource_Resolve(control->textResourceId);
  commandStream = resolvedText.text;
  textExtent = RichTextCommandStream_MeasureRegs(control->packedTextStyle,commandStream);
  textYOrTileX = (int)(skinSizeOrEndCapSize.logicalHeightPixels - textExtent.heightPixels) >> 1;
  textStyle = g_UiTextStyleDisabled;
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (textStyle = g_UiTextStyleNormal, ((control->selectable).stateFlags & UI_BUTTON_ALTERNATE_STATE) != 0)) {
    textStyle = g_UiTextStyleAlternate;
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
              (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,
               textYOrTileX + (control->selectable).base.top,textXOrFocusEnd + (control->selectable).base.left);
  }
  else {
    textXOrFocusEnd = textXOrFocusEnd + (control->selectable).base.left;
    textYOrTileX = textYOrTileX + (control->selectable).base.top;
    textX = textXOrFocusEnd;
    textY = textYOrTileX;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,textYOrTileX,textXOrFocusEnd - 2,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    skinSizeOrEndCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                                 g_UiWindowTextureSource);
    focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                          g_UiWindowTextureSource);
    focusTileXOrTop = textXOrFocusEnd - 2 + focusTileSize.logicalWidthPixels;
    focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                          g_UiWindowTextureSource);
    textXOrFocusEnd = ((textXOrFocusEnd + 4) - skinSizeOrEndCapSize.logicalWidthPixels) + textExtent.widthPixels;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,textYOrTileX,textXOrFocusEnd,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    if (clipLeft < textXOrFocusEnd) {
      textXOrFocusEnd = clipLeft;
    }
    do {
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipTop,textXOrFocusEnd,clipBottom,clipRight,textYOrTileX,focusTileXOrTop,TEXT_SHADOW_COLOR_ARGB,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      focusTileXOrTop = focusTileXOrTop + focusTileSize.logicalWidthPixels;
    } while (focusTileXOrTop < textXOrFocusEnd);
    focusTileXOrTop = textY - 1;
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,textY,textX);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,focusTileXOrTop,textX - 3,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    skinSizeOrEndCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                                 g_UiWindowTextureSource);
    focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                          g_UiWindowTextureSource);
    textYOrTileX = textX - 3 + focusTileSize.logicalWidthPixels;
    focusTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                          g_UiWindowTextureSource);
    textXOrFocusEnd = ((textX + 3) - skinSizeOrEndCapSize.logicalWidthPixels) + textExtent.widthPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,focusTileXOrTop,textXOrFocusEnd,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,g_UiWindowTextureSource,
               g_FramebufferAccess);
    if (clipLeft < textXOrFocusEnd) {
      textXOrFocusEnd = clipLeft;
    }
    do {
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,textXOrFocusEnd,clipBottom,clipRight,focusTileXOrTop,textYOrTileX,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      textYOrTileX = textYOrTileX + focusTileSize.logicalWidthPixels;
    } while (textYOrTileX < textXOrFocusEnd);
  }
  g_GraphicsFramebufferEndAccess();
  return;
}

