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
   Ownership: ui/controls/text.
   Purpose: Decrements the tooltip countdown while the target remains eligible. On expiry, resolves and prepares
   the target text; otherwise refreshes hover tracking from the last pointer position.
   Local calls: UiTooltip_PrepareTargetText, UiTooltip_UpdateHoverTarget.
*/
void __thandor_void_preserve_eax_ecx_edx UiTooltip_TickCountdown(void)

{
  if ((g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) &&
     ((g_UiTooltipState.targetNode == (UiNodeBase *)0x0 ||
      (((g_UiTooltipState.targetNode)->nodeFlags & UI_NODE_SUPPRESSED) == 0)))) {
    if ((g_UiTooltipState.countdownFrames != 0) &&
       (g_UiTooltipState.countdownFrames = g_UiTooltipState.countdownFrames - 1,
       g_UiTooltipState.countdownFrames == 0)) {
      UiTooltip_PrepareTargetText(g_UiTooltipState.targetNode);
    }
    return;
  }
  UiTooltip_UpdateHoverTarget(g_UiTooltipState.pointerY,g_UiTooltipState.pointerX);
  return;
}


/* Address: 0x004B5F20.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[12]@004B58A0.
   Local calls: UiTextEditControl_RecomputeLayoutAndClampScroll, UiNumericTextControl_ParseAndCommitValue.
   Cross-module calls: UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input], UiActionQueue_Enqueue
   [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiNumericTextEditControl_HandleKeyboardAndCommitCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiNumericTextControl *control)

{
  int *selectionBoundary;
  ushort displacedCodeUnit;
  UiNumericTextEditStateFlags requiredFormatFlag;
  UiTextCodeUnitIndex codeUnitIndex;
  int countOrFieldOffset;
  int shiftCountOrScanIndex;
  uint insertIndex;
  word *sourceCursor;
  word *destinationCursor;
  bool delegatedResult;
  
  if ((((control->editStateFlags & UI_NUMERIC_TEXT_READ_ONLY) != 0) ||
      (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0)) || ((keyboardStateMask & 0x30) != 0))
  goto UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
  if ((keyCode & 0xffff0000) == 0) {
    if (keyCode == 0x2d) {
      requiredFormatFlag = control->editStateFlags & UI_NUMERIC_TEXT_SIGNED_VALUE;
joined_r0x004b61ef:
      if (requiredFormatFlag == 0) {
UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent:
        delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNextCf
                           (keyboardStateMask,keyCode,&control->base);
        return delegatedResult;
      }
    }
    else {
      if (keyCode < 0x30) goto UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
      if (0x39 < keyCode) {
        if ((keyCode < 0x41) || ((0x46 < keyCode && ((keyCode < 0x61 || (0x66 < keyCode))))))
        goto UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        requiredFormatFlag = control->editStateFlags & UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT;
        goto joined_r0x004b61ef;
      }
    }
    insertIndex = control->cursorIndex;
    if ((insertIndex != control->selectionStart) || (insertIndex != control->selectionEnd)) {
      codeUnitIndex = control->selectionEnd;
      countOrFieldOffset = codeUnitIndex - control->selectionStart;
      sourceCursor = control->textBuffer + codeUnitIndex;
      destinationCursor = control->textBuffer + control->selectionStart;
      for (shiftCountOrScanIndex = 0x10 - codeUnitIndex; shiftCountOrScanIndex != 0; shiftCountOrScanIndex = shiftCountOrScanIndex + -1) {
        *destinationCursor = *sourceCursor;
        sourceCursor = sourceCursor + 1;
        destinationCursor = destinationCursor + 1;
      }
      for (; countOrFieldOffset != 0; countOrFieldOffset = countOrFieldOffset + -1) {
        *destinationCursor = 0;
        destinationCursor = destinationCursor + 1;
      }
      insertIndex = control->selectionStart;
      control->cursorIndex = insertIndex;
      control->selectionEnd = insertIndex;
    }
    if (insertIndex < 0xe) {
      control->cursorIndex = control->cursorIndex + 1;
      control->selectionStart = control->selectionStart + 1;
      control->selectionEnd = control->selectionEnd + 1;
      if ((control->editStateFlags & UI_NUMERIC_TEXT_OVERWRITE_MODE) == 0) {
        do {
          LOCK();
          displacedCodeUnit = control->textBuffer[insertIndex];
          control->textBuffer[insertIndex] = (ushort)keyCode;
          keyCode = (UiKeyboardEventCode)displacedCodeUnit;
          UNLOCK();
          insertIndex = insertIndex + 1;
        } while (insertIndex < 0xe);
      }
      else {
        control->textBuffer[insertIndex] = (word)keyCode;
      }
    }
  }
  else if ((keyboardStateMask & 0xc) == 0) {
    if ((keyboardStateMask & 3) == 0) {
      if (keyCode == 0x10003) {
        codeUnitIndex = control->cursorIndex;
        if ((codeUnitIndex != control->selectionStart) || (codeUnitIndex != control->selectionEnd))
        goto UiNumericTextEdit_DeleteSelectedRange;
        if (control->cursorIndex == 0) goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
        sourceCursor = control->textBuffer + codeUnitIndex;
        destinationCursor = control->textBuffer + (codeUnitIndex - 1);
        for (countOrFieldOffset = 0x10 - codeUnitIndex; countOrFieldOffset != 0; countOrFieldOffset = countOrFieldOffset + -1) {
          *destinationCursor = *sourceCursor;
          sourceCursor = sourceCursor + 1;
          destinationCursor = destinationCursor + 1;
        }
        control->cursorIndex = control->cursorIndex - 1;
      }
      else {
        if (keyCode == 0x10006) {
          codeUnitIndex = control->cursorIndex;
          if ((codeUnitIndex == control->selectionStart) && (codeUnitIndex == control->selectionEnd)) {
            if (control->textBuffer[codeUnitIndex] == 0)
            goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
            sourceCursor = control->textBuffer + codeUnitIndex + 1;
            destinationCursor = control->textBuffer + codeUnitIndex;
            for (countOrFieldOffset = 0xf - codeUnitIndex; countOrFieldOffset != 0; countOrFieldOffset = countOrFieldOffset + -1) {
              *destinationCursor = *sourceCursor;
              sourceCursor = sourceCursor + 1;
              destinationCursor = destinationCursor + 1;
            }
            goto UiNumericTextEdit_RecomputeLayoutAfterEdit;
          }
UiNumericTextEdit_DeleteSelectedRange:
          codeUnitIndex = control->selectionEnd;
          countOrFieldOffset = codeUnitIndex - control->selectionStart;
          sourceCursor = control->textBuffer + codeUnitIndex;
          destinationCursor = control->textBuffer + control->selectionStart;
          for (shiftCountOrScanIndex = 0x10 - codeUnitIndex; shiftCountOrScanIndex != 0; shiftCountOrScanIndex = shiftCountOrScanIndex + -1) {
            *destinationCursor = *sourceCursor;
            sourceCursor = sourceCursor + 1;
            destinationCursor = destinationCursor + 1;
          }
          for (; countOrFieldOffset != 0; countOrFieldOffset = countOrFieldOffset + -1) {
            *destinationCursor = 0;
            destinationCursor = destinationCursor + 1;
          }
          control->cursorIndex = control->selectionStart;
          control->selectionEnd = control->selectionStart;
          goto UiNumericTextEdit_RecomputeLayoutAfterEdit;
        }
        if (keyCode == 0x10007) {
          control->editStateFlags = control->editStateFlags ^ UI_NUMERIC_TEXT_OVERWRITE_MODE;
          goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
        }
        if (keyCode == 0x10010) goto UiNumericTextEdit_MoveCursorToStartAndCollapseSelection;
        if (keyCode == 0x10018) goto UiNumericTextEdit_MoveCursorToEndAndCollapseSelection;
        if (keyCode == 0x10014) {
          if (control->cursorIndex != 0) {
            control->cursorIndex = control->cursorIndex - 1;
          }
        }
        else {
          if (keyCode != 0x10016) {
            if (keyCode == 0x10001) {
              if ((control->editStateFlags & UI_NUMERIC_TEXT_ACTION_ON_ENTER_ONLY) != 0) {
                UiActionQueue_Enqueue(control->actionId,control);
                if (((control->editStateFlags & UI_NUMERIC_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
                   (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
                  (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
                }
                return false;
              }
            }
            else if ((keyCode & 0x30000) == 0x30000) {
              return false;
            }
            goto UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
          }
          if (control->textBuffer[control->cursorIndex] != 0) {
            control->cursorIndex = control->cursorIndex + 1;
          }
        }
      }
UiNumericTextEdit_CollapseSelectionAtCursor:
      control->selectionStart = control->cursorIndex;
      control->selectionEnd = control->cursorIndex;
    }
    else {
      if (keyCode == 0x10010) {
UiNumericTextEdit_ExtendSelectionToStart:
        codeUnitIndex = control->cursorIndex;
        if (codeUnitIndex == 0) goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
        control->cursorIndex = 0;
        if (codeUnitIndex == control->selectionStart) {
          control->selectionStart = 0;
        }
        else {
          control->selectionEnd = 0;
        }
        goto UiNumericTextEdit_NormalizeSelectionOrder;
      }
      if (keyCode == 0x10018) goto UiNumericTextEdit_ExtendSelectionToEnd;
      if (keyCode == 0x10014) {
        codeUnitIndex = control->cursorIndex;
        if (codeUnitIndex == 0) goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
        control->cursorIndex = control->cursorIndex - 1;
        if (codeUnitIndex == control->selectionStart) {
          control->selectionStart = control->selectionStart - 1;
        }
        else {
          control->selectionEnd = control->selectionEnd - 1;
        }
      }
      else {
        if (keyCode != 0x10016) goto UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        codeUnitIndex = control->cursorIndex;
        if (control->textBuffer[codeUnitIndex] == 0) goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
        control->cursorIndex = control->cursorIndex + 1;
        if (codeUnitIndex == control->selectionEnd) {
          control->selectionEnd = control->selectionEnd + 1;
        }
        else {
          control->selectionStart = control->selectionStart + 1;
        }
      }
    }
  }
  else {
    if ((keyboardStateMask & 3) == 0) {
      if (keyCode == 0x10016) {
UiNumericTextEdit_MoveCursorToEndAndCollapseSelection:
        while (control->textBuffer[control->cursorIndex] != 0) {
          control->cursorIndex = control->cursorIndex + 1;
        }
      }
      else {
        if (keyCode != 0x10014) goto UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
UiNumericTextEdit_MoveCursorToStartAndCollapseSelection:
        control->cursorIndex = 0;
      }
      goto UiNumericTextEdit_CollapseSelectionAtCursor;
    }
    if (keyCode == 0x10014) goto UiNumericTextEdit_ExtendSelectionToStart;
    if (keyCode != 0x10016) goto UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
UiNumericTextEdit_ExtendSelectionToEnd:
    codeUnitIndex = control->cursorIndex;
    if (control->textBuffer[codeUnitIndex] == 0) goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
    control->cursorIndex = 0;
    countOrFieldOffset = 0x60;
    if (codeUnitIndex == control->selectionEnd) {
      countOrFieldOffset = 100;
    }
    do {
      selectionBoundary = (int *)((int)control->textBuffer + countOrFieldOffset + -0x6c);
      *selectionBoundary = *selectionBoundary + 1;
      control->cursorIndex = control->cursorIndex + 1;
      shiftCountOrScanIndex = codeUnitIndex + 1;
      codeUnitIndex = codeUnitIndex + 1;
    } while (control->textBuffer[shiftCountOrScanIndex] != 0);
UiNumericTextEdit_NormalizeSelectionOrder:
    if (control->selectionEnd < control->selectionStart) {
      LOCK();
      codeUnitIndex = control->selectionEnd;
      control->selectionEnd = control->selectionStart;
      UNLOCK();
      control->selectionStart = codeUnitIndex;
    }
  }
UiNumericTextEdit_RecomputeLayoutAfterEdit:
  UiTextEditControl_RecomputeLayoutAndClampScroll((UiTextEditControl *)control);
UiNumericTextEdit_ParseCommitInvalidateAndReturn:
  UiNumericTextControl_ParseAndCommitValue(control);
  UiNode_InvalidateRoot(&control->base);
  if (((control->editStateFlags & UI_NUMERIC_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
     (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
    (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
  }
  return false;
}


/* Address: 0x004B68C0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B6800[12]@004B6800.
   Local calls: UiTextEditControl_RecomputeLayoutAndClampScroll, UiPathTextControl_UpdateDos83Validity.
   Cross-module calls: UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input], UiActionQueue_Enqueue
   [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiPathTextEditControl_HandleKeyboardAndValidateCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiPathTextEditControl *control)

{
  int *selectionBoundary;
  ushort displacedCodeUnit;
  UiTextCodeUnitIndex segmentBoundaryIndex;
  UiTextCodeUnitIndex codeUnitIndex;
  int countOrFieldOffset;
  int shiftCountOrScanIndex;
  uint insertIndex;
  word *sourceCursor;
  word *destinationCursor;
  bool delegatedResult;
  
  if (((control->editStateFlags & UI_TEXT_EDIT_READ_ONLY) != 0) ||
     (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0))
  goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
  if ((((keyCode == 0x40) ||
       ((((keyCode == 0x7c || (keyCode == 0x7e)) || (keyCode == 0xb2)) ||
        ((keyCode == 0xb3 || (keyCode == 0x7b)))))) || (keyCode == 0x5b)) ||
     (((keyCode == 0x5d || (keyCode == 0x7d)) ||
      ((keyCode == 0x5c || ((keyCode == 0xb5 || (keyCode == 0x80)))))))) {
UiPathTextEdit_ValidateCharacterAndInsert:
    if (keyCode == 0x2a) {
UiPathTextEdit_CheckWildcardPermission:
      if ((control->editStateFlags & 2) == 0)
      goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
    }
    else if ((keyCode != 0x2d) && (keyCode != 0x2e)) {
      if (keyCode < 0x30) goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
      if (0x39 < keyCode) {
        if (keyCode == 0x3a) {
UiPathTextEdit_CheckColonOrBackslashRestriction:
          if ((control->editStateFlags & 4) != 0) {
UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent:
            delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNextCf
                               (keyboardStateMask,keyCode,&control->base);
            return delegatedResult;
          }
        }
        else {
          if (keyCode == 0x3f) goto UiPathTextEdit_CheckWildcardPermission;
          if (keyCode < 0x41) goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
          if (0x5a < keyCode) {
            if (keyCode == 0x5c) goto UiPathTextEdit_CheckColonOrBackslashRestriction;
            if ((keyCode < 0x61) || (0x7a < keyCode))
            goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
          }
        }
      }
    }
    insertIndex = control->cursorIndex;
    if ((insertIndex != control->selectionStart) || (insertIndex != control->selectionEnd)) {
      codeUnitIndex = control->selectionEnd;
      countOrFieldOffset = codeUnitIndex - control->selectionStart;
      sourceCursor = control->pathBuffer + codeUnitIndex;
      destinationCursor = control->pathBuffer + control->selectionStart;
      for (shiftCountOrScanIndex = 0x100 - codeUnitIndex; shiftCountOrScanIndex != 0; shiftCountOrScanIndex = shiftCountOrScanIndex + -1) {
        *destinationCursor = *sourceCursor;
        sourceCursor = sourceCursor + 1;
        destinationCursor = destinationCursor + 1;
      }
      for (; countOrFieldOffset != 0; countOrFieldOffset = countOrFieldOffset + -1) {
        *destinationCursor = 0;
        destinationCursor = destinationCursor + 1;
      }
      insertIndex = control->selectionStart;
      control->cursorIndex = insertIndex;
      control->selectionEnd = insertIndex;
    }
    if (insertIndex < 0xfe) {
      control->cursorIndex = control->cursorIndex + 1;
      control->selectionStart = control->selectionStart + 1;
      control->selectionEnd = control->selectionEnd + 1;
      if ((control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) == 0) {
        do {
          LOCK();
          displacedCodeUnit = control->pathBuffer[insertIndex];
          control->pathBuffer[insertIndex] = (ushort)keyCode;
          keyCode = (UiKeyboardEventCode)displacedCodeUnit;
          UNLOCK();
          insertIndex = insertIndex + 1;
        } while (insertIndex < 0xfe);
      }
      else {
        control->pathBuffer[insertIndex] = (word)keyCode;
      }
    }
  }
  else {
    if ((keyboardStateMask & 0x30) != 0)
    goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
    if ((keyCode & 0xffff0000) == 0) {
      if ((((keyboardStateMask & 0x3c) != 0) && (0x40 < keyCode)) &&
         ((keyCode < 0x5b || ((0x60 < keyCode && (keyCode < 0x7b))))))
      goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
      goto UiPathTextEdit_ValidateCharacterAndInsert;
    }
    if ((keyboardStateMask & 0xc) == 0) {
      if ((keyboardStateMask & 3) != 0) {
        if (keyCode == 0x10010) {
          codeUnitIndex = control->cursorIndex;
          if (codeUnitIndex == 0) goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
          control->cursorIndex = 0;
          if (codeUnitIndex == control->selectionStart) {
            control->selectionStart = 0;
          }
          else {
            control->selectionEnd = 0;
          }
        }
        else {
          if (keyCode != 0x10018) {
            if (keyCode == 0x10014) {
              codeUnitIndex = control->cursorIndex;
              if (codeUnitIndex == 0) goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
              control->cursorIndex = control->cursorIndex - 1;
              if (codeUnitIndex == control->selectionStart) {
                control->selectionStart = control->selectionStart - 1;
              }
              else {
                control->selectionEnd = control->selectionEnd - 1;
              }
            }
            else {
              if (keyCode != 0x10016) goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
              codeUnitIndex = control->cursorIndex;
              if (control->pathBuffer[codeUnitIndex] == 0)
              goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
              control->cursorIndex = control->cursorIndex + 1;
              if (codeUnitIndex == control->selectionEnd) {
                control->selectionEnd = control->selectionEnd + 1;
              }
              else {
                control->selectionStart = control->selectionStart + 1;
              }
            }
            goto UiPathTextEdit_RecomputeLayoutAfterEdit;
          }
          codeUnitIndex = control->cursorIndex;
          if (control->pathBuffer[codeUnitIndex] == 0)
          goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
          control->cursorIndex = 0;
          countOrFieldOffset = 0x60;
          if (codeUnitIndex == control->selectionEnd) {
            countOrFieldOffset = 100;
          }
          do {
            selectionBoundary = (int *)((int)control->pathBuffer + countOrFieldOffset + -0x6c);
            *selectionBoundary = *selectionBoundary + 1;
            control->cursorIndex = control->cursorIndex + 1;
            shiftCountOrScanIndex = codeUnitIndex + 1;
            codeUnitIndex = codeUnitIndex + 1;
          } while (control->pathBuffer[shiftCountOrScanIndex] != 0);
        }
        goto UiPathTextEdit_NormalizeSelectionOrder;
      }
      if (keyCode == 0x10003) {
        codeUnitIndex = control->cursorIndex;
        if ((codeUnitIndex != control->selectionStart) || (codeUnitIndex != control->selectionEnd))
        goto UiPathTextEdit_DeleteSelectedRange;
        if (control->cursorIndex == 0) goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
        sourceCursor = control->pathBuffer + codeUnitIndex;
        destinationCursor = control->pathBuffer + (codeUnitIndex - 1);
        for (countOrFieldOffset = 0x100 - codeUnitIndex; countOrFieldOffset != 0; countOrFieldOffset = countOrFieldOffset + -1) {
          *destinationCursor = *sourceCursor;
          sourceCursor = sourceCursor + 1;
          destinationCursor = destinationCursor + 1;
        }
        control->cursorIndex = control->cursorIndex - 1;
      }
      else {
        if (keyCode == 0x10006) {
          codeUnitIndex = control->cursorIndex;
          if ((codeUnitIndex == control->selectionStart) && (codeUnitIndex == control->selectionEnd)) {
            if (control->pathBuffer[codeUnitIndex] == 0)
            goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
            sourceCursor = control->pathBuffer + codeUnitIndex + 1;
            destinationCursor = control->pathBuffer + codeUnitIndex;
            for (countOrFieldOffset = 0xff - codeUnitIndex; countOrFieldOffset != 0; countOrFieldOffset = countOrFieldOffset + -1) {
              *destinationCursor = *sourceCursor;
              sourceCursor = sourceCursor + 1;
              destinationCursor = destinationCursor + 1;
            }
            goto UiPathTextEdit_RecomputeLayoutAfterEdit;
          }
UiPathTextEdit_DeleteSelectedRange:
          codeUnitIndex = control->selectionEnd;
          countOrFieldOffset = codeUnitIndex - control->selectionStart;
          sourceCursor = control->pathBuffer + codeUnitIndex;
          destinationCursor = control->pathBuffer + control->selectionStart;
          for (shiftCountOrScanIndex = 0x100 - codeUnitIndex; shiftCountOrScanIndex != 0; shiftCountOrScanIndex = shiftCountOrScanIndex + -1) {
            *destinationCursor = *sourceCursor;
            sourceCursor = sourceCursor + 1;
            destinationCursor = destinationCursor + 1;
          }
          for (; countOrFieldOffset != 0; countOrFieldOffset = countOrFieldOffset + -1) {
            *destinationCursor = 0;
            destinationCursor = destinationCursor + 1;
          }
          control->cursorIndex = control->selectionStart;
          control->selectionEnd = control->selectionStart;
          goto UiPathTextEdit_RecomputeLayoutAfterEdit;
        }
        if (keyCode == 0x10007) {
          control->editStateFlags = control->editStateFlags ^ UI_TEXT_EDIT_OVERWRITE_MODE;
          goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
        }
        if (keyCode == 0x10010) {
          control->cursorIndex = 0;
        }
        else if (keyCode == 0x10018) {
          while (control->pathBuffer[control->cursorIndex] != 0) {
            control->cursorIndex = control->cursorIndex + 1;
          }
        }
        else if (keyCode == 0x10014) {
          if (control->cursorIndex != 0) {
            control->cursorIndex = control->cursorIndex - 1;
          }
        }
        else {
          if (keyCode != 0x10016) {
            if (keyCode == 0x10001) {
              if ((control->editStateFlags & UI_TEXT_EDIT_ACTION_ON_ENTER_ONLY) != 0) {
                UiActionQueue_Enqueue(control->actionId,control);
                if (((control->editStateFlags & UI_TEXT_EDIT_PLAY_INTERACTION_SOUND) != 0) &&
                   (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
                  (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
                }
                return false;
              }
            }
            else if ((keyCode & 0x30000) == 0x30000) {
              return false;
            }
            goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
          }
          if (control->pathBuffer[control->cursorIndex] != 0) {
            control->cursorIndex = control->cursorIndex + 1;
          }
        }
      }
      control->selectionStart = control->cursorIndex;
      control->selectionEnd = control->cursorIndex;
    }
    else if ((keyboardStateMask & 3) == 0) {
      if (keyCode == 0x10014) {
        codeUnitIndex = control->cursorIndex;
        if (codeUnitIndex != 0) {
          segmentBoundaryIndex = 0;
          if (codeUnitIndex != 1) {
            for (countOrFieldOffset = codeUnitIndex - 2;
                ((segmentBoundaryIndex = 0, countOrFieldOffset != 0 && (segmentBoundaryIndex = countOrFieldOffset + 1, control->pathBuffer[countOrFieldOffset] != 0x5c))
                && (control->pathBuffer[countOrFieldOffset] != 0x2e)); countOrFieldOffset = countOrFieldOffset + -1) {
            }
          }
          control->cursorIndex = segmentBoundaryIndex;
          control->selectionStart = segmentBoundaryIndex;
          control->selectionEnd = segmentBoundaryIndex;
        }
      }
      else {
        if (keyCode != 0x10016) goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        codeUnitIndex = control->cursorIndex;
        do {
          segmentBoundaryIndex = codeUnitIndex;
          if ((control->pathBuffer[codeUnitIndex] == 0) ||
             (segmentBoundaryIndex = codeUnitIndex + 1, control->pathBuffer[codeUnitIndex] == 0x5c)) break;
          sourceCursor = control->pathBuffer + codeUnitIndex;
          codeUnitIndex = segmentBoundaryIndex;
        } while (*sourceCursor != 0x2e);
        control->cursorIndex = segmentBoundaryIndex;
        control->selectionStart = segmentBoundaryIndex;
        control->selectionEnd = segmentBoundaryIndex;
      }
    }
    else {
      if (keyCode == 0x10014) {
        codeUnitIndex = control->cursorIndex;
        countOrFieldOffset = 0x60;
        if (codeUnitIndex == 0) goto UiPathTextEdit_RecomputeLayoutAfterEdit;
        if (codeUnitIndex != control->selectionStart) {
          countOrFieldOffset = 100;
        }
        segmentBoundaryIndex = 0;
        if (codeUnitIndex != 1) {
          for (shiftCountOrScanIndex = codeUnitIndex - 2;
              ((segmentBoundaryIndex = 0, shiftCountOrScanIndex != 0 && (segmentBoundaryIndex = shiftCountOrScanIndex + 1, control->pathBuffer[shiftCountOrScanIndex] != 0x5c)) &&
              (control->pathBuffer[shiftCountOrScanIndex] != 0x2e)); shiftCountOrScanIndex = shiftCountOrScanIndex + -1) {
          }
        }
        control->cursorIndex = segmentBoundaryIndex;
        *(UiTextCodeUnitIndex *)((int)control->pathBuffer + countOrFieldOffset + -0x6c) = segmentBoundaryIndex;
      }
      else {
        if (keyCode != 0x10016) goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        codeUnitIndex = control->cursorIndex;
        countOrFieldOffset = 0x60;
        if (codeUnitIndex != control->selectionStart) {
          countOrFieldOffset = 100;
        }
        do {
          segmentBoundaryIndex = codeUnitIndex;
          if ((control->pathBuffer[codeUnitIndex] == 0) ||
             (segmentBoundaryIndex = codeUnitIndex + 1, control->pathBuffer[codeUnitIndex] == 0x5c)) break;
          sourceCursor = control->pathBuffer + codeUnitIndex;
          codeUnitIndex = segmentBoundaryIndex;
        } while (*sourceCursor != 0x2e);
        control->cursorIndex = segmentBoundaryIndex;
        *(UiTextCodeUnitIndex *)((int)control->pathBuffer + countOrFieldOffset + -0x6c) = segmentBoundaryIndex;
      }
UiPathTextEdit_NormalizeSelectionOrder:
      if (control->selectionEnd < control->selectionStart) {
        LOCK();
        codeUnitIndex = control->selectionEnd;
        control->selectionEnd = control->selectionStart;
        UNLOCK();
        control->selectionStart = codeUnitIndex;
      }
    }
  }
UiPathTextEdit_RecomputeLayoutAfterEdit:
  UiTextEditControl_RecomputeLayoutAndClampScroll((UiTextEditControl *)control);
UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn:
  UiPathTextControl_UpdateDos83Validity(control);
  if ((control->editStateFlags & UI_TEXT_EDIT_ACTION_ON_ENTER_ONLY) == 0) {
    UiActionQueue_Enqueue(control->actionId,control);
  }
  UiNode_InvalidateRoot(&control->base);
  if (((control->editStateFlags & UI_TEXT_EDIT_PLAY_INTERACTION_SOUND) != 0) &&
     (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
    (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
  }
  return false;
}


/* Address: 0x004B7110.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7050[12]@004B7050.
   Local calls: UiTextEditControl_RecomputeLayoutAndClampScroll, UiTextControl_UpdateNonEmptyValidity.
   Cross-module calls: UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input], UiActionQueue_Enqueue
   [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiRequiredTextEditControl_HandleKeyboardAndValidateCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiRequiredTextEditControl *control)

{
  int *selectionBoundary;
  ushort displacedCodeUnit;
  UiTextCodeUnitIndex codeUnitIndex;
  UiTextCodeUnitIndex wordBoundaryIndex;
  UiTextCodeUnitIndex scanIndex;
  UiTextCodeUnitCount remainingCodeUnits;
  int countOrFieldOffset;
  int shiftCountOrScanIndex;
  uint insertLimit;
  uint insertIndex;
  word *sourceCursor;
  word *destinationCursor;
  bool delegatedResult;
  
  insertIndex = control->cursorIndex;
  if (((control->editStateFlags & UI_REQUIRED_TEXT_READ_ONLY) != 0) ||
     (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
UiRequiredTextEdit_DelegateRejectedOrUnhandledKeyboardEvent:
    delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
    return delegatedResult;
  }
  if ((((keyCode == 0x40) ||
       ((((keyCode == 0x7c || (keyCode == 0x7e)) || (keyCode == 0xb2)) ||
        ((keyCode == 0xb3 || (keyCode == 0x7b)))))) || (keyCode == 0x5b)) ||
     (((keyCode == 0x5d || (keyCode == 0x7d)) ||
      ((keyCode == 0x5c || ((keyCode == 0xb5 || (keyCode == 0x80)))))))) {
UiRequiredTextEdit_InsertCharacterOrReplaceSelection:
    insertLimit = control->bufferCapacityCodeUnits - 1;
    if ((insertIndex != control->selectionStart) || (insertIndex != control->selectionEnd)) {
      codeUnitIndex = control->selectionEnd;
      countOrFieldOffset = codeUnitIndex - control->selectionStart;
      sourceCursor = control->textPrefix6C + codeUnitIndex;
      destinationCursor = control->textPrefix6C + control->selectionStart;
      for (shiftCountOrScanIndex = control->bufferCapacityCodeUnits - codeUnitIndex; shiftCountOrScanIndex != 0; shiftCountOrScanIndex = shiftCountOrScanIndex + -1) {
        *destinationCursor = *sourceCursor;
        sourceCursor = sourceCursor + 1;
        destinationCursor = destinationCursor + 1;
      }
      for (; countOrFieldOffset != 0; countOrFieldOffset = countOrFieldOffset + -1) {
        *destinationCursor = 0;
        destinationCursor = destinationCursor + 1;
      }
      insertIndex = control->selectionStart;
      control->cursorIndex = insertIndex;
      control->selectionEnd = insertIndex;
    }
    if (insertIndex < insertLimit) {
      control->cursorIndex = control->cursorIndex + 1;
      control->selectionStart = control->selectionStart + 1;
      control->selectionEnd = control->selectionEnd + 1;
      if ((control->editStateFlags & UI_REQUIRED_TEXT_OVERWRITE_MODE) == 0) {
        do {
          LOCK();
          displacedCodeUnit = control->textPrefix6C[insertIndex];
          control->textPrefix6C[insertIndex] = (ushort)keyCode;
          keyCode = (UiKeyboardEventCode)displacedCodeUnit;
          UNLOCK();
          insertIndex = insertIndex + 1;
        } while (insertIndex < insertLimit);
      }
      else {
        control->textPrefix6C[insertIndex] = (word)keyCode;
      }
    }
  }
  else {
    if ((keyboardStateMask & 0x30) != 0)
    goto UiRequiredTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
    if ((keyCode & 0xffff0000) == 0) {
      if ((((keyboardStateMask & 0x3c) != 0) && (0x40 < keyCode)) &&
         ((keyCode < 0x5b || ((0x60 < keyCode && (keyCode < 0x7b))))))
      goto UiRequiredTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
      goto UiRequiredTextEdit_InsertCharacterOrReplaceSelection;
    }
    if ((keyboardStateMask & 0xc) == 0) {
      if ((keyboardStateMask & 3) != 0) {
        if (keyCode == 0x10010) {
          codeUnitIndex = control->cursorIndex;
          if (codeUnitIndex == 0) goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
          control->cursorIndex = 0;
          if (codeUnitIndex == control->selectionStart) {
            control->selectionStart = 0;
          }
          else {
            control->selectionEnd = 0;
          }
        }
        else {
          if (keyCode != 0x10018) {
            if (keyCode == 0x10014) {
              codeUnitIndex = control->cursorIndex;
              if (codeUnitIndex == 0) goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
              control->cursorIndex = control->cursorIndex - 1;
              if (codeUnitIndex == control->selectionStart) {
                control->selectionStart = control->selectionStart - 1;
              }
              else {
                control->selectionEnd = control->selectionEnd - 1;
              }
            }
            else {
              if (keyCode != 0x10016)
              goto UiRequiredTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
              codeUnitIndex = control->cursorIndex;
              if (control->textPrefix6C[codeUnitIndex] == 0)
              goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
              control->cursorIndex = control->cursorIndex + 1;
              if (codeUnitIndex == control->selectionEnd) {
                control->selectionEnd = control->selectionEnd + 1;
              }
              else {
                control->selectionStart = control->selectionStart + 1;
              }
            }
            goto UiRequiredTextEdit_RecomputeLayoutAfterEdit;
          }
          codeUnitIndex = control->cursorIndex;
          if (control->textPrefix6C[codeUnitIndex] == 0)
          goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
          control->cursorIndex = 0;
          countOrFieldOffset = 0x60;
          if (codeUnitIndex == control->selectionEnd) {
            countOrFieldOffset = 100;
          }
          do {
            selectionBoundary = (int *)((int)control->textPrefix6C + countOrFieldOffset + -0x6c);
            *selectionBoundary = *selectionBoundary + 1;
            control->cursorIndex = control->cursorIndex + 1;
            shiftCountOrScanIndex = codeUnitIndex + 1;
            codeUnitIndex = codeUnitIndex + 1;
          } while (control->textPrefix6C[shiftCountOrScanIndex] != 0);
        }
        goto UiRequiredTextEdit_NormalizeSelectionOrder;
      }
      if (keyCode == 0x10003) {
        codeUnitIndex = control->cursorIndex;
        if ((codeUnitIndex != control->selectionStart) || (codeUnitIndex != control->selectionEnd))
        goto UiRequiredTextEdit_DeleteSelectedRange;
        if (control->cursorIndex == 0)
        goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
        sourceCursor = control->textPrefix6C + codeUnitIndex;
        destinationCursor = control->textPrefix6C + (codeUnitIndex - 1);
        for (countOrFieldOffset = control->bufferCapacityCodeUnits - codeUnitIndex; countOrFieldOffset != 0; countOrFieldOffset = countOrFieldOffset + -1) {
          *destinationCursor = *sourceCursor;
          sourceCursor = sourceCursor + 1;
          destinationCursor = destinationCursor + 1;
        }
        control->cursorIndex = control->cursorIndex - 1;
      }
      else {
        if (keyCode == 0x10006) {
          codeUnitIndex = control->cursorIndex;
          if ((codeUnitIndex == control->selectionStart) && (codeUnitIndex == control->selectionEnd)) {
            if (control->textPrefix6C[codeUnitIndex] == 0)
            goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
            countOrFieldOffset = control->bufferCapacityCodeUnits - codeUnitIndex;
            sourceCursor = control->textPrefix6C + codeUnitIndex + 1;
            destinationCursor = control->textPrefix6C + codeUnitIndex;
            while (countOrFieldOffset = countOrFieldOffset + -1, countOrFieldOffset != 0) {
              *destinationCursor = *sourceCursor;
              sourceCursor = sourceCursor + 1;
              destinationCursor = destinationCursor + 1;
            }
            goto UiRequiredTextEdit_RecomputeLayoutAfterEdit;
          }
UiRequiredTextEdit_DeleteSelectedRange:
          codeUnitIndex = control->selectionEnd;
          countOrFieldOffset = codeUnitIndex - control->selectionStart;
          sourceCursor = control->textPrefix6C + codeUnitIndex;
          destinationCursor = control->textPrefix6C + control->selectionStart;
          for (shiftCountOrScanIndex = control->bufferCapacityCodeUnits - codeUnitIndex; shiftCountOrScanIndex != 0; shiftCountOrScanIndex = shiftCountOrScanIndex + -1) {
            *destinationCursor = *sourceCursor;
            sourceCursor = sourceCursor + 1;
            destinationCursor = destinationCursor + 1;
          }
          for (; countOrFieldOffset != 0; countOrFieldOffset = countOrFieldOffset + -1) {
            *destinationCursor = 0;
            destinationCursor = destinationCursor + 1;
          }
          control->cursorIndex = control->selectionStart;
          control->selectionEnd = control->selectionStart;
          goto UiRequiredTextEdit_RecomputeLayoutAfterEdit;
        }
        if (keyCode == 0x10007) {
          control->editStateFlags = control->editStateFlags ^ UI_REQUIRED_TEXT_OVERWRITE_MODE;
          goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
        }
        if (keyCode == 0x10010) {
          control->cursorIndex = 0;
        }
        else if (keyCode == 0x10018) {
          while (control->textPrefix6C[control->cursorIndex] != 0) {
            control->cursorIndex = control->cursorIndex + 1;
          }
        }
        else if (keyCode == 0x10014) {
          if (control->cursorIndex != 0) {
            control->cursorIndex = control->cursorIndex - 1;
          }
        }
        else {
          if (keyCode != 0x10016) {
            if (keyCode == 0x10001) {
              if ((control->editStateFlags & UI_REQUIRED_TEXT_ACTION_ON_ENTER_ONLY) != 0) {
                UiActionQueue_Enqueue(control->actionId,control);
                if (((control->editStateFlags & UI_REQUIRED_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
                   (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
                  (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
                }
                return false;
              }
            }
            else if (keyCode == 0x10000) {
              if ((control->editStateFlags & UI_REQUIRED_TEXT_ESCAPE_CLEARS_AND_QUEUES_ACTION) != 0)
              {
                sourceCursor = control->textPrefix6C;
                for (remainingCodeUnits = control->bufferCapacityCodeUnits; remainingCodeUnits != 0; remainingCodeUnits = remainingCodeUnits - 1) {
                  *sourceCursor = 0;
                  sourceCursor = sourceCursor + 1;
                }
                control->cursorIndex = 0;
                control->selectionStart = 0;
                control->selectionEnd = 0;
                UiActionQueue_Enqueue(control->actionId,control);
                if (((control->editStateFlags & UI_REQUIRED_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
                   (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
                  (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
                }
                return false;
              }
            }
            else if ((keyCode & 0x30000) == 0x30000) {
              return false;
            }
            goto UiRequiredTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
          }
          if (control->textPrefix6C[control->cursorIndex] != 0) {
            control->cursorIndex = control->cursorIndex + 1;
          }
        }
      }
      control->selectionStart = control->cursorIndex;
      control->selectionEnd = control->cursorIndex;
    }
    else if ((keyboardStateMask & 3) == 0) {
      if (keyCode == 0x10014) {
        codeUnitIndex = control->cursorIndex;
        if (codeUnitIndex != 0) {
          if (control->textPrefix6C[codeUnitIndex - 1] == 0x20) {
            do {
              wordBoundaryIndex = codeUnitIndex - 1;
              if (wordBoundaryIndex == 0) break;
              countOrFieldOffset = codeUnitIndex - 2;
              codeUnitIndex = wordBoundaryIndex;
            } while (control->textPrefix6C[countOrFieldOffset] == 0x20);
          }
          else {
            do {
              scanIndex = codeUnitIndex;
              wordBoundaryIndex = scanIndex - 1;
              if (wordBoundaryIndex == 0) goto UiRequiredTextEdit_MoveCursorToPreviousWordBoundary;
              codeUnitIndex = wordBoundaryIndex;
            } while (control->textPrefix6C[scanIndex - 2] != 0x20);
            if ((1 < (int)wordBoundaryIndex) && (control->textPrefix6C[scanIndex - 3] != 0x20)) {
              wordBoundaryIndex = scanIndex - 2;
            }
          }
UiRequiredTextEdit_MoveCursorToPreviousWordBoundary:
          control->cursorIndex = wordBoundaryIndex;
          control->selectionStart = wordBoundaryIndex;
          control->selectionEnd = wordBoundaryIndex;
        }
      }
      else {
        if (keyCode != 0x10016) goto UiRequiredTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        codeUnitIndex = control->cursorIndex;
        if (control->textPrefix6C[codeUnitIndex] == 0x20) {
          for (; (control->textPrefix6C[codeUnitIndex] != 0 && (control->textPrefix6C[codeUnitIndex] == 0x20));
              codeUnitIndex = codeUnitIndex + 1) {
          }
        }
        else {
          do {
            wordBoundaryIndex = codeUnitIndex;
            codeUnitIndex = wordBoundaryIndex;
            if (control->textPrefix6C[wordBoundaryIndex] == 0)
            goto UiRequiredTextEdit_MoveCursorToNextWordBoundary;
            codeUnitIndex = wordBoundaryIndex + 1;
          } while (control->textPrefix6C[wordBoundaryIndex] != 0x20);
          if (control->textPrefix6C[wordBoundaryIndex + 1] == 0x20) {
            codeUnitIndex = wordBoundaryIndex;
          }
        }
UiRequiredTextEdit_MoveCursorToNextWordBoundary:
        control->cursorIndex = codeUnitIndex;
        control->selectionStart = codeUnitIndex;
        control->selectionEnd = codeUnitIndex;
      }
    }
    else {
      if (keyCode == 0x10014) {
        codeUnitIndex = control->cursorIndex;
        countOrFieldOffset = 0x60;
        if (codeUnitIndex == 0) goto UiRequiredTextEdit_RecomputeLayoutAfterEdit;
        if (codeUnitIndex != control->selectionStart) {
          countOrFieldOffset = 100;
        }
        if (control->textPrefix6C[codeUnitIndex - 1] == 0x20) {
          do {
            wordBoundaryIndex = codeUnitIndex - 1;
            if (wordBoundaryIndex == 0) break;
            shiftCountOrScanIndex = codeUnitIndex - 2;
            codeUnitIndex = wordBoundaryIndex;
          } while (control->textPrefix6C[shiftCountOrScanIndex] == 0x20);
        }
        else {
          do {
            scanIndex = codeUnitIndex;
            wordBoundaryIndex = scanIndex - 1;
            if (wordBoundaryIndex == 0) goto UiRequiredTextEdit_ExtendSelectionToPreviousWordBoundary;
            codeUnitIndex = wordBoundaryIndex;
          } while (control->textPrefix6C[scanIndex - 2] != 0x20);
          if ((1 < (int)wordBoundaryIndex) && (control->textPrefix6C[scanIndex - 3] != 0x20)) {
            wordBoundaryIndex = scanIndex - 2;
          }
        }
UiRequiredTextEdit_ExtendSelectionToPreviousWordBoundary:
        control->cursorIndex = wordBoundaryIndex;
        *(UiTextCodeUnitIndex *)((int)control->textPrefix6C + countOrFieldOffset + -0x6c) = wordBoundaryIndex;
      }
      else {
        if (keyCode != 0x10016) goto UiRequiredTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        codeUnitIndex = control->cursorIndex;
        countOrFieldOffset = 0x60;
        if ((codeUnitIndex == control->selectionStart) ||
           (countOrFieldOffset = 100, control->textPrefix6C[codeUnitIndex] != 0x20)) {
          do {
            wordBoundaryIndex = codeUnitIndex;
            codeUnitIndex = wordBoundaryIndex;
            if (control->textPrefix6C[wordBoundaryIndex] == 0)
            goto UiRequiredTextEdit_ExtendSelectionToNextWordBoundary;
            codeUnitIndex = wordBoundaryIndex + 1;
          } while (control->textPrefix6C[wordBoundaryIndex] != 0x20);
          if (control->textPrefix6C[wordBoundaryIndex + 1] == 0x20) {
            codeUnitIndex = wordBoundaryIndex;
          }
        }
        else {
          for (; (control->textPrefix6C[codeUnitIndex] != 0 && (control->textPrefix6C[codeUnitIndex] == 0x20));
              codeUnitIndex = codeUnitIndex + 1) {
          }
        }
UiRequiredTextEdit_ExtendSelectionToNextWordBoundary:
        control->cursorIndex = codeUnitIndex;
        *(UiTextCodeUnitIndex *)((int)control->textPrefix6C + countOrFieldOffset + -0x6c) = codeUnitIndex;
      }
UiRequiredTextEdit_NormalizeSelectionOrder:
      if (control->selectionEnd < control->selectionStart) {
        LOCK();
        codeUnitIndex = control->selectionEnd;
        control->selectionEnd = control->selectionStart;
        UNLOCK();
        control->selectionStart = codeUnitIndex;
      }
    }
  }
UiRequiredTextEdit_RecomputeLayoutAfterEdit:
  UiTextEditControl_RecomputeLayoutAndClampScroll((UiTextEditControl *)control);
UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn:
  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)control);
  if ((control->editStateFlags & UI_REQUIRED_TEXT_ACTION_ON_ENTER_ONLY) == 0) {
    UiActionQueue_Enqueue(control->actionId,control);
  }
  UiNode_InvalidateRoot(&control->base);
  if (((control->editStateFlags & UI_REQUIRED_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
     (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
    (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
  }
  return false;
}


/* Address: 0x004227B0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00422720[2]@00422720.
   Local calls: UiTextButtonControl_DrawClipped.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiGraphicsAdapterTextButton_DrawFormattedAdapterText
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control)

{
  UiPackedTextStyle adapterIndex;
  GraphicsAdapterRecord *adapterRecords;
  word *stream;
  word *replacementPayload;
  TextResourceResolveEaxCf5 resolvedText;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve(control->textResourceId);
    adapterRecords = g_GraphicsAdapters;
    stream = resolvedText.eax;
    if (((control->selectable).stateFlags & 0x80) != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].packedTextStyle,
                 (word *)&g_GraphicsAdapterFormatScratch0Utf16);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_GraphicsAdapterFormatScratch0Utf16,stream);
      UiTextButtonControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,control);
      return;
    }
    if (((control->selectable).stateFlags & 0x800) == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].packedTextStyle,
                 (word *)&g_GraphicsAdapterFormatScratch0Utf16);
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].textResourceId,
                 (word *)&g_GraphicsAdapterFormatScratch1Utf16);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_GraphicsAdapterFormatScratch0Utf16,stream);
      RichTextCommandStream_PatchPayloadBySelector(1,&g_GraphicsAdapterFormatScratch1Utf16,stream);
      UiTextButtonControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,control);
      return;
    }
    adapterIndex = control[-1].packedTextStyle;
    RichTextCommandStream_PatchPayloadBySelector
              (0,g_GraphicsAdapters[adapterIndex].driverDescriptionUtf16,stream);
    if (adapterRecords[adapterIndex].deviceGuid.Data1 == 0) {
      resolvedText = TextResource_Resolve(0x111);
      replacementPayload = resolvedText.eax;
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
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[0]@004B58A0.
   Local calls: UiNumericTextControl_RebuildTextFromValue.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiNumericTextEditControl_RelocateAndRebuildText
          (UiSerializedRelocationDelta relocationDelta,UiNumericTextControl *control)

{
  word *textCursor;
  word currentCodeUnit;
  UiNodeFlags *nodeFlagsField;
  
  if (((control->base).nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) == 0) {
    nodeFlagsField = &(control->base).nodeFlags;
    *nodeFlagsField = *nodeFlagsField | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  control->editStateFlags = control->editStateFlags & 0xffffef;
  UiNumericTextControl_RebuildTextFromValue(control);
  textCursor = control->textBuffer;
  control->cursorIndex = 0;
  control->selectionStart = 0;
  control->selectionEnd = 0;
  currentCodeUnit = *textCursor;
  while (currentCodeUnit != 0) {
    textCursor = textCursor + 1;
    control->selectionEnd = control->selectionEnd + 1;
    currentCodeUnit = *textCursor;
  }
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}


/* Address: 0x004B5960.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[2]@004B58A0; g_UiNodeVtable_004B6800[2]@004B6800;
   g_UiNodeVtable_004B7050[2]@004B7050.
   Local calls: UiTextEditControl_MeasurePrefixWidth.
   Cross-module calls: UiWindow_BlitTiledInterior [ui/controls/layout], UiWindow_BlitTiledHorizontalEdge
   [ui/controls/layout], UiWindow_BlitTiledVerticalEdge [ui/controls/layout], FontGlyph_GetLogicalSizeActiveRegs
   [assets/text/resources], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTextEditControl_DrawTextSelectionAndCaret
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextEditControl *control)

{
  UiTextCodeUnitCount prefixLength;
  UiPixelCoordinate startOrCaretWidth;
  UiPixelCoordinate selectionEndWidth;
  int styleVerticalOffset;
  int leftOrTextOffsetX;
  dword tileBottom;
  int topOrTextOffsetY;
  int bottomOrCaretY;
  UiPackedTextStyle packedStyle;
  int rightOrCaretX;
  bool framebufferUnavailable;
  GraphicsTextureSizeEaxEdxCf9 cornerTileSize;
  FontGlyphSizeEaxEdxCf9 fontSize;
  dword borderWidthOrCaretFrame;
  GraphicsTextureSourceAsset *caretTextureSource;
  SoftwareFramebufferAccess *caretFramebuffer;
  
  framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
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
      cornerTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x6d,g_UiWindowTextureSource);
      tileBottom = cornerTileSize.logicalHeightPixels;
      borderWidthOrCaretFrame = cornerTileSize.logicalWidthPixels;
      rightOrCaretX = rightOrCaretX - borderWidthOrCaretFrame;
      bottomOrCaretY = bottomOrCaretY - tileBottom;
      if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_TILED_INTERIOR) != 0) {
        UiWindow_BlitTiledInterior
                  (clipTop,clipLeft,clipBottom,clipRight,0x7a,bottomOrCaretY,rightOrCaretX,tileBottom,borderWidthOrCaretFrame,control)
        ;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,(control->base).left,0x6a
                 ,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,
                 rightOrCaretX + (control->base).left,0x6b,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,bottomOrCaretY + (control->base).top,
                 (control->base).left,0x6c,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,bottomOrCaretY + (control->base).top,
                 rightOrCaretX + (control->base).left,0x6d,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x6e,rightOrCaretX,0,borderWidthOrCaretFrame,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x6f,bottomOrCaretY,tileBottom,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x70,bottomOrCaretY,tileBottom,rightOrCaretX,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x71,rightOrCaretX,bottomOrCaretY,borderWidthOrCaretFrame,control);
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
                (clipTop,clipLeft,clipBottom,clipRight,0x8b,rightOrCaretX,topOrTextOffsetY + -1,startOrCaretWidth + leftOrTextOffsetX,control);
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
                (clipTop,clipLeft,clipBottom,clipRight,packedStyle,control->textPrefix6C,
                 topOrTextOffsetY + (control->base).top,leftOrTextOffsetX + (control->base).left);
    }
    else {
      startOrCaretWidth = UiTextEditControl_MeasurePrefixWidth(control->cursorIndex,control);
      rightOrCaretX = startOrCaretWidth + -2 + leftOrTextOffsetX + (control->base).left;
      borderWidthOrCaretFrame = 0x89;
      if ((control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) != 0) {
        borderWidthOrCaretFrame = 0x8a;
      }
      styleVerticalOffset = (int)(packedStyle << 0x10) >> 0x18;
      bottomOrCaretY = topOrTextOffsetY + -1 + (control->base).top + styleVerticalOffset;
      if ((control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) != 0) {
        bottomOrCaretY = bottomOrCaretY + 1;
      }
      caretTextureSource = g_UiWindowTextureSource;
      caretFramebuffer = g_FramebufferAccess;
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,bottomOrCaretY,rightOrCaretX + styleVerticalOffset,0x7f000000,0x89,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,packedStyle,control->textPrefix6C,
                 topOrTextOffsetY + (control->base).top,leftOrTextOffsetX + (control->base).left);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,topOrTextOffsetY + (control->base).top + -1,rightOrCaretX,borderWidthOrCaretFrame
                 ,caretTextureSource,caretFramebuffer);
    }
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}


/* Address: 0x004B5DF0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[4]@004B58A0; g_UiNodeVtable_004B6800[4]@004B6800;
   g_UiNodeVtable_004B7050[4]@004B7050.
   Local calls: UiTextEditControl_FindCursorIndexAtX.
*/
void __thandor_preserve_eax
UiTextEditControl_BeginSelectionAtPointer
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
     (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
    (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
  }
  return;
}


/* Address: 0x004B5EA0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[8]@004B58A0; g_UiNodeVtable_004B6800[8]@004B6800;
   g_UiNodeVtable_004B7050[8]@004B7050.
   Local calls: UiTextEditControl_FindCursorIndexAtX.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax_edx
UiTextEditControl_UpdateSelectionFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control)

{
  UiTextCodeUnitIndex previousCursorOrSelectionStart;
  UiNodeVtable *nodeVtable;
  uint movedBoundaryIndex;
  
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
    (*nodeVtable->layout)(&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}


/* Address: 0x004B6850.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B6800[0]@004B6800.
   Local calls: UiPathTextControl_UpdateDos83Validity.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiPathTextEditControl_RelocateAndValidateDos83
          (UiSerializedRelocationDelta relocationDelta,UiPathTextEditControl *control)

{
  word *textCursor;
  UiNodeFlags *nodeFlagsField;
  word currentCodeUnit;
  
  if (((control->base).nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) == 0) {
    nodeFlagsField = &(control->base).nodeFlags;
    *nodeFlagsField = *nodeFlagsField | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  control->editStateFlags = control->editStateFlags & 0xffffef;
  UiPathTextControl_UpdateDos83Validity(control);
  textCursor = control->pathBuffer;
  control->cursorIndex = 0;
  control->selectionStart = 0;
  control->selectionEnd = 0;
  currentCodeUnit = *textCursor;
  while (currentCodeUnit != 0) {
    textCursor = textCursor + 1;
    control->selectionEnd = control->selectionEnd + 1;
    currentCodeUnit = *textCursor;
  }
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}


/* Address: 0x004B70A0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7050[0]@004B7050.
   Local calls: UiTextControl_UpdateNonEmptyValidity.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiRequiredTextEditControl_RelocateAndValidateNonEmpty
          (UiSerializedRelocationDelta relocationDelta,UiRequiredTextEditControl *control)

{
  word *textCursor;
  UiNodeFlags *nodeFlagsField;
  word currentCodeUnit;
  
  if (((control->base).nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) == 0) {
    nodeFlagsField = &(control->base).nodeFlags;
    *nodeFlagsField = *nodeFlagsField | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  control->editStateFlags = control->editStateFlags & 0xffffef;
  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)control);
  textCursor = control->textPrefix6C;
  control->cursorIndex = 0;
  control->selectionStart = 0;
  control->selectionEnd = 0;
  currentCodeUnit = *textCursor;
  while (currentCodeUnit != 0) {
    textCursor = textCursor + 1;
    control->selectionEnd = control->selectionEnd + 1;
    currentCodeUnit = *textCursor;
  }
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}


/* Address: 0x004BB5C0.
   Ownership: ui/controls/text.
   Purpose: Bubble-sorts the pointer list in ascending expanded-text order, restores selected-record identity, and
   invalidates its row. Typed parameters: p0 fieldOffset→UiPointerListFieldByteOffset_V342. Calling convention,
   exact VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Local calls: UiPointerList_CompareExpandedTextFlags.
   Cross-module calls: UiScrollableControl_ClampOffsetsToViewport [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SortByExpandedTextFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *swappedRecord;
  int comparisonsOrRowTop;
  int remainingPasses;
  UiListRowCount remainingRows;
  void **passAnchorSlot;
  void **rowSlotCursor;
  CompareFlagsCfZf2 compareFlags;
  void *selectedRecord;
  
  rowSlotCursor = control->rowSlots;
  if (rowSlotCursor != (void **)0x0) {
    comparisonsOrRowTop = control->rowCount - 1;
    if ((comparisonsOrRowTop != 0) && (-1 < comparisonsOrRowTop)) {
      selectedRecord = *control->selectedRowSlot;
      passAnchorSlot = rowSlotCursor;
      remainingPasses = comparisonsOrRowTop;
      do {
        do {
          rowSlotCursor = rowSlotCursor + 1;
          compareFlags = UiPointerList_CompareExpandedTextFlags
                            ((word *)((int)*rowSlotCursor + fieldOffset),
                             (word *)((int)*passAnchorSlot + fieldOffset));
          if (!compareFlags.carry) {
            LOCK();
            swappedRecord = *rowSlotCursor;
            *rowSlotCursor = *passAnchorSlot;
            UNLOCK();
            *passAnchorSlot = swappedRecord;
          }
          comparisonsOrRowTop = comparisonsOrRowTop + -1;
        } while (comparisonsOrRowTop != 0);
        rowSlotCursor = passAnchorSlot + 1;
        comparisonsOrRowTop = remainingPasses + -1;
        passAnchorSlot = rowSlotCursor;
        remainingPasses = comparisonsOrRowTop;
      } while (comparisonsOrRowTop != 0);
      rowSlotCursor = control->rowSlots;
      remainingRows = control->rowCount;
      comparisonsOrRowTop = 0;
      do {
        if (selectedRecord == *rowSlotCursor)
        goto 
        UiPointerList_SortByExpandedTextFieldAscending_CommitResolvedSelectedRowSlotAndClampViewport
        ;
        comparisonsOrRowTop = comparisonsOrRowTop + control->rowHeight;
        rowSlotCursor = rowSlotCursor + 1;
        remainingRows = remainingRows - 1;
      } while (remainingRows != 0);
      rowSlotCursor = control->rowSlots;
UiPointerList_SortByExpandedTextFieldAscending_CommitResolvedSelectedRowSlotAndClampViewport:
      control->selectedRowSlot = rowSlotCursor;
      UiScrollableControl_ClampOffsetsToViewport
                (comparisonsOrRowTop + 1 + control->rowHeight,(control->base).rightOffset,comparisonsOrRowTop,0,
                 (UiScrollableControl *)(control->base).parent);
    }
  }
  return;
}


/* Address: 0x004BB6B0.
   Ownership: ui/controls/text.
   Purpose: Bubble-sorts the pointer list in descending expanded-text order, restores selected-record identity, and
   invalidates its row. Typed parameters: p0 fieldOffset→UiPointerListFieldByteOffset_V342. Calling convention,
   exact VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Local calls: UiPointerList_CompareExpandedTextFlags.
   Cross-module calls: UiScrollableControl_ClampOffsetsToViewport [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SortByExpandedTextFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *swappedRecord;
  int comparisonsOrRowTop;
  int remainingPasses;
  UiListRowCount remainingRows;
  void **passAnchorSlot;
  void **rowSlotCursor;
  CompareFlagsCfZf2 compareFlags;
  void *selectedRecord;
  
  rowSlotCursor = control->rowSlots;
  if (rowSlotCursor != (void **)0x0) {
    comparisonsOrRowTop = control->rowCount - 1;
    if ((comparisonsOrRowTop != 0) && (-1 < comparisonsOrRowTop)) {
      selectedRecord = *control->selectedRowSlot;
      passAnchorSlot = rowSlotCursor;
      remainingPasses = comparisonsOrRowTop;
      do {
        do {
          rowSlotCursor = rowSlotCursor + 1;
          compareFlags = UiPointerList_CompareExpandedTextFlags
                            ((word *)((int)*rowSlotCursor + fieldOffset),
                             (word *)((int)*passAnchorSlot + fieldOffset));
          if (compareFlags.carry || compareFlags.zero) {
            LOCK();
            swappedRecord = *rowSlotCursor;
            *rowSlotCursor = *passAnchorSlot;
            UNLOCK();
            *passAnchorSlot = swappedRecord;
          }
          comparisonsOrRowTop = comparisonsOrRowTop + -1;
        } while (comparisonsOrRowTop != 0);
        rowSlotCursor = passAnchorSlot + 1;
        comparisonsOrRowTop = remainingPasses + -1;
        passAnchorSlot = rowSlotCursor;
        remainingPasses = comparisonsOrRowTop;
      } while (comparisonsOrRowTop != 0);
      rowSlotCursor = control->rowSlots;
      remainingRows = control->rowCount;
      comparisonsOrRowTop = 0;
      do {
        if (selectedRecord == *rowSlotCursor)
        goto 
        UiPointerList_SortByExpandedTextFieldDescending_CommitResolvedSelectedRowSlotAndClampViewport
        ;
        comparisonsOrRowTop = comparisonsOrRowTop + control->rowHeight;
        rowSlotCursor = rowSlotCursor + 1;
        remainingRows = remainingRows - 1;
      } while (remainingRows != 0);
      rowSlotCursor = control->rowSlots;
UiPointerList_SortByExpandedTextFieldDescending_CommitResolvedSelectedRowSlotAndClampViewport:
      control->selectedRowSlot = rowSlotCursor;
      UiScrollableControl_ClampOffsetsToViewport
                (comparisonsOrRowTop + 1 + control->rowHeight,(control->base).rightOffset,comparisonsOrRowTop,0,
                 (UiScrollableControl *)(control->base).parent);
    }
  }
  return;
}


/* Address: 0x005156A0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00515610[2]@00515610.
   Local calls: UiTextButtonControl_DrawClipped.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiNumericPairTextButton_DrawFormattedValues
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control)

{
  TextResourceResolveEaxCf5 resolvedText;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve(control->textResourceId);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)control[1].selectable.base.nextSibling,
               (word *)&g_UiNumericPairFirstValueScratchUtf16);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)control[1].selectable.base.firstChild,
               (word *)&g_UiNumericPairSecondValueScratchUtf16);
    RichTextCommandStream_PatchPayloadBySelector(0,&g_UiNumericPairFirstValueScratchUtf16,resolvedText.eax)
    ;
    RichTextCommandStream_PatchPayloadBySelector
              (1,&g_UiNumericPairSecondValueScratchUtf16,resolvedText.eax);
    UiTextButtonControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,control);
  }
  return;
}


/* Address: 0x00515780.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00515730[2]@00515730.
   Local calls: UiTextButtonControl_DrawClipped.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiPayloadPairTextButton_DrawFormattedPayloads
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control)

{
  TextResourceResolveEaxCf5 resolvedText;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve(control->textResourceId);
    RichTextCommandStream_PatchPayloadBySelector(0,control[1].selectable.base.nextSibling,resolvedText.eax)
    ;
    RichTextCommandStream_PatchPayloadBySelector(1,control[1].selectable.base.firstChild,resolvedText.eax);
    UiTextButtonControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,control);
  }
  return;
}


/* Address: 0x004B0320.
   Ownership: ui/controls/text.
   Purpose: When the countdown has expired, resolves the target text, positions a three-part tooltip frame within
   the target root bounds, draws the text, and clears the framebuffer when no root exists. Typed parameters: p0
   clipBottom→UiPixelCoordinate_V297, p1 clipRight→UiPixelCoordinate_V297, p2 clipTop→UiPixelCoordinate_V297, p3
   clipLeft→UiPixelCoordinate_V297. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Cross-module calls: UiNode_GetRoot [ui/core/runtime], TextResource_Resolve [assets/text/resources],
   RichTextCommandStream_MeasureRegs [assets/text/richtext], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTooltip_Draw(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
              UiPixelCoordinate clipLeft)

{
  int targetRight;
  UiNodeBase *tooltipTarget;
  UiNodeBase *rootNode;
  word *commandStream;
  dword edgeTileWidth;
  sdword frameLeft;
  sdword frameRight;
  int frameWidthOrMiddleEnd;
  int targetLeftOrTileX;
  int frameTop;
  bool framebufferUnavailable;
  RichTextExtentRegs textExtent;
  TextResourceResolveEaxCf5 resolvedText;
  GraphicsTextureSizeEaxEdxCf9 tileSize;
  
  tooltipTarget = g_UiTooltipState.targetNode;
  if ((g_UiTooltipState.targetNode != (UiNodeBase *)0x0) && (g_UiTooltipState.countdownFrames == 0))
  {
    rootNode = UiNode_GetRoot(g_UiTooltipState.targetNode);
    commandStream = (word *)tooltipTarget[-1].nodeFlags;
    if ((tooltipTarget->nodeFlags & UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)commandStream);
      commandStream = resolvedText.eax;
    }
    textExtent = RichTextCommandStream_MeasureRegs(g_UiTooltipTextStyle,commandStream);
    tileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xbc,g_UiWindowTextureSource);
    edgeTileWidth = tileSize.logicalWidthPixels;
    frameWidthOrMiddleEnd = textExtent.widthPixels + edgeTileWidth * 2;
    targetLeftOrTileX = tooltipTarget->left;
    frameTop = tooltipTarget->top - tileSize.logicalHeightPixels;
    targetRight = tooltipTarget->right;
    framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
    if (!framebufferUnavailable) {
      frameLeft = (targetLeftOrTileX + targetRight) - frameWidthOrMiddleEnd >> 1;
      if (rootNode == (UiNodeBase *)0xffffffff) {
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
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipBottom,clipRight,clipTop,clipLeft,frameTop,frameLeft,0xbc,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      frameWidthOrMiddleEnd = frameRight - edgeTileWidth;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipBottom,clipRight,clipTop,clipLeft,frameTop,frameWidthOrMiddleEnd,0xbe,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      if (clipRight < frameWidthOrMiddleEnd) {
        frameWidthOrMiddleEnd = clipRight;
      }
      tileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xbd,g_UiWindowTextureSource);
      targetLeftOrTileX = edgeTileWidth + frameLeft;
      do {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipBottom,frameWidthOrMiddleEnd,clipTop,clipLeft,frameTop,targetLeftOrTileX,0xbd,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        targetLeftOrTileX = targetLeftOrTileX + tileSize.logicalWidthPixels;
      } while (targetLeftOrTileX < frameWidthOrMiddleEnd);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiTooltipTextStyle,commandStream,frameTop + 3,
                 edgeTileWidth + frameLeft);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  if (g_UiRootNode == (UiRootNode *)0xffffffff) {
    framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
    if (!framebufferUnavailable) {
      (*g_GraphicsFramebufferFillRectArgb)
                (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0
                 ,0,0x80000000,g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  return;
}


/* Address: 0x004B0F90.
   Ownership: ui/controls/text.
   Purpose: Pops UI roots until the current root equals the protected global boundary pointer at 004B0E34 or the
   stack sentinel. CF set from a pop stops the loop; EAX is preserved.
   Cross-module calls: UiRootStack_PopCf [ui/controls/layout].
*/
bool __thandor_cf_preserve_eax UiRootStack_PopUntilWindowTextureBoundaryCf(void)

{
  bool rootWasValid;
  bool popStopped;
  
  while (((GraphicsTextureSourceAsset *)g_UiRootNode != g_UiWindowTextureSource &&
         (g_UiRootNode != (UiRootNode *)0xffffffff))) {
    popStopped = UiRootStack_PopCf(g_UiRootNode);
    if (popStopped) {
      return true;
    }
  }
  return false;
}


/* Address: 0x004B1DD0.
   Ownership: ui/controls/text.
   Purpose: Expands the serialized layout rectangle by the configured frame inset when state flag 0x04 is set, then
   relocates child pointers.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiFramedTextButtonControl_Relocate
          (UiSerializedRelocationDelta relocationDelta,UiFramedTextButtonControl *control)

{
  sdword *bottomOffsetField;
  sdword *frameEdgeOffsetField;
  sdword *edgeOffsetField;
  sdword *trailingEdgeOffsetField;
  int frameInset;
  
  frameInset = g_UiWindowFrameInset;
  if (((control->selectable).stateFlags & 4) != 0) {
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
   Ownership: ui/controls/text.
   Purpose: Handles ui framed text button control draw clipped.
   Cross-module calls: UiWindow_BlitTiledHorizontalEdge [ui/controls/layout], UiWindow_BlitTiledVerticalEdge
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], RichTextCommandStream_DrawSingleLine [assets/text/richtext],
   UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiFramedTextButtonControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiFramedTextButtonControl *control)

{
  dword tileEnd;
  word *commandStream;
  dword frameTileOrTextWidth;
  int focusTileXOrTop;
  dword tileStart;
  int bottomEdgeOrTextX;
  uint textStyle;
  int rightEdgeOrTextY;
  bool framebufferUnavailable;
  RichTextExtentRegs textExtent;
  TextResourceResolveEaxCf5 resolvedText;
  GraphicsTextureSizeEaxEdxCf9 tileSizeOrEndCapSize;
  GraphicsTextureSizeEaxEdxCf9 focusTileSize;
  int baselineY;
  int drawX;
  
  framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
  if (framebufferUnavailable) goto UiFramedTextButtonControl_DrawClipped_DrawChildrenIfEnabledAndReturn;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & 4) == 0) {
      frameTileOrTextWidth = 0x4a;
    }
    else {
      frameTileOrTextWidth = 0x94;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      frameTileOrTextWidth = frameTileOrTextWidth + 8;
    }
UiFramedTextButtonControl_DrawClipped_RenderSelectedFrameTextAndFocusChrome:
    tileSizeOrEndCapSize = (*g_GraphicsTextureSourceGetLogicalSize)(frameTileOrTextWidth,g_UiWindowTextureSource);
    tileStart = tileSizeOrEndCapSize.logicalHeightPixels;
    tileEnd = tileSizeOrEndCapSize.logicalWidthPixels;
    rightEdgeOrTextY = (control->selectable).base.layoutWidth - tileEnd;
    bottomEdgeOrTextX = (control->selectable).base.layoutHeight - tileStart;
    if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) ||
       (((control->selectable).stateFlags & 0x800) == 0)) {
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                 (control->selectable).base.left,frameTileOrTextWidth,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                 rightEdgeOrTextY + (control->selectable).base.left,frameTileOrTextWidth + 1,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeOrTextX + (control->selectable).base.top,
                 (control->selectable).base.left,frameTileOrTextWidth + 2,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeOrTextX + (control->selectable).base.top,
                 rightEdgeOrTextY + (control->selectable).base.left,frameTileOrTextWidth + 3,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,frameTileOrTextWidth + 4,rightEdgeOrTextY,0,tileEnd,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,frameTileOrTextWidth + 5,bottomEdgeOrTextX,tileStart,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,frameTileOrTextWidth + 6,bottomEdgeOrTextX,tileStart,rightEdgeOrTextY,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,frameTileOrTextWidth + 7,rightEdgeOrTextY,bottomEdgeOrTextX,tileEnd,control);
    }
    resolvedText = TextResource_Resolve(control->textResourceId);
    commandStream = resolvedText.eax;
    textExtent = RichTextCommandStream_MeasureRegs(control->packedTextStyle,commandStream);
    frameTileOrTextWidth = textExtent.widthPixels;
    bottomEdgeOrTextX = (int)((control->selectable).base.layoutWidth - frameTileOrTextWidth) >> 1;
    rightEdgeOrTextY = (int)((control->selectable).base.layoutHeight - textExtent.heightPixels) >> 1;
    textStyle = g_UiTextStyleDisabled;
    if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
       (textStyle = g_UiTextStyleNormal,
       ((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
      textStyle = g_UiTextStyleSelected;
    }
    if (((control->selectable).stateFlags & 0x100) == 0) {
      control->packedTextStyle = control->packedTextStyle & 0xff0000;
    }
    else {
      textStyle = textStyle & 0xffffff;
    }
    if (((control->selectable).stateFlags & 0x200) == 0) {
      control->packedTextStyle = control->packedTextStyle & 0xff000000;
    }
    else {
      textStyle = textStyle & 0xff00ffff;
    }
    textStyle = textStyle | control->packedTextStyle;
    if ((((control->selectable).base.nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) ||
       (((control->selectable).stateFlags & 0x1000) != 0)) {
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,
                 rightEdgeOrTextY + (control->selectable).base.top,bottomEdgeOrTextX + (control->selectable).base.left);
    }
    else {
      bottomEdgeOrTextX = bottomEdgeOrTextX + (control->selectable).base.left;
      rightEdgeOrTextY = rightEdgeOrTextY + (control->selectable).base.top;
      baselineY = bottomEdgeOrTextX;
      drawX = rightEdgeOrTextY;
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,rightEdgeOrTextY,bottomEdgeOrTextX + -2,0x7f000000,0x86,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      tileSizeOrEndCapSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
      focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
      focusTileXOrTop = bottomEdgeOrTextX + -2 + focusTileSize.logicalWidthPixels;
      focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
      bottomEdgeOrTextX = ((bottomEdgeOrTextX + 4) - tileSizeOrEndCapSize.logicalWidthPixels) + frameTileOrTextWidth;
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,rightEdgeOrTextY,bottomEdgeOrTextX,0x7f000000,0x88,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      if (clipLeft < bottomEdgeOrTextX) {
        bottomEdgeOrTextX = clipLeft;
      }
      do {
        (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                  (clipTop,bottomEdgeOrTextX,clipBottom,clipRight,rightEdgeOrTextY,focusTileXOrTop,0x7f000000,0x87,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        focusTileXOrTop = focusTileXOrTop + focusTileSize.logicalWidthPixels;
      } while (focusTileXOrTop < bottomEdgeOrTextX);
      focusTileXOrTop = drawX + -1;
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,drawX,baselineY);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,focusTileXOrTop,baselineY + -3,0x86,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      tileSizeOrEndCapSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
      focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
      rightEdgeOrTextY = baselineY + -3 + focusTileSize.logicalWidthPixels;
      focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
      bottomEdgeOrTextX = ((baselineY + 3) - tileSizeOrEndCapSize.logicalWidthPixels) + frameTileOrTextWidth;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,focusTileXOrTop,bottomEdgeOrTextX,0x88,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      if (clipLeft < bottomEdgeOrTextX) {
        bottomEdgeOrTextX = clipLeft;
      }
      do {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,bottomEdgeOrTextX,clipBottom,clipRight,focusTileXOrTop,rightEdgeOrTextY,0x87,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        rightEdgeOrTextY = rightEdgeOrTextY + focusTileSize.logicalWidthPixels;
      } while (rightEdgeOrTextY < bottomEdgeOrTextX);
    }
  }
  else if (((control->selectable).stateFlags & 0x400) == 0) {
    if (((control->selectable).stateFlags & 4) == 0) {
      frameTileOrTextWidth = 0x8c;
    }
    else {
      frameTileOrTextWidth = 0xa4;
    }
    goto UiFramedTextButtonControl_DrawClipped_RenderSelectedFrameTextAndFocusChrome;
  }
  (*g_GraphicsFramebufferEndAccess)();
UiFramedTextButtonControl_DrawClipped_DrawChildrenIfEnabledAndReturn:
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    UiContainer_DrawIntersectingChildren
              (clipTop,clipLeft,clipBottom,clipRight,(UiNodeBase *)control);
  }
  return;
}


/* Address: 0x004B22A0.
   Ownership: ui/controls/text.
   Purpose: Updates momentary or toggle selected state, optionally plays activationSoundId, queues actionId, and
   invalidates the root.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime], UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiFramedTextButtonControl_NonRightPress
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
      if ((((control->selectable).stateFlags & 0x80) != 0) && (control->activationSoundId != 0)) {
        (*g_SoundPlayOneShot)
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
      if ((((control->selectable).stateFlags & 0x80) != 0) && (control->activationSoundId != 0)) {
        (*g_SoundPlayOneShot)
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
   Ownership: ui/controls/text.
   Purpose: Clears momentary selected state on release, optionally plays activationSoundId, queues actionId, and
   invalidates the root.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiFramedTextButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  
  if (((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
      (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0)) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    if ((((control->selectable).stateFlags & 0x80) != 0) && (control->activationSoundId != 0)) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control->activationSoundId
                );
    }
    stateFlagsField = &(control->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}


/* Address: 0x004B23F0.
   Ownership: ui/controls/text.
   Purpose: Performs rectangular capture tracking with an optional frame inset and updates selected hover/pressed
   state.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax
UiFramedTextButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control)

{
  UiSelectableStateFlags *clearedStateFlagsField;
  sdword *edgeField;
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
       ((((control->selectable).stateFlags & 4) != 0 &&
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
   Ownership: ui/controls/text.
   Purpose: Returns the control when the point lies inside its rectangle and, when enabled, inside the frame-inset
   interior; otherwise returns the 0xFFFFFFFF sentinel.
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiFramedTextButtonControl_HitTestRect
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFramedTextButtonControl *control)

{
  sdword *edgeField;
  UiFramedTextButtonControl *hitResult;
  int relativeX;
  int relativeY;
  
  hitResult = (UiFramedTextButtonControl *)0xffffffff;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    edgeField = &(control->selectable).base.left;
    relativeX = pointerX - *edgeField;
    if (((((*edgeField <= pointerX) &&
          (edgeField = &(control->selectable).base.top, relativeY = pointerY - *edgeField, *edgeField <= pointerY
          )) && (relativeX < (control->selectable).base.layoutWidth)) &&
        (relativeY < (control->selectable).base.layoutHeight)) &&
       ((((control->selectable).stateFlags & 4) == 0 ||
        (((g_UiWindowFrameInset <= relativeX && (g_UiWindowFrameInset <= relativeY)) &&
         ((relativeX + g_UiWindowFrameInset < (control->selectable).base.layoutWidth &&
          (relativeY + g_UiWindowFrameInset < (control->selectable).base.layoutHeight)))))))) {
      hitResult = control;
    }
  }
  return (UiNodeBase *)hitResult;
}


/* Address: 0x004B27D0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B2740[2]@004B2740.
   Cross-module calls: UiWindow_BlitTiledHorizontalEdge [ui/controls/layout], UiWindow_BlitTiledVerticalEdge
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiWindowControl_DrawFramedTextAndChrome
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  char iconOffsetX;
  char iconOffsetY;
  dword tileEnd;
  word *commandStream;
  int styleOffsetOrIconY;
  dword frameTileOrTextWidth;
  int focusTileX;
  dword tileStart;
  int bottomEdgeOrTextX;
  uint textStyle;
  int focusCoordOrIconX;
  int rightEdgeOrTextY;
  bool framebufferUnavailable;
  RichTextExtentRegs textExtent;
  TextResourceResolveEaxCf5 resolvedText;
  GraphicsTextureSizeEaxEdxCf9 tileSizeOrEndCapSize;
  GraphicsTextureSizeEaxEdxCf9 focusTileSize;
  
  framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
  if (framebufferUnavailable) {
    return;
  }
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((uint)control[1].nextSibling & 4) == 0) {
      frameTileOrTextWidth = 0x4a;
    }
    else {
      frameTileOrTextWidth = 0x94;
    }
    if (((uint)control[1].nextSibling & 2) != 0) {
      frameTileOrTextWidth = frameTileOrTextWidth + 8;
    }
  }
  else {
    if (((uint)control[1].nextSibling & 0x400) != 0)
    goto UiWindowControl_DrawFramedTextAndChrome_EndFramebufferAccess;
    if (((uint)control[1].nextSibling & 4) == 0) {
      frameTileOrTextWidth = 0x8c;
    }
    else {
      frameTileOrTextWidth = 0xa4;
    }
  }
  tileSizeOrEndCapSize = (*g_GraphicsTextureSourceGetLogicalSize)(frameTileOrTextWidth,g_UiWindowTextureSource);
  tileStart = tileSizeOrEndCapSize.logicalHeightPixels;
  tileEnd = tileSizeOrEndCapSize.logicalWidthPixels;
  rightEdgeOrTextY = control->layoutWidth - tileEnd;
  bottomEdgeOrTextX = control->layoutHeight - tileStart;
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,frameTileOrTextWidth,
             g_UiWindowTextureSource,g_FramebufferAccess);
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,control->top,rightEdgeOrTextY + control->left,frameTileOrTextWidth + 1,
             g_UiWindowTextureSource,g_FramebufferAccess);
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeOrTextX + control->top,control->left,frameTileOrTextWidth + 2,
             g_UiWindowTextureSource,g_FramebufferAccess);
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeOrTextX + control->top,rightEdgeOrTextY + control->left,
             frameTileOrTextWidth + 3,g_UiWindowTextureSource,g_FramebufferAccess);
  UiWindow_BlitTiledHorizontalEdge
            (clipTop,clipLeft,clipBottom,clipRight,frameTileOrTextWidth + 4,rightEdgeOrTextY,0,tileEnd,control);
  UiWindow_BlitTiledVerticalEdge
            (clipTop,clipLeft,clipBottom,clipRight,frameTileOrTextWidth + 5,bottomEdgeOrTextX,tileStart,0,control);
  UiWindow_BlitTiledVerticalEdge
            (clipTop,clipLeft,clipBottom,clipRight,frameTileOrTextWidth + 6,bottomEdgeOrTextX,tileStart,rightEdgeOrTextY,control);
  UiWindow_BlitTiledHorizontalEdge
            (clipTop,clipLeft,clipBottom,clipRight,frameTileOrTextWidth + 7,rightEdgeOrTextY,bottomEdgeOrTextX,tileEnd,control);
  resolvedText = TextResource_Resolve(control[1].top);
  commandStream = resolvedText.eax;
  bottomEdgeOrTextX = control->layoutWidth;
  textExtent = RichTextCommandStream_MeasureRegs(control[1].right,commandStream);
  frameTileOrTextWidth = textExtent.widthPixels;
  rightEdgeOrTextY = (int)(control->layoutHeight - textExtent.heightPixels) >> 1;
  bottomEdgeOrTextX = ((int)(((uint)(bottomEdgeOrTextX * 3) >> 2) - frameTileOrTextWidth) >> 1) + ((uint)control->layoutWidth >> 2);
  textStyle = g_UiTextStyleDisabled;
  if (((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (textStyle = g_UiTextStyleNormal, ((uint)control[1].nextSibling & 2) != 0)) {
    textStyle = g_UiTextStyleSelected;
  }
  if (((uint)control[1].nextSibling & 0x100) == 0) {
    control[1].right = control[1].right & 0xff0000;
  }
  else {
    textStyle = textStyle & 0xffffff;
  }
  if (((uint)control[1].nextSibling & 0x200) == 0) {
    control[1].right = control[1].right & 0xff000000;
  }
  else {
    textStyle = textStyle & 0xff00ffff;
  }
  textStyle = textStyle | control[1].right;
  if ((control->nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,rightEdgeOrTextY + control->top,
               bottomEdgeOrTextX + control->left);
  }
  else {
    bottomEdgeOrTextX = bottomEdgeOrTextX + control->left;
    rightEdgeOrTextY = rightEdgeOrTextY + control->top;
    styleOffsetOrIconY = (int)(textStyle << 0x10) >> 0x18;
    focusCoordOrIconX = styleOffsetOrIconY + -3 + bottomEdgeOrTextX;
    styleOffsetOrIconY = styleOffsetOrIconY + -1 + rightEdgeOrTextY;
    (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,styleOffsetOrIconY,focusCoordOrIconX,0x7f000000,0x86,
               g_UiWindowTextureSource,g_FramebufferAccess);
    tileSizeOrEndCapSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
    focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
    focusTileX = focusCoordOrIconX + focusTileSize.logicalWidthPixels;
    focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
    focusCoordOrIconX = ((focusCoordOrIconX + 6) - tileSizeOrEndCapSize.logicalWidthPixels) + frameTileOrTextWidth;
    (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,styleOffsetOrIconY,focusCoordOrIconX,0x7f000000,0x88,
               g_UiWindowTextureSource,g_FramebufferAccess);
    if (clipLeft < focusCoordOrIconX) {
      focusCoordOrIconX = clipLeft;
    }
    do {
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,focusCoordOrIconX,clipBottom,clipRight,styleOffsetOrIconY,focusTileX,0x7f000000,0x87,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      focusTileX = focusTileX + focusTileSize.logicalWidthPixels;
    } while (focusTileX < focusCoordOrIconX);
    focusCoordOrIconX = rightEdgeOrTextY + -1;
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,rightEdgeOrTextY,bottomEdgeOrTextX);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,focusCoordOrIconX,bottomEdgeOrTextX + -3,0x86,g_UiWindowTextureSource,
               g_FramebufferAccess);
    tileSizeOrEndCapSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
    focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
    rightEdgeOrTextY = bottomEdgeOrTextX + -3 + focusTileSize.logicalWidthPixels;
    focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
    bottomEdgeOrTextX = ((bottomEdgeOrTextX + 3) - tileSizeOrEndCapSize.logicalWidthPixels) + frameTileOrTextWidth;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,focusCoordOrIconX,bottomEdgeOrTextX,0x88,g_UiWindowTextureSource,
               g_FramebufferAccess);
    if (clipLeft < bottomEdgeOrTextX) {
      bottomEdgeOrTextX = clipLeft;
    }
    do {
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,bottomEdgeOrTextX,clipBottom,clipRight,focusCoordOrIconX,rightEdgeOrTextY,0x87,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      rightEdgeOrTextY = rightEdgeOrTextY + focusTileSize.logicalWidthPixels;
    } while (rightEdgeOrTextY < bottomEdgeOrTextX);
  }
  tileSizeOrEndCapSize = (*g_GraphicsTextureSourceGetLogicalSize)
                     ((dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent);
  focusCoordOrIconX = (((uint)control->layoutWidth >> 2) - tileSizeOrEndCapSize.logicalWidthPixels) + control->left;
  styleOffsetOrIconY = ((int)(control->layoutHeight - tileSizeOrEndCapSize.logicalHeightPixels) >> 1) + control->top;
  bottomEdgeOrTextX = focusCoordOrIconX;
  rightEdgeOrTextY = styleOffsetOrIconY;
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((uint)control[1].nextSibling & 2) == 0) {
      iconOffsetX = (char)control[1].left;
      iconOffsetY = *(char *)((int)&control[1].left + 1);
    }
    else {
      iconOffsetX = *(char *)((int)&control[1].left + 2);
      iconOffsetY = *(char *)((int)&control[1].left + 3);
    }
    bottomEdgeOrTextX = focusCoordOrIconX + iconOffsetX;
    rightEdgeOrTextY = styleOffsetOrIconY + iconOffsetY;
  }
  (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,rightEdgeOrTextY,bottomEdgeOrTextX,0x7f000000,(dword)control[1].vtable,
             (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,styleOffsetOrIconY,focusCoordOrIconX,(dword)control[1].vtable,
             (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
UiWindowControl_DrawFramedTextAndChrome_EndFramebufferAccess:
  (*g_GraphicsFramebufferEndAccess)();
  return;
}


/* Address: 0x004B2E40.
   Ownership: ui/controls/text.
   Purpose: Relocates child pointers through the common container helper; the text resource ID and packed style are
   scalar fields and require no relocation.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTextButtonControl_Relocate
          (UiSerializedRelocationDelta relocationDelta,UiTextButtonControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  return;
}


/* Address: 0x004B31B0.
   Ownership: ui/controls/text.
   Purpose: Hit-tests the button skin, optionally plays activationSoundId, updates selected/toggle state, queues
   actionId, and invalidates the control root.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_ecx_edx
UiTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextButtonControl *control)

{
  UiSelectableStateFlags *clearedStateFlagsField;
  bool opaquePixelHit;
  bool pixelHit;
  UiSelectableStateFlags *toggleStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) == 0) {
      pixelHit = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,0x46,g_UiWindowTextureSource);
      if (pixelHit) {
        if ((((control->selectable).stateFlags & 0x80) != 0) && (control->activationSoundId != 0)) {
          (*g_SoundPlayOneShot)
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
      pixelHit = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,0x40,g_UiWindowTextureSource);
      if (pixelHit) {
        if ((((control->selectable).stateFlags & 0x80) != 0) && (control->activationSoundId != 0)) {
          (*g_SoundPlayOneShot)
                    (g_UiSoundGainQ15,g_UiSoundGainQ15,
                     (DirectSoundVoiceSet *)control->activationSoundId);
        }
        toggleStateFlagsField = &(control->selectable).stateFlags;
        *toggleStateFlagsField = *toggleStateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
        clearedStateFlagsField = &(control->selectable).stateFlags;
        *clearedStateFlagsField = *clearedStateFlagsField & 0xffffffbf;
        UiActionQueue_Enqueue((control->selectable).actionId,control);
        UiNode_InvalidateRoot((UiNodeBase *)control);
      }
    }
  }
  return;
}


/* Address: 0x004B32C0.
   Ownership: ui/controls/text.
   Purpose: Handles focused Space-key activation, optionally plays activationSoundId, updates selected/toggle
   state, queues actionId, invalidates the root, and conveys consumption through CF.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime],
   UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiTextButtonControl_KeyboardEventCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextButtonControl *control)

{
  UiSelectableStateFlags *selectedStateFlagsField;
  bool delegatedResult;
  UiSelectableStateFlags *stateFlagsField;
  UiSelectableStateFlags *selectionStateFlagsField;
  
  if ((((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) && (keyCode == 0x20)) &&
      (control == (UiTextButtonControl *)g_UiKeyboardFocusNode)) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION) == 0)) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
      if ((((control->selectable).stateFlags & 0x80) != 0) && (control->activationSoundId != 0)) {
        (*g_SoundPlayOneShot)
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)control->activationSoundId);
      }
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
      selectionStateFlagsField = &(control->selectable).stateFlags;
      *selectionStateFlagsField = *selectionStateFlagsField & 0xffffffbf;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return false;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      if ((((control->selectable).stateFlags & 0x80) != 0) && (control->activationSoundId != 0)) {
        (*g_SoundPlayOneShot)
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
  delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNextCf
                    (keyboardStateMask,keyCode,(UiNodeBase *)control);
  return delegatedResult;
}


/* Address: 0x004B37C0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3770[2]@004B3770.
   Cross-module calls: UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiImagePanelControl_DrawAlignedTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

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
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    clippedLeft = control->left;
    if (control->left < clipRight) {
      clippedLeft = clipRight;
    }
    clippedTop = control->top;
    if (control->top < clipBottom) {
      clippedTop = clipBottom;
    }
    clippedRight = control->right;
    if (clipLeft < control->right) {
      clippedRight = clipLeft;
    }
    clippedBottom = control->bottom;
    if (clipTop < control->bottom) {
      clippedBottom = clipTop;
    }
    drawX = control->left;
    drawY = control->top;
    if ((GraphicsTextureSourceAsset *)control[1].parent != (GraphicsTextureSourceAsset *)0x0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)
                        ((dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent);
      slackWidth = control->layoutWidth - textureSize.logicalWidthPixels;
      slackHeight = control->layoutHeight - textureSize.logicalHeightPixels;
      if (((uint)control[1].nextSibling & 2) != 0) {
        drawX = drawX + slackWidth;
      }
      if (((uint)control[1].nextSibling & 8) != 0) {
        drawY = drawY + slackHeight;
      }
      if (((uint)control[1].nextSibling & 1) != 0) {
        drawX = drawX + (slackWidth >> 1);
      }
      if (((uint)control[1].nextSibling & 4) != 0) {
        drawY = drawY + (slackHeight >> 1);
      }
      framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
      if (!framebufferUnavailable) {
        if (((uint)control[1].nextSibling & 0x10) != 0) {
          (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                    (clippedBottom,clippedRight,clippedTop,clippedLeft,*(char *)((int)&control[1].firstChild + 1) + drawY,
                     *(char *)&control[1].firstChild + drawX,0x7f000000,(dword)control[1].vtable,
                     (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
        }
        if (((uint)control[1].nextSibling & 0x80) == 0) {
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (clippedBottom,clippedRight,clippedTop,clippedLeft,drawY,drawX,(dword)control[1].vtable,
                     (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
        }
        else {
          (*g_GraphicsTextureSourceStretchDirectColorBilinear)
                    (control->layoutHeight,control->layoutWidth,control->top,control->left,
                     (dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent,
                     g_FramebufferAccess);
        }
        (*g_GraphicsFramebufferEndAccess)();
      }
      UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
    }
  }
  return;
}


/* Address: 0x004B3960.
   Ownership: ui/controls/text.
   Purpose: Handles ui image panel control hit test aligned texture and children.
   Cross-module calls: UiContainer_HitTestChildren [ui/controls/layout].
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiImagePanelControl_HitTestAlignedTextureAndChildren(int pointerY,int pointerX,UiNodeBase *control)

{
  bool childrenAlreadyRetried;
  UiNodeBase *hitNode;
  int slackWidth;
  int drawX;
  int slackHeight;
  int drawY;
  bool opaqueHit;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  
  hitNode = (UiNodeBase *)0xffffffff;
  childrenAlreadyRetried = false;
  if (((uint)control[1].nextSibling & 0x20) != 0) {
    return (UiNodeBase *)0xffffffff;
  }
  if ((control->nodeFlags & UI_NODE_ALLOW_CHILD_HIT_TEST_OUTSIDE_BOUNDS) != 0)
  goto UiImagePanelHitTest_CheckChildren;
  do {
    if (((uint)control[1].nextSibling & 0x40) == 0) {
      drawX = control->left;
      drawY = control->top;
      if ((GraphicsTextureSourceAsset *)control[1].parent == (GraphicsTextureSourceAsset *)0x0) {
        return hitNode;
      }
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)
                        ((dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent);
      slackWidth = control->layoutWidth - textureSize.logicalWidthPixels;
      slackHeight = control->layoutHeight - textureSize.logicalHeightPixels;
      if (((uint)control[1].nextSibling & 2) != 0) {
        drawX = drawX + slackWidth;
      }
      if (((uint)control[1].nextSibling & 8) != 0) {
        drawY = drawY + slackHeight;
      }
      if (((uint)control[1].nextSibling & 1) != 0) {
        drawX = drawX + (slackWidth >> 1);
      }
      if (((uint)control[1].nextSibling & 4) != 0) {
        drawY = drawY + (slackHeight >> 1);
      }
      opaqueHit = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,drawY,drawX,(dword)control[1].vtable,
                         (GraphicsTextureSourceAsset *)control[1].parent);
      if (!opaqueHit) {
        return (UiNodeBase *)0xffffffff;
      }
    }
UiImagePanelHitTest_CheckChildren:
    hitNode = UiContainer_HitTestChildren(pointerY,pointerX,control);
    if (childrenAlreadyRetried) {
      return hitNode;
    }
    childrenAlreadyRetried = true;
  } while (hitNode == control);
  return hitNode;
}


/* Address: 0x004B3AA0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3A50[2]@004B3A50.
   Cross-module calls: UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiFillPanelControl_DrawColorOrTiledTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  int controlRight;
  int controlBottom;
  dword tileWidth;
  int tileLeft;
  dword tileHeight;
  sdword tileTop;
  bool framebufferUnavailable;
  GraphicsTextureSizeEaxEdxCf9 tileSize;
  
  controlRight = control->right;
  controlBottom = control->bottom;
  tileLeft = control->left;
  tileTop = control->top;
  if (control[1].parent == (UiNodeBase *)0x0) {
    framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
    if (!framebufferUnavailable) {
      (*g_GraphicsFramebufferFillRectArgb)
                (clipTop,clipLeft,clipBottom,clipRight,controlBottom,controlRight,tileTop,tileLeft,(dword)control[1].vtable,
                 g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  else {
    if (controlRight < clipLeft) {
      clipLeft = controlRight;
    }
    if (controlBottom < clipTop) {
      clipTop = controlBottom;
    }
    tileSize = (*g_GraphicsTextureSourceGetLogicalSize)
                      ((dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent);
    tileHeight = tileSize.logicalHeightPixels;
    tileWidth = tileSize.logicalWidthPixels;
    framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
    if (!framebufferUnavailable) {
UiFillPanelControl_DrawColorOrTiledTextureAndChildren_BlitNextTextureTile:
      do {
        if (((uint)control[1].nextSibling & 0x10) != 0) {
          (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                    (clipTop,clipLeft,clipBottom,clipRight,
                     *(char *)((int)&control[1].firstChild + 1) + tileTop,
                     *(char *)&control[1].firstChild + tileLeft,0x7f000000,(dword)control[1].vtable,
                     (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
        }
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,tileTop,tileLeft,(dword)control[1].vtable,
                   (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
        if (((uint)control[1].nextSibling & 1) != 0) {
          tileLeft = tileLeft + tileWidth;
          if (tileLeft < clipLeft)
          goto UiFillPanelControl_DrawColorOrTiledTextureAndChildren_BlitNextTextureTile;
          tileLeft = control->left;
        }
        if ((((uint)control[1].nextSibling & 2) == 0) || (tileTop = tileTop + tileHeight, clipTop <= tileTop))
        goto 
        UiFillPanelControl_DrawColorOrTiledTextureAndChildren_EndFramebufferAccessBeforeChildDraw;
      } while( true );
    }
  }
UiFillPanelControl_DrawColorOrTiledTextureAndChildren_DrawIntersectingChildrenAndReturn:
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
  return;
UiFillPanelControl_DrawColorOrTiledTextureAndChildren_EndFramebufferAccessBeforeChildDraw:
  (*g_GraphicsFramebufferEndAccess)();
  goto UiFillPanelControl_DrawColorOrTiledTextureAndChildren_DrawIntersectingChildrenAndReturn;
}


/* Address: 0x004B5E60.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[5]@004B58A0; g_UiNodeVtable_004B6800[5]@004B6800;
   g_UiNodeVtable_004B7050[5]@004B7050.
*/
void UiTextEditControl_EndSelection
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xffffffbf);
  if ((((uint)control[1].nextSibling & 0x400) != 0) && (control[1].bottom != 0)) {
    (*g_SoundPlayOneShot)
              (g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control[1].bottom);
  }
  return;
}


/* Address: 0x004B6480.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[14]@004B58A0; g_UiNodeVtable_004B6800[14]@004B6800;
   g_UiNodeVtable_004B7050[14]@004B7050.
   Cross-module calls: UiKeyboardFocus_ReleaseNode [ui/controls/input], UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTextEditControl_SuppressIfActionId(UiActionId actionId,UiTextEditControl *control)

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
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[15]@004B58A0; g_UiNodeVtable_004B6800[15]@004B6800;
   g_UiNodeVtable_004B7050[15]@004B7050.
   Cross-module calls: UiKeyboardFocus_AcquireIfNone [ui/controls/input], UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTextEditControl_UnsuppressIfActionId(UiActionId actionId,UiTextEditControl *control)

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
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[16]@004B58A0; g_UiNodeVtable_004B6800[16]@004B6800;
   g_UiNodeVtable_004B7050[16]@004B7050.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax UiTextEditControl_TickCaretBlink(UiTextEditControl *control)

{
  int blinkPhaseIncrement;
  UiTextEditStateFlags *stateFlagsField;
  UiTextEditStateFlags previousStateFlags;
  
  if (control == (UiTextEditControl *)g_UiKeyboardFocusNode) {
    stateFlagsField = &control->editStateFlags;
    previousStateFlags = *stateFlagsField;
    *stateFlagsField = *stateFlagsField - 0x1000000;
    if (previousStateFlags < 0x1000000) {
      blinkPhaseIncrement = g_UiTextEditCaretBlinkPhaseStep * 0x1000000;
      control->editStateFlags = control->editStateFlags ^ UI_TEXT_EDIT_CARET_VISIBLE_PHASE;
      control->editStateFlags = control->editStateFlags + blinkPhaseIncrement;
      UiNode_InvalidateRoot(&control->base);
    }
  }
  return;
}


/* Address: 0x004B95E0.
   Ownership: ui/controls/text.
   Purpose: Measures and aligns one rich-text line, draws the optional focused frame, renders the line with state-
   dependent style overrides, temporarily redirects keyboard focus for child drawing, and restores focus state.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], RichTextCommandStream_DrawSingleLine [assets/text/richtext],
   UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSingleLineTextControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  UiNodeVtable *packedStyleOverride;
  UiNodeBase *streamCapWidthOrChild;
  UiNodeBase *capWidthOrStream;
  int rightCapXOrFrameTop;
  uint textStyle;
  int lineWidth;
  int alignOffsetY;
  int frameTopOrTileX;
  int alignOffsetX;
  int tileX;
  bool framebufferUnavailable;
  RichTextExtentRegs textExtent;
  TextResourceResolveEaxCf5 resolvedText;
  GraphicsTextureSizeEaxEdxCf9 tileSize;
  UiPixelCoordinate originalClipLeft;
  UiPixelCoordinate restoredClipLeft;
  
  originalClipLeft = clipLeft;
  textStyle = g_UiTextStyleNormal;
  alignOffsetX = 0;
  alignOffsetY = 0;
  if (((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) ||
     (((uint)control[1].nextSibling & 0x40) == 0)) {
    if (((uint)control[1].nextSibling & 0x100) == 0) {
      control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xffffff);
    }
    else {
      textStyle = g_UiTextStyleNormal & 0xffffff;
    }
    if (((uint)control[1].nextSibling & 0x200) == 0) {
      control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xff00ffff);
    }
    else {
      textStyle = textStyle & 0xff00ffff;
    }
    packedStyleOverride = control[1].vtable;
    streamCapWidthOrChild = control[1].parent;
    if (((uint)control[1].nextSibling & 0x10) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)streamCapWidthOrChild);
      streamCapWidthOrChild = (UiNodeBase *)resolvedText.eax;
    }
    textExtent = RichTextCommandStream_MeasureRegs((textStyle | (uint)packedStyleOverride) & 0xffff0000,(word *)streamCapWidthOrChild);
    lineWidth = (int)(g_UiTextStyleNormal << 0x10) >> 0x18;
    if (lineWidth < 0) {
      lineWidth = -lineWidth;
    }
    lineWidth = textExtent.widthPixels + lineWidth;
    tileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
    streamCapWidthOrChild = (UiNodeBase *)tileSize.logicalWidthPixels;
    if (control[1].firstChild != (UiNodeBase *)0x0) {
      lineWidth = lineWidth + (int)streamCapWidthOrChild * 2;
    }
    if (((uint)control[1].nextSibling & 2) != 0) {
      alignOffsetX = control->layoutWidth - lineWidth;
    }
    if (((uint)control[1].nextSibling & 8) != 0) {
      alignOffsetY = control->layoutHeight - tileSize.logicalHeightPixels;
    }
    if (((uint)control[1].nextSibling & 4) != 0) {
      alignOffsetY = (int)(control->layoutHeight - tileSize.logicalHeightPixels) >> 1;
    }
    if (((uint)control[1].nextSibling & 1) != 0) {
      alignOffsetX = control->layoutWidth - lineWidth >> 1;
    }
    lineWidth = lineWidth + -1;
    framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
    if (!framebufferUnavailable) {
      restoredClipLeft = clipLeft;
      if ((control->nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) != 0) {
        tileX = alignOffsetX + 1 + control->left;
        frameTopOrTileX = alignOffsetY + 1 + control->top;
        rightCapXOrFrameTop = lineWidth + tileX;
        (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,frameTopOrTileX,tileX,0x7f000000,0x86,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        rightCapXOrFrameTop = rightCapXOrFrameTop - (int)streamCapWidthOrChild;
        (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,frameTopOrTileX,rightCapXOrFrameTop,0x7f000000,0x88,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        tileX = (int)&streamCapWidthOrChild->nextSibling + tileX;
        if (rightCapXOrFrameTop <= clipLeft) {
          clipLeft = rightCapXOrFrameTop;
        }
        tileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
        capWidthOrStream = streamCapWidthOrChild;
        do {
          (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                    (clipTop,clipLeft,clipBottom,clipRight,frameTopOrTileX,tileX,0x7f000000,0x87,
                     g_UiWindowTextureSource,g_FramebufferAccess);
          tileX = tileX + tileSize.logicalWidthPixels;
        } while (tileX < clipLeft);
        frameTopOrTileX = alignOffsetX + control->left;
        rightCapXOrFrameTop = alignOffsetY + control->top;
        streamCapWidthOrChild = capWidthOrStream;
        restoredClipLeft = originalClipLeft;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,originalClipLeft,clipBottom,clipRight,rightCapXOrFrameTop,frameTopOrTileX,0x86,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        lineWidth = (lineWidth + frameTopOrTileX) - (int)capWidthOrStream;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,originalClipLeft,clipBottom,clipRight,rightCapXOrFrameTop,lineWidth,0x88,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        frameTopOrTileX = (int)&capWidthOrStream->nextSibling + frameTopOrTileX;
        clipLeft = originalClipLeft;
        if (lineWidth <= originalClipLeft) {
          clipLeft = lineWidth;
        }
        tileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
        do {
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (clipTop,clipLeft,clipBottom,clipRight,rightCapXOrFrameTop,frameTopOrTileX,0x87,g_UiWindowTextureSource,
                     g_FramebufferAccess);
          frameTopOrTileX = frameTopOrTileX + tileSize.logicalWidthPixels;
        } while (frameTopOrTileX < clipLeft);
      }
      clipLeft = restoredClipLeft;
      if (control[1].firstChild != (UiNodeBase *)0x0) {
        alignOffsetX = (int)&streamCapWidthOrChild->nextSibling + alignOffsetX;
        alignOffsetY = alignOffsetY + 1;
      }
      capWidthOrStream = control[1].parent;
      if (((uint)control[1].nextSibling & 0x10) == 0) {
        resolvedText = TextResource_Resolve((TextResourceId)capWidthOrStream);
        capWidthOrStream = (UiNodeBase *)resolvedText.eax;
      }
      streamCapWidthOrChild = control[1].firstChild;
      textStyle = g_UiTextStyleNormal;
      if ((streamCapWidthOrChild != (UiNodeBase *)0x0) && ((streamCapWidthOrChild->nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
        textStyle = g_UiTextStyleDisabled;
      }
      if (((uint)control[1].nextSibling & 0x100) == 0) {
        control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xffffff);
      }
      else {
        textStyle = textStyle & 0xffffff;
      }
      if (((uint)control[1].nextSibling & 0x200) == 0) {
        control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xff00ffff);
      }
      else {
        textStyle = textStyle & 0xff00ffff;
      }
      control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xffff0000);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,textStyle | (uint)control[1].vtable,
                 (word *)capWidthOrStream,alignOffsetY + control->top,alignOffsetX + control->left);
      (*g_GraphicsFramebufferEndAccess)();
    }
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = streamCapWidthOrChild;
      streamCapWidthOrChild->nodeFlags = streamCapWidthOrChild->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
      UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
      streamCapWidthOrChild->nodeFlags = streamCapWidthOrChild->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
      g_UiKeyboardFocusNode = control;
    }
    else {
      UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
    }
  }
  return;
}


/* Address: 0x004B9E90.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9E40[2]@004B9E40.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext], UiWindow_BlitTiledHorizontalEdge
   [ui/controls/layout], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTextListControl_DrawRowsAndSelection
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextListControl *control)

{
  int rowTop;
  uint lastVisibleRow;
  int highlightWidth;
  word **lastRowSlot;
  word **rowSlot;
  bool framebufferUnavailable;
  RichTextExtentRegs rowExtent;
  GraphicsTextureSizeEaxEdxCf9 capSize;
  
  if (control->rowCount != 0) {
    rowTop = (clipBottom - (control->base).top) / (int)control->rowHeight;
    if (rowTop < 0) {
      rowTop = 0;
    }
    rowSlot = control->rowTextSlots + rowTop;
    lastVisibleRow = (int)((clipTop - (control->base).top) + control->rowHeight) / (int)control->rowHeight;
    rowTop = rowTop * control->rowHeight;
    if (control->rowCount <= lastVisibleRow) {
      lastVisibleRow = control->rowCount - 1;
    }
    lastRowSlot = control->rowTextSlots + lastVisibleRow;
    if (rowSlot <= lastRowSlot) {
      framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
      if (!framebufferUnavailable) {
        do {
          if (rowSlot == control->selectedRowSlot) {
            rowExtent = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,*rowSlot);
            highlightWidth = rowExtent.widthPixels + 6;
            if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x82,highlightWidth,rowTop,0,control);
            }
            else {
              capSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x83,g_UiWindowTextureSource);
              highlightWidth = highlightWidth - capSize.logicalWidthPixels;
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x84,highlightWidth,rowTop,
                         capSize.logicalWidthPixels,control);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,rowTop + (control->base).top,
                         (control->base).left,0x83,g_UiWindowTextureSource,g_FramebufferAccess);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,rowTop + (control->base).top,
                         highlightWidth + (control->base).left,0x85,g_UiWindowTextureSource,
                         g_FramebufferAccess);
            }
          }
          RichTextCommandStream_DrawSingleLine
                    (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,*rowSlot,
                     rowTop + 1 + (control->base).top,(control->base).left + 3);
          rowSlot = rowSlot + 1;
          rowTop = rowTop + control->rowHeight;
        } while (rowSlot <= lastRowSlot);
        (*g_GraphicsFramebufferEndAccess)();
      }
    }
  }
  return;
}


/* Address: 0x004BA040.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9E40[4]@004B9E40.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext],
   UiScrollableControl_ClampOffsetsToViewport [ui/controls/lists], UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTextListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextListControl *control)

{
  sdword *topField;
  sdword *leftField;
  uint rowIndex;
  int controlLeftOrRowTop;
  RichTextExtentRegs rowExtent;
  word **clickedRowSlot;
  
  topField = &(control->base).top;
  if (((*topField <= pointerY) &&
      (leftField = &(control->base).left, controlLeftOrRowTop = *leftField, *leftField <= pointerX)) &&
     (rowIndex = (uint)(pointerY - *topField) / control->rowHeight, rowIndex < control->rowCount)) {
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
           (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
          (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
        }
      }
    }
  }
  return;
}


/* Address: 0x004BA130.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9E40[12]@004B9E40.
   Cross-module calls: UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input], UiActionQueue_Enqueue
   [ui/core/runtime], UiScrollableControl_QueryContentSizeRegs [ui/controls/lists],
   UiScrollableControl_ClampOffsetsToViewport [ui/controls/lists].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiTextListControl_HandleKeyboardNavigationAndSearchCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextListControl *control)

{
  word **previousSelectedSlot;
  word **slotCursorOrSelection;
  int rowIndexOrTopOrPulse;
  uint pageDownRow;
  UiListRowCount remainingRows;
  word **candidateSlot;
  bool delegatedOrMismatch;
  UiScrollableContentDimensionsEdxEax8 contentSize;
  
  previousSelectedSlot = control->selectedRowSlot;
  if ((keyCode & 0xffff0000) == 0) {
    if (((keyboardStateMask & 0x3c) != 0) ||
       ((control->listStateFlags & UI_TEXT_LIST_TYPE_SEARCH_ENABLED) == 0)) {
UiTextListControl_DelegateUnhandledKeyboardEvent:
      delegatedOrMismatch = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
      return delegatedOrMismatch;
    }
    remainingRows = control->rowCount;
    slotCursorOrSelection = control->rowTextSlots;
    if (remainingRows != 0) {
      do {
        candidateSlot = slotCursorOrSelection;
        delegatedOrMismatch = (byte)(*(code *)g_KeyboardAsciiCaseTransformCallbacks3.compareCaseInsensitiveFlags)
                          (keyCode,*(dword *)*candidateSlot);
        if (!delegatedOrMismatch) break;
        remainingRows = remainingRows - 1;
        slotCursorOrSelection = candidateSlot + 1;
      } while (remainingRows != 0);
      control->selectedRowSlot = candidateSlot;
    }
  }
  else if (keyCode == 0x10001) {
    control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_SELECTION_CONFIRMED;
    UiActionQueue_Enqueue(control->actionId,control);
  }
  else if (keyCode == 0x10010) {
    control->selectedRowSlot = control->rowTextSlots;
  }
  else if (keyCode == 0x10018) {
    control->selectedRowSlot = control->rowTextSlots + (control->rowCount - 1);
  }
  else if (keyCode == 0x10012) {
    contentSize = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)(control->base).parent);
    rowIndexOrTopOrPulse = ((uint)((int)control->selectedRowSlot - (int)control->rowTextSlots) >> 2) -
            ((int)((contentSize >> 0x20) / (ulonglong)control->rowHeight) + -1);
    if (rowIndexOrTopOrPulse < 0) {
      rowIndexOrTopOrPulse = 0;
    }
    control->selectedRowSlot = control->rowTextSlots + rowIndexOrTopOrPulse;
  }
  else if (keyCode == 0x1001a) {
    contentSize = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)(control->base).parent);
    pageDownRow = ((uint)((int)control->selectedRowSlot - (int)control->rowTextSlots) >> 2) +
            (int)((contentSize >> 0x20) / (ulonglong)control->rowHeight) + -1;
    if (control->rowCount <= pageDownRow) {
      pageDownRow = control->rowCount - 1;
    }
    control->selectedRowSlot = control->rowTextSlots + pageDownRow;
  }
  else if (keyCode == 0x10011) {
    if (control->rowTextSlots <= control->selectedRowSlot + -1) {
      control->selectedRowSlot = control->selectedRowSlot + -1;
    }
  }
  else {
    if (keyCode != 0x10019) goto UiTextListControl_DelegateUnhandledKeyboardEvent;
    if (((uint)((int)control->selectedRowSlot - (int)control->rowTextSlots) >> 2) + 1 <
        control->rowCount) {
      control->selectedRowSlot = control->selectedRowSlot + 1;
    }
  }
  slotCursorOrSelection = control->selectedRowSlot;
  if (slotCursorOrSelection != previousSelectedSlot) {
    if (((control->listStateFlags & UI_TEXT_LIST_PLAY_SELECTION_SOUND) != 0) &&
       (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
      (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
    }
    rowIndexOrTopOrPulse = ((uint)((int)slotCursorOrSelection - (int)control->rowTextSlots) >> 2) * control->rowHeight;
    UiScrollableControl_ClampOffsetsToViewport
              (rowIndexOrTopOrPulse + control->rowHeight + 1,(control->base).rightOffset,rowIndexOrTopOrPulse,0,
               (UiScrollableControl *)(control->base).parent);
    rowIndexOrTopOrPulse = g_UiListActivationPulseFrames;
    control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_DEFERRED_ACTION_PENDING;
    control->listStateFlags = control->listStateFlags & 0xffffff;
    control->listStateFlags = control->listStateFlags | rowIndexOrTopOrPulse << 0x18;
  }
  return false;
}


/* Address: 0x004BA390.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9E40[16]@004B9E40.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_preserve_eax UiTextListControl_TickActivationPulse(UiTextListControl *control)

{
  if (((control->listStateFlags & UI_TEXT_LIST_DEFERRED_ACTION_PENDING) != 0) &&
     (control->listStateFlags = control->listStateFlags - 0x1000000,
     (control->listStateFlags & 0xff000000) == 0)) {
    control->listStateFlags = control->listStateFlags & 0xfffff9;
    UiActionQueue_Enqueue(control->actionId,control);
  }
  return;
}


/* Address: 0x004BA3D0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9E40[15]@004B9E40.
   Cross-module calls: UiContainer_UnsuppressActionId [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTextListControl_UnsuppressIfActionId(UiActionId actionId,UiTextListControl *control)

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
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9E40[14]@004B9E40.
   Cross-module calls: UiContainer_SuppressActionId [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTextListControl_SuppressIfActionId(UiActionId actionId,UiTextListControl *control)

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
   Ownership: ui/controls/text.
   Purpose: Initializes a pointer-list control from rich-text row pointers, computes line height and maximum
   measured row width, clears offsets, and requests parent layout.
   Cross-module calls: FontGlyph_GetLogicalSizeForStyleRegs [assets/text/resources],
   RichTextCommandStream_MeasureRegs [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_InitializeMeasuredTextRows
          (UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control)

{
  UiNodeBase *parentNode;
  UiPixelExtent rowHeightPixels;
  uint maximumTextWidthPixels;
  RichTextExtentRegs measuredTextExtent;
  FontGlyphSizeEaxEdxCf9 fontSize;
  UiNodeVtable *parentVtable;
  
  fontSize = FontGlyph_GetLogicalSizeForStyleRegs(g_UiListTextStyle,0);
  rowHeightPixels = fontSize.lineHeight + 1;
  control->rowHeight = rowHeightPixels;
  control->rowCount = rowCount;
  control->rowSlots = rowPointers;
  maximumTextWidthPixels = 0;
  control->selectedRowSlot = rowPointers;
  (control->base).bottomOffset = rowHeightPixels * rowCount + 1;
  for (; rowCount != 0; rowCount = rowCount - 1) {
    measuredTextExtent = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,*rowPointers);
    if (maximumTextWidthPixels < measuredTextExtent.widthPixels) {
      maximumTextWidthPixels = measuredTextExtent.widthPixels;
    }
    rowPointers = rowPointers + 1;
  }
  parentNode = (control->base).parent;
  parentVtable = parentNode->vtable;
  (control->base).rightOffset = maximumTextWidthPixels + 6;
  (control->base).leftOffset = 0;
  (control->base).topOffset = 0;
  (*parentVtable->layout)(parentNode);
  return;
}


/* Address: 0x004BC490.
   Ownership: ui/controls/text.
   Purpose: Draws a wrapped rich-text control inside a clip rectangle, resolving either a localized resource or
   direct command stream, then draws intersecting children.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawWrappedBlockCf
   [assets/text/richtext], UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiWrappedTextControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  UiNodeVtable *packedStyleOverride;
  UiNodeBase *commandStream;
  uint textStyle;
  bool framebufferUnavailable;
  TextResourceResolveEaxCf5 resolvedText;
  
  framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
  if (!framebufferUnavailable) {
    if (((uint)control[1].nextSibling & 0x40) == 0) {
      control[1].firstChild = (UiNodeBase *)control->layoutWidth;
    }
    textStyle = g_UiTextStyleNormal;
    if (((uint)control[1].nextSibling & 0x100) == 0) {
      control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xffffff);
    }
    else {
      textStyle = g_UiTextStyleNormal & 0xffffff;
    }
    if (((uint)control[1].nextSibling & 0x200) == 0) {
      control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xff00ffff);
    }
    else {
      textStyle = textStyle & 0xff00ffff;
    }
    packedStyleOverride = control[1].vtable;
    commandStream = control[1].parent;
    if (((uint)control[1].nextSibling & 0x10) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)commandStream);
      commandStream = (UiNodeBase *)resolvedText.eax;
    }
    RichTextCommandStream_DrawWrappedBlockCf
              (clipTop,clipLeft,clipBottom,clipRight,(textStyle | (uint)packedStyleOverride) & 0xffff0000,
               (word *)commandStream,(UiPixelExtent)control[1].firstChild,control->top,control->left
              );
    (*g_GraphicsFramebufferEndAccess)();
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
  return;
}


/* Address: 0x004BCC80.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BCC30[2]@004BCC30.
   Cross-module calls: UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiNineSlicePanelControl_DrawTextureFrameAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  UiNodeBase *baseTextureFrame;
  dword slice4Width;
  int leftEdgeX;
  int topEdgeY;
  int bottomEdgeY;
  int rightEdgeX;
  bool framebufferUnavailable;
  GraphicsTextureSizeEaxEdxCf9 slice0Size;
  GraphicsTextureSizeEaxEdxCf9 slice1Size;
  GraphicsTextureSizeEaxEdxCf9 slice2Size;
  GraphicsTextureSizeEaxEdxCf9 slice3Size;
  GraphicsTextureSizeEaxEdxCf9 slice4Or5Size;
  GraphicsTextureSizeEaxEdxCf9 slice6Size;
  GraphicsTextureSizeEaxEdxCf9 slice7Size;
  
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
    if (!framebufferUnavailable) {
      baseTextureFrame = control[1].parent;
      slice0Size = (*g_GraphicsTextureSourceGetLogicalSize)
                        ((dword)baseTextureFrame,(GraphicsTextureSourceAsset *)control[1].firstChild);
      slice1Size = (*g_GraphicsTextureSourceGetLogicalSize)
                        ((dword)((int)&baseTextureFrame->nextSibling + 1),
                         (GraphicsTextureSourceAsset *)control[1].firstChild);
      slice2Size = (*g_GraphicsTextureSourceGetLogicalSize)
                        ((dword)((int)&baseTextureFrame->nextSibling + 2),
                         (GraphicsTextureSourceAsset *)control[1].firstChild);
      slice3Size = (*g_GraphicsTextureSourceGetLogicalSize)
                         ((dword)((int)&baseTextureFrame->nextSibling + 3),
                          (GraphicsTextureSourceAsset *)control[1].firstChild);
      slice4Or5Size = (*g_GraphicsTextureSourceGetLogicalSize)
                         ((dword)&baseTextureFrame->firstChild,
                          (GraphicsTextureSourceAsset *)control[1].firstChild);
      slice4Width = slice4Or5Size.logicalWidthPixels;
      slice4Or5Size = (*g_GraphicsTextureSourceGetLogicalSize)
                         ((dword)((int)&baseTextureFrame->firstChild + 1),
                          (GraphicsTextureSourceAsset *)control[1].firstChild);
      slice6Size = (*g_GraphicsTextureSourceGetLogicalSize)
                         ((dword)((int)&baseTextureFrame->firstChild + 2),
                          (GraphicsTextureSourceAsset *)control[1].firstChild);
      slice7Size = (*g_GraphicsTextureSourceGetLogicalSize)
                         ((dword)((int)&baseTextureFrame->firstChild + 3),
                          (GraphicsTextureSourceAsset *)control[1].firstChild);
      leftEdgeX = control->left;
      topEdgeY = control->top;
      bottomEdgeY = control->bottom;
      rightEdgeX = control->right - slice1Size.logicalWidthPixels;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,topEdgeY,leftEdgeX,(dword)baseTextureFrame,
                 (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,topEdgeY,rightEdgeX,
                 (dword)((int)&baseTextureFrame->nextSibling + 1),
                 (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      leftEdgeX = leftEdgeX + slice0Size.logicalWidthPixels;
      (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,rightEdgeX,topEdgeY,leftEdgeX,
                 (dword)((int)&baseTextureFrame->nextSibling + 2),
                 (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      leftEdgeX = leftEdgeX - slice0Size.logicalWidthPixels;
      topEdgeY = topEdgeY + slice0Size.logicalHeightPixels;
      bottomEdgeY = bottomEdgeY - slice4Or5Size.logicalHeightPixels;
      (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY,-0x80000000,topEdgeY,leftEdgeX,
                 (dword)((int)&baseTextureFrame->nextSibling + 3),
                 (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY,leftEdgeX,
                 (dword)((int)&baseTextureFrame->firstChild + 1),
                 (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      rightEdgeX = (rightEdgeX + slice1Size.logicalWidthPixels) - slice4Width;
      topEdgeY = (topEdgeY - slice0Size.logicalHeightPixels) + slice1Size.logicalHeightPixels;
      bottomEdgeY = (bottomEdgeY + slice4Or5Size.logicalHeightPixels) - slice6Size.logicalHeightPixels;
      (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY,-0x80000000,topEdgeY,rightEdgeX,
                 (dword)&baseTextureFrame->firstChild,(GraphicsTextureSourceAsset *)control[1].firstChild,
                 g_FramebufferAccess);
      rightEdgeX = (rightEdgeX + slice4Width) - slice6Size.logicalWidthPixels;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY,rightEdgeX,
                 (dword)((int)&baseTextureFrame->firstChild + 2),
                 (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      leftEdgeX = leftEdgeX + slice4Or5Size.logicalWidthPixels;
      bottomEdgeY = (bottomEdgeY + slice6Size.logicalHeightPixels) - slice7Size.logicalHeightPixels;
      (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,rightEdgeX,bottomEdgeY,leftEdgeX,
                 (dword)((int)&baseTextureFrame->firstChild + 3),
                 (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,bottomEdgeY,
                 (rightEdgeX + slice6Size.logicalWidthPixels) - slice4Width,
                 (topEdgeY - slice1Size.logicalHeightPixels) + slice2Size.logicalHeightPixels,
                 (leftEdgeX - slice4Or5Size.logicalWidthPixels) + slice3Size.logicalWidthPixels,
                 (dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].firstChild,
                 g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
  return;
}


/* Address: 0x00515830.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_005157E0[0]@005157E0.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiFormattedContainer_RelocateWithPatchedTextPayloads
          (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  word *stream;
  TextResourceResolveEaxCf5 resolvedText;
  
  if ((control->nodeFlags & UI_NODE_TOOLTIP_ELIGIBLE) != 0) {
    resolvedText = TextResource_Resolve(control[-1].nodeFlags);
    stream = resolvedText.eax;
    RichTextCommandStream_PatchPayloadBySelector(0,&control[1].top,stream);
    RichTextCommandStream_PatchPayloadBySelector(1,&control[1].bottomOffset,stream);
    if (((uint)control[1].nextSibling & 2) != 0) {
      RichTextCommandStream_PatchPayloadBySelector(2,control + 2,stream);
      control[2].nextSibling = (UiNodeBase *)0x0;
    }
  }
  control[1].top = 0;
  control[1].bottomOffset = 0;
  UiContainer_RelocateChildren(relocationDelta,control);
  return;
}


/* Address: 0x005158B0.
   Ownership: ui/controls/text.
   Purpose: Handles ui formatted container draw clipped.
   Cross-module calls: GraphicsTextureSource_BlitTiledSourceAlpha [graphics/resources/texture].
*/
void __thandor_void_preserve_eax_ecx_edx
UiFormattedContainer_DrawClipped
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  UiNodeBase *primaryValue;
  UiNodeBase *secondaryValue;
  uint fillPercent;
  int scaleRange;
  UiNodeBase *scaleLimit;
  dword textureFrame;
  int barEndOrSpanOrMarkerX;
  int variantOrFillEnd;
  int barStartX;
  bool framebufferUnavailable;
  GraphicsTextureSizeEaxEdxCf9 frameSize;
  int tertiaryMarkerOffset;
  
  if ((control[1].parent == (UiNodeBase *)0x0) ||
     (framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)(), framebufferUnavailable))
  goto UiFormattedContainer_RefreshNumericText;
  if (clipRight < control->left) {
    clipRight = control->left;
  }
  if (clipBottom < control->top) {
    clipBottom = control->top;
  }
  if (control->right < clipLeft) {
    clipLeft = control->right;
  }
  if (control->bottom < clipTop) {
    clipTop = control->bottom;
  }
  textureFrame = control[1].left;
  barStartX = control->left;
  barEndOrSpanOrMarkerX = control->right;
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,control->top,barStartX,textureFrame,
             (GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
  frameSize = (*g_GraphicsTextureSourceGetLogicalSize)
                     (textureFrame,(GraphicsTextureSourceAsset *)control[1].vtable);
  barStartX = barStartX + frameSize.logicalWidthPixels;
  frameSize = (*g_GraphicsTextureSourceGetLogicalSize)
                     (textureFrame + 2,(GraphicsTextureSourceAsset *)control[1].vtable);
  barEndOrSpanOrMarkerX = barEndOrSpanOrMarkerX - frameSize.logicalWidthPixels;
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,control->top,barEndOrSpanOrMarkerX,textureFrame + 2,
             (GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
  if (barStartX < barEndOrSpanOrMarkerX) {
    GraphicsTextureSource_BlitTiledSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,barEndOrSpanOrMarkerX,control->top,barStartX,textureFrame + 1,
               (GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
    primaryValue = control[1].firstChild;
    scaleRange = control[1].layoutHeight;
    barEndOrSpanOrMarkerX = barEndOrSpanOrMarkerX - barStartX;
    while( true ) {
      for (; (scaleRange < (int)primaryValue || (scaleRange < (int)control[1].parent)); scaleRange = scaleRange << 2) {
      }
      if (((uint)control[1].nextSibling & 2) == 0) goto UiFormattedContainer_UseResolvedScaleRange;
      if ((int)control[1].nodeFlags <= scaleRange) break;
      scaleRange = scaleRange << 2;
    }
    tertiaryMarkerOffset = (int)(((longlong)(int)control[1].nodeFlags * (longlong)barEndOrSpanOrMarkerX) / (longlong)scaleRange);
UiFormattedContainer_UseResolvedScaleRange:
    secondaryValue = control[1].parent;
    scaleLimit = control[1].parent;
    if ((((uint)control[1].nextSibling & 2) != 0) && ((int)control[1].nodeFlags < (int)scaleLimit)) {
      scaleLimit = (UiNodeBase *)control[1].nodeFlags;
    }
    fillPercent = (uint)(((longlong)(int)primaryValue * 100) / (longlong)(int)scaleLimit);
    if (((uint)control[1].nextSibling & 1) == 0) {
      variantOrFillEnd = 3;
      if ((((0x4f < fillPercent) && (variantOrFillEnd = 6, 0x53 < fillPercent)) && (variantOrFillEnd = 9, 0x57 < fillPercent)) &&
         ((variantOrFillEnd = 0xc, 0x5b < fillPercent && (variantOrFillEnd = 0xf, 0x5f < fillPercent)))) {
UiFormattedContainer_SelectMaximumTextureVariant:
        variantOrFillEnd = 0x12;
      }
    }
    else {
      variantOrFillEnd = 0x12;
      if (((((7 < fillPercent) && (variantOrFillEnd = 0xf, 0xf < fillPercent)) && (variantOrFillEnd = 0xc, 0x17 < fillPercent)) &&
          ((((variantOrFillEnd = 9, 0x1f < fillPercent && (variantOrFillEnd = 6, 0x27 < fillPercent)) &&
            ((variantOrFillEnd = 3, 0x55 < fillPercent && ((variantOrFillEnd = 6, 0x57 < fillPercent && (variantOrFillEnd = 9, 0x59 < fillPercent))))))
           && (variantOrFillEnd = 0xc, 0x5b < fillPercent)))) && (variantOrFillEnd = 0xf, 0x5d < fillPercent))
      goto UiFormattedContainer_SelectMaximumTextureVariant;
    }
    textureFrame = variantOrFillEnd + control[1].left;
    if (control[1].firstChild != (UiNodeBase *)0x0) {
      frameSize = (*g_GraphicsTextureSourceGetLogicalSize)
                         (textureFrame,(GraphicsTextureSourceAsset *)control[1].vtable);
      barStartX = barStartX - frameSize.logicalWidthPixels;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,barStartX,textureFrame,
                 (GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
      barStartX = barStartX + frameSize.logicalWidthPixels;
      variantOrFillEnd = (int)(((longlong)(int)primaryValue * (longlong)barEndOrSpanOrMarkerX) / (longlong)scaleRange) + barStartX;
      GraphicsTextureSource_BlitTiledSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,variantOrFillEnd,control->top,barStartX,
                 textureFrame + 1,(GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,variantOrFillEnd,textureFrame + 2,
                 (GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
    }
    barEndOrSpanOrMarkerX = barStartX + (int)(((longlong)(int)secondaryValue * (longlong)barEndOrSpanOrMarkerX) / (longlong)scaleRange);
    textureFrame = control[1].left + 0x15;
    frameSize = (*g_GraphicsTextureSourceGetLogicalSize)
                       (textureFrame,(GraphicsTextureSourceAsset *)control[1].vtable);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,
               barEndOrSpanOrMarkerX - ((int)frameSize.logicalWidthPixels >> 1),textureFrame,
               (GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
    if (((uint)control[1].nextSibling & 2) != 0) {
      textureFrame = control[1].left + 0x15;
      frameSize = (*g_GraphicsTextureSourceGetLogicalSize)
                         (textureFrame,(GraphicsTextureSourceAsset *)control[1].vtable);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,
                 (barStartX + tertiaryMarkerOffset) - ((int)frameSize.logicalWidthPixels >> 1),textureFrame,
                 (GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
    }
  }
  (*g_GraphicsFramebufferEndAccess)();
UiFormattedContainer_RefreshNumericText:
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)control[1].firstChild,
             (word *)&control[1].top);
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)control[1].parent,
             (word *)&control[1].bottomOffset);
  if (((uint)control[1].nextSibling & 2) != 0) {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[1].nodeFlags,(word *)(control + 2));
  }
  return;
}


/* Address: 0x00516D10.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00516CC0[2]@00516CC0.
   Cross-module calls: SelectionPanel_RenderArmyRuntimeMetrics [gameplay/selection/runtime],
   UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiArmyMetricsPanel_DrawTextureMetricsAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

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
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    clippedLeft = control->left;
    if (control->left < clipRight) {
      clippedLeft = clipRight;
    }
    clippedTop = control->top;
    if (control->top < clipBottom) {
      clippedTop = clipBottom;
    }
    clippedRight = control->right;
    if (clipLeft < control->right) {
      clippedRight = clipLeft;
    }
    clippedBottom = control->bottom;
    if (clipTop < control->bottom) {
      clippedBottom = clipTop;
    }
    drawX = control->left;
    drawY = control->top;
    if ((GraphicsTextureSourceAsset *)control[1].parent != (GraphicsTextureSourceAsset *)0x0) {
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)
                        ((dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent);
      slackWidth = control->layoutWidth - textureSize.logicalWidthPixels;
      slackHeight = control->layoutHeight - textureSize.logicalHeightPixels;
      if (((uint)control[1].nextSibling & 2) != 0) {
        drawX = drawX + slackWidth;
      }
      if (((uint)control[1].nextSibling & 8) != 0) {
        drawY = drawY + slackHeight;
      }
      if (((uint)control[1].nextSibling & 1) != 0) {
        drawX = drawX + (slackWidth >> 1);
      }
      if (((uint)control[1].nextSibling & 4) != 0) {
        drawY = drawY + (slackHeight >> 1);
      }
      framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
      if (!framebufferUnavailable) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clippedBottom,clippedRight,clippedTop,clippedLeft,drawY,drawX,(dword)control[1].vtable,
                   (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
        (*g_GraphicsFramebufferEndAccess)();
        savedPanelData = g_SelectionPanelData;
        savedTextureSource = g_SelectionPanelTextureSource;
        g_SelectionPanelTextureSource = savedTextureSource;
        g_SelectionPanelData = savedPanelData;
        if (control[1].left != 0) {
          g_SelectionPanelTextureSource = g_InfoPanelTextureSource;
          g_SelectionPanelData = g_InfoPanelData;
          SelectionPanel_RenderArmyRuntimeMetrics
                    (clipTop,clipLeft,clipBottom,clipRight,control->bottom,control->right,
                     control->top,control->left,(RuntimeModelFactionPrefix10 *)control[1].left);
          g_SelectionPanelTextureSource = savedTextureSource;
          g_SelectionPanelData = savedPanelData;
        }
      }
      UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
    }
  }
  return;
}


/* Address: 0x00519110.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00518C90[2]@00518C90.
   Cross-module calls: SoftwareTexture_BilinearBlendScaleSubresources [graphics/backend/software],
   UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  bool framebufferUnavailable;
  
  if (((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (control[1].firstChild != (UiNodeBase *)0x0)) {
    framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
    if (!framebufferUnavailable) {
      SoftwareTexture_BilinearBlendScaleSubresources
                (control->layoutHeight,control->layoutWidth,control->top,control->left,
                 (qword *)control[1].right,(qword *)control[1].top,
                 (GraphicsSubresourceIndex)control[1].vtable,
                 (GraphicsSubresourceIndex)control[1].parent,(int *)control[1].firstChild,
                 (int *)g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
  return;
}


/* Address: 0x00519190.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00518C90[4]@00518C90.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_preserve_eax
UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  UiActionQueue_Enqueue(control[1].left,control);
  return;
}


/* Address: 0x005191B0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00518C90[6]@00518C90.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_preserve_eax
UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  UiActionQueue_Enqueue(control[1].left,control);
  return;
}


/* Address: 0x005191D0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00518C90[12]@00518C90.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiKeyboardFocus_MoveNext [ui/controls/input].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiSoftwareTexturePreviewControl_HandleKeyboardActivationCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control)

{
  if (keyCode != 0x10002) {
    UiActionQueue_Enqueue(control[1].left,control);
    return false;
  }
  UiKeyboardFocus_MoveNext();
  return false;
}


/* Address: 0x004B0150.
   Ownership: ui/controls/text.
   Purpose: Stores the pointer coordinates, hit-tests the active root when no pointer capture exists, accepts only
   nodes with nodeFlags 0x100, reloads the delay countdown on target changes, and clears the previous tooltip text.
   Local calls: UiTooltip_PrepareTargetText.
*/
void __thandor_void_preserve_eax_ecx_edx
UiTooltip_UpdateHoverTarget(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiNodeBase *node;
  UiNodeBase *hitTestNode;
  
  node = g_UiTooltipState.targetNode;
  g_UiTooltipState.pointerX = pointerX;
  g_UiTooltipState.pointerY = pointerY;
  g_UiTooltipState.targetNode = (UiNodeBase *)0x0;
  if ((((((g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) &&
         (g_UiRootNode != (UiRootNode *)0xffffffff)) && ((g_UiRootNode->base).left <= pointerX)) &&
       (((g_UiRootNode->base).top <= pointerY && (pointerX < (g_UiRootNode->base).right)))) &&
      ((pointerY < (g_UiRootNode->base).bottom &&
       ((hitTestNode = (*((g_UiRootNode->base).vtable)->hitTest)
                                 (pointerY,pointerX,&g_UiRootNode->base),
        hitTestNode != (UiNodeBase *)0xffffffff &&
        ((hitTestNode->nodeFlags & UI_NODE_TOOLTIP_ELIGIBLE) != 0)))))) &&
     (g_UiTooltipState.targetNode = hitTestNode, hitTestNode == node)) {
    return;
  }
  g_UiTooltipState.countdownFrames = g_UiTooltipDelayFrames;
  UiTooltip_PrepareTargetText(node);
  return;
}


/* Address: 0x004B6520.
   Ownership: ui/controls/text.
   Purpose: Clears the exact 0x20-byte UTF-16 edit buffer, formats the value at +0x58 as signed decimal or
   uppercase hexadecimal according to state flags, and refreshes range validity.
   Local calls: UiNumericTextControl_UpdateRangeValidity.
*/
void __thandor_void_preserve_eax_ecx_edx
UiNumericTextControl_RebuildTextFromValue(UiNumericTextControl *control)

{
  uint remainingValue;
  sbyte rotateShift;
  uint highBitOrDigitsLeft;
  uint digitCodeUnit;
  word *outputCursor;
  
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
  control->textBuffer[0xb] = 0;
  control->textBuffer[0xc] = 0;
  control->textBuffer[0xd] = 0;
  control->textBuffer[0xe] = 0;
  control->textBuffer[0xf] = 0;
  if (((control->editStateFlags & UI_NUMERIC_TEXT_SIGNED_VALUE) != 0) && ((int)remainingValue < 0)) {
    *outputCursor = 0x2d;
    remainingValue = -remainingValue;
    outputCursor = control->textBuffer + 1;
  }
  if ((control->editStateFlags & UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT) == 0) {
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,remainingValue,outputCursor);
  }
  else {
    highBitOrDigitsLeft = 0x1f;
    if (remainingValue != 0) {
      for (; remainingValue >> highBitOrDigitsLeft == 0; highBitOrDigitsLeft = highBitOrDigitsLeft - 1) {
      }
    }
    if (remainingValue != 0) {
      rotateShift = (sbyte)(highBitOrDigitsLeft & 0x1c);
      highBitOrDigitsLeft = (highBitOrDigitsLeft & 0x1c) >> 2;
      remainingValue = remainingValue >> rotateShift | remainingValue << 0x20 - rotateShift;
      do {
        while( true ) {
          digitCodeUnit = (remainingValue >> 0x1c) + 0x30;
          if (0x39 < digitCodeUnit) break;
          *outputCursor = (word)digitCodeUnit;
          highBitOrDigitsLeft = highBitOrDigitsLeft - 1;
          remainingValue = remainingValue << 4;
          outputCursor = outputCursor + 1;
          if (highBitOrDigitsLeft == 0)
          goto UiNumericTextControl_RebuildTextFromValue_UpdateRangeValidityAfterFormatting;
        }
        *outputCursor = (ushort)(remainingValue >> 0x1c) + 0x37;
        highBitOrDigitsLeft = highBitOrDigitsLeft - 1;
        remainingValue = remainingValue << 4;
        outputCursor = outputCursor + 1;
      } while (highBitOrDigitsLeft != 0);
    }
    else {
      *outputCursor = 0x30;
    }
  }
UiNumericTextControl_RebuildTextFromValue_UpdateRangeValidityAfterFormatting:
  UiNumericTextControl_UpdateRangeValidity(control);
  return;
}


/* Address: 0x004B65F0.
   Ownership: ui/controls/text.
   Purpose: Parses the UTF-16 edit buffer as signed decimal or hexadecimal, commits the value at +0x58, optionally
   queues the configured action, and refreshes validity. Invalid input clears state bit 1.
   Local calls: UiNumericTextControl_UpdateRangeValidity.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_preserve_eax_edx
UiNumericTextControl_ParseAndCommitValue(UiNumericTextControl *control)

{
  uint parsedValue;
  uint codeUnit;
  uint digitValue;
  int sign;
  word *textCursor;
  
  textCursor = control->textBuffer;
  sign = 1;
  if (*textCursor != 0) {
    if (*textCursor == 0x2d) {
      sign = -1;
      textCursor = control->textBuffer + 1;
    }
    if ((control->editStateFlags & UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT) == 0) {
      parsedValue = 0;
      for (; codeUnit = (uint)*textCursor, codeUnit != 0; textCursor = textCursor + 1) {
        if ((codeUnit < 0x30) || (9 < codeUnit - 0x30))
        goto UiNumericTextControl_ParseAndCommitValue_ClearValidityAndReturnAfterParseReject;
        parsedValue = parsedValue * 10 + (codeUnit - 0x30);
      }
    }
    else {
      parsedValue = 0;
      for (; codeUnit = (uint)*textCursor, codeUnit != 0; textCursor = textCursor + 1) {
        digitValue = codeUnit - 0x30;
        if ((codeUnit < 0x30) ||
           ((9 < digitValue &&
            ((digitValue = codeUnit - 0x37, digitValue < 10 ||
             ((0xf < digitValue && ((digitValue = codeUnit - 0x57, digitValue < 10 || (0xf < digitValue))))))))))
        goto UiNumericTextControl_ParseAndCommitValue_ClearValidityAndReturnAfterParseReject;
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
UiNumericTextControl_ParseAndCommitValue_ClearValidityAndReturnAfterParseReject:
  control->editStateFlags = control->editStateFlags & ~UI_NUMERIC_TEXT_VALUE_VALID;
  return;
}


/* Address: 0x004B0250.
   Ownership: ui/controls/text.
   Purpose: Reads the tooltip reference stored immediately before the serialized node. nodeFlags 0x200 selects a
   direct UTF-16 pointer; otherwise the value is resolved as a localized text resource ID. Measures the text and
   requests redraw.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], UiRootStack_InvalidateAll [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx UiTooltip_PrepareTargetText(UiNodeBase *node)

{
  word *commandStream;
  TextResourceResolveEaxCf5 resolvedText;
  
  if (node != (UiNodeBase *)0x0) {
    commandStream = (word *)node[-1].nodeFlags;
    if ((node->nodeFlags & UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)commandStream);
      commandStream = resolvedText.eax;
    }
    RichTextCommandStream_MeasureRegs(g_UiTooltipTextStyle,commandStream);
    (*g_GraphicsTextureSourceGetLogicalSize)(0xbc,g_UiWindowTextureSource);
    UiRootStack_InvalidateAll();
  }
  return;
}


/* Address: 0x004B66E0.
   Ownership: ui/controls/text.
   Purpose: Tests the value at +0x58 against the exact minimum and maximum fields at +0x8C and +0x90, using state
   flag 2 to select signed comparison, then updates validity bit 1.
*/
void __thandor_preserve_eax UiNumericTextControl_UpdateRangeValidity(UiNumericTextControl *control)

{
  uint currentNumericValue;
  
  currentNumericValue = control->currentValue;
  if ((control->editStateFlags & UI_NUMERIC_TEXT_SIGNED_VALUE) == 0) {
    if ((currentNumericValue < (uint)control->minimumValue) ||
       ((uint)control->maximumValue < currentNumericValue)) {
UiNumericTextControl_UpdateRangeValidity_ClearValidityForOutOfRangeValue:
      control->editStateFlags = control->editStateFlags & ~UI_NUMERIC_TEXT_VALUE_VALID;
      return;
    }
  }
  else if (((int)currentNumericValue < control->minimumValue) ||
          (control->maximumValue < (int)currentNumericValue))
  goto UiNumericTextControl_UpdateRangeValidity_ClearValidityForOutOfRangeValue;
  control->editStateFlags = control->editStateFlags | UI_NUMERIC_TEXT_VALUE_VALID;
  return;
}


/* Address: 0x004B6740.
   Ownership: ui/controls/text.
   Purpose: Measures up to prefixLength UTF-16 glyphs from control offset +0x6C using the active edit-text style
   and returns the accumulated width in EAX.
   Cross-module calls: FontGlyph_GetLogicalSizeForStyleRegs [assets/text/resources].
*/
UiPixelCoordinate __thandor_eax_preserve_ecx_edx
UiTextEditControl_MeasurePrefixWidth(UiTextCodeUnitCount prefixLength,UiTextEditControl *control)

{
  int accumulatedWidth;
  uint glyphIndex;
  FontGlyphSizeEaxEdxCf9 glyphSize;
  
  glyphIndex = 0;
  accumulatedWidth = 0;
  if (prefixLength != 0) {
    do {
      if (control->textPrefix6C[glyphIndex] == 0) {
        return accumulatedWidth;
      }
      glyphSize = FontGlyph_GetLogicalSizeForStyleRegs
                        (g_UiTextEditActiveTextStyle,(uint)control->textPrefix6C[glyphIndex]);
      glyphIndex = glyphIndex + 1;
      accumulatedWidth = accumulatedWidth + glyphSize.width;
    } while (glyphIndex < prefixLength);
  }
  return accumulatedWidth;
}


/* Address: 0x004B6790.
   Ownership: ui/controls/text.
   Purpose: Converts a pointer X coordinate to the nearest UTF-16 cursor index, accounting for control left,
   horizontal scroll, and the optional left decoration selected by state flag 8.
   Cross-module calls: FontGlyph_GetLogicalSizeForStyleRegs [assets/text/resources].
*/
UiTextCodeUnitCount __thandor_eax_preserve_ecx_edx
UiTextEditControl_FindCursorIndexAtX(UiPixelCoordinate pointerX,UiTextEditControl *control)

{
  int nextTextIndex;
  int targetOffsetX;
  int currentTextIndex;
  int measuredPrefixWidthPixels;
  GraphicsTextureSizeEaxEdxCf9 decorationSize;
  FontGlyphSizeEaxEdxCf9 glyphSize;
  
  targetOffsetX = (pointerX - (control->base).left) + control->horizontalScrollPixels;
  if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_FRAMED_CHROME) != 0) {
    decorationSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x6a,g_UiWindowTextureSource);
    targetOffsetX = targetOffsetX - decorationSize.logicalWidthPixels;
  }
  measuredPrefixWidthPixels = 0;
  nextTextIndex = 0;
  do {
    currentTextIndex = nextTextIndex;
    if (control->textPrefix6C[currentTextIndex] == 0) {
      return currentTextIndex;
    }
    glyphSize = FontGlyph_GetLogicalSizeForStyleRegs
                      (g_UiTextEditActiveTextStyle,(uint)control->textPrefix6C[currentTextIndex]);
    measuredPrefixWidthPixels = measuredPrefixWidthPixels + glyphSize.width;
    nextTextIndex = currentTextIndex + 1;
  } while (measuredPrefixWidthPixels < targetOffsetX);
  return currentTextIndex;
}


/* Address: 0x004B7010.
   Ownership: ui/controls/text.
   Purpose: Calls the installed DOS 8.3 path validator with flags derived from control state bits 1 and 2, then
   updates validity bit 1 from the validator's CF result.
*/
void __thandor_void_preserve_eax_ecx_edx
UiPathTextControl_UpdateDos83Validity(UiPathTextEditControl *control)

{
  bool pathRejected;
  bool validatorRejected;
  
  validatorRejected = (bool)(*g_FileSystemValidateDos83Path)
                          (control->editStateFlags >> 1 & 3,(byte *)control->pathBuffer);
  if (validatorRejected) {
    control->editStateFlags = control->editStateFlags & ~UI_TEXT_EDIT_VALUE_VALID;
  }
  else {
    control->editStateFlags = control->editStateFlags | UI_TEXT_EDIT_VALUE_VALID;
  }
  return;
}


/* Address: 0x004B78F0.
   Ownership: ui/controls/text.
   Purpose: Sets validity bit 1 exactly when the first UTF-16 code unit at control offset +0x6C is nonzero.
*/
void __thandor_void_preserve_eax_ecx_edx
UiTextControl_UpdateNonEmptyValidity(UiTextEditControl *control)

{
  if (control->textPrefix6C[0] == 0) {
    control->editStateFlags = control->editStateFlags & ~UI_TEXT_EDIT_VALUE_VALID;
  }
  else {
    control->editStateFlags = control->editStateFlags | UI_TEXT_EDIT_VALUE_VALID;
  }
  return;
}


/* Address: 0x004BB570.
   Ownership: ui/controls/text.
   Purpose: Expands both rich-text streams into fixed buffers, then compares them with the ASCII-case-insensitive
   UTF-16 comparator. EAX and EDX remain preserved.
   Cross-module calls: RichTextCommandStream_CopyExpandedCf [assets/text/richtext].
*/
CompareFlagsCfZf2 __thandor_void_preserve_eax_ecx_edx
UiPointerList_CompareExpandedTextFlags(word *rightText,word *leftText)

{
  bool copyCarry;
  undefined1 in_ZF;
  RichTextCopyExpandedEaxCf5 copyResult;
  CompareFlagsCfZf2 compareFlags;
  
  RichTextCommandStream_CopyExpandedCf(0x400,(word *)&g_UiPointerListExpandedLeftTextUtf16,leftText)
  ;
  copyResult = RichTextCommandStream_CopyExpandedCf
                    (0x400,(word *)&g_UiPointerListExpandedRightTextUtf16,rightText);
  copyCarry = copyResult.carry;
  (*(code *)g_Utf16StringCompareAsciiCaseInsensitiveFlagsCf)
            (&g_UiPointerListExpandedRightTextUtf16,&g_UiPointerListExpandedLeftTextUtf16);
  compareFlags.carry = copyCarry;
  compareFlags.zero = (bool)in_ZF;
  return compareFlags;
}


/* Address: 0x004B5D00.
   Ownership: ui/controls/text.
   Purpose: Recomputes the edit control content rectangle, measures the cursor and active prefix, accounts for
   window decorations, and clamps the horizontal scroll offset so the active glyph remains visible.
   Local calls: UiTextEditControl_MeasurePrefixWidth.
   Cross-module calls: FontGlyph_GetLogicalSizeActiveRegs [assets/text/resources].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTextEditControl_RecomputeLayoutAndClampScroll(UiTextEditControl *control)

{
  UiTextCodeUnitCount prefixLength;
  UiPixelOffset cursorWidthOrMinScroll;
  UiPixelCoordinate fullTextWidth;
  int overflowOrContentWidth;
  UiPixelOffset maxScrollOffset;
  FontGlyphSizeEaxEdxCf9 glyphSize;
  GraphicsTextureSizeEaxEdxCf9 decorationSize;
  
  (control->base).layoutWidth = (control->base).right - (control->base).left;
  (control->base).layoutHeight = (control->base).bottom - (control->base).top;
  prefixLength = control->cursorIndex;
  cursorWidthOrMinScroll = UiTextEditControl_MeasurePrefixWidth(prefixLength,control);
  maxScrollOffset = cursorWidthOrMinScroll;
  if (prefixLength != 0) {
    glyphSize = FontGlyph_GetLogicalSizeActiveRegs((uint)control->textPrefix6C[prefixLength - 1]);
    maxScrollOffset = cursorWidthOrMinScroll - glyphSize.width;
  }
  overflowOrContentWidth = cursorWidthOrMinScroll - (control->base).layoutWidth;
  if (control->textPrefix6C[prefixLength] != 0) {
    glyphSize = FontGlyph_GetLogicalSizeActiveRegs((uint)control->textPrefix6C[prefixLength]);
    overflowOrContentWidth = overflowOrContentWidth + glyphSize.width;
  }
  fullTextWidth = UiTextEditControl_MeasurePrefixWidth(0x10,control);
  decorationSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x8a,g_UiWindowTextureSource);
  cursorWidthOrMinScroll = overflowOrContentWidth + decorationSize.logicalWidthPixels;
  overflowOrContentWidth = fullTextWidth + decorationSize.logicalWidthPixels;
  if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_FRAMED_CHROME) != 0) {
    decorationSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x6a,g_UiWindowTextureSource);
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
   Ownership: ui/controls/text.
   Purpose: Draws the button skin for current selectable state, resolves textResourceId, measures and draws the
   UTF-16 text using packedTextStyle, and honors clipping and suppression flags.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTextButtonControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control)

{
  word *commandStream;
  dword skinFrame;
  int textXOrFocusEnd;
  int focusTileXOrTop;
  int textYOrTileX;
  uint textStyle;
  bool framebufferUnavailable;
  RichTextExtentRegs textExtent;
  TextResourceResolveEaxCf5 resolvedText;
  GraphicsTextureSizeEaxEdxCf9 skinSizeOrEndCapSize;
  GraphicsTextureSizeEaxEdxCf9 focusTileSize;
  int baselineY;
  int drawX;
  
  framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
  if (framebufferUnavailable) {
    return;
  }
  skinFrame = 0x46;
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    skinFrame = 0x48;
  }
  if ((((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) &&
     (skinFrame = skinFrame - 6, ((control->selectable).stateFlags & 0x40) != 0)) {
    skinFrame = 0x44;
  }
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    if (((control->selectable).stateFlags & 0x400) != 0)
    goto UiTextButtonControl_DrawClipped_EndFramebufferAccessAndReturn;
    skinFrame = skinFrame + 1;
  }
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
             (control->selectable).base.left,skinFrame,g_UiWindowTextureSource,g_FramebufferAccess);
  skinSizeOrEndCapSize = (*g_GraphicsTextureSourceGetLogicalSize)(skinFrame,g_UiWindowTextureSource);
  textXOrFocusEnd = skinSizeOrEndCapSize.logicalWidthPixels + 6;
  resolvedText = TextResource_Resolve(control->textResourceId);
  commandStream = resolvedText.eax;
  textExtent = RichTextCommandStream_MeasureRegs(control->packedTextStyle,commandStream);
  textYOrTileX = (int)(skinSizeOrEndCapSize.logicalHeightPixels - textExtent.heightPixels) >> 1;
  textStyle = g_UiTextStyleDisabled;
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (textStyle = g_UiTextStyleNormal, ((control->selectable).stateFlags & 0x40) != 0)) {
    textStyle = g_UiTextStyleAlternate;
  }
  if (((control->selectable).stateFlags & 0x100) == 0) {
    control->packedTextStyle = control->packedTextStyle & 0xff0000;
  }
  else {
    textStyle = textStyle & 0xffffff;
  }
  if (((control->selectable).stateFlags & 0x200) == 0) {
    control->packedTextStyle = control->packedTextStyle & 0xff000000;
  }
  else {
    textStyle = textStyle & 0xff00ffff;
  }
  textStyle = textStyle | control->packedTextStyle;
  if ((((control->selectable).base.nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) ||
     (((control->selectable).stateFlags & 0x1000) != 0)) {
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,
               textYOrTileX + (control->selectable).base.top,textXOrFocusEnd + (control->selectable).base.left);
  }
  else {
    textXOrFocusEnd = textXOrFocusEnd + (control->selectable).base.left;
    textYOrTileX = textYOrTileX + (control->selectable).base.top;
    baselineY = textXOrFocusEnd;
    drawX = textYOrTileX;
    (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,textYOrTileX,textXOrFocusEnd + -2,0x7f000000,0x86,
               g_UiWindowTextureSource,g_FramebufferAccess);
    skinSizeOrEndCapSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
    focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
    focusTileXOrTop = textXOrFocusEnd + -2 + focusTileSize.logicalWidthPixels;
    focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
    textXOrFocusEnd = ((textXOrFocusEnd + 4) - skinSizeOrEndCapSize.logicalWidthPixels) + textExtent.widthPixels;
    (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,textYOrTileX,textXOrFocusEnd,0x7f000000,0x88,
               g_UiWindowTextureSource,g_FramebufferAccess);
    if (clipLeft < textXOrFocusEnd) {
      textXOrFocusEnd = clipLeft;
    }
    do {
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,textXOrFocusEnd,clipBottom,clipRight,textYOrTileX,focusTileXOrTop,0x7f000000,0x87,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      focusTileXOrTop = focusTileXOrTop + focusTileSize.logicalWidthPixels;
    } while (focusTileXOrTop < textXOrFocusEnd);
    focusTileXOrTop = drawX + -1;
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,textStyle,commandStream,drawX,baselineY);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,focusTileXOrTop,baselineY + -3,0x86,
               g_UiWindowTextureSource,g_FramebufferAccess);
    skinSizeOrEndCapSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
    focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
    textYOrTileX = baselineY + -3 + focusTileSize.logicalWidthPixels;
    focusTileSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
    textXOrFocusEnd = ((baselineY + 3) - skinSizeOrEndCapSize.logicalWidthPixels) + textExtent.widthPixels;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,focusTileXOrTop,textXOrFocusEnd,0x88,g_UiWindowTextureSource,
               g_FramebufferAccess);
    if (clipLeft < textXOrFocusEnd) {
      textXOrFocusEnd = clipLeft;
    }
    do {
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,textXOrFocusEnd,clipBottom,clipRight,focusTileXOrTop,textYOrTileX,0x87,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      textYOrTileX = textYOrTileX + focusTileSize.logicalWidthPixels;
    } while (textYOrTileX < textXOrFocusEnd);
  }
UiTextButtonControl_DrawClipped_EndFramebufferAccessAndReturn:
  (*g_GraphicsFramebufferEndAccess)();
  return;
}

